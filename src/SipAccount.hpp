#pragma once
#ifndef SIPSERVER_SIPACCOUNT_HPP
#define SIPSERVER_SIPACCOUNT_HPP

#include <asio/ip/udp.hpp>

struct SipAccount {
    std::string name;
    asio::ip::udp::endpoint address;
    //std::string password;

    SipAccount() = default;
    SipAccount(std::string name, asio::ip::udp::endpoint endPoint);
    SipAccount(std::string name, std::string endPointString);

    operator std::string() const;
    bool operator ==(const SipAccount& other);
};

#endif //SIPSERVER_SIPACCOUNT_HPP
