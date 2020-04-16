//
// utility_buffer.cpp
// ~~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <string>
#include "../headers/frederick2_namespace.hpp"
#include "../headers/utility_buffer.hpp"

namespace enums = frederick2::httpEnums;
namespace utility = frederick2::utility;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global variable definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global function definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// frederick2::utility::bufferStringReader member definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////

utility::bufferStringReader::bufferStringReader(std::string *inBuffer)
{
    this->buffer = inBuffer;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::utility::bufferStringReader::extractFixedSize
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus utility::bufferStringReader::extractFixedSize
(size_t contentLength, std::string& content, size_t& received, size_t maxTime)
{
    bool timeout{false};
    bool clockRunning{false};
    size_t maxTimeMills = maxTime * 1000;
    auto startTime{std::chrono::steady_clock::now()};
    auto curTime{std::chrono::steady_clock::now()};

    std::string contentData;
    received = 0;

    size_t contentRemaining = contentLength - contentData.size();
    while(!timeout && contentRemaining > 0)
    {
        size_t curSize = this->buffer->size();
        if(curSize == 0)
        {
            if(clockRunning)
            {
                clockRunning = true;
                curTime = std::chrono::steady_clock::now();
            }
            else
            {
                clockRunning = true;
                startTime = std::chrono::steady_clock::now();
                curTime = startTime;
            }                    
        }
        else if(curSize < contentRemaining)
        {
            contentData.append(this->buffer->substr(0, curSize));
            this->buffer->erase(0, curSize);
            received += curSize;
            contentRemaining = contentLength - contentData.size();
            clockRunning = false;
        }
        else
        {
            contentData.append(this->buffer->substr(0, contentRemaining));
            this->buffer->erase(0, contentRemaining);
            received += contentRemaining;
            contentRemaining = contentLength - contentData.size();
            clockRunning = false;
        }
        if(clockRunning)
        {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count();
            if(duration > maxTimeMills)
            {
                timeout = true;
            }
        }
    }
    if(timeout)
    {
        this->statusReason = "Connection timed out on request";
        return(enums::httpStatus::REQUEST_TIMEOUT);
    }

    content = std::move(contentData);
    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::utility::bufferStringReader::extractLeadingEOL
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus utility::bufferStringReader::extractLeadingEOL(size_t& bytesReceived)
{
    ///////////////////////////////////////////////////////////////////////////////
    // Check for CRLF at front of buffer and remove if present
    ///////////////////////////////////////////////////////////////////////////////
    
    bytesReceived = 0;
    if(this->buffer->size() >= 2)
    {
        if(this->buffer->at(0) == '\r' && this->buffer->at(1) == '\n')
        {
            this->buffer->erase(0,2);
            bytesReceived = 2;
        }
    }

    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::utility::bufferStringReader::extractSingleLine
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus utility::bufferStringReader::extractSingleLine
(std::string& outString, size_t& received, size_t maxSize, size_t maxTime)
{
    ///////////////////////////////////////////////////////////////////////////////
    // Start timeout Clock
    ///////////////////////////////////////////////////////////////////////////////
    
    bool timeout{false};
    size_t maxTimeMills = maxTime * 1000;
    auto startTime{std::chrono::steady_clock::now()};
    auto curTime{std::chrono::steady_clock::now()};
    
    ///////////////////////////////////////////////////////////////////////////////
    // Max implemented Request Line size is maxSize
    // Copy data from buffer into workingLine until
    // EoL found or workingLine size exceeds maxSize
    ///////////////////////////////////////////////////////////////////////////////
    
    std::string workingLine;
    size_t workingSize{workingLine.size()};
    received = 0;

    size_t endlFound{std::string::npos};
    while(!timeout && endlFound == std::string::npos && workingSize < maxSize)
    {
        std::string maxSearchString;
        size_t searchSize = maxSize - workingSize;
        size_t bufferSize{this->buffer->size()};
        if(bufferSize >= searchSize)
        {
            maxSearchString = this->buffer->substr(0, searchSize);
            endlFound = maxSearchString.find("\r\n");
        }
        else
        {
            maxSearchString = this->buffer->substr(0, bufferSize);
            endlFound = maxSearchString.find("\r\n");
        }
        if(endlFound == std::string::npos)
        {
            workingLine.append(this->buffer->substr(0, maxSearchString.size()));
            this->buffer->erase(0, maxSearchString.size());
            received += maxSearchString.size();
        }
        else
        {
            workingLine.append(this->buffer->substr(0,endlFound));
            this->buffer->erase(0, endlFound + 2);
            received += endlFound + 2;
        }
        workingSize = workingLine.size();
        curTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count();
        if(duration > maxTimeMills)
        {
            timeout = true;
        }    
    }
    if(timeout)
    {
        ///////////////////////////////////////////////////////////////////////////////
        // If reached here
        // more than maxTime in seconds elapsed and EoL still not found
        // Return Request Timeout
        ///////////////////////////////////////////////////////////////////////////////
    
        this->statusReason = "Timeout reading line from buffer";
        return(enums::httpStatus::REQUEST_TIMEOUT);
    }
    if(endlFound == std::string::npos)
    {
        ///////////////////////////////////////////////////////////////////////////////
        // If reached here and endlFound is npos then EoL exists beyond max size
        // Return Bad Request
        ///////////////////////////////////////////////////////////////////////////////
    
        this->statusReason = "Buffer line exceeds maximum length";
        return(enums::httpStatus::BAD_REQUEST);
    }

    outString = std::move(workingLine);
    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::utility::bufferStringReader::getStatusReason
///////////////////////////////////////////////////////////////////////////////

std::string utility::bufferStringReader::getStatusReason()
{
    return(std::move(this->statusReason));
}

///////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////

utility::bufferStringReader::~bufferStringReader()
{
    this->buffer = nullptr;
}
