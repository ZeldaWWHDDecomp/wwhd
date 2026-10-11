/** HD sound archive player, faders and stream views; private decomp source. */
#pragma once
#include "wwhd.h"
struct hdPlayerFader { be<f32> mVolume; u8 _opaque04[0x14]; };
WWHD_SIZE(hdPlayerFader, 0x18);
struct hdPlayerArchiveInterface { be<u32> mVtable; u8 _opaque04[0x14]; be<u32> mPlayerCount; u8 _opaque1C[0x100]; };
WWHD_SIZE(hdPlayerArchiveInterface, 0x11C);
struct hdPlayerSoundTable { u8 _opaque00[0x2C]; gfn mIsPlaying; };
struct hdPlayerSoundInfo { be<u32> mId; be<u32> mHandleId; };
struct hdPlayerSound {
    u8 _opaque00[0xFC]; be<u32> mStarted;
    u8 _opaque100[0x168]; gptr<hdPlayerSoundInfo> mInfo;
    u8 _opaque26C[0x28]; gptr<hdPlayerSoundTable> mVtable;
    u32 isPlaying() { return gabi::call_ptr<u32>(mVtable->mIsPlaying.get(), this); }
};
WWHD_OFFSET(hdPlayerSound, mInfo, 0x268);
WWHD_OFFSET(hdPlayerSound, mVtable, 0x294);
struct hdPlayerSoundHandle { gptr<hdPlayerSound> mSound; };
struct hdSoundPlayer {
    be<u32> mVtable; hdPlayerArchiveInterface mArchiveInterface;
    be<u32> mSecondaryVtable; u8 _opaque124[0x70];
    hdPlayerFader mMasterFaders[2]; hdPlayerFader mBgmFaders[2];
    be<u8> mMenuFlag;
    be<u8> field_0x1f5; // HD: independent mode flags; purpose unconfirmed.
    be<u8> field_0x1f6;
    be<u8> field_0x1f7;
    be<u8> field_0x1f8;
    u8 _padding1F9[3]; be<u32> mMode; be<u32> mPaused;
    hdPlayerSoundHandle mStreamHandle; hdPlayerSoundHandle mStaffRollHandle;
    be<u32>* pauseForConstruction() {
        // HD: this == -0x200 makes the embedded pause address wrap to NULL.
        return gabi::at<be<u32>>(gabi::ea(this) + offsetof(hdSoundPlayer, mPaused));
    }
};
WWHD_SIZE(hdSoundPlayer, 0x20C);
WWHD_OFFSET(hdSoundPlayer, mMasterFaders, 0x194);
WWHD_OFFSET(hdSoundPlayer, mBgmFaders, 0x1C4);
WWHD_OFFSET(hdSoundPlayer, mStreamHandle, 0x204);
struct hdPlayerControllerType { be<u32> mNameAddress; };
struct hdPlayerControllerTable { u8 _opaque00[0xC]; gfn mCheckType; };
struct hdPlayerController {
    u8 _opaque00[0x10]; gptr<hdPlayerControllerTable> mVtable;
    u8 _opaque14[0xA0]; be<u8> mDistance; u8 _opaqueB5[0xA1F]; be<s32> mActive;
    u32 checkType(hdPlayerControllerType* type) { return gabi::call_ptr<u32>(mVtable->mCheckType.get(), this, type); }
};
WWHD_OFFSET(hdPlayerController, mActive, 0xAD4);
struct hdPlayerListNode { gptr<hdPlayerListNode> mNext; };
struct hdPlayerSubPlayer {
    u8 _opaque00[4]; hdPlayerListNode mSoundList;
    hdPlayerListNode* soundListHead() {
        // HD: the embedded sentinel can wrap to guest address zero.
        return gabi::at<hdPlayerListNode>(gabi::ea(this) + offsetof(hdPlayerSubPlayer, mSoundList));
    }
};
enum hdPlayerChannel { PLAYER_CHANNEL_TV, PLAYER_CHANNEL_DRC };
static constexpr u32 PLAYER_ID_BASE = 0x04000000;
static constexpr u32 PLAYER_CHANNEL_MASK = 0x21; // HD: original NW4F channel-selection mask.
enum hdPlayerScanCallback { PLAYER_SCAN_MATCH = 0x0202FC44, PLAYER_SCAN_STOP = 0x0202FE78 };
struct hdPlayerStringTable { u8 _opaque00[0x14]; gfn mAssureTermination; };
struct hdPlayerSafeString {
    gptr<const be<u8>> mText; gptr<hdPlayerStringTable> mVtable;
    void assureTermination() { gabi::call_ptr(mVtable->mAssureTermination.get(), this); }
};
struct hdPlayerFixedString {
    gptr<be<u8>> mText; be<u32> mVtable; be<u32> mCapacity;
    void terminate() {
        // HD: retain the wrapped last-byte address for every capacity.
        *gabi::at<be<u8>>(gabi::ea(mText.get()) + (u32)mCapacity - 1) = 0;
    }
};
struct hdPlayerOutputVolume { u8 mStorage[4]; };
struct hdPlayerHeaderStatics { be<u32> mWords[4]; };
inline u32 hdPlayerReadInfoId(hdPlayerSoundInfo* info) {
    // HD: the original reads guest address zero when a sound has no info.
    return *gabi::at<be<u32>>(gabi::ea(info));
}
inline u32 hdPlayerReadInfoHandle(hdPlayerSoundInfo* info) {
    // HD: info +4 is read even when info is NULL.
    return *gabi::at<be<u32>>(gabi::ea(info) + offsetof(hdPlayerSoundInfo, mHandleId));
}
inline hdPlayerSound* hdPlayerSoundFromNode(hdPlayerListNode* node) {
    // HD: intrusive sound-list nodes live at sound +0x27C; subtraction wraps.
    return gabi::at<hdPlayerSound>(gabi::ea(node) - 0x27C);
}
inline bool hdPlayerStringEquals(const be<u8>* first, const be<u8>* second) {
    for (u32 i = 0; i < 0x40001; ++i) {
        // HD: bounded SafeString comparison with guest address wrapping.
        u8 a = *gabi::at<const be<u8>>(gabi::ea(first) + i);
        u8 b = *gabi::at<const be<u8>>(gabi::ea(second) + i);
        if (a != b) return false;
        if (a == 0) return true;
    }
    return false;
}
inline f32 hdPlayerDivide(f32 first, f32 second) {
    // HD: fdivs keeps the first NaN payload, as the original PowerPC instruction does.
    if (first != first) return gabi::ppc_qnan(first);
    if (second != second) return gabi::ppc_qnan(second);
    return (f32)((f64)first / (f64)second);
}
using hdPlayerFaderConstructor = u32 (*)(hdPlayerFader*);
void hdPlayerDestroyHandle(hdPlayerSoundHandle*);
void hdPlayerDestroyBase(hdSoundPlayer*, u32);
u32 hdPlayerForwardSecondary(hdSoundPlayer*, u32);
void hdPlayerConstructBase(hdSoundPlayer*);
void hdPlayerConstructFaderArray(void*, u32, u32, hdPlayerFaderConstructor);
u32 hdPlayerInitializeFader(hdPlayerFader*, f32);
u32 hdPlayerFade(hdPlayerFader*, u32, f32);
void hdPlayerResetBase(hdSoundPlayer*);
void hdPlayerConstructOutputVolume(hdPlayerOutputVolume*);
void hdPlayerSetOutputVolume(hdPlayerOutputVolume*, u32, f32);
void* hdPlayerGetManagerHeap();
s32 hdPlayerGetHeapState(void*);
void hdPlayerSetHeapState(void*, u32);
u32 hdPlayerForwardBase(hdSoundPlayer*, u32, u32);
hdPlayerController* hdPlayerGetController(void*, u32);
void hdPlayerUpdateFader(hdPlayerFader*);
void* hdPlayerGetNamedPlayer(hdPlayerArchiveInterface*, const char*);
void hdPlayerSetPlayerChannels(void*, u32);
void hdPlayerSetChannelVolume(void*, u32, f32);
void hdPlayerSetPlayerVolume(void*, f32);
void hdPlayerUpdateBase(hdSoundPlayer*);
const be<u8>* hdPlayerGetPlayerName(u32);
hdPlayerSubPlayer* hdPlayerGetPlayerById(hdPlayerArchiveInterface*, u32);
void hdPlayerSetPlayerPaused(void*, u32, u32);
u32 hdPlayerPauseAllResult(hdSoundPlayer*, u32);
u32 hdPlayerResumeAllResult(hdSoundPlayer*, u32);
void hdPlayerAttachHandle(hdPlayerSoundHandle*, hdPlayerSound*);
u32 hdPlayerStopSound(hdPlayerSound*, u32);
u32 hdPlayerStartStream(hdPlayerArchiveInterface*, hdPlayerSoundHandle*, const char*, u32);
u32 hdPlayerPauseStream(hdPlayerSound*, u32, u32);
void hdPlayerConstructHeaderByte(be<u8>*);
void hdPlayerDestroyHeaderByte(be<u8>*);
