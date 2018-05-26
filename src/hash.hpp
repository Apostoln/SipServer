#ifndef SIPSERVER_HASH_HPP
#define SIPSERVER_HASH_HPP

#include <resip/stack/CallId.hxx>

namespace std { //TODO: refactor utils
    template <> struct hash<resip::CallId>
    {
        size_t operator()(const resip::CallId &x) const
        {
            return hash<string>()(string(x.value().c_str()));
        }
    };
}

#endif //SIPSERVER_HASH_HPP
