#ifndef TAI_ASSIGNMENT3_GROUP8_COMPRESSOR_H
#define TAI_ASSIGNMENT3_GROUP8_COMPRESSOR_H

#include <string>
#include <unordered_map>

using namespace std;

class Compressor {
public:
    explicit Compressor(string name);

    string getName();

    unsigned int getBits(string content, string tempFolder);

private:
    string name;
};

#endif //TAI_ASSIGNMENT3_GROUP8_COMPRESSOR_H