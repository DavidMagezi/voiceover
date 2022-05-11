//Copyright (c) David A. Magezi 2021

#ifndef FILE_H
#define FILE_H

#include <string>

namespace audio{
class File{

public:
    File(std::string file_name);
    ~File();

protected:
    std::string file_name_;
};
}// namespace audio
#endif // FILE_H

