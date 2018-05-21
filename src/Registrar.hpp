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
        Db* db;

    public:
        Registrar(Db* db);
        ~Registrar();
        bool addUser(const SipUser &user);
};

#endif //SIPSERVER_REGISTRAR_HPP
