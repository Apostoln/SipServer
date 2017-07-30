#pragma once

#include <iostream>
#include <algorithm>

#include <SipServer.hpp>

SipServer::SipServer():
    serverIo(new asio::io_service()),
    networkInterface(asio::ip::address()),
    port(0)
{}

SipServer::SipServer(asio::io_service* ioService, asio::ip::address networkInterface, unsigned short port):
    serverIo(ioService),
    networkInterface(networkInterface),
    port(port)
{}

SipServer::~SipServer() {
    delete(serverSocket);
    delete(serverIo);
}

void SipServer::init() {
    updateSocket();

    // If port is not specified by user, we should find a port number that actually assigned to socket;
    if(port == 0) {
        this->port = serverSocket->local_endpoint().port();
    }
}

void SipServer::changePort(unsigned short port) {
    setPort(port);
    this->updateSocket();
}

void SipServer::changeServerIo(asio::io_service* serverIo) {
    setServerIo(serverIo);
    this->updateSocket();
}

void SipServer::changeNetworkInterface(asio::ip::address networkInterface) {
    setNetworkInterface(networkInterface);
    this->updateSocket();
}


void SipServer::updateSocket() {
    try {
        asio::ip::udp::endpoint endPoint;
        if (networkInterface.is_unspecified()) {
            endPoint = asio::ip::udp::endpoint(asio::ip::udp::v4(), port);
        }
        else {
            endPoint = asio::ip::udp::endpoint(networkInterface, port);
        }

        this->serverSocket = new asio::ip::udp::socket(*serverIo, endPoint);
    }
    catch (asio::system_error & e) {
        auto errorCode = e.code().value();
        std::cerr << "Error code: "  << e.code() << std::endl;
        std::cerr << "\"" << e.what() << "\"" << std::endl;
        switch (errorCode) {
            case 13:
                std::cerr << "Port is unavailable" << std::endl;
                if (port < 1024) {
                    std::cerr << "Port must be > 1024 on Unix, port is " << port << std::endl;
                }
                break;
            case 99:
                std::cerr << "Network interface is not supported: " << networkInterface.to_string() << std::endl;
                break;
            default:
                std::cerr << "Unknown asio error" << std::endl;
        }
        exit(errorCode);
    }
}

void SipServer::run() {
    asio::ip::udp::endpoint clientEndPoint;

    std::cout << "Server is started" << std::endl
              << "Listening UDP port " << this->getPort() << std::endl;

    while(true) {
        char buff[1024] = {0};
        //Amount of received bytes
        size_t bytes = serverSocket->receive_from(asio::buffer(buff), clientEndPoint);

        //Add new connection if it is not exist
        if (std::find(clients.begin(), clients.end(), clientEndPoint) == clients.end()) {
            clients.push_back(clientEndPoint);
            std::cout << "Client was added: " << clientEndPoint.address() << ":" << clientEndPoint.port() << std::endl;
        }

        if (bytes != 0) {
            serverSocket->send_to(asio::buffer(buff), clientEndPoint);
            std::cout << clientEndPoint.address() << ":"
                      << clientEndPoint.port() << "> "
                      << buff << std::endl;

            if (std::string(buff) == "q") {
                //Remove closed connection from vector
                removeClient(clientEndPoint);
            }
        }
    }
}

void SipServer::removeClient(asio::ip::udp::endpoint& client) {
    std::cout << "Connection with " << client.address() << ":" << client.port()
              << " is closed" << std::endl;

    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    if (clients.empty()) {
        std::cout << "There are no connections now, server is closed" << std::endl;
        serverSocket->close();
        exit(0);
    }
}

void SipServer::setPort(unsigned short port) {
    this->port = port;
}

void SipServer::setServerIo(asio::io_service* serverIo) {
    this->serverIo = serverIo;
}

void SipServer::setNetworkInterface(asio::ip::address networkInterface) {
    this->networkInterface = networkInterface;
}

unsigned short SipServer::getPort() {
    return port;
}

asio::ip::address SipServer::getNetworkInterface() {
    return networkInterface;
}

std::vector<asio::ip::udp::endpoint> SipServer::getClients() {
    return clients;
}