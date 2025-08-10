#pragma once

#include <cstdint>

class CMathFrequency
{
public:
    CMathFrequency() = default;
    ~CMathFrequency() = default;

    static void Imaginary(const uint8_t* data, const int& size, uint8_t* REX, uint8_t* IMX);

    /**
     * @brief DFT - Discrete Fourier Transform
     * @brief Комплексное Дискретное Преобразование Фурье
     * @param data
     * @param size
     */
    static void ComplexDTF(const uint8_t* data, const int& size);

    static void DTF(uint8_t* REX, uint8_t* IMX, const int& size);

    static float Gauss(const float& value, const float& sigma = 1.0f, const float& mu = 0.0f);

    static bool FFT(float* Rdat, float* Idat, int N, int LogN, int Ft_Flag);

    void WriteGauss(const uint8_t* data, const int& size);

    void WriteFFT(const float* data, const int& size);
};