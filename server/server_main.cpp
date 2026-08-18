#include "NetworkServer.hpp"
#include "SignatureDatabase.hpp"
#include "SignatureDetectionStrategy.hpp"

#include <cstdint>
#include <iostream>

int main() {
    constexpr std::uint16_t port = 9090;

    SignatureDatabase database(
        "data/signatures.txt"
    );

    SignatureDetectionStrategy strategy(
        database
    );

    NetworkServer server(
        port,
        strategy
    );

    server.start();

    return 0;
}
