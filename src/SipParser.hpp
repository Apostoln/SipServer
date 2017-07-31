#pragma once
#ifndef SIPSERVER_SIPPARSER_HPP
#define SIPSERVER_SIPPARSER_HPP

#include <regex>

#include <SipMessage.hpp>

class SipParser {
    private:
        static SipMessageType getType(std::string&);
    public:
        static SipMessage parse(const char* raw);
};


#endif //SIPSERVER_SIPPARSER_HPP
