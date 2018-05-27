#pragma once
#ifndef SIPSERVER_SIPSERVER_HPP
#define SIPSERVER_SIPSERVER_HPP

#include <vector>

#include <asio/io_service.hpp>
#include <asio/ip/udp.hpp>

#include <resip/stack/SipMessage.hxx>
#include <resip/stack/CallId.hxx>

#include <Registrar.hpp>
#include <AuthManager.hpp>
#include <hash.hpp>

class SipServer {
    private:
        asio::io_service* serverIo; // Object for connecting to I/O services of OS.
        asio::ip::udp::socket* serverSocket;
        unsigned short port;
        asio::ip::address networkInterface;

        using MessagesQueue = std::vector<resip::SipMessage>;
        std::unordered_map<resip::CallId, MessagesQueue> dialogs;

        Registrar* registrar;
        AuthManager* authManager;

    private:
        void setPort(unsigned short port);
        void setServerIo(asio::io_service* serverIo);
        void setNetworkInterface(asio::ip::address networkInterface);

        void process(resip::SipMessage& incomingMessage);

    public:
        class Builder;

        SipServer();
        SipServer(asio::io_service* ioService, asio::ip::address networkInterface, unsigned short port, Registrar* registrar);
        ~SipServer();
        void init();
        void updateSocket();
        void run();

        void changePort(unsigned short port);
        void changeServerIo(asio::io_service* serverIo);
        void changeNetworkInterface(asio::ip::address networkInterface);

        asio::ip::udp::endpoint makeEndPoint(resip::NameAddr contact);

        void onRegister(resip::SipMessage registerRequest);
        void onUnsupported(resip::SipMessage unsupportedRequest);

        bool send(resip::SipMessage msg, asio::ip::udp::endpoint to);
        std::shared_ptr<resip::SipMessage> receive(asio::ip::udp::endpoint from);
        std::shared_ptr<resip::SipMessage> receive(resip::CallId callId);

        unsigned short getPort();
        asio::ip::address getNetworkInterface();
};

#endif //SIPSERVER_SIPSERVER_HPP
