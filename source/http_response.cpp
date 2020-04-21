//
// http_response.cpp
// ~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <memory>
#include <string>
#include <utility>

#include "../headers/frederick2_namespace.hpp"
#include "../headers/http_response.hpp"
#include "../headers/server_enum.hpp"
#include "../headers/utility_parse.hpp"

namespace enums = frederick2::httpEnums;
namespace packet = frederick2::httpPacket;
namespace utility = frederick2::utility;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global variable definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global function definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// frederick2:: member definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////

packet::httpResponse::httpResponse()
{
    this->versionMajor = 1;
    this->versionMinor = 1;
    this->contentChunks = 0;
    this->contentLength = 0;
    this->hasContent = false;
    this->contentChunked = false;
    this->protocol = enums::httpProtocol::HTTP;
    this->requestMethod = enums::httpMethod::GET;
    this->status = enums::httpStatus::OK;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpResponse::addCookie
///////////////////////////////////////////////////////////////////////////////

void packet::httpResponse::addCookie(const std::string& cName, const std::string& cValue)
{
    this->cookies[cName] = cValue;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpResponse::addHeader
///////////////////////////////////////////////////////////////////////////////

void packet::httpResponse::addHeader(const std::string& hName, const std::string& hValue)
{
    this->headers[hName] = hValue;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpResponse::getHasContent
///////////////////////////////////////////////////////////////////////////////

bool packet::httpResponse::getHasContent()
{
    return(this->hasContent);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpResponse::getHeader
///////////////////////////////////////////////////////////////////////////////

std::string packet::httpResponse::getHeader(const std::string& hName)
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
// frederick2::httpPacket::httpResponse::getHeaderLines
///////////////////////////////////////////////////////////////////////////////

std::string packet::httpResponse::getHeaderLines()
{
    std::string newLine{"\r\n"};
    std::string returnString;
    
    for(std::pair<std::string, std::string> element : this->headers)
    {
	    returnString.append(element.first);
        returnString.append(": ");
        returnString.append(element.second);
        returnString.append(newLine);
    }

    for(std::pair<std::string, std::string> element : this->cookies)
    {
        returnString.append("Set-Cookie: ");
        returnString.append(element.first);
        returnString.append("=");
        returnString.append(element.second);
        returnString.append(newLine);
    }

    returnString.append(newLine);    
    return(std::move(returnString));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpResponse::getStatus
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::httpResponse::getStatus()
{
    return(this->status);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpResponse::getStatusLine
///////////////////////////////////////////////////////////////////////////////

std::string packet::httpResponse::getStatusLine()
{
    std::string newLine{"\r\n"};
    std::string returnString;
    returnString.append(enums::converter::protocol2str(this->protocol));
    returnString.append("/");
    returnString.append(std::to_string(this->versionMajor));
    returnString.append(".");
    returnString.append(std::to_string(this->versionMinor));
    returnString.append(" ");
    returnString.append(std::to_string((int)this->status));
    returnString.append(" ");
    returnString.append(enums::converter::status2str(this->status));
    returnString.append(newLine);
    return(std::move(returnString));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpResponse::handleContent
///////////////////////////////////////////////////////////////////////////////

void packet::httpResponse::handleContent()
{
    if(!this->hasContent)
    {
        if(this->requestMethod == enums::httpMethod::CONNECT){
            return;
        }

        int statusCode{static_cast<int>(this->status)};
        if(statusCode > 200 && statusCode != 204)
        {
            this->addHeader("Content-Length", "0");
        }

        return;
    }
    
    std::unique_ptr<utility::parseUtilities> parser{new utility::parseUtilities()};
    this->chunks.clear();
    if(this->content.size() > 256)
    {
        this->contentChunked = true;
        std::string newLine{"\r\n"};
        std::string workingContent{this->content};
        
        std::string chunk;
        size_t chunkSize{0};
        std::string chunkContent;
            
        while(workingContent.size() > 0)
        {
            if(workingContent.size() > 256)
            {
                chunkSize = 256;
                chunkContent = workingContent.substr(0, chunkSize);
                workingContent.erase(0, chunkSize);
            }
            else
            {
                chunkSize = workingContent.size();
                chunkContent = workingContent;
                workingContent.clear();
            }
            chunk.clear();
            parser->toHex(chunkSize, chunk);
            chunk.append(newLine);
            chunk.append(chunkContent);
            chunk.append(newLine);
            this->chunks.push_back(chunk);            
        }

        chunk.clear();
        chunkSize = 0;
        parser->toHex(chunkSize, chunk);
        chunk.append(newLine);
        this->chunks.push_back(chunk);
        this->addHeader("Transfer-Encoding", "chunked");
    }
    else
    {
        this->contentChunked = false;
        this->contentLength = this->content.size();
        this->addHeader("Content-Length", std::to_string(this->contentLength));
    }

    if(this->requestMethod == enums::httpMethod::HEAD)
    {
        this->content.clear();
        this->chunks.clear();
        this->contentChunked = false;
        this->hasContent = false;
    }

    return;    
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpResponse::setContent
///////////////////////////////////////////////////////////////////////////////

void packet::httpResponse::setContent(const std::string& newContent)
{
    this->content = newContent;
    this->hasContent = true;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpResponse::setMajorVersion
///////////////////////////////////////////////////////////////////////////////

void packet::httpResponse::setMajorVersion(int majVersion)
{
    this->versionMajor = majVersion;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpResponse::setMinorVersion
///////////////////////////////////////////////////////////////////////////////

void packet::httpResponse::setMinorVersion(int minVersion)
{
    this->versionMinor = minVersion;
}
    
///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpResponse::setProtocol
///////////////////////////////////////////////////////////////////////////////

void packet::httpResponse::setProtocol(enums::httpProtocol inProtocol)
{
    this->protocol = inProtocol;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpResponse::setRequestMethod
///////////////////////////////////////////////////////////////////////////////

void packet::httpResponse::setRequestMethod(enums::httpMethod inMethod)
{
    this->requestMethod = inMethod;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpResponse::setStatus
///////////////////////////////////////////////////////////////////////////////

void packet::httpResponse::setStatus(enums::httpStatus inStatus)
{
    this->status = inStatus;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpResponse::setStatusReason
///////////////////////////////////////////////////////////////////////////////

void packet::httpResponse::setStatusReason(const std::string& reason)
{
    this->statusReason = reason;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpPacket::httpResponse::toString
///////////////////////////////////////////////////////////////////////////////

std::string packet::httpResponse::toString()
{
    std::string returnString;
    returnString.append(this->getStatusLine());
    returnString.append(this->getHeaderLines());

    if(this->hasContent)
    {
        if(this->contentChunked)
        {
            std::string newLine{"\r\n"};
            for(size_t index = 0; index < this->chunks.size(); index++){
                returnString.append(this->chunks[index]);
            }
            returnString.append(newLine);
        }
        else
        {
            returnString.append(this->content);
        }        
    }
    
    return(std::move(returnString));
}

///////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////

packet::httpResponse::~httpResponse()
{

}