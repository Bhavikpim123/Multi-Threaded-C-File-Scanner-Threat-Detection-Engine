#include "NetworkServer.hpp"
#include "SignatureDatabase.hpp"
#include "SignatureDetectionStrategy.hpp"

#include <cstdint>

int main() {

    constexpr std::uint16_t port = 9090;

    constexpr std::size_t workerCount = 4;

    constexpr std::size_t queueSize = 32;

    SignatureDatabase database(
        "data/signatures.txt");

    SignatureDetectionStrategy strategy(
        database);

    NetworkServer server(
        port,
        strategy,
        workerCount,
        queueSize);

    server.start();

    return 0;
}
