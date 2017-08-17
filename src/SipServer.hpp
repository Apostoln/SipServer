#pragma once
#ifndef SIPSERVER_SIPSERVER_HPP
#define SIPSERVER_SIPSERVER_HPP

#include <vector>

#include <asio/io_service.hpp>
#include <asio/ip/udp.hpp>

#include <SipMessage.hpp>

class SipServer {
    private:
        asio::io_service* serverIo; // Object for connecting to I/O services of OS.
        asio::ip::udp::socket* serverSocket;
        unsigned short port;
        asio::ip::address networkInterface;
        std::vector<asio::ip::udp::endpoint> clients;

        void setPort(unsigned short port);
        void setServerIo(asio::io_service* serverIo);
        void setNetworkInterface(asio::ip::address networkInterface);

        static SipMessage formOutgoingMessage(SipMessage incomingMessage) {
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

    public:
        class Builder;

        SipServer();
        SipServer(asio::io_service* ioService, asio::ip::address networkInterface, unsigned short port);
        ~SipServer();
        void init();
        void updateSocket();
        void run();
        void removeClient(asio::ip::udp::endpoint& client);

        void changePort(unsigned short port);
        void changeServerIo(asio::io_service* serverIo);
        void changeNetworkInterface(asio::ip::address networkInterface);


        unsigned short getPort();
        asio::ip::address getNetworkInterface();
        std::vector<asio::ip::udp::endpoint> getClients();
};

#endif //SIPSERVER_SIPSERVER_HPP
