/* d_com_inf_game: game info (dComIfG_inf_c / dComIfG_play_c) and its out-of-line helpers, WWHD.
 *
 * Translation unit 0251FD14..02525C94, from the image: the function-less unit whose __sinit is
 * 0251FC80 (probably d_com_inf_actor) precedes it; this unit starts with dComIfG_play_c::init
 * (0251FD14, tail-called by dComIfG_play_c::ct) and ends with its __sinit 02525A48 and the
 * per-TU inline copies behind it (02525ADC..02525C80: dSv_memory_c constructor, phase_3,
 * SafeString/destructor copies, dComIfG_inf_c destructor). The matcher's
 * "__sinit_d_com_inf_game_cpp" (0251FD94) is the dComIfG_play_c constructor. Part 1 (this file):
 * 0251FD14..02522F6C; part 2 in d_com_inf_game_2.cpp.
 *
 * HD: the game info g_dComIfG_gameInfo is a function-local static at 1046F0B0 constructed on first
 * use by its accessor 025200D4 (dComIfGp_get in bindings.h); the play object is at +0x12A0, the
 * save data through the pointer at 101F84DC (info = save + 0x20). Strings are compared through
 * sead::SafeString temporaries {top, vtable 1004B8C0} with inlined strcmp loops (at most 0x40001
 * characters); fixed name buffers are filled with inlined strncpy loops. */
#include "d/d_com_inf_game_unit.h"

// HD: the hdGameInfo_ free-entry prefix distinguishes these verified out-of-line copies
// from the shared GameCube inline accessors. Native adapters keep full register widths.
namespace GameInfoMemory1 {
static inline u32 ld(u32 a) {
    return gabi::load<u32>(a);
}
static inline u16 ld16(u32 a) {
    return gabi::load<u16>(a);
}
static inline s16 lds16(u32 a) {
    return gabi::load<s16>(a);
}
static inline u8 ld8(u32 a) {
    return gabi::load<u8>(a);
}
static inline s8 lds8(u32 a) {
    return gabi::load<s8>(a);
}
static inline f32 ldf(u32 a) {
    return gabi::load<f32>(a);
}
static inline void st(u32 a, u32 v) {
    gabi::store<u32>(a, v);
}
static inline void st16(u32 a, u16 v) {
    gabi::store<u16>(a, v);
}
static inline void st8(u32 a, u8 v) {
    gabi::store<u8>(a, v);
}
static inline void stf(u32 a, f32 v) {
    gabi::store<f32>(a, v);
}
} // namespace GameInfoMemory1

void gameInfo1_setPictureNum(u32, u32);
void gameInfo1_setArrowKeepType(u32);
u32 gameInfo1_getPictureInfo(u32);
void gameInfo1_onCollect(u32, u32, u32);
u32 gameInfo1_mapInfoOceanX(u32);
u32 gameInfo1_mapInfoOceanZ(u32);
void gameInfo1_setNextTime(f32);
void gameInfo1_assureTermination(u32);
u32 gameInfo1_isGetCollectMap(u32, u32);
u32 gameInfo1_isCollect(u32, u32, u32);
u32 gameInfo1_isSymbol(u32, u32);
u32 gameInfo1_isTriforce(u32, u32);
u32 gameInfo1_isTact(u32, u32);
u32 gameInfo1_getReverbStage(u32, u32);
void gameInfo1_deleteObjectArchive(u32, u32);
s32 gameInfo1_phaseHandler(u32, u32, u32);
s32 gameInfo1_syncObjectArchive(u32, u32);
u32 gameInfo1_setObjectArchive(u32, u32, u32);
void gameInfo1_audioResetProcess();
void gameInfo1_stopBgm(u32);
void gameInfo1_sceneReRequest(u32, u32);
void gameInfo1_sceneChangeRequest(u32, u32, u32, u32, u32);
void gameInfo1_setSceneName(u32, u32, u32);
u32 gameInfo1_setNextStage(u32, u32, u32, u32, u32, u32);
void gameInfo1_drawListConstructor(u32);
void gameInfo1_memoryInit(u32);
void gameInfo1_detectConstructor(u32);
void gameInfo1_vibrationConstructor(u32);
void gameInfo1_attentionConstructor(u32);
void gameInfo1_eventManagerConstructor(u32);
void gameInfo1_eventControlConstructor(u32);
void gameInfo1_stageDataConstructor(u32);
void gameInfo1_nextStageConstructor(u32);
void gameInfo1_admConstructor(u32);
void gameInfo1_collisionConstructor(u32);
void gameInfo1_backgroundConstructor(u32);
void gameInfo1_returnPlace_set(u32, u32, u32, u32);
u32 gameInfo1_getEventReg(u32, u32);
u32 gameInfo1_isEventBit(u32, u32);
u32 gameInfo1_getStatusRoomDt(u32, u32);
u32 gameInfo1_dSv_memBit_isDungeonItem(u32, u32);
u32 gameInfo1_dSv_memBit_onDungeonItem(u32, u32);
u32 gameInfo1_dSv_memBit_isTbox(u32, u32);
u32 gameInfo1_dSv_memBit_onTbox(u32, u32);
u32 gameInfo1_getGameInfo();
void gameInfo1_construct_array(u32, u32, u32, u32);
void gameInfo1_memcpy(u32, u32, u32);
void gameInfo1_memzero(u32, u32);
u32 gameInfo1_operator_new(u32);
u32 gameInfo1_JUT_ASSERT(u32, u32, u32);

// HD: private native adapters retain the original full register widths and call order.
static inline u32 JUT_ASSERT_l(u32 file, s32 line, u32 msg) {
    return gameInfo1_JUT_ASSERT(file, line, msg);
}
static inline u32 operator_new_l(u32 size) {
    return gameInfo1_operator_new(size);
}
static inline void memzero_l(u32 p, u32 n) {
    gameInfo1_memzero(p, n);
}
static inline void memcpy_l(u32 d, u32 s, u32 n) {
    gameInfo1_memcpy(d, s, n);
}
static inline void construct_array_l(u32 p, u32 n, u32 size, u32 ct) {
    gameInfo1_construct_array(p, n, size, ct);
}
static inline u32 gameInfoAddress() {
    return gameInfo1_getGameInfo();
}
static inline u32 savep() {
    return gabi::load<u32>(0x101F84DC);
}

static inline u32 vcall(u32 obj, u32 slot) {
    return gabi::call_ptr<u32>(GameInfoMemory1::ld(GameInfoMemory1::ld(obj) + slot), obj);
}

static constexpr u32 SS_VT = 0x1004B8C0;
static constexpr u32 FILE_ = 0x1004B9D0; /* "d_com_inf_game.cpp" */

/* stage data (play + 0x3EB0 = info + 0x5150) virtuals */
static inline u32 stage() {
    return gameInfoAddress() + 0x5150;
}
static inline u32 getStagInfo() {
    u32 s = stage();
    return vcall(s, 0x15C);
}
static inline u32 dSv_memBit_onTbox_l(u32 m, s32 no) {
    return gameInfo1_dSv_memBit_onTbox(m, no);
}
static inline u32 dSv_memBit_isTbox_l(u32 m, s32 no) {
    return gameInfo1_dSv_memBit_isTbox(m, no);
}
static inline u32 dSv_memBit_onDungeonItem_l(u32 m, s32 no) {
    return gameInfo1_dSv_memBit_onDungeonItem(m, no);
}
static inline u32 dSv_memBit_isDungeonItem_l(u32 m, s32 no) {
    return gameInfo1_dSv_memBit_isDungeonItem(m, no);
}
static inline u32 getStatusRoomDt_l(u32 rc, u32 roomNo) {
    return gameInfo1_getStatusRoomDt(rc, roomNo);
}
static inline u32 isEventBit_l(u32 ev, u32 flag) {
    return gameInfo1_isEventBit(ev, flag);
}
static inline u32 getEventReg_l(u32 ev, u32 reg) {
    return gameInfo1_getEventReg(ev, reg);
}
static inline void returnPlace_set_l(u32 p, u32 name, s32 room, u32 point) {
    gameInfo1_returnPlace_set(p, name, room, point);
}

/* sead::SafeString {top, vtable} */

/* inlined strcmp of two SafeStrings: equal if the same pointer or the same characters up to a
 * NUL; a run of 0x40001 equal non-NUL characters counts as different */
