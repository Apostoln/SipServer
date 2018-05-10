#pragma once

#include <algorithm>

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
    source(source), users(users)
{}

void Registrar::download() {
    LOG(DEBUG) << "Downloading account to registrar from " << this->source;
    users.clear(); //clear if non-empty
    std::ifstream fin(source);

    if (!fin.is_open()) {
        std::string description = "File " + this->source + " is damaged or not exist";
        throw ExitException(ErrorCode::ACCOUNTS_FILE_UNREACHABLE, description);
    }

    //read file to buffer
    std::vector<std::string> buffer((std::istream_iterator<std::string>(fin)),
                                    std::istream_iterator<std::string>());

    for (auto i: buffer) {
        std::stringstream stream(i);
        std::string temp;
        std::getline(stream, temp, ','); //split string for ',' delimiter
        std::string name = temp;
        std::getline(stream, temp);
        std::string addressString = temp;
        users.push_back(SipUser(name, addressString));
    }

    if (users.empty()) {
        LOG(WARNING) << "Accounts list is empty";
    }
    else {
        LOG(DEBUG) << users.size() << " users downloaded:";
        for (const auto& user: users) {
            LOG(DEBUG) << static_cast<std::string>(user);
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
    LOG(DEBUG) << users.size() << " users uploaded:";
    for(const auto& user: users) {
        auto userString = static_cast<std::string>(user);
        fout << userString << std::endl;
        LOG(DEBUG) << userString;
    }
}

std::vector<SipUser> Registrar::getUsers() {
    return users;
}

Registrar Registrar::operator=(Registrar& other)  {
    if(this == &other)
        return *this;
    source = other.source;
    users = other.users;
    return *this;
}

Registrar::~Registrar() {
    LOG(DEBUG) << "~Registrar() is called";
    upload();
}

bool Registrar::addUser(const SipUser &user) {
    if (std::find(users.begin(), users.end(), user) == users.end()) {
        users.push_back(user);
        return true;
    }
    return false;
}
