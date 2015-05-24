using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace FMIFramMakerApp
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }



        private void btnMaker_Click(object sender, EventArgs e)
        {
            var m = new FrameMaker();
            List<byte> payload = (new MessageClientToServer24()).FrameMaker(0xffffffff,0x1,"Hi Server");
            List<byte> payload2 = (new MessageServerToClient21()).FrameMaker(0x12321231, "Hi Client");

            List<byte> payload3 = (new MessageServerToClient2A()).FrameMaker(0xffffffff, "Hi Client2a");


            List<byte> bufferMessageClientToServer = m.GenralFrameMaker(161, 0x0024, payload.ToList());
            List<byte> bufferMessageServertoclient = m.GenralFrameMaker(161, 0x0021, payload2.ToList());
            List<byte> bufferMessageServertoclient2A = m.GenralFrameMaker(161, 0x002a, payload3.ToList());
           
            
            uint messageid = 0x1;
            List<byte> ackFromServerMessage = m.GenralFrameMaker(161, 0x0025,  messageid.ToByteList());
      



            string f = "Message Client To Server : "+BitConverter.ToString(bufferMessageClientToServer.ToArray()).Replace("-", " ");
            string ack = "Message Ack Server To Client :" + BitConverter.ToString(ackFromServerMessage.ToArray()).Replace("-", " ");

            string messageserver = "Message  Server To Client  21  :" + BitConverter.ToString(bufferMessageServertoclient.ToArray()).Replace("-", " ");
            string messageserver2A = "Message  Server To Client -> 2A :" + BitConverter.ToString(bufferMessageServertoclient2A.ToArray()).Replace("-", " ");
            Console.WriteLine(f);
            Console.WriteLine(ack);
            Console.WriteLine(messageserver);
            Console.WriteLine(messageserver2A);
        }

    

     
    }
}
