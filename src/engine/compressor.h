#ifndef TAI_ASSIGNMENT3_GROUP8_COMPRESSOR_H
#define TAI_ASSIGNMENT3_GROUP8_COMPRESSOR_H

#include <string>
#include <unordered_map>

using namespace std;

class Compressor {
public:
    explicit Compressor(string name, string tempFolder);

    string getName();

    unsigned int getBits(string content);

private:
    string name;
    string tempFolder;
};

#endif //TAI_ASSIGNMENT3_GROUP8_COMPRESSOR_H