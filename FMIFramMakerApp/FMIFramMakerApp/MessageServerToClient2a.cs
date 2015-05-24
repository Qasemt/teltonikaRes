using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FMIFramMakerApp
{
    /// <summary>
    /// page 11 
    /// </summary>
    public class MessageServerToClient2A
    {
        public int TimeSec;
        byte _idSize;
        byte _messageType;
        ushort _reserved;  /* set to 0 */
        public int MessageId;
        public string Message;


        public List<byte> FrameMaker(uint timeSec, string message)
        {
            _idSize = 0;
            _messageType = 0;
            _reserved = 0;

            var bufferdata = new List<byte>();
            timeSec.ToByteList().ForEach(bufferdata.Add);
            _idSize.ToByteList().ForEach(bufferdata.Add);
            _messageType.ToByteList().ForEach(bufferdata.Add);
            _reserved.ToByteList().ForEach(bufferdata.Add);
            MessageId.ToByteList().ForEach(bufferdata.Add);
            
            Encoding.ASCII.GetBytes(message).ToList().ForEach(bufferdata.Add);
            return bufferdata;
        }
    }
}
