#include <iostream>
#include "FileScanner.hpp"

int main(int argc, char* argv[]) {
    std::cout << "C++ File Scanner Engine" << std::endl;

    FileScanner scanner;
    std::string scanPath = (argc > 1) ? argv[1] : ".";
    
    std::cout << "Scanning directory: " << scanPath << std::endl;
    auto files = scanner.discoverFiles(scanPath);
    
    std::cout << "Discovered " << files.size() << " files." << std::endl;
    for (size_t i = 0; i < files.size() && i < 5; ++i) {
        std::cout << " - " << files[i] << std::endl;
    }
    if (files.size() > 5) {
        std::cout << " - ... and " << (files.size() - 5) << " more." << std::endl;
    }

    return 0;
}
