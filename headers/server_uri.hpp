//
// server_uri.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SERVER_URI_HPP
#define SERVER_URI_HPP

#include <deque>
#include <map>
#include <string>
#include <vector>

#include "frederick2_namespace.hpp"
#include "server_enum.hpp"

class frederick2::httpServer::uri
{
public:
    uri();
    std::vector<std::string> getFragments();
    std::string getFragmentString();
    strMAPstr getParameters();
    std::string getParameterString();
    std::string getHost();
    frederick2::httpEnums::uriHostType getHostType();
    std::string getPassword();
    std::string getPathString();
    int getPort();
    frederick2::httpEnums::uriScheme getScheme();
    std::deque<std::string> getSegments();
    std::string getURIString();
    std::string getUsername();
    bool hasFragments();
    bool hasHost();
    bool hasParameters();
    bool hasPath();
    bool hasPort();
    bool hasScheme();
    bool hasUserInfo();
    void addFragment(const std::string&);
    void addParameter(const std::string&, const std::string&);
    void addSegment(const std::string&);
    void setFragmentString(const std::string&);
    void setHost(const std::string&);
    void setHostType(frederick2::httpEnums::uriHostType);
    void setParameterString(const std::string&);
    void setPassword(const std::string&);
    void setPort(int);
    void setScheme(frederick2::httpEnums::uriScheme);
    void setURIString(const std::string&);
    void setUserInfo(const std::string&, const std::string&);
    void setUsername(const std::string&);
    ~uri();
protected:
private:
    bool fragmentProvided;
    bool userInfoProvided;
    bool hostProvided;
    bool parametersProvided;
    bool pathProvided;
    bool portProvided;
    bool schemeProvided;
    int port;
    std::string fullFragmentString;
    std::string fullParameterString;
    std::string fullURIString;
    std::string host;
    std::string password;
    std::string username;
    strMAPstr parameters;
    std::deque<std::string> segments;
    std::vector<std::string> fragments;    
    frederick2::httpEnums::uriHostType hostType;
    frederick2::httpEnums::uriScheme scheme;
};

#endif