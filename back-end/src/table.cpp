#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <string>
#include <filesystem>
#include <variant>
#include <vector>
#include <algorithm>

#include "../headers/table.hpp"

using namespace std;
namespace fs = std::filesystem;

TableHeader::TableHeader() {
    signature[0] = 'S';
    signature[1] = 'T';
    signature[2] = 'T';
    signature[3] = 'B';
    colsCount = 0;
    dataHeadersCount = 0;
    rowsCount = 0;
    currentRowID = 0;
}

Table::Table(string table_file) {
    this->table_file = table_file;
    header = TableHeader();

    dataHeaders = new TableDataHeader[MAX_DATA_HEADERS_COUNT];
    rows = new int32_t[MAX_ROWS_COUNT];

    for(int i = 0; i < MAX_COLS_COUNT; i++)
        cols[i] = {};

    for(int i = 0; i < MAX_DATA_HEADERS_COUNT; i++)
        dataHeaders[i] = {};

    for(int i = 0; i < MAX_ROWS_COUNT; i++)
        rows[i] = -1;
}

Table::~Table() {
    delete[] dataHeaders;
    delete[] rows;
    free(data);
}

void Table::writeStructure() { 
    ofstream out(table_file, ios::binary);

    if(!out)
        throw runtime_error("Table: Cannot open/create a file");

    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    out.write(reinterpret_cast<const char*>(cols), sizeof(cols));
    out.write(reinterpret_cast<const char*>(dataHeaders), sizeof(TableDataHeader) * MAX_DATA_HEADERS_COUNT);
    out.write(reinterpret_cast<const char*>(rows), sizeof(int32_t) * MAX_ROWS_COUNT);

    out.close();
}

void Table::readStructure() {
    ifstream in(table_file, ios::binary);

    if(!in)
        throw runtime_error("Table: Cannot open a file");

    in.read(reinterpret_cast<char*>(&header), sizeof(header));
    if(header.signature[0] != 'S' || header.signature[1] != 'T' || header.signature[2] != 'T' || header.signature[3] != 'B')
        throw runtime_error("Table: Invalid file format");

    in.read(reinterpret_cast<char*>(cols), sizeof(cols));
    in.read(reinterpret_cast<char*>(dataHeaders), sizeof(TableDataHeader) * MAX_DATA_HEADERS_COUNT);
    in.read(reinterpret_cast<char*>(rows), sizeof(int32_t) * MAX_ROWS_COUNT);

    in.close();
}

void Table::init() {
    ifstream in(table_file, ios::binary);
    if(in) {
        in.close();
        readStructure();
    } else {
        writeStructure();
    }
}

void Table::createCols(vector<TableCol> colsToInsert) {
    for(int i = 0; i < MAX_COLS_COUNT; i++)
        cols[i] = {};

    for(const TableCol& col: colsToInsert) {
        if(header.colsCount >= MAX_COLS_COUNT)
        throw runtime_error("Table: Maximum columns count reached");

        if(strlen(col.name) == 0)
            throw runtime_error("Table: Column name cannot be empty");
        
        if(strlen(col.name) >= MAX_COL_NAME)
            throw runtime_error("Table: Column name is too long");

        for(int i = 0; i < MAX_COLS_COUNT; i++) {
            if(cols[i].isOccupied && strcmp(cols[i].name, col.name) == 0) {
                throw runtime_error("Table: Column with this name already exists");
            }
        }

        bool isSpace = false;
        for(int i = 0; i < MAX_COLS_COUNT; i++) {
            if(cols[i].isOccupied == 0) {
                cols[i] = col;
                cols[i].isOccupied = 1;
                header.colsCount++;
                isSpace = true;
                break;
            }
        }

        if(!isSpace)
            throw runtime_error("Table: No space for column " + string(col.name));
    }
}

int Table::searchColByName(string name) {
    for(int i = 0; i < MAX_COLS_COUNT; i++) {
        if(cols[i].isOccupied && strcmp(cols[i].name, name.c_str()) == 0) {
            return i;
        }
    }
    return -1;
}

dataVector Table::showCols() {
    dataVector selectedCols;
    for(int i = 0; i < MAX_COLS_COUNT; i++) {
        if(cols[i].isOccupied)
            selectedCols.push_back(string(cols[i].name));
    }
    return selectedCols;
}

