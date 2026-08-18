#pragma once

#include "FileHasher.hpp"
#include "ScanResult.hpp"
#include "Task.hpp"

class FileAnalyzer {
public:
    ScanResult analyze(const ScanTask& task) const;

private:
    FileHasher hasher_;
};
