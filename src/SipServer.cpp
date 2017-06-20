#pragma once

#include <iostream>

#include <SipServer.hpp>

// Delegating constructors
SipServer::SipServer(asio::io_service* ioService, unsigned short port) {
    // Server port is 0 on default if other value is not specified on constructor arguments.
    // If server port value is 0, system set for new socket any free and allowed udp port.
    this->serverIo = ioService;
    this->port = port;
    this->serverSocket = new asio::ip::udp::socket(*serverIo,
                                                   asio::ip::udp::endpoint(asio::ip::udp::v4(), port));

    // If port is not specified by user, we should find a port number that actually assigned to socket;
    if(port == 0) {
        this->port = serverSocket->local_endpoint().port();
    }
}

SipServer::SipServer(unsigned short port):
        SipServer(new asio::io_service, port)
{}

SipServer::SipServer():
        SipServer(0)
{}

SipServer::~SipServer() {
    delete(serverSocket);
    delete(serverIo);
}

unsigned short SipServer::getPort() {
    return port;
}

void SipServer::setPort(unsigned short port) {
    this->port=port;
}

void SipServer::run() {
    asio::ip::udp::endpoint clientEndPoint;

    std::cout << "Server is started" << std::endl
              << "Listening UDP port " << this->getPort() << std::endl;

    while(true) {
        char buff[1024] = {0};
        //Amount of received bytes
        size_t bytes = serverSocket->receive_from(asio::buffer(buff), clientEndPoint);
        if (bytes != 0) {
            serverSocket->send_to(asio::buffer(buff), clientEndPoint);
            std::cout << buff << std::endl;
            if (std::string(buff) == "q") {
                std::cout << "Server is closed" << std::endl;
                return;
            }
        }
    }
}


