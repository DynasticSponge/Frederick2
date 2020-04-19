//
// http_response.hpp
// ~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>

#include "frederick2_namespace.hpp"

class frederick2::httpPacket::httpResponse
{
public:
    httpResponse();
    void addCookie(const std::string&, const std::string&);
    void addHeader(const std::string&, const std::string&);
    void setContent(const std::string&);
    void setStatus(frederick2::httpEnums::httpStatus);
    void setStatusReason(const std::string&);
    ~httpResponse();
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
    bool getHasContent();
    std::string getHeader(const std::string&);
    std::string getHeaderLines();
    frederick2::httpEnums::httpStatus getStatus();
    std::string getStatusLine();
    void handleContent();
    void setMajorVersion(int);
    void setMinorVersion(int);
    void setProtocol(frederick2::httpEnums::httpProtocol);
    std::string toString();
    ///////////////////////////////////////////////////////////////////////////////
    // Private Properties
    ///////////////////////////////////////////////////////////////////////////////
    int versionMajor;
    int versionMinor;
    int contentChunks;
    size_t contentLength;
    bool hasContent;
    bool contentChunked;
    strMAPstr cookies;
    strMAPstr headers;
    strVECTOR chunks;
    std::string content;
    std::string statusReason;
    frederick2::httpEnums::httpProtocol protocol;
    frederick2::httpEnums::httpStatus status;
};

#endif