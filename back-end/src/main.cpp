#include <iostream>
#include <cstring>
#include <filesystem>
#include <variant>
#include <vector>

#include "../headers/db.hpp"
#include "../headers/table.hpp"
#include "../headers/test.hpp"

using namespace std;
namespace fs = std::filesystem;

void printDataVector(dataVector vec) {
    for(const auto &val : vec) {
        if(holds_alternative<datatype_int>(val))
            cout << get<datatype_int>(val) << "\t";
        else if(holds_alternative<datatype_float>(val))
            cout << get<datatype_float>(val) << "\t";
        else if(holds_alternative<datatype_varchar>(val))
            cout << get<datatype_varchar>(val) << "\t";
        else if(holds_alternative<datatype_bool>(val)) {
            if(get<datatype_bool>(val))
                cout << "true\t";
            else
                cout << "false\t";
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
    db->createTable("people");

    int toDelete = db->searchTableByName("OwO");
    db->deleteTable(toDelete);

    
    db->writeStructure();

    Table *people = db->tables[db->searchTableByName("people")];
    people->init();
    people->createCol({1, "id", 0, DATA_TYPE_INT});
    people->createCol({1, "name", 512, DATA_TYPE_VARCHAR});
    people->createCol({1, "age", 0, DATA_TYPE_INT});
    people->createCol({1, "isStudent",  0, DATA_TYPE_BOOL});

    people->writeStructure();


    people->readData();

    people->insertMultipleRows({{1, "Klara", 21, true}, {2, "Maurycy", 45, false}, {3, "Angel", 25, false}});
    
    people->writeData();
    people->readData();

    // dataVector data = people->select({0, 1, 2, 3}, 2);
    // printDataVector(data);

    // dataMatrix data2 = people->selectMultipleRows({0, 1, 2, 3}, {1, 2, 3});
    // printDataMartix(data2);

    dataVector cols = people->showCols();
    dataMatrix data = people->selectAll();
    printDataVector(cols);
    printDataMartix(data);

    cout << "TABLES:" << endl;
    dataMatrix tables = db->showTables();
    printDataMartix(tables);

    cout << "All operations completed successfully!" << endl;

    test();
    delete db;
}

