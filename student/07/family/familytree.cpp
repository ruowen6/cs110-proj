#include "familytree.hh"
#include <iostream>
#include <set>
#include <string>
#include <algorithm>

using namespace std;


Familytree::Familytree()
{
}

void Familytree::addNewPerson(const string &id, int height, ostream &output)
{
    //if not first add
    if(getPointer(id)){
        output << ALREADY_ADDED << endl;
        return;
    }
    shared_ptr<Person> thisPerson
            = make_shared<Person>(Person{id, height, {nullptr, nullptr}, {}});
    peopleMap_.insert({{id, thisPerson}});

    //test
    //testTEST(output, "addNewPerson");
}

void Familytree::addRelation(const string &child,
                             const vector<string> &parents, ostream &output)
{//what to do in this function with the output stream??
    Person* parentA = nullptr;
    Person* parentB = nullptr;
    //if the parents added as members of this familytree
    if(peopleMap_.find(parents[0]) != peopleMap_.end()){
            parentA = peopleMap_.at(parents[0]).get();
    }
    if(peopleMap_.find(parents[1]) != peopleMap_.end()){
            parentB = peopleMap_.at(parents[1]).get();
    }
    //find the person and add the parents relation
    shared_ptr<Person> childToAdd = nullptr;
    for(auto& thisPerson:peopleMap_){
        if(thisPerson.first == child){
            childToAdd = thisPerson.second;
            childToAdd->parents_ = {parentA, parentB};
            /* we add relation for one person each round,
             * so once the child is found, we don't need to go through
             * the items remained in this for loop */
            break;
        }
    }
    /* after adding relation this round, if child_to_add is nullptr
     * it means the child does not exist...
     * thus some error happens to the datafile */
    if(!childToAdd){
        printNotFound(child, output);
        return;
    }
    //find the parent of that person and add children relation
    for(auto& parent:{parentA, parentB}){
        if(parent){
            getPointer(parent->id_)->children_.push_back(childToAdd.get());
        }
    }

    //test
    //testTEST(output, "addRelation");
}

void Familytree::printPersons(Params, ostream &output) const
{
    for(auto& person:peopleMap_){
        output << person.first << ", " << person.second->height_ << endl;
    }
}

void Familytree::printChildren(Params params, std::ostream &output) const
{
    string groupName = "children";
    string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(isPersonNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};

    collectDescendantsWithDepth(thisPerson_name, namelist);

    printGroup(thisPerson_name, groupName, namelist, output);
}

void Familytree::printParents(Params params, ostream &output) const
{
    string groupName = "parents";
    string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(isPersonNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};

    collectAncestorsWithDepth(thisPerson_name, namelist);

    printGroup(thisPerson_name, groupName, namelist, output);
}

void Familytree::printSiblings(Params params, ostream &output) const
{
    string groupName = "siblings";
    string thisPerson_name = "";
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
    string groupName = "cousins";
    string thisPerson_name = "";
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

void Familytree::printTallestInLineage(Params params, ostream &output) const
{
    string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(isPersonNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist;
    collectDescendants(thisPerson_name, namelist);
    int thisPerson_height = peopleMap_.at(thisPerson_name)->height_;
    int tallestPerson_height = thisPerson_height;
    //find the tallest height
    for(auto& eachDescendant:namelist){
        int eachDescendant_height = peopleMap_.at(eachDescendant)->height_;
        if(tallestPerson_height < eachDescendant_height){
            tallestPerson_height = eachDescendant_height;
        }
    }
    //find the tallest person
    string tallestPerson_name = thisPerson_name;
    for(auto& eachDescendant:namelist){
        int eachDescendant_height = peopleMap_.at(eachDescendant)->height_;
        if(eachDescendant_height == tallestPerson_height){
            tallestPerson_name = eachDescendant;
        }
    }
    if(tallestPerson_name == thisPerson_name){
        output << "With the height of " << tallestPerson_height << ", "
                << thisPerson_name
                << " is the tallest person in his/her lineage." << endl;
    }
    else{
        output << "With the height of " << tallestPerson_height << ", "
                << tallestPerson_name
                << " is the tallest person in "
                << thisPerson_name << "'s lineage." << endl;
    }
}

void Familytree::printShortestInLineage(Params params, ostream &output) const
{
    string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(isPersonNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist;
    collectDescendants(thisPerson_name, namelist);
    int thisPerson_height = peopleMap_.at(thisPerson_name)->height_;
    int shortestPerson_height = thisPerson_height;
    //find the shortest height
    for(auto& eachDescendant:namelist){
        int eachDescendant_height = peopleMap_.at(eachDescendant)->height_;
        if(shortestPerson_height > eachDescendant_height){
            shortestPerson_height = eachDescendant_height;
        }
    }
    //find the shortest person
    string shortestPerson_name = thisPerson_name;
    for(auto& eachDescendant:namelist){
        int eachDescendant_height = peopleMap_.at(eachDescendant)->height_;
        if(eachDescendant_height == shortestPerson_height){
            shortestPerson_name = eachDescendant;
        }
    }
    if(shortestPerson_name == thisPerson_name){
        output << "With the height of " << shortestPerson_height << ", "
                << thisPerson_name
                << " is the shortest person in his/her lineage." << endl;
    }
    else{
        output << "With the height of " << shortestPerson_height << ", "
                << shortestPerson_name
                << " is the shortest person in "
                << thisPerson_name << "'s lineage." << endl;
    }
}

void Familytree::printGrandChildrenN(Params params, std::ostream &output) const
{
    string groupName = "grandchildren";
    string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(isPersonNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};
    int depth = stoi(params.at(1)) + 1;

    collectDescendantsWithDepth(thisPerson_name, namelist, depth);

    if(!(depth - 1)){
        output << WRONG_LEVEL << endl;
        return;
    }

    printGroup(thisPerson_name, groupName, namelist, output, depth - 1);

}

void Familytree::printGrandParentsN(Params params, std::ostream &output) const
{
    string groupName = "grandparents";
    string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(isPersonNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};
    int depth = stoi(params.at(1)) + 1;

    collectAncestorsWithDepth(thisPerson_name, namelist, depth);

    if(!(depth - 1)){
        output << WRONG_LEVEL << endl;
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
    output << "Error. " << id << " not found." << endl;
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
        output << group << "." << endl;
        return;
    }
    output << id << " has " << container.size() << " " ;
    if(depth - 1){
        for(int i = depth - 1; i > 0; i--){
            output << "great-";
        }
    }
    output << group << ":" << endl;

    for(auto& memberName:container){
        output << memberName << endl;
    }
}

//below are funcs i set myself

void Familytree::collectDescendants(const string &id, IdSet &descendantsList) const
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

void Familytree::collectDescendantsWithDepth(const string &id,
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

void Familytree::collectAncestorsWithDepth(const string &id,
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

bool Familytree::isPersonNotFound(string& thisPerson_name, Person *&thisPerson,
                                   Params params, ostream& output) const
{
    thisPerson_name = params.at(0);
    thisPerson = getPointer(thisPerson_name);
    if(!thisPerson){
        printNotFound(thisPerson_name, output);
        /* if not found,
         * the outer if statement using this function will be true
         * which means the person not found
         * then the branch would be quitted */
        return true;
    }
    return false;
}

void Familytree::testTEST(ostream &output, string type)
{
    static int counter = 1;
    output << "No." << counter << " " << type << " info added." << endl;
    counter++;
}
