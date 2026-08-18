#include "TaskQueue.hpp"

void TaskQueue::push(const ScanTask& task) {
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (shutdownRequested_) {
            return;
        }

        queue_.push(task);
    }

    condition_.notify_one();
}

std::optional<ScanTask> TaskQueue::pop() {
    std::unique_lock<std::mutex> lock(mutex_);

    condition_.wait(lock, [this] {
        return !queue_.empty() || shutdownRequested_;
    });

    if (queue_.empty() && shutdownRequested_) {
        return std::nullopt;
    }

    ScanTask task = queue_.front();
    queue_.pop();

    return task;
}

void TaskQueue::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        shutdownRequested_ = true;
    }

    condition_.notify_all();
}

bool TaskQueue::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}
