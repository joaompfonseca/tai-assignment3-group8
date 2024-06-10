#include "database.h"
#include "../util/file_reader.h"
#include <fstream>
#include <tuple>
#include <algorithm>
#include <iostream>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

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

bool Database::loadCacheBits() {
    string cacheFilePath = databaseFolder + "/" + compressor.getName() + ".cache";
    ifstream file(cacheFilePath);
    if (!file.is_open()) {
        return false;
    }
    string line;
    while (getline(file, line)) {
        size_t delimiter = line.find(';');
        string label = line.substr(0, delimiter);
        unsigned int bits = stoi(line.substr(delimiter + 1));
        cacheBits[label] = bits;
    }
    return true;
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
    bool cacheExists = loadCacheBits();
    cout << cacheExists << endl;

    for (const auto &entry: directory_iterator(databaseFolder)) {
        path filePath = entry.path().string();
        if (filePath.has_extension() && filePath.extension().string() != ".freqs") {
            continue;
        }
        // read signature from file
        FileReader fileReader = FileReader(filePath.string());
        fileReader.read();
        string signature = fileReader.getContent();
        // add entry to database
        add(filePath.stem().string(), signature);
    }

    // save bits to cache
    if (!cacheExists) {
        saveCacheBits();
    }
}

vector<tuple<string, double>> Database::query(string qSignature, unsigned int topK) {
    vector<tuple<string, double>> result;

    // calculate bits of the query signature
    unsigned int qBits = compressor.getBits(qSignature);

    for (auto &[eLabel, eContent]: storage) {
        string eSignature = get<0>(eContent);
        unsigned int eBits = get<1>(eContent);

        // concatenate query and entry signatures, calculate bits
        string qeSignature = qSignature + eSignature;
        unsigned int qeBits = compressor.getBits(qeSignature);

        // calculate the Normalized Compression Distance
        double ncd = ((double) (qeBits - min(qBits, eBits))) / ((double) max(qBits, eBits));
        result.emplace_back(eLabel, ncd);
    }

    // sort the results by NCD
    sort(result.begin(), result.end(), [](const tuple<string, double> &a, const tuple<string, double> &b) {
        return get<1>(a) < get<1>(b);
    });

    // return only the top K results
    if (result.size() > topK) {
        result.resize(topK);
    }

    return result;
}
