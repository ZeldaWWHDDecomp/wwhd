// HD native heap and message adapters; layouts reconstructed from this game's instructions.
#pragma once
#include "sead/HeapRegistry.h"
typedef be<u8> NativeHeapNameByte;

struct JKRNativeMessage {
    be<u32> mValue;
    be<u32> mArguments[3];
};
WWHD_OFFSET(JKRNativeMessage,mValue,0);
WWHD_OFFSET(JKRNativeMessage,mArguments,4);
WWHD_SIZE(JKRNativeMessage,16);

u32 sendNativeMessage(void*,JKRNativeMessage*,u32);
u32 receiveNativeMessage(void*,JKRNativeMessage*,u32);
void* allocateNativeAligned(u32,s32);
