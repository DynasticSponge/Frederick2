//
// http_request.cpp
// ~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cctype>
#include <chrono>
#include <exception>
#include <memory>
#include <string>

#include "../headers/frederick2_namespace.hpp"
#include "../headers/http_request.hpp"
#include "../headers/parse_content.hpp"
#include "../headers/parse_header.hpp"
#include "../headers/server_enum.hpp"
#include "../headers/server_uri.hpp"

namespace enums = frederick2::httpEnums;
namespace packet = frederick2::httpPacket;
namespace http = frederick2::httpServer;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global variable definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global function definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// frederick2::network::connection member definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////

packet::httpRequest::httpRequest(std::string* inBuffer)
{
    this->buffer = inBuffer;
    this->hasContent = false;
    this->contentChunked = false;
    this->versionMajor = 1;
    this->versionMinor = 1;
    this->contentReceived = 0;
    this->contentLength = 0;
    this->bytesReceived = 0;
    this->method = enums::httpMethod::OPTIONS;
    this->protocol = enums::httpProtocol::HTTP;
    this->requestStatus = enums::httpStatus::OK;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::addFileSegment
///////////////////////////////////////////////////////////////////////////////

void packet::httpRequest::addFileSegment(const std::string& sName)
{
    if(this->filePath.size() > 0)
    {
        this->filePath.append("/");
    }
    this->filePath.append(sName);
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::addHeader
///////////////////////////////////////////////////////////////////////////////

void packet::httpRequest::addHeader(const std::string& hName, const std::string& hValue)
{
    this->headers[hName] = hValue;    
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::addPathParameter
///////////////////////////////////////////////////////////////////////////////

void packet::httpRequest::addPathParameter(const std::string& pName, const std::string& pValue)
{
    this->pathParameters[pName] = pValue;    
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::buildRequest
///////////////////////////////////////////////////////////////////////////////

bool packet::httpRequest::buildRequest()
{
    std::unique_ptr<packet::headerParser> headerParser{new packet::headerParser(this->buffer)};
    
    ///////////////////////////////////////////////////////////////////////////////
    // Collect and Parse Out Header
    ///////////////////////////////////////////////////////////////////////////////
    
    this->requestStatus = headerParser->execute();
    if(this->requestStatus == enums::httpStatus::OK) 
    {
        this->bytesReceived += headerParser->getBytesReceived();
        this->cookies = headerParser->getCookies();
        this->headers = headerParser->getHeaders();
        this->versionMajor = headerParser->getMajorVersion();
        this->method = headerParser->getMethod();
        this->versionMinor = headerParser->getMinorVersion();
        this->protocol = headerParser->getProtocol();            
        this->originalRequestLine = headerParser->getRequestLineString();
        this->uriObj = headerParser->getURI();
        this->queryParameters = this->uriObj.getParameters();
        this->originalURIString = headerParser->getURIString();
    }
    else
    {
        this->statusReason = headerParser->getStatusReason();
    }
            
    ///////////////////////////////////////////////////////////////////////////////
    // Validate Header
    ///////////////////////////////////////////////////////////////////////////////

    if(this->requestStatus == enums::httpStatus::OK)
    {
        this->requestStatus = this->validateRequest();
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    // Collect Content
    ///////////////////////////////////////////////////////////////////////////////
    
    if(this->hasContent && this->requestStatus == enums::httpStatus::OK)
    {
        this->requestStatus = this->collectRequestBody();
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Release Buffer and Return
    ///////////////////////////////////////////////////////////////////////////////
    
    this->buffer = nullptr;
    return(true);   
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::collectRequestBody
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::httpRequest::collectRequestBody()
{
    std::unique_ptr<packet::contentParser> bodyParser{new packet::contentParser(this->buffer)};
    if(this->contentChunked)
    {
        this->requestStatus = bodyParser->executeChunked();
    }
    else
    {
        this->requestStatus = bodyParser->executeContinuous(this->contentLength);
    }
    
    if(this->requestStatus == enums::httpStatus::OK)
    {
        this->bytesReceived += bodyParser->getBytesReceived();
        this->content = bodyParser->getContent();
        this->contentLength = bodyParser->getContentLength();
        this->contentReceived = bodyParser->getContentReceived();
    }
    else
    {
        this->statusReason = bodyParser->getStatusReason();
    }
    
    return(this->requestStatus);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::getContent
///////////////////////////////////////////////////////////////////////////////

std::string packet::httpRequest::getContent()
{
    return(this->content);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::getCookies
///////////////////////////////////////////////////////////////////////////////

strMAPstr packet::httpRequest::getCookies()
{
    return(this->cookies);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::getFilePath
///////////////////////////////////////////////////////////////////////////////

std::string packet::httpRequest::getFilePath()
{
    return(this->filePath);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::getHeader
///////////////////////////////////////////////////////////////////////////////

std::string packet::httpRequest::getHeader(const std::string& hName)
{
    std::string returnString;
    
    try
    {
        returnString = this->headers.at(hName);
    }
    catch(const std::out_of_range& e)
    {
        returnString.clear();
    }
    
    return(std::move(returnString));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::getHeaders
///////////////////////////////////////////////////////////////////////////////

strMAPstr packet::httpRequest::getHeaders()
{
    return(this->headers);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::getMajorVersion
///////////////////////////////////////////////////////////////////////////////

int packet::httpRequest::getMajorVersion()
{
    return(this->versionMajor);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::getMethod
///////////////////////////////////////////////////////////////////////////////

frederick2::httpEnums::httpMethod packet::httpRequest::getMethod()
{
    return(this->method);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::getMinorVersion
///////////////////////////////////////////////////////////////////////////////

int packet::httpRequest::getMinorVersion()
{
    return(this->versionMinor);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::getPathParameter
///////////////////////////////////////////////////////////////////////////////

std::string packet::httpRequest::getPathParameter(const std::string& pName)
{
    std::string returnString;
    
    try
    {
        returnString = this->pathParameters.at(pName);
    }
    catch(const std::out_of_range& e)
    {
        returnString.clear();
    }
    
    return(std::move(returnString));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::getProtocol
///////////////////////////////////////////////////////////////////////////////

frederick2::httpEnums::httpProtocol packet::httpRequest::getProtocol()
{
    return(this->protocol);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::getQueryParameter
///////////////////////////////////////////////////////////////////////////////

std::string packet::httpRequest::getQueryParameter(const std::string& pName)
{
    std::string returnString;
    
    try
    {
        returnString = this->queryParameters.at(pName);
    }
    catch(const std::out_of_range& e)
    {
        returnString.clear();
    }
    
    return(std::move(returnString));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::getStatus
///////////////////////////////////////////////////////////////////////////////

frederick2::httpEnums::httpStatus packet::httpRequest::getStatus()
{
    return(this->requestStatus);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::getStatusReason
///////////////////////////////////////////////////////////////////////////////

std::string packet::httpRequest::getStatusReason()
{
    return(this->statusReason);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::getURI
///////////////////////////////////////////////////////////////////////////////

frederick2::httpServer::uri packet::httpRequest::getURI()
{
    return(this->uriObj);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::getHasContent
///////////////////////////////////////////////////////////////////////////////

bool packet::httpRequest::getHasContent()
{
    return(this->hasContent);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::setMethod
///////////////////////////////////////////////////////////////////////////////

void packet::httpRequest::setMethod(enums::httpMethod inMethod)
{
    this->method = inMethod;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpRequest::validateRequest
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::httpRequest::validateRequest()
{
    if(this->versionMajor != 1)
    {
        this->requestStatus = enums::httpStatus::HTTP_VERSION_NOT_SUPPORTED;
        this->statusReason = "Server is only conformant to HTTP/1.1";
        return(this->requestStatus);
    }

    auto cLenSearch{this->headers.find("Content-Length")};
    if(cLenSearch != this->headers.end())
    {
        this->contentLength = std::stoi(this->headers["Content-Length"]);
        this->hasContent = true;
    }
            
    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////

packet::httpRequest::~httpRequest()
{
    this->buffer = nullptr;
}