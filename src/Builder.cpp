#pragma once

#include "Builder.hpp"

using Builder = SipServer::Builder;

Builder::Builder():
    sipServer(new SipServer())
{}

Builder Builder::ioService(asio::io_service* serverIo) {
    this->sipServer->setServerIo(serverIo);
    return *this;
}

Builder Builder::port(unsigned short port) {
    this->sipServer->setPort(port);
    return *this;
}

Builder Builder::networkInterface(asio::ip::address networkInterface) {
    this->sipServer->setNetworkInterface(networkInterface);
    return *this;
}

Builder Builder::networkInterface(char *string) {
    this->sipServer->setNetworkInterface(asio::ip::address::from_string(string));
    return *this;
}

SipServer Builder::build() {
    sipServer->init();
    return *sipServer;
}


