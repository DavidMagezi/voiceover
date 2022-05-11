//Copyright (c) David A. Magezi 2021

#ifndef DEVICE_WRITER_H
#define DEVICE_WRITER_H

#include <vector>

#include "RtAudio.h"

#include "../audio/device.h"

namespace audio{
class DeviceWriter : public Device{ 
public :
    DeviceWriter();   
    ~DeviceWriter();

    int Callback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
             double streamTime, RtAudioStreamStatus status);
    int PlayData();
    int PlaySaw();
    int Probe(bool is_logging = false);
    static int SawCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
             double streamTime, RtAudioStreamStatus status, void *userData );
    int SetData(std::vector<double> audio_data);
    void SetDevice(int device_number);
    void SetChannel(int first_channel);
    void SetMonoToStereo(bool mono_to_stereo);
    void SetStereoStatus(bool is_stereo);
    int StopSaw();

private:
    std::vector<double> ConvertMonoToStereo(std::vector<double> mono);
    static int StaticCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
             double streamTime, RtAudioStreamStatus status, void *userData );

    int first_channel_;
    std::vector<double> data_;
    int device_number_;
    unsigned int frame_index_;
    static constexpr RtAudio::StreamParameters *input_parameters_null = nullptr;
    size_t maximum_number_of_output_samples_;
    bool mono_to_stereo_;
    unsigned int number_of_channels_;
};
} // namespace audio
#endif // DEVICE_WRITER_H

