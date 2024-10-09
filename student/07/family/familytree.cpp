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
        output << ALREADY_ADDED << endl;
        return;
    }
    shared_ptr<Person> thisPerson
            = make_shared<Person>(Person{id, height, {nullptr, nullptr}, {}});
    people_map_.insert({{id, thisPerson}});

    //test
    //testTEST(output, "addNewPerson");
}

void Familytree::addRelation(const string &child,
                             const vector<string> &parents, ostream &output)
{//what to do in this function with the output stream??
    Person* parent_1 = nullptr;
    Person* parent_2 = nullptr;
    //if the parents added as members of this familytree
    if(people_map_.find(parents[0]) != people_map_.end()){
            parent_1 = people_map_.at(parents[0]).get();
    }
    if(people_map_.find(parents[1]) != people_map_.end()){
            parent_2 = people_map_.at(parents[1]).get();
    }
    //find the person and add the parents relation
    shared_ptr<Person> child_to_add = nullptr;
    for(auto& thisPerson:people_map_){
        if(thisPerson.first == child){
            child_to_add = thisPerson.second;
            child_to_add->parents_ = {parent_1, parent_2};
            break;
        }
    }
    /* after adding relation this round, if child_to_add is nullptr
     * it means the child does not exist...
     * thus some error happens to the datafile */
    if(!child_to_add){
        printNotFound(child, output);
        return;
    }
    //find the parent of that person and add children relation
    for(auto parent:{parent_1, parent_2}){
        if(parent){
            people_map_.at(parent->id_)->children_.push_back(child_to_add.get());
        }
    }

    //test
    //testTEST(output, "addRelation");
}

void Familytree::printPersons(Params, ostream &output) const
{
    for(auto& person:people_map_){
        output << person.first << ", " << person.second->height_ << endl;
    }
}

void Familytree::printChildren(Params params, std::ostream &output) const
{
    string groupName = "children";
    string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(is_personNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};

    collectDescendants_with_depth(thisPerson_name, namelist);

    printGroup(thisPerson_name, groupName, namelist, output);
}

void Familytree::printParents(Params params, ostream &output) const
{
    string groupName = "parents";
    string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(is_personNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};

    collectAncestors_with_depth(thisPerson_name, namelist);

    printGroup(thisPerson_name, groupName, namelist, output);
}

void Familytree::printSiblings(Params params, ostream &output) const
{
    string groupName = "siblings";
    string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(is_personNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};
    IdSet namelist_parent = {};

    //find the parents
    collectAncestors_with_depth(thisPerson_name, namelist_parent);

    //find the children of this person's parents
    for(auto& parentName:namelist_parent){
        collectDescendants_with_depth(parentName, namelist);
    }
    //siblings are the children from this person's parent
    //but except this person her/himself
    namelist.erase(thisPerson_name);

    printGroup(thisPerson_name, groupName, namelist, output);
}

void Familytree::printCousins(Params params, std::ostream &output) const
{
    string groupName = "cousins";
    string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(is_personNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};
    IdSet namelist_parents = {};
    IdSet namelist_grandparents = {};
    IdSet namelist_siblings_of_parents = {};

    //find the parents
    collectAncestors_with_depth(thisPerson_name, namelist_parents);

    //find the grandparents
    collectAncestors_with_depth(thisPerson_name, namelist_grandparents, 2);

    //find the siblings of the person's parents
    for(auto& siblings_of_parents_Name:namelist_grandparents){
        collectDescendants_with_depth(siblings_of_parents_Name, namelist_siblings_of_parents);
    }
    //parents' siblings are the children from grandparents
    //but except parents themselves
    for(auto& parentName:namelist_parents){
        namelist_siblings_of_parents.erase(parentName);
    }

    //find the children of the siblings of parent ==> cousins
    for(auto& cousins_Name:namelist_siblings_of_parents){
        collectDescendants_with_depth(cousins_Name, namelist);
    }

    printGroup(thisPerson_name, groupName, namelist, output);
}

void Familytree::printTallestInLineage(Params params, std::ostream &output) const
{
    string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(is_personNotFound(thisPerson_name, thisPerson, params, output)){
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
    string thisPerson_name = "";
    Person* thisPerson = nullptr;
    if(is_personNotFound(thisPerson_name, thisPerson, params, output)){
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
    if(is_personNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};
    int depth = stoi(params.at(1)) + 1;

    collectDescendants_with_depth(thisPerson_name, namelist, depth);

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
    if(is_personNotFound(thisPerson_name, thisPerson, params, output)){
        return;
    }

    IdSet namelist = {};
    int depth = stoi(params.at(1)) + 1;

    collectAncestors_with_depth(thisPerson_name, namelist, depth);

    if(!(depth - 1)){
        output << WRONG_LEVEL << endl;
        return;
    }

    printGroup(thisPerson_name, groupName, namelist, output, depth - 1);

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

void Familytree::collectDescendants_with_depth(const string &id,
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
                collectDescendants_with_depth(child->id_,
                                              descendantsList,
                                              maxDepth, currentDepth + 1);
            }
        }
    }
}

void Familytree::collectAncestors_with_depth(const string &id,
                                             IdSet &ancestorsList,
                                             int maxDepth,
                                             int currentDepth) const
{
    if(currentDepth > maxDepth){
        return;
    }
    Person* eachPerson = people_map_.at(id).get();
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
                collectAncestors_with_depth(parent->id_, ancestorsList,
                                            maxDepth, currentDepth + 1);
            }
        }
    }
}

bool Familytree::is_personNotFound(string& thisPerson_name, Person *&thisPerson,
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
