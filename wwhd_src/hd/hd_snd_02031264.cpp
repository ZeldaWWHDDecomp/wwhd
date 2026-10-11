/** HD BGM/JAudio controller and SE wrapper. No GameCube counterpart. */
#include "bindings.h"
#include "hd/hd_snd_02031264.h"
// HD: original external array-constructor callback.
static constexpr u32 BGM_FADER_CONSTRUCTOR = 0x02031258;
static f32 sOne;
static f32 sZero;
static f32 sDrcScale;
static f32 sMinusOne;
static char sBgmNameText;
static hdBgmOutput* sOutput;
static hdBgmSafeString sBgmName;

/* 02031264 */
static void fadeOutput(hdBgmController* i_this, u32 frames, f32 tv, f32 drc) {
    hdBgmFade(&i_this->mOutputFaders[0], frames, tv);
    hdBgmFade(&i_this->mOutputFaders[1], frames, drc);
}
/* 020312CC */
static hdBgmController* constructBgmController(hdBgmController* i_this) {
    if (i_this == NULL) {
        i_this = (hdBgmController*)operator_new(sizeof(hdBgmController));
        if (i_this == NULL) return NULL;
    }
    i_this->field_0x8 = 0;
    i_this->mInitialized = 0;
    i_this->field_0x4 = 0;
    i_this->mVtable = 0x100048D0;
    i_this->mJAudio = NULL;
    hdBgmConstructArray(i_this->mOutputFaders, 2, sizeof(hdBgmFader), BGM_FADER_CONSTRUCTOR);
    f32 one = sOne;
    hdBgmInitializeFader(&i_this->mBgmFader, one);
    i_this->mBridge = NULL;
    be<u32>* pause = i_this->pauseForConstruction();
    i_this->mOutputOff = 0;
    if (pause == NULL) pause = (be<u32>*)operator_new(sizeof(be<u32>));
    if (pause != NULL) *pause = 0;
    i_this->mDelayedSeTimer = -1;
    hdBgmJAudio* jaudio = (hdBgmJAudio*)operator_new(sizeof(hdBgmJAudio));
    if (jaudio != NULL) jaudio = hdBgmConstructJAudio(jaudio);
    i_this->mJAudio = jaudio;
    hdBgmBridge* bridge = (hdBgmBridge*)operator_new(sizeof(hdBgmBridge));
    if (bridge != NULL) bridge = hdBgmConstructBridge(bridge);
    i_this->mBridge = bridge;
    fadeOutput(i_this, 0, one, sZero);
    hdBgmFade(&i_this->mBgmFader, 0, one);
    i_this->mPaused = 0;
    return i_this;
}
/* 02031408 */
static void clearBgmInitialization(hdBgmController* i_this) { i_this->mInitialized = 0; }
/* 02031414 */
static void destroyBgmController(hdBgmController* i_this, s32 flags) {
    if (i_this == NULL) return;
    i_this->mVtable = 0x100048D0;
    clearBgmInitialization(i_this);
    hdBgmJAudio* jaudio = i_this->mJAudio;
    if (jaudio != NULL) {
        jaudio->mVtable->destroy(jaudio);
        i_this->mJAudio = NULL;
    }
    hdBgmBridge* bridge = i_this->mBridge;
    if (bridge != NULL) {
        bridge->mVtable->destroy(bridge);
        i_this->mBridge = NULL;
    }
    if (flags & 1) operator_delete(i_this);
}
/* 020314C4 */
static void initializeBgmController(hdBgmController* i_this, u32 unused, u32 option) {
    (void)unused;
    if (i_this->mInitialized != 0) return;
    hdBgmInitializeJAudio(i_this->mJAudio, option, 0x3000000);
    hdBgmAttachBridge(i_this->mJAudio, i_this->mBridge);
    i_this->mInitialized = 1;
}
/* 0203151C */
static void updateBgmOutput(hdBgmController* i_this) {
    hdBgmUpdateFader(&i_this->mOutputFaders[0]);
    hdBgmUpdateFader(&i_this->mOutputFaders[1]);
    hdBgmUpdateFader(&i_this->mBgmFader);
    hdBgmOutput* output = sOutput;
    if (output == NULL) return;
    f32 tvLevel = i_this->mOutputFaders[0].mVolume;
    f32 bgmLevel = i_this->mBgmFader.mVolume;
    f32 tvSquared = gabi::fmuls_ppc(tvLevel, tvLevel);
    f32 drcLevel = i_this->mOutputFaders[1].mVolume;
    f32 scale = sDrcScale;
    f32 drcSquared = gabi::fmuls_ppc(drcLevel, drcLevel);
    f32 tv = gabi::fmuls_ppc(tvSquared, bgmLevel);
    f32 drc = gabi::fmuls_ppc(gabi::fmuls_ppc(drcSquared, scale), bgmLevel);
    hdBgmSetOutputVolumes(output, tv, drc);
    // HD: unordered (NaN) also selects output-off.
    i_this->mOutputOff = bgmLevel > sZero ? 0 : 1;
}
/* 020315CC */
static void updateBgmController(hdBgmController* i_this) {
    if (i_this->mInitialized == 0) return;
    updateBgmOutput(i_this);
    hdBgmJAudio* jaudio = i_this->mJAudio;
    if (jaudio == NULL) return;
    s32 timer = i_this->mDelayedSeTimer;
    if (timer > 0) {
        if (i_this->mPaused != 0 || jaudio->mPaused != 0) {
            hdBgmUpdateJAudio(jaudio);
            return;
        }
        --timer;
        i_this->mDelayedSeTimer = timer;
        if (timer > 0) {
            hdBgmUpdateJAudio(i_this->mJAudio);
            return;
        }
        f32 one = sOne;
        f32 minusOne = sMinusOne;
        // HD: delayed SE identifier and original floating argument order.
        hdBgmStartDelayedSe(i_this->mJAudio, 0x806, 0, 0, 0, 0, one, one, minusOne, minusOne);
    }
    i_this->mDelayedSeTimer = -1;
    hdBgmUpdateJAudio(i_this->mJAudio);
}
/* 02031688 */
static u32 fadeBgmOut(hdBgmController* i_this, u32 frames) {
    return hdBgmFade(&i_this->mBgmFader, frames, sZero);
}
/* 02031698 */
static u32 pauseBgm(hdBgmController* i_this, u32 frames) {
    if (i_this->mPaused != 0) return gabi::ea(i_this);
    return fadeBgmOut(i_this, frames);
}
/* 020316A8 */
static u32 fadeBgmIn(hdBgmController* i_this, u32 frames) {
    return hdBgmFade(&i_this->mBgmFader, frames, sOne);
}
/* 020316B8 */
static u32 resumeBgm(hdBgmController* i_this, u32 frames) {
    if (i_this->mPaused != 0) return gabi::ea(i_this);
    return fadeBgmIn(i_this, frames);
}
/* 020316C8 */
static u32 unpauseBgm(hdBgmController* i_this, u32 frames) {
    i_this->mPaused = 0;
    return fadeBgmIn(i_this, frames);
}
/* 020316D4 */
static void armDelayedSe(hdBgmController* i_this) { i_this->mDelayedSeTimer = 0x28; }
/* 020316E0 */
static void disarmDelayedSe(hdBgmController* i_this) { i_this->mDelayedSeTimer = -1; }
/* 020316EC */
static void __sinit_hd_bgm_cpp() {
    sinit_header_statics_rodata(0x1020046C, 0x1018EE2C, 0x10200478, 0x100048B8);
    sBgmName.mVtable = 0x10004864;
    sBgmName.mText = (const char*)&sBgmNameText;
}
/* 020317A0 */
static void destroyBgmCompanion(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 020317B4 */
static void emptyBgmCompanion() {}
/* 020317B8 */
static u32 getBgmCompanionZero() { return 0; }
/* 020317C0 */
static hdBgmSePlayer* constructSePlayer(hdBgmSePlayer* i_this, u32 firstOption, u32 secondOption) {
    if (i_this == NULL) {
        i_this = (hdBgmSePlayer*)operator_new(sizeof(hdBgmSePlayer));
        if (i_this == NULL) return NULL;
    }
    i_this->mPlayer = NULL;
    i_this->mPlayer = hdBgmConstructNwPlayer(NULL, firstOption, secondOption);
    return i_this;
}
/* 02031830 */
static u32 stopAllSe(hdBgmSePlayer* i_this, u32 frames) {
    return hdBgmStopAllSounds(i_this->mPlayer, frames);
}
/* 02031838 */
static void destroySePlayer(hdBgmSePlayer* i_this, s32 flags) {
    if (i_this == NULL) return;
    stopAllSe(i_this, 0);
    hdBgmNwPlayer* player = i_this->mPlayer;
    if (player != NULL) {
        player->mVtable->destroy(player);
        i_this->mPlayer = NULL;
    }
    if (flags & 1) operator_delete(i_this);
}
/* 020318B4 */
static void* startSe(hdBgmSePlayer* i_this, u32 id) {
    void* handle = hdBgmAllocateSoundHandle(i_this->mPlayer, id, 1);
    if (handle == NULL) return NULL;
    u32 archive = hdBgmGetSoundArchive();
    if (hdBgmBindSound(hdBgmArchiveInterface(archive), handle, id, 0) != 0) return NULL;
    return handle;
}
/* 0203195C */
static void* prepareSe(hdBgmSePlayer* i_this, u32 id) {
    void* handle = hdBgmAllocateSoundHandle(i_this->mPlayer, id, 0);
    if (handle == NULL) return NULL;
    u32 archive = hdBgmGetSoundArchive();
    if (hdBgmBindPreparedSound(hdBgmArchiveInterface(archive), handle, id, 0) != 0) return NULL;
    return handle;
}
/* 02031920 */
static void* startSeByName(hdBgmSePlayer* i_this, const char* name) {
    u32 id = hdBgmLookupSoundName(name);
    return startSe(i_this, id);
}
/* 020319C8 */
static void* prepareSeByName(hdBgmSePlayer* i_this, const char* name) {
    u32 id = hdBgmLookupSoundName(name);
    return prepareSe(i_this, id);
}
/* 02031A04 */
static void __sinit_hd_se_player_cpp() {
    sinit_header_statics_rodata(0x10200488, 0x1018EE50, 0x10200494, 0x10004970);
}
/* WWHD glue (tools/verify/autoglue.py): private HD controller calls and constants
 data sOne 10004850
 data sZero 100048AC
 data sDrcScale 100048B0
 data sMinusOne 100048B4
 data sOutput 101F9FF4
 data sBgmName 10200464
 data sBgmNameText 100048C0
 ext 028EFFD0 void hdBgmConstructArray(void*, u32, u32, u32)
 ext 02762258 u32 hdBgmFade(hdBgmFader*, u32, f32)
 ext 02762170 void hdBgmInitializeFader(hdBgmFader*, f32)
 ext 027621EC void hdBgmUpdateFader(hdBgmFader*)
 ext 02025B44 hdBgmJAudio* hdBgmConstructJAudio(hdBgmJAudio*)
 ext 0202C27C hdBgmBridge* hdBgmConstructBridge(hdBgmBridge*)
 ext 02026044 void hdBgmInitializeJAudio(hdBgmJAudio*, u32, u32)
 ext 0280348C void hdBgmAttachBridge(hdBgmJAudio*, hdBgmBridge*)
 ext 0281A2D0 void hdBgmSetOutputVolumes(hdBgmOutput*, f32, f32)
 ext 0202636C void hdBgmUpdateJAudio(hdBgmJAudio*)
 ext 0201EBA0 void hdBgmStartDelayedSe(hdBgmJAudio*, u32, u32, u32, u32, u32, f32, f32, f32, f32)
 ext 02761D48 hdBgmNwPlayer* hdBgmConstructNwPlayer(hdBgmNwPlayer*, u32, u32)
 ext 02761E2C u32 hdBgmStopAllSounds(hdBgmNwPlayer*, u32)
 ext 027620A8 void* hdBgmAllocateSoundHandle(hdBgmNwPlayer*, u32, u32)
 ext 0202DAD0 u32 hdBgmGetSoundArchive()
 ext 02897C94 u32 hdBgmBindSound(void*, void*, u32, u32)
 ext 02897DE4 u32 hdBgmBindPreparedSound(void*, void*, u32, u32)
 ext 0202DC30 u32 hdBgmLookupSoundName(const char*)
*/
