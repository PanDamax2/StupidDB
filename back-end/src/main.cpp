#include <iostream>
#include <cstring>
#include <filesystem>

#include "../headers/db.hpp"

using namespace std;
namespace fs = std::filesystem;

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

    Table* uwu = db->tables[db->searchTableByName("UwU")];
    uwu->readStructure();
    uwu->createCol({1, "id", 4, DATA_TYPE_INT});
    uwu->createCol({1, "name", 512, DATA_TYPE_VARCHAR});
    uwu->createCol({1, "age", 4, DATA_TYPE_INT});

    uwu->deleteCol(uwu->searchColByName("name"));

    uwu->writeStructure();


    delete db;
}