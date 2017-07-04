#pragma once
#ifndef SIPSERVER_BUILDER_HPP
#define SIPSERVER_BUILDER_HPP

#include <SipServer.hpp>

class SipServer::Builder {
    private:
        asio::io_service* serverIo = new asio::io_service;
        asio::ip::address networkInterface = asio::ip::address();
        unsigned short port = 0;

    public:
        Builder();
        Builder setIoService(asio::io_service* serverIo);
        Builder setPort(unsigned short port);
        Builder setNetworkInterface(asio::ip::address networkInterface);
        SipServer build();
};


#endif //SIPSERVER_BUILDER_HPP
