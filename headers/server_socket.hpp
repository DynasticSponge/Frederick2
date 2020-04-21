//
// server_socket.hpp
// ~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <poll.h>
#include <string>

#include "frederick2_namespace.hpp"

class frederick2::httpServer::socket
{
public:    
    socket(const socket&) = delete;
    socket& operator=(const socket&) = delete;
    ~socket();
protected:
private:
    ///////////////////////////////////////////////////////////////////////////////
    // Friend Declarations
    ///////////////////////////////////////////////////////////////////////////////
    friend class frederick2::httpServer::httpServer;
    friend class frederick2::httpServer::connection;
    ///////////////////////////////////////////////////////////////////////////////
    // Private Functions
    ///////////////////////////////////////////////////////////////////////////////
    explicit socket(const std::string&, const std::string&);
    explicit socket(int);
    void close();
    int getFD();
    bool listen(int);
    bool pollIn();
    void shutdown(bool, bool);   
    ///////////////////////////////////////////////////////////////////////////////
    // Private Properties
    ///////////////////////////////////////////////////////////////////////////////
    int sock;
    struct pollfd pollFD;
};

#endif