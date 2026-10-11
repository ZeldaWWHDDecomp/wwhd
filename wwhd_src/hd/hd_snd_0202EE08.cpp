/** HD sound player, channel faders and stream handles. No GameCube counterpart. */
#include "bindings.h"
#include "hd/hd_snd_0202EE08.h"
static char sBgmPlayerName0;
static char sBgmPlayerName1;
static char sDrcTvPlayerName;
static char sStreamName0;
static char sStreamName1;
static char sStreamName2;
static char sStreamName3;
static f32 sOne;
static f32 sZero;
static f32 sMaximumPadVolume;
static f32 sPadNearDistance;
static f32 sPadFarDistance;
static f32 sTvScale;
static f32 sPadCurveRange;
static f32 sPadCurveRangeInitial;
static f32 sFirstConstant;
static f32 sSecondConstant;
static f32 sFirstConstantInitial;
static f32 sSecondConstantInitial;
static u32 sOutputVolumeInitialized;
static hdPlayerOutputVolume sOutputVolume;
static void* sControllerManager;
static u32 sControllerTypeInitialized;
static hdPlayerControllerType sControllerType;
static hdPlayerStringTable sStringTable;
static u32 sMatchId;
static u32 sMatchExcludeFirst;
static u32 sMatchExcludeSecond;
static u8 sMatchFound;
static u8 sMatchStartedOnly;
static u32 sStopExcludeFirst;
static u32 sStopExcludeSecond;
static u32 sStopFade;
static hdPlayerHeaderStatics sCurveHeader;
static f32 sCurvePiLow;
static f32 sCurvePiHigh;
static f32 sCurvePiLowInitial;
static f32 sCurvePiHighInitial;
static u8 sCurveHeaderByte;
static u8 sCurveHeaderSecondByte;

