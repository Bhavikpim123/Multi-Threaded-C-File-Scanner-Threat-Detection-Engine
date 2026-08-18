#pragma once

#include <string>
#include <unordered_set>

class SignatureDatabase {
public:
    explicit SignatureDatabase(const std::string& databasePath);

    bool contains(const std::string& hash) const;

private:
    std::unordered_set<std::string> signatures_;
};
