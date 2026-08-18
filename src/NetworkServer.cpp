#include "NetworkServer.hpp"

#include "Task.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

NetworkServer::NetworkServer(
    std::uint16_t port,
    const DetectionStrategy& detectionStrategy)
    : port_(port),
      analyzer_(detectionStrategy) {
}

void NetworkServer::start() {
    const int serverSocket =
        socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0) {
        std::cerr << "Failed to create socket: "
                  << std::strerror(errno)
                  << '\n';
        return;
    }

    int reuseAddress = 1;

    if (setsockopt(
            serverSocket,
            SOL_SOCKET,
            SO_REUSEADDR,
            &reuseAddress,
            sizeof(reuseAddress)) < 0) {

        std::cerr << "Failed to configure socket: "
                  << std::strerror(errno)
                  << '\n';

        close(serverSocket);
        return;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port_);

    if (bind(
            serverSocket,
            reinterpret_cast<sockaddr*>(&serverAddress),
            sizeof(serverAddress)) < 0) {

        std::cerr << "Failed to bind socket: "
                  << std::strerror(errno)
                  << '\n';

        close(serverSocket);
        return;
    }

    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Failed to listen: "
                  << std::strerror(errno)
                  << '\n';

        close(serverSocket);
        return;
    }

    std::cout << "Scanner server listening on port "
              << port_
              << '\n';

    while (true) {
        sockaddr_in clientAddress{};
        socklen_t clientLength =
            sizeof(clientAddress);

        const int clientSocket =
            accept(
                serverSocket,
                reinterpret_cast<sockaddr*>(&clientAddress),
                &clientLength);

        if (clientSocket < 0) {
            std::cerr << "Failed to accept client: "
                      << std::strerror(errno)
                      << '\n';
            continue;
        }

        std::cout << "Client connected.\n";

        handleClient(clientSocket);

        close(clientSocket);

        std::cout << "Client disconnected.\n";
    }

    close(serverSocket);
}

void NetworkServer::handleClient(int clientSocket) {
    char buffer[4096]{};

    const ssize_t bytesReceived =
        recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0);

    if (bytesReceived <= 0) {
        return;
    }

    buffer[bytesReceived] = '\0';

    const std::string filePath(buffer);

    std::cout << "Scan request received: "
              << filePath
              << '\n';

    std::error_code error;

    if (!std::filesystem::is_regular_file(
            filePath,
            error)) {

        const std::string response =
            "ERROR|File does not exist\n";

        send(
            clientSocket,
            response.c_str(),
            response.size(),
            0);

        return;
    }

    ScanTask task;

    task.filePath = filePath;

    task.fileSize =
        std::filesystem::file_size(
            task.filePath,
            error);

    if (error) {
        const std::string response =
            "ERROR|Unable to read file size\n";

        send(
            clientSocket,
            response.c_str(),
            response.size(),
            0);

        return;
    }

    const ScanResult result =
        analyzer_.analyze(task);

    std::string status;

    switch (result.status) {
        case ScanStatus::Safe:
            status = "SAFE";
            break;

        case ScanStatus::Suspicious:
            status = "SUSPICIOUS";
            break;

        case ScanStatus::Error:
            status = "ERROR";
            break;
    }

    const std::string response =
        status + "|" +
        result.filePath.string() + "|" +
        result.fileHash + "|" +
        std::to_string(result.fileSize) +
        "\n";

    send(
        clientSocket,
        response.c_str(),
        response.size(),
        0);
}
