#pragma once
#ifndef SIPSERVER_AUTHMANAGER_HPP
#define SIPSERVER_AUTHMANAGER_HPP

#include <chrono>

#include <resip/stack/Helper.hxx>
#include <rutil/Data.hxx>

#include <SipAccount.hpp>

using namespace std::chrono_literals;

class Nonce {
    private:
        resip::Data mValue;
        std::chrono::steady_clock::time_point mCreated;
        const std::chrono::steady_clock::duration mTimeout = 1min;

    private:
        void generateNonce();

    public:
        Nonce();
        resip::Data getValue();
};

class AuthManager {
    public:
        AuthManager(std::string source);
        ~AuthManager();
        void addAuthParameters(resip::SipMessage& msg);
        bool isAuth(resip::SipMessage& msg);

    private:
        void download();
        void upload();

    private:
        std::string source;
        std::vector<SipAccount> accounts;
        Nonce nonce;
};




#endif //SIPSERVER_AUTHMANAGER_HPP
