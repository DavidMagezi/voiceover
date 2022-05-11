//Copyright (c) David A. Magezi 2021
//inspired by nachtimwald.com/2017/08/18/wrapping-c-objects-in-c

#ifndef _VOICE_OVER_H
#define _VOICE_OVER_H

#include "../audio/device_duplex.h"
#include "../audio/device_writer.h"

#include <string>

namespace audio{
class VoiceOver{
public:
    VoiceOver();
    ~VoiceOver();

    void AbortTrial();
    void ClearLog();
    void PrepareDuplexTrial(bool is_veridical);
    void PreparePlaybackTrial();
    void ProbeDevices();
    void SetFirstOutputChannel(int first_channel);
    void SetFrameRate_kHz(double frame_rate_kHz);
    void SetFramesPerBuffer(unsigned int frames_per_buffer);
    void SetInputChannel(int channel_number);
    void SetInputDevice(int device_number);
    void SetLogLevel(int log_level);
    void SetOutputDevice(int device_number);
    void SetPlaybackFileName(std::string playback_file_name);
    void SetRecordedFileName(std::string recorded_file_name);
    void SetRmsThreshold(double rms_threshold = 0.5);
    void SetStereoStatus(bool is_stereo);
    void SetTrialDurationMilliseconds(double trial_duration_milliseconds);
    void RunTrial(bool is_veridical = false);
    void RunDuplexTrial();
    void RunPlaybackTrial();
    void TestDuplexSawWave();
    void TestPlaybackFile();
    void TestPlaybackSawWave();
    void TestRecord();

private:
    DeviceDuplex device_duplex_; 
    DeviceWriter device_writer_; 
    double frame_rate_kHz_;
    unsigned int frames_per_buffer_;
    int input_channel_;
    unsigned int input_device_;
    bool is_output_stereo_;
    static constexpr double KILO_ = 1000; 
    int log_level_;
    static constexpr unsigned int LOG_INSTANCE_PROBE_DEVICES_ = 1;
    static constexpr char LOG_PREFIX_[] = "log_voice_over_";
    static constexpr char LOG_SUFFIX_[] = ".txt";
    int output_channel_first_;
    unsigned int output_device_;
    std::string playback_file_name_;
    std::string recorded_file_name_;
    double rms_threshold_;
    double trial_duration_milliseconds_;

    //out of alphabetical order, due to calculations on construction
    size_t number_of_samples_per_trial;
};
} // namespace audio
#endif // _VOICE_OVER_H

