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
    if(people_map_.find(id) != people_map_.end()){
        output << "Error. Person already added." << endl;
        return;
    }
    shared_ptr<Person> thisPerson
            = make_shared<Person>(Person{id, height, {nullptr, nullptr}, {}});
    people_map_.insert({{id, thisPerson}});

    //test
    static int counter = 1;
    output << "No." << counter << " person added." << endl;
    counter++;
}

void Familytree::addRelation(const string &child,
                             const vector<string> &parents, ostream &output)
{
    Person* parent_1 = nullptr;
    Person* parent_2 = nullptr;
    if(people_map_.find(parents[0]) != people_map_.end()){
            parent_1 = people_map_.at(parents[0]).get();
    }
    if(people_map_.find(parents[1]) != people_map_.end()){
            parent_2 = people_map_.at(parents[1]).get();
    }
    //find the person and add the parents relation
    shared_ptr<Person> child_to_add;
    for(auto& thisPerson:people_map_){
        if(thisPerson.first == child){
            child_to_add = thisPerson.second;
            child_to_add->parents_ = {parent_1, parent_2};
        }
    }
    //find the parent of that person and add children relation
    for(auto parent:{parent_1, parent_2}){
        if(parent){
            people_map_.at(parent->id_)->children_.push_back(child_to_add.get());
        }
    }

    //test
    static int counter = 1;
    output << "No." << counter << " relation added." << endl;
    counter++;
}

void Familytree::printPersons(Params, ostream &output) const
{
    for(auto& person:people_map_){
        output << person.first << ", " << person.second->height_ << endl;
    }
}

void Familytree::printChildren(Params params, std::ostream &output) const
{
    string thisPerson_name = params.at(0);
    Person* thisPerson = getPointer(thisPerson_name);
    if(!thisPerson){
        printNotFound(thisPerson_name, output);
        return;
    }

    auto childrenVector = thisPerson->children_;
    IdSet namelist = vectorToIdSet(childrenVector);

    if(namelist.empty()){
        output << thisPerson_name << " has no children." << endl;
        return;
    }
    output << thisPerson_name << " has " << namelist.size() << " children:" << endl;
    for(auto& childName:namelist){
        output << childName << endl;
    }
}

void Familytree::printParents(Params params, ostream &output) const
{
    string thisPerson_name = params.at(0);
    Person* thisPerson = getPointer(thisPerson_name);
    if(!thisPerson){
        printNotFound(thisPerson_name, output);
        return;
    }

    auto parentVector = thisPerson->parents_;
    IdSet namelist = vectorToIdSet(parentVector);

    if(namelist.empty()){
        output << thisPerson_name << " has no parents." << endl;
        return;
    }
    output << thisPerson_name << " has " << namelist.size() << " parents:" << endl;
    for(auto& parentName:namelist){
        output << parentName << endl;
    }
}

void Familytree::printSiblings(Params params, ostream &output) const
{
    string thisPerson_name = params.at(0);
    Person* thisPerson = getPointer(thisPerson_name);
    if(!thisPerson){
        printNotFound(thisPerson_name, output);
        return;
    }

    vector<Person*> siblingsVector = {};
    for(auto& parent:thisPerson->parents_){
        if(!parent){
            continue;
        }
        siblingsVector.insert(siblingsVector.end(),
                              parent->children_.begin(), parent->children_.end());
    }

    IdSet namelist = vectorToIdSet(siblingsVector);
    //siblings are the children from this person's parent but except this person
    namelist.erase(thisPerson_name);

    if(namelist.empty()){
        output << thisPerson_name << " has no siblings." << endl;
        return;
    }
    output << thisPerson_name << " has " << namelist.size() << " siblings:" << endl;
    for(auto& siblingsName:namelist){
        output << siblingsName << endl;
    }
}

