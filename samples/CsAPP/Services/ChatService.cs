using Microsoft.AspNetCore.SignalR.Client;
//using Microsoft.WinRTC.WebRtcWrapper.webrtc.PeerConnection;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
//using System.Windows.Media;
using CsAPP.Model;


namespace CsAPP.Services
{
    public class ChatService
    {
        private readonly HubConnection hubConnection;

        public event Action<Chat> MessageRecieved;
        public ChatService(HubConnection connection)
        {
            hubConnection = connection;

            hubConnection.On<Chat>("sendMessage", (color) => MessageRecieved?.Invoke(color) );
        }

        public async Task Connect()
        {
            await hubConnection.StartAsync();
        }

        public async Task SendMessage(Chat chat)
        {
            await hubConnection.SendAsync("SendMessage", chat);
        }

        //PeerConnection peerConnection;
    }
}
