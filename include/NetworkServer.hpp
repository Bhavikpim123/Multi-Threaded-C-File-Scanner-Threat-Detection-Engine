#pragma once

#include "DetectionStrategy.hpp"
#include "NetworkThreadPool.hpp"

#include <cstdint>
#include <cstddef>

class NetworkServer {
public:
    NetworkServer(
        std::uint16_t port,
        const DetectionStrategy& detectionStrategy,
        std::size_t workerCount);

    void start();

private:
    std::uint16_t port_;

    NetworkThreadPool threadPool_;
};
