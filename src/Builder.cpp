#pragma once

#include "Builder.hpp"

using Builder = SipServer::Builder;

Builder::Builder() {}

Builder Builder::setIoService(asio::io_service* serverIo) {
    this->serverIo = serverIo;
    return *this;
}

Builder Builder::setPort(unsigned short port) {
    this->port = port;
    return *this;
}

Builder Builder::setNetworkInterface(asio::ip::address networkInterface) {
    this->networkInterface = networkInterface;
    return *this;
}

SipServer Builder::build() {
    //TODO: Network inerface
    return SipServer(serverIo, port);
}