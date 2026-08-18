#include "FileScanner.hpp"
#include <filesystem>

namespace fs = std::filesystem;

std::vector<std::string> FileScanner::discoverFiles(const std::string& directoryPath) {
    std::vector<std::string> discoveredFiles;

    if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
        return discoveredFiles; // Return empty if not a valid directory
    }

    try {
        for (const auto& entry : fs::recursive_directory_iterator(directoryPath)) {
            if (fs::is_regular_file(entry.status())) {
                discoveredFiles.push_back(entry.path().string());
            }
        }
    } catch (const fs::filesystem_error& e) {
        // Handle permissions or other filesystem errors if needed
    }

    return discoveredFiles;
}
