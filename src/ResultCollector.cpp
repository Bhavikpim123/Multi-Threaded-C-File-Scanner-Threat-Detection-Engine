#include "ResultCollector.hpp"

void ResultCollector::add(const ScanResult& result) {
    std::lock_guard<std::mutex> lock(mutex_);
    results_.push_back(result);
}

std::vector<ScanResult> ResultCollector::results() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return results_;
}

std::size_t ResultCollector::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return results_.size();
}
