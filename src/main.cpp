#include "FileScanner.hpp"
#include "ResultCollector.hpp"
#include "SignatureDatabase.hpp"
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

    SignatureDatabase signatureDatabase(
        "data/signatures.txt"
    );

    ResultCollector resultCollector;

    ThreadPool pool(
        4,
        signatureDatabase,
        resultCollector
    );

    pool.start();

    for (const auto& task : tasks) {
        pool.submit(task);
    }

    pool.stop();

    const auto results = resultCollector.results();

    std::size_t safeCount = 0;
    std::size_t suspiciousCount = 0;
    std::size_t errorCount = 0;

    for (const auto& result : results) {
        switch (result.status) {
            case ScanStatus::Safe:
                ++safeCount;
                break;

            case ScanStatus::Suspicious:
                ++suspiciousCount;
                break;

            case ScanStatus::Error:
                ++errorCount;
                break;
        }
    }

    std::cout << "\n========== Scan Report ==========\n";
    std::cout << "Files scanned: " << results.size() << '\n';
    std::cout << "Safe:          " << safeCount << '\n';
    std::cout << "Suspicious:    " << suspiciousCount << '\n';
    std::cout << "Errors:        " << errorCount << '\n';
    std::cout << "=================================\n";

    return 0;
}
