//Copyright (c) David A. Magezi 2021

#ifndef FILE_WRITER_H
#define FILE_WRTTER_H

#include <string>
#include <vector>

//#include <sndfile.hh>

#include "../audio/file.h"

namespace audio{
class FileWriter : public File { 

public:
    FileWriter(std::string file_name);
    ~FileWriter();

    int run(std::vector<double> data);
};
} // namespace audio
#endif // FILE:WRITER_H