void Table::readData() {
    ifstream in(table_file, ios::binary);

    if(!in)
        throw runtime_error("Table: Cannot open a file");

    in.seekg(0, ios::end);
    dataSize = static_cast<size_t>(in.tellg()) - STRUCTURE_SIZE;
    in.seekg(STRUCTURE_SIZE, ios::beg);

    free(data);
    data = (uint8_t*)malloc(dataSize);
    in.read(reinterpret_cast<char*>(data), dataSize);
    
    in.close();
}

void Table::writeData() {
    writeStructure();

    ofstream out(table_file, ios::binary | ios::in | ios::out);

    if(!out)
        throw runtime_error("Table: Cannot open a file");

    out.seekp(STRUCTURE_SIZE, ios::beg);
    out.write(reinterpret_cast<const char*>(data), dataSize);

    out.close();
}

uint32_t Table::insert(dataVector rowData) {
    if(rowData.size() != header.colsCount)
        throw runtime_error("Table: Row data does not match columns count");
    
    if(header.rowsCount >= MAX_ROWS_COUNT)
        throw runtime_error("Table: Maximum rows count reached");

    for(int i = 0; i < MAX_ROWS_COUNT; i++) {
        if(rows[i] == -1) {
            header.rowsCount++;
            header.currentRowID++;
            rows[i] = header.currentRowID;
            break;
        }
    }

    for(uint32_t i = 0; i < header.colsCount; i++) {
        TableCol* colPtr = &cols[i];
        TableDataHeader *dataHeaderPtr = nullptr;

        for(int i = 0; i < MAX_DATA_HEADERS_COUNT; i++) {
            if(dataHeaders[i].isOccupied == 0) {
                dataHeaderPtr = &dataHeaders[i];
                break;
            }
        }

        if(dataHeaderPtr == nullptr)
            throw runtime_error("Cannot find data header");
        
        header.dataHeadersCount++;
        dataHeaderPtr->isOccupied = 1;
        dataHeaderPtr->rowID = header.currentRowID;
        dataHeaderPtr->colID = i;
        dataHeaderPtr->start = dataSize;

        const auto& value = rowData[i];

        if(holds_alternative<datatype_int>(value)) {
            if(colPtr->datatype != DATA_TYPE_INT)
                throw runtime_error("Table: Data type mismatch for column " + string(colPtr->name));

            datatype_int val = get<datatype_int>(value);
            data = (uint8_t*)realloc(data, dataSize + sizeof(datatype_int));
            memcpy(data + dataSize, &val, sizeof(datatype_int));
            dataSize += sizeof(datatype_int);
            dataHeaderPtr->size = sizeof(datatype_int);
        } else if(holds_alternative<datatype_float>(value)) {
            if(colPtr->datatype != DATA_TYPE_FLOAT)
                throw runtime_error("Table: Data type mismatch for column " + string(colPtr->name));

            datatype_float val = get<datatype_float>(value);
            data = (uint8_t*)realloc(data, dataSize + sizeof(datatype_float));
            memcpy(data + dataSize, &val, sizeof(datatype_float));
            dataSize += sizeof(datatype_float);
            dataHeaderPtr->size = sizeof(datatype_float);
        } else if(holds_alternative<datatype_varchar>(value)) {
            if(colPtr->datatype != DATA_TYPE_VARCHAR)
                throw runtime_error("Table: Data type mismatch for column " + string(colPtr->name));

            datatype_varchar val = get<datatype_varchar>(value);
            size_t strSize = val.size() + 1;

            if(colPtr->size < strSize)
                throw runtime_error("Table: String size exceeds column size for column " + string(colPtr->name));

            data = (uint8_t*)realloc(data, dataSize + colPtr->size);
            memcpy(data + dataSize, val.c_str(), strSize);
            dataSize += colPtr->size;
            dataHeaderPtr->size = static_cast<uint32_t>(colPtr->size);
        } else if(holds_alternative<datatype_bool>(value)) {
            if(colPtr->datatype != DATA_TYPE_BOOL)
                throw runtime_error("Table: Data type mismatch for column " + string(colPtr->name));

            datatype_bool val = get<datatype_bool>(value);
            data = (uint8_t*)realloc(data, dataSize + sizeof(datatype_bool));
            memcpy(data + dataSize, &val, sizeof(datatype_bool));
            dataSize += sizeof(datatype_bool);
            dataHeaderPtr->size = sizeof(datatype_bool);
        } else {
            throw runtime_error("Table: Unsupported data type for column " + string(colPtr->name));
        }

        if(dataHeaderPtr == nullptr)
            throw runtime_error("Table: No space for new data header");
    }

    return header.currentRowID;
}