static inline bool ss_equal(u32 a, u32 b) {
    if (a == b)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = (u8)*GameInfoHD::view<be<u8>>(a + i);
        u8 cb = (u8)*GameInfoHD::view<be<u8>>(b + i);
        if (ca != cb)
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}
/* inlined strncpy(dst, src, n) */
static inline void strncpy_l(u32 dst, u32 src, u32 n) {
    u32 i = 0;
    for (; i < n; i++) {
        u8 c = (u8)*GameInfoHD::view<be<u8>>(src + i);
        *GameInfoHD::view<be<u8>>(dst + i) = c;
        if (c == 0)
            break;
    }
    for (; i < n; i++)
        *GameInfoHD::view<be<u8>>(dst + i) = 0;
}

// dComIfG_play_c::init (player pointers)
/* 0251FD14 */
void GameInfoHD::dComIfG_play_c::init() {
    this->mPlayerInfo[0].mpPlayer.v = 0;
    this->mPlayerInfo[0].mCameraID = 0xFF;
    for (u32 i = 0; i < 3; i++)
        this->mpPlayerPtr[i] = nullptr;
}

// dComIfG_play_c::ct
/* 0251FD3C */
void GameInfoHD::dComIfG_play_c::ct() {
    this->mpFlowerPacket = 0;
    this->mLkDArcIdx = 0xFF;
    this->mIkadaLinkPos.y = 0.0f;
    this->mShipRoomId = 0xFF;
    this->mIkadaLinkPos.z = 0.0f;
    this->mShipId = 0xFF;
    this->mpWoodPacket = 0;
    this->mIkadaShipBeforeRoomId = 0xFF;
    this->mParticle.v = 0;
    this->mIkadaShipId = 0xFF;
    GameInfoMemory1::st8(gabi::ea(this) + 0x4A8C, 0);
    this->mDlstWindowNum = 0;
    this->mpMagmaPacket = 0;
    this->mpTreePacket = 0;
    this->mIkadaLinkPos.x = 0.0f;
    this->mpGrassPacket = 0;
    GameInfoHD::view<GameInfoHD::dComIfG_play_c>(gabi::ea(this))->init();
}

// dComIfG_play_c::dComIfG_play_c (matcher: __sinit_d_com_inf_game_cpp)
/* 0251FD94 */
u32 hdGameInfo_dComIfG_play_c_ctor(u32 i_this) {
    if (i_this == 0) {
        i_this = operator_new_l(0x4A90);
        if (i_this == 0)
            return 0;
    }
    gameInfo1_backgroundConstructor(i_this); /* dBgS */
    GameInfoMemory1::st(i_this + 0x1400, 0x100436D8);
    gameInfo1_collisionConstructor(i_this + 0x1404); /* dCcS */
    gameInfo1_admConstructor(i_this + 0x3DF8);
    memzero_l(i_this + 0x3E94, 0xC);
    gameInfo1_nextStageConstructor(i_this + 0x3EA0);
    gameInfo1_stageDataConstructor(i_this + 0x3EB0);
    gameInfo1_eventControlConstructor(i_this + 0x3F30); /* dEvt_control_c */
    gameInfo1_eventManagerConstructor(i_this + 0x4024); /* dEvent_manager_c */
    gameInfo1_attentionConstructor(i_this + 0x4564);    /* dAttention_c */
    gameInfo1_vibrationConstructor(i_this + 0x46FC);    /* dVibration_c */
    gameInfo1_detectConstructor(i_this + 0x4780);       /* dDetect_c */
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpMsgArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpDmsgArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mTactMsgArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpMenuArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpFont0Archive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpFont1Archive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpAnmArchive = 0;
    /* sead::FixedSafeString<8> at +0x47B0 (inline constructors allocate when null) */
    u32 fs = i_this + 0x47B0;
    bool ok = true;
    if (fs == 0) {
        fs = operator_new_l(0x14);
        ok = fs != 0;
    }
    if (ok) {
        u32 b = fs;
        bool bok = true;
        if (b == 0) {
            b = operator_new_l(0xC);
            bok = b != 0;
        }
        if (bok) {
            GameInfoHD::view<GameInfoHD::FixedSafeString8>(b)->mStringTop = fs + 0xC;
            GameInfoHD::view<GameInfoHD::FixedSafeString8>(b)->__vtbl = 0x1004B8D8;
            GameInfoHD::view<GameInfoHD::FixedSafeString8>(b)->mBufferSize = 8;
            GameInfoHD::view<GameInfoHD::FixedSafeString8>(fs)->mBuffer[7] = 0;
        }
        u32 top = (u32)GameInfoHD::view<GameInfoHD::FixedSafeString8>(fs)->mStringTop;
        GameInfoHD::view<GameInfoHD::FixedSafeString8>(fs)->__vtbl = 0x1004B948;
        *GameInfoHD::view<be<u8>>(top) = 0;
        GameInfoHD::view<GameInfoHD::FixedSafeString8>(fs)->__vtbl = 0x1004B960;
    }
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpLkDArc = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mDlstWindowNum = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mLkDArcIdx = 0xFF;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mpGrassPacket = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mpMagmaPacket = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpWindResArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpActionIconArchive = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mParticle.v = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpEnglishTextArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpHyruleTextArchive = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mpFlowerPacket = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mpTreePacket = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpOptionResArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpDmapResArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpSaveResArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpItemIconArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpCameraResArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpNameResArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpFmapArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpSwimResArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpErrorResArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpScopeResArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpItemResArchive = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mpWoodPacket = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpFmapResArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpClothResArchive = 0;
    GameInfoHD::view<GameInfoHD::PlayArchives>(i_this)->mpCollectResArchive = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mStatus = 0;
    memzero_l(i_this + 0x48A0, 0xF0);
    /* sead::WFixedSafeString<17> at +0x4950 */
    u32 ws = i_this + 0x48A0 + 0xB0;
    bool wok = true;
    if (ws == 0) {
        ws = operator_new_l(0x30);
        wok = ws != 0;
    }
    if (wok) {
        GameInfoHD::view<GameInfoHD::WFixedSafeString17>(ws)->mBuffer[16] = 0;
        GameInfoHD::view<GameInfoHD::WFixedSafeString17>(ws)->mStringTop = ws + 0xC;
        GameInfoHD::view<GameInfoHD::WFixedSafeString17>(ws)->mBuffer[0] = 0;
        GameInfoHD::view<GameInfoHD::WFixedSafeString17>(ws)->__vtbl = 0x1004B990;
        GameInfoHD::view<GameInfoHD::WFixedSafeString17>(ws)->mBufferSize = 0x11;
    }
    memzero_l(i_this + 0x4990, 0x30);
    memzero_l(i_this + 0x49C0, 0x72);
    GameInfoMemory1::st(i_this + 0x4A34, 0);
    memzero_l(i_this + 0x4A48, 8);
    GameInfoHD::view<dComIfG_play_c>(i_this)->mCurrentViewport = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mCurrentWindow = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mIkadaShipBeforeRoomId = 0xFF;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mIkadaShipId = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mTimerInfo.mTimerLimitTimeMs = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mTimerInfo.mTimerNowTimeMs = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mTimerInfo.mTimerMode = 0xFFFFFFFF;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mCurrentGrafPort = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mCurrentView = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mShipRoomId = 0xFF;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mTimerInfo.mTimerPtr = 0;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mShipId = 0xFF;
    GameInfoHD::view<dComIfG_play_c>(i_this)->mTimerInfo.mTimerType = 0;
    memzero_l(i_this + 0x4A78, 0xC);
    GameInfoHD::view<dComIfG_play_c>(i_this)->mpFmapData = 0;
    GameInfoMemory1::st8(i_this + 0x4A8C, 0);
    GameInfoHD::view<dComIfG_play_c>(i_this)->mpItemTable = 0;
    GameInfoHD::view<GameInfoHD::dComIfG_play_c>(i_this)->ct();
    return i_this;
}

// dComIfG_inf_c::ct
/* 02520020 */
void hdGameInfo_dComIfG_inf_c_ct(u32 i_this) {
    GameInfoMemory1::st8(i_this + 0x62F1, 0xFF);
    GameInfoHD::view<GameInfoHD::dComIfG_play_c>(i_this + 0x12A0)->ct();
}

