#include "familytree.hh"

#include <iostream>
#include <set>
#include <string>

Familytree::Familytree()
{
}

void Familytree::addNewPerson(const std::string& id, int height,
                              std::ostream& output)
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

void Familytree::addRelation(const std::string& child,
                             const std::vector<std::string>& parents,
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

void Familytree::printPersons(Params, std::ostream& output) const
{
    for(auto& person:peopleMap_){
        //print:        id,            height in the Person struct
        output << person.first << ", " << person.second->height_ << std::endl;
    }
}

void Familytree::printChildren(Params params, std::ostream& output) const
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
    collectRelationsWithDepth(thisPersonName, namelist, "CHILD_DIRECTION");
    //print the name list
    printGroup(thisPersonName, groupName, namelist, output);
}

void Familytree::printParents(Params params, std::ostream& output) const
{
    /* set the group name to help systematic message print
     * and more readable */
    std::string groupName = "parents";
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
    //collect the parent data
    collectRelationsWithDepth(thisPersonName, namelist, "PARENT_DIRECTION");
    //print the name list
    printGroup(thisPersonName, groupName, namelist, output);
}

void Familytree::printSiblings(Params params, std::ostream& output) const
{
    /* set the group name to help systematic message print
     * and more readable */
    std::string groupName = "siblings";
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
    //helper container, to put the parents name list
    IdSet namelistParent = {};

    //find the parents
    collectRelationsWithDepth(thisPersonName, namelistParent, "PARENT_DIRECTION");


    //find the children of this person's parents
    for(auto& parentName:namelistParent){
        collectRelationsWithDepth(parentName, namelist, "CHILD_DIRECTION");
    }
    /* siblings are the children from this person's parent
     * but except this person her/himself */
    namelist.erase(thisPersonName);
    //print the name list
    printGroup(thisPersonName, groupName, namelist, output);
}

/* Main logic in this function:
 * the cousins are the children of parents' siblings */
void Familytree::printCousins(Params params, std::ostream& output) const
{
    /* set the group name to help systematic message print
     * and more readable */
    std::string groupName = "cousins";
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
    //helper container, to put the name list of parents
    IdSet namelistParents = {};
    //helper container, to put the name list of grandparents
    IdSet namelistGrandparents = {};
    //helper container, to put the name list of children of grandparents
    IdSet namelistAuntUncle = {};

    //find the parents
    collectRelationsWithDepth(thisPersonName, namelistParents,
                              "PARENT_DIRECTION");

    //find the grandparents
    collectRelationsWithDepth(thisPersonName, namelistGrandparents,
                              "PARENT_DIRECTION", 2);

    //find the siblings of the person's parents ==> aunts and uncles
    for(auto& auntUncleName:namelistGrandparents){
        collectRelationsWithDepth
                (auntUncleName, namelistAuntUncle, "CHILD_DIRECTION");
    }
    /* parents' siblings are the children from grandparents
     * but except parents themselves */
    for(auto& parentName:namelistParents){
        namelistAuntUncle.erase(parentName);
    }

    //find the children of aunts and uncles ==> cousins
    for(auto& cousinsName:namelistAuntUncle){
        collectRelationsWithDepth(cousinsName, namelist, "CHILD_DIRECTION");
    }
    //print the name list
    printGroup(thisPersonName, groupName, namelist, output);
}

void Familytree::printTallestInLineage(Params params,
                                       std::ostream& output) const
{
    //get the person's name from user's input
    std::string thisPersonName = params.at(0);
    //search the person in the dataset by name
    Person* thisPerson = getPointer(thisPersonName);
    //if not found (getPointer return nullptr)
    if(!thisPerson){
        printNotFound(thisPersonName, output);
        return;
    }
    //not for finding the shortest ==> for tallest
    bool isForShortest = false;
    //initialize the tallest person's name
    std::string resultName = "";
    //initialize the height of the tallest person
    int resultHeight = NO_HEIGHT;
    //collect the data of height and name for the tallest person
    collectHeightResult(thisPersonName, resultName, resultHeight,
                        isForShortest);
    //print the data
    printHeightResult(thisPersonName, resultName, resultHeight,
                      isForShortest, output);
}

void Familytree::printShortestInLineage(Params params,
                                        std::ostream& output) const
{
    //get the person's name from user's input
    std::string thisPersonName = params.at(0);
    //search the person in the dataset by name
    Person* thisPerson = getPointer(thisPersonName);
    //if not found (getPointer return nullptr)
    if(!thisPerson){
        printNotFound(thisPersonName, output);
        return;
    }

    bool isForShortest = true;
    //initialize the shortest person's name
    std::string resultName = "";
    //initialize the height of the shortest person
    int resultHeight = NO_HEIGHT;
    //collect the data of height and name for the shortest person
    collectHeightResult(thisPersonName, resultName, resultHeight,
                        isForShortest);
    //print the data
    printHeightResult(thisPersonName, resultName, resultHeight,
                      isForShortest, output);
}

