#pragma once

#include <rtl-sdr.h>

#include <atomic>
#include <mutex>
#include <queue>
#include <thread>

#define SIZE_RTL_SDR_BUFFER 32768

class CRtlSdr
{
private:
    const int mIndex;
    rtlsdr_dev_t* mDevice;

    std::mutex mMutex;
    std::thread* mThread;

    std::queue<uint8_t*> mQueue;

    void loop();

public:
    CRtlSdr();
    ~CRtlSdr() = default;

    int Open();
    int Close();
    int ResetBuffer();
    int SetSampleRate(const uint32_t& rate);
    int SetCenterFrequency(const uint32_t& frequency);
    int SetBandwidth(const uint32_t& bw);
    int ShowDeviceParams(const int& index);
    int ReadBuffer(uint8_t* data);

    size_t GetSizeQueue();
};