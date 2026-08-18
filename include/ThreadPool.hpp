#pragma once

#include "TaskQueue.hpp"

#include <atomic>
#include <cstddef>
#include <thread>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(std::size_t threadCount);

    ~ThreadPool();

    void start();

    void stop();

private:
    void workerLoop();

    TaskQueue taskQueue_;
    std::vector<std::thread> workers_;
    std::atomic<bool> running_;
    std::size_t threadCount_;
};
