#include "database.h"
#include "../util/file_reader.h"
#include <fstream>
#include <tuple>
#include <algorithm>
#include <iostream>
#include <filesystem>

using namespace std;

Database::Database(string databaseFolder, Compressor &compressor) : compressor(compressor) {
    this->databaseFolder = databaseFolder;
    this->storage = unordered_map<string, tuple<string, unsigned int>>();
    this->cacheBits = unordered_map<string, unsigned int>();
}

void Database::add(string label, string signature) {
    unsigned int bits;
    if (cacheBits.find(label) != cacheBits.end()) {
        bits = cacheBits[label];
    } else {
        bits = compressor.getBits(signature);
        cacheBits[label] = bits;
    }
    storage[label] = make_tuple(signature, bits);
}

void Database::loadCacheBits() {
    string cacheFilePath = databaseFolder + "/" + compressor.getName() + ".cache";
    ifstream file(cacheFilePath);
    string line;
    while (getline(file, line)) {
        size_t delimiter = line.find(';');
        string label = line.substr(0, delimiter);
        unsigned int bits = stoi(line.substr(delimiter + 1));
        cacheBits[label] = bits;
    }
}

void Database::saveCacheBits() {
    string cacheFilePath = databaseFolder + "/" + compressor.getName() + ".cache";
    ofstream file(cacheFilePath);
    for (auto &[label, bits]: cacheBits) {
        file << label << ";" << bits << endl;
    }
}

void Database::load() {
    // load bits from cache
    loadCacheBits();

    for (const auto &entry: filesystem::directory_iterator(databaseFolder)) {
        string filePath = entry.path().string();
        if (filePath.substr(filePath.find_last_of('.') + 1) != "freqs") {
            continue;
        }
        // read signature from file
        FileReader fileReader = FileReader(filePath);
        fileReader.read();
        string signature = fileReader.getContent();
        // add entry to database
        add(filePath, signature);
    }

    // save bits to cache
    saveCacheBits();
}

vector<tuple<string, double>> Database::query(string qSignature) {
    vector<tuple<string, double>> result;

    // Calculate bits of the query signature
    unsigned int qBits = compressor.getBits(qSignature);

    for (auto &[eLabel, eContent]: storage) {
        string eSignature = get<0>(eContent);
        unsigned int eBits = get<1>(eContent);

        // Concatenate query and entry signatures, calculate bits
        string qeSignature = qSignature + eSignature;
        unsigned int qeBits = compressor.getBits(qeSignature);

        // Calculate the Normalized Compression Distance
        double ncd = ((double) (qeBits - min(qBits, eBits))) / ((double) max(qBits, eBits));
        result.emplace_back(eLabel, ncd);
    }

    // Sort the results by NCD
    sort(result.begin(), result.end(), [](const tuple<string, double> &a, const tuple<string, double> &b) {
        return get<1>(a) < get<1>(b);
    });

    return result;
}
