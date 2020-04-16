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
    std::string toString();
    void addCookie(const std::string&, const std::string&);
    void addHeader(const std::string&, const std::string&);
    frederick2::httpEnums::httpStatus getStatus();
    bool getHasContent();
    void handleContent();
    void handleErrorResponse();
    void setContent(const std::string&);
    void setMajorVersion(int);
    void setMinorVersion(int);
    void setProtocol(frederick2::httpEnums::httpProtocol);
    void setStatus(frederick2::httpEnums::httpStatus);
    void setStatusReason(const std::string&);
    ~httpResponse();
protected:
private:
    ///////////////////////////////////////////////////////////////////////////////
    // Private Functions
    ///////////////////////////////////////////////////////////////////////////////
    std::string getErrorResponseContent();
    std::string getHeaderLines();
    std::string getStatusLine();
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