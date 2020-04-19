//
// utility_buffer.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef UTILITY_BUFFER_HPP
#define UTILITY_BUFFER_HPP

#include <string>

#include "frederick2_namespace.hpp"
#include "server_enum.hpp"

class frederick2::utility::bufferStringReader
{
public:
    explicit bufferStringReader(std::string*);
    frederick2::httpEnums::httpStatus extractFixedSize(size_t, std::string&, size_t&, size_t);
    frederick2::httpEnums::httpStatus extractLeadingEOL(size_t&);
    frederick2::httpEnums::httpStatus extractSingleLine(std::string&, size_t&, size_t, size_t);
    std::string getStatusReason();
    ~bufferStringReader();
protected:
private:
    ///////////////////////////////////////////////////////////////////////////////
    // Private Properties
    ///////////////////////////////////////////////////////////////////////////////
    std::string *buffer;
    std::string statusReason;
};

#endif