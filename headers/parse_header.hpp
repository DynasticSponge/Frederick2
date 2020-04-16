//
// parse_header.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PARSE_HEADER_HPP
#define PARSE_HEADER_HPP

#include <map>
#include <string>
#include <vector>

#include "frederick2_namespace.hpp"
#include "server_enum.hpp"
#include "server_uri.hpp"

class frederick2::httpPacket::headerParser
{
public:
    explicit headerParser(std::string*);
    frederick2::httpEnums::httpStatus execute();
    size_t getBytesReceived();
    strMAPstr getCookies();
    strMAPstr getHeaders();
    int getMajorVersion();
    frederick2::httpEnums::httpMethod getMethod();
    int getMinorVersion();
    frederick2::httpEnums::httpProtocol getProtocol();
    std::string getRequestLineString();
    std::string getStatusReason();
    frederick2::httpServer::uri getURI();
    std::string getURIString();
    ~headerParser();
protected:
private:
    frederick2::httpEnums::httpStatus handleNewHeaderField(const std::string&, const std::string&);
    frederick2::httpEnums::httpStatus parseCookie(const std::string&);
    int versionMajor;
    int versionMinor;
    size_t bytesReceived;
    std::string *buffer;
    std::string originalRequestLine;
    std::string originalURIString;
    std::string statusReason;
    strMAPstr cookies;
    strMAPstr headers;
    frederick2::httpEnums::httpMethod method;
    frederick2::httpEnums::httpProtocol protocol;
    frederick2::httpServer::uri uriObj;    
};

#endif