#Copyright (c) David A. Magezi 2021

CXX = g++
CFLAGS = -Wall -std=c++20

LOCAL_DIR =  ../../../local

RT_AUDIO_NAME = rtaudio
RT_AUDIO_DIR = $(LOCAL_DIR)/$(RT_AUDIO_NAME)-5.1.0
RT_AUDIO_INCLUDE = $(RT_AUDIO_DIR)
RT_AUDIO_LIB_DIR = $(RT_AUDIO_DIR)/.libs
RT_AUDIO_LIB = $(RT_AUDIO_NAME)

PLOG_DIR = $(LOCAL_DIR)/plog-master/include/

SNDFILE_LIB = sndfile

WINDOWS_FLAGS = -pthread -D__WINDWOS_ASIO__
WINDOWS_LIBS = -lpthread -lole32
WINDOWS_LIBS_OTHER = -ldsound -lwinmm -lksuser -lmfplat -lmfuuid -lwmcodecdspuuid

ASIO_SRC_DIR = $(RT_AUDIO_DIR)/include
ASIO_OBJECT_DIR = $(RT_AUDIO_DIR)/include/.libs
_ASIO_OBJS_SUFFIX = asio.o asiolist.o asiodrivers.o iasiothiscallresolver.o
_ASIO_OBJS = $(addprefix lib$(RT_AUDIO_NAME)_la-,$(_ASIO_OBJS_SUFFIX))
ASIO_OBJS = $(addprefix $(ASIO_OBJECT_DIR)/, $(_ASIO_OBJS))

UNIT_TEST_LIB = cppunit

BASE_DIR = ../..

BIN_DIR =  $(BASE_DIR)/bin
TEST_DEVICES_AND_FILES_STEM = test_devices_and_files 
TEST_DEVICES_AND_FILES_SRC = $(TEST_DEVICES_AND_FILES_STEM).cpp
TEST_DEVICES_AND_FILES = $(BIN_DIR)/$(TEST_DEVICES_AND_FILES_STEM)

SRC_DIR = $(BASE_DIR)/src

AUDIO_NAME = audio
AUDIO_DIR = $(SRC_DIR)/$(AUDIO_NAME)
_AUDIO_DEPS = device.h device_duplex.h device_reader.h device_writer.h file.h file_reader.h file_writer.h pause.h rms.h threshold.h

_AUDIO_SRCS = $(_AUDIO_DEPS:.h=.cpp) 
AUDIO_SRCS = $(addprefix $(AUDIO_DIR)/, $(_AUDIO_SRCS))

UNIT_TEST_NAME = unit_tests
UNIT_TEST_DIR =  $(SRC_DIR)/$(UNIT_TEST_NAME)
_UNIT_TEST_DEPS =  devices_fixture.h files_fixture.h


_UNIT_TEST_SRCS = $(_UNIT_TEST_DEPS:.h=.cpp) 
UNIT_TEST_SRCS = $(addprefix $(UNIT_TEST_DIR)/, $(_UNIT_TEST_SRCS))
UNIT_TEST_DEPS = $(addprefix $(UNIT_TEST_DIR)/, $(_UNIT_TEST_DEPS))

#objects
OBJECT_DIR = $(BASE_DIR)/objs

AUDIO_OBJECT_DIR = $(OBJECT_DIR)/$(AUDIO_NAME)
AUDIO_DEPS = $(addprefix $(AUDIO_DIR)/, $(_AUDIO_DEPS))
_AUDIO_OBJS = $(_AUDIO_DEPS:.h=.o) 
AUDIO_OBJS = $(addprefix $(AUDIO_OBJECT_DIR)/, $(_AUDIO_OBJS))


all: $(TEST_DEVICES_AND_FILES) 

$(AUDIO_OBJECT_DIR):
	mkdir -p $@

$(AUDIO_OBJS): $(AUDIO_OBJECT_DIR)/%.o: $(AUDIO_DIR)/%.cpp $(AUDIO_DEPS) | $(AUDIO_OBJECT_DIR)
	$(CXX)  $< -c -o $@ $(CFLAGS) -I$(RT_AUDIO_DIR) -I$(PLOG_DIR) 

$(TEST_DEVICES_AND_FILES): $(TEST_DEVICES_AND_FILES_SRC) $(AUDIO_OBJS) $(UNIT_TEST_SRCS) $(UNIT_TEST_DEPS) 
	$(CXX) -g $(CFLAGS) $(TEST_DEVICES_AND_FILES_SRC) $(AUDIO_OBJS) $(UNIT_TEST_SRCS) -o @ -I$(RT_AUDIO_DIR) -L$(RT_AUDIO_LIB_DIR) -l$(RT_AUDIO_LIB) $(ASIO_OBJS) $(WINDOWS_FLAGS) $(WINDOWS_LIBS) $(WINDOWS_LIBS_OTHER) -I$(PLOG_DIR) -l$(UNIT_TEST_LIB) -l$(SNDFILE_LIB)

test:
	#rm log_tests.txt
	$(TEST_DEVICES_AND_FILES)

clean:
	rm -f $(TEST_DEVICES_AND_FILES) log*.txt $(AUDIO_OBJS) #note that this still works on windows, despite the .exe extension
