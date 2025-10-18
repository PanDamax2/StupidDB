#ifndef DB_H
#define DB_H

#include <cstdint>
#include <string>

#include "../headers/table.hpp"

#define DB_STRUCTURE_FILE "/db.stdb"

//WARMING: CHANGING THIS VALUES MAY DESTROY DATABASE
#define MAX_TABLE_NAME 512
#define MAX_TABLES_COUNT 32

using namespace std;

class DBheader {
public:
    uint8_t signature[4];
    uint32_t tablesCount;

    DBheader();
};

class DBtable {
public:
    uint8_t isOccupied;
    char name[MAX_TABLE_NAME];
};

class DBStructure {
protected:
    DBheader header;
    DBtable dbTables[MAX_TABLES_COUNT];

    string db_structure_file;
public:
    void writeStructure();
    void readStructure();
};

class DB: public DBStructure {
public:
    Table *tables[MAX_TABLES_COUNT];
    string db_location;
    DB(string db_location);
    ~DB();
    void init();
    int createTable(string name);
    void deleteTable(int tableID);
    int searchTableByName(string name);
};

#endif