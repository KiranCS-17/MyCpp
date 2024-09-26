#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "broadcast.grpc.pb.h"
#include <thread>
#include <unistd.h>
#include "Logger.h"
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using broadcast::BroadcastMessage;
using broadcast::BroadcastService;

Logger fileLogger(INFO, "app_client.log", 1024 * 1024);
std::string log_message;
std::string pid_str;
class BroadcastClient {
public:
  BroadcastClient(std::shared_ptr<Channel> channel) : stub_(BroadcastService::NewStub(channel)) {}
  void Broadcast() {
    ClientContext context;
    std::shared_ptr<ClientReaderWriter<BroadcastMessage, BroadcastMessage>> stream(stub_->Broadcast(&context));
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() + std::chrono::seconds(1000);
    context.set_compression_algorithm(GRPC_COMPRESS_GZIP);
    context.set_deadline(deadline);

    std::thread writer_thread([stream]() {
      std::vector<std::string> messages = {"Message "};
      pid_t pid = getpid();
       pid_str = std::to_string(pid);
       log_message = "Sending req. to server from " + pid_str;
      LOG(fileLogger, INFO, log_message);
      std::cout << log_message << std::endl;
      for (const auto& message : messages) {
        BroadcastMessage request;
        request.set_content(message+pid_str);
        stream->Write(request);
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }

      stream->WritesDone();
    });

    BroadcastMessage response;
    while (stream->Read(&response)) {
      log_message = "Received Broadcasted content by client " + pid_str + "with size " + std::to_string(response.content().size()); 
      LOG(fileLogger, INFO, log_message);
      std::cout << "Received broadcasted message: " << "by " << pid_str <<"Size" << response.content().size() << std::endl;
    }

    writer_thread.join();
    grpc::Status status = stream->Finish();
    if (!status.ok()) {
      std::cerr << "Error: " << status.error_code() << ": " << status.error_message() << std::endl;
    }
  }

private:
  std::unique_ptr<BroadcastService::Stub> stub_;
};

int main() {
  grpc::ChannelArguments args;
    args.SetMaxReceiveMessageSize(900 * 1024 * 1024);  // Set 100MB limit for receiving
    args.SetMaxSendMessageSize(900 * 1024 * 1024);     // Set 100MB limit for sending
    auto channel = grpc::CreateCustomChannel("10.43.239.242:30008", grpc::InsecureChannelCredentials(), args);
    BroadcastClient client(channel);
    client.Broadcast();
  return 0;
}

