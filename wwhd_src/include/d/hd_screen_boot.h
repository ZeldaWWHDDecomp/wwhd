/* HD Boot_00 screen. No GameCube counterpart; private game-derived reconstruction. */
#pragma once
#include "d/hd_parts.h"

typedef gptr<hdScreenUiSystem> hdBootUiPointer;
typedef gptr<void> hdBootResourcePointer;
struct hdBootSafeStringPair { hdUiSafeString mNames[2]; };
WWHD_SIZE(hdBootSafeStringPair, 0x10);
WWHD_OFFSET(hdBootSafeStringPair, mNames, 0);
struct hdBootRuntimeWords { be<u32> mWords[4]; };
struct hdBootRegistration { be<u32> mWords[3]; };
struct hdBootLifetimeContext { be<u8> mByte; };
WWHD_SIZE(hdBootRuntimeWords, 0x10);
WWHD_SIZE(hdBootRegistration, 0xC);
WWHD_SIZE(hdBootLifetimeContext, 1);
struct hdBootGroupIndex { be<u32> mIndex; };
WWHD_SIZE(hdBootGroupIndex, 4);
WWHD_OFFSET(hdBootGroupIndex, mIndex, 0);

struct hdBoot_c : hdParts_c {
    static hdBoot_c* construct(hdBoot_c* screen);
    static void destroy(hdBoot_c* screen, u32 flags);
    void loopEnter();
    void loopExecute();
    void outAllEnter();
    void outAllExecute();
    void attachBootArchives(hdUiSafeString* names, u32 count);
    u32 initializeBoot(u32 resource, u32 option, void* archive);
    u32 loadBoot(hdUiSafeString* name, hdPartsResAccessor_c* accessor, u32 resource, u32 option, void* archive);
    void finalizeBoot();
    void executeBootState();
};
WWHD_SIZE(hdBoot_c, 0x50);
WWHD_OFFSET(hdBoot_c, mStateHolder, 8);
WWHD_OFFSET(hdBoot_c, mSecondaryVtable, 0x30);
WWHD_OFFSET(hdBoot_c, mpLayout, 0x44);

u32 hdBootStateKey(hdPartsStateId_c* state);
