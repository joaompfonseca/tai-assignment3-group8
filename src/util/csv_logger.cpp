#include <iostream>
#include "csv_logger.h"

CSVLogger::CSVLogger(string filename) {
    this->filePath = filename;
    file.open(filePath, ios::out | ios::app); // open file in append mode
}

CSVLogger::CSVLogger() {
    this->filePath = "";
}

void CSVLogger::append(vector<string> values) {
    for (int i = 0; i < (int) values.size(); i++) {
        file << values[i];
        if (i < (int) values.size() - 1) {
            file << ",";
        }
    }
    file << endl;
}

void CSVLogger::close() {
    file.close();
}