#include "TaskQueue.hpp"

void TaskQueue::push(const ScanTask& task) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(task);
    }

    condition_.notify_one();
}

ScanTask TaskQueue::pop() {
    std::unique_lock<std::mutex> lock(mutex_);

    condition_.wait(lock, [this] {
        return !queue_.empty();
    });

    ScanTask task = queue_.front();
    queue_.pop();

    return task;
}

bool TaskQueue::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}
