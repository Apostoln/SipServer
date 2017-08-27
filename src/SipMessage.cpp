#include "SipMessage.hpp"


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