#include "familytree.hh"

#include <iostream>
#include <set>
#include <string>

Familytree::Familytree()
{
}

void Familytree::addNewPerson(const std::string &id, int height,
                              std::ostream &output)
{
    //if not first add
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
                             std::ostream &output)
{
    //try to find parents' pointers with parents' id
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
    //find the parent of that person and add children relation
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
        output << person.first << ", " << person.second->height_ << std::endl;
    }
}

void Familytree::printChildren(Params params, std::ostream &output) const
{
    std::string groupName = "children";
    std::string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(isPersonNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};

    collectDescendantsWithDepth(thisPerson_name, namelist);

    printGroup(thisPerson_name, groupName, namelist, output);
}

void Familytree::printParents(Params params, std::ostream &output) const
{
    std::string groupName = "parents";
    std::string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(isPersonNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};

    collectAncestorsWithDepth(thisPerson_name, namelist);

    printGroup(thisPerson_name, groupName, namelist, output);
}

void Familytree::printSiblings(Params params, std::ostream &output) const
{
    std::string groupName = "siblings";
    std::string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(isPersonNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};
    IdSet namelist_parent = {};

    //find the parents
    collectAncestorsWithDepth(thisPerson_name, namelist_parent);

    //find the children of this person's parents
    for(auto& parentName:namelist_parent){
        collectDescendantsWithDepth(parentName, namelist);
    }
    /* siblings are the children from this person's parent
     * but except this person her/himself */
    namelist.erase(thisPerson_name);

    printGroup(thisPerson_name, groupName, namelist, output);
}

void Familytree::printCousins(Params params, std::ostream &output) const
{
    std::string groupName = "cousins";
    std::string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(isPersonNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};
    IdSet namelist_parents = {};
    IdSet namelist_grandparents = {};
    IdSet namelist_cousins = {};

    //find the parents
    collectAncestorsWithDepth(thisPerson_name, namelist_parents);

    //find the grandparents
    collectAncestorsWithDepth(thisPerson_name, namelist_grandparents, 2);

    //find the siblings of the person's parents
    for(auto& cousins_Name:namelist_grandparents){
        collectDescendantsWithDepth
                (cousins_Name, namelist_cousins);
    }
    /* parents' siblings are the children from grandparents
     * but except parents themselves */
    for(auto& parentName:namelist_parents){
        namelist_cousins.erase(parentName);
    }

    //find the children of the siblings of parent ==> cousins
    for(auto& cousins_Name:namelist_cousins){
        collectDescendantsWithDepth(cousins_Name, namelist);
    }

    printGroup(thisPerson_name, groupName, namelist, output);
}

void Familytree::printTallestInLineage(Params params,
                                       std::ostream &output) const
{
    std::string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(isPersonNotFound(thisPerson_name, thisPerson, params, output)){
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
    std::string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(isPersonNotFound(thisPerson_name, thisPerson, params, output)){
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
    std::string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(isPersonNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};
    int depth = stoi(params.at(1)) + 1;

    collectDescendantsWithDepth(thisPerson_name, namelist, depth);

    if(!(depth - 1)){
        output << WRONG_LEVEL << std::endl;
        return;
    }

    printGroup(thisPerson_name, groupName, namelist, output, depth - 1);

}

void Familytree::printGrandParentsN(Params params, std::ostream &output) const
{
    std::string groupName = "grandparents";
    std::string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(isPersonNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};
    int depth = stoi(params.at(1)) + 1;

    collectAncestorsWithDepth(thisPerson_name, namelist, depth);

    if(!(depth - 1)){
        output << WRONG_LEVEL << std::endl;
        return;
    }

    printGroup(thisPerson_name, groupName, namelist, output, depth - 1);

}

Person *Familytree::getPointer(const std::string &id) const
{
    if(peopleMap_.find(id) == peopleMap_.end()){
        return nullptr;
    }
    return peopleMap_.at(id).get();
}

void Familytree::printNotFound(const std::string &id, std::ostream &output) const
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

//below are funcs i set myself

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

void Familytree::collectDescendantsWithDepth(const std::string &id,
                                             IdSet &descendantsList,
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
    for(auto& child:eachPerson->children_){
        if(child){
            if(currentDepth == maxDepth){
                descendantsList.insert(child->id_);
            }
            else{
                collectDescendantsWithDepth(child->id_,
                                              descendantsList,
                                              maxDepth, currentDepth + 1);
            }
        }
    }
}

void Familytree::collectAncestorsWithDepth(const std::string &id,
                                           IdSet &ancestorsList,
                                           int maxDepth,
                                           int currentDepth) const
{
    if(currentDepth > maxDepth){
        return;
    }
    Person* eachPerson = peopleMap_.at(id).get();
    if(!eachPerson){
        return;
    }
    for(auto& parent:eachPerson->parents_){
        if(!parent){
            continue;
        }
        else{
            if(currentDepth == maxDepth){
                ancestorsList.insert(parent->id_);
            }
            else{
                collectAncestorsWithDepth(parent->id_, ancestorsList,
                                            maxDepth, currentDepth + 1);
            }
        }
    }
}

bool Familytree::isPersonNotFound(std::string& thisPerson_name,
                                  Person *&thisPerson,
                                  Params params, std::ostream& output) const
{
    thisPerson_name = params.at(0);
    thisPerson = getPointer(thisPerson_name);
    if(!thisPerson){
        printNotFound(thisPerson_name, output);
        /* if not found,
         * the outer if condition using this function will be true,
         * which means the person not found.
         * Then the branch would be quitted */
        return true;
    }
    return false;
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

void Familytree::testTEST(std::ostream &output, std::string type)
{
    static int counter = 1;
    output << "No." << counter << " " << type << " info added." << std::endl;
    counter++;
}
