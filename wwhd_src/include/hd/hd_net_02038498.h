/** HD NetBossMgr/SpotPass views; private decomp source. */
#pragma once
#include "wwhd.h"
struct hdNetBossManager;
struct hdNetBossVirtualTable {
    u8 _opaque00[0xC]; gfn mDestroy;
    u8 _opaque10[0xC]; gfn mSendMessage;
    u8 _opaque20[4]; gfn mDestroyHeap;
    u8 _opaque28[4]; gfn mStart;
    u8 _opaque30[0x1C]; gfn mStartThread;
};
WWHD_OFFSET(hdNetBossVirtualTable, mStartThread, 0x4C);
struct hdNetBossObject {
    u8 _opaque00[0xC]; gptr<hdNetBossVirtualTable> mVtable;
    u32 destroy(u32 flags) { return gabi::call_ptr<u32>(mVtable->mDestroy.get(), this, flags); }
    u32 start() { return gabi::call_ptr<u32>(mVtable->mStart.get(), this); }
    u32 startThread() { return gabi::call_ptr<u32>(mVtable->mStartThread.get(), this, 0); }
    u32 sendMessage(u32 message, u32 blocking) { return gabi::call_ptr<u32>(mVtable->mSendMessage.get(), this, message, blocking); }
    u32 destroyHeap() { return gabi::call_ptr<u32>(mVtable->mDestroyHeap.get(), this); }
};
struct hdNetBossManager {
    u8 _opaque00[0xC]; gptr<hdNetBossVirtualTable> mDisposerVtable;
    gptr<void> mWorkBuffer; gptr<hdNetBossObject> mHeap;
    be<u32> mState; gptr<hdNetBossObject> mThread; be<u32> mLastResult;
    u32 destroyDisposer(u32 flags) { return gabi::call_ptr<u32>(mDisposerVtable->mDestroy.get(), this, flags); }
};
WWHD_SIZE(hdNetBossManager, 0x24);
WWHD_OFFSET(hdNetBossManager, mState, 0x18);
struct hdNetBossCompanion {
    u8 _opaque00[0x10]; u8 mBase[0x10]; be<u32> mFirstVtable;
    be<u32> mSecondVtable; u8 _opaque28[8]; be<u32> mThirdVtable;
};
struct hdNetBossFlagObject { u8 _opaque00[0xF0]; be<u8> mFlag; u8 _paddingF1[3]; };
WWHD_SIZE(hdNetBossFlagObject, 0xF4);
struct hdNetBossStringTable { u8 _opaque00[0x14]; gfn mAssureTermination; };
struct hdNetBossString {
    gptr<be<u8>> mText; gptr<hdNetBossStringTable> mVtable;
    void assureTermination() { gabi::call_ptr<u32>(mVtable->mAssureTermination.get(), this); }
};
struct hdNetBossFixedString {
    gptr<be<u8>> mText; be<u32> mVtable; be<u32> mCapacity;
    void terminate() {
        // HD: retain wrapped arithmetic for all capacity values.
        *gabi::at<be<u8>>(gabi::ea(mText.get()) + (u32)mCapacity - 1) = 0;
    }
};
struct hdNetBossThreadName { be<u32> mTextAddress; be<u32> mVtable; };
struct hdNetBossDelegate {
    be<u32> mVtable; gptr<hdNetBossManager> mObject;
    be<s16> mThisAdjustment; be<s16> mVirtualIndex; be<u32> mFunction;
};
WWHD_SIZE(hdNetBossDelegate, 0x10);
struct hdNetBossResult { be<u32> mValue; };
struct hdNetBossTaskSetting { be<u32> mStorage[0x488]; };
WWHD_SIZE(hdNetBossTaskSetting, 0x1220);
struct hdNetBossTask { be<u32> mStorage[8]; };
struct hdNetBossSaveParameters { u8 mStorage[0x7C]; };
enum hdNetBossState { NET_BOSS_IDLE, NET_BOSS_REGISTERING, NET_BOSS_THREAD_STARTED };
void hdNetBossDestroyCompanionBase(void*, u32);
void* hdNetBossAllocateFromHeap(u32, void*, u32);
void* hdNetBossAllocateAligned(u32, void*, u32);
void hdNetBossConstructDisposer(hdNetBossManager*, void*, u32);
void hdNetBossDestroyDisposerBase(hdNetBossManager*, u32);
hdNetBossObject* hdNetBossCreateHeap(u32, hdNetBossThreadName*, void*, u32, u32);
hdNetBossObject* hdNetBossConstructThread(hdNetBossObject*, hdNetBossThreadName*, hdNetBossDelegate*, void*, u32, u32, u32, u32, u32);
u32 hdNetBossInitializeLibrary();
u32 hdNetBossGetSaveParameter(hdNetBossSaveParameters*, u32);
hdNetBossSaveParameters* hdNetBossConstructSaveParameters(hdNetBossSaveParameters*);
void hdNetBossReadSaveParameters(void*, hdNetBossSaveParameters*);
void hdNetBossSetTaskParameter(hdNetBossTaskSetting*, u32, u32);
void hdNetBossConstructTaskSetting(hdNetBossTaskSetting*);
void hdNetBossInitializeTaskSetting(hdNetBossTaskSetting*, void*, u32);
void hdNetBossConstructTask(hdNetBossTask*);
u32 hdNetBossInitializeTask(hdNetBossTask*, const void*, u32);
u32 hdNetBossIsTaskRegistered(hdNetBossTask*);
u32 hdNetBossUnregisterTask(hdNetBossTask*);
u32 hdNetBossRegisterTask(hdNetBossTask*, hdNetBossTaskSetting*);
u32 hdNetBossStartScheduling(hdNetBossTask*, u32);
void hdNetBossDestroyTask(hdNetBossTask*, u32);
void hdNetBossDestroyTaskSetting(hdNetBossTaskSetting*, u32);
