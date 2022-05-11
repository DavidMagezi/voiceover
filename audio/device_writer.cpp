//Copyright (c) David A. Magezi 2021

#include "../audio/device_writer.h"

#include <algorithm> //std::find
#include <fstream>
#include <sstream>

#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"

namespace audio{
DeviceWriter::DeviceWriter():
    audio::Device(),
    first_channel_{0},
    data_{},
    device_number_{0},
    frame_index_{0},
    maximum_number_of_output_samples_{data_.size()},
    mono_to_stereo_(false),
    number_of_channels_{2}{
}

DeviceWriter::~DeviceWriter(){
}

int DeviceWriter::Callback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status){
    int exitStatus(rt::Continue);

    PLOG_IF(plog::debug,status) << "Underflow detected";

    using CURR_TYPE = double;

    //for playback - start
    size_t currPlaybackElement(std::min(size_t(frame_index_ * number_of_channels_),
                maximum_number_of_output_samples_));
    CURR_TYPE *out = (CURR_TYPE*) outputBuffer;
    //for playback - end

    IF_PLOG(plog::debug){
        PLOG(plog::debug) << "There are actually " << nBufferFrames << " samples in the next buffer";
        PLOG(plog::debug) << "Frame index at start of this buffer is " << frame_index_;
    }

    unsigned int maximum_number_of_output_frames = maximum_number_of_output_samples_/number_of_channels_;
    
    unsigned int number_of_frames_to_playback(nBufferFrames);
    if ( frame_index_ + int(nBufferFrames) > maximum_number_of_output_frames ) {
        number_of_frames_to_playback = maximum_number_of_output_frames - frame_index_;
        exitStatus = rt::Abort;
    }

    IF_PLOG(plog::debug){
        PLOG(plog::debug) << "number of frames to playback is " << number_of_frames_to_playback;
    }

    for (unsigned int nextFrame = 0; nextFrame < number_of_frames_to_playback; nextFrame++){
        IF_PLOG(plog::debug){
            PLOG(plog::debug) << "next frame is " << nextFrame;
        }
        //for playback
        for (unsigned int channel_index = 0; channel_index < number_of_channels_; channel_index++){
            if (currPlaybackElement < maximum_number_of_output_samples_){
                *out++ = data_[currPlaybackElement++]; 
            }else{
                *out++ = 0; 
            }
        }
    }

    frame_index_ += number_of_frames_to_playback;
    if ( frame_index_ >= maximum_number_of_output_frames) { 
        exitStatus = rt::Abort;
    }

    IF_PLOG(plog::debug){
        PLOG(plog::debug) << "Frame index at end of this buffer is " << frame_index_;
    }

    return exitStatus;
}

int DeviceWriter::Probe(bool is_logging){
    unsigned int number_of_devices(device_.getDeviceCount());
    if (is_logging){
        std::stringstream ss;
        RtAudio::DeviceInfo info;
        for (unsigned int i = 0; i <number_of_devices; i++) {
            info = device_.getDeviceInfo(i);
            if (info.probed == true) {
                ss << "device: " << i << newline_;
                ss << "name: " << info.name << newline_;
                ss << "maximum number of output channels: " << info.outputChannels << newline_;
                ss << "maximum number of input channels: " << info.inputChannels << newline_;
                ss << "maximum number of duplex channels: " << info.duplexChannels << newline_;
                ss << "is default output: " << info.isDefaultOutput << newline_;
                ss << "is default input: " << info.isDefaultInput << newline_;
                ss << "preferred frame rate: " << info.preferredSampleRate << newline_;
                if ( info.nativeFormats == 0 )
                    ss << "No natively supported data formats(?)!";
                else {
                    ss << "Natively supported data formats:\n";
                    if ( info.nativeFormats & RTAUDIO_SINT8 )
                        ss << "  8-bit integer" << "\n"; 
                    if ( info.nativeFormats & RTAUDIO_SINT16 )
                        ss << "  16-bit integer" << "\n";
                    if ( info.nativeFormats & RTAUDIO_SINT24 )
                        ss << "  24-bit integer" << newline_;
                    if ( info.nativeFormats & RTAUDIO_SINT32 )
                        ss << "  32-bit integer" << newline_;
                    if ( info.nativeFormats & RTAUDIO_FLOAT32 )
                        ss << "  32-bit floating point" << newline_;
                    if ( info.nativeFormats & RTAUDIO_FLOAT64 )
                        ss << "  64-bit floating point" << newline_;
                }
                if ( info.sampleRates.size() < 1 ) {
                    ss << "No supported sample rates found!";
                } else {
                    ss << "Supported sample rates = ";
                }
                for (unsigned int j=0; j<info.sampleRates.size(); j++){
                    ss << info.sampleRates[j] << " ";
                }
                ss << std::endl;

                ss << "The desired frame rate of " << desired_frame_rate_kHz << " kHz"; 
                auto it(std::find(info.sampleRates.begin(),info.sampleRates.end(),desired_frame_rate_kHz*KILO_));
                if (it !=info.sampleRates.end()) {
                    ss << " is supported" << newline_;
                } else {
                    ss << " is NOT supported" << std::endl;
                }
                ss << newline_ << std::endl;
            }
        }
        std::ofstream log_file("log_probe.txt");
        log_file << ss.str();
        log_file.close();
    }
    return number_of_devices;
}


