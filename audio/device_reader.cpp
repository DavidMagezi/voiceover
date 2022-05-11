//Copyright (c) David A. Magezi 2021

#include "../audio/device_reader.h"

#include <algorithm>    //std::min
#include <chrono>       //std::chrono
#include <fstream> //debugline, to be replaced by plog
#include <sstream> //debugline, to be replaced by plog

namespace audio{

DeviceReader::DeviceReader():
    Device(),
    data_{},
    frame_index_{0},
    maximum_number_of_input_frames_{0}{
}

DeviceReader::~DeviceReader(){
}


int DeviceReader::Callback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status){
    int exitStatus(rt::Continue);

    using CURR_TYPE = double;
    
    //for recording - start
    CURR_TYPE *in = (CURR_TYPE*) inputBuffer;
    unsigned int pointerPosition(std::min(frame_index_ * number_of_input_channels_,
                maximum_number_of_input_frames_));
    //for recording - end

    unsigned int number_of_frames_to_record(nBufferFrames);
    if ( frame_index_ + int(nBufferFrames) > maximum_number_of_input_frames_ ) {
        number_of_frames_to_record = maximum_number_of_input_frames_ - frame_index_;
        exitStatus = rt::Abort;
    }

    for (unsigned int nextFrame = 0; nextFrame < number_of_frames_to_record; nextFrame++){
        //for recording  start
        if (exitStatus == rt::Continue){
            for (unsigned int input_channel_index = 0; input_channel_index < number_of_input_channels_; input_channel_index++){
                //data_[pointerPosition + nextFrame] = *in;
                data_[pointerPosition + nextFrame] = *in++;
            } 
        }else{
            for (unsigned int input_channel_index = 0; input_channel_index < number_of_input_channels_; input_channel_index++){
                data_[pointerPosition + nextFrame] = 0;
            }
        }
        //for recording end
    }

    frame_index_ += number_of_frames_to_record;
    if ( frame_index_ >= maximum_number_of_input_frames_) { 
        exitStatus = rt::Abort;
    }


    return exitStatus;
}

int DeviceReader::StaticCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData ){
    return static_cast<DeviceReader*>(userData)->Callback(outputBuffer,inputBuffer,nBufferFrames,streamTime,status);
}

std::vector<double> DeviceReader::GetData(){
    return data_;
}

int DeviceReader::Probe(bool is_logging){
    return 0;//debugline?
}

int DeviceReader::Record(size_t number_of_samples){
    maximum_number_of_input_frames_ = number_of_samples;
    frame_index_ = 0;
    data_ = std::vector<double>(maximum_number_of_input_frames_,0); 

    std::chrono::high_resolution_clock::time_point tStart,tLoop;
    std::chrono::duration<double> elapsed_time;
    const double maximum_duration_of_input_seconds{5};//to prevent infinite while loop, if errof


    int error_code{-4};
    std::stringstream ss; //change with plog (after eating) ... 
    if ( device_.getDeviceCount() < 1 ) {
        error_code = -3;
        ss << "Error code: " << error_code << ", insufficient devices\n";
        return error_code;
    }
    RtAudio::StreamParameters input_parameters;
    input_parameters.deviceId = device_.getDefaultInputDevice();
    input_parameters.nChannels = 1;
    input_parameters.firstChannel = 0;
    unsigned int frame_rate_Hz = desired_frame_rate_kHz * KILO_;
    unsigned int frames_per_buffer = desired_frames_per_buffer; // e.g. 256 sample frames, add underscore at end, to neaten up (change device.h and device_writer.cpp)
    try {
        device_.openStream(output_parameters_null,&input_parameters,RTAUDIO_FLOAT64,
                        frame_rate_Hz, &frames_per_buffer, &StaticCallback,this);
        device_.startStream();
        error_code = 0;
        ss << "Error code: " << error_code << ", successfully opened  and started stream\n";
    } catch ( RtAudioError& e ) {
        error_code = -2;
        ss << "Error code: " << error_code << ", failed to open  and start stream\n";
        ss << e.getMessage();
        return( error_code );
    }
    try{
        tStart = std::chrono::high_resolution_clock::now();
        //while ( input_.isStreamRunning() && elapsed_time.count() < maximum_duration_of_input_seconds ) {
        while ( elapsed_time.count() < maximum_duration_of_input_seconds ) {
            tLoop = std::chrono::high_resolution_clock::now();
            elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(tLoop - tStart);
        }
    }catch (std::exception& e){
        error_code = -5;
        ss << e.what() << std::endl;
        return(error_code);
    }
    try {
        // Stop the stream
        device_.stopStream();
        ss << "Error code: " << error_code << ", successfully stopped stream\n";
        ss << "Frame index is " << frame_index_ << "\n";
        ss << "Maximum number of input frames is " << maximum_number_of_input_frames_ << "\n";
        ss << "Frames per buffer " << frames_per_buffer << "\n";
    } catch (RtAudioError& e) {
        error_code = -6;
        ss << "Error code: " << error_code << ", failed to stop stream\n";
        ss << e.getMessage();
    }
    if (device_.isStreamOpen()) device_.closeStream();
    std::ofstream log_file("log_record.txt");
    log_file << ss.str();
    log_file.close();
    //debug start
    //data_ = std::vector<double>(5000,0);
    //data_[2] = -4;
    //data_[4997] = 4;
    //debug end
    return error_code;
}

void DeviceReader::SetDevice(int device_number){
    device_number_ = device_number;
}

void DeviceReader::SetChannel(int input_channel){
    input_channel_ = input_channel;
}

}//namespace audio
