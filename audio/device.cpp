//Copyright (c) David A. Magezi 2021

#include "../audio/device.h"

namespace audio{
Device::Device():
    frame_rate_kHz_{44.1},
    frames_per_buffer_{512},
    newline_{"\n"}{
}

Device::~Device(){
}

unsigned int Device::GetDefaultInputDevice(){
    return device_.getDefaultInputDevice();
}

unsigned int Device::GetDefaultOutputDevice(){
    return device_.getDefaultOutputDevice();
}

void Device::SetFrameRate_kHz(double frame_rate_kHz){
    frame_rate_kHz_ = frame_rate_kHz;
}

void Device::SetFramesPerBuffer(unsigned int frames_per_buffer){
    frames_per_buffer_ = frames_per_buffer;
}

}//namespace audio

