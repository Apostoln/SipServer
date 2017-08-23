#pragma once
#ifndef SIPSERVER_REGISTRAR_HPP
#define SIPSERVER_REGISTRAR_HPP

#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iterator>
#include <sstream>

#include "SipAccount.hpp"

class Registrar {
    private:
        std::string source;
        std::vector<SipAccount> accounts;
        //std::unordered_map<std::string, std::string> accounts;
    public:
        Registrar() = default;
        Registrar(std::string& source);
        Registrar(std::string&& source);
        Registrar(const Registrar &other);
        void load();
        std::vector<SipAccount> getAccounts();
        Registrar operator=(Registrar &other);
};

#endif //SIPSERVER_REGISTRAR_HPP
