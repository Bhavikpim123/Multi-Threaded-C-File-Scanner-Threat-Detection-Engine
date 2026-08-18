#pragma once

#include "ScanResult.hpp"

#include <cstddef>
#include <mutex>
#include <vector>

class ResultCollector {
public:
    void add(const ScanResult& result);

    std::vector<ScanResult> results() const;

    std::size_t size() const;

private:
    mutable std::mutex mutex_;
    std::vector<ScanResult> results_;
};
