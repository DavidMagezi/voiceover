//Copyright (c) David A. Magezi 2021
//inspired by nachtimwald.com/2017/08/18/wrapping-c-objects-in-c

#ifndef _VOICE_OVER_INTERFACE_H
#define _VOICE_OVER_INTERFACE_H

typedef void* v_type;
v_type v_;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__declspec(dllexport)
void CreateVoiceOver();

__declspec(dllexport)
void DestroyVoiceOver();

__declspec(dllexport)
void AbortTrial();    

__declspec(dllexport)
void ClearLog();    

__declspec(dllexport)
void ProbeDevices();    

__declspec(dllexport)
void SetFirstOutputChannel(int first_channel);

__declspec(dllexport)
void SetFrameRate_kHz(double frame_rate_kHz);    

__declspec(dllexport)
void SetFramesPerBuffer(unsigned int frames_per_buffer);    

__declspec(dllexport)
void SetInputChannel(int channel_number);    

__declspec(dllexport)
void SetInputDevice(int device_number);    

__declspec(dllexport)
void SetLogLevel(int log_level);

__declspec(dllexport)
void SetOutputDevice(int device_number);

__declspec(dllexport)
void SetPlaybackFileName(const char playback_file_name[]);

__declspec(dllexport)
void SetRecordedFileName(const char recorded_file_name[]);

__declspec(dllexport)
void SetRmsThreshold(double rms_threshold);

__declspec(dllexport)
void SetStereoStatus(bool is_stereo);

__declspec(dllexport)
void SetTrialDurationMilliseconds(double trial_duration_milliseconds);

__declspec(dllexport)
void PrepareDuplexTrial(bool is_veridical);

__declspec(dllexport)
void PreparePlaybackTrial();

__declspec(dllexport)
void RunTrial(bool is_veridical);

__declspec(dllexport)
void RunDuplexTrial();

__declspec(dllexport)
void RunPlaybackTrial();

__declspec(dllexport)
void TestDuplexSawWave();

__declspec(dllexport)
void TestPlaybackFile();

__declspec(dllexport)
void TestPlaybackSawWave();

__declspec(dllexport)
void TestRecord();


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _VOICE_OVER_INTERFACE_H

