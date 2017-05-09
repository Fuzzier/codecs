#pragma once
#include "BitMatrix.h"
#include <vector>

class HammingCodecs
{
public:
    HammingCodecs(size_t numMessageBits);

    size_t GetNumberOfMessageBits(void) const;
    size_t GetNumberOfRedundantBits(void) const;
    size_t GetNumberOfCodeBits(void) const;

    const BitMatrix& GetEncoderMatrix(void) const;
    const BitMatrix& GetCheckerMatrix(void) const;
    const BitMatrix& GetDecoderMatrix(void) const;

    /**
     * @param [in] message   The size of \c message must be equal to \c GetNumberOfMessageBits().
     */
    std::vector<bool> Encode(const std::vector<bool>& message) const;

    /**
     * @param [in] code   The size of \c code must be equal to \c GetNumberOfCodeBits().
     *
     * @return If there's no error, 0 is returned.
     *         Otherwise, the index (1-based) of the error bit is returned.
     */
    size_t CheckError(const std::vector<bool>& code) const;

    /**
     * @param [in] code   The size of \c code must be equal to \c GetNumberOfCodeBits().
     */
    std::vector<bool> Decode(const std::vector<bool>& code) const;

private:
    void CalculateNumberOfRedundantBits(void);
    void CalculateEncoder(void);
    void CalculateChecker(void);
    void CalculateDecoder(void);

public:
    static void Test(void);

private:
    size_t numMessageBits_;
    size_t numRedundantBits_;
    BitMatrix encoder_;
    BitMatrix checker_;
    BitMatrix decoder_;
};
