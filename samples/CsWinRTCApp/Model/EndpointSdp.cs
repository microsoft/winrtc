using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsWinRTCApp.Model
{
    public class EndpointSdp
    {
        [JsonProperty("from")]
        public string From;

        [JsonProperty("sdp")]
        public string Sdp;

        [JsonProperty("video")]
        public bool IsVideo;

        [JsonProperty("audio")]
        public bool IsAudio;
    }
}
