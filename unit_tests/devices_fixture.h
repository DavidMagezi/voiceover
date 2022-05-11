//Copyright (c) David A. Magezi 2021

#ifndef DEVICES_FIXTURE_H
#define DEVICES_FIXTURE_H

#include <memory> //std::unique_ptr

#include <cppunit/Test.h>
#include <cppunit/TestFixture.h>

#include "../audio/device_duplex.h"
#include "../audio/device_reader.h"
#include "../audio/device_writer.h"

namespace unit_test{
class  DevicesFixture : public CppUnit::TestFixture {
public:
    void setUp();
    static CppUnit::Test* suite();
    void tearDown();

    void testDeviceCount();
    void testDuplexMock();
    void testDuplexWithSawWave();
    void testPlayAudioData();
    void testPlaySawWave();
    void testRecord();
    void testRMS();
    void testOnsetAndOffset();

private:
    //std::unique_ptr<audio::DeviceDuplex> device_duplex_;
    std::unique_ptr<audio::DeviceDuplex> device_duplex_mock_voice_;
    std::unique_ptr<audio::DeviceReader> device_reader_;
    std::unique_ptr<audio::DeviceWriter> device_writer_;

    static constexpr unsigned int playback_log_instance = 1;
    static constexpr unsigned int duplex_log_instance = 2;
};

}//namespace unit_test
#endif //DEVICES_FIXTURE_H


