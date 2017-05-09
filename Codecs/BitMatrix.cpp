#include "BitMatrix.h"
#include "DataIo.h"
#include <cassert>
#include <memory>
#include <sstream>

BitMatrix::BitMatrix(void) :
    d1_(0),
    d2_(0),
    data_(nullptr)
{
}

BitMatrix::BitMatrix(size_t d1, size_t d2) :
    d1_(d1),
    d2_(d2),
    data_(nullptr)
{
    data_ = new bool[d1 * d2];
    assert(data_);
    std::memset(data_, 0, d1_ * d2_);
}

BitMatrix::BitMatrix(const BitMatrix& rhs) :
    d1_(rhs.d1_),
    d2_(rhs.d2_)
{
    if (data_)
    {
        delete[] data_;
        data_ = nullptr;
    }
    data_ = new bool[d1_ * d2_];
    assert(data_);
    std::memcpy(data_, rhs.data_, d1_ * d2_);
}

BitMatrix::BitMatrix(BitMatrix&& rhs) :
    d1_(rhs.d1_),
    d2_(rhs.d2_),
    data_(rhs.data_)
{
    rhs.d1_ = 0;
    rhs.d2_ = 0;
    rhs.data_ = nullptr;
}

BitMatrix::~BitMatrix(void)
{
    if (data_)
    {
        delete[] data_;
    }
}

void BitMatrix::Clear(void)
{
    if (data_)
    {
        delete[] data_;
        data_ = nullptr;
    }
    d1_ = 0;
    d2_ = 0;
}

BitMatrix& BitMatrix::operator=(const BitMatrix& rhs)
{
    Clear();
    d1_ = rhs.d1_;
    d2_ = rhs.d2_;
    data_ = new bool[d1_ * d2_];
    assert(data_);
    std::memcpy(data_, rhs.data_, d1_ * d2_);
    return *this;
}

BitMatrix& BitMatrix::operator=(BitMatrix&& rhs)
{
    Clear();
    d1_ = rhs.d1_;
    d2_ = rhs.d2_;
    data_ = rhs.data_;
    rhs.d1_ = 0;
    rhs.d2_ = 0;
    rhs.data_ = nullptr;
    return *this;
}

bool BitMatrix::operator==(const BitMatrix& rhs) const
{
    bool result = true;
    result = (d1_ == rhs.d1_);
    if (result)
    {
        result = (d2_ == rhs.d2_);
    }
    if (result)
    {
        result = !std::memcmp(data_, rhs.data_, d1_ * d2_);
    }
    return result;
}

bool BitMatrix::operator!=(const BitMatrix& rhs) const
{
    return !(*this == rhs);
}

BitMatrix BitMatrix::FromString(const std::string& matrix)
{
    BitMatrix result;
    bool bad = false;
    do
    {
        // Find '['.
        size_t openBracket = matrix.find('[', 0);
        if (openBracket == std::string::npos)
        {
            bad = true;
            break;
        }
        size_t closeBracket = matrix.find(']', openBracket + 1);
        if (closeBracket == std::string::npos)
        {
            bad = true;
            break;
        }
        // Obtain d1 and d2.
        size_t d1 = 0;
        size_t d2 = 0;
        size_t start = openBracket + 1;
        while (start < closeBracket)
        {
            size_t end = matrix.find_first_of(";\r\n", start + 1);
            if (end == std::string::npos || end > closeBracket)
            {
                end = closeBracket;
            }
            std::vector<bool> row = DataIo::FromString(matrix.substr(start, end - start));
            if (row.size())
            {
                if (!d2)
                {
                    d2 = row.size();
                }
                else if (d2 != row.size())
                {
                    // Column size mismatch.
                    bad = true;
                    break;
                }
                ++d1;
            }
            start = end + 1;
        }
        if (bad)
        {
            break;
        }
        // Generate matrix.
        result = BitMatrix(d1, d2);
        size_t i1 = 0;
        start = openBracket + 1;
        while (start < closeBracket)
        {
            size_t end = matrix.find_first_of(";\r\n", start + 1);
            if (end == std::string::npos || end > closeBracket)
            {
                end = closeBracket;
            }
            std::vector<bool> row = DataIo::FromString(matrix.substr(start, end - start));
            if (row.size())
            {
                for (size_t i2 = 0; i2 < row.size(); ++i2)
                {
                    result.Get(i1, i2) = row[i2];
                }
                ++i1;
            }
            start = end + 1;
        }
    }
    while (false);
    return result;
}

std::string BitMatrix::ToString(void) const
{
    std::ostringstream oss;
    oss << "[";
    for (size_t i1 = 0; i1 < d1_; ++i1)
    {
        if (i1 == 0)
        {
            oss << ' ';
        }
        else
        {
            oss << ';' << std::endl << "  ";
        }
        for (size_t i2 = 0; i2 < d2_; ++i2)
        {
            oss << (Get(i1, i2) ? 1 : 0) << ' ';
        }
    }
    oss << ']';
    return oss.str();
}

size_t BitMatrix::GetSize(size_t d) const
{
    assert(d <= 1);
    return d == 0 ? d1_ : d2_;
}

const bool& BitMatrix::Get(size_t i1, size_t i2) const
{
    assert(i1 < d1_);
    assert(i2 < d2_);
    return data_[i2 * d1_ + i1];
}

bool& BitMatrix::Get(size_t i1, size_t i2)
{
    assert(i1 < d1_);
    assert(i2 < d2_);
    return data_[i2 * d1_ + i1];
}

std::vector<bool> BitMatrix::Multiply(const std::vector<bool>& vec) const
{
    assert(vec.size() == d2_);
    std::vector<bool> result(d1_, false);
    const bool* p = data_;
    for (size_t i = 0; i < d2_; ++i)
    {
        for (size_t j = 0; j < d1_; ++j)
        {
            result[j] = result[j] ^ (vec[i] & *p);
            ++p;
        }
    }
    return result;
}

#include "DataIo.h"
void BitMatrix::Test(void)
{
    // FromString
    BitMatrix m1 = FromString("[1 1 1 ;\r\n 0 0 0 \r\n 1 0 1]");
    assert(m1.Get(0,0) == true);
    assert(m1.Get(0,1) == true);
    assert(m1.Get(0,2) == true);
    assert(m1.Get(1,0) == false);
    assert(m1.Get(1,1) == false);
    assert(m1.Get(1,2) == false);
    assert(m1.Get(2,0) == true);
    assert(m1.Get(2,1) == false);
    assert(m1.Get(2,2) == true);

    // ToString
    std::string str = m1.ToString();
    assert(str == "[ 1 1 1 ;\n"
                  "  0 0 0 ;\n"
                  "  1 0 1 ]");

    // Multiply
    BitMatrix bm = FromString("[ 1 1 ; \
                                 1 0 ; \
                                 0 1 ]");

    std::vector<bool> v = DataIo::FromString("1 \
                                              0");

    std::vector<bool> u = bm.Multiply(v);
    std::vector<bool> result = DataIo::FromString("1 \
                                                   1 \
                                                   0");
    assert(DataIo::IsEqual(u, result));
}
