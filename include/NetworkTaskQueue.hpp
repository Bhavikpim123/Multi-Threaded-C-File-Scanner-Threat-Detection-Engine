#pragma once

#include "NetworkTask.hpp"

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

class NetworkTaskQueue {
public:
    void push(NetworkTask task);

    std::optional<NetworkTask> pop();

    void shutdown();

private:
    std::queue<NetworkTask> tasks_;

    mutable std::mutex mutex_;
    std::condition_variable condition_;

    bool shutdown_ = false;
};
