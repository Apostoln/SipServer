#ifndef SIPSERVER_SIPACCOUNT_HPP
#define SIPSERVER_SIPACCOUNT_HPP

#include <string>

struct SipAccount {
    std::string name;
    std::string pass;

    operator std::string() const;
    bool operator ==(const std::string& other);
};

#endif //SIPSERVER_SIPACCOUNT_HPP
