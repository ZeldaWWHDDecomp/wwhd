/** HD BGM controller and SE-player views; private decomp source. */
#pragma once
#include "wwhd.h"
struct hdBgmFader { be<f32> mVolume; u8 _opaque04[0x14]; };
WWHD_SIZE(hdBgmFader, 0x18);
struct hdBgmDestructorTable { u8 _opaque00[0xC]; gfn mDestroy;
    void destroy(void* object) const { gabi::call_ptr(mDestroy.get(), object, 3); }
};
struct hdBgmJAudio { u8 _opaque00[0x2C]; gptr<hdBgmDestructorTable> mVtable; be<u8> mPaused; u8 _opaque31[0x21C3]; };
WWHD_SIZE(hdBgmJAudio, 0x21F4);
struct hdBgmBridge { gptr<hdBgmDestructorTable> mVtable; u8 _opaque04[0x40]; };
WWHD_SIZE(hdBgmBridge, 0x44);
struct hdBgmController {
    be<u32> mVtable;
    be<u32> field_0x4; // HD: zeroed bookkeeping; role unconfirmed.
    be<u32> field_0x8; // HD: zeroed bookkeeping; role unconfirmed.
    gptr<hdBgmJAudio> mJAudio;
    be<u8> mInitialized; u8 _padding11[3];
    hdBgmFader mOutputFaders[2];
    hdBgmFader mBgmFader;
    gptr<hdBgmBridge> mBridge;
    be<u8> mOutputOff; u8 _padding61[3];
    be<u32> mPaused;
    be<s32> mDelayedSeTimer;
    be<u32>* pauseForConstruction() {
        // HD: the constructor tests the wrapped embedded address, including NULL.
        return gabi::at<be<u32>>(gabi::ea(this) + offsetof(hdBgmController, mPaused));
    }
};
WWHD_OFFSET(hdBgmController, mBgmFader, 0x44);
WWHD_OFFSET(hdBgmController, mPaused, 0x64);
WWHD_SIZE(hdBgmController, 0x6C);
struct hdBgmNwPlayer { u8 _opaque00[0xC]; gptr<hdBgmDestructorTable> mVtable; };
struct hdBgmSePlayer { gptr<hdBgmNwPlayer> mPlayer; };
struct hdBgmSafeString { gptr<const char> mText; be<u32> mVtable; };
struct hdBgmOutput;
void hdBgmConstructArray(void*, u32, u32, u32);
u32 hdBgmFade(hdBgmFader*, u32, f32);
void hdBgmInitializeFader(hdBgmFader*, f32);
void hdBgmUpdateFader(hdBgmFader*);
hdBgmJAudio* hdBgmConstructJAudio(hdBgmJAudio*);
hdBgmBridge* hdBgmConstructBridge(hdBgmBridge*);
void hdBgmInitializeJAudio(hdBgmJAudio*, u32, u32);
void hdBgmAttachBridge(hdBgmJAudio*, hdBgmBridge*);
void hdBgmSetOutputVolumes(hdBgmOutput*, f32, f32);
void hdBgmUpdateJAudio(hdBgmJAudio*);
void hdBgmStartDelayedSe(hdBgmJAudio*, u32, u32, u32, u32, u32, f32, f32, f32, f32);
hdBgmNwPlayer* hdBgmConstructNwPlayer(hdBgmNwPlayer*, u32, u32);
u32 hdBgmStopAllSounds(hdBgmNwPlayer*, u32);
void* hdBgmAllocateSoundHandle(hdBgmNwPlayer*, u32, u32);
u32 hdBgmGetSoundArchive();
u32 hdBgmBindSound(void*, void*, u32, u32);
u32 hdBgmBindPreparedSound(void*, void*, u32, u32);
u32 hdBgmLookupSoundName(const char*);
inline void* hdBgmArchiveInterface(u32 archive) {
    // HD: addition wraps even when the archive getter returns NULL.
    return gabi::at<void>(archive + 4);
}
