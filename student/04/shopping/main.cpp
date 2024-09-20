/* Chain stores
 *
 * Desc:
 *   This program reads input data concerning chain stores from an input
 * file, the lines of which are of the form:
 * chain_store;store_location;product_name;product_price.
 * The program checks if each line consists of the abovementioned four
 * data fields.
 *   The data written in the file is collected into an appropriate data
 * structure consisting of STL containers and structs. After that the user
 * can make searches for the data structure by using the commands
 * chains, stores, selection, cheapest, and products. These commands
 * have a varying number of parameters, and the program checks if the
 * user gave a correct number of parameters and if the given parameters
 * (chain store, store location, product name) can be found from the data
 * structure.
 *   The program terminates with the command quit.
 *
 * Program author
 * Name: Ruowen Liu
 * Student number: 152273523
 * UserID: fvruli
 * E-Mail: ruowen.liu@tuni.fi
 *
 * Notes about the program and it's implementation (if any):
 *
 * */

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <map>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

struct Product {
    string product_name;
    double price;

    //operator==(par) par.productname == productnam
};
/* The data structure here is used to steore all the data from the csv file.
 * It can be interpreted in this way:
 * map<chainName, map<locationName, vector<eachProduct> > >
 * */
using MarketData = map<string, map<string, vector<Product> > >;
/**
 * @brief read_success - read csv file inside the function
 *        and store the data to the datasets;
 *        meanwhile, it print out the error message
 *        when the file failed opened or when data missing
 * @param allData      - all data read from csv file
 * @param productList  - the dataset for only product names
 * @return a boolean value telling the status of reading result;
 *         only when error or data-missing happens, return false
 */
bool read_success(MarketData* allData, set<string>* productList);
/**
 * @brief read_cmd_and_varNum - read command from user;
 *        split the command by the space;
 *        identify each variable of the command
 *        pattern:
 *
 *          when user type the following line:
 *         > selection Prisma Kaleva everything
 *          | cmd_0   |cmd_1 |cmd_2 |cmd_border|
 *
 * @param cmd_0      - 1st part without space in the string;
 *        identified as the stem of the command
 * @param cmd_1      - 2nd part without space in the string;
 *        identified as the 1st variable to the command
 * @param cmd_2      - 3rd part without space in the string;
 *        identified as the 2nd variable to the command
 * @param cmd_border - 4th part without space in the string;
 *        identified as the unwanted variable to the command
 *        (max 2 variables needed)
 * @return the amount of variable to one command
 */
int read_cmd_and_varNum(string* cmd_0, string* cmd_1, string* cmd_2, string* cmd_border);
/**
 * @brief find_cheapest_price - based on all the data, with given target product name,
 *        the func. searches the lowest price
 *        and record the store list for the location
 *        where the target product set with the lowest price
 * @param allData             - all data read from csv file
 * @param cheapestList        - a vector made with pair<chainName, location>
 * @param productName         - product we are searching for the lowset price
 * @return the lowest price we found; if all out-of-stock, return -1.0 for identifying
 */
double find_cheapest_price(MarketData* allData, vector<pair<string, string> >* cheapestList, string productName);

//a test function; print all the data formatted. Not required in this project.
void print_all(MarketData* allData);

