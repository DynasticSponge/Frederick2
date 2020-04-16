//
// server_connection.hpp
// ~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SERVER_CONNECTION_HPP
#define SERVER_CONNECTION_HPP

#include <future>
#include <string>
#include <sys/socket.h>

#include "frederick2_namespace.hpp"
#include "server.hpp"
#include "server_socket.hpp"

class frederick2::httpServer::connection
{
public:
    explicit connection(frederick2::httpServer::httpServer*);
    connection() = delete;
    bool acceptConnection(int);    
    bool handleConnection(std::future<void>);
    bool readData(std::future<void>);
    bool sendData(std::future<void>);
    ~connection();
protected:
private:
    ///////////////////////////////////////////////////////////////////////////////
    // Private Functions
    ///////////////////////////////////////////////////////////////////////////////
    void close();
    ///////////////////////////////////////////////////////////////////////////////
    // Private Properties
    ///////////////////////////////////////////////////////////////////////////////
    int socketFD;
    socklen_t addressLength;
    struct sockaddr address;
    std::string receiveBuffer;
    std::string sendBuffer;
    frederick2::httpServer::httpServer *host;
};

#endif