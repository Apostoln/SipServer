#pragma once
#ifndef SIPSERVER_REGISTRAR_HPP
#define SIPSERVER_REGISTRAR_HPP

#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iterator>
#include <sstream>

#include "SipUser.hpp"
#include "Db.hpp"


class Registrar {
    private:
        std::shared_ptr<Db> db;
        static constexpr size_t DEFAULT_EXPIRED = 3600;

    public:
        Registrar(std::shared_ptr<Db> db);
        ~Registrar();
        bool addUser(const SipUser &user);
        asio::ip::udp::endpoint getEndPoint(const std::string& username);
};

#endif //SIPSERVER_REGISTRAR_HPP
