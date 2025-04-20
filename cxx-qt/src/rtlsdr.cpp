#include "rtlsdr.h"

#include <iostream>

#define SIZE_MAX_QUEUE 1000

CRtlSdr::CRtlSdr()
    : mIndex(0)
    , mDevice(nullptr)
    , mThread(nullptr)
{
}

int CRtlSdr::Open()
{
    int result = 1;

    uint32_t count = rtlsdr_get_device_count();

    std::cout << "Device count: " << count << std::endl;

    if(count > 0) {

        std::cout << "Index: " << mIndex << std::endl;

        ShowDeviceParams(mIndex);

        std::cout << "Open... ";
        result = rtlsdr_open(&mDevice, mIndex);
        if(result != 0) {
            std::cout << "error: " << result << std::endl;
            return result;
        } else
            std::cout << "success" << std::endl;

        std::cout << "Reset buffer... ";
        result = rtlsdr_reset_buffer(mDevice);
        if(result != 0) {
            std::cout << "error: " << result << std::endl;
            return result;
        } else
            std::cout << "success" << std::endl;

        mThread = new std::thread(&CRtlSdr::loop, this);
        std::cout << "Thread start: " << std::endl;
    }

    return result;
}

int CRtlSdr::ShowDeviceParams(const int& index)
{
    int result = 1;
    char manufacture[256] = { 0 };
    char product[256] = { 0 };
    char serial[256] = { 0 };

    const char* device_name = rtlsdr_get_device_name(index);
    if(device_name != nullptr) {

        std::cout << "Device name: " << device_name << std::endl;
    }

    result = rtlsdr_get_device_usb_strings(index, manufacture, product, serial);
    if(result == 0) {

        std::cout << "Manufacture: " << manufacture << std::endl;
        std::cout << "Product: " << product << std::endl;
        std::cout << "Serial: " << serial << std::endl;
    }

    return result;
}

int CRtlSdr::SetCenterFrequency(const uint32_t& frequency)
{
    return rtlsdr_set_center_freq(mDevice, frequency);
}

int CRtlSdr::SetSampleRate(const uint32_t& rate)
{
    return rtlsdr_set_sample_rate(mDevice, rate);
}

int CRtlSdr::SetBandwidth(const uint32_t& bw)
{
    return rtlsdr_set_tuner_bandwidth(mDevice, bw);
}

int CRtlSdr::ResetBuffer()
{
    return rtlsdr_reset_buffer(mDevice);
}

void CRtlSdr::loop()
{
    int result = 0;
    uint8_t* buffer = nullptr;
    int nRead = 0;

    while(mDevice != nullptr) {

        if(mDevice != nullptr) {

            std::this_thread::sleep_for(std::chrono::milliseconds(8));

            std::lock_guard<std::mutex> lck(mMutex);

            if(mQueue.size() < SIZE_MAX_QUEUE) {

                buffer = new uint8_t[SIZE_RTL_SDR_BUFFER];

                std::cout << "Read... ";
                result = rtlsdr_read_sync(mDevice, buffer, SIZE_RTL_SDR_BUFFER, &nRead);
                if(result == 0 && nRead == SIZE_RTL_SDR_BUFFER) {
                    std::cout << "success: size: " << nRead << std::endl;
                    mQueue.push(buffer);
                } else {
                    delete[] buffer;
                    std::cout << "error: " << result;
                    break;
                }
            }
        }
    }
}

size_t CRtlSdr::GetSizeQueue()
{
    std::lock_guard<std::mutex> lck(mMutex);
    return mQueue.size();
}

int CRtlSdr::ReadBuffer(uint8_t* data)
{
    std::lock_guard<std::mutex> lck(mMutex);

    if(mQueue.empty() == false) {

        uint8_t* buffer = mQueue.front();

        memcpy(data, buffer, SIZE_RTL_SDR_BUFFER);

        delete[] buffer;

        mQueue.pop();

        return 0;
    }

    return 1;
}

int CRtlSdr::Close()
{
    int result = 0;

    if(mDevice != nullptr) {

        mMutex.lock();

        std::cout << "Close... ";
        result = rtlsdr_close(mDevice);
        if(result == 0) {
            std::cout << "success" << std::endl;

            mDevice = nullptr;

            mMutex.unlock();

            mThread->join();
            delete mThread;

            std::cout << "Thread stop" << std::endl;

        } else {
            std::cout << "error: " << result << std::endl;

            mMutex.unlock();
        }

    } else
        std::cout << "Device already close" << std::endl;

    return result;
}