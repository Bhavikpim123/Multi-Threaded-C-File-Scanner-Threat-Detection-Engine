#pragma once

#include <cstdint>
#include <string>

class NetworkClient {
public:
    NetworkClient(
        const std::string& host,
        std::uint16_t port);

    bool scanFile(const std::string& filePath);

private:
    std::string host_;
    std::uint16_t port_;
};
