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

std::uintmax_t ResultCollector::totalBytes() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::uintmax_t total = 0;

    for (const auto& result : results_) {
        total += result.fileSize;
    }

    return total;
}

std::chrono::microseconds
ResultCollector::totalAnalysisTime() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::chrono::microseconds total{0};

    for (const auto& result : results_) {
        total += result.scanDuration;
    }

    return total;
}

std::chrono::microseconds
ResultCollector::averageHashTime() const {
    std::lock_guard<std::mutex> lock(mutex_);

    if (results_.empty()) {
        return std::chrono::microseconds{0};
    }

    std::chrono::microseconds total{0};

    for (const auto& result : results_) {
        total += result.hashDuration;
    }

    return total / results_.size();
}

std::chrono::microseconds
ResultCollector::averageDetectionTime() const {
    std::lock_guard<std::mutex> lock(mutex_);

    if (results_.empty()) {
        return std::chrono::microseconds{0};
    }

    std::chrono::microseconds total{0};

    for (const auto& result : results_) {
        total += result.detectionDuration;
    }

    return total / results_.size();
}
