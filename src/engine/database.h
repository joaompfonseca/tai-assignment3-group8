#ifndef TAI_ASSIGNMENT3_GROUP8_DATABASE_H
#define TAI_ASSIGNMENT3_GROUP8_DATABASE_H

#include <string>
#include <vector>
#include "compressor.h"

using namespace std;

class Database {
public:
    explicit Database(Compressor &compressor);

    void add(string label, string signature);

    vector<tuple<string, double>> query(string signature);

private:
    Compressor compressor;
    vector<tuple<string, string, unsigned int>> storage; // <label, signature, bits>
};

#endif //TAI_ASSIGNMENT3_GROUP8_DATABASE_H