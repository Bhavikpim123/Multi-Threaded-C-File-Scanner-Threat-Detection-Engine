#include "FileScanner.hpp"
#include "FileAnalyzer.hpp"

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

    FileAnalyzer analyzer;

    for (const auto& task : tasks) {
        const ScanResult result = analyzer.analyze(task);

        std::cout << "File: "
                  << result.filePath << '\n';

        std::cout << "  Size: "
                  << result.fileSize
                  << " bytes\n";

        std::cout << "  Extension: "
                  << result.extension << '\n';

        std::cout << "  Readable: "
                  << (result.readable ? "yes" : "no")
                  << '\n';

        std::cout << "  Scan time: "
                  << result.scanDuration.count()
                  << " us\n\n";
    }

    return 0;
}
