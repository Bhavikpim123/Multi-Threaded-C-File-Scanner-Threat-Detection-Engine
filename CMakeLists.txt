cmake_minimum_required(VERSION 3.20)

project(cpp_file_scanner
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenSSL REQUIRED)

add_executable(file_scanner
    src/main.cpp
    src/FileScanner.cpp
    src/TaskQueue.cpp
    src/ThreadPool.cpp
    src/FileAnalyzer.cpp
    src/FileHasher.cpp
    src/SignatureDatabase.cpp
    src/SignatureDetectionStrategy.cpp
    src/ResultCollector.cpp
)

target_include_directories(file_scanner PRIVATE include)

target_link_libraries(file_scanner PRIVATE OpenSSL::Crypto)
