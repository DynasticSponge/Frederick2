//
// parse_content.cpp
// ~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <memory>
#include <string>

#include "../headers/frederick2_namespace.hpp"
#include "../headers/parse_content.hpp"
#include "../headers/server_enum.hpp"
#include "../headers/utility_buffer.hpp"
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
// frederick2::utility::percentEncoder member definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////

packet::contentParser::contentParser(std::string *inBuffer)
{
    this->buffer = inBuffer;
    this->bytesReceived = 0;
    this->chunksReceived = 0;
    this->contentLength = 0;
    this->contentReceived = 0;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::contentParser::executeChunked
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::contentParser::executeChunked()
{
    std::unique_ptr<utility::bufferStringReader> buffReader{new utility::bufferStringReader(this->buffer)};
    enums::httpStatus returnStatus{enums::httpStatus::OK};
    bool lastChunkReached{false};
    size_t bytesRead{0};

    while(!lastChunkReached)
    {
        size_t chunkLength{0};
        strMAPstr chunkExtensions;
        returnStatus = this->parseChunkHeader(chunkLength, chunkExtensions);
        if(returnStatus != enums::httpStatus::OK)
        {
            return(returnStatus);
        }

        this->contentLength += chunkLength;

        if(chunkLength != 0)
        {
            ///////////////////////////////////////////////////////////////////////////////
            // Read in fixed contentLength bytes of chunk data
            ///////////////////////////////////////////////////////////////////////////////
            
            std::string chunkData;            

            returnStatus = buffReader->extractFixedSize(chunkLength, chunkData, bytesRead, 30);
            if(returnStatus != enums::httpStatus::OK)
            {
                this->statusReason = buffReader->getStatusReason();
                return(returnStatus);
            }
            else
            {
                this->bytesReceived += bytesRead;
                this->contentReceived += bytesRead;
            }
            
            ///////////////////////////////////////////////////////////////////////////////
            // consume mandatory CRLF after end of chunk data
            // append chunkdata to total content
            ///////////////////////////////////////////////////////////////////////////////
            
            returnStatus = buffReader->extractLeadingEOL(bytesRead);
            if(returnStatus != enums::httpStatus::OK)
            {
                this->statusReason = buffReader->getStatusReason();
                return(returnStatus);
            }
            else
            {
                this->bytesReceived += bytesRead;
            }
            
            ///////////////////////////////////////////////////////////////////////////////
            // append chunkdata to total content
            ///////////////////////////////////////////////////////////////////////////////
            
            this->content.append(chunkData);
            this->chunksReceived++;
        }
        else
        {
            lastChunkReached = true;
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    // extract trailer-part stuff until line with only CRLF
    ///////////////////////////////////////////////////////////////////////////////
     
    size_t endlFound{this->buffer->find("\r\n")};
    while(endlFound != 0)
    {
        std::string trailerPart;
        returnStatus = buffReader->extractSingleLine(trailerPart, bytesRead, 8192, 30);
        if(returnStatus != enums::httpStatus::OK)
        {
            this->statusReason = buffReader->getStatusReason();
            return(returnStatus);
        }
        else
        {
            this->bytesReceived += bytesRead;

            // ***** process the extrated line here *****
            
        }        
        endlFound = this->buffer->find("\r\n");
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    // extract trainling CRLF
    ///////////////////////////////////////////////////////////////////////////////
    
    returnStatus = buffReader->extractLeadingEOL(bytesRead);
    if(returnStatus != enums::httpStatus::OK)
    {
        this->statusReason = buffReader->getStatusReason();
        return(returnStatus);
    }
    else
    {
        this->bytesReceived += bytesRead;
    }
        
    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::contentParser::executeContinuous
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::contentParser::executeContinuous(const size_t& contentLength)
{
    std::unique_ptr<utility::bufferStringReader> buffReader{new utility::bufferStringReader(this->buffer)};
    enums::httpStatus returnStatus{enums::httpStatus::OK};
    
    this->contentLength = contentLength;
    std::string bodyData;
    size_t bytesRead{0};

    returnStatus = buffReader->extractFixedSize(contentLength, bodyData, bytesRead, 30);
    if(returnStatus != enums::httpStatus::OK)
    {
        this->statusReason = buffReader->getStatusReason();
        return(returnStatus);
    }
    else
    {
        this->content = std::move(bodyData);
        this->bytesReceived += bytesRead;
        this->contentReceived += bytesRead;
    }
    
    return(returnStatus);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::contentParser::extractChunkLength
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::contentParser::extractChunkLength(std::string& workingStr, size_t& length)
{
    std::string chunkLenHex;
    size_t semiFound{workingStr.find_first_of(';')};
    if(semiFound != std::string::npos)
    {
        if(semiFound == 0)
        {
            this->statusReason = "Invalid chunk header";
            return(enums::httpStatus::BAD_REQUEST);
        }
        chunkLenHex = workingStr.substr(0, semiFound);
        workingStr.erase(0, semiFound + 1);
    }
    else
    {
        chunkLenHex = workingStr;
        workingStr.clear();
    }

    try
    {
        length = std::stoi(chunkLenHex, 0, 16);
    }
    catch (const std::invalid_argument& e)
    {
        this->statusReason = "Invalid chunk header.  Invalid chunk length.";
        return(enums::httpStatus::BAD_REQUEST);
    }
    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::contentParser::getBytesReceived
///////////////////////////////////////////////////////////////////////////////

size_t packet::contentParser::getBytesReceived()
{
    return(std::move(this->bytesReceived));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::contentParser::getContent
///////////////////////////////////////////////////////////////////////////////

std::string packet::contentParser::getContent()
{
    return(std::move(this->content));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::contentParser::getContentLength
///////////////////////////////////////////////////////////////////////////////

size_t packet::contentParser::getContentLength()
{
    return(std::move(this->contentLength));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::contentParser::getContentReceived
///////////////////////////////////////////////////////////////////////////////

size_t packet::contentParser::getContentReceived()
{
    return(std::move(this->contentReceived));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::contentParser::getStatusReason
///////////////////////////////////////////////////////////////////////////////

std::string packet::contentParser::getStatusReason()
{
    return(std::move(this->statusReason));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::contentParser::parseChunkHeader
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::contentParser::parseChunkHeader(size_t& chunkLength, strMAPstr& extensionMap)
{
    std::unique_ptr<utility::bufferStringReader> buffReader{new utility::bufferStringReader(this->buffer)};
    std::unique_ptr<utility::parseUtilities> parseUtil{new utility::parseUtilities()};
    enums::httpStatus returnStatus{enums::httpStatus::OK};
    
    std::string workingLine;
    size_t bytesRead{0};
    
    returnStatus = buffReader->extractSingleLine(workingLine, bytesRead, 8192, 30);
    if(returnStatus != enums::httpStatus::OK)
    {
        this->statusReason = buffReader->getStatusReason();
        return(returnStatus);
    }
    else
    {
        this->bytesReceived += bytesRead;
    }
    

    returnStatus = this->extractChunkLength(workingLine, chunkLength);
    if(returnStatus != enums::httpStatus::OK)
    {
        return(returnStatus);
    }

    while(workingLine.size() > 0)
    {
        std::string extensionStr;
        size_t semiFound{workingLine.find_first_of(';')};
        if(semiFound != std::string::npos)
        {
            extensionStr = workingLine.substr(0, semiFound);
            workingLine.erase(0, semiFound + 1);
        }
        else
        {
            extensionStr = workingLine;
            workingLine.clear();
        }

        std::string eNameInit;
        std::string eNameFinal;
        std::string eValueInit;
        std::string eValueFinal;                
        size_t eqFound{extensionStr.find_first_of('=')};
        if(eqFound != std::string::npos)
        {
            if(eqFound == 0)
            {
                this->statusReason = "Invalid chunk extension value";
                return(enums::httpStatus::BAD_REQUEST);
            }
            else
            {
                eNameInit = extensionStr.substr(0, eqFound);
                eValueInit = extensionStr.substr(eqFound + 1);
            }
        }
        else
        {
            eNameInit = extensionStr;
        }
        
        parseUtil->toLower(eNameInit, eNameFinal);
        size_t dqFound{eValueInit.find_first_of('\"')};
        if(dqFound != std::string::npos)
        {
            if(!parseUtil->dqExtract(eValueInit, eValueFinal, true))
            {
                this->statusReason = "Invalid characters in chunk extension";
                return(enums::httpStatus::BAD_REQUEST);
            }
        }
        else
        {
            parseUtil->toLower(eValueInit, eValueFinal);
        }

        extensionMap.insert({eNameFinal, eValueFinal});
    }
    
    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////

packet::contentParser::~contentParser()
{
    this->buffer = nullptr;
}