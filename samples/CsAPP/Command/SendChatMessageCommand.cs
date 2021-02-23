using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
//using ConsoleApp2.Model;
using CsAPP.Model;
using CsAPP.Services;
using CsAPP.ViewModels;


namespace CsAPP.Command
{
    class SendChatMessageCommand : ICommand
    {
        private readonly ChatViewModel viewModel;

        private readonly ChatService chatService;

        public SendChatMessageCommand(ChatViewModel viewModel, ChatService chatService)
        {
            this.viewModel = viewModel;
            this.chatService = chatService;
        }

        public event EventHandler CanExecuteChanged;

        public bool CanExecute(object parameter)
        {
            return true;

        }

        public async void Execute(object parameter)
        {
            try
            {
                await chatService.SendMessage(new Chat()
                {
                    Red = viewModel.Red,
                    Green = viewModel.Green,
                    Blue = viewModel.Blue,
                });

                viewModel.ErrorMessage = string.Empty;

            }
            catch (Exception)
            {
                viewModel.ErrorMessage = "Can not Connect";
            }
        }
    }
}
