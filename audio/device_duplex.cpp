//Copyright (c) David A. Magezi 2021

#include "../audio/device_duplex.h"

#include <chrono> //std::chrono
#include <fstream> //debugline, to be replaced by plog
#include <sstream> //debugline, to be replaced by plog

#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"

namespace audio{
DeviceDuplex::DeviceDuplex():
    Device(),
    playback_data_{},
    recorded_data_{},
    input_channel_{0},
    input_device_{GetDefaultInputDevice()},
    is_veridical_{true},
    frame_index_{0},
    maximum_number_of_input_frames_{0},
    number_of_playback_channels_{1},
    output_channel_first_{0},
    output_device_{GetDefaultOutputDevice()}, 
    playback_frame_index_{0},
    threshold_(0,0){ 
}

DeviceDuplex::~DeviceDuplex(){
}


int DeviceDuplex::Callback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status){
    int exitStatus(rt::Continue);

    using CURR_TYPE = double;

    //PLOG(plog::debug) << "This is the standard callback";
    //PLOG(plog::debug) << "There are actually " << nBufferFrames << " samples in the next buffer";
    //PLOG(plog::verbose) << "Frame index at start of this buffer is " << frame_index_;
    
    //for recording - start
    CURR_TYPE *in = (CURR_TYPE*) inputBuffer;
    unsigned int pointerPosition(std::min(frame_index_ * number_of_input_channels_,
                maximum_number_of_input_frames_));
    //for recording - end

    //for playback - start
    size_t currPlaybackElement(std::min(size_t(playback_frame_index_ * number_of_output_channels_),
                maximum_number_of_output_samples_));
    CURR_TYPE *out = (CURR_TYPE*) outputBuffer;
    //for playback - end

    unsigned int number_of_frames_to_record(nBufferFrames);
    if ( frame_index_ + int(nBufferFrames) > maximum_number_of_input_frames_ ) {
        number_of_frames_to_record = maximum_number_of_input_frames_ - frame_index_;
        exitStatus = rt::Abort;
    }

    int maximum_number_of_output_frames = maximum_number_of_output_samples_/number_of_output_channels_;
    
    unsigned int number_of_frames_to_playback(nBufferFrames);
    if ( playback_frame_index_ + int(nBufferFrames) > maximum_number_of_output_frames ) {
        number_of_frames_to_playback = maximum_number_of_output_frames - playback_frame_index_;
        exitStatus = rt::Abort;
    }

    //PLOG(plog::verbose) << "number of frames to record is " << number_of_frames_to_record;

    for (unsigned int nextFrame = 0; nextFrame < number_of_frames_to_record; nextFrame++){
        //PLOG(plog::verbose) << "next frame is " << nextFrame;
        //for recording  start
        if (exitStatus == rt::Continue){
            for (unsigned int input_channel_index = 0; input_channel_index < number_of_input_channels_; input_channel_index++){
                recorded_data_[pointerPosition + nextFrame] = *in++;
            } 
        }else{
            for (unsigned int input_channel_index = 0; input_channel_index < number_of_input_channels_; input_channel_index++){
                recorded_data_[pointerPosition + nextFrame] = 0;
            }
        }
        //for recording end
        
        //checking voicing status start
        threshold_(recorded_data_[pointerPosition + nextFrame]);
        //checking voicing status end

        threshold_.Calculate(); // or within loop?
        status_ = threshold_.GetStatus(); // or within loop?

        //for playback start
        switch (status_) {
            case audio::Threshold::Status::voice:
                {
                    //IF_PLOG(plog::debug){
                    //    PLOG(plog::verbose) << "we have voicing during frame " << pointerPosition + nextFrame; 
                    //}
                    //for (unsigned int channel_index = 0; channel_index < 1; channel_index++){ //debugline - formalize, will be important for stereo recordings of strangers
                    for (int channel_index = 0; channel_index < number_of_playback_channels_; channel_index++){
                        if (currPlaybackElement < maximum_number_of_output_samples_){
                            *out++ = playback_data_[currPlaybackElement++]; 
                            if (number_of_playback_channels_ > 0 && number_of_playback_channels_ < number_of_output_channels_) {
                                *out++ = playback_data_[currPlaybackElement]; 
                            }
                        }else{
                            *out++ = 0; 
                        }
                    }
                }
                break;
            case audio::Threshold::Status::pre_voice:
                {
                    //IF_PLOG(plog::debug){
                    //    PLOG(plog::verbose) << "no voicing detected yet during frame " << pointerPosition + nextFrame; 
                    //}
                    *out++ = 0; 
                }
                break;
            case audio::Threshold::Status::post_voice:
                {
                    exitStatus = rt::Abort;
                }
                break;
            case audio::Threshold::Status::not_set:
                {
                    //error or throw exeption here
                    exitStatus = rt::Abort;
                }
                break;
        }
        //for playback end
    }

    frame_index_ += number_of_frames_to_record;
    if ( frame_index_ >= maximum_number_of_input_frames_) { 
        exitStatus = rt::Abort;
    }

    playback_frame_index_ += number_of_frames_to_playback;
    if ( playback_frame_index_ >= maximum_number_of_output_frames) { 
        exitStatus = rt::Abort;
    }

    //PLOG(plog::verbose) << "Frame index at end of this buffer is " << frame_index_;

    //recorded_data_ = std::vector<double>(999,99);//debug_line


    return exitStatus;
}

