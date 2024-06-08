#include <filesystem>
#include <iostream>
#include "engine/compressor.h"
#include "engine/database.h"
#include "util/file_reader.h"
#include "util/program_arguments.h"
#include "util/csv_logger.h"

using namespace std;
using namespace std::filesystem;

int main(int argc, char *argv[]) {
    // parse program arguments
    ProgramArguments args = getProgramArguments(argc, argv);

    // print program configuration
    cout << "Program configuration:" << endl
         << args << endl;

    // instantiate compressor
    Compressor compressor = Compressor(args.compressionMethod);

    // instantiate database
    Database database = Database(args.databaseFolder, compressor);
    database.load();

    // instantiate logger
    CSVLogger logger;
    if (!args.logFilePath.empty()) {
        logger = CSVLogger(args.logFilePath);
    }

    // query database
    for (const auto &entry: directory_iterator(args.queriesFolder)) {
        // get path not the string
        path filePath = entry.path();
        if (filePath.has_extension() && filePath.extension().string() != ".freqs") {
            continue;
        }

        // read signature from file
        FileReader fileReader = FileReader(filePath.string());
        fileReader.read();
        string signature = fileReader.getContent();

        // query database
        cout << "Querying " << filePath.stem().string() << "..." << endl;
        vector<tuple<basic_string<char>, double>> result = database.query(signature, args.topK);

        // print or log results
        if (args.logFilePath.empty()) {
            cout << "Results" << endl;
            for (const auto &entry: result) {
                cout << "   " << get<0>(entry) << " (" << get<1>(entry) << ")" << endl;
            }
        } else {
            vector<string> output = {filePath.stem().string()};
            for (const auto &entry: result) {
                output.push_back(get<0>(entry));
            }
            logger.append(output);
        }
    }

    return EXIT_SUCCESS;
}
