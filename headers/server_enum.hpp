//
// server_enum.hpp
// ~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SERVER_ENUM_HPP
#define SERVER_ENUM_HPP

#include "frederick2_namespace.hpp"

enum class frederick2::httpEnums::httpMethod
{   
    ENUMERROR,
    CONNECT,
    DELETE,
    GET,
    HEAD,
    OPTIONS,
    PATCH,
    POST,
    PUT,
    TRACE
};

enum class frederick2::httpEnums::httpProtocol
{
    ENUMERROR,
    HTTP
};

enum class frederick2::httpEnums::httpStatus
{
    ENUMERROR = -1,
    ///////////////////////////////////////////////////////////////////////////////    
    // INFORMATIONAL_RESPONSE
    ///////////////////////////////////////////////////////////////////////////////
    CONTINUE = 100,
    SWITCHING_PROTOCOLS = 101,
    PROCESSING = 102,
    EARLY_HINTS = 103,
    ///////////////////////////////////////////////////////////////////////////////    
    // SUCCESS
    ///////////////////////////////////////////////////////////////////////////////
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NON_AUTHORITATIVE_INFORMATION = 203,
    NO_CONTENT = 204,
    RESET_CONTENT = 205,
    PARTIAL_CONTENT = 206,
    MULTI_STATUS = 207,
    ALREADY_REPORTED = 208,
    IM_USED = 226,   
    ///////////////////////////////////////////////////////////////////////////////    
    // REDIRECTION
    ///////////////////////////////////////////////////////////////////////////////    
    MULTIPLE_CHOICES = 300,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OTHER = 303,
    NOT_MODIFIED = 304,
    USE_PROXY = 305,
    SWITCH_PROXY = 306,
    TEMPORARY_REDIRECT = 307,
    PERMANENT_REDIRECT = 308,
    ///////////////////////////////////////////////////////////////////////////////    
    // CLIENT ERRORS
    ///////////////////////////////////////////////////////////////////////////////    
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    NOT_ACCEPTABLE = 406,
    PROXY_AUTHENTICATION_REQUIRED = 407,
    REQUEST_TIMEOUT = 408,
    CONFLICT = 409,
    GONE = 410,
    LENGTH_REQUIRED = 411,
    PRECONDITION_FAILED = 412,
    PAYLOAD_TOO_LARGE = 413,
    URI_TOO_LONG = 414,
    UNSUPPORTED_MEDIA_TYPE = 415,
    RANGE_NOT_SATISFIABLE = 416,
    EXPECTATION_FAILED = 417,
    IM_A_TEAPOT = 418,
    MISDIRECTED_REQUEST = 421,
    UNPROCESSABLE_ENTITY = 422,
    LOCKED = 423,
    FAILED_DEPENDENCY = 424,
    TOO_EARLY = 425,
    UPGRADE_REQUIRED = 426,
    PRECONDITION_REQUIRED = 428,
    TOO_MANY_REQUESTS = 429,
    REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
    UNAVAILABLE_FOR_LEGAL_REASONS = 451,
    ///////////////////////////////////////////////////////////////////////////////    
    // SERVER ERRORS
    ///////////////////////////////////////////////////////////////////////////////    
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
    GATEWAY_TIMEOUT = 504,
    HTTP_VERSION_NOT_SUPPORTED = 505,
    VARIANT_ALSO_NEGOTIATES = 506,
    INSUFFICIENT_STORAGE = 507,
    LOOP_DETECTED = 508,
    NOT_EXTENDED = 510,
    NETWORK_AUTHENTICATION_REQUIRED = 511    
};

enum class frederick2::httpEnums::resourceType
{
    ENUMERROR,
    DYNAMIC,
    FILESYSTEM,
    STATIC
};

enum class frederick2::httpEnums::uriHostType
{
    ENUMERROR,
    IPV4_ADDRESS,
    IPV6_ADDRESS,
    REGISTERED_NAME
};

enum class frederick2::httpEnums::uriScheme
{
    ENUMERROR,
    http,
    https
};

class frederick2::httpEnums::converter
{
public:
    converter();
    static std::string method2str(frederick2::httpEnums::httpMethod);
    static std::string protocol2str(frederick2::httpEnums::httpProtocol);
    static std::string status2str(frederick2::httpEnums::httpStatus);
    static frederick2::httpEnums::httpMethod str2method(const std::string&);
    static frederick2::httpEnums::httpProtocol str2protocol(const std::string&);
    static frederick2::httpEnums::uriScheme str2scheme(const std::string&);
    ~converter();
protected:
private:
    ///////////////////////////////////////////////////////////////////////////////
    // Private Functions
    ///////////////////////////////////////////////////////////////////////////////
    static void initValidMethods();
    static void initValidProtocols();
    static void initValidSchemes();
    static void initValidMethodStrings();
    static void initValidProtocolStrings();
    static void initValidStatusStrings();
    ///////////////////////////////////////////////////////////////////////////////
    // Private Properties
    ///////////////////////////////////////////////////////////////////////////////
    static inline bool isInitVM = false;
    static inline bool isInitVP = false;
    static inline bool isInitVS = false;
    static inline bool isInitVMS = false;
    static inline bool isInitVPS = false;
    static inline bool isInitVSS = false;
    static inline strMAPmethod validMethods;
    static inline strMAPprotocol validProtocols;
    static inline strMAPscheme validSchemes;
    static inline methodMAPstr validMethodStrings;
    static inline protocolMAPstr validProtocolStrings;
    static inline statusMAPstr validStatusStrings;
};

#endif