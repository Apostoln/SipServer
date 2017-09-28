#pragma once
#ifndef SIPSERVER_SIPMESSAGE_HPP
#define SIPSERVER_SIPMESSAGE_HPP

#include <map>

#include <asio/ip/udp.hpp>


enum class SipMessageType {
    Request = 0,
    Response = 1,
    Unknown = 2
};

class SipMessage {
    friend class SipServer;

    private:
        std::string startString;
        std::multimap<std::string, std::string> headers;
        std::multimap<std::string, std::string> body;
        SipMessageType type;
        std::string method;

        std::string senderId;
        asio::ip::udp::endpoint senderEndPoint;

        static SipMessageType parseType(std::string&);
        static std::string parseMethod(std::string&);
        void parseContact(std::string& str);

    public:
        SipMessage() = default;
        SipMessage(const char* rowStringMessage);
        SipMessageType getSipMessageType();
        std::string getMethod();
        operator std::string() const;
};


#endif //SIPSERVER_SIPMESSAGE_HPP
