#include <iostream>
#include <cstring>
#include <filesystem>
#include <variant>
#include <vector>

#include "../headers/db.hpp"
#include "../headers/table.hpp"

using namespace std;
namespace fs = std::filesystem;

void printDataVector(dataVector vec) {
    for(const auto &val : vec) {
        if(holds_alternative<datatype_int>(val))
            cout << get<datatype_int>(val) << " ";
        else if(holds_alternative<datatype_float>(val))
            cout << get<datatype_float>(val) << " ";
        else if(holds_alternative<datatype_varchar>(val))
            cout << get<datatype_varchar>(val) << " ";
        else if(holds_alternative<datatype_bool>(val)) {
            if(get<datatype_bool>(val))
                cout << "true ";
            else
                cout << "false ";
        }
    }
    cout << endl;
}

void printDataMartix(dataMatrix mat) {
    for(const auto &vec: mat) {
        printDataVector(vec);
    }
}

int main() {
    if(fs::exists("db"))
        fs::remove_all("db");

    DB *db = new DB("db");
    db->init();   

    db->createTable("OwO");
    db->createTable("UwU");

    int toDelete = db->searchTableByName("OwO");

    db->deleteTable(toDelete);
    
    db->writeStructure();

    Table *uwu = db->tables[db->searchTableByName("UwU")];
    uwu->init();
    uwu->createCol({1, "id", 0, DATA_TYPE_INT});
    uwu->createCol({1, "name", 512, DATA_TYPE_VARCHAR});
    uwu->createCol({1, "age", 0, DATA_TYPE_INT});
    uwu->createCol({1, "isStudent",  0, DATA_TYPE_BOOL});

    // uwu->deleteCol(uwu->searchColByName("name"));

    // int row1 = uwu->createRow();
    // int row2 = uwu->createRow();
    // int row3 = uwu->createRow();

    // uwu->deleteRow(row2);

    uwu->writeStructure();

    // uwu->readData();
    // cout << reinterpret_cast<const char*>(uwu->data) << endl;
    // cout << "Data size: " << uwu->dataSize << " bytes" << endl;


    uwu->readData();

    uwu->insertMultipleRows({{1, "Klara", 21, true}, {2, "Maurycy", 45, false}, {3, "Angel", 25, false}});
    
    uwu->writeData();
    uwu->readData();

    dataVector data = uwu->select({0, 1, 2, 3}, 2);
    printDataVector(data);

    dataMatrix data2 = uwu->selectMultipleRows({0, 1, 2, 3}, {1, 2, 3});
    printDataMartix(data2);

    dataMatrix data3 = uwu->selectAll();
    printDataMartix(data3);

    cout << "All operations completed successfully!" << endl;

    delete db;
}

