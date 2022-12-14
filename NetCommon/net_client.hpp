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
            client_interface() {}

            virtual ~client_interface() {
                // If the client is destroyed(shutdown), always try discoinnect from server
                Disconnect();
            }

        
        public:
            // Connect to server with hostname/ip-address and port
            bool Connect(const std::string& host, const uint16_t port) {
                    
                try {
                    
                    // Resolve hostname/ip-address into tangible physical address
                    asio::ip::tcp::resolver resolver(m_context);
                    // if the host can not be resolved -> an exception will be thrown and caught by our catch block
                    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

                    // Create connection
                    m_connection = std::make_unique<connection<T>>(
                        connection<T>::owner::client,
                        m_context,
                        asio::ip::tcp::socket(m_context),
                        m_qMessagesIn);

                    // Tell the connection object to connect to server
                    m_connection->ConnectToServer(endpoints);

                    // Start context thread
                    thrContext = std::thread([this]() { m_context.run(); });


                } catch(std::exception& e) {
                    std::cerr << " Client Exception: " << e.what() << "\n";
                    return false;
                }

                return true;
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
            // The client has a single instance of a "connection" object, which handles data transfer
            std::unique_ptr<connection<T>> m_connection;

        private:
            // This is the thread safe queue of incoming messages from server
            tsqueue<owned_message<T>> m_qMessagesIn;
        };
    }
}