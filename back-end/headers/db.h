#ifndef DB_H
#define DB_H

#define DB_FILE "db.stdb"

//WARMING: CHANGING THIS VALUES MAY DESTROY DATABASE
#define MAX_TABLE_NAME 512
#define MAX_COL_NAME 512

#define MAX_TABLES_COUNT 1024
#define MAX_COLS_COUNT 	8192
#define MAX_DATA_HEADERS_COUNT 262144

#define CHUNK_SIZE 512
#define MAX_DATA_SIZE 512000

#include <cstdint>

using namespace std;

class DBheader {
    public:
        uint8_t signature[4];
        uint32_t tablesCount;
        uint32_t totalRowsCount;
        uint32_t totalColsCount;
        uint32_t totalDataHeadersCount;

        DBheader();
};

class DBtable {
    public:
        uint8_t isOccupied;
        uint8_t name[MAX_TABLE_NAME];
        uint32_t colsCount;
        uint32_t dataHeadersCount;
        uint32_t rowsCount;
        uint32_t currentRowID;
};

class DBcol {
    public:
        uint8_t isOccupied;
        uint8_t name[MAX_COL_NAME];
        uint32_t size;
        uint8_t datatype;
        uint32_t tablePtr;
};

class DBdataHeader {
    public:
        uint8_t isOccupied;
        uint32_t colPtr;
        uint32_t tablePtr;
        uint32_t rowID;
        uint64_t start;
        uint64_t size;
};

class DBStructure {
    public:
        DBheader header;
        DBtable tables[MAX_TABLES_COUNT];
        DBcol cols[MAX_COLS_COUNT];
        DBdataHeader dataHeaders[MAX_DATA_HEADERS_COUNT];

        void writeStructure();
        void readStructure();
};

class DB: public DBStructure {
    public:
        DB();
};

#endif