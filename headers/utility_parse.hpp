//
// utility_parse.hpp
// ~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef UTILITY_PARSE_HPP
#define UTILITY_PARSE_HPP

#include <string>

#include "frederick2_namespace.hpp"

class frederick2::utility::parseUtilities
{
public:
    parseUtilities();
    bool dqExtract(const std::string&, std::string&, bool);
    bool escapeReplace(const std::string&, std::string&);
    bool isHex(char);
    bool pctDecode(const std::string&, std::string&);
    bool pctEncode(const std::string&, std::string&);
    bool replace(std::string&, const std::string&, const std::string&);
    bool toHex(const size_t&, std::string&);
    bool toLower(const std::string&, std::string&);
    bool toUpper(const std::string&, std::string&);
    ~parseUtilities();
protected:
private:
};

#endif