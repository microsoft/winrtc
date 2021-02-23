//using Microsoft.AspNet.SignalR.Client;
using CsAPP.ViewModels;
using Microsoft.AspNetCore.SignalR.Client;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using CsAPP.Services;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace CsAPP
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        HubConnection connection = new HubConnectionBuilder().WithUrl("http://localhost:5000/azurehub")
                .Build();
        public MainPage()
        {
            this.InitializeComponent();
            this.Loaded += OnLoaded;
            Application.Current.Suspending += App_Suspending;
        }

        private void App_Suspending(object sender, SuspendingEventArgs e)
        {
            throw new NotImplementedException();
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            connection = new HubConnectionBuilder().Build("http://localhost:5000/azurehub");

            ChatViewModel chatViewModel = ChatViewModel.CreatedConnectedViewModel(new ChatService(connection));

            DataContext = new MainViewModels(chatViewModel);
        }
    }
}
