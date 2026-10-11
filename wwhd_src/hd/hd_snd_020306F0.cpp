/** HD free sound interface. No GameCube counterpart. */
#include "bindings.h"
#include "hd/hd_snd_020306F0.h"
static void* sSoundManager;
static hdSoundInterfaceJAudio* sJAudio;
static f32 sOne;
static f32 sZero;
static f32 sFaderOne;
static u32 sHeapTypeInitialized;
static u32 sParentTypeInitialized;
static u32 sHeapType;
static u32 sParentType;
/* 020306F0 */
static u32 isSoundHeapDerived(void* self, void* ti) {
    (void)self;
    if (sHeapTypeInitialized == 0) {
        sHeapTypeInitialized = 1;
        sHeapType = 0x10004768;
    }
    if (ti == &sHeapType) return 1;
    if (sParentTypeInitialized == 0) {
        sParentTypeInitialized = 1;
        sParentType = 0x10004758;
    }
    return ti == &sParentType ? 1 : 0;
}

/* 02030768 */
static void destroySoundHeap(hdSoundInterfaceHeap* self, s32 flags) {
    if (self == NULL) return;
    self->mVtable = 0x1015EBB8;
    hdInterfaceDestroyTrailing(self->mTrailingMember, 2);
    hdInterfaceDestroyMember(self->mSecondMember, 2);
    hdInterfaceDestroyMember(self->mFirstMember, 2);
    if (flags & 1) operator_delete(self);
}

/* 020307E0 */
static hdSoundInterfaceSe* getSePlayer() {
    return hdInterfaceGetSe(sSoundManager);
}

/* 020307EC */
static hdSoundInterfaceBgm* getBgmController() {
    return hdInterfaceGetBgm(sSoundManager);
}

/* 020307F8 */
static hdSoundInterfacePlayer* getSoundPlayer() {
    return hdInterfaceGetPlayer(sSoundManager);
}

/* 0203103C */
static hdSoundInterfaceArchive* getSoundArchive() {
    return hdInterfaceGetArchive(sSoundManager);
}

static inline hdSoundInterfaceSe* se() { return getSePlayer(); }
static inline hdSoundInterfaceBgm* bgm() { return getBgmController(); }
static inline hdSoundInterfacePlayer* player() { return getSoundPlayer(); }

/* fade the BGM controller and the player's master faders (FRAMES, TV, DRC) */
static inline void fade_both(u32 frames, f32 tv, f32 drc) {
    hdInterfaceFadeBgm(bgm(), frames, tv, drc);
    hdInterfaceFadePlayer(player(), frames, tv, drc);
}

// HD: optional game-mode SE, then fade both outputs over three frames.
/* 02030804 */
static void switchSoundGameMode(u32 withSe) {
    if (withSe != 0) hdInterfaceStartSe(se(), "SE_GAME_MODE_SWITCH");
    f32 zero = sZero;
    fade_both(3, zero, zero);
}

// HD: switch TV/GamePad output over five frames.
/* 02030880 */
static void fadeSoundOutputMode(u32 mode) {
    f32 one = sOne, zero = sZero;
    if (mode == SOUND_OUTPUT_TV) fade_both(5, one, zero);
    else if (mode == SOUND_OUTPUT_DRC) fade_both(5, zero, one);
    else if (mode == SOUND_OUTPUT_BOTH) fade_both(5, one, one);
}

// HD: switch TV/GamePad output immediately.
/* 020309AC */
static void setSoundOutputMode(u32 mode) {
    f32 one = sOne, zero = sZero;
    if (mode == SOUND_OUTPUT_TV) fade_both(0, one, zero);
    else if (mode == SOUND_OUTPUT_DRC) fade_both(0, zero, one);
    else if (mode == SOUND_OUTPUT_BOTH) fade_both(0, one, one);
}

// HD: only the two existing player modes are accepted.
/* 02030AD8 */
static void setPlayerMode(u32 mode) {
    if (mode > 1) return;
    player()->mMode = mode == 0 ? 0 : 1;
}

/* 02030B38 */
static void playSe(const char* name) {
    hdInterfaceStartSe(se(), name);
}

// HD: prepare the named SE and adjust its volume and pitch.
/* 02030B6C */
static void setSeParams(const char* name, f32 a, f32 b) {
    hdSoundInterfaceHandle* h = hdInterfacePrepareSe(se(), name);
    if (h == NULL || h->mSound == NULL) return;
    hdInterfaceSetVolume(h->mSound, 0, a);
    if (h->mSound == NULL) return;
    hdInterfaceSetPitch(h->mSound, b);
}

/* 02030C10 */
static void bgmUpdate() {
    hdInterfaceDisarmSe(bgm());
}

