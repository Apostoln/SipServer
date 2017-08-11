#pragma once

#include <iostream>
#include <algorithm>

#include <easylogging++.h>

#include <SipServer.hpp>

SipServer::SipServer():
    serverIo(new asio::io_service()),
    networkInterface(asio::ip::address()),
    port(0)
{
    LOG(DEBUG) << "Constructor SipServer() is called";
}

SipServer::SipServer(asio::io_service* ioService, asio::ip::address networkInterface, unsigned short port):
    serverIo(ioService),
    networkInterface(networkInterface),
    port(port)
{
    LOG(DEBUG) << "Constructor SipServer(asio::io_service*, asio::ip::address, unsigned short) is called";
}

SipServer::~SipServer() {
    LOG(DEBUG) << "Destructor ~SipServer() is called";
    delete(serverSocket);
    delete(serverIo);
}

void SipServer::init() {
    updateSocket();

    // If port is not specified by user, we should find a port number that actually assigned to socket;
    if(port == 0) {
        this->port = serverSocket->local_endpoint().port();
    }

    LOG(DEBUG) << "SipServer is initialized on " << networkInterface.to_string()
               << ":"  << this->port;
}

void SipServer::changePort(unsigned short port) {
    setPort(port);
    this->updateSocket();
    LOG(DEBUG) << "Port is changed to " << this->port;
}

void SipServer::changeServerIo(asio::io_service* serverIo) {
    setServerIo(serverIo);
    this->updateSocket();
    LOG(DEBUG) << "ServerIO is changed";
}

void SipServer::changeNetworkInterface(asio::ip::address networkInterface) {
    setNetworkInterface(networkInterface);
    this->updateSocket();
    LOG(DEBUG) << "Network interface is changed to " << networkInterface.to_string();
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
        LOG(DEBUG) << "Server socket is updated";
    }
    catch (asio::system_error & e) {
        auto errorCode = e.code().value();
        //TODO:Move error messages to logger
        std::cerr << "Error code: "  << e.code() << std::endl;
        std::cerr << "\"" << e.what() << "\"" << std::endl;
        LOG(ERROR) << "Error code: "  << e.code() << std::endl;
        LOG(ERROR) << "\"" << e.what() << "\"" << std::endl;
        switch (errorCode) {
            case 13:
                std::cerr << "Port is unavailable" << std::endl;
                LOG(ERROR) << "Port is unavailable" << std::endl;
                if (port < 1024) {
                    std::cerr << "Port must be > 1024 on Unix, port is " << port << std::endl;
                    LOG(ERROR) << "Port must be > 1024 on Unix, port is " << port << std::endl;
                }
                break;
            case 99:
                std::cerr << "Network interface is not supported: " << networkInterface.to_string() << std::endl;
                LOG(ERROR) << "Network interface is not supported: " << networkInterface.to_string() << std::endl;
                break;
            default:
                std::cerr << "Unknown asio error" << std::endl;
                LOG(ERROR) << "Unknown asio error" << std::endl;
        }
        LOG(DEBUG) << "Exit with error code" << errorCode << std::endl;
        exit(errorCode);
    }
}

void SipServer::run() {
    asio::ip::udp::endpoint clientEndPoint;

    std::cout << "Server is started" << std::endl
              << "Listening UDP port " << this->getPort() << std::endl;
    LOG(INFO) << "Server is started";
    LOG(INFO) << "Listening UDP port " << this->getPort();

    while(true) {
        char buff[1024] = {0};
        //Amount of received bytes
        size_t bytesReceived = serverSocket->receive_from(asio::buffer(buff), clientEndPoint);
        LOG(INFO) << bytesReceived << " bytes received ";

        //Add new connection if it is not exist
        if (std::find(clients.begin(), clients.end(), clientEndPoint) == clients.end()) {
            clients.push_back(clientEndPoint);
            std::cout << "Client was added: " << clientEndPoint.address() << ":" << clientEndPoint.port() << std::endl;
            LOG(INFO) << "Client was added: " << clientEndPoint.address() << ":" << clientEndPoint.port() << std::endl;
        }

        if (bytesReceived != 0) {
            size_t bytesSent = serverSocket->send_to(asio::buffer(buff), clientEndPoint);
            LOG(INFO) << bytesSent << " bytes sent ";

            std::cout << clientEndPoint.address() << ":"
                      << clientEndPoint.port() << "> "
                      << buff << std::endl;
            LOG(INFO) << clientEndPoint.address() << ":"
                      << clientEndPoint.port() << "> "
                      << buff;

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
    LOG(INFO) << "Connection with " << client.address() << ":" << client.port()
              << " is closed";

    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    if (clients.empty()) {
        std::cout << "There are no connections now, server is closed" << std::endl;
        LOG(INFO) << "There are no connections now, server is closed";
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