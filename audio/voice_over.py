#Copyright (c) David A. Magezi 2021
print("Testing voice over")

import ctypes
import sys
import os.path
import time

if len(sys.argv) > 1:
    libname = str(sys.argv[1])
    print(libname)

voiceoverlib = ctypes.cdll.LoadLibrary(libname)
print(voiceoverlib)
voiceoverlib.CreateVoiceOver()

probe_devices_bit = int('00000001',2);
saw_wave_bit = int('00000010',2);
playback_bit = int('00000100',2);
record_bit = int('00001000',2);
veridical_bit = int('00010000',2);
duplex_saw_wave_bit = int('00100000',2);
duplex_bit = int('01000000',2);

bit_mask = int('00010000',2); # <=== MODIFY HERE

if (bit_mask & probe_devices_bit):
    voiceoverlib.ProbeDevices()

frame_rate_kHz = float("44.1") #44.1 works fine here (but not in matlab)
frames_per_buffer = int("2048") #512 sounds too "rough" (? ok at 16 kHz)
output_device = int("0")
output_channel = int("0")
is_stereo = False; # better to work out from header of .wav using libsndfile (and set automatically)
rms_threshold = float("0.001")
trial_duration_milliseconds = float("2000")
voiceoverlib.SetFrameRate_kHz(ctypes.c_double(frame_rate_kHz))
voiceoverlib.SetFramesPerBuffer(ctypes.c_uint(frames_per_buffer))
voiceoverlib.SetOutputDevice(ctypes.c_int(output_device))
voiceoverlib.SetFirstOutputChannel(ctypes.c_int(output_channel))
voiceoverlib.SetStereoStatus(ctypes.c_bool(is_stereo)) #CULL-LIST: update and eliminate
voiceoverlib.SetTrialDurationMilliseconds(ctypes.c_double(trial_duration_milliseconds))

if (bit_mask & saw_wave_bit):
    voiceoverlib.TestPlaybackSawWave()
    time.sleep(2)

#playback_file_name = b"../../data/Female_SessionA_Melody1_A_SecondHigh_Note1_shortened_ManipulatedRangeHigh_Note1.wav"
playback_file_name = b"../../data/Female_SessionA_Melody1_A_SecondHigh_Note1_original.wav"

if (bit_mask & playback_bit):
    if os.path.exists(playback_file_name):
        audapter_playback_file_name = b"../../data/from_audapter_input.wav"
        voiceoverlib.SetPlaybackFileName(ctypes.c_char_p(audapter_playback_file_name))
        #voiceoverlib.TestPlaybackFile()

        audapter_frame_rate_kHz = float("16") 
        voiceoverlib.SetFrameRate_kHz(ctypes.c_double(audapter_frame_rate_kHz))
        voiceoverlib.PreparePlaybackTrial()
        voiceoverlib.RunPlaybackTrial()
    else:
        message = playback_file_name + " is not found"
        print(message)

if (bit_mask & record_bit):
    recorded_file_name = b"../../data/recording_only.wav"
    voiceoverlib.SetRecordedFileName(ctypes.c_char_p(recorded_file_name))
    voiceoverlib.TestRecord()

    duplex_recorded_file_name = b"../../data/duplex_recording.wav"
    voiceoverlib.SetRecordedFileName(ctypes.c_char_p(duplex_recorded_file_name))

voiceoverlib.SetRmsThreshold(ctypes.c_double(rms_threshold))

if (bit_mask & veridical_bit):
    is_veridical = True;
    #voiceoverlib.RunTrial(ctypes.c_bool(is_veridical))
    voiceoverlib.PrepareDuplexTrial(ctypes.c_bool(is_veridical))
    voiceoverlib.RunDuplexTrial()

if (bit_mask & duplex_saw_wave_bit):
        voiceoverlib.TestDuplexSawWave()

if (bit_mask & duplex_bit):
    if os.path.exists(playback_file_name):
        print("Standard duplex trial running")
        voiceoverlib.SetPlaybackFileName(ctypes.c_char_p(playback_file_name))
        is_veridical = False;
        voiceoverlib.PrepareDuplexTrial(ctypes.c_bool(is_veridical))
        voiceoverlib.RunDuplexTrial()

voiceoverlib.DestroyVoiceOver()
