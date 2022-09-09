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
    
   
};
int main() {
    CustomClient c;
    c.Connect("127.0.0.1", 60000);


    return 0;
}
