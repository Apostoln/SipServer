#include <argparse.hpp>
#include <easylogging++.h>

#include <SipServer.hpp>
#include <Builder.hpp>

INITIALIZE_EASYLOGGINGPP //crutch for logger

using uint = unsigned int;

int main(int argc, const char* argv[]) {
    LOG(INFO) << "Sip server is started";
    ArgumentParser parser;
    parser.addArgument("-p", "--port", 1);
    parser.addArgument("-n", "--networkInterface", 1);
    parser.parse(argc, argv);

    auto portArg = parser.retrieve<std::string>("port");
    auto networkInterfaceArg = parser.retrieve<std::string>("networkInterface");

    SipServer::Builder sipServerBuilder;
    if (!portArg.empty()) {
        auto port = (uint) std::stoi(portArg);
        sipServerBuilder.port(port);
    }
    if (!networkInterfaceArg.empty()) {
        auto networkInterface = networkInterfaceArg.c_str();
        sipServerBuilder.networkInterface(networkInterface);
    }

    SipServer server = sipServerBuilder.build();
    server.run();
    return 0;
}