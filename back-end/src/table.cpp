#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <filesystem>

#include "../headers/table.hpp"

TableHeader::TableHeader() {
    signature[0] = 'S';
    signature[1] = 'T';
    signature[2] = 'T';
    signature[3] = 'B';
    colsCount = 0;
}

Table::Table(string table_file) {
    this->table_file = table_file;
    header = TableHeader();

    dataHeaders = new TableDataHeader[MAX_DATA_HEADERS_COUNT];

    for(int i = 0; i < MAX_COLS_COUNT; i++)
        cols[i] = {};

    for(int i = 0; i < MAX_DATA_HEADERS_COUNT; i++)
        dataHeaders[i] = {};
}

Table::~Table() {
    delete[] dataHeaders;
}

void Table::writeStructure() { 
    ofstream out(table_file, ios::binary);

    if(!out)
        throw runtime_error("Table: Cannot open/create a file");

    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    out.write(reinterpret_cast<const char*>(cols), sizeof(cols));
    out.write(reinterpret_cast<const char*>(dataHeaders), sizeof(TableDataHeader) * MAX_DATA_HEADERS_COUNT);

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
    in.read(reinterpret_cast<char*>(dataHeaders), sizeof(dataHeaders));

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

int Table::createCol(TableCol col) {
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

    for(int i = 0; i < MAX_COLS_COUNT; i++) {
        if(cols[i].isOccupied == 0) {
            cols[i] = col;
            cols[i].isOccupied = 1;
            header.colsCount++;
            return i;
        }
    }
    throw runtime_error("Table: No space for new column");
}

void Table::deleteCol(int colID) {
    if(colID < 0 || colID >= MAX_COLS_COUNT)
        throw runtime_error("Table: Invalid column ID");

    if(cols[colID].isOccupied == 0)
        throw runtime_error("Table: Column does not exist");

    cols[colID] = {};
    header.colsCount--;
}

int Table::searchColByName(string name) {
    for(int i = 0; i < MAX_COLS_COUNT; i++) {
        if(cols[i].isOccupied && strcmp(cols[i].name, name.c_str()) == 0) {
            return i;
        }
    }
    return -1;
}