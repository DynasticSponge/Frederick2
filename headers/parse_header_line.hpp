//
// parse_header_line.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PARSE_HEADER_LINE_HPP
#define PARSE_HEADER_LINE_HPP


#include <string>

#include "frederick2_namespace.hpp"
#include "server_enum.hpp"

class frederick2::httpPacket::headerLineParser
{
public:
    explicit headerLineParser(std::string*);
    frederick2::httpEnums::httpStatus execute();
    size_t getBytesReceived();
    std::string getFieldName();
    std::string getFieldValueString();
    std::string getHeaderLineString();
    std::string getStatusReason();
    ~headerLineParser();
protected:
private:
    size_t bytesReceived;
    std::string *buffer;
    std::string fieldName;
    std::string statusReason;
    std::string originalHeaderLine;
    std::string originalValueString;
};

#endif