void Familytree::printGrandChildrenN(Params params, std::ostream& output) const
{
    /* set the group name to help systematic message print
     * and more readable */
    std::string groupName = "grandchildren";
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
    //get the searching depth from the user's input
    int depth = stoi(params.at(1));
    //level should always be no less than 1 (children is 0)
    if(depth < 1){
        output << WRONG_LEVEL << std::endl;
        return;
    }
    //collect the data of all the members with certain depth
    collectRelationsWithDepth(thisPersonName, namelist,
                              "CHILD_DIRECTION", depth);
    //print the data
    /* using depth - 1 is because:
     * depth = 1 is for grandchildren, but it is still the
     * similar group to other group like children
     * so using depth - 1 = 0 when printing the result
     * will not trigger the "great-" string */
    printGroup(thisPersonName, groupName, namelist, output, depth);
}

void Familytree::printGrandParentsN(Params params, std::ostream& output) const
{
    /* set the group name to help systematic message print
     * and more readable */
    std::string groupName = "grandparents";
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
    //get the searching depth from the user's input
    int depth = stoi(params.at(1));
    //level should always be no less than 1 (parent is 0)
    if(depth < 1){
        output << WRONG_LEVEL << std::endl;
        return;
    }
    //collect the data of all the members with certain depth
    collectRelationsWithDepth(thisPersonName, namelist,
                              "PARENT_DIRECTION", depth);
    //print the data
    /* using depth - 1 is because:
     * depth = 1 is for grandchildren, but it is still the
     * similar group to other group like children
     * so using depth - 1 = 0 when printing the result
     * will not trigger the "great-" string */
    printGroup(thisPersonName, groupName, namelist, output, depth);
}

//======== below are private functions ========

Person *Familytree::getPointer(const std::string& id) const
{
    //if person not found
    if(peopleMap_.find(id) == peopleMap_.end()){
        return nullptr;
    }
    //else return the Person pointer of this person
    return peopleMap_.at(id).get();
}

void Familytree::printNotFound(const std::string& id,
                               std::ostream& output) const
{
    output << "Error. " << id << " not found." << std::endl;
}

IdSet Familytree::vectorToIdSet(const std::vector<Person*>& container) const
{
    IdSet idList = {};
    /* with the given container contains members' pointers,
     * if the pointer is valid (not nullptr),
     * then the id(string) of this person can be added to the idSet*/
    for(auto person:container){
        if(person){
            idList.insert(person->id_);
        }
    }
    return idList;
}

void Familytree::printGroup(const std::string& id, const std::string& group,
                            const IdSet& container, std::ostream& output,
                            const int depth) const
{
    //no id in the container ==> no result in previous searching
    if(container.empty()){
        output << id << " has no ";
        /* when depth = 1, it is searching grandchildren/grandparents,
         * still no need to use any "great-" string.
         * So this branch should start from depth = 2
         * and print one "great-" */
        for(int i = 0; i < depth - 1; i++){
            output << "great-";
        }
        output << group << "." << std::endl;
        return;
    }
    /* when there is at least one id:
     * the size of the container is the amount of target members */
    output << id << " has " << container.size() << " " ;
    /* when depth = 1, it is searching grandchildren/grandparents,
     * still no need to use any "great-" string.
     * So this branch should start from depth = 2
     * and print one "great-" */
    for(int i = 0; i < depth - 1; i++){
        output << "great-";
    }
    output << group << ":" << std::endl;
    //print every member's name
    for(auto& memberName:container){
        output << memberName << std::endl;
    }
}

//======== below are my addition private functions ========

void Familytree::collectDescendants(const std::string& id,
                                    IdSet& descendantsList) const
{
    //get the pointer with the name of the person in current round
    Person* eachPerson = getPointer(id);
    //when it is nullptr ==> last round is the end of the branch of tree
    if(!eachPerson){
        return;
    }
    //in the children group of current person
    for(auto& child:eachPerson->children_){
        //if the element inside the children group is valid
        if(child){
            //then the element is one of the descendant
            descendantsList.insert(child->id_);
            //recurssively find the children of each child and so on...
            collectDescendants(child->id_, descendantsList);
        }
    }
}

void Familytree::collectRelationsWithDepth(const std::string& id,
                                             IdSet& membersList,
                                             std::string direction,
                                             int maxDepth,
                                             int currentDepth) const
{
    //if the search exceeds the depth, then stop
    if(currentDepth > maxDepth){
        return;
    }
    //get the pointer with the name of the person in current round
    Person* eachPerson = getPointer(id);
    //when it is nullptr ==> last round is the end of the branch of tree
    if(!eachPerson){
        return;
    }

    std::vector<Person*> membersToSearch;
    if(direction == "PARENT_DIRECTION"){
        membersToSearch = eachPerson->parents_;
    }
    else{
        membersToSearch = eachPerson->children_;
    }

    for(auto& member:membersToSearch){
        if(member){
            if(currentDepth == maxDepth){
                membersList.insert(member->id_);
            }
            else{
                collectRelationsWithDepth(member->id_, membersList,
                                          direction,
                                          maxDepth, currentDepth + 1);
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

void Familytree::collectHeightResult(const std::string& thisPerson_name,
                                     std::string& resultName,
                                     int& resultHeight,
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

//for personal test ONLY
void Familytree::testTEST(std::ostream& output, std::string location)
{
    //count the line order of datafile
    static int counter = 1;
    output << "No." << counter << " " << location << " info added." << std::endl;
    counter++;
}
