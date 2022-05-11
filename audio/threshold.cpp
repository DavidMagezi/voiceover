// Copyright (c) David A. Magezi 2021

#include "../audio/threshold.h"

//#include<iostream> //debugline

namespace audio{

Threshold::Threshold(unsigned int total_number_of_samples, unsigned int rms_window):
    rms_{total_number_of_samples,rms_window},
    status_{Threshold::Status::not_set}{

}

Threshold::~Threshold(){
}

void Threshold::Calculate(){

    double threshold_value{rms_threshold_}; 
    double rms_value{rms_.Calculate()};
    if (status_ == Status::not_set){
        status_ = Status::pre_voice;
    }
    if (status_ == Status::pre_voice){
        if (rms_value > threshold_value) {
            status_ = Status::voice;
        }
    }
    if (status_ == Status::voice){
        //std::cout << "voice is on <---- " << std::endl;
        if (rms_value < threshold_value) {
            status_ = Status::post_voice;
            //std::cout << "voice is now OFF!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        }
    }
}

double Threshold::Get(){
    return rms_threshold_;
}

double Threshold::GetRMS(){
    return rms_.Calculate();
}

Threshold::Status Threshold::GetStatus(){
    return status_;
}

void Threshold::operator()(double next_value){
    rms_(next_value);
}

void Threshold::Reset(unsigned int total_number_of_samples, unsigned int rms_window){
    rms_ = Rms(total_number_of_samples,rms_window);
    status_ = Threshold::Status::not_set;
}

void Threshold::Set(double threshold_value){
    rms_threshold_ = threshold_value;
}

double Threshold::rms_threshold_ = 0.1;

} // namespace audio
