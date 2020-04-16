//
// server_connection.cpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <exception>
#include <future>
#include <memory>
#include <cstring>
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
// frederick2::network::connection member definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////

server::connection::connection(server::httpServer *hostServer)
{
    this->host = hostServer;
    this->socketFD = -1;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::network::connection::acceptConnection
///////////////////////////////////////////////////////////////////////////////

bool server::connection::acceptConnection(int sockFD)
{
    bool returnValue{false};
    this->socketFD = ::accept(sockFD, &this->address, &this->addressLength);
    if (this->socketFD > 0)
    {
        returnValue = true;
    }
    return(returnValue);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::network::connection::close
///////////////////////////////////////////////////////////////////////////////

void server::connection::close()
{
    ::close(this->socketFD);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::network::connection::handleInput()
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
    while(signalStatus != std::future_status::ready)
    {
        if(this->receiveBuffer.size() > 0)
        {
            packet::httpRequest *request{new packet::httpRequest(&this->receiveBuffer)};
            request->buildRequest();
            
            packet::httpResponse *response{this->host->handleRequest(request)};
            std::string outString{response->toString()};
            
            this->sendBuffer.append(outString);
            
            delete request;
            delete response;
            request = nullptr;
            response = nullptr;
            break;
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
// frederick2::network::connection::readData
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

    auto signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));    
    while(signalStatus != std::future_status::ready)
    {
        ssize_t bytesReceived{::recv(this->socketFD, &rawBuffer[0], rawBuffer.capacity(), 0)};
        if(bytesReceived >= 0)
        {
            this->receiveBuffer.append(&rawBuffer[0], bytesReceived);
            rawBuffer.clear();
        }
        else
        {
            break;
        }        
        signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));
    }
    return(true);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::network::connection::sendData
///////////////////////////////////////////////////////////////////////////////

bool server::connection::sendData(std::future<void> exitSignal)
{
    ///////////////////////////////////////////////////////////////////////////////
    // continuously read raw chars into receivebuffer
    ///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////

server::connection::~connection()
{
    this->host = nullptr;
}