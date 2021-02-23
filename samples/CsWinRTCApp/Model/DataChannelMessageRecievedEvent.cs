using Microsoft.WinRTC.WebRtcWrapper.webrtc;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsWinRTCApp.Model
{
    public class DataChannelMessageRecievedEvent
    {
        public DataChannel DataChannel
        {
            get; private set;
        }

        public byte[] Buffer
        {
            get; private set;
        }

        public string Text
        {
            get; private set;
        }

        public DataChannelMessageRecievedEvent(DataChannel channel, byte[] buffer, string text)
            : base()
        {
            DataChannel = channel;
            Buffer = buffer;
            Text = text;
        }
    }
}
