#include "NetworkThreadPool.hpp"

#include "Task.hpp"

#include <cerrno>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

NetworkThreadPool::NetworkThreadPool(
    std::size_t threadCount,
    const DetectionStrategy& detectionStrategy)
    : analyzer_(detectionStrategy),
      threadCount_(threadCount),
      running_(false) {
}

NetworkThreadPool::~NetworkThreadPool() {
    stop();
}

void NetworkThreadPool::start() {
    if (running_) {
        return;
    }

    running_ = true;

    workers_.reserve(threadCount_);

    for (std::size_t i = 0; i < threadCount_; ++i) {
        workers_.emplace_back(
            &NetworkThreadPool::workerLoop,
            this
        );
    }

    std::cout
        << "Network thread pool started with "
        << threadCount_
        << " workers.\n";
}

void NetworkThreadPool::submit(int clientSocket) {
    taskQueue_.push(NetworkTask{clientSocket});
}

void NetworkThreadPool::stop() {
    if (!running_) {
        return;
    }

    running_ = false;

    taskQueue_.shutdown();

    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    workers_.clear();
}

void NetworkThreadPool::workerLoop() {
    while (true) {
        auto task = taskQueue_.pop();

        if (!task.has_value()) {
            break;
        }

        handleClient(task->clientSocket);

        close(task->clientSocket);
    }
}

void NetworkThreadPool::handleClient(int clientSocket) {
    char buffer[4096]{};

    const ssize_t bytesReceived =
        recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

    if (bytesReceived <= 0) {
        return;
    }

    buffer[bytesReceived] = '\0';

    const std::string filePath(buffer);

    std::cout
        << "Worker "
        << std::this_thread::get_id()
        << " processing: "
        << filePath
        << '\n';

    std::error_code error;

    if (!std::filesystem::is_regular_file(
            filePath,
            error)) {

        const std::string response =
            "ERROR|File does not exist\n";

        send(
            clientSocket,
            response.c_str(),
            response.size(),
            0
        );

        return;
    }

    ScanTask task;

    task.filePath = filePath;

    task.fileSize =
        std::filesystem::file_size(
            task.filePath,
            error
        );

    if (error) {
        const std::string response =
            "ERROR|Unable to read file size\n";

        send(
            clientSocket,
            response.c_str(),
            response.size(),
            0
        );

        return;
    }

    const ScanResult result =
        analyzer_.analyze(task);

    std::string status;

    switch (result.status) {
        case ScanStatus::Safe:
            status = "SAFE";
            break;

        case ScanStatus::Suspicious:
            status = "SUSPICIOUS";
            break;

        case ScanStatus::Error:
            status = "ERROR";
            break;
    }

    const std::string response =
        status + "|" +
        result.filePath.string() + "|" +
        result.fileHash + "|" +
        std::to_string(result.fileSize) +
        "\n";

    send(
        clientSocket,
        response.c_str(),
        response.size(),
        0
    );
}
