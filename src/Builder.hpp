#pragma once
#ifndef SIPSERVER_BUILDER_HPP
#define SIPSERVER_BUILDER_HPP

#include <SipServer.hpp>

class SipServer::Builder {
    private:
        SipServer* sipServer;

    public:
        Builder();
        Builder ioService(asio::io_service* serverIo);
        Builder port(unsigned short port);
        Builder networkInterface(asio::ip::address networkInterface);
        Builder networkInterface(char* string);
        SipServer build();
};

#endif //SIPSERVER_BUILDER_HPP