int DeviceDuplex::CallbackSawWave( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status){
    int exitStatus(rt::Continue);

    using CURR_TYPE = double;
    
    //for recording - start
    CURR_TYPE *in = (CURR_TYPE*) inputBuffer;
    unsigned int pointerPosition(std::min(frame_index_ * number_of_input_channels_,
                maximum_number_of_input_frames_));
    //for recording - end

    //for saw_wave start
    unsigned int saw_j;
    CURR_TYPE *saw_buffer = (CURR_TYPE *) outputBuffer;
    //for saw_wave end
    unsigned int number_of_frames_to_record(nBufferFrames);
    if ( frame_index_ + int(nBufferFrames) > maximum_number_of_input_frames_ ) {
        number_of_frames_to_record = maximum_number_of_input_frames_ - frame_index_;
        exitStatus = rt::Abort;
    }

    for (unsigned int nextFrame = 0; nextFrame < number_of_frames_to_record; nextFrame++){
        //for recording  start
        if (exitStatus == rt::Continue){
            for (unsigned int input_channel_index = 0; input_channel_index < number_of_input_channels_; input_channel_index++){
                //recorded_data_[pointerPosition + nextFrame] = *in;
                recorded_data_[pointerPosition + nextFrame] = *in++;
            } 
        }else{
            for (unsigned int input_channel_index = 0; input_channel_index < number_of_input_channels_; input_channel_index++){
                recorded_data_[pointerPosition + nextFrame] = 0;
            }
        }
        //for recording end
        
        
        //checking voicing status start
        threshold_(recorded_data_[pointerPosition + nextFrame]);
        //checking voicing status end
        
        //for saw wave start
        if (status_ == audio::Threshold::Status::voice){
            for ( saw_j=0; saw_j<2; saw_j++ ) {
              *saw_buffer++ = saw_wave_data_[saw_j];
              saw_wave_data_[saw_j] += 0.005 * (saw_j+1+(saw_j*0.1));
              if ( saw_wave_data_[saw_j] >= 1.0 ) saw_wave_data_[saw_j] -= 2.0;
            }
        }
        //
        //for saw wave end
    }

    frame_index_ += number_of_frames_to_record;
    if ( frame_index_ >= maximum_number_of_input_frames_) { 
        exitStatus = rt::Abort;
    }

    threshold_.Calculate(); // or within loop?
    status_ = threshold_.GetStatus(); // or within loop?


    return exitStatus;
}

