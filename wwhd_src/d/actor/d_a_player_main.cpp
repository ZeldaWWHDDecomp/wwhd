/**
 * d_a_player_main.cpp (WWHD)
 * Player - Link (daPy_lk_c): phase 1 (layout check), small accessors and checks
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_player_main.cpp and its .inc files) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * The address ranges #01..#08 add their functions in d_a_player_main_NN.cpp files.
 */
#include "d/actor/d_a_player_main.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E1A40 JAIZelBasic::seStart(id, pos, mtrlSndId, reverb) (mDoAud_seStart, no NULL checks) */
static inline void mDoAud_seStart_l(u32 id, cXyz* pos, u32 mtrl, s8 reverb) { gabi::call(0x025E1A40, id, pos, mtrl, (s32)reverb); }
/* 025E1988 one-argument system sound (seStartSystem) */
static inline void mDoAud_seStartSystem_l(u32 id) { gabi::call(0x025E1988, id); }
/* play + 0x5CD8: player status 0 of player 0 */
static inline void dComIfGp_setPlayerStatus0_l(u32 flag) {
    u32 a = dComIfGp_ea() + 0x5CD8;
    gabi::store<u32>(a, gabi::load<u32>(a) | flag);
}

/* 023D78E4 */
BOOL daPy_lk_c::checkSingleItemEquipAnime() {
    WWHD_FUNC(0x023D78E4, BOOL, this);
    /* checkUpperAnime(..): m_anm_heap_upper[UPPER_MOVE2_e].mIdx (u16 at +0) */
    u16 idx = gabi::load<u16>(gabi::ea(this) + 0x5888);
    return idx == 0x105 || idx == 0x106;
}
VERIFY(0x023D78E4, &daPy_lk_c::checkSingleItemEquipAnime);

/* 023D794C */
BOOL daPy_lk_c::checkEquipAnime() {
    WWHD_FUNC(0x023D794C, BOOL, this);
    return gabi::load<u16>(gabi::ea(this) + 0x5888) == 0xD7 /* dRes_INDEX_LKANM_BCK_REST_e */ || gabi::call<BOOL>(0x023D7904, this) /* checkItemEquipAnime */;
}
VERIFY(0x023D794C, &daPy_lk_c::checkEquipAnime);

/* 023D85CC */
BOOL daPy_lk_c::checkBottleItem(int item) {
    WWHD_FUNC(0x023D85CC, BOOL, this, item);
    return (u32)(item - 0x50) < 0x11;
}
VERIFY(0x023D85CC, &daPy_lk_c::checkBottleItem);

/* 023D85E4 */
BOOL daPy_lk_c::checkPhotoBoxItem(int item) {
    WWHD_FUNC(0x023D85E4, BOOL, this, item);
    return item == 0x23 /* CAMERA */ || item == 0x26 /* CAMERA2 */;
}
VERIFY(0x023D85E4, &daPy_lk_c::checkPhotoBoxItem);

/* 023D8600 */
BOOL daPy_lk_c::checkBowItem(int item) {
    WWHD_FUNC(0x023D8600, BOOL, this, item);
    return item == 0x27 /* BOW */ || item == 0x35 /* MAGIC_ARROW */ || item == 0x36 /* LIGHT_ARROW */;
}
VERIFY(0x023D8600, &daPy_lk_c::checkBowItem);

/* 023DC0F4 */
u32 daPy_lk_c::seStartOnlyReverb(u32 id) {
    WWHD_FUNC(0x023DC0F4, u32, this, id);
    return gabi::call<u32>(0x025E1A40, id, &current.pos, 0u, (s32)(s8)mReverb); /* mDoAud_seStart */
}
VERIFY(0x023DC0F4, &daPy_lk_c::seStartOnlyReverb);

/* 023DC63C */
static void daPy_actorKeep_clearData(daPy_actorKeep_l* k) {
    WWHD_FUNC(0x023DC63C, void, k);
    k->mID = 0xFFFFFFFF;
    k->mActor = nullptr;
}
VERIFY(0x023DC63C, daPy_actorKeep_clearData);

/* 023DCB3C */
void daPy_lk_c::cancelNoDamageMode() {
    WWHD_FUNC(0x023DCB3C, void, this);
    mTinkleShieldTimer = 0;
    setNoResetFlg1(noResetFlg1() & ~1u); /* offNoResetFlg1(daPyFlg1_EQUIP_DRAGON_SHIELD) */
}
VERIFY(0x023DCB3C, &daPy_lk_c::cancelNoDamageMode);

