using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using static Client_2.Protocol;

namespace Client_2
{
    internal class Client
    {
        private Socket socket;
        private int port = 12345;
        private string serverIp = "127.0.0.1";
        private RequestBuilder requestBuilder = new RequestBuilder();
        private string fileDirectory = "client_files";

        public void Connect()
        {
            socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            socket.Connect(IPAddress.Parse(serverIp), port);
            Console.WriteLine("Connected to server");
        }

        public bool SendAll(byte[] data, int size)
        {
            int totalSent = 0;
            while(totalSent < size)
            {
                int sent = socket.Send(data, totalSent, size - totalSent, SocketFlags.None);

                if(sent == 0)
                {
                    return false;
                }
                totalSent += sent;
            }
            return true;
        }

        public bool RecvAll(byte[] buffer, int size)
        {
            int totalReceived = 0;
            while(totalReceived < size)
            {
                int received = socket.Receive(buffer, totalReceived, size - totalReceived, SocketFlags.None);

                if(received == 0)
                {
                    return false;
                }
                totalReceived += received;
            }
            return true;
        }
        private bool SendRequest(byte[] header)
        {
            int headerSize = header.Length;
            int netSize = IPAddress.HostToNetworkOrder(headerSize);
            byte[] sizeBytes = BitConverter.GetBytes(netSize);

            if (!SendAll(sizeBytes, REQUEST_HEADER_SIZE_BYTES))
            {
                return false;
            }

            if (!SendAll(header, headerSize))
            {
                return false;
            }

            return true;
        }

        private Status ReceiveStatus()
        {
            byte[] buffer = new byte[STATUS_BYTES];
            RecvAll(buffer, STATUS_BYTES);
            Status status = (Status)buffer[0];
            return status;
        }

        private uint ReceivePayloadLength()
        {
            byte[] buffer = new byte[PAYLOAD_LENGTH_BYTES];
            RecvAll(buffer, PAYLOAD_LENGTH_BYTES);

            int netLength = BitConverter.ToInt32(buffer, 0);
            return (uint)IPAddress.NetworkToHostOrder(netLength);
        }

        public void Get(string fileName)
        {
            byte[] header = requestBuilder.BuildGetRequest(fileName);
            if (!SendRequest(header))
            {
                Console.WriteLine("Failed to send request");
                return;
            }

            Status status = ReceiveStatus();
            if (status == Status.FILE_NOT_FOUND)
            {
                Console.WriteLine("File not found");
                return;
            }

            uint payloadLength = ReceivePayloadLength();

            using var file = new FileStream(Path.Combine(fileDirectory, fileName), FileMode.Create, FileAccess.Write);

            byte[] buffer = new byte[CHUNK_SIZE];
            uint totalReceived = 0;

            while (totalReceived < payloadLength)
            {
                int toRead = (int)Math.Min(CHUNK_SIZE, payloadLength - totalReceived);

                if(!RecvAll(buffer, toRead))
                {
                    return;
                }
                file.Write(buffer, 0, toRead);

                totalReceived += (uint)toRead;
            }
            Console.WriteLine("File was successfully received");
        }

        public void List()
        {
            byte[] header = requestBuilder.BuildListRequest();

            if (!SendRequest(header))
            {
                Console.WriteLine("Failed to send request");
                return;
            }

            Status status = ReceiveStatus();

            if (status != Status.SUCCESS)
            {
                Console.WriteLine("Server error");
                return;
            }

            uint payloadLength = ReceivePayloadLength();

            byte[] buffer = new byte[payloadLength];
            if(!RecvAll(buffer, (int)payloadLength))
            {
                Console.WriteLine("Failed to receive payload");
                return;
            }

            Console.WriteLine(Encoding.UTF8.GetString(buffer));
        }

        public void Put(string fileName)
        {
            string path = Path.Combine(fileDirectory, fileName);

            if (!File.Exists(path))
            {
                Console.WriteLine("File not found");
                return;
            }

            uint fileSize = (uint)new FileInfo(path).Length;

            byte[] header = requestBuilder.BuildPutRequest(fileName, fileSize);

            if (!SendRequest(header))
            {
                Console.WriteLine("Failed to send request");
                return;
            }

            using var file = new FileStream(path, FileMode.Open, FileAccess.Read);

            byte[] buffer = new byte[CHUNK_SIZE];
            uint totalSent = 0;

            while (totalSent < fileSize)
            {
                int toSend = (int)Math.Min(CHUNK_SIZE, fileSize - totalSent);

                int read = file.Read(buffer, 0, toSend);
                if (read <= 0)
                {
                    return;
                }

                if (!SendAll(buffer, read))
                {
                    return;
                }

                totalSent += (uint)read;
            }

            Status status = ReceiveStatus();
            if (status == Status.SUCCESS)
            {
                Console.WriteLine("File was successfully uploaded to the server");
            }
        }

        public void Delete(string fileName)
        {
            byte[] header = requestBuilder.BuildDeleteRequest(fileName);

            if (!SendRequest(header))
            {
                Console.WriteLine("Failed to send request");
                return;
            }

            Status status = ReceiveStatus();
            if (status == Status.FILE_NOT_FOUND)
            {
                Console.WriteLine("File not found");
                return;
            }
            else if(status == Status.SUCCESS)
            {
                Console.WriteLine("File was successfully deleted from server");
                return;
            }
        }

        public void Info(string fileName)
        {
            byte[] header = requestBuilder.BuildInfoRequest(fileName);

            if (!SendRequest(header))
            {
                Console.WriteLine("Failed to send request");
                return;
            }

            if (ReceiveStatus() == Status.FILE_NOT_FOUND)
            {
                Console.WriteLine("File not found");
                return;
            }

            uint size = ReceivePayloadLength();
            Console.WriteLine($"File size: {size} bytes");
        }
    }
}
