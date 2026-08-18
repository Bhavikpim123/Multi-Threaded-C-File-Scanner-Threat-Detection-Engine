#pragma once

#include "Task.hpp"

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

class TaskQueue {
public:
    void push(const ScanTask& task);

    std::optional<ScanTask> pop();

    void shutdown();

    bool empty() const;

private:
    std::queue<ScanTask> queue_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    bool shutdownRequested_ = false;
};