vector<uint32_t> Table::insertMultipleRows(dataMatrix rowsData) {
    vector<uint32_t> rowIDs;
    for(const auto& row: rowsData) {
        rowIDs.push_back(insert(row));
    }

    return rowIDs;
}

dataVector Table::select(vector<uint32_t> colIDs, uint32_t rowID) {
    dataVector selectedData;

    if(colIDs.size() > header.colsCount)
        throw runtime_error("Table: inserted column count is bigger than column count in database");

    for(const uint32_t &colID: colIDs) {
        if(colID >= header.colsCount)
            throw runtime_error("Table: Inwalid column ID");
        

        if(cols[colID].isOccupied != 1)
            throw runtime_error("Cannot find a column of ID " + rowID);
        
        TableDataHeader* dataHeaderPtr = nullptr;
        for(int i = 0; i < MAX_DATA_HEADERS_COUNT; i++) {
            if(dataHeaders[i].isOccupied == 1 && dataHeaders[i].colID == colID && dataHeaders[i].rowID == rowID) {
                dataHeaderPtr = &dataHeaders[i];
                break;
            }
        }

        if(dataHeaderPtr == nullptr)
            throw runtime_error("Table: Cannot find selected data");

        
        switch(cols[colID].datatype) {
            case DATA_TYPE_INT:
                datatype_int intSelectedDataPart;
                memcpy(&intSelectedDataPart, data + dataHeaderPtr->start, sizeof(datatype_int));
                selectedData.push_back(intSelectedDataPart);
                break;

            case DATA_TYPE_FLOAT:
                datatype_float floatSelectedDataPart;
                memcpy(&floatSelectedDataPart, data + dataHeaderPtr->start, sizeof(datatype_float));
                selectedData.push_back(floatSelectedDataPart);
                break; 
                
            case DATA_TYPE_VARCHAR:
                char* charSelectedDataPart;
                charSelectedDataPart = (char*)malloc(dataHeaderPtr->size);
                memcpy(charSelectedDataPart, data + dataHeaderPtr->start, dataHeaderPtr->size);
                selectedData.push_back(string(charSelectedDataPart));
                free(charSelectedDataPart);
                break;

            case DATA_TYPE_BOOL:
                datatype_bool boolSelectedDataPart;
                memcpy(&boolSelectedDataPart, data + dataHeaderPtr->start, sizeof(datatype_bool));
                selectedData.push_back(boolSelectedDataPart);
                break; 
        }
    }

    return selectedData;
}

dataMatrix Table::selectMultipleRows(vector<uint32_t> colIDs, vector<uint32_t> rowIDs) {
    dataMatrix selectedData;
    for(const uint32_t &rowID : rowIDs) {
        selectedData.push_back(select(colIDs, rowID));
    }

    return selectedData;
}

dataMatrix Table::selectAll() {
    vector<uint32_t> colIDs;
    for(uint32_t i = 0; i < MAX_COLS_COUNT; i++) {
        if(cols[i].isOccupied == 1) {
            colIDs.push_back(i);
        }
    }

    vector<uint32_t> rowIDs;
    for(int i = 0; i < MAX_ROWS_COUNT; i++) {
        if(rows[i] != -1) {
            rowIDs.push_back(rows[i]);
        }
    }

    return selectMultipleRows(colIDs, rowIDs);
}

