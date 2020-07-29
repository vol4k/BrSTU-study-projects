using System;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Text;
using System.Collections.Generic;

namespace Lab6Client
{
    class Program
    {
        static int counter = 0;
        static string adress;
        static int port;
        static void Main(string[] args)
        {
            Load(); // загружаем первый днс

            if (adress == null) return;

            while (true)
            {
                IPEndPoint ipPoint = new IPEndPoint(IPAddress.Parse(adress), port);
                Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                socket.Connect(ipPoint);
                Console.Write("Введите сообщение:");
                string message = Console.ReadLine();
                byte[] data = Encoding.Unicode.GetBytes(message);
                socket.Send(data);
                data = new byte[256];
                StringBuilder builder = new StringBuilder();
                int bytes = 0;
                do
                {
                    bytes = socket.Receive(data, data.Length, 0);
                    builder.Append(Encoding.Unicode.GetString(data, 0, bytes));
                }
                while (socket.Available > 0);

                Console.WriteLine("\t#Запрос №{0}", ++counter);

                Console.WriteLine("Ответ сервера: " + builder.ToString());
                socket.Shutdown(SocketShutdown.Both);
                socket.Close();
            }
        }

        static void Load()
        {
            FileStream file = new FileStream("D:\\dns.txt", FileMode.OpenOrCreate, FileAccess.Read);
            StreamReader reader = new StreamReader(file);
            if (file.Position != file.Length)
            {
                string Text = reader.ReadLine();
                string[] words = Text.Split(':');
                adress = words[0];
                port = Convert.ToInt32(words[1]);
            }
            else
            {
                adress = null;
                port = 0;
            }
            Console.WriteLine("Подключение к {0}:{1}", adress, port);
            reader.Close();
            file.Close();
        }
    }
}