int main(void){
    MarketData allDataStored;
    set<string> allProducts;
    //read the file and receive the file-reading status
    bool readStatusSuccess = read_success(&allDataStored, &allProducts);
    if(!readStatusSuccess){return EXIT_FAILURE;}
    while (readStatusSuccess) {
        cout << "> ";
        string command, cmd_1, cmd_2, cmd_border;
        /*get the num of non-empty strings after command
         *thus it's the amount of variable */
        int amountOfVar = read_cmd_and_varNum(&command, &cmd_1, &cmd_2, &cmd_border);

        if(command == "quit"){
            /*cmd "quit" directly terminates the program
             *thus should have no variable
             *(cmd_1, cmd_2, and cmd_border should be empty) */
            if(amountOfVar != 0){cout << "Error: error in command " << command << endl;}
            else{return EXIT_SUCCESS;}
        }
        else if (command == "products"){
            /*cmd "products" directly print out all products
             *regardless of the chain or location
             *thus should have no variable
             *(cmd_1, cmd_2, and cmd_border should be empty) */
            if(amountOfVar != 0){cout << "Error: error in command " << command << endl;}
            else{
                //product names were directly stored in (set)allProducts
                for(auto &product:allProducts){
                    cout << product << endl;
                }
            }
        }
        else if (command == "chains"){
            /*cmd "chains" directly print out all chainName
             *regardless of other factors
             *thus should have no variable
             *(cmd_1, cmd_2, and cmd_border should be empty) */
            if(amountOfVar != 0){cout << "Error: error in command " << command << endl;}
            else{
                //chain here is pair<chainName, map<store, vector<product> > >
                for(auto &chain:allDataStored){
                    cout << chain.first << endl;
                }
            }
        }
        else if (command == "stores"){
            /*cmd "stores" prints out all locations of a certain chainName
             *thus should have only 1 variable
             *(cmd_2 and cmd_border should be empty) */
            if(amountOfVar != 1){cout << "Error: error in command " << command << endl;}
            /*cmd_1 here is the target chainName from user
             *if not found in the keys of the map... */
            else if(allDataStored.find(cmd_1) == allDataStored.end()){
                cout << "Error: unknown chain name" << endl;
            }
            else{
                /*stores here is
                 *map<location, vector<product> > under the given chainName */
                for(auto &stores:allDataStored.at(cmd_1)){
                    cout << stores.first << endl;
                }
            }
        }
        else if (command == "cheapest"){
            /*cmd "cheapest" finds out the list of chain-location with given productName
             *thus should have only 1 variable
             *cmd_2 and cmd_border should be empty) */
            if(amountOfVar != 1){cout << "Error: error in command " << command << endl;}
            //(set)allProducts directly stored all occured productName
            else if(allProducts.find(cmd_1) == allProducts.end()){
                cout << "The product is not part of product selection" << endl;
            }
            else{
                //set a vector made by pair<chainName, location>
                vector<pair<string, string> > cheapestList;
                /*receive the lowest price searched by this function;
                 *read the content of allDataStored,
                 *directly change the content of cheapestList*/
                double price = find_cheapest_price(&allDataStored, &cheapestList, cmd_1);
                if(price == -1.0){
                    cout << "The product is temporarily out of stock everywhere" << endl;
                }
                else{
                    //set the format of output figure ( = %.2f)
                    cout << fixed << setprecision(2) << price << " " << "euros" << endl;
                    for(auto &eachStore:cheapestList){
                        cout << eachStore.first << " " << eachStore.second << endl;
                    }
                }
            }
        }
        else if (command == "selection"){
            /*cmd "selection" finds out the all the products
             *with given chainName(cmd_1) and location(cmd_2)
             *thus should have only 2 variables
             *(cmd_border should be empty) */
            if(amountOfVar != 2){cout << "Error: error in command " << command << endl;}
            //when chainName(cmd_1) can't be found
            else if(allDataStored.find(cmd_1) == allDataStored.end()){
                cout << "Error: unknown chain name" << endl;
            }
            //when location(cmd_2) can't be found
            else if(allDataStored.at(cmd_1).find(cmd_2) == allDataStored.at(cmd_1).end()){
                cout << "Error: unknown store" << endl;
            }
            else{
                //products here is vector<Product>
                for(auto products:allDataStored.at(cmd_1).at(cmd_2)){
                    cout << products.product_name << " ";
                    if(products.price == -1.0){cout << "out of stock" << endl;}
                    //set the format of the figure ( = %.2f)
                    else{cout << fixed << setprecision(2) << products.price << endl;}
                }
            }
        }
        //this cmd "printall" branch is only for test...
        //else if (command == "printall"){print_all(&allDataStored);}

        //all other cmd stems are unknown; then we wait for next input from user
        else{cout << "Error: unknown command: " << command << endl;}
    }
    return 0;
}

/* process the input csv data; when error, cout error message
 * at the sametime, assign the value to the dataset
 * finally, return boolean value to tell the status of data-reading */
