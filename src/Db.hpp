#ifndef SIPSERVER_DB_HXX
#define SIPSERVER_DB_HXX

#include <easylogging++.h>

#include <sqlite3.h>
#include <sqlite_orm.h>

using namespace sqlite_orm;

struct User {
    size_t id;
    std::string name;
    std::string password;
};

struct Location {
    size_t id;
    std::string address;
    size_t expired;
    size_t user;
};

inline auto initStorage(const std::string& path) {
    auto storage = make_storage(path,
                                make_table("users",
                                           make_column("id",
                                                       &User::id,
                                                       autoincrement(),
                                                       primary_key()),
                                           make_column("name",
                                                       &User::name,
                                                       unique()),
                                           make_column("password",
                                                       &User::password)),
                                make_table("locations",
                                           make_column("id",
                                                       &Location::id,
                                                       autoincrement(),
                                                       primary_key()),
                                           make_column("address",
                                                       &Location::address),
                                           make_column("expired",
                                                       &Location::expired),
                                           make_column("user",
                                                       &Location::user),
                                           foreign_key(&Location::user)
                                                   .references(&User::id)));
    return storage;
}


class Db {
    public: //TODO: what to do with storage?
        using Storage = decltype(initStorage(""));
        Storage storage;

    public:
        Db(const std::string& path):
            storage(initStorage(path)) {
            LOG(DEBUG) << "Init storage in db " << path;
        }
};


#endif //SIPSERVER_DB_HXX
