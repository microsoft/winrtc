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
    public partial class Page1 : ContentPage
    {
        public Page1()
        {
            InitializeComponent();
        }

         private async void Button_Clicked(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(groupNameInput.Text))
            {
                await DisplayAlert("Hey", "Fill group name", "Close");
            }
            else
            {
                await Navigation.PushAsync(new Video_Call(groupNameInput.Text, nameInput.Text));
            }
        }
    }
}