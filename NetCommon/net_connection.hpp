#pragma once
#include "net_common.hpp"
#include "net_tsqueue.hpp"
#include "net_message.hpp"

namespace olc {
    namespace net {
        // std::enable_shared_from_this enable us to create a shared from inside the class
        template <typename T>
        class connection : public std::enable_shared_from_this<connection<T>> {
            public:
                connection() {}
                virtual ~connection() {}
                
                // only called by clients
                bool ConnectToServer();
                // can be called by clients and servers
                bool Disconnect();
                // is the connection valid, open and currently active?
                bool IsConnected() const;

                // send a message
                bool Send(message<T> const& msg);
            
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


        };

    }
    
}