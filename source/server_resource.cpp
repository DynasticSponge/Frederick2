//
// server_resource.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <functional>
#include <map>
#include <string>

#include "../headers/frederick2_namespace.hpp"
#include "../headers/server_enum.hpp"
#include "../headers/server_resource.hpp"

namespace enums = frederick2::httpEnums;
namespace server = frederick2::httpServer;

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

server::resource::resource(const std::string& targetName, enums::resourceType targetType)
{
    this->hasDynamic = false;
    this->hasFilesystem = false;
    this->name = targetName;
    this->type = targetType;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::resource::addChild
///////////////////////////////////////////////////////////////////////////////

server::resource* server::resource::addChild(const std::string& targetName, enums::resourceType targetType)
{   
    server::resource *newResource{nullptr};
    
    if(this->type == enums::resourceType::FILESYSTEM)
    {
        return(newResource);
    }

    if(targetType == enums::resourceType::DYNAMIC){
        if(this->hasDynamic)
        {
            return(newResource);
        }
        newResource = new server::resource(targetName, targetType);
        this->children["@@DYNAMIC"] = newResource;
        this->hasDynamic = true;
    }
    else if (targetType == enums::resourceType::FILESYSTEM)
    {
        if(this->hasFilesystem)
        {
            return(newResource);
        }
        newResource = new server::resource(targetName, targetType);
        this->children[targetName] = newResource;
        this->hasFilesystem = true;
    }
    else
    {
        newResource = new server::resource(targetName, targetType);
        this->children[targetName] = newResource;
    }
    
    return(newResource);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::resource::addHandler
///////////////////////////////////////////////////////////////////////////////

void server::resource::addHandler(enums::httpMethod targetMethod, httpHandler targetFunction)
{
    this->handlers[targetMethod] = targetFunction;
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::resource::getChild
///////////////////////////////////////////////////////////////////////////////

server::resource* server::resource::getChild(const std::string& targetName)
{
    server::resource *returnResource{nullptr};
    
    try
    {
        returnResource = this->children.at(targetName);
        
    }
    catch(const std::out_of_range& e)
    {
        if(this->hasDynamic)
        {
            returnResource = this->children.at("@@DYNAMIC");
        }
    }

    return(returnResource); 
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::resource::getHandler
///////////////////////////////////////////////////////////////////////////////

handlerCheck server::resource::getHandler(enums::httpMethod targetMethod)
{
    handlerCheck returnCheck;
    returnCheck.first = true;
    try
    {
        returnCheck.second = this->handlers.at(targetMethod);
    }
    catch(const std::out_of_range& e)
    {
        returnCheck.first = false;
    }
    return(std::move(returnCheck));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::resource::getMethodList
///////////////////////////////////////////////////////////////////////////////

std::string server::resource::getMethodList()
{
    std::string returnString;
    for (auto it = this->handlers.begin(); it != this->handlers.end(); it++)
    {
        if(returnString.size() > 0)
        {
            returnString.append(", ");
        }
        returnString.append(enums::converter::method2str(it->first));
    }
    return(returnString);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::resource::getType
///////////////////////////////////////////////////////////////////////////////

enums::resourceType server::resource::getType()
{
    return(this->type);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::resource::getName
///////////////////////////////////////////////////////////////////////////////

std::string server::resource::getName()
{
    return(this->name);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::httpServer::resource::removeChild
///////////////////////////////////////////////////////////////////////////////

void server::resource::removeChild(const std::string& targetName)
{
    server::resource *targetResource{nullptr};
    try
    {
        targetResource = this->children.at(targetName);
        enums::resourceType targetType = targetResource->getType();
        delete targetResource;
        this->children.erase(targetName);
        if(targetType == enums::resourceType::FILESYSTEM)
        {
            this->hasFilesystem = false;
        }        
    }
    catch(const std::out_of_range& e)
    {
        if(this->hasDynamic)
        {
            targetResource = this->children.at("@@DYNAMIC");
            if(targetResource->getName() == targetName)
            {
                delete targetResource;
                this->children.erase("@@DYNAMIC");
                this->hasDynamic = false;
            }
        }
    }

    return;
}

///////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////

server::resource::~resource()
{    
    for (auto iter = this->children.begin(); iter != this->children.end(); iter++)
    {
        server::resource *resourcePtr{iter->second};
        delete resourcePtr;
    }
    this->children.clear();
}