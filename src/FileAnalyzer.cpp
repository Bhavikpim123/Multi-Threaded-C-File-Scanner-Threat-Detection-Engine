#include "FileAnalyzer.hpp"

#include <chrono>
#include <fstream>

FileAnalyzer::FileAnalyzer(
    const DetectionStrategy& detectionStrategy)
    : detectionStrategy_(detectionStrategy) {
}

ScanResult FileAnalyzer::analyze(
    const ScanTask& task) const {

    const auto start = std::chrono::steady_clock::now();

    ScanResult result{};

    result.filePath = task.filePath;
    result.fileSize = task.fileSize;
    result.extension = task.filePath.extension().string();
    result.status = ScanStatus::Error;

    std::ifstream file(task.filePath, std::ios::binary);

    result.readable = file.good();

    if (!result.readable) {
        const auto end = std::chrono::steady_clock::now();

        result.scanDuration =
            std::chrono::duration_cast<std::chrono::microseconds>(
                end - start
            );

        return result;
    }

    result.fileHash = hasher_.calculateHash(task.filePath);

    result.status =
        detectionStrategy_.detect(result.fileHash);

    const auto end = std::chrono::steady_clock::now();

    result.scanDuration =
        std::chrono::duration_cast<std::chrono::microseconds>(
            end - start
        );

    return result;
}
