//
// server.hpp
// ~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SERVER_HPP
#define SERVER_HPP

#include <future>
#include <string>
#include <vector>

#include "frederick2_namespace.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "server_connection.hpp"

class frederick2::httpServer::httpServer
{
public:
    explicit httpServer();
    httpServer(const httpServer&) = delete;
    httpServer& operator= (const httpServer&) = delete;
    frederick2::httpServer::resource* getResourceTree();
    bool runServer(std::future<void>);
    void setBindAddress(const std::string&);
    void setBindPort(int);
    void setConnectionTimeout(size_t);
    void setListenQueue(int);
    bool start();
    void stop();
    ~httpServer();
protected:
private:
    ///////////////////////////////////////////////////////////////////////////////
    // Friend Declarations
    ///////////////////////////////////////////////////////////////////////////////
    friend class frederick2::httpServer::connection;
    ///////////////////////////////////////////////////////////////////////////////
    // Private Functions
    ///////////////////////////////////////////////////////////////////////////////
    frederick2::httpPacket::httpResponse *handleRequest(frederick2::httpPacket::httpRequest*);
    frederick2::httpServer::resource *lookupResource(frederick2::httpPacket::httpRequest*);
    ///////////////////////////////////////////////////////////////////////////////
    // Private Properties
    ///////////////////////////////////////////////////////////////////////////////
    bool didAsyncStart;
    bool hasBindAddr;
    bool hasBindPort;
    bool hasListenQueue;
    int bindPort;
    int listenQueue;
    size_t connectionTimeout;
    std::string strBindAddr;
    std::future<bool> catchRunServer;
    std::promise<void> killRunServer;
    std::vector<std::future<bool>> childFutures;
    std::vector<std::promise<void>> childPromises; 
    frederick2::httpServer::resource *rootResource;
};

#endif