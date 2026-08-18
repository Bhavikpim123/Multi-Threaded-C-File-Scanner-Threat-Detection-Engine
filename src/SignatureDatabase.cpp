#include "SignatureDatabase.hpp"

#include <fstream>

SignatureDatabase::SignatureDatabase(
    const std::string& databasePath) {

    std::ifstream file(databasePath);

    if (!file) {
        return;
    }

    std::string hash;

    while (std::getline(file, hash)) {
        if (!hash.empty()) {
            signatures_.insert(hash);
        }
    }
}

bool SignatureDatabase::contains(
    const std::string& hash) const {

    return signatures_.find(hash) != signatures_.end();
}
