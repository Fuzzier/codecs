#include <iostream>
#include "DataIo.h"
#include "PolynomialDivider.h"
#include "BitMatrix.h"
#include "HammingCodecs.h"
#include "UiEngine.h"

void Test(void);

int main(void)
{
    std::cout << "Codecs v1.1 (2017.05.11)" << std::endl;
    std::cout << "(c) University of Electronic Science and Technology of China (UESTC)." << std::endl;
    std::cout << "All right reserved." << std::endl;
    Test();
//    return 0;
    UiEngine ui;
    ui.Loop();
    return 0;
}

void Test(void)
{
    DataIo::Test();
    PolynomialDivider::Test();
    BitMatrix::Test();
    HammingCodecs::Test();
}
