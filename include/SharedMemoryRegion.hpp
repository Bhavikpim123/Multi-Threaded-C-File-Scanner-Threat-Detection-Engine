#pragma once

#include <cstdint>
#include <semaphore.h>

struct SharedMemoryRegion {
    sem_t requestReady;
    sem_t responseReady;

    std::int32_t status;

    std::uint64_t fileSize;

    char filePath[512];

    char fileHash[65];
};
