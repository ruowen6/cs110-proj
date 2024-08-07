/*
#############################################################################
# COMP.CS.110 Ohjelmointi 2: Tekniikat / Programming 2: Techniques          #
# Project: Suku on pahin / All in the family                                #
# File: utils.hh                                                            #
# Description: Module for different utilities needed in the program.        #
# Notes: * This is a part of an exercise program                            #
#        * Student's don't touch this file.                                 #
#        * Understanding how the module works isn't necessary to complete   #
#          this exercise.                                                   #
#############################################################################
*/
#ifndef UTILS_HH
#define UTILS_HH

#include <vector>
#include <string>

namespace Utils
{
/**
 * @brief split
 * @param str
 * @param delim
 * @return vector containing the parts, no delim chars
 * Splits the given string at every delim char.
 */
std::vector<std::string> split(const std::string& str, char delim = ';');

/**
 * @brief isEmpty
 * @param str
 * @return true if given string consists only of empty spaces
 */
bool isEmpty(const std::string& str);

/**
 * @brief isNumeric
 * @param str
 * @return true if given string is numeric
 */
bool isNumeric(const std::string& str);

}

#endif // UTILS_HH


