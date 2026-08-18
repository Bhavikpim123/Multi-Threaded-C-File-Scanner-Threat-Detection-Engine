#include "ThreadPool.hpp"

#include <iostream>

ThreadPool::ThreadPool(
    std::size_t threadCount,
    const SignatureDatabase& signatureDatabase,
    ResultCollector& resultCollector)
    : analyzer_(signatureDatabase),
      resultCollector_(resultCollector),
      running_(false),
      threadCount_(threadCount) {
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::start() {
    if (running_) {
        return;
    }

    running_ = true;

    for (std::size_t i = 0; i < threadCount_; ++i) {
        workers_.emplace_back(&ThreadPool::workerLoop, this);
    }
}

void ThreadPool::stop() {
    if (!running_) {
        return;
    }

    running_ = false;

    taskQueue_.shutdown();

    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    workers_.clear();
}

void ThreadPool::submit(const ScanTask& task) {
    taskQueue_.push(task);
}

void ThreadPool::workerLoop() {
    std::cout << "Worker thread started: "
              << std::this_thread::get_id()
              << '\n';
	    while (true) {
        std::optional<ScanTask> task = taskQueue_.pop();

        if (!task.has_value()) {
            break;
        }

        const ScanResult result = analyzer_.analyze(*task);

        resultCollector_.add(result);

        const char* status = "ERROR";

        switch (result.status) {
            case ScanStatus::Safe:
                status = "SAFE";
                break;

            case ScanStatus::Suspicious:
                status = "SUSPICIOUS";
                break;

            case ScanStatus::Error:
                status = "ERROR";
                break;
        }

        std::cout << "Worker "
                  << std::this_thread::get_id()
                  << " analyzed: "
                  << result.filePath
                  << " | Size: "
                  << result.fileSize
                  << " bytes"
                  << " | Extension: "
                  << result.extension
                  << " | Readable: "
                  << (result.readable ? "yes" : "no")
                  << " | SHA-256: "
                  << result.fileHash
                  << " | Status: "
                  << status
                  << " | Time: "
                  << result.scanDuration.count()
                  << " us"
                  << '\n';
    }

    std::cout << "Worker thread stopped: "
              << std::this_thread::get_id()
              << '\n';
}
