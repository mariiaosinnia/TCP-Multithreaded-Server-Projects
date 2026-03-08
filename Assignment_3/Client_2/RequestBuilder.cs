using System.Net;
using System.Text;
using static Client_2.Protocol;

namespace Client_2
{
    internal class RequestBuilder
    {
        private byte[] SerializeCommand(Command command)
        {
            return new byte[] { (byte)command };
        }

        private byte[] SerializeFileName(string fileName)
        {
            List<byte> result = new List<byte>();

            byte[] fileNameBytes = Encoding.UTF8.GetBytes(fileName);

            ushort length = (ushort)fileNameBytes.Length;
            ushort netLength = (ushort)IPAddress.HostToNetworkOrder((short)length);

            result.AddRange(BitConverter.GetBytes(netLength));
            result.AddRange(fileNameBytes);

            return result.ToArray();
        }

        private byte[] SerializeFileSize(uint fileSize)
        {
            int netFileSize = IPAddress.HostToNetworkOrder((int)fileSize);
            return BitConverter.GetBytes(netFileSize);
        }

        public byte[] BuildGetRequest(string fileName)
        {
            List<byte> request = new List<byte>();
            request.AddRange(SerializeCommand(Command.Get));
            request.AddRange(SerializeFileName(fileName));
            return request.ToArray();
        }

        public byte[] BuildListRequest()
        {
            return SerializeCommand(Command.List);
        }

        public byte[] BuildPutRequest(string fileName, uint fileSize)
        {
            List<byte> request = new List<byte>();
            request.AddRange(SerializeCommand(Command.Put));
            request.AddRange(SerializeFileName(fileName));
            request.AddRange(SerializeFileSize(fileSize));
            return request.ToArray();
        }

        public byte[] BuildDeleteRequest(string fileName)
        {
            List<byte> request = new List<byte>();
            request.AddRange(SerializeCommand(Command.Delete));
            request.AddRange(SerializeFileName(fileName));
            return request.ToArray();
        }

        public byte[] BuildInfoRequest(string fileName)
        {
            List<byte> request = new List<byte>();
            request.AddRange(SerializeCommand(Command.Info));
            request.AddRange(SerializeFileName(fileName));
            return request.ToArray();
        }
    }
}
