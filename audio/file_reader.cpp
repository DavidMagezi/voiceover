//Copyright (c) David A. Magezi 2021

#include "../audio/file_reader.h"

#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"

#include <sndfile.hh>

namespace audio{
FileReader::FileReader(std::string file_name):
    File(file_name),
    playback_file_{file_name_.c_str()}{
}

FileReader::~FileReader(){
}

int FileReader::GetChannels(){
    return playback_file_.channels();
}

std::vector<double> FileReader::run(){
    size_t number_of_frames;
    std::vector<double> data;
    //SndfileHandle playback_file(file_name_.c_str());
    if(playback_file_.frames() > 0 && playback_file_.channels() > 0 && 
        playback_file_.samplerate() > 0){
        number_of_frames = playback_file_.frames();
        int number_of_channels = playback_file_.channels();
        size_t number_of_samples{number_of_frames*number_of_channels};
        data = std::vector<double>(number_of_samples);
        long long int number_of_samples_read{playback_file_.read(data.data(),number_of_samples)};
        if((number_of_samples - number_of_samples_read) != 0){
            PLOG(plog::error) << "Wav file not successfully read";
            PLOG(plog::error) << "Number of Channels expected:" << number_of_channels;
            PLOG(plog::error) << "Number of Frames expected:" << number_of_frames;
            PLOG(plog::error) << "Total Number of Samples expected:" << number_of_samples;
            PLOG(plog::error) << "Number of samples actually read was: " << number_of_samples_read;
        }else{
            PLOG(plog::debug) << "Successfully opened wav file( " << file_name_ << ") for playback";
        }
    }else{
        PLOG(plog::error) << "unable to read wav file"; 
        PLOG(plog::error) << "Path should be: " << file_name_;
    }
    return data;
}

} // namespace audio

