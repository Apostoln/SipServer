#pragma once

#include <iostream>
#include <algorithm>
#include <type_traits>
#include <string>
using namespace std::string_literals;

#include <easylogging++.h>

#include <SipServer.hpp>
#include <ExitException.hpp>
#include <ErrorCode.hpp>
#include <SipParser.hpp>

SipMessage SipServer::formOutgoingMessage(SipMessage incomingMessage) {
    std::vector<std::string> headers;
    std::vector<std::string> values;
    for(auto i: incomingMessage.headers) {
        headers.push_back(i.first);
        values.push_back(i.second);
    }
    std::ostringstream streamForHeaders;
    std::copy(headers.begin(), headers.end(), std::ostream_iterator<std::string>(streamForHeaders,","));
    std::string headersHeader = streamForHeaders.str();
    headersHeader.pop_back(); //remove extra comma
    incomingMessage.headers.insert(std::make_pair("Headers",headersHeader));

    std::ostringstream streamForValues;
    std::copy(values.begin(), values.end(), std::ostream_iterator<std::string>(streamForValues,","));
    std::string valuesHeader = streamForValues.str();
    valuesHeader.pop_back(); //remove extra comma
    incomingMessage.headers.insert(std::make_pair("Values",valuesHeader));
    return incomingMessage;
}

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
        auto asioErrorCode = e.code().value();
        LOG(DEBUG) << "Asio error code: "  << e.code();
        LOG(DEBUG) << "\"" << e.what() << "\"";
        ErrorCode errorCode = ErrorCode::SUCCESSFULLY;
        std::string additionalDescription = "";
        switch (asioErrorCode) {
            case 13:
                if (port < 1024) {
                    errorCode = ErrorCode::PORT_SYSTEM;
                }
                else {
                    errorCode = ErrorCode::PORT_UNAVAILABLE;
                }
                additionalDescription = "Port is " + std::to_string(port);
                break;
            case 98:
                errorCode = ErrorCode::PORT_UNAVAILABLE;
                additionalDescription = "Port is " + std::to_string(port);
                break;
            case 99:
                errorCode = ErrorCode::NETWORK_INTERFACE_ERROR;
                additionalDescription = "Network interface is " + networkInterface.to_string();
                break;
            default:
                errorCode = ErrorCode::UNKNOWN_ASIO_ERROR;
        }
        LOG(DEBUG) << "Throwing ExitException with error code " << static_cast<std::underlying_type<ErrorCode >::type>(errorCode);
        throw ExitException(errorCode, additionalDescription);
    }
}

void SipServer::run() {
    asio::ip::udp::endpoint clientEndPoint;

    std::cout << "Server is started" << std::endl
              << "Listening UDP port " << this->getPort() << std::endl;
    LOG(INFO) << "Server is started";
    LOG(INFO) << "Listening UDP port " << this->getPort();

    while(true) {
        char buff[4096] = {0};
        //Amount of received bytes
        size_t bytesReceived = serverSocket->receive_from(asio::buffer(buff), clientEndPoint);
        std::cout << bytesReceived << " bytes received: " << std::endl;
        std::cout << clientEndPoint.address() << ":"
                  << clientEndPoint.port() << " > "
                  << buff << std::endl;
        LOG(INFO) << bytesReceived << " bytes received: ";
        LOG(INFO) << clientEndPoint.address() << ":"
                  << clientEndPoint.port() << " > "
                  << buff;

        //Add new connection if it is not exist
        if (std::find(clients.begin(), clients.end(), clientEndPoint) == clients.end()) {
            clients.push_back(clientEndPoint);
            std::cout << "Client was added: " << clientEndPoint.address() << ":" << clientEndPoint.port() << std::endl;
            LOG(INFO) << "Client was added: " << clientEndPoint.address() << ":" << clientEndPoint.port();
        }

        if (bytesReceived != 0) {
            try {
                SipMessage incomingMessage = SipParser::parse(buff);
                auto outgoingMessage = formOutgoingMessage(incomingMessage);

                size_t bytesSent = serverSocket->send_to(asio::buffer(static_cast<std::string>(outgoingMessage)),
                                                         clientEndPoint);

                std::cout << bytesSent << " bytes sent: " << std::endl;
                LOG(INFO) << bytesSent << " bytes sent: ";

                std::cout << clientEndPoint.address() << ":"
                          << clientEndPoint.port() << " < "
                          << static_cast<std::string>(outgoingMessage) << std::endl;
                LOG(INFO) << clientEndPoint.address() << ":"
                          << clientEndPoint.port() << " < "
                          << static_cast<std::string>(outgoingMessage);

                if (std::string(buff) == "q") {
                    //Remove closed connection from vector
                    removeClient(clientEndPoint);
                }
            }
            catch(std::logic_error& e) {
                std::cerr << e.what() << std::endl;
                LOG(ERROR) << e.what();
                exit(5);
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
        throw ExitException(ErrorCode::SUCCESSFULLY);
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