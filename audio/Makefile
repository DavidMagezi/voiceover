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

BASE_DIR = ../..

SRC_DIR = $(BASE_DIR)/src

AUDIO_NAME = audio
AUDIO_DIR = $(SRC_DIR)/$(AUDIO_NAME)
_AUDIO_DEPS = device.h device_duplex.h device_reader.h device_writer.h file.h file_reader.h file_writer.h pause.h rms.h threshold.h 

_AUDIO_SRCS = $(_AUDIO_DEPS:.h=.cpp) 
AUDIO_SRCS = $(addprefix $(AUDIO_DIR)/, $(_AUDIO_SRCS))

#objects
OBJECT_DIR = $(BASE_DIR)/objs

AUDIO_OBJECT_DIR = $(OBJECT_DIR)/$(AUDIO_NAME)
AUDIO_DEPS = $(addprefix $(AUDIO_DIR)/, $(_AUDIO_DEPS))
_AUDIO_OBJS = $(_AUDIO_DEPS:.h=.o) 
AUDIO_OBJS = $(addprefix $(AUDIO_OBJECT_DIR)/, $(_AUDIO_OBJS))


BIN_DIR = ../../bin
VOICE_OVER_LIB = $(BIN_DIR)/voiceover.dll

all: $(VOICE_OVER_LIB)

$(AUDIO_OBJECT_DIR):
	mkdir -p $@

$(AUDIO_OBJS): $(AUDIO_OBJECT_DIR)/%.o: $(AUDIO_DIR)/%.cpp $(AUDIO_DEPS) | $(AUDIO_OBJECT_DIR)
	$(CXX)  $< -c -o $@ $(CFLAGS) -I$(RT_AUDIO_DIR) -I$(PLOG_DIR) 

$(VOICE_OVER_LIB): $(AUDIO_OBJS) $(AUDIO_DIR)/voice_over.cpp $(AUDIO_DIR)/voice_over.h $(AUDIO_DIR)/voice_over_interface.cpp $(AUDIO_DIR)/voice_over_interface.h 
	$(CXX) $(CFLAGS) -shared -Wl,--out-implib,libtstdll.a -s $(AUDIO_DIR)/voice_over.cpp $(AUDIO_OBJS) $(AUDIO_DIR)/voice_over_interface.cpp -o $(VOICE_OVER_LIB) -I$(PLOG_DIR) -I$(RT_AUDIO_DIR) -L$(RT_AUDIO_LIB_DIR) -l$(RT_AUDIO_LIB)  $(ASIO_OBJS) $(WINDOWS_FLAGS) $(WINDOWS_LIBS) $(WINDOWS_LIBS_OTHER) -l$(SNDFILE_LIB) -fpic #-Werror 

#why is this after -shared (?): -Wl,--out-implib,libtstdll.a 

test:
	python voice_over.py $(VOICE_OVER_LIB)

clean:
	rm -f $(VOICE_OVER_LIB) log*.txt 
