#include <iostream>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include "file_transfer.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReader;
using file_transfer::FileRequest;
using file_transfer::FileChunk;
using file_transfer::FileService;

class FileClient {
public:
    FileClient(std::shared_ptr<Channel> channel)
        : stub_(FileService::NewStub(channel)) {}

    void GetFile(const std::string& filename, const std::string& output_file) {
        FileRequest request;
        request.set_filename(filename);

        FileChunk chunk;
        ClientContext context;
        context.set_compression_algorithm(GRPC_COMPRESS_GZIP);
        std::unique_ptr<ClientReader<FileChunk>> reader(
            stub_->GetFile(&context, request));

        std::ofstream output(output_file, std::ios::binary);
        if (!output.is_open()) {
            std::cerr << "Error opening output file: " << output_file << std::endl;
            return;
        }

        size_t bytes_received = 0;
        while (reader->Read(&chunk)) {
            output.write(chunk.chunk_data().data(), chunk.chunk_data().size());
            bytes_received += chunk.chunk_data().size();
            std::cout << "Received chunk of size: " << chunk.chunk_data().size() << " bytes\n";
        }

        output.close();
        
        Status status = reader->Finish();
        if (status.ok()) {
            std::cout << "Successfully received file. Total bytes: " << bytes_received << std::endl;
        } else {
            std::cout << "File transfer failed: " << status.error_message() << std::endl;
        }
    }

private:
    std::unique_ptr<FileService::Stub> stub_;
};

int main() {
    FileClient client(grpc::CreateChannel("10.43.239.242:30008", grpc::InsecureChannelCredentials()));
    
    // Request the file from the server
    std::string filename = "large_data.json";    // The file on the server
    std::string output_file = "received.json"; // Local file where data will be written
    client.GetFile(filename, output_file);

    return 0;
}

