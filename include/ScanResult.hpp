#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>

struct ScanResult {
    std::filesystem::path filePath;
    std::uintmax_t fileSize;
    std::string extension;
    bool readable;
    std::chrono::microseconds scanDuration;
};