/* 02030C34 */
static void menuIn() {
    hdSoundInterfaceJAudio* b = sJAudio;
    if (b == NULL) return;
    hdInterfaceMenuIn(b);
    player()->mMenuFlag = 1;
}

/* 02030C70 */
static void menuOut() {
    hdSoundInterfaceJAudio* b = sJAudio;
    if (b == NULL) return;
    hdInterfaceMenuOut(b);
    player()->mMenuFlag = 0;
}

/* 02030CAC */
static u32 getMenuFlag() {
    return player()->mMenuFlag;
}

/* 02030CD0 */
static void setPlayerFlagF5() {
    player()->field_0x1f5 = 1;
}

/* 02030CF8 */
static void clearPlayerFlagF5() {
    player()->field_0x1f5 = 0;
}

/* 02030D20 */
static u32 getPlayerFlagF5() {
    return player()->field_0x1f5;
}

/* 02030D44 */
static void setPlayerFlagF6() {
    player()->field_0x1f6 = 1;
}

/* 02030D6C */
static void clearPlayerFlagF6() {
    player()->field_0x1f6 = 0;
}

/* 02030D94 */
static void setPlayerFlagF7() {
    player()->field_0x1f7 = 1;
    bgmUpdate();
}

/* 02030DC0 */
static void clearPlayerFlagF7() {
    player()->field_0x1f7 = 0;
}

/* 02030DE8 */
static void setPlayerFlagF8() {
    player()->field_0x1f8 = 1;
}

/* 02030E10 */
static void clearPlayerFlagF8() {
    player()->field_0x1f8 = 0;
}

/* 02030E38 */
static void setBridgeFlag() {
    bgm()->mBridge->field_0x38 = 1;
}

/* 02030E64 */
static void clearBridgeFlag() {
    bgm()->mBridge->field_0x38 = 0;
}

/* 02030E90 */
static u32 streamStart(u32 i) {
    switch (i) {
    case 0: return hdInterfaceStreamStart0(player());
    case 1: return hdInterfaceStreamStart1(player());
    case 2: return hdInterfaceStreamStart2(player());
    case 3: return hdInterfaceStreamStart3(player());
    default: return 0;
    }
}

/* 02030F1C */
static u32 streamIsPlaying(u32 i) {
    switch (i) {
    case 0: return hdInterfaceStreamIsPlaying0(player());
    case 1: return hdInterfaceStreamIsPlaying1(player());
    case 2: return hdInterfaceStreamIsPlaying2(player());
    case 3: return hdInterfaceStreamIsPlaying3(player());
    default: return 0;
    }
}

/* 02030FA8 */
static u32 streamPause(u32 i, u32 a, u32 b) {
    switch (i) {
    case 0: return hdInterfaceStreamPause0(player(), a, b);
    case 1: return hdInterfaceStreamPause1(player(), a, b);
    case 2: return hdInterfaceStreamPause2(player(), a, b);
    case 3: return hdInterfaceStreamPause3(player(), a, b);
    default: return i;
    }
}

/* 02031048 */
static void loadStatic() {
    hdInterfaceLoadStatic(getSoundArchive());
}

/* 0203106C */
static void loadBgmWds() {
    hdInterfaceLoadBgmWds(getSoundArchive());
}

/* 02031090 */
static u32 jaiQuery1() {
    hdSoundInterfaceJAudio* b = sJAudio;
    if (b == NULL) return 0;
    return hdInterfaceQueryFirst(b);
}

/* 020310A4 */
static u32 jaiQuery2() {
    hdSoundInterfaceJAudio* b = sJAudio;
    if (b == NULL) return 1;
    return hdInterfaceQuerySecond(b);
}

/* 020310C0 */
static void unpause() {
    hdInterfaceUnpausePlayer(player(), 1);
    hdInterfaceUnpauseBgm(bgm(), 1);
}

// HD: reset each independent player flag in the original call order.
/* 020310F4 */
static void reset() {
    setSoundOutputMode(0);
    setBridgeFlag();
    menuOut();
    clearPlayerFlagF5();
    clearPlayerFlagF6();
    clearPlayerFlagF7();
    unpause();
    clearPlayerFlagF8();
    bgmUpdate();
}

/* 02031138 */
static void pauseAll() {
    hdInterfacePausePlayer(player(), 3);
    hdInterfacePauseBgm(bgm(), 3);
}

/* 0203116C */
static void resumeAll() {
    hdInterfaceResumePlayer(player(), 3);
    hdInterfaceResumeBgm(bgm(), 3);
}

/* 020311A0 */
static void bgmStop() {
    hdInterfaceArmSe(bgm());
}

/* 020311C4 */
static void __sinit_hd_sound_interface_cpp() {
    sinit_header_statics_rodata(0x10200448, 0x1018EE08, 0x10200454, 0x10004848);
}