int DeviceWriter::SawCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData ){
    unsigned int i, j;
    double *buffer = (double *) outputBuffer;
    double *lastValues = (double *) userData;
    if ( status ) std::cout << "Stream underflow detected!" << std::endl;
    // Write interleaved audio data.
    for ( i=0; i<nBufferFrames; i++ ) {
        for ( j=0; j<2; j++ ) {
          *buffer++ = lastValues[j];
          lastValues[j] += 0.005 * (j+1+(j*0.1));
          if ( lastValues[j] >= 1.0 ) lastValues[j] -= 2.0;
        }
    }
return 0;
}

int DeviceWriter::PlayData(){
    //then callback (see ../unit_tests/devices_fixture.cpp)
    int  error_code{-4};
    if ( device_.getDeviceCount() < 1 ) {
        error_code = -3;
        PLOG(plog::error) << "Error code: " << error_code << ", insufficient devices";
        return error_code;
    }
    RtAudio::StreamParameters output_parameters;

    output_parameters.deviceId = device_number_;
    output_parameters.nChannels = number_of_channels_;
    output_parameters.firstChannel = first_channel_;

    unsigned int frame_rate_Hz = frame_rate_kHz_ * KILO_;

    plog::Severity previous_severity = plog::get()->getMaxSeverity();
    plog::get()->setMaxSeverity(plog::error);
    try {
        device_.openStream( &output_parameters, input_parameters_null, RTAUDIO_FLOAT64,
                        frame_rate_Hz, &frames_per_buffer_, StaticCallback, this);
        device_.startStream();
        error_code = 0;
        PLOG(plog::verbose) << "Successfully opened  and started stream. \"Error\" code is " << error_code;
    } catch ( RtAudioError& e ) {
        error_code = -2;
        PLOG(plog::error) << "Error code: " << error_code << ", failed to open  and start stream";
        PLOG(plog::error) << e.getMessage();
        return( error_code );
    }

    try {
        while (device_.isStreamRunning()){
        }
        error_code = 0;
        PLOG(plog::verbose) << "Successfully waited for while stream was open . \"Error code\": " << error_code; 
    } catch (RtAudioError& e) {
        error_code = -1;
        PLOG(plog::error) << "Error code: " << error_code << ", failed to wait while stream was open";
        PLOG(plog::error) << e.getMessage();
    }

    try {
        // Stop the stream
        device_.stopStream();
        error_code = 0;
        PLOG(plog::verbose) << "Successfully stopped stream. \"Error code\": " << error_code; 
    } catch (RtAudioError& e) {
        error_code = -1;
        PLOG(plog::error) << "Error code: " << error_code << ", failed to stop stream";
        PLOG(plog::error) << e.getMessage();
    }
    if ( device_.isStreamOpen() ) device_.closeStream();
    plog::get()->setMaxSeverity(previous_severity);
    return error_code;
}

