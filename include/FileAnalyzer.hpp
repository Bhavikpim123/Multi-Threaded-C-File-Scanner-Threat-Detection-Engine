#pragma once

#include "DetectionStrategy.hpp"
#include "FileHasher.hpp"
#include "ScanResult.hpp"
#include "Task.hpp"

class FileAnalyzer {
public:
    explicit FileAnalyzer(
        const DetectionStrategy& detectionStrategy);

    ScanResult analyze(const ScanTask& task) const;

private:
    FileHasher hasher_;
    const DetectionStrategy& detectionStrategy_;
};
