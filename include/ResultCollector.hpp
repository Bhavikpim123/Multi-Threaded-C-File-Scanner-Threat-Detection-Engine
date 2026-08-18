#pragma once

#include "ScanResult.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <vector>

class ResultCollector {
public:
    void add(const ScanResult& result);

    std::vector<ScanResult> results() const;

    std::size_t size() const;

    std::uintmax_t totalBytes() const;
    std::chrono::microseconds totalAnalysisTime() const;
    std::chrono::microseconds averageHashTime() const;
    std::chrono::microseconds averageDetectionTime() const;

private:
    mutable std::mutex mutex_;
    std::vector<ScanResult> results_;
};
