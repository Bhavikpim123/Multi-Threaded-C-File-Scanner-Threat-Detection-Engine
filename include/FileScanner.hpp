#ifndef FILE_SCANNER_HPP
#define FILE_SCANNER_HPP

#include <string>
#include <vector>

#include "Task.hpp"

class FileScanner {
public:
    // Recursively traverse a directory and return a list of discovered tasks
    std::vector<ScanTask> discoverFiles(const std::string& directoryPath);
};

#endif // FILE_SCANNER_HPP
