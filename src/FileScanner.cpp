#include "FileScanner.hpp"
#include <filesystem>

namespace fs = std::filesystem;

std::vector<ScanTask> FileScanner::discoverFiles(const std::string& directoryPath) {
    std::vector<ScanTask> discoveredTasks;

    if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
        return discoveredTasks; // Return empty if not a valid directory
    }

    try {
        for (const auto& entry : fs::recursive_directory_iterator(directoryPath)) {
            if (fs::is_regular_file(entry.status())) {
                ScanTask task;
                task.filePath = entry.path();
                task.fileSize = fs::file_size(entry.path());
                discoveredTasks.push_back(task);
            }
        }
    } catch (const fs::filesystem_error& e) {
        // Handle permissions or other filesystem errors if needed
    }

    return discoveredTasks;
}
