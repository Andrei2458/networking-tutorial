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
int main() {
    CustomClient c;
    c.Connect("127.0.0.1", 60000);

    bool bQuit = false;
    while (!bQuit) {

        // get input from user
        int input = 0;
        std::cout << "Choose a message to send...\n";
        std::cin >> input;
        if (input > 0 && input < 4) {
            switch (input)
            {
            case 1:
                c.PingServer();
                break;
            
            case 2:

            
            case 3:
                bQuit = true;
                break;
            }
        }

        // Is the client still connecte
        if (c.IsConnected()) {
            // Are there any incoming messages?
            if (!c.Incoming().empty()){

                auto msg = c.Incoming().pop_front().msg;

                switch (msg.header.id) {
                    case CustomMsgTypes::ServerPing:
                        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
                        std::chrono::system_clock::time_point timeThen;
                        msg >> timeThen;
                        std::cout << "Ping: " << std::chrono::duration<double>(timeNow-timeThen).count() << "\n";
                }
            }
        } else { 
            // if the client is not connecte
            std::cout << "Server Down\n";
            bQuit = true;
        }
    }


    return 0;
}
