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

AuthManager::AuthManager(std::string source):
        source(source)
{
    download();
}

AuthManager::~AuthManager() {
    upload();
}


void AuthManager::download() {
    LOG(DEBUG) << "Downloading account to registrar from " << this->source;
    accounts.clear(); //clear if non-empty
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
        std::string pass = temp;
        accounts.push_back(SipAccount{name, pass}); //TODO: parse pass
    }

    if (accounts.empty()) {
        LOG(WARNING) << "Accounts list is empty";
    }
    else {
        LOG(DEBUG) << accounts.size() << " accounts downloaded:";
        for (const auto& account: accounts) {
            LOG(DEBUG) << static_cast<std::string>(account);
        }
    }
}

void AuthManager::upload() {
    LOG(DEBUG) << "Uploading account to " << this->source << " from registrar";

    //rewrite file
    std::ofstream fout(source);
    if(!fout.is_open()) {
        std::string description = "File " + this->source + " is damaged or not exist";
        throw ExitException(ErrorCode::ACCOUNTS_FILE_UNREACHABLE,description);
    }
    LOG(DEBUG) << accounts.size() << " users uploaded:";
    for(const auto& account: accounts) {
        auto accountString = static_cast<std::string>(account);
        fout << accountString << std::endl;
        LOG(DEBUG) << accountString;
    }
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
    std::string usernameString(username.c_str());
    auto it = std::find(accounts.begin(), accounts.end(), usernameString);
    if (it == accounts.end()) {
        LOG(DEBUG) << "Account " << username  << " is not found";
        return false;
    }

    const char* password = it->pass.c_str();
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
    return response == authHeader.param(resip::p_response);
}




