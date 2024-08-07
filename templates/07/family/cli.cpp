/*
#############################################################################
# COMP.CS.110 Ohjelmointi 2: Tekniikat / Programming 2: Techniques          #
# Project: Suku on pahin / All in the family                                #
# File: cli.cpp                                                             #
# Description: Commandline interface for the program.                       #
# Notes: * This is a part of an exercise program                            #
#        * Student's don't touch this file.                                 #
# Check the cli.hh for more info.                                           #
#############################################################################
*/
#include "cli.hh"
#include "utils.hh"

#include <iostream>
#include <algorithm>

Cli::Cli(std::shared_ptr<Familytree> db) : database_(db)
{
}

bool Cli::exec_prompt()
{
    // Query for the command
    std::string line;
    std::cout << PROMPT;
    std::getline(std::cin, line);

    // Parsing command to the actual command and its parameters
    std::vector<std::string> input = Utils::split(line, ' ');
    if ( input.empty() or Utils::isEmpty(input.at(0)) )
    {
        return true;
    }
    std::string command_str = input.front();
    input.erase(input.begin());


    CommandInfo* command = findCommand(command_str);

    // Checking special commands
    if( command == nullptr )
    {
        std::cout << UNKNOWN_COMMAND << std::endl;
        return true;
    }
    if( command->id_ == "Q" )
    {
        return false;
    }
    if( command->params_.size() != input.size() )
    {
        std::cout << WRONG_PARAMETERS << std::endl;
        return true;
    }
    if( command->id_ == "N" and not Utils::isNumeric(input.at(1)))
    {
        std::cout << NOT_NUMERIC << std::endl;
        return true;
    }

    // Calling command method through the function pointer
    (database_.get()->*(command->funcPtr_))(input, std::cout);
    return true;
}

CommandInfo *Cli::findCommand(std::string& command_name)
{
    for( unsigned int i = 0; i < command_name.size(); i++ )
    {
        command_name.at(i) = std::toupper(command_name.at(i));
    }

    for( auto iter = commands_.begin(); iter != commands_.end(); ++iter )
    {
        for( auto& alias : iter->allNames_ )
        {
            if( alias == command_name )
            {
                return &(*iter);
            }
        }
    }
    return nullptr;
}

