/* HD EndingEnd screen; private game-derived reconstruction. */
#pragma once
#include "d/hd_parts.h"
typedef gptr<hdScreenUiSystem> hdEndingUiPointer;
struct hdEndingGroupIndex { be<u32> mIndex; };
WWHD_SIZE(hdEndingGroupIndex, 4);
WWHD_OFFSET(hdEndingGroupIndex, mIndex, 0);
struct hdEndingRuntimeWords { be<u32> mWords[4]; };
struct hdEndingRegistration { be<u32> mWords[3]; };
struct hdEndingContext { be<u8> mByte; };
WWHD_SIZE(hdEndingRuntimeWords, 0x10);
WWHD_SIZE(hdEndingRegistration, 0xC);
WWHD_SIZE(hdEndingContext, 1);
struct hdEndingEnd_c : hdParts_c {
    static hdEndingEnd_c* construct(hdEndingEnd_c* screen);
    static void destroy(hdEndingEnd_c* screen, u32 flags);
    void allEnter();
    void allExecute();
    u32 initializeEnding(u32 resource, u32 option, void* archive);
    u32 loadEnding(hdUiSafeString* name, hdPartsResAccessor_c* accessor, u32 resource, u32 option, void* archive);
    void finalizeEnding();
    void executeEndingState();
};
WWHD_SIZE(hdEndingEnd_c, 0x50);
WWHD_OFFSET(hdEndingEnd_c, mStateHolder, 8);
WWHD_OFFSET(hdEndingEnd_c, mSecondaryVtable, 0x30);
WWHD_OFFSET(hdEndingEnd_c, mpLayout, 0x44);
u32 hdEndingEndStateKey(hdPartsStateId_c* state);
