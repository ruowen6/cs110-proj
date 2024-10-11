/*
#############################################################################
# COMP.CS.110 Ohjelmointi 2: Tekniikat / Programming 2: Techniques          #
# Project: Suku on pahin / All in the family                                #
# File: familytree.hh                                                       #
# Description: Familytree class                                             #
#   Contains a datastructure that is populated with Person structs and      #
#   provides some query functions.                                          #
# Notes: * This is a part of an exercise program                            #
#        * Student's aren't allowed to alter public interface!              #
#        * All changes to private side are allowed.                         #
#############################################################################
*/
#ifndef FAMILYTREE_HH
#define FAMILYTREE_HH

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <memory>
#include <map>

using Params = const std::vector<std::string>&;

// Named constants to improve readability in other modules.
const std::string NO_ID = "";
const int NO_HEIGHT = -1;

// Error messages
const std::string ALREADY_ADDED = "Error. Person already added.";
const std::string WRONG_LEVEL = "Error. Level can't be less than 1.";

// Struct for the persons data.
struct Person
{
    std::string id_ = NO_ID;
    int height_ = NO_HEIGHT;
    std::vector<Person*> parents_{nullptr, nullptr};
    std::vector<Person*> children_;
};

using IdSet = std::set<std::string>;

/**
 * @brief The Familytree class
 */
class Familytree
{
public:
    /**
     * @brief Familytree
     */
    Familytree();

    /**
     * @brief addNewPerson
     * @param id
     * @param height
     * @param output
     * Add a new person to the datastructure.
     */
    void addNewPerson(const std::string& id, int height,
                      std::ostream &output);

    /**
     * @brief addRelation
     * @param child
     * @param parents
     * @param output
     * Add a new parent-child relation.
     */
    void addRelation(const std::string& child,
                     const std::vector<std::string>& parents,
                     std::ostream& output);

    /**
     * @brief printPersons
     * @param output
     * Print all stored persons with their ids and heights.
     */
    void printPersons(Params, std::ostream &output) const;

    /**
     * @brief printChildren
     * @param params (contains person's id)
     * @param output
     * Print all children of the given person.
     */
    void printChildren(Params params, std::ostream& output) const;

    /**
     * @brief printParents
     * @param params (contains person's id)
     * @param output
     * Print all parents (one or two) of the given person.
     */
    void printParents(Params params, std::ostream& output) const;

    /**
     * @brief printSiblings
     * @param params (contains person's id)
     * @param output
     * Print all siblings of the given person.
     */
    void printSiblings(Params params, std::ostream& output) const;

    /**
     * @brief printCousins
     * @param params (contains person's id)
     * @param output
     * Print all cousins of the given person.
     */
    void printCousins(Params params, std::ostream& output) const;

    /**
     * @brief printTallestInLineage
     * @param params (contains person's id)
     * @param output
     * Print the tallest person of the given person's lineage.
     */
    void printTallestInLineage(Params params, std::ostream& output) const;

    /**
     * @brief printShortestInLineage
     * @param params (contains person's id)
     * @param output
     * Print the shortest person of the given person's lineage.
     */
    void printShortestInLineage(Params params, std::ostream& output) const;

    /**
     * @brief printGrandChildrenN
     * @param params (contains person's id, and distance as a string)
     * @param output
     * Print all grandchildren in the given distance of the given person.
     */
    void printGrandChildrenN(Params params, std::ostream& output) const;

    /**
     * @brief printGrandParentsN
     * @param params
     * @param output
     * Print all grandparents in the given distance of the given person.
     */
    void printGrandParentsN(Params params, std::ostream& output) const;

private:
    /* The following functions are meant to make project easier.
     * You can implement them if you want and/or create your own.
     * Anyway it would be a good idea to implement more functions
     * to make things easier and to avoid "copy-paste-coding".
     */

    /**
     * @brief getPointer
     * @param id
     * @return the pointer to Person object with the given id.
     */
    Person* getPointer(const std::string& id) const;

    /**
     * @brief printNotFound
     * @param id
     * @param output
     * Print the error message for id not found.
     */
    void printNotFound(const std::string& id, std::ostream& output) const;

    /**
     * @brief vectorToIdSet
     * @param container
     * @return set of ids of those persons included in the given container
     * vector.
     */
    IdSet vectorToIdSet(const std::vector<Person*> &container) const;

    /**
     * @brief printGroup
     * @param id
     * @param group can be e.g. "children", "parents", "siblings", "cousins"
     * @param container contains the ids of the group members
     * @param output
     * Print the data in the container.
     */
    void printGroup(const std::string& id, const std::string& group,
                    const IdSet& container, std::ostream& output,
                    const int depth = 0) const;

    //======== below are private variable/functions I add ========

    /**
     * @brief collectDescendants
     * @param id
     * @param descendantsList collects the ids of all the descendants
     *        of the given person
     * Collects all the ids of descendants and put into the descendantsList
     */
    void collectDescendants(const std::string& id,
                            IdSet& descendantsList) const;

    /**
     * @brief collectDescendantsWithDepth
     * @param id
     * @param descendantsList collects the ids of all the descendants
     *        of the given person at the given depth
     * @param maxDepth e.g. children = 1, grandchildren = 2
     * @param currentDepth is the current depth inside the recursion
     * Collects all the ids of descendants at a certain level
     * and put into the descendantsList
     */
    void collectDescendantsWithDepth(const std::string& id,
                                     IdSet& descendantsList,
                                     int maxDepth = 1,
                                     int currentDepth = 1) const;

    /**
     * @brief collectAncestorsWithDepth
     * @param id
     * @param ancestorsList collects the ids of all the ancestors
     *        of the given person at the given depth
     * @param maxDepth e.g. parents = 1, grandparents = 2
     * @param currentDepth is the current depth inside the recursion
     * Collects all the ids of ancestors at a certain level
     * and put into the ancestorsList
     */
    void collectAncestorsWithDepth(const std::string& id,
                                   IdSet& ancestorsList,
                                   int maxDepth = 1,
                                   int currentDepth = 1) const;

    /**
     * @brief printHeightResult
     * @param thisPerson_name
     * @param resultName is the id of the tallest/shortest person
     * @param resultHeight is the height of the tallest/shortest person
     * @param isForShortest is whether it is for
     *        the printShortestInLineage funtion
     * @param output
     * Print the message about tallest/shortest person's id and height
     */
    void printHeightResult(const std::string& thisPerson_name,
                           const std::string& resultName, int resultHeight,
                           bool isForShortest, std::ostream& output) const;

    /**
     * @brief collectHeightResult
     * @param thisPerson_name
     * @param resultName is the id of the tallest/shortest person
     * @param resultHeight is the height of the tallest/shortest person
     * @param isForShortest is whether it is for
     *        the printShortestInLineage funtion
     * Collect the data about tallest/shortest person's id and height
     */
    void collectHeightResult(const std::string& thisPerson_name,
                             std::string& resultName,
                             int& resultHeight, bool isForShortest) const;

    std::map<std::string, std::shared_ptr<Person>> peopleMap_;

    //for personal test ONLY
    /**
     * @brief testTEST
     * @param output
     * @param type is where the error occurs
     */
    void testTEST(std::ostream& output, std::string location);
};

#endif // FAMILYTREE_HH
