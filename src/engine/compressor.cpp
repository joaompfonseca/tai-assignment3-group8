#include "compressor.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <sstream>


using namespace std;

Compressor::Compressor(string name, string tempFolder) {
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
    this->tempFolder = tempFolder;
}

string Compressor::getName() {
    return name;
}

unsigned int Compressor::getBits(string content) {
    unsigned int bytes = 0;
    ostringstream command;
    int sysResult;

    string tempContent = tempFolder + "/content.temp";
    string tempBytes = tempFolder + "/bytes.temp";

    // Create a temporary file with the content to compress
    ofstream contentFile(tempContent);
    contentFile << content;
    contentFile.close();

    // compress the file and save number of bytes
    if (name == "gzip") {
        command << "gzip -fk " << tempContent << " > /dev/null 2>&1"
                << " && wc -c " << tempContent << ".gz > " << tempBytes;
    } else if (name == "bzip2") {
        command << "bzip2 -fk " << tempContent << " > /dev/null 2>&1"
                << " && wc -c " << tempContent << ".bz2 > " << tempBytes;
    } else if (name == "lzma") {
        command << "lzma -fk " << tempContent << " > /dev/null 2>&1"
                << " && wc -c " << tempContent << ".lzma > " << tempBytes;
    } else if (name == "zstd") {
        command << "zstd -fk " << tempContent << " > /dev/null 2>&1"
                << " && wc -c " << tempContent << ".zst > " << tempBytes;
    } else {
        cerr << "Unsupported compressor: " << name << endl;
        exit(EXIT_FAILURE);
    }

    sysResult = system(command.str().c_str());
    if (sysResult != 0) {
        cerr << "Error compressing content" << endl;
        exit(EXIT_FAILURE);
    }
    command.str("");

    // read the number of bytes from the file
    ifstream bytesFile(tempBytes);
    bytesFile >> bytes;
    bytesFile.close();

    // remove temporary files
    command << "rm " << tempFolder << "/*.temp*";
    sysResult = system(command.str().c_str());
    if (sysResult != 0) {
        // ignore the error, it's just temporary files
    }

    return bytes * 8;
}
