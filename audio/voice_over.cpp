//Copyright (c) David A. Magezi 2021

#include "../audio/voice_over.h"

#include <chrono>   //std::chrono
#include <cmath>   //std::ceil
#include <fstream> //std::ofstream
#include <iostream> //std::cout, std::endl
#include <string>  //std::string
#include <sstream> //std::stringstream

#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"

#include "../audio/device_reader.h"
#include "../audio/device_writer.h"

#include "../audio/file_reader.h"
#include "../audio/file_writer.h"

namespace audio{
VoiceOver::VoiceOver():
    //device_duplex_{}, //necessary?
    frame_rate_kHz_{44.1},
    frames_per_buffer_{512},
    input_channel_{0},
    //input_device_{device_duplex_.GetDefaultInputDevice()},
    is_output_stereo_{true},
    log_level_{0},
    output_channel_first_{0},
    //output_device_{device_duplex_.GetDefaultOutputDevice()},
    playback_file_name_{"NOT_SET_playback.wav"},
    recorded_file_name_{"NOT_SET_recorded.wav"},
    rms_threshold_{0.1},
    trial_duration_milliseconds_{5000},
    number_of_samples_per_trial{static_cast<unsigned int>(std::ceil(trial_duration_milliseconds_*frame_rate_kHz_))}{
        //refactoring target (start)
        std::stringstream log_file_name_ss;
        log_file_name_ss << LOG_PREFIX_ << "general" << LOG_SUFFIX_;
        std::string log_file_name(log_file_name_ss.str());
        std::ofstream log_file_stream;
        log_file_stream.open(log_file_name, std::ofstream::out | std::ofstream::trunc);
        log_file_stream.close();
        plog::init(plog::verbose,log_file_name.c_str());
        //refactoring target (end)
}

VoiceOver::~VoiceOver(){
}

void VoiceOver::AbortTrial(){
}

void VoiceOver::ClearLog(){
}

void VoiceOver::PrepareDuplexTrial(bool is_veridical){
    std::cout << "Value of is veridical is " << is_veridical << "." << std::endl; //debugline

    // create separate public method to set these up
    device_duplex_.SetFirstOutputChannel(output_channel_first_);
    device_duplex_.SetFrameRate_kHz(frame_rate_kHz_);
    device_duplex_.SetFramesPerBuffer(frames_per_buffer_);
    device_duplex_.SetInputDevice(output_device_);
    device_duplex_.SetInputChannel(output_channel_first_);
    device_duplex_.SetOutputDevice(output_device_);
    device_duplex_.SetRmsThreshold(rms_threshold_);
    //device_duplex_.SetStereoStatus(is_output_stereo_); //could be read from libsndfile (and does not need to be set by user)

    if (is_veridical){
        device_duplex_.SetVeridical(is_veridical);
    } else {
        audio::FileReader file_reader(playback_file_name_);
        std::vector<double> playback_data(file_reader.run());
        device_duplex_.SetData(playback_data, file_reader.GetChannels());
    }
}

void VoiceOver::PreparePlaybackTrial(){
    device_writer_.SetFrameRate_kHz(frame_rate_kHz_);
    device_writer_.SetFramesPerBuffer(frames_per_buffer_);
    device_writer_.SetDevice(output_device_);
    device_writer_.SetChannel(output_channel_first_);

    audio::FileReader file_reader(playback_file_name_);
    if (file_reader.GetChannels() == 1) {
        //device_writer_.SetStereoStatus(false); 
        
        device_writer_.SetStereoStatus(true); 
        device_writer_.SetMonoToStereo(true); 
    } else { // only first two channels will be played
        device_writer_.SetStereoStatus(true); 
        device_writer_.SetMonoToStereo(false); 
    }

    std::vector<double> playback_data(file_reader.run());
    device_writer_.SetData(playback_data);
}

void VoiceOver::ProbeDevices(){
    std::stringstream log_file_name_ss;
    log_file_name_ss << LOG_PREFIX_ << "probe_devices" << LOG_SUFFIX_;
    std::string log_file_name(log_file_name_ss.str());
    std::ofstream log_file_stream;
    log_file_stream.open(log_file_name, std::ofstream::out | std::ofstream::trunc);
    log_file_stream.close();
    plog::init<LOG_INSTANCE_PROBE_DEVICES_>(plog::verbose,log_file_name.c_str());
    PLOG_(LOG_INSTANCE_PROBE_DEVICES_,plog::none) << "Probing devices (start)";

    RtAudio device;
    unsigned int number_of_devices(device.getDeviceCount());
    std::string newline = "\n";
    std::stringstream ss;
    RtAudio::DeviceInfo info;
    ss << number_of_devices << " devices have been found" << newline;
    for (unsigned int i = 0; i <number_of_devices; i++) {
        info = device.getDeviceInfo(i);
        if (info.probed == true) {
            ss << newline << "device: " << i << newline;
            ss << "name: " << info.name << newline;
            ss << "maximum number of output channels: " << info.outputChannels << newline;
            ss << "maximum number of input channels: " << info.inputChannels << newline;
            ss << "maximum number of duplex channels: " << info.duplexChannels << newline;
            std::string is_default_output{(info.isDefaultOutput)  ? "yes" : "no"};
            ss << "is default output: " << is_default_output << newline;
            std::string is_default_input{(info.isDefaultInput)  ? "yes" : "no"};
            ss << "is default input: " << is_default_input << newline;
            if ( info.nativeFormats == 0 )
                ss << "No natively supported data formats(?)!";
            else {
                ss << "Natively supported data formats:" << newline;
                if ( info.nativeFormats & RTAUDIO_SINT8 )
                    ss << "  8-bit integer" << newline; 
                if ( info.nativeFormats & RTAUDIO_SINT16 )
                    ss << "  16-bit integer" << newline;
                if ( info.nativeFormats & RTAUDIO_SINT24 )
                    ss << "  24-bit integer" << newline;
                if ( info.nativeFormats & RTAUDIO_SINT32 )
                    ss << "  32-bit integer" << newline;
                if ( info.nativeFormats & RTAUDIO_FLOAT32 )
                    ss << "  32-bit floating point" << newline;
                if ( info.nativeFormats & RTAUDIO_FLOAT64 )
                    ss << "  64-bit floating point" << newline;
            }
            ss << "preferred frame rate: " << info.preferredSampleRate << newline;
            if ( info.sampleRates.size() < 1 ) {
                ss << "No supported sample rates found!";
            } else {
                ss << "Supported sample rates are : ";
            }
            for (unsigned int j=0; j<info.sampleRates.size(); j++){
                ss << info.sampleRates[j] << " ";
            }
            ss << newline;

            ss << "The desired frame rate of " << frame_rate_kHz_ << " kHz"; 
            auto it(std::find(info.sampleRates.begin(),info.sampleRates.end(),frame_rate_kHz_*KILO_));
            if (it !=info.sampleRates.end()) {
                ss << " is supported" << newline;
            } else {
                ss << " is NOT supported" << newline;
            }
            ss << newline << std::endl;
        } else {
            ss << newline << "Device " << i << " could NOT successfully be probed" << newline;
        }
    }
    PLOG_(LOG_INSTANCE_PROBE_DEVICES_,plog::none) << ss.str();
    PLOG_(LOG_INSTANCE_PROBE_DEVICES_,plog::none) << "Probing devices (end)";
    std::cout << "Probe results saved to " << log_file_name << std::endl;
}

void VoiceOver::SetFirstOutputChannel(int first_channel){
    output_channel_first_ = first_channel;
}

void VoiceOver::SetFrameRate_kHz(double frame_rate_kHz){
    frame_rate_kHz_ = frame_rate_kHz;
}

void VoiceOver::SetFramesPerBuffer(unsigned int frames_per_buffer){
    frames_per_buffer_ = frames_per_buffer;
}

void VoiceOver::SetInputChannel(int channel_number){
    input_channel_ = channel_number;
}

void VoiceOver::SetInputDevice(int device_number){
    input_device_ = device_number;
}

void VoiceOver::SetLogLevel(int log_level){
}

void VoiceOver::SetOutputDevice(int device_number){
    output_device_ = device_number;
}


void VoiceOver::SetPlaybackFileName(std::string playback_file_name){
    playback_file_name_ = playback_file_name;
}

void VoiceOver::SetRecordedFileName(std::string recorded_file_name){
    recorded_file_name_ = recorded_file_name;
}

void VoiceOver::SetRmsThreshold(double rms_threshold){
    rms_threshold_ = rms_threshold;
}

void VoiceOver::SetStereoStatus(bool is_stereo){
    is_output_stereo_ = is_stereo;
}

void VoiceOver::SetTrialDurationMilliseconds(double trial_duration_milliseconds){
    trial_duration_milliseconds_ = trial_duration_milliseconds;
}

void VoiceOver::RunTrial(bool is_veridical){

    std::cout << "Value of is veridical is " << is_veridical << "." << std::endl; //debugline

    // create separate public method to set these up
    device_duplex_.SetFirstOutputChannel(output_channel_first_);
    device_duplex_.SetFrameRate_kHz(frame_rate_kHz_);
    device_duplex_.SetFramesPerBuffer(frames_per_buffer_);
    device_duplex_.SetInputDevice(output_device_);
    device_duplex_.SetInputChannel(output_channel_first_);
    device_duplex_.SetOutputDevice(output_device_);
    device_duplex_.SetRmsThreshold(rms_threshold_);
    //device_duplex_.SetStereoStatus(is_output_stereo_); //could be read from libsndfile (and does not need to be set by user)

    if (is_veridical){
        device_duplex_.SetVeridical(is_veridical);
    } else {
        audio::FileReader file_reader(playback_file_name_);
        std::vector<double> playback_data(file_reader.run());
        device_duplex_.SetData(playback_data, file_reader.GetChannels());
    }

    size_t number_of_samples_to_record{static_cast<size_t>(std::ceil(trial_duration_milliseconds_ *  frame_rate_kHz_))};
    device_duplex_.Trial(number_of_samples_to_record);
    std::vector<double> recorded_data(device_duplex_.GetData());
    audio::FileWriter file_writer(recorded_file_name_);
    file_writer.run(recorded_data);
}

void VoiceOver::RunDuplexTrial(){
    size_t number_of_samples_to_record{static_cast<size_t>(std::ceil(trial_duration_milliseconds_ *  frame_rate_kHz_))};
    device_duplex_.Trial(number_of_samples_to_record);
    std::vector<double> recorded_data(device_duplex_.GetData());
    audio::FileWriter file_writer(recorded_file_name_);
    file_writer.run(recorded_data);
}

void VoiceOver::RunPlaybackTrial(){
    device_writer_.PlayData();
}

void VoiceOver::TestDuplexSawWave(){
    // create separate public method to set these up
    device_duplex_.SetFirstOutputChannel(output_channel_first_);
    device_duplex_.SetFrameRate_kHz(frame_rate_kHz_);
    device_duplex_.SetFramesPerBuffer(frames_per_buffer_);
    device_duplex_.SetInputDevice(output_device_);
    device_duplex_.SetInputChannel(output_channel_first_);
    device_duplex_.SetOutputDevice(output_device_);
    device_duplex_.SetRmsThreshold(rms_threshold_);
    //device_duplex_.SetStereoStatus(is_output_stereo_); //in this case cannot be read from libsndfile 

	//device_duplex_.SetVeridical(false);
	//device_duplex_.SetData(std::vector<double>());

    size_t number_of_samples_to_record{static_cast<size_t>(std::ceil(trial_duration_milliseconds_ *  frame_rate_kHz_))};
    device_duplex_.SawTrial(number_of_samples_to_record);
    std::vector<double> recorded_data(device_duplex_.GetData());
    audio::FileWriter file_writer(recorded_file_name_);
    file_writer.run(recorded_data);
}

void VoiceOver::TestPlaybackFile(){
    audio::DeviceWriter device_writer;
    device_writer.SetFrameRate_kHz(frame_rate_kHz_);
    device_writer.SetFramesPerBuffer(frames_per_buffer_);
    device_writer.SetDevice(output_device_);
    device_writer.SetChannel(output_channel_first_);

    audio::FileReader file_reader(playback_file_name_);
    if (file_reader.GetChannels() == 1) {
        device_writer.SetStereoStatus(false); 
    } else {
        device_writer.SetStereoStatus(true); 
    }

    device_writer.SetStereoStatus(is_output_stereo_); 
    std::vector<double> playback_data(file_reader.run());
    device_writer.SetData(playback_data);
    device_writer.PlayData();
}

void VoiceOver::TestPlaybackSawWave(){
    audio::DeviceWriter device_writer;
    PLOG(plog::debug) << "Expected frame rate is  " << frame_rate_kHz_;
    device_writer.SetFrameRate_kHz(frame_rate_kHz_);
    //device_writer.SetFramesPerBuffer(frames_per_buffer_);
    //device_writer.SetDevice(output_device_);
    //device_writer.SetChannel(output_channel_first_);
    //if (file_reader.GetChannels() == 1) {
    //    device_writer.SetStereoStatus(false); 
    //} else {
    //    device_writer.SetStereoStatus(true); 
    //}

    device_writer.PlaySaw();
    std::chrono::high_resolution_clock::time_point tStart,tLoop;
    std::chrono::duration<double> elapsed_time;
    const double maximum_duration_of_output_seconds{trial_duration_milliseconds_ / KILO_};
    tStart = std::chrono::high_resolution_clock::now();
    while ( elapsed_time.count() < maximum_duration_of_output_seconds ) {
        tLoop = std::chrono::high_resolution_clock::now();
        elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(tLoop - tStart);
    }
    device_writer.StopSaw();
}

void VoiceOver::TestRecord(){
    audio::DeviceReader device_reader;
    device_reader.SetFrameRate_kHz(frame_rate_kHz_);
    device_reader.SetFramesPerBuffer(frames_per_buffer_);
    device_reader.SetDevice(input_device_);
    device_reader.SetChannel(input_channel_);

    size_t number_of_samples_to_record{static_cast<size_t>(std::ceil(trial_duration_milliseconds_ *  frame_rate_kHz_))};

    device_reader.Record(number_of_samples_to_record);
    std::vector<double> recorded_data(device_reader.GetData());
    audio::FileWriter file_writer(recorded_file_name_);
    file_writer.run(recorded_data);
}

}//namespace audio

