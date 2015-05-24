using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FMIFramMakerApp
{
  public  class MessageServerToClient21
    {
        public int TimeSec;
        public int MessageId;
        public string Message;

        public List<byte> FrameMaker(uint timeSec, string message)
        {
            var bufferdata = new List<byte>();
            timeSec.ToByteList().ForEach(bufferdata.Add);
            Encoding.ASCII.GetBytes(message).ToList().ForEach(bufferdata.Add);
            return bufferdata;
        }
    }
}
