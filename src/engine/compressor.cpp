#include "compressor.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>


using namespace std;

Compressor::Compressor(string name) {
    int sysResult;
    if (name == "gzip") {
        sysResult = system("gzip --version > /dev/null 2>&1");
    } else if (name == "bzip2") {
        sysResult = system("bzip2 --version > /dev/null 2>&1");
    } else if (name == "lzma") {
        sysResult = system("xz --version > /dev/null 2>&1");
    } else if (name == "zstd") {
        sysResult = system("zstd --version > /dev/null 2>&1");
    } else {
        cerr << "Unsupported compressor: " << name << endl;
        exit(EXIT_FAILURE);
    }
    if (sysResult != 0) {
        cerr << "Compressor not found: " << name << endl;
        exit(EXIT_FAILURE);
    }
    this->name = name;
}

unsigned int Compressor::getBits(string content) {
    unsigned int bytes = 0;
    int sysResult;

    // Create a temporary file with the content to compress
    ofstream contentFile("content.temp");
    contentFile << content;
    contentFile.close();

    // compress the file and save number of bytes
    if (name == "gzip") {
        sysResult = system("gzip -fk content.temp > /dev/null 2>&1") + system("wc -c content.temp.gz > bytes.temp");
    } else if (name == "bzip2") {
        sysResult = system("bzip2 -fk content.temp > /dev/null 2>&1") + system("wc -c content.temp.bz2 > bytes.temp");
    } else if (name == "lzma") {
        sysResult = system("lzma -fk content.temp > /dev/null 2>&1") + system("wc -c content.temp.lzma > bytes.temp");
    } else if (name == "zstd") {
        sysResult = system("zstd -fk content.temp > /dev/null 2>&1") + system("wc -c content.temp.zst > bytes.temp");
    } else {
        cerr << "Unsupported compressor: " << name << endl;
        exit(EXIT_FAILURE);
    }
    if (sysResult != 0) {
        cerr << "Error compressing content" << endl;
        exit(EXIT_FAILURE);
    }

    // read the number of bytes from the file
    ifstream bytesFile("bytes.temp");
    bytesFile >> bytes;
    bytesFile.close();

    // remove temporary files
    sysResult = system("rm *.temp*");
    if (sysResult != 0) {
        // ignore the error, it's just temporary files
    }

    return bytes * 8;
}
