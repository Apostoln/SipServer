#include <sstream>
#include <iostream>

#include <easylogging++.h>

#include "SipParser.hpp"
#include "ErrorCode.hpp"
#include "ExitException.hpp"


SipMessage SipParser::parse(const char* raw) {
    SipMessage result;
    std::istringstream iss(raw);
    std::string tmp;
    std::getline(iss, tmp);
    result.type = getType(tmp);
    result.startString = tmp;
    if (SipMessageType::Unknown == result.type) {
        throw ExitException(ErrorCode::PARSING_ERROR);
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

        result.headers.insert(std::make_pair(key,value));
    }

    MethodType method = SipParser::getMethod(result.startString);
    /*if (MethodType::NONE == method) {
        std::string description = "Method in request is not parsed. Starting string " + result.startString;
        throw ExitException(ErrorCode::PARSING_ERROR, description);
    }*/
    result.method = method;
    return result;
}

SipMessageType SipParser::getType(std::string& str) {
    static std::regex request("[A-Z]{3,9} .+ SIP\\/2\\.0");
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

MethodType SipParser::getMethod(std::string& str) {
    static std::regex methodRegex("([A-Z]{3,9}) .+");
    std::smatch matched;
    if (std::regex_match(str, matched, methodRegex)) {
        auto matchedString = matched[1];
        if ("REGISTER" == matchedString) {
            return MethodType::REGISTER;
        } //TODO: Other method
        else {
            return MethodType::NONE;
        }
    }
    return MethodType::NONE;
}
