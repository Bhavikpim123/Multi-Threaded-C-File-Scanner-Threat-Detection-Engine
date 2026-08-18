#include "FileHasher.hpp"

#include <openssl/evp.h>

#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

std::string FileHasher::calculateHash(
    const std::filesystem::path& filePath
) const {
    std::ifstream file(filePath, std::ios::binary);

    if (!file) {
        return {};
    }

    EVP_MD_CTX* context = EVP_MD_CTX_new();

    if (!context) {
        return {};
    }

    if (EVP_DigestInit_ex(context, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(context);
        return {};
    }

    std::vector<char> buffer(8192);

    while (file.read(buffer.data(), buffer.size()) ||
           file.gcount() > 0) {

        const std::streamsize bytesRead = file.gcount();

        if (EVP_DigestUpdate(
                context,
                buffer.data(),
                static_cast<std::size_t>(bytesRead)) != 1) {

            EVP_MD_CTX_free(context);
            return {};
        }
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digestLength = 0;

    if (EVP_DigestFinal_ex(
            context,
            digest,
            &digestLength) != 1) {

        EVP_MD_CTX_free(context);
        return {};
    }

    EVP_MD_CTX_free(context);

    std::ostringstream hash;

    hash << std::hex
         << std::setfill('0');

    for (unsigned int i = 0; i < digestLength; ++i) {
        hash << std::setw(2)
             << static_cast<unsigned int>(digest[i]);
    }

    return hash.str();
}
