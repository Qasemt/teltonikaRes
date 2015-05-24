using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FMIFramMakerApp
{
    public static class ExtentionMethods
    {
        public static List<byte> ToByteList(this int value)
        {
             byte[] tmpData = BitConverter.GetBytes(value);
             return tmpData.ToList();
        }
        public static List<byte> ToByteList(this uint value)
        {
            byte[] tmpData = BitConverter.GetBytes(value);
            return tmpData.ToList();
        }
        public static List<byte> ToByteList(this short value)
        {
            byte[] tmpData = BitConverter.GetBytes(value);
            return tmpData.ToList();
        }
        public static List<byte> ToByteList(this ushort value)
        {
            byte[] tmpData = BitConverter.GetBytes(value);
            return tmpData.ToList();
        }

        public static List<byte> ToByteList(this byte value)
        {
            var d = (new List<byte>());
            d.Add(value);
            return d;
        }
    
    }
}
