#pragma once
#ifndef SIPSERVER_SIPSERVER_HPP
#define SIPSERVER_SIPSERVER_HPP

#include <asio.hpp>

class SipServer {
    public:
        asio::io_service* serverIo;
        asio::ip::udp::socket* serverSocket;
        unsigned short port;
    public:
        SipServer(asio::io_service* ioService, unsigned short port) {
            this->serverIo = ioService;
            this->port = port;
            this->serverSocket = new asio::ip::udp::socket(*serverIo, asio::ip::udp::endpoint(asio::ip::udp::v4(), port));
            if(port == 0) {
                this->port = serverSocket->local_endpoint().port();
            }
        }

        SipServer(unsigned short port):
                SipServer(new asio::io_service, port)
        {}

        SipServer():
                SipServer(0)
        {}

        ~SipServer() {
            delete(serverSocket);
            delete(serverIo);
        }

};

#endif //SIPSERVER_SIPSERVER_HPP