/* 023DD754 */
BOOL daPy_lk_c::checkMabaAnimeBtp(int idx) {
    WWHD_FUNC(0x023DD754, BOOL, this, idx);
    return (u32)(idx - 0x22D) < 0xC;
}
VERIFY(0x023DD754, &daPy_lk_c::checkMabaAnimeBtp);

/* 023DDF64 */
J3DModel* daPy_lk_c::setItemHeap() {
    WWHD_FUNC(0x023DDF64, J3DModel*, this);
    if (!(resetFlg0() & 0x4000)) { /* !checkResetFlg0(daPyRFlg0_UNK4000) */
        mCurrItemHeapIdx = mCurrItemHeapIdx ^ 1;
        setResetFlg0(resetFlg0() | 0x4000);
    }
    /* setAnimeHeap(mpItemHeaps[mCurrItemHeapIdx]) (023DDEA8, unnamed by the matcher) */
    u32 heap = gabi::load<u32>(gabi::ea(this) + 0x4438 + mCurrItemHeapIdx * 4);
    return gabi::call<J3DModel*>(0x023DDEA8, this, heap);
}
VERIFY(0x023DDF64, &daPy_lk_c::setItemHeap);

/* 023DE638 */
static void daPy_actorKeep_setData(daPy_actorKeep_l* k, fopAc_ac_c* actor) {
    WWHD_FUNC(0x023DE638, void, k, actor);
    k->mActor = actor;
    k->mID = actor != nullptr ? gabi::load<u32>(gabi::ea(actor) + 4) /* fopAcM_GetID */ : 0xFFFFFFFF;
}
VERIFY(0x023DE638, daPy_actorKeep_setData);

/* 023E0470 */
u32 daPy_lk_c::seStartMapInfo(u32 id) {
    WWHD_FUNC(0x023E0470, u32, this, id);
    return gabi::call<u32>(0x025E1A40, id, &current.pos, (u32)mMtrlSndId, (s32)(s8)mReverb); /* mDoAud_seStart */
}
VERIFY(0x023E0470, &daPy_lk_c::seStartMapInfo);

/* 023E063C */
void daPy_lk_c::setHandModel(int anmIdx) {
    WWHD_FUNC(0x023E063C, void, this, anmIdx);
    u32 e = 0x100366A0 + anmIdx * 8; /* mAnmDataTable[anmIdx] (8 bytes each, .data 0x100366A0) */
    mLeftHandIdx = gabi::load<u8>(e + 4);
    mRightHandIdx = gabi::load<u8>(e + 5);
}
VERIFY(0x023E063C, &daPy_lk_c::setHandModel);

/* 023E07C0 */
void daPy_lk_c::resetSeAnime() {
    WWHD_FUNC(0x023E07C0, void, this);
    mpSeAnm = 0x10035574; /* HD: a pointer to the "no SE animation" data (GameCube mSeAnmIdx = -1) */
    m34F0 = 0xFFFF;
    mpSeAnmFrameCtrl = 0;
}
VERIFY(0x023E07C0, &daPy_lk_c::resetSeAnime);

/* 023E3358 */
int daPy_lk_c::getDirectionFromAngle(s16 angle) {
    WWHD_FUNC(0x023E3358, int, this, angle);
    s32 a = angle;
    if ((a < 0 ? -a : a) > 0x6000) {
        return 1; /* DIR_BACKWARD */
    } else if (angle >= 0x2000) {
        return 2; /* DIR_LEFT */
    } else if (angle <= -0x2000) {
        return 3; /* DIR_RIGHT */
    }
    return 0; /* DIR_FORWARD */
}
VERIFY(0x023E3358, &daPy_lk_c::getDirectionFromAngle);

/* 023E3398 */
int daPy_lk_c::getDirectionFromCurrentAngle() {
    WWHD_FUNC(0x023E3398, int, this);
    return getDirectionFromAngle((s16)(m34E8 - current.angle.y));
}
VERIFY(0x023E3398, &daPy_lk_c::getDirectionFromCurrentAngle);

/* 023E4E18 */
int daPy_lk_c::getDirectionFromShapeAngle() {
    WWHD_FUNC(0x023E4E18, int, this);
    return getDirectionFromAngle((s16)(m34E8 - shape_angle.y));
}
VERIFY(0x023E4E18, &daPy_lk_c::getDirectionFromShapeAngle);

/* 023E8C74 */
BOOL daPy_lk_c::itemButton() {
    WWHD_FUNC(0x023E8C74, BOOL, this);
    u8 btn = mReadyItemBtn;
    u8 b = mItemButton;
    if (btn == 0 /* dItemBtn_X_e */) {
        return b & 4; /* itemButtonX() */
    }
    return b & (btn == 1 /* dItemBtn_Y_e */ ? 8 : 0x10); /* itemButtonY() / itemButtonZ() */
}
VERIFY(0x023E8C74, &daPy_lk_c::itemButton);

