//
// server.cpp
// ~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <deque>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <string>

#include "../headers/frederick2_namespace.hpp"
#include "../headers/http_request.hpp"
#include "../headers/http_response.hpp"
#include "../headers/server.hpp"
#include "../headers/server_connection.hpp"
#include "../headers/server_enum.hpp"
#include "../headers/server_resource.hpp"
#include "../headers/server_socket.hpp"
#include "../headers/server_uri.hpp"
#include "../headers/utility_parse.hpp"

namespace enums = frederick2::httpEnums;
namespace packet = frederick2::httpPacket;
namespace server = frederick2::httpServer;
namespace utility = frederick2::utility;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global variable definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global function definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::httpServer member definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////

server::httpServer::httpServer()
{
    this->didAsyncStart = false;
    this->hasBindAddr = false;
    this->hasBindPort = false;
    this->hasListenQueue = false;
    this->bindPort = -1;
    this->connectionTimeout = 30;
    this->listenQueue = -1;
    this->rootResource = new server::resource("RESOURCE_ROOT", enums::resourceType::STATIC);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::httpServer::getResourceTree
///////////////////////////////////////////////////////////////////////////////

server::resource* server::httpServer::getResourceTree()
{
    return(this->rootResource);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::httpServer::handleRequest
///////////////////////////////////////////////////////////////////////////////

packet::httpResponse *server::httpServer::handleRequest(packet::httpRequest *inbound)
{
    bool errorResponse{false};
    packet::httpResponse *outbound{new packet::httpResponse()};
    server::resource *targetResource{nullptr};
    
    // check for error detected during request parse/build
    if(inbound->getStatus() != enums::httpStatus::OK)
    {
        errorResponse = true;
        if(inbound->getStatus() == enums::httpStatus::REQUEST_TIMEOUT)
        {
            inbound->addHeader("Connection", "close");
        }
        outbound->setStatus(inbound->getStatus());
        outbound->setStatusReason(inbound->getStatusReason());        
    }

    // check for HTTP version less than max conformant version
    if(!errorResponse)
    {
        if(inbound->getMinorVersion() != 1)
        {
            errorResponse = true;
            outbound->setStatus(enums::httpStatus::UPGRADE_REQUIRED);
            outbound->setStatusReason("Server is only conformant to HTTP/1.1");
            outbound->addHeader("Upgrade", "HTTP/1.1");
            inbound->addHeader("Connection", "upgrade");
        }
    }   

    // identify target resource
    if(!errorResponse)
    {   
        targetResource = this->lookupResource(inbound);
        if(targetResource == nullptr)
        {
            errorResponse = true;
            outbound->setStatus(enums::httpStatus::NOT_FOUND);
            outbound->setStatusReason("Target Resource Not Found");
        }
    }

    // identify and call appropriate handler
    if(!errorResponse)
    {
        handlerCheck resourceHandler{targetResource->getHandler(inbound->getMethod())};
        if(resourceHandler.first){
            resourceHandler.second(inbound, outbound);
        }
        else
        {
            outbound->setStatus(enums::httpStatus::METHOD_NOT_ALLOWED);
            outbound->setStatusReason("Target resource does not support method");
            outbound->addHeader("Allow", targetResource->getMethodList());
        }
    }

    // update content headers 
    outbound->handleContent();
    
    // identify and set appropriate close header
    std::string connHeaderValue{inbound->getHeader("Connection")};
    if(connHeaderValue.size() == 0)
    {
        connHeaderValue.append("close");
    }
    outbound->addHeader("Connection", connHeaderValue);
    
    return(outbound);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::httpServer::lookupResource
///////////////////////////////////////////////////////////////////////////////

server::resource* server::httpServer::lookupResource(packet::httpRequest* inbound)
{
    server::uri requestURI{inbound->getURI()};
    std::deque<std::string> requestSegments{requestURI.getSegments()};
    server::resource *targetResource{this->rootResource};
    if(requestSegments.front() != "RESOURCE_ROOT")
    {
        return(nullptr);
    }
    if(requestSegments.size() == 1)
    {
        return(targetResource);
    }
    requestSegments.pop_front();
    while(targetResource != nullptr && requestSegments.size() > 0)
    {
        if(targetResource->getType() == enums::resourceType::FILESYSTEM)
        {
            inbound->addFileSegment(requestSegments.front());
        }
        else
        {
            targetResource = targetResource->getChild(requestSegments.front());
            if(targetResource != nullptr)
            {
                if(targetResource->getType() == enums::resourceType::DYNAMIC)
                {
                    inbound->addPathParameter(targetResource->getName(), requestSegments.front());
                }
            }            
        }        
        requestSegments.pop_front();
    }
    return(targetResource);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::httpServer::runServer
///////////////////////////////////////////////////////////////////////////////

bool server::httpServer::runServer(std::future<void> exitSignal)
{
    if(!this->didAsyncStart)
    {
        return(this->didAsyncStart);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Create socket and set it to listen for new connnections
    ///////////////////////////////////////////////////////////////////////////////
    
    std::string strBindPort{std::to_string(this->bindPort)};
    std::unique_ptr<server::socket> listenSock{new server::socket(this->strBindAddr, strBindPort)};
    if(!listenSock->listen(this->listenQueue)){
        std::string errMsg{"cannot listen on port: "};
        errMsg.append(strBindPort);
        throw std::runtime_error(errMsg);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Continuously accept new connections
    ///////////////////////////////////////////////////////////////////////////////
    auto signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));
    while(signalStatus != std::future_status::ready)
    {
        if(listenSock->pollIn())
        {
            server::connection *newConn{new server::connection(this)};
            newConn->setMaxTime(this->connectionTimeout);
            if(newConn->acceptConnection(listenSock->getFD()))
            {
                auto funcPtr = &server::connection::handleConnection;
                std::promise<void> childPromise;
                std::future<void> childExit{childPromise.get_future()};
                this->childPromises.push_back(std::move(childPromise));
                this->childFutures.push_back(std::async(std::launch::async, funcPtr, newConn, std::move(childExit)));
            }
        }
        signalStatus = exitSignal.wait_for(std::chrono::milliseconds(0));                
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Wait for threads to stop then clear the pool
    ///////////////////////////////////////////////////////////////////////////////
    
    for(int pIndex = 0; pIndex < this->childPromises.size(); pIndex++)
    {
        std::promise<void> childPromise{std::move(this->childPromises[pIndex])};
        childPromise.set_value();
    }

    for(int fIndex = 0; fIndex < this->childFutures.size(); fIndex++)
    {
        std::future<bool> childFuture{std::move(this->childFutures[fIndex])};
        if(childFuture.valid())
        {
            bool childFinished = childFuture.get();
            if(!childFinished)
            {
                throw std::runtime_error("wtf: connection::handleConnection returned false");
            }
        }
    }

    listenSock->close();
    return(true);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::httpServer::setBindAddress
///////////////////////////////////////////////////////////////////////////////

void server::httpServer::setBindAddress(const std::string& bindAddr)
{
    this->strBindAddr = bindAddr;
    this->hasBindAddr = true;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::httpServer::setBindPort
///////////////////////////////////////////////////////////////////////////////

void server::httpServer::setBindPort(int port)
{
    this->bindPort = port;
    this->hasBindPort = true;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::httpServer::setBindAddress
///////////////////////////////////////////////////////////////////////////////

void server::httpServer::setListenQueue(int queueSize)
{
    this->listenQueue = queueSize;
    this->hasListenQueue = true;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::httpServer::setConnectionTimeout
///////////////////////////////////////////////////////////////////////////////

void server::httpServer::setConnectionTimeout(size_t timeout)
{
    this->connectionTimeout = timeout;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::httpServer::start
///////////////////////////////////////////////////////////////////////////////

bool server::httpServer::start()
{
    bool safeStart{this->hasBindAddr};
    safeStart &= this->hasBindPort;
    safeStart &= this->hasListenQueue;
    if(!safeStart){
        return(safeStart);
    }
    this->didAsyncStart = safeStart;
    auto funcPtr = &server::httpServer::runServer;
    std::future<void> futureExit{this->killRunServer.get_future()};
    this->catchRunServer = std::async(std::launch::async, funcPtr, this, std::move(futureExit));
    return(safeStart);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::httpServer::stop
///////////////////////////////////////////////////////////////////////////////

void server::httpServer::stop()
{
    this->killRunServer.set_value();
    if(this->catchRunServer.valid())
    {
        bool serverFinished = this->catchRunServer.get();
        if(!serverFinished)
        {
            throw std::runtime_error("wtf: httpServer::runServer returned false");
        }
    }
    this->didAsyncStart = false;
}

///////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////

server::httpServer::~httpServer()
{
    delete this->rootResource;
    this->rootResource = nullptr;
}