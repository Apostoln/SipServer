#pragma once

#include <string>

#include <asio/ip/udp.hpp>

#include "SipUser.hpp"

SipUser::SipUser(std::string name, asio::ip::udp::endpoint endPoint):
    name(name), address(endPoint) {}

SipUser::SipUser(std::string name, std::string endPointString):
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

SipUser::operator std::string() const {
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

bool SipUser::operator==(const SipUser &other) {
    return name == other.name && address == other.address;
}
