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

class Registrar {
    private:
        std::string source;
        std::vector<SipUser> users;

    public:
        Registrar() = default;
        Registrar(std::string& source);
        Registrar(std::string&& source);
        Registrar(const Registrar &other);
        ~Registrar();
        void download();
        void upload();
        bool addUser(const SipUser &user);
        std::vector<SipUser> getUsers();
        Registrar operator=(Registrar &other);
};

#endif //SIPSERVER_REGISTRAR_HPP
