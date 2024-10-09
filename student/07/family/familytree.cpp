#include "familytree.hh"
#include <iostream>
#include <set>
#include <string>

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

void Familytree::addRelation(const string &child, const vector<string> &parents, ostream &output)
{
    //for test
    static int counter = 1;
    output << "No." << counter << " relation start" << endl;

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
    if(people_map_.find(thisPerson_name) == people_map_.end()){
        output << "Error. " << thisPerson_name << " not found." << endl;
        return;
    }
    shared_ptr<Person> thisPerson = people_map_.at(thisPerson_name);

    auto childrenSet = thisPerson->children_;
    int childrenAmount = childrenSet.size();

    if(!childrenAmount){
        output << thisPerson_name << "has no children." << endl;
        return;
    }
    output << thisPerson_name << "has " << childrenAmount << " children:" << endl;
    for(auto child:childrenSet){
        output << child->id_ << endl;
    }
}

void Familytree::printParents(Params params, ostream &output) const
{
    string thisPerson_name = params.at(0);
    if(people_map_.find(thisPerson_name) == people_map_.end()){
        output << "Error. " << thisPerson_name << " not found." << endl;
        return;
    }
    shared_ptr<Person> thisPerson = people_map_.at(thisPerson_name);

    auto parentSet = thisPerson->parents_;
    int parentAmount = 0;
    for(auto parent:parentSet){
        if(!parent){
            continue;
        }
        parentAmount++;
    }

    if(!parentAmount){
        output << thisPerson_name << "has no parents." << endl;
        return;
    }
    output << thisPerson_name << "has " << parentAmount << " parents:" << endl;
    for(auto parent:parentSet){
        if(!parent){
            continue;
        }
        output << parent->id_ << endl;
    }
}

void Familytree::printSiblings(Params params, ostream &output) const
{

}

void Familytree::printCousins(Params params, std::ostream &output) const
{

}

void Familytree::printTallestInLineage(Params params, std::ostream &output) const
{

}

void Familytree::printShortestInLineage(Params params, std::ostream &output) const
{

}

void Familytree::printGrandChildrenN(Params params, std::ostream &output) const
{

}

void Familytree::printGrandParentsN(Params params, std::ostream &output) const
{

}
