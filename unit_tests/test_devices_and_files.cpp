#include <iostream>

#include <cppunit/ui/text/TestRunner.h>

#include "../unit_tests/devices_fixture.h"
#include "../unit_tests/files_fixture.h"

int main(){
    std::cout << "Testing Devices" << std::endl;
    CppUnit::TextUi::TestRunner runner;
    
    runner.addTest( unit_test::DevicesFixture::suite() );
    runner.addTest( unit_test::FilesFixture::suite() );
    runner.run();
}

