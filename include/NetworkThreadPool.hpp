#pragma once

#include "FileAnalyzer.hpp"
#include "NetworkTaskQueue.hpp"

#include <cstddef>
#include <thread>
#include <vector>

class NetworkThreadPool {
public:
    NetworkThreadPool(
        std::size_t threadCount,
        const DetectionStrategy& detectionStrategy);

    ~NetworkThreadPool();

    void start();

    void submit(int clientSocket);

    void stop();

private:
    void workerLoop();

    void handleClient(int clientSocket);

    NetworkTaskQueue taskQueue_;

    FileAnalyzer analyzer_;

    std::vector<std::thread> workers_;

    std::size_t threadCount_;

    bool running_;
};
