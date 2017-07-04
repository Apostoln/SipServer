#include <SipServer.hpp>
#include <Builder.hpp>

int main(int argc, char* argv[]) {
    //TODO: Args parsing
    //TODO: Using builders
    SipServer::Builder sipServerBuilder;
    if (argc != 1) {
        // Create with specified port from main function arguments.
        auto port = (unsigned short) std::atoi(argv[1]);
        sipServerBuilder.setPort(port);
    }
    SipServer server = sipServerBuilder.build();
    server.run();
    return 0;
}