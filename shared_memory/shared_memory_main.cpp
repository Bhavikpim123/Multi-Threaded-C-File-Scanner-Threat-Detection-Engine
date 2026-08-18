#include "SharedMemoryController.hpp"
#include "SignatureDatabase.hpp"
#include "SignatureDetectionStrategy.hpp"

#include <iostream>

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr
            << "Usage: "
            << argv[0]
            << " <file>\n";

        return 1;
    }

    SignatureDatabase database(
        "data/signatures.txt");

    SignatureDetectionStrategy strategy(
        database);

    SharedMemoryController controller(
        strategy);

    return controller.scanFile(argv[1])
        ? 0
        : 1;
}
