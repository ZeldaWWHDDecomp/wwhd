/* Shared JAudio 1 moving parameter layout. */
#pragma once
#include "bindings.h"
struct JAISound;
namespace JAInter {
struct MoveParaSet {
    void init();
    s32 set(f32 value, u32 frames);
    s32 move();
    be<f32> mTargetValue;
    be<f32> mCurrentValue;
    be<f32> mMoveAmount;
    be<u32> mMoveCounter;
};
struct MoveParaSetInitHalf : MoveParaSet { MoveParaSetInitHalf(); };
struct MoveParaSetInitZero : MoveParaSet { MoveParaSetInitZero(); };
// HD: the sound-list prefix is followed by a critical section.
struct LinkSound {
    LinkSound();
    void init(JAISound*, u32);
    JAISound* getSound();
    void releaseSound(JAISound*);
    gptr<void> freeStart;
    gptr<void> usedEnd;
    gptr<void> Buffer;
    u8 criticalSection[0x3C];
};
WWHD_OFFSET(LinkSound, Buffer, 8);
WWHD_OFFSET(LinkSound, criticalSection, 12);
WWHD_SIZE(LinkSound, 0x48);
WWHD_OFFSET(MoveParaSet, mTargetValue, 0);
WWHD_OFFSET(MoveParaSet, mCurrentValue, 4);
WWHD_OFFSET(MoveParaSet, mMoveAmount, 8);
WWHD_OFFSET(MoveParaSet, mMoveCounter, 12);
WWHD_SIZE(MoveParaSet, 16);
WWHD_SIZE(MoveParaSetInitHalf, 16);
WWHD_SIZE(MoveParaSetInitZero, 16);
}

void constructLinkSoundCriticalSection(void*);
