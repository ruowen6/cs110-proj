#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <map>
#include <vector>
#include <set>
#include <algorithm>

#define NONE_STOCK_SIGN "out-of-stock"


using namespace std;

struct Product {
    string product_name;
    double price;
};

using MarketData = map<string, map<string, vector<Product>>>;

bool read_success(MarketData* allData, set<string>* productList);
int read_cmd_and_varNum(string* cmd_0, string* cmd_1, string* cmd_2, string* cmd_border);
double find_cheapest_price(MarketData* allData, vector<pair<string, string>>* cheapestList, string productName);

void print_all(MarketData* allData);




int main(void){
    MarketData allDataStored;
    set<string> allProducts;
    bool readStatusSuccess = read_success(&allDataStored, &allProducts);

    if(!readStatusSuccess){return EXIT_FAILURE;}

    while (readStatusSuccess) {
        cout << "> ";
        string command, cmd_1, cmd_2, cmd_border;
        int amountOfVar = read_cmd_and_varNum(&command, &cmd_1, &cmd_2, &cmd_border);

        if(command == "quit"){
            if(amountOfVar != 0){cout << "Error: error in command " << command << endl;}
            else{return EXIT_SUCCESS;}
        }
        else if (command == "products"){
            if(amountOfVar != 0){cout << "Error: error in command " << command << endl;}
            else{
                for(auto product:allProducts){
                    cout << product << endl;
                }
            }
        }
        else if (command == "chains"){
            if(amountOfVar != 0){cout << "Error: error in command " << command << endl;}
            else{
                for(auto chain:allDataStored){
                    cout << chain.first << endl;
                }
            }
        }
        else if (command == "stores"){
            if(amountOfVar != 1){cout << "Error: error in command " << command << endl;}
            else if(allDataStored.find(cmd_1) == allDataStored.end()){
                cout << "Error: unknown chain name" << endl;
            }
            else{
                for(auto stores:allDataStored.at(cmd_1)){
                    cout << stores.first << endl;
                }
            }
        }
        else if (command == "cheapest"){
            if(amountOfVar != 1){cout << "Error: error in command " << command << endl;}
            else if(allProducts.find(cmd_1) == allProducts.end()){
                cout << "The product is not part of product selection" << endl;
            }
            else{
                vector<pair<string, string>> cheapestList;
                double price = find_cheapest_price(&allDataStored, &cheapestList, cmd_1);
                if(price == -1.0){
                    cout << "The product is temporarily out of stock everywhere" << endl;
                }
                else{
                    cout << fixed << setprecision(2) << price << " " << "euros" << endl;
                    for(auto eachStore:cheapestList){
                        cout << eachStore.first << " " << eachStore.second << endl;
                    }
                }
            }
        }
        else if (command == "selection"){
            if(amountOfVar != 2){cout << "Error: error in command " << command << endl;}
            else if(allDataStored.find(cmd_1) == allDataStored.end()){
                cout << "Error: unknown chain name" << endl;
            }
            else if(allDataStored.at(cmd_1).find(cmd_2) == allDataStored.at(cmd_1).end()){
                cout << "Error: unknown store" << endl;
            }
            else{
                for(auto products:allDataStored.at(cmd_1).at(cmd_2)){
                    cout << products.product_name << " ";
                    if(products.price == -1.0){cout << "out of stock" << endl;}
                    else{cout << fixed << setprecision(2) << products.price << endl;}
                }
            }
        }
        //this cmd is only for test
        //else if (command == "printall"){print_all(&allDataStored);}
        else{cout << "Error: unknown command: " << command << endl;}
    }
    return 0;
}

//process the input csv data; error, cout error message
//and assign the value to the dataset
bool read_success(MarketData* allData, set<string>* productList){
    string inputFName;
    cout << "Input file: ";
    getline(cin, inputFName);
    ifstream listFileOB(inputFName);
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

double find_cheapest_price(MarketData* allData, vector<pair<string, string>>* cheapestList, string productName){
    double lowestPrice = -1.0;
    for(auto chains:*allData){
        for(auto stores:chains.second){
            for(auto products:stores.second){
                if(products.product_name == productName){
                    if((lowestPrice == -1.0)
                            || (lowestPrice > products.price && products.price != -1.0)){
                        lowestPrice = products.price;
                    }
                }
            }
        }
    }
    for(auto chains:*allData){
        for(auto stores:chains.second){
            for(auto products:stores.second){
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
    for(auto chain:*allData){
        cout << chain.first << endl;
        for(auto store:chain.second){
            cout << "    " << store.first << endl;
            for(auto products:store.second){
                if(products.price == -1){
                    cout << "        " << products.product_name << "  \t" << NONE_STOCK_SIGN << endl;
                }
                else{
                    cout << "        " << products.product_name << "  \t"
                            << fixed << setprecision(2) << products.price << endl;
                }
            }
        }
    }
}