/* 023EE9FC */
void daPy_lk_c::setSubjectMode() {
    WWHD_FUNC(0x023EE9FC, void, this);
    dComIfGp_setPlayerStatus0_l(0x2000); /* daPyStts0_SUBJECT_e */
    mDoAud_seStartSystem_l(0x8FA);       /* JA_SE_SUBJ_VIEW_IN */
}
VERIFY(0x023EE9FC, &daPy_lk_c::setSubjectMode);

/* 023F73A4 */
void daPy_lk_c::resetCurse() {
    WWHD_FUNC(0x023F73A4, void, this);
    if (gabi::load<u16>(0x101CEF16) == 1) { /* HD: a global (GameCube: the curse state) */
        gabi::call(0x023DCA08, this); /* endDamageEmitter */
    }
}
VERIFY(0x023F73A4, &daPy_lk_c::resetCurse);

/* 023F931C */
BOOL daPy_lk_c::checkSuccessGuard(int atSpl) {
    WWHD_FUNC(0x023F931C, BOOL, this, atSpl);
    /* mCyl.ChkTgShieldHit(): the TG hit flags of mCyl (+0x98) */
    if (!(gabi::load<u32>(gabi::ea(&mCyl) + 0x98) & 2) || atSpl >= 8 /* dCcG_At_Spl_UNK8 */) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x023F931C, &daPy_lk_c::checkSuccessGuard);

/* 023FD4D0 */
u32 daPy_lk_c::setParamData(int roomNo, int spawnType, int eventInfoIdx, int extraParams) {
    WWHD_FUNC(0x023FD4D0, u32, this, roomNo, spawnType, eventInfoIdx, extraParams);
    return (roomNo & 0x3F) | (spawnType & 0xF) << 0xC | (u32)eventInfoIdx << 0x18 | extraParams;
}
VERIFY(0x023FD4D0, &daPy_lk_c::setParamData);

/* 024072F4 */
u32 daPy_lk_c::seStartSwordCut(u32 id) {
    WWHD_FUNC(0x024072F4, u32, this, id);
    return gabi::call<u32>(0x025E1A40, id, gabi::at<cXyz>(gabi::ea(this) + 0x3E4) /* mSwordTopPos */, 0u, (s32)(s8)mReverb); /* mDoAud_seStart */
}
VERIFY(0x024072F4, &daPy_lk_c::seStartSwordCut);

/* 0241E428 */
BOOL daPy_lk_c::procPolyDamage() {
    WWHD_FUNC(0x0241E428, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) { /* UNDER_MOVE0_e */
        gabi::call<BOOL>(0x023F14E0, this, 0); /* checkNextMode */
    }
    return TRUE;
}
VERIFY(0x0241E428, &daPy_lk_c::procPolyDamage);

/* 024430A4 */
s16 daPy_lk_c::checkAutoJumpFlying() {
    WWHD_FUNC(0x024430A4, s16, this);
    if (mCurProc != 0x24 /* daPyProc_AUTO_JUMP_e */) {
        return -1;
    }
    return mProcVar0;
}
VERIFY(0x024430A4, &daPy_lk_c::checkAutoJumpFlying);

/* 024435E0 */
void daPy_lk_c::onDekuSpReturnFlg(u8 i_point) {
    WWHD_FUNC(0x024435E0, void, this, i_point);
    if (i_point != 0xFF) {
        mDekuSpRestartPoint = i_point;
    }
    mNoResetFlg0 = mNoResetFlg0 | 0x10; /* onNoResetFlg0(daPyFlg0_DEKU_SP_RETURN_FLG) */
}
VERIFY(0x024435E0, &daPy_lk_c::onDekuSpReturnFlg);

/* 02443728 */
BOOL daPy_lk_c::checkPlayerGuard() {
    WWHD_FUNC(0x02443728, BOOL, this);
    s32 proc = mCurProc;
    if (proc == 0xC /* daPyProc_GUARD_SLIP_e */) return TRUE;
    u16 upper = gabi::load<u16>(gabi::ea(this) + 0x5888); /* m_anm_heap_upper[UPPER_MOVE2_e].mIdx */
    return upper == 0x16 || upper == 0x1B || proc == 0x6D || proc == 0xD;
}
VERIFY(0x02443728, &daPy_lk_c::checkPlayerGuard);
