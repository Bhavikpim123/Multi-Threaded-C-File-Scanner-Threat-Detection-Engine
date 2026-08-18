#include "NetworkServer.hpp"
#include "SignatureDatabase.hpp"
#include "SignatureDetectionStrategy.hpp"

#include <cstdint>

int main() {
    constexpr std::uint16_t port = 9090;
    constexpr std::size_t workerCount = 4;

    SignatureDatabase database(
        "data/signatures.txt"
    );

    SignatureDetectionStrategy strategy(
        database
    );

    NetworkServer server(
        port,
        strategy,
        workerCount
    );

    server.start();

    return 0;
}
