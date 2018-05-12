#pragma once
#ifndef SIPSERVER_SIPUSER_HPP
#define SIPSERVER_SIPUSER_HPP

#include <asio/ip/udp.hpp>

struct SipUser {
    std::string name;
    asio::ip::udp::endpoint address;

    SipUser() = default;
    SipUser(std::string name, asio::ip::udp::endpoint endPoint);
    SipUser(std::string name, std::string endPointString);

    operator std::string() const;
    bool operator ==(const SipUser& other);
};

#endif //SIPSERVER_SIPUSER_HPP
