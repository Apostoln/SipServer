#include <argparse.hpp>
#include <easylogging++.h>

#include <SipServer.hpp>
#include <Builder.hpp>
#include <utils.hpp>

INITIALIZE_EASYLOGGINGPP //crutch for logger

using uint = unsigned int;

int main(int argc, const char* argv[]) {
    ArgumentParser parser;
    parser.addArgument("-p", "--port", 1);
    parser.addArgument("-n", "--networkInterface", 1);
    parser.addArgument("-l", "--logLevel", 1);
    parser.addArgument("-c", "--cout", 1);
    parser.parse(argc, argv);

    auto portArg = parser.retrieve<std::string>("port");
    auto networkInterfaceArg = parser.retrieve<std::string>("networkInterface");
    auto logLevel = getLogLevel(parser.retrieve<std::string>("logLevel"));
    bool isConsoleOut = !parser.retrieve<std::string>("cout").empty() ;

    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%level %datetime{%H:%m:%s} [%fbase]: %msg");
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::ToStandardOutput, isConsoleOut? "true": "false");
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::addFlag(el::LoggingFlag::HierarchicalLogging);
    el::Loggers::setLoggingLevel(logLevel);

    LOG(INFO) << "Logger of SipServer is started";
    LOG(INFO) << "Log level is " << (uint)logLevel;
    LOG(INFO) << "Echo to stdout is " << (isConsoleOut? "" : "not") << "specified";

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