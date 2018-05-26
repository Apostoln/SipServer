#include <iostream>
#include <algorithm>
#include <type_traits>
#include <string>
using namespace std::string_literals;

#include <easylogging++.h>

#include <resip/stack/SipMessage.hxx>
#include <resip/stack/MethodTypes.hxx>
#include <resip/stack/Helper.hxx>
#include <resip/stack/NameAddr.hxx>
#include "resip/stack/HeaderFieldValue.hxx"
#include <rutil/Data.hxx>

#include <SipServer.hpp>
#include <ExitException.hpp>
#include <ErrorCode.hpp>
#include <Registrar.hpp>
#include <thread>

std::string toString(resip::SipMessage msg) {
    std::ostringstream oss;
    oss << msg;
    return oss.str();
}

bool SipServer::send(resip::SipMessage msg, asio::ip::udp::endpoint to) {
    size_t bytesSent = serverSocket->send_to(asio::buffer(toString(msg)),
                                             to);
    LOG(INFO) << bytesSent << " bytes sent: ";
    LOG(INFO) << to.address() << ":"
              << to.port() << " < " << std::endl
              << msg;
    return bytesSent > 0;
}

std::shared_ptr<resip::SipMessage> SipServer::receive(asio::ip::udp::endpoint from) {
    char buff[4096] = {0};
    //Amount of received bytes
    LOG(DEBUG) << "Wait receiving new message from network";
    size_t bytesReceived = serverSocket->receive_from(asio::buffer(buff),
                                                      from);
    resip::Data data(buff);

    LOG(INFO) << bytesReceived << " bytes received: ";
    LOG(INFO) << from.address() << ":"
              << from.port() << " > " << std::endl
              << buff;

    auto msgPtr = std::shared_ptr<resip::SipMessage>(resip::SipMessage::make(data)); //TODO check memory leak
    return msgPtr;

}

std::shared_ptr<resip::SipMessage> SipServer::receive(resip::CallId callId) {
    LOG(DEBUG) << "Receive message from incoming queue by callId " << callId.value();
    MessagesQueue& messages = dialogs[callId];
    LOG(DEBUG) << "Start waiting";
    while (messages.empty()) ; //wait receiving message //TODO timeout
    LOG(DEBUG) << "End waiting";
    auto result = messages.back();
    LOG(DEBUG) << "Received message from incoming queue:\n" << toString(result) ;
    messages.pop_back();
    return std::shared_ptr<resip::SipMessage>(new resip::SipMessage(result));
}

void SipServer::onRegister(resip::SipMessage registerRequest) {
    LOG(DEBUG) << "onRegister scenario";
    auto uri = registerRequest.header(resip::h_Contacts).front().uri();
    std::string userId = uri.user().c_str(); //TODO get username from uri not from username
    std::string senderIp = uri.host().c_str();
    auto senderPort = uri.port();
    asio::ip::udp::endpoint userEndPoint(asio::ip::address::from_string(senderIp), senderPort);

    resip::CallId callId = registerRequest.header(resip::h_CallId);

    // Send 401 with WWW-Authenticate
    auto response401 = *resip::Helper::makeResponse(registerRequest, 401);
    authManager->addAuthParameters(response401);

    send(response401, userEndPoint);

    // Receive register with Authorization
    auto registerWithAuth = *receive(callId);

    //Check response
    bool isAuthed = authManager->isAuth(registerWithAuth);

    if (isAuthed) {
        // Send 200 OK
        auto response200 = *resip::Helper::makeResponse(registerWithAuth, 200);
        send(response200, userEndPoint);

        // Add to users
        SipUser user(userId, userEndPoint);
        if (registrar->addUser(user)) {
            LOG(DEBUG) << "Adding account " << static_cast<std::string>(user);
        }
        else {
            //TODO: Response with 404 Not Found and 200OK only after adding user not before
            //TODO: Case when user already registered on this location
        }
    }
    else {
        LOG(DEBUG) << "User is unauthorized";
        onRegister(registerWithAuth); //send 401 and process again
        //TODO: Test this case
    }

}

void SipServer::process(resip::SipMessage& incomingMessage) {
    LOG(DEBUG) << "process() ";
    if (resip::REGISTER == incomingMessage.method() ) {
        LOG(DEBUG) << "REGISTER method is observed";
        onRegister(incomingMessage);
        return;
    }
    //TODO: remove dialog
}

SipServer::SipServer():
    serverIo(new asio::io_service()),
    networkInterface(asio::ip::address()),
    port(0),
    registrar(nullptr)
{
    LOG(DEBUG) << "Constructor SipServer() is called";
}

SipServer::SipServer(asio::io_service* ioService, asio::ip::address networkInterface, unsigned short port, Registrar* registrar):
    serverIo(ioService),
    networkInterface(networkInterface),
    port(port),
    registrar(registrar)
{
    LOG(DEBUG) << "Constructor SipServer(asio::io_service*, asio::ip::address, unsigned short) is called";
}

