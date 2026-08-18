#include "NetworkClient.hpp"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr
            << "Usage: "
            << argv[0]
            << " <file>\n";

        return 1;
    }

    NetworkClient client(
        "127.0.0.1",
        9090
    );

    return client.scanFile(argv[1])
        ? 0
        : 1;
}
