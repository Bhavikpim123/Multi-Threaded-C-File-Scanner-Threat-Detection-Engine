#include "FileScanner.hpp"
#include "ResultCollector.hpp"
#include "SignatureDatabase.hpp"
#include "ThreadPool.hpp"

#include <chrono>
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

    const auto scanStart = std::chrono::steady_clock::now();

    pool.start();

    for (const auto& task : tasks) {
        pool.submit(task);
    }

    pool.stop();

    const auto scanEnd = std::chrono::steady_clock::now();

    const auto totalScanTime =
        std::chrono::duration_cast<std::chrono::microseconds>(
            scanEnd - scanStart
        );

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

    const auto totalBytes = resultCollector.totalBytes();

    const double elapsedSeconds =
        totalScanTime.count() / 1'000'000.0;

    const double megabytes =
        static_cast<double>(totalBytes) / (1024.0 * 1024.0);

    const double throughput =
        elapsedSeconds > 0.0
            ? megabytes / elapsedSeconds
            : 0.0;

    std::cout << "\n========== Performance ==========\n";

    std::cout << "Total bytes:       "
              << totalBytes
              << '\n';

    std::cout << "Total scan time:   "
              << totalScanTime.count()
              << " us\n";

    std::cout << "Average file time: "
              << (tasks.empty()
                      ? 0
                      : totalScanTime.count() /
                            static_cast<long long>(tasks.size()))
              << " us\n";

    std::cout << "Throughput:        "
              << throughput
              << " MB/s\n";

    std::cout << "=================================\n";

    return 0;
}
