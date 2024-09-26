#include <iostream>
#include<fstream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "broadcast.grpc.pb.h"
#include <thread>
#include "Logger.h"
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using broadcast::BroadcastMessage;
using broadcast::BroadcastService;

std::string file_content;
Logger fileLogger(INFO, "app_server.log", 1024 * 1024);
void readFileContents() {
    // Path to the large JSON file
    std::string file_path = "large_data.json";

    // Open the file in binary mode and move the file pointer to the end
    std::ifstream file(file_path, std::ios::in | std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return;
    }

    // Get the size of the file
    std::streampos file_size = file.tellg();

    // Move the file pointer to the beginning
    file.seekg(0, std::ios::beg);

    // Create a string to hold the file contents
    
    file_content.resize(file_size);

    // Read the entire file into the string
    file.read(&file_content[0], file_size);

    // Close the file
    file.close();

    /* Output the size of the file and the first 500 characters (just for verification)
    std::cout << "File Size: " << file_content.size() << " bytes" << std::endl;
    std::cout << "First 500 characters of the file:\n" << file_content.substr(0, 500) << std::endl;*/

}


class BroadcastServiceImpl final : public BroadcastService::Service {
public:
  Status Broadcast(ServerContext* context, grpc::ServerReaderWriter<BroadcastMessage, BroadcastMessage>* stream) override {
    BroadcastMessage request;
    while (stream->Read(&request)) {
     
     
      std::cout << "Received message: " << request.content() << std::endl;
      std::string recvd_data = "Received data from pid " + request.content();  
      LOG(fileLogger, INFO, recvd_data);
      // Broadcast the received message to all connected clients
      BroadcastMessage response;
      LOG(fileLogger, INFO, "Broadcasted large_data.json");
      response.set_content("Broadcasting: " + file_content);
      stream->Write(response);
    }
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:1234");
  BroadcastServiceImpl service;

  ServerBuilder builder;
      // Increase the maximum message size (e.g., 100MB)
  builder.SetMaxReceiveMessageSize(900 * 1024 * 1024);  // Set 900MB limit for receiving
  builder.SetMaxSendMessageSize(900 * 1024 * 1024);     // Set 900MB limit for sending
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main() {
  readFileContents(); 
  RunServer();
  return 0;
}

