#pragma once
#include <vector>

class PolynomialDivider
{
public:
    /**
     * @param [in]  dividend
     * @param [in]  divisor
     * @param [out] quotient
     * @param [out] remainder
     */
    static void Divide(const std::vector<bool>& dividend, const std::vector<bool>& divisor,
                       std::vector<bool>& quotient, std::vector<bool>& remainder);

    static void Test(void);
};
