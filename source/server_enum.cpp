//
// server_enum.cpp
// ~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>

#include "../headers/frederick2_namespace.hpp"
#include "../headers/server_enum.hpp"

namespace enums = frederick2::httpEnums;

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
// frederick2::httpEnums::converter::initValidMethods
///////////////////////////////////////////////////////////////////////////////

void enums::converter::initValidMethods()
{
    enums::converter::validMethods.insert({"CONNECT", enums::httpMethod::CONNECT});
    enums::converter::validMethods.insert({"DELETE", enums::httpMethod::DELETE});
    enums::converter::validMethods.insert({"GET", enums::httpMethod::GET});
    enums::converter::validMethods.insert({"HEAD", enums::httpMethod::HEAD});
    enums::converter::validMethods.insert({"OPTIONS", enums::httpMethod::OPTIONS});
    enums::converter::validMethods.insert({"PATCH", enums::httpMethod::PATCH});
    enums::converter::validMethods.insert({"POST", enums::httpMethod::POST});
    enums::converter::validMethods.insert({"PUT", enums::httpMethod::PUT});
    enums::converter::validMethods.insert({"TRACE", enums::httpMethod::TRACE});
    enums::converter::isInitVM = true;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpEnums::converter::initValidMethodStrings
///////////////////////////////////////////////////////////////////////////////

void enums::converter::initValidMethodStrings()
{
    enums::converter::validMethodStrings.insert({enums::httpMethod::CONNECT, "CONNECT"});
    enums::converter::validMethodStrings.insert({enums::httpMethod::DELETE, "DELETE"});
    enums::converter::validMethodStrings.insert({enums::httpMethod::GET, "GET"});
    enums::converter::validMethodStrings.insert({enums::httpMethod::HEAD, "HEAD"});
    enums::converter::validMethodStrings.insert({enums::httpMethod::OPTIONS, "OPTIONS"});
    enums::converter::validMethodStrings.insert({enums::httpMethod::PATCH, "PATCH"});
    enums::converter::validMethodStrings.insert({enums::httpMethod::POST, "POST"});
    enums::converter::validMethodStrings.insert({enums::httpMethod::PUT, "PUT"});
    enums::converter::validMethodStrings.insert({enums::httpMethod::TRACE, "TRACE"});
    enums::converter::isInitVMS = true;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpEnums::converter::initValidProtocols
///////////////////////////////////////////////////////////////////////////////

void enums::converter::initValidProtocols()
{
    enums::converter::validProtocols.insert({"HTTP", enums::httpProtocol::HTTP});
    enums::converter::isInitVP = true;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpEnums::converter::initValidProtocolStrings
///////////////////////////////////////////////////////////////////////////////

void enums::converter::initValidProtocolStrings()
{
    enums::converter::validProtocolStrings.insert({enums::httpProtocol::HTTP, "HTTP"});
    enums::converter::isInitVPS = true;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpEnums::converter::initValidSchemes
///////////////////////////////////////////////////////////////////////////////

void enums::converter::initValidSchemes()
{
    enums::converter::validSchemes.insert({"http", enums::uriScheme::http});
    enums::converter::validSchemes.insert({"https", enums::uriScheme::https});
    enums::converter::isInitVS = true;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpEnums::converter::initValidStatusStrings
///////////////////////////////////////////////////////////////////////////////

void enums::converter::initValidStatusStrings()
{
    enums::converter::validStatusStrings.insert({enums::httpStatus::CONTINUE, "CONTINUE"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::SWITCHING_PROTOCOLS, "SWITCHING_PROTOCOLS"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::PROCESSING, "PROCESSING"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::EARLY_HINTS, "EARLY_HINTS"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::OK, "OK"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::CREATED, "CREATED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::ACCEPTED, "ACCEPTED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::NON_AUTHORITATIVE_INFORMATION, "NON_AUTHORITATIVE_INFORMATION"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::NO_CONTENT, "NO_CONTENT"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::RESET_CONTENT, "RESET_CONTENT"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::PARTIAL_CONTENT, "PARTIAL_CONTENT"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::MULTI_STATUS, "MULTI_STATUS"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::ALREADY_REPORTED, "ALREADY_REPORTED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::IM_USED, "IM_USED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::MULTIPLE_CHOICES, "MULTIPLE_CHOICES"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::MOVED_PERMANENTLY, "MOVED_PERMANENTLY"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::FOUND, "FOUND"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::SEE_OTHER, "SEE_OTHER"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::NOT_MODIFIED, "NOT_MODIFIED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::USE_PROXY, "USE_PROXY"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::SWITCH_PROXY, "SWITCH_PROXY"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::TEMPORARY_REDIRECT, "TEMPORARY_REDIRECT"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::PERMANENT_REDIRECT, "PERMANENT_REDIRECT"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::BAD_REQUEST, "BAD_REQUEST"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::UNAUTHORIZED, "UNAUTHORIZED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::FORBIDDEN, "FORBIDDEN"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::NOT_FOUND, "NOT_FOUND"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::METHOD_NOT_ALLOWED, "METHOD_NOT_ALLOWED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::NOT_ACCEPTABLE, "NOT_ACCEPTABLE"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::PROXY_AUTHENTICATION_REQUIRED, "PROXY_AUTHENTICATION_REQUIRED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::REQUEST_TIMEOUT, "REQUEST_TIMEOUT"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::CONFLICT, "CONFLICT"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::GONE, "GONE"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::LENGTH_REQUIRED, "LENGTH_REQUIRED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::PRECONDITION_FAILED, "PRECONDITION_FAILED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::PAYLOAD_TOO_LARGE, "PAYLOAD_TOO_LARGE"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::URI_TOO_LONG, "URI_TOO_LONG"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::UNSUPPORTED_MEDIA_TYPE, "UNSUPPORTED_MEDIA_TYPE"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::RANGE_NOT_SATISFIABLE, "RANGE_NOT_SATISFIABLE"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::EXPECTATION_FAILED, "EXPECTATION_FAILED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::IM_A_TEAPOT, "IM_A_TEAPOT"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::MISDIRECTED_REQUEST, "MISDIRECTED_REQUEST"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::UNPROCESSABLE_ENTITY, "UNPROCESSABLE_ENTITY"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::LOCKED, "LOCKED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::FAILED_DEPENDENCY, "FAILED_DEPENDENCY"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::TOO_EARLY, "TOO_EARLY"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::UPGRADE_REQUIRED, "UPGRADE_REQUIRED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::PRECONDITION_REQUIRED, "PRECONDITION_REQUIRED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::TOO_MANY_REQUESTS, "TOO_MANY_REQUESTS"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::REQUEST_HEADER_FIELDS_TOO_LARGE, "REQUEST_HEADER_FIELDS_TOO_LARGE"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::UNAVAILABLE_FOR_LEGAL_REASONS, "UNAVAILABLE_FOR_LEGAL_REASONS"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::INTERNAL_SERVER_ERROR, "INTERNAL_SERVER_ERROR"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::NOT_IMPLEMENTED, "NOT_IMPLEMENTED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::BAD_GATEWAY, "BAD_GATEWAY"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::SERVICE_UNAVAILABLE, "SERVICE_UNAVAILABLE"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::GATEWAY_TIMEOUT, "GATEWAY_TIMEOUT"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::HTTP_VERSION_NOT_SUPPORTED, "HTTP_VERSION_NOT_SUPPORTED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::VARIANT_ALSO_NEGOTIATES, "VARIANT_ALSO_NEGOTIATES"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::INSUFFICIENT_STORAGE, "INSUFFICIENT_STORAGE"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::LOOP_DETECTED, "LOOP_DETECTED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::NOT_EXTENDED, "NOT_EXTENDED"});
    enums::converter::validStatusStrings.insert({enums::httpStatus::NETWORK_AUTHENTICATION_REQUIRED, "NETWORK_AUTHENTICATION_REQUIRED"});
    enums::converter::isInitVSS = true;
    return;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpEnums::converter::method2str
///////////////////////////////////////////////////////////////////////////////

std::string enums::converter::method2str(enums::httpMethod method)
{
    if(!enums::converter::isInitVMS)
    {
        enums::converter::initValidMethodStrings();
    }
    return(enums::converter::validMethodStrings[method]);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpEnums::converter::protocol2str
///////////////////////////////////////////////////////////////////////////////

std::string enums::converter::protocol2str(enums::httpProtocol protocol)
{
    if(!enums::converter::isInitVPS)
    {
        enums::converter::initValidProtocolStrings();
    }
    return(enums::converter::validProtocolStrings[protocol]);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpEnums::converter::status2str
///////////////////////////////////////////////////////////////////////////////

std::string enums::converter::status2str(enums::httpStatus status)
{
    if(!enums::converter::isInitVSS)
    {
        enums::converter::initValidStatusStrings();
    }
    return(enums::converter::validStatusStrings[status]);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpEnums::converter::str2method
///////////////////////////////////////////////////////////////////////////////

enums::httpMethod enums::converter::str2method(const std::string& methodString)
{
    enums::httpMethod returnMethod{};
    if(!enums::converter::isInitVM)
    {
        enums::converter::initValidMethods();
    }
    try
    {
        returnMethod = enums::converter::validMethods.at(methodString);
    }
    catch(const std::out_of_range& e)
    {
        returnMethod = enums::httpMethod::ENUMERROR;
    }
    return(returnMethod);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpEnums::converter::str2protocol
///////////////////////////////////////////////////////////////////////////////

enums::httpProtocol enums::converter::str2protocol(const std::string& protocolString)
{
    enums::httpProtocol returnProtocol;
    if(!enums::converter::isInitVP)
    {
        enums::converter::initValidProtocols();
    }
    try
    {
        returnProtocol = enums::converter::validProtocols.at(protocolString);
    }
    catch(const std::exception& e)
    {
        returnProtocol = enums::httpProtocol::ENUMERROR;
    }    
    return(returnProtocol);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpEnums::converter::str2scheme
///////////////////////////////////////////////////////////////////////////////

enums::uriScheme enums::converter::str2scheme(const std::string& schemeString)
{
    enums::uriScheme returnScheme;
    if(!enums::converter::isInitVS)
    {
        enums::converter::initValidSchemes();
    }
    try
    {
        returnScheme = enums::converter::validSchemes.at(schemeString);
    }
    catch(const std::exception& e)
    {
        returnScheme = enums::uriScheme::ENUMERROR;
    }    
    return(returnScheme);
}
