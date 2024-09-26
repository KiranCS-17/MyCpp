#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <vector>
#include <chrono>

using boost::asio::ip::tcp;

// Function to send a request to the server
void send_request(boost::asio::io_context& io_context, const std::string& server, const std::string& port, int request_id) {
    try {
        // Create a TCP resolver and socket
        tcp::resolver resolver(io_context);
        tcp::socket socket(io_context);

        // Resolve the server and port
        boost::asio::connect(socket, resolver.resolve(server, port));

        // Build the request (simple HTTP GET in this case)
        std::string request = "GET / HTTP/1.1\r\nHost: " + server + "\r\n\r\n";

        // Send the request
        boost::asio::write(socket, boost::asio::buffer(request));

        // Receive the response (optional)
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        std::cout << "Request " << request_id << " sent and response received.\n";

    } catch (std::exception& e) {
        std::cerr << "Error on request " << request_id << ": " << e.what() << "\n";
    }
}

// Function to manage sending requests in parallel
void send_requests_concurrently(int num_requests, const std::string& server, const std::string& port) {
    // Create a Boost.Asio context
    boost::asio::io_context io_context;

    // Vector to hold threads
    std::vector<std::thread> threads;

    // Start timer
    auto start_time = std::chrono::high_resolution_clock::now();

    // Spawn threads for each request
    for (int i = 0; i < num_requests; ++i) {
        threads.emplace_back([&io_context, server, port, i]() {
            send_request(io_context, server, port, i);
        });
    }

    // Join all threads (wait for them to finish)
    for (auto& t : threads) {
        t.join();
    }

    // End timer and calculate elapsed time
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "All requests sent in: " << elapsed.count() << " seconds\n";
}

int main() {
    const int num_requests = 1000;
    const std::string server = "127.0.0.1";  // Replace with your server IP
    const std::string port = "80";           // Replace with your server port

    send_requests_concurrently(num_requests, server, port);

    return 0;
}

