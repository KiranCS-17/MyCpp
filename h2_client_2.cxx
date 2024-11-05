#include <iostream>
#include <nghttp2/asio_http2_client.h>

using boost::asio::ip::tcp;

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::client;

int main(int argc, char *argv[]) {
    boost::system::error_code ec;
    boost::asio::io_service io_service;

    // Connect to localhost:3000
    session sess(io_service, "localhost", "3000");

    sess.on_connect([&sess](tcp::resolver::iterator endpoint_it) {
        boost::system::error_code ec;

        // Prepare custom headers

        header_map headers;
        headers.emplace("Custom-Header-1", "Value1");
        headers.emplace("Custom-Header-2", "Value2");
        headers.emplace("Content-Type", "application/json"); // For JSON data
        
        // For POST request, include the body data
        std::string  body = R"({"key": "value"})"; // Example JSON body
        
        // Submit a POST request with custom headers and body
           auto req = sess.submit(ec, "POST", "http://localhost:3000/",body, headers); 
             

        if (ec) {
            std::cerr << "Error submitting request: " << ec.message() << std::endl;
            return;
        }

        req->on_response([](const response &res) {
            // Print status code and response header fields
            std::cerr << "HTTP/2 " << res.status_code() << std::endl;
            for (auto &kv : res.header()) {
                std::cerr << kv.first << ": " << kv.second.value << "\n";
            }
            std::cerr << std::endl;

            res.on_data([](const uint8_t *data, std::size_t len) {
                std::cerr.write(reinterpret_cast<const char *>(data), len);
                std::cerr << std::endl;
            });
        });

        req->on_close([&sess](uint32_t error_code) {
            // Shutdown session after the first request is done
            sess.shutdown();
        });
    });

    sess.on_error([](const boost::system::error_code &ec) {
        std::cerr << "error: " << ec.message() << std::endl;
    });

    io_service.run();
}
