#pragma once

#include "DetectionStrategy.hpp"
#include "IPCMessage.hpp"

#include <string>

class IPCController {
public:
    explicit IPCController(
        const DetectionStrategy& detectionStrategy);

    bool scanFile(
        const std::string& filePath);

private:
    const DetectionStrategy& detectionStrategy_;

    static bool readFull(
        int fd,
        void* buffer,
        std::size_t size);

    static bool writeFull(
        int fd,
        const void* buffer,
        std::size_t size);
};