SipServer::~SipServer() {
    LOG(DEBUG) << "Destructor ~SipServer() is called";
    delete(serverSocket);
    delete(serverIo);
    delete(registrar);
    delete(authManager);
}

void SipServer::init() {
    updateSocket();

    // If port is not specified by user, we should find a port number that actually assigned to socket;
    if(port == 0) {
        this->port = serverSocket->local_endpoint().port();
    }

    LOG(DEBUG) << "SipServer is initialized on " << networkInterface.to_string()
               << ":"  << this->port;
}

void SipServer::changePort(unsigned short port) {
    setPort(port);
    this->updateSocket();
    LOG(DEBUG) << "Port is changed to " << this->port;
}

void SipServer::changeServerIo(asio::io_service* serverIo) {
    setServerIo(serverIo);
    this->updateSocket();
    LOG(DEBUG) << "ServerIO is changed";
}

void SipServer::changeNetworkInterface(asio::ip::address networkInterface) {
    setNetworkInterface(networkInterface);
    this->updateSocket();
    LOG(DEBUG) << "Network interface is changed to " << networkInterface.to_string();
}

void SipServer::updateSocket() {
    try {
        asio::ip::udp::endpoint endPoint;
        if (networkInterface.is_unspecified()) {
            endPoint = asio::ip::udp::endpoint(asio::ip::udp::v4(), port);
        }
        else {
            endPoint = asio::ip::udp::endpoint(networkInterface, port);
        }

        this->serverSocket = new asio::ip::udp::socket(*serverIo, endPoint);
        LOG(DEBUG) << "Server socket is updated";
    }
    catch (asio::system_error & e) {
        auto asioErrorCode = e.code().value();
        LOG(DEBUG) << "Asio error code: "  << e.code();
        LOG(DEBUG) << "\"" << e.what() << "\"";
        ErrorCode errorCode = ErrorCode::SUCCESSFULLY;
        std::string additionalDescription = "";
        switch (asioErrorCode) {
            case 13:
                if (port < 1024) {
                    errorCode = ErrorCode::PORT_SYSTEM;
                }
                else {
                    errorCode = ErrorCode::PORT_UNAVAILABLE;
                }
                additionalDescription = "Port is " + std::to_string(port);
                break;
            case 98:
                errorCode = ErrorCode::PORT_UNAVAILABLE;
                additionalDescription = "Port is " + std::to_string(port);
                break;
            case 99:
                errorCode = ErrorCode::NETWORK_INTERFACE_ERROR;
                additionalDescription = "Network interface is " + networkInterface.to_string();
                break;
            default:
                errorCode = ErrorCode::UNKNOWN_ASIO_ERROR;
        }
        LOG(DEBUG) << "Throwing ExitException with error code " << static_cast<std::underlying_type<ErrorCode >::type>(errorCode);
        throw ExitException(errorCode, additionalDescription);
    }
}

void SipServer::run() {
    asio::ip::udp::endpoint clientEndPoint; // TODO: Move into main loop?

    std::cout << "Server is started" << std::endl
              << "Listening UDP port " << this->getPort() << std::endl;
    LOG(INFO) << "Server is started";
    LOG(INFO) << "Listening UDP port " << this->getPort();

    while(true) {
        auto incomingMessage = receive(clientEndPoint);

        if (nullptr != incomingMessage) {
            resip::CallId callId = incomingMessage->header(resip::h_CallId);

            if (dialogs.empty()) {
                LOG(DEBUG) << "Dialogs are empty";
            }
            else {
                LOG(DEBUG) << "Dialog : size"; //TODO: remove
                for (auto &i: dialogs) {
                    LOG(DEBUG) << i.first << " : " << i.second.size();
                }
            }

            if (dialogs.find(callId) == dialogs.end()) {
                LOG(DEBUG) << "No dialog found for " << callId.value() << " , create new one";
                dialogs[callId] = {};
                std::thread worker([=]() {
                    LOG(DEBUG) << "New thread run, id = " << std::this_thread::get_id();
                    process(*incomingMessage);
                });
                worker.detach();
            }
            else {
                LOG(DEBUG) << "Add message to dialog " << callId.value();
                dialogs[callId].push_back(*incomingMessage);
            }
        }
        else {
            LOG(ERROR) << "Incoming message is incorrect";
        }
    }
}

void SipServer::setPort(unsigned short port) {
    this->port = port;
}

void SipServer::setServerIo(asio::io_service* serverIo) {
    this->serverIo = serverIo;
}

void SipServer::setNetworkInterface(asio::ip::address networkInterface) {
    this->networkInterface = networkInterface;
}

unsigned short SipServer::getPort() {
    return port;
}

asio::ip::address SipServer::getNetworkInterface() {
    return networkInterface;
}
