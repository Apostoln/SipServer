#pragma once

#include <easylogging++.h>
#include "Registrar.hpp"

Registrar::Registrar(std::string& source):
    source(source)
{
    load();
}

Registrar::Registrar(std::string&& source):
    source(std::move(source))
{
    load();
}

Registrar::Registrar(const Registrar& other):
    source(source), accounts(accounts)
{}

void Registrar::load() {
    LOG(DEBUG) << "Loading account to registrar from " << this->source;
    accounts.clear(); //clear if non-empty
    std::fstream fin(source);

    if(!fin.is_open()) {
        LOG(ERROR) << "Can't open file with accounts. File "
                   << this->source << " is damaged or not exist";
        exit(6);
    }

    //read file to buffer
    std::vector<std::string> buffer((std::istream_iterator<std::string>(fin)),
                                    std::istream_iterator<std::string>());

    for(auto i: buffer) {
        std::stringstream stream(i);
        std::string temp;
        std::getline(stream, temp, ','); //split string for ',' delimiter
        std::string name = temp;
        std::getline(stream, temp);
        std::string addressString = temp;
        accounts.push_back(SipAccount(name, addressString));
    }

    if(accounts.empty()) {
        LOG(WARNING) << "Accounts list is empty";
    }
    else {
        LOG(DEBUG) << "Accounts loaded:";
        for (auto a: accounts) {
            LOG(DEBUG) << (std::string) a;
        }
    }
}

std::vector<SipAccount> Registrar::getAccounts() {
    return accounts;
}

Registrar Registrar::operator=(Registrar& other)  {
    if(this == &other)
        return *this;
    source = other.source;
    accounts = other.accounts;
    return *this;
}