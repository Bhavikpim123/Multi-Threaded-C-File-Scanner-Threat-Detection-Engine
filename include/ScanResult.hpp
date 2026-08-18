#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>

enum class ScanStatus {
    Safe,
    Suspicious,
    Error
};

struct ScanResult {
    std::filesystem::path filePath;
    std::uintmax_t fileSize;
    std::string extension;
    bool readable;
    std::string fileHash;
    ScanStatus status;
    std::chrono::microseconds scanDuration;
};
