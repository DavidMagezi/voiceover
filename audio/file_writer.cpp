//Copyright (c) David A. Magezi 2021

#include "../audio/file_writer.h"

#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"

#include <sndfile.hh>

namespace audio{
FileWriter::FileWriter(std::string file_name):
    File(file_name){
}

FileWriter::~FileWriter(){
}

int FileWriter::run(std::vector<double> data){
    int number_of_samples_recorded{0}; //debugline;
    unsigned int number_of_channels{1};
    int frame_rate_Hz {44100};
    SndfileHandle record_file(file_name_.c_str(),
        SFM_WRITE,
        SF_FORMAT_WAV | SF_FORMAT_DOUBLE,
        number_of_channels,
	frame_rate_Hz
    );
    size_t number_of_samples_to_write(data.size());
    record_file.write(data.data(),number_of_samples_to_write);
    PLOG(plog::debug) << number_of_samples_to_write << " samples recorded to " << file_name_;
    return number_of_samples_recorded;
}

} // namespace audio



