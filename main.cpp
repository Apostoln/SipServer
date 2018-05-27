#include <argparse.hpp>

// Macro for enabling thread-sage mode of logger must be defined
// before compilation or inside easylogging++.h
// #define ELPP_THREAD_SAFE
// #define ELPP_NO_DEFAULT_LOG_FILE
#include <easylogging++.h>


#include <sqlite3.h>
#include <sqlite_orm.h>

#include <SipServer.hpp>
#include <Builder.hpp>

#include <AuthManager.hpp>
#include <ExitException.hpp>
#include <Registrar.hpp>
#include <utils/logHelper.hpp>
#include <utils/constants.hpp>

#include <Db.hpp>


INITIALIZE_EASYLOGGINGPP //crutch for logger

using uint = unsigned int;


int main(int argc, const char* argv[]) {
    ArgumentParser parser;
    parser.addArgument("-p", "--port", 1);
    parser.addArgument("-n", "--networkInterface", 1);
    parser.addArgument("-l", "--logLevel", 1);
    parser.addArgument("-c", "--cout", 1);
    parser.addArgument("-f", "--fileLogger", 1);
    parser.addArgument("-d", "--database", 1);
    parser.parse(argc, argv);

    auto portArg = parser.retrieve<std::string>("port");
    auto networkInterfaceArg = parser.retrieve<std::string>("networkInterface");
    auto logLevel = getLogLevel(parser.retrieve<std::string>("logLevel"));
    bool isConsoleOut = !parser.retrieve<std::string>("cout").empty();
    auto pathToDb = parser.retrieve<std::string>("database");
    auto loggingFile = parser.retrieve<std::string>("fileLogger");

    if (el::Level::Unknown == logLevel) {
        LOG(DEBUG) << "Log level is not specified. Using default log level: " << DEFAULT_LOG_LEVEL;
        logLevel = getLogLevel(DEFAULT_LOG_LEVEL);
    }
    if (loggingFile.empty()) {
        LOG(DEBUG) << "Log file is not specified. Default path to log file is used: " << DEFAULT_LOG_FILE_PATH;
        loggingFile = DEFAULT_LOG_FILE_PATH;
    }

    if (pathToDb.empty()) {
        LOG(ERROR) << "Path to db isn't specified";
        return 1;
    }

    configureLogger(isConsoleOut, loggingFile, logLevel);

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
        std::cout << pathToDb << std::endl;
        auto db = std::make_shared<Db>(pathToDb);
        Registrar* registrar = new Registrar(db);
        AuthManager* authManager = new AuthManager(db);
        SipServer server = sipServerBuilder.registrar(registrar)
                                           .authManager(authManager)
                                           .build();
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