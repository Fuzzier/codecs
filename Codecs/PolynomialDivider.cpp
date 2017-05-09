#include "PolynomialDivider.h"
#include <cassert>
#include <algorithm>

void PolynomialDivider::Divide(const std::vector<bool>& dividend, const std::vector<bool>& divisor,
                               std::vector<bool>& quotient, std::vector<bool>& remainder)
{
    std::vector<bool> middle(dividend);
    auto af = middle.begin();
    auto al = middle.end();

    auto bf = divisor.begin();
    auto bl = divisor.end();

    while (bf != bl && !*bf)
    {
        ++bf;
    }
    // The divisor must not be zero.
    assert(bf != bl);

    quotient.clear();
    remainder.clear();

    while (al - af >= bl - bf)
    {
        quotient.push_back(*af);
        if (*af)
        {
            std::transform(bf, bl, af, af, [] (bool a, bool b) -> bool { return a^b; });
        }
        ++af;
    }
    if (quotient.empty())
    {
        quotient.push_back(false);
    }
    size_t numPrependingZeros = (bl - bf) - (al - af) - 1;
    while (numPrependingZeros > 0)
    {
        remainder.push_back(false);
        --numPrependingZeros;
    }
    while (af != al)
    {
        remainder.push_back(*af);
        ++af;
    }
}

#include "DataIo.h"
#include <iostream>
void PolynomialDivider::Test(void)
{
    std::vector<bool> dividend;
    std::vector<bool> divisor;
    std::vector<bool> quotient;
    std::vector<bool> remainder;

    divisor = DataIo::FromString("10001");
    Divide(dividend, divisor, quotient, remainder);
    assert(DataIo::IsEqual(quotient, DataIo::FromString("0")));
    assert(DataIo::IsEqual(remainder, DataIo::FromString("0000")));

    dividend = DataIo::FromString("1100 0101");
    divisor = DataIo::FromString("10001");
    Divide(dividend, divisor, quotient, remainder);
    assert(DataIo::IsEqual(quotient, DataIo::FromString("1100")));
    assert(DataIo::IsEqual(remainder, DataIo::FromString("1001")));

    dividend = DataIo::FromString("1100 0111 1010");
    divisor = DataIo::FromString("10001");
    Divide(dividend, divisor, quotient, remainder);
    assert(DataIo::IsEqual(quotient, DataIo::FromString("11001011")));
    assert(DataIo::IsEqual(remainder, DataIo::FromString("1")));
}
