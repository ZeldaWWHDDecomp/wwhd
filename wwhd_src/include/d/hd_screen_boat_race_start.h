/* HD boat-race countdown screen. Binary-derived; no GameCube counterpart. */
#pragma once
#include "d/hd_parts.h"
struct hdBoatRaceRegistration { be<u32> mWords[3]; };
struct hdBoatRaceWords { be<u32> mWords[4]; };
struct hdBoatRaceRange { be<f32> mLower, mUpper; };
class hdScreenBoatRaceStart_c : public hdParts_c {
public:
    /* 0x50 */ be<u8> mVisible;
    /* 0x51 */ be<u8> mStarted;
    u8 mPadding[2];
    static hdScreenBoatRaceStart_c* construct(hdScreenBoatRaceStart_c*);
    static void destroy(hdScreenBoatRaceStart_c*, u32 flags);
    void enterAll();
    void executeAll();
    void leaveAll();
    u32 setup(u32 first, u32 second, void* descriptor);
    void release();
    void execute();
    void draw(void* info);
    u32 load(void* name, u32 archive, u32 first, u32 second, void* descriptor);
};
WWHD_OFFSET(hdScreenBoatRaceStart_c, mVisible, 0x50);
WWHD_OFFSET(hdScreenBoatRaceStart_c, mStarted, 0x51);
WWHD_SIZE(hdScreenBoatRaceStart_c, 0x54);
