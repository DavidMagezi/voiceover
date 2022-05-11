//Copyright (c) David A. Magezi 2021

#include "../unit_tests/devices_fixture.h"

#include <algorithm>    //std::min, std::max
#include <chrono>
#include <cmath> //std::sin, std::abs
#include <iostream> //std::cout
#include <numbers> //std::numbers
#include <thread>
#include <vector>

#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"

#include "RtAudio.h"

#include "../audio/rms.h"
#include "../audio/threshold.h"

namespace unit_test{

void DevicesFixture::setUp(){ 
    plog::init(plog::verbose,"log_tests.txt");
    plog::init<duplex_log_instance>(plog::verbose,"log_duplex.txt");
    plog::init<playback_log_instance>(plog::verbose,"log_playback.txt");
    //device_duplex_ = std::make_unique<audio::DeviceDuplex>();
    device_duplex_mock_voice_ = std::make_unique<audio::DeviceDuplex>();
    device_reader_ = std::make_unique<audio::DeviceReader>();
    device_writer_ = std::make_unique<audio::DeviceWriter>();
}

CppUnit::Test* DevicesFixture::suite(){
    CppUnit::TestSuite* suite_of_tests(new CppUnit::TestSuite("Devices"));

    //the number of test corresponds to how many times each line in the "PLOG" hfile is repeated 
    
    suite_of_tests->addTest(new CppUnit::TestCaller<DevicesFixture>("testDeviceCount",
			    &unit_test::DevicesFixture::testDeviceCount));
    suite_of_tests->addTest(new CppUnit::TestCaller<DevicesFixture>("testPlayAudioData",
			    &unit_test::DevicesFixture::testPlayAudioData));
    suite_of_tests->addTest(new CppUnit::TestCaller<DevicesFixture>("testPlaySawWave",
			    &unit_test::DevicesFixture::testPlaySawWave));
    suite_of_tests->addTest(new CppUnit::TestCaller<DevicesFixture>("testRecord",
                &unit_test::DevicesFixture::testRecord));
    suite_of_tests->addTest(new CppUnit::TestCaller<DevicesFixture>("testRMS",
			    &unit_test::DevicesFixture::testRMS));
    suite_of_tests->addTest(new CppUnit::TestCaller<DevicesFixture>("testOnsetAndOffset",
			    &unit_test::DevicesFixture::testOnsetAndOffset));
    suite_of_tests->addTest(new CppUnit::TestCaller<DevicesFixture>("testDuplexWithSawWave",
			    &unit_test::DevicesFixture::testDuplexWithSawWave));
    suite_of_tests->addTest(new CppUnit::TestCaller<DevicesFixture>("testDuplexMock",
			    &unit_test::DevicesFixture::testDuplexMock));
    return suite_of_tests;
}

void DevicesFixture::tearDown(){ 
    //device_duplex_.reset(nullptr);
    device_duplex_mock_voice_.reset(nullptr);
    device_reader_.reset(nullptr);
    device_writer_.reset(nullptr);

}

void DevicesFixture::testDeviceCount(){
    bool is_logging{true};//when true, this is an integration test
    CPPUNIT_ASSERT(device_writer_->Probe(is_logging) > 0);
}

void DevicesFixture::testDuplexMock(){
    constexpr unsigned int number_of_buffers = 5; //100;
    constexpr unsigned int samples_per_buffer = 10; //256;
    constexpr double frame_rate_kHz  = 44.1;

    constexpr double signal_frequency_kHz = 1;
    constexpr double starting_phase = 0;
    constexpr double frame_duration = 1/frame_rate_kHz;
    double peak_amplitude;

    unsigned int number_of_samples{number_of_buffers*samples_per_buffer};
    constexpr unsigned int mock_voice_onset = 14; //300;
    constexpr unsigned int mock_voice_offset = 36; //1900;

    PLOG_(duplex_log_instance,plog::debug) << "There should be " << samples_per_buffer << " samples in each buffer";
    device_duplex_mock_voice_->ResetRecordedData(number_of_samples,samples_per_buffer);

    unsigned int i_sample_total;
    std::vector<double> mock_voice;
    RtAudioStreamStatus status(0);
    for (unsigned int i_buffer = 0; i_buffer < number_of_buffers; i_buffer++){
        mock_voice = std::vector<double>(0);
        for (unsigned int i_sample = 0; i_sample < samples_per_buffer; i_sample ++){
            i_sample_total = (i_buffer*samples_per_buffer) + i_sample;
            if (i_sample_total > mock_voice_onset && i_sample_total < mock_voice_offset){
                peak_amplitude = 1;
            } else {
                peak_amplitude = 0;
            }
            mock_voice.push_back(peak_amplitude*sin(2*std::numbers::pi*signal_frequency_kHz*(i_sample_total*frame_duration) + starting_phase));
        }
        PLOG_(duplex_log_instance,plog::debug) << "There should be " << samples_per_buffer << " samples in the next buffer";
        device_duplex_mock_voice_->Callback(nullptr, mock_voice.data(), samples_per_buffer, double(0), status);
    }
    std::vector<double> recorded_data{device_duplex_mock_voice_->GetData()};
    unsigned int peek_shift{3};
    PLOG_(duplex_log_instance,plog::debug) << "Details of mock voice recorded";
    PLOG_(duplex_log_instance,plog::debug) << "There were " << recorded_data.size() << " samples of mock voice recorded.";
    PLOG_(duplex_log_instance,plog::debug) << "There should have been " << number_of_samples << " samples";
    PLOG_IF_(duplex_log_instance,plog::debug, mock_voice_onset - peek_shift >= 0) << "Mock voice recorded  (before onset) at position " << mock_voice_onset - peek_shift << " has a value of " << recorded_data[mock_voice_onset - peek_shift];
    PLOG_IF_(duplex_log_instance,plog::debug, mock_voice_onset + peek_shift < recorded_data.size()) << "Mock voice recorded  (after onset) at position " << mock_voice_onset + peek_shift << " has a value of " << recorded_data[mock_voice_onset + peek_shift];
    PLOG_IF_(duplex_log_instance,plog::debug, mock_voice_offset + peek_shift < recorded_data.size()) << "Mock voice recorded  (after offset) at position " << mock_voice_offset + peek_shift << " has a value of " << recorded_data[mock_voice_offset + peek_shift];

    //size_t number_of_samples(50000);
    ////CPPUNIT_ASSERT(device_duplex_->SawTrial(number_of_samples) >= 0);
}

void DevicesFixture::testDuplexWithSawWave(){
    //size_t number_of_samples(50000);
    //CPPUNIT_ASSERT(device_duplex_->SawTrial(number_of_samples) >= 0);
    //debugline above (as well as initialization of duplex_duplex_ in start up)
}

void DevicesFixture::testPlayAudioData(){
    PLOG_(playback_log_instance,plog::debug) << "Playing audio";

    std::vector<double> audio_data{0,1,2,3,4,5,6,7,8,9,0,-1,-2,-3,-4,-5,-6,-7,-8,-9};
    device_writer_->SetData(audio_data);

    constexpr unsigned int samples_per_buffer = 10; //256;
    //constexpr double frame_rate_kHz  = 44.1;

    PLOG_(playback_log_instance,plog::debug) << "There should be " << samples_per_buffer << " samples in each buffer";

    std::vector<double> mock_output(audio_data.size(),-10);
    RtAudioStreamStatus status(0);
    unsigned int i_sample_total{0};
    while (i_sample_total < audio_data.size()){
        device_writer_->Callback(mock_output.data() + i_sample_total, nullptr, samples_per_buffer, double(0), status);
        i_sample_total += samples_per_buffer;
    }
    PLOG_(playback_log_instance,plog::verbose) << "Minimum value in mock playback is " << *std::min_element(mock_output.begin(),mock_output.end());
    PLOG_(playback_log_instance,plog::verbose) << "Minimum value in mock playback should be " << *std::min_element(audio_data.begin(),audio_data.end());
    PLOG_(playback_log_instance,plog::verbose) << "Maximum value in mock playback is " << *std::max_element(mock_output.begin(),mock_output.end());
    PLOG_(playback_log_instance,plog::verbose) << "Maximum value in mock playback should be " << *std::max_element(audio_data.begin(),audio_data.end());
    //CPPUNIT_ASSERT(device_writer_->PlayData() >= 0);
}

void DevicesFixture::testPlaySawWave(){
    PLOG(plog::none) << "Will play saw";
    int play{device_writer_->PlaySaw()};
    PLOG(plog::none) << "Playing saw";
    std::chrono::high_resolution_clock::time_point tStart,tLoop;
    std::chrono::duration<double> elapsed_time;
    const double maximum_duration_of_input_seconds{2}; 
    while ( elapsed_time.count() < maximum_duration_of_input_seconds ) {
        tLoop = std::chrono::high_resolution_clock::now();
        elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(tLoop - tStart);
    }
    int stop{device_writer_->StopSaw()};
    PLOG(plog::none) << "have played saw";
    CPPUNIT_ASSERT(play == 0 && stop == 0);
}

void DevicesFixture::testRecord(){
    size_t number_of_samples(50000);
    std::vector<double> audio_data(number_of_samples);

    int error_code{device_reader_->Record(number_of_samples)};
    CPPUNIT_ASSERT(error_code == 0);
    audio_data = device_reader_->GetData();

    PLOG(plog::none) << "Testing Recording";
    size_t number_of_elements_recorded{audio_data.size()};
    PLOG(plog::verbose) << "Error code was " << error_code; 
    PLOG(plog::verbose) << "There are " << number_of_elements_recorded << " samples recorded. (Intended was " << number_of_samples << ")."; 
    PLOG_IF(plog::verbose, number_of_elements_recorded > 0) << "Minimum value recorded is " << *std::min_element(audio_data.begin(),audio_data.end());
    PLOG_IF(plog::verbose, number_of_elements_recorded > 0) << "Maximum value recorded is " << *std::max_element(audio_data.begin(),audio_data.end());

    std::vector<double> mock_audio_data{-4,0,4};
    size_t number_of_elements_mocked{mock_audio_data.size()};
    PLOG_IF(plog::verbose, number_of_elements_mocked > 0) << "(MOCK) Minimum value recorded is " << *std::min_element(mock_audio_data.begin(),mock_audio_data.end());
    PLOG_IF(plog::verbose, number_of_elements_mocked > 0) << "(MOCK) Maximum value recorded is " << *std::max_element(mock_audio_data.begin(),mock_audio_data.end());

}

void DevicesFixture::testRMS(){
    constexpr unsigned int number_of_buffers = 100;
    constexpr unsigned int samples_per_buffer = 256;
    constexpr double frame_rate_kHz  = 44.1;

    constexpr double signal_frequency_kHz = 1;
    constexpr double starting_phase = 0;
    constexpr double frame_duration = 1/frame_rate_kHz;
    constexpr double peak_amplitude = 1;

    unsigned int number_of_samples{number_of_buffers*samples_per_buffer};
    audio::Rms rms(number_of_samples,number_of_samples);

    double current_value;
    for (unsigned int i_sample = 0;i_sample < number_of_buffers * samples_per_buffer; i_sample ++){
        current_value = peak_amplitude*sin(2*std::numbers::pi*signal_frequency_kHz*(i_sample*frame_duration) + starting_phase);
        rms(current_value);
    }

    constexpr double correct_rms = 1/std::sqrt(2);//the rms of a sine wave is 1/sqrt(2) of the peak amplitude. About 0.7

    //std::cout << "correct RMS should be " << correct_rms << ", calculated RMS is " << rms.Calculate() << std::endl;
    double tolerance{0.000001};

    CPPUNIT_ASSERT(std::abs(rms.Calculate() - correct_rms) < tolerance);
}

void DevicesFixture::testOnsetAndOffset(){
    PLOG(plog::none) << "Testing onset and offset";

    constexpr int number_of_buffers = 6;
    constexpr int samples_per_buffer = 10;

    constexpr int actual_onset = 11;
    constexpr int actual_offset = 25;

    CPPUNIT_ASSERT(actual_offset < (number_of_buffers * samples_per_buffer));

    constexpr int number_of_samples = number_of_buffers*samples_per_buffer;
    std::vector<double> input(number_of_samples,0);
    for (int i_sample = 0;i_sample < number_of_samples; i_sample ++){
        if (i_sample >= actual_onset && i_sample < actual_offset){
            input[i_sample] = 9999;
        }
        IF_PLOG(plog::verbose){
            PLOG(plog::verbose) << "input " << i_sample << ": " << input[i_sample];
        }
    }

    audio::Threshold threshold(number_of_samples,samples_per_buffer);
    audio::Threshold::Status status;
    int onset_buffer{-1};
    int offset_buffer{-1};
    for (unsigned int i_buffer = 0; i_buffer < number_of_buffers; i_buffer++){
        for (unsigned int i_sample = 0; i_sample < samples_per_buffer; i_sample ++){
            threshold(input[(i_buffer*samples_per_buffer) + i_sample]);
        }
        PLOG(plog::verbose) << "RMS for buffer " << i_buffer << " is " << threshold.GetRMS();
        threshold.Calculate();
        status = threshold.GetStatus();
        if (status == audio::Threshold::Status::voice){
            if (onset_buffer == -1){
                onset_buffer = i_buffer;
                PLOG(plog::debug) << "Voicing starts at " << i_buffer;
            }
        } else if (status == audio::Threshold::Status::post_voice){
            if (offset_buffer == -1){
                offset_buffer = i_buffer;
                PLOG(plog::debug) << "Voicing ends at " << i_buffer;
            }
        } else{
            PLOG(plog::debug) << "Period of NO voicing detected during buffer " << i_buffer;
	}
    }
    PLOG_IF(plog::debug,status == audio::Threshold::Status::pre_voice) << "no voice onset detected";
    CPPUNIT_ASSERT(onset_buffer > -1 && onset_buffer < number_of_buffers && offset_buffer > -1 && onset_buffer < number_of_buffers);
}

} //namespace unit_test

