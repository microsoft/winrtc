using Microsoft.AspNet.SignalR.Client;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.WinRTC.WebRtcWrapper.webrtc;
using Microsoft.WinRTC.WebRtcWrapper.webrtc.PeerConnection;

namespace CsWinRTCApp.ViewModels
{
    class webrtcModel
    {
        IceGatheringState _gather;
        IceTransportType _iceTransport;
        DataChannel _dataChannel;
        MediaStreamTrack _videotrack;
        DataChannelInit dataChannelInit;
        AudioTrack




        public ObservableCollection<Peer> Peers = new ObservableCollection<Peer>();

        public event PropertyChangedEventHandler PropertyChanged;

        private Peer _remotePeer;

        public Peer RemotePeer
        {
            get
            {
                if (_remotePeer == null)
                    _remotePeer = SelectedPeer;
                return _remotePeer;
            }
            set
            {
                if (_remotePeer == value)
                    return;
                _remotePeer = value;
            }
        }

        private Peer _selectedPeer;
        private bool _isSendEnabled = false;

        public Peer SelectedPeer
        {
            get { return _selectedPeer; }
            set
            {
                if (_selectedPeer == value)
                    return;

                var oldValue = _selectedPeer;
                _selectedPeer = value;
                OnPropertyChanged(nameof(SelectedPeer));

                SelectedPeerChanged(oldValue, value);
            }
        }

        private string _conversation = string.Empty;
        public string Conversation 
        {
            get { return _conversation; }
            set
            {
                if (_conversation == value)
                    return;

                _conversation = value;
                OnPropertyChanged(nameof(Conversation));
            }
        }

        private string _message = string.Empty;
        public string Message 
        {
            get { return _message; }
            set
            {
                if (_message == value)
                    return;

                _message = value;
                OnPropertyChanged(nameof(Message));
            }
        }
        public bool IsSendEnabled 
        {
            get 
            {
                return _isSendEnabled;
            }
            set
            {
                if (_isSendEnabled == value)
                    return;

                _isSendEnabled = value;
                OnPropertyChanged(nameof(IsSendEnabled));
            } 
        }

        private void SelectedPeerChanged(Peer oldValue, Peer value)
        {
            if (_dataChannel != null)
            {
                _dataChannel.Close();
                //_sctp.Stop();
                //_dtls.Stop();
                //_ice.Stop();
            }


            Conversation = string.Empty;
            Message = string.Empty;
            IsSendEnabled = false;

            InitializeORTC();
        }

        private void InitializeORTC()
        {
            var iceserver = new List<IceServer>
            {
                new IceServer { Urls = { "stun:stun.l.google.com:19302" } }
            };
        }

        protected void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
