using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsWinRTCApp.ViewModels
{
    class Peer
    {
        public int Id { get; private set; }
        public string Name { get; private set; }
        public string Message { get; private set; }

        public Peer(int id, string name)
        {
            Id = id;
            Name = name;
        }

        public Peer(int id, string name, string message)
        {
            Id = id;
            Name = name;
            Message = message;
        }
    }
}
