//Copyright (c) David A. Magezi 2021

#ifndef DEVICE_H
#define DEVICE_H

#include "RtAudio.h"

namespace audio{
class Device{ 
public :
    Device();   
    ~Device();

    virtual int Probe(bool is_logging = false) = 0;
    void SetFrameRate_kHz(double frame_rate_kHz);
    void SetFramesPerBuffer(unsigned int frames_per_buffer);
    unsigned int GetDefaultInputDevice();
    unsigned int GetDefaultOutputDevice();

protected:
    struct rt{
        static constexpr int Continue = 0;
        static constexpr int StopAndDrain = 1;
        static constexpr int Abort = 2;
    };

    static constexpr unsigned int desired_frame_rate_kHz = 48;
    static constexpr unsigned int desired_frames_per_buffer = 512;//on windows, need to manually set ASIO to this value?

    RtAudio device_;

    double frame_rate_kHz_;
    unsigned int frames_per_buffer_;
    static constexpr unsigned int KILO_ = 1000;
    std::string newline_;
};
} // namespace audio
#endif // DEVICE_H

