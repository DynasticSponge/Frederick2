//
// parse_request_line.cpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <memory>
#include <string>

#include "../headers/frederick2_namespace.hpp"
#include "../headers/parse_request_line.hpp"
#include "../headers/server_uri.hpp"
#include "../headers/utility_buffer.hpp"
#include "../headers/utility_parse.hpp"

namespace enums = frederick2::httpEnums;
namespace packet = frederick2::httpPacket;
namespace server = frederick2::httpServer;
namespace utility = frederick2::utility;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global variable definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global function definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser member definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////

packet::requestLineParser::requestLineParser(std::string* inBuffer)
{
    this->buffer = inBuffer;
    this->bytesReceived = 0;
    this->versionMajor = -1;
    this->versionMinor = -1;
    this->statusReason = "No execution occurred";
    this->method = enums::httpMethod::OPTIONS;
    this->protocol = enums::httpProtocol::HTTP;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::execute
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::execute()
{
    enums::httpStatus requestStatus{enums::httpStatus::OK};
    std::unique_ptr<utility::bufferStringReader> buffReader{new utility::bufferStringReader(this->buffer)};
    std::string wspCharsStr{" \f\n\r\t\v"};
    const char *wspChars{wspCharsStr.c_str()};
    
    std::string workingLine;
    size_t bytesRead{0};
    
    ///////////////////////////////////////////////////////////////////////////////
    // Check for left over CRLF from previous request and discard if found
    // (RFC 7230 [3.5])
    ///////////////////////////////////////////////////////////////////////////////
    
    requestStatus = buffReader->extractLeadingEOL(bytesRead);
    if(requestStatus != enums::httpStatus::OK){
        this->statusReason = buffReader->getStatusReason();
        return(requestStatus);
    }
    else
    {
        this->bytesReceived += bytesRead;
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    // Get first full line of text (up to \r\n) and place in workingLine
    // Max line size = 8192, Timeout = 30s
    ///////////////////////////////////////////////////////////////////////////////
    
    requestStatus = buffReader->extractSingleLine(workingLine, bytesRead, 8192, 30);
    if(requestStatus != enums::httpStatus::OK){
        this->statusReason = buffReader->getStatusReason();
        return(requestStatus);
    }
    else
    {
        this->bytesReceived += bytesRead;
    }

    requestStatus = this->extractMethod(workingLine);
    if(requestStatus != enums::httpStatus::OK){
        return(requestStatus);
    };

    requestStatus = this->extractURI(workingLine);
    if(requestStatus != enums::httpStatus::OK){
        return(requestStatus);
    };

    requestStatus = this->extractProtocol(workingLine);
    if(requestStatus != enums::httpStatus::OK){
        return(requestStatus);
    };

    requestStatus = this->extractMajorVersion(workingLine);
    if(requestStatus != enums::httpStatus::OK){
        return(requestStatus);
    };

    requestStatus = this->extractMinorVersion(workingLine);
    if(requestStatus != enums::httpStatus::OK){
        return(requestStatus);
    };
    
    ///////////////////////////////////////////////////////////////////////////////
    // Major Component Parsing Finished
    // Valid Request Line
    // Parse URI string into URI object
    ///////////////////////////////////////////////////////////////////////////////
    
    return(this->parseURI());
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::extractMajorVersion
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::extractMajorVersion(std::string& workingLine)
{
    std::string wspCharsStr{" \f\n\r\t\v"};
    const char *wspChars{wspCharsStr.c_str()};

    ///////////////////////////////////////////////////////////////////////////////
    // Find first occurance of '.'
    // If whitepace occurs before '.', return Bad Request
    // If at front, return Bad Request
    // If no '.', return Bad Request
    // Else extract front to occurance as versionMajor
    ///////////////////////////////////////////////////////////////////////////////
    
    size_t wspFound{workingLine.find_first_of(wspChars)};
    size_t dotFound{workingLine.find_first_of('.')};
    if(wspFound != std::string::npos && wspFound < dotFound){
        this->statusReason = "Invalid whitespace in HTTP Version declaration: Major Version (RFC7230 [2.6])";
        return(enums::httpStatus::BAD_REQUEST);
    }
    if(dotFound == 0 || dotFound == std::string::npos)
    {
        this->statusReason = "Invalid HTTP Version declaration (RFC7230 [2.6])";
        return(enums::httpStatus::BAD_REQUEST);
    }
    try
    {
        this->versionMajor = std::stoi(workingLine.substr(0, dotFound));
        workingLine.erase(0, dotFound + 1);
    }
    catch(const std::invalid_argument& e)
    {
        this->statusReason = "Invalid HTTP Version declaration: Major Version (RFC7230 [2.6])";
        return(enums::httpStatus::BAD_REQUEST);
    }

    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::extractMethod
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::extractMethod(std::string& workingLine)
{
    std::string wspCharsStr{" \f\n\r\t\v"};
    const char *wspChars{wspCharsStr.c_str()};

    ///////////////////////////////////////////////////////////////////////////////
    // Find first occurance of whitespace
    // If at front, return Bad Request
    // If no whitespace, return Bad Request
    // Else extract front to occurance as httpMethod
    ///////////////////////////////////////////////////////////////////////////////
    
    size_t wspFound{workingLine.find_first_of(wspChars)};
    if(wspFound == 0)
    {
        this->statusReason = "Invalid whitespace at start of request line (RFC7230 [3.1.1])";
        return(enums::httpStatus::BAD_REQUEST);
    }
    if(wspFound == std::string::npos)
    {
        this->statusReason = "No whitespace found in request line (RFC7230 [3.1.1])";
        return(enums::httpStatus::BAD_REQUEST);
    }

    this->method = enums::converter::str2method(workingLine.substr(0, wspFound));
    workingLine.erase(0, wspFound + 1);
    
    if(this->method == enums::httpMethod::ENUMERROR)
    {
        this->statusReason = "Unknown HTTP method requested (RFC7231 [4.1])";
        return(enums::httpStatus::NOT_IMPLEMENTED);
    }
    
    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::extractMinorVersion
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::extractMinorVersion(std::string& workingLine)
{
    std::string wspCharsStr{" \f\n\r\t\v"};
    const char *wspChars{wspCharsStr.c_str()};

    ///////////////////////////////////////////////////////////////////////////////
    // Find first occurance of whitespace
    // If found, return Bad Request
    ///////////////////////////////////////////////////////////////////////////////
    size_t wspFound{workingLine.find_first_of(wspChars)};
    if(wspFound == 0)
    {
        this->statusReason = "Invalid whitespace in HTTP Version declaration: Minor Version (RFC7230 [2.6])";
        return(enums::httpStatus::BAD_REQUEST);
    }
    if(wspFound != std::string::npos)
    {
        this->statusReason = "Invalid whitespace after HTTP Version (RFC7230 [3.1.1])";
        return(enums::httpStatus::BAD_REQUEST);
    }
    try
    {
        this->versionMinor = std::stoi(workingLine);
    }
    catch(const std::invalid_argument& e)
    {
        this->statusReason = "Invalid HTTP Version declaration: Minor Version (RFC7230 [2.6])";
        return(enums::httpStatus::BAD_REQUEST);
    }

    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::extractProtocol
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::extractProtocol(std::string& workingLine)
{
    std::string wspCharsStr{" \f\n\r\t\v"};
    const char *wspChars{wspCharsStr.c_str()};

    ///////////////////////////////////////////////////////////////////////////////
    // Find first occurance of whitespace
    // If at front, return Bad Request
    ///////////////////////////////////////////////////////////////////////////////
    size_t wspFound{workingLine.find_first_of(wspChars)};
    if(wspFound == 0)
    {
        this->statusReason = "Invalid whitespace between Request Target and HTTP Version (RFC7230 [3.1.1])";
        return(enums::httpStatus::BAD_REQUEST);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Find first occurance of '/'
    // If whitepace occurs before '/', return Bad Request
    // If at front, return Bad Request
    // If no '/', return Bad Request
    // Else extract front to occurance as httpProtocol
    ///////////////////////////////////////////////////////////////////////////////
    
    size_t slashFound{workingLine.find_first_of('/')};
    if(wspFound != std::string::npos && wspFound < slashFound){
        this->statusReason = "Invalid whitespace in HTTP Version declaration (RFC7230 [2.6])";
        return(enums::httpStatus::BAD_REQUEST);
    }
    if(slashFound == 0 || slashFound == std::string::npos)
    {
        this->statusReason = "Invalid HTTP Version declaration (RFC7230 [2.6])";
        return(enums::httpStatus::BAD_REQUEST);
    }

    this->protocol = enums::converter::str2protocol(workingLine.substr(0, slashFound));
    workingLine.erase(0, slashFound + 1);
    
    if(this->protocol == enums::httpProtocol::ENUMERROR)
    {
        this->statusReason = "Invalid HTTP Version declaration (RFC7230 [2.6])";
        return(enums::httpStatus::BAD_REQUEST);
    }
    
    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::extractURI
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::extractURI(std::string& workingLine)
{
    std::string wspCharsStr{" \f\n\r\t\v"};
    const char *wspChars{wspCharsStr.c_str()};

    ///////////////////////////////////////////////////////////////////////////////
    // Find first occurance of whitespace
    // If at front, return Bad Request
    // If no whitespace, return Bad Request
    // Else extract as URI string
    ///////////////////////////////////////////////////////////////////////////////
    
    size_t wspFound{workingLine.find_first_of(wspChars)};
    if(wspFound == 0)
    {
        this->statusReason = "Invalid whitespace between Method and Request Target (RFC7230 [3.1.1])";
        return(enums::httpStatus::BAD_REQUEST);
    }
    if(wspFound == std::string::npos)
    {
        this->statusReason = "No whitespace found after Request Target (RFC7230 [3.1.1])";
        return(enums::httpStatus::BAD_REQUEST);
    }

    this->originalURIString = workingLine.substr(0, wspFound);
    workingLine.erase(0, wspFound + 1);

    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::getBytesReceived
///////////////////////////////////////////////////////////////////////////////

size_t packet::requestLineParser::getBytesReceived()
{
    return(this->bytesReceived);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::getMajorVersion
///////////////////////////////////////////////////////////////////////////////

int packet::requestLineParser::getMajorVersion()
{
    return(this->versionMajor);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::getMethod
///////////////////////////////////////////////////////////////////////////////

enums::httpMethod packet::requestLineParser::getMethod()
{
    return(this->method);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::getMinorVersion
///////////////////////////////////////////////////////////////////////////////

int packet::requestLineParser::getMinorVersion()
{
    return(this->versionMinor);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::getProtocol
///////////////////////////////////////////////////////////////////////////////

enums::httpProtocol packet::requestLineParser::getProtocol()
{
    return(this->protocol);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::getRequestLineString
///////////////////////////////////////////////////////////////////////////////

std::string packet::requestLineParser::getRequestLineString()
{
    return(this->originalRequestLine);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::getStatusReason
///////////////////////////////////////////////////////////////////////////////

std::string packet::requestLineParser::getStatusReason()
{
    return(this->statusReason);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::getURI
///////////////////////////////////////////////////////////////////////////////

server::uri packet::requestLineParser::getURI()
{
    return(this->uri);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::getURIString
///////////////////////////////////////////////////////////////////////////////

std::string packet::requestLineParser::getURIString()
{
    return(this->originalURIString);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::parseAuthority
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::parseAuthority(const std::string& inString)
{
    enums::httpStatus returnStatus{enums::httpStatus::OK};
    std::string workingStr{inString};

    size_t atFound{workingStr.find("@")};
    if(atFound != std::string::npos)
    {
        std::string userInfoStr{workingStr.substr(0, atFound)};
        workingStr.erase(0, atFound + 1);
        returnStatus = this->parseUserInfo(userInfoStr); 
        if(returnStatus != enums::httpStatus::OK)
        {
            return(returnStatus);
        }
    }

    std::string hostString;
    size_t colFound{workingStr.find(":")};
    if(colFound == std::string::npos)
    {
        hostString = workingStr;
        workingStr.clear();
    }
    else
    {
        hostString = workingStr.substr(0, colFound);
        workingStr.erase(0, colFound + 1);
    }
    returnStatus = this->parseHost(hostString);
    if(returnStatus != enums::httpStatus::OK)
    {
        return(returnStatus);
    }

    if(workingStr.size() > 0)
    {
        try
        {
            int portNumber{std::stoi(workingStr)};
            this->uri.setPort(portNumber);
        }
        catch(const std::invalid_argument& e)
        {
            this->statusReason = "Invaild characters in port designation";
            return(enums::httpStatus::BAD_REQUEST);
        }        
    }

    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::parseFragments
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::parseFragments(const std::string& inString)
{
    std::unique_ptr<utility::parseUtilities> parseUtil{new utility::parseUtilities()};
    std::string decodedStr;
    std::string workingStr;

    if(!parseUtil->pctDecode(inString, decodedStr))
    {
        this->statusReason = "Disallowed characters in fragment string";
        return(enums::httpStatus::BAD_REQUEST);
    }

    parseUtil->toLower(decodedStr, workingStr);
    this->uri.setFragmentString(workingStr);

    while(workingStr.size() > 0)
    {
        std::string fragStr;
        size_t commaFound{workingStr.find_first_of(',')};
        if(commaFound != std::string::npos)
        {
            fragStr = workingStr.substr(0, commaFound);
            workingStr.erase(0, commaFound + 1);
        }
        else
        {
            fragStr = workingStr;
            workingStr.clear();
        }

        this->uri.addFragment(fragStr);
    }

    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::parseHost
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::parseHost(const std::string& inString)
{
    std::unique_ptr<utility::parseUtilities> parseUtil{new utility::parseUtilities()};
    std::string decodedStr;
    std::string workingStr;
    
    if(!parseUtil->pctDecode(inString, decodedStr))
    {
        this->statusReason = "Disallowed characters in host designation";
        return(enums::httpStatus::BAD_REQUEST);
    }

    parseUtil->toLower(decodedStr, workingStr);

    size_t IPv6Found{workingStr.find_first_of("[:]")};
    if(IPv6Found != std::string::npos)
    {
        this->uri.setHostType(enums::uriHostType::IPV6_ADDRESS);
        return(this->parseHostIP6(workingStr));
    }
    
    size_t nonIPv4Found{workingStr.find_first_not_of("0123456789.")};
    if(nonIPv4Found != std::string::npos)
    {
        this->uri.setHostType(enums::uriHostType::REGISTERED_NAME);    
        return(this->parseHostDNS(workingStr));
    }
    
    this->uri.setHostType(enums::uriHostType::IPV4_ADDRESS);
    return(this->parseHostIP4(workingStr));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::parseHostDNS
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::parseHostDNS(const std::string& inString)
{
    this->uri.setHostType(enums::uriHostType::REGISTERED_NAME);
    std::string allowedChars{"abcdefghijklmnopqrstuvwxyz0123456789-"};
    std::string workingString{inString};

    while(workingString.size() > 0)
    {
        std::string label;
        size_t dotFound{workingString.find_first_of('.')};
        if(dotFound != std::string::npos)
        {
            label = workingString.substr(0, dotFound);
            workingString.erase(0, dotFound + 1);
        }
        else
        {
            label = workingString;
            workingString.clear();
        }

        size_t disallowFound{label.find_first_not_of(allowedChars)};
        if(disallowFound != std::string::npos)
        {
            this->statusReason = "Disallowed characters in registered name";
            return(enums::httpStatus::BAD_REQUEST);
        }
        if(label[0] < 'a' || label[0] > 'z'){
            this->statusReason = "Registered name labels must start with a letter";
            return(enums::httpStatus::BAD_REQUEST);
        }
        if(label[label.size() - 1] == '-' || (dotFound != std::string::npos && workingString.size() == 0))
        {
            this->statusReason = "Registered name labels must end with a letter or number";
            return(enums::httpStatus::BAD_REQUEST);
        }
    }
    this->uri.setHost(inString);
    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::parseHostIP4
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::parseHostIP4(const std::string& inString)
{
    this->uri.setHostType(enums::uriHostType::IPV4_ADDRESS);
    std::string allowedChars{"0123456789"};
    std::string workingString{inString};

    for(int index = 0; index < 3; index++)
    {
        size_t dotFound{workingString.find_first_of('.')};
        if(dotFound != std::string::npos)
        {
            std::string octet{workingString.substr(0, dotFound)};
            workingString.erase(0, dotFound + 1);
            try
            {
                int octetValue{std::stoi(octet)};
                if(octetValue < 0 || octetValue > 255)
                {
                    this->statusReason = "Invalid octect value in IP address.  Must be between 0 and 255";
                    return(enums::httpStatus::BAD_REQUEST);
                }
            }
            catch(const std::invalid_argument& e)
            {
                this->statusReason = "Invalid character in IP Address";
                return(enums::httpStatus::BAD_REQUEST);
            }
        }
        else
        {
            this->statusReason = "IP Address missing octet";
            return(enums::httpStatus::BAD_REQUEST);
        }
    }
    if(workingString.size() == 0)
    {
        this->statusReason = "IP Address missing octet";
        return(enums::httpStatus::BAD_REQUEST);
    }
    try
    {   
        int octetValue{std::stoi(workingString)};
        if(octetValue < 0 || octetValue > 255)
        {
            this->statusReason = "Invalid octect value in IP address.  Must be between 0 and 255";
            return(enums::httpStatus::BAD_REQUEST);
        }
    }
    catch(const std::invalid_argument& e)
    {
        this->statusReason = "Invalid character in IP Address";
        return(enums::httpStatus::BAD_REQUEST);
    }
    this->uri.setHost(inString);
    return(enums::httpStatus::OK);    
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::parseHostIP6
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::parseHostIP6(const std::string& inString)
{
    std::string allowedChars{"abcdef0123456789:"};
    std::string workingStr{inString};
    
    size_t oBracketFound{workingStr.find_first_of('[')};
    if(oBracketFound != 0)
    {
        this->statusReason = "Invalid character in IPv6 Literal";
        return(enums::httpStatus::BAD_REQUEST);
    }
    workingStr.erase(0, 1);

    size_t cBracketFound{workingStr.find_first_of(']')};
    if(cBracketFound != (workingStr.size() - 1))
    {
        this->statusReason = "Invalid character in IPv6 Literal";
        return(enums::httpStatus::BAD_REQUEST);
    }
    workingStr.erase(cBracketFound);

    size_t disallowFound{workingStr.find_first_not_of(allowedChars)};
    if(disallowFound != std::string::npos)
    {
        this->statusReason = "Invalid character in IPv6 Literal";
        return(enums::httpStatus::BAD_REQUEST);
    }

    int elideCount{0};
    int segCount{0};
    int segMax{8};
    size_t colFound{workingStr.find_first_of(":")};
    if(colFound == std::string::npos)
    {
        this->statusReason = "Missing segments in IPv6 Literal";
        return(enums::httpStatus::BAD_REQUEST);
    }
    if(colFound == 0)
    {
        if(workingStr[1] != ':')
        {
            this->statusReason = "Invalid segment in IPv6 Literal";
            return(enums::httpStatus::BAD_REQUEST);
        }
        else
        {
            elideCount++;
            segMax--;
            workingStr.erase(0, 2);
        }        
    }
    while(workingStr.size() > 0)
    {
        colFound = workingStr.find_first_of(":");
        if(colFound != std::string::npos)
        {
            if(colFound == 0)
            {
                if(elideCount == 1)
                {
                    this->statusReason = "Invalid elision in IPv6 Literal";
                    return(enums::httpStatus::BAD_REQUEST); 
                }
                else
                {
                    elideCount++;
                    segMax--;
                    workingStr.erase(0, 1);
                }
            }
            else 
            {
                std::string h16{workingStr.substr(0,colFound)};
                workingStr.erase(0, colFound + 1);
                if(h16.size() < 1 || h16.size() > 4)
                {
                    this->statusReason = "Invalid segment value in IPv6 Literal";
                    return(enums::httpStatus::BAD_REQUEST);
                }
                segCount++;
            }
        }
        else
        {
            std::string h16{workingStr};
            workingStr.clear();
            if(h16.size() < 1 || h16.size() > 4)
            {
                this->statusReason = "Invalid segment value in IPv6 Literal";
                return(enums::httpStatus::BAD_REQUEST);
            }
            segCount++;
        }
    }
    if(segCount > segMax)
    {
        this->statusReason = "Additional segments in IPv6 Literal";
        return(enums::httpStatus::BAD_REQUEST);
    }
    
    this->uri.setHost(inString);
    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::parseParameters
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::parseParameters(const std::string& inString)
{
    std::unique_ptr<utility::parseUtilities> parseUtil{new utility::parseUtilities()};
    std::string decodedStr;
    std::string workingStr;

    if(!parseUtil->pctDecode(inString, decodedStr))
    {
        this->statusReason = "Disallowed characters in query string";
        return(enums::httpStatus::BAD_REQUEST);
    }

    parseUtil->toLower(decodedStr, workingStr);
    this->uri.setParameterString(workingStr);

    while(workingStr.size() > 0)
    {
        std::string queryStr;
        size_t ampFound{workingStr.find_first_of('&')};
        if(ampFound != std::string::npos)
        {
            queryStr = workingStr.substr(0, ampFound);
            workingStr.erase(0, ampFound + 1);
        }
        else
        {
            queryStr = workingStr;
            workingStr.clear();
        }

        size_t eqFound{queryStr.find_first_of('=')};
        if(eqFound == std::string::npos)
        {
            this->statusReason = "Invalid query paramter declaration";
            return(enums::httpStatus::BAD_REQUEST);
        }
        else
        {
            std::string qName{queryStr.substr(0, eqFound)};
            std::string qValue{queryStr.substr(eqFound + 1)};
            this->uri.addParameter(qName, qValue);
        }        
    }

    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::parsePath
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::parsePath(const std::string& inString)
{
    std::unique_ptr<utility::parseUtilities> parseUtil{new utility::parseUtilities()};
    this->uri.addSegment("RESOURCE_ROOT");
    std::string workingStr{inString};
    size_t slashFound{workingStr.find_first_of('/')};
    if(slashFound == 0)
    {
        workingStr.erase(0,1);
    }

    while(workingStr.size() > 0)
    {
        slashFound = workingStr.find_first_of('/');
        if(slashFound != std::string::npos)
        {
            if(slashFound == 0)
            {
                this->statusReason = "Invalid path segment";
                return(enums::httpStatus::BAD_REQUEST);
            }
            else
            {
                std::string encodedSegment{workingStr.substr(0, slashFound)};
                workingStr.erase(0, slashFound + 1);
                std::string decodedSegment;
                std::string segment;
                if(!parseUtil->pctDecode(encodedSegment, decodedSegment))
                {
                    this->statusReason = "Disallowed characters in path segment";
                    return(enums::httpStatus::BAD_REQUEST);
                }
                parseUtil->toLower(decodedSegment, segment);
                this->uri.addSegment(segment);
            }
        }
        else
        {
            std::string encodedSegment{workingStr};
            workingStr.clear();
            std::string decodedSegment;
            std::string segment;
            if(!parseUtil->pctDecode(encodedSegment, decodedSegment))
            {
                this->statusReason = "Disallowed characters in path segment";
                return(enums::httpStatus::BAD_REQUEST);
            }
            parseUtil->toLower(decodedSegment, segment);
            this->uri.addSegment(segment);
        }
    }
    
    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::parseScheme
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::parseScheme(const std::string& inString)
{
    
    std::string workingStr{inString};

    size_t colFound{workingStr.find(":")};
    if(colFound == std::string::npos)
    {
        this->statusReason = "Invalid URI Formation: Malformed scheme. Missing colon (RFC3986 [3])";
        return(enums::httpStatus::BAD_REQUEST);
    }

    enums::uriScheme scheme{enums::converter::str2scheme(workingStr.substr(0, colFound))};

    if(scheme == enums::uriScheme::ENUMERROR)
    {
        this->statusReason = "Unreckongnized URI scheme (RFC7230 [2.7])";
        return(enums::httpStatus::BAD_REQUEST);
    }
    
    this->uri.setScheme(scheme);
    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::parseURI
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::parseURI()
{
    enums::httpStatus returnStatus{enums::httpStatus::OK};
    this->uri.setURIString(this->originalURIString);
    std::string workingStr{this->originalURIString};
    
    ///////////////////////////////////////////////////////////////////////////////
    // Determine if scheme and authority are present
    // by looking for the "//" substring in the URI
    // if no "//" then scheme, userinfo, host, and port
    // cannot not be present in URI
    ///////////////////////////////////////////////////////////////////////////////

    size_t slash2Found{workingStr.find("//")};
    if(slash2Found != std::string::npos)
    {
        ///////////////////////////////////////////////////////////////////////////////
        //  "//" found... so we must have scheme and authority components
        ///////////////////////////////////////////////////////////////////////////////
            
        std::string schemeStr{workingStr.substr(0,slash2Found)};
        workingStr.erase(0, slash2Found + 2);
        returnStatus = this->parseScheme(schemeStr); 
        if(returnStatus != enums::httpStatus::OK)
        {
            return(returnStatus);
        }
        
        ///////////////////////////////////////////////////////////////////////////////
        // successfully pulled out scheme component
        // pull/process full authority component (userinfo, host, port)
        ///////////////////////////////////////////////////////////////////////////////
        
        std::string authString;
        size_t slashFound{workingStr.find_first_of('/')};
        if(slashFound == std::string::npos)
        {
            authString = workingStr;
            workingStr.clear();
        }
        else
        {
            authString = workingStr.substr(0, slashFound);
            workingStr.erase(0, slashFound);
        }
        
        returnStatus = this->parseAuthority(authString);
        if(returnStatus != enums::httpStatus::OK)
        {
            return(returnStatus);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // pull/process full path component
    ///////////////////////////////////////////////////////////////////////////////

    std::string pathString;
    size_t queFound{workingStr.find_first_of('?')};
    if(queFound == std::string::npos)
    {
        size_t hashFound{workingStr.find_first_of('#')};
        if(hashFound == std::string::npos)
        {
            pathString = workingStr;
            workingStr.clear();
        }
        else
        {
            pathString = workingStr.substr(0, hashFound);
            workingStr.erase(0, hashFound);
        }
    }
    else
    {
        pathString = workingStr.substr(0, queFound);
        workingStr.erase(0, queFound);
    }
       
    returnStatus = this->parsePath(pathString);
    if(returnStatus != enums::httpStatus::OK)
    {
        return(returnStatus);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // successfully pulled out path component
    // if queryCompExists pull/process query component
    ///////////////////////////////////////////////////////////////////////////////

    queFound = workingStr.find_first_of('?');
    if(queFound != std::string::npos)
    {
        if(queFound == 0)
        {
            workingStr.erase(0, 1);
        }
        else
        {
            this->statusReason = "Error parsing path segments";
            return(enums::httpStatus::INTERNAL_SERVER_ERROR);
        }

        std::string queryString;
        size_t hashFound{workingStr.find_first_of('#')};
        if(hashFound == std::string::npos)
        {
            queryString = workingStr;
            workingStr.clear();
        }
        else
        {
            queryString = workingStr.substr(0, hashFound);
            workingStr.erase(0, hashFound);
        }
        
        returnStatus = this->parseParameters(queryString);
        if(returnStatus != enums::httpStatus::OK)
        {
            return(returnStatus);
        }        
    }

    ///////////////////////////////////////////////////////////////////////////////
    // pull/process full path component
    ///////////////////////////////////////////////////////////////////////////////

    size_t hashFound{workingStr.find_first_of('#')};
    if(hashFound != std::string::npos)
    {
        if(hashFound == 0)
        {
            workingStr.erase(0, 1);
        }
        else
        {
            this->statusReason = "Error parsing query string";
            return(enums::httpStatus::INTERNAL_SERVER_ERROR);
        }

        returnStatus = this->parseFragments(workingStr);
        if(returnStatus != enums::httpStatus::OK)
        {
            return(returnStatus);
        } 
    }

    return(enums::httpStatus::OK);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpRequest::requestLineParser::parseUserInfo
///////////////////////////////////////////////////////////////////////////////

enums::httpStatus packet::requestLineParser::parseUserInfo(const std::string& inString)
{
    
    std::unique_ptr<utility::parseUtilities> parseUtil{new utility::parseUtilities()}; 
    std::string workingStr{inString};
    std::string encodedUserStr;
    std::string lowerUserStr;
    std::string encodedPassStr;
    std::string userString;
    std::string passString;

    size_t colFound = workingStr.find(":");
    if(colFound == std::string::npos)
    {
        encodedUserStr = workingStr;
        if(!parseUtil->pctDecode(encodedUserStr, userString))
        {
            this->statusReason = "Disallowed characters in username";
            return(enums::httpStatus::BAD_REQUEST);
        }

        this->uri.setUsername(userString);
    }
    else
    {
        encodedUserStr = workingStr.substr(0, colFound);
        if(!parseUtil->pctDecode(encodedUserStr, userString))
        {
            this->statusReason = "Disallowed characters in username";
            return(enums::httpStatus::BAD_REQUEST);
        }
        parseUtil->toLower(userString, lowerUserStr);
        this->uri.setUsername(lowerUserStr);
        
        encodedPassStr = workingStr.substr(colFound + 1);
        if(!parseUtil->pctDecode(encodedPassStr, passString))
        {
            this->statusReason = "Disallowed characters in password";
            return(enums::httpStatus::BAD_REQUEST);
        }
        this->uri.setPassword(passString);
    }

    return(enums::httpStatus::OK);
}


///////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////

packet::requestLineParser::~requestLineParser()
{
    this->buffer = nullptr;
}