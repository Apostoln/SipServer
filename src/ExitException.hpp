#pragma once
#ifndef SIPSERVER_EXITEXCEPTION_HPP
#define SIPSERVER_EXITEXCEPTION_HPP

#include <exception>
#include <string>

#include <ErrorCode.hpp>

class ExitException: std::exception {
    private:
        ErrorCode errorCode = ErrorCode::UNKNOWN;
        std::string whatMessage = "";
    public:
        ExitException(ErrorCode errorCode);
        virtual ~ExitException() = default;
        const char* what();
        ErrorCode getErrorCode();
};


#endif //SIPSERVER_EXITEXCEPTION_HPP
