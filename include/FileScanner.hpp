#ifndef FILE_SCANNER_HPP
#define FILE_SCANNER_HPP

#include <string>
#include <vector>

class FileScanner {
public:
    // Recursively traverse a directory and return a list of discovered files
    std::vector<std::string> discoverFiles(const std::string& directoryPath);
};

#endif // FILE_SCANNER_HPP
