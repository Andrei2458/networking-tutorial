#include <iostream>
#include "../NetCommon/olc_net.hpp"

enum class CustomMsgTypes : uint32_t {
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage
};

class CustomServer : public olc::net::server_interface<CustomMsgTypes> {

    public: 
        CustomServer(uint16_t nPort) : olc::net::server_interface<CustomMsgTypes>(nPort) {

        }
    
    protected:
        
        virtual bool OnClientConnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client) {
            olc::net::message<CustomMsgTypes> msg;
            msg.header.id = CustomMsgTypes::ServerAccept;
            client->Send(msg);
            return true;
        }

        // Called when a client appears to have disconnected
        virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client) {
            std::cout << "[SERVER] Removing client [" << client->GetID() << "]\n";
        }

        // Called when a message arrives
        virtual void OnMessage(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client, olc::net::message<CustomMsgTypes>& msg) {
            switch (msg.header.id) {
                case CustomMsgTypes::ServerPing:
                {
                    std::cout << "[" << client->GetID() << "]: Server Ping\n";
                    // simply bounce message back to client
                    client->Send(msg);
                    break;
                }
                default:
                {
                    std::cout << "[SERVER] Unknown message from the client\n";
                    break;
                }
                

            }
        }

};

int main() {
    CustomServer server(60000);
    server.Start();

    while(1) {
        server.Update(-1, true);
    }

    return 0;
}

