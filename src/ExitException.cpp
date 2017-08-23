#include "ExitException.hpp"

ExitException::ExitException(ErrorCode errorCode):
            std::exception(), errorCode(errorCode)
{}

const char* ExitException::what() {
    return whatMessage.c_str();
}

ErrorCode ExitException::getErrorCode() {
    return errorCode;
}

