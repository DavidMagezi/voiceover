//Copyright (c) David A. Magezi 2021

#ifndef DEVICE_DUPLEX_H
#define DEVICE_DUPLEX_H

#include <vector>

#include "RtAudio.h"

#include "../audio/device.h"
#include "../audio/threshold.h"

namespace audio{
class DeviceDuplex : public Device{ 
public :
    DeviceDuplex();   
    ~DeviceDuplex();

    int Callback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
             double streamTime, RtAudioStreamStatus status);
    int CallbackSawWave( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
             double streamTime, RtAudioStreamStatus status);
    int CallbackVeridical( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
             double streamTime, RtAudioStreamStatus status);

    std::vector<double> GetData();
    int Probe(bool is_logging = false);
    void ResetRecordedData(size_t number_of_samples, unsigned int samples_per_rms_window);
    int SawTrial(size_t number_of_samples);

    int SetData(std::vector<double> audio_data, int number_of_channels);
    void SetFirstOutputChannel(int first_channel);
    void SetInputChannel(int channel_number);
    void SetInputDevice(int device_number);
    void SetOutputDevice(int device_number);
    void SetRmsThreshold(double rms_threshold);
    void SetVeridical(bool is_veridical);
    int Trial(size_t number_of_samples);

private:
    static int SawCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
             double streamTime, RtAudioStreamStatus status, void *userData );
    static int StaticCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
             double streamTime, RtAudioStreamStatus status, void *userData );
    static int StaticCallbackSawWave( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
             double streamTime, RtAudioStreamStatus status, void *userData );
    static int StaticCallbackVeridical( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
             double streamTime, RtAudioStreamStatus status, void *userData );

    std::vector<double> playback_data_,recorded_data_;
    double saw_wave_data_[2];

    int input_channel_;
    unsigned int input_device_;
    bool is_veridical_;
    int frame_index_;
    int maximum_number_of_input_frames_;
    size_t maximum_number_of_output_samples_;
    static constexpr int number_of_input_channels_ = 1; //assumes single microphone
    static constexpr int number_of_output_channels_ = 2; // assumes stereo speakers
    int number_of_playback_channels_;
    int output_channel_first_;
    unsigned int output_device_;
    int playback_frame_index_;
    audio::Threshold::Status status_;
    audio::Threshold threshold_;
};
} // namespace audio
#endif // DEVICE_DUPLEX_H

