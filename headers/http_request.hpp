//
// http_request.hpp
// ~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <map>
#include <string>
#include <vector>

#include "frederick2_namespace.hpp"
#include "server.hpp"
#include "server_connection.hpp"
#include "server_uri.hpp"

class frederick2::httpPacket::httpRequest
{
public:
    std::string getContent();
    strMAPstr getCookies();
    std::string getFilePath();
    std::string getHeader(const std::string&);    
    strMAPstr getHeaders();
    int getMajorVersion();
    frederick2::httpEnums::httpMethod getMethod();
    int getMinorVersion();
    std::string getPathParameter(const std::string&);
    frederick2::httpEnums::httpProtocol getProtocol();
    std::string getQueryParameter(const std::string&);
    frederick2::httpEnums::httpStatus getStatus();
    std::string getStatusReason();
    bool getHasContent();
    ~httpRequest();
protected:
private:
    ///////////////////////////////////////////////////////////////////////////////
    // Friend Declarations
    ///////////////////////////////////////////////////////////////////////////////
    friend class frederick2::httpServer::connection;
    friend class frederick2::httpServer::httpServer;
    ///////////////////////////////////////////////////////////////////////////////
    // Private Functions
    ///////////////////////////////////////////////////////////////////////////////
    explicit httpRequest(std::string*);
    void addFileSegment(const std::string&);
    void addHeader(const std::string&, const std::string&);
    void addPathParameter(const std::string&, const std::string&);
    bool buildRequest();
    frederick2::httpEnums::httpStatus collectRequestBody();
    frederick2::httpServer::uri getURI();
    void setMethod(frederick2::httpEnums::httpMethod);
    frederick2::httpEnums::httpStatus validateRequest();
    ///////////////////////////////////////////////////////////////////////////////
    // Private Properties
    ///////////////////////////////////////////////////////////////////////////////
    std::string *buffer;
    std::string content;
    std::string filePath;
    std::string originalRequestLine;
    std::string originalURIString;
    std::string statusReason;
    int versionMajor;
    int versionMinor;
    size_t contentReceived;
    size_t contentLength;
    size_t bytesReceived;
    bool hasContent;
    bool contentChunked;
    strMAPstr cookies;
    strMAPstr headers;
    strMAPstr pathParameters;
    strMAPstr queryParameters;
    frederick2::httpServer::uri uriObj;
    frederick2::httpEnums::httpMethod method;
    frederick2::httpEnums::httpProtocol protocol;
    frederick2::httpEnums::httpStatus requestStatus;      
};

#endif