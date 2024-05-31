#include "file_reader.h"
#include <fstream>
#include <iostream>

using namespace std;

FileReader::FileReader(string filePath) {
    this->filePath = filePath;
    this->content = "";
}

string FileReader::getContent() {
    return content;
}

void FileReader::read() {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Unable to open file: " << filePath << endl;
        exit(EXIT_FAILURE);
    }
    content = string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}
