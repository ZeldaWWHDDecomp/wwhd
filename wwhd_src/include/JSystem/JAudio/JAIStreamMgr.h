/* Cafe stream control retains the interface but removes GC stream processing. */
#pragma once
#include "JSystem/JAudio/JAIBasic.h"
#include "JSystem/JAudio/JAISound.h"
#include "JSystem/JAudio/JAISoundParams.h"
namespace JAInter {
struct StreamUpdateData {
    be<u8> mState,mReadState,mPrepareFlag,mReserved;
    be<f32> mVolume,mPitch,mPan;
    be<u32> mActiveFlags;
    gptr<JAISound> mSound;
};
WWHD_OFFSET(StreamUpdateData,mActiveFlags,0x10);
WWHD_SIZE(StreamUpdateData,0x18);
struct StreamParameter {
    be<u8> mPauseMode;
    u8 reserved[3];
    be<u32> mUnknown,mVolumeFlags,mPitchFlags,mPanFlags;
    MoveParaSet mVolumes[20];
    gptr<MoveParaSet> mPitches,mPans;
    gptr<StreamUpdateData> mUpdateData;
};
WWHD_OFFSET(StreamParameter,mVolumes,0x14);
WWHD_OFFSET(StreamParameter,mPitches,0x154);
WWHD_OFFSET(StreamParameter,mPans,0x158);
WWHD_OFFSET(StreamParameter,mUpdateData,0x15C);
WWHD_SIZE(StreamParameter,0x160);
namespace StreamMgr {
void init();
void storeStreamBuffer(gptr<JAISound>*,Actor*,u32,u32,u8,JAISoundInfoView*);
void releaseStreamBuffer(JAISound*,u32);
void processGFrameStream();
}}
void streamConstructCritical(void*);
void streamDestroyCritical(void*,s32);
void streamDelete(void*);
void streamRegister(void*);
