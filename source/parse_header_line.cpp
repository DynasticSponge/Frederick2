//
// parse_header_line.cpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <exception>
#include <memory>
#include <string>

#include "../headers/frederick2_namespace.hpp"
#include "../headers/parse_header_line.hpp"
#include "../headers/server_enum.hpp"
#include "../headers/utility_buffer.hpp"

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
// frederick2::httpRequest::headerLineParser member definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////

packet::headerLineParser::headerLineParser(std::string *inBuffer)
{
    this->buffer = inBuffer;
    this->bytesReceived = 0;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerLineParser::execute
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::headerLineParser::execute()
{
    std::unique_ptr<utility::bufferStringReader> buffReader{new utility::bufferStringReader(this->buffer)};
    enums::httpStatus returnStatus{enums::httpStatus::OK};
    std::string wspCharsStr{" \f\n\r\t\v"};
    const char *wspChars{wspCharsStr.c_str()};
    
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
        this->originalHeaderLine = workingLine;
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    // Find first occurance of whitespace
    // If at front, return Bad Request
    // Find first occurance of ':'
    // If whitespace occurs before ':', return Bad Request
    // If no ':' return Bad Request
    // Else extract front to ':' as field name
    ///////////////////////////////////////////////////////////////////////////////
    
    size_t wspFound{workingLine.find_first_of(wspChars)};
    size_t colFound{workingLine.find_first_of(':')};
    if(wspFound == 0)
    {
        this->statusReason = "Invalid whitespace at start of header line (RFC7230 [3.2.4])";
        return(enums::httpStatus::BAD_REQUEST);
    }
    if(colFound == std::string::npos)
    {
        this->statusReason = "Invalid header line. No divider between name:value pair. (RFC7230 [3.2])";
        return(enums::httpStatus::BAD_REQUEST);
    }
    if(wspFound != std::string::npos && wspFound < colFound)
    {
        this->statusReason = "Invalid whitespace before divider between name:value pair. (RFC7230 [3.2.4])";
        return(enums::httpStatus::BAD_REQUEST);
    }

    this->fieldName = workingLine.substr(0, colFound);
    workingLine.erase(0, colFound + 1);

    ///////////////////////////////////////////////////////////////////////////////
    // find first non whitespace character in remaining workingLine
    // find last non whitespace character in remaining workingLine
    // header value string is the range between
    ///////////////////////////////////////////////////////////////////////////////
                    
    size_t nonWspFoundFirst = workingLine.find_first_not_of(wspChars);
    size_t nonWspFoundLast = workingLine.find_last_not_of(wspChars);
    if(nonWspFoundFirst == std::string::npos)
    {
        ///////////////////////////////////////////////////////////////////////////////
        // no non-whitepsace charcters found in remaining headerLine
        // header cannot have no value
        // flag error
        ///////////////////////////////////////////////////////////////////////////////
                        
        this->statusReason = "Invalid header.  Missing value in name:value pair. (RFC7230 [3.2])";
        return(enums::httpStatus::BAD_REQUEST);
    }
    else
    {
        size_t valueLength = (nonWspFoundLast - nonWspFoundFirst) + 1;
        this->originalValueString = workingLine.substr(nonWspFoundFirst, valueLength);
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    // Major Component Parsing Finished
    // Valid Header Line
    // Parse Header Value string into Value object
    ///////////////////////////////////////////////////////////////////////////////
    
    //return(this->parseValue());

    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerLineParser::getBytesReceived
///////////////////////////////////////////////////////////////////////////////

size_t packet::headerLineParser::getBytesReceived()
{
    return(this->bytesReceived);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerLineParser::getFieldName
///////////////////////////////////////////////////////////////////////////////

std::string packet::headerLineParser::getFieldName()
{
    return(this->fieldName);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerLineParser::getFieldValueString
///////////////////////////////////////////////////////////////////////////////

std::string packet::headerLineParser::getFieldValueString()
{
    return(this->originalValueString);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerLineParser::getHeaderLineString
///////////////////////////////////////////////////////////////////////////////

std::string packet::headerLineParser::getHeaderLineString()
{
    return(this->originalHeaderLine);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::headerLineParser::getStatusReason
///////////////////////////////////////////////////////////////////////////////

std::string packet::headerLineParser::getStatusReason()
{
    return(this->statusReason);
}

///////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////

packet::headerLineParser::~headerLineParser()
{
    this->buffer = nullptr;
}