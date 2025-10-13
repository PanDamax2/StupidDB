#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <filesystem>

#include "../headers/db.hpp"
#include "../headers/table.hpp"

using namespace std;
namespace fs = std::filesystem;

DBheader::DBheader() {
    signature[0] = 'S';
    signature[1] = 'T';
    signature[2] = 'D';
    signature[3] = 'B';
    tablesCount = 0;;
}

void DBStructure::writeStructure() { 
    ofstream out(db_structure_file, ios::binary);

    if(!out)
        throw runtime_error("DB Structure: Cannot open/create a file");

    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    out.write(reinterpret_cast<const char*>(dbTables), sizeof(dbTables));

    out.close();

}

void DBStructure::readStructure() {
    ifstream in(db_structure_file, ios::binary);

    if(!in)
        throw runtime_error("DB Structure: Cannot open a file");

    
    in.read(reinterpret_cast<char*>(&header), sizeof(header));
    if(header.signature[0] != 'S' || header.signature[1] != 'T' || header.signature[2] != 'D' || header.signature[3] != 'B')
        throw runtime_error("DB Structure: Invalid file format");

    in.read(reinterpret_cast<char*>(dbTables), sizeof(dbTables));

    in.close();
}

DB::DB(string db_location) {
    this->db_location = db_location;
    db_structure_file = db_location + DB_STRUCTURE_FILE;
    header = DBheader();

    for(int i = 0; i < MAX_TABLES_COUNT; i++) {
        dbTables[i] = {};
        tables[i] = {};
    }
}

DB::~DB() {
    for(int i = 0; i < MAX_TABLES_COUNT; i++) {
        if(tables[i]) {
            delete tables[i];
        }
    }
}

void DB::init() {
    if(!fs::exists(fs::path(db_structure_file))) {
        fs::create_directories(db_location);
    }

    ifstream in(db_structure_file, ios::binary);
    if(in) {
        in.close();
        readStructure();
    } else {
        writeStructure();
    }

    for(int i = 0; i < MAX_TABLES_COUNT; i++) {
        if(dbTables[i].isOccupied) {
            string table_file = db_location + "/" + dbTables[i].name + ".sttb";
            cout << dbTables[i].name << endl;
            tables[i] = new Table(table_file);
            tables[i]->init();
        }
    }
}

int DB::createTable(string name) {
    if(header.tablesCount >= MAX_TABLES_COUNT)
        throw runtime_error("DB: Maximum tables count reached");

    if(name.empty())
        throw runtime_error("DB: Table name cannot be empty");
    
    if(name.length() >= MAX_TABLE_NAME)
        throw runtime_error("DB: Table name is too long");
    
    for(int i = 0; i < MAX_TABLES_COUNT; i++) {
        if(dbTables[i].isOccupied && strcmp(dbTables[i].name, name.c_str()) == 0) {
            throw runtime_error("DB: Table with this name already exists");
        }
    }

    for(int i = 0; i < MAX_TABLES_COUNT; i++) {
        if(dbTables[i].isOccupied == 0) {
            dbTables[i].isOccupied = 1;
            strncpy(dbTables[i].name, name.c_str(), MAX_TABLE_NAME);
            header.tablesCount++;

            string table_file = db_location + "/" + name + ".sttb";
            tables[i] = new Table(table_file);
            tables[i]->writeStructure();

            return i;
        }
    }
    throw runtime_error("DB: No space for new table");
}

void DB::deleteTable(int tableID) {
    if(tableID < 0 || tableID >= MAX_TABLES_COUNT)
        throw runtime_error("DB: Invalid table ID");

    if(dbTables[tableID].isOccupied == 0)
        throw runtime_error("DB: Table does not exist");

    dbTables[tableID] = {};
    header.tablesCount--;

    if(tables[tableID]) {
        fs::remove(tables[tableID]->table_file);
        delete tables[tableID];
        tables[tableID] = {};
    }
}

int DB::searchTableByName(string name) {
    for(int i = 0; i < MAX_TABLES_COUNT; i++) {
        if(dbTables[i].isOccupied && strcmp(dbTables[i].name, name.c_str()) == 0) {
            return i;
        }
    }
    return -1;
}

// int DBStructure::createCol(DBcol col) {
//     if(header.totalColsCount >= MAX_COLS_COUNT)
//         throw runtime_error("DB: Maximum columns count reached");

//     if(strlen(col.name) == 0)
//         throw runtime_error("DB: Column name cannot be empty");
    
//     if(strlen(col.name) >= MAX_COL_NAME)
//         throw runtime_error("DB: Column name is too long");
    
//     for(int i = 0; i < MAX_COLS_COUNT; i++) {
//         if(cols[i].isOccupied && strcmp(cols[i].name, col.name) == 0 && cols[i].tableID == col.tableID) {
//             throw runtime_error("DB: Column with this name in table already exists");
//         }
//     }

//     for(int i = 0; i < MAX_COLS_COUNT; i++) {
//         if(cols[i].isOccupied == 0) {
//             cols[i] = col;
//             cols[i].isOccupied = 1;
//             header.totalColsCount++;
//             return i;
//         }
//     }
//     throw runtime_error("DB: No space for new column");
// }

// void DBStructure::deleteCol(int colID) {
//     if(colID < 0 || colID >= MAX_COLS_COUNT)
//         throw runtime_error("DB: Invalid column ID");

//     if(cols[colID].isOccupied == 0)
//         throw runtime_error("DB: Column does not exist");

//     cols[colID] = {};
//     header.totalColsCount--;
// }

