using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace Lab6Server
{
    class Program
    {
        static int counter = 0;
        static string adress;
        static int port;
        static void Main(string[] args)
        {
            hug();

            IPEndPoint ipPoint = new IPEndPoint(IPAddress.Parse(adress), port);
            Socket listenSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            listenSocket.Bind(ipPoint);
            Console.WriteLine("Сервер запущен. Ожидание подключений...");
            while (true)
            {
                listenSocket.Listen(10);
                Socket handler = listenSocket.Accept();
                StringBuilder builder = new StringBuilder();
                int bytes = 0;
                byte[] data = new byte[256];
                do
                {
                    bytes = handler.Receive(data);
                    builder.Append(Encoding.Unicode.GetString(data, 0, bytes));
                }
                while (handler.Available > 0);

                string request = Search(builder.ToString());

                Console.WriteLine("\t#Запрос №{0}", ++counter);

                if (request == null)
                {
                    Console.WriteLine("Нужных данных на сервере найдено не было\n" +
                        "Попытка повторения запроса к соседнему серверу...");

                    Tuple<string, int> next = LoadNext();

                    if (next.Item1 == null) {
                        data = Encoding.Unicode.GetBytes("no data\n");
                        handler.Send(data);
                        Console.WriteLine("Больше серверов нет");
                        continue;
                    }

                    IPEndPoint ipPointNext = new IPEndPoint(IPAddress.Parse(next.Item1), next.Item2);
                    Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                    socket.Connect(ipPointNext);
                    socket.Send(data);

                    builder = new StringBuilder();
                    bytes = 0;
                    data = new byte[256];


                    do
                    {
                        bytes = socket.Receive(data, data.Length, 0);
                        builder.Append(Encoding.Unicode.GetString(data, 0, bytes));
                    }
                    while (socket.Available > 0);

                    if (builder.ToString() == "no data\n")
                    {
                        data = Encoding.Unicode.GetBytes("no data\n");
                        handler.Send(data);
                        Console.WriteLine("Данные не были найдены");
                    }
                    else
                    {
                        data = Encoding.Unicode.GetBytes(builder.ToString() + '\n');
                        handler.Send(data);
                        Console.WriteLine("Данные были отправлены");
                    }
                    socket.Shutdown(SocketShutdown.Both);
                    socket.Close();
                }
                else
                {
                    request += '\n';
                    data = Encoding.Unicode.GetBytes(request);
                    handler.Send(data);
                    Console.WriteLine("Данные были отправлены");
                }

                handler.Shutdown(SocketShutdown.Both);
                handler.Close();
            }
        }

        static void hug()
        {
            Console.WriteLine("Введите данные: ip:port");
            string Text = Console.ReadLine();
            FileStream file = new FileStream("D:\\dns.txt", FileMode.Append, FileAccess.Write);
            StreamWriter writer = new StreamWriter(file);
            writer.WriteLine(Text);
            string[] data = Text.Split(':');
            adress = data[0];
            port = Convert.ToInt32(data[1]);
            writer.Close();
            file.Close();
        }

        static Tuple<string,int> LoadNext()
        {
            FileStream file = new FileStream("D:\\dns.txt", FileMode.OpenOrCreate, FileAccess.Read);
            StreamReader reader = new StreamReader(file);
            Tuple<string, int> res = new Tuple<string, int>(null, 0);
            string[] words = { null, null };

            bool writeNext = false;

            while (!reader.EndOfStream)
            {
                string Text = reader.ReadLine();
                words = Text.Split(':');
                if (writeNext)
                {
                    res = new Tuple<string, int>(words[0], Convert.ToInt32(words[1]));
                    break;
                }
                else if (words[0] == adress && Convert.ToInt32(words[1]) == port)
                {
                    writeNext = true;
                }
            }
            Console.WriteLine("Подключение к {0}:{1}", words[0], words[1]);

            reader.Close();
            file.Close();
            return res;
        }

        static string Search(string request)
        {
            if(request.IndexOf('\0') >= 0)
             request = request.Remove(request.IndexOf('\0'));
            request = request.ToLower();
            FileStream file = new FileStream(string.Format("D:\\data_{0}_{1}.txt", adress, port), FileMode.OpenOrCreate, FileAccess.Read);
            StreamReader reader = new StreamReader(file);
            string res = null;
            while (!reader.EndOfStream)
            {
                string Text = reader.ReadLine();
                string[] words = Text.Split(' ');
                if (words[0] == request)
                    res = words[1];
            }
            reader.Close();
            file.Close();
            return res;
        }
    }

}