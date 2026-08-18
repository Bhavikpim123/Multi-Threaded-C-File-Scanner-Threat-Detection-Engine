#pragma once

#include <unistd.h>

class FileDescriptor {
public:
    FileDescriptor() = default;

    explicit FileDescriptor(int fd)
        : fd_(fd) {
    }

    ~FileDescriptor() {
        reset();
    }

    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;

    FileDescriptor(FileDescriptor&& other) noexcept
        : fd_(other.fd_) {
        other.fd_ = -1;
    }

    FileDescriptor& operator=(FileDescriptor&& other) noexcept {
        if (this != &other) {
            reset();

            fd_ = other.fd_;
            other.fd_ = -1;
        }

        return *this;
    }

    int get() const {
        return fd_;
    }

    bool valid() const {
        return fd_ >= 0;
    }

    int release() {
        const int result = fd_;
        fd_ = -1;
        return result;
    }

    void reset(int fd = -1) {
        if (fd_ >= 0) {
            close(fd_);
        }

        fd_ = fd;
    }

private:
    int fd_ = -1;
};