int DeviceWriter::PlaySaw(){
    int error_code{-4};
    if ( device_.getDeviceCount() < 1 ) {
        error_code = -3;
        PLOG(plog::error) << "Error code: " << error_code << ", insufficient devices";
        return error_code;
    }
    RtAudio::StreamParameters output_parameters;

    //output_parameters.deviceId = device_.getDefaultOutputDevice();
    output_parameters.deviceId = device_number_;
    output_parameters.nChannels = number_of_channels_;
    output_parameters.firstChannel = first_channel_;
    //unsigned int frame_rate = 44100;

    unsigned int frame_rate_Hz = static_cast<unsigned int>(frame_rate_kHz_ * KILO_);
    PLOG(plog::debug) << "The intended frame rate in kHz is " << frame_rate_kHz_;
    PLOG(plog::debug) << "The calculated frame rate in Hz is " << frame_rate_Hz;
    //unsigned int frames_per_buffer = desired_frames_per_buffer; // e.g. 256 sample frames

    double data[2];
    try {
        device_.openStream( &output_parameters, input_parameters_null, RTAUDIO_FLOAT64,
                        frame_rate_Hz, &frames_per_buffer_, &SawCallback, (void *)&data );
        device_.startStream();
        error_code = 0;
        PLOG(plog::verbose) << "Successfully opened  and started stream. \"Error\" code is " << error_code;
    } catch ( RtAudioError& e ) {
        error_code = -2;
        PLOG(plog::error) << "Error code: " << error_code << ", failed to open  and start stream";
        PLOG(plog::error) << e.getMessage();
        return( error_code );
    }
    return error_code;
}

int DeviceWriter::SetData(std::vector<double> audio_data){
    data_ = audio_data;
    if (mono_to_stereo_) { 
        data_ = ConvertMonoToStereo(data_);
    }
    frame_index_ = 0;
    maximum_number_of_output_samples_ = data_.size();
    return maximum_number_of_output_samples_;
}

void DeviceWriter::SetDevice(int device_number){
    device_number_ = device_number;
}

void DeviceWriter::SetChannel(int first_channel){
    first_channel_ = first_channel;
}

void DeviceWriter::SetMonoToStereo(bool mono_to_stereo){
    if (mono_to_stereo){
        if (number_of_channels_ == 2) {
            mono_to_stereo_ = mono_to_stereo;
        }
    }
}

void DeviceWriter::SetStereoStatus(bool is_stereo){
    if (is_stereo) {
        number_of_channels_ = 2;
    } else {
        number_of_channels_ = 1;
    }
}


int DeviceWriter::StopSaw(){
    int error_code{-4};
    try {
        // Stop the stream
        device_.stopStream();
        error_code = 0;
        PLOG(plog::verbose) << "Successfully stopped stream. \"Error code\": " << error_code; 
    } catch (RtAudioError& e) {
        error_code = -1;
        PLOG(plog::error) << "Error code: " << error_code << ", failed to stop stream";
        PLOG(plog::error) << e.getMessage();
    }
    if ( device_.isStreamOpen() ) device_.closeStream();
    return error_code;
}

//private functions
std::vector<double> DeviceWriter::ConvertMonoToStereo(std::vector<double> mono){
    const size_t number_of_mono_samples{mono.size()};
    const unsigned int number_of_channels_in_stereo{2};
    std::vector<double> stereo(number_of_mono_samples*number_of_channels_in_stereo);
    for (size_t i = 0; i < mono.size(); i++){
        stereo[i * number_of_channels_in_stereo] = mono[i];
        stereo[i * number_of_channels_in_stereo + 1] = mono[i];
    }
    return stereo;
}

int DeviceWriter::StaticCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData ){
    return static_cast<DeviceWriter*>(userData)->Callback(outputBuffer,inputBuffer,nBufferFrames,streamTime,status);
}

}//namespace audio