int DeviceDuplex::CallbackVeridical( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus underflow_status){
    int exitStatus(rt::Continue);

    PLOG_IF(plog::debug,underflow_status) << "Underflow detected";

    using CURR_TYPE = double;

    IF_PLOG(plog::debug){
        PLOG(plog::debug) << "This is the veridical callback";
        PLOG(plog::debug) << "There are actually " << nBufferFrames << " samples in the next buffer";
        PLOG(plog::verbose) << "Frame index at start of this buffer is " << frame_index_;
    }
    
    //for recording - start
    CURR_TYPE *in = (CURR_TYPE*) inputBuffer;
    unsigned int pointerPosition(std::min(frame_index_ * number_of_input_channels_,
                maximum_number_of_input_frames_));
    //for recording - end

    //for playback - start
    CURR_TYPE *out = (CURR_TYPE*) outputBuffer;
    //for playback - end

    unsigned int number_of_frames_to_record(nBufferFrames);
    if ( frame_index_ + int(nBufferFrames) > maximum_number_of_input_frames_ ) {
        number_of_frames_to_record = maximum_number_of_input_frames_ - frame_index_;
        exitStatus = rt::Abort;
    }

    IF_PLOG(plog::debug){
        PLOG(plog::verbose) << "number of frames to record is " << number_of_frames_to_record;
    }


    for (unsigned int nextFrame = 0; nextFrame < number_of_frames_to_record; nextFrame++){
        IF_PLOG(plog::debug){
            PLOG(plog::verbose) << "next frame is " << nextFrame;
        }
        //for recording  start
        if (exitStatus == rt::Continue){
            for (unsigned int input_channel_index = 0; input_channel_index < number_of_input_channels_; input_channel_index++){
                recorded_data_[pointerPosition + nextFrame] = *in++;
            } 
        }else{
            for (unsigned int input_channel_index = 0; input_channel_index < number_of_input_channels_; input_channel_index++){
                recorded_data_[pointerPosition + nextFrame] = 0;
            }
        }
        //for recording end
        
        //checking voicing status start
        threshold_.Calculate(); // or within loop?

        threshold_(recorded_data_[pointerPosition + nextFrame]);
        IF_PLOG(plog::debug){
            PLOG(plog::verbose) << "The next value is " << recorded_data_[pointerPosition + nextFrame];
        }

        status_ = threshold_.GetStatus(); // within loop is too slow?
        //status_ = audio::Threshold::Status::voice; //debugline
        //checking voicing status end


        //for playback start
        switch (status_) {
            case audio::Threshold::Status::voice:
                {
                    IF_PLOG(plog::debug){
                        PLOG(plog::verbose) << "we have voicing during frame " << pointerPosition + nextFrame; 
                    }
                    for (int output_channel_index = 0; output_channel_index < number_of_output_channels_; output_channel_index ++) {
                        *out++ = recorded_data_[pointerPosition + nextFrame]; 
                    }
                }
                break;
            case audio::Threshold::Status::pre_voice:
                {
                    IF_PLOG(plog::debug){
                        PLOG(plog::verbose) << "no voicing detected yet during frame " << pointerPosition + nextFrame; 
                    }
                    *out++ = 0; 
                }
                break;
            case audio::Threshold::Status::post_voice:
                {
                    exitStatus = rt::Abort;
                }
                break;
            case audio::Threshold::Status::not_set:
                {
                    //error or throw exeption here
                    exitStatus = rt::Abort;
                }
                break;
        }
        //for playback end

    }



    frame_index_ += number_of_frames_to_record;
    if ( frame_index_ >= maximum_number_of_input_frames_) { 
        exitStatus = rt::Abort;
    }

    IF_PLOG(plog::debug){
        PLOG(plog::verbose) << "Frame index at end of this buffer is " << frame_index_;
    }

    return exitStatus;
}
std::vector<double> DeviceDuplex::GetData(){
    return recorded_data_;
}

int DeviceDuplex::Probe(bool is_logging){
    return 0;//debugline? <= probably, just make part of Device (with three categories input,output,duplex)
}

void DeviceDuplex::ResetRecordedData(size_t number_of_samples, unsigned int samples_per_rms_window){
    maximum_number_of_input_frames_ = number_of_samples;
    frame_index_ = 0;
    recorded_data_ = std::vector<double>(maximum_number_of_input_frames_,0);
    threshold_.Reset(maximum_number_of_input_frames_,samples_per_rms_window); 
}

