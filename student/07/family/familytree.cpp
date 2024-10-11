#include "familytree.hh"

#include <iostream>
#include <set>
#include <string>

const bool PARENT_DIRECTION = true;
const bool CHILD_DIRECTION = false;

Familytree::Familytree()
{
}

void Familytree::addNewPerson(const std::string &id, int height,
                              std::ostream &output)
{
    /* if not the first time add
     * the getPointer function will return a valid pointer */
    if(getPointer(id)){
        output << ALREADY_ADDED << std::endl;
        return;
    }
    std::shared_ptr<Person> thisPerson
            = std::make_shared<Person>(
                Person{id, height, {nullptr, nullptr}, {}});
    peopleMap_.insert({{id, thisPerson}});
    //testTEST(output, "addNewPerson");
}

void Familytree::addRelation(const std::string &child,
                             const std::vector<std::string> &parents,
                             std::ostream& output)
{
    /* try to find parents' pointers with parents' id,
     * if can't find, the parentA/parentB would be nullptr */
    Person* parentA = getPointer(parents[0]);
    Person* parentB = getPointer(parents[1]);

    //check if the person exists
    Person* childToAdd = getPointer(child);
    if(!childToAdd){
        printNotFound(child, output);
        return;
    }
    //find the person and add the parents relation to the person
    if(peopleMap_.find(child) != peopleMap_.end()){
        childToAdd->parents_ = {parentA, parentB};
    }
    //find the parent for that person and add children relation
    for(auto& parent:{parentA, parentB}){
        if(parent){
            getPointer(parent->id_)->children_.push_back(childToAdd);
        }
    }
    //testTEST(output, "addRelation");
}

void Familytree::printPersons(Params, std::ostream &output) const
{
    for(auto& person:peopleMap_){
        //print:        id,            height in the Person struct
        output << person.first << ", " << person.second->height_ << std::endl;
    }
}

void Familytree::printChildren(Params params, std::ostream &output) const
{
    /* set the group name to help systematic message print
     * and more readable */
    std::string groupName = "children";
    //get the person's name from user's input
    std::string thisPersonName = params.at(0);
    //search the person in the dataset by name
    Person* thisPerson = getPointer(thisPersonName);
    //if not found (getPointer return nullptr)
    if(!thisPerson){
        printNotFound(thisPersonName, output);
        return;
    }
    //container to put the output name list
    IdSet namelist = {};
    //collect the children data
    collectRelationsWithDepth(thisPersonName, namelist, CHILD_DIRECTION);
    //print the name list
    printGroup(thisPersonName, groupName, namelist, output);
}

void Familytree::printParents(Params params, std::ostream &output) const
{
    std::string groupName = "parents";
    std::string thisPerson_name = params.at(0);
    Person* thisPerson = getPointer(thisPerson_name);
    if(!thisPerson){
        printNotFound(thisPerson_name, output);
        return;
    }

    IdSet namelist = {};

    collectRelationsWithDepth(thisPerson_name, namelist, PARENT_DIRECTION);

    printGroup(thisPerson_name, groupName, namelist, output);
}

void Familytree::printSiblings(Params params, std::ostream &output) const
{
    std::string groupName = "siblings";
    std::string thisPerson_name = params.at(0);
    Person* thisPerson = getPointer(thisPerson_name);
    if(!thisPerson){
        printNotFound(thisPerson_name, output);
        return;
    }

    IdSet namelist = {};
    IdSet namelist_parent = {};

    //find the parents
    collectRelationsWithDepth(thisPerson_name, namelist_parent, PARENT_DIRECTION);


    //find the children of this person's parents
    for(auto& parentName:namelist_parent){
        collectRelationsWithDepth(parentName, namelist, CHILD_DIRECTION);
    }
    /* siblings are the children from this person's parent
     * but except this person her/himself */
    namelist.erase(thisPerson_name);

    printGroup(thisPerson_name, groupName, namelist, output);
}

