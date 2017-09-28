#include <sstream>
#include <iostream>
#include <regex>

#include <easylogging++.h>
#include <asio/ip/udp.hpp>

#include "SipMessage.hpp"
#include "ErrorCode.hpp"
#include "ExitException.hpp"

SipMessage::SipMessage(const char * rawStringMessage) {
    std::istringstream iss(rawStringMessage);
    std::string tmp;
    std::getline(iss, tmp);
    this->type = parseType(tmp);
    this->startString = tmp;
    if (SipMessageType::Unknown == this->type) {
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
                this->body.insert(std::make_pair(key,value));
            }
            break;
        }
        std::istringstream line(tmp);
        std::string key;
        std::string value;
        std::getline(line, key, ':');
        std::getline(line, value);

        value.erase(0, value.find_first_not_of(' '));       //prefixing spaces

        this->headers.insert(std::make_pair(key,value));
    }

    if (SipMessageType::Request == type) {
        this->method = parseMethod(this->startString);
    }
    if(headers.find("Contact") != headers.end()) {
        parseContact(headers.find("Contact")->second);
    }
}

SipMessage::operator std::string() const {
    std::string result;
    result += startString;
    result += "\n";
    for(auto elem: headers) {
        result += elem.first;
        result += ": ";
        result += elem.second;
        result += "\n";
    }
    result += "\n";
    for(auto elem: body) {
        result += elem.first;
        result += "=";
        result += elem.second;
        result += "\n";
    }
    return result;
}

SipMessageType SipMessage::getSipMessageType() {
    return type;
}

std::string SipMessage::getMethod() {
    return method;
}

SipMessageType SipMessage::parseType(std::string& str) {
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

std::string SipMessage::parseMethod(std::string& str) {
    static std::regex methodRegex("([A-Z]{3,9}) .+");
    std::smatch matched;
    if (std::regex_match(str, matched, methodRegex)) {
        auto matchedString = matched[1];
        return matchedString;
    }
    else {
        std::string description = "Method is not found";
        throw ExitException(ErrorCode::PARSING_ERROR, description);
    }
}

void SipMessage::parseContact(std::string& str) {
    static std::regex parseContactRegext("<?(\\w+:)?(\\w+)@((\\d{1,3}\\.){3}\\d{1,3}):?(\\d{4,6})?>?.*");
    std::smatch matched;
    if (std::regex_match(str, matched, parseContactRegext)) {
        LOG(DEBUG) << "\"Contact\" header is valid";
    }
    else {
        std::string description = "\"Contact\" header is not valid";
        throw ExitException(ErrorCode::PARSING_ERROR );
    }
}