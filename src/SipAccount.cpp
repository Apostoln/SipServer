#pragma once

#include <string>

#include <asio/ip/udp.hpp>

#include "SipAccount.hpp"

SipAccount::SipAccount(std::string name, asio::ip::udp::endpoint endPoint):
    name(name), address(endPoint) {}

SipAccount::SipAccount(std::string name, std::string endPointString):
    name(name)
{
    std::stringstream splitter(endPointString);
    std::string temp;
    std::getline(splitter, temp, ':');
    std::string ipString = temp;
    auto ip = asio::ip::address::from_string(ipString);
    std::getline(splitter, temp);
    std::string portString = temp;
    auto port = std::stoi(portString);
    this->address = asio::ip::udp::endpoint(ip, port);
}

SipAccount::operator std::string() const {
    std::string result = "";
    if(name.empty() && address != asio::ip::udp::endpoint()) {
        return result;
    }
    result += name;
    result += ",";
    result += address.address().to_string();
    result += ":";
    result += std::to_string(address.port());
    return result;
}

bool SipAccount::operator==(const SipAccount &other) {
    return name == other.name && address == other.address;
}
