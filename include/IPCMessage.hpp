#pragma once

#include <cstdint>

struct IPCMessage {
    std::int32_t status;
    std::uint64_t fileSize;
    char filePath[512];
    char fileHash[65];
};
