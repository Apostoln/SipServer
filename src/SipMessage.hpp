#pragma once
#ifndef SIPSERVER_SIPMESSAGE_HPP
#define SIPSERVER_SIPMESSAGE_HPP

#include <map>

enum class SipMessageType {
    Request = 0,
    Response = 1,
    Unknown = 2
};

class SipMessage {
    friend class SipParser;

    public:
        std::string startString;
        std::multimap<std::string, std::string> headers;
        std::multimap<std::string, std::string> body;
        SipMessageType type;
    public:
        SipMessage() = default;
        operator std::string() const;
};


#endif //SIPSERVER_SIPMESSAGE_HPP
