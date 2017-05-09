#include "HammingCodecs.h"
#include "DataIo.h"
#include <cassert>

HammingCodecs::HammingCodecs(size_t numMessageBits) :
    numMessageBits_(numMessageBits)
{
    assert(numMessageBits > 0);
    CalculateNumberOfRedundantBits();
    CalculateEncoder();
    CalculateChecker();
    CalculateDecoder();
}

void HammingCodecs::CalculateNumberOfRedundantBits(void)
{
    numRedundantBits_ = 2;
    size_t bound = 4;
    while (bound < numMessageBits_ + numRedundantBits_ + 1)
    {
        ++numRedundantBits_;
        bound <<= 1;
    }
}

void HammingCodecs::CalculateEncoder(void)
{
    size_t numCodeBits = GetNumberOfCodeBits();
    encoder_ = BitMatrix(numCodeBits, numMessageBits_);
    // The position of redundant bits.
    // Redundant bits are placed at $2^0 - 1, 2^1 - 1, 2^2 - 1, ...$.
    // The position of message bits.
    // The 'm'-th message bit is placed at the 'mbPos[m]'-th code bit.
    std::vector<size_t> mbPos(numMessageBits_, 0);
    size_t p = 1;	// 2^0
    size_t c = 0;
    size_t m = 0;
    while (c < numCodeBits)
    {
        // If the 'c'-th code bit is not a redundant bit,
        // i.e. if (c + 1) != 2^r.
        if (c + 1 != p)
        {
            mbPos[m] = c;
            ++m;
        }
        else
        {
            p <<= 1; // 2^r
        }
        ++c;
    }
    // Build the generator matrix.
    p = 1;
    c = 0;
    m = 0;
    while (c < numCodeBits)
    {
        // If the 'c'-th code bit is a redundant bit.
        if (c + 1 == p)
        {
            // The 'c'-th code bit is a linear combination of message bits.
            // Set the coefficients of the corresponding message bits.
            for (size_t m = 0; m < numMessageBits_; ++m)
            {
                if ((mbPos[m] + 1) & p)
                {
                    encoder_.Get(c, m) = true;
                }
            }
            // Prepare the next redundant bit position to check.
            p <<= 1;
        }
        // If the 'c'-th code bit is not a redundant bit,
        // it's one of the message bits.
        else
        {
            encoder_.Get(c, m) = true;
            ++m;
        }
        ++c;
    }
}

void HammingCodecs::CalculateChecker(void)
{
    size_t numCodeBits = GetNumberOfCodeBits();
    checker_ = BitMatrix(numRedundantBits_, numCodeBits);
    // The position of redundant bits.
    // Redundant bits are placed at $2^0 - 1, 2^1 - 1, 2^2 - 1, ...$.
    // The position of message bits.
    // The 'm'-th message bit is placed at the 'mbPos[m]'-th code bit.
    std::vector<size_t> mbPos(numMessageBits_, 0);
    size_t r = 0;	// the 'r'-th redundant bit
    size_t p = 1;	// 2^r
    while (r < numRedundantBits_)
    {
        // The 'r'-th redundant bit is a combination of code bits.
        for (size_t c = 0; c < numCodeBits; ++c)
        {
            if ((c + 1) & p)
            {
                checker_.Get(r, c) = true;
            }
        }
        ++r;
        p <<= 1; // 2^r
    }
}

void HammingCodecs::CalculateDecoder(void)
{
    size_t numCodeBits = GetNumberOfCodeBits();
    decoder_ = BitMatrix(numMessageBits_, numCodeBits);
    size_t p = 1;	// 2^0
    size_t c = 0;
    size_t m = 0;
    while (c < numCodeBits)
    {
        // If the 'c'-th code bit is not a redundant bit,
        // i.e. if (c + 1) != 2^r.
        if (c + 1 != p)
        {
            decoder_.Get(m, c) = true;
            ++m;
        }
        else
        {
            p <<= 1; // 2^r
        }
        ++c;
    }
}

size_t HammingCodecs::GetNumberOfMessageBits(void) const
{
    return numMessageBits_;
}

size_t HammingCodecs::GetNumberOfRedundantBits(void) const
{
    return numRedundantBits_;
}

size_t HammingCodecs::GetNumberOfCodeBits(void) const
{
    return numMessageBits_ + numRedundantBits_;
}

const BitMatrix& HammingCodecs::GetEncoderMatrix(void) const
{
    return encoder_;
}

