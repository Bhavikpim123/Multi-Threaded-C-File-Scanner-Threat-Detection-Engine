#pragma once

#include <filesystem>
#include <string>

class FileHasher {
public:
    std::string calculateHash(
        const std::filesystem::path& filePath
    ) const;
};
