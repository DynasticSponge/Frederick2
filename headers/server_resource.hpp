//
// server_resource.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SERVER_RESOURCE_HPP
#define SERVER_RESOURCE_HPP

#include <string>

#include "frederick2_namespace.hpp"

class frederick2::httpServer::resource
{
public:
    frederick2::httpServer::resource* addChild(const std::string&, frederick2::httpEnums::resourceType);
    void addHandler(frederick2::httpEnums::httpMethod, httpHandler);
    frederick2::httpServer::resource* getChild(const std::string&);
    handlerCheck getHandler(frederick2::httpEnums::httpMethod);
    std::string getMethodList();
    std::string getName();
    frederick2::httpEnums::resourceType getType();
    void removeChild(const std::string&);
    ~resource();
protected:
private:
    ///////////////////////////////////////////////////////////////////////////////
    // Friend Declarations
    ///////////////////////////////////////////////////////////////////////////////
    friend class frederick2::httpServer::httpServer;
    ///////////////////////////////////////////////////////////////////////////////
    // Private Functions
    ///////////////////////////////////////////////////////////////////////////////
    explicit resource(const std::string&, frederick2::httpEnums::resourceType);
    ///////////////////////////////////////////////////////////////////////////////
    // Private Properties
    ///////////////////////////////////////////////////////////////////////////////
    bool hasDynamic;
    bool hasFilesystem;
    std::string name;
    strMAPresource children;
    methodMAPhandler handlers;
    frederick2::httpEnums::resourceType type;
};

#endif