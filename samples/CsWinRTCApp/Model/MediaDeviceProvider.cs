using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Controls;

namespace CsWinRTCApp.Model
{
    public interface MediaDeviceProvider
    {
        MediaElement PeerVideo { get; set; }
        MediaElement SelfVideo { get; set; }

        Task<bool> RequestAccessForMediaCaptureAsync();
    }
}
