//#define ELPP_NO_DEFAULT_LOG_FILE
//#define ELPP_DEFAULT_LOG_FILE "log1/sipserver.log"

#include <argparse.hpp>
#include <easylogging++.h>

#include <SipServer.hpp>
#include <Builder.hpp>
#include <ExitException.hpp>
#include <utils.hpp>

INITIALIZE_EASYLOGGINGPP //crutch for logger

using uint = unsigned int;

el::Level defaultLogLevel = el::Level::Info;
const std::string defaultLogFilePath = "log/sipserver.log";

int main(int argc, const char* argv[]) {
    ArgumentParser parser;
    parser.addArgument("-p", "--port", 1);
    parser.addArgument("-n", "--networkInterface", 1);
    parser.addArgument("-l", "--logLevel", 1);
    parser.addArgument("-c", "--cout", 1);
    parser.addArgument("-f", "--fileLogger", 1);
    parser.parse(argc, argv);

    auto portArg = parser.retrieve<std::string>("port");
    auto networkInterfaceArg = parser.retrieve<std::string>("networkInterface");
    auto logLevel = getLogLevel(parser.retrieve<std::string>("logLevel"));
    logLevel = logLevel == el::Level::Unknown? defaultLogLevel : logLevel;
    bool isConsoleOut = !parser.retrieve<std::string>("cout").empty() ;

    auto loggingFile = parser.retrieve<std::string>("fileLogger");
    loggingFile = loggingFile != ""? loggingFile: defaultLogFilePath;
    LOG_IF(loggingFile == defaultLogFilePath, DEBUG) << "Default path to log file is used";

    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%level %datetime{%H:%m:%s} [%fbase]: %msg");
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::ToStandardOutput, isConsoleOut? "true": "false");
    if ("" != loggingFile) {
        el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Filename, loggingFile);
    }
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::addFlag(el::LoggingFlag::HierarchicalLogging);
    el::Loggers::setLoggingLevel(logLevel);

    LOG(INFO) << "Logger of SipServer is started";
    LOG(INFO) << "Log is wrote to " << loggingFile;
    LOG(INFO) << "Log level is " << (uint)logLevel;
    LOG(INFO) << "Echo to stdout is " << (isConsoleOut? "" : "not ") << "specified";

    SipServer::Builder sipServerBuilder;
    if (!portArg.empty()) {
        auto port = (uint) std::stoi(portArg);
        sipServerBuilder.port(port);
    }
    if (!networkInterfaceArg.empty()) {
        auto networkInterface = networkInterfaceArg.c_str();
        sipServerBuilder.networkInterface(networkInterface);
    }
    try {
        SipServer server = sipServerBuilder.build();
        server.run();
    }
    catch (ExitException& e) {
        auto errorCodeNum = static_cast<std::underlying_type<ErrorCode >::type>(e.getErrorCode());
        LOG(ERROR) << "Exit with error code " << errorCodeNum;
        return errorCodeNum;
    }
    return 0;
}