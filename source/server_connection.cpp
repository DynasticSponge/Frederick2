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
    this->host = hostServer;
    this->connectionError = false;
    this->maxTime = 30;
    this->socketFD = -1;    
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::acceptConnection
///////////////////////////////////////////////////////////////////////////////

bool server::connection::acceptConnection(int sockFD)
{
    bool returnValue{false};
    this->socketFD = ::accept(sockFD, &this->address, &this->addressLength);
    if (this->socketFD > 0)
    {
        size_t optval{1};
        socklen_t optlen{sizeof(optval)};
        setsockopt(this->socketFD, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
        optval = 30000;
        optlen = sizeof(optval);
        setsockopt(this->socketFD, IPPROTO_TCP, TCP_USER_TIMEOUT, &optval, optlen);
        returnValue = true;
    }
    return(returnValue);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::close
///////////////////////////////////////////////////////////////////////////////

void server::connection::close()
{
    ::close(this->socketFD);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::handleInput()
///////////////////////////////////////////////////////////////////////////////

bool server::connection::handleConnection(std::future<void> exitSignal)
{
    auto readFuncPtr = &server::connection::readData;
    std::promise<void> promiseRead;
    std::future<void> futureRead = promiseRead.get_future();
    std::future<bool> readFuture = std::async(std::launch::async, readFuncPtr, this, std::move(futureRead));
    
    auto sendFuncPtr = &server::connection::sendData;
    std::promise<void> promiseSend;
    std::future<void> futureSend = promiseSend.get_future();
    std::future<bool> sendFuture = std::async(std::launch::async, sendFuncPtr, this, std::move(futureSend));
    
    auto signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));    
    while(signalStatus != std::future_status::ready && !this->connectionError)
    {
        if(this->receiveBuffer.size() > 0)
        {
            packet::httpRequest *request{new packet::httpRequest(&this->receiveBuffer)};
            request->buildRequest();
            
            packet::httpResponse *response{this->host->handleRequest(request)};
            std::string outString{response->toString()};
            
            this->sendBuffer.append(outString);
            bool closeConn{response->getHeader("Connection") == "close"};
            
            delete request;
            delete response;
            request = nullptr;
            response = nullptr;

            if(closeConn)
            {
                break;
            }                       
        }
        signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));        
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    // close connetion
    ///////////////////////////////////////////////////////////////////////////////
    
    while(this->sendBuffer.size() > 0)
    {
        continue;
    }
    
    ::shutdown(this->socketFD, SHUT_RD);
    promiseRead.set_value();
    if(readFuture.valid()){
        bool readFinished = readFuture.get();
        if(!readFinished)
        {
            throw std::runtime_error("wtf: connection::readData returned false");
        }
    }    
    
    promiseSend.set_value();
    if(sendFuture.valid()){
        bool sendFinished = sendFuture.get();
        if(!sendFinished)
        {
            throw std::runtime_error("wtf: connection::sendData returned false");
        }
    }
    ::shutdown(this->socketFD, SHUT_RDWR);
    ::close(this->socketFD);
    
    return(true);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::readData
///////////////////////////////////////////////////////////////////////////////

bool server::connection::readData(std::future<void> exitSignal)
{
    ///////////////////////////////////////////////////////////////////////////////
    // initialize raw char buffer
    ///////////////////////////////////////////////////////////////////////////////
    
    std::vector<char> rawBuffer(256);

    ///////////////////////////////////////////////////////////////////////////////
    // continuously read raw chars into receivebuffer
    ///////////////////////////////////////////////////////////////////////////////

    bool clockRunning{false};
    size_t maxTimeMills{this->maxTime * 1000};
    auto startTime{std::chrono::steady_clock::now()};
    auto curTime{std::chrono::steady_clock::now()};

    auto signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));    
    while(signalStatus != std::future_status::ready)
    {
        ssize_t bytesReceived{::recv(this->socketFD, &rawBuffer[0], rawBuffer.capacity(), MSG_DONTWAIT)};
        if(bytesReceived > 0)
        {
            this->receiveBuffer.append(&rawBuffer[0], bytesReceived);
            rawBuffer.clear();
            clockRunning = false;
        }
        else
        {   
            bool otherError{true};
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {   
                otherError = false;
                if(!clockRunning)
                {
                    startTime = std::chrono::steady_clock::now();
                    clockRunning = true;
                }
                else
                {
                    curTime = std::chrono::steady_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count();
                    if(duration > maxTimeMills)
                    {
                        otherError = true;
                    }
                }               
            }
            if(otherError)
            {
                this->connectionError = true;
                break;
            }                    
        }        
        signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));
    }
    return(true);
}

