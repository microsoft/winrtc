using Microsoft.WinRTC.WebRtcWrapper.webrtc.PeerConnection;
using Microsoft.WinRTC.WebRtcWrapper.webrtc;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Media.Capture;
using Windows.Media.Core;
using Windows.Media.MediaProperties;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Microsoft.WinRTC.WebRtcWrapper.webrtc.VideoFrame;
using Windows.Media.Playback;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace CsWinRTCApp
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class VideoCallPage : Page
    {
        //private NodeDssSignaler _signaler;
        private PeerConnectionFactory ConnectionFactory;
        private PeerConnection _peerConnection;
        Microsoft.WinRTC.WebRtcWrapper.webrtc.AudioTrack _microphoneSource;
        Microsoft.WinRTC.WebRtcWrapper.webrtc.VideoTrack _webcamSource;
        AudioSource _localAudioTrack;
        VideoTrackSource _localVideoTrack;
        RtpTransceiver _audioTransceiver;
        RtpTransceiver _videoTransceiver;
        private MediaStreamSource _localVideoSource;
        private VideoBridge _localVideoBridge = new VideoBridge(3);
        private bool _localVideoPlaying = false;
        private object _localVideoLock = new object();
        private object _remoteVideoLock = new object();
        private bool _remoteVideoPlaying = false;
        private MediaStreamSource _remoteVideoSource;
        private VideoBridge _remoteVideoBridge = new VideoBridge(5);
        private VideoTrackSource _remoteVideoTrack;
        private DataChannel dataChannel;
        public VideoCallPage()

        {
            this.InitializeComponent();
            this.Loaded += OnLoaded;
            Application.Current.Suspending += App_Suspending;
        }

        private void App_Suspending(object sender, SuspendingEventArgs e)
        {
            if (_peerConnection != null)
            {
                //_peerConnection.Close();
                //_peerConnection.Dispose();
                _peerConnection = null;
            }
            localVideoPlayerElement.SetMediaPlayer(null);

            remoteVideoPlayerElement.SetMediaPlayer(null);
        }


        PeerConnectionObserver peerObserver = new PeerConnectionObserver();
        private async void OnLoaded(object sender, RoutedEventArgs e)
        {
            _peerConnection = new PeerConnection();

            var config = new RTCConfiguration()
            {
                IceServers = new List<IceServer>
                {
                    new IceServer{Urls = {"stun:stun.l.google.com:19302"}}
                }
            };

            peerObserver.OnIceCandidate += OnIceCadidate;
            //peerObserver.ToString += SendMessage;

            //var dependencies = PeerConnectionDependencies;

            //ConnectionFactory.CreatePeerConnection(config, dependencies);
            //_peerConnection.
            ///////////////////////////////////////////////////////////////
            var settings = new MediaCaptureInitializationSettings();
            settings.StreamingCaptureMode = StreamingCaptureMode.AudioAndVideo;
            var capture = new MediaCapture();
            await capture.InitializeAsync(settings);

            List<DeviceIdentification> devicelist = await VideoCaptureModule_DeviceInfo.GetDeviceName();

            foreach (var device in deviceList)
            {
                Debugger.Log(0, "", $"Webcam {device.name} (id: {device.DeviceUniqueId}) {device.ProductUniqueId}\n");
            }


            Debugger.Log(0, "", "Peer connection initialized successfully.\n");

            _webcamSource = await VideoTrack.CreateAsync();
            _webcamSource.I420AVideoFrameReady += LocalI420AFrameReady;
           
             
        }

        private void RunOnMainThread(Windows.UI.Core.DispatchedHandler handler)
        {
            if (Dispatcher.HasThreadAccess)
            {
                handler.Invoke();
            }
            else
            {
                // Note: use a discard "_" to silence CS4014 warning
                _ = Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, handler);
            }
        }

        private MediaStreamSource CreateI420VideoStreamSource(uint width, uint height, int framerate)
        {
            if (width == 0)
            {
                throw new ArgumentException("Invalid zero width for video.", "width");
            }
            if (height == 0)
            {
                throw new ArgumentException("Invalid zero height for video.", "height");
            }

            var videoProperties = VideoEncodingProperties.CreateUncompressed(MediaEncodingSubtypes.Iyuv, width, height);
            var videoStreamDesc = new VideoStreamDescriptor(videoProperties);
            videoStreamDesc.EncodingProperties.FrameRate.Numerator = (uint)framerate;
            videoStreamDesc.EncodingProperties.FrameRate.Denominator = 1;
            // Bitrate in bits per second : framerate * frame pixel size * I420=12bpp
            videoStreamDesc.EncodingProperties.Bitrate = ((uint)framerate * width * height * 12);
            var videoStreamSource = new MediaStreamSource(videoStreamDesc);
            videoStreamSource.BufferTime = TimeSpan.Zero;
            videoStreamSource.SampleRequested += OnMediaRequested;
            videoStreamSource.IsLive = true; // Enables optimizations for live sources
            videoStreamSource.CanSeek = false; // Cannot seek live WebRTC video stream
            return videoStreamSource;
        }

        IceCandidate _iceCandiadate;
        private void OnIceCadidate(IceCandidate iceCandidate)
        {
            var msg = _iceCandiadate.SdpMid(iceCandidate);
            (Application.Current as App).conn.Send(msg);
        }

        //private void SendMessage(Mess)

        private void LocalI420AFrameReady(VideoFrame frame)
        {
            lock (_localVideoLock)
            {
                if (_localVideoLock)
                {
                    _localVideoPlaying = true;

                    uint with = frame.Width;
                    uint height = frame.Height;

                    RunOnMainThread(() =>
                    {
                        int framerate = 30;
                        _localVideoSource = CreateI420VideoStreamSource(
                            width, height, framerate);
                        var localVideoPlayer = new MediaPlayer();
                        localVideoPlayer.Source = MediaSource.CreateFromMediaStreamSource(_localVideoSource);
                        localVideoPlayerElement.SetMediaPlayer(localVideoPlayer);
                        localVideoPlayer.Play();
                    })
                }
            }
            _localVideoBridge.HandleIncomingVideoFrame(frame);
        }


        private void OnMediaRequested(MediaStreamSource sender, MediaStreamSourceSampleRequestedEventArgs args)
        {
            VideoBridge videoBridge;
            if (sender == _localVideoSource)
                videoBridge = _localVideoBridge;
            else if (sender == _remoteVideoSource)
                videoBridge = _remoteVideoBridge;
            else
                return;
            videoBridge.TryServeVideoFrame(args);
        }

        private void RemoteI420AFrameReady(I420AVideoFrame frame)
        {
            lock (_remoteVideoLock)
            {
                if (!_remoteVideoPlaying)
                {
                    _remoteVideoPlaying = true;
                    uint width = frame.width;
                    uint height = frame.height;
                    RunOnMainThread(() =>
                    {
                        // Bridge the remote video track with the remote media player UI
                        int framerate = 30; // assumed, for lack of an actual value
                        _remoteVideoSource = CreateI420VideoStreamSource(width, height,
                            framerate);
                        var remoteVideoPlayer = new MediaPlayer();
                        remoteVideoPlayer.Source = MediaSource.CreateFromMediaStreamSource(
                            _remoteVideoSource);
                        remoteVideoPlayerElement.SetMediaPlayer(remoteVideoPlayer);
                        remoteVideoPlayer.Play();
                    });
                }
            }
            _remoteVideoBridge.HandleIncomingVideoFrame(frame);
        }
    }
}
