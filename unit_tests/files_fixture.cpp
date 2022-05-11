//Copyright (c) David A. Magezi 2021

#include "../unit_tests/files_fixture.h"

#include <string>
#include <vector>

#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"

#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

namespace unit_test{

void FilesFixture::setUp(){ 
    plog::init(plog::verbose,"log_file_tests.txt");
    std::string file_name_input{"test_in.wav"};
    std::string file_name_output{"test_out.wav"};
    file_reader_ = std::make_unique<audio::FileReader>(file_name_input);
    file_writer_ = std::make_unique<audio::FileWriter>(file_name_output);
}

CppUnit::Test* FilesFixture::suite(){
    CppUnit::TestSuite* suite_of_tests(new CppUnit::TestSuite("Devices"));
    suite_of_tests->addTest(new CppUnit::TestCaller<FilesFixture>("testFileReader",
			    &unit_test::FilesFixture::testReader));
    suite_of_tests->addTest(new CppUnit::TestCaller<FilesFixture>("testFileWriter",
			    &unit_test::FilesFixture::testWriter));
    return suite_of_tests;
}

void FilesFixture::tearDown(){ 
    file_reader_.reset(nullptr);
    file_writer_.reset(nullptr);

}

void FilesFixture::testReader(){
    std::vector<double> data(file_reader_->run());
    CPPUNIT_ASSERT(data.size() > 0);
}

void FilesFixture::testWriter(){
    std::vector<double> data{0.1,0.2,0.3};
    int number_of_samples_recorded{file_writer_->run(data)};
    int intended_number(data.size());
    CPPUNIT_ASSERT(number_of_samples_recorded == intended_number);
}

} //namespace unit_test
