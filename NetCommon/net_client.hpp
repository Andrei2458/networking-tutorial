#pragma once
#include "net_common.hpp"
#include "net_message.hpp"
#include "net_tsqueue.hpp"
#include "net_connection.hpp"

namespace olc {
    namespace net {
        template <typename T>
        // Responsible for setting up ASIO and setting up the connection
        // It acts as an access point for your app to talk to the server
        class client_interface {
            public:

                // Constructor and Destructor
                client_interface() : m_socket(m_context) {
                    // Initialize the socket with the io context, so it can do stuff
                }

                virtual ~client_interface() {
                    // If the client is destroyed(shutdown), always try discoinnect from server
                    Disconnect();
                }

                // Connect to server with hostname/ip-address and port
                bool Connect(std::string const& host, uint16_t const port) {
                    
                    try {
                        // Create connection
                        m_connection = std::make_unique<connection<T>>(); // TODO

                        // Resolve hostname/ip-address into tangiable physical address
                        asio::ip::tcp::resolver resolver(m_context);
                        m_endpoints = resolver.resolve(host, std::to_string(port));

                        // Tell the connection object to connect to server
                        m_connection->ConnectToServer(m_endpoints);

                        // Start context thread
                        thrContext = std::thread([this]() { m.context.run(); });


                    } catch(std::exception& e) {
                        std::cerr << " Client Exception: " << e.what() << "\n";
                    }

                    return false;
                }

                // Disconnect from server
                void Disconnect() {
                    // If conncetion exist, and is connected then...
                    if(IsConnected()) {
                        // ...disconnect from server gracefully
                        m_connection->Disconnect();
                    }

                    // Either way, we're also done with the asio context...
                    m_context.stop();
                    // ... and its thread
                    if(thrContext.joinable()) {
                        thrContext.join();
                    }

                    // Destroy the connection object
                    // releases the unique pointer
                    m_connection.release();
                }

                // Check if connection is still valid
                bool IsConnected() {
                    if(m_connection) {
                        return m_connection->IsConnected();
                    }
                    return false;
                }

                // Send message to server
                void Send(message<T> const& msg) {
                    if(IsConnected()) {
                        m_connection->Send(msg);
                    }
                }

                // Retrieve queue of messages from server (like a Get)
                tsqueue<owned_message<T>>& Incoming() {
                    return m_qMessagesIn;
                }

            protected:
                // client interface owns the asio context
                // asio context handles the data trasnfer
                asio::io_context m_context;
                // ...but needs a thread of its own to execute its work commands
                std::thread thrContext;
                // This is the hardware socket that is connected to the server
                asio::ip::tcp::socket m_socket;
                // The client has a single instance of a "connection" object, which handles data transfer
                std::unique_ptr<connection<T>> m_connection;

            private:
                // This is the thread safe queue of incoming messages from server
                tsqueue<owned_message<T>> m_qMessagesIn;
        };
    }
}