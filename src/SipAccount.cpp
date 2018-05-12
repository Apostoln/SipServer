#include "SipAccount.hpp"

SipAccount::operator std::string() const {
    std::string result = "";
    result += name;
    result += ",";
    result += pass;
    return result;
}

bool SipAccount::operator==(const std::string &other) {
    return name == other;
}