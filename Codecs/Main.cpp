#include <iostream>
#include "DataIo.h"
#include "PolynomialDivider.h"
#include "BitMatrix.h"
#include "HammingCodecs.h"
#include "UiEngine.h"

void Test(void);

int main(void)
{
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
