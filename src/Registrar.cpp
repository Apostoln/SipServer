#pragma once

#include <algorithm>
#include <sstream>

#include <easylogging++.h>
#include "Registrar.hpp"
#include "ExitException.hpp"


Registrar::Registrar(Db* db):
    db(db) {
}

Registrar::~Registrar() {
    LOG(DEBUG) << "~Registrar() is called";
}

bool Registrar::addUser(const SipUser &user) {
    std::ostringstream oss; //string with address
    oss << user.address;

    size_t dumpTimeStamp = 42; //TODO: todo

    auto usersWithSuchName = db->storage.get_all<User>(where(c(&User::name) == user.name));
    if (usersWithSuchName.empty() || usersWithSuchName.size() > 1 ) {
        return false;
    }
    auto userId = usersWithSuchName[0].id;

    Location location {0, oss.str(), dumpTimeStamp, userId };
    return db->storage.insert(location);
}
