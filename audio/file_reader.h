//Copyright (c) David A. Magezi 2021

#ifndef FILE_READER_H
#define FILE_READER_H

#include <string>
#include <vector>

#include <sndfile.hh>

#include "../audio/file.h"

namespace audio{
class FileReader : public File { 

public:
    FileReader(std::string file_name);
    ~FileReader();

    int GetChannels();
    std::vector<double> run();
private:
    SndfileHandle playback_file_;

};
} // namespace audio
#endif // FILE_READER_H



