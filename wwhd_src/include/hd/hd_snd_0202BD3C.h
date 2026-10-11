/** HD sound manager and handle-holder layouts; private decomp source. */
#pragma once
#include "wwhd.h"
struct hdSoundManagerContainer {
    gptr<void> mSubsystems[6];
};
WWHD_SIZE(hdSoundManagerContainer, 0x18);
struct hdSoundManagerPlayer {
    u8 mStorage[0x24];
};
struct hdSoundManager {
    u8 _padding00[0xC];
    be<u32> mDisposerVtable;
    gptr<hdSoundManagerContainer> mContainer;
    gptr<hdSoundManagerPlayer> mPlayer;
};
WWHD_OFFSET(hdSoundManager, mDisposerVtable, 0xC);
WWHD_OFFSET(hdSoundManager, mContainer, 0x10);
WWHD_OFFSET(hdSoundManager, mPlayer, 0x14);
WWHD_SIZE(hdSoundManager, 0x18);
struct hdSoundHandleHolder {
    be<u32> mVtable;
    gptr<void> mHandle;
    be<u32> field_0x8; // HD: constructor zeroes this bookkeeping word; role unconfirmed.
    be<s32> mId;
    be<u32> field_0x10; // HD: constructor zeroes this bookkeeping word; role unconfirmed.
};
WWHD_OFFSET(hdSoundHandleHolder, mHandle, 4);
WWHD_OFFSET(hdSoundHandleHolder, mId, 0xC);
WWHD_SIZE(hdSoundHandleHolder, 0x14);
hdSoundManagerContainer* hdSoundManagerConstructContainer(hdSoundManagerContainer*, void*);
void hdSoundManagerDestroyContainer(hdSoundManagerContainer*, s32);
hdSoundManagerPlayer* hdSoundManagerConstructPlayer(hdSoundManagerPlayer*);
void* hdSoundManagerGetFirstSubsystem(hdSoundManagerContainer*);
void* hdSoundManagerGetSecondSubsystem(hdSoundManagerContainer*);
void* hdSoundManagerGetThirdSubsystem(hdSoundManagerContainer*);
void* hdSoundManagerGetFourthSubsystem(hdSoundManagerContainer*);
void* hdSoundManagerGetFifthSubsystem(hdSoundManagerContainer*);
void* hdSoundManagerGetSixthSubsystem(hdSoundManagerContainer*);
void hdSoundManagerSetFirstSubsystem(hdSoundManagerPlayer*, void*);
void hdSoundManagerSetSecondSubsystem(hdSoundManagerPlayer*, void*);
void hdSoundManagerSetThirdSubsystem(hdSoundManagerPlayer*, void*);
void hdSoundManagerSetLastSubsystem(hdSoundManagerPlayer*, void*);
void* hdSoundManagerAllocate(u32, void*, u32);
void hdSoundManagerConstructDisposer(hdSoundManager*, void*, u32);
void hdSoundManagerDestroyDisposer(hdSoundManager*, u32);
u32 hdSoundManagerConnectResult(hdSoundManager*);
void hdSoundManagerBuildPaths(u32);
u32 hdSoundHandleStop(void*, u32, u32);
u32 hdSoundHandlePause(void*, u32, u32);
u32 hdSoundHandleFirstForward(void*, u32, u32);
u32 hdSoundHandleSecondForward(void*, u32, u32);
void hdSoundHandleStopForDestroy(hdSoundHandleHolder*, u32);
void hdSoundHandleDestroy(gptr<void>*);
