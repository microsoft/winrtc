using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.Media.Core;
using Microsoft.WinRTC.WebRtcWrapper.webrtc;
using Microsoft.WinRTC.WebRtcWrapper.webrtc.PeerConnection;
using CsWinRTCApp.Signalling;
using CsWinRTCApp.Model;
using Windows.UI.Core;
using System.Diagnostics;
using System.Reactive.Linq;
using System.Reactive;

namespace CsWinRTCApp.ViewModels
{
    public class WinrtcClient : IDisposable
    {
        public const string DEFAULT_CHANNRL_LABEL = "default";

        private static Mutex mut = new Mutex();

        public bool _wait;

        private string _answerSdp;
        private bool _disposed;

        private MediaStreamTrack _selfVideoTrack;
        private MediaStreamTrack _selfAudioTrack;

        private MediaStreamTrack _remoteVideoTrack;
        private MediaStreamTrack _remoteAudioTrack;

        private TaskCompletionSource<bool> _AnswerHanler;
        private TaskCompletionSource<bool> _handShakeHanler;

        private readonly RTCConfiguration _config;
        private Dictionary<string, DataChannel> _dataChannels;

        private IDisposable _remoteRenegotiationNeeded;

        private Task _reNegotiationTask;

        public interface DataChannelEvent
        {
            DataChannel Channel { get; }
        }

        public PeerConnection PeerConnection
        {
            get; private set;
        }

        public PeerConnectionDependencies PeerConnectionDependencies
        {
            get; private set;
        }

        public PeerConnectionObserver PeerConnectionObserver
        {
            get; private set;
        }

        public PeerConnectionFactory PeerConnectionFactory;

        public List<IceServer> IceServers
        {
            get; private set;
        }

        public string RemoteEndpoint
        {
            get; private set;
        }

        public WinrtcSignaling Signaling
        {
            get; private set;
        }

        public MediaDeviceProvider MediaProvider
        {
            get; private set;
        }

        public event EventHandler<DataChannelMessageRecievedEvent> DataReceived;

        protected WinrtcClient(string remoteEndPoint, WinrtcSignaling winrtcSignaling, MediaDeviceProvider mediaProvider, IEnumerable<IceServer> iceServers) : this()
        {
            if (!iceServers.Any())
            {
                throw new ArgumentException(nameof(iceServers));
            }

            Signaling = winrtcSignaling;
            MediaProvider = mediaProvider;
            RemoteEndpoint = remoteEndPoint;
            IceServers = iceServers is List<IceServer> server ? server : iceServers.ToList();
            _config = GetConfiguration();

            //PeerConnectionFactorys = new PeerConnectionFactory(_config);
            //PeerConnection = new PeerConnection(_config);

            // Event Subs
            PeerConnectionObserver.OnIceCandidate += OnIceCandidateAddedAsync;
            PeerConnectionObserver.OnTrack += OnPeerConnectionTrackReceived;
            PeerConnectionObserver.OnTrack += OnPeerConnectionTrackRemove;

            var _peerConnectionObserver = PeerConnectionObserver;
            var _prConnectionDependencies = new PeerConnectionDependencies(_peerConnectionObserver);
            //PeerConnectionDependencies(_peerConnectionObserver);

            PeerConnectionFactory.CreatePeerConnection(_config, _prConnectionDependencies);

            // Subcribe to spool signaling
            Signaling.On(MessageType.Answer, OnAnswerReceivedAsync);
            Signaling.On(MessageType.Candidate, OnICECandidateReceivedAsync);

        }

        private WinrtcClient()
        {
            _dataChannels = new Dictionary<string, DataChannel>();
        }

        ~WinrtcClient()
        {
            Dispose(false);
        }

        //public static void InitLibrary(CoreDispatcher dispatcher)
        //{
        //    IEventQueue queue = EventQueueMaker.Bind(dispatcher);
        //    var configuration = new WebRtcLibConfiguration
        //    {
        //        Queue = queue,
        //        AudioCaptureFrameProcessingQueue = EventQueue.GetOrCreateThreadQueueByName("AudioCaptureProcessingQueue"),
        //        AudioRenderFrameProcessingQueue = EventQueue.GetOrCreateThreadQueueByName("AudioRenderProcessingQueue"),
        //        VideoFrameProcessingQueue = EventQueue.GetOrCreateThreadQueueByName("VideoFrameProcessingQueue"),
        //    };