const BitMatrix& HammingCodecs::GetCheckerMatrix(void) const
{
    return checker_;
}

const BitMatrix& HammingCodecs::GetDecoderMatrix(void) const
{
    return decoder_;
}

std::vector<bool> HammingCodecs::Encode(const std::vector<bool>& message) const
{
    assert(message.size() == GetNumberOfMessageBits());
    return encoder_.Multiply(message);
}

size_t HammingCodecs::CheckError(const std::vector<bool>& code) const
{
    assert(code.size() == GetNumberOfCodeBits());
    std::vector<bool> error = checker_.Multiply(code);
    size_t errorPosition = 0;
    size_t p = 1;
    for (auto it = error.begin(); it != error.end(); ++ it)
    {
        if (*it)
        {
            errorPosition |= p;
        }
        p <<= 1;
    }
    return errorPosition;
}

std::vector<bool> HammingCodecs::Decode(const std::vector<bool>& code) const
{
    assert(code.size() == GetNumberOfCodeBits());
    std::vector<bool> result = decoder_.Multiply(code);
    // Correct error.
    size_t e = CheckError(code);    // the index of error bit (1-based) in the code bits
    if (e)
    {
        size_t p = 1;	// 2^0
        size_t c = 0;
        size_t m = 0;
        while (c < e)
        {
            if ((c + 1) == p)
            {
                p <<= 1;    // the position of the next redundant bit.
            }
            else
            {
                ++m;
            }
            ++c;
        }
        // If the error bit is not a redundant bit.
        if (e != (p >> 1))
        {
            result[m] = !result[m];
        }
    }
    return result;
}

#include <string>
#include <iostream>
void HammingCodecs::Test(void)
{
    // 
    HammingCodecs hc(8);
    BitMatrix e8 = BitMatrix::FromString(
        "[ 1   1   0   1   1   0   1   0 ;"
        "  1   0   1   1   0   1   1   0 ;"
        "  1   0   0   0   0   0   0   0 ;"
        "  0   1   1   1   0   0   0   1 ;"
        "  0   1   0   0   0   0   0   0 ;"
        "  0   0   1   0   0   0   0   0 ;"
        "  0   0   0   1   0   0   0   0 ;"
        "  0   0   0   0   1   1   1   1 ;"
        "  0   0   0   0   1   0   0   0 ;"
        "  0   0   0   0   0   1   0   0 ;"
        "  0   0   0   0   0   0   1   0 ;"
        "  0   0   0   0   0   0   0   1 ]");
    assert(e8 == hc.GetEncoderMatrix());

    BitMatrix c8 = BitMatrix::FromString(
        "[ 1   0   1   0   1   0   1   0   1   0   1   0 ;"
        "  0   1   1   0   0   1   1   0   0   1   1   0 ;"
        "  0   0   0   1   1   1   1   0   0   0   0   1 ;"
        "  0   0   0   0   0   0   0   1   1   1   1   1 ]");
    assert(c8 == hc.GetCheckerMatrix());

    BitMatrix d8 = BitMatrix::FromString(
        "[ 0   0   1   0   0   0   0   0   0   0   0   0 ;"
        "  0   0   0   0   1   0   0   0   0   0   0   0 ;"
        "  0   0   0   0   0   1   0   0   0   0   0   0 ;"
        "  0   0   0   0   0   0   1   0   0   0   0   0 ;"
        "  0   0   0   0   0   0   0   0   1   0   0   0 ;"
        "  0   0   0   0   0   0   0   0   0   1   0   0 ;"
        "  0   0   0   0   0   0   0   0   0   0   1   0 ;"
        "  0   0   0   0   0   0   0   0   0   0   0   1 ]");
    assert(d8 == hc.GetDecoderMatrix());

    // Encode
    std::vector<bool> msg = DataIo::FromString("1010 1010");
    std::vector<bool> code = DataIo::FromString("1111 0100 1010");
    assert(code == hc.Encode(msg));

    // CheckError
    code = DataIo::FromString("1111 0100 1010");
    size_t err = hc.CheckError(code);
    assert(err == 0);

    code = DataIo::FromString("1111 0101 1010");
    err = hc.CheckError(code);
    assert(err == 8);

    // Decode
    msg = DataIo::FromString("1010 1010");
    code = DataIo::FromString("1111 0100 1010");
    assert(msg == hc.Decode(code));

    msg = DataIo::FromString("1010 1010");
    code = DataIo::FromString("1111 0101 1010");
    assert(msg == hc.Decode(code));
}
