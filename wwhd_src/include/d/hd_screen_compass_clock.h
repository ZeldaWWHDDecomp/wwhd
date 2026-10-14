/* HD compass/clock HUD and transitions between its two displays. */
#pragma once
#include "d/hd_parts_no_control.h"
#include "d/hd_parts.h"
struct hdCompassPaneTable_c { u8 _00[0x5c]; gfn mFindPane; };
struct hdCompassPane_c {
    u8 _00[8]; gptr<hdCompassPaneTable_c> mVtable; u8 _0c[0x1c];
    be<f32> mRotationX,mRotationY,mRotationZ; u8 _34[0x10]; be<u8> mFlags;
};
struct hdCompassActor_c { u8 _00[0x32a]; be<s16> mHeading; };
struct hdCompassGame_c { u8 _00[0x44]; be<f32> mClockAngle; };
WWHD_OFFSET(hdCompassPane_c,mRotationZ,0x30);
WWHD_OFFSET(hdCompassPane_c,mFlags,0x44);
WWHD_OFFSET(hdCompassActor_c,mHeading,0x32a);
enum hdCompassClockMode_e { COMPASS_CLOCK_COMPASS=0, COMPASS_CLOCK_DAY=1, COMPASS_CLOCK_NIGHT=2, COMPASS_CLOCK_UNSET=3 };
/* a no-control part (FW-parts_msg_base) */
struct hdCompassClock_c : NoControlPart_l {
    gptr<hdCompassPane_c> mpCompassPane,mpClockPane;
    be<u32> mLastPhase,mMode; be<u8> mCompassOnly; u8 _61[3];
    static hdCompassClock_c* construct(hdCompassClock_c* object);
    static void destroy(hdCompassClock_c* object,u32 flags);
    void waitEnter(); void waitExecute(); void updateCompass(); void updateClock(); void updatePhase(); u32 updateMode();
    void displayExecute(); void setModeFrame();
    void compassInEnter(); void compassInExecute(); void clockInEnter(); void clockInExecute();
    void compassOutEnter(); void compassOutExecute(); void clockOutEnter(); void clockOutExecute();
    void findPanes(); u32 setup(u32 first,u32 second,u32 third); void toWait();
    u32 load(void* name,u32 first,u32 second,u32 third,void* desc);
};
WWHD_OFFSET(hdCompassClock_c,mpCompassPane,0x50);
WWHD_OFFSET(hdCompassClock_c,mMode,0x5c);
WWHD_OFFSET(hdCompassClock_c,mCompassOnly,0x60);
WWHD_SIZE(hdCompassClock_c,0x64);
