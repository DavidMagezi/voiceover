//Copyright (c) David A. Magezi 2021

//#include <sndfile.hh>

#include "../audio/file.h"

namespace audio{
File::File(std::string file_name):
    file_name_(file_name){
}

File::~File(){
}

} // namespace audio


