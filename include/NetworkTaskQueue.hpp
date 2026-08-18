#pragma once

#include "BoundedRingBuffer.hpp"
#include "NetworkTask.hpp"

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>

class NetworkTaskQueue {
public:
    explicit NetworkTaskQueue(
        std::size_t maxSize);

    bool push(NetworkTask task);

    std::optional<NetworkTask> pop();

    void shutdown();

    bool empty() const;

    std::size_t size() const;

    std::size_t capacity() const;

private:
    BoundedRingBuffer<NetworkTask> tasks_;

    mutable std::mutex mutex_;

    std::condition_variable notEmpty_;

    std::condition_variable notFull_;

    bool shutdown_ = false;
};
