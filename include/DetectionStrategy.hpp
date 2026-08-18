#pragma once

#include "ScanResult.hpp"

#include <string>

class DetectionStrategy {
public:
    virtual ~DetectionStrategy() = default;

    virtual ScanStatus detect(
        const std::string& fileHash) const = 0;
};
