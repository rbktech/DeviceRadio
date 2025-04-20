#pragma once

#include <cstdint>

class CMathFrequency
{
public:
    CMathFrequency() = default;
    ~CMathFrequency() = default;

    static void Imaginary(const uint8_t* data, const int& size);

    /**
     * @brief DFT - Discrete Fourier Transform
     * @brief Комплексное Дискретное Преобразование Фурье
     * @param data
     * @param size
     */
    static void ComplexDTF(const uint8_t* data, const int& size);
};