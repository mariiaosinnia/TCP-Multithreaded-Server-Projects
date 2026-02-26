using static Client_2.Protocol;

namespace Client_2
{
    internal class Program
    {
        static Command ParseCommand(string str)
        {
            return str switch
            {
                "GET" => Command.Get,
                "LIST" => Command.List,
                "PUT" => Command.Put,
                "DELETE" => Command.Delete,
                "INFO" => Command.Info,
                _ => Command.Unknown
            };
        }

        static void Main()
        {
            Client client = new Client();
            client.Connect();

            while (true)
            {
                Console.Write("> ");
                string? line = Console.ReadLine();

                if (string.IsNullOrWhiteSpace(line))
                {
                    continue;
                }

                if (line.Equals("exit", StringComparison.OrdinalIgnoreCase))
                {
                    break;
                }

                var parts = line.Split(' ', StringSplitOptions.RemoveEmptyEntries);
                var commandStr = parts[0];
                var args = parts.Skip(1).ToList();

                Command cmd = ParseCommand(commandStr);

                switch (cmd)
                {
                    case Command.Get:
                        if (args.Count == 0)
                        {
                            Console.WriteLine("Not enough arguments");
                            break;
                        }
                        client.Get(args[0]);
                        break;

                    case Command.List:
                        client.List();
                        break;

                    case Command.Put:
                        if (args.Count == 0)
                        {
                            Console.WriteLine("Not enough arguments");
                            break;
                        }
                        client.Put(args[0]);
                        break;

                    case Command.Delete:
                        if (args.Count == 0)
                        {
                            Console.WriteLine("Not enough arguments");
                            break;
                        }
                        client.Delete(args[0]);
                        break;

                    case Command.Info:
                        if (args.Count == 0)
                        {
                            Console.WriteLine("Not enough arguments");
                            break;
                        }
                        client.Info(args[0]);
                        break;

                    case Command.Unknown:
                        Console.WriteLine("Unknown command");
                        break;
                }
            }

            Console.WriteLine("Client terminated.");
        }
    }
}
