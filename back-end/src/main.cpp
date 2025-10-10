#include <iostream>
#include "../headers/db.h"

using namespace std;

int main() {
    DB db = DB();
    db.header.totalDataHeadersCount = 2137;
    db.writeStructure();
}