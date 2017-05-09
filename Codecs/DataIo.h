#pragma once
#include <string>
#include <vector>

class DataIo
{
public:
    /**
     * Convert a string in big-endian order.
     * Only '1' and '0' are recognized, all other characters are ignored.
     */
    static std::vector<bool> FromString(const std::string& buffer);

    /**
     * Output as a string in big-endian order.
     * Spaces are added to make the output in 4-bit groups.
     */
    static std::string ToString(const std::vector<bool>& data);

    static bool IsEqual(const std::vector<bool>& lhs, const std::vector<bool>& rhs);

    static bool IsZero(const std::vector<bool> &lhs);

    static void Test(void);
};
