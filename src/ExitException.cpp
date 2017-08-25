#include <iostream>
#include <string>
#include <cstring>
#include "ExitException.hpp"

ExitException::ExitException(ErrorCode errorCode, std::string additionalDescription):
            std::exception(), errorCode(errorCode), additionalDescription(additionalDescription)
{}

const char* ExitException::what() {
    messages[ErrorCode::SUCCESSFULLY] = "OK";
    messages[ErrorCode::PORT_UNAVAILABLE] = "Port is unavailable";
    messages[ErrorCode::PORT_SYSTEM] = "Port is unavailable. Port must be > 1024 on Unix";
    messages[ErrorCode::NETWORK_INTERFACE_ERROR] = "Network interface is not supported";
    messages[ErrorCode::UNKNOWN_ASIO_ERROR] = "Unknown asio error";
    messages[ErrorCode::PARSING_ERROR] = "Sip message type parsing error";
    messages[ErrorCode::ACCOUNTS_FILE_UNREACHABLE] = "Can't open file with accounts. File";
    
    std::string errorMessage = messages[errorCode];
    if ("" != additionalDescription) {
        errorMessage +=  ". " + additionalDescription;
    }

    char* rowStringErrorMessage = new char[errorMessage.length()+1];
    std::strcpy(rowStringErrorMessage, errorMessage.c_str());
    return rowStringErrorMessage;
}

ErrorCode ExitException::getErrorCode() {
    return errorCode;
}

