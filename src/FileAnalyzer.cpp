#include "FileAnalyzer.hpp"

#include <chrono>
#include <fstream>

ScanResult FileAnalyzer::analyze(const ScanTask& task) const {
    const auto start = std::chrono::steady_clock::now();

    ScanResult result{};
    result.filePath = task.filePath;
    result.fileSize = task.fileSize;
    result.extension = task.filePath.extension().string();

    std::ifstream file(task.filePath, std::ios::binary);
    result.readable = file.good();

    const auto end = std::chrono::steady_clock::now();

    result.scanDuration =
        std::chrono::duration_cast<std::chrono::microseconds>(
            end - start
        );

    return result;
}
