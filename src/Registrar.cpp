#pragma once
#include "Registrar.hpp"

Registrar::Registrar(std::string& source):
    source(source) {
            load();
}

Registrar::Registrar(std::string&& source):
    source(std::move(source)) {
            load();
}

void Registrar::load() {
    accounts.clear(); //clear if non-empty
    std::fstream fin(source);
    //read file to buffer
    std::vector<std::string> buffer((std::istream_iterator<std::string>(fin)),
                                    std::istream_iterator<std::string>());

    for(auto i: buffer) {
        std::stringstream stream(i);
        std::string temp;
        std::getline(stream, temp, ','); //split string for ',' delimiter
        auto name = temp;
        std::getline(stream, temp);
        auto pass = temp;
        accounts[name] = pass;
    }
}

std::unordered_map<std::string, std::string> Registrar::getAccount() {
    return accounts;
}