        //    WebRtcLib.Setup(configuration);
        //}

        public async static Task<WinrtcClient> CreateAndSendOfferAsync(string remoteEndPoint, WinrtcSignaling signaling, MediaDeviceProvider mediaProvider, IEnumerable<IceServer> iceServers, params string[] dataChannels)
        {
            var client = await Task.Run(async () =>
            {
                var instance = new WinrtcClient(remoteEndPoint, signaling, mediaProvider, iceServers);

                instance._handShakeHanler = new TaskCompletionSource<bool>();

                if (!dataChannels.Any())
                {
                    instance.CreateDataChannel(DEFAULT_CHANNRL_LABEL);
                }
                else
                {
                    foreach (var channelLabel in dataChannels)
                    {
                        instance.CreateDataChannel(channelLabel);
                    }
                }

                await instance.SendOfferAsync();
                instance._handShakeHanler.SetResult(true);

                // Only subscribe to this after initial negotiation has been made.
                instance.SubscribeToNegotiationNeeded();
                return instance;
            });

            return client;
        }

        public async static Task<WinrtcClient> CreateFromOfferAndSendAnswerAsync(EndpointSdp offer, WinrtcSignaling signaling, MediaDeviceProvider mediaDevice, IEnumerable<IceServer> servers)
        {
            var client = await Task.Run(async () =>
            {
                var instance = new WinrtcClient(offer.From, signaling, mediaDevice, servers);
                instance._handShakeHanler = new TaskCompletionSource<bool>();
                await instance.OnOfferReceivedAsync(offer);
                instance._handShakeHanler.SetResult(true);
                instance.SubscribeToNegotiationNeeded();
                return instance;
            });
            return client;
        }

        public async Task OnOfferReceivedAsync(EndpointSdp offer)
        {
            _remoteRenegotiationNeeded?.Dispose();

            SessionDescription sessionDescription
            (
                Offer = offer.Sdp,
                Type = SdpType.Offer
            )

            //SetSessionDescriptionObserver setSessionDescriptionObserver;
            Debug.WriteLine("Settings remote description");
            await PeerConnection.SetRemoteDescription();
            
        }

        private void SubscribeToNegotiationNeeded(bool skipFirst = false)
        {
            Debug.WriteLine($"Re_subscribing");
            var obv = Observable.FromEvent<PeerConnectionObserverOnRenegotiationNeededDelegate, Unit>(h => () => h(Unit.Default), h => PeerConnectionObserver.OnRenegotiationNeeded += h, h => PeerConnectionObserver.OnRenegotiationNeeded -= h)
                .Throttle(TimeSpan.FromSeconds(2));

            if (skipFirst)
            {
                obv = obv.Skip(1);
            }

            _remoteRenegotiationNeeded = obv.Subscribe(OnRenegotiationNeeded);
        }

        private async void OnRenegotiationNeeded(Unit obj)
        {
            if (_reNegotiationTask != null)
            {
                await _reNegotiationTask;
            }

            Debug.WriteLine("OnNegotiationNeeded!");
            _reNegotiationTask = Task.Run(SendOfferAsync);
            await _reNegotiationTask;

            _reNegotiationTask = null;
        }

        private Task SendOfferAsync()
        {
            throw new NotImplementedException();
        }

        private RTCConfiguration GetConfiguration()
        {
            throw new NotImplementedException();
        }

        private void OnPeerConnectionTrackRemove(RtpTransceiver value)
        {
            throw new NotImplementedException();
        }

        private void OnPeerConnectionTrackReceived(RtpTransceiver value)
        {
            throw new NotImplementedException();
        }

        private void OnIceCandidateAddedAsync(IceCandidate value)
        {
            throw new NotImplementedException();
        }

        private void OnAnswerReceivedAsync(string obj)
        {
            throw new NotImplementedException();
        }
        
        public void CreateDataChannel(string channelName)
        {
            DataChannel channel = PeerConnection.CreateDataChannel(channelName, new DataChannelInit { Id = (ushort)_dataChannels.Count }) as DataChannel;
            InternalAddDataChannel(channel);
        }

        private void InternalAddDataChannel(DataChannel channel)
        {
            throw new NotImplementedException();
        }

        private void OnICECandidateReceivedAsync(string obj)
        {
            throw new NotImplementedException();
        }

        public void Dispose(bool disposing)
        {
            throw new NotImplementedException();
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
    }
}
