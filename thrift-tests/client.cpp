#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include "gen-cpp/PersonService.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

int main() {
    // Create and open transport to the server
    auto socket = std::make_shared<TSocket>("localhost", 9090);
    auto transport = std::make_shared<TBufferedTransport>(socket);
    auto protocol = std::make_shared<TBinaryProtocol>(transport);
    example::PersonServiceClient client(protocol);

    try {
        // Open the transport
        transport->open();

        // Call ping
        client.ping();
        printf("Ping sent successfully\n");

        // Call getPerson
        example::Person person;
        client.getPerson(person, 1); // Pass a reference for the return value
        printf("Received person: ID = %d, Name = %s, Age = %d\n",
               person.id, person.name.c_str(), person.age);

        // Close the transport
        transport->close();
    } catch (TException& tx) {
        printf("ERROR: %s\n", tx.what());
    }

    return 0;
}
