using System.Collections.ObjectModel;
using System.Drawing;
using System.Windows.Input;
using CsAPP.Command;
using CsAPP.Model;
using CsAPP.Services;

namespace CsAPP.ViewModels
{
    public class ChatViewModel : ViewModelBase
    {
        private byte _red;
        public byte Red
        {
            get
            {
                return _red;
            }
            set
            {
                _red = value;
                OnPropertyChanged(nameof(Red));
            }
        }

        private byte _green;
        public byte Green
        {
            get
            {
                return _green;
            }
            set
            {
                _green = value;
                OnPropertyChanged(nameof(Green));
            }
        }

        private byte _blue;
        public byte Blue
        {
            get
            {
                return _blue;
            }
            set
            {
                _blue = value;
                OnPropertyChanged(nameof(Blue));
            }
        }

        private string _errorMessage = string.Empty;
        public string ErrorMessage
        {
            get
            {
                return _errorMessage;
            }
            set
            {
                _errorMessage = value;
                OnPropertyChanged(nameof(ErrorMessage));
                OnPropertyChanged(nameof(HasErrorMessage));
            }
        }

        public bool HasErrorMessage => !string.IsNullOrEmpty(ErrorMessage);

        private bool _isConnected;
        public bool IsConnected
        {
            get
            {
                return _isConnected;
            }
            set
            {
                _isConnected = value;
                OnPropertyChanged(nameof(IsConnected));
            }
        }

        public ObservableCollection<TextChatViewModels> Messages { get; }

        public ICommand SendMessageCommand { get; }

        public ChatViewModel(ChatService chatService)
        {
            SendMessageCommand = new SendChatMessageCommand(this, chatService);

            Messages = new ObservableCollection<TextChatViewModels>();

            chatService.MessageRecieved += ChatService_ColorMessageReceived;

        }

        public static ChatViewModel CreatedConnectedViewModel(ChatService chatService)
        {
            ChatViewModel viewModel = new ChatViewModel(chatService);

            chatService.Connect().ContinueWith(task =>
            {
                if (task.Exception != null)
                {
                    viewModel.ErrorMessage = "Unable to connect to  chat hub";
                }
            });

            return viewModel;
        }

        private void ChatService_ColorMessageReceived(Chat color)
        {
            Messages.Add(new TextChatViewModels(color));
        }
    }
}