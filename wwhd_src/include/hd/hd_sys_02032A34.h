/** HD screen dimming views. */
#pragma once
#include "wwhd.h"

struct hdScreenDimMutex {
    u8 mStorage[0x3C]; // Opaque sead mutex, constructed and used by the out-of-unit methods.
};
struct hdScreenDim_c {
    be<u8> mEnabled;
    u8 _padding01[3];
    hdScreenDimMutex mMutex;
};
WWHD_OFFSET(hdScreenDim_c, mEnabled, 0);
WWHD_OFFSET(hdScreenDim_c, mMutex, 4);
WWHD_SIZE(hdScreenDim_c, 0x40);
struct hdScreenDimSystemState {
    be<u32> mEnabled;
};
WWHD_SIZE(hdScreenDimSystemState, 4);

void hdScreenDimMutexConstruct(hdScreenDimMutex* mutex);
void hdScreenDimMutexDestroy(hdScreenDimMutex* mutex, s32 flags);
void hdScreenDimMutexLock(hdScreenDimMutex* mutex);
void hdScreenDimMutexUnlock(hdScreenDimMutex* mutex);
void IMEnableDim();
void IMDisableDim();
void IMIsDimEnabled(be<u32>* enabled);
