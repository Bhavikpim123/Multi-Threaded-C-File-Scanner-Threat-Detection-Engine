#pragma once

#include <filesystem>
#include <cstdint>

struct ScanTask {
    std::filesystem::path filePath;
    std::uintmax_t fileSize;
};
