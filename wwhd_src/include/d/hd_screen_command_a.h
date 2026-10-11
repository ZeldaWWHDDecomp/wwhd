/** HD HUD command, key, heart and icon state views. No GameCube counterpart.
 * Shared only by this unit's six source files; private decomp source. */
#pragma once
#include "wwhd.h"
#include "d/d_com_inf_game.h"
struct hdHudScreenVtable {
    u8 _padding00[0x8C];
    gfn mUpdate;
    u8 _padding90[0x24];
    gfn mCommandUpdate;
    u8 _paddingB8[0x94];
    gfn mCanShow;
};
WWHD_OFFSET(hdHudScreenVtable, mCommandUpdate, 0xB4);
WWHD_OFFSET(hdHudScreenVtable, mCanShow, 0x14C);
struct hdHudStateMachine { u8 mStorage[4]; };
struct hdHudStateDescriptor { u8 mStorage[4]; };
struct hdHudScreenCore {
    u8 _padding00[4];
    gptr<hdHudScreenVtable> mVtable;
    u8 _padding08[0x10];
    hdHudStateMachine mStateMachine;
    void update() { gabi::call_ptr<void>(mVtable->mUpdate.get(), this); }
    void updateCommand() { gabi::call_ptr<void>(mVtable->mCommandUpdate.get(), this); }
    u32 canShow() { return gabi::call_ptr<u32>(mVtable->mCanShow.get(), this); }
};
WWHD_OFFSET(hdHudScreenCore, mStateMachine, 0x18);
WWHD_SIZE(hdHudScreenCore, 0x1C);
struct hdHudCommandA : hdHudScreenCore {
    u8 _padding1C[0x54];
    be<u32> mWaitTimer;
    be<u8> mEnabled;
};
WWHD_OFFSET(hdHudCommandA, mWaitTimer, 0x70);
WWHD_OFFSET(hdHudCommandA, mEnabled, 0x74);
struct hdHudCommandGuide : hdHudScreenCore {
    u8 _padding1C[0xB4];
    be<u32> mWaitTimer;
    u8 _paddingD4[8];
    be<u8> mCachedFlag;
    be<u8> mCachedCanShow;
};
WWHD_OFFSET(hdHudCommandGuide, mWaitTimer, 0xD0);
WWHD_OFFSET(hdHudCommandGuide, mCachedFlag, 0xDC);
struct hdHudInputEvent {
    be<u32> mType;
    u8 mPayload[36];
};
WWHD_SIZE(hdHudInputEvent, 40);
struct hdHudEventManager {
    u8 _padding00[0x30];
    hdHudInputEvent mCurrentEvent;
};
WWHD_OFFSET(hdHudEventManager, mCurrentEvent, 0x30);
struct hdHudWindControl : hdHudScreenCore {
    u8 _padding1C[0x34];
    be<u8> mWaitTimer;
    u8 _padding51[3];
    be<u8> mReady;
    be<u8> mForced;
};
WWHD_OFFSET(hdHudWindControl, mWaitTimer, 0x50);
WWHD_OFFSET(hdHudWindControl, mReady, 0x54);
struct hdHudAnimationOwner {
    u8 _padding00[0xD4];
    gptr<void> mAnimation;
};
WWHD_OFFSET(hdHudAnimationOwner, mAnimation, 0xD4);
struct hdHudDungeonKey : hdHudScreenCore {
    u8 _padding1C[0x28];
    gptr<hdHudAnimationOwner> mAnimationOwner;
    u8 _padding48[8];
    be<u32> mSparkleActive[2];
    u8 _padding58[4];
    be<u32> mSparkleTimer;
    be<u32> mMinimumInterval;
    be<u32> mRandomInterval;
    void* animation() { return mAnimationOwner->mAnimation; }
};
WWHD_OFFSET(hdHudDungeonKey, mAnimationOwner, 0x44);
WWHD_OFFSET(hdHudDungeonKey, mSparkleActive, 0x50);
WWHD_OFFSET(hdHudDungeonKey, mSparkleTimer, 0x5C);
WWHD_OFFSET(hdHudDungeonKey, mRandomInterval, 0x64);
struct hdHudKeyIndices { be<u32> mIndices[2]; };
WWHD_SIZE(hdHudKeyIndices, 8);
using hdHudKeyAnimationIds = be<u32>[3];
struct hdHudHeartChild {
    u8 _padding00[0x58];
    be<u32> mNextState;
};
WWHD_OFFSET(hdHudHeartChild, mNextState, 0x58);
struct hdHudHeart : hdHudScreenCore {
    u8 _padding1C[0x2C];
    be<u32> mHeartCount;
    gptr<gptr<hdHudHeartChild>> mHearts;
    u8 _padding50[0xA0];
    be<u32> mRefillTimer;
    be<u32> mShownLife;
    be<u32> mShownContainers;
    be<u32> mSelectedHeart;
    hdHudHeartChild* heart(u32 index) {
        gptr<hdHudHeartChild>* hearts = mHearts;
        if (index < mHeartCount) {
            // HD: array selection retains guest-address wrapping for arbitrary indices.
            hearts = gabi::at<gptr<hdHudHeartChild>>(gabi::ea(hearts) + index * 4);
        }
        return *hearts;
    }
};
WWHD_OFFSET(hdHudHeart, mHeartCount, 0x48);
WWHD_OFFSET(hdHudHeart, mRefillTimer, 0xF0);
WWHD_OFFSET(hdHudHeart, mSelectedHeart, 0xFC);
struct hdHudSaveInfo {
    u8 _padding00[0x20];
    dSv_player_status_a_c mStatusA;
};
WWHD_OFFSET(hdHudSaveInfo, mStatusA, 0x20);
struct hdHudLoopBossIcon : hdHudScreenCore {
    u8 _padding1C[0x28];
    gptr<hdHudAnimationOwner> mAnimationOwner;
    u8 _padding48[8];
    be<u32> mTimers[2];
    void* animation() { return mAnimationOwner->mAnimation; }
};
WWHD_OFFSET(hdHudLoopBossIcon, mTimers, 0x50);
void hdHudChangeState(hdHudStateMachine*, hdHudStateDescriptor*);
void hdHudCopyInputEvent(hdHudInputEvent*, hdHudInputEvent*);
u32 hdHudIsWindEvent(u32);
void hdHudStartAnimation(void*, u32, u32, f32);
u32 hdHudAnimationFinished(void*, u32);
u32 hdHudGetHeartValue(hdHudHeart*, u32);
void hdHudSetHeartValue(hdHudHeartChild*, u32);
void hdHudSetHeartVisible(hdHudHeartChild*, u32);
