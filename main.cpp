#include <iostream>
#include <asio.hpp>
#include <SipServer.hpp>

int main() {
    SipServer server;
    server.run();
    return 0;
}