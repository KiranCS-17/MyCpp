#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include "gen-cpp/PersonService.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

class PersonServiceHandler : public example::PersonServiceIf {
public:
    PersonServiceHandler() {}

    void ping() override {
        printf("Ping received\n");
    }

    void getPerson(example::Person& _return, const int32_t id) override {
        // Set the details of the person
        _return.id = id;
        _return.name = "John Doe";
        _return.age = 30;
        printf("getPerson(%d) called\n", id);
    }
};

int main() {
    int port = 9090;
    auto handler = std::make_shared<PersonServiceHandler>();
    auto processor = std::make_shared<example::PersonServiceProcessor>(handler);
    auto serverTransport = std::make_shared<TServerSocket>(port);
    auto transportFactory = std::make_shared<TBufferedTransportFactory>();
    auto protocolFactory = std::make_shared<TBinaryProtocolFactory>();

    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    printf("Starting the server on port %d...\n", port);
    server.serve();
    return 0;
}

