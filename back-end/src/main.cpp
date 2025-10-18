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

    Table *uwu = db->tables[db->searchTableByName("UwU")];
    uwu->init();
    uwu->createCol({1, "id", DATA_TYPE_INT_SIZE, DATA_TYPE_INT});
    uwu->createCol({1, "name", 512, DATA_TYPE_VARCHAR});
    uwu->createCol({1, "age", DATA_TYPE_INT_SIZE, DATA_TYPE_INT});
    uwu->createCol({1, "isStudent", DATA_TYPE_BOOL_SIZE, DATA_TYPE_BOOL});

    // uwu->deleteCol(uwu->searchColByName("name"));

    // int row1 = uwu->createRow();
    // int row2 = uwu->createRow();
    // int row3 = uwu->createRow();

    // uwu->deleteRow(row2);

    uwu->writeStructure();

    // uwu->readData();
    // cout << reinterpret_cast<const char*>(uwu->data) << endl;
    // cout << "Data size: " << uwu->dataSize << " bytes" << endl;


    // const char *text = "siusiak!";
    // size_t textSize = strlen(text) + 1;
    // uwu->dataSize = textSize;
    // uwu->data = (uint8_t*)malloc(textSize);
    // memcpy(uwu->data, text, textSize); 
    // uwu->writeData();

    uwu->readData();

    uwu->insert({1, "Klara", 21, true});
    uwu->insert({2, "Maurycy", 45, false});
    uwu->insert({3, "Debil", 25, false});

    uwu->writeStructure();
    uwu->writeData();

    cout << "All operations completed successfully!" << endl;

    delete db;
}

