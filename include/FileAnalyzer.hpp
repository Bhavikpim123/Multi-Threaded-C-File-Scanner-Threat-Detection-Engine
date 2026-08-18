#pragma once

#include "FileHasher.hpp"
#include "ScanResult.hpp"
#include "SignatureDatabase.hpp"
#include "Task.hpp"

class FileAnalyzer {
public:
    explicit FileAnalyzer(const SignatureDatabase& signatureDatabase);

    ScanResult analyze(const ScanTask& task) const;

private:
    FileHasher hasher_;
    const SignatureDatabase& signatureDatabase_;
};
