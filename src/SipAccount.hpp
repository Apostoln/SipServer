#pragma once
#ifndef SIPSERVER_SIPACCOUNT_HPP
#define SIPSERVER_SIPACCOUNT_HPP

#include <asio/ip/udp.hpp>

struct SipAccount {
    std::string name;
    asio::ip::udp::endpoint address;
    //std::string password;

    SipAccount() = default;
    SipAccount(std::string name, asio::ip::udp::endpoint endPoint):
        name(name), address(endPoint) {}
    SipAccount(std::string name, std::string endPointString):
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
};

#endif //SIPSERVER_SIPACCOUNT_HPP
