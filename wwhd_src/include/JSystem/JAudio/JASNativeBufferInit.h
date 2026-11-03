// DSP/DAC initialization views, based on this game's global storage accesses.
#pragma once
#include "JSystem/JAudio/JASDSPBuf.h"
#include "JSystem/JAudio/JASDeviceMixer.h"
#include "sead/Allocation.h"
struct NativeDacBufferSet { gptr<be<s16>> mBuffers[3]; };
struct NativeOutputModeMap { be<u8> mModes[4]; };
using NativeDspBufferTable = gptr<gptr<be<s16>>>;
WWHD_SIZE(NativeDacBufferSet,12);
WWHD_SIZE(NativeOutputModeMap,4);
be<s16>* allocateNativeAudioBuffer(u32,sead::AllocationHeap*,s32);
void* clearNativeAudioBuffer(void*,u32,u32);
void queryNativeAudioDeviceMode(u32,be<u32>*);
u32 nativeDacSampleCount();
