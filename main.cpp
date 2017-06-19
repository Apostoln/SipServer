#include <iostream>
#include <asio.hpp>
#include <SipServer.hpp>

int main() {
    SipServer a;
    std::cout << a.port;
    return 0;
}