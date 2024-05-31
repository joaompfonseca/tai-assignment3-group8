#include <filesystem>
#include <iostream>
#include "engine/compressor.h"
#include "engine/database.h"
#include "util/file_reader.h"
#include "util/program_arguments.h"

using namespace std;

int main(int argc, char *argv[]) {
    // parse program arguments
    ProgramArguments args = getProgramArguments(argc, argv);

    // print program configuration
    cout << "Program configuration:" << endl
         << args << endl;

    // instantiate compressor
    Compressor compressor = Compressor(args.compressionMethod);

    // instantiate database
    Database database = Database(compressor);

    // add entries to database
    for (const auto &entry: std::filesystem::directory_iterator(args.databaseFolder)) {
        string filePath = entry.path().string();
        if (filePath.substr(filePath.find_last_of('.') + 1) != "freqs") {
            continue;
        }
        // read signature from file
        FileReader fileReader = FileReader(filePath);
        fileReader.read();
        string signature = fileReader.getContent();
        // add entry to database
        database.add(filePath, signature);
    }

    // query database
    for (const auto &entry: std::filesystem::directory_iterator(args.queriesFolder)) {
        string filePath = entry.path().string();
        if (filePath.substr(filePath.find_last_of('.') + 1) != "freqs") {
            continue;
        }
        // read signature from file
        FileReader fileReader = FileReader(filePath);
        fileReader.read();
        string signature = fileReader.getContent();
        // query database
        cout << "Querying " << filePath << "..." << endl;
        vector<tuple<basic_string<char>, double>> result = database.query(signature);
        // print results
        cout << "Results" << endl;
        for (const auto &entry: result) {
            cout << "   " << get<0>(entry) << " (" << get<1>(entry) << ")" << endl;
        }
    }

    return EXIT_SUCCESS;
}
