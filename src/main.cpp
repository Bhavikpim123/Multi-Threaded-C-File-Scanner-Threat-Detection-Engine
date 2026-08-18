#include "FileScanner.hpp"
#include "ThreadPool.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "C++ File Scanner Engine\n\n";

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <directory>\n";
        return 1;
    }

    FileScanner scanner;

    const auto tasks = scanner.discoverFiles(argv[1]);

    std::cout << "Discovered "
              << tasks.size()
              << " files.\n\n";

    ThreadPool pool(4);

    pool.start();

    for (const auto& task : tasks) {
        pool.submit(task);
    }

    pool.stop();

    std::cout << "\nAll scan tasks processed.\n";

    return 0;
}
