//
// server_connection.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SERVER_CONNECTION_HPP
#define SERVER_CONNECTION_HPP

#include <chrono>
#include <future>
#include <openssl/ssl.h>
#include <string>
#include <sys/socket.h>

#include "frederick2_namespace.hpp"
#include "server.hpp"
#include "server_socket.hpp"

class frederick2::httpServer::connection
{
public:
    connection() = delete;    
    ~connection();
protected:
private:
    ///////////////////////////////////////////////////////////////////////////////
    // Friend Declarations
    ///////////////////////////////////////////////////////////////////////////////
    friend class frederick2::httpServer::httpServer;
    ///////////////////////////////////////////////////////////////////////////////
    // Private Functions
    ///////////////////////////////////////////////////////////////////////////////
    explicit connection(frederick2::httpServer::httpServer*);
    bool acceptConnection(int);    
    void close();
    bool handleConnection(std::future<void>);
    bool handleIO(std::future<void>);
    void readData();
    void readDataSSL();
    void sendData(std::string);
    void sendDataSSL(std::string);
    void setMaxTime(size_t);
    void setSSLContext(SSL_CTX*);
    void setSSLPrivateKey(const std::string&);
    void setSSLPublicCert(const std::string&);
    void setUseSSL(bool);
    void shutdownSSLConnection();    
    ///////////////////////////////////////////////////////////////////////////////
    // Private Properties
    ///////////////////////////////////////////////////////////////////////////////
    bool connectionError;
    bool useSSL;
    bool sslActive;
    size_t maxTime;
    SSL_CTX *sslContext;
    SSL *sslConnection;
    socklen_t addressLength;
    struct sockaddr address;
    std::string receiveBuffer;
    std::string sendBuffer;
    std::string sslCertPath;
    std::string sslKeyPath;
    frederick2::httpServer::httpServer *host;
    frederick2::httpServer::socket *sock;
};

#endif