/* 0202EE08 */
static void __sinit_hd_sound_constants_cpp() {
    sinit_header_statics_rodata(0x102003B0, 0x1018ED38, 0x102003BC, 0x1000442C);
    f32 first = sFirstConstantInitial;
    f32 second = sSecondConstantInitial;
    sFirstConstant = first;
    sSecondConstant = second;
}
/* 0202EEB8 */
static void destroyFirstPlayerCompanion(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 0202EECC */
static void emptyPlayerString() {}
/* 0202EED0 */
static void assurePlayerStringTermination(hdPlayerFixedString* i_this) { i_this->terminate(); }
/* 0202EEE8 */
static void destroySecondPlayerCompanion(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 0202EEFC */
static void destroyThirdPlayerCompanion(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 0202EF10 */
static void destroyFourthPlayerCompanion(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 0202EF24 */
static void __sinit_hd_player_fader_cpp() {
    sinit_header_statics_rodata(0x102003CC, 0x1018ED5C, 0x102003D8, 0x10004450);
}
/* 0202EFB8 */
static u32 constructPlayerFader(hdPlayerFader* i_this) { return hdPlayerInitializeFader(i_this, sOne); }
/* 0202EFC4 */
static void destroySoundPlayer(hdSoundPlayer* i_this, s32 flags) {
    if (i_this == NULL) return;
    i_this->mArchiveInterface.mVtable = 0x10004688;
    i_this->mSecondaryVtable = 0x100046A8;
    hdPlayerDestroyHandle(&i_this->mStaffRollHandle);
    hdPlayerDestroyHandle(&i_this->mStreamHandle);
    hdPlayerDestroyBase(i_this, 0);
    if (flags & 1) operator_delete(i_this);
}
/* 0202F040 */
static u32 forwardSoundPlayerSecondary(void* secondary, u32 option) {
    // HD: secondary-base adjustment wraps in the guest address space.
    return hdPlayerForwardSecondary(gabi::at<hdSoundPlayer>(gabi::ea(secondary) - 4), option);
}
/* 0202F048 */
static hdSoundPlayer* constructSoundPlayer(hdSoundPlayer* i_this) {
    if (i_this == NULL) {
        i_this = (hdSoundPlayer*)operator_new(sizeof(hdSoundPlayer));
        if (i_this == NULL) return NULL;
    }
    hdPlayerConstructBase(i_this);
    i_this->mVtable = 0x10004610;
    i_this->mArchiveInterface.mVtable = 0x10004688;
    i_this->mSecondaryVtable = 0x100046A8;
    hdPlayerConstructFaderArray(i_this->mMasterFaders, 2, sizeof(hdPlayerFader), constructPlayerFader);
    f32 one = sOne;
    hdPlayerInitializeFader(&i_this->mBgmFaders[0], one);
    hdPlayerInitializeFader(&i_this->mBgmFaders[1], one);
    i_this->mMenuFlag = 0;
    i_this->mMode = 0;
    i_this->field_0x1f7 = 0;
    i_this->field_0x1f5 = 0;
    i_this->field_0x1f8 = 0;
    i_this->field_0x1f6 = 0;
    be<u32>* pause = i_this->pauseForConstruction();
    if (pause == NULL) pause = (be<u32>*)operator_new(sizeof(be<u32>));
    if (pause != NULL) *pause = 0;
    i_this->mStaffRollHandle.mSound = NULL;
    i_this->mStreamHandle.mSound = NULL;
    return i_this;
}
/* 0202F14C */
static void fadePlayerMaster(hdSoundPlayer* i_this, u32 frames, f32 tv, f32 drc) {
    hdPlayerFade(&i_this->mMasterFaders[0], frames, tv);
    hdPlayerFade(&i_this->mMasterFaders[1], frames, drc);
}
/* 0202F1B4 */
static u32 fadeFirstBgmPlayer(hdSoundPlayer* i_this, u32 frames, f32 volume) {
    return hdPlayerFade(&i_this->mBgmFaders[0], frames, volume);
}
/* 0202F1BC */
static u32 fadeSecondBgmPlayer(hdSoundPlayer* i_this, u32 frames, f32 volume) {
    return hdPlayerFade(&i_this->mBgmFaders[1], frames, volume);
}
/* 0202F1C4 */
static void resetSoundPlayer(hdSoundPlayer* i_this) {
    hdPlayerResetBase(i_this);
    if (sOutputVolumeInitialized == 0) {
        sOutputVolumeInitialized = 1;
        hdPlayerConstructOutputVolume(&sOutputVolume);
    }
    f32 one = sOne;
    hdPlayerSetOutputVolume(&sOutputVolume, 0, one);
    fadePlayerMaster(i_this, 0, one, sZero);
    fadeFirstBgmPlayer(i_this, 0, one);
    fadeSecondBgmPlayer(i_this, 0, one);
    i_this->mPaused = 0;
    void* heap = hdPlayerGetManagerHeap();
    if (heap != NULL && hdPlayerGetHeapState(heap) == 2) hdPlayerSetHeapState(heap, 0);
}
/* 0202F2B4 */
static u32 forwardSoundPlayer(hdSoundPlayer* i_this, u32 first, u32 second) {
    return hdPlayerForwardBase(i_this, first, second);
}
/* 0202F2B8 */
static u32 getControllerDistance() {
    void* manager = sControllerManager;
    if (manager == NULL) return 0;
    hdPlayerController* controller = hdPlayerGetController(manager, 8);
    if (controller == NULL) return 0;
    if (sControllerTypeInitialized == 0) {
        sControllerTypeInitialized = 1;
        sControllerType.mNameAddress = 0x100044B0;
    }
    if (controller->checkType(&sControllerType) == 0) return 0;
    if (controller->mActive <= 0) return 0;
    return controller->mDistance;
}
/* 0202F378 */
static f32 getControllerVolume(hdSoundPlayer* i_this) {
    (void)i_this;
    u32 distance = getControllerDistance();
    f32 value = (f32)(f64)distance;
    f32 nearDistance = sPadNearDistance;
    f32 volume = sMaximumPadVolume;
    if (value <= nearDistance) return volume;
    if (!(value < sPadFarDistance)) return sZero;
    f32 ratio = hdPlayerDivide(gabi::fsubs_ppc(value, nearDistance), sPadCurveRange);
    f32 remaining = gabi::fsubs_ppc(sOne, ratio);
    return gabi::fmuls_ppc(gabi::fmuls_ppc(volume, remaining), remaining);
}
static void setPlayerChannelVolume(void* player, u32 channel, f32 volume) { hdPlayerSetChannelVolume(player, channel, volume); }

/* 0202F41C */
static void updatePlayerVolumes(hdSoundPlayer* i_this) {
    hdPlayerUpdateFader(&i_this->mMasterFaders[0]);
    hdPlayerUpdateFader(&i_this->mMasterFaders[1]);
    if (i_this->mArchiveInterface.mPlayerCount == 0) return;
    f32 drcMaster = (f32)i_this->mMasterFaders[1].mVolume;
    f32 tvMaster = (f32)i_this->mMasterFaders[0].mVolume;
    f32 drc = gabi::fmuls_ppc(drcMaster, drcMaster);
    f32 tvScale = sTvScale;
    f32 tv = gabi::fmuls_ppc(gabi::fmuls_ppc(tvMaster, tvMaster), tvScale);
    void* players[10];
    players[0] = hdPlayerGetNamedPlayer(&i_this->mArchiveInterface, "PLAYER_SYSTEM_SE");
    players[1] = hdPlayerGetNamedPlayer(&i_this->mArchiveInterface, "PLAYER_SEQ_BGM");
    players[2] = hdPlayerGetNamedPlayer(&i_this->mArchiveInterface, "PLAYER_STRM_BGM");
    players[3] = hdPlayerGetNamedPlayer(&i_this->mArchiveInterface, "PLAYER_STRM_SUB_BGM");
    players[4] = hdPlayerGetNamedPlayer(&i_this->mArchiveInterface, "PLAYER_MENU_UI");
    players[5] = hdPlayerGetNamedPlayer(&i_this->mArchiveInterface, "PLAYER_DRC_ONLY");
    players[6] = hdPlayerGetNamedPlayer(&i_this->mArchiveInterface, "PLAYER_DRC_GAME");
    players[7] = hdPlayerGetNamedPlayer(&i_this->mArchiveInterface, "PLAYER_DRC_TV");
    players[8] = hdPlayerGetNamedPlayer(&i_this->mArchiveInterface, "PLAYER_ITEM_SET");
    players[9] = hdPlayerGetNamedPlayer(&i_this->mArchiveInterface, "PLAYER_CONTROLLER");
    for (u32 i = 0; i < 10; ++i) hdPlayerSetPlayerChannels( players[i], PLAYER_CHANNEL_MASK);
    void* systemSe = players[0];
    void* sequenceBgm = players[1];
    void* streamBgm = players[2];
    void* subBgm = players[3];
    void* menu = players[4];
    void* drcOnly = players[5];
    void* drcGame = players[6];
    void* drcTv = players[7];
    void* item = players[8];
    void* controllerPlayer = players[9];
    setPlayerChannelVolume(systemSe, PLAYER_CHANNEL_TV, tv);
    setPlayerChannelVolume(systemSe, PLAYER_CHANNEL_DRC, drc);
    setPlayerChannelVolume(sequenceBgm, PLAYER_CHANNEL_TV, tv);
    setPlayerChannelVolume(sequenceBgm, PLAYER_CHANNEL_DRC, drc);
    setPlayerChannelVolume(streamBgm, PLAYER_CHANNEL_TV, tv);
    setPlayerChannelVolume(streamBgm, PLAYER_CHANNEL_DRC, drc);
    setPlayerChannelVolume(subBgm, PLAYER_CHANNEL_TV, tv);
    setPlayerChannelVolume(subBgm, PLAYER_CHANNEL_DRC, drc);
    bool loud = i_this->field_0x1f7 != 0 || i_this->field_0x1f8 != 0; /* read before the DRC_GAME calls */
    f32 one = sOne;
    setPlayerChannelVolume(drcGame, PLAYER_CHANNEL_TV, tv);
    setPlayerChannelVolume(drcGame, PLAYER_CHANNEL_DRC, loud ? drc : one);
    f32 zero = sZero;
    bool drcMode = i_this->mMenuFlag != 0 && i_this->mMode == 1;
    if (drcMode) {
        setPlayerChannelVolume(menu, PLAYER_CHANNEL_TV, tvScale);
        setPlayerChannelVolume(menu, PLAYER_CHANNEL_DRC, zero);
    } else if (i_this->field_0x1f6 != 0 || i_this->field_0x1f7 != 0 || i_this->field_0x1f8 != 0) {
        setPlayerChannelVolume(menu, PLAYER_CHANNEL_TV, tv);
        setPlayerChannelVolume(menu, PLAYER_CHANNEL_DRC, drc);
    } else {
        f32 pv = getControllerVolume(i_this);
        setPlayerChannelVolume(menu, PLAYER_CHANNEL_TV, gabi::fmuls_ppc(pv, tv));
        setPlayerChannelVolume(menu, PLAYER_CHANNEL_DRC, one);
    }
    setPlayerChannelVolume(drcOnly, PLAYER_CHANNEL_TV, zero);
    setPlayerChannelVolume(drcOnly, PLAYER_CHANNEL_DRC, one);
    setPlayerChannelVolume(drcTv, PLAYER_CHANNEL_TV, tvScale);
    setPlayerChannelVolume(drcTv, PLAYER_CHANNEL_DRC, one);
    if (i_this->mMenuFlag == 0) {
        setPlayerChannelVolume(item, PLAYER_CHANNEL_TV, tv);
        setPlayerChannelVolume(item, PLAYER_CHANNEL_DRC, one);
    } else if (i_this->mMode == 1) {
        setPlayerChannelVolume(item, PLAYER_CHANNEL_TV, tvScale);
        setPlayerChannelVolume(item, PLAYER_CHANNEL_DRC, zero);
    } else {
        setPlayerChannelVolume(item, PLAYER_CHANNEL_TV, zero);
        setPlayerChannelVolume(item, PLAYER_CHANNEL_DRC, one);
    }
    if (i_this->mMode == 1) {
        setPlayerChannelVolume(controllerPlayer, PLAYER_CHANNEL_TV, tvScale);
        setPlayerChannelVolume(controllerPlayer, PLAYER_CHANNEL_DRC, zero);
    } else {
        f32 pv = getControllerVolume(i_this);
        setPlayerChannelVolume(controllerPlayer, PLAYER_CHANNEL_TV, gabi::fmuls_ppc(pv, tv));
        setPlayerChannelVolume(controllerPlayer, PLAYER_CHANNEL_DRC, one);
    }
}
/* 0202F994 */
static void updateBgmVolume0(hdSoundPlayer* i_this) {
    hdPlayerUpdateFader(&i_this->mBgmFaders[0]);
    if (i_this->mArchiveInterface.mPlayerCount == 0) return;
    void* player = hdPlayerGetNamedPlayer(&i_this->mArchiveInterface, (const char*)&sBgmPlayerName0);
    hdPlayerSetPlayerVolume(player, i_this->mBgmFaders[0].mVolume);
}
/* 0202F9E8 */
static void updateBgmVolume1(hdSoundPlayer* i_this) {
    hdPlayerUpdateFader(&i_this->mBgmFaders[1]);
    if (i_this->mArchiveInterface.mPlayerCount == 0) return;
    void* player = hdPlayerGetNamedPlayer(&i_this->mArchiveInterface, (const char*)&sBgmPlayerName1);
    hdPlayerSetPlayerVolume(player, i_this->mBgmFaders[1].mVolume);
}
/* 0202FA3C */
static void updateSoundPlayer(hdSoundPlayer* i_this) {
    updatePlayerVolumes(i_this);
    updateBgmVolume0(i_this);
    updateBgmVolume1(i_this);
    hdPlayerUpdateBase(i_this);
}
/* 0202FA80 */
static void pauseAllPlayers(hdSoundPlayer* i_this, u32 frames) {
    u32 count = i_this->mArchiveInterface.mPlayerCount;
    if (count == 0) return;
    hdPlayerSafeString excluded;
    hdPlayerSafeString name;
    for (u32 index = 0; count != 0; --count, ++index) {
        u32 id = index + PLAYER_ID_BASE;
        excluded.mVtable = &sStringTable;
        excluded.mText = (const be<u8>*)&sDrcTvPlayerName;
        const be<u8>* text = hdPlayerGetPlayerName(id);
        name.mVtable = &sStringTable;
        name.mText = text;
        excluded.assureTermination();
        excluded.assureTermination();
        hdPlayerStringTable* nameTable = name.mVtable;
        const be<u8>* first = excluded.mText;
        gabi::call_ptr(nameTable->mAssureTermination.get(), &name);
        const be<u8>* second = name.mText;
        if (first == second || hdPlayerStringEquals(first, second)) continue;
        hdPlayerSubPlayer* player = hdPlayerGetPlayerById(&i_this->mArchiveInterface, id);
        hdPlayerSetPlayerPaused(player, 1, frames);
    }
}
/* 0202FB98 */
static u32 pauseSoundPlayer(hdSoundPlayer* i_this, u32 frames) {
    if (i_this->mPaused != 0) return gabi::ea(i_this);
    // HD: this call forwards r3 left by the void pause-all function.
    return hdPlayerPauseAllResult(i_this, frames);
}
/* 0202FBA8 */
static void resumeAllPlayers(hdSoundPlayer* i_this, u32 frames) {
    u32 count = i_this->mArchiveInterface.mPlayerCount;
    for (u32 index = 0; count != 0; --count, ++index) {
        hdPlayerSubPlayer* player = hdPlayerGetPlayerById(&i_this->mArchiveInterface, index + PLAYER_ID_BASE);
        hdPlayerSetPlayerPaused(player, 0, frames);
    }
}
/* 0202FC28 */
static u32 resumeSoundPlayer(hdSoundPlayer* i_this, u32 frames) {
    if (i_this->mPaused != 0) return gabi::ea(i_this);
    // HD: preserve the void resume-all function's resulting r3.
    return hdPlayerResumeAllResult(i_this, frames);
}
/* 0202FC38 */
static u32 unpauseSoundPlayer(hdSoundPlayer* i_this, u32 frames) {
    i_this->mPaused = 0;
    return hdPlayerResumeAllResult(i_this, frames);
}
/* 0202FC44 */
static void matchPlayerSound(hdPlayerSoundHandle* handle) {
    hdPlayerSound* sound = handle->mSound;
    hdPlayerSoundInfo* info = sound == NULL ? NULL : sound->mInfo.get();
    u32 key = sMatchId;
    if (hdPlayerReadInfoId(info) != key) return;
    if (sMatchStartedOnly != 0) {
        if (handle->mSound == NULL) return;
        if (sound->isPlaying() == 0) return;
        hdPlayerSound* current = handle->mSound;
        if (current == NULL) return;
        if (current->mStarted == 0) return;
    }
    u32 first = sMatchExcludeFirst;
    u32 id = hdPlayerReadInfoHandle(info);
    if (first != 0 && id == first) return;
    u32 second = sMatchExcludeSecond;
    if (second != 0 && id == second) return;
    sMatchFound = 1;
}
/* inlined into isPlayerSoundPlaying, stopPlayerSounds */
static bool scanPlayers(hdSoundPlayer* i_this, u32 count, u32 callback, bool stopOnHit) {
    hdPlayerSoundHandle handle;
    for (u32 index = 0; count != 0; --count, ++index) {
        hdPlayerSubPlayer* player = hdPlayerGetPlayerById(&i_this->mArchiveInterface, index + PLAYER_ID_BASE);
        hdPlayerListNode* head = player->soundListHead();
        for (hdPlayerListNode* node = player->mSoundList.mNext; node != head; ) {
            hdPlayerListNode* current = node;
            node = node->mNext;
            handle.mSound = NULL;
            hdPlayerAttachHandle(&handle, hdPlayerSoundFromNode(current));
            gabi::call(callback, &handle);
            hdPlayerDestroyHandle(&handle);
        }
        if (stopOnHit && sMatchFound != 0) return true;
    }
    return false;
}
/* 0202FD40 */
static u32 isPlayerSoundPlaying(hdSoundPlayer* i_this, u32 key, u32 startedOnly, u32 firstExclude, u32 secondExclude) {
    u32 count = i_this->mArchiveInterface.mPlayerCount;
    if (count == 0) return 0;
    sMatchExcludeFirst = firstExclude;
    sMatchFound = 0;
    sMatchExcludeSecond = secondExclude;
    sMatchStartedOnly = (u8)startedOnly;
    sMatchId = key;
    if (scanPlayers(i_this, count, PLAYER_SCAN_MATCH, true)) {
        sMatchId = 0xFFFFFFFF;
        sMatchExcludeFirst = 0;
        sMatchExcludeSecond = 0;
        sMatchFound = 0;
        sMatchStartedOnly = 0;
        return 1;
    }
    sMatchId = 0xFFFFFFFF;
    sMatchExcludeSecond = 0;
    sMatchStartedOnly = 0;
    sMatchFound = 0;
    sMatchExcludeFirst = 0;
    return 0;
}
/* 0202FE78 */
static u32 stopPlayerSound(hdPlayerSoundHandle* handle) {
    hdPlayerSound* sound = handle->mSound;
    u32 first = sStopExcludeFirst;
    hdPlayerSoundInfo* info = sound == NULL ? NULL : sound->mInfo.get();
    u32 id = hdPlayerReadInfoHandle(info);
    if (first != 0 && id == first) return gabi::ea(handle);
    u32 second = sStopExcludeSecond;
    if (second != 0 && id == second) return gabi::ea(handle);
    hdPlayerSound* current = handle->mSound;
    u32 fade = sStopFade;
    if (current == NULL) return gabi::ea(handle);
    return hdPlayerStopSound(sound, fade);
}
/* 0202FEF4 */
static void stopPlayerSounds(hdSoundPlayer* i_this, u32 fade, u32 firstExclude, u32 secondExclude) {
    u32 count = i_this->mArchiveInterface.mPlayerCount;
    if (count == 0) return;
    sStopExcludeFirst = firstExclude;
    sStopFade = fade;
    sStopExcludeSecond = secondExclude;
    scanPlayers(i_this, count, PLAYER_SCAN_STOP, false);
    sStopExcludeFirst = 0;
    sStopFade = 0;
    sStopExcludeSecond = 0;
}
/* 0202FFB8 */
static u32 startHdStream0(hdSoundPlayer* i_this) {
    return hdPlayerStartStream(&i_this->mArchiveInterface, &i_this->mStreamHandle, (const char*)&sStreamName0, 0) == 0 ? 1 : 0;
}
/* 0202FFF4 */
static u32 isHdStreamPlaying0(hdSoundPlayer* i_this) {
    if (i_this->mStreamHandle.mSound == NULL) return 0;
    hdPlayerSound* sound = i_this->mStreamHandle.mSound;
    return sound->isPlaying() != 0 ? 1 : 0;
}
/* 02030044 */
static u32 pauseHdStream0(hdSoundPlayer* i_this, u32 first, u32 second) {
    hdPlayerSound* sound = i_this->mStreamHandle.mSound;
    if (sound == NULL) return 0;
    return hdPlayerPauseStream(sound, first, second);
}
/* 02030054 */
static u32 startHdStream1(hdSoundPlayer* i_this) {
    return hdPlayerStartStream(&i_this->mArchiveInterface, &i_this->mStreamHandle, (const char*)&sStreamName1, 0) == 0 ? 1 : 0;
}
/* 02030090 */
static u32 isHdStreamPlaying1(hdSoundPlayer* i_this) {
    if (i_this->mStreamHandle.mSound == NULL) return 0;
    hdPlayerSound* sound = i_this->mStreamHandle.mSound;
    return sound->isPlaying() != 0 ? 1 : 0;
}
/* 020300E0 */
static u32 pauseHdStream1(hdSoundPlayer* i_this, u32 first, u32 second) {
    hdPlayerSound* sound = i_this->mStreamHandle.mSound;
    if (sound == NULL) return 0;
    return hdPlayerPauseStream(sound, first, second);
}
/* 020300F0 */
static u32 startHdStream2(hdSoundPlayer* i_this) {
    return hdPlayerStartStream(&i_this->mArchiveInterface, &i_this->mStreamHandle, (const char*)&sStreamName2, 0) == 0 ? 1 : 0;
}
/* 0203012C */
static u32 isHdStreamPlaying2(hdSoundPlayer* i_this) {
    if (i_this->mStreamHandle.mSound == NULL) return 0;
    hdPlayerSound* sound = i_this->mStreamHandle.mSound;
    return sound->isPlaying() != 0 ? 1 : 0;
}
/* 0203017C */
static u32 pauseHdStream2(hdSoundPlayer* i_this, u32 first, u32 second) {
    hdPlayerSound* sound = i_this->mStreamHandle.mSound;
    if (sound == NULL) return 0;
    return hdPlayerPauseStream(sound, first, second);
}
/* 0203018C */
static u32 startHdStream3(hdSoundPlayer* i_this) {
    return hdPlayerStartStream(&i_this->mArchiveInterface, &i_this->mStaffRollHandle, (const char*)&sStreamName3, 0) == 0 ? 1 : 0;
}
/* 020301C8 */
static u32 isHdStreamPlaying3(hdSoundPlayer* i_this) {
    if (i_this->mStaffRollHandle.mSound == NULL) return 0;
    hdPlayerSound* sound = i_this->mStaffRollHandle.mSound;
    return sound->isPlaying() != 0 ? 1 : 0;
}
/* 02030218 */
static u32 pauseHdStream3(hdSoundPlayer* i_this, u32 first, u32 second) {
    hdPlayerSound* sound = i_this->mStaffRollHandle.mSound;
    if (sound == NULL) return 0;
    return hdPlayerPauseStream(sound, first, second);
}
/* 02030228 */
static void __sinit_hd_sound_player_cpp() {
    // HD: this TU's pi pair is at header -0x10; the curve range occupies header -8.
    sCurveHeader.mWords[2] = 0;
    sCurveHeader.mWords[0] = 0;
    sCurveHeader.mWords[3] = 0;
    sCurveHeader.mWords[1] = 0;
    __register_global_object(0x1018ED80);
    f32 low = sCurvePiLowInitial;
    f32 high = sCurvePiHighInitial;
    sCurvePiLow = low;
    sCurvePiHigh = high;
    hdPlayerConstructHeaderByte(&sCurveHeaderByte);
    __register_global_object(0x1018ED8C);
    hdPlayerDestroyHeaderByte(&sCurveHeaderSecondByte);
    __register_global_object(0x1018ED98);
    sPadCurveRange = sPadCurveRangeInitial;
}
/* 020302CC */
static void destroyFinalPlayerCompanion(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 020302E0 */
static void emptyFinalPlayerCompanion() {}
/* WWHD glue (tools/verify/autoglue.py): private player data and named bindings
 data sBgmPlayerName0 1000458C
 data sBgmPlayerName1 1000459C
 data sDrcTvPlayerName 100045AC
 data sStreamName0 100045BC
 data sStreamName1 100045CC
 data sStreamName2 100045DC
 data sStreamName3 100045F0
 data sOne 10004458
 data sZero 100044C0
 data sMaximumPadVolume 100044D0
 data sPadNearDistance 100044D4
 data sPadFarDistance 100044D8
 data sTvScale 100044DC
 data sPadCurveRange 102003F0
 data sPadCurveRangeInitial 1000460C
 data sFirstConstant 102003A8
 data sSecondConstant 102003AC
 data sFirstConstantInitial 10004434
 data sSecondConstantInitial 10004438
 data sOutputVolumeInitialized 101FD8C8
 data sOutputVolume 101FCE00
 data sControllerManager 101F8AE8
 data sControllerTypeInitialized 101FD694
 data sControllerType 101FD968
 data sStringTable 10004478
 data sMatchId 1018EDA4
 data sMatchExcludeFirst 1018EDA8
 data sMatchExcludeSecond 1018EDAC
 data sMatchFound 1018EDBE
 data sMatchStartedOnly 1018EDBF
 data sStopExcludeFirst 1018EDB0
 data sStopExcludeSecond 1018EDB4
 data sStopFade 1018EDB8
 data sCurveHeader 102003F8
 data sCurvePiLow 102003E8
 data sCurvePiHigh 102003EC
 data sCurvePiLowInitial 10004604
 data sCurvePiHighInitial 10004608
 data sCurveHeaderByte 102003F4
 data sCurveHeaderSecondByte 102003F5
 str 10004558
 str 1000456C
 str 100044E0
 str 10004520
 str 1000457C
 str 100044F0
 str 10004500
 str 10004534
 str 10004510
 str 10004544
 ext 02896CD8 void hdPlayerDestroyHandle(hdPlayerSoundHandle*)
 ext 02763360 void hdPlayerDestroyBase(hdSoundPlayer*, u32)
 ext 02763498 u32 hdPlayerForwardSecondary(hdSoundPlayer*, u32)
 ext 0276359C void hdPlayerConstructBase(hdSoundPlayer*)
 ext 028EFFD0 void hdPlayerConstructFaderArray(void*, u32, u32, hdPlayerFaderConstructor)
 ext 02762170 u32 hdPlayerInitializeFader(hdPlayerFader*, f32)
 ext 02762258 u32 hdPlayerFade(hdPlayerFader*, u32, f32)
 ext 02763C80 void hdPlayerResetBase(hdSoundPlayer*)
 ext 02888A50 void hdPlayerConstructOutputVolume(hdPlayerOutputVolume*)
 ext 02889F18 void hdPlayerSetOutputVolume(hdPlayerOutputVolume*, u32, f32)
 ext 0202DB6C void* hdPlayerGetManagerHeap()
 ext 02762C88 s32 hdPlayerGetHeapState(void*)
 ext 02762BC8 void hdPlayerSetHeapState(void*, u32)
 ext 02763758 u32 hdPlayerForwardBase(hdSoundPlayer*, u32, u32)
 ext 0273CB8C hdPlayerController* hdPlayerGetController(void*, u32)
 ext 027621EC void hdPlayerUpdateFader(hdPlayerFader*)
 ext 0289419C void* hdPlayerGetNamedPlayer(hdPlayerArchiveInterface*, const char*)
 ext 02897654 void hdPlayerSetPlayerChannels(void*, u32)
 ext 0289765C void hdPlayerSetChannelVolume(void*, u32, f32)
 ext 02897640 void hdPlayerSetPlayerVolume(void*, f32)
 ext 027637B4 void hdPlayerUpdateBase(hdSoundPlayer*)
 ext 0202DC64 const be<u8>* hdPlayerGetPlayerName(u32)
 ext 02893CE8 hdPlayerSubPlayer* hdPlayerGetPlayerById(hdPlayerArchiveInterface*, u32)
 ext 028975CC void hdPlayerSetPlayerPaused(void*, u32, u32)
 ext 0202FA80 u32 hdPlayerPauseAllResult(hdSoundPlayer*, u32)
 ext 0202FBA8 u32 hdPlayerResumeAllResult(hdSoundPlayer*, u32)
 ext 02896D70 void hdPlayerAttachHandle(hdPlayerSoundHandle*, hdPlayerSound*)
 ext 02883FA8 u32 hdPlayerStopSound(hdPlayerSound*, u32)
 ext 02897F48 u32 hdPlayerStartStream(hdPlayerArchiveInterface*, hdPlayerSoundHandle*, const char*, u32)
 ext 028850D4 u32 hdPlayerPauseStream(hdPlayerSound*, u32, u32)
 ext 028ED6F8 void hdPlayerConstructHeaderByte(be<u8>*)
 ext 028EAB2C void hdPlayerDestroyHeaderByte(be<u8>*)
*/