/* 02031258 */
static u32 constructDefaultFader(hdSoundInterfaceFader* self) {
    return hdInterfaceInitializeFader(self, sFaderOne);
}

/* WWHD glue (tools/verify/autoglue.py): private interface globals and calls
 str 10004834
 data sSoundManager 1018EC64
 data sJAudio 101FFC78
 data sOne 10004820
 data sZero 10004830
 data sFaderOne 10004850
 data sHeapTypeInitialized 101FD648
 data sParentTypeInitialized 101FD588
 data sHeapType 101FD958
 data sParentType 101FD95C
 ext 02766334 void hdInterfaceDestroyTrailing(void*, u32)
 ext 02760168 void hdInterfaceDestroyMember(void*, u32)
 ext 0202BF88 hdSoundInterfaceSe* hdInterfaceGetSe(void*)
 ext 0202BF98 hdSoundInterfaceBgm* hdInterfaceGetBgm(void*)
 ext 0202BF78 hdSoundInterfacePlayer* hdInterfaceGetPlayer(void*)
 ext 0202BF80 hdSoundInterfaceArchive* hdInterfaceGetArchive(void*)
 ext 02031264 void hdInterfaceFadeBgm(hdSoundInterfaceBgm*, u32, f32, f32)
 ext 0202F14C void hdInterfaceFadePlayer(hdSoundInterfacePlayer*, u32, f32, f32)
 ext 02031920 void hdInterfaceStartSe(hdSoundInterfaceSe*, const char*)
 ext 020319C8 hdSoundInterfaceHandle* hdInterfacePrepareSe(hdSoundInterfaceSe*, const char*)
 ext 02885914 void hdInterfaceSetVolume(void*, u32, f32)
 ext 028859A0 void hdInterfaceSetPitch(void*, f32)
 ext 020316E0 void hdInterfaceDisarmSe(hdSoundInterfaceBgm*)
 ext 0201E5E0 void hdInterfaceMenuIn(hdSoundInterfaceJAudio*)
 ext 0201DCD4 void hdInterfaceMenuOut(hdSoundInterfaceJAudio*)
 ext 02030468 void hdInterfaceLoadStatic(hdSoundInterfaceArchive*)
 ext 02030474 void hdInterfaceLoadBgmWds(hdSoundInterfaceArchive*)
 ext 02027A7C u32 hdInterfaceQueryFirst(hdSoundInterfaceJAudio*)
 ext 020278E8 u32 hdInterfaceQuerySecond(hdSoundInterfaceJAudio*)
 ext 0202FC38 void hdInterfaceUnpausePlayer(hdSoundInterfacePlayer*, u32)
 ext 020316C8 void hdInterfaceUnpauseBgm(hdSoundInterfaceBgm*, u32)
 ext 0202FB98 void hdInterfacePausePlayer(hdSoundInterfacePlayer*, u32)
 ext 02031698 void hdInterfacePauseBgm(hdSoundInterfaceBgm*, u32)
 ext 0202FC28 void hdInterfaceResumePlayer(hdSoundInterfacePlayer*, u32)
 ext 020316B8 void hdInterfaceResumeBgm(hdSoundInterfaceBgm*, u32)
 ext 020316D4 void hdInterfaceArmSe(hdSoundInterfaceBgm*)
 ext 02762170 u32 hdInterfaceInitializeFader(hdSoundInterfaceFader*, f32)
 ext 0202FFB8 u32 hdInterfaceStreamStart0(hdSoundInterfacePlayer*)
 ext 02030054 u32 hdInterfaceStreamStart1(hdSoundInterfacePlayer*)
 ext 020300F0 u32 hdInterfaceStreamStart2(hdSoundInterfacePlayer*)
 ext 0203018C u32 hdInterfaceStreamStart3(hdSoundInterfacePlayer*)
 ext 0202FFF4 u32 hdInterfaceStreamIsPlaying0(hdSoundInterfacePlayer*)
 ext 02030090 u32 hdInterfaceStreamIsPlaying1(hdSoundInterfacePlayer*)
 ext 0203012C u32 hdInterfaceStreamIsPlaying2(hdSoundInterfacePlayer*)
 ext 020301C8 u32 hdInterfaceStreamIsPlaying3(hdSoundInterfacePlayer*)
 ext 02030044 u32 hdInterfaceStreamPause0(hdSoundInterfacePlayer*, u32, u32)
 ext 020300E0 u32 hdInterfaceStreamPause1(hdSoundInterfacePlayer*, u32, u32)
 ext 0203017C u32 hdInterfaceStreamPause2(hdSoundInterfacePlayer*, u32, u32)
 ext 02030218 u32 hdInterfaceStreamPause3(hdSoundInterfacePlayer*, u32, u32)
*/