/*
///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::readDataSSL
///////////////////////////////////////////////////////////////////////////////

bool server::connection::readDataSSL(std::future<void> exitSignal)
{
    ///////////////////////////////////////////////////////////////////////////////
    // initialize raw char buffer
    ///////////////////////////////////////////////////////////////////////////////
    
    std::vector<char> rawBuffer(256);

    ///////////////////////////////////////////////////////////////////////////////
    // continuously read raw chars into receivebuffer
    ///////////////////////////////////////////////////////////////////////////////

    bool clockRunning{false};
    size_t maxTimeMills{this->maxTime * 1000};
    auto startTime{std::chrono::steady_clock::now()};
    auto curTime{std::chrono::steady_clock::now()};

    auto signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));    
    while(signalStatus != std::future_status::ready)
    {
        ssize_t bytesReceived{::recv(this->socketFD, &rawBuffer[0], rawBuffer.capacity(), MSG_DONTWAIT)};
        if(bytesReceived > 0)
        {
            this->receiveBuffer.append(&rawBuffer[0], bytesReceived);
            rawBuffer.clear();
            clockRunning = false;
        }
        else
        {   
            bool otherError{true};
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {   
                otherError = false;
                if(!clockRunning)
                {
                    startTime = std::chrono::steady_clock::now();
                    clockRunning = true;
                }
                else
                {
                    curTime = std::chrono::steady_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count();
                    if(duration > maxTimeMills)
                    {
                        otherError = true;
                    }
                }               
            }
            if(otherError)
            {
                this->connectionError = true;
                break;
            }                    
        }        
        signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));
    }
    return(true);
}
*/

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::sendData
///////////////////////////////////////////////////////////////////////////////

bool server::connection::sendData(std::future<void> exitSignal)
{
    auto signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));    
    while(signalStatus != std::future_status::ready)
    {
        size_t buffSize{this->sendBuffer.size()};
        while(buffSize > 0)
        {   
            std::string outStr;
            int sendReturn{0};
            
            if(buffSize > 255)
            {
                outStr = this->sendBuffer.substr(0, 255);
                this->sendBuffer.erase(0, 255);
            }
            else
            {
                outStr = this->sendBuffer.substr(0, buffSize);
                this->sendBuffer.erase(0, buffSize);
            }
            
            char *outCStr = (char*)outStr.c_str();
            size_t outCStrLen = outStr.size();

            while (outCStrLen > 0)
            {
                ssize_t numSent = ::send(this->socketFD, outCStr, outCStrLen, 0);
                if (numSent < 0)
                {
                    this->connectionError = true;
                    break;
                }
                outCStr += numSent;
                outCStrLen -= numSent;
            }
            buffSize = this->sendBuffer.size();
        }
        signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));
    }
    return(true);
}

/*
///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::sendDataSSL
///////////////////////////////////////////////////////////////////////////////

bool server::connection::sendDataSSL(std::future<void> exitSignal)
{
    auto signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));    
    while(signalStatus != std::future_status::ready)
    {
        size_t buffSize{this->sendBuffer.size()};
        while(buffSize > 0)
        {   
            std::string outStr;
            int sendReturn{0};
            
            if(buffSize > 255)
            {
                outStr = this->sendBuffer.substr(0, 255);
                this->sendBuffer.erase(0, 255);
            }
            else
            {
                outStr = this->sendBuffer.substr(0, buffSize);
                this->sendBuffer.erase(0, buffSize);
            }
            
            char *outCStr = (char*)outStr.c_str();
            size_t outCStrLen = outStr.size();

            while (outCStrLen > 0)
            {
                ssize_t numSent = ::send(this->socketFD, outCStr, outCStrLen, 0);
                if (numSent < 0)
                {
                    this->connectionError = true;
                    break;
                }
                outCStr += numSent;
                outCStrLen -= numSent;
            }
            buffSize = this->sendBuffer.size();
        }
        signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));
    }
    return(true);
}
*/

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::connection::setMaxTime
///////////////////////////////////////////////////////////////////////////////

void server::connection::setMaxTime(size_t timeout)
{
    this->maxTime = timeout;
    return;
}


///////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////

server::connection::~connection()
{
    this->host = nullptr;
}