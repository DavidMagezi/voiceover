// Copyright (c) David A. Magezi 2021

#ifndef RMS_H
#define RMS_H

#include <vector>
namespace audio{
class Rms{
public :
    Rms(unsigned int total_number_of_samlples, unsigned int rms_window);
    ~Rms();
    double Calculate();
    void operator()(double next_value);
private: 
    unsigned int index_;
    unsigned int rms_window_;
    size_t total_number_of_samples_;
    std::vector<double> squared_values_;
};
} // namespace audio
#endif // RMS_H

