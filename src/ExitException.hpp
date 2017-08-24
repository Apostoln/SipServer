#pragma once
#ifndef SIPSERVER_EXITEXCEPTION_HPP
#define SIPSERVER_EXITEXCEPTION_HPP

#include <exception>
#include <string>
#include <unordered_map>

#include "ErrorCode.hpp"

class ExitException: std::exception {
    private:
        ErrorCode errorCode = ErrorCode::UNKNOWN_ASIO_ERROR;
        std::string additionalDescription = "";
        std::unordered_map<ErrorCode, std::string> messages;

    public:
        ExitException(ErrorCode errorCode, std::string additionalDescription = "");
        virtual ~ExitException() = default;
        const char* what();
        ErrorCode getErrorCode();
};


#endif //SIPSERVER_EXITEXCEPTION_HPP
