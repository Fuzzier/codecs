#include "UiEngine.h"
#include <string>
#include <iostream>
#include <cctype>
#include <conio.h>

enum ChoiceValues
{
    CV_INPUT_MESSAGE = 1,
    CV_INPUT_CRC,
    CV_CRC_ENCODE,
    CV_CRC_CHECK,
    CV_INPUT_HAMMING_CODE,
    CV_HAMMING_ENCODE,
    CV_HAMMING_DECODE,
    CV_EXIT,
    CV_COUNT,
};

void UiEngine::Loop(void)
{
    int choice = CV_EXIT;
    do
    {
        bool wait = true;
        choice = OfferChoices();
        switch (choice)
        {
        case CV_INPUT_MESSAGE:
            InputMessage();
            break;
        case CV_INPUT_CRC:
            InputCrcGenerator();
            break;
        case CV_CRC_ENCODE:
            CrcEncode();
            break;
        case CV_CRC_CHECK:
            CrcCheck();
            break;
        case CV_INPUT_HAMMING_CODE:
            InputHammingMessageBlockSize();
            break;
        case CV_HAMMING_ENCODE:
            HammingEncode();
            break;
        case CV_HAMMING_DECODE:
            HammingDecode();
            break;
        case CV_EXIT:
            std::cout << "Farewell." << std::endl;
            wait = false;
            break;
        default:
            std::cout << "Unrecognized choice." << std::endl;
            wait = false;
        }
        if (wait)
        {
            std::cout << "Press any key to continue.";
            _getch();
            std::cout << std::endl;
        }
    }
    while (choice != CV_EXIT);
}

int UiEngine::OfferChoices(void)
{
    int result = CV_COUNT;
    std::cout << "============================" << std::endl
              << "Available choices: " << std::endl
              << "1. Set current bit sequence." << std::endl
              << "2. Set CRC generator." << std::endl
              << "3. CRC encode." << std::endl
              << "4. CRC check." << std::endl
              << "5. Set Hamming code" << std::endl
              << "6. Hamming encode." << std::endl
              << "7. Hamming decode." << std::endl
              << "8. Exit." << std::endl
              << "Your choice: ";
    std::string line;
    std::getline(std::cin, line);
    auto it = line.begin();
    while (it != line.end() && std::isspace(*it))
    {
        ++it;
    }
    if (it != line.end())
    {
        if (*it >= '0' && *it < '0' + CV_COUNT)
        {
            result = *it - '0';
        }
    }
    return result;
}

void UiEngine::InputMessage(void)
{
    std::cout << "Input a bit sequence (0, 1 or space) in big-endian order:" << std::endl;
    std::string line;
    std::getline(std::cin, line);
    bitSeq_ = DataIo::FromString(line);
    ShowMessage();
}

void UiEngine::ShowMessage(void) const
{
    std::cout << "The current bit sequence (" << bitSeq_.size() <<  " bits):" << std::endl
              << DataIo::ToString(bitSeq_) << std::endl;
}

void UiEngine::InputCrcGenerator(void)
{
    std::cout << "Input CRC generator (0, 1 or space) in big-endian order:" << std::endl;
    std::string line;
    std::getline(std::cin, line);
    crcGen_ = DataIo::FromString(line);
    ShowCrcGenerator();
}

void UiEngine::ShowCrcGenerator(void) const
{
    std::cout << "The current CRC generator (" << bitSeq_.size() <<  " bits):" << std::endl
              << DataIo::ToString(crcGen_) << std::endl;
}

void UiEngine::CrcEncode(void)
{
    std::vector<bool> quotient;
    std::vector<bool> remainder;
    if (DataIo::IsZero(crcGen_))
    {
        std::cout << "Error: CRC generator has not been set!" << std::endl;
        return;
    }
    PolynomialDivider::Divide(bitSeq_, crcGen_, quotient, remainder);
    std::cout << "The quotient (" << quotient.size() << " bits):" << std::endl
              << DataIo::ToString(quotient) << std::endl;
    std::cout << "The remainder (" << remainder.size() << " bits):" << std::endl
              << DataIo::ToString(remainder) << std::endl;
    for (auto it = remainder.begin(); it != remainder.end(); ++it)
    {
        bitSeq_.push_back(*it);
    }
    ShowMessage();
}

void UiEngine::CrcCheck(void)
{
    std::vector<bool> quotient;
    std::vector<bool> remainder;
    if (DataIo::IsZero(crcGen_))
    {
        std::cout << "Error: CRC generator has not been set!" << std::endl;
        return;
    }
    if (bitSeq_.size() < crcGen_.size())
    {
        std::cout << "Error: The current bit sequence doesn't contain CRC code!" << std::endl;
        return;
    }
    PolynomialDivider::Divide(bitSeq_, crcGen_, quotient, remainder);
    std::cout << "The quotient (" << quotient.size() << " bits):" << std::endl
              << DataIo::ToString(quotient) << std::endl;
    std::cout << "The remainder (" << remainder.size() << " bits):" << std::endl
              << DataIo::ToString(remainder) << std::endl;
    if (DataIo::IsZero(remainder))
    {
        std::cout << "No error is found." << std::endl;
    }
    else
    {
        std::cout << "There is error." << std::endl;
    }
    bitSeq_.resize(bitSeq_.size() - (crcGen_.size() - 1));
    ShowMessage();
}

void UiEngine::InputHammingMessageBlockSize(void)
{
    std::cout << "Input Hamming message block size:" << std::endl;
    std::string line;
    std::getline(std::cin, line);
    size_t size = std::strtoul(line.c_str(), nullptr, 10);
    if (size == 0)
    {
        std::cout << "Error: The size cannot be 0!" << std::endl;
    }
    if (size >= 1024)
    {
        std::cout << "Error: The size is too large!" << std::endl;
    }
    hamming_.reset(new HammingCodecs(size));
    ShowHammingCodecs();
}

void UiEngine::ShowHammingCodecs(void) const
{
    const BitMatrix& g = hamming_->GetEncoderMatrix();
    std::cout << "The current Hamming generator (" << g.GetSize(0) <<  "x" << g.GetSize(1) << " matrix):" << std::endl
              << g.ToString() << std::endl;
}

void UiEngine::HammingEncode(void)
{
    if (!hamming_)
    {
        std::cout << "Error: Hamming code has not been set!" << std::endl;
        return;
    }
    if (hamming_->GetNumberOfMessageBits() != bitSeq_.size())
    {
        std::cout << "Error: The size of current bit sequence (" << bitSeq_.size() << " bits) "
                  << "mismatches the Hamming message block size (" << hamming_->GetNumberOfMessageBits() << ")!" << std::endl;
        return;
    }
    std::vector<bool> code = hamming_->Encode(bitSeq_);
    bitSeq_ = code;
    ShowMessage();
}

void UiEngine::HammingDecode(void)
{
    if (!hamming_)
    {
        std::cout << "Error: Hamming code has not been set!" << std::endl;
        return;
    }
    if (hamming_->GetNumberOfCodeBits() != bitSeq_.size())
    {
        std::cout << "Error: The size of current bit sequence (" << bitSeq_.size() << " bits) "
                  << "mismatches the Hamming code block size (" << hamming_->GetNumberOfCodeBits() << ")!" << std::endl;
        return;
    }
    size_t error = hamming_->CheckError(bitSeq_);
    if (!error)
    {
        std::cout << "No error is found." << std::endl;
    }
    else
    {
        std::cout << "An error is found at the " << error << "-th bit." << std::endl;
    }
    std::vector<bool> code = hamming_->Decode(bitSeq_);
    bitSeq_ = code;
    ShowMessage();
}