bool read_success(MarketData* allData, set<string>* productList){
    string inputFName;
    cout << "Input file: ";
    getline(cin, inputFName);
    ifstream listFileOB(inputFName);
    //when the input filename doesn't exist
    if(!listFileOB){
        cout << "Error: the input file cannot be opened" << endl;
        return false;
    }
    string eachLine;
    while(getline(listFileOB, eachLine)){
        string eachField;
        stringstream lineStream(eachLine);
        string chainName, storeName, pName, pPriceStr;
        getline(lineStream, chainName, ';');
        getline(lineStream, storeName, ';');
        getline(lineStream, pName, ';');
        getline(lineStream, pPriceStr, ';');

        if(chainName.empty() || storeName.empty() || pName.empty() || pPriceStr.empty()) {
            cout << "Error: the input file has an erroneous line" << endl;
            return false;}
        if(chainName.find(' ') != string::npos || storeName.find(' ') != string::npos
                || pName.find(' ') != string::npos || pPriceStr.find(' ') != string::npos) {
            cout << "Error: the input file has an erroneous line" << endl;
            return false;}

        //make the product list
        productList->insert(pName);

        double pPriceDouble;
        if(pPriceStr == "out-of-stock"){pPriceDouble = -1.0;}
        else{pPriceDouble = stod(pPriceStr);}

        Product eachProduct = {pName, pPriceDouble};
        //chainName hasn't been stored
        if(allData->find(chainName) == allData->end()){
            allData->insert({chainName, {{storeName, {eachProduct}}}});
        }
        //chainName has been stored
        else{
            //...but storeName hasn't been stored
            if(allData->at(chainName).find(storeName) == allData->at(chainName).end()){
                allData->at(chainName).insert({storeName, {eachProduct}});
            }
            //storeName has been stored
            else{
                //...but we don't know if this product has price history
                // Check if the product already exists
                bool productFound = false;
                for (auto& product : allData->at(chainName).at(storeName)) {
                    if (eachProduct.product_name == product.product_name) {
                        product.price = eachProduct.price;  // Update the price
                        productFound = true;
                        break;
                    }
                }
                // If the product was not found, add it
                if (!productFound) {
                    allData->at(chainName).at(storeName).push_back(eachProduct);
                }
            }
        }
    }
    listFileOB.close();
    //finally, sort the vector by productName
    for(auto& chainName:*allData){
        for(auto& storeName:chainName.second){
            std::sort(storeName.second.begin(), storeName.second.end(), [](const Product& a, const Product& b){
                return a.product_name < b.product_name;
            });
        }
    }
    return true;
}

int read_cmd_and_varNum(string* cmd_0, string* cmd_1, string* cmd_2, string* cmd_border){
    string lineCMD;
    getline(cin, lineCMD);
    stringstream streamCMD(lineCMD);

    streamCMD >> *cmd_0;
    streamCMD >> *cmd_1;
    streamCMD >> *cmd_2;
    getline(streamCMD, *cmd_border);

    if(!(*cmd_border).empty()){return 9;}
        else if(!(*cmd_2).empty()){return 2;}
        else if(!(*cmd_1).empty()){return 1;}
        else{return 0;}
}

double find_cheapest_price(MarketData* allData, vector<pair<string, string> >* cheapestList, string productName){
    double lowestPrice = -1.0;
    for(auto& chains:*allData){
        for(auto& stores:chains.second){
            for(auto& products:stores.second){
                if(products.product_name == productName){
                    if((lowestPrice == -1.0)
                            || (lowestPrice > products.price && products.price != -1.0)){
                        lowestPrice = products.price;
                    }
                }
            }
        }
    }
    for(auto& chains:*allData){
        for(auto& stores:chains.second){
            for(auto& products:stores.second){
                if(products.price == lowestPrice && lowestPrice != -1.0){
                    cheapestList->push_back({chains.first, stores.first});
                }
            }
        }
    }
    return lowestPrice;
}

void print_all(MarketData* allData){
    cout << "Here are the list of all products in all supermarkets:" << endl << endl;
    for(auto& chain:*allData){
        cout << chain.first << endl;
        for(auto& store:chain.second){
            cout << "    " << store.first << endl;
            for(auto& products:store.second){
                if(products.price == -1){
                    cout << "        " << products.product_name << "  \t" << "out-of-stock" << endl;
                }
                else{
                    cout << "        " << products.product_name << "  \t"
                            << fixed << setprecision(2) << products.price << endl;
                }
            }
        }
    }
}
