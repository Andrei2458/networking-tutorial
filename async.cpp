#include <iostream>
// for a delay
#include <chrono>
#include <thread>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

// create a reasonbile larg buffer - as we do not now how much data we'll get at some point
std::vector<char> vBuffer(20 * 1024);

// a function that will handle the reading
// for more info about the lambda signature, search for info about the async_read_some()
void GrabSomeData(asio::ip::tcp::socket& socket) {
    socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
        [&](std::error_code ec, std::size_t length)
        {
            if(!ec){
                std::cout <<"\n----------------------------------\n";
                std::cout <<"\n\n\nRead " << length << "bytes\n\n\n";
                std::cout <<"\n----------------------------------\n";
                for (auto elem : vBuffer) {
                    std::cout << elem;
                }

                GrabSomeData(socket);
            }
        }    
    );
}

int main() {

    asio::error_code ec;

    // Create a "context" -- the platform specific interface
    asio::io_context context;

    // Give some fake tasks to asio so the context doesn't finish
    asio::io_context::work idleWork(context);

    // Start the context in its own thread
    std::thread thrContext = std::thread([&]() { context.run(); });

    // Get the address of something we want to connect to
    // 51.38.81.49 and  93.184.216.34
    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec), 80);

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
        //start reading before it asks for data
        GrabSomeData(socket);  
        
        std::string sReqiuest = 
            "GET /index.html HTTP/1.1\r\n"
            "Host: david-barr.co.uk\r\n"
            "Connection: close\r\n\r\n";
        
        socket.write_some(asio::buffer(sReqiuest.data(), sReqiuest.size()), ec);

         // set a big delay so the program does not stop before receving data
         using namespace std::chrono_literals;
         std::this_thread::sleep_for(20000ms);

        context.stop();
        if (thrContext.joinable()) {
            thrContext.join();
        }
    }



    return 0;
}