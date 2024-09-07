/*
#############################################################################
# COMP.CS.110 Ohjelmointi 2: Tekniikat / Programming 2: Techniques          #
# Project: Suku on pahin / All in the family                                #
# File: cli.hh                                                              #
# Description: Commandline interface for the program.                       #
# Notes: * This is a part of an exercise program                            #
#        * Student's don't touch this file.                                 #
#        * Understanding how the module works isn't necessary to complete   #
#          this exercise.                                                   #
#        * This is also an example how you can use function pointers to     #
#          a class' functions.                                              #
#        * This could be upgraded to have commandhistory with a relative    #
#          ease.                                                            #
#############################################################################
*/
#ifndef CLI_HH
#define CLI_HH

#include "familytree.hh"

#include <string>
#include <vector>
#include <memory>

// Declare a type of function called
using MemberFunc = void (Familytree::*)(const std::vector<std::string>& params,
                                        std::ostream&) const;

// Struct describing a command
struct CommandInfo
{
    std::string id_; // needed only for quit and commands with numeric params
    std::vector<std::string> allNames_;
    std::vector<std::string> params_;
    MemberFunc funcPtr_;
};

// Error messages
const std::string WRONG_PARAMETERS = "Wrong amount of parameters.";
const std::string NOT_NUMERIC = "Wrong type of parameters.";
const std::string UNKNOWN_COMMAND = "Unknown command.";


class Cli
{
public:    
    /**
     * @brief Cli
     * @param db (database) pointer to the Familytree
     */
    Cli(std::shared_ptr<Familytree> db);

    /**
     * @brief exec_prompt runs the interface
     * @return true for normal commands, false for exit command
     */
    bool exec_prompt();

private:
    // Pointer to the Familytree object the functions are called to
    std::shared_ptr<Familytree> database_;

    // Prompt printed for every query
    const std::string PROMPT = "> ";

    // Vector containing those commands the CLI can recognize
    std::vector<CommandInfo> commands_ = {
        {"Q",{"QUIT","EXIT","Q","LOPETA"}, {}, nullptr},
        {"",{"PRINT","TREE","FAMILYTREE","SUKUPUU","PUU"}, {}, &Familytree::printPersons},
        {"",{"CHILDREN","LAPSET"}, {"person"}, &Familytree::printChildren},
        {"",{"COUSINS","SERKUT"}, {"person"}, &Familytree::printCousins},
        {"",{"SIBLINGS","SISARUKSET"},{"person"},&Familytree::printSiblings},
        {"",{"PARENTS","VANHEMMAT"},{"person"},&Familytree::printParents},
        {"",{"TALLEST","PISIN"}, {"person"},&Familytree::printTallestInLineage},
        {"",{"SHORTEST","LYHYIN","LYHIN"}, {"person"}, &Familytree::printShortestInLineage},
        {"N",{"GRANDCHILDREN","LAPSENLAPSET","GC","LL"}, {"person", "N"},&Familytree::printGrandChildrenN},
        {"N",{"GRANDPARENTS","ISOVANHEMMAT","GP","IV"}, {"person", "N"},&Familytree::printGrandParentsN},
        {"",{},{},nullptr}
    };

    /**
     * @brief findCommand
     * @param command_name
     * @return pointer to the command found from the commands_ vector;
     * if not found, returns a nullptr
     */
    CommandInfo* findCommand(std::string& command_name);
};

#endif // CLI_HH
