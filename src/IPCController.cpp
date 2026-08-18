#include "IPCController.hpp"

#include "IPCWorker.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

IPCController::IPCController(
    const DetectionStrategy& detectionStrategy)
    : detectionStrategy_(detectionStrategy) {
}

bool IPCController::readFull(
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

bool IPCController::writeFull(
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

bool IPCController::scanFile(
    const std::string& filePath) {

    int requestPipe[2];
    int responsePipe[2];

    if (pipe(requestPipe) < 0) {
        std::cerr
            << "Failed to create request pipe: "
            << std::strerror(errno)
            << '\n';

        return false;
    }

    if (pipe(responsePipe) < 0) {
        std::cerr
            << "Failed to create response pipe: "
            << std::strerror(errno)
            << '\n';

        close(requestPipe[0]);
        close(requestPipe[1]);

        return false;
    }

    const pid_t childPid = fork();

    if (childPid < 0) {
        std::cerr
            << "fork() failed: "
            << std::strerror(errno)
            << '\n';

        close(requestPipe[0]);
        close(requestPipe[1]);
        close(responsePipe[0]);
        close(responsePipe[1]);

        return false;
    }

    if (childPid == 0) {
        // Child process

        close(requestPipe[1]);
        close(responsePipe[0]);

        IPCWorker worker(
            detectionStrategy_);

        worker.run(
            requestPipe[0],
            responsePipe[1]);

        close(requestPipe[0]);
        close(responsePipe[1]);

        _exit(0);
    }

    // Parent process

    close(requestPipe[0]);
    close(responsePipe[1]);

    IPCMessage request{};

    std::strncpy(
        request.filePath,
        filePath.c_str(),
        sizeof(request.filePath) - 1);

    request.filePath[
        sizeof(request.filePath) - 1] = '\0';

    if (!writeFull(
            requestPipe[1],
            &request,
            sizeof(request))) {

        std::cerr
            << "Failed to send IPC request.\n";

        close(requestPipe[1]);
        close(responsePipe[0]);

        waitpid(childPid, nullptr, 0);

        return false;
    }

    close(requestPipe[1]);

    IPCMessage response{};

    if (!readFull(
            responsePipe[0],
            &response,
            sizeof(response))) {

        std::cerr
            << "Failed to receive IPC response.\n";

        close(responsePipe[0]);

        waitpid(childPid, nullptr, 0);

        return false;
    }

    close(responsePipe[0]);

    int childStatus = 0;

    if (waitpid(
            childPid,
            &childStatus,
            0) < 0) {

        std::cerr
            << "waitpid() failed: "
            << std::strerror(errno)
            << '\n';

        return false;
    }

    std::string status;

    switch (response.status) {
        case 0:
            status = "SAFE";
            break;

        case 1:
            status = "SUSPICIOUS";
            break;

        default:
            status = "ERROR";
            break;
    }

    std::cout
        << "IPC response:\n"
        << status
        << '|'
        << response.filePath
        << '|'
        << response.fileHash
        << '|'
        << response.fileSize
        << '\n';

    return WIFEXITED(childStatus) &&
           WEXITSTATUS(childStatus) == 0;
}
