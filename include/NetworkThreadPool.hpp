#pragma once

#include "FileAnalyzer.hpp"
#include "NetworkTaskQueue.hpp"
#include "ScannerMetrics.hpp"

#include <cstddef>
#include <thread>
#include <vector>

class NetworkThreadPool {
public:
    NetworkThreadPool(
        std::size_t threadCount,
        std::size_t queueSize,
        const DetectionStrategy& detectionStrategy);

    ~NetworkThreadPool();

    void start();

    bool submit(int clientSocket);

    void stop();

    const ScannerMetrics& metrics() const;

    std::size_t queueSize() const;

    std::size_t queueCapacity() const;

private:
    void workerLoop();

    void handleClient(
        int clientSocket);

    NetworkTaskQueue taskQueue_;

    FileAnalyzer analyzer_;

    std::vector<std::thread> workers_;

    std::size_t threadCount_;

    bool running_ = false;

    ScannerMetrics metrics_;
};
