#pragma once

#include "FileAnalyzer.hpp"
#include "SignatureDatabase.hpp"
#include "TaskQueue.hpp"

#include <atomic>
#include <cstddef>
#include <thread>
#include <vector>

class ThreadPool {
public:
    ThreadPool(
        std::size_t threadCount,
        const SignatureDatabase& signatureDatabase);

    ~ThreadPool();

    void start();

    void stop();

    void submit(const ScanTask& task);

private:
    void workerLoop();

    TaskQueue taskQueue_;
    FileAnalyzer analyzer_;
    std::vector<std::thread> workers_;
    std::atomic<bool> running_;
    std::size_t threadCount_;
};