void Familytree::printCousins(Params params, std::ostream &output) const
{
    std::string groupName = "cousins";
    std::string thisPerson_name = params.at(0);
    Person* thisPerson = getPointer(thisPerson_name);
    if(!thisPerson){
        printNotFound(thisPerson_name, output);
        return;
    }

    IdSet namelist = {};
    //namelistParents
    IdSet namelist_parents = {};
    IdSet namelist_grandparents = {};
    IdSet namelist_cousins = {};

    //find the parents
    collectRelationsWithDepth(thisPerson_name, namelist_parents, PARENT_DIRECTION);

    //find the grandparents
    collectRelationsWithDepth(thisPerson_name, namelist_grandparents,
                              PARENT_DIRECTION, 2);

    //find the siblings of the person's parents
    for(auto& cousins_Name:namelist_grandparents){
        collectRelationsWithDepth
                (cousins_Name, namelist_cousins, CHILD_DIRECTION);
    }
    /* parents' siblings are the children from grandparents
     * but except parents themselves */
    for(auto& parentName:namelist_parents){
        namelist_cousins.erase(parentName);
    }

    //find the children of the siblings of parent ==> cousins
    for(auto& cousins_Name:namelist_cousins){
        collectRelationsWithDepth(cousins_Name, namelist, CHILD_DIRECTION);
    }

    printGroup(thisPerson_name, groupName, namelist, output);
}

void Familytree::printTallestInLineage(Params params,
                                       std::ostream &output) const
{
    std::string thisPerson_name = params.at(0);
    Person* thisPerson = getPointer(thisPerson_name);
    if(!thisPerson){//thisPersonName
        printNotFound(thisPerson_name, output);
        return;
    }

    bool isForShortest = false;
    std::string resultName = "";
    int resultHeight = NO_HEIGHT;

    collectHeightResult(thisPerson_name, resultName, resultHeight,
                        isForShortest);

    printHeightResult(thisPerson_name, resultName, resultHeight,
                      isForShortest, output);
}

void Familytree::printShortestInLineage(Params params,
                                        std::ostream &output) const
{
    std::string thisPerson_name = params.at(0);
    Person* thisPerson = getPointer(thisPerson_name);
    if(!thisPerson){
        printNotFound(thisPerson_name, output);
        return;
    }

    bool isForShortest = true;
    std::string resultName = "";
    int resultHeight = NO_HEIGHT;

    collectHeightResult(thisPerson_name, resultName, resultHeight,
                        isForShortest);

    printHeightResult(thisPerson_name, resultName, resultHeight,
                      isForShortest, output);
}

void Familytree::printGrandChildrenN(Params params, std::ostream &output) const
{
    std::string groupName = "grandchildren";
    std::string thisPerson_name = params.at(0);
    Person* thisPerson = getPointer(thisPerson_name);
    if(!thisPerson){
        printNotFound(thisPerson_name, output);
        return;
    }

    IdSet namelist = {};
    int depth = stoi(params.at(1)) + 1;

    collectRelationsWithDepth(thisPerson_name, namelist, CHILD_DIRECTION, depth);

    if(!(depth - 1)){
        output << WRONG_LEVEL << std::endl;
        return;
    }

    printGroup(thisPerson_name, groupName, namelist, output, depth - 1);

}

void Familytree::printGrandParentsN(Params params, std::ostream &output) const
{
    std::string groupName = "grandparents";
    std::string thisPerson_name = params.at(0);
    Person* thisPerson = getPointer(thisPerson_name);
    if(!thisPerson){
        printNotFound(thisPerson_name, output);
        return;
    }

    IdSet namelist = {};
    int depth = stoi(params.at(1)) + 1;

    collectRelationsWithDepth(thisPerson_name, namelist, PARENT_DIRECTION, depth);

    if(!(depth - 1)){
        output << WRONG_LEVEL << std::endl;
        return;
    }

    printGroup(thisPerson_name, groupName, namelist, output, depth - 1);

}

//======== below are private functions ========

Person *Familytree::getPointer(const std::string &id) const
{
    if(peopleMap_.find(id) == peopleMap_.end()){
        return nullptr;
    }
    return peopleMap_.at(id).get();
}

void Familytree::printNotFound(const std::string &id,
                               std::ostream &output) const
{
    output << "Error. " << id << " not found." << std::endl;
}

IdSet Familytree::vectorToIdSet(const std::vector<Person *> &container) const
{
    IdSet idList = {};
    for(auto person:container){
        if(person){
            idList.insert(person->id_);
        }
    }
    return idList;
}

