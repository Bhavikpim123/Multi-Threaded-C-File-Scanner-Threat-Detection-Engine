#pragma once

#include "DetectionStrategy.hpp"
#include "SignatureDatabase.hpp"

class SignatureDetectionStrategy final
    : public DetectionStrategy {

public:
    explicit SignatureDetectionStrategy(
        const SignatureDatabase& database);

    ScanStatus detect(
        const std::string& fileHash) const override;

private:
    const SignatureDatabase& database_;
};
