using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsWinRTCApp.Signalling
{
    public interface WinrtcSignaling
    {
        string SelfConnectionId
        {
            get;
        }

        void On(string messageType, Action<string> handler);

        Task SendAsync(string newMessage, string type, string connectionId);
    }
}
