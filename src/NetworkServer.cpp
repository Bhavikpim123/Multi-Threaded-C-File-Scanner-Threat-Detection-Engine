#include "NetworkServer.hpp"

#include <cerrno>
#include <csignal>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace {

volatile std::sig_atomic_t
shutdownRequested = 0;

void handleSignal(int signal) {

    if (signal == SIGINT ||
        signal == SIGTERM) {

        shutdownRequested = 1;
    }
}

}

NetworkServer::NetworkServer(
    std::uint16_t port,
    const DetectionStrategy& detectionStrategy,
    std::size_t workerCount,
    std::size_t queueSize)
    : port_(port),
      threadPool_(
          workerCount,
          queueSize,
          detectionStrategy) {
}

void NetworkServer::start() {

    struct sigaction signalAction{};

    signalAction.sa_handler =
        handleSignal;

    sigemptyset(
        &signalAction.sa_mask);

    signalAction.sa_flags = 0;

    sigaction(
        SIGINT,
        &signalAction,
        nullptr);

    sigaction(
        SIGTERM,
        &signalAction,
        nullptr);

    const int serverSocket =
        socket(
            AF_INET,
            SOCK_STREAM,
            0);

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

    serverAddress.sin_family =
        AF_INET;

    serverAddress.sin_addr.s_addr =
        INADDR_ANY;

    serverAddress.sin_port =
        htons(port_);

    if (bind(
            serverSocket,
            reinterpret_cast<sockaddr*>(
                &serverAddress),
            sizeof(serverAddress)) < 0) {

        std::cerr
            << "Failed to bind socket: "
            << std::strerror(errno)
            << '\n';

        close(serverSocket);

        return;
    }

    if (listen(
            serverSocket,
            20) < 0) {

        std::cerr
            << "Failed to listen: "
            << std::strerror(errno)
            << '\n';

        close(serverSocket);

        return;
    }

    threadPool_.start();

    std::cout
        << "Queue capacity: "
        << threadPool_.queueCapacity()
        << '\n';

    std::cout
        << "Concurrent scanner server listening on port "
        << port_
        << '\n';

    while (!shutdownRequested) {

        sockaddr_in clientAddress{};

        socklen_t clientLength =
            sizeof(clientAddress);

        const int clientSocket =
            accept(
                serverSocket,
                reinterpret_cast<sockaddr*>(
                    &clientAddress),
                &clientLength);

        if (clientSocket < 0) {

            if (errno == EINTR &&
                shutdownRequested) {

                break;
            }

            if (errno == EINTR) {
                continue;
            }

            std::cerr
                << "Failed to accept client: "
                << std::strerror(errno)
                << '\n';

            continue;
        }

        if (shutdownRequested) {
            close(clientSocket);
            break;
        }

        threadPool_.submit(
            clientSocket);
    }

    close(serverSocket);

    std::cout
        << "\nShutdown requested.\n";

    threadPool_.stop();

    const auto& metrics =
        threadPool_.metrics();

    std::cout
        << "\n========== Server Metrics ==========\n"
        << "Requests accepted:  "
        << metrics.requestsAccepted()
        << '\n'
        << "Requests rejected:  "
        << metrics.requestsRejected()
        << '\n'
        << "Requests completed: "
        << metrics.requestsCompleted()
        << '\n'
        << "Requests failed:    "
        << metrics.requestsFailed()
        << '\n'
        << "Active workers:     "
        << metrics.activeWorkers()
        << '\n'
        << "====================================\n";
}
