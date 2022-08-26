#include <iostream>
// for a delay
#include <chrono>
#include <thread>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>


int main() {

    asio::error_code ec;

    // Create a "context" -- the platform specific interface
    asio::io_context context;

    // Get the address of something we want to connect to
    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34"), 80);

    // Create a socket - the context will deliver the implementation
    // ip::tcp::socker = a networking socket
    asio::ip::tcp::socket socket(context);

    // tell the socket to try and connect
    socket.connect(endpoint, ec);

    // check if there was an error
    if(!ec) {
        std::cout << "Connected!\n";
    } else {
        std::cout << "Failed to connect to address:\n" << ec.message() << "\n";
    }

    if(socket.is_open()) {
        std::string sReqiuest = 
            "GET /index.html HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Connection: close\r\n\r\n";
        
        socket.write_some(asio::buffer(sReqiuest.data(), sReqiuest.size()), ec);
        // really bad practice to hardcode pause times
        // using namespace std::chrono_literals; // namespace for '200ms'
        // std::this_thread::sleep_for(200ms);

        // asio provides a wait function until there is data to read
        // even this does not work great - it does not now how much time to wait for all the data so there might be some data skipped
        // here we need asyncronous ASIO
        socket.wait(socket.wait_read);
        
        size_t bytes = socket.available();
        std::cout << "Bytes available: " << bytes << "\n";

        if(bytes > 0) {
            std::vector<char> vBuffer(bytes);
            socket.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), ec);

            for(auto c : vBuffer) {
                std::cout << c;
            }
        }
    }

    return 0;
}