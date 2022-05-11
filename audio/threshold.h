// Copyright (c) David A. Magezi 2021

#ifndef THRESHOLD_H
#define THRESHOLD_H

#include  "../audio/rms.h"
namespace audio{
class Threshold{
public :
    Threshold(unsigned int total_number_of_samlples, unsigned int rms_window);
    ~Threshold();
    enum  class Status {not_set, pre_voice, voice, post_voice};
    void Calculate();
    static double Get();
    double GetRMS();
    Status GetStatus();
    void operator()(double next_value);
    void Reset(unsigned int total_number_of_samples, unsigned int rms_window);
    static void Set(double value);
private: 
    Rms rms_;
    Status status_;
    static double rms_threshold_;
};
} // namespace audio
#endif // THRESHOLD_H
