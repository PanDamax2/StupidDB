#ifndef TABLE_H
#define TABLE_H

#define MAX_COLS_COUNT 32
#define MAX_COL_NAME 512

//WARMING: CHANGING THIS VALUES MAY DESTROY TABLE
#define MAX_DATA_HEADERS_COUNT 1048576
#define MaAX_DATA_HEADER_SIZE 512

#define DATA_TYPE_INT 0
#define DATA_TYPE_FLOAT 1
#define DATA_TYPE_VARCHAR 2
#define DATA_TYPE_BOOL 3

#include <cstdint>
#include <string>

using namespace std;

class TableCol {
public:
    uint8_t isOccupied;
    char name[MAX_COL_NAME];
    uint32_t size;
    uint8_t datatype;
};

class TableDataHeader {
public:
    uint8_t isOccupied;
    uint32_t rowID;
    uint64_t start;
    uint64_t size;
};

class TableHeader {
public:
    uint8_t signature[4];
    uint32_t colsCount;

    TableHeader();
};

class Table {
    TableHeader header;
    TableCol cols[MAX_COLS_COUNT];
    TableDataHeader *dataHeaders;
public:
    string table_file;
    Table(string table_file);
    ~Table();
    void init();
    void writeStructure();
    void readStructure();
    int createCol(TableCol col);
    void deleteCol(int colID);
    int searchColByName(string name);
};

#endif