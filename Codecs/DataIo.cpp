#include "DataIo.h"
#include <iostream>
#include <algorithm>
#include <cassert>

std::vector<bool> DataIo::FromString(const std::string& buffer)
{
    std::vector<bool> data;
    size_t len = buffer.length();
    for (size_t i = 0; i < len; ++i)
    {
        switch (buffer[i])
        {
        case '0':
            data.push_back(false);
            break;
        case '1':
            data.push_back(true);
            break;
        }
    }
    return data;
}

bool DataIo::IsEqual(const std::vector<bool>& lhs, const std::vector<bool>& rhs)
{
    bool result = false;
    auto lf = lhs.begin();
    auto ll = lhs.end();
    auto rf = rhs.begin();
    auto rl = rhs.end();
    // Ignore preceding zeros.
    while (lf != ll && !*lf)
    {
        ++lf;
    }
    while (rf != rl && !*rf)
    {
        ++rf;
    }
    if (ll - lf == rl - rf)
    {
        result = std::equal(lf, ll, rf);
    }
    return result;
}

bool DataIo::IsZero(const std::vector<bool> &lhs)
{
    bool result = true;
    for (auto it = lhs.begin(); result && it != lhs.end(); ++it)
    {
        result = !*it;
    }
    return result;
}

std::string DataIo::ToString(const std::vector<bool>& data)
{
    std::string buffer;
    size_t n = data.size();
    if (n == 0)
    {
        buffer.push_back('0');
    }
    else
    {
        for (size_t i = 0; i < data.size(); ++i, --n)
        {
            if (i != 0 && n % 4 == 0)
            {
                buffer.push_back(' ');
            }
            buffer.push_back(data[i] ? '1' : '0');
        }
    }
    return buffer;
}

void DataIo::Test(void)
{
    // FromString
    std::string buffer("0 1 1100 10101010");
    std::vector<bool> result;
    result.push_back(false);
    result.push_back(true);
    result.push_back(true);
    result.push_back(true);
    result.push_back(false);
    result.push_back(false);
    result.push_back(true);
    result.push_back(false);
    result.push_back(true);
    result.push_back(false);
    result.push_back(true);
    result.push_back(false);
    result.push_back(true);
    result.push_back(false);

    std::vector<bool> data = FromString(buffer);
    for (size_t i = 0; i < data.size(); ++i)
    {
        assert(data[i] == result[i]);
    }

    // IsEqual
    std::vector<bool> lhs = FromString("001010");
    std::vector<bool> rhs = FromString("00001010");
    assert(IsEqual(lhs, rhs));
}
