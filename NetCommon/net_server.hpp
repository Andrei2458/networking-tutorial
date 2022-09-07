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
                server_interface(uint16_t port)
                    : m_asioAcceptor(m_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4() , port)) {

                }

                virtual ~server_interface() {
                    Stop();
                }

                bool Start() {
                    try {
                        // need to issue some work before the start of the context
                        WaitForClientConnection();
                        // start context in a thread of its own
                        m_threadContext = std::thread([this]() { m_asioContext.run(); });
                    }
                    catch (std::exception& e) {

                        // Something prohibeted the server from listening
                        std::cerr << "[SERVER] Exception: " << e.what() << "\n";
                        return false;
                    }

                    std::cout << "[SERVER] Started!\n";
                    return true;
                }

                void Stop() {
                    // Request the context to close
                    m_asioContext.stop();

                    // Tidy up the context thread
                    if(m_threadContext.joinable()) {
                        m_threadContext.join();
                    }
                    // Inform that server stopped
                    std::cout << "Server Stopped\n!";
                }

                // ASYNC - Instruct asio to wait for connection
                void WaitForClientConnection() {
                    m_asioAcceptor.async_accept(
                        [this](std::error_code ec, asio::ip::tcp::socket socket)
                        {
                           if(!ec) {

                                // NO ERRORS - CONNECTION NOT ACCEPTED BY SERVER YET
                                std::cout << "[Server] New connection: " << socket.remote_endpoint() << "\n";
                            
                                std::shared_ptr<connection<T>> newconn = 
                                    std::make_shared<connection<T>>(connection<T>::owner::server, 
                                        m_asioContext, std::move(socket), m_qMessagesIn);

                                    // Give the server a change to deny connection
                                    if(OnClientConnect(newconn)) {
                                        // Conncetion accepted by the server
                                        // add the current connection in the server's list of conn
                                        m_deqConnections.push_back(std::move(newconn));
                                        // provide an id to the conn
                                        m_deqConnections.back()->ConnectToClient(nIDCounter++);

                                        std::cout << "[" << m_deqConnections.back()->GetID() << "] Connection Approved!\n";

                                    } else {
                                        std::cout << "[-----] Connection Denied\n";
                                    }

                            } else {
                                // Error has occured durring acceptance
                                std::cout << "[SERVER New Connection Error: " << ec.message() << "\n";
                            } 

                            // Prime the asio context with more work - simply wait
                            // for another connection
                            WaitForClientConnection();
                        }
                    );
                }

                // Send a message to a specific client
                void MessageClient(std::shared_ptr<connection<T>> client, message<T> const& msg) {
                    if(client && client->IsConnected()) {
                        client->Send(msg);
                    } else {
                        // if the client is not connceted anymore - we can call the function that takes care of a disconnected client
                        OnClientDisconnect(client);
                        // client is not longer valid => delete client
                        client.reset();
                        m_deqConnections.erase(
                            std::remove(begin(m_deqConnections), end(m_deqConnections), client), end(m_deqConnections)
                        );
                    }  
                }

                // Send message to all clients - with option to ignore a client
                void MessageAllClients(message<T> const& msg, std::shared_ptr<connection<T>> pIgnoreClient = nullptr) {

                    bool bInvalidClientsExists = false;

                    for(auto& client : m_deqConnections) {
                        // Check client is connected...
                        if (client && client->IsConnected()) {
                            // ...it is!
                            if(client != pIgnoreClient) {
                                client->Send(msg);
                            }
                        } else {
                            // The client couldn't be contacte, so asssume it has disconnected
                            OnClientDisconnect(client);
                            client.reset();
                            bInvalidClientsExists = true;
                        }
                    }

                    // Erase not valid connections outside the loop
                    // Otherwise we would change the collection while we were iterrating through it <- NOT OK
                    if(bInvalidClientsExists) {
                        m_deqConnections.erase(std::remove(begin(m_deqConnections), end(m_deqConnections), nullptr), end(m_deqConnections));
                    }
                }

                void Update(size_t nMaxMessages = -1) {

                    size_t nMessageCount = 0;
                    while (nMessageCount < nMaxMessages && !m_qMessagesIn.empty()) {

                        // Grab the front message (the oldest)
                        auto msg = m_qMessagesIn.pop_front();

                        // Pass to message handler
                        OnMessage(msg.remote, msg.msg);

                        nMessageCount++;
                    }

                    
                }
            
            protected:
                // since we know this is a base class - we know that other classes will inherit it
                // protected gives similar to public access rights for classes that inherit the class
                // Called when a client connects, you can veto the connection by returning false
                virtual bool OnClientConnect(std::shared_ptr<connection<T>> client) {

                    return false;
                }

                // Called when a client appears to have disconnected
                virtual void OnClientDisconnect(std::shared_ptr<connection<T>> client) {
                    // to remove a player when it disconnects
                }

                // Called when a message arrives
                // Tells the server what to do when a message arrives
                virtual void OnMessage(std::shared_ptr<connection<T>> client, message<T>& msg) {

                }

                // Thread Safe Queue for incoming messages
                tsqueue<owned_message<T>> m_qMessagesIn;

                // Container of active validated connections
                std::deque<std::shared_ptr<connection<T>>> m_deqConnections;

                // In order to work it needs a context -> a the context needs a thread
                // Order of declaration is important - it is also the order of initialisation
                asio::io_context m_asioContext;
                std::thread m_threadContext;

                // One of the things that the server doesn't have is a socket of its own
                // It kind of does - but it's hidden from us by the asio library
                // We need to get the sockets of the connected clients
                // We can do this via an asio object called an acceptor
                asio::ip::tcp::acceptor m_asioAcceptor;

                // every client in the system is represented by a numerical Identifier (nID)
                // the ID number is not relevant as long as it's unique for every connection
                uint32_t nIDCounter = 10000;
                // this information will be transmited to the clients when they connect
                





        };
    }
}