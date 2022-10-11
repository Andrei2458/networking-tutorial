#pragma once
#include "net_common.hpp"
#include "net_tsqueue.hpp"
#include "net_message.hpp"

namespace olc {

    namespace net {

        // std::enable_shared_from_this enable us to create a shared pointer, internally, from inside the class
        template <typename T>
        class connection : public std::enable_shared_from_this<connection<T>> {
        public:

            enum class owner {
                server,
                client
            };

            connection(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qIn) 
                : m_asioContext(asioContext), m_socket(std::move(socket)), m_qMessagesIn(qIn) {
                
                m_nOwnerType = parent;
            }
                
            virtual ~connection() {

            }

            uint32_t GetID() const {
                    
                return id;
            }
                

        public:
            void ConnectToClient(uint32_t uid = 0) {

                if (m_nOwnerType == owner::server) {

                    if (m_socket.is_open()) {

                        id = uid;
                        std::cout << "[SERVER] will try to read a new header!\n";
                        ReadHeader();                            
                    }
                }
            }
                
            // only called by clients
            void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints) {
                //only clients can connect to server
                if (m_nOwnerType == owner::client) {
                    // Requests asio attempts to connect to an endpoint
                    asio::async_connect(m_socket, endpoints,
                        [this](std::error_code ec, asio::ip::tcp::endpoint endpoint){
                        if (!ec) {
                            ReadHeader();
                        }
                        else {
                            std::cout << "[CLIENT] Can not connect to server...\n";
                        }
                    });
                }
            }


            // can be called by clients and servers
            void Disconnect() {
                if (IsConnected()) {
                    asio::post(m_asioContext, [this]() { m_socket.close(); });
                }
            }

            // is the connection valid, open and currently active?
            bool IsConnected() const {

                return m_socket.is_open();
            }

        public:
            // send a message
            // post function is used to inject work into a context
            void Send(const message<T>& msg) {
                asio::post(m_asioContext,
                    [this, msg](){
                        // check if messages are already being written
                        bool bWritingMessage = !m_qMessagesOut.empty();
                        //add are message to the queue
                        m_qMessagesOut.push_back(msg);

                        if (!bWritingMessage) {
                            WriteHeader();
                        }
                    });
            }
            
        private: 
            // ASYNC - Prime context ready to read a message header
            void ReadHeader() {
                asio:async_read(m_socket, asio::buffer(&m_msgTemporaryIn.header, sizeof(message_header<T>)),
                    [this](std::error_code ec, std::size_t length) {
                        if (!ec){
                            if (m_msgTemporaryIn.header.size > 0) {
                                std::cout << "[SERVER] Just read async a Header.\n";
                                // resize the temporary variable corresponding to the size of the received message
                                m_msgTemporaryIn.body.resize(m_msgTemporaryIn.header.size);
                                ReadBody();
                            } else {
                                // empty message received
                                AddToIncomingMessageQueue();
                            }

                        } else {
                            std::cout << "[" << id << "] Read Header Fail.\n";
                            m_socket.close();
                        }
                    });
            }

            // ASYNC - prime context ready to read a message body
            void ReadBody() {
                asio::async_read(m_socket, asio::buffer(m_msgTemporaryIn.body.data(), m_msgTemporaryIn.header.size),
                    [this](std::error_code ec, std::size_t length) {
                        if (!ec) {
                            AddToIncomingMessageQueue();
                        } else {
                            std::cout << "[" << id << "] Read Body Fail.\n";
                            m_socket.close();
                        }
                    }
                );
            }

            // ASYNC - prime context ready to write a message header
            void WriteHeader() {
                asio::async_write(m_socket, asio::buffer(&m_qMessagesOut.front().header, sizeof(message_header<T>)),
                    [this](std::error_code ec, std::size_t length){
                        if (!ec) {
                            if (m_qMessagesOut.front().body.size() > 0) {
                                WriteBody();
                            } else {
                                // if the message does not have a body we have to remove it
                                m_qMessagesOut.pop_front();

                                // we can also check if there are other messages to be sent
                                if (!m_qMessagesOut.empty()) {
                                    WriteHeader();
                                }
                            }

                        } else {
                            std::cout << "[" << id << "} Write Header Fail.\n";
                            m_socket.close();
                        }
                    });
            }
                
            // ASYNC - prime context ready to write a message body
            void WriteBody() {
                asio::async_write(m_socket, asio::buffer(m_qMessagesOut.front().body.data(), m_qMessagesOut.front().body.size()),
                    [this](std::error_code ec, std::size_t length){
                        if (!ec) {
                            m_qMessagesOut.pop_front();

                            if (!m_qMessagesOut.empty()) {
                                WriteHeader();
                            }
                        } else {
                            std::cout << "[" << id << "] Write Body Fail.\n";
                            m_socket.close();
                        }
                    });
            }

            void AddToIncomingMessageQueue() {
                if (m_nOwnerType == owner::server) {
                    m_qMessagesIn.push_back({ this->shared_from_this(), m_msgTemporaryIn });
                } else {
                    // if the message comes from a client
                    // clients have only one connection so it is not relevant 
                    m_qMessagesIn.push_back({ nullptr, m_msgTemporaryIn });
                }
                // because we completely read a message (header and body at this point)
                // we can read another message (starting with the header)
                ReadHeader();
            }

        protected:
            // Each connection has an unique socket to a remote
            asio::ip::tcp::socket m_socket;

            // This context is shared with the whole asio instance
            asio::io_context& m_asioContext;

            // This queue holds all messages to be sent to the remote side
            // of this connection
            tsqueue<message<T>> m_qMessagesOut;

            // This queue holds all messages that have been recieved from
            // the remote side of this connection. 
            // It is a reference as the "owner" of this connection is expected to 
            // provide a queue
            tsqueue<owned_message<T>>& m_qMessagesIn;
            message<T> m_msgTemporaryIn;


            // The owner decides how some of the connection behaves
            owner m_nOwnerType = owner::server;
            uint32_t id = 0;


        };

    }
    
}