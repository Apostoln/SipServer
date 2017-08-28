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

    MethodType method = parseMethod(this->startString);
    /*if (MethodType::NONE == method) {
        std::string description = "Method in request is not parsed. Starting string " + this->startString;
        throw ExitException(ErrorCode::PARSING_ERROR, description);
    }*/
    this->method = method;
    auto parsedContact = parseContact(headers.find("Contact")->second);
    //senderId = parsedContact.first;
    //senderEndPoint = parsedContact.second;
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

MethodType SipMessage::getMethod() {
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

MethodType SipMessage::parseMethod(std::string& str) {
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

std::pair<std::string, asio::ip::udp::endpoint> SipMessage::parseContact(std::string& str) {
    static std::regex parseContactRegext("(\\w+:)?(\\w+)@((\\d{1,3}\\.){3}\\d{1,3}):(\\d{4,6}).*");
    std::string senderId;
    asio::ip::udp::endpoint senderEndPoint;
    std::smatch matched;
    if (std::regex_match(str, matched, parseContactRegext)) {
        senderId = matched[2];
        auto senderIpAddress = matched[3];
        auto senderPort = matched[5];
        senderEndPoint = asio::ip::udp::endpoint(asio::ip::address::from_string(senderIpAddress), std::stoi(senderPort));
        LOG(DEBUG) << "Sender endPoint: " << senderIpAddress << ":" << senderPort;
    }
    else {
        //throw ExitException(ErrorCode::PARSING_ERROR);
    }
    return std::make_pair(senderId, senderEndPoint);
}