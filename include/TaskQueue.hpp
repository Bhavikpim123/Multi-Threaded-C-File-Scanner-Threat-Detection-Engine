#pragma once

#include "Task.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>

class TaskQueue {
public:
    void push(const ScanTask& task);

    ScanTask pop();

    bool empty() const;

private:
    std::queue<ScanTask> queue_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
};
