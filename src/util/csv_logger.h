#ifndef TAI_ASSIGNMENT3_GROUP8_CSV_LOGGER_H
#define TAI_ASSIGNMENT3_GROUP8_CSV_LOGGER_H

#include <string>
#include <vector>
#include <fstream>

using namespace std;

class CSVLogger {
public:
    CSVLogger(string filename);
    CSVLogger();
    void append(vector<string> values);
    void close();

private:
    string filePath;
    vector<string> columns;
    ofstream file;
};

#endif //TAI_ASSIGNMENT3_GROUP8_CSV_LOGGER_H