using CsAPP.Model;
using System;
using System.Collections.Generic;
using System.Text;
//using System.Windows.Media;
using Windows.UI;
using Windows.UI.Xaml.Media;
//using Common.Model;


namespace CsAPP.ViewModels
{
    public class TextChatViewModels : ViewModelBase
    {
        public Chat Chat { get; set; }

        public Brush ColorBrush
        {
            get
            {
                try
                {
                    return new SolidColorBrush(Color.FromArgb(
                        Chat.Blue,
                        Chat.Red,
                        Chat.Green,
                        Chat.Blue));
                }
                catch (FormatException)
                {
                    return new SolidColorBrush(Colors.Black);
                }
            }
        }

        public TextChatViewModels(Chat chat)
        {
            Chat = chat;
        }
    }
}