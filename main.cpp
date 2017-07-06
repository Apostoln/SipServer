#include <SipServer.hpp>
#include <Builder.hpp>

int main(int argc, char* argv[]) {
    //TODO: Args parsing

    SipServer::Builder sipServerBuilder;
    if (argc >= 2) {
        // Create with specified port from main function arguments.
        auto port = (unsigned short) std::atoi(argv[1]);
        sipServerBuilder.port(port);
    }
    if (argc >= 3) {
        auto networkInterface = argv[2];
        sipServerBuilder.networkInterface(networkInterface);
    }

    SipServer server = sipServerBuilder.build();
    server.run();
    return 0;
}