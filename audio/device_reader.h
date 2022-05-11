//Copyright (c) David A. Magezi 2021

#ifndef DEVICE_READER_H
#define DEVICE_READER_H

#include <vector>

#include "RtAudio.h"

#include "../audio/device.h"

namespace audio{
class DeviceReader : public Device{ 
public :
    DeviceReader();   
    ~DeviceReader();

    std::vector<double> GetData();
    int Probe(bool is_logging = false); //to be culled?
    int Record(size_t number_of_samples);
    void SetDevice(int device_number);
    void SetChannel(int first_channel);

private:
    int Callback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
             double streamTime, RtAudioStreamStatus status);
    static int StaticCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
             double streamTime, RtAudioStreamStatus status, void *userData );

    std::vector<double> data_;
    int device_number_;
    int frame_index_;
    RtAudio input_;
    int maximum_number_of_input_frames_;
    static constexpr int number_of_input_channels_ = 1;
    int input_channel_;
    static constexpr RtAudio::StreamParameters *output_parameters_null = nullptr;
};
} // namespace audio
#endif // DEVICE_READER_H

