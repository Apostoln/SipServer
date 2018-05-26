#pragma once

#include <algorithm>
#include <sstream>

#include <easylogging++.h>
#include "Registrar.hpp"
#include "ExitException.hpp"


Registrar::Registrar(std::shared_ptr<Db> db):
    db(db) {
}

Registrar::~Registrar() {
    LOG(DEBUG) << "~Registrar() is called";
}

bool Registrar::addUser(const SipUser &user) {
    std::ostringstream oss; //string with address
    oss << user.address;

    size_t dumpTimeStamp = DEFAULT_EXPIRED; //todo:implement expired

    auto usersWithSuchName = db->storage.get_all<User>(where(c(&User::name) == user.name));
    if (usersWithSuchName.empty() || usersWithSuchName.size() > 1 ) {
        LOG(ERROR) << "No user in db with name " << user.name;
        return false;
    }
    auto userId = usersWithSuchName[0].id;

    Location location {0, oss.str(), dumpTimeStamp, userId };
    try {
        db->storage.insert(location);
    }
    catch (std::system_error& e) {
        //TODO: check if already present
        LOG(WARNING) << "Error while adding record to locations"
                  << e.code() << ": " << e.what();
        return false;
    }
    return true;
}
