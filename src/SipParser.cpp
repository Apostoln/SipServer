#include <sstream>
#include <iostream>
#include "SipParser.hpp"

using namespace std::string_literals;

SipMessage SipParser::parse(const char* raw) {
    SipMessage result;
    std::istringstream iss(raw);
    std::string tmp;
    std::getline(iss, tmp);
    result.type = getType(tmp);
    result.startString = tmp;
    if (SipMessageType::Unknown == result.type) {
        throw std::logic_error("Sip message type parsing error"s); //Todo: Exception for prs err
    }

    while(std::getline(iss, tmp)) {
        //Todo: СДЕЛАТЬ НАРМАЛЬНА
        if("" == tmp) {
            while(std::getline(iss, tmp)) {
                std::istringstream line(tmp);
                std::string key;
                std::string value;
                std::getline(line, key, '=');
                std::getline(line, value);
                result.body.insert(std::make_pair(key,value));
            }
            break;
        }
        std::istringstream line(tmp);
        std::string key;
        std::string value;
        std::getline(line, key, ':');
        std::getline(line, value);

        value.erase(0, value.find_first_not_of(' '));       //prefixing spaces
        //str.erase(str.find_last_not_of(' ')+1);         //surfixing spaces

        result.headers.insert(std::make_pair(key,value));
    }
    return result;
}

SipMessageType SipParser::getType(std::string& str) {
    static std::regex request("[A-Z]{3,9} \\w+:\\w+@.+ SIP\\/2\\.0");
    static std::regex response("SIP\\/2\\.0 \\d{3} \\w+");
    if (std::regex_match(str, request)) {
        return SipMessageType::Request;
    }
    else if (std::regex_match(str, response)) {
        return SipMessageType::Response;
    }
    else {
        return SipMessageType::Unknown;
    }
}