#ifndef TAI_ASSIGNMENT3_GROUP8_DATABASE_H
#define TAI_ASSIGNMENT3_GROUP8_DATABASE_H

#include <string>
#include <vector>
#include <unordered_map>
#include "compressor.h"

using namespace std;

class Database {
public:
    explicit Database(string databaseFolder, Compressor &compressor);

    void load();

    vector<tuple<string, double>> query(string signature);

private:
    string databaseFolder;
    Compressor compressor;
    unordered_map<string, unsigned int> cacheBits; // label: bits
    unordered_map<string, tuple<string, unsigned int>> storage; // label: <signature, bits>

    void loadCacheBits();

    void saveCacheBits();

    void add(string label, string signature);
};

#endif //TAI_ASSIGNMENT3_GROUP8_DATABASE_H