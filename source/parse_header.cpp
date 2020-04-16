//
// parse_header.cpp
// ~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <exception>
#include <memory>
#include <string>

#include "../headers/frederick2_namespace.hpp"
#include "../headers/parse_header.hpp"
#include "../headers/parse_header_line.hpp"
#include "../headers/parse_request_line.hpp"
#include "../headers/server_uri.hpp"
#include "../headers/utility_parse.hpp"

namespace enums = frederick2::httpEnums;
namespace utility = frederick2::utility;
namespace packet = frederick2::httpPacket;
namespace server = frederick2::httpServer;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global variable definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global function definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerParser member definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////

packet::headerParser::headerParser(std::string *inBuffer)
{
    this->buffer = inBuffer;
    this->bytesReceived = 0;

    this->versionMajor = -1;
    this->versionMinor = -1;
    this->method = enums::httpMethod::OPTIONS;
    this->protocol = enums::httpProtocol::HTTP;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerParser::execute
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::headerParser::execute()
{
    enums::httpStatus headerStatus{enums::httpStatus::OK};
    
    std::unique_ptr<packet::requestLineParser> reqLineParser{new packet::requestLineParser(this->buffer)};    
    headerStatus = reqLineParser->execute();
    switch(headerStatus)
    {
        case enums::httpStatus::OK:
            this->method = reqLineParser->getMethod();
            this->protocol = reqLineParser->getProtocol();
            this->versionMajor = reqLineParser->getMajorVersion();
            this->versionMinor = reqLineParser->getMinorVersion();
            this->uriObj = reqLineParser->getURI();
            this->originalURIString = reqLineParser->getURIString();
            this->originalRequestLine = reqLineParser->getRequestLineString();
            this->bytesReceived += reqLineParser->getBytesReceived();
            break;
        case enums::httpStatus::BAD_REQUEST:
            [[fallthrough]];
        case enums::httpStatus::NOT_IMPLEMENTED:
            [[fallthrough]];
        case enums::httpStatus::REQUEST_TIMEOUT:
            this->statusReason = reqLineParser->getStatusReason();
            return(headerStatus);
        default:
            this->statusReason = "Invalid return from reqLineParser";
            return(enums::httpStatus::INTERNAL_SERVER_ERROR);
    }

    size_t endlFound{this->buffer->find("\r\n")};
    while(endlFound != 0)
    {
        std::unique_ptr<packet::headerLineParser > headLineParser{new packet::headerLineParser(this->buffer)};
        headerStatus = headLineParser->execute();
        switch(headerStatus)
        {
            case enums::httpStatus::OK:
            {
                std::string fieldName = headLineParser->getFieldName();
                std::string originalValueString = headLineParser->getFieldValueString();
                std::string originalHeaderLine = headLineParser->getHeaderLineString();
                this->bytesReceived += headLineParser->getBytesReceived();
                headerStatus = this->handleNewHeaderField(fieldName, originalValueString);
                if(headerStatus != enums::httpStatus::OK)
                {
                    return(headerStatus);
                }
                break;
            }
            case enums::httpStatus::BAD_REQUEST:
                [[fallthrough]];
            case enums::httpStatus::REQUEST_TIMEOUT:
                this->statusReason = headLineParser->getStatusReason();
                return(headerStatus);
            default:
                this->statusReason = "Invalid return from headLineParser";
                return(enums::httpStatus::INTERNAL_SERVER_ERROR);
        }    
        endlFound = this->buffer->find("\r\n");
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Consume the CRLF that marks the end of the message header
    ///////////////////////////////////////////////////////////////////////////////
    this->buffer->erase(0, 2);
    this->bytesReceived += 2;

    return(headerStatus);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerParser::getBytesReceived
///////////////////////////////////////////////////////////////////////////////

size_t packet::headerParser::getBytesReceived()
{
    return(std::move(this->bytesReceived));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerParser::getCookies
///////////////////////////////////////////////////////////////////////////////

strMAPstr packet::headerParser::getCookies()
{
    return(std::move(this->cookies));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerParser::getHeaders
///////////////////////////////////////////////////////////////////////////////

strMAPstr packet::headerParser::getHeaders()
{
    return(std::move(this->headers));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerParser::getMajorVersion
///////////////////////////////////////////////////////////////////////////////

int packet::headerParser::getMajorVersion()
{
    return(std::move(this->versionMajor));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerParser::getMethod
///////////////////////////////////////////////////////////////////////////////

enums::httpMethod packet::headerParser::getMethod()
{
    return(std::move(this->method));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerParser::getMinorVersion
///////////////////////////////////////////////////////////////////////////////

int packet::headerParser::getMinorVersion()
{
    return(std::move(this->versionMinor));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerParser::getProtocol
///////////////////////////////////////////////////////////////////////////////

enums::httpProtocol packet::headerParser::getProtocol()
{
    return(std::move(this->protocol));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerParser::getRequestLineString
///////////////////////////////////////////////////////////////////////////////

std::string packet::headerParser::getRequestLineString()
{
    return(std::move(this->originalRequestLine));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerParser::getStatusReason
///////////////////////////////////////////////////////////////////////////////

std::string packet::headerParser::getStatusReason()
{
    return(std::move(this->statusReason));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerParser::getURI
///////////////////////////////////////////////////////////////////////////////

server::uri packet::headerParser::getURI()
{
    return(std::move(this->uriObj));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerParser::getURIString
///////////////////////////////////////////////////////////////////////////////

std::string packet::headerParser::getURIString()
{
    return(std::move(this->originalURIString));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerParser::handleNewHeaderField
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::headerParser::handleNewHeaderField(const std::string& name, const std::string& value)
{
    enums::httpStatus returnStatus{enums::httpStatus::OK};

    if(name == "Cookie")
    {
        auto cookieSearch = this->headers.find("Cookie");
        if(cookieSearch == this->headers.end())
        {
            returnStatus = this->parseCookie(value);
            switch(returnStatus)
            {
                case enums::httpStatus::OK:
                    this->headers["Cookie"] = "true";
                    break;
                case enums::httpStatus::BAD_REQUEST:
                    return(returnStatus);
                    break;    
                default:
                    this->statusReason = "Invalid return from parseCookie";
                    return(enums::httpStatus::INTERNAL_SERVER_ERROR);
                    break;
            }
        }
        else
        {
            this->statusReason = "Invalid header. Multiple Cookies.";
            return(enums::httpStatus::BAD_REQUEST);
        }
        return(returnStatus);
    }
    
    auto search = this->headers.find(name);
    if(search == this->headers.end())
    {
        this->headers[name] = value;                
    }
    else
    {
        std::string newValue{this->headers[name]}; 
        newValue.append(",");
        newValue.append(value);
        this->headers[name] = newValue;
    }

    return(returnStatus);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerParser::parseCookie
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::headerParser::parseCookie(const std::string& cValue)
{
    std::unique_ptr<utility::parseUtilities> parseUtil{new utility::parseUtilities()};
    std::string workingStr;

    if(!parseUtil->pctDecode(cValue, workingStr))
    {
        this->statusReason = "Disallowed characters in cookie string";
        return(enums::httpStatus::BAD_REQUEST);
    }
    
    while(workingStr.size() > 0)
    {
        std::string cookiePair;
        size_t semiFound{workingStr.find_first_of(';')};
        if(semiFound != std::string::npos)
        {
            cookiePair = workingStr.substr(0, semiFound);
            workingStr.erase(0, semiFound + 1);
        }
        else
        {
            cookiePair = workingStr;
            workingStr.clear();
        }
        size_t eqFound{cookiePair.find_first_of('=')};
        if(eqFound == std::string::npos || eqFound == 0 || eqFound == (cookiePair.size() - 1))
        {
            this->statusReason = "Invalid cookie pair.";
            return(enums::httpStatus::BAD_REQUEST);
        }
        else
        {
            std::string cookieNameFinal;
            std::string cookieValueFinal;
            std::string cookieNameInit{cookiePair.substr(0, eqFound)};
            std::string cookieValueInit{cookiePair.substr(eqFound + 1)};
            parseUtil->toLower(cookieNameInit, cookieNameFinal);
            size_t dqueFound{cookieValueInit.find_first_of('\"')};
            if(dqueFound != std::string::npos)
            {
                if(!parseUtil->dqExtract(cookieValueInit, cookieValueFinal, true))
                {
                    this->statusReason = "Disallowed characters in cookie string";
                    return(enums::httpStatus::BAD_REQUEST);
                }    
            }
            else
            {
                parseUtil->toLower(cookieValueInit, cookieValueFinal);
            }
            this->cookies.insert({cookieNameFinal, cookieValueFinal});            
        }
    }

    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////

packet::headerParser::~headerParser()
{
    this->buffer = nullptr;
}