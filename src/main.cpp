#include "FileScanner.hpp"
#include "TaskQueue.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "C++ File Scanner Engine\n\n";

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <directory>\n";
        return 1;
    }

    FileScanner scanner;

    const auto tasks = scanner.discoverFiles(argv[1]);

    TaskQueue taskQueue;

    for (const auto& task : tasks) {
        taskQueue.push(task);
    }

    std::cout << "Created " << tasks.size()
              << " scan tasks.\n\n";

    while (!taskQueue.empty()) {
        const ScanTask task = taskQueue.pop();

        std::cout << "Queued task:\n";
        std::cout << "  File: " << task.filePath << '\n';
        std::cout << "  Size: " << task.fileSize << " bytes\n\n";
    }

    return 0;
}
