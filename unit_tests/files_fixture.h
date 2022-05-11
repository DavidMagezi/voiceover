//Copyright (c) David A. Magezi 2021

#ifndef FILES_FIXTURE_H
#define FILES_FIXTURE_H

#include <memory>
#include <string>

#include <cppunit/Test.h>
#include <cppunit/TestFixture.h>

#include "../audio/file_reader.h"
#include "../audio/file_writer.h"

namespace unit_test{
class  FilesFixture : public CppUnit::TestFixture {
public:
    void setUp();
    static CppUnit::Test* suite();
    void tearDown();

    void testReader();
    void testWriter();

private:
    std::unique_ptr<audio::FileReader> file_reader_;
    std::unique_ptr<audio::FileWriter> file_writer_;
};

}//namespace unit_test
#endif //FILES_FIXTURE_H


