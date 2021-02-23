using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace CsWinRTC
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Video_Call : ContentPage
    {
        public Video_Call(string groupname, string name)
        {
            this.InitializeComponent();
            Control.Room(groupname);
        }
    }
}