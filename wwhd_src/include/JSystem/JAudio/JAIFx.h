/* JAudio 1 effects buffers; GC names, verified against WWHD. */
#pragma once
#include "JSystem/JAudio/JAIGlobalParameter.h"
namespace JAInter { namespace Fx {
struct initOnCodeFxScene_s {
    be<u32> sceneCount;
    be<u32> bufferSizes[4];
    be<u32> sceneOffsets[1];
};
WWHD_OFFSET(initOnCodeFxScene_s, bufferSizes, 4);
WWHD_OFFSET(initOnCodeFxScene_s, sceneOffsets, 0x14);
WWHD_SIZE(initOnCodeFxScene_s, 0x18);
void init();
u32 getBufferSizeMax(u32 line);
void setSceneMax(u8 count);
void setBufferMax(u32 a, u32 b, u32 c, u32 d);
void setTablePointer(gptr<void>* table);
void setScenePointer(u32 scene, void* config);
void setBufferPointer(u32 line, s16* buffer);
s16* getBufferPointer(u32 line);
}}

void fxAssert(const char*, u32, const char*);
void* allocateFxArray(u32, JKRHeap*, s32);
void setFxLine(u32, s16*, void*);
void registerFxStatics(AudioDestructorRecord*);
