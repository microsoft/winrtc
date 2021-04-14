using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsWinRTCApp.Model
{
    public class IceCandidate
    {
        [JsonProperty("candidate")]
        public string Candidate
        {
            get; set;
        }

        [JsonProperty("sdpMid")]
        public string SdpMid
        {
            get; set;
        }

        [JsonProperty("sdpMLineIndex")]
        public int SdpMLineIndex
        {
            get; set;
        }
    }
}