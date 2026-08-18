#pragma once

#include "DetectionStrategy.hpp"
#include "FileAnalyzer.hpp"
#include "ResultCollector.hpp"
#include "TaskQueue.hpp"

#include <atomic>
#include <cstddef>
#include <thread>
#include <vector>

class ThreadPool {
public:
    ThreadPool(
        std::size_t threadCount,
        const DetectionStrategy& detectionStrategy,
        ResultCollector& resultCollector);

    ~ThreadPool();

    void start();

    void stop();

    void submit(const ScanTask& task);

private:
    void workerLoop();

    TaskQueue taskQueue_;
    FileAnalyzer analyzer_;
    ResultCollector& resultCollector_;
    std::vector<std::thread> workers_;
    std::atomic<bool> running_;
    std::size_t threadCount_;
};
