#include "program_arguments.h"

#include <unordered_set>
#include <getopt.h>

using namespace std;

ProgramArguments getProgramArguments(int argc, char *argv[]) {
    ProgramArguments args;
    unordered_set<char> requiredArgs;
    int opt;
    while ((opt = getopt(argc, argv, "d:q:c:l:h")) != -1) {
        switch (opt) {
            case 'd':
                args.databaseFolder = optarg;
                requiredArgs.insert('d');
                break;
            case 'q':
                args.queriesFolder = optarg;
                requiredArgs.insert('q');
            case 'c':
                args.compressionMethod = optarg;
                requiredArgs.insert('c');
            case 'l':
                args.logFilePath = optarg;
                break;
            case 'h':
                cout << "Usage: ./what_the_music REQUIRED OPTIONAL" << endl
                     << "Required arguments:" << endl
                     << " -d database_folder    : folder containing the music files to be inserted in the database (string)" << endl
                     << " -q queries_folder     : folder containing the music files to be queried on the database (string)" << endl
                     << " -c compression_method : compression method of the music files (string)" << endl
                     << "                       : --- valid args: maxfreqs, gzip, bzip2, lzma, zstd" << endl
                     << "Optional arguments:" << endl
                     << " -h                    : shows how to use the program" << endl
                     << " -l log_file_path      : path to the file where the log will be written (string, default is empty)" << endl;
                exit(EXIT_SUCCESS);
            case '?':
                if (optopt == 'd' || optopt == 'q' || optopt == 'c' || optopt == 'l') {
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
    for (char requiredArg: {'d', 'q', 'c'}) {
        if (requiredArgs.find(requiredArg) == requiredArgs.end()) {
            cerr << "Error: Option -" << requiredArg << " is required." << endl;
            exit(EXIT_FAILURE);
        }
    }
    return args;
}