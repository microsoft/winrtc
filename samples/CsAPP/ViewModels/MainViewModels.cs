using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsAPP.ViewModels
{
    class MainViewModels
    {
        public ChatViewModel ChatViewModel { get; }

        public MainViewModels(ChatViewModel chatViewModel)
        {
            ChatViewModel = chatViewModel;
        }
    }
}
