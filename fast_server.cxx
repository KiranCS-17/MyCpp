#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <iostream>
#include <memory>
#include <thread>
#include <string>

using boost::asio::ip::tcp;

// The HTTP response to be sent to the clients
const std::string httpResponse =
    "HTTP/1.1 200 OK\r\n"
    "Content-Length: 13\r\n"
    "Content-Type: text/plain\r\n"
    "\r\n"
    "Hello, world!";

// Function to handle a single client connection
void handleClient(std::shared_ptr<tcp::socket> socket) {
    try {
        char buffer[1024];
        boost::system::error_code error;

        // Read the HTTP request (non-blocking, simplified for example purposes)
        size_t bytesRead = socket->read_some(boost::asio::buffer(buffer), error);
        
        if (!error) {
            // Write the HTTP response back to the client
            boost::asio::write(*socket, boost::asio::buffer(httpResponse), error);
        }
        
        // Close the socket when done
        socket->shutdown(tcp::socket::shutdown_both);
        socket->close();
    } catch (std::exception& e) {
        std::cerr << "Exception in handling client: " << e.what() << std::endl;
    }
}

// Main server function to accept connections and distribute them to a thread pool
void startServer(boost::asio::io_service& io_service, unsigned short port, boost::asio::thread_pool& pool) {
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

    while (true) {
        // Create a new socket for each incoming connection
        auto socket = std::make_shared<tcp::socket>(io_service);
        
        // Accept the connection
        acceptor.accept(*socket);

        // Submit the client handling task to the thread pool
        boost::asio::post(pool, [socket]() { handleClient(socket); });
    }
}

int main() {
    try {
        // Boost Asio I/O service to manage async operations
        boost::asio::io_service io_service;
        
        // Define a thread pool with a fixed number of threads (e.g., 4 threads)
        boost::asio::thread_pool pool(4);

        // Start the server on port 1234
        std::thread serverThread([&io_service, &pool]() {
            startServer(io_service, 1234, pool);
        });

        // Join the server thread to main
        serverThread.join();
        
        // Run the thread pool (optional to manage additional tasks)
        pool.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

