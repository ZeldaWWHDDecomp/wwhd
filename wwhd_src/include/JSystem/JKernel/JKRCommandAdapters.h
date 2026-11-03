// HD decompression command parameter view, restricted to canonical opaque storage.
#pragma once
#include "JSystem/JKernel/JKRDecomp.h"
struct NativeDecompParameters {
    be<u32> mReserved;
    gptr<void> mSource;
    gptr<void> mDestination;
    be<u32> mSourceLength;
    be<u32> mDestinationLength;
};
WWHD_SIZE(NativeDecompParameters,0x14);
WWHD_OFFSET(NativeDecompParameters,mSource,4);
WWHD_OFFSET(NativeDecompParameters,mDestination,8);
WWHD_OFFSET(NativeDecompParameters,mSourceLength,12);
WWHD_OFFSET(NativeDecompParameters,mDestinationLength,16);
void* currentNativeCommandHeap();
JKRDecompCommand* allocateNativeCommand(u32,void*,s32);
JKRDecompCommand* constructNativeCommand(JKRDecompCommand*);
