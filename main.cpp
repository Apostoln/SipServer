#include <SipServer.hpp>

int main(int argc, char* argv[]) {
    SipServer server;
    if (argc > 1) {
        //Server port is 0 on default if other value is not specified on main function arguments.
        //If port value is 0, system set for socket any free and allowed port.
        auto port = (unsigned short) std::atoi(argv[1]);
        server.setPort(port);
    }
    server.run();
    return 0;
}