#pragma once

#include <atomic>
#include <cstdint>

class ScannerMetrics {
public:
    void requestAccepted() {
        requestsAccepted_.fetch_add(
            1,
            std::memory_order_relaxed);
    }

    void requestRejected() {
        requestsRejected_.fetch_add(
            1,
            std::memory_order_relaxed);
    }

    void requestCompleted() {
        requestsCompleted_.fetch_add(
            1,
            std::memory_order_relaxed);
    }

    void requestFailed() {
        requestsFailed_.fetch_add(
            1,
            std::memory_order_relaxed);
    }

    void workerStarted() {
        activeWorkers_.fetch_add(
            1,
            std::memory_order_relaxed);
    }

    void workerFinished() {
        activeWorkers_.fetch_sub(
            1,
            std::memory_order_relaxed);
    }

    std::uint64_t requestsAccepted() const {
        return requestsAccepted_.load(
            std::memory_order_relaxed);
    }

    std::uint64_t requestsRejected() const {
        return requestsRejected_.load(
            std::memory_order_relaxed);
    }

    std::uint64_t requestsCompleted() const {
        return requestsCompleted_.load(
            std::memory_order_relaxed);
    }

    std::uint64_t requestsFailed() const {
        return requestsFailed_.load(
            std::memory_order_relaxed);
    }

    std::uint64_t activeWorkers() const {
        return activeWorkers_.load(
            std::memory_order_relaxed);
    }

private:
    std::atomic<std::uint64_t>
        requestsAccepted_{0};

    std::atomic<std::uint64_t>
        requestsRejected_{0};

    std::atomic<std::uint64_t>
        requestsCompleted_{0};

    std::atomic<std::uint64_t>
        requestsFailed_{0};

    std::atomic<std::uint64_t>
        activeWorkers_{0};
};
