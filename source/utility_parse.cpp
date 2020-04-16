//
// utility_parse.cpp
// ~~~~~~~~~~~~~~~~~
//
// Author: Joseph Adomatis
// Copyright (c) 2020 Joseph R Adomatis (joseph dot adomatis at gmail dot com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <exception>
#include <sstream>
#include <string>

#include "../headers/frederick2_namespace.hpp"
#include "../headers/utility_parse.hpp"

namespace utility = frederick2::utility;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global variable definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global function definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// frederick2::utility::parseUtilities member definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////
utility::parseUtilities::parseUtilities()
{
    
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::utility::parseUtilities::dqExtract
///////////////////////////////////////////////////////////////////////////////

bool utility::parseUtilities::dqExtract(const std::string& original, std::string& revised, bool escape)
{
    size_t dqueFoundS{original.find_first_of('\"')};
    if(dqueFoundS != std::string::npos)
    {
        ///////////////////////////////////////////////////////////////////////////////
        // double quoted string should have DQUOTE as first and last char
        ///////////////////////////////////////////////////////////////////////////////
        size_t dqueFoundE{original.find_last_of('\"')};
        if(dqueFoundS != 0 || dqueFoundE != (original.size() - 1))
        {
            return(false);
        }
        else
        {
            ///////////////////////////////////////////////////////////////////////////////
            // DQUOTE are first and last char
            // extract out the string between them
            // if escape = true then run the escapeReplace on the extracted string
            ///////////////////////////////////////////////////////////////////////////////
            std::string extractInit{original.substr(1, original.size() - 2)};
            if(escape)
            {
                std::string extractFinal;
                this->escapeReplace(extractInit, extractFinal);
                revised = std::move(extractFinal);
            }
            else
            {
                revised = std::move(extractInit);
            }
        }
    }
    else
    {
        ///////////////////////////////////////////////////////////////////////////////
        // no DQUOTE found in string.. just set revised = original
        ///////////////////////////////////////////////////////////////////////////////
        revised = original;
    }

    return(true);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::utility::parseUtilities::escapeReplace
///////////////////////////////////////////////////////////////////////////////

bool utility::parseUtilities::escapeReplace(const std::string& original, std::string& revised)
{
    std::string bldStr;
    size_t maxBound{original.size()};
    size_t maxSlashBound{(original.size() - 1)};
    for(size_t i = 0; i < maxBound; i++)
    {
        char c0{original[i]};
        
        if (c0 == '\\')
        {
            if(i < maxSlashBound)
            {
                ///////////////////////////////////////////////////////////////////////////////
                // '\' not the last char
                // get next char
                ///////////////////////////////////////////////////////////////////////////////
                
                char c1{original[i+1]};
                bldStr.append(1, c1);
                i += 1;
            }
            else
            {
                ///////////////////////////////////////////////////////////////////////////////
                // '\' is last char.. 
                // return false 
                ///////////////////////////////////////////////////////////////////////////////
                
                return(false);
            }                       
        }
        else
        {
            ///////////////////////////////////////////////////////////////////////////////
            // target char isnt a '\'
            // just append it to bldStr
            ///////////////////////////////////////////////////////////////////////////////
            
            bldStr.append(1, c0);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // move contents of bldStr back into supplied string ref
    ///////////////////////////////////////////////////////////////////////////////
    
    revised = std::move(bldStr);
    return(true);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::utility::parseUtilities::isHex
///////////////////////////////////////////////////////////////////////////////

bool utility::parseUtilities::isHex(char c)
{
    return((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::utility::parseUtilities::pctDecode
///////////////////////////////////////////////////////////////////////////////

bool utility::parseUtilities::pctDecode(const std::string& original, std::string& revised)
{
    bool returnValue{true};
    std::string bldStr;

    size_t maxBound{original.size()};
    size_t maxHexBound{(original.size() - 2)};
    for(size_t i = 0; i < maxBound; i++)
    {
        char c0{original[i]};
        
        if(c0 == '+')
        {
            ///////////////////////////////////////////////////////////////////////////////
            // '+' decodes to space
            ///////////////////////////////////////////////////////////////////////////////
            
            bldStr.append(1, ' ');
        }
        else if (c0 == '%')
        {
            if(i < maxHexBound)
            {
                ///////////////////////////////////////////////////////////////////////////////
                // '%' not one of the last two chars
                // check next 2 chars to ensure they are potential hex values
                ///////////////////////////////////////////////////////////////////////////////
                
                char c1{original[i+1]};
                char c2{original[i+2]};
                if(this->isHex(c1) && this->isHex(c2))
                {
                    ///////////////////////////////////////////////////////////////////////////////
                    // next 2 chars are hex, use stoi base 16 to convert them to int
                    // then static cast into to unsigned char
                    ///////////////////////////////////////////////////////////////////////////////
                    
                    unsigned int numVal = std::stoi(original.substr(i+1, 2), 0, 16);
                    unsigned char outChar{(unsigned char)numVal};

                    ///////////////////////////////////////////////////////////////////////////////
                    // add decoded char to bldStr
                    // because the next 2 chars were part of this single encoded char
                    // manual increment i 2 more spaces to skip evaluating them individually
                    ///////////////////////////////////////////////////////////////////////////////
                    
                    bldStr.append(1, outChar);
                    i += 2;
                }
                else
                {
                    ///////////////////////////////////////////////////////////////////////////////
                    // 1 or both of the chars after '%' was not a hex val
                    // go ahead and add '%' to bldStr and report invalid URI
                    ///////////////////////////////////////////////////////////////////////////////
                    
                    returnValue = false;
                    bldStr.append(1, c0);
                }                
            }
            else
            {
                ///////////////////////////////////////////////////////////////////////////////
                // '%' is one of the last 2 chars.. 
                // go ahead and add to bldStr and report invalid URI 
                ///////////////////////////////////////////////////////////////////////////////
                
                returnValue = false;
                bldStr.append(1, c0);
            }                       
        }
        else
        {
            ///////////////////////////////////////////////////////////////////////////////
            // target char isnt a special character...
            // just append it to bldStr
            ///////////////////////////////////////////////////////////////////////////////
            
            bldStr.append(1, c0);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // move contents of bldStr back into supplied string ref
    ///////////////////////////////////////////////////////////////////////////////
    
    revised = std::move(bldStr);
    return(returnValue);   
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::utility::parseUtilities::pctEncode
///////////////////////////////////////////////////////////////////////////////

bool utility::parseUtilities::pctEncode(const std::string& original, std::string& revised)
{
    bool returnValue{true};
    return(returnValue);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::utility::parseUtilities::replace
///////////////////////////////////////////////////////////////////////////////

bool utility::parseUtilities::replace(std::string& src, const std::string& sPre, const std::string& sPost)
{
    bool returnValue{true};
    size_t sPreSize = sPre.size();
    size_t sPreFound{src.find(sPre)};
    while(sPreFound != std::string::npos)
    {
        src.replace(sPreFound, sPreSize, sPost);
        sPreFound = src.find(sPre);    
    }
    return(returnValue);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::utility::parseUtilities::toHex
///////////////////////////////////////////////////////////////////////////////

bool utility::parseUtilities::toHex(const size_t& original, std::string& revised)
{
    bool opSuccess{true};
    revised.clear();
    std::stringstream ss;
    ss << std::hex << original;
    ss >> revised;
    return(opSuccess);
}

///////////////////////////////////////////////////////////////////////////////
// frederick2::utility::parseUtilities::toLower
///////////////////////////////////////////////////////////////////////////////

bool utility::parseUtilities::toLower(const std::string& original, std::string& revised)
{
    bool opSuccess{true};
    std::string str{original};
    
    try
    {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){return(std::tolower(c));});
    }
    catch(const std::exception& e)
    {
        opSuccess = false;
    }
    
    if(opSuccess)
    {
        revised = std::move(str);
    }
    else
    {
        revised = original;
    }
    
    return(opSuccess);
}

///////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////

utility::parseUtilities::~parseUtilities()
{
    
}