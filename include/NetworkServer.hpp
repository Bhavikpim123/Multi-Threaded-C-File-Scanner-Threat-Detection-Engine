#pragma once

#include "DetectionStrategy.hpp"
#include "NetworkThreadPool.hpp"

#include <cstddef>
#include <cstdint>

class NetworkServer {
public:
    NetworkServer(
        std::uint16_t port,
        const DetectionStrategy& detectionStrategy,
        std::size_t workerCount,
        std::size_t queueSize);

    void start();

private:
    std::uint16_t port_;

    NetworkThreadPool threadPool_;
};
