/* HD: the "InOutLoop" UI part (hd_parts_in_out_loop.cpp): plays an In animation, loops (with an
 * optional looping sound) and plays Out on request; states ::StateID_Wait / _In / _Loop / _Out.
 * No GameCube counterpart. "InOutLoop" and the state names are HD strings; class, member and method
 * names are ours. */
#pragma once
#include "d/hd_parts.h"

/* 02030B6C: probably starts (or keeps playing) the UI sound with this label */
inline void hdPartsInOutLoop_startSound(const be<u8>* label, f32 volume, f32 pitch) { gabi::call(0x02030B6C, label, volume, pitch); }

/* animation entries of the layout (slot table sAnimSlots) */
enum hdPartsInOutLoopAnim_e {
    IN_OUT_LOOP_ANIM_IN = 0,
    IN_OUT_LOOP_ANIM_OUT = 1,
    IN_OUT_LOOP_ANIM_LOOP = 2,
};

struct hdPartsInOutLoop_c : hdParts_c {
    /* 0x50 */ be<u8> mIsActive;     /* set by requestIn; requestOut plays Out only while set */
    /* 0x51 */ be<u8> mInRequested;
    /* 0x52 */ be<u8> mOutRequested;
    /* 0x53 */ be<u8> mIsClosed;     /* Out finished (or closed without playing it) */
    /* 0x54 */ hdUiSafeString mLoopSound;

    static void destroy(hdPartsInOutLoop_c* i_this, s32 flags);
    void waitEnter();
    void waitExecute();
    void waitExit();
    void startAnim(u32 anim);
    void inEnter();
    void inExecute();
    void loopExecute();
    void outEnter();
    void outExecute();
    void outExit();
    static hdPartsInOutLoop_c* construct(hdPartsInOutLoop_c* i_this);
    void setAnimFrame(u32 anim, u32 frame);
    u32 init(void* resource, u32 arg1, u32 arg2, hdPartsDesc_c* desc);
    void finalize();
    void executeState();
    void requestIn();
    void deactivate();
    void requestOut();
    void setLoopSound(hdUiSafeString* label);
    u32 load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
};
WWHD_OFFSET(hdPartsInOutLoop_c, mIsActive, 0x50);
WWHD_OFFSET(hdPartsInOutLoop_c, mIsClosed, 0x53);
WWHD_OFFSET(hdPartsInOutLoop_c, mLoopSound, 0x54);
WWHD_SIZE(hdPartsInOutLoop_c, 0x5C);
