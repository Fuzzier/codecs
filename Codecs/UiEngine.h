#pragma once
#include "DataIo.h"
#include "BitMatrix.h"
#include "PolynomialDivider.h"
#include "HammingCodecs.h"
#include <vector>
#include <memory>

class UiEngine
{
public:
    void Loop(void);

private:
    int OfferChoices(void);

    void InputMessage(void);
    void ShowMessage(void) const;

    void InputCrcGenerator(void);
    void ShowCrcGenerator(void) const;
    void CrcEncode(void);
    void CrcCheck(void);

    void InputHammingMessageBlockSize(void);
    void ShowHammingCodecs(void) const;
    void HammingEncode(void);
    void HammingDecode(void);

private:
    std::vector<bool> bitSeq_;
    std::vector<bool> crcGen_;
    std::unique_ptr<HammingCodecs> hamming_;
};
