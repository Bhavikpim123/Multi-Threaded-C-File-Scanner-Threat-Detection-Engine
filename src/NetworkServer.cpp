#include "NetworkServer.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

NetworkServer::NetworkServer(
    std::uint16_t port,
    const DetectionStrategy& detectionStrategy,
    std::size_t workerCount)
    : port_(port),
      threadPool_(
          workerCount,
          detectionStrategy) {
}

void NetworkServer::start() {
    const int serverSocket =
        socket(
            AF_INET,
            SOCK_STREAM,
            0
        );

    if (serverSocket < 0) {
        std::cerr
            << "Failed to create socket: "
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

        std::cerr
            << "Failed to configure socket: "
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

        std::cerr
            << "Failed to bind socket: "
            << std::strerror(errno)
            << '\n';

        close(serverSocket);
        return;
    }

    if (listen(serverSocket, 20) < 0) {
        std::cerr
            << "Failed to listen: "
            << std::strerror(errno)
            << '\n';

        close(serverSocket);
        return;
    }

    threadPool_.start();

    std::cout
        << "Concurrent scanner server listening on port "
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
                &clientLength
            );

        if (clientSocket < 0) {
            std::cerr
                << "Failed to accept client: "
                << std::strerror(errno)
                << '\n';

            continue;
        }

        std::cout
            << "Client accepted. Socket: "
            << clientSocket
            << '\n';

        threadPool_.submit(clientSocket);
    }

    threadPool_.stop();

    close(serverSocket);
}
