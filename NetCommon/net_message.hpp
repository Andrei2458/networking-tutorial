#pragma once
#include "net_common.hpp"

namespace olc {
    namespace net {
        // Message header is sent at start of all messages. The template allows us 
        // to use "enum class" to ensure that messages are valid at compile time

        template <typename T>
        struct message_header {
            T id{};
            uint32_t size = 0;
            
        };

        template <typename T>
        struct message {
            message_header<T> header{};
            std::vector<uint8_t> body;

            size_t size() const {
                return sizeof(message_header<T>) + body.size();
            }

            // Override for std::cout compatibility
            // produces friendly description of messages
            // 'friend' as it should be available from every possible location
            friend std::ostream& operator<< (std::ostream os, message<T> const& msg) {
                os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;
                return os;
            }

            //Pushes any POD-like data into the message buffer
            // if the user pushes a float -> DataType becomes float, if the users pushes an int, DataType becomes int ...
            template<typename DataType>
            friend message<T>& operator<< (message<T>& msg, DataType const& data) {

                // check data the data type provided to this function is of type standard layout (google it)
                // due to static_assert the message is posted if the DataType is not standard layot (expression evaluated as false)
                static_assert(std::is_standard_layout<DataType>::value, "Data is to complex to be pushed into vector!\n");

                // Cache current size of vector, as this will be the point we insert the data
                size_t i = msg.body.size();

                // Resize the vector by the size of the data being pushed
                msg.body.resize(msg.body.size() + sizeof(DataType));

                // Physically copy the data into the newly allocated vector space
                std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

                // Recalculate the message size
                msg.header.size = msg.size();

                // Return the target message so it can be "chained"
                return msg;
            }

            template<typename DataType> 
            friend message<T>& operator>> (message<T>& msg, DataType& data) {
                // Check that the type of the data being pushed is trivially copyable
                static_assert(std::is_standard_layout<DataType>::value, "Data is to complex to be pushed into vector!\n");
            
                // Get the location at the end of the vector where the pulled data starts
                size_t i = msg.body.size() - sizeof(DataType);

                // Physically copy the data from the vector into the user variable
                std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

                // Shrink the vector to remove read bytes, and reset end position
                msg.body.resize(i);

                // Recalculate the message size
                msg.body.resize(i);

                // Return the target message so it can be "chained"
                return msg;

            }
        };

        
        
    }
}