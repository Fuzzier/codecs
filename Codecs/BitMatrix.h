#pragma once
#include <vector>

class BitMatrix
{
public:
    BitMatrix(void);
    BitMatrix(size_t d1, size_t d2);
    BitMatrix(const BitMatrix& rhs);
    BitMatrix(BitMatrix&& rhs);

    ~BitMatrix(void);

    BitMatrix& operator=(const BitMatrix& rhs);
    BitMatrix& operator=(BitMatrix&& rhs);

    bool operator==(const BitMatrix& rhs) const;
    bool operator!=(const BitMatrix& rhs) const;

    /*
     * Get the size of dimension $d$.
     * @param [in] d   d is 0 or 1.
     */
    size_t GetSize(size_t d) const;

    static BitMatrix FromString(const std::string& matrix);
    std::string ToString(void) const;

    /**
     * @param [in] i1   0 <= i1 <= d1_ - 1
     * @param [in] i2   0 <= i2 <= d2_ - 1
     */
    const bool& Get(size_t i1, size_t i2) const;

    /**
     * @param [in] i1   0 <= i1 <= d1_ - 1
     * @param [in] i2   0 <= i2 <= d2_ - 1
     */
    bool& Get(size_t i1, size_t i2);

    /**
     * Multiply a column vector.
     */
    std::vector<bool> Multiply(const std::vector<bool>& vec) const;

private:
    void Clear(void);

public:
    static void Test(void);

private:
    size_t d1_;     ///< number of rows.
    size_t d2_;     ///< number of columns.
    bool* data_;    ///< values are stored column-size.
};
