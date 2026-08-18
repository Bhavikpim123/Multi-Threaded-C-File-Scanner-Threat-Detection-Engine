#include "SharedMemoryController.hpp"

#include "IPCMessage.hpp"
#include "SharedMemoryRegion.hpp"
#include "Task.hpp"
#include "FileAnalyzer.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctime>

#include <filesystem>

namespace {

constexpr const char* SHARED_MEMORY_NAME =
    "/bhavik_scanner_shared_memory";

void cleanupSharedMemory(
    int shmFd,
    SharedMemoryRegion* region) {

    if (region != nullptr &&
        region != MAP_FAILED) {

        munmap(
            region,
            sizeof(SharedMemoryRegion));
    }

    if (shmFd >= 0) {
        close(shmFd);
    }

    shm_unlink(
        SHARED_MEMORY_NAME);
}

}

SharedMemoryController::SharedMemoryController(
    const DetectionStrategy& detectionStrategy)
    : detectionStrategy_(detectionStrategy) {
}

bool SharedMemoryController::scanFile(
    const std::string& filePath) {

    shm_unlink(
        SHARED_MEMORY_NAME);

    const int shmFd =
        shm_open(
            SHARED_MEMORY_NAME,
            O_CREAT | O_RDWR,
            0600);

    if (shmFd < 0) {
        std::cerr
            << "shm_open() failed: "
            << std::strerror(errno)
            << '\n';

        return false;
    }

    if (ftruncate(
            shmFd,
            sizeof(SharedMemoryRegion)) < 0) {

        std::cerr
            << "ftruncate() failed: "
            << std::strerror(errno)
            << '\n';

        close(shmFd);
        shm_unlink(SHARED_MEMORY_NAME);

        return false;
    }

    auto* region =
        static_cast<SharedMemoryRegion*>(
            mmap(
                nullptr,
                sizeof(SharedMemoryRegion),
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                shmFd,
                0));

    if (region == MAP_FAILED) {
        std::cerr
            << "mmap() failed: "
            << std::strerror(errno)
            << '\n';

        close(shmFd);
        shm_unlink(SHARED_MEMORY_NAME);

        return false;
    }

    std::memset(
        region,
        0,
        sizeof(SharedMemoryRegion));

    if (sem_init(
            &region->requestReady,
            1,
            0) < 0) {

        std::cerr
            << "Failed to initialize request semaphore: "
            << std::strerror(errno)
            << '\n';

        cleanupSharedMemory(
            shmFd,
            region);

        return false;
    }

    if (sem_init(
            &region->responseReady,
            1,
            0) < 0) {

        std::cerr
            << "Failed to initialize response semaphore: "
            << std::strerror(errno)
            << '\n';

        sem_destroy(
            &region->requestReady);

        cleanupSharedMemory(
            shmFd,
            region);

        return false;
    }

    std::strncpy(
        region->filePath,
        filePath.c_str(),
        sizeof(region->filePath) - 1);

    region->filePath[
        sizeof(region->filePath) - 1] = '\0';

    const pid_t childPid = fork();

    if (childPid < 0) {
        std::cerr
            << "fork() failed: "
            << std::strerror(errno)
            << '\n';

        sem_destroy(
            &region->requestReady);

        sem_destroy(
            &region->responseReady);

        cleanupSharedMemory(
            shmFd,
            region);

        return false;
    }

    if (childPid == 0) {
        // Child process

        if (sem_wait(
                &region->requestReady) < 0) {

            _exit(1);
        }

        const std::filesystem::path path(
            region->filePath);

        std::error_code error;

        if (!std::filesystem::is_regular_file(
                path,
                error)) {

            region->status = 2;
            region->fileSize = 0;
            region->fileHash[0] = '\0';

            sem_post(
                &region->responseReady);

            munmap(
                region,
                sizeof(SharedMemoryRegion));

            close(shmFd);

            _exit(0);
        }

        ScanTask task;

        task.filePath = path;

        task.fileSize =
            std::filesystem::file_size(
                path,
                error);

        if (error) {
            region->status = 2;
            region->fileSize = 0;
            region->fileHash[0] = '\0';

            sem_post(
                &region->responseReady);

            munmap(
                region,
                sizeof(SharedMemoryRegion));

            close(shmFd);

            _exit(0);
        }

        FileAnalyzer analyzer(
            detectionStrategy_);

        const ScanResult result =
            analyzer.analyze(task);

        region->fileSize =
            result.fileSize;

        std::strncpy(
            region->fileHash,
            result.fileHash.c_str(),
            sizeof(region->fileHash) - 1);

        region->fileHash[
            sizeof(region->fileHash) - 1] = '\0';

        switch (result.status) {
            case ScanStatus::Safe:
                region->status = 0;
                break;

            case ScanStatus::Suspicious:
                region->status = 1;
                break;

            case ScanStatus::Error:
                region->status = 2;
                break;
        }

        sem_post(
            &region->responseReady);

        munmap(
            region,
            sizeof(SharedMemoryRegion));

        close(shmFd);

        _exit(0);
    }

    // Parent process

    if (sem_post(
            &region->requestReady) < 0) {

        std::cerr
            << "sem_post() failed: "
            << std::strerror(errno)
            << '\n';

        kill(childPid, SIGTERM);

        waitpid(
            childPid,
            nullptr,
            0);

        sem_destroy(
            &region->requestReady);

        sem_destroy(
            &region->responseReady);

        cleanupSharedMemory(
            shmFd,
            region);

        return false;
    }

    timespec timeout{};

    if (clock_gettime(
            CLOCK_REALTIME,
            &timeout) < 0) {

        std::cerr
            << "clock_gettime() failed: "
            << std::strerror(errno)
            << '\n';

        kill(childPid, SIGTERM);

        waitpid(
            childPid,
            nullptr,
            0);

        sem_destroy(
            &region->requestReady);

        sem_destroy(
            &region->responseReady);

        cleanupSharedMemory(
            shmFd,
            region);

        return false;
    }

    timeout.tv_sec += 5;

    if (sem_timedwait(
            &region->responseReady,
            &timeout) < 0) {

        if (errno == ETIMEDOUT) {

            std::cerr
                << "Shared memory IPC timed out.\n";

        } else {

            std::cerr
                << "sem_timedwait() failed: "
                << std::strerror(errno)
                << '\n';
        }

        kill(childPid, SIGTERM);

        waitpid(
            childPid,
            nullptr,
            0);

        sem_destroy(
            &region->requestReady);

        sem_destroy(
            &region->responseReady);

        cleanupSharedMemory(
            shmFd,
            region);

        return false;
    }



    std::string status;

    switch (region->status) {
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
        << "Shared memory response:\n"
        << status
        << '|'
        << region->filePath
        << '|'
        << region->fileHash
        << '|'
        << region->fileSize
        << '\n';

    int childStatus = 0;

    if (waitpid(
            childPid,
            &childStatus,
            0) < 0) {

        std::cerr
            << "waitpid() failed: "
            << std::strerror(errno)
            << '\n';

        sem_destroy(
            &region->requestReady);

        sem_destroy(
            &region->responseReady);

        cleanupSharedMemory(
            shmFd,
            region);

        return false;
    }

    sem_destroy(
        &region->requestReady);

    sem_destroy(
        &region->responseReady);

    cleanupSharedMemory(
        shmFd,
        region);

    return WIFEXITED(childStatus) &&
           WEXITSTATUS(childStatus) == 0;
}
