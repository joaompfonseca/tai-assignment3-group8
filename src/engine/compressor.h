#ifndef TAI_ASSIGNMENT3_GROUP8_COMPRESSOR_H
#define TAI_ASSIGNMENT3_GROUP8_COMPRESSOR_H

#include <string>

using namespace std;

class Compressor {
public:
    explicit Compressor(string name);

    unsigned int getBits(string content);

private:
    string name;
};

#endif //TAI_ASSIGNMENT3_GROUP8_COMPRESSOR_H