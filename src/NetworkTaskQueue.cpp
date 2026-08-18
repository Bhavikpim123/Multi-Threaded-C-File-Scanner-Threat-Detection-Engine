#include "NetworkTaskQueue.hpp"

NetworkTaskQueue::NetworkTaskQueue(
    std::size_t maxSize)
    : tasks_(maxSize) {
}

bool NetworkTaskQueue::push(
    NetworkTask task) {

    std::unique_lock<std::mutex> lock(
        mutex_);

    notFull_.wait(
        lock,
        [this] {
            return shutdown_ ||
                   !tasks_.full();
        });

    if (shutdown_) {
        return false;
    }

    const bool inserted =
        tasks_.push(
            std::move(task));

    if (!inserted) {
        return false;
    }

    lock.unlock();

    notEmpty_.notify_one();

    return true;
}

std::optional<NetworkTask>
NetworkTaskQueue::pop() {

    std::unique_lock<std::mutex> lock(
        mutex_);

    notEmpty_.wait(
        lock,
        [this] {
            return shutdown_ ||
                   !tasks_.empty();
        });

    if (tasks_.empty()) {
        return std::nullopt;
    }

    auto task =
        tasks_.pop();

    lock.unlock();

    notFull_.notify_one();

    return task;
}

void NetworkTaskQueue::shutdown() {

    {
        std::lock_guard<std::mutex> lock(
            mutex_);

        shutdown_ = true;
    }

    notEmpty_.notify_all();

    notFull_.notify_all();
}

bool NetworkTaskQueue::empty() const {

    std::lock_guard<std::mutex> lock(
        mutex_);

    return tasks_.empty();
}

std::size_t NetworkTaskQueue::size() const {

    std::lock_guard<std::mutex> lock(
        mutex_);

    return tasks_.size();
}

std::size_t NetworkTaskQueue::capacity() const {

    std::lock_guard<std::mutex> lock(
        mutex_);

    return tasks_.capacity();
}
