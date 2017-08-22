#pragma once
#ifndef SIPSERVER_REGISTRAR_HPP
#define SIPSERVER_REGISTRAR_HPP

#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iterator>
#include <sstream>


class Registrar {
    private:
        std::string source;
        std::unordered_map<std::string, std::string> accounts;
    public:
        Registrar(std::string& source);
        Registrar(std::string&& source);
        void load();
        std::unordered_map<std::string, std::string> getAccount();
};

#endif //SIPSERVER_REGISTRAR_HPP
