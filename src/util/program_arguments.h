#ifndef TAI_ASSIGNMENT3_GROUP8_PROGRAM_ARGUMENTS_H
#define TAI_ASSIGNMENT3_GROUP8_PROGRAM_ARGUMENTS_H

#include <iostream>

using namespace std;

struct ProgramArguments {
    string databaseFolder;
    string queriesFolder;
    string compressionMethod;
    string logFilePath;

    // overload the << operator to print the program arguments
    friend ostream &operator<<(ostream &os, const ProgramArguments &args) {
        os << "(-d) Source of music files in database: " << args.databaseFolder << endl;
        os << "(-q) Source of music files to query: " << args.queriesFolder << endl;
        os << "(-c) Compression method being used: " << args.compressionMethod << endl;
        os << "(-l) Log file path: " << args.logFilePath;
        return os;
    }
};

ProgramArguments getProgramArguments(int argc, char *argv[]);

#endif //TAI_ASSIGNMENT3_GROUP8_PROGRAM_ARGUMENTS_H
