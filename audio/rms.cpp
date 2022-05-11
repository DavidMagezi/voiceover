// Copyright (c) David A. Magezi 2021

#include "../audio/rms.h"

#include <numeric>   //std::accumulate
#include <cmath>  //std::pow

namespace audio{

Rms::Rms(unsigned int total_number_of_samples, unsigned int rms_window):
    index_{0},
    rms_window_{rms_window},
    total_number_of_samples_{total_number_of_samples},
    squared_values_(total_number_of_samples){
}

Rms::~Rms(){
}

double Rms::Calculate(){
    double rms_value(0 ); 
    if (index_ >= rms_window_ && index_ <= total_number_of_samples_){
        double sum_of_squares{std::accumulate(
                squared_values_.begin() + (index_ - rms_window_),
                squared_values_.begin() + index_,
                double(0))};
        rms_value = std::sqrt(sum_of_squares/rms_window_);
    }
    return rms_value;
}

void Rms::operator()(double next_value){
    if (index_ < total_number_of_samples_){
        squared_values_[index_++] = std::pow(next_value,2);
    }
}
    
} // namespace audio

