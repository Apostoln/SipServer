#include <argparse.hpp>
#include <easylogging++.h>

#include <SipServer.hpp>
#include <Builder.hpp>

#include <ExitException.hpp>
#include <Registrar.hpp>
#include <utils.hpp>
#include <constants.hpp>

INITIALIZE_EASYLOGGINGPP //crutch for logger

using uint = unsigned int;


int main(int argc, const char* argv[]) {
    ArgumentParser parser;
    parser.addArgument("-p", "--port", 1);
    parser.addArgument("-n", "--networkInterface", 1);
    parser.addArgument("-l", "--logLevel", 1);
    parser.addArgument("-c", "--cout", 1);
    parser.addArgument("-f", "--fileLogger", 1);
    parser.addArgument("-a", "--accounts", 1);
    parser.parse(argc, argv);

    auto portArg = parser.retrieve<std::string>("port");
    auto networkInterfaceArg = parser.retrieve<std::string>("networkInterface");
    auto logLevel = getLogLevel(parser.retrieve<std::string>("logLevel"));
    bool isConsoleOut = !parser.retrieve<std::string>("cout").empty();
    auto pathToAccounts = parser.retrieve<std::string>("accounts");
    auto loggingFile = parser.retrieve<std::string>("fileLogger");

    if (el::Level::Unknown == logLevel) {
        LOG(DEBUG) << "Log level is not specified. Using default log level: " << DEFAULT_LOG_LEVEL;
        logLevel = getLogLevel(DEFAULT_LOG_LEVEL);
    }

    if (loggingFile.empty()) {
        LOG(DEBUG) << "Log file is not specified. Default path to log file is used: " << DEFAULT_LOG_FILE_PATH;
        loggingFile = DEFAULT_LOG_FILE_PATH;
    }

    if (pathToAccounts.empty()) {
        LOG(DEBUG) << "Path to file with accounts is not specified. "
                   << "Trying to use default path " << DEFAULT_PATH_TO_ACCOUNTS;
        pathToAccounts = DEFAULT_PATH_TO_ACCOUNTS;
    }

    configureLogger(isConsoleOut, loggingFile, logLevel);

    Registrar* registrar = new Registrar(pathToAccounts);

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
        if (ErrorCode::SUCCESSFULLY == e.getErrorCode()) {
            LOG(DEBUG) << "SipServer closed successfully";
            return 0;
        }
        auto errorCodeNum = static_cast<std::underlying_type<ErrorCode >::type>(e.getErrorCode());
        auto errorMessage = e.what();
        std::cerr << errorMessage << std::endl;
        LOG(ERROR) << errorMessage;
        LOG(ERROR) << "Exit with error code " << errorCodeNum;
        return errorCodeNum;
    }

    return 0;
}