#ifndef SIPSERVER_METHODTYPE_HPP
#define SIPSERVER_METHODTYPE_HPP

enum class MethodType {
    NONE = 0,
    REGISTER = 1
};

namespace std {
    template <>
    struct hash<MethodType> {
        size_t operator()(MethodType method) const {
            return hash<int>()(static_cast<std::underlying_type<MethodType >::type>(method));
        }
    };
}

#endif //SIPSERVER_METHODTYPE_HPP
