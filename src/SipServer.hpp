#pragma once
#ifndef SIPSERVER_SIPSERVER_HPP
#define SIPSERVER_SIPSERVER_HPP

#include <vector>

#include <asio/io_service.hpp>
#include <asio/ip/udp.hpp>

class SipServer {
    private:
        asio::io_service* serverIo; // Object for connecting to I/O services of OS.
        asio::ip::udp::socket* serverSocket;
        unsigned short port;
        asio::ip::address networkInterface;
        std::vector<asio::ip::udp::endpoint> clients;

    public:
        class Builder;

        SipServer();
        SipServer(asio::io_service* ioService, asio::ip::address networkInterface, unsigned short port);
        SipServer(const SipServer& sipServer);
        ~SipServer();
        unsigned short getPort();
        void changePort(unsigned short port);
        void updateSocket(asio::ip::udp::endpoint endPoint);
        void run();
        void removeClient(asio::ip::udp::endpoint& client);

        void setPort(unsigned short port);
        void setServerIo(asio::io_service* serverIo);
        void setNetworkInterface(asio::ip::address networkInterface);
};

#endif //SIPSERVER_SIPSERVER_HPP
