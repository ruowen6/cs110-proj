/*
#############################################################################
# COMP.CS.110 Ohjelmointi 2: Tekniikat / Programming 2: Techniques          #
# Project: Suku on pahin / All in the family                                #
# File: utils.cpp                                                           #
# Description: Module for different utilities needed in the program.        #
# Notes: * This is a part of an exercise program                            #
#        * Student's don't touch this file.                                 #
# Check the utils.hh for more info.                                         #
#############################################################################
*/
#include "utils.hh"

std::vector<std::string> Utils::split(const std::string& str, char delim)
{
    std::vector<std::string> result = {""};
    bool insideQuotation = false;
    for ( char currentChar : str )
    {
        if ( currentChar == '"' )
        {
            insideQuotation = not insideQuotation;
        }
        else if ( currentChar == delim and not insideQuotation)
        {
            result.push_back("");
        }
        else
        {
            result.back().push_back(currentChar);
        }
    }
    return result;
}

bool Utils::isEmpty(const std::string &str)
{
    for ( char ch : str )
    {
        if ( ch != ' ' )
        {
            return false;
        }
    }
    return true;
}

bool Utils::isNumeric(const std::string& s)
{
    for ( unsigned int i = 0; i < s.size(); ++i )
    {
        if( not isdigit(s.at(i)) )
        {
            return false;
        }
    }
    return true;
}

