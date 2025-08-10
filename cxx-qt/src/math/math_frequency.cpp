#include "math_frequency.h"

#include <cmath>

#include "rtlsdr.h"

void CMathFrequency::Imaginary(const uint8_t* data, const int& size, uint8_t* REX, uint8_t* IMX)
{
    // const int N = SIZE_RTL_SDR_BUFFER;
    // uint8_t REX[N] = { 0 };
    // uint8_t IMX[N] = { 0 };
    const int N = size;

    memcpy(REX, data, size);
    memcpy(IMX, data, size);

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

float CMathFrequency::Gauss(const float& value, const float& sigma, const float& mu)
{
    if(value < -1.0 || 1.0 < value)
        return value;

    float g = std::exp(- std::pow(value - mu, 2.f) / 2.f * std::pow(sigma, 2.f)) / (std::sqrt(2.f * float(M_PI)) * sigma);
    return g;
}

void CMathFrequency::DTF(uint8_t* REX, uint8_t* IMX, const int& size)
{
    const int N = size;
    // int REX[N];
    // int IMX[N];

    int TR = 0;
    int TI = 0;

    int K = 0;

    int LE = 0;
    int LE2 = 0;

    int UR = 0;
    int UI = 0;

    double SR = 0.0;
    double SI = 0.0;

    int JM1 = 0;
    int IP = 0;

    M_PI;
    int NM1 = N - 1;
    int ND2 = N >> 1;

    int M = static_cast<int>(std::log(N)/std::log(2));
    int J = ND2;

    for(int i = 1; i < N - 2; i++) {
        if(i >= J) {
            goto S1190;
        }
        TR = REX[J];
        TI = IMX[J];

        REX[J] = REX[i];
        IMX[J] = IMX[i];

        REX[i] = TR;
        IMX[i] = TI;

        S1190:
        {
            K = ND2;

            S1200:
            {
                if(K > J)
                    goto S1240;

                J = J - K;
                K = K >> 1;

                goto S1200;

                S1240:
                {
                    J = J + K;
                }
            }
        }
    }

    for(int L = 1; L < M; L++) {
        LE = static_cast<int>(std::pow(2, L));
        LE2 = LE >> 1;
        UR = 1;
        UI - 0;
        SR = std::cos(M_PI / LE2);
        SI = -std::sin(M_PI / LE2);

        for(J = 1; J < LE2; J++) {
            JM1 = J - 1;

            for(int I = JM1; I < NM1;) {

                IP = I + LE2;
                TR = REX[IP] * UR - IMX[IP] * UI;
                TI = REX[IP] * UI + IMX[IP] * UR;
                REX[IP] = REX[I] - TR;
                IMX[IP] = IMX[I] - TI;
                REX[I] = REX[I] + TR;
                IMX[I] = IMX[I] + TI;

                I += LE;

                TR = UR;
                UR = TR * SR - UI * SI;
                UI = TR * SI + UI * SR;
            }
        }
    }
}


#define  NUMBER_IS_2_POW_K(x)   ((!((x)&((x)-1)))&&((x)>1))  // x is pow(2, k), k=1,2, ...
#define  FT_DIRECT        -1    // Direct transform.
#define  FT_INVERSE        1    // Inverse transform.

bool CMathFrequency::FFT(float* Rdat, float* Idat, int N, int LogN, int Ft_Flag)
{
    // parameters error check:
    if((Rdat == NULL) || (Idat == NULL))                  return false;
    if((N > 16384) || (N < 1))                            return false;
    if(!NUMBER_IS_2_POW_K(N))                             return false;
    if((LogN < 2) || (LogN > 14))                         return false;
    if((Ft_Flag != FT_DIRECT) && (Ft_Flag != FT_INVERSE)) return false;

    int  i, j, n, k, io, ie, in, nn;
    float         ru, iu, rtp, itp, rtq, itq, rw, iw, sr;

    static const float Rcoef[14] = //
            {  -1.0000000000000000F,  0.0000000000000000F,  0.7071067811865475F, //
               0.9238795325112867F,  0.9807852804032304F,  0.9951847266721969F, //
               0.9987954562051724F,  0.9996988186962042F,  0.9999247018391445F, //
               0.9999811752826011F,  0.9999952938095761F,  0.9999988234517018F, //
               0.9999997058628822F,  0.9999999264657178F //
            }; //

    static const float Icoef[14] = //
            {   0.0000000000000000F, -1.0000000000000000F, -0.7071067811865474F, //
                -0.3826834323650897F, -0.1950903220161282F, -0.0980171403295606F, //
                -0.0490676743274180F, -0.0245412285229122F, -0.0122715382857199F, //
                -0.0061358846491544F, -0.0030679567629659F, -0.0015339801862847F, //
                -0.0007669903187427F, -0.0003834951875714F //
            }; //

    nn = N >> 1;
    ie = N;
    for(n=1; n<=LogN; n++)
    {
        rw = Rcoef[LogN - n];
        iw = Icoef[LogN - n];
        if(Ft_Flag == FT_INVERSE) iw = -iw;
        in = ie >> 1;
        ru = 1.0F;
        iu = 0.0F;
        for(j=0; j<in; j++)
        {
            for(i=j; i<N; i+=ie)
            {
                io       = i + in;
                rtp      = Rdat[i]  + Rdat[io];
                itp      = Idat[i]  + Idat[io];
                rtq      = Rdat[i]  - Rdat[io];
                itq      = Idat[i]  - Idat[io];
                Rdat[io] = rtq * ru - itq * iu;
                Idat[io] = itq * ru + rtq * iu;
                Rdat[i]  = rtp;
                Idat[i]  = itp;
            }

            sr = ru;
            ru = ru * rw - iu * iw;
            iu = iu * rw + sr * iw;
        }

        ie >>= 1;
    }

    for(j=i=1; i<N; i++)
    {
        if(i < j)
        {
            io       = i - 1;
            in       = j - 1;
            rtp      = Rdat[in];
            itp      = Idat[in];
            Rdat[in] = Rdat[io];
            Idat[in] = Idat[io];
            Rdat[io] = rtp;
            Idat[io] = itp;
        }

        k = nn;

        while(k < j)
        {
            j   = j - k;
            k >>= 1;
        }

        j = j + k;
    }

    if(Ft_Flag == FT_DIRECT) return true;

    rw = 1.0F / N;

    for(i=0; i<N; i++)
    {
        Rdat[i] *= rw;
        Idat[i] *= rw;
    }

    return true;
}

void CMathFrequency::WriteGauss(const uint8_t* data, const int& size)
{
    float* buffer = new float[size * 5];

    uint8_t max = 0.0f;

    for(int i = 0; i < size; i++)
        if(data[i] > max)
            max = data[i];

    float x = 0.0f;
    for(int i = 0, n = 0; i < size; i++, n = i * 5) {
        buffer[n + 0] = (x += 0.01);
        buffer[n + 1] = data[i];
        // buffer[DRAW_X] = data[i];
        // buffer[DRAW_Y] = CMathFrequency::Gauss(data[i] / max) * 100;
        buffer[n + 2] = 0.0f;
    }

    delete[] buffer;
}

#define FT_DIRECT -1 // Direct transform.

void CMathFrequency::WriteFFT(const float* data, const int& size)
{
    float Re[size];
    float Im[size];
    float p = 2.f * 3.141592653589f / size; // будет 8 отсчетов на период

    int i;
    // формируем сигнал
    for(i = 0; i < size; i++) {
        Re[i] = cos(p * i); // заполняем действительную часть сигнала
        Im[i] = 0.0;        // заполняем мнимую часть сигнала
    }

    CMathFrequency::FFT(Re, Im, size, 3, FT_DIRECT); // вычисляем прямое БПФ

    // выводим действительную и мнимую части спектра и спектр мощности
    for(i = 0; i < size; i++) {
        // fprintf(f, "%10.6f  %10.6f  %10.6f\n", Re[i], Im[i], Re[i] * Re[i] + Im[i] * Im[i]);
    }

    // ------------------------------

    float* buffer = new float[size * 5];

    float x = 0.0f;
    for(int j = 0, n = 0; j < size; j++, n = j * 5) {
        buffer[n + 0] = (x += 0.01);
        buffer[n + 1] = Re[i] * Re[i] + Im[i] * Im[i];
        buffer[n + 2] = 0.0f;
    }

    delete[] buffer;
}