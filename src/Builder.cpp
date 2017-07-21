#pragma once

#include "Builder.hpp"

using Builder = SipServer::Builder;

Builder::Builder():
    sipServer(new SipServer())
{}

Builder Builder::ioService(asio::io_service* serverIo) {
    //this->sipServer->setServerIo(serverIo);
    sipServer->serverIo = serverIo;
    return *this;
}

Builder Builder::port(unsigned short port) {
    sipServer->port = port;
    return *this;
}

Builder Builder::networkInterface(asio::ip::address networkInterface) {
    sipServer->networkInterface = networkInterface;
    return *this;
}

Builder Builder::networkInterface(const char *string) {
    sipServer->networkInterface = asio::ip::address::from_string(string);
    return *this;
}

SipServer Builder::build() {
    sipServer->init();
    return *sipServer;
}