void Table::cleanData() {
    vector<TableDataHeader> dataHeadersVector;
    for(int i = 0; i < MAX_DATA_HEADERS_COUNT; i++) {
        if(dataHeaders[i].isOccupied)
            dataHeadersVector.push_back(dataHeaders[i]);
    }

    if(dataHeadersVector.size() > 1) {
        sort(dataHeadersVector.begin(), dataHeadersVector.end(),
            [](const TableDataHeader &a, const TableDataHeader &b) {
                return a.start < b.start;
            });
    }
   
    
    for(size_t i; i + 1 < dataHeadersVector.size(); i++) {
        TableDataHeader cur = dataHeadersVector[i];
        TableDataHeader next = dataHeadersVector[i + 1];

        uint64_t gapStart = cur.start + cur.size;                     
        if (gapStart < next.start) {                                       
            uint64_t gap  = next.start - gapStart;           
            uint64_t tailSrc = gapStart + gap;                        
            uint64_t tailLen = dataSize - tailSrc;                     

            memmove(data + gapStart, data + tailSrc, tailLen);   
            dataSize -= gap;                                                 

            for(size_t j = i + 1; j < dataHeadersVector.size(); j++)
                dataHeadersVector[j].start -= gap;

            data = (uint8_t*)realloc(data, dataSize);
        }
    }

    for(int i = 0; i < MAX_DATA_HEADERS_COUNT; i++)
        dataHeaders[i] = {};

    copy(dataHeadersVector.begin(), dataHeadersVector.end(), dataHeaders);
}

void Table::deleteRow(uint32_t rowID) {
    bool rowsFound = false;
    for(int i = 0; i < MAX_ROWS_COUNT; i++) {
        if(rows[i] == (int)rowID) {
            rows[i] = -1;
            rowsFound = true;
        }
    }

    if(!rowsFound)
        throw runtime_error("Cannot find row to delete");

    header.rowsCount--;

    for(int i = 0; i < MAX_DATA_HEADERS_COUNT; i++) {
        if(dataHeaders[i].isOccupied && dataHeaders[i].rowID == rowID) {
            dataHeaders[i].isOccupied = 0;
            header.dataHeadersCount--;
        }
    }
    cleanData();
}

void Table::deleteMultipleRows(vector<uint32_t> rowIDs) {
    for(const uint32_t &row : rowIDs) {
        deleteRow(row);
    }
}

void Table::truncate() {
    vector<uint32_t> rowsIDs;
    for(int i = 0; i < MAX_ROWS_COUNT; i++) {
        if(rows[i] != -1)
            rowsIDs.push_back(rows[i]);
    }

    deleteMultipleRows(rowsIDs);
}

void Table::update(uint32_t colID, uint32_t rowID, dataVariable dataToUpdate) {
    TableDataHeader *dataHeaderPtr = nullptr;
    for(int i = 0; i < MAX_DATA_HEADERS_COUNT; i++) {
        if(dataHeaders[i].isOccupied && dataHeaders[i].colID == colID && dataHeaders[i].rowID == rowID) {
            dataHeaderPtr = &dataHeaders[i];
            break;
        }
    }

    if(dataHeaderPtr == nullptr)
        throw runtime_error("Invalid column or row");

    if(holds_alternative<datatype_int>(dataToUpdate)) {
        if(cols[colID].datatype != DATA_TYPE_INT)
            throw runtime_error("Invalid data type");

        datatype_int val = get<datatype_int>(dataToUpdate);
        memcpy(data + dataHeaderPtr->start, &val, sizeof(datatype_int));
    } else if(holds_alternative<datatype_float>(dataToUpdate)) {
         if(cols[colID].datatype != DATA_TYPE_FLOAT)
            throw runtime_error("Invalid data type");

        datatype_float val = get<datatype_float>(dataToUpdate);
        memcpy(data + dataHeaderPtr->start, &val, sizeof(datatype_float));
    } else if(holds_alternative<datatype_varchar>(dataToUpdate)) {
         if(cols[colID].datatype != DATA_TYPE_VARCHAR)
            throw runtime_error("Invalid data type");
        
        datatype_varchar val = get<datatype_varchar>(dataToUpdate);
        size_t strSize = val.size() + 1;

        if(dataHeaderPtr->size < strSize)
            throw runtime_error("String is too big");

        
        memcpy(data + dataHeaderPtr->start, val.c_str(), strSize);
    } else if(holds_alternative<datatype_bool>(dataToUpdate)) {
         if(cols[colID].datatype != DATA_TYPE_FLOAT)
            throw runtime_error("Invalid data type");

        datatype_bool val = get<datatype_bool>(dataToUpdate);
        memcpy(data + dataHeaderPtr->start, &val, sizeof(datatype_bool));
    } else
        throw runtime_error("Invalid data type");
}
