#pragma once

#include <rtl-sdr.h>
#include <iostream>
#include <complex>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <functional>

#define SIZE_GAINS 15
#define SIZE_BUFFER 128 // 8192

struct Coords
{
    double x = 0;
    double y = 0;
};

class CSDRBlunt
{
private:
    std::mutex mMutex;
    std::queue<Coords*> mQueue;
    uint8_t mBuffer[SIZE_BUFFER] = { 0 };
    int mProcess;

    std::thread* mThread;

    rtlsdr_dev_t* mDev;

    int mGains[50] = { 0 };

    void callBack(unsigned char *buf, uint32_t len, void* ctx);

    std::function<void(const std::vector<Coords>& data)> mFunction;

    void read();

public:
    CSDRBlunt();
    ~CSDRBlunt();

    int open(const int& device_index = 0);
    int close();

    int setCenterFrequency(const uint32_t& frequency);

    int setSampleRate(const uint32_t& rate);

    int setGain(const int& gain);

    int setBandwidth(const uint32_t& bw);

    int setAutoGain();

    int resetBuffer();

    bool readSamples(std::vector<std::complex<float>>& buffer, uint32_t samples_to_read);

    void setFunction(const std::function<void(const std::vector<Coords>& data)>& function);

    int recv(Coords* coords);
};
