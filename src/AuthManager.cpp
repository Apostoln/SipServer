#include <easylogging++.h>

#include "AuthManager.hpp"
#include "ExitException.hpp"
#include "SipAccount.hpp"

void Nonce::generateNonce() {
    // crutch
    resip::Data timeStamp = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()).c_str(); //KILL ME PLEASE
    std::shared_ptr<resip::SipMessage> seedDummyMessage(resip::Helper::makeRegister(resip::NameAddr("sip:foo@127.0.0.1"), resip::NameAddr("sip:bar@127.0.0.1")));
    mValue = resip::Helper::makeNonce(*seedDummyMessage, timeStamp);
}

Nonce::Nonce():
        mCreated(std::chrono::steady_clock::now()) {
    generateNonce();
}

resip::Data Nonce::getValue() {
    if (mCreated - std::chrono::steady_clock::now() > mTimeout) {
        generateNonce();
    }
    return mValue;
}

AuthManager::AuthManager(std::shared_ptr<Db> db):
        db(db) {
}

AuthManager::~AuthManager() {
}


void AuthManager::addAuthParameters(resip::SipMessage& msg) {
    resip::Auth auth;
    auth.scheme() = "Digest";
    auth.param(resip::p_nonce) = nonce.getValue();
    auth.param(resip::p_algorithm) = "MD5";
    auth.param(resip::p_realm) = msg.header(resip::h_To).uri().getAor();
    msg.header(resip::h_WWWAuthenticates).push_back(auth);
}

AuthResult AuthManager::isAuth(resip::SipMessage& msg) {
    if (!msg.exists(resip::h_Authorizations)) {
        return AuthResult::DIGEST_FAILED;
    }
    auto authHeader = msg.header(resip::h_Authorizations).front();

    auto username = authHeader.param(resip::p_username);
    std::string usernameString(username.c_str());

    auto accounts = db->storage.get_all<User>(where(c(&User::name) == usernameString));
    if (accounts.empty()) {
        LOG(WARNING) << "Account " << username  << " is not found";
        return AuthResult::USER_NOT_FOUND;
    }

    const char* password = accounts.at(0).password.c_str();
    auto realm = authHeader.param(resip::p_realm);
    auto method = getMethodName(msg.method());
    auto uri = authHeader.param(resip::p_uri);
    auto nonce = this->nonce;

    resip::Data response = resip::Helper::makeResponseMD5(username,
                                                          password,
                                                          realm,
                                                          method,
                                                          uri,
                                                          nonce.getValue());
    return response == authHeader.param(resip::p_response)
           ? AuthResult::OK
           : AuthResult::DIGEST_FAILED;
}



