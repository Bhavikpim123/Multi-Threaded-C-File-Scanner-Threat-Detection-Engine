#include <iostream>
#include "FileScanner.hpp"

int main(int argc, char* argv[]) {
    std::cout << "C++ File Scanner Engine\n\n";

    FileScanner scanner;
    std::string scanPath = (argc > 1) ? argv[1] : ".";
    
    std::cout << "Scanning directory: " << scanPath << "\n\n";
    auto tasks = scanner.discoverFiles(scanPath);
    
    std::cout << "Created " << tasks.size() << " scan tasks.\n\n";
    for (size_t i = 0; i < tasks.size(); ++i) {
        std::cout << "Task " << (i + 1) << "\n";
        std::cout << "  File: " << tasks[i].filePath.string() << "\n";
        std::cout << "  Size: " << tasks[i].fileSize << " bytes\n\n";
    }

    return 0;
}
