#include "ThreadPool.hpp"

#include <iostream>

ThreadPool::ThreadPool(std::size_t threadCount)
    : running_(false),
      threadCount_(threadCount) {
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::start() {
    if (running_) {
        return;
    }

    running_ = true;

    for (std::size_t i = 0; i < threadCount_; ++i) {
        workers_.emplace_back(&ThreadPool::workerLoop, this);
    }
}

void ThreadPool::stop() {
    if (!running_) {
        return;
    }

    running_ = false;

    // Workers currently waiting on TaskQueue::pop()
    // will be handled properly in the next step when
    // we add shutdown support to TaskQueue.
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    workers_.clear();
}

void ThreadPool::workerLoop() {
    std::cout << "Worker thread started: "
              << std::this_thread::get_id()
              << '\n';

    while (running_) {
        // Task processing will be implemented next.
        //
        // We intentionally don't call taskQueue_.pop()
        // yet because TaskQueue currently waits forever
        // when the queue is empty.
        std::this_thread::yield();
    }

    std::cout << "Worker thread stopped: "
              << std::this_thread::get_id()
              << '\n';
}
