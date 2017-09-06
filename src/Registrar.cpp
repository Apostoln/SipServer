#pragma once

#include <easylogging++.h>
#include "Registrar.hpp"
#include "ExitException.hpp"

Registrar::Registrar(std::string& source):
    source(source)
{
    download();
}

Registrar::Registrar(std::string&& source):
    source(std::move(source))
{
    download();
}

Registrar::Registrar(const Registrar& other):
    source(source), accounts(accounts)
{}

void Registrar::download() {
    LOG(DEBUG) << "Downloading account to registrar from " << this->source;
    accounts.clear(); //clear if non-empty
    std::ifstream fin(source);

    if(!fin.is_open()) {
        std::string description = "File " + this->source + " is damaged or not exist";
        throw ExitException(ErrorCode::ACCOUNTS_FILE_UNREACHABLE, description);
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
        LOG(DEBUG) << accounts.size() << " accounts downloaded:";
        for (auto account: accounts) {
            LOG(DEBUG) << static_cast<std::string>(account);
        }
    }
}

void Registrar::upload() {
    LOG(DEBUG) << "Uploading account to " << this->source << " from registrar";

    //rewrite file
    std::ofstream fout(source);
    if(!fout.is_open()) {
        std::string description = "File " + this->source + " is damaged or not exist";
        throw ExitException(ErrorCode::ACCOUNTS_FILE_UNREACHABLE,description);
    }
    LOG(DEBUG) << accounts.size() << " accounts uploaded:";
    for(auto account: accounts) {
        auto accountString = static_cast<std::string>(account);
        fout << accountString << std::endl;
        LOG(DEBUG) << accountString;
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

Registrar::~Registrar() {
    LOG(DEBUG) << "~Registrar() is called";
    upload();
}
