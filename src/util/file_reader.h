#ifndef TAI_ASSIGNMENT3_GROUP8_FILE_READER_H
#define TAI_ASSIGNMENT3_GROUP8_FILE_READER_H

#include <string>

using namespace std;

class FileReader {
public:
    explicit FileReader(string filePath);

    string getContent();

    void read();

private:
    string filePath;
    string content;
};

#endif //TAI_ASSIGNMENT3_GROUP8_FILE_READER_H