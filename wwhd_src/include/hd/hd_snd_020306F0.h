/** HD sound-interface layouts; private decomp source. */
#pragma once
#include "wwhd.h"
struct hdSoundInterfaceSe;
struct hdSoundInterfaceArchive;
struct hdSoundInterfaceJAudio;
struct hdSoundInterfaceFader;
struct hdSoundInterfaceHandle { gptr<void> mSound; };
struct hdSoundInterfaceBridge { u8 _opaque00[0x38]; be<u8> field_0x38; }; // HD: flag purpose unconfirmed.
struct hdSoundInterfaceBgm { u8 _opaque00[0x5C]; gptr<hdSoundInterfaceBridge> mBridge; };
struct hdSoundInterfacePlayer {
    u8 _opaque00[0x1F4]; be<u8> mMenuFlag;
    be<u8> field_0x1f5; // HD: independently toggled flags; meaning unconfirmed.
    be<u8> field_0x1f6;
    be<u8> field_0x1f7;
    be<u8> field_0x1f8;
    u8 _padding1f9[3]; be<u32> mMode;
};
WWHD_OFFSET(hdSoundInterfacePlayer, mMode, 0x1FC);
struct hdSoundInterfaceHeap {
    u8 _opaque00[4]; be<u32> mVtable; u8 _opaque08[0x3C];
    u8 mFirstMember[0x60]; u8 mSecondMember[0x58]; u8 mTrailingMember[4];
};
WWHD_OFFSET(hdSoundInterfaceHeap, mTrailingMember, 0xFC);
enum hdSoundOutputMode { SOUND_OUTPUT_TV, SOUND_OUTPUT_DRC, SOUND_OUTPUT_BOTH };
void hdInterfaceDestroyTrailing(void*, u32);
void hdInterfaceDestroyMember(void*, u32);
hdSoundInterfaceSe* hdInterfaceGetSe(void*);
hdSoundInterfaceBgm* hdInterfaceGetBgm(void*);
hdSoundInterfacePlayer* hdInterfaceGetPlayer(void*);
hdSoundInterfaceArchive* hdInterfaceGetArchive(void*);
void hdInterfaceFadeBgm(hdSoundInterfaceBgm*, u32, f32, f32);
void hdInterfaceFadePlayer(hdSoundInterfacePlayer*, u32, f32, f32);
void hdInterfaceStartSe(hdSoundInterfaceSe*, const char*);
hdSoundInterfaceHandle* hdInterfacePrepareSe(hdSoundInterfaceSe*, const char*);
void hdInterfaceSetVolume(void*, u32, f32);
void hdInterfaceSetPitch(void*, f32);
void hdInterfaceDisarmSe(hdSoundInterfaceBgm*);
void hdInterfaceMenuIn(hdSoundInterfaceJAudio*);
void hdInterfaceMenuOut(hdSoundInterfaceJAudio*);
void hdInterfaceLoadStatic(hdSoundInterfaceArchive*);
void hdInterfaceLoadBgmWds(hdSoundInterfaceArchive*);
u32 hdInterfaceQueryFirst(hdSoundInterfaceJAudio*);
u32 hdInterfaceQuerySecond(hdSoundInterfaceJAudio*);
void hdInterfaceUnpausePlayer(hdSoundInterfacePlayer*, u32);
void hdInterfaceUnpauseBgm(hdSoundInterfaceBgm*, u32);
void hdInterfacePausePlayer(hdSoundInterfacePlayer*, u32);
void hdInterfacePauseBgm(hdSoundInterfaceBgm*, u32);
void hdInterfaceResumePlayer(hdSoundInterfacePlayer*, u32);
void hdInterfaceResumeBgm(hdSoundInterfaceBgm*, u32);
void hdInterfaceArmSe(hdSoundInterfaceBgm*);
u32 hdInterfaceInitializeFader(hdSoundInterfaceFader*, f32);
u32 hdInterfaceStreamStart0(hdSoundInterfacePlayer*);
u32 hdInterfaceStreamStart1(hdSoundInterfacePlayer*);
u32 hdInterfaceStreamStart2(hdSoundInterfacePlayer*);
u32 hdInterfaceStreamStart3(hdSoundInterfacePlayer*);
u32 hdInterfaceStreamIsPlaying0(hdSoundInterfacePlayer*);
u32 hdInterfaceStreamIsPlaying1(hdSoundInterfacePlayer*);
u32 hdInterfaceStreamIsPlaying2(hdSoundInterfacePlayer*);
u32 hdInterfaceStreamIsPlaying3(hdSoundInterfacePlayer*);
u32 hdInterfaceStreamPause0(hdSoundInterfacePlayer*, u32, u32);
u32 hdInterfaceStreamPause1(hdSoundInterfacePlayer*, u32, u32);
u32 hdInterfaceStreamPause2(hdSoundInterfacePlayer*, u32, u32);
u32 hdInterfaceStreamPause3(hdSoundInterfacePlayer*, u32, u32);
