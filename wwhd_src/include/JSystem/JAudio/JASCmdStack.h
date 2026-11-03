/* JAudio port commands; HD uses JSU links and serializes access with a mutex. */
#pragma once
#include "JSystem/JSupport/JSUList.h"
namespace JASystem { class TTrack; namespace Kernel {
struct TPortArgs {
    union {
    struct {
    gptr<TTrack> mTrack;
    be<u32> mFlags;
    be<f32> mTrackVolume,mTrackPitch,mTrackPan,mTrackFxmix,mTrackDolby;
    be<f32> mAuxMix;
    be<u32> mInterruptData;
    be<f32> mTrackTempo;
    };
    be<f32> mValues[10];
    };
};
WWHD_OFFSET(TPortArgs,mFlags,4);
WWHD_OFFSET(TPortArgs,mTrackTempo,0x24);
WWHD_SIZE(TPortArgs,0x28);
using TPortFunc=void(*)(TPortArgs*);
struct TPortCmd : JSUPtrLink {
    be<u32> mFunc;
    gptr<TPortArgs> mArgs;
    void destroy(s32 flags);
    bool setPortCmd(u32 func, TPortArgs* args);
    bool setPortCmd(TPortFunc func, TPortArgs* args) { return setPortCmd(gabi::guest_fn(reinterpret_cast<const void*>(func)),args); }
    u8 addPortCmdOnce();
};
void portCmdProcOnce(JSUPtrList*);
void portCmdProcStay(JSUPtrList*);
void portCmdMain();
WWHD_OFFSET(TPortCmd,mFunc,0x10);
WWHD_OFFSET(TPortCmd,mArgs,0x14);
WWHD_SIZE(TPortCmd,0x18);
}}

void portLinkDestroy(JSUPtrLink*,s32);
void portDelete(void*);
void portLock(void*);
void portUnlock(void*);
void portMutexConstruct(void*);
void portRegister(void*);

u32 portListAppend(JSUPtrList*,JSUPtrLink*);
u32 portListRemove(JSUPtrList*,JSUPtrLink*);
void portListInitiate(JSUPtrList*);
void portListDestroy(JSUPtrList*,s32);
