#include "AuthManager.hpp"


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

void AuthManager::addAuthParameters(resip::SipMessage& msg) {
    resip::Auth auth;
    auth.scheme() = "Digest";
    auth.param(resip::p_nonce) = nonce.getValue();
    auth.param(resip::p_algorithm) = "MD5";
    auth.param(resip::p_realm) = msg.header(resip::h_To).uri().getAor();
    msg.header(resip::h_WWWAuthenticates).push_back(auth);
}

bool AuthManager::isAuth(resip::SipMessage& msg) {
    if (!msg.exists(resip::h_Authorizations)) {
        return false;
    }
    auto authHeader = msg.header(resip::h_Authorizations).front();

    auto username = authHeader.param(resip::p_username);
    const char* password = "qwerty"; //TODO: Take from account
    auto realm = authHeader.param(resip::p_realm);
    auto method = getMethodName(msg.method());
    auto uri = authHeader.param(resip::p_uri);
    auto nonce = this->nonce; //TODO: Nonces generation

    resip::Data response = resip::Helper::makeResponseMD5(username,
                                                          password,
                                                          realm,
                                                          method,
                                                          uri,
                                                          nonce.getValue());
    return response == authHeader.param(resip::p_response);
}



