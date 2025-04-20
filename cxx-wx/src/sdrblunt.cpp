#include <thread>
#include <mutex>
#include <cstring>
#include "sdrblunt.h"

CSDRBlunt::CSDRBlunt()
     : mDev(nullptr)
    , mFunction(nullptr)
{
    mProcess = 1;
    mThread = new std::thread(&CSDRBlunt::read, this);
}

CSDRBlunt::~CSDRBlunt()
{
    mProcess = 0;
    mThread->join();
    delete mThread;
    close();
}

int CSDRBlunt::open(const int& device_index)
{
    int result = -1;

    char manufacture[100] = { 0 };
    char product[100] = { 0 };
    char serial[100] = { 0 };

    result = rtlsdr_open(&mDev, device_index);
    if(result == 0) {
        result = rtlsdr_set_testmode(mDev, false);
        if(result == 0) {

            result = rtlsdr_get_tuner_gains(mDev, mGains);
            if(result != 0) {

                const char* name = rtlsdr_get_device_name(device_index);
                printf("%s\n", name);

                // result = rtlsdr_get_device_usb_strings(device_index, manufacture, product, serial);
                printf("Manufacture: %s\n", manufacture);
                printf("Product: %s\n", product);
                printf("Serial: %s\n", serial);

                rtlsdr_tuner type = rtlsdr_get_tuner_type(mDev);
                switch(type) {
                    case RTLSDR_TUNER_UNKNOWN:
                        printf("Type: RTLSDR_TUNER_UNKNOWN\n");
                        break;
                    case RTLSDR_TUNER_E4000:
                        printf("Type: RTLSDR_TUNER_E4000\n");
                        break;
                    case RTLSDR_TUNER_FC0012:
                        printf("Type: RTLSDR_TUNER_FC0012\n");
                        break;
                    case RTLSDR_TUNER_FC0013:
                        printf("Type: RTLSDR_TUNER_FC0013\n");
                        break;
                    case RTLSDR_TUNER_FC2580:
                        printf("Type: RTLSDR_TUNER_FC2580\n");
                        break;
                    case RTLSDR_TUNER_R820T:
                        printf("Type: RTLSDR_TUNER_R820T\n");
                        break;
                    case RTLSDR_TUNER_R828D:
                        printf("Type: RTLSDR_TUNER_R828D\n");
                        break;
                }
            }
        }
    }

    return result;
}

int CSDRBlunt::close()
{
    int result = 0;

    if(mDev != nullptr) {
        result = rtlsdr_close(mDev);
        mDev = nullptr;
    }

    return result;
}

int CSDRBlunt::setCenterFrequency(const uint32_t& frequency)
{
    return rtlsdr_set_center_freq(mDev, frequency);
}

int CSDRBlunt::setSampleRate(const uint32_t& rate)
{
    return rtlsdr_set_sample_rate(mDev, rate);
}

int CSDRBlunt::setGain(const int& index)
{
    return rtlsdr_set_tuner_gain(mDev, mGains[index]);
}

int CSDRBlunt::setAutoGain()
{
    return rtlsdr_set_tuner_gain_mode(mDev, 0);
}

int CSDRBlunt::resetBuffer()
{
    return rtlsdr_reset_buffer(mDev);
}

int CSDRBlunt::setBandwidth(const uint32_t& bw)
{
    return rtlsdr_set_tuner_bandwidth(mDev, bw);
}

void CSDRBlunt::callBack(unsigned char *buf, uint32_t len, void* ctx)
{
    std::vector<Coords> data;
    if(mFunction != nullptr)
        mFunction(data);
}

bool CSDRBlunt::readSamples(std::vector<std::complex<float>>& buffer, uint32_t samples_to_read)
{
    unsigned char* buf = new unsigned char[samples_to_read * 2];

    int n_read = rtlsdr_read_sync(mDev, buf, samples_to_read, 0);

    if (n_read <= 0) {
        delete[] buf;
        return false;
    }

    // Преобразование в комплексные числа
    buffer.resize(n_read);
    for(int i = 0; i < n_read; i++) {
        int16_t real = static_cast<int16_t>(buf[i * 2]) + (static_cast<int16_t>(buf[i * 2 + 1]) << 8);
        buffer[i] = std::complex<float>(real / 32768.0f);
    }

    delete[] buf;
    return true;
}

void CSDRBlunt::setFunction(const std::function<void(const std::vector<Coords>& data)>& function)
{
    mFunction = function;
}

void CSDRBlunt::read()
{
    int result = -1;
    int num_bytes_read = 0;

    while(mProcess == 1) {

        std::lock_guard<std::mutex> lck(mMutex);
        if(mQueue.size() < 100) {

            result = rtlsdr_reset_buffer(mDev);
            if(result == 0) {

                memset(mBuffer, 0, SIZE_BUFFER);

                result = rtlsdr_read_sync(mDev, mBuffer, SIZE_BUFFER, &num_bytes_read);
                if(result != 0 && num_bytes_read != SIZE_BUFFER) {
                    result = close();
                    if(result != 0)
                        break;
                }

                Coords* coords = new Coords[SIZE_BUFFER];

                for(int i = 0; i < SIZE_BUFFER; i++) {

                    coords[i].x = i;
                    coords[i].y = mBuffer[i];
                }

                mQueue.push(coords);
            }
        }
    }
}

int CSDRBlunt::recv(Coords* coordsOut)
{
    std::lock_guard<std::mutex> lck(mMutex);
    if(mQueue.empty() == false) {

        Coords* coordsIn = mQueue.front();

        memcpy(coordsOut, coordsIn, SIZE_BUFFER * sizeof(Coords));

        delete[] coordsIn;

        mQueue.pop();

        return 0;
    }

    return 1;
}