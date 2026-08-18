#pragma once

#include "ScanResult.hpp"
#include "Task.hpp"

class FileAnalyzer {
public:
    ScanResult analyze(const ScanTask& task) const;
};
