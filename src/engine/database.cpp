#include "database.h"
#include <fstream>
#include <tuple>
#include <algorithm>
#include <iostream>

using namespace std;

Database::Database(Compressor &compressor) : compressor(compressor) {
    storage = vector<tuple<string, string, unsigned int>>(); // <label, signature, bits>
}

void Database::add(string label, string signature) {
    unsigned int bits = compressor.getBits(signature);
    storage.emplace_back(label, signature, bits);
}

vector<tuple<string, double>> Database::query(string qSignature) {
    vector<tuple<string, double>> result;

    // Calculate bits of the query signature
    unsigned int qBits = compressor.getBits(qSignature);

    for (auto &entry: storage) {
        string eLabel = get<0>(entry);
        string eSignature = get<1>(entry);
        unsigned int eBits = get<2>(entry);

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
