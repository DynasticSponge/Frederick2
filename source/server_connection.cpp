//
// server_connection.cpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <errno.h>
#include <exception>
#include <future>
#include <memory>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "../headers/frederick2_namespace.hpp"
#include "../headers/http_request.hpp"
#include "../headers/http_response.hpp"
#include "../headers/server_connection.hpp"
#include "../headers/server_enum.hpp"

namespace enums = frederick2::httpEnums;
namespace packet = frederick2::httpPacket;
namespace server = frederick2::httpServer;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global variable definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global function definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection member definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////

server::connection::connection(server::httpServer *hostServer)
{
    this->connectionError = false;
    this->host = hostServer;
    this->maxTime = 30;
    this->sock = nullptr;
    this->sslActive = false;
    this->sslContext = nullptr;
    this->sslConnection = nullptr;
    this->useSSL = false;    
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::acceptConnection
///////////////////////////////////////////////////////////////////////////////

bool server::connection::acceptConnection(int sockFD)
{
    bool returnValue{false};
    ERR_clear_error();
    this->sock = new server::socket(::accept(sockFD, &this->address, &this->addressLength));
    if (this->sock->getFD() > 0)
    {
        size_t optval{1};
        socklen_t optlen{sizeof(optval)};
        setsockopt(this->sock->getFD(), SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
        optval = 30000;
        optlen = sizeof(optval);
        setsockopt(this->sock->getFD(), IPPROTO_TCP, TCP_USER_TIMEOUT, &optval, optlen);
        returnValue = true;

        if(this->useSSL)
        {
            this->sslConnection = SSL_new(this->sslContext);
            SSL_set_fd(this->sslConnection, this->sock->getFD());
            
            auto sslError = SSL_accept(this->sslConnection);
            if(sslError <= 0)
            {
                auto sslErrorDesc{SSL_get_error(this->sslConnection, sslError)};
                this->shutdownSSLConnection();
                returnValue = false;
            }
            this->sslActive = true;
        }
    }
    return(returnValue);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::close
///////////////////////////////////////////////////////////////////////////////

void server::connection::close()
{
    this->sock->close();
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::handleConnection()
///////////////////////////////////////////////////////////////////////////////

bool server::connection::handleConnection(std::future<void> exitSignal)
{   
    auto startTime{std::chrono::steady_clock::now()};
    auto curTime{std::chrono::steady_clock::now()};
    bool clockRunning{false};
    size_t maxTimeMills{this->maxTime * 1000};
    auto signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));    
    while(signalStatus != std::future_status::ready && !this->connectionError)
    {
        if(this->sock->pollIn())
        {   
            clockRunning = false;
            packet::httpRequest *request{new packet::httpRequest(&this->receiveBuffer)};
            auto buildFuncPtr = &packet::httpRequest::buildRequest;
            std::future<bool> buildFuture{std::async(std::launch::async, buildFuncPtr, request)};
            auto buildStatus = buildFuture.wait_for(std::chrono::milliseconds(0));    
            while(buildStatus != std::future_status::ready && !this->connectionError)
            {
                if(this->sslActive)
                {
                    this->readDataSSL();
                }
                else
                {
                    this->readData();
                }
                buildStatus = buildFuture.wait_for(std::chrono::milliseconds(0));
            }
            if(!this->connectionError)
            {
                bool buildGet{buildFuture.get()};
                if(!buildGet)
                {
                    throw std::runtime_error("wtf: packet::httpRequest::buildRequest returned false");
                }
                
                packet::httpResponse *response{this->host->handleRequest(request)};
                std::string outString{response->toString()};

                if(this->sslActive)
                {
                    this->sendDataSSL(std::move(outString));
                }
                else
                {
                    this->sendData(std::move(outString));
                }
                
                bool closeConn{response->getHeader("Connection") == "close"};
                
                delete request;
                delete response;
                
                if(closeConn)
                {
                    break;
                }
            }                                   
        }
        else
        {
            if(!clockRunning)
            {
                clockRunning = true;
                startTime = std::chrono::steady_clock::now();
            }
            else
            {
                curTime = std::chrono::steady_clock::now();
                auto timeLapse{curTime - startTime};
                if(std::chrono::duration_cast<std::chrono::milliseconds>(timeLapse).count() > maxTimeMills)
                {
                    this->connectionError = true;
                }
            }
        }
        
        signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));        
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    // close connetion
    ///////////////////////////////////////////////////////////////////////////////
    
    if(this->sslActive)
    {
        this->shutdownSSLConnection();
    }
    
    this->sock->shutdown(true, true);
    this->sock->close();   
    
    return(true);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::readData
///////////////////////////////////////////////////////////////////////////////

void server::connection::readData()
{
    std::vector<char> rawBuffer(256);
    while(this->sock->pollIn())
    {
        ssize_t bytesReceived{::recv(this->sock->getFD(), &rawBuffer[0], rawBuffer.capacity(), 0)};
        if(bytesReceived > 0)
        {
            this->receiveBuffer.append(&rawBuffer[0], bytesReceived);
            rawBuffer.clear();                        
        }
        else
        {
            this->connectionError = true;
        }
    }
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::readDataSSL
///////////////////////////////////////////////////////////////////////////////

void server::connection::readDataSSL()
{
    std::vector<char> rawBuffer(256);
    if(this->sock->pollIn())
    {
        do
        {
            ERR_clear_error();
            ssize_t bytesReceived{SSL_read(this->sslConnection, &rawBuffer[0], rawBuffer.capacity())};
            if(bytesReceived > 0)
            {
                this->receiveBuffer.append(&rawBuffer[0], bytesReceived);
                rawBuffer.clear();
            }
            else
            {
                this->connectionError = true;
                break;
            }
        } while (SSL_pending(this->sslConnection) > 0);
    }
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::sendData
///////////////////////////////////////////////////////////////////////////////

void server::connection::sendData(std::string sendBuffer)
{
    char *outCStr = (char*)sendBuffer.c_str();
    size_t outCStrLen = sendBuffer.size();

    while (outCStrLen > 0)
    {
        ssize_t numSent{::send(this->sock->getFD(), outCStr, outCStrLen, 0)};
        if (numSent < 0)
        {
            this->connectionError = true;
            break;
        }
        outCStr += numSent;
        outCStrLen -= numSent;
    }
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::sendDataSSL
///////////////////////////////////////////////////////////////////////////////

void server::connection::sendDataSSL(std::string sendBuffer)
{
    size_t buffSize{sendBuffer.size()};
    ERR_clear_error();
    ssize_t numSent{SSL_write(this->sslConnection, sendBuffer.c_str(), buffSize)};
    if(numSent < 0)
    {
        this->connectionError = true;
    }
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::setMaxTime
///////////////////////////////////////////////////////////////////////////////

void server::connection::setMaxTime(size_t timeout)
{
    this->maxTime = timeout;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::setSSLContext
///////////////////////////////////////////////////////////////////////////////

void server::connection::setSSLContext(SSL_CTX *context)
{
    this->sslContext = context;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::setSSLPrivateKey
///////////////////////////////////////////////////////////////////////////////

void server::connection::setSSLPrivateKey(const std::string& keyPath)
{
    this->sslKeyPath = keyPath;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::setSSLPublicCert
///////////////////////////////////////////////////////////////////////////////

void server::connection::setSSLPublicCert(const std::string& certPath)
{
    this->sslCertPath = certPath;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::setUseSSL
///////////////////////////////////////////////////////////////////////////////

void server::connection::setUseSSL(bool sslFlag)
{
    this->useSSL = sslFlag;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::shutdownSSLConnection
///////////////////////////////////////////////////////////////////////////////

void server::connection::shutdownSSLConnection()
{
    if(SSL_shutdown(this->sslConnection) == 0)
    {
        SSL_shutdown(this->sslConnection);
    }
    SSL_free(this->sslConnection);
    this->sslActive = false;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////

server::connection::~connection()
{
    if(this->sock != nullptr)
    {
        delete this->sock;
        this->sock = nullptr;
    }
    this->host = nullptr;
}