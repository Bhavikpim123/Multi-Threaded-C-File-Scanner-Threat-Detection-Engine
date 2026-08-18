#include "FileScanner.hpp"
#include "ResultCollector.hpp"
#include "SignatureDatabase.hpp"
#include "SignatureDetectionStrategy.hpp"
#include "ThreadPool.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <workers> <directory>\n";
        return 1;
    }

    const int workerCount = std::stoi(argv[1]);
    const std::string targetDir = argv[2];

    FileScanner scanner;

    auto discoveryStart = std::chrono::steady_clock::now();
    const auto tasks = scanner.discoverFiles(targetDir);
    auto discoveryEnd = std::chrono::steady_clock::now();
    
    auto discoveryTime = std::chrono::duration_cast<std::chrono::microseconds>(discoveryEnd - discoveryStart);

    SignatureDatabase signatureDatabase("data/signatures.txt");
    SignatureDetectionStrategy detectionStrategy(signatureDatabase);
    ResultCollector resultCollector;

    ThreadPool pool(workerCount, detectionStrategy, resultCollector);

    auto scanStart = std::chrono::steady_clock::now();

    pool.start();
    for (const auto& task : tasks) {
        pool.submit(task);
    }
    pool.stop();

    auto scanEnd = std::chrono::steady_clock::now();

    const auto totalScanTime = std::chrono::duration_cast<std::chrono::microseconds>(scanEnd - scanStart);

    const auto totalBytes = resultCollector.totalBytes();
    const double elapsedSeconds = totalScanTime.count() / 1'000'000.0;
    const double megabytes = static_cast<double>(totalBytes) / (1024.0 * 1024.0);
    const double throughput = elapsedSeconds > 0.0 ? megabytes / elapsedSeconds : 0.0;
    const double filesPerSecond = elapsedSeconds > 0.0 ? tasks.size() / elapsedSeconds : 0.0;

    // Output formatted for scripts to parse easily
    std::cout << workerCount << ","
              << tasks.size() << ","
              << totalBytes << ","
              << discoveryTime.count() << ","
              << totalScanTime.count() << ","
              << throughput << ","
              << filesPerSecond << ","
              << resultCollector.averageHashTime().count() << ","
              << resultCollector.averageDetectionTime().count() << "\n";

    return 0;
}
