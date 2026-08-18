#pragma once

#include "DetectionStrategy.hpp"

class IPCWorker {
public:
    explicit IPCWorker(
        const DetectionStrategy& detectionStrategy);

    void run(
        int requestReadFd,
        int responseWriteFd);

private:
    const DetectionStrategy& detectionStrategy_;
};