int DeviceDuplex::SawTrial(size_t number_of_samples){
    //plog::init(plog::verbose,"log_saw_trial.txt");
    maximum_number_of_input_frames_ = number_of_samples;
    frame_index_ = 0;
    recorded_data_ = std::vector<double>(maximum_number_of_input_frames_,0); 

    std::chrono::high_resolution_clock::time_point tStart,tLoop;
    std::chrono::duration<double> elapsed_time;
    const double maximum_duration_of_input_seconds{5};//to prevent infinite while loop, if error

    int error_code{-4};
    std::stringstream ss; //change with plog (after eating) ... 
    if ( device_.getDeviceCount() < 1 ) {
        error_code = -3;
        ss << "Error code: " << error_code << ", insufficient devices\n";
        return error_code;
    }
    RtAudio::StreamParameters input_parameters, output_parameters;
    input_parameters.deviceId = device_.getDefaultInputDevice();
    input_parameters.nChannels = 1;
    input_parameters.firstChannel = 0;
    output_parameters.deviceId = device_.getDefaultOutputDevice();
    output_parameters.nChannels = 2;
    output_parameters.firstChannel = 0;
    //unsigned int frame_rate = 44100;
    unsigned int frame_rate_Hz = desired_frame_rate_kHz * KILO_;
    unsigned int frames_per_buffer = desired_frames_per_buffer; // e.g. 256 sample frames

    threshold_ = audio::Threshold(number_of_samples,frames_per_buffer);
    PLOG(plog::verbose) << "Value of threshold is " << threshold_.Get();
    //int onset_buffer{-1}; //debugline
    //int offset_buffer{-1}; //debugline
    try {
        device_.openStream(&output_parameters,&input_parameters,RTAUDIO_FLOAT64,
                        frame_rate_Hz, &frames_per_buffer, &StaticCallbackSawWave,this);
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
        //while ( device_.isStreamRunning() && elapsed_time.count() < maximum_duration_of_input_seconds ) {
        unsigned int i_buffer{0};
        while ( elapsed_time.count() < maximum_duration_of_input_seconds ) {
            /*
            status_ = threshold_.GetStatus();//separate threads needed?
            if (status_ == audio::Threshold::Status::voice){
                if (onset_buffer == -1){
                    onset_buffer = i_buffer;
                    PLOG(plog::debug) << "Voicing  (duplex saw) starts at " << i_buffer;
                }
            } else if (status_ == audio::Threshold::Status::post_voice){
                if (offset_buffer == -1){
                    offset_buffer = i_buffer;
                    PLOG(plog::debug) << "Voicing  (duplex saw) ends at " << i_buffer;
                }
            } else{
                PLOG(plog::debug) << "Period of NO voicing  (duplex saw) detected during buffer " << i_buffer;
            }
            PLOG_IF(plog::debug,status_ == audio::Threshold::Status::pre_voice) << "no voice onset detected during trial (duplex saw)";
            */
            i_buffer ++;
            tLoop = std::chrono::high_resolution_clock::now();
            elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(tLoop - tStart);
        }
        PLOG(plog::debug) << "Total number of buffers was " << i_buffer;
        PLOG(plog::debug) << "Total number of buffers should have been " << number_of_samples/frames_per_buffer;
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
    std::ofstream log_file("log_duplex_saw.txt");
    log_file << ss.str();
    log_file.close();
    return error_code;
}

int DeviceDuplex::SetData(std::vector<double> audio_data, int number_of_channels){
    playback_data_ = audio_data;
    is_veridical_ = false;
    playback_frame_index_ = 0;
    maximum_number_of_output_samples_ = playback_data_.size();
    number_of_playback_channels_ = number_of_channels;
    //why is this value returned, how do we refactor with ResetRecordedData 
    return maximum_number_of_output_samples_;
}


void DeviceDuplex::SetInputDevice(int device_number){
    input_device_ = device_number;
}

void DeviceDuplex::SetInputChannel(int input_channel){
    input_channel_ = input_channel;
}

void DeviceDuplex::SetOutputDevice(int device_number){
    output_device_ = device_number;
}

void DeviceDuplex::SetFirstOutputChannel(int output_channel){
    output_channel_first_ = output_channel;
}

void DeviceDuplex::SetRmsThreshold(double rms_threshold){
    threshold_.Set(rms_threshold);
}

void DeviceDuplex::SetVeridical(bool is_veridical){
    is_veridical_ = is_veridical;
    if (is_veridical_){
        playback_data_ = std::vector<double>();
    }
}

int DeviceDuplex::Trial(size_t number_of_samples){
    maximum_number_of_input_frames_ = number_of_samples;
    frame_index_ = 0;
    recorded_data_ = std::vector<double>(maximum_number_of_input_frames_,0); 

    std::chrono::high_resolution_clock::time_point tStart,tLoop;
    std::chrono::duration<double> elapsed_time;
    const double maximum_duration_of_input_seconds{5};//to prevent infinite while loop, if errof


    int error_code{-4};
    std::stringstream ss; //change with plog (after eating) ... 
    if ( device_.getDeviceCount() < 1 ) {
        error_code = -3;
        PLOG(plog::error) << "Error code: " << error_code << ", insufficient devices";
        return error_code;
    }
    RtAudio::StreamParameters input_parameters;
    input_parameters.deviceId = input_device_;
    input_parameters.nChannels = number_of_input_channels_;
    input_parameters.firstChannel = input_channel_;

    RtAudio::StreamParameters output_parameters;
    output_parameters.deviceId = output_device_;
    output_parameters.nChannels = number_of_output_channels_;
    output_parameters.firstChannel = output_channel_first_;

    unsigned int frame_rate_Hz = frame_rate_kHz_ * KILO_;

    threshold_ = audio::Threshold(maximum_number_of_input_frames_,frames_per_buffer_);
    PLOG(plog::verbose) << "Value of threshold is " << threshold_.Get();

    std::string not_str{(is_veridical_) ? "actually" : "NOT"};
    PLOG(plog::verbose) << "This trial is " << not_str << " veridical";

    plog::Severity previous_severity = plog::get()->getMaxSeverity();
    plog::get()->setMaxSeverity(plog::error);
    //plog::get()->setMaxSeverity(plog::verbose); //debugline

    try {
        if (is_veridical_){
            device_.openStream(&output_parameters, &input_parameters,RTAUDIO_FLOAT64,
                    frame_rate_Hz, &frames_per_buffer_, &StaticCallbackVeridical,this);
        } else {
            device_.openStream(&output_parameters, &input_parameters,RTAUDIO_FLOAT64,
                    frame_rate_Hz, &frames_per_buffer_, &StaticCallback,this);
        }
        device_.startStream();
        error_code = 0;
        PLOG(plog::verbose) << "Successfully opened  and started stream. \"Error\" code is " << error_code;
    } catch ( RtAudioError& e ) {
        error_code = -2;
        PLOG(plog::error) << "Error code: " << error_code << ", failed to open  and start stream";
        PLOG(plog::error) << e.getMessage();
        return( error_code );
    }
    try{
        tStart = std::chrono::high_resolution_clock::now();
        while (device_.isStreamRunning() && elapsed_time.count() < maximum_duration_of_input_seconds ) {
            tLoop = std::chrono::high_resolution_clock::now();
            elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(tLoop - tStart);
        }
        error_code = 0;
        PLOG(plog::verbose) << "Successfully waited for while stream was open . \"Error code\": " << error_code; 
    }catch (std::exception& e){
        error_code = -5;
        PLOG(plog::error) << "Error code: " << error_code << ", failed to wait while stream was open";
        PLOG(plog::error) << e.what();
        return(error_code);
    }
    try {
        // Stop the stream
        device_.stopStream();
        PLOG(plog::verbose) << "Successfully stopped stream. \"Error code\": " << error_code; 
        PLOG(plog::verbose) << "Frame index is " << frame_index_;
        PLOG(plog::verbose) << "Maximum number of input frames is " << maximum_number_of_input_frames_;
        PLOG(plog::verbose) << "Frames per buffer are " << frames_per_buffer_;
    } catch (RtAudioError& e) {
        error_code = -6;
        PLOG(plog::error) << "Error code: " << error_code << ", failed to stop stream";
        PLOG(plog::error) << e.getMessage();
    }
    if (device_.isStreamOpen()) device_.closeStream();
    plog::get()->setMaxSeverity(previous_severity);
    return error_code;
}

//private functions 

int DeviceDuplex::SawCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
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


int DeviceDuplex::StaticCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData ){
    return static_cast<DeviceDuplex*>(userData)->Callback(outputBuffer,inputBuffer,nBufferFrames,streamTime,status);
}

int DeviceDuplex::StaticCallbackSawWave( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData ){
    return static_cast<DeviceDuplex*>(userData)->CallbackSawWave(outputBuffer,inputBuffer,nBufferFrames,streamTime,status);
}

int DeviceDuplex::StaticCallbackVeridical( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData ){
    return static_cast<DeviceDuplex*>(userData)->CallbackVeridical(outputBuffer,inputBuffer,nBufferFrames,streamTime,status);
}

} // namespace audio

