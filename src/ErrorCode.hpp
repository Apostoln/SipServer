#pragma once
#ifndef SIPSERVER_ERRORCODE_HPP
#define SIPSERVER_ERRORCODE_HPP

#include <type_traits>

enum class ErrorCode {
    SUCCESSFULLY = 0,
    PORT_UNAVAILABLE = 1,
    PORT_SYSTEM = 2,
    NETWORK_INTERFACE_ERROR = 3,
    UNKNOWN_ASIO_ERROR = 4
};

namespace std {
    template <>
    struct hash<ErrorCode> {
        size_t operator()(ErrorCode errorCode) const {
            return hash<int>()(static_cast<std::underlying_type<ErrorCode >::type>(errorCode));
        }
    };
}



#endif //SIPSERVER_ERRORCODE_HPP
