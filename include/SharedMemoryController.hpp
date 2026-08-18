#pragma once

#include "DetectionStrategy.hpp"

#include <string>

class SharedMemoryController {
public:
    explicit SharedMemoryController(
        const DetectionStrategy& detectionStrategy);

    bool scanFile(
        const std::string& filePath);

private:
    const DetectionStrategy& detectionStrategy_;
};
