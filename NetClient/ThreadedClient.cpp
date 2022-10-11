#include "../NetCommon/olc_net.hpp" 
#include <iostream>

enum class CustomMsgTypes : uint32_t {
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage
};

// class CustomClient : public olc::net::client_interface<CustomMsgTypes>
class CustomClient : public olc::net::client_interface<CustomMsgTypes> {
    
    public:
        void PingServer() {
            olc::net::message<CustomMsgTypes> msg;
            msg.header.id = CustomMsgTypes::ServerPing;

            // determine the RTT time (caution with this implementation since it is dependent on system cloc)
            // 1. get current time
            std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
            // 2. load the time in the message
            msg << timeNow;
            Send(msg);
        }
   
};
int main()
{
    CustomClient c;
    c.Connect("127.0.0.1", 60000);

    bool bQuit = false;
    int option, old_option;
    std::thread keyReader([&option, &bQuit]()
        {
            while(!bQuit)
            {
                std::cout << "[1].Ping\n"
                        "[2].Do nothing...\n"
                        "[99].Quit\n";

                std::cin >> option; 
            }
        });
    //No reason to wait for the thread to stop
    keyReader.detach();
    do
    {
        if (option == 1 && old_option == option)
        {
            c.PingServer();
            option = 0;
        }
        else if (option == 2 && old_option == option)
        {
            std::cout << "[CLIENT] Do nothing...";
            option = 0;
        }

        if (c.IsConnected())
        {
            if (!c.Incoming().empty())
            {
                auto msg = c.Incoming().pop_front().msg;

                switch (msg.header.id)
                {
                case CustomMsgTypes::ServerAccept:
                {
                    std::cout << "Server Acepted Connection\n";
                    break;
                }

                case CustomMsgTypes::ServerPing:
                {
                    std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
                    std::chrono::system_clock::time_point timeThen;
                    msg >> timeThen;
                    std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
                    break;
                }

                case CustomMsgTypes::ServerMessage:
                {
                    uint32_t clientID;
                    msg >> clientID;
                    std::cout << "Message from: " << clientID << "\n";
                    break;
                }
                }
            }
        }
        else
        {
            std::cout << "The Server was thunderstruck!\n";
            option = 99;
        }        
        old_option = option;

    } while (option != 99);

    bQuit = true;
}