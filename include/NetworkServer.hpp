#pragma once

#include "DetectionStrategy.hpp"
#include "FileAnalyzer.hpp"

#include <cstdint>
#include <string>

class NetworkServer {
public:
    NetworkServer(
        std::uint16_t port,
        const DetectionStrategy& detectionStrategy);

    void start();

private:
    void handleClient(int clientSocket);

    std::uint16_t port_;
    FileAnalyzer analyzer_;
};
