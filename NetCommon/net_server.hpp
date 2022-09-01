#pragma once

#include "net_common.hpp"
#include "net_tsqueue.hpp"
#include "net_message.hpp"
#include "net_connection.hpp"

namespace olc {
    namespace net {
        
        template <typename T>
        class server_interface {
            
            public:
                // port number where the server will listen to
                server_interface(uint16_t port) {

                }

                virtual ~server_interface() {

                }

                bool Start() {

                }

                bool Stop() {

                }

                // ASYNC - Instruct asio to wait for connection
                void WaitForClientConnection() {

                }

                // Send a message to a specific client
                void MessageClient(std::shared_ptr<connection<T>> client, message<T> const& msg)

                // Send message to all clients - with option to ignore a client
                void MessageAllClients(message<T>> const& msg, std::shared_ptr<connection<T>> pIgnoreClient = nullptr)
            
            protected:
                // Called when a client connects, you can veto the connection by returning false
                virtual bool OnClientConnect(std::shared_ptr<connection<T>> client) {

                    return false;
                }
        };
    }
}