#ifndef TABLE_H
#define TABLE_H

#include <cstdint>
#include <string>
#include <vector>
#include <variant>

using namespace std;

#define MAX_COLS_COUNT 32
#define MAX_COL_NAME 512

//WARMING: CHANGING THIS VALUES MAY DESTROY TABLE
#define MAX_DATA_HEADERS_COUNT 1048576
#define MAX_DATA_HEADER_SIZE 8192
#define MAX_ROWS_COUNT MAX_DATA_HEADERS_COUNT

#define DATA_TYPE_INT 0
#define DATA_TYPE_FLOAT 1
#define DATA_TYPE_VARCHAR 2
#define DATA_TYPE_BOOL 3

#define STRUCTURE_SIZE (sizeof(TableHeader) + sizeof(TableCol) * MAX_COLS_COUNT + sizeof(TableDataHeader) * MAX_DATA_HEADERS_COUNT + sizeof(int32_t) * MAX_ROWS_COUNT)

typedef int32_t datatype_int;
typedef float datatype_float;
typedef string datatype_varchar;
typedef bool datatype_bool;

using dataVector = vector<variant<datatype_int, datatype_float, datatype_varchar, datatype_bool>>;
using dataMatrix = vector<dataVector>;

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
    uint32_t colID;
    uint64_t start;
    uint32_t size;
};

class TableHeader {
public:
    uint8_t signature[4];
    uint32_t colsCount;
    uint32_t tableHeadersCount;
    uint32_t rowsCount;
    uint32_t currentRowID;

    TableHeader();
};

class Table {
    TableHeader header;
    TableDataHeader *dataHeaders;
    uint8_t *data;
    size_t dataSize;
public:
    TableCol cols[MAX_COLS_COUNT];
    string table_file;
    int32_t *rows;
    Table(string table_file);
    ~Table();
    void init();
    void writeStructure();
    void readStructure();
    int createCol(TableCol col);
    void deleteCol(int colID);
    int searchColByName(string name);
    int createRow();
    void deleteRow(int rowID);
    void readData();
    void writeData();
    void insert(dataVector rowData);
    void insertMultipleRows(dataMatrix rowsData);
    dataVector select(vector<uint32_t> colIDs, uint32_t rowID);
    dataMatrix selectMultipleRows(vector<uint32_t> colIDs, vector<uint32_t> rowIDs);
    dataMatrix selectAll();
};

#endif