void Familytree::printGroup(const std::string &id, const std::string &group,
                            const IdSet &container, std::ostream &output,
                            const int depth) const
{
    if(container.empty()){
        output << id << " has no ";
        if(depth - 1){
            for(int i = depth - 1; i > 0; i--){
                output << "great-";
            }
        }
        output << group << "." << std::endl;
        return;
    }
    output << id << " has " << container.size() << " " ;
    if(depth - 1){
        for(int i = depth - 1; i > 0; i--){
            output << "great-";
        }
    }
    output << group << ":" << std::endl;

    for(auto& memberName:container){
        output << memberName << std::endl;
    }
}

//======== below are private functions I add ========

void Familytree::collectDescendants(const std::string &id,
                                    IdSet &descendantsList) const
{
    Person* eachPerson = getPointer(id);
    if(!eachPerson){
        return;
    }
    for(auto& child:eachPerson->children_){
        if(child){
            descendantsList.insert(child->id_);
            collectDescendants(child->id_, descendantsList);
        }
    }
}

void Familytree::collectRelationsWithDepth(const std::string &id,
                                             IdSet &membersList,
                                             bool direction,
                                             int maxDepth,
                                             int currentDepth) const
{
    if(currentDepth > maxDepth){
        return;
    }
    Person* eachPerson = getPointer(id);
    if(!eachPerson){
        return;
    }
    if(direction == PARENT_DIRECTION){
        for(auto& parent:eachPerson->parents_){
            if(parent){
                if(currentDepth == maxDepth){
                    membersList.insert(parent->id_);
                }
                else{
                    collectRelationsWithDepth(parent->id_, membersList,
                                              PARENT_DIRECTION,
                                              maxDepth, currentDepth + 1);
                }
            }
        }
    }
    else{
        for(auto& child:eachPerson->children_){
            if(child){
                if(currentDepth == maxDepth){
                    membersList.insert(child->id_);
                    }
                else{
                    collectRelationsWithDepth(child->id_, membersList,
                                              CHILD_DIRECTION,
                                              maxDepth, currentDepth + 1);
                }
            }
        }
    }
}

void Familytree::printHeightResult(const std::string& thisPerson_name,
                                   const std::string& resultName,
                                   int resultHeight, bool isForShortest,
                                   std::ostream& output) const
{
    if(resultHeight == NO_HEIGHT){
        output << "ERROR in height, check function: collectHeightResult";
        return;
    }

    std::string forFunction = "tallest";
    if(isForShortest){
        forFunction = "shortest";
    }
    if(resultName == thisPerson_name){
        output << "With the height of " << resultHeight << ", "
                << thisPerson_name
                << " is the " << forFunction
                << " person in his/her lineage." << std::endl;
    }
    else{
        output << "With the height of " << resultHeight << ", "
                << resultName
                << " is the " << forFunction
                << " person in "
                << thisPerson_name << "'s lineage." << std::endl;
    }
}

void Familytree::collectHeightResult(const std::string &thisPerson_name,
                                     std::string &resultName,
                                     int &resultHeight,
                                     bool isForShortest) const
{
    IdSet namelist;
    collectDescendants(thisPerson_name, namelist);
    //initialize the result by the person's height
    resultHeight = getPointer(thisPerson_name)->height_;
    //find the target height
    for(auto& eachDescendant:namelist){
        int eachDescendant_height = getPointer(eachDescendant)->height_;
        if(isForShortest){
            if(resultHeight > eachDescendant_height){
                resultHeight = eachDescendant_height;
            }
        }
        else{
            if(resultHeight < eachDescendant_height){
                resultHeight = eachDescendant_height;
            }
        }
    }
    //find the target person
    resultName = thisPerson_name;
    for(auto& eachDescendant:namelist){
        int eachDescendant_height = getPointer(eachDescendant)->height_;
        if(eachDescendant_height == resultHeight){
            resultName = eachDescendant;
        }
    }
}

void Familytree::testTEST(std::ostream &output, std::string location)
{
    static int counter = 1;
    output << "No." << counter << " " << location << " info added." << std::endl;
    counter++;
}
