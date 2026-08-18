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

add_executable(scanner_server
    server/server_main.cpp
    src/NetworkServer.cpp
    src/FileAnalyzer.cpp
    src/FileHasher.cpp
    src/SignatureDatabase.cpp
    src/SignatureDetectionStrategy.cpp
)

add_executable(scanner_client
    client/client_main.cpp
    src/NetworkClient.cpp
)

target_include_directories(file_scanner PRIVATE include)
target_link_libraries(file_scanner PRIVATE OpenSSL::Crypto pthread)

target_include_directories(scanner_server PRIVATE include)
target_link_libraries(scanner_server PRIVATE OpenSSL::Crypto pthread)

target_include_directories(scanner_client PRIVATE include)
target_link_libraries(scanner_client PRIVATE pthread)
