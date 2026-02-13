/*
#############################################################################
# COMP.CS.110 Ohjelmointi 2: Tekniikat / Programming 2: Techniques          #
# Project: Suku on pahin / All in the family                                #
# Program description: Queries user for CSV-file and allows simple searches #
#       based on the data.                                                  #
# File: main.cpp                                                            #
# Description: Main-module performs the followin operations:                #
#       * Query for input file.                                             #
#       * Parse the CSV-data                                                #
#       * Pass the parsed data to Familytree-module                         #
#       * Launch Cli-module                                                 #
# Notes: * This is an exercise program.                                     #
#        * Student's don't touch this file.                                 #
#############################################################################
*/
#include "familytree.hh"
#include "cli.hh"
#include "utils.hh"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

// Constants to make CSV-parsing more readable.
const char CSV_DELIMITER = ';';
enum CsvFields { CSV_NAME, CSV_HEIGHT, CSV_FATHER, CSV_MOTHER, CSV_VALUES };

// Struct for parent-child relations.
struct Relation
{
    std::string child_;
    std::vector<std::string> parents_;
};

/**
 * @brief parsePerson
 * @param line: string to be parsed
 * @param relations
 * @return Person struct that was created, in erroneuos input the fields id_
 *         and height_ keep their initial values (NO_NAME and NO_HEIGTH)
 * Parse the given line to a new person and child-parent relationship.
 */
Person parsePerson(const std::string& line, std::vector<Relation> &relations)
{
    Person new_person;
    Relation new_relation;
    std::vector<std::string> parsed_data = Utils::split(line, CSV_DELIMITER);

    // Check that there was correct amount of fields in the line.
    if( parsed_data.size() == CSV_VALUES )
    {
        new_person.id_ = parsed_data[CSV_NAME];

        if( Utils::isNumeric(parsed_data[CSV_HEIGHT]) )
        {
            new_person.height_ = std::stoi(parsed_data[CSV_HEIGHT]);
        }
        else
        {
            std::cout << "Invalid argument." << std::endl;
        }

        new_relation.child_ = parsed_data[CSV_NAME];
        new_relation.parents_.push_back(parsed_data[CSV_FATHER]);
        new_relation.parents_.push_back(parsed_data[CSV_MOTHER]);
        relations.push_back(new_relation);
    }

    return new_person;
}

/**
 * @brief populateDatabase
 * @param datafile
 * @param database
 * @return true iff there were no errors in the datafile
 * Read the datafile and populate database with its content
 */
bool populateDatabase(std::ifstream& datafile,
                      std::shared_ptr<Familytree> database)
{
    std::string line = "";
    std::vector<std::string> parsed_line;
    std::vector<Relation> relations;
    int line_number = 0;

    while( std::getline(datafile, line) )
    {
        // Linenumbering for error-printing.
        ++line_number;

        // Skip empty and commented lines.
        if( line.empty() or line[0] == '#' )
        {
            continue;
        }

        // Parse the line to a new person and relation.
        Person new_person = parsePerson(line, relations);
        if( new_person.id_ == NO_ID or new_person.height_ == NO_HEIGHT )
        {
            std::cout << "Error in datafile, line " << line_number
                      << std::endl;
            return false;
        }

        // Add the new person to the database.
        database->addNewPerson(new_person.id_, new_person.height_, std::cout);
    }

    // After the persons have been created, add the child-parent relations.
    for( auto iter = relations.begin(); iter != relations.end(); ++iter )
    {
        database->addRelation(iter->child_, iter->parents_, std::cout);
    }
    return true;
}

/**
 * @brief main
 * @return
 * Ask for the input file, populate database on it, and launch CLI.
 */
int main()
{
    std::string cmd_string;
    std::shared_ptr<Familytree> database = std::make_shared<Familytree>();

    // File query
    std::cout << "Input file: ";
    std::getline(std::cin, cmd_string);
    std::ifstream datafile(cmd_string);

    if( not datafile )
    {
        std::cout << "Could not open file: " << cmd_string << std::endl;
        return EXIT_FAILURE;
    }

    if( not populateDatabase(datafile, database) )
    {
        return EXIT_FAILURE;
    }

    // Constructing the command-line interpreter with the given datastructure
    Cli commandline(database);

    // Empty loop that runs the CLI.
    // CLI returns false only on exit-command
    while( commandline.exec_prompt() ){}

    return EXIT_SUCCESS;
}
