#include "NetworkTaskQueue.hpp"

void NetworkTaskQueue::push(NetworkTask task) {
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (shutdown_) {
            return;
        }

        tasks_.push(task);
    }

    condition_.notify_one();
}

std::optional<NetworkTask> NetworkTaskQueue::pop() {
    std::unique_lock<std::mutex> lock(mutex_);

    condition_.wait(
        lock,
        [this] {
            return shutdown_ || !tasks_.empty();
        });

    if (tasks_.empty()) {
        return std::nullopt;
    }

    NetworkTask task = tasks_.front();
    tasks_.pop();

    return task;
}

void NetworkTaskQueue::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex_);

        shutdown_ = true;
    }

    condition_.notify_all();
}
