#include "NetworkThreadPool.hpp"

#include "Task.hpp"

#include <cerrno>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

NetworkThreadPool::NetworkThreadPool(
    std::size_t threadCount,
    std::size_t queueSize,
    const DetectionStrategy& detectionStrategy)
    : taskQueue_(queueSize),
      analyzer_(detectionStrategy),
      threadCount_(threadCount) {
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

    for (std::size_t i = 0;
         i < threadCount_;
         ++i) {

        workers_.emplace_back(
            &NetworkThreadPool::workerLoop,
            this);
    }

    std::cout
        << "Network thread pool started with "
        << threadCount_
        << " workers.\n";
}

bool NetworkThreadPool::submit(
    int clientSocket) {

    if (!running_) {
        metrics_.requestRejected();

        close(clientSocket);

        return false;
    }

    const bool accepted =
        taskQueue_.push(
            NetworkTask{clientSocket});

    if (!accepted) {
        metrics_.requestRejected();

        close(clientSocket);

        return false;
    }

    metrics_.requestAccepted();

    return true;
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

    std::cout
        << "Network thread pool stopped.\n";
}

void NetworkThreadPool::workerLoop() {

    metrics_.workerStarted();

    while (true) {

        auto task =
            taskQueue_.pop();

        if (!task.has_value()) {
            break;
        }

        handleClient(
            task->clientSocket);

        close(
            task->clientSocket);
    }

    metrics_.workerFinished();
}

void NetworkThreadPool::handleClient(
    int clientSocket) {

    char buffer[4096]{};

    const ssize_t bytesReceived =
        recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0);

    if (bytesReceived <= 0) {

        metrics_.requestFailed();

        return;
    }

    buffer[bytesReceived] = '\0';

    const std::string filePath(
        buffer);

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
            0);

        metrics_.requestFailed();

        return;
    }

    ScanTask task;

    task.filePath = filePath;

    task.fileSize =
        std::filesystem::file_size(
            task.filePath,
            error);

    if (error) {

        const std::string response =
            "ERROR|Unable to read file size\n";

        send(
            clientSocket,
            response.c_str(),
            response.size(),
            0);

        metrics_.requestFailed();

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

    const ssize_t sent =
        send(
            clientSocket,
            response.c_str(),
            response.size(),
            0);

    if (sent < 0) {
        metrics_.requestFailed();
        return;
    }

    metrics_.requestCompleted();
}

const ScannerMetrics&
NetworkThreadPool::metrics() const {
    return metrics_;
}
