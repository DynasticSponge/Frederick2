//
// frederick2_namespace.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FREDERICK2_NAMESPACE_HPP
#define FREDERICK2_NAMESPACE_HPP

#include <functional>
#include <map>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Namespace Type Declarations
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace frederick2
{
    namespace httpEnums
    {
        class converter;
        enum class httpHeader;
        enum class httpMethod;
        enum class httpProtocol;
        enum class httpStatus;
        enum class resourceType;
        enum class uriHostType;
        enum class uriScheme;
    }

    namespace httpPacket
    {
        class contentParser;
        class headerParser;
        class headerLineParser;
        class httpRequest;
        class httpResponse;
        class requestLineParser;
    }

    namespace httpServer
    {
        class connection;
        class httpServer;
        class resource;
        class socket;
        class uri;           
    }

    namespace utility
    {
        class bufferStringReader;
        class parseUtilities;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Type Declarations
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::function<void(frederick2::httpPacket::httpRequest*, frederick2::httpPacket::httpResponse*)> httpHandler;
typedef std::map<frederick2::httpEnums::httpMethod, httpHandler> methodMAPhandler;
typedef std::map<frederick2::httpEnums::httpMethod, std::string> methodMAPstr;
typedef std::map<frederick2::httpEnums::httpProtocol, std::string> protocolMAPstr;
typedef std::map<frederick2::httpEnums::httpStatus, std::string> statusMAPstr;
typedef std::map<std::string, frederick2::httpEnums::httpMethod> strMAPmethod;
typedef std::map<std::string, frederick2::httpEnums::httpProtocol> strMAPprotocol;
typedef std::map<std::string, frederick2::httpServer::resource*> strMAPresource;
typedef std::map<std::string, frederick2::httpEnums::uriScheme> strMAPscheme;
typedef std::map<std::string, std::string> strMAPstr;
typedef std::pair<bool, httpHandler> handlerCheck;
typedef std::vector<std::string> strVECTOR;

#endif