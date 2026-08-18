#include "IPCWorker.hpp"

#include "IPCMessage.hpp"
#include "Task.hpp"
#include "FileAnalyzer.hpp"

#include <cerrno>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

namespace {

bool readFull(
    int fd,
    void* buffer,
    std::size_t size) {

    auto* data =
        static_cast<char*>(buffer);

    std::size_t totalRead = 0;

    while (totalRead < size) {
        const ssize_t bytes =
            read(
                fd,
                data + totalRead,
                size - totalRead);

        if (bytes == 0) {
            return false;
        }

        if (bytes < 0) {
            if (errno == EINTR) {
                continue;
            }

            return false;
        }

        totalRead +=
            static_cast<std::size_t>(bytes);
    }

    return true;
}

bool writeFull(
    int fd,
    const void* buffer,
    std::size_t size) {

    const auto* data =
        static_cast<const char*>(buffer);

    std::size_t totalWritten = 0;

    while (totalWritten < size) {
        const ssize_t bytes =
            write(
                fd,
                data + totalWritten,
                size - totalWritten);

        if (bytes < 0) {
            if (errno == EINTR) {
                continue;
            }

            return false;
        }

        totalWritten +=
            static_cast<std::size_t>(bytes);
    }

    return true;
}

} // namespace

IPCWorker::IPCWorker(
    const DetectionStrategy& detectionStrategy)
    : detectionStrategy_(detectionStrategy) {
}

void IPCWorker::run(
    int requestReadFd,
    int responseWriteFd) {

    IPCMessage request{};

    if (!readFull(
            requestReadFd,
            &request,
            sizeof(request))) {

        std::cerr
            << "IPC worker failed to read request.\n";

        return;
    }

    IPCMessage response{};

    std::strncpy(
        response.filePath,
        request.filePath,
        sizeof(response.filePath) - 1);

    response.filePath[
        sizeof(response.filePath) - 1] = '\0';

    const std::filesystem::path filePath(
        request.filePath);

    std::error_code error;

    if (!std::filesystem::is_regular_file(
            filePath,
            error)) {

        response.status = 2;
        response.fileSize = 0;
        response.fileHash[0] = '\0';

        writeFull(
            responseWriteFd,
            &response,
            sizeof(response));

        return;
    }

    ScanTask task;

    task.filePath = filePath;

    task.fileSize =
        std::filesystem::file_size(
            filePath,
            error);

    if (error) {
        response.status = 2;
        response.fileSize = 0;
        response.fileHash[0] = '\0';

        writeFull(
            responseWriteFd,
            &response,
            sizeof(response));

        return;
    }

    const ScanResult result =
        FileAnalyzer(
            detectionStrategy_)
            .analyze(task);

    response.fileSize = result.fileSize;

    std::strncpy(
        response.fileHash,
        result.fileHash.c_str(),
        sizeof(response.fileHash) - 1);

    response.fileHash[
        sizeof(response.fileHash) - 1] = '\0';

    switch (result.status) {
        case ScanStatus::Safe:
            response.status = 0;
            break;

        case ScanStatus::Suspicious:
            response.status = 1;
            break;

        case ScanStatus::Error:
            response.status = 2;
            break;
    }

    if (!writeFull(
            responseWriteFd,
            &response,
            sizeof(response))) {

        std::cerr
            << "IPC worker failed to write response.\n";
    }
}
