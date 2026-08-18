#include "NetworkClient.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

NetworkClient::NetworkClient(
    const std::string& host,
    std::uint16_t port)
    : host_(host),
      port_(port) {
}

bool NetworkClient::scanFile(
    const std::string& filePath) {

    const int socketFd =
        socket(AF_INET, SOCK_STREAM, 0);

    if (socketFd < 0) {
        std::cerr << "Failed to create client socket: "
                  << std::strerror(errno)
                  << '\n';

        return false;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_);

    if (inet_pton(
            AF_INET,
            host_.c_str(),
            &serverAddress.sin_addr) <= 0) {

        std::cerr << "Invalid server address.\n";

        close(socketFd);
        return false;
    }

    if (connect(
            socketFd,
            reinterpret_cast<sockaddr*>(&serverAddress),
            sizeof(serverAddress)) < 0) {

        std::cerr << "Failed to connect: "
                  << std::strerror(errno)
                  << '\n';

        close(socketFd);
        return false;
    }

    std::cout << "Connected to scanner server.\n";

    if (send(
            socketFd,
            filePath.c_str(),
            filePath.size(),
            0) < 0) {

        std::cerr << "Failed to send scan request.\n";

        close(socketFd);
        return false;
    }

    char buffer[4096]{};

    const ssize_t bytesReceived =
        recv(
            socketFd,
            buffer,
            sizeof(buffer) - 1,
            0);

    if (bytesReceived <= 0) {
        std::cerr << "Failed to receive response.\n";

        close(socketFd);
        return false;
    }

    buffer[bytesReceived] = '\0';

    std::cout << "Server response:\n"
              << buffer;

    close(socketFd);

    return true;
}
