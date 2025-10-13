#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>

#include "../headers/db.h"

using namespace std;

DBheader::DBheader() {
    signature[0] = 'S';
    signature[1] = 'T';
    signature[2] = 'D';
    signature[3] = 'B';
}

void DBStructure::writeStructure() { 
    ofstream out(DB_FILE, ios::binary);

    if(!out)
        throw runtime_error("DB Structure: Cannot open/create a file");

    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    out.write(reinterpret_cast<const char*>(tables), sizeof(tables));
    out.write(reinterpret_cast<const char*>(cols), sizeof(cols));
    out.write(reinterpret_cast<const char*>(dataHeaders), sizeof(dataHeaders));

    out.close();

}

void DBStructure::readStructure() {
    ifstream in(DB_FILE, ios::binary);

    if(!in)
        throw runtime_error("DB Structure: Cannot open a file");

    
    in.read(reinterpret_cast<char*>(&header), sizeof(header));
    if(header.signature[0] != 'S' || header.signature[1] != 'T' || header.signature[2] != 'D' || header.signature[3] != 'B')
        throw runtime_error("DB Structure: Invalid file format");

    in.read(reinterpret_cast<char*>(tables), sizeof(tables));
    in.read(reinterpret_cast<char*>(cols), sizeof(cols));
    in.read(reinterpret_cast<char*>(dataHeaders), sizeof(dataHeaders));

    in.close();
}

DB::DB() {
    ifstream in(DB_FILE);
    bool fileExists = in.good();
    in.close();

    if(fileExists) {
        readStructure();
        return;
    }
    
    header = DBheader();
    header.tablesCount = 0;
    header.totalRowsCount = 0;
    header.totalColsCount = 0;
    header.totalDataHeadersCount = 0;

    for(int i = 0; i < MAX_TABLES_COUNT; i++)
        tables[i] = DBtable();

    for(int i = 0; i < MAX_COLS_COUNT; i++)
        cols[i] = DBcol();

    for(int i = 0; i < MAX_DATA_HEADERS_COUNT; i++)
        dataHeaders[i] = DBdataHeader();
    

    writeStructure();
}