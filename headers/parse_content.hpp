//
// parse_content.hpp
// ~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PARSE_CONTENT_HPP
#define PARSE_CONTENT_HPP

#include <string>

#include "frederick2_namespace.hpp"
#include "server_enum.hpp"

class frederick2::httpPacket::contentParser
{
public:
    explicit contentParser(std::string*);
    frederick2::httpEnums::httpStatus executeChunked();
    frederick2::httpEnums::httpStatus executeContinuous(const size_t&);
    size_t getBytesReceived();
    std::string getContent();
    size_t getContentLength();
    size_t getContentReceived();
    std::string getStatusReason();
    ~contentParser();
protected:
private:
    frederick2::httpEnums::httpStatus extractChunkLength(std::string&, size_t&);
    frederick2::httpEnums::httpStatus extractContentData(size_t contentLength, std::string&);
    frederick2::httpEnums::httpStatus parseChunkHeader(size_t&, strMAPstr&);
    size_t bytesReceived;
    size_t chunksReceived;
    size_t contentLength;
    size_t contentReceived;
    std::string *buffer;
    std::string content;
    std::string statusReason;
};

#endif