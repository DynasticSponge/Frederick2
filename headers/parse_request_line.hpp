//
// parse_request_line.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PARSE_REQUEST_LINE_HPP
#define PARSE_REQUEST_LINE_HPP

#include <map>
#include <string>

#include "frederick2_namespace.hpp"
#include "server_enum.hpp"
#include "server_uri.hpp"

class frederick2::httpPacket::requestLineParser
{
public:
    explicit requestLineParser(std::string*);
    frederick2::httpEnums::httpStatus execute();
    size_t getBytesReceived();
    int getMajorVersion();
    frederick2::httpEnums::httpMethod getMethod();
    int getMinorVersion();
    frederick2::httpEnums::httpProtocol getProtocol();
    std::string getRequestLineString();
    std::string getStatusReason();
    frederick2::httpServer::uri getURI();
    std::string getURIString();
    ~requestLineParser();
protected:
private:
    ///////////////////////////////////////////////////////////////////////////////
    // Private Functions
    ///////////////////////////////////////////////////////////////////////////////
    frederick2::httpEnums::httpStatus extractMajorVersion(std::string&);
    frederick2::httpEnums::httpStatus extractMethod(std::string&);
    frederick2::httpEnums::httpStatus extractMinorVersion(std::string&);
    frederick2::httpEnums::httpStatus extractProtocol(std::string&);
    frederick2::httpEnums::httpStatus extractURI(std::string&);
    frederick2::httpEnums::httpStatus parseAuthority(const std::string&);
    frederick2::httpEnums::httpStatus parseFragments(const std::string&);
    frederick2::httpEnums::httpStatus parseHost(const std::string&);
    frederick2::httpEnums::httpStatus parseHostDNS(const std::string&);
    frederick2::httpEnums::httpStatus parseHostIP4(const std::string&);
    frederick2::httpEnums::httpStatus parseHostIP6(const std::string&);
    frederick2::httpEnums::httpStatus parseParameters(const std::string&);
    frederick2::httpEnums::httpStatus parsePath(const std::string&);
    frederick2::httpEnums::httpStatus parseScheme(const std::string&);
    frederick2::httpEnums::httpStatus parseURI();
    frederick2::httpEnums::httpStatus parseUserInfo(const std::string&);
    ///////////////////////////////////////////////////////////////////////////////
    // Private Properties
    ///////////////////////////////////////////////////////////////////////////////
    int versionMajor;
    int versionMinor;
    size_t bytesReceived;
    std::string* buffer;
    std::string statusReason;
    std::string originalRequestLine;
    std::string originalURIString;
    frederick2::httpServer::uri uri;
    frederick2::httpEnums::httpMethod method;
    frederick2::httpEnums::httpProtocol protocol;
};

#endif