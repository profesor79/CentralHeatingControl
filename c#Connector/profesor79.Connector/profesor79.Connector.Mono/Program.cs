using System;
using System.Text;
using SerialPortLib;

namespace profesor79.Connector.Mono
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            var serialPort = new SerialPortInput();

            // Listen to Serial Port events

            serialPort.ConnectionStatusChanged +=
                delegate(object sender, ConnectionStatusChangedEventArgs args2)
                {
                    Console.WriteLine("Connected = {0}", args2.Connected);
                };

            serialPort.MessageReceived +=
                delegate(object sender, MessageReceivedEventArgs args3)
                {
                    Console.WriteLine("Received message: {0}", BitConverter.ToString(args3.Data));
                };

            // Set port options
            serialPort.SetPort("/dev/ttyUSB0", 115200);

            // Connect the serial port
            serialPort.Connect();

            // Send a message
            var message = Encoding.UTF8.GetBytes("Hello World!");
            serialPort.SendMessage(message);
        }
    }
}