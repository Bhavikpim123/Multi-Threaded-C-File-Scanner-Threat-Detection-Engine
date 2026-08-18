#include "SignatureDetectionStrategy.hpp"

SignatureDetectionStrategy::SignatureDetectionStrategy(
    const SignatureDatabase& database)
    : database_(database) {
}

ScanStatus SignatureDetectionStrategy::detect(
    const std::string& fileHash) const {

    if (fileHash.empty()) {
        return ScanStatus::Error;
    }

    if (database_.contains(fileHash)) {
        return ScanStatus::Suspicious;
    }

    return ScanStatus::Safe;
}
