using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
//using Microsoft.Azure.SignalR;
using Microsoft.AspNetCore.SignalR;
//using Windows.UI.Xaml.Controls;

namespace WinRTC_SignalR.SignalRHub
{
    class AzureHub : Hub
    {
        //private static Dictionary<string, string> userNames = new Dictionary<string, string>();

        public Task Send(string name, string message, string room)
        {
            IClientProxy proxy;
            if (!string.IsNullOrWhiteSpace(room))
            {
                proxy = Clients.GroupExcept(room, Context.ConnectionId);
            }
            else
            {
                proxy = Clients.All;
            }

           Console.WriteLine($"{proxy}, {name}, {message}");

           return  proxy.SendAsync("sendMessage", name, message);

        } 
        
        public void SendClient(string name, string message)
        {
            Clients.Client(Context.ConnectionId).SendAsync("sendclient", name, message);

        }
        
        public async Task RoomUserRegister(string room)
        {
            await Groups.AddToGroupAsync(Context.ConnectionId, room);
        }
       
        public async Task RoomUserRemove(string room)
        {
            await Groups.RemoveFromGroupAsync(Context.ConnectionId, room);
        }

        public async Task OnConnect()
        {
            await base.OnConnectedAsync();
        }

        public async Task Disconnect(Exception exception)
        {
            //await Clients.All.SendAsync("usersLoggedOut", userNames);

            await base.OnDisconnectedAsync(exception);

            await Clients.AllExcept(Context.ConnectionId).SendAsync("ClientDisconnected", Context.ConnectionId);
        }
    }
}
