//Copyright (c) David A. Magezi 2021

#include "voice_over_interface.h"

#include "../audio/voice_over.h"


void CreateVoiceOver(){
    v_ = reinterpret_cast<void*>(new audio::VoiceOver());
}

void DestroyVoiceOver(){
    delete reinterpret_cast<audio::VoiceOver*>(v_);
}

void AbortTrial(){
    reinterpret_cast<audio::VoiceOver*>(v_)->AbortTrial();
}

void ClearLog(){
    reinterpret_cast<audio::VoiceOver*>(v_)->ClearLog();
}

void ProbeDevices(){
    //v_->ProbeDevices();
    reinterpret_cast<audio::VoiceOver*>(v_)->ProbeDevices();
}

void SetFirstOutputChannel(int first_channel){
    reinterpret_cast<audio::VoiceOver*>(v_)->SetFirstOutputChannel(first_channel);
}

void SetFrameRate_kHz(double frame_rate_kHz){
    reinterpret_cast<audio::VoiceOver*>(v_)->SetFrameRate_kHz(frame_rate_kHz);
}

void SetFramesPerBuffer(unsigned int frames_per_buffer){
    reinterpret_cast<audio::VoiceOver*>(v_)->SetFramesPerBuffer(frames_per_buffer);
}

void SetInputChannel(int channel_number){
    reinterpret_cast<audio::VoiceOver*>(v_)->SetInputChannel(channel_number);
}

void SetInputDevice(int device_number){
    reinterpret_cast<audio::VoiceOver*>(v_)->SetInputDevice(device_number);
}

void SetLogLevel(int log_level){
    reinterpret_cast<audio::VoiceOver*>(v_)->SetLogLevel(log_level);
}

void SetOutputDevice(int device_number){
    reinterpret_cast<audio::VoiceOver*>(v_)->SetOutputDevice(device_number);
}

void SetPlaybackFileName(const char playback_file_name[]){
    reinterpret_cast<audio::VoiceOver*>(v_)->SetPlaybackFileName(playback_file_name);
}

void SetRecordedFileName(const char recorded_file_name[]){
    reinterpret_cast<audio::VoiceOver*>(v_)->SetRecordedFileName(recorded_file_name);
}

void SetRmsThreshold(double rms_threshold){
    reinterpret_cast<audio::VoiceOver*>(v_)->SetRmsThreshold(rms_threshold);
}

void SetStereoStatus(bool is_stereo){
    reinterpret_cast<audio::VoiceOver*>(v_)->SetStereoStatus(is_stereo);
}

void SetTrialDurationMilliseconds(double trial_duration_milliseconds){
    reinterpret_cast<audio::VoiceOver*>(v_)->SetTrialDurationMilliseconds(trial_duration_milliseconds);
}

void PrepareDuplexTrial(bool is_veridical){
    reinterpret_cast<audio::VoiceOver*>(v_)->PrepareDuplexTrial(is_veridical);
}

void PreparePlaybackTrial(){
    reinterpret_cast<audio::VoiceOver*>(v_)->PreparePlaybackTrial();
}


void RunTrial(bool is_veridical = false){
    reinterpret_cast<audio::VoiceOver*>(v_)->RunTrial(is_veridical);
}

void RunDuplexTrial(){
    reinterpret_cast<audio::VoiceOver*>(v_)->RunDuplexTrial();
}

void RunPlaybackTrial(){
    reinterpret_cast<audio::VoiceOver*>(v_)->RunPlaybackTrial();
}

void TestDuplexSawWave(){
    reinterpret_cast<audio::VoiceOver*>(v_)->TestDuplexSawWave();
}

void TestPlaybackFile(){
    reinterpret_cast<audio::VoiceOver*>(v_)->TestPlaybackFile();
}

void TestPlaybackSawWave(){
    reinterpret_cast<audio::VoiceOver*>(v_)->TestPlaybackSawWave();
}

void TestRecord(){
    reinterpret_cast<audio::VoiceOver*>(v_)->TestRecord();
}

