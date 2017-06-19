#pragma once
#ifndef SIPSERVER_SIPSERVER_HPP
#define SIPSERVER_SIPSERVER_HPP

#include <asio.hpp>

class SipServer {
    private:
        asio::io_service* serverIo;
        asio::ip::udp::socket* serverSocket;
        unsigned short port;
    public:
        SipServer(asio::io_service* ioService, unsigned short port) {
            this->serverIo = ioService;
            this->port = port;
            this->serverSocket = new asio::ip::udp::socket(*serverIo,
                                                           asio::ip::udp::endpoint(asio::ip::udp::v4(), port));
            if(port == 0) {
                this->port = serverSocket->local_endpoint().port();
            }
        }

        SipServer(unsigned short port):
                SipServer(new asio::io_service, port)
        {}

        SipServer():
                SipServer(0)
        {}

        ~SipServer() {
            delete(serverSocket);
            delete(serverIo);
        }

        unsigned short getPort() {
            return port;
        }

        void run() {
            asio::ip::udp::endpoint clientEndPoint;
            std::cout << "Server is started" << std::endl
                      << "Listening udp port " << this->getPort() << std::endl;
            while(true) {
                char buff[1024] = {0};
                size_t bytes = serverSocket->receive_from(asio::buffer(buff), clientEndPoint);
                if (bytes != 0) {
                    std::cout << buff << std::endl;
                    if (std::string(buff) == "q") {
                        std::cout << "Server is closed" << std::endl;
                        return;
                    }
                }

            }
        }

};

#endif //SIPSERVER_SIPSERVER_HPP