void Familytree::printCousins(Params params, std::ostream &output) const
{
    string thisPerson_name = params.at(0);
    Person* thisPerson = getPointer(thisPerson_name);
    if(!thisPerson){
        printNotFound(thisPerson_name, output);
        return;
    }

    vector<Person*> cousinsParentVector = {};
    //for each parent
    for(auto& parent:thisPerson->parents_){
        if(!parent){
            continue;
        }
        //find grandparents (parents of parents)
        for(auto& grandparent:parent->parents_){
            if(!grandparent){
                continue;
            }
            cousinsParentVector.insert(cousinsParentVector.end(),
                                  grandparent->children_.begin(),
                                       grandparent->children_.end());
            //cousins' parents are the siblings of this person's parents
            cousinsParentVector.erase(
                        remove(cousinsParentVector.begin(),
                               cousinsParentVector.end(),
                               parent),
                        cousinsParentVector.end());
        }
    }
    //cousins are cousins' parents' children
    vector<Person*> cousinsVector = {};
    for(auto& cousinsParent:cousinsParentVector){
        cousinsVector.insert(cousinsVector.end(),
                             cousinsParent->children_.begin(),
                             cousinsParent->children_.end());
    }

    IdSet namelist = vectorToIdSet(cousinsVector);

    if(namelist.empty()){
        output << thisPerson_name << " has no cousins." << endl;
        return;
    }
    output << thisPerson_name << " has " << namelist.size() << " siblings:" << endl;
    for(auto& cousinsName:namelist){
        output << cousinsName << endl;
    }
}

void Familytree::printTallestInLineage(Params params, std::ostream &output) const
{
    string thisPerson_name = params.at(0);
    Person* thisPerson = getPointer(thisPerson_name);
    if(!thisPerson){
        printNotFound(thisPerson_name, output);
        return;
    }

    IdSet namelist;
    collectDescendants(thisPerson_name, namelist);
    int thisPerson_height = people_map_.at(thisPerson_name)->height_;
    int tallestPerson_height = thisPerson_height;
    //find the tallest height
    for(auto& eachDescendant:namelist){
        int eachDescendant_height = people_map_.at(eachDescendant)->height_;
        if(tallestPerson_height < eachDescendant_height){
            tallestPerson_height = eachDescendant_height;
        }
    }
    //find the tallest person
    string tallestPerson_name = thisPerson_name;
    for(auto& eachDescendant:namelist){
        int eachDescendant_height = people_map_.at(eachDescendant)->height_;
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

void Familytree::printShortestInLineage(Params params, std::ostream &output) const
{
    string thisPerson_name = params.at(0);
    Person* thisPerson = getPointer(thisPerson_name);
    if(!thisPerson){
        printNotFound(thisPerson_name, output);
        return;
    }

    IdSet namelist;
    collectDescendants(thisPerson_name, namelist);
    int thisPerson_height = people_map_.at(thisPerson_name)->height_;
    int shortestPerson_height = thisPerson_height;
    //find the shortest height
    for(auto& eachDescendant:namelist){
        int eachDescendant_height = people_map_.at(eachDescendant)->height_;
        if(shortestPerson_height > eachDescendant_height){
            shortestPerson_height = eachDescendant_height;
        }
    }
    //find the shortest person
    string shortestPerson_name = thisPerson_name;
    for(auto& eachDescendant:namelist){
        int eachDescendant_height = people_map_.at(eachDescendant)->height_;
        if(eachDescendant_height == shortestPerson_height){
            shortestPerson_name = eachDescendant;
        }
    }
    if(shortestPerson_name == thisPerson_name){
        output << "With the height of " << shortestPerson_height << ", "
                << thisPerson_name
                << " is the tallest person in his/her lineage." << endl;
    }
    else{
        output << "With the height of " << shortestPerson_height << ", "
                << shortestPerson_name
                << " is the tallest person in "
                << thisPerson_name << "'s lineage." << endl;
    }
}

void Familytree::printGrandChildrenN(Params params, std::ostream &output) const
{
    if(0){
        output << params.at(0);
    }
}

void Familytree::printGrandParentsN(Params params, std::ostream &output) const
{
    if(0){
        output << params.at(0);
    }
}

Person *Familytree::getPointer(const std::string &id) const
{
    if(people_map_.find(id) == people_map_.end()){
        return nullptr;
    }
    return people_map_.at(id).get();
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