// dComIfG_inf_c::dComIfG_inf_c (HD 0x62F8)
/* 02520030 */
u32 hdGameInfo_dComIfG_inf_c_ctor(u32 i_this) {
    if (i_this == 0) {
        i_this = operator_new_l(0x62F8);
        if (i_this == 0)
            return 0;
    }
    memzero_l(i_this, 0x12A0);
    construct_array_l(i_this + 0x380, 0x10, 0x24, 0x02525ADC);
    gameInfo1_memoryInit(i_this + 0x778); /* dSv_memory_c::init */
    construct_array_l(i_this + 0x7A8, 0x20, 0x4C, 0x025B9774);
    hdGameInfo_dComIfG_play_c_ctor(i_this + 0x12A0);
    gameInfo1_drawListConstructor(i_this + 0x5D30); /* dDlst_list_c */
    GameInfoMemory1::st8(i_this + 0x62F1, 0xFF);
    hdGameInfo_dComIfG_inf_c_ct(i_this);
    return i_this;
}

// HD accessor of g_dComIfG_gameInfo (function-local static, constructed on first use)
/* 025200D4 */
u32 hdGameInfo_dComIfG_getGameInfo() {
    if (GameInfoMemory1::ld(0x104753A8) == 0) {
        GameInfoMemory1::st(0x104753A8, 1);
        hdGameInfo_dComIfG_inf_c_ctor(0x1046F0B0);
        __register_global_object(0x101D5DE8);
    }
    return 0x1046F0B0;
}

/* 0252012C */
s32 hdGameInfo_dComIfGp_setNextStage(u32 i_stageName, u32 i_point, u32 i_roomNo, u32 i_layer,
                                     f32 i_lastSpeed, u32 i_lastMode, u32 i_setPoint, u32 i_wipe) {
    u32 ns = gameInfoAddress() + 0x5140; /* dStage_nextStage_c */
    if (gameInfo1_setNextStage(ns, i_stageName, i_roomNo, i_point, i_layer, i_wipe) == 0)
        return 0;
    u32 player = (u32)GameInfoHD::view<dComIfG_inf_c>(gameInfoAddress())->play.mpPlayerPtr[0].v;
    if (player != 0) {
        u32 flags = GameInfoMemory1::ld(player + 0x3BC);
        if (flags & 1)
            i_lastMode |= 0x8000;
        i_lastMode |= (u32)(s32)GameInfoMemory1::lds16(player + 0x699E) << 16;
        if (flags & 0x8000)
            i_lastMode |= 0x4000;
    }
    u32 sv = savep();
    GameInfoHD::view<GameInfoHD::SaveStorage>(sv)->mInfo.mRestart.mLastSpeedF = i_lastSpeed;
    GameInfoHD::view<GameInfoHD::SaveStorage>(sv)->mInfo.mRestart.mLastMode = i_lastMode;
    if (i_setPoint != 0)
        GameInfoHD::view<GameInfoHD::SaveStorage>(savep())->mInfo.mRestart.mStartCode =
            (u16)i_point;
    return 1;
}

/* 02520230 */
s32 hdGameInfo_dComIfG_changeOpeningScene(u32 i_scene, u32 i_procName) {
    GameInfoHD::view<dComIfG_inf_c>(gameInfoAddress())->play.mNextStage.enable =
        0; /* dComIfGp_offEnableNextStage */
    hdGameInfo_dComIfGp_setNextStage(gabi::guest_addr("sea_T"), 0, 0x2C, 0, 0.0f, 0, 1, 0);
    u32 ns = gameInfoAddress() + 0x5140;
    s32 room = (s8)GameInfoHD::view<dComIfG_inf_c>(gameInfoAddress())->play.mNextStage.mRoomNo;
    s32 layer = (s8)GameInfoHD::view<dComIfG_inf_c>(gameInfoAddress())->play.mNextStage.mLayer;
    gameInfo1_setSceneName(ns, room, layer); /* mDoAud_setSceneName */
    GameInfoHD::view<GameInfoHD::SaveStorage>(savep())->mInfo.mRestart.mRestartParam = 0;
    gameInfo1_sceneChangeRequest(i_scene, i_procName, 0, 0x1E, 1); /* fopScnM_ChangeReq */
    gameInfo1_sceneReRequest(i_procName, 0);                       /* fopScnM_ReRequest */
    return 1;
}

/* 025202F8 */
s32 hdGameInfo_dComIfG_resetToOpening(u32 i_scene) {
    if (GameInfoMemory1::ld(GameInfoMemory1::ld(0x101F4974)) == 0)
        return 0;
    hdGameInfo_dComIfG_changeOpeningScene(i_scene, 8);
    gameInfo1_stopBgm(0x1E);
    gameInfo1_audioResetProcess();
    return 1;
}

// HD hdGameInfo_dComIfG_setObjectRes(name) through a SafeString
/* 02520354 */
u32 hdGameInfo_dComIfG_setObjectRes(u32 i_arcName, u32 p2, u32 p3) {
    gabi::Local<SafeString> ss;
    ss->mStringTop = i_arcName;
    ss->__vtbl = SS_VT;
    return gameInfo1_setObjectArchive(GameInfoMemory1::ld(0x101F4F54), gabi::ea(ss.get()), 0);
}

// gameInfo_phase_1
/* 02520394 */
s32 hdGameInfo_phase_1(u32 i_arcName) {
    s32 r = 2;
    if (hdGameInfo_dComIfG_setObjectRes(i_arcName, 0, 0) == 0)
        r = 5;
    return r;
}

// HD hdGameInfo_dComIfG_syncObjectRes(name)
/* 025203D8 */
s32 hdGameInfo_dComIfG_syncObjectRes(u32 i_arcName) {
    gabi::Local<SafeString> ss;
    ss->mStringTop = i_arcName;
    ss->__vtbl = SS_VT;
    return gameInfo1_syncObjectArchive(GameInfoMemory1::ld(0x101F4F54), gabi::ea(ss.get()));
}

// gameInfo_phase_2
/* 02520414 */
s32 hdGameInfo_phase_2(u32 i_arcName) {
    s32 r = hdGameInfo_dComIfG_syncObjectRes(i_arcName);
    if (r < 0)
        return 5;
    return r > 0 ? 0 : 2;
}

// gameInfo_dComIfG_resLoad = dComLbG_PhaseHandler(phase, l_method {gameInfo_phase_1,
// gameInfo_phase_2, phase_3}, name)
/* 02520460 */
s32 hdGameInfo_dComIfG_resLoad(u32 i_phase, u32 i_arcName) {
    if ((u32)GameInfoHD::view<request_of_phase_process_class>(i_phase)->id == 2)
        return 4;
    return gameInfo1_phaseHandler(i_phase, 0x101D5DF4, i_arcName);
}

// HD hdGameInfo_dComIfG_deleteObjectRes(name) (matcher: dRes_control_c::deleteRes)
/* 02520488 */
s32 hdGameInfo_dComIfG_deleteObjectRes(u32 i_arcName) {
    gabi::Local<SafeString> ss;
    ss->mStringTop = i_arcName;
    ss->__vtbl = SS_VT;
    gameInfo1_deleteObjectArchive(GameInfoMemory1::ld(0x101F4F54), gabi::ea(ss.get()));
    return 1;
}

/* 025204C8 */
s32 hdGameInfo_dComIfG_resDelete(u32 i_phase, u32 i_resName) {
    if ((u32)GameInfoHD::view<request_of_phase_process_class>(i_phase)->id == 1)
        JUT_ASSERT_l(FILE_, 0x515, gabi::guest_addr("i_phase->id != 1"));
    if ((u32)GameInfoHD::view<request_of_phase_process_class>(i_phase)->id == 2) {
        hdGameInfo_dComIfG_deleteObjectRes(i_resName);
        GameInfoHD::view<request_of_phase_process_class>(i_phase)->id = 0;
    }
    return 0;
}

/* 02520540 */
u32 hdGameInfo_dComIfGp_getReverb(u32 i_roomNo) {
    u32 info = vcall(stage(), 0x64);
    return gameInfo1_getReverbStage(info, i_roomNo);
}

