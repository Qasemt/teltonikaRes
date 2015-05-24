using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FMIFramMakerApp
{

    /// <summary>
    /// page 48 document 
    /// </summary>
    public enum PacketId:byte
    {
        Ack = 6,
        Command = 10,
        DateTimeData = 14,
        Nak = 21,
        UnitIdEsn = 38,
        PvtData = 51,
        LegacyStopMessage = 135,
        LegacyTextMessage = 136,
        FleetManagementPacket = 161
    }

    public class FrameMaker
    {

        const int Etx = 0x03;
        private const int Dle = 0x10;//16

        private readonly List<byte> _finalgarminMessage = new List<byte>();

        /// <summary>
        /// page 6 document
        /// </summary>
        /// <param name="pid"></param>
        /// <param name="length"></param>
        /// <param name="payload"></param>
        public void FramMaker(byte pid, byte length, List<byte> payload)
        {

            var bufferdata = new List<byte>();

            //step1 : import Data Link Escape
            _finalgarminMessage.Add(Dle); // DLE 16

            //step2 : import id
            pid.ToByteList().ForEach(bufferdata.Add);

            //step3 : import len
            length.ToByteList().ForEach(bufferdata.Add);

           ((ushort) (0x0024)).ToByteList().ForEach(bufferdata.Add); //frame send message 24 client to server

            //step4  import message Data ..........
            payload.ForEach(bufferdata.Add);


            //step5 : calc chksum
            byte checksum = Calc_CheckSum(bufferdata);
            checksum.ToByteList().ForEach(bufferdata.Add);

            //step 6 : import Segment Data
            bufferdata.ForEach(_finalgarminMessage.Add);

            //step7 : import Data Link Escape
            _finalgarminMessage.Add(Dle); // DLE 16

            //step8 : End of Text
            _finalgarminMessage.Add(Etx); // ETX //3

        }

        /// <summary>
        /// page 20
        /// </summary>
        /// <param name="message"></param>
        /// <returns></returns>
        public List<byte> SendMessage(string message)
        {
            List<byte> payload = new List<byte>();
            //---- value is the number of seconds since 12:00 am December 31, 1989 UTC. A hex value of 0xFFFFFFFF represents an invalid time
            payload.Add(0xff);
            payload.Add(0xff);
            payload.Add(0xff);
            payload.Add(0xff);
            //-------------------------------------------------------------------------
            payload.Add(0x11); //page 20 document //--> message Id uniq
            payload.Add(0x00);
            payload.Add(0x00);
            payload.Add(0x00);

            // typedef struct /* D603 */
            //{
            // time_type origination_time;
            // uint32 unique_id;
            // uchar_t8  text_message[/* variable length, null-terminated string, 200 bytes max */];
            //}    client_to_server_open_text_msg_data_type;

            byte[] noneUtf8 = Encoding.ASCII.GetBytes(message);
       //     byte[] utf8 = Encoding.UTF8.GetBytes(message);
            noneUtf8.ToList().ForEach(payload.Add);
            FramMaker((byte)PacketId.FleetManagementPacket, (byte)(payload.Count + 2), payload);
            return _finalgarminMessage;
        }

        byte Calc_CheckSum(List<byte> buffer)
        {
            short cksum = 0;
            for (int x = 0; x < buffer.Count; x++)
            {
                cksum += buffer[x];
            }
            return (byte)Complement2(cksum);
        }
        int Complement2(int val)
        {
         //   return (val << 16) >> 16;


            int value = val;
            value = ~value; // NOT
            value = value + 1;
            return value;
        }



        //---------------------------- ack message 25 server to client 

        public List<byte> GenralFrameMaker(byte packetId,ushort subPacketId,List<byte> payLoad)
        {
            var bufferdata = new List<byte>();
           var finalgarminMessage = new List<byte>();
            //step1 : import Data Link Escape
            finalgarminMessage.Add(Dle); // DLE 16

            //step2 : import id //161
            (packetId).ToByteList().ForEach(bufferdata.Add);

            //---------------- Payload len ----------------------
            //step3 : import len
            byte length = (byte)(payLoad.Count+2);
        
            length.ToByteList().ForEach(bufferdata.Add);

            (subPacketId).ToByteList().ForEach(bufferdata.Add); 
            //step4  import message Data ..........
            payLoad.ForEach(bufferdata.Add);


            //step5 : calc chksum
            byte checksum = Calc_CheckSum(bufferdata);
            checksum.ToByteList().ForEach(bufferdata.Add);

            //step 6 : import Segment Data
            bufferdata.ForEach(finalgarminMessage.Add);

            //step7 : import Data Link Escape
            finalgarminMessage.Add(Dle); // DLE 16

            //step8 : End of Text
            finalgarminMessage.Add(Etx); // ETX //3

            return finalgarminMessage;
        }

    }
}
