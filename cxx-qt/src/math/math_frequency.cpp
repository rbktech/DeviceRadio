#include "math_frequency.h"

#include <cmath>

#include "rtlsdr.h"

void CMathFrequency::Imaginary(const uint8_t* data, const int& size)
{
    const int N = SIZE_RTL_SDR_BUFFER;
    uint8_t REX[N] = { 0 };
    uint8_t IMX[N] = { 0 };

    memcpy(REX, data, size);
    memcpy(REX, data, size);

    for(int K = N / 2 + 1; K < N - 1; K++) {
        REX[K] = REX[N - K];
        IMX[K] = -IMX[N - K];
    }
}

void CMathFrequency::ComplexDTF(const uint8_t* data, const int& size)
{
    const int N = SIZE_RTL_SDR_BUFFER;
    double REX[N] = { 0 };
    double IMX[N] = { 0 };

    double XR[N] = { 0 };
    double XI[N] = { 0 };

    double SR = 0.0f;
    double SI = 0.0f;

    for(int K = 0, I = 0; K < N - 1 && I < N - 1; K++, I++) {

        SR = std::cos(2 * M_PI * K * I / N);
        SI = -std::sin(2 * M_PI * K * I / N);

        REX[K] = REX[K] + XR[I] * SR - XI[I] * SI;
        IMX[K] = IMX[K] + XR[I] * SI + XI[I] * SR;
    }
}