/* 02520584 */
u32 hdGameInfo_dComIfGp_getShip(u32 i_roomNo, u32 i_id) {
    u32 roomDt = getStatusRoomDt_l(gameInfoAddress() + 0x51CC, i_roomNo);
    if (roomDt == 0)
        return 0;
    u32 ship = vcall(roomDt, 0x1FC);
    if (ship == 0)
        return 0;
    s32 n = (s32)GameInfoHD::view<GameInfoHD::EntryList>(ship)->num;
    if (n <= 0)
        return 0;
    if (i_id == 0xFF)
        return 0;
    u32 p = (u32)GameInfoHD::view<GameInfoHD::EntryList>(ship)->m_entries;
    if (p == 0)
        return 0;
    for (s32 i = 0; i < n; i++, p += 0x10)
        if (i_id == (u8)GameInfoHD::view<GameInfoHD::Ship>(p)->field_0xe)
            return p;
    return 0;
}

/* 02520630 */
u32 hdGameInfo_dComIfGp_getMapTrans(u32 i_roomNo, u32 o_transX, u32 o_transY, u32 o_angle) {
    u32 multi = vcall(stage(), 0x20C);
    if (multi == 0)
        return 0;
    s32 n = (s32)GameInfoHD::view<GameInfoHD::EntryList>(multi)->num;
    u32 p = (u32)GameInfoHD::view<GameInfoHD::EntryList>(multi)->m_entries;
    for (s32 i = 0; i < n; i++, p += 0xC) {
        if (i_roomNo == (u8)GameInfoHD::view<GameInfoHD::Multi>(p)->mRoomNo) {
            GameInfoMemory1::stf(o_transX, (f32)GameInfoHD::view<GameInfoHD::Multi>(p)->mTransX);
            GameInfoMemory1::stf(o_transY, (f32)GameInfoHD::view<GameInfoHD::Multi>(p)->mTransY);
            GameInfoMemory1::st16(o_angle,
                                  (u16)(s16)GameInfoHD::view<GameInfoHD::Multi>(p)->mAngle);
            return 1;
        }
    }
    return 0;
}

/* 02520708 */
u32 hdGameInfo_dComIfGp_getRoomCamera(u32 i_roomNo) {
    u32 roomDt = getStatusRoomDt_l(gameInfoAddress() + 0x51CC, i_roomNo);
    if (roomDt == 0)
        return 0;
    return vcall(roomDt, 0x24);
}

/* 02520770 */
u32 hdGameInfo_dComIfGp_getRoomArrow(u32 i_roomNo) {
    u32 roomDt = getStatusRoomDt_l(gameInfoAddress() + 0x51CC, i_roomNo);
    if (roomDt == 0)
        return 0;
    return vcall(roomDt, 0x34);
}

static inline u32 curStageNo() {
    return ((u8)GameInfoHD::view<GameInfoHD::StageInfo>(getStagInfo())->mSaveTableBits >> 1) & 0x7F;
}

/* 025207D8 */
void hdGameInfo_dComIfGs_onStageTbox(u32 i_stageNo, u32 i_no) {
    if (i_stageNo == curStageNo())
        dSv_memBit_onTbox_l(savep() + 0x798, i_no);
    dSv_memBit_onTbox_l(savep() + 0x3A0 + i_stageNo * 0x24, i_no);
}

/* 02520864 */
u32 hdGameInfo_dComIfGs_isStageTbox(u32 i_stageNo, u32 i_no) {
    if (i_stageNo == curStageNo())
        return dSv_memBit_isTbox_l(savep() + 0x798, i_no);
    return dSv_memBit_isTbox_l(savep() + 0x3A0 + i_stageNo * 0x24, i_no);
}

static inline u32 isStageDungeonItem(u32 i_stageNo, s32 item) {
    u32 cur = curStageNo();
    u32 sv = savep();
    if (i_stageNo == cur)
        return dSv_memBit_isDungeonItem_l(sv + 0x798, item);
    return dSv_memBit_isDungeonItem_l(sv + 0x3A0 + i_stageNo * 0x24, item);
}

// HD: out-of-line gameInfo_dComIfGs_isStageMap
/* 025208F8 */
u32 hdGameInfo_dComIfGs_isStageMap(u32 i_stageNo) {
    return isStageDungeonItem(i_stageNo, dSv_memBit_c::MAP);
}

// HD: out-of-line gameInfo_dComIfGs_isStageCompass
/* 0252097C */
u32 hdGameInfo_dComIfGs_isStageCompass(u32 i_stageNo) {
    return isStageDungeonItem(i_stageNo, dSv_memBit_c::COMPASS);
}

// HD: out-of-line gameInfo_dComIfGs_isStageBossKey
/* 02520A00 */
u32 hdGameInfo_dComIfGs_isStageBossKey(u32 i_stageNo) {
    return isStageDungeonItem(i_stageNo, dSv_memBit_c::BOSS_KEY);
}

/* 02520A84 */
u32 hdGameInfo_dComIfGs_isStageBossEnemy(u32 i_stageNo) {
    return isStageDungeonItem(i_stageNo, dSv_memBit_c::STAGE_BOSS_ENEMY);
}

/* 02520B08 */
void hdGameInfo_dComIfGs_onStageLife(u32 i_stageNo) {
    if (i_stageNo == curStageNo())
        dSv_memBit_onDungeonItem_l(savep() + 0x798, dSv_memBit_c::STAGE_LIFE);
    dSv_memBit_onDungeonItem_l(savep() + 0x3A0 + i_stageNo * 0x24, dSv_memBit_c::STAGE_LIFE);
}

/* 02520B88 */
u32 hdGameInfo_dComIfGs_isStageLife(u32 i_stageNo) {
    return isStageDungeonItem(i_stageNo, dSv_memBit_c::STAGE_LIFE);
}

/* dComIfGs_getItem(i) as inlined in HD: item slots save+0x5C (0..0x14), bag slots (0x18..0x1F at
 * save+0x96, 0x24..0x2B at save+0x9E, 0x30..0x37 at save+0xA6), 0xFF otherwise */
static inline u8 getItem_l(u32 sv, s32 i) {
    if (i < 0x15)
        return (u8)GameInfoHD::view<GameInfoHD::SaveStorage>(sv)
            ->mInfo.mSavedata.mPlayer.mPlayerItem.mItems[i];
    if (i < 0x18)
        return 0xFF;
    if (i < 0x20)
        return (u8)GameInfoHD::view<GameInfoHD::SaveStorage>(sv)
            ->mInfo.mSavedata.mPlayer.mBagItem.mBeast[i - 0x18];
    if (i < 0x24)
        return 0xFF;
    if (i < 0x2C)
        return (u8)GameInfoHD::view<GameInfoHD::SaveStorage>(sv)
            ->mInfo.mSavedata.mPlayer.mBagItem.mBait[i - 0x24];
    if ((u32)(i - 0x30) < 8)
        return (u8)GameInfoHD::view<GameInfoHD::SaveStorage>(sv)
            ->mInfo.mSavedata.mPlayer.mBagItem.mReserve[i - 0x30];
    return 0xFF;
}
static inline u32 collect() {
    return savep() + 0xD4;
}
static inline u32 isTact_l(s32 i) {
    return gameInfo1_isTact(collect(), i);
}
static inline u32 isTriforce_l(s32 i) {
    return gameInfo1_isTriforce(collect(), i);
}
static inline u32 isSymbol_l(s32 i) {
    return gameInfo1_isSymbol(collect(), i);
}
static inline u32 isCollect_l(s32 i, s32 j) {
    return gameInfo1_isCollect(collect(), i, j);
}

/* switch cases shared by checkGetItem / checkGetItemNum: -1 when not one of them */
static inline s32 checkSpecial(u32 item) {
    if (item == GameInfoHD::dItemNo_PIRATES_CHARM_e)
        return isCollect_l(3, 0) != 0;
    if (item == GameInfoHD::dItemNo_HEROS_CHARM_e)
        return isCollect_l(4, 0) != 0;
    if (item >= GameInfoHD::dItemNo_TRIFORCE1_e && item <= GameInfoHD::dItemNo_SONG_OF_PASSING_e) {
        u32 k = item - GameInfoHD::dItemNo_TRIFORCE1_e;
        if (k < 8)
            return isTriforce_l(k) != 0;
        if (k < 0xB)
            return isSymbol_l(k - 8) != 0;
        if (k >= 0xC)
            return isTact_l(k - 0xC) != 0;
    }
    return -1;
}

/* 02520C0C */
u8 hdGameInfo_dComIfGs_checkGetItem(u32 i_itemNo) {
    s32 sp = checkSpecial(i_itemNo);
    if (sp >= 0)
        return (u8)sp;
    u8 get_item = 0;
    u32 sv = savep();
    for (s32 i = 0; i < 0x3C; i++)
        if (getItem_l(sv, i) == i_itemNo)
            get_item++;
    for (u32 i = 0; i < 3; i++)
        if ((u8)GameInfoHD::view<GameInfoHD::SaveStorage>(sv)
                ->mInfo.mSavedata.mPlayer.mPlayerStatusA.mSelectEquip[i] == i_itemNo)
            get_item++;
    if (i_itemNo - 0xBF < 0x40 && gameInfo1_isGetCollectMap(sv + 0xE4, 0xFE - i_itemNo) != 0)
        get_item++;
    return get_item;
}

/* 02520FE4 */
u8 hdGameInfo_dComIfGs_checkGetItemNum(u32 i_itemNo) {
    s32 sp = checkSpecial(i_itemNo);
    if (sp >= 0)
        return (u8)sp;
    u8 get_item = 0;
    if (i_itemNo == GameInfoHD::dItemNo_BOW_e) { /* bow: arrows */
        u32 sv = savep();
        if ((u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerItem.mItems[12] != 0)
            get_item =
                (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mItemRecord.mItemRecord2.mArrowNum;
        return get_item;
    }
    if (i_itemNo == GameInfoHD::dItemNo_BOMB_BAG_e) { /* bomb bag: bombs */
        u32 sv = savep();
        if ((u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerItem.mItems[13] != 0)
            get_item =
                (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mItemRecord.mItemRecord2.mBombNum;
        return get_item;
    }
    s32 beast = -1;
    if (i_itemNo >= GameInfoHD::dItemNo_SKULL_NECKLACE_e &&
        i_itemNo <= GameInfoHD::dItemNo_BLUE_JELLY_e)
        beast = i_itemNo - GameInfoHD::dItemNo_SKULL_NECKLACE_e;
    else if (i_itemNo == GameInfoHD::dItemNo_JOY_PENDANT_e)
        beast = 7;
    if (beast >= 0) {
        u32 sv = savep();
        for (u32 i = 0; i < 8; i++)
            if ((u8)GameInfoHD::view<GameInfoHD::SaveStorage>(sv)
                    ->mInfo.mSavedata.mPlayer.mBagItem.mBeast[i] == i_itemNo)
                get_item = (u8)GameInfoHD::view<GameInfoHD::SaveStorage>(sv)
                               ->mInfo.mSavedata.mPlayer.mBagItemRecord.mBeastNum[beast];
        return get_item;
    }
    u32 sv = savep();
    for (s32 i = 0; i < 0x3C; i++)
        if (getItem_l(sv, i) == i_itemNo)
            get_item = 1;
    for (u32 i = 0; i < 3; i++)
        if ((u8)GameInfoHD::view<GameInfoHD::SaveStorage>(sv)
                ->mInfo.mSavedata.mPlayer.mPlayerStatusA.mSelectEquip[i] == i_itemNo)
            get_item = 1;
    if (i_itemNo - 0xBF < 0x40 && gameInfo1_isGetCollectMap(sv + 0xE4, 0xFE - i_itemNo) != 0)
        get_item = 1;
    return get_item;
}

/* 025215BC */
u32 hdGameInfo_getSceneList(s32 i_no) {
    u32 sclsInfo = vcall(stage(), 0x16C);
    if (sclsInfo == 0)
        JUT_ASSERT_l(0x1004BA40, 0x96D, gabi::guest_addr("sclsInfo != (0)"));
    if (!(i_no >= 0 && i_no < (s32)GameInfoHD::view<GameInfoHD::EntryList>(sclsInfo)->num))
        JUT_ASSERT_l(0x1004BA40, 0x96E, gabi::guest_addr("0 <= i_no && i_no < sclsInfo->num"));
    u32 sclsData = (u32)GameInfoHD::view<GameInfoHD::EntryList>(sclsInfo)->m_entries;
    if (sclsData == 0)
        JUT_ASSERT_l(0x1004BA40, 0x971, gabi::guest_addr("sclsData != (0)"));
    return sclsData + (u32)i_no * 0xC;
}

/* 02521678 */
u32 hdGameInfo_dComIfGd_getMeshSceneList(u32 i_vec) {
    f32 fx = (f32)GameInfoHD::view<cXyz>(i_vec)->x;
    f32 fz = (f32)GameInfoHD::view<cXyz>(i_vec)->z;
    s32 x = gabi::ftoi((fx + 350000.0f) * 2e-05f);
    s32 z = gabi::ftoi((fz + 350000.0f) * 2e-05f);
    if (x < 0)
        x = 0;
    else if (x > 13)
        x = 13;
    if (z < 0)
        z = 0;
    else if (z > 13)
        z = 13;
    return hdGameInfo_getSceneList((x & 1) + ((x >> 1) + (z >> 1) * 7) * 4 + (z & 1) * 2);
}

// l_landingEvent at 101D5E00
/* 0252174C */
u32 hdGameInfo_dComIfGs_checkSeaLandingEvent(u32 i_roomNo) {
    u32 e = 0x101D5E00;
    for (u32 i = 0; i < 6; i++, e += 4) {
        if (i_roomNo == (u32)(s32)(s8)GameInfoHD::view<GameInfoHD::LandingEvent>(e)->roomNo &&
            isEventBit_l(savep() + 0x644,
                         (u16)GameInfoHD::view<GameInfoHD::LandingEvent>(e)->event) == 0)
            return 0;
    }
    return 1;
}

/* SafeString temporaries compared as in the HD strcmp: a's assureTermination twice (the first
 * devirtualised to f), then b's */
static inline bool ss_cmp_eq(u32 a, u32 b, u32 first_direct) {
    if (first_direct)
        gabi::call(first_direct, a);
    else
        gabi::call_ptr(GameInfoMemory1::ld((u32)GameInfoHD::view<SafeString>(a)->__vtbl + 0x14), a);
    gabi::call_ptr(GameInfoMemory1::ld((u32)GameInfoHD::view<SafeString>(a)->__vtbl + 0x14), a);
    u32 ta = (u32)GameInfoHD::view<SafeString>(a)->mStringTop;
    gabi::call_ptr(
        GameInfoMemory1::ld((u32)GameInfoHD::view<GameInfoHD::FixedSafeString8>(b)->__vtbl + 0x14),
        b);
    u32 tb = (u32)GameInfoHD::view<GameInfoHD::FixedSafeString8>(b)->mStringTop;
    return ss_equal(ta, tb);
}

// l_checkData at 101D5E18 (0xE-byte entries)
/* 025217F8 */
void hdGameInfo_dComIfGs_setGameStartStage() {
    gabi::Local<u8[8]> name;
    u32 buf = gabi::ea(name.get());
    u32 d = 0x101D5E18;
    for (u32 i = 0; i < 5; i++, d += 0xE) {
        if ((u8)GameInfoHD::view<GameInfoHD::StartCheck>(d)->mbHasEvent == 1 &&
            isEventBit_l(savep() + 0x644,
                         (u16)GameInfoHD::view<GameInfoHD::StartCheck>(d)->mEvent) != 0)
            break;
    }
    bool emptyName;
    {
        gabi::Local<SafeString> a;
        gabi::Local<SafeString> b;
        u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get());
        GameInfoHD::view<SafeString>(aa)->__vtbl = SS_VT;
        GameInfoHD::view<SafeString>(bb)->mStringTop = gabi::guest_addr("");
        GameInfoHD::view<SafeString>(aa)->mStringTop = d + 4;
        GameInfoHD::view<SafeString>(bb)->__vtbl = SS_VT;
        /* the first assureTermination: direct call to the inline copy, then the virtual */
        gameInfo1_assureTermination(aa);
        gabi::call_ptr(GameInfoMemory1::ld((u32)GameInfoHD::view<SafeString>(aa)->__vtbl + 0x14),
                       aa);
        u32 ta = (u32)GameInfoHD::view<SafeString>(aa)->mStringTop;
        gabi::call_ptr(GameInfoMemory1::ld((u32)GameInfoHD::view<SafeString>(bb)->__vtbl + 0x14),
                       bb);
        u32 tb = (u32)GameInfoHD::view<SafeString>(bb)->mStringTop;
        emptyName = ss_equal(ta, tb);
    }
    if (!emptyName) {
        strncpy_l(buf, d + 4, 8);
        s32 room = (s8)GameInfoHD::view<GameInfoHD::StartCheck>(d)->mRoomNo;
        u32 point = (u8)GameInfoHD::view<GameInfoHD::StartCheck>(d)->mStartCode;
        returnPlace_set_l(savep() + 0x50, buf, room, point);
        return;
    }
    u32 stage_type =
        ((u32)GameInfoHD::view<GameInfoHD::StageInfo>(getStagInfo())->mStageTypeBits >> 16) & 7;
    u32 stageNo = curStageNo();
    bool pship;
    {
        gabi::Local<SafeString> a;
        gabi::Local<SafeString> b;
        u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get());
        GameInfoHD::view<SafeString>(aa)->__vtbl = SS_VT;
        GameInfoHD::view<SafeString>(aa)->mStringTop = gabi::guest_addr("PShip");
        u32 g = gameInfoAddress();
        GameInfoHD::view<SafeString>(bb)->mStringTop = g + 0x5134;
        GameInfoHD::view<SafeString>(bb)->__vtbl = SS_VT;
        pship = ss_cmp_eq(aa, bb, 0);
    }
    u32 sea = 0x1004BA60; /* "sea" */
    if (pship) {
        strncpy_l(buf, sea, 8);
        s32 room = (s8)getEventReg_l(savep() + 0x644, 0xC3FF);
        u32 point = getEventReg_l(savep() + 0x644, 0x85FF);
        gameInfo1_setNextTime(120.0f); /* dKy_set_nexttime */
        returnPlace_set_l(savep() + 0x50, buf, room, point);
        return;
    }
    if (stage_type == 7) { /* sea */
        u32 player = (u32)GameInfoHD::view<dComIfG_inf_c>(gameInfoAddress())->play.mpPlayerPtr[0].v;
        s32 room = GameInfoMemory1::lds8(player + 0x326);
        u32 point = GameInfoMemory1::ld8(player + 0x69E7);
        if (room >= 0 && point != 0xFF &&
            hdGameInfo_dComIfGs_checkSeaLandingEvent((u32)room) != 0) {
            u32 g = gameInfoAddress();
            strncpy_l(buf, g + 0x5134, 7);
            *GameInfoHD::view<be<u8>>(buf + 7) = 0;
            returnPlace_set_l(savep() + 0x50, buf, room, point);
            return;
        }
        u32 ship = (u32)GameInfoHD::view<dComIfG_inf_c>(gameInfoAddress())->play.mpPlayerPtr[2].v;
        u32 scls = hdGameInfo_dComIfGd_getMeshSceneList(ship != 0 ? ship + 0x314 : player + 0x314);
        strncpy_l(buf, scls, 8);
        s32 sroom = (s8)GameInfoHD::view<GameInfoHD::SceneList>(scls)->mRoom;
        returnPlace_set_l(savep() + 0x50, buf, sroom,
                          (u8)GameInfoHD::view<GameInfoHD::SceneList>(scls)->mStart);
        return;
    }
    if (stage_type == 1 || stage_type == 3 || stage_type == 6 || stage_type == 8 || stageNo == 9) {
        u32 scls = hdGameInfo_getSceneList(0);
        strncpy_l(buf, scls, 8);
        s32 sroom = (s8)GameInfoHD::view<GameInfoHD::SceneList>(scls)->mRoom;
        returnPlace_set_l(savep() + 0x50, buf, sroom,
                          (u8)GameInfoHD::view<GameInfoHD::SceneList>(scls)->mStart);
        return;
    }
    if (stageNo == 0xA) { /* ship: the raft's position before boarding */
        gabi::Local<u32[3]> pos;
        u32 p = gabi::ea(pos.get());
        u32 g = gameInfoAddress();
        GameInfoMemory1::st(
            p + 0, GameInfoHD::wordBits(GameInfoHD::view<dComIfG_inf_c>(g)->play.mIkadaLinkPos.x));
        GameInfoMemory1::st(
            p + 4, GameInfoHD::wordBits(GameInfoHD::view<dComIfG_inf_c>(g)->play.mIkadaLinkPos.y));
        GameInfoMemory1::st(
            p + 8, GameInfoHD::wordBits(GameInfoHD::view<dComIfG_inf_c>(g)->play.mIkadaLinkPos.z));
        u32 scls = hdGameInfo_dComIfGd_getMeshSceneList(p);
        strncpy_l(buf, scls, 8);
        s32 sroom = (s8)GameInfoHD::view<GameInfoHD::SceneList>(scls)->mRoom;
        returnPlace_set_l(savep() + 0x50, buf, sroom,
                          (u8)GameInfoHD::view<GameInfoHD::SceneList>(scls)->mStart);
        return;
    }
    if (stageNo - 0xB <= 2) {
        strncpy_l(buf, sea, 8);
        u32 mapInfo = vcall(stage(), 0x74);
        if (mapInfo == 0)
            JUT_ASSERT_l(0x1004BA70, 0xA78, gabi::guest_addr("mapInfo != (0)"));
        u32 oz = gameInfo1_mapInfoOceanZ(mapInfo);
        u32 t = oz * 7 + 0x15;
        u32 ox = gameInfo1_mapInfoOceanX(mapInfo);
        s32 room = (s8)(t + (ox + 3) + 1);
        returnPlace_set_l(savep() + 0x50, buf, room, 0);
        return;
    }
    strncpy_l(buf, sea, 8);
    returnPlace_set_l(savep() + 0x50, buf, 0xB, 0);
}

/* 02522198 */
void hdGameInfo_dComIfGs_gameStart() {
    GameInfoHD::view<dComIfG_inf_c>(gameInfoAddress())->play.mNextStage.enable = 0;
    u32 sv = savep();
    s32 room = (s8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mReturnPlace.mRoomNo;
    u32 point = (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mReturnPlace.mPoint;
    hdGameInfo_dComIfGp_setNextStage(sv + 0x50, point, (u32)room, 0xFFFFFFFF, 0.0f, 0, 1, 0);
}

// the boss stage's recollection table: DRC 3 -> 0, FW 4 -> 1, ET 6 -> 2, WT 7 -> 3
/* 025221F0 */
void hdGameInfo_dComIfGs_copyPlayerRecollectionData() {
    if ((((u32)GameInfoHD::view<GameInfoHD::StageInfo>(getStagInfo())->mStageTypeBits >> 16) & 7) !=
        3)
        return;
    u32 tbl;
    if (curStageNo() == 3)
        tbl = 0;
    else if (curStageNo() == 4)
        tbl = 1;
    else if (curStageNo() == 6)
        tbl = 2;
    else if (curStageNo() == 7)
        tbl = 3;
    else
        return;
    gabi::Local<u8[0x70]> stts;
    u32 b = gabi::ea(stts.get());
    memcpy_l(b + 0, savep() + 0x20, 0x18);
    memcpy_l(b + 0x18, savep() + 0x5C, 0x15);
    memcpy_l(b + 0x2D, savep() + 0x88, 3);
    memcpy_l(b + 0x30, savep() + 0x8E, 3);
    memcpy_l(b + 0x33, savep() + 0x96, 0x18);
    memcpy_l(b + 0x4B, savep() + 0xBC, 0x18);
    memcpy_l(b + 0x63, savep() + 0xD4, 0xD);
    memcpy_l(savep() + 0x1DC + tbl * 0x70, b, 0x70);
}

/* 02522398 */
void hdGameInfo_dComIfGs_setSelectEquip(u32 i_type, u32 i_itemNo) {
    if (i_type > 2) {
        GameInfoHD::view<GameInfoHD::SaveStorage>(savep())
            ->mInfo.mSavedata.mPlayer.mPlayerStatusA.mSelectEquip[i_type] = (u8)i_itemNo;
        return;
    }
    s32 bit = -1;
    if (i_type == 0) {
        if (i_itemNo >= GameInfoHD::dItemNo_SWORD_e &&
            i_itemNo <= GameInfoHD::dItemNo_MASTER_SWORD_2_e)
            bit = GameInfoMemory1::ld8(0x1004BA5C + i_itemNo);
        else if (i_itemNo == GameInfoHD::dItemNo_MASTER_SWORD_3_e)
            bit = 3;
    } else if (i_type == 1) {
        if (i_itemNo == GameInfoHD::dItemNo_SHIELD_e)
            bit = 0;
        else if (i_itemNo == GameInfoHD::dItemNo_MIRROR_SHIELD_e)
            bit = 1;
    } else {
        if (i_itemNo == GameInfoHD::dItemNo_POWER_BRACELETS_e)
            bit = 0;
    }
    if (bit >= 0)
        gameInfo1_onCollect(savep() + 0xD4, i_type, bit); /* onCollect */
    GameInfoHD::view<GameInfoHD::SaveStorage>(savep())
        ->mInfo.mSavedata.mPlayer.mPlayerStatusA.mSelectEquip[i_type] = (u8)i_itemNo;
}

/* the recollected status: copy the current status into the play buffer (gi + 0x5C60) and the
 * recollection (save + 0x1DC + tbl * 0x70) into the current status */
static inline void saveStatusToBuffer(u32 buf) {
    memcpy_l(buf, savep() + 0x20, 0x18);
    buf += 0x18;
    memcpy_l(buf, savep() + 0x5C, 0x15);
    buf += 0x15;
    memcpy_l(buf, savep() + 0x88, 3);
    buf += 3;
    memcpy_l(buf, savep() + 0x8E, 3);
    buf += 3;
    memcpy_l(buf, savep() + 0x96, 0x18);
    buf += 0x18;
    memcpy_l(buf, savep() + 0xBC, 0x18);
    memcpy_l(buf + 0x18, savep() + 0xD4, 0xD);
}
static inline void loadStatusFromBuffer(u32 src) {
    memcpy_l(savep() + 0x20, src, 0x18);
    src += 0x18;
    memcpy_l(savep() + 0x5C, src, 0x15);
    src += 0x15;
    memcpy_l(savep() + 0x88, src, 3);
    src += 3;
    memcpy_l(savep() + 0x8E, src, 3);
    src += 3;
    memcpy_l(savep() + 0x96, src, 0x18);
    src += 0x18;
    memcpy_l(savep() + 0xBC, src, 0x18);
    memcpy_l(savep() + 0xD4, src + 0x18, 0xD);
}
static inline u32 pictureInfo() {
    return gameInfo1_getPictureInfo(savep() + 0x12C0);
}

/* 025224A4 */
void hdGameInfo_dComIfGs_setPlayerRecollectionData() {
    gameInfo1_setArrowKeepType(0); /* daArrow_c::setKeepType(TYPE_NORMAL) */
    const u32 names[4] = {0x1004BA98, 0x1004BAA0, 0x1004BAA8, 0x1004BAB0}; /* "Xboss0".."Xboss3" */
    const u16 events[4] = {0x3D80, 0x3D40, 0x3D20, 0x3D10};
    s32 tbl = -1;
    for (u32 k = 0; k < 4 && tbl < 0; k++) {
        gabi::Local<SafeString> a;
        gabi::Local<SafeString> b;
        u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get());
        GameInfoHD::view<SafeString>(aa)->mStringTop = names[k];
        GameInfoHD::view<SafeString>(aa)->__vtbl = SS_VT;
        u32 g = gameInfoAddress();
        GameInfoHD::view<SafeString>(bb)->mStringTop = g + 0x5134;
        GameInfoHD::view<SafeString>(bb)->__vtbl = SS_VT;
        if (ss_cmp_eq(aa, bb, 0) && isEventBit_l(savep() + 0x644, events[k]) != 0)
            tbl = (s32)k;
    }
    if (tbl < 0) {
        u32 sv = savep();
        GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerStatusA.mSelectItem[0] = 0xFF;
        GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mSelectItem[1] = 0xFF;
        GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mSelectItem[2] = 0xFF;
        return;
    }
    GameInfoHD::view<GameInfoHD::PlaySupplement>(gameInfoAddress() + 0x12A0)
        ->mPlayerInfoBufferStageNo = (u8)(tbl + 1); /* dComIfGp_setPlayerInfoBufferStageNo */
    u32 sv = savep();
    u32 off = (u32)tbl * 0x70;
    if (GameInfoMemory1::ld8(sv + 0x1F4 + off) != 0x20) { /* no telescope in the recollection */
        GameInfoHD::view<GameInfoHD::SaveStorage>(sv)
            ->mInfo.mSavedata.mPlayer.mPlayerStatusA.mSelectItem[0] = 0xFF;
        GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mSelectItem[1] = 0xFF;
        GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mSelectItem[2] = 0xFF;
        return;
    }
    u8 v34 = (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerStatusA.mMagic,
       v8f = (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mItemMax.mItemMax2.mArrowNum,
       v89 = (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mItemRecord.mItemRecord2.mArrowNum,
       v90 = (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mItemMax.mItemMax2.mBombNum;
    u8 maxLife = GameInfoMemory1::ld8(sv + 0x21),
       v33 = (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerStatusA.mMaxMagic,
       v8a = (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mItemRecord.mItemRecord2.mBombNum;
    u16 life = (u16)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerStatusA.mLife;
    u8 picNum = GameInfoMemory1::ld8(pictureInfo() + 0x3C030C);
    sv = savep();
    u8 eq[5];
    for (u32 i = 0; i < 5; i++)
        eq[i] = (u8)GameInfoHD::view<GameInfoHD::SaveStorage>(sv)
                    ->mInfo.mSavedata.mPlayer.mPlayerStatusA.mSelectEquip[i];
    u8 bot[4] = {(u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerItem.mItems[14],
                 (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerItem.mItems[15],
                 (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerItem.mItems[16],
                 (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerItem.mItems[17]};
    u8 cam = (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerItem.mItems[8];
    u8 col[5];
    for (u32 i = 0; i < 5; i++)
        col[i] = (u8)GameInfoHD::view<GameInfoHD::SaveStorage>(sv)
                     ->mInfo.mSavedata.mPlayer.mCollect.mCollect[i];
    saveStatusToBuffer(gameInfoAddress() + 0x5C60);
    loadStatusFromBuffer(savep() + 0x1DC + off);
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mMaxLife = maxLife;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mLife = life;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mMaxMagic = v33;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mMagic = v34;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mItemMax.mItemMax2.mArrowNum = v8f;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mItemRecord.mItemRecord2.mArrowNum = v89;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mItemMax.mItemMax2.mBombNum = v90;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mItemRecord.mItemRecord2.mBombNum = v8a;
    gameInfo1_setPictureNum(pictureInfo(), picNum);
    for (u32 i = 0; i < 5; i++)
        GameInfoHD::view<GameInfoHD::SaveStorage>(savep())
            ->mInfo.mSavedata.mPlayer.mCollect.mCollect[i] = col[i];
    for (u32 i = 0; i < 5; i++)
        hdGameInfo_dComIfGs_setSelectEquip(i, eq[i]);
    for (u32 i = 0; i < 5; i++) {
        u8 e = (u8)GameInfoHD::view<GameInfoHD::SaveStorage>(savep())
                   ->mInfo.mSavedata.mPlayer.mPlayerStatusA.mSelectEquip[i];
        GameInfoMemory1::st8(gameInfoAddress() + 0x5BC0 + i, e);
    }
    for (u32 i = 0; i < 4; i++)
        GameInfoHD::view<GameInfoHD::SaveStorage>(savep())
            ->mInfo.mSavedata.mPlayer.mPlayerItem.mItems[14 + i] = bot[i];
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerItem.mItems[8] = cam;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mSelectItem[0] = 0xFF;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mSelectItem[1] = 0xFF;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mSelectItem[2] = 0xFF;
}

/* 02522C60 */
void hdGameInfo_dComIfGs_revPlayerRecollectionData() {
    if ((u8)GameInfoHD::view<GameInfoHD::PlaySupplement>(gameInfoAddress() + 0x12A0)
            ->mPlayerInfoBufferStageNo == 0)
        return;
    gameInfoAddress();
    GameInfoHD::view<GameInfoHD::PlaySupplement>(gameInfoAddress() + 0x12A0)
        ->mPlayerInfoBufferStageNo = 0;
    u32 sv = savep();
    u8 v90 = (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mItemMax.mItemMax2.mBombNum,
       v33 = (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerStatusA.mMaxMagic,
       v8f = (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mItemMax.mItemMax2.mArrowNum;
    u16 life = (u16)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerStatusA.mLife;
    u8 v34 = (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerStatusA.mMagic;
    u16 maxLife = (u16)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerStatusA.mMaxLife;
    u8 v89 = (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mItemRecord.mItemRecord2.mArrowNum,
       v8a = (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mItemRecord.mItemRecord2.mBombNum;
    u8 picNum = GameInfoMemory1::ld8(pictureInfo() + 0x3C030C);
    sv = savep();
    u8 eq[5];
    for (u32 i = 0; i < 5; i++)
        eq[i] = (u8)GameInfoHD::view<GameInfoHD::SaveStorage>(sv)
                    ->mInfo.mSavedata.mPlayer.mPlayerStatusA.mSelectEquip[i];
    u8 col[5];
    for (u32 i = 0; i < 5; i++)
        col[i] = (u8)GameInfoHD::view<GameInfoHD::SaveStorage>(sv)
                     ->mInfo.mSavedata.mPlayer.mCollect.mCollect[i];
    u8 bot[4] = {(u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerItem.mItems[14],
                 (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerItem.mItems[15],
                 (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerItem.mItems[16],
                 (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerItem.mItems[17]};
    u8 cam = (u8)GameInfoHD::view<dSv_player_c>(sv + 0x20)->mPlayerItem.mItems[8];
    loadStatusFromBuffer(gameInfoAddress() + 0x5C60);
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mMaxLife = (u8)maxLife;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mLife = life;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mMaxMagic = v33;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mMagic = v34;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mItemMax.mItemMax2.mArrowNum = v8f;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mItemRecord.mItemRecord2.mArrowNum = v89;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mItemMax.mItemMax2.mBombNum = v90;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mItemRecord.mItemRecord2.mBombNum = v8a;
    gameInfo1_setPictureNum(pictureInfo(), picNum);
    for (u32 i = 0; i < 5; i++)
        GameInfoHD::view<GameInfoHD::SaveStorage>(savep())
            ->mInfo.mSavedata.mPlayer.mCollect.mCollect[i] = col[i];
    for (u32 i = 0; i < 5; i++)
        hdGameInfo_dComIfGs_setSelectEquip(i, eq[i]);
    for (u32 i = 0; i < 5; i++) {
        u8 e = (u8)GameInfoHD::view<GameInfoHD::SaveStorage>(savep())
                   ->mInfo.mSavedata.mPlayer.mPlayerStatusA.mSelectEquip[i];
        GameInfoMemory1::st8(gameInfoAddress() + 0x5BC0 + i, e);
    }
    for (u32 i = 0; i < 4; i++)
        GameInfoHD::view<GameInfoHD::SaveStorage>(savep())
            ->mInfo.mSavedata.mPlayer.mPlayerItem.mItems[14 + i] = bot[i];
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerItem.mItems[8] = cam;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mSelectItem[0] = 0xFF;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mSelectItem[1] = 0xFF;
    GameInfoHD::view<dSv_player_c>(savep() + 0x20)->mPlayerStatusA.mSelectItem[2] = 0xFF;
}

// HD, resource control flag (*(101F4F54) + 0xD8) != 0
/* 02522F50 */
u32 hdGameInfo_dComIfG_isResFlag() {
    return GameInfoMemory1::ld(GameInfoMemory1::ld(0x101F4F54) + 0xD8) != 0 ? 1 : 0;
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
ext 027262B0 void gameInfo1_setPictureNum(u32,u32)
ext 02526118 void gameInfo1_setArrowKeepType(u32)
ext 02720144 u32 gameInfo1_getPictureInfo(u32)
ext 025B7944 void gameInfo1_onCollect(u32,u32,u32)
ext 025C1BA0 u32 gameInfo1_mapInfoOceanX(u32)
ext 025C1BB8 u32 gameInfo1_mapInfoOceanZ(u32)
ext 02560728 void gameInfo1_setNextTime(f32)
ext 02525C34 void gameInfo1_assureTermination(u32)
ext 025B7F68 u32 gameInfo1_isGetCollectMap(u32,u32)
ext 025B7A2C u32 gameInfo1_isCollect(u32,u32,u32)
ext 025B7D90 u32 gameInfo1_isSymbol(u32,u32)
ext 025B7C50 u32 gameInfo1_isTriforce(u32,u32)
ext 025B7B10 u32 gameInfo1_isTact(u32,u32)
ext 025C2150 u32 gameInfo1_getReverbStage(u32,u32)
ext 0260E500 void gameInfo1_deleteObjectArchive(u32,u32)
ext 02525FE4 s32 gameInfo1_phaseHandler(u32,u32,u32)
ext 0260F414 s32 gameInfo1_syncObjectArchive(u32,u32)
ext 0260CBB4 u32 gameInfo1_setObjectArchive(u32,u32,u32)
ext 025E1D40 void gameInfo1_audioResetProcess()
ext 025E1904 void gameInfo1_stopBgm(u32)
ext 025DC964 void gameInfo1_sceneReRequest(u32,u32)
ext 025DC86C void gameInfo1_sceneChangeRequest(u32,u32,u32,u32,u32)
ext 025E17CC void gameInfo1_setSceneName(u32,u32,u32)
ext 025C3FEC u32 gameInfo1_setNextStage(u32,u32,u32,u32,u32,u32)
ext 0252E9CC void gameInfo1_drawListConstructor(u32)
ext 025B9170 void gameInfo1_memoryInit(u32)
ext 0252A0E0 void gameInfo1_detectConstructor(u32)
ext 025CC0CC void gameInfo1_vibrationConstructor(u32)
ext 024ED630 void gameInfo1_attentionConstructor(u32)
ext 025436A4 void gameInfo1_eventManagerConstructor(u32)
ext 0253EAB8 void gameInfo1_eventControlConstructor(u32)
ext 025C4548 void gameInfo1_stageDataConstructor(u32)
ext 025C3FAC void gameInfo1_nextStageConstructor(u32)
ext 025ABC48 void gameInfo1_admConstructor(u32)
ext 02518704 void gameInfo1_collisionConstructor(u32)
ext 02007FBC void gameInfo1_backgroundConstructor(u32)
ext 025B50DC void gameInfo1_returnPlace_set(u32,u32,u32,u32)
ext 025B8BB0 u32 gameInfo1_getEventReg(u32,u32)
ext 025B8B94 u32 gameInfo1_isEventBit(u32,u32)
ext 025C11DC u32 gameInfo1_getStatusRoomDt(u32,u32)
ext 025B9100 u32 gameInfo1_dSv_memBit_isDungeonItem(u32,u32)
ext 025B9098 u32 gameInfo1_dSv_memBit_onDungeonItem(u32,u32)
ext 025B8C74 u32 gameInfo1_dSv_memBit_isTbox(u32,u32)
ext 025B8C0C u32 gameInfo1_dSv_memBit_onTbox(u32,u32)
ext 025200D4 u32 gameInfo1_getGameInfo()
ext 028EFFD0 void gameInfo1_construct_array(u32,u32,u32,u32)
ext C000A848 void gameInfo1_memcpy(u32,u32,u32)
ext 028F521C void gameInfo1_memzero(u32,u32)
ext 0273AD10 u32 gameInfo1_operator_new(u32)
ext 0273AA24 u32 gameInfo1_JUT_ASSERT(u32,u32,u32)
str 1004B9C8 "sea_T" in 02520230
str 1004B9E4 "i_phase->id != 1" in 025204C8
str 1004B9FC "sclsInfo != (0)" in 025215BC
str 1004BA1C "0 <= i_no && i_no < sclsInfo->num" in 025215BC
str 1004BA0C "sclsData != (0)" in 025215BC
str 1004BA6C "" in 025217F8
str 1004BA64 "PShip" in 025217F8
str 1004BA84 "mapInfo != (0)" in 025217F8
*/
