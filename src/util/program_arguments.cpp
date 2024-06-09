#include "program_arguments.h"

#include <unordered_set>
#include <getopt.h>

using namespace std;

ProgramArguments getProgramArguments(int argc, char *argv[]) {
    ProgramArguments args;
    unordered_set<char> requiredArgs;
    int opt;
    while ((opt = getopt(argc, argv, "d:q:c:k:l:t:h")) != -1) {
        switch (opt) {
            case 'd':
                args.databaseFolder = optarg;
                requiredArgs.insert('d');
                break;
            case 'q':
                args.queriesFolder = optarg;
                requiredArgs.insert('q');
                break;
            case 'c':
                args.compressionMethod = optarg;
                requiredArgs.insert('c');
                break;
            case 'k':
                try {
                    int arg = stoi(optarg);
                    if (arg <= 0) {
                        throw invalid_argument(nullptr);
                    }
                    args.topK = arg;
                } catch (const invalid_argument &e) {
                    cerr << "Error: Top-k results (-k) must be an integer greater than 0." << endl;
                    exit(EXIT_FAILURE);
                }
                requiredArgs.insert('k');
                break;
            case 'l':
                args.logFilePath = optarg;
                break;
            case 't':
                args.tempFolder = optarg;
                break;
            case 'h':
                cout << "Usage: ./what_the_music REQUIRED OPTIONAL" << endl
                     << "Required arguments:" << endl
                     << " -d database_folder    : folder containing the music signatures to be inserted in the database (string)" << endl
                     << " -q queries_folder     : folder containing the music signatures to be queried on the database (string)" << endl
                     << " -c compression_method : compression method of the music signatures (string)" << endl
                     << "                       : --- valid args: gzip, bzip2, lzma, zstd" << endl
                     << " -k top_k_results      : number of top-k results to be returned (int)" << endl
                     << "Optional arguments:" << endl
                     << " -h                    : shows how to use the program" << endl
                     << " -l log_file_path      : path to the file where the log will be written (string, default is empty)" << endl
                     << " -t temp_folder        : folder to store temporary files (string, default is queries_folder)" << endl;
                exit(EXIT_SUCCESS);
            case '?':
                if (optopt == 'd' || optopt == 'q' || optopt == 'c' || optopt == 'k' || optopt == 'l' || optopt == 't') {
                    cerr << "Option -" << static_cast<char>(optopt) << " requires an argument." << endl;
                } else {
                    cerr << "Unknown option -" << static_cast<char>(optopt) << endl;
                }
                exit(EXIT_FAILURE);
            default:
                exit(EXIT_FAILURE);
        }
    }
    // Ensure that required options are provided
    for (char requiredArg: {'d', 'q', 'c', 'k'}) {
        if (requiredArgs.find(requiredArg) == requiredArgs.end()) {
            cerr << "Error: Option -" << requiredArg << " is required." << endl;
            exit(EXIT_FAILURE);
        }
    }
    // Ensure that optional arguments have default values if not provided
    if (args.tempFolder.empty()) {
        args.tempFolder = args.queriesFolder;
    }
    return args;
}