/**
 * d_a_player_main_07.cpp (WWHD)
 * Player - Link (daPy_lk_c), address range #07 (0242DAC8..0243F057): grab, swim, battle jumps,
 * ship, rope, boomerang/bow/hookshot subjects, cut reverse, fan, tact, vomit, hammer, push/pull,
 * bottle and weapon procs.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_player_main.cpp and its .inc files) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Functions of other ranges are called by address (gabi::call), not as C++ members.
 */
#include "d/actor/d_a_player_main.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0200ECD4 cLib_addCalc(f32* value, f32 target, f32 scale, f32 maxStep, f32 minStep) */
static inline f32 cLib_addCalc_l(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
/* cBgS::GetTriPla(bg, poly) of a cBgS_PolyInfo; the cM3dGPla normal at +0 */
static inline u32 dBgS_GetTriPla_l(u32 poly) {
    u32 bgs = gabi::ea(dComIfG_Bgsp());
    u32 bg = gabi::load<u16>(poly + 2);
    u32 pl = gabi::load<u16>(poly + 0);
    return gabi::call<u32>(0x020084C8, bgs, bg, pl);
}
/* camera attention: dAttention_c::LockonTruth() || (flags (+0x20) & 0x20000000) (Lockon()) */
static inline BOOL dAttention_LockonTruth_l(u32 attn) { return gabi::call<BOOL>(0x024EDFCC, attn); }
/* the camera of the camera info index (play + 0x5AF8 + idx * 0x34); dCamera_c at +0x248 */
static inline u32 dComIfGp_getCamera_l(s32 idx) { return gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5AF8); }
/* play + 0x5CDC: dComIfGp_setPlayerStatus1(0, flag) */
static inline void dComIfGp_onPlayerStatus1_l(u32 flag) {
    u32 a = dComIfGp_ea() + 0x5CDC;
    gabi::store<u32>(a, gabi::load<u32>(a) | flag);
}
/* play + 0x5CD8: dComIfGp_checkPlayerStatus0(0, flag) */
static inline u32 dComIfGp_checkPlayerStatus0_l(u32 flag) { return gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & flag; }

/* daPy_lk_c members in the opaque blocks of the shared header (offsets measured in this range) */
#define LK_FIELD(T, off) (*gabi::at<be<T>>(gabi::ea(this) + (off)))
#define mGrabWearY LK_FIELD(f32, 0x3CC)       /* daPy_py_c field_0x2b0 (checkGrabWear: < 0) */
#define mDemoMode LK_FIELD(u32, 0x430)        /* daPy_py_c mDemo.mDemoMode (GameCube 0x314) */
#define mMaxNormalSpeed LK_FIELD(f32, 0x3C4)  /* daPy_py_c (GameCube 0x2A8) */
#define mLinkLinChkPoly (gabi::ea(this) + 0x9E4) /* mLinkLinChk's cBgS_PolyInfo */
#define mLinkLinChkCross (*gabi::at<cXyz>(gabi::ea(this) + 0xA00)) /* mLinkLinChk.GetCross() (+0x30) */

/* ---- daPy_lk_c functions of other ranges (by address) ---- */
enum : u32 {
    LK_commonProcInit = 0x023DFDD8,
    LK_setSingleMoveAnime = 0x023E0A04,
    LK_setBlendMoveAnime = 0x023E14A0,
    LK_setActAnimeUpper = 0x023DE7E8,
    LK_resetActAnimeUpper = 0x023DC6A4,
    LK_freeGrabItem = 0x023DCF8C,
    LK_actorKeep_clearData = 0x023DC63C,
    LK_actorKeep_setData = 0x023DE638,
    LK_initGrabNextMode = 0x0241F470,
    LK_setAnimeEquipSword = 0x023E9F60,
    LK_setSpeedAndAngleAtn = 0x02417538,
    LK_setSpeedAndAngleNormal = 0x0241650C,
    LK_checkGrabSpecialHeavyState = 0x023DBBE8,
    LK_checkNextActionGrab = 0x023EA1AC,
    LK_checkNextMode = 0x023F14E0,
    LK_setWeaponBlur = 0x0242D888,
    LK_procGrabUp_init = 0x0242D8DC,
    LK_procGrabMiss_init = 0x0242DA4C,
};
#define LK_checkNextMode(n) gabi::call<BOOL>(LK_checkNextMode, this, (s32)(n))
/* virtual daPy_lk_c::voiceStart(u32) (vtable slot 0xE4) */
#define LK_voiceStart(n) gabi::call_ptr(gabi::load<u32>(__vtbl + 0xE4), this, (u32)(n))
#define LK_cutEnd() dComIfGp_evmng_cutEnd(mStaffIdx)
/* checkGrabAnime(): m_anm_heap_upper[UPPER_MOVE2_e].mIdx is GRABWAIT (0x95) or GRABWAITB (0x96) */
#define LK_checkGrabAnime() (gabi::load<u16>(gabi::ea(this) + 0x5888) == 0x95 || gabi::load<u16>(gabi::ea(this) + 0x5888) == 0x96)
/* checkAttentionLock(): mpAttention->Lockon() (the attention pointer is read once) */
static inline BOOL lk_attentionLock(u32 attn) { return dAttention_LockonTruth_l(attn) || (gabi::load<u32>(attn + 0x20) & 0x20000000); }
#define LK_checkAttentionLock() lk_attentionLock(mpAttention)

/* HD: m_pbCalc[PART_UPPER_e]->setRatio(2, v) became a per-joint weight table: every joint of the
 * model (joint count from the model data's joint tree) gets the weight */
static inline void lk_setUpperRatio(u32 calc, f32 v) {
    u32 tree = gabi::call<u32>(0x027F3F94 /* J3DModelData joint tree (matcher: __nw) */, gabi::load<u32>(gabi::load<u32>(calc + 0x80) + 0xAC));
    s32 n = gabi::call<s32>(0x027E0174, tree, 0);
    for (s32 i = 0; i < n; i++) {
        gabi::store<f32>(gabi::load<u32>(gabi::load<u32>(calc + 0x7C) + 0x2C) + i * 4, v);
    }
}

/* 0242DAC8 */
BOOL daPy_lk_c::procGrabReady() {
    WWHD_FUNC(0x0242DAC8, BOOL, this);
    fopAc_ac_c* grab = mActorKeepGrab.mActor;
    if (grab == nullptr && mActorKeepRope.mActor == nullptr) {
        return LK_checkNextMode(0);
    }
    if (mProcVar6 == 2) {
        if (mFrameCtrlUnder[0].getRate() < 0.01f) {
            gabi::call(LK_procGrabUp_init, this);
        }
    } else if (mProcVar6 == 1) {
        if (mFrameCtrlUnder[0].getRate() < 0.01f) {
            if (grab != nullptr) {
                if (fpcM_GetName(grab) == 0x1CF /* fpcNm_BOKO_e */) {
                    u32 id = mActorKeepGrab.mID;
                    u32 actor = gabi::ea(mActorKeepGrab.mActor.get());
                    mActorKeepEquip.mID = id;
                    mEquipItem = 0x101; /* daPyItem_BOKO_e */
                    gabi::store<u32>(gabi::ea(&mActorKeepEquip) + 4, actor);
                    gabi::call(LK_actorKeep_clearData, &mActorKeepGrab);
                    gabi::call(LK_setWeaponBlur, this);
                    LK_checkNextMode(0);
                } else if (grab->actor_status & 0x08010000) {
                    f32 rate = (grab->actor_status & 0x08000000) ? 0.8f : 0.4f; /* HD: HIO folded */
                    gabi::call(LK_setSingleMoveAnime, this, 0x67 /* ANM_GRABNG */, rate, 0.0f, 5, 1.0f);
                    mProcVar6 = 2;
                } else {
                    gabi::call(LK_procGrabUp_init, this);
                }
            } else if (mActorKeepRope.mActor != nullptr) {
                gabi::call(LK_procGrabMiss_init, this);
            } else {
                LK_checkNextMode(0);
            }
        }
    } else if (mProcVar6 == 0 && gabi::load<u16>(gabi::ea(this) + 0x5888) == 0xFFFF /* checkNoUpperAnime() */) {
        gabi::call(LK_setSingleMoveAnime, this, 0x65 /* ANM_GRABP */, 0.8f, 0.0f, 4, 1.0f);
        mProcVar6 = 1;
    }
    return TRUE;
}
VERIFY(0x0242DAC8, &daPy_lk_c::procGrabReady);

/* 0242DC84 */
BOOL daPy_lk_c::procGrabRebound_init() {
    WWHD_FUNC(0x0242DC84, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x75 /* daPyProc_GRAB_REBOUND_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x6B /* ANM_GRABRE */, 1.1f, 3.0f, 0x13, 2.0f); /* HD: HIO folded */
    return TRUE;
}
VERIFY(0x0242DC84, &daPy_lk_c::procGrabRebound_init);

/* 0242DCE0 */
BOOL daPy_lk_c::procGrabUp() {
    WWHD_FUNC(0x0242DCE0, BOOL, this);
    fopAc_ac_c* grab = mActorKeepGrab.mActor;
    if (grab == nullptr) {
        return LK_checkNextMode(0);
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        setResetFlg0(resetFlg0() | 0x20 /* daPyRFlg0_GRAB_UP_END */);
        if (grab->actor_status & 0x08010000) {
            procGrabRebound_init();
        } else {
            gabi::call(LK_initGrabNextMode, this);
        }
    } else if (mFrameCtrlUnder[0].getFrame() > 9.0f) {
        if (grab->actor_status & 0x08010000) {
            if (mStickDistance > 0.05f) {
                setResetFlg0(resetFlg0() | 0x20);
                procGrabRebound_init();
            }
        } else {
            if (!LK_checkGrabAnime()) {
                gabi::call(LK_setActAnimeUpper, this, 0x95 /* GRABWAIT */, 2 /* UPPER_MOVE2_e */, 0.0f, 0.0f, -1, -1.0f);
                lk_setUpperRatio(m_pbCalc[1], 0.0f);
            }
            if (LK_checkNextMode(1)) {
                setResetFlg0(resetFlg0() | 0x20);
                if (LK_checkGrabAnime()) {
                    lk_setUpperRatio(m_pbCalc[1], 1.0f);
                }
            }
        }
    }
    return TRUE;
}
VERIFY(0x0242DCE0, &daPy_lk_c::procGrabUp);

/* 0242DF08 */
BOOL daPy_lk_c::procGrabMiss() {
    WWHD_FUNC(0x0242DF08, BOOL, this);
    if (mActorKeepRope.mActor == nullptr) {
        LK_checkNextMode(0);
    }
    if (mProcVar6 == 0) {
        if (mFrameCtrlUnder[0].getRate() < 0.01f) {
            if (mProcVar0 > 0) {
                mProcVar0 = mProcVar0 - 1;
            } else {
                gabi::call(LK_setSingleMoveAnime, this, 0x65 /* ANM_GRABP */, -0.8f, 0.0f, 4, 4.0f); /* HD: HIO folded */
                mProcVar6 = 1;
                gabi::call(LK_freeGrabItem, this);
            }
        }
    } else if (mFrameCtrlUnder[0].getRate() > -0.01f) {
        LK_checkNextMode(0);
    } else if (mFrameCtrlUnder[0].getFrame() < 0.0f) {
        LK_checkNextMode(1);
    }
    return TRUE;
}
VERIFY(0x0242DF08, &daPy_lk_c::procGrabMiss);

/* 0242E010 */
BOOL daPy_lk_c::procGrabThrow() {
    WWHD_FUNC(0x0242E010, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    fopAc_ac_c* grab = mActorKeepGrab.mActor;
    cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.6f, 2.5f, 1.8f); /* HD: HIO folded */
    if (mProcVar7 == 0) {
        if (frameCtrl.getRate() < 0.01f) {
            gabi::call(LK_setSingleMoveAnime, this, 0x6A /* ANM_GRABTHROW */, 0.8f, 1.0f, 0xE, 0.0f);
            mProcVar7 = 1;
        }
        return TRUE;
    }
    if (frameCtrl.checkPass(2.0f) && grab != nullptr) {
        grab->current.angle.y = shape_angle.y;
        grab->speed.y = 34.0f;
        grab->speedF = 19.0f;
        s32 idx = mCameraInfoIdx;
        u32 cam = dComIfGp_getCamera_l(idx);
        gabi::call(0x02515470 /* dCamera_c::ForceLockOn */, cam + 0x248, (u32)mActorKeepGrab.mID);
        if (grab->actor_status & 0x10000) {
            LK_voiceStart(18);
        } else if (fpcM_GetName(grab) == 0x126 /* fpcNm_BOMB_e */ || fpcM_GetName(grab) == 0x127 /* fpcNm_Bomb2_e */) {
            LK_voiceStart(0);
        } else {
            LK_voiceStart(17);
        }
        gabi::call(LK_freeGrabItem, this);
    }
    if (frameCtrl.getRate() < 0.01f) {
        if (mProcVar6 != 0) {
            mProcVar6 = 0;
            gabi::call(LK_setAnimeEquipSword, this, 0);
        }
        LK_checkNextMode(0);
    } else if (frameCtrl.getFrame() > 7.0f) {
        if (mProcVar6 != 0) {
            mProcVar6 = 0;
            gabi::call(LK_setAnimeEquipSword, this, 0);
        }
        LK_checkNextMode(1);
    }
    return TRUE;
}
VERIFY(0x0242E010, &daPy_lk_c::procGrabThrow);

/* 0242E264 */
BOOL daPy_lk_c::procGrabPut() {
    WWHD_FUNC(0x0242E264, BOOL, this);
    fopAc_ac_c* grab = mActorKeepGrab.mActor;
    if (grab == nullptr) {
        if (mDemoMode == 0x38 /* DEMO_GRAB_PUT_e */) {
            LK_cutEnd();
            return TRUE;
        }
        return LK_checkNextMode(0);
    }
    if (mProcVar6 == 1) {
        if (mGrabWearY < 0.0f) { /* checkGrabWear() */
            return TRUE;
        }
        mProcVar6 = 0;
        gabi::call(LK_setSingleMoveAnime, this, 0x66 /* ANM_GRABUP */, -1.1f, 0.0f, 7, 0.0f); /* HD: HIO folded */
    }
    gabi::Local<cXyz> pos; /* sp+0x14 */
    {
        s16 a = shape_angle.y;
        f32 z = grab->current.pos.z;
        f32 r = m35C8;
        f32 x = grab->current.pos.x;
        pos->y = grab->current.pos.y;
        pos->x = gabi::fmadds(r, cM_ssin(a), x);
        pos->z = gabi::fmadds(r, cM_scos(a), z);
    }
    dBgS_LinChk_Set(mLinkLinChk, &m370C, pos, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        u32 pla = dBgS_GetTriPla_l(mLinkLinChkPoly);
        if (pla != 0) {
            f32 ny = gabi::load<f32>(pla + 4);
            if (ny < 0.5f && !(ny < -0.8f)) { /* cBgW_CheckBWall */
                gabi::Local<cXyz> d; /* sp+0x08 */
                cXyz_mi(pos, d, &mLinkLinChkCross);
                f32 len = std_sqrtf(PSVECSquareMag(d));
                gabi::Local<cXyz> xz; /* sp+0x20 */
                xz->x = d->x;
                xz->y = 0.0f;
                xz->z = d->z;
                f32 lenXZ = std_sqrtf(PSVECSquareMag(xz));
                if (lenXZ > 0.01f) {
                    len = len / lenXZ;
                }
                d->x = d->x * len;
                d->z = d->z * len;
                current.pos.x = current.pos.x - d->x;
                current.pos.z = current.pos.z - d->z;
                grab->current.pos.x = grab->current.pos.x - d->x;
                grab->current.pos.z = grab->current.pos.z - d->z;
            }
        }
    }
    m370C.x = grab->current.pos.x;
    m370C.z = grab->current.pos.z;
    m370C.y = grab->current.pos.y;
    if (mFrameCtrlUnder[0].getRate() > -0.01f) {
        grab->speedF = 0.0f;
        gabi::call(LK_freeGrabItem, this);
        if (mDemoMode == 0x38) {
            LK_cutEnd();
        } else {
            LK_checkNextMode(0);
        }
    } else if (mFrameCtrlUnder[0].getFrame() < 0.0f && LK_checkNextMode(1)) {
        grab->speedF = 0.0f;
        gabi::call(LK_freeGrabItem, this);
    }
    return TRUE;
}
VERIFY(0x0242E264, &daPy_lk_c::procGrabPut);

/* 0242E604 */
BOOL daPy_lk_c::procGrabWait() {
    WWHD_FUNC(0x0242E604, BOOL, this);
    if (mActorKeepGrab.mActor == nullptr) {
        gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
        return LK_checkNextMode(0);
    }
    if (LK_checkAttentionLock()) {
        gabi::call(LK_setSpeedAndAngleAtn, this);
    } else {
        gabi::call(LK_setSpeedAndAngleNormal, this, 0xBB8); /* HD: HIO folded */
    }
    if (m35D8 > -29.0f) {
        mFrameCtrlUnder[0].setRate(1.0f); /* HD: HIO folded */
    } else {
        if (mFrameCtrlUnder[0].getRate() > 0.0f && mProcVar6 == 0) {
            mProcVar6 = 1;
            if (gabi::load<u16>(gabi::ea(this) + 0x5848) == 0x95 /* m_anm_heap_under[0].mIdx == GRABWAIT */) {
                seStartMapInfo(0x2829 /* JA_SE_LK_BARREL_PUT_ON */);
            }
        }
        mFrameCtrlUnder[0].setRate(0.0f);
    }
    if (LK_checkNextMode(0)) {
        if (LK_checkGrabAnime()) {
            lk_setUpperRatio(m_pbCalc[1], 1.0f);
        }
    } else if (shape_angle.y != m34DE && !LK_checkAttentionLock()) {
        f32 morf = (noResetFlg1() & 0x800000) ? -1.0f : 2.4f; /* HD: HIO folded */
        gabi::call(LK_setBlendMoveAnime, this, morf);
        lk_setUpperRatio(m_pbCalc[1], 1.0f);
        s16 d = (s16)(shape_angle.y - m34DE);
        setNoResetFlg1(noResetFlg1() | 0x800000);
        m35A0 = (f32)d * 0.005f;
    } else if (noResetFlg1() & 0x800000) {
        if (gabi::call<BOOL>(LK_checkGrabSpecialHeavyState, this)) {
            gabi::call(LK_setSingleMoveAnime, this, 0x69 /* ANM_GRABWAITB */, 1.2f, 0.0f, -1, 3.0f);
        } else {
            gabi::call(LK_setSingleMoveAnime, this, 0x68 /* ANM_GRABWAIT */, 1.0f, 0.0f, -1, 3.0f);
        }
        lk_setUpperRatio(m_pbCalc[1], 0.0f);
        m35A0 = 0.0f;
        setNoResetFlg1(noResetFlg1() & ~0x800000u);
    }
    return TRUE;
}
VERIFY(0x0242E604, &daPy_lk_c::procGrabWait);

/* 0242E9D4 */
BOOL daPy_lk_c::procGrabHeavyWait() {
    WWHD_FUNC(0x0242E9D4, BOOL, this);
    if (mActorKeepGrab.mActor == nullptr) {
        gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
        return LK_checkNextMode(0);
    }
    if (!LK_checkAttentionLock() && mStickDistance > 0.05f) {
        cLib_addCalcAngleS(&shape_angle.y, m34E8, 0x28, 0x12C, 0x28); /* HD: HIO folded */
        s16 sy = shape_angle.y;
        s16 d = (s16)(sy - current.angle.y);
        f32 r = (f32)d / 300.0f;
        current.angle.y = sy;
        m35A0 = r;
        f32 morf = (noResetFlg1() & 0x800000) ? -1.0f : 2.4f;
        gabi::call(LK_setBlendMoveAnime, this, morf);
        lk_setUpperRatio(m_pbCalc[1], 1.0f);
        setNoResetFlg1(noResetFlg1() | 0x800000);
    } else {
        m35A0 = 0.0f;
        if (noResetFlg1() & 0x800000) {
            gabi::call(LK_setSingleMoveAnime, this, 0x69 /* ANM_GRABWAITB */, 1.2f, 0.0f, -1, 3.0f);
            lk_setUpperRatio(m_pbCalc[1], 0.0f);
            setNoResetFlg1(noResetFlg1() & ~0x800000u);
        }
    }
    if (mActorKeepGrab.mActor == nullptr) {
        LK_checkNextMode(0);
    } else {
        gabi::call(LK_checkNextActionGrab, this);
    }
    return TRUE;
}
VERIFY(0x0242E9D4, &daPy_lk_c::procGrabHeavyWait);

/* 0242EC50 */
BOOL daPy_lk_c::procGrabRebound() {
    WWHD_FUNC(0x0242EC50, BOOL, this);
    fopAc_ac_c* grab = mActorKeepGrab.mActor;
    if (grab == nullptr) {
        gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
        return LK_checkNextMode(0);
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        gabi::call(LK_initGrabNextMode, this);
    } else if (mFrameCtrlUnder[0].getFrame() > 6.0f && (grab->actor_status & 0x10000)) {
        gabi::call(LK_initGrabNextMode, this);
    }
    return TRUE;
}
VERIFY(0x0242EC50, &daPy_lk_c::procGrabRebound);

/* ---- swim ---- */
enum : u32 {
    LK_setNormalSpeedF = 0x02416230,
    LK_setShapeAngleToAtnActor = 0x02419C70,
    LK_procShipReady_init = 0x023ECA94,
    LK_setTalkStatus = 0x023EC260,
    LK_checkSwimFallCheck = 0x023FE918,
    LK_swimOutAfter = 0x023DFAAC,
    LK_procFall_init = 0x023F6564,
    LK_startRestartRoom = 0x023FD4E4,
    LK_procSwimWait_init = 0x023F8B00,
    LK_setSwimTimerStartStop = 0x023F8924,
    LK_procSwimMove_init = 0x02412E58,
    LK_changeSwimUpProc = 0x02421FBC,
    LK_getSwimTimerRate = 0x023F8858,
    LK_setSwimMoveAnime = 0x02412C30,
    LK_setTextureAnime = 0x023DD768,
    LK_setSwimTail = 0x02412D54,
    LK_changeFrontWallTypeProc = 0x02418E00,
};
#define mAcchGroundH LK_FIELD(f32, 0x8A0)          /* mAcch.GetGroundH() (dBgS_Acch + 0x94) */
#define mAcchGndPoly (gabi::ea(this) + 0x8F4)      /* mAcch.m_gnd (cBgS_PolyInfo, dBgS_Acch + 0xE8) */
#define mPlayerTail LK_FIELD(u32, 0x8260)          /* HD-only (tail): nonzero blocks the swim talk */
/* daPy_swimTailEcallBack_c::onEnd(): mEnd (+4) = 1, emitter (+0x20) = NULL [?] */
static inline void lk_swimTail_onEnd(u32 cb) {
    gabi::store<u8>(cb + 4, 1);
    gabi::store<u32>(cb + 0x20, 0);
}
static inline BOOL dBgS_ChkPolySafe_l(u32 poly) { return gabi::call<BOOL>(0x02008254, dComIfG_Bgsp(), poly); } /* cBgS::ChkPolySafe */
/* play + 0x5BB7: dComIfGp_setDoStatus */
static inline void dComIfGp_setDoStatus_l(u8 s) { gabi::store<u8>(dComIfGp_ea() + 0x5BB7, s); }

/* 0242ECFC */
void daPy_lk_c::setSpeedAndAngleSwim() {
    WWHD_FUNC(0x0242ECFC, void, this);
    f32 speedTarget;
    BOOL backward = FALSE;
    if (mCurProc != 0x35 /* daPyProc_SWIM_UP_e */) {
        if (!LK_checkAttentionLock()) {
            s16 old = shape_angle.y;
            if (mStickDistance > 0.05f) {
                if (getDirectionFromShapeAngle() == 1 /* DIR_BACKWARD */) {
                    lk_swimTail_onEnd(gabi::ea(this) + 0x66A8);
                    lk_swimTail_onEnd(gabi::ea(this) + 0x66A8 + 0x28);
                    shape_angle.y = m34E8;
                    current.angle.y = shape_angle.y;
                } else {
                    cLib_addCalcAngleS(&shape_angle.y, m34E8, 0x11, 0x1388, 0x4B0); /* HD: HIO folded */
                }
                speedTarget = gabi::fmuls_ppc(gabi::fmuls_ppc(3.0f, mStickDistance), cM_scos((s16)(shape_angle.y - old)));
            } else {
                speedTarget = 0.0f;
            }
            cLib_addCalcAngleS(&current.angle.y, shape_angle.y, 2, 0x2000, 0x1000);
            mMaxNormalSpeed = 18.0f;
        } else {
            gabi::call(LK_setShapeAngleToAtnActor, this);
            if (mStickDistance > 0.05f) {
                s16 old = current.angle.y;
                if (getDirectionFromCurrentAngle() == 1 /* DIR_BACKWARD */) {
                    lk_swimTail_onEnd(gabi::ea(this) + 0x66A8);
                    lk_swimTail_onEnd(gabi::ea(this) + 0x66A8 + 0x28);
                    speedTarget = 0.0f;
                    backward = TRUE;
                } else {
                    cLib_addCalcAngleS(&current.angle.y, m34E8, 0x11, 0x1388, 0x4B0);
                    speedTarget = gabi::fmuls_ppc(gabi::fmuls_ppc(3.0f, mStickDistance), cM_scos((s16)(current.angle.y - old)));
                }
            } else {
                speedTarget = 0.0f;
            }
            f32 c = cM_scos((s16)(current.angle.y - shape_angle.y));
            mMaxNormalSpeed = gabi::fmuls_ppc(18.0f, gabi::fmadds(gabi::fadds_ppc(c, 1.0f), 0.25f, 0.5f));
        }
    } else {
        speedTarget = 0.0f;
        mMaxNormalSpeed = 18.0f;
    }
    gabi::call(LK_setNormalSpeedF, this, speedTarget, 0.02f, 2.0f, 0.5f);
    if (backward && mNormalSpeed < 5.0f) {
        current.angle.y = m34E8;
        mNormalSpeed = 0.0f;
    }
    if (dComIfGp_checkPlayerStatus0_l(0x10 /* daPyStts0_UNK10_e */)) {
        mNormalSpeed = 0.0f;
    }
}
VERIFY(0x0242ECFC, &daPy_lk_c::setSpeedAndAngleSwim);

/* 0242F180 */
BOOL daPy_lk_c::checkNextModeSwim() {
    WWHD_FUNC(0x0242F180, BOOL, this);
    u32 entry = mpAttnEntryA;
    if (entry != 0 && gabi::load<s32>(entry + 8) == 7 /* fopAc_Attn_TYPE_SHIP_e */) {
        dComIfGp_setDoStatus_l(0x1C /* dActStts_GET_IN_SHIP_e */);
        if (mItemTrigger & 1 /* doTrigger() */) {
            return gabi::call<BOOL>(LK_procShipReady_init, this);
        }
    }
    if (gabi::call<BOOL>(LK_setTalkStatus, this) && mPlayerTail == 0 /* HD */ && (mItemTrigger & 1)) {
        return gabi::call<BOOL>(0x025D744C /* fopAcM_orderTalkEvent */, this, mpAttnActorA.get());
    }
    return FALSE;
}
VERIFY(0x0242F180, &daPy_lk_c::checkNextModeSwim);

/* 0242F254 */
BOOL daPy_lk_c::changeSwimOutProc() {
    WWHD_FUNC(0x0242F254, BOOL, this);
    u32 pla;
    if (mAcchGroundH != -1000000000.0f && dBgS_ChkPolySafe_l(mAcchGndPoly)) {
        pla = dBgS_GetTriPla_l(mAcchGndPoly);
    } else {
        pla = 0;
    }
    if (gabi::call<BOOL>(LK_checkSwimFallCheck, this)) {
        if (mCurProc == 0x37 /* daPyProc_SWIM_MOVE_e */ && mDirection != 0) {
            current.pos.y = current.pos.y + m35C4;
        }
        gabi::call(LK_swimOutAfter, this, 1);
        return gabi::call<BOOL>(LK_procFall_init, this, 1, 6.0f); /* HD: HIO folded */
    }
    if (!(mNoResetFlg0 & 0x80) ||
        (pla != 0 && !(gabi::load<f32>(pla + 4) < 0.5f) /* cBgW_CheckBGround */ && mWaterY - mAcchGroundH < 85.0f)) {
        f32 y = mWaterY;
        current.pos.y = y;
        if (mCurProc == 0x37 && mDirection != 0) {
            current.pos.y = y + m35C4;
        }
        gabi::call(LK_swimOutAfter, this, 1);
        return LK_checkNextMode(0);
    }
    if (gabi::load<s32>(dComIfGp_ea() + 0x5B4C) <= 0 /* dComIfGp_getItemTimeCount() */ &&
        gabi::call<BOOL>(LK_startRestartRoom, this, 5, 0xC9, -1.0f, 0)) {
        LK_voiceStart(0x21);
        setNoResetFlg1(noResetFlg1() | 0x40000000);
        mFrameCtrlUnder[0].setRate(0.0f);
    }
    return FALSE;
}
VERIFY(0x0242F254, &daPy_lk_c::changeSwimOutProc);

/* 0242F45C */
BOOL daPy_lk_c::procSwimUp() {
    WWHD_FUNC(0x0242F45C, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    setSpeedAndAngleSwim();
    if (!changeSwimOutProc()) {
        if (frameCtrl.getRate() < 0.01f) {
            gabi::call(LK_procSwimWait_init, this, 0);
        } else if (frameCtrl.getFrame() > 19.0f) {
            if (mStickDistance > 0.05f) {
                gabi::call(LK_procSwimMove_init, this, 0);
            }
        } else if (frameCtrl.checkPass(4.0f)) {
            LK_voiceStart(0x1A);
            seStartOnlyReverb(0x2840 /* JA_SE_LK_WALK_IN_WATER */);
            seStartOnlyReverb(0x3807 /* JA_SE_LK_SWIM */);
        } else {
            current.pos.y = mWaterY;
            m35C4 = 0.0f; /* HD: HIO folded */
        }
    }
    gabi::call(LK_setSwimTimerStartStop, this);
    return TRUE;
}
VERIFY(0x0242F45C, &daPy_lk_c::procSwimUp);

/* 0242F590 */
BOOL daPy_lk_c::procSwimWait() {
    WWHD_FUNC(0x0242F590, BOOL, this);
    setSpeedAndAngleSwim();
    f32 r = cM_rndF(0.3f);
    s16 step = (s16)gabi::ftoi(gabi::fmuls_ppc(2330.0f, gabi::fadds_ppc(r, 0.85f)));
    mProcVar2 = (s16)(mProcVar2 + step);
    m35C4 = gabi::fmadds(5.0f, cM_ssin(mProcVar2), 1.0f);
    if (changeSwimOutProc()) {
        return TRUE;
    }
    if (!(mNoResetFlg0 & 0x100)) {
        if (gabi::call<BOOL>(LK_changeSwimUpProc, this)) {
            return TRUE;
        }
    } else {
        current.pos.y = mWaterY;
    }
    if (checkNextModeSwim()) {
        return TRUE;
    }
    if (dComIfGp_checkPlayerStatus0_l(0x10)) {
        fopAc_ac_c* partner = gabi::call<fopAc_ac_c*>(0x025D7C6C /* fopAcM_getTalkEventPartner */, this);
        if (partner != nullptr) {
            s16 target = cLib_targetAngleY(&current.pos, &partner->eyePos);
            cLib_addCalcAngleS(&shape_angle.y, target, 4, 0x1000, 0x200);
            current.angle.y = shape_angle.y;
        }
    }
    f32 rate = gabi::call<f32>(LK_getSwimTimerRate, this);
    mFrameCtrlUnder[0].setRate(gabi::fmadds(rate, 2.5f, 0.5f));
    if (mStickDistance > 0.05f) {
        gabi::call(LK_procSwimMove_init, this, 1);
    }
    gabi::call(LK_setSwimTimerStartStop, this);
    return TRUE;
}
VERIFY(0x0242F590, &daPy_lk_c::procSwimWait);

/* 0242F70C */
BOOL daPy_lk_c::procSwimMove() {
    WWHD_FUNC(0x0242F70C, BOOL, this);
    setSpeedAndAngleSwim();
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    s32 direction;
    if (LK_checkAttentionLock() && mStickDistance > 0.05f) {
        direction = getDirectionFromShapeAngle();
    } else {
        direction = 0; /* DIR_FORWARD */
    }
    if ((u32)mDirection != (u32)direction) {
        f32 old = m35C4;
        mDirection = (u8)direction;
        s32 anm;
        f32 off;
        if ((u8)direction == 0) {
            anm = 0x83; /* ANM_SWIMING */
            off = 0.0f; /* HD: HIO folded */
        } else {
            off = -80.0f;
            if (mDirection == 2 /* DIR_LEFT */) {
                anm = 0xB; /* ANM_ATNDLS */
            } else if (mDirection == 3 /* DIR_RIGHT */) {
                anm = 0xC; /* ANM_ATNDRS */
            } else {
                anm = 0xE; /* ANM_ATNWB */
            }
        }
        m35C4 = off;
        u32 info = gabi::load<u32>(m_old_fdata + 0x1C); /* m_old_fdata->getOldFrameTransInfo(0) */
        gabi::store<f32>(info + 0x18, gabi::load<f32>(info + 0x18) - (off - old));
        gabi::call(LK_setSwimMoveAnime, this, anm);
        if (mDirection != 0) {
            gabi::call(LK_setTextureAnime, this, 3, 0);
            mModeFlg = (mModeFlg | 0x100) & ~0x400u;
            mpSeAnmFrameCtrl = 0;
        } else {
            mModeFlg = (mModeFlg & ~0x100u) | 0x400;
        }
    } else {
        f32 base = 0.6f + (0.5f * std::fabs((f32)mNormalSpeed)) / mMaxNormalSpeed; /* HD: HIO folded */
        f32 rate = gabi::call<f32>(LK_getSwimTimerRate, this);
        frameCtrl.setRate(gabi::fmadds(rate, 1.0f, base));
    }
    if (mDirection == 1 /* DIR_BACKWARD */) {
        frameCtrl.setRate(frameCtrl.getRate() * 0.5f);
    }
    if (changeSwimOutProc()) {
        return TRUE;
    }
    if (!(mNoResetFlg0 & 0x100)) {
        if (gabi::call<BOOL>(LK_changeSwimUpProc, this)) {
            return TRUE;
        }
    } else {
        current.pos.y = mWaterY;
        gabi::call(LK_setSwimTail, this);
    }
    if (checkNextModeSwim()) {
        return TRUE;
    }
    if (gabi::call<BOOL>(LK_changeFrontWallTypeProc, this)) {
        gabi::call(LK_swimOutAfter, this, 1);
        return TRUE;
    }
    if (!(mStickDistance > 0.05f)) {
        gabi::call(LK_procSwimWait_init, this, 1);
    } else if ((mDirection == 0 && frameCtrl.checkPass(20.0f)) || (mDirection != 0 && frameCtrl.checkPass(0.0f))) {
        seStartOnlyReverb(0x3807 /* JA_SE_LK_SWIM */);
    }
    gabi::call(LK_setSwimTimerStartStop, this);
    return TRUE;
}
VERIFY(0x0242F70C, &daPy_lk_c::procSwimMove);

/* ---- battle ---- */
enum : u32 {
    LK_getItemAnimeResource = 0x023DDEEC, /* unnamed by the matcher */
    LK_setBlurPosResource = 0x023E55A4,
    LK_setExtraFinishCutAtParam = 0x023E6A58,
    LK_changeLandProc = 0x0241C898,
    LK_setFallVoice = 0x0241D6CC,
    LK_resetFootEffect = 0x023DF9C0,
    LK_endFlameDamageEmitter = 0x023E4E2C,
    LK_setDamagePoint = 0x023F51D0,
    LK_procBackJump_init = 0x023EE710,
};
#define LK_mAcchChkGroundHit() (gabi::load<u32>(gabi::ea(this) + 0x834) & 0x20) /* mAcch.ChkGroundHit() (m_flags, dBgS_Acch + 0x28) */
/* checkNormalSwordEquip(): the selected sword (save + 0x2E) is the normal sword or the mini game type (play + 0x5CEA) is 2 */
static inline BOOL lk_checkNormalSwordEquip() {
    return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x2E) == 0x38 || gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 2;
}
/* mSwordAnim (mDoExt_bckAnm, +0x4444).changeBckOnly(getItemAnimeResource(idx)) */
static inline void lk_swordAnim_changeBckOnly(daPy_lk_c* t, u32 idx) {
    u32 res = gabi::call<u32>(LK_getItemAnimeResource, t, idx);
    gabi::call(0x025E871C, gabi::ea(t) + 0x4444, res);
}
/* dComIfGp_getVibration().StartShock(6, -0x21, cXyz(0.0f, 1.0f, 0.0f)) */
static inline void lk_startShockUp() {
    gabi::Local<cXyz> up;
    u32 v = gabi::ea(dComIfGp_getVibration());
    up->x = 0.0f;
    up->y = 1.0f;
    up->z = 0.0f;
    gabi::call(0x025CB374, v, 6, -0x21, up.get());
}
/* play + 0x5CD8: dComIfGp_setPlayerStatus0(0, flag) */
static inline void dComIfGp_onPlayerStatus0_l(u32 flag) {
    u32 a = dComIfGp_ea() + 0x5CD8;
    gabi::store<u32>(a, gabi::load<u32>(a) | flag);
}
/* cXyz assignment through integer registers (GHS struct copy) */
static inline void lk_xyz_copy(cXyz* dst, const cXyz* src) {
    u32 d = gabi::ea(dst), s = gabi::ea(src);
    gabi::store<u32>(d + 0, gabi::load<u32>(s + 0));
    gabi::store<u32>(d + 4, gabi::load<u32>(s + 4));
    gabi::store<u32>(d + 8, gabi::load<u32>(s + 8));
}
/* HD inline anm setFrame (mpCutfBpk / mpCutfBtk): frame (+4), the animation's frame (through the pointer
 * at +ptrOff) and a functor at +fnOff ({f32 result, f32 a, f32 b, ?, fn, arg}) re-evaluated, then
 * 027DF40C(&functor) (as d_a_dr2) */
static inline void anm_setFrame_hd(u32 a, f32 frame, u32 ptrOff, u32 fnOff) {
    u32 fp = gabi::load<u32>(a + ptrOff);
    gabi::store<f32>(a + 4, frame);
    gabi::store<f32>(fp, frame);
    u32 fn = gabi::load<u32>(a + fnOff);
    u32 target = gabi::load<u32>(fn + 0x10);
    f32 p3 = gabi::load<f32>(fn + 8);
    f32 p2 = gabi::load<f32>(fn + 4);
    u32 arg = gabi::load<u32>(fn + 0x14);
    f32 res = gabi::call_ptr<f32>(target, arg, frame, p2, p3);
    gabi::store<f32>(fn, res);
    gabi::call(0x027DF40C, a + fnOff);
}
#define LK_cutfBpk (gabi::ea(this) + 0x4980) /* HD brkAnm (GameCube mpCutfBpk) [?] */
#define LK_cutfBtk (gabi::ea(this) + 0x49F4) /* HD btkAnm (GameCube mpCutfBtk) [?] */
static inline void lk_cutf_setFrame(u32 self, f32 f) {
    anm_setFrame_hd(self + 0x4980, f, 0x10, 0x18);
    anm_setFrame_hd(self + 0x49F4, f, 0x68, 0x10);
}
/* J3DAnmBase::getFrameMax (HD: virtual, vtable at +4, slot 0x14) */
static inline s32 anm_getFrameMax(u32 anm) { return gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(anm + 4) + 0x14), anm); }

/* 0242FA80 */
BOOL daPy_lk_c::procBtJumpCut_init(cXyz* pos) {
    WWHD_FUNC(0x0242FA80, BOOL, this, pos);
    gabi::call(LK_commonProcInit, this, 0x5E /* daPyProc_BT_JUMP_CUT_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x6D /* ANM_MJMPC */, 0.6f, 1.0f, 8, 0.0f); /* HD: HIO folded */
    if (lk_checkNormalSwordEquip()) {
        lk_swordAnim_changeBckOnly(this, 0xC4 /* MJMPCA */);
    } else {
        lk_swordAnim_changeBckOnly(this, 0xC5 /* MJMPCMS */);
    }
    m35EC = 1.0f;
    mNormalSpeed = 0.0f;
    gravity = 0.0f;
    speed.y = 0.0f;
    lk_xyz_copy(&m370C, pos);
    mProcVar6 = 0;
    dComIfGp_onPlayerStatus0_l(0x402 /* daPyStts0_UNK2_e | daPyStts0_UNK400_e */);
    LK_voiceStart(1);
    gabi::call(LK_setBlurPosResource, this, 0x27A /* _BTJUMPCUT_POS */);
    gabi::call(LK_setExtraFinishCutAtParam, this, 5 /* CUT_TYPE_BT_JUMPCUT */);
    lk_startShockUp();
    return TRUE;
}
VERIFY(0x0242FA80, &daPy_lk_c::procBtJumpCut_init);

/* 0242FCA0 */
BOOL daPy_lk_c::procBtJump() {
    WWHD_FUNC(0x0242FCA0, BOOL, this);
    if (mProcVar6 == 0) {
        if (!(mFrameCtrlUnder[0].getFrame() < 0.0f)) { /* HD: HIO folded */
            mNormalSpeed = m35A4;
            speed.y = m35A0;
            mProcVar6 = 1;
        }
    } else {
        if (LK_mAcchChkGroundHit()) {
            return gabi::call<BOOL>(LK_changeLandProc, this, 2.0f);
        }
        if (mProcVar6 == 1) {
            mProcVar0 = mProcVar0 + 1;
            if (speed.y < -gravity) {
                return procBtJumpCut_init(&m370C);
            }
        } else {
            s16 a = cM_atan2s(mNormalSpeed, -speed.y);
            m34F2 = (s16)gabi::ftoi(-3500.0f * cM_ssin(a * 2));
            gabi::call(LK_setFallVoice, this);
        }
    }
    return TRUE;
}
VERIFY(0x0242FCA0, &daPy_lk_c::procBtJump);

/* 0242FDD0 */
BOOL daPy_lk_c::procBtJumpCut() {
    WWHD_FUNC(0x0242FDD0, BOOL, this);
    m35EC = mFrameCtrlUnder[0].getFrame();
    if (speed.y < 0.0f) {
        s16 a = cM_atan2s(mNormalSpeed, -speed.y);
        m34F2 = (s16)gabi::ftoi(-3500.0f * cM_ssin(a * 2));
    }
    f32 frame = mFrameCtrlUnder[0].getFrame();
    if (!(frame < 2.5f) && frame < 6.5f) {
        setResetFlg0(resetFlg0() | 2);
        if (!(mNoResetFlg0 & 0x40 /* daPyFlg0_CUT_AT_FLG */)) {
            setResetFlg0(resetFlg0() | 1);
            seStartSwordCut(0x2800 /* JA_SE_LK_SW_KAZEKIRI_S */);
        }
    }
    if (LK_mAcchChkGroundHit()) {
        return gabi::call<BOOL>(LK_changeLandProc, this, 2.0f);
    }
    if (mProcVar6 == 0) {
        if (mFrameCtrlUnder[0].getRate() < 0.01f) {
            mNormalSpeed = 12.5f;
            gravity = -3.75f;
            mProcVar6 = 1;
            speed.y = 32.5f;
        }
    } else if (mProcVar6 == 1 && speed.y < -gravity) {
        mProcVar6 = 2;
        gabi::call(LK_setSingleMoveAnime, this, 0xD /* ANM_JMPEDS */, 0.0f, 0.0f, -1, 10.0f);
        mModeFlg = mModeFlg & ~0x400u;
        gabi::call(LK_setTextureAnime, this, 0x37, 0);
        resetSeAnime();
    }
    gabi::call(LK_setFallVoice, this);
    return TRUE;
}
VERIFY(0x0242FDD0, &daPy_lk_c::procBtJumpCut);

/* 0242FFD8 */
BOOL daPy_lk_c::procBtSlide() {
    WWHD_FUNC(0x0242FFD8, BOOL, this);
    f32 r = cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.5f, 6.25f, 2.5f); /* HD: HIO folded */
    if (!(r > 0.001f)) {
        LK_checkNextMode(0);
    }
    if (mNormalSpeed < 2.5f) {
        gabi::call(LK_resetFootEffect, this);
    }
    return TRUE;
}
VERIFY(0x0242FFD8, &daPy_lk_c::procBtSlide);

/* 02430078 */
BOOL daPy_lk_c::procBtRollCut_init(cXyz* pos) {
    WWHD_FUNC(0x02430078, BOOL, this, pos);
    gabi::call(LK_commonProcInit, this, 0x61 /* daPyProc_BT_ROLL_CUT_e */);
    s16 d = (s16)(current.angle.y - shape_angle.y);
    if (d > 0) {
        m34EC = 1;
        mProcVar6 = 1;
        gabi::call(LK_setBlurPosResource, this, 0x27B /* _BTROTATECUTL_POS */);
        gabi::call(LK_setSingleMoveAnime, this, 0x70 /* ANM_MROLLLC */, 0.5f, 3.0f, 6, 0.0f);
    } else {
        m34EC = -1;
        mProcVar6 = -1;
        gabi::call(LK_setBlurPosResource, this, 0x27C /* _BTROTATECUTR_POS */);
        gabi::call(LK_setSingleMoveAnime, this, 0x71 /* ANM_MROLLRC */, 0.5f, 3.0f, 6, 0.0f);
    }
    if (lk_checkNormalSwordEquip()) {
        lk_swordAnim_changeBckOnly(this, 0xC7 /* MROLLCA */);
    } else {
        lk_swordAnim_changeBckOnly(this, 0xC8 /* MROLLCMS */);
    }
    m35EC = 3.0f;
    lk_xyz_copy(&m370C, pos);
    mNormalSpeed = 20.0f;
    gravity = -4.0f;
    mProcVar2 = 0;
    speed.y = 40.0f;
    LK_voiceStart(1);
    gabi::call(LK_setExtraFinishCutAtParam, this, 0xF /* CUT_TYPE_BT_ROLLCUT */);
    mNoResetFlg0 = mNoResetFlg0 & ~0x40000u; /* daPyFlg0_NO_FALL_VOICE */
    setResetFlg0(resetFlg0() | 3);
    seStartSwordCut(0x2800 /* JA_SE_LK_SW_KAZEKIRI_S */);
    dComIfGp_onPlayerStatus0_l(0x402);
    lk_startShockUp();
    return TRUE;
}
VERIFY(0x02430078, &daPy_lk_c::procBtRollCut_init);

/* 024302C0 */
BOOL daPy_lk_c::procBtRoll() {
    WWHD_FUNC(0x024302C0, BOOL, this);
    s16 old = mProcVar2;
    cLib_addCalcAngleS(&mProcVar2, 0, 5, 0x6A4, 0x1E); /* HD: HIO folded */
    s16 d = (s16)(old - mProcVar2);
    if (old > 0) {
        current.angle.y = (s16)(mProcVar3 + 0x4000);
    } else {
        current.angle.y = (s16)(mProcVar3 - 0x4000);
    }
    gabi::Local<cXyz> center; /* sp+0x08 */
    f32 r = m35A0;
    center->x = gabi::fnmsubs(r, cM_ssin(mProcVar3), current.pos.x);
    center->y = current.pos.y;
    center->z = gabi::fnmsubs(r, cM_scos(mProcVar3), current.pos.z);
    mProcVar3 = (s16)(mProcVar3 + d);
    current.pos.x = gabi::fmadds(r, cM_ssin(mProcVar3), center->x);
    current.pos.z = gabi::fmadds(r, cM_scos(mProcVar3), center->z);
    shape_angle.y = (s16)(mProcVar3 + 0x8000);
    if (std::abs((s32)d) < 0x1E) {
        gabi::call(LK_resetFootEffect, this);
    }
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    if (frameCtrl.getRate() < 0.01f) {
        procBtRollCut_init(center);
    } else if (frameCtrl.checkPass(10.0f)) {
        gabi::call(LK_endFlameDamageEmitter, this);
    }
    return TRUE;
}
VERIFY(0x024302C0, &daPy_lk_c::procBtRoll);

/* 024304C8 */
BOOL daPy_lk_c::procBtRollCut() {
    WWHD_FUNC(0x024304C8, BOOL, this);
    m35EC = mFrameCtrlUnder[0].getFrame();
    s16 target = cLib_targetAngleY(&current.pos, &m370C);
    cLib_addCalcAngleS(&shape_angle.y, target, 5, 0x5E8, 0x13C);
    current.angle.y = (s16)(shape_angle.y + mProcVar6 * 0x4000);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        if (mProcVar2 == 0) {
            mProcVar2 = 1;
            setResetFlg0(resetFlg0() | 2);
        } else if (mProcVar2 == 1) {
            gabi::call(LK_setSingleMoveAnime, this, 0xD /* ANM_JMPEDS */, 0.0f, 0.0f, -1, 15.0f);
            mModeFlg = mModeFlg & ~0x400u;
            gabi::call(LK_setTextureAnime, this, 0x37, 0);
            resetSeAnime();
            mProcVar2 = 2;
        }
        if (mProcVar2 > 1) {
            s32 dir = mProcVar6;
            if ((dir > 0 && m34EC < 0) || (dir < 0 && m34EC > 0)) {
                cLib_addCalcAngleS(&m34EC, (s16)-dir, 4, 0x1800, 0x200);
            } else {
                m34EC = (s16)(m34EC + dir * 0x1800);
            }
        }
        if (LK_mAcchChkGroundHit()) {
            s16 a = (s16)(shape_angle.y + m34EC);
            current.angle.y = a;
            shape_angle.y = a;
            return gabi::call<BOOL>(LK_changeLandProc, this, 1.3f);
        }
    } else {
        setResetFlg0(resetFlg0() | 2);
    }
    if (speed.y < -gravity) {
        gravity = -3.5f;
    }
    gabi::call(LK_setFallVoice, this);
    return TRUE;
}
VERIFY(0x024304C8, &daPy_lk_c::procBtRollCut);

/* 024306C8 */
BOOL daPy_lk_c::procBtVerticalJumpCut_init() {
    WWHD_FUNC(0x024306C8, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x63 /* daPyProc_BT_VERTICAL_JUMP_CUT_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x73 /* ANM_MSTEPOVERA */, 1.5f, 4.0f, 0x13, 8.0f);
    lk_cutf_setFrame(gabi::ea(this), 0.0f);
    gravity = -7.0f;
    gabi::call(LK_setExtraFinishCutAtParam, this, 0x10 /* CUT_TYPE_BT_VERTICALJUMPCUT */);
    dComIfGp_onPlayerStatus0_l(0x402);
    return TRUE;
}
VERIFY(0x024306C8, &daPy_lk_c::procBtVerticalJumpCut_init);

/* 024307E4 */
BOOL daPy_lk_c::procBtVerticalJump() {
    WWHD_FUNC(0x024307E4, BOOL, this);
    gabi::call(LK_setFallVoice, this);
    if (LK_mAcchChkGroundHit()) {
        gabi::call(LK_changeLandProc, this, 1.3f);
    } else if (speed.y < -(gravity + gravity)) {
        procBtVerticalJumpCut_init();
    }
    return TRUE;
}
VERIFY(0x024307E4, &daPy_lk_c::procBtVerticalJump);

/* 02430858 */
BOOL daPy_lk_c::procBtVerticalJumpLand_init() {
    WWHD_FUNC(0x02430858, BOOL, this);
    f32 h = m35F0 - current.pos.y;
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0 /* !dComIfGp_event_runCheck() */) {
        if (!(h < 6000.0f)) { /* HD: HIO folded */
            gabi::call(LK_setDamagePoint, this, -2.0f);
        } else if (!(h < 2000.0f)) {
            gabi::call(LK_setDamagePoint, this, -1.0f);
        }
    }
    gabi::call(LK_commonProcInit, this, 0x64 /* daPyProc_BT_VERTICAL_JUMP_LAND_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x74 /* ANM_MSTEPOVERLAND */, 1.0f, 2.0f, 0x11, 0.0f);
    lk_startShockUp();
    if (!(mNoResetFlg0 & 0x10000000)) {
        seStartOnlyReverb(0x283C /* JA_SE_LK_JUMP_ATTACK */);
    }
    return TRUE;
}
VERIFY(0x02430858, &daPy_lk_c::procBtVerticalJumpLand_init);

/* 02430988 */
BOOL daPy_lk_c::procBtVerticalJumpCut() {
    WWHD_FUNC(0x02430988, BOOL, this);
    gabi::call(LK_setFallVoice, this);
    f32 frame = mFrameCtrlUnder[0].getFrame() - 9.0f;
    if (frame < 0.0f) {
        frame = 0.0f;
    } else if (!((f32)anm_getFrameMax(gabi::load<u32>(gabi::ea(this) + 0x4990)) < frame)) {
        frame = (f32)anm_getFrameMax(gabi::load<u32>(gabi::ea(this) + 0x4990)) - 0.001f;
    }
    lk_cutf_setFrame(gabi::ea(this), frame);
    if (LK_mAcchChkGroundHit()) {
        procBtVerticalJumpLand_init();
    } else if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        if (!(mNoResetFlg0 & 0x40 /* daPyFlg0_CUT_AT_FLG */)) {
            LK_voiceStart(1);
            setResetFlg0(resetFlg0() | 1);
            seStartSwordCut(0x2800 /* JA_SE_LK_SW_KAZEKIRI_S */);
            gravity = -10.0f;
        }
        setResetFlg0(resetFlg0() | 2);
    }
    return TRUE;
}
VERIFY(0x02430988, &daPy_lk_c::procBtVerticalJumpCut);

/* 02430C00 */
BOOL daPy_lk_c::procBtVerticalJumpLand() {
    WWHD_FUNC(0x02430C00, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        gabi::call(LK_procBackJump_init, this);
        mNormalSpeed = 5.5f;
        speed.y = 25.0f;
    }
    return TRUE;
}
VERIFY(0x02430C00, &daPy_lk_c::procBtVerticalJumpLand);

/* ---- ship ---- */
enum : u32 {
    LK_setBodyAngleXReadyAnime = 0x02427230,
    LK_deleteEquipItem = 0x023DC7AC,
    LK_setShipRidePos = 0x023E287C,
    LK_setScopeModel = 0x023DED8C,
    LK_setPhotoBoxModel = 0x023DEDE4,
    LK_setBowModel = 0x023DEA24,
    LK_setBowReadyAnime = 0x023E7BEC,
    LK_makeItemType = 0x023DF600,
    LK_orderTalk = 0x023EC724,
    LK_checkShipNotNormalMode = 0x023D69CC,
    LK_checkSetItemTrigger = 0x023E9AF4,
    LK_procShipSteer_init = 0x023F1D84,
    LK_procShipPaddle_init = 0x023F1E60,
    LK_procShipCannon_init = 0x023F1AC8,
    LK_procShipCrane_init = 0x023F1C68,
    LK_procTactWait_init = 0x023E92A0,
    LK_changeDragonShield = 0x023E9D50,
    LK_procBottleDrink_init = 0x023EF280,
    LK_procBottleOpen_init = 0x023EF3E0,
    LK_procBottleSwing_init = 0x023EDD9C,
    LK_procFoodSet_init = 0x023EEB70,
    LK_procFoodThrow_init = 0x023EED74,
};
#define mBodyAngleY LK_FIELD(s16, 0x3D2) /* daPy_py_c mBodyAngle.y (GameCube 0x2B6) */
#define mHDBoomerangFlag LK_FIELD(u8, 0x827C) /* HD-only (tail): cleared by procShipBoomerang_init [?] */
#define mHDTingleTalk LK_FIELD(u8, 0x8264)    /* HD-only (tail): set when the ship's Tingle Tuner talk event is ordered */
/* dComIfGp_getShipActor(): play + 0x5B3C */
static inline u32 dComIfGp_getShipActor_l() { return gabi::load<u32>(dComIfGp_ea() + 0x5B3C); }
/* dComIfGp_getSelectItem(btn): play + 0x5BBB + btn */
static inline u8 dComIfGp_getSelectItem_l(u32 btn) { return gabi::load<u8>(dComIfGp_ea() + 0x5BBB + btn); }
/* daBoomerang_c::onCancelFlg() (HD: the flag at +0x26274) */
static inline void lk_boomerang_onCancelFlg(u32 b) {
    if (b != 0) {
        gabi::store<u8>(b + 0x26274, 1);
    }
}
/* play + 0x5CDC: dComIfGp_checkPlayerStatus1(0, flag) */
static inline u32 dComIfGp_checkPlayerStatus1_l(u32 flag) { return gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & flag; }
/* play + 0x5BB7 / 0x5BB6: the do status and the HD second (B) button status */
static inline u8 dComIfGp_getDoStatus_l() { return gabi::load<u8>(dComIfGp_ea() + 0x5BB7); }
static inline u8 dComIfGp_getBStatus_l() { return gabi::load<u8>(dComIfGp_ea() + 0x5BB6); }
static inline void dComIfGp_setBStatus_l(u8 s) { gabi::store<u8>(dComIfGp_ea() + 0x5BB6, s); }

/* 02430C60 */
void daPy_lk_c::setShipAttentionAnmSpeed(f32 rate) {
    WWHD_FUNC(0x02430C60, void, this, rate);
    if (LK_checkAttentionLock()) {
        mFrameCtrlUnder[0].setRate(1.25f); /* HD: HIO folded */
        mFrameCtrlUpper[2].setRate(rate);
    } else {
        mFrameCtrlUnder[0].setRate(0.0f);
        mFrameCtrlUnder[0].setFrame(0.0f);
        mFrameCtrlUpper[2].setRate(0.0f);
        mFrameCtrlUpper[2].setFrame(0.0f);
    }
}
VERIFY(0x02430C60, &daPy_lk_c::setShipAttentionAnmSpeed);

/* 02430D28 */
void daPy_lk_c::setShipAttnetionBodyAngle() {
    WWHD_FUNC(0x02430D28, void, this);
    gabi::call(LK_setBodyAngleXReadyAnime, this);
    s16 target = 0;
    if (mpAttnActorLockOn != nullptr) {
        gabi::call(LK_setShapeAngleToAtnActor, this);
    } else {
        f32 stick = mStickDistance; /* kept across the call (GHS) */
        if (stick > 0.05f) {
            s32 dir = getDirectionFromAngle(m34DC);
            if (dir == 2 /* DIR_LEFT */) {
                s16 lim = (s16)gabi::ftoi(512.0f * stick);
                if (lim > mProcVar5) {
                    s16 v = (s16)(mProcVar5 + gabi::ftoi(64.0f * stick));
                    if (v > lim) {
                        v = lim;
                    }
                    mProcVar5 = v;
                    target = v;
                } else {
                    target = lim;
                }
            } else if (dir == 3 /* DIR_RIGHT */) {
                s16 lim = (s16)gabi::ftoi(-512.0f * stick);
                if (lim < mProcVar5) {
                    s16 v = (s16)(mProcVar5 - gabi::ftoi(64.0f * stick));
                    if (v < lim) {
                        v = lim;
                    }
                    mProcVar5 = v;
                    target = v;
                } else {
                    target = lim;
                }
            }
        }
        cLib_addCalcAngleS(&mProcVar5, target, 3, 0x40, 0x10);
        shape_angle.y = (s16)(shape_angle.y + mProcVar5);
    }
    current.angle.y = shape_angle.y;
}
VERIFY(0x02430D28, &daPy_lk_c::setShipAttnetionBodyAngle);

/* 02430EC0 */
BOOL daPy_lk_c::procShipGetOff_init() {
    WWHD_FUNC(0x02430EC0, BOOL, this);
    if (mCurProc == 0x90 /* daPyProc_SHIP_GET_OFF_e */) {
        return FALSE;
    }
    u32 ship = dComIfGp_getShipActor_l();
    gabi::call(LK_commonProcInit, this, 0x90);
    gabi::call(LK_deleteEquipItem, this, 1);
    gabi::call(LK_setSingleMoveAnime, this, 2 /* ANM_DASH */, 0.8f, 0.0f, -1, 5.0f); /* HD: HIO folded */
    gravity = 0.0f;
    mNormalSpeed = 8.0f;
    s16 a = (s16)(gabi::load<s16>(ship + 0x32A) - 0x4000);
    mProcVar6 = 0;
    current.angle.y = a;
    shape_angle.y = a;
    m370C.x = gabi::fmadds(57.0f, cM_ssin(a), current.pos.x); /* l_ship_ledge */
    m370C.y = gabi::load<f32>(ship + 0x318) + 35.0f;
    m370C.z = gabi::fmadds(57.0f, cM_scos(a), current.pos.z);
    gabi::store<u8>(ship + 0x636, 5); /* ship->setGetOffFirst() */
    mBodyAngleY = 0;
    m35A0 = gabi::load<f32>(ship + 0x318);
    return TRUE;
}
VERIFY(0x02430EC0, &daPy_lk_c::procShipGetOff_init);

/* 02431028 HD-only (unnamed): the ship's put-away trigger: the B trigger, or the HD pad buttons
 * (*(0x101F5088) + 0x18) while player status 1 bit 4 (0x40000) or bit 2 (0x80000) is set */
BOOL daPy_lk_c::checkShipPutAwayTrigger() {
    WWHD_FUNC(0x02431028, BOOL, this);
    if (mItemTrigger & 2) {
        return TRUE;
    }
    if (dComIfGp_checkPlayerStatus1_l(4) && (gabi::load<u32>(gabi::load<u32>(0x101F5088) + 0x18) & 0x40000)) {
        return TRUE;
    }
    if (dComIfGp_checkPlayerStatus1_l(2) && (gabi::load<u32>(gabi::load<u32>(0x101F5088) + 0x18) & 0x80000)) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02431028, &daPy_lk_c::checkShipPutAwayTrigger);

/* 024310B8 */
BOOL daPy_lk_c::procShipScope_init(int scope) {
    WWHD_FUNC(0x024310B8, BOOL, this, scope);
    u32 ship = dComIfGp_getShipActor_l();
    gabi::call(LK_commonProcInit, this, 0x8A /* daPyProc_SHIP_SCOPE_e */);
    gabi::call(LK_deleteEquipItem, this, 1);
    gravity = 0.0f;
    gabi::store<u8>(ship + 0x636, 2); /* ship->setPaddleMove() */
    gabi::call(LK_setShipRidePos, this, 0);
    gabi::call(LK_setSingleMoveAnime, this, 0 /* ANM_WAITS */, 1.1f, 0.0f, -1, 5.0f); /* HD: HIO folded */
    dComIfGp_onPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */);
    gabi::call(0x025E1988 /* seStartSystem */, 0x822 /* JA_SE_ITM_SUBMENU_IN_1 */);
    mNoResetFlg0 = mNoResetFlg0 & ~0x80000u; /* daPyFlg0_SCOPE_CANCEL */
    u8 btn = mReadyItemBtn;
    mEquipItem = dComIfGp_getSelectItem_l(btn);
    u32 play = dComIfGp_ea();
    if (scope != 0) {
        gabi::store<u32>(play + 0x5CD8, gabi::load<u32>(play + 0x5CD8) | 0x200000 /* daPyStts0_TELESCOPE_LOOK_e */);
        gabi::call(LK_setScopeModel, this);
    } else {
        gabi::store<u32>(play + 0x5CDC, gabi::load<u32>(play + 0x5CDC) | 8 /* daPyStts1_PICTO_BOX_AIM_e */);
        u32 a = dComIfGp_ea() + 0x5CDC;
        gabi::store<u32>(a, gabi::load<u32>(a) & ~0x100000u); /* HD */
        gabi::call(LK_setPhotoBoxModel, this);
    }
    return TRUE;
}
VERIFY(0x024310B8, &daPy_lk_c::procShipScope_init);

/* 024311FC */
BOOL daPy_lk_c::procShipBow_init() {
    WWHD_FUNC(0x024311FC, BOOL, this);
    u32 ship = dComIfGp_getShipActor_l();
    if (mCurProc == 0x8D /* daPyProc_SHIP_BOW_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x8D);
    gabi::call(LK_deleteEquipItem, this, 1);
    gravity = 0.0f;
    gabi::store<u8>(ship + 0x636, 2); /* ship->setPaddleMove() */
    if (mDemoMode == 0x44 /* checkBowMiniGame() */) {
        mEquipItem = 0x27; /* dItemNo_BOW_e */
    } else {
        u8 btn = mReadyItemBtn;
        mEquipItem = dComIfGp_getSelectItem_l(btn);
    }
    gabi::call(LK_setSingleMoveAnime, this, 8 /* ANM_ATNRS */, 0.0f, 0.0f, -1, 5.0f);
    gabi::call(LK_setBowModel, this);
    gabi::call(LK_setBowReadyAnime, this);
    dComIfGp_onPlayerStatus0_l(0x11000 /* daPyStts0_BOW_AIM_e | daPyStts0_SHIP_RIDE_e */);
    gabi::call(LK_setShipRidePos, this, 0);
    mProcVar2 = gabi::load<s16>(ship + 0x32A);
    mProcVar5 = 0;
    if (LK_checkAttentionLock() && mDemoMode != 0x44) {
        mProcVar6 = 1;
    } else {
        mProcVar6 = 0;
    }
    return TRUE;
}
VERIFY(0x024311FC, &daPy_lk_c::procShipBow_init);

/* 02431404 */
BOOL daPy_lk_c::procShipBoomerang_init() {
    WWHD_FUNC(0x02431404, BOOL, this);
    u32 ship = dComIfGp_getShipActor_l();
    if (mCurProc == 0x8B /* daPyProc_SHIP_BOOMERANG_e */) {
        return FALSE;
    }
    mHDBoomerangFlag = 0; /* HD */
    m355C = 0;
    m355E = itemButton() == 0;
    gabi::call(LK_commonProcInit, this, 0x8B);
    gabi::call(LK_deleteEquipItem, this, 1);
    gravity = 0.0f;
    gabi::store<u8>(ship + 0x636, 2); /* ship->setPaddleMove() */
    mEquipItem = 0x2D; /* dItemNo_BOOMERANG_e */
    gabi::call(LK_setSingleMoveAnime, this, 8 /* ANM_ATNRS */, 0.0f, 0.0f, -1, 5.0f);
    gabi::call(LK_setActAnimeUpper, this, 0x35 /* BOOMWAIT */, 2 /* UPPER_MOVE2_e */, 0.0f, 0.0f, -1, -1.0f);
    gabi::call(LK_setTextureAnime, this, 0, 0);
    gabi::call(LK_makeItemType, this);
    dComIfGp_onPlayerStatus0_l(0x90000 /* daPyStts0_SHIP_RIDE_e | daPyStts0_BOOMERANG_AIM_e */);
    gabi::call(LK_setShipRidePos, this, 0);
    s16 a = gabi::load<s16>(ship + 0x32A);
    mProcVar5 = 0;
    mProcVar2 = a;
    return TRUE;
}
VERIFY(0x02431404, &daPy_lk_c::procShipBoomerang_init);

/* 0243155C */
BOOL daPy_lk_c::procShipHookshot_init() {
    WWHD_FUNC(0x0243155C, BOOL, this);
    u32 ship = dComIfGp_getShipActor_l();
    if (mCurProc == 0x8C /* daPyProc_SHIP_HOOKSHOT_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x8C);
    gabi::call(LK_deleteEquipItem, this, 1);
    gravity = 0.0f;
    gabi::store<u8>(ship + 0x636, 2); /* ship->setPaddleMove() */
    m355C = 10;
    mEquipItem = 0x2F; /* dItemNo_HOOKSHOT_e */
    gabi::call(LK_setSingleMoveAnime, this, 7 /* ANM_ATNLS */, 0.0f, 0.0f, -1, 5.0f);
    gabi::call(LK_setActAnimeUpper, this, 0xA7 /* HOOKSHOTWAIT */, 2 /* UPPER_MOVE2_e */, 0.0f, 0.0f, -1, -1.0f);
    gabi::call(LK_setTextureAnime, this, 0, 0);
    gabi::call(LK_makeItemType, this);
    dComIfGp_onPlayerStatus0_l(0x14000 /* daPyStts0_HOOKSHOT_AIM_e | daPyStts0_SHIP_RIDE_e */);
    gabi::call(LK_setShipRidePos, this, 0);
    mProcVar2 = gabi::load<s16>(ship + 0x32A);
    mProcVar6 = LK_checkAttentionLock() ? 1 : 0;
    return TRUE;
}
VERIFY(0x0243155C, &daPy_lk_c::procShipHookshot_init);

/* 024316EC */
BOOL daPy_lk_c::changeShipEndProc() {
    WWHD_FUNC(0x024316EC, BOOL, this);
    u32 ship = dComIfGp_getShipActor_l();
    if (ship == 0) {
        return LK_checkNextMode(0);
    }
    mNoResetFlg0 = mNoResetFlg0 & ~0x200u; /* daPyFlg0_SHIP_DROP */
    if (mItemTrigger & 1 /* doTrigger() */) {
        lk_boomerang_onCancelFlg(gabi::ea(mActorKeepThrow.mActor.get()));
    }
    s32 proc = mCurProc;
    if (proc != 0x8A && proc != 0x8D && proc != 0x8C && (proc != 0x8B || mEquipItem != 0x2D) &&
        !(gabi::load<u32>(ship + 0x644) & 1) /* ship->getFlyFlg() */) {
        gabi::call(LK_setTalkStatus, this);
        if (gabi::call<BOOL>(LK_orderTalk, this)) {
            return TRUE;
        }
    }
    if (gabi::load<u8>(dComIfGp_ea() + 0x5CEA) != 1 && gabi::load<f32>(ship + 0x370) < 3.0f) {
        gabi::Local<cXyz> belt; /* ship->getBeltSpeed() (HD: the XZ length of the vector at +0x132C) */
        f32 bx = gabi::load<f32>(ship + 0x132C);
        belt->y = 0.0f;
        belt->z = gabi::load<f32>(ship + 0x1334);
        belt->x = bx;
        if (std_sqrtf(PSVECSquareMag(belt)) < 3.0f) {
            if (!gabi::call<BOOL>(LK_checkShipNotNormalMode, this) ||
                (u32)(gabi::load<u16>(gabi::ea(this) + 0x5848) - 0x8A) <= 2 /* FREEA, FREEB, FREED */) {
                if (!dComIfGp_checkPlayerStatus0_l(0x2000 /* daPyStts0_SUBJECT_e */) && !(mItemButton & 0x40) &&
                    mCurProc == 0x89 /* daPyProc_SHIP_PADDLE_e */ && gabi::load<u32>(ship + 0x704) == 0 &&
                    gabi::load<u32>(ship + 0x70C) == 0 /* !ship->checkForceMove() */) {
                    /* HD: the get-off action moved to the B button */
                    if (dComIfGp_getDoStatus_l() == 0) {
                        dComIfGp_setDoStatus_l(0x14);
                    }
                    if (dComIfGp_getBStatus_l() == 0) {
                        dComIfGp_setBStatus_l(0x1D /* GET_OUT_SHIP */);
                        if (mItemTrigger & 2) {
                            return procShipGetOff_init();
                        }
                    }
                }
            }
        }
    }
    if (mCurProc == 0x8A || (gabi::load<u32>(ship + 0x644) & 1)) {
        return FALSE;
    }
    u8 part = gabi::load<u8>(ship + 0x637);
    if (part == 1 /* PART_STEER_e */ || gabi::load<u32>(ship + 0x704) != 0 || gabi::load<u32>(ship + 0x70C) != 0 ||
        (gabi::load<f32>(ship + 0x370) < 3.0f && part != 0 /* PART_WAIT_e */)) {
        if (dComIfGp_getBStatus_l() == 0) {
            dComIfGp_setBStatus_l(8 /* PUT_AWAY */);
        }
        /* HD: while steering with the sail (0x77) selected, the do status shows the sail state */
        if (gabi::load<u8>(ship + 0x637) == 1 && gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x5D) == 0x77 &&
            gabi::load<u8>(dComIfGp_ea() + 0x5CEA) != 1) {
            u8 st = gabi::load<u8>(0x100360D4 + (gabi::load<u8>(dComIfGp_ea() + 0x5D2C) & 1));
            if (dComIfGp_getDoStatus_l() == 0) {
                dComIfGp_setDoStatus_l(st);
            }
        }
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x78 /* dItemNo_SAIL_e */, 1) ||
        gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x77, 1) /* HD */) {
        lk_boomerang_onCancelFlg(gabi::ea(mActorKeepThrow.mActor.get()));
        gabi::store<u8>(dComIfGp_ea() + 0x5D2C, 0); /* HD */
        return gabi::call<BOOL>(LK_procShipSteer_init, this);
    }
    if (dComIfGp_getBStatus_l() == 8 && checkShipPutAwayTrigger()) { /* HD */
        return gabi::call<BOOL>(LK_procShipPaddle_init, this);
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x31 /* dItemNo_BOMB_BAG_e */, 1)) {
        lk_boomerang_onCancelFlg(gabi::ea(mActorKeepThrow.mActor.get()));
        return gabi::call<BOOL>(LK_procShipCannon_init, this);
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x25 /* dItemNo_GRAPPLING_HOOK_e */, 1)) {
        lk_boomerang_onCancelFlg(gabi::ea(mActorKeepThrow.mActor.get()));
        return gabi::call<BOOL>(LK_procShipCrane_init, this);
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x20 /* dItemNo_TELESCOPE_e */, 1)) {
        lk_boomerang_onCancelFlg(gabi::ea(mActorKeepThrow.mActor.get()));
        return procShipScope_init(1);
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x109 /* daPyItem_PHOTOBOX_e */, 1)) {
        lk_boomerang_onCancelFlg(gabi::ea(mActorKeepThrow.mActor.get()));
        return procShipScope_init(0);
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x108 /* daPyItem_BOW_e */, 1)) {
        lk_boomerang_onCancelFlg(gabi::ea(mActorKeepThrow.mActor.get()));
        return procShipBow_init();
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x2D /* dItemNo_BOOMERANG_e */, 1) && mActorKeepThrow.mActor == nullptr) {
        return procShipBoomerang_init();
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x2F /* dItemNo_HOOKSHOT_e */, 1)) {
        lk_boomerang_onCancelFlg(gabi::ea(mActorKeepThrow.mActor.get()));
        return procShipHookshot_init();
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x22 /* dItemNo_WIND_WAKER_e */, 1)) {
        lk_boomerang_onCancelFlg(gabi::ea(mActorKeepThrow.mActor.get()));
        return gabi::call<BOOL>(LK_procTactWait_init, this, -1);
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x21 /* dItemNo_TINGLE_TUNER_e */, 1)) {
        /* HD: fopAcM_orderTalkEvent(this, dComIfGp_getAgb()) inlined, with an HD flag */
        mHDTingleTalk = 1;
        u32 agb = gabi::load<u32>(dComIfGp_ea() + 0x5B2C);
        gabi::call(0x0253EC0C /* dEvt_control_c::order */, dComIfGp_ea() + 0x51D0, 3, 1, 0, 0xFFFF, this, agb, -1, 0xFF);
        s32 snap = 1;
        if (gabi::call<BOOL>(LK_checkShipNotNormalMode, this)) {
            snap = 0;
        }
        gabi::call(LK_setShipRidePos, this, snap);
        return TRUE;
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x2A /* dItemNo_MAGIC_ARMOR_e */, 1)) {
        s32 snap = 1;
        if (gabi::call<BOOL>(LK_checkShipNotNormalMode, this)) {
            snap = 0;
        }
        gabi::call(LK_setShipRidePos, this, snap);
        return gabi::call<BOOL>(LK_changeDragonShield, this, 1);
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x105 /* daPyItem_DRINK_BOTTLE_e */, 0)) {
        u8 btn = mReadyItemBtn;
        return gabi::call<BOOL>(LK_procBottleDrink_init, this, (u32)dComIfGp_getSelectItem_l(btn));
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x57 /* dItemNo_FAIRY_BOTTLE_e */, 0)) {
        return gabi::call<BOOL>(LK_procBottleOpen_init, this, 0x57);
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x106 /* daPyItem_OPEN_BOTTLE_e */, 1)) {
        u8 btn = mReadyItemBtn;
        return gabi::call<BOOL>(LK_procBottleOpen_init, this, (u32)dComIfGp_getSelectItem_l(btn));
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x50 /* dItemNo_EMPTY_BOTTLE_e */, 1)) {
        return gabi::call<BOOL>(LK_procBottleSwing_init, this, 1);
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x83 /* dItemNo_HYOI_PEAR_e */, 1)) {
        return gabi::call<BOOL>(LK_procFoodSet_init, this);
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x107 /* daPyItem_ESA_e */, 1)) {
        return gabi::call<BOOL>(LK_procFoodThrow_init, this);
    }
    /* HD: the do button toggles the sail state (play + 0x5D2C) */
    if ((mItemTrigger & 1) && (dComIfGp_getDoStatus_l() == 0x42 || dComIfGp_getDoStatus_l() == 0x41)) {
        u8 v = (u8)((gabi::load<u8>(dComIfGp_ea() + 0x5D2C) + 1) & 1);
        gabi::store<u8>(dComIfGp_ea() + 0x5D2C, v);
    }
    return FALSE;
}
VERIFY(0x024316EC, &daPy_lk_c::changeShipEndProc);

enum : u32 {
    LK_seenActorAngleY = 0x025D68A0, /* fopAcM_seenActorAngleY */
    LK_checkSubjectEnd = 0x02415EF0,
    LK_initShipBaseAnime = 0x023F19D0,
    LK_checkScopeEnd = 0x02415AF8, /* unnamed by the matcher: checkScopeEnd (body as GameCube) */
};
/* ship->getBodyMtx(): the body morf (+0x3B4) model's (+0x90) base matrix (NULL-preserving) */
static inline Mtx34* lk_shipBodyMtx(u32 ship) {
    u32 m = gabi::load<u32>(gabi::load<u32>(ship + 0x3B4) + 0x90);
    return gabi::at<Mtx34>(m ? m + 0xC8 : 0);
}
/* 020076E0: button bit 2 held on pad n (*(0x101F5088) + 0x124) (d_a_ship: A, d_camera_util: B) */
static inline BOOL CPad_holdBit2_l(s32 pad) { return gabi::call<BOOL>(0x020076E0, pad); }
/* play + 0x5BB5: dComIfGp_setRStatus */
static inline void dComIfGp_setRStatus_l(u8 s) { gabi::store<u8>(dComIfGp_ea() + 0x5BB5, s); }
/* camera attention status of a camera info index: play + 0x5B00 + idx * 0x34 */
static inline u32 dComIfGp_getCameraAttentionStatus_l(s32 idx) { return gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5B00); }
/* dComIfGp_event_runCheck(): play + 0x5292 */
static inline u8 dComIfGp_event_runCheck_l() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }

/* 0243213C */
BOOL daPy_lk_c::procShipReady() {
    WWHD_FUNC(0x0243213C, BOOL, this);
    u32 ship = dComIfGp_getShipActor_l();
    if (ship == 0) {
        return LK_checkNextMode(0);
    }
    if (mProcVar6 == 0) {
        s16 side = mProcVar0;
        Mtx34* mtx = lk_shipBodyMtx(ship);
        if (side == 0) {
            PSMTXMultVec(mtx, gabi::at<cXyz>(0x10034FB4) /* l_ship_ledge */, &current.pos);
        } else {
            PSMTXMultVec(mtx, gabi::at<cXyz>(0x10034FA8) /* l_ship_redge */, &current.pos);
        }
        if (mFrameCtrlUnder[0].getRate() < 0.01f) {
            mProcVar6 = 1;
            mNormalSpeed = 6.0f;
            gabi::call(LK_setSingleMoveAnime, this, 1 /* ANM_WALK */, 0.8f, 0.0f, -1, 5.0f); /* HD: HIO folded */
            m34C2 = 10;
            mModeFlg = mModeFlg & ~0x420u; /* ModeFlg_HANG | ModeFlg_00000400 */
            gabi::store<u8>(ship + 0x636, 4); /* ship->setReadySecond() */
            m35A0 = gabi::load<f32>(ship + 0x318);
        }
    } else {
        current.pos.y = current.pos.y + (gabi::load<f32>(ship + 0x318) - m35A0);
        cLib_chaseF(&current.pos.y, gabi::load<f32>(ship + 0x318) + 15.0f /* l_ship_offset.y */, 2.0f);
        m35A0 = gabi::load<f32>(ship + 0x318);
        if (gabi::call<s32>(LK_seenActorAngleY, this, ship) >= 0x4000) {
            gabi::call(LK_procShipPaddle_init, this);
        }
    }
    return TRUE;
}
VERIFY(0x0243213C, &daPy_lk_c::procShipReady);

/* 024322EC */
BOOL daPy_lk_c::procShipJumpRide() {
    WWHD_FUNC(0x024322EC, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        gabi::call(LK_procShipPaddle_init, this);
    } else {
        gabi::call(LK_setShipRidePos, this, 0);
    }
    return TRUE;
}
VERIFY(0x024322EC, &daPy_lk_c::procShipJumpRide);

/* 02432334 */
BOOL daPy_lk_c::procShipSteer() {
    WWHD_FUNC(0x02432334, BOOL, this);
    if (changeShipEndProc()) {
        return TRUE;
    }
    gabi::call(LK_setShipRidePos, this, 1);
    return TRUE;
}
VERIFY(0x02432334, &daPy_lk_c::procShipSteer);

/* 02432378 */
BOOL daPy_lk_c::procShipPaddle() {
    WWHD_FUNC(0x02432378, BOOL, this);
    if (changeShipEndProc()) {
        return TRUE;
    }
    gabi::call(LK_setShipRidePos, this, 1);
    if (!CPad_holdBit2_l(0)) { /* HD */
        dComIfGp_setRStatus_l(0x2C /* dActStts_CRUISE_e */);
    }
    if (dComIfGp_getDoStatus_l() == 0) {
        dComIfGp_setDoStatus_l(0x14); /* HD */
    }
    if (dComIfGp_getBStatus_l() == 0 && !(gabi::load<f32>(dComIfGp_getShipActor_l() + 0x370) < 3.0f)) {
        dComIfGp_setBStatus_l(0x13 /* dActStts_STOP_e */);
    }
    if (dComIfGp_checkPlayerStatus0_l(0x2000 /* daPyStts0_SUBJECT_e */)) {
        dComIfGp_setBStatus_l(7 /* dActStts_RETURN_e */);
        if (gabi::call<BOOL>(LK_checkSubjectEnd, this, 1)) {
            u32 a = dComIfGp_ea() + 0x5CD8;
            gabi::store<u32>(a, gabi::load<u32>(a) & ~0x2000u);
        }
    } else {
        setResetFlg0(resetFlg0() | 0x4000000 /* daPyRFlg0_SUBJECT_ACCEPT */);
        s32 idx = mCameraInfoIdx;
        if ((dComIfGp_getCameraAttentionStatus_l(idx) & 0x1000) && !dComIfGp_event_runCheck_l()) {
            setSubjectMode();
        }
    }
    u32 ship = dComIfGp_getShipActor_l();
    s16 timer = mProcVar0;
    if (timer == -1 || timer == -2) {
        if (timer == -2) {
            if (mFrameCtrlUnder[0].checkPass(168.0f)) {
                LK_voiceStart(48);
            } else if (mFrameCtrlUnder[0].checkPass(105.0f)) {
                LK_voiceStart(47);
            }
        }
        if (mStickDistance > 0.05f || dComIfGp_event_runCheck_l() || dComIfGp_getDoStatus_l() != 0x1D /* GET_OUT_SHIP */ ||
            gabi::load<u32>(ship + 0x704) != 0 || gabi::load<u32>(ship + 0x70C) != 0 || mFrameCtrlUnder[0].getRate() < 0.01f) {
            gabi::call(LK_initShipBaseAnime, this);
            mProcVar0 = (s16)gabi::ftoi(cM_rndF(150.0f) + 300.0f);
        }
    } else if (!dComIfGp_event_runCheck_l() && dComIfGp_getDoStatus_l() == 0x1D && !(mStickDistance > 0.05f) &&
               gabi::load<u32>(ship + 0x704) == 0 && gabi::load<u32>(ship + 0x70C) == 0 &&
               !gabi::call<BOOL>(LK_checkShipNotNormalMode, this)) {
        mProcVar0 = (s16)(mProcVar0 - 1);
        if (mProcVar0 == 0) {
            f32 r = cM_rnd();
            if (r < 0.3333f) {
                mProcVar0 = -1;
                gabi::call(LK_setSingleMoveAnime, this, 0xE1 /* ANM_FREEA */, 1.0f, 0.0f, -1, 5.0f);
            } else if (r < 0.6666f) {
                mProcVar0 = -2;
                gabi::call(LK_setSingleMoveAnime, this, 0xE2 /* ANM_FREEB */, 1.0f, 0.0f, -1, 5.0f);
            } else {
                mProcVar0 = -1;
                gabi::call(LK_setSingleMoveAnime, this, 0xE3 /* ANM_FREED */, 1.0f, 0.0f, -1, 5.0f);
            }
        }
    } else {
        mProcVar0 = (s16)gabi::ftoi(cM_rndF(150.0f) + 300.0f);
    }
    return TRUE;
}
VERIFY(0x02432378, &daPy_lk_c::procShipPaddle);

/* HD inline: an anm frame functor ({f32 result, f32 a, f32 b, ?, fn, arg}) re-evaluated for a frame */
static inline void lk_functor_eval(u32 fn, f32 frame) {
    u32 target = gabi::load<u32>(fn + 0x10);
    u32 arg = gabi::load<u32>(fn + 0x14);
    f32 p2 = gabi::load<f32>(fn + 4);
    f32 p3 = gabi::load<f32>(fn + 8);
    gabi::store<f32>(fn, gabi::call_ptr<f32>(target, arg, frame, p2, p3));
}

/* 024327A4 */
BOOL daPy_lk_c::procShipScope() {
    WWHD_FUNC(0x024327A4, BOOL, this);
    s32 snap = 1;
    if (changeShipEndProc()) {
        return TRUE;
    }
    if (gabi::call<BOOL>(LK_checkScopeEnd, this)) {
        u32 a = dComIfGp_ea() + 0x5CDC;
        gabi::store<u32>(a, gabi::load<u32>(a) & ~0x100000u); /* HD */
        gabi::call(LK_procShipPaddle_init, this);
        mNoResetFlg0 = mNoResetFlg0 & ~0x80000u; /* daPyFlg0_SCOPE_CANCEL */
        gabi::call(LK_setShipRidePos, this, snap);
        return TRUE;
    }
    s32 idx = mCameraInfoIdx;
    if ((dComIfGp_getCameraAttentionStatus_l(idx) & 0x10) && !dComIfGp_event_runCheck_l() &&
        gabi::load<u16>(gabi::ea(this) + 0x420) == 0 /* !checkPlayerDemoMode() */) {
        u32 st1 = gabi::load<u32>(dComIfGp_ea() + 0x5CDC);
        s32 camIdx = mCameraInfoIdx;
        if (st1 & 0x100000) {
            /* HD: aiming the picto box at Link himself (selfie): the camera angle turned around and the
             * face chosen with the pad (*(0x101F5088) + 0x124) from the table at 0x10035064 */
            (void)dComIfGp_ea();
            s32 ci = mCameraInfoIdx;
            u32 cam = gabi::load<u32>(dComIfGp_ea() + ci * 0x34 + 0x5AF8);
            gabi::Local<be<s16>> ang;
            u32 p = gabi::call<u32>(0x0200658C /* cSAngle::cSAngle(s16) */, ang.get(), (s16)(gabi::load<s16>(cam + 0x236) + 0x1500));
            s16 inv = gabi::call<s16>(0x02006804 /* cSAngle::Inv */, p);
            shape_angle.y = inv;
            current.angle.y = inv;
            u32 pad = gabi::load<u32>(gabi::load<u32>(0x101F5088) + 0x124);
            s32 sel = (pad & 4) ? 5 : 0;
            for (s32 i = 1; i <= 4; i++) {
                if (gabi::load<u32>(0x100360D4 + i * 4) & pad) {
                    sel += i;
                    break;
                }
            }
            if (pad & 0x80) {
                sel = 10;
            }
            u32 entry = 0x10035064 + sel * 0xC;
            u32 face = gabi::load<u32>(entry);
            mFace = face;
            f32 frame = gabi::load<f32>(entry + 4);
            gabi::call(LK_setTextureAnime, this, face & 0xFFFF, gabi::ftoi(frame));
            gabi::store<f32>(gabi::load<u32>(gabi::ea(this) + 0x5F4), (f32)(u16)m3530);
            gabi::store<f32>(gabi::load<u32>(gabi::ea(this) + 0x5FC), (f32)(u16)m3532);
            lk_functor_eval(gabi::load<u32>(gabi::ea(this) + 0x644), (f32)(u16)m3530);
            lk_functor_eval(gabi::load<u32>(gabi::ea(this) + 0x69C), (f32)(u16)m3530);
            lk_functor_eval(gabi::load<u32>(gabi::ea(this) + 0x6F4), (f32)(u16)m3532);
            if (gabi::load<u8>(entry + 8) != 0) {
                mModeFlg = mModeFlg | 0x08000180;
            } else {
                u32 f = mModeFlg & ~0x100u;
                m3530 = (u16)gabi::ftoi(frame);
                mModeFlg = f | 0x08000080;
            }
            if (face == 0x92) {
                setNoResetFlg1(noResetFlg1() | 0x100);
            } else {
                setNoResetFlg1(noResetFlg1() & ~0x100u);
            }
            snap = 0;
        } else {
            u32 cam = gabi::load<u32>(dComIfGp_ea() + camIdx * 0x34 + 0x5AF8);
            mModeFlg = mModeFlg & 0xF7FFFE7F;
            s16 a = gabi::load<s16>(cam + 0x236); /* fopCamM_GetAngleY */
            shape_angle.y = a;
            snap = 0;
            current.angle.y = a;
        }
    }
    mNoResetFlg0 = mNoResetFlg0 & ~0x80000u;
    gabi::call(LK_setShipRidePos, this, snap);
    return TRUE;
}
VERIFY(0x024327A4, &daPy_lk_c::procShipScope);

enum : u32 {
    LK_checkNextActionBoomerangReady = 0x023EB510,
    LK_setBodyAngleToCamera = 0x02416E90,
    LK_checkSightLine = 0x023EBCB0,
    LK_itemTrigger = 0x023EAB40,
    LK_getReadyItem = 0x023EAA80,
    LK_checkNextActionHookshotReady = 0x023EAD5C,
    LK_checkNextActionBowReady = 0x023EB208,
    LK_HD_setShipBStatus = 0x023D6188, /* unnamed by the matcher: HD, sets the B status (STOP 0x13 / PUT_AWAY 8) from the ship's state */
};
/* mSightPacket (0x58E8): draw flag +4, lock flag +5, frame +6, position +8 */
#define LK_sightPacket (gabi::ea(this) + 0x58E8)
static inline void lk_sight_setPos(u32 sp, u32 pos) { lk_xyz_copy(gabi::at<cXyz>(sp + 8), gabi::at<cXyz>(pos)); }

/* 02432B28 */
BOOL daPy_lk_c::procShipBoomerang() {
    WWHD_FUNC(0x02432B28, BOOL, this);
    if (changeShipEndProc()) {
        u32 a = dComIfGp_ea() + 0x5CD8;
        gabi::store<u32>(a, gabi::load<u32>(a) & ~0x400000u); /* daPyStts0_BOOMERANG_WAIT_e */
        return TRUE;
    }
    gabi::call(LK_setShipRidePos, this, 0);
    if (mActorKeepThrow.mActor == nullptr && gabi::load<u16>(gabi::ea(this) + 0x5888) != 0x33 /* BOOMCATCH */) {
        dComIfGp_setDoStatus_l(7 /* dActStts_RETURN_e */);
        if (mItemTrigger & 3) { /* HD: do or B trigger */
            gabi::call(LK_procShipPaddle_init, this);
            gabi::call(LK_deleteEquipItem, this, 1);
            return TRUE;
        }
    }
    gabi::store<u8>(LK_sightPacket + 4, 0); /* mSightPacket.offDrawFlg() */
    u32 ship = dComIfGp_getShipActor_l();
    u16 upper = gabi::load<u16>(gabi::ea(this) + 0x5888);
    if (upper != 0x33) {
        if (mEquipItem == 0x2D /* dItemNo_BOOMERANG_e */) {
            if (upper == 0x35 /* BOOMWAIT */) {
                setShipAttentionAnmSpeed(0.8f); /* HD: HIO folded */
                if (gabi::load<u16>(gabi::ea(this) + 0x5888) != 0x34) { /* HD */
                    gabi::call(LK_checkNextActionBoomerangReady, this);
                }
                if (LK_checkAttentionLock()) {
                    setShipAttnetionBodyAngle();
                } else {
                    mProcVar5 = 0;
                    if (gabi::call<BOOL>(LK_setBodyAngleToCamera, this)) {
                        u32 boomerang = gabi::ea(mActorKeepEquip.mActor.get());
                        if (boomerang != 0) {
                            gabi::Local<cXyz> sight; /* sp+0x08 */
                            f64 flyMax = gabi::call<f64>(0x020CECF0 /* daBoomerang_c::getFlyMax */, boomerang);
                            gabi::call(LK_checkSightLine, this, flyMax, sight.get());
                            lk_sight_setPos(LK_sightPacket, gabi::ea(sight.get()));
                            gabi::store<u8>(LK_sightPacket + 4, 1); /* onDrawFlg() */
                        }
                    }
                }
                if (mpAttnActorLockOn == nullptr) {
                    shape_angle.y = (s16)(shape_angle.y + (s16)(gabi::load<s16>(ship + 0x32A) - mProcVar2));
                }
            }
        } else {
            if (mActorKeepThrow.mActor == nullptr) {
                return gabi::call<BOOL>(LK_procShipPaddle_init, this);
            }
            gabi::call(LK_setShapeAngleToAtnActor, this);
            mFrameCtrlUnder[0].setRate(1.25f);
            if (gabi::call<BOOL>(LK_itemTrigger, this) && gabi::call<s32>(LK_getReadyItem, this) == 0x2D) { /* HD */
                gabi::store<u8>(gabi::ea(mActorKeepThrow.mActor.get()) + 0x26274, 1); /* boomerang->onCancelFlg() */
            }
        }
    }
    mProcVar2 = gabi::load<s16>(ship + 0x32A);
    return TRUE;
}
VERIFY(0x02432B28, &daPy_lk_c::procShipBoomerang);

/* 02432D80 */
void daPy_lk_c::setHookshotSight() {
    WWHD_FUNC(0x02432D80, void, this);
    gabi::Local<cXyz> dir;    /* sp+0x08 */
    gabi::Local<cXyz> t1;     /* sp+0x14 */
    gabi::Local<cXyz> t2;     /* sp+0x20 */
    gabi::Local<cXyz> endP;   /* sp+0x2C (GameCube sp38) */
    gabi::Local<cXyz> startP; /* sp+0x38 (GameCube sp44) */
    s16 bx = gabi::load<s16>(gabi::ea(this) + 0x3D0); /* mBodyAngle.x */
    f32 cb = cM_scos(bx);
    dir->x = cM_ssin(shape_angle.y) * cb;
    dir->y = -cM_ssin(bx);
    dir->z = cM_scos(shape_angle.y) * cb;
    cXyz_ml(dir, t2, 1500.0f);
    cXyz_pl(&mHookshotRootPos, t1, t2);
    lk_xyz_copy(endP, t1);
    cXyz_ml(dir, t1, 60.0f);
    cXyz_mi(&mHookshotRootPos, t2, t1);
    lk_xyz_copy(startP, t2);
    dBgS_LinChk_Set(mRopeLinChk, startP, endP, this);
    u32 hookshot = gabi::ea(mActorKeepEquip.mActor.get());
    BOOL cross = cBgS_LineCross(dComIfG_Bgsp(), mRopeLinChk);
    u32 sp = LK_sightPacket;
    gabi::store<u8>(sp + 4, 1); /* onDrawFlg() */
    u32 hs = hookshot + 0xD50C; /* daHookshot_c sight data (HD offset) */
    u32 ropeCross = gabi::ea(this) + 0xA6C; /* mRopeLinChk.GetCross() */
    BOOL objSight;
    if (!cross) {
        if (!gabi::call<BOOL>(0x025160DC /* dCcD_GObjInf::ChkAtHit */, hs + 0x124)) { /* !hookshot->getSightHit() */
            lk_sight_setPos(sp, gabi::ea(endP.get()));
            gabi::store<u8>(sp + 5, 0); /* offLockFlg() */
            return;
        }
        objSight = TRUE;
    } else {
        f32 d1 = gabi::call<f32>(0x028E8DE8 /* PSVECSquareDistance */, &mHookshotRootPos, &current.pos);
        f32 d2 = gabi::call<f32>(0x028E8DE8, ropeCross, &current.pos);
        if (d1 > d2) {
            lk_sight_setPos(sp, gabi::ea(&mHookshotRootPos));
            objSight = FALSE;
        } else if (!gabi::call<BOOL>(0x025160DC, hs + 0x124)) {
            objSight = FALSE;
            lk_sight_setPos(sp, ropeCross);
        } else {
            f32 d3 = gabi::call<f32>(0x028E8DE8, ropeCross, &mHookshotRootPos);
            f32 d4 = gabi::call<f32>(0x028E8DE8, hs + 0x1C, &mHookshotRootPos);
            if (d3 > d4) {
                objSight = TRUE;
            } else {
                objSight = FALSE;
                lk_sight_setPos(sp, ropeCross);
            }
        }
    }
    BOOL hook;
    if (objSight) {
        lk_sight_setPos(sp, hs + 0x1C); /* hookshot->getObjSightCrossPos() */
        hook = gabi::load<u32>(hs) != 0; /* getObjHookFlg() */
    } else {
        hook = gabi::call<u32>(0x024EF398 /* dBgS::GetPolyId2 (ChkPolyHSStick) */, dComIfG_Bgsp(), gabi::ea(this) + 0xA50) != 0;
    }
    if (hook) {
        if (gabi::load<u8>(sp + 5) != 0) { /* getLockFlg() */
            u8 f = (u8)(gabi::load<u8>(sp + 6) + 1); /* incFrame() */
            if (f == 0x1A) {
                gabi::store<u8>(sp + 6, 0);
                gabi::call(0x025E1988 /* seStartSystem */, 0x820 /* JA_SE_INDICATOR_1 */);
            } else {
                gabi::store<u8>(sp + 6, f);
                if (f == 0) {
                    gabi::call(0x025E1988, 0x820);
                }
            }
        } else {
            gabi::store<u8>(sp + 6, 0);
            gabi::store<u8>(sp + 5, 1); /* onLockFlg() */
            gabi::call(0x025E1988, 0x820);
        }
    } else {
        gabi::store<u8>(sp + 5, 0); /* offLockFlg() */
    }
}
VERIFY(0x02432D80, &daPy_lk_c::setHookshotSight);

/* 02433104 */
BOOL daPy_lk_c::procShipHookshot() {
    WWHD_FUNC(0x02433104, BOOL, this);
    if (changeShipEndProc()) {
        return TRUE;
    }
    gabi::call(LK_setShipRidePos, this, 0);
    dComIfGp_setDoStatus_l(7 /* dActStts_RETURN_e */);
    u32 hookshot = gabi::ea(mActorKeepEquip.mActor.get());
    if ((mItemTrigger & 3) /* HD: do or B */ || hookshot == 0 || (mProcVar6 != 0 && m355E == 0 && !LK_checkAttentionLock())) {
        gabi::call(LK_procShipPaddle_init, this);
        gabi::call(LK_deleteEquipItem, this, 1);
        return TRUE;
    }
    gabi::store<u8>(LK_sightPacket + 4, 0); /* mSightPacket.offDrawFlg() */
    gabi::call(LK_checkNextActionHookshotReady, this);
    u32 ship = dComIfGp_getShipActor_l();
    if (gabi::load<u32>(hookshot + 0xB0) == 0 /* hookshot->checkWait() */) {
        setShipAttentionAnmSpeed(1.0f);
    }
    if (LK_checkAttentionLock()) {
        if (gabi::load<u32>(hookshot + 0xB0) == 0) {
            setShipAttnetionBodyAngle();
        } else {
            mProcVar5 = 0;
        }
        mProcVar6 = 1;
    } else {
        mProcVar5 = 0;
        gabi::call(LK_setBodyAngleToCamera, this);
        mProcVar6 = 0;
    }
    if (mpAttnActorLockOn == nullptr) {
        shape_angle.y = (s16)(shape_angle.y + (s16)(gabi::load<s16>(ship + 0x32A) - mProcVar2));
    }
    u32 st;
    if (gabi::load<u32>(hookshot + 0xB0) == 0) {
        if (!LK_checkAttentionLock()) {
            setHookshotSight();
        } else {
            gabi::store<u8>(LK_sightPacket + 4, 0);
        }
        st = dComIfGp_ea() + 0x5CD8;
        gabi::store<u32>(st, gabi::load<u32>(st) & ~0x40000u);
    } else {
        st = dComIfGp_ea() + 0x5CD8;
        gabi::store<u32>(st, gabi::load<u32>(st) | 0x40000);
    }
    mProcVar2 = gabi::load<s16>(ship + 0x32A);
    return TRUE;
}
VERIFY(0x02433104, &daPy_lk_c::procShipHookshot);

/* 0243333C */
BOOL daPy_lk_c::procShipBow() {
    WWHD_FUNC(0x0243333C, BOOL, this);
    if (mDemoMode != 0x44 /* !checkBowMiniGame() */ && changeShipEndProc()) {
        return TRUE;
    }
    gabi::call(LK_setShipRidePos, this, 0);
    if (mDemoMode != 0x44) {
        dComIfGp_setDoStatus_l(7 /* dActStts_RETURN_e */);
        if ((mItemTrigger & 3) /* HD: do or B */ ||
            (!LK_checkAttentionLock() && mActorKeepEquip.mActor == nullptr && mProcVar6 != 0)) {
            gabi::call(LK_procShipPaddle_init, this);
            gabi::call(LK_deleteEquipItem, this, 1);
            return TRUE;
        }
    } else {
        LK_cutEnd();
    }
    u32 ship = dComIfGp_getShipActor_l();
    if (gabi::load<u16>(gabi::ea(this) + 0x5888) == 0x36 /* checkBowWaitAnime() */) {
        setShipAttentionAnmSpeed(1.0f);
    }
    if (LK_checkAttentionLock() && mDemoMode != 0x44) {
        setShipAttnetionBodyAngle();
        mProcVar6 = 1;
    } else {
        mProcVar5 = 0;
        gabi::call(LK_setBodyAngleToCamera, this);
        mProcVar6 = 0;
    }
    if (mpAttnActorLockOn == nullptr) {
        shape_angle.y = (s16)(shape_angle.y + (s16)(gabi::load<s16>(ship + 0x32A) - mProcVar2));
    }
    mProcVar2 = gabi::load<s16>(ship + 0x32A);
    gabi::call(LK_checkNextActionBowReady, this);
    m35EC = mFrameCtrlUpper[2].getFrame();
    return TRUE;
}
VERIFY(0x0243333C, &daPy_lk_c::procShipBow);

/* 024334EC */
BOOL daPy_lk_c::procShipCannon() {
    WWHD_FUNC(0x024334EC, BOOL, this);
    if (!CPad_holdBit2_l(0)) { /* HD */
        dComIfGp_setRStatus_l(0x2C /* dActStts_CRUISE_e */);
    }
    if (dComIfGp_getDoStatus_l() == 0) {
        dComIfGp_setDoStatus_l(0x3F); /* HD */
    }
    gabi::call(LK_HD_setShipBStatus);
    if (!changeShipEndProc()) {
        gabi::call(LK_setShipRidePos, this, 1);
    }
    return TRUE;
}
VERIFY(0x024334EC, &daPy_lk_c::procShipCannon);

enum : u32 {
    LK_initShipCraneAnime = 0x023F1B90,
    LK_checkFanGlideProc = 0x0241A578,
};
/* daSalvage_c::getSalvageKind() == 1 (HD inline: the salvage id (0x10475638) and the registry's type (025B4CB8)) */
static inline BOOL lk_salvageKindIs1() {
    s32 id = gabi::load<s32>(0x10475638);
    if (id == -1) {
        return FALSE;
    }
    return gabi::call<u8>(0x025B4CB8, gabi::load<u32>(0x10475634), id) == 1;
}

/* 02433570 */
BOOL daPy_lk_c::procShipCrane() {
    WWHD_FUNC(0x02433570, BOOL, this);
    u32 ship = dComIfGp_getShipActor_l();
    if (ship == 0) {
        return LK_checkNextMode(0);
    }
    if (!CPad_holdBit2_l(0)) { /* HD */
        dComIfGp_setRStatus_l(0x2C /* dActStts_CRUISE_e */);
    }
    if (gabi::load<u16>(gabi::ea(this) + 0x5888) != 0xFFFF /* !checkNoUpperAnime() */) {
        if (dComIfGp_getDoStatus_l() == 0) {
            dComIfGp_setDoStatus_l(0x40); /* HD */
        }
        gabi::call(LK_HD_setShipBStatus);
        if (changeShipEndProc()) {
            return TRUE;
        }
        if (gabi::load<s16>(ship + 0x686) != 0 /* ship->getRopeCnt() */) {
            gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
            gabi::call(LK_initShipCraneAnime, this);
        }
    } else if (gabi::load<u8>(ship + 0x63A) == 2 /* ship->checkSalvageDemo() */) {
        if (mProcVar2 == 0 && (gabi::load<u32>(ship + 0x644) & 0x40000) /* ship->checkCraneUpEnd() */) {
            mProcVar2 = 1;
            s32 anm;
            if (gabi::load<s16>(ship + 0x682) > 0 /* ship->getCraneBaseAngle() */) {
                anm = lk_salvageKindIs1() ? 0xD1 /* ANM_SALVRGOOD */ : 0xCF /* ANM_SALVRBAD */;
            } else {
                anm = lk_salvageKindIs1() ? 0xD2 /* ANM_SALVLGOOD */ : 0xD0 /* ANM_SALVLBAD */;
            }
            mModeFlg = (mModeFlg & ~0x100u) | 0x400;
            gabi::call(LK_setSingleMoveAnime, this, anm, 1.0f, 0.0f, -1, 5.0f);
        } else if (mProcVar6 == 0) {
            if (gabi::load<s16>(ship + 0x682) > 0) {
                gabi::call(LK_setSingleMoveAnime, this, 0xCD /* ANM_SALVRWAIT */, 1.0f, 0.0f, -1, 5.0f);
                mProcVar6 = 3;
            } else {
                gabi::call(LK_setSingleMoveAnime, this, 0xCE /* ANM_SALVLWAIT */, 1.0f, 0.0f, -1, 5.0f);
                mProcVar6 = 2;
            }
        }
    } else {
        if (dComIfGp_getDoStatus_l() == 0) {
            dComIfGp_setDoStatus_l(0x40); /* HD */
        }
        gabi::call(LK_HD_setShipBStatus);
        if (changeShipEndProc()) {
            return TRUE;
        }
        if (mProcVar6 == 3 && gabi::load<s16>(ship + 0x682) < 0) {
            mProcVar6 = 0;
            gabi::call(LK_setSingleMoveAnime, this, 0xCC /* ANM_SALVLR */, 1.0f, 0.0f, -1, 5.0f);
            mModeFlg = (mModeFlg & ~0x100u) | 0x400;
            mProcVar7 = 2;
        } else if (mProcVar6 == 2 && gabi::load<s16>(ship + 0x682) > 0) {
            mProcVar6 = 0;
            gabi::call(LK_setSingleMoveAnime, this, 0xCC /* ANM_SALVLR */, -1.0f, 0.0f, -1, 5.0f);
            mModeFlg = (mModeFlg & ~0x100u) | 0x400;
            mProcVar7 = 3;
        } else if (mProcVar6 == 0) {
            f32 rate = mFrameCtrlUnder[0].getRate();
            s32 side = mProcVar7;
            if (std::fabs(rate) < 0.01f) {
                if (side == 3) {
                    gabi::call(LK_setSingleMoveAnime, this, 0xCD /* ANM_SALVRWAIT */, 1.0f, 0.0f, -1, 5.0f);
                    mProcVar6 = 3;
                } else {
                    gabi::call(LK_setSingleMoveAnime, this, 0xCE /* ANM_SALVLWAIT */, 1.0f, 0.0f, -1, 5.0f);
                    mProcVar6 = 2;
                }
                mModeFlg = (mModeFlg | 0x100) & ~0x400u;
            } else if ((side == 3 && gabi::load<s16>(ship + 0x682) < 0) || (side == 2 && gabi::load<s16>(ship + 0x682) > 0)) {
                mFrameCtrlUnder[0].setRate(-rate);
                if (mProcVar7 == 3) {
                    mProcVar7 = 2;
                } else {
                    mProcVar7 = 3;
                }
            }
        } else {
            s16 cnt = gabi::load<s16>(ship + 0x686);
            if (cnt == 0xFA /* ship->checkRopeCntMax() */) {
                gabi::call(LK_setTextureAnime, this, 9, 0);
            } else if (cnt > 0x14 /* ship->checkRopeDownStart() */) {
                gabi::call(LK_setTextureAnime, this, 6, 0);
            } else {
                gabi::call(LK_setTextureAnime, this, 0, 0);
            }
        }
    }
    u32 flg = mModeFlg;
    if ((flg & 0x100) || mProcVar6 == 0) {
        mModeFlg = flg | 0x08000080;
    } else {
        mModeFlg = flg & ~0x08000080u;
    }
    gabi::call(LK_setShipRidePos, this, 1);
    return TRUE;
}
VERIFY(0x02433570, &daPy_lk_c::procShipCrane);

/* 02433BB8 */
BOOL daPy_lk_c::procShipGetOff() {
    WWHD_FUNC(0x02433BB8, BOOL, this);
    u32 ship = dComIfGp_getShipActor_l();
    if (ship == 0) {
        return LK_checkNextMode(0);
    }
    if (mProcVar6 == 0) {
        current.pos.y = current.pos.y + (gabi::load<f32>(ship + 0x318) - m35A0);
        cLib_chaseF(&current.pos.y, gabi::load<f32>(ship + 0x318) + 35.0f /* l_ship_ledge.y */, 4.0f);
        m35A0 = gabi::load<f32>(ship + 0x318);
        s16 target = cLib_targetAngleY(&current.pos, &m370C);
        if (cLib_distanceAngleS(target, shape_angle.y) >= 0x4000) {
            gabi::call(LK_setSingleMoveAnime, this, 0x3C /* ANM_JMPST */, 0.8f, 1.0f, 5, 1.0f); /* HD: HIO folded */
            speed.y = 8.0f;
            gravity = -2.5f;
            mProcVar6 = 1;
            mNormalSpeed = 8.0f;
            m34C2 = 1;
            gabi::store<u8>(ship + 0x636, 6); /* ship->setGetOffSecond() */
            mModeFlg = mModeFlg | 2; /* ModeFlg_MIDAIR */
        }
    } else {
        s32 staff = mStaffIdx;
        m34C2 = 1;
        dComIfGp_evmng_cutEnd(staff);
        if (LK_mAcchChkGroundHit()) {
            gabi::call(LK_changeLandProc, this, 1.3f);
        } else if (gabi::call<BOOL>(LK_checkFanGlideProc, this, 0)) {
            return TRUE;
        } else if (speed.y < -gravity) {
            gabi::call(LK_procFall_init, this, 0, 7.0f);
            gabi::call(LK_setTextureAnime, this, 0x37, 0);
        }
    }
    return TRUE;
}
VERIFY(0x02433BB8, &daPy_lk_c::procShipGetOff);

/* 02433D88 */
BOOL daPy_lk_c::procShipRestart() {
    WWHD_FUNC(0x02433D88, BOOL, this);
    if (dComIfGp_getShipActor_l() == 0) {
        return LK_checkNextMode(0);
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        gabi::call(LK_procShipPaddle_init, this);
    }
    gabi::call(LK_setShipRidePos, this, 1);
    return TRUE;
}
VERIFY(0x02433D88, &daPy_lk_c::procShipRestart);

/* ---- rope ---- */
enum : u32 {
    LK_freeRopeItem = 0x023DCA80,
    LK_checkBossGomaStage = 0x023EB6C8,
};
#define mRopePos (*gabi::at<cXyz>(gabi::ea(this) + 0x408)) /* daPy_py_c (GameCube 0x2EC) */
#define mRoofChkPos (*gabi::at<cXyz>(gabi::ea(this) + 0xBA0)) /* mRoofChk's position (+0x38) */

/* 02433E0C */
f32 daPy_lk_c::checkRopeRoofHit(s16 angle) {
    WWHD_FUNC(0x02433E0C, f32, this, angle);
    f32 x = gabi::fnmsubs(15.0f, cM_ssin(angle), current.pos.x);
    f32 z = gabi::fnmsubs(15.0f, cM_scos(angle), current.pos.z);
    mRoofChkPos.y = current.pos.y;
    mRoofChkPos.x = x;
    mRoofChkPos.z = z;
    return (f32)gabi::call<f64>(0x024EF6E8 /* dBgS::RoofChk */, dComIfG_Bgsp(), mRoofChk);
}
VERIFY(0x02433E0C, &daPy_lk_c::checkRopeRoofHit);

/* 02433E8C */
int daPy_lk_c::changeRopeEndProc(int fall) {
    WWHD_FUNC(0x02433E8C, int, this, fall);
    /* HD: no letting go on the stage "ADMumi" (sead::SafeString comparison with the start stage name) */
    {
        gabi::Local<SafeString> name;  /* sp+0x08 */
        gabi::Local<SafeString> stage; /* sp+0x10 */
        name->mStringTop = 0x100360E8; /* "ADMumi" */
        name->__vtbl = 0x10034B24;
        u32 play = dComIfGp_ea();
        stage->__vtbl = 0x10034B24;
        stage->mStringTop = play + 0x5134; /* dComIfGp_getStartStageName() */
        gabi::call_ptr(gabi::load<u32>(name->__vtbl + 0x14), name.get());
        gabi::call_ptr(gabi::load<u32>(name->__vtbl + 0x14), name.get());
        u32 a = name->mStringTop;
        gabi::call_ptr(gabi::load<u32>(stage->__vtbl + 0x14), stage.get());
        u32 b = stage->mStringTop;
        if (a == b) {
            return FALSE;
        }
        u32 p = name->mStringTop;
        u32 q = stage->mStringTop;
        for (u32 n = 0x40001; n != 0; n--) {
            u8 c = gabi::load<u8>(p);
            if (c != gabi::load<u8>(q)) {
                break;
            }
            if (c == 0) {
                return FALSE;
            }
            p++;
            q++;
        }
    }
    dComIfGp_setDoStatus_l(0x16 /* dActStts_LET_GO_ROPE_e */);
    if ((mItemTrigger & 3) /* doTrigger() || cancelTrigger() */ ||
        (!gabi::call_ptr<BOOL>(gabi::load<u32>(__vtbl + 0xD4), this) /* checkRopeTag() (HD: virtual) */ &&
         gabi::call<BOOL>(LK_itemTrigger, this))) {
        gabi::call(LK_freeRopeItem, this);
        LK_voiceStart(6);
        if ((mItemTrigger & 2) && !gabi::call<BOOL>(LK_checkBossGomaStage, this)) {
            gabi::call(LK_setAnimeEquipSword, this, 0);
        }
        if (fall != 0) {
            gabi::call(LK_procFall_init, this, 1, 6.0f); /* HD: HIO folded */
        }
        mNoResetFlg0 = mNoResetFlg0 | 0x400000;
        setNoResetFlg1(noResetFlg1() | 0x8000000);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02433E8C, &daPy_lk_c::changeRopeEndProc);

/* 02434078 */
BOOL daPy_lk_c::procRopeUpHang_init() {
    WWHD_FUNC(0x02434078, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x7F /* daPyProc_ROPE_UP_HANG_e */);
    dComIfGp_onPlayerStatus0_l(0x800000 /* daPyStts0_UNK800000_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x4C /* ANM_VJMP */, 1.0f, 10.0f, -1, 3.0f);
    s16 a = (s16)(m352C + 0x8000);
    mNormalSpeed = 0.0f;
    shape_angle.y = a;
    gravity = 0.0f;
    current.pos.y = current.pos.y - 80.0f;
    current.angle.y = a;
    speed.y = 0.0f;
    return TRUE;
}
VERIFY(0x02434078, &daPy_lk_c::procRopeUpHang_init);

/* 02434120 */
int daPy_lk_c::changeRopeToHangProc() {
    WWHD_FUNC(0x02434120, int, this);
    if (mProcVar6 == 0) {
        return FALSE;
    }
    f32 s = cM_ssin(shape_angle.y);
    f32 c = cM_scos(shape_angle.y);
    gabi::Local<cXyz> start; /* sp+0x08 */
    gabi::Local<cXyz> end;   /* sp+0x14 */
    f32 sx = gabi::fnmsubs(20.0f, s, mRopePos.x);
    f32 sz = gabi::fnmsubs(20.0f, c, mRopePos.z);
    f32 sy = mRopePos.y;
    start->x = sx;
    end->y = sy;
    end->x = gabi::fmadds(50.0f, s, sx);
    start->z = sz;
    end->z = gabi::fmadds(50.0f, c, sz);
    start->y = sy;
    dBgS_LinChk_Set(mLinkLinChk, start, end, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        return FALSE;
    }
    u32 pla = dBgS_GetTriPla_l(mLinkLinChkPoly);
    if (pla == 0) { /* HD: NULL check */
        return FALSE;
    }
    s16 a = cM_atan2s(gabi::load<f32>(pla + 0), gabi::load<f32>(pla + 8));
    if (cLib_distanceAngleS(a, (s16)(shape_angle.y + 0x8000)) > 0x2000) {
        return FALSE;
    }
    m352C = a;
    procRopeUpHang_init();
    return TRUE;
}
VERIFY(0x02434120, &daPy_lk_c::changeRopeToHangProc);

/* 02434260 */
BOOL daPy_lk_c::checkRopeSwingWall(cXyz* start, cXyz* end, s16* swing, f32* angle) {
    WWHD_FUNC(0x02434260, BOOL, this, start, end, swing, angle);
    /* function-local statics: dynamic_scale (0.25) and particle_scale (0.75), guard words first */
    if (gabi::load<u32>(0x1046D20C) == 0) {
        gabi::store<u32>(0x1046D20C, 1);
        gabi::store<f32>(0x1046D218, 0.25f);
        gabi::store<f32>(0x1046D21C, 0.25f);
        gabi::store<f32>(0x1046D214, 0.25f);
    }
    if (gabi::load<u32>(0x1046D210) == 0) {
        gabi::store<u32>(0x1046D210, 1);
        gabi::store<f32>(0x1046D224, 0.75f);
        gabi::store<f32>(0x1046D228, 0.75f);
        gabi::store<f32>(0x1046D220, 0.75f);
    }
    dBgS_LinChk_Set(mLinkLinChk, start, end, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        return FALSE;
    }
    f32 s = (f32)(-(s32)*gabi::at<be<s16>>(gabi::ea(swing))) / (f32)mProcVar2;
    if (s > 1.0f) {
        s = 1.0f;
    } else if (s < -1.0f) {
        s = -1.0f;
    }
    be<f32>* ang = gabi::at<be<f32>>(gabi::ea(angle));
    f64 a;
    if (*ang > 0.0f) {
        f64 r = gabi::call<f64>(0x028F4384 /* std::sqrtf */, gabi::fnmsubs(s, s, 1.0f));
        a = gabi::call<f64>(0x0201971C /* cM_atan2f */, s, -r);
    } else {
        f64 r = gabi::call<f64>(0x028F4384, gabi::fnmsubs(s, s, 1.0f));
        a = gabi::call<f64>(0x0201971C, s, r);
    }
    *ang = (f32)a;
    m35A8 = (f32)(a / (f64)(f32)m35A4);
    u32 fe = gabi::ea(this) + 0x6648; /* mFootEffect[1] (daPy_footEffect_c, 0x4C) */
    if (gabi::load<s32>(fe + 0x48) != -1) { /* getID() */
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(fe) + 0x44), fe);               /* smoke callback remove() */
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(fe + 0x20) + 0x44), fe + 0x20); /* other callback remove() */
    }
    u32 pla = dBgS_GetTriPla_l(mLinkLinChkPoly);
    if (pla == 0) { /* HD: NULL check */
        return FALSE;
    }
    gabi::Local<cXyz> scaled; /* sp+0x38 */
    gabi::Local<cXyz> pos;    /* sp+0x2C */
    cXyz_ml(gabi::at<cXyz>(pla), scaled, 10.0f);
    cXyz_pl(&mLinkLinChkCross, pos, scaled);
    lk_xyz_copy(gabi::at<cXyz>(fe + 0x34), pos); /* setPos() */
    gabi::Local<cXyz> xz; /* sp+0x20 */
    xz->x = gabi::load<f32>(pla + 0);
    xz->z = gabi::load<f32>(pla + 8);
    xz->y = 0.0f;
    f64 lxz = gabi::call<f64>(0x028F4384, PSVECSquareMag(xz));
    s16 ax = gabi::call<s16>(0x020195B0 /* cM_atan2s */, lxz, gabi::load<f32>(pla + 4));
    s16 ay = cM_atan2s(gabi::load<f32>(pla + 0), gabi::load<f32>(pla + 8));
    gabi::store<s16>(fe + 0x40, ax); /* setAngle() */
    gabi::store<s16>(fe + 0x42, ay);
    gabi::store<s16>(fe + 0x44, 0);
    s8 room = current.roomNo;
    dPa_control_c* pa = dComIfGp_getParticle();
    u32 em = gabi::ea(dPa_control_set(pa, 3, 0x2022 /* ID_AK_JT_ELEMENTSMOKE00 */, gabi::at<cXyz>(fe + 0x34),
                                      gabi::at<csXyz>(fe + 0x40), nullptr, 0xA0, gabi::at<dPa_levelEcallBack>(fe), room,
                                      nullptr, nullptr, nullptr));
    if (em != 0) {
        gabi::store<f32>(em + 0x58, 1.0f); /* setSpread() */
        gabi::store<f32>(em + 0x34, 8.0f); /* setRate() */
        gabi::store<u32>(em + 0x220, gabi::load<u32>(0x1046D214)); /* setGlobalDynamicsScale() */
        gabi::store<u32>(em + 0x224, gabi::load<u32>(0x1046D218));
        gabi::store<u32>(em + 0x228, gabi::load<u32>(0x1046D21C));
        gabi::store<u32>(em + 0x238, gabi::load<u32>(0x1046D220)); /* setGlobalParticleScale() */
        gabi::store<u32>(em + 0x23C, gabi::load<u32>(0x1046D224));
        gabi::store<u32>(em + 0x240, gabi::load<u32>(0x1046D228));
    }
    return TRUE;
}
VERIFY(0x02434260, &daPy_lk_c::checkRopeSwingWall);

enum : u32 {
    LK_procHangStart_init = 0x02417D30,
    LK_checkSpecialRope = 0x0241CB30,
    LK_checkItemModeActorPointer = 0x02427370,
    LK_setSpeedAndAngleAtnActor = 0x02416B70,
    LK_HD_02416FA8 = 0x02416FA8, /* unnamed by the matcher: HD, called after the rope aim speed boost */
    LK_procRopeSwing_init = 0x0241D040,
};
/* mSightPacket lock update when the target is hookable (getLockFlg/incFrame/checkSEFrame/onLockFlg) */
static inline void lk_sight_lock(u32 sp) {
    if (gabi::load<u8>(sp + 5) != 0) {
        u8 f = (u8)(gabi::load<u8>(sp + 6) + 1);
        if (f == 0x1A) {
            gabi::store<u8>(sp + 6, 0);
            gabi::call(0x025E1988 /* seStartSystem */, 0x820 /* JA_SE_INDICATOR_1 */);
        } else {
            gabi::store<u8>(sp + 6, f);
            if (f == 0) {
                gabi::call(0x025E1988, 0x820);
            }
        }
    } else {
        gabi::store<u8>(sp + 6, 0);
        gabi::store<u8>(sp + 5, 1);
        gabi::call(0x025E1988, 0x820);
    }
}

/* 02434E64 */
BOOL daPy_lk_c::checkHangRopeActorNull() {
    WWHD_FUNC(0x02434E64, BOOL, this);
    if (mActorKeepRope.mActor == nullptr) {
        if (gabi::call_ptr<BOOL>(gabi::load<u32>(__vtbl + 0xD4), this) /* checkRopeTag() (HD: virtual) */) {
            mEquipItem = 0x100; /* daPyItem_NONE_e */
        }
        gabi::call(LK_procFall_init, this, 1, 6.0f); /* HD: HIO folded */
        mNoResetFlg0 = mNoResetFlg0 | 0x400000;
        setNoResetFlg1(noResetFlg1() | 0x8000000);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02434E64, &daPy_lk_c::checkHangRopeActorNull);

/* 02434F04 */
int daPy_lk_c::specialRopeHangUp() {
    WWHD_FUNC(0x02434F04, int, this);
    if (!gabi::call<BOOL>(LK_checkSpecialRope, this)) {
        return FALSE;
    }
    if (shape_angle.y < 0x4000) {
        current.pos.z = -15.8f;
        shape_angle.y = 0;
        m352C = (s16)0x8000;
    } else {
        current.pos.z = 15.8f;
        shape_angle.y = -0x8000;
        m352C = 0;
    }
    current.pos.x = 361.4f;
    current.pos.y = 4464.8f;
    old.pos.x = current.pos.x;
    old.pos.y = current.pos.y;
    old.pos.z = current.pos.z;
    current.angle.y = shape_angle.y;
    lk_xyz_copy(&m3724, &current.pos);
    gabi::call(LK_setSingleMoveAnime, this, 0x4C /* ANM_VJMP */, 1.0f, 10.0f, -1, 3.0f);
    return gabi::call<BOOL>(LK_procHangStart_init, this);
}
VERIFY(0x02434F04, &daPy_lk_c::specialRopeHangUp);

/* 024350A0 */
BOOL daPy_lk_c::procRopeSubject() {
    WWHD_FUNC(0x024350A0, BOOL, this);
    if (!gabi::call<BOOL>(LK_checkItemModeActorPointer, this)) {
        return TRUE;
    }
    u32 rope = gabi::ea(mActorKeepEquip.mActor.get());
    u32 param = gabi::load<u32>(rope + 0xB0); /* fopAcM_GetParam(rope) */
    u32 st = dComIfGp_ea() + 0x5CD8;
    if (param == 0) {
        gabi::store<u32>(st, gabi::load<u32>(st) & ~0x40000u);
    } else {
        gabi::store<u32>(st, gabi::load<u32>(st) | 0x40000);
    }
    if (LK_checkAttentionLock()) {
        gabi::call(LK_setSpeedAndAngleAtn, this);
    } else if (gabi::load<u16>(gabi::ea(this) + 0x5888) != 0xE2) { /* HD */
        gabi::call(LK_setSpeedAndAngleAtnActor, this);
    }
    if (LK_checkNextMode(0)) {
        return TRUE;
    }
    if (gabi::call<BOOL>(LK_setBodyAngleToCamera, this)) {
        if (LK_FIELD(s16, 0x3D0) > 0) { /* mBodyAngle.x */
            LK_FIELD(s16, 0x3D0) = 0;
        }
        gabi::Local<cXyz> sight; /* sp+0x08 */
        gabi::call(LK_checkSightLine, this, 2200.0f, sight.get());
        u32 sp = LK_sightPacket;
        gabi::store<u8>(sp + 4, 1); /* onDrawFlg() */
        lk_sight_setPos(sp, gabi::ea(sight.get()));
        if (gabi::call<BOOL>(0x0216EEB8 /* himo2: search_target (setTargetPos) */, rope, sight.get(), &m3600, &m3604)) {
            gabi::store<u8>(sp + 4, 1);
            lk_sight_lock(sp);
        } else {
            gabi::store<u8>(sp + 5, 0); /* offLockFlg() */
        }
    }
    mNormalSpeed = (f32)((f64)(f32)mNormalSpeed * 1.2); /* HD */
    gabi::call(LK_HD_02416FA8, this);
    return TRUE;
}
VERIFY(0x024350A0, &daPy_lk_c::procRopeSubject);

/* 024352F4 */
BOOL daPy_lk_c::procRopeReady() {
    WWHD_FUNC(0x024352F4, BOOL, this);
    if (!gabi::call<BOOL>(LK_checkItemModeActorPointer, this)) {
        return TRUE;
    }
    u32 rope = gabi::ea(mActorKeepEquip.mActor.get());
    lk_xyz_copy(&mRopePos, gabi::at<cXyz>(rope + 0x314));
    m370C.x = mRopePos.x;
    m370C.z = mRopePos.z;
    gabi::Local<cXyz> d0; /* sp+0x1C */
    gabi::Local<cXyz> d;  /* sp+0x10 */
    cXyz_mi(&m370C, d0, &current.pos);
    lk_xyz_copy(d, d0);
    f64 len = gabi::call<f64>(0x028F4384 /* std::sqrtf */, PSVECSquareMag(d));
    cLib_chaseF(&m35A0, 50.0f, 5.0f);
    shape_angle.x = (s16)gabi::ftoi((f32)mProcVar2 * m35A0 * 0.025f);
    if (len > (f64)(f32)m35A0) {
        gabi::Local<cXyz> step; /* sp+0x28 */
        cXyz_ml(d, step, (f32)((f64)(f32)m35A0 / len));
        PSVECAdd(&current.pos, step, &current.pos);
    }
    f32 m = m35A0;
    if (!(len > (f64)(m + m))) {
        gabi::call(LK_procRopeSwing_init, this, gabi::ea(mActorKeepEquip.mActor.get()), (s32)shape_angle.x);
    }
    return TRUE;
}
VERIFY(0x024352F4, &daPy_lk_c::procRopeReady);

#define mGndChkPos (*gabi::at<cXyz>(gabi::ea(this) + 0xB38)) /* mGndChk's position (dBgS_GndChk + 0x24) */

/* 02435458 */
BOOL daPy_lk_c::procRopeHangWait_init(int mode) {
    WWHD_FUNC(0x02435458, BOOL, this, mode);
    gabi::call(LK_commonProcInit, this, 0x79 /* daPyProc_ROPE_HANG_WAIT_e */);
    lk_xyz_copy(&mRopePos, &mActorKeepRope.mActor->current.pos);
    current.pos.x = mRopePos.x;
    current.pos.z = mRopePos.z;
    dComIfGp_onPlayerStatus0_l(0x800000 /* daPyStts0_UNK800000_e */);
    gravity = 0.0f;
    if (mode == 0) {
        mProcVar0 = 0x5A; /* HD: HIO folded */
        gabi::call(LK_setSingleMoveAnime, this, 0x75 /* ANM_ROPECATCH */, 1.0f, (f32)mFrameCtrlUnder[0].getEnd() - 0.001f, -1, 4.0f);
        gabi::call(LK_setTextureAnime, this, 2, 0);
    } else {
        mProcVar0 = -1;
        gabi::call(LK_setSingleMoveAnime, this, 0x78 /* ANM_ROPEWAIT */, 0.5f, 0.0f, -1, 4.0f);
    }
    mProcVar2 = 0;
    return TRUE;
}
VERIFY(0x02435458, &daPy_lk_c::procRopeHangWait_init);

/* 0243656C */
BOOL daPy_lk_c::procRopeUp_init() {
    WWHD_FUNC(0x0243656C, BOOL, this);
    fopAc_ac_c* rope = mActorKeepRope.mActor;
    lk_xyz_copy(&mRopePos, &rope->current.pos);
    mProcVar6 = 0;
    f32 top;
    if (gabi::call<BOOL>(LK_checkBossGomaStage, this)) {
        top = mRopePos.y - 200.0f;
    } else {
        top = mRopePos.y - 100.0f;
        f32 roof = (f32)(gabi::call<f64>(0x02433E0C /* checkRopeRoofHit */, this, (s32)shape_angle.y) - 60.0);
        if (roof < top) {
            top = roof;
        } else if ((rope != nullptr && fpcM_GetName(rope) == 0x1BE /* fpcNm_HIMO2_e */) || gabi::call<BOOL>(LK_checkSpecialRope, this)) {
            mProcVar6 = 1;
        }
    }
    if (top > current.pos.y) {
        gabi::call(LK_commonProcInit, this, 0x7A /* daPyProc_ROPE_UP_e */);
        dComIfGp_onPlayerStatus0_l(0x800000);
        gabi::call(LK_setSingleMoveAnime, this, 0x79 /* ANM_ROPECLIMB */, 0.8f, 0.0f, -1, 1.5f); /* HD: HIO folded */
        m35A0 = top;
        mFrameCtrlUnder[0].setAttribute(0 /* EMode_NONE */);
        gravity = 0.0f;
        return TRUE;
    }
    if (specialRopeHangUp()) {
        return TRUE;
    }
    if (changeRopeToHangProc()) {
        return TRUE;
    }
    return procRopeHangWait_init(0);
}
VERIFY(0x0243656C, &daPy_lk_c::procRopeUp_init);

/* 02436780 */
BOOL daPy_lk_c::procRopeDown_init() {
    WWHD_FUNC(0x02436780, BOOL, this);
    lk_xyz_copy(&mRopePos, &mActorKeepRope.mActor->current.pos);
    if (mActorKeepEquip.mActor != nullptr) {
        if (!(m3604 < 0.0f)) {
            m35A4 = m3600 - m3604;
        } else if (gabi::call<BOOL>(LK_checkBossGomaStage, this)) {
            m35A4 = m3600 - 100.0f;
        } else {
            f32 y = m3600 - 300.0f;
            lk_xyz_copy(&mGndChkPos, &mRopePos);
            m35A4 = y;
            f64 g = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
            if (g > (f64)(f32)(m35A4 + -175.0f)) {
                m35A4 = (f32)(g + 175.0);
            }
        }
    } else {
        m35A4 = mRopePos.y - gabi::load<f32>(gabi::ea(mActorKeepRope.mActor.get()) + 0x1914); /* himo3: getPlayerMoveLength() */
    }
    if (current.pos.y > m35A4) {
        gabi::call(LK_commonProcInit, this, 0x7B /* daPyProc_ROPE_DOWN_e */);
        dComIfGp_onPlayerStatus0_l(0x800000);
        gabi::call(LK_setSingleMoveAnime, this, 0x7A /* ANM_ROPEDOWN */, 1.5f, 0.0f, -1, 4.0f); /* HD: HIO folded */
        gravity = 0.0f;
        m35A0 = 0.0f;
        return TRUE;
    }
    return procRopeHangWait_init(1);
}
VERIFY(0x02436780, &daPy_lk_c::procRopeDown_init);

/* 0243692C */
BOOL daPy_lk_c::procRopeSwingStart_init() {
    WWHD_FUNC(0x0243692C, BOOL, this);
    lk_xyz_copy(&mRopePos, &mActorKeepRope.mActor->current.pos);
    gabi::call(LK_commonProcInit, this, 0x7C /* daPyProc_ROPE_SWING_START_e */);
    f64 sq = gabi::call<f64>(0x028E8DE8 /* PSVECSquareDistance */, &mRopePos, &current.pos);
    f64 d = gabi::call<f64>(0x028F4384 /* std::sqrtf */, sq);
    f32 r = (f32)(500.0 / d);
    m35A0 = (f32)d;
    if (r > 1.0f) {
        r = 1.0f;
    }
    m35A4 = r;
    gabi::call(LK_setSingleMoveAnime, this, 0x77 /* ANM_ROPESWINGB */, 1.0f, 0.0f, -1, 20.0f);
    mProcVar2 = (s16)gabi::ftoi(2048.0f * m35A4);
    if (gabi::call<BOOL>(LK_checkSpecialRope, this)) {
        mProcVar2 = (s16)gabi::ftoi((f32)mProcVar2 * 0.125f);
    }
    gravity = 0.0f;
    mProcVar0 = 0;
    lk_xyz_copy(&mRopePos, &mActorKeepRope.mActor->current.pos);
    dComIfGp_onPlayerStatus0_l(0x800000);
    return TRUE;
}
VERIFY(0x0243692C, &daPy_lk_c::procRopeSwingStart_init);

/* 02436AB4 */
BOOL daPy_lk_c::procRopeHangWait() {
    WWHD_FUNC(0x02436AB4, BOOL, this);
    dComIfGp_setRStatus_l(0x13 /* dActStts_STOP_e */);
    if (mProcVar0 > 0) {
        mProcVar0 = mProcVar0 - 1;
    }
    if (checkHangRopeActorNull()) {
        return TRUE;
    }
    if (changeRopeEndProc(1)) {
        return TRUE;
    }
    lk_xyz_copy(&mRopePos, &mActorKeepRope.mActor->current.pos);
    f32 stick = mStickDistance; /* kept across the call (GHS) */
    current.pos.x = mRopePos.x;
    current.pos.z = mRopePos.z;
    s16 target = 0;
    if (stick > 0.05f) {
        s32 dir = getDirectionFromAngle(m34DC);
        if ((mItemButton & 0x40) /* spActionButton() */ && dir == 0 /* DIR_FORWARD */) {
            if (std::abs((s32)mProcVar2) <= 0x80) {
                procRopeUp_init();
            }
        } else if ((mItemButton & 0x40) && dir == 1 /* DIR_BACKWARD */) {
            if (std::abs((s32)mProcVar2) <= 0x80) {
                procRopeDown_init();
            }
        } else if (dir == 0 || dir == 1) {
            return procRopeSwingStart_init();
        } else if (dir == 2 || dir == 3) {
            /* HD: left/right swing the hang angle with or without the action button */
            if (dir == 2 /* DIR_LEFT */) {
                s16 lim = (s16)gabi::ftoi(512.0f * stick);
                if (lim > mProcVar2) {
                    s16 v = (s16)(mProcVar2 + gabi::ftoi(64.0f * stick));
                    if (v > lim) {
                        v = lim;
                    }
                    mProcVar2 = v;
                    target = v;
                } else {
                    target = lim;
                }
            } else {
                s16 lim = (s16)gabi::ftoi(-512.0f * stick);
                if (lim < mProcVar2) {
                    s16 v = (s16)(mProcVar2 - gabi::ftoi(64.0f * stick));
                    if (v < lim) {
                        v = lim;
                    }
                    mProcVar2 = v;
                    target = v;
                } else {
                    target = lim;
                }
            }
            if (mProcVar0 != -1) {
                mProcVar0 = -1;
                gabi::call(LK_setSingleMoveAnime, this, 0x78 /* ANM_ROPEWAIT */, 0.5f, 0.0f, -1, 6.0f); /* HD: HIO folded */
            }
        }
    } else if (mProcVar0 == 0) {
        mProcVar0 = -1;
        gabi::call(LK_setSingleMoveAnime, this, 0x78 /* ANM_ROPEWAIT */, 0.5f, 0.0f, -1, 6.0f);
    }
    cLib_addCalcAngleS(&mProcVar2, target, 3, 0x40, 0x10);
    f64 roof;
    if (mProcVar2 > 0) {
        roof = gabi::call<f64>(0x02433E0C /* checkRopeRoofHit */, this, (s32)(s16)(shape_angle.y + 0x2000));
    } else {
        roof = gabi::call<f64>(0x02433E0C, this, (s32)(s16)(shape_angle.y - 0x2000));
    }
    if ((f32)(roof + -55.0f) > current.pos.y) {
        s16 a = (s16)(shape_angle.y + mProcVar2);
        shape_angle.y = a;
        current.angle.y = a;
    }
    return TRUE;
}
VERIFY(0x02436AB4, &daPy_lk_c::procRopeHangWait);

enum : u32 {
    LK_HD_setSpeedAndAngleAtnActor2 = 0x02419CC8, /* unnamed by the matcher: an HD copy of setSpeedAndAngleAtnActor */
    LK_setBlendAtnMoveAnime = 0x023E81E4,
    LK_checkNextRopeMode = 0x023E91E4,
};
#define LK_mAcchChkWallHit() (gabi::load<u32>(gabi::ea(this) + 0x834) & 0x10) /* mAcch.ChkWallHit() */

/* 02436E6C */
BOOL daPy_lk_c::procRopeUp() {
    WWHD_FUNC(0x02436E6C, BOOL, this);
    dComIfGp_setRStatus_l(0x13 /* dActStts_STOP_e */);
    if (checkHangRopeActorNull()) {
        return TRUE;
    }
    if (changeRopeEndProc(1)) {
        return TRUE;
    }
    lk_xyz_copy(&mRopePos, &mActorKeepRope.mActor->current.pos);
    current.pos.x = mRopePos.x;
    current.pos.z = mRopePos.z;
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        s32 dir = getDirectionFromAngle(m34DC);
        if (!(mStickDistance > 0.05f)) {
            procRopeHangWait_init(0);
        } else if (!(mItemButton & 0x40) /* !spActionButton() */) {
            procRopeSwingStart_init();
        } else if (dir == 1 /* DIR_BACKWARD */) {
            procRopeDown_init();
        } else if (dir == 0 /* DIR_FORWARD */) {
            if (m35A0 > current.pos.y) {
                gabi::call(LK_setSingleMoveAnime, this, 0x79 /* ANM_ROPECLIMB */, 0.8f, 0.0f, -1, -1.0f); /* HD: HIO folded */
                mFrameCtrlUnder[0].setAttribute(0 /* EMode_NONE */);
            } else if (!specialRopeHangUp()) {
                procRopeHangWait_init(0);
            }
        } else {
            procRopeHangWait_init(0);
        }
    } else {
        cLib_chaseF(&current.pos.y, m35A0, 5.0f);
        if (!(mFrameCtrlUnder[0].getFrame() < 12.0f)) {
            setResetFlg0(resetFlg0() | 4 /* daPyRFlg0_ROPE_GRAB_RIGHT_HAND */);
        }
    }
    return TRUE;
}
VERIFY(0x02436E6C, &daPy_lk_c::procRopeUp);

/* 02437000 */
BOOL daPy_lk_c::procRopeDown() {
    WWHD_FUNC(0x02437000, BOOL, this);
    dComIfGp_setRStatus_l(0x13 /* dActStts_STOP_e */);
    if (checkHangRopeActorNull()) {
        return TRUE;
    }
    if (changeRopeEndProc(1)) {
        return TRUE;
    }
    lk_xyz_copy(&mRopePos, &mActorKeepRope.mActor->current.pos);
    current.pos.x = mRopePos.x;
    current.pos.z = mRopePos.z;
    s32 dir = getDirectionFromAngle(m34DC);
    if (!(mStickDistance > 0.05f)) {
        procRopeHangWait_init(1);
    } else if (!(mItemButton & 0x40) /* !spActionButton() */) {
        procRopeSwingStart_init();
    } else if (dir == 0 /* DIR_FORWARD */) {
        procRopeUp_init();
    } else if (dir == 1 /* DIR_BACKWARD */) {
        f32 v = m35A0 + 1.5f;
        if (v > 27.0f) { /* HD: HIO folded */
            v = 27.0f;
        }
        m35A0 = v;
        if (cLib_chaseF(&current.pos.y, m35A4, v)) {
            procRopeHangWait_init(1);
        }
    } else {
        procRopeHangWait_init(1);
    }
    return TRUE;
}
VERIFY(0x02437000, &daPy_lk_c::procRopeDown);

/* 02437134 */
BOOL daPy_lk_c::procRopeSwingStart() {
    WWHD_FUNC(0x02437134, BOOL, this);
    dComIfGp_setRStatus_l(0x13 /* dActStts_STOP_e */);
    gabi::Local<cXyz> offset; /* sp+0x10 */
    offset->x = 0.0f;
    offset->z = 0.0f;
    offset->y = -m35A0;
    if (checkHangRopeActorNull()) {
        return TRUE;
    }
    if (!changeRopeEndProc(1)) {
        lk_xyz_copy(&mRopePos, &mActorKeepRope.mActor->current.pos);
        if (mProcVar0 == 1) {
            if (!cLib_addCalcAngleS(&shape_angle.x, mProcVar2, 5, 0x180, 0x40)) {
                gabi::call(LK_procRopeSwing_init, this, 0, (s32)shape_angle.x);
            }
        } else if (!cLib_addCalcAngleS(&shape_angle.x, mProcVar2, 5, 0xA0, 0x40)) {
            if (mProcVar2 > 0) {
                gabi::call(LK_setSingleMoveAnime, this, 0x76 /* ANM_ROPESWINGF */, 1.0f, 0.0f, -1, 10.0f);
                mProcVar2 = (s16)gabi::ftoi(-4096.0f * m35A4);
            } else {
                gabi::call(LK_setSingleMoveAnime, this, 0x77 /* ANM_ROPESWINGB */, 1.0f, 0.0f, -1, 10.0f);
                mProcVar2 = (s16)gabi::ftoi(4096.0f * m35A4);
            }
            mProcVar0 = 1;
            if (gabi::call<BOOL>(LK_checkSpecialRope, this)) {
                mProcVar2 = (s16)gabi::ftoi((f32)mProcVar2 * 0.125f);
            }
        }
        if (LK_mAcchChkWallHit()) {
            gabi::call(LK_procRopeSwing_init, this, 0, (s32)shape_angle.x);
        }
    }
    mDoMtx_stack_c::transS(mRopePos.x, mRopePos.y, mRopePos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, 0);
    PSMTXMultVec(mDoMtx_stack_c::get(), offset, &current.pos);
    return TRUE;
}
VERIFY(0x02437134, &daPy_lk_c::procRopeSwingStart);

/* 02437398 */
BOOL daPy_lk_c::procRopeMove() {
    WWHD_FUNC(0x02437398, BOOL, this);
    if (!gabi::call<BOOL>(LK_checkItemModeActorPointer, this)) {
        return TRUE;
    }
    if (gabi::load<u32>(gabi::ea(mActorKeepEquip.mActor.get()) + 0xB0) == 0 /* fopAcM_GetParam() */ &&
        gabi::call_ptr<BOOL>(gabi::load<u32>(__vtbl + 0xDC), this) /* checkRopeReadyAnime() (HD: virtual) */) {
        if (mpAttnActorLockOn != nullptr) {
            gabi::call(LK_HD_setSpeedAndAngleAtnActor2, this);
        } else if (m355E == 0 && !LK_checkAttentionLock()) {
            gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
        } else {
            gabi::call(LK_setSpeedAndAngleAtn, this);
        }
    }
    if (LK_checkNextMode(0)) {
        return TRUE;
    }
    f32 morf = -1.0f;
    if (std::fabs((f32)mNormalSpeed) < 0.001f) {
        mModeFlg = mModeFlg | 1;
        if (mDirection != 3 /* DIR_RIGHT */) {
            mDirection = 3;
            morf = 2.4f; /* HD: HIO folded */
        }
    } else {
        mModeFlg = mModeFlg & ~1u;
    }
    gabi::call(LK_setBlendAtnMoveAnime, this, morf);
    gabi::call(LK_setBodyAngleXReadyAnime, this);
    return TRUE;
}
VERIFY(0x02437398, &daPy_lk_c::procRopeMove);

/* 02437530 */
BOOL daPy_lk_c::procRopeThrowCatch() {
    WWHD_FUNC(0x02437530, BOOL, this);
    if (!gabi::call<BOOL>(LK_checkItemModeActorPointer, this)) {
        return TRUE;
    }
    BOOL lock = LK_checkAttentionLock();
    u32 st = dComIfGp_ea() + 0x5CD8;
    if (lock) {
        gabi::store<u32>(st, gabi::load<u32>(st) & ~0x40000u);
    } else {
        gabi::store<u32>(st, gabi::load<u32>(st) | 0x40000);
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        mProcVar0 = (s16)(mProcVar0 - 1);
        u32 rope = gabi::ea(mActorKeepEquip.mActor.get());
        if (gabi::load<u32>(rope + 0x3F8) == 0 /* rope->m02DC */ || mProcVar0 == 0) {
            gabi::call(LK_checkNextRopeMode, this);
        }
    }
    return TRUE;
}
VERIFY(0x02437530, &daPy_lk_c::procRopeThrowCatch);

/* 02437630 */
BOOL daPy_lk_c::procRopeUpHang() {
    WWHD_FUNC(0x02437630, BOOL, this);
    if (checkHangRopeActorNull()) {
        return TRUE;
    }
    lk_xyz_copy(&mRopePos, &mActorKeepRope.mActor->current.pos);
    current.pos.x = mRopePos.x;
    current.pos.z = mRopePos.z;
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        lk_xyz_copy(&m3724, &mRopePos);
        m352C = (s16)(shape_angle.y + 0x8000);
        gabi::call(LK_procHangStart_init, this);
    }
    return TRUE;
}
VERIFY(0x02437630, &daPy_lk_c::procRopeUpHang);

/* ---- boomerang, bow, hookshot subjects ---- */
/* HD: the aim procs speed Link up while aiming (mNormalSpeed *= 1.2, then the unnamed 02416FA8) */
static inline void lk_aimSpeedBoost(daPy_lk_c* t) {
    t->mNormalSpeed = (f32)((f64)(f32)t->mNormalSpeed * 1.2);
    gabi::call(LK_HD_02416FA8, t);
}

/* 024376D8 */
BOOL daPy_lk_c::procBoomerangSubject() {
    WWHD_FUNC(0x024376D8, BOOL, this);
    if (!gabi::call<BOOL>(LK_checkItemModeActorPointer, this)) {
        return TRUE;
    }
    if (gabi::load<u16>(gabi::ea(this) + 0x5888) == 0x35 /* checkBoomerangReadyAnime() */) {
        dComIfGp_setDoStatus_l(7 /* dActStts_RETURN_e */);
    }
    gabi::call(LK_setShapeAngleToAtnActor, this);
    if (LK_checkAttentionLock()) {
        gabi::call(LK_setSpeedAndAngleAtn, this);
    } else {
        gabi::call(LK_setSpeedAndAngleAtnActor, this); /* HD */
    }
    if (LK_checkNextMode(0)) {
        gabi::store<u8>(LK_sightPacket + 4, 0); /* mSightPacket.offDrawFlg() */
        return TRUE;
    }
    if (gabi::call<BOOL>(LK_setBodyAngleToCamera, this)) {
        u32 boomerang = gabi::ea(mActorKeepEquip.mActor.get());
        if (boomerang != 0) {
            gabi::Local<cXyz> sight; /* sp+0x08 */
            f64 flyMax = gabi::call<f64>(0x020CECF0 /* daBoomerang_c::getFlyMax */, boomerang);
            gabi::call(LK_checkSightLine, this, flyMax, sight.get());
            lk_sight_setPos(LK_sightPacket, gabi::ea(sight.get()));
            gabi::store<u8>(LK_sightPacket + 4, 1); /* onDrawFlg() */
        }
    }
    lk_aimSpeedBoost(this);
    return TRUE;
}
VERIFY(0x024376D8, &daPy_lk_c::procBoomerangSubject);

/* 02437810 */
BOOL daPy_lk_c::procBoomerangMove() {
    WWHD_FUNC(0x02437810, BOOL, this);
    if (!gabi::call<BOOL>(LK_checkItemModeActorPointer, this)) {
        return TRUE;
    }
    if (mpAttnActorLockOn != nullptr) {
        gabi::call(LK_HD_setSpeedAndAngleAtnActor2, this);
    } else {
        gabi::call(LK_setSpeedAndAngleAtn, this);
    }
    if (LK_checkNextMode(0)) {
        return TRUE;
    }
    f32 morf = -1.0f;
    if (std::fabs((f32)mNormalSpeed) < 0.001f) {
        mModeFlg = mModeFlg | 1;
        if (mDirection != 3 /* DIR_RIGHT */) {
            mDirection = 3;
            morf = 2.4f; /* HD: HIO folded */
        }
    } else {
        mModeFlg = mModeFlg & ~1u;
    }
    gabi::call(LK_setBlendAtnMoveAnime, this, morf);
    gabi::call(LK_setBodyAngleXReadyAnime, this);
    return TRUE;
}
VERIFY(0x02437810, &daPy_lk_c::procBoomerangMove);

/* 02437900 */
BOOL daPy_lk_c::procBoomerangCatch() {
    WWHD_FUNC(0x02437900, BOOL, this);
    if (LK_checkAttentionLock()) {
        gabi::call(LK_setSpeedAndAngleAtn, this);
    } else {
        gabi::call(LK_setSpeedAndAngleNormal, this, 0xBB8); /* HD: HIO folded */
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
    }
    LK_checkNextMode(0);
    return TRUE;
}
VERIFY(0x02437900, &daPy_lk_c::procBoomerangCatch);

/* 024379C0 */
BOOL daPy_lk_c::procBowSubject() {
    WWHD_FUNC(0x024379C0, BOOL, this);
    dComIfGp_setDoStatus_l(7 /* dActStts_RETURN_e */);
    if (LK_checkAttentionLock()) {
        gabi::call(LK_setSpeedAndAngleAtn, this);
    } else {
        gabi::call(LK_setSpeedAndAngleAtnActor, this); /* HD */
    }
    if (!LK_checkNextMode(0)) {
        gabi::call(LK_setBodyAngleToCamera, this);
        lk_aimSpeedBoost(this);
        m35EC = mFrameCtrlUpper[2].getFrame();
    }
    return TRUE;
}
VERIFY(0x024379C0, &daPy_lk_c::procBowSubject);

/* 02437A8C */
BOOL daPy_lk_c::procBowMove() {
    WWHD_FUNC(0x02437A8C, BOOL, this);
    if (mpAttnActorLockOn != nullptr) {
        gabi::call(LK_HD_setSpeedAndAngleAtnActor2, this);
    } else if (!LK_checkAttentionLock() && mActorKeepEquip.mActor == nullptr) {
        gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
    } else {
        gabi::call(LK_setSpeedAndAngleAtn, this);
    }
    if (!LK_checkNextMode(0)) {
        f32 morf = -1.0f;
        if (std::fabs((f32)mNormalSpeed) < 0.001f) {
            mModeFlg = mModeFlg | 1;
            if (mDirection != 3 /* DIR_RIGHT */) {
                mDirection = 3;
                morf = 2.4f; /* HD: HIO folded */
            }
            current.angle.y = (s16)(shape_angle.y - 0x4000);
        } else {
            mModeFlg = mModeFlg & ~1u;
        }
        gabi::call(LK_setBlendAtnMoveAnime, this, morf);
        gabi::call(LK_setBodyAngleXReadyAnime, this);
        m35EC = mFrameCtrlUpper[2].getFrame();
    }
    return TRUE;
}
VERIFY(0x02437A8C, &daPy_lk_c::procBowMove);

/* 02437C0C */
BOOL daPy_lk_c::procHookshotSubject() {
    WWHD_FUNC(0x02437C0C, BOOL, this);
    if (!gabi::call<BOOL>(LK_checkItemModeActorPointer, this)) {
        return TRUE;
    }
    u32 hookshot = gabi::ea(mActorKeepEquip.mActor.get());
    if (gabi::load<u32>(hookshot + 0xB0) == 0 /* hookshot->checkWait() */) {
        dComIfGp_setDoStatus_l(7 /* dActStts_RETURN_e */);
    }
    if (LK_checkAttentionLock()) {
        gabi::call(LK_setSpeedAndAngleAtn, this);
    } else if (gabi::load<u32>(hookshot + 0xB0) == 0) {
        gabi::call(LK_setSpeedAndAngleAtnActor, this); /* HD */
    }
    gabi::store<u8>(LK_sightPacket + 4, 0); /* mSightPacket.offDrawFlg() */
    if (!LK_checkNextMode(0)) {
        if (gabi::load<u32>(hookshot + 0xB0) == 0 && gabi::call<BOOL>(LK_setBodyAngleToCamera, this) &&
            gabi::load<u32>(hookshot + 0xB0) == 0) {
            setHookshotSight();
        }
        lk_aimSpeedBoost(this);
    }
    u32 wait = gabi::load<u32>(hookshot + 0xB0);
    u32 st = dComIfGp_ea() + 0x5CD8;
    if (wait == 0) {
        gabi::store<u32>(st, gabi::load<u32>(st) & ~0x40000u);
    } else {
        gabi::store<u32>(st, gabi::load<u32>(st) | 0x40000);
    }
    return TRUE;
}
VERIFY(0x02437C0C, &daPy_lk_c::procHookshotSubject);

enum : u32 {
    LK_procShipJumpRide_init = 0x0241C244,
};
/* a float copy through an FPR that the recompiled original keeps bit-exact (no SNaN quieting) */
static inline void fcpy_l(u32 dst, u32 src) { gmem_stf32(dst, gmem_ld32(src)); }

/* 02437D60 */
BOOL daPy_lk_c::procHookshotMove() {
    WWHD_FUNC(0x02437D60, BOOL, this);
    if (!gabi::call<BOOL>(LK_checkItemModeActorPointer, this)) {
        return TRUE;
    }
    u32 hookshot = gabi::ea(mActorKeepEquip.mActor.get());
    gabi::store<u8>(LK_sightPacket + 4, 0); /* mSightPacket.offDrawFlg() */
    if (gabi::load<u32>(hookshot + 0xB0) == 0 /* hookshot->checkWait() */) {
        if (mpAttnActorLockOn != nullptr) {
            gabi::call(LK_HD_setSpeedAndAngleAtnActor2, this);
        } else if (m355E == 0 && !LK_checkAttentionLock()) {
            gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
        } else {
            gabi::call(LK_setSpeedAndAngleAtn, this);
        }
    }
    if (!LK_checkNextMode(0)) {
        f32 morf = -1.0f;
        if (std::fabs((f32)mNormalSpeed) < 0.001f) {
            mModeFlg = mModeFlg | 1;
            if (mDirection != 2 /* DIR_LEFT */) {
                mDirection = 2;
                morf = 2.4f; /* HD: HIO folded */
            }
            current.angle.y = (s16)(shape_angle.y + 0x4000);
        } else {
            mModeFlg = mModeFlg & ~1u;
        }
        if (gabi::load<u32>(hookshot + 0xB0) == 0) {
            gabi::call(LK_setBlendAtnMoveAnime, this, morf);
            gabi::call(LK_setBodyAngleXReadyAnime, this);
        }
    }
    return TRUE;
}
VERIFY(0x02437D60, &daPy_lk_c::procHookshotMove);

/* 02437F00 */
BOOL daPy_lk_c::procHookshotFly() {
    WWHD_FUNC(0x02437F00, BOOL, this);
    if (!gabi::call<BOOL>(LK_checkItemModeActorPointer, this)) {
        LK_voiceStart(10);
        return TRUE;
    }
    u32 hookshot = gabi::ea(mActorKeepEquip.mActor.get());
    if (gabi::load<u32>(hookshot + 0xB0) != 3 /* !hookshot->checkPull() */) {
        if (gabi::load<u8>(hookshot + 0xD4FE) != 0 /* checkShipRideFlg() */ && gabi::call<BOOL>(LK_procShipJumpRide_init, this)) {
            return TRUE;
        }
        if (!LK_mAcchChkGroundHit() && gabi::call<BOOL>(LK_changeFrontWallTypeProc, this)) {
            LK_voiceStart(10);
            return TRUE;
        }
        dBgS_LinChk_Set(mLinkLinChk, &m370C, &current.pos, this);
        if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
            lk_xyz_copy(&current.pos, &mLinkLinChkCross);
        }
        s16 a = shape_angle.y;
        f32 x = gabi::fnmsubs(35.0f, cM_ssin(a), gabi::load<f32>(hookshot + 0x314));
        current.pos.x = x;
        f32 y = current.pos.y;
        f32 z = gabi::fnmsubs(35.0f, cM_scos(a), gabi::load<f32>(hookshot + 0x31C));
        m35F0 = y;
        m35F4 = y;
        m3688.x = x;
        m3688.z = z;
        m3688.y = y;
        current.pos.z = z;
        if (LK_mAcchChkGroundHit()) {
            LK_checkNextMode(0);
        } else {
            gabi::call(LK_procFall_init, this, 1, 0.0f);
        }
        LK_voiceStart(10);
    } else {
        shape_angle.x = gabi::load<s16>(hookshot + 0x328);
        s16 ay = gabi::load<s16>(hookshot + 0x32A);
        shape_angle.y = ay;
        current.angle.y = ay;
        gabi::Local<cXyz> move; /* sp+0x08: hookshot->getMoveVec() (HD +0xD51C) */
        fcpy_l(gabi::ea(&move->x), hookshot + 0xD51C);
        fcpy_l(gabi::ea(&move->y), hookshot + 0xD520);
        fcpy_l(gabi::ea(&move->z), hookshot + 0xD524);
        PSVECAdd(&current.pos, move, &current.pos);
        lk_xyz_copy(&m370C, &old.pos);
    }
    return TRUE;
}
VERIFY(0x02437F00, &daPy_lk_c::procHookshotFly);

/* ---- cut reverse ---- */
enum : u32 {
    LK_checkHeavyStateOn = 0x023DBC24,
};
#define mSwordTopPos (*gabi::at<cXyz>(gabi::ea(this) + 0x3E4)) /* daPy_py_c (GameCube 0x2C8) */
/* dComIfGp_getVibration().StartShock(strength, flags, cXyz(0.0f, 1.0f, 0.0f)) */
static inline void lk_startShock(s32 strength, s32 flags) {
    gabi::Local<cXyz> up;
    u32 v = gabi::ea(dComIfGp_getVibration());
    up->x = 0.0f;
    up->y = 1.0f;
    up->z = 0.0f;
    gabi::call(0x025CB374, v, strength, flags, up.get());
}

/* 02438128 */
BOOL daPy_lk_c::procCutReverse_init(int anm) {
    WWHD_FUNC(0x02438128, BOOL, this, anm);
    lk_startShock(4, -0x31);
    if (anm == 0x2B /* ANM_JATTACK */) {
        return TRUE;
    }
    BOOL keep = (mNoResetFlg0 >> 2) & 1; /* daPyFlg0_UNK4 */
    gabi::call(LK_commonProcInit, this, 0x5A /* daPyProc_CUT_REVERSE_e */);
    if (keep) {
        mNoResetFlg0 = mNoResetFlg0 | 4;
    }
    gabi::call(LK_setSingleMoveAnime, this, anm, 0.5f, 2.0f, 0x12, 0.0f); /* HD: HIO folded */
    s16 a = (s16)(shape_angle.y + 0x8000);
    m3522 = 0;
    mNormalSpeed = 12.0f;
    current.angle.y = a;
    LK_voiceStart(37);
    if (mCurrAttributeCode == 0xF /* dBgS_Attr_ICE_e */ && !gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
        s16 c = current.angle.y;
        m36A0.x = gabi::fmadds(10.0f, cM_ssin(c), m36A0.x);
        m36A0.z = gabi::fmadds(10.0f, cM_scos(c), m36A0.z);
    }
    return TRUE;
}
VERIFY(0x02438128, &daPy_lk_c::procCutReverse_init);

/* dKy_Sound_set(cXyz pos (by value), int, u32 actorId, int) */
static inline void lk_dKy_Sound_set_pos(fopAc_ac_c* a, s32 p1, s32 p3) {
    gabi::Local<cXyz> pos; /* sp+0x2C */
    pos->y = a->current.pos.y;
    u32 id = gabi::load<u32>(gabi::ea(a) + 4); /* fopAcM_GetID(this) */
    pos->z = a->current.pos.z;
    pos->x = a->current.pos.x;
    gabi::call(0x0255F458 /* dKy_Sound_set */, pos.get(), p1, id, p3);
}

/* 0243829C */
int daPy_lk_c::changeCutReverseProc(int anm) {
    WWHD_FUNC(0x0243829C, int, this, anm);
    if (mEquipItem != 0x33 /* dItemNo_SKULL_HAMMER_e */) {
        /* mAtCps[i].ChkAtShieldHit() */
        if ((gabi::load<u32>(gabi::ea(this) + 0x7B70) & 1) || (gabi::load<u32>(gabi::ea(this) + 0x7CA8) & 1) ||
            (gabi::load<u32>(gabi::ea(this) + 0x7DE0) & 1)) {
            return procCutReverse_init(anm);
        }
    }
    if (!(mNoResetFlg0 & 0x40 /* daPyFlg0_CUT_AT_FLG */)) {
        return FALSE;
    }
    gabi::Local<cXyz> start; /* sp+0x20 */
    start->x = current.pos.x;
    start->y = current.pos.y + 30.0f;
    start->z = current.pos.z;
    dBgS_LinChk_Set(mLinkLinChk, start, &mSwordTopPos, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        return FALSE;
    }
    u32 pla = dBgS_GetTriPla_l(mLinkLinChkPoly);
    if (pla == 0) { /* HD: NULL check */
        return FALSE;
    }
    f32 ny = gabi::load<f32>(pla + 4);
    if (!(ny < 0.5f) /* cBgW_CheckBGround */ && !(mCurProc == 0x53 /* daPyProc_HAMMER_FRONT_SWING_e */ && ny < 0.6427876f)) {
        if (mCurProc == 0x53) {
            return TRUE;
        }
        return FALSE;
    }
    if (mEquipItem != 0x34 /* dItemNo_DEKU_LEAF_e */) {
        u32 mtrl = gabi::call<u32>(0x024EECAC /* dBgS::GetMtrlSndId */, dComIfG_Bgsp(), mLinkLinChkPoly);
        u32 at = gabi::load<u32>(gabi::ea(this) + 0x7B2C); /* mAtCps[0].GetAtType() */
        if (at == 0x80 /* AT_TYPE_BOKO_STICK */) {
            gabi::call(0x025E1A40 /* mDoAud_seStart */, 0x2833 /* JA_SE_LK_W_WEP_HIT */, &mSwordTopPos, mtrl, (s32)mReverb);
        } else if (at == 0x1000000 /* AT_TYPE_STALFOS_MACE */ || at == 0x10000 /* AT_TYPE_SKULL_HAMMER */) {
            if (at != 0x10000 || !(mNoResetFlg0 & 0x10000000)) {
                gabi::call(0x025E1A40, 0x2855 /* JA_SE_LK_HAMMER_HIT */, &mSwordTopPos, mtrl, (s32)mReverb);
            }
        } else {
            gabi::call(0x025E1A40, 0x2803 /* JA_SE_LK_SW_HIT_S */, &mSwordTopPos, mtrl, (s32)mReverb);
        }
        if (gabi::load<u32>(gabi::ea(this) + 0x7B2C) == 0x10000 && mCurProc != 0x5B /* daPyProc_JUMP_CUT_e */) {
            mNoResetFlg0 = mNoResetFlg0 | 0x10000000;
        }
        s32 attr = gabi::call<s32>(0x024EF0F4 /* dBgS::GetAttributeCode */, dComIfG_Bgsp(), mLinkLinChkPoly);
        if (attr == 2 /* WOOD */ || attr == 3 /* STONE */ || attr == 0x14 /* METAL */) {
            u32 pla2 = dBgS_GetTriPla_l(mLinkLinChkPoly);
            if (pla2 != 0) {
                gabi::Local<cXyz> xz; /* sp+0x38 */
                f32 px = gabi::load<f32>(pla2 + 0);
                xz->y = 0.0f;
                xz->z = gabi::load<f32>(pla2 + 8);
                xz->x = px;
                f64 lxz = gabi::call<f64>(0x028F4384 /* std::sqrtf */, PSVECSquareMag(xz));
                gabi::Local<csXyz> ang; /* sp+0x18 */
                ang->x = gabi::call<s16>(0x020195B0 /* cM_atan2s */, -gabi::load<f32>(pla2 + 4), lxz);
                ang->y = cM_atan2s(gabi::load<f32>(pla2 + 0), gabi::load<f32>(pla2 + 8));
                ang->z = 0;
                if (attr == 2) {
                    dPa_control_c* pa = dComIfGp_getParticle();
                    u32 em = gabi::ea(dPa_control_set(pa, 1, 0x2B /* ID_AK_JN_ELEMENTKIKUZU00 */, &mLinkLinChkCross, ang,
                                                      nullptr, 0xFF, nullptr, -1, gabi::at<GXColor>(gabi::ea(this) + 0x1A8),
                                                      gabi::at<GXColor>(gabi::ea(this) + 0x1A8), nullptr));
                    if (em != 0) {
                        gabi::store<f32>(em + 0x58, 0.2f);  /* setSpread() */
                        gabi::store<f32>(em + 0x34, 8.0f);  /* setRate() */
                        gabi::store<u32>(em + 0x5C, 1);     /* setMaxFrame() */
                        gabi::store<f32>(em + 0x7C, 0.15f); /* setVolumeSweep() */
                    }
                } else {
                    ang->x = (s16)(ang->x + 0x4000);
                    dPa_control_c* pa = dComIfGp_getParticle();
                    u32 em = gabi::ea(dPa_control_set(pa, 1, 0x2C /* ID_AK_JN_ELEMENTHIBANA00 */, &mLinkLinChkCross, ang,
                                                      nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr));
                    if (em != 0) {
                        gabi::store<u32>(em + 0x5C, 1);     /* setMaxFrame() */
                        gabi::store<f32>(em + 0x34, 20.0f); /* setRate() */
                        gabi::store<f32>(em + 0x6C, 20.0f); /* setAwayFromAxisSpeed() */
                        gabi::store<f32>(em + 0x70, 20.0f); /* setDirectionalSpeed() */
                    }
                }
            }
        }
    }
    lk_dKy_Sound_set_pos(this, 100, 5);
    return procCutReverse_init(anm);
}
VERIFY(0x0243829C, &daPy_lk_c::changeCutReverseProc);

/* ---- fan ---- */
enum : u32 {
    LK_mtxFollow_end = 0x023D4538,
    LK_mtxFollow_makeEmitter = 0x023D457C,
    LK_getGroundAngle = 0x023E7D6C,
};
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline u32 lk_getAnmMtx(u32 model, s32 jnt) {
    u32 blk = gabi::load<u32>(model + 0x2C);
    u16 f = gabi::load<u16>(blk + 4);
    u32 mtx = gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, (u16)(f | 0x10));
    return mtx + jnt * 0x30;
}
/* strcmp(dComIfGp_getStartStageName(), name) == 0 (GHS inline loop) */
static inline BOOL lk_stageNameIs(u32 name) {
    u32 p = dComIfGp_ea() + 0x5134;
    u32 q = name;
    u8 a, b;
    for (;;) {
        a = gabi::load<u8>(p);
        b = gabi::load<u8>(q);
        if (a != b || a == 0) {
            break;
        }
        p++;
        q++;
    }
    return a == b;
}

/* 024386C4 */
BOOL daPy_lk_c::procFanSwing() {
    WWHD_FUNC(0x024386C4, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    if (mpAttnActorLockOn != nullptr) {
        mModeFlg = mModeFlg | 0x80;
    } else {
        mModeFlg = mModeFlg & ~0x80u;
    }
    if (changeCutReverseProc(0x15 /* ANM_CUTREL */)) {
        return TRUE;
    }
    if (frameCtrl.checkPass(4.0f) && mProcVar0 != 0) {
        u32 cb = gabi::ea(this) + 0x6808; /* mFanSwingCb */
        u32 em = gabi::load<u32>(cb + 8);
        if (em != 0) { /* mFanSwingCb.deleteCallBack() */
            gabi::store<u32>(em + 0x254, gabi::load<u32>(em + 0x254) & ~0x40u);
            gabi::store<u32>(gabi::load<u32>(cb + 8) + 0x1E4, 0);
            gabi::store<u32>(cb + 8, 0);
        }
        u32 mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 0 /* CL_JNT_LINK_ROOT_e */);
        gabi::Local<cXyz> root; /* sp+0x30: mDoMtx_multVecZero */
        root->x = gabi::load<f32>(mtx + 0xC);
        root->y = gabi::load<f32>(mtx + 0x1C);
        root->z = gabi::load<f32>(mtx + 0x2C);
        dPa_control_c* pa = dComIfGp_getParticle();
        dPa_control_set(pa, 1, 0x4A /* ID_AK_JN_LEAFFAN00 */, root, &shape_angle, nullptr, 0xFF,
                        gabi::at<dPa_levelEcallBack>(cb), -1, nullptr, nullptr, nullptr);
    }
    if (frameCtrl.getFrame() < 22.0f) {
        cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.6f, 2.5f, 1.8f); /* HD: HIO folded */
    } else if (mProcVar6 == 0) {
        if (mProcVar0 != 0) {
            if (!gabi::call<BOOL>(LK_checkHeavyStateOn, this) &&
                (!LK_mAcchChkGroundHit() || (mNoResetFlg0 & 0xA0000000) || !dBgS_ChkPolySafe_l(mAcchGndPoly) ||
                 !gabi::call<BOOL>(0x024EEABC /* dBgS::ChkMoveBG */, dComIfG_Bgsp(), mAcchGndPoly))) {
                mNormalSpeed = 10.0f;
                s16 a = (s16)(shape_angle.y + 0x8000);
                current.angle.y = a;
                if (mCurrAttributeCode == 0xF /* dBgS_Attr_ICE_e */) {
                    m36A0.x = gabi::fmadds(10.0f, cM_ssin(a), m36A0.x);
                    m36A0.z = gabi::fmadds(10.0f, cM_scos(current.angle.y), m36A0.z);
                }
            }
            m3536 = shape_angle.y;
            m353A = 3;
            m3534 = 10;
            gabi::Local<cXyz> windEnd; /* sp+0x24 (GameCube local_60) */
            windEnd->y = current.pos.y + 70.0f;
            windEnd->x = current.pos.x;
            windEnd->z = current.pos.z;
            fopAc_ac_c* lock = mpAttnActorLockOn;
            s16 ax = 0;
            if (lock != nullptr) {
                gabi::Local<cXyz> d;  /* sp+0x54 */
                gabi::Local<cXyz> xz; /* sp+0x48 */
                cXyz_mi(&lock->eyePos, d, &current.pos);
                xz->x = d->x;
                xz->y = 0.0f;
                xz->z = d->z;
                f64 lxz = gabi::call<f64>(0x028F4384 /* std::sqrtf */, PSVECSquareMag(xz));
                ax = gabi::call<s16>(0x020195B0 /* cM_atan2s */, -d->y, lxz);
            }
            m3538 = ax;
            anm_setFrame_hd(gabi::ea(this) + 0x54F4, 0.0f, 0x10, 0x20); /* mpYuchw00Brk->setFrame(0.0f) */
            gabi::call(LK_mtxFollow_end, &m33E8);
            u32 mtx = lk_getAnmMtx(gabi::load<u32>(gabi::ea(this) + 0x53F0) /* mpYuchw00Model */, 2 /* YUCHW00_JNT_LWIND_e */);
            gabi::call(LK_mtxFollow_makeEmitter, &m33E8, 0x32 /* ID_AK_JN_UCHIWAWIND00 */, mtx, windEnd.get(), nullptr);
            u32 em = gabi::ea(m33E8.mpEmitter.get());
            if (em != 0) { /* setGlobalPrmColor(tevStr.mColorC0) */
                gabi::store<u8>(em + 0x244, gabi::load<u8>(gabi::ea(this) + 0x1A1));
                gabi::store<u8>(em + 0x246, gabi::load<u8>(gabi::ea(this) + 0x1A5));
                gabi::store<u8>(em + 0x245, gabi::load<u8>(gabi::ea(this) + 0x1A3));
            }
            gabi::call(0x02018808 /* mFanWindCps.SetStartEnd (cM3dGCps) */, gabi::ea(this) + 0x7FDC, &current.pos, windEnd.get());
            seStartOnlyReverb(0x2844 /* JA_SE_LK_FAN_SWING */);
            gabi::Local<cXyz> land; /* sp+0x18 (GameCube local_48) */
            {
                s16 a = shape_angle.y;
                f32 x = gabi::fmadds(100.0f, cM_ssin(a), current.pos.x);
                f32 y = current.pos.y + 50.0f;
                f32 z = gabi::fmadds(100.0f, cM_scos(a), current.pos.z);
                mGndChkPos.x = x;
                land->x = x;
                land->y = y;
                land->z = z;
                mGndChkPos.y = y;
                mGndChkPos.z = z;
            }
            f64 g = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
            land->y = (f32)g;
            if (!(g < (f64)(f32)(current.pos.y - 50.0f))) {
                s32 attr = gabi::call<s32>(0x024EF0F4 /* dBgS::GetAttributeCode */, dComIfG_Bgsp(), gabi::ea(this) + 0xB28);
                if (lk_stageNameIs(0x1003615C /* "Adanmae" */)) {
                    dPa_control_c* pa = dComIfGp_getParticle();
                    dPa_control_set(pa, 1, 0x8236 /* ID_AK_SN_VOLCANICASHESFAN00 */, land, &shape_angle, nullptr, 0xFF, nullptr,
                                    -1, nullptr, nullptr, nullptr);
                }
                gabi::store<u8>(0x1047B299, 1); /* dPa_control_c::getSmokeEcallback()->onWindOff() */
                gabi::Local<be<s32>> out; /* sp+0x60 */
                dPa_control_c* pa = dComIfGp_getParticle();
                u32 em2 = gabi::call<u32>(0x025A87C0 /* dPa_control_c::setSimpleLand */, pa, attr, land.get(), 0, 1.0f, 1.0f, 1.0f,
                                          gabi::ea(this) + 0x110 /* &tevStr */, out.get(), 0x17);
                gabi::store<u8>(0x1047B299, 0); /* offWindOff() */
                if (em2 != 0) {
                    s16 ay = (s16)(shape_angle.y + 0x8000);
                    s16 gx = gabi::call<s16>(LK_getGroundAngle, this, gabi::ea(this) + 0xB28, (s32)ay);
                    s16 gz = gabi::call<s16>(LK_getGroundAngle, this, gabi::ea(this) + 0xB28, (s32)(s16)(ay - 0x4000));
                    mDoMtx_stack_c::transS(land->x, land->y, land->z);
                    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), gx, ay, gz);
                    gabi::call(0x028249B0 /* JPASetRMtxTVecfromMtx */, mDoMtx_stack_c::get(), em2 + 0x1F0, em2 + 0x22C);
                    gabi::store<f32>(em2 + 0x58, 0.45f); /* setSpread() */
                    gabi::store<f32>(em2 + 0x34, attr == 4 /* dBgS_Attr_GRASS_e */ ? 100.0f : 50.0f); /* setRate() */
                }
            }
            gabi::Local<cXyz> squal; /* sp+0x3C */
            squal->z = current.pos.z;
            squal->x = current.pos.x;
            squal->y = current.pos.y + 50.0f;
            gabi::call(0x0257DC9C /* dKyw_squal_set */, squal.get(), 1000, (s32)shape_angle.y, 280.0f, 1000.0f, 1.0f, 140.0f, 0.08f);
        } else {
            seStartSwordCut(0x2800 /* JA_SE_LK_SW_KAZEKIRI_S */);
        }
        LK_voiceStart(1);
        mProcVar6 = 1;
    } else if (cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.5f, 1.5f, 0.3f) < 0.5f) {
        gabi::call(LK_resetFootEffect, this);
    }
    m35EC = frameCtrl.getFrame();
    if (frameCtrl.getRate() < 0.01f) {
        mNormalSpeed = 0.0f;
        LK_checkNextMode(0);
    }
    return TRUE;
}
VERIFY(0x024386C4, &daPy_lk_c::procFanSwing);

/* ---- tact (Wind Waker) ---- */
enum : u32 {
    LK_checkShipRideUseItem = 0x023E26EC,
    LK_initShipRideUseItem = 0x023E2E18,
};

/* 02439C14 */
u16 daPy_lk_c::getTactPlayRightArmAnm(s32 dir) {
    WWHD_FUNC(0x02439C14, u16, this, dir);
    if (dir == 1) {
        return 0xB; /* ACTIONTAKTRUP */
    } else if (dir == 3) {
        return 8; /* ACTIONTAKTRDW */
    } else if (dir == 2) {
        return 0x12C; /* WAITTAKTRHANDL */
    } else if (dir == 4) {
        return 0x12D; /* WAITTAKTRHANDR */
    }
    return 0x127; /* WAITTAKT */
}
VERIFY(0x02439C14, &daPy_lk_c::getTactPlayRightArmAnm);

/* 02439C58 */
u16 daPy_lk_c::getTactPlayLeftArmAnm(s32 dir) {
    WWHD_FUNC(0x02439C58, u16, this, dir);
    if (dir == 1) {
        return 0x12B; /* WAITTAKTLHANDU */
    } else if (dir == 3) {
        return 0x128; /* WAITTAKTLHANDD */
    } else if (dir == 2) {
        return 0x129; /* WAITTAKTLHANDL */
    } else if (dir == 4) {
        return 0x12A; /* WAITTAKTLHANDR */
    }
    return 0x127; /* WAITTAKT */
}
VERIFY(0x02439C58, &daPy_lk_c::getTactPlayLeftArmAnm);

/* 02439C9C (unnamed by the matcher: checkNpcStatus, GameCube 8014D8AC) */
BOOL daPy_lk_c::checkNpcStatus() {
    WWHD_FUNC(0x02439C9C, BOOL, this);
    if (gabi::load<u32>(dComIfGp_ea() + 0x5B38) /* dComIfGp_getCb1Player() */ != 0) {
        if (!(gabi::load<u32>(gabi::load<u32>(dComIfGp_ea() + 0x5B38) + 0x3BC) & 0x40) /* !checkNpcNotChange() */) {
            u32 cb1 = gabi::load<u32>(dComIfGp_ea() + 0x5B38);
            if ((u32)(s32)gabi::load<s8>(cb1 + 0x326) == (u32)(s32)current.roomNo) {
                return TRUE;
            }
        }
    }
    return FALSE;
}
VERIFY(0x02439C9C, &daPy_lk_c::checkNpcStatus);

/* 02439D24 HD-only (unnamed): the tact cancel trigger: the B trigger or the HD pad button 0x10000 */
BOOL daPy_lk_c::checkTactCancelTrigger() {
    WWHD_FUNC(0x02439D24, BOOL, this);
    if ((mItemTrigger & 2) || (gabi::load<u32>(gabi::load<u32>(0x101F5088) + 0x18) & 0x10000)) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02439D24, &daPy_lk_c::checkTactCancelTrigger);

/* 02439D54 */
BOOL daPy_lk_c::procTactPlay_init(s32 song, int warp, int noMsg) {
    WWHD_FUNC(0x02439D54, BOOL, this, song, warp, noMsg);
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 0);
    gabi::call(LK_commonProcInit, this, 0x9B /* daPyProc_TACT_PLAY_e */);
    u32 a = dComIfGp_ea() + 0x5CDC;
    gabi::store<u32>(a, gabi::load<u32>(a) | 1 /* daPyStts1_WIND_WAKER_CONDUCT_e */);
    mProcVar6 = song;
    u32 msgMgr = gabi::load<u32>(0x101F4B5C);
    u32 sh = (u32)song & 0x3F;
    u8 bit = (u8)((sh & 0x20) ? 0 : (1u << sh));
    if (noMsg == 0 && !(gabi::load<u8>(gabi::load<u32>(0x101F8344) + 0x241) & bit) /* HD: song not yet learned */) {
        if (gabi::call<s32>(0x025F7DB0 /* fopMsgM_messageSet */, msgMgr, 0x5AD + song, 0) != -1) {
            mGameOverId = 0;
        }
    } else {
        mGameOverId = 0xFFFFFFFF;
    }
    gabi::call(0x025E1EF0 /* mDoAud_tact_setVolume */, 0.0f);
    gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
    gabi::call(LK_resetActAnimeUpper, this, 1 /* UPPER_MOVE1_e */, -1.0f);
    mProcVar5 = 10;
    s32 anm;
    if (song == 0) {
        anm = 0xE4; /* ANM_TAKTKAZE */
    } else if (song == 1) {
        anm = 0xE5; /* ANM_TAKTSIPPU */
    } else if (song == 5) {
        anm = 0xE6; /* ANM_TAKTCHUYA */
    } else if (song == 4) {
        anm = 0xE7; /* ANM_TAKTFUJIN */
    } else if (song == 2) {
        anm = 0xE8; /* ANM_TAKTAYATSURI */
    } else {
        anm = 0xE9; /* ANM_TAKTCHISIN */
    }
    gabi::call(LK_setSingleMoveAnime, this, anm, 1.0f, 0.0f, -1, 3.0f);
    gabi::call(LK_initShipRideUseItem, this, use, 2);
    if (use != 0 && mProcVar6 == 1) {
        u32 stage = dComIfGp_ea() + 0x5150; /* dComIfGp_getStageStagInfo(): virtual getStagInfo (slot 0x15C) */
        u32 info = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stage) + 0x15C), stage);
        if (((gabi::load<u32>(info + 0xC) >> 16) & 7) == 7 /* dStageType_SEA_e */ && m34CC != 1 && warp != 0) {
            u32 ship = dComIfGp_getShipActor_l();
            u32 id = fopAcM_create(0x1BB /* fpcNm_TORNADO_e */, 1, &current.pos, current.roomNo, nullptr, nullptr, -1, 0);
            gabi::store<u32>(ship + 0x714, id); /* ship->setTactWarpID() */
        }
    }
    u32 model = gabi::load<u32>(gabi::ea(this) + 0x4440); /* mpEquipItemModel */
    gabi::call(LK_mtxFollow_makeEmitter, &m32E4, 0x51 /* ID_AK_JN_TAKT01 */, model ? model + 0xC8 : 0, &current.pos, nullptr);
    mProcVar4 = (s16)noMsg;
    m3624 = 0;
    mProcVar7 = warp;
    mProcVar0 = 0;
    return TRUE;
}
VERIFY(0x02439D54, &daPy_lk_c::procTactPlay_init);

enum : u32 {
    LK_checkEndMessage = 0x0241FBCC,
    LK_endDemoMode = 0x023F2048,
    LK_procTactPlayEnd_init = 0x02412948,
    LK_setTactModel = 0x023DEF04,
};
/* dCam_getBody()->EndEventCamera(fopAcM_GetID(this)) */
static inline void lk_endEventCamera(fopAc_ac_c* a) {
    u32 cam = gabi::call<u32>(0x024F8044 /* dCam_getBody */);
    gabi::call(0x0253E860 /* dCamera_c::EndEventCamera */, cam, gabi::load<u32>(gabi::ea(a) + 4));
}
/* HD: mAnmRatioUpper[i].setRatio(v) became a per-joint weight table (count +8, data +0xC) */
static inline void lk_anmRatio_set(u32 pack, f32 v) {
    for (s32 i = 0; i < gabi::load<s32>(pack + 8); i++) {
        gabi::store<f32>(gabi::load<u32>(pack + 0xC) + i * 4, v);
    }
}

/* 0243AAD4 HD-only (unnamed): whether the played song takes effect here; otherwise the "nothing
 * happened" message (m3624) is set (GameCube: inside procTactPlay) */
BOOL daPy_lk_c::checkTactSongUsable() {
    WWHD_FUNC(0x0243AAD4, BOOL, this);
    s32 song = mProcVar6;
    if (song == 0) {
        if (gabi::call<BOOL>(0x0257E6EC /* dKyw_gbwind_use_check */)) {
            return TRUE;
        }
        m3624 = 0x14A5;
        return FALSE;
    }
    if (song == 1) {
        u32 ship = dComIfGp_getShipActor_l();
        if (ship != 0 && gabi::load<s32>(ship + 0x714) != -1 /* getTactWarpID() */) {
            return TRUE;
        }
        m3624 = 0x14A6;
        return FALSE;
    }
    if (song == 2) {
        if (checkNpcStatus() && !dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
            return TRUE;
        }
        m3624 = 0x14A8;
        return FALSE;
    }
    if (song == 5) {
        if (gabi::call<BOOL>(0x02560C84 /* dKy_daynighttact_stop_chk */)) {
            m3624 = 0x14A7;
            return FALSE;
        }
        s32 room = current.roomNo;
        u32 rd = gabi::call<u32>(0x025C11DC /* dStage_roomControl_c::getStatusRoomDt */, dComIfGp_ea() + 0x51CC, room);
        u32 fili = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(rd) + 0x1DC), rd); /* getFileListInfo() */
        if (fili == 0) {
            JUT_ASSERT_fail(gabi::at<char>(0x10035020), 0x4DD, gabi::at<char>(0x1003502C));
            return TRUE;
        }
        if (gabi::load<u32>(fili) & 0x40000000 /* dStage_FileList_dt_GetSongOk() */) {
            m3624 = 0x14AA;
            return FALSE;
        }
        return TRUE;
    }
    m3624 = 0x14A9;
    return FALSE;
}
VERIFY(0x0243AAD4, &daPy_lk_c::checkTactSongUsable);

/* 0243AC64 */
BOOL daPy_lk_c::procTactPlay() {
    WWHD_FUNC(0x0243AC64, BOOL, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(LK_setShipRidePos, this, 1);
        shape_angle.y = (s16)(shape_angle.y - 0x4000);
    }
    /* HD: a song Link already knows (save bit) skips the play and takes effect right away */
    if (mProcVar4 == 0 && mProcVar7 != 0) {
        s32 song = mProcVar6;
        if (mTactZevPartnerId == 0xFFFFFFFF || (u32)m34CC != (u32)song) {
            u32 sh = (u32)song & 0x3F;
            u8 bit = (u8)((sh & 0x20) ? 0 : (1u << sh));
            if (gabi::load<u8>(gabi::load<u32>(0x101F8344) + 0x241) & bit) {
                gabi::call(0x025E1E94 /* mDoAud_tact_reset */);
                gabi::store<u8>(dComIfGp_ea() + 0x5BD1, 0); /* dComIfGp_setMetronomeOff() */
                gabi::call(LK_setSingleMoveAnime, this, 0xA8 /* ANM_WAITTAKT */, 0.8f, 0.0f, -1, 10.0f);
                gabi::call(LK_setTextureAnime, this, 6, 0);
                mModeFlg = (mModeFlg | 0x100) & ~0x400u;
                if (checkTactSongUsable()) {
                    gabi::call(0x025E1988 /* seStartSystem */, 0x871 /* JA_SE_TAKT_MATCHED */);
                    return gabi::call<BOOL>(LK_procTactPlayEnd_init, this, (s32)mProcVar6);
                }
            }
        }
    }
    gabi::call(0x025E1F14 /* mDoAud_tact_ambientPlay */);
    if (m3624 != 0) {
        if (gabi::call<BOOL>(LK_checkEndMessage, this, (u32)m3624)) {
            gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
            dComIfGp_event_reset();
            lk_endEventCamera(this);
            gabi::call(LK_endDemoMode, this);
            gabi::call(0x027170AC /* HD: song result to the save */, gabi::load<u32>(0x101F8344), (u32)gabi::load<u8>(gabi::ea(this) + 0x69C3));
            return TRUE;
        }
        return TRUE;
    }
    if (mProcVar5 > 0) {
        mProcVar5 = (s16)(mProcVar5 - 1);
        if (mProcVar5 == 0) {
            gabi::call(0x025E1F58 /* mDoAud_tact_melodyPlay */, (s32)mProcVar6);
        }
    }
    if (mProcVar0 != 0) {
        if (mProcVar4 != 0) {
            gabi::store<u8>(dComIfGp_ea() + 0x5BD1, 0); /* dComIfGp_setMetronomeOff() */
            LK_cutEnd();
            return TRUE;
        }
        u32 msgMgr = gabi::load<u32>(0x101F4B5C);
        u32 st = gabi::call<u32>(0x025F795C /* fopMsgM_SearchByID (HD: the message status) */, msgMgr);
        if (st == 0 || st == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
            fopAc_ac_c* partner = nullptr;
            gabi::store<u8>(dComIfGp_ea() + 0x5BD1, 0);
            if (st == 0x12) {
                gabi::call(0x025F74D0 /* HD: set the message status */, msgMgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
            }
            mGameOverId = 0xFFFFFFFF;
            u32 id = mTactZevPartnerId;
            if (id != 0xFFFFFFFF && (u32)m34CC == (u32)mProcVar6) {
                gabi::Local<be<u32>> key; /* fopAcM_SearchByID */
                *key = id;
                partner = fopAcIt_Judge(0x025E1234 /* fpcSch_JudgeByID */, key.get());
            }
            if (mProcVar7 == 0) {
                LK_cutEnd();
                return TRUE;
            }
            if (partner != nullptr) {
                if (m3494 == 0) {
                    gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
                    dComIfGp_event_reset();
                    lk_endEventCamera(this);
                    gabi::call(LK_endDemoMode, this);
                    return TRUE;
                }
                return gabi::call<BOOL>(LK_procTactPlayEnd_init, this, -1);
            }
            if (checkTactSongUsable()) {
                gabi::call(0x027170AC, gabi::load<u32>(0x101F8344), (u32)gabi::load<u8>(gabi::ea(this) + 0x69C3));
                return gabi::call<BOOL>(LK_procTactPlayEnd_init, this, (s32)mProcVar6);
            }
        }
    } else if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        gabi::call(0x025E1988 /* seStartSystem */, 0x871 /* JA_SE_TAKT_MATCHED */);
        gabi::store<u8>(gabi::load<u32>(0x101F4B5C) + 0x921, 1); /* fopMsgM_messageSendOn() */
        mProcVar0 = 1;
        gabi::call(0x025E1E94 /* mDoAud_tact_reset */);
        gabi::call(LK_setSingleMoveAnime, this, 0xA8 /* ANM_WAITTAKT */, 0.8f, 0.0f, -1, 10.0f); /* HD: HIO folded */
        gabi::call(LK_setTextureAnime, this, 6, 0);
        mModeFlg = (mModeFlg | 0x100) & ~0x400u;
    }
    return TRUE;
}
VERIFY(0x0243AC64, &daPy_lk_c::procTactPlay);

/* 0243B188 */
u32 daPy_lk_c::getDayNightParamData() {
    WWHD_FUNC(0x0243B188, u32, this);
    s32 type = dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */) ? 2 : 0;
    u32 ship = dComIfGp_getShipActor_l();
    s32 room = current.roomNo;
    return setParamData(room, type, 0xCC, ship != 0 ? 0x140 : 0x40);
}
VERIFY(0x0243B188, &daPy_lk_c::getDayNightParamData);

/* 0243B200 */
BOOL daPy_lk_c::procTactPlayEnd() {
    WWHD_FUNC(0x0243B200, BOOL, this);
    u32 evmng = 0;
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(LK_setShipRidePos, this, 0);
        if (mProcVar6 == 0 &&
            !gabi::call<BOOL>(0x025445B8 /* dEvent_manager_c::startCheckOld */, dComIfGp_ea() + 0x52C4, 0x101CECCC /* l_tact_wind_change_event_label */) &&
            !gabi::call<BOOL>(0x025445B8, dComIfGp_ea() + 0x52C4, 0x101CECAC /* l_tact_wind_change_event_label2 */)) {
            /* HD: turn towards the new wind direction (save data) */
            s16 target = (s16)(gabi::load<u16>(gabi::load<u32>(gabi::load<u32>(0x101F8344) + 0x1C4) + 0x92) - 0x8000);
            cLib_addCalcAngleS(&shape_angle.y, target, 4, 0x2000, 0x100);
            current.angle.y = shape_angle.y;
        }
    }
    (void)evmng;
    s32 song = mProcVar6;
    if (song == 0) {
        return TRUE; /* HD: GameCube sets the A status here */
    }
    if (song == 5) {
        if (gabi::call<BOOL>(0x0254457C /* dEvent_manager_c::endCheckOld */, dComIfGp_ea() + 0x52C4, 0x101CECC0 /* l_tact_night_event_label */) &&
            !(mNoResetFlg0 & 0x4000)) {
            mNoResetFlg0 = mNoResetFlg0 | 0x4000;
            u32 save = gabi::load<u32>(0x101F84DC) + 0x1278; /* dSv_turnRestart_c */
            if (dComIfGp_getShipActor_l() != 0) {
                u32 ship = dComIfGp_getShipActor_l();
                s32 room = current.roomNo;
                s16 ay = shape_angle.y;
                u32 param = getDayNightParamData();
                save = gabi::load<u32>(0x101F84DC) + 0x1278;
                gabi::call(0x025B998C /* dSv_turnRestart_c::set */, save, &current.pos, (s32)ay, room, param, ship + 0x314,
                           (s32)gabi::load<s16>(ship + 0x32A), 1);
            } else {
                s32 room = current.roomNo;
                s16 ay = shape_angle.y;
                u32 param = getDayNightParamData();
                save = gabi::load<u32>(0x101F84DC) + 0x1278;
                gabi::call(0x025B998C, save, &current.pos, (s32)ay, room, param, &current.pos, (s32)shape_angle.y, 0);
            }
            if (gabi::load<s32>(dComIfGp_ea() + 0x5CFC) == 3 /* dComIfG_getTimerMode() */) {
                u32 timer = gabi::load<u32>(dComIfGp_ea() + 0x5CF0);
                if (timer != 0) {
                    gabi::call(0x025C58D8 /* dTimer_c::deleteRequest */, timer);
                }
            }
            gabi::call(0x025C3DE4 /* dStage_turnRestart */);
            gabi::call(0x025E1E88 /* mDoAud_taktModeMuteOff */);
        }
        return TRUE;
    }
    if (song == 2) {
        if (gabi::call<BOOL>(0x0254457C, dComIfGp_ea() + 0x52C4, 0x101CEC98 /* l_tact_event_label */)) {
            dComIfGp_event_reset();
            gabi::call(0x023D4688 /* daPy_py_c::changePlayer */, this, gabi::load<u32>(dComIfGp_ea() + 0x5B38));
            gabi::call(LK_endDemoMode, this);
        }
        return TRUE;
    }
    if (song == 1) {
        if (mProcVar7 == 0) {
            if (gabi::load<u8>(dComIfGp_ea() + 0x5BB2) == 0 /* dComIfGp_getMesgStatus() */) {
                gabi::store<u8>(dComIfGp_ea() + 0x5BDB, 1); /* dComIfGp_fmapOpenOn() */
                mProcVar7 = 1;
            }
            return TRUE;
        }
        lk_endEventCamera(this);
        u32 ship = dComIfGp_getShipActor_l();
        if (ship == 0 || gabi::load<s32>(ship + 0x6AC) < 0 /* getTactWarpPosNum() */) {
            dComIfGp_event_reset();
            gabi::call(LK_endDemoMode, this);
            if (ship != 0) {
                gabi::call(0x025D57E4 /* fopAcM_delete(fpc_ProcID) */, gabi::load<u32>(ship + 0x714));
                gabi::store<s32>(ship + 0x714, -1);
            }
        } else {
            if (gabi::load<s32>(dComIfGp_ea() + 0x5CFC) == 3) {
                u32 timer = gabi::load<u32>(dComIfGp_ea() + 0x5CF0);
                if (timer != 0) {
                    gabi::call(0x025C58D8 /* dTimer_c::deleteRequest */, timer);
                }
            }
            gabi::call(LK_procShipPaddle_init, this);
            gabi::store<u8>(ship + 0x636, 0xE); /* ship->setTactWarp() */
        }
    }
    return TRUE;
}
VERIFY(0x0243B200, &daPy_lk_c::procTactPlayEnd);

/* 0243B50C */
BOOL daPy_lk_c::procTactPlayOriginal_init() {
    WWHD_FUNC(0x0243B50C, BOOL, this);
    if (mCurProc == 0x9D /* daPyProc_TACT_PLAY_ORIGINAL_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0x9D);
    u32 a = dComIfGp_ea() + 0x5CDC;
    gabi::store<u32>(a, gabi::load<u32>(a) | 1 /* daPyStts1_WIND_WAKER_CONDUCT_e */);
    mProcVar0 = 0;
    gabi::store<u8>(0x101CEF18, 0); /* daPy_matAnm_c::offMabaFlg() */
    gabi::store<u8>(0x101CEF19, 1);
    gabi::call(LK_setSingleMoveAnime, this, 0xA8 /* ANM_WAITTAKT */, 0.8f, 0.0f, -1, 6.0f); /* HD: HIO folded */
    s16 idx = mProcVar0;
    s32 m = gabi::call<s32>(0x0254045C /* dEvt_control_c::getTactFreeMStick */, dComIfGp_ea() + 0x51D0, (s32)idx);
    mProcVar6 = m;
    gabi::call(LK_setActAnimeUpper, this, (u32)getTactPlayRightArmAnm(m), 1 /* UPPER_MOVE1_e */, 0.0f, 0.0f, -1, -1.0f);
    lk_anmRatio_set(gabi::ea(this) + 0x5828, 0.0f); /* mAnmRatioUpper[UPPER_MOVE1_e].setRatio(0.0f) */
    idx = mProcVar0;
    s32 c = gabi::call<s32>(0x02540468 /* dEvt_control_c::getTactFreeCStick */, dComIfGp_ea() + 0x51D0, (s32)idx);
    mProcVar7 = c;
    gabi::call(LK_setActAnimeUpper, this, (u32)getTactPlayLeftArmAnm(c), 2 /* UPPER_MOVE2_e */, 0.0f, 0.0f, -1, -1.0f);
    lk_anmRatio_set(gabi::ea(this) + 0x5838, 0.0f); /* mAnmRatioUpper[UPPER_MOVE2_e].setRatio(0.0f) */
    gabi::call(LK_setTextureAnime, this, 0, 0);
    s32 m6 = mProcVar6;
    s32 m7 = mProcVar7;
    m3530 = 3;
    gabi::call(0x025E1EA0 /* mDoAud_tact_setStickPos */, m6, m7);
    gabi::call(0x025E1EC0 /* mDoAud_tact_playArmSwing */, (s32)mProcVar6, (s32)mProcVar7);
    gabi::call(LK_setTactModel, this);
    u32 model = gabi::load<u32>(gabi::ea(this) + 0x4440); /* mpEquipItemModel */
    gabi::call(LK_mtxFollow_makeEmitter, &m32E4, 0x51 /* ID_AK_JN_TAKT01 */, model ? model + 0xC8 : 0, &current.pos, nullptr);
    mProcVar5 = 0;
    mProcVar3 = 0xF;
    gabi::call(0x025E1E7C /* mDoAud_taktModeMute */);
    return TRUE;
}
VERIFY(0x0243B50C, &daPy_lk_c::procTactPlayOriginal_init);

/* 0243B74C */
BOOL daPy_lk_c::procTactPlayOriginal() {
    WWHD_FUNC(0x0243B74C, BOOL, this);
    gabi::call(0x025E1988 /* seStartSystem */, 0x205E /* JA_SE_LK_WTAKT_USING */);
    if (gabi::load<f32>(m_old_fdata + 0xC) < 0.01f) { /* m_old_fdata->getOldFrameRate() */
        if (mProcVar3 > 0) {
            mProcVar3 = (s16)(mProcVar3 - 1);
            if (mProcVar3 == 0) {
                mProcVar0 = (s16)(mProcVar0 + 1);
            }
        } else if (mProcVar0 == 5) {
            LK_cutEnd();
            gabi::call(0x025E1ED4 /* mDoAud_tact_stopArmSwing */);
        } else {
            s16 idx = mProcVar0;
            mProcVar3 = 0xF;
            s32 m = gabi::call<s32>(0x0254045C /* dEvt_control_c::getTactFreeMStick */, dComIfGp_ea() + 0x51D0, (s32)idx);
            mProcVar6 = m;
            gabi::call(LK_setActAnimeUpper, this, (u32)getTactPlayRightArmAnm(m), 1 /* UPPER_MOVE1_e */, 0.08f, 0.0f, -1, 5.0f);
            lk_anmRatio_set(gabi::ea(this) + 0x5828, 0.0f);
            s32 m6 = mProcVar6;
            if (m6 == 0 || m6 == 2 || m6 == 4) {
                f32 fr = mFrameCtrlUnder[0].getFrame();
                mFrameCtrlUpper[1].setRate(0.8f);
                mFrameCtrlUpper[1].setFrame(fr);
            }
            idx = mProcVar0;
            s32 c = gabi::call<s32>(0x02540468 /* dEvt_control_c::getTactFreeCStick */, dComIfGp_ea() + 0x51D0, (s32)idx);
            mProcVar7 = c;
            gabi::call(LK_setActAnimeUpper, this, (u32)getTactPlayLeftArmAnm(c), 2 /* UPPER_MOVE2_e */, 0.8f, 0.0f, -1, 5.0f);
            lk_anmRatio_set(gabi::ea(this) + 0x5838, 0.0f);
            s32 a6 = mProcVar6;
            mFrameCtrlUpper[2].setFrame(mFrameCtrlUnder[0].getFrame());
            gabi::call(0x025E1EA0 /* mDoAud_tact_setStickPos */, a6, (s32)mProcVar7);
            gabi::call(0x025E1EC0 /* mDoAud_tact_playArmSwing */, (s32)mProcVar6, (s32)mProcVar7);
        }
    }
    if (mProcVar3 != 0 && mProcVar5 != 0) {
        gabi::call(0x025E1EB4 /* mDoAud_tact_play */);
    }
    mProcVar5 = 1;
    return TRUE;
}
VERIFY(0x0243B74C, &daPy_lk_c::procTactPlayOriginal);

/* ---- vomit (jump flowers) ---- */
enum : u32 {
    LK_checkJumpFlower = 0x023F13A0,
    LK_procVomitJump_init = 0x023E47D4,
};
/* mStts.ClrCcMove() (the cc move vector at mStts + 0) */
static inline void lk_stts_clrCcMove(u32 self) {
    gabi::store<f32>(self + 0x7624, 0.0f);
    gabi::store<f32>(self + 0x7628, 0.0f);
    gabi::store<f32>(self + 0x7620, 0.0f);
}

/* 0243B9C4 */
BOOL daPy_lk_c::procVomitReady() {
    WWHD_FUNC(0x0243B9C4, BOOL, this);
    gabi::call(LK_setFallVoice, this);
    if (!gabi::call<BOOL>(LK_checkJumpFlower, this)) {
        if (LK_mAcchChkGroundHit()) {
            gabi::call(LK_changeLandProc, this, 1.3f);
        } else if (gabi::call<BOOL>(LK_checkFanGlideProc, this, 0)) {
            return TRUE;
        } else if (speed.y < -gravity) {
            gabi::call(LK_setSingleMoveAnime, this, 0xD /* ANM_JMPEDS */, 0.0f, 0.0f, -1, 6.0f); /* HD: HIO folded */
            mModeFlg = mModeFlg & ~0x400u;
            gabi::call(LK_setTextureAnime, this, 0x37, 0);
            resetSeAnime();
        }
    }
    return TRUE;
}
VERIFY(0x0243B9C4, &daPy_lk_c::procVomitReady);

/* 0243BA94 */
BOOL daPy_lk_c::procVomitWait() {
    WWHD_FUNC(0x0243BA94, BOOL, this);
    if ((noResetFlg1() & 0x10 /* daPyFlg1_FORCE_VOMIT_JUMP */) || mProcVar0 != 0) {
        if (!dComIfGp_event_runCheck_l()) {
            gabi::call(LK_procVomitJump_init, this, 0);
        } else {
            mProcVar0 = 1;
        }
        return TRUE;
    }
    s16 target = 0;
    f32 stick = mStickDistance; /* kept across the call (GHS) */
    if (stick > 0.05f) {
        s32 dir = getDirectionFromAngle(m34DC);
        if (dir == 2 /* DIR_LEFT */) {
            s16 lim = (s16)gabi::ftoi(512.0f * stick);
            if (lim > mProcVar2) {
                s16 v = (s16)(mProcVar2 + gabi::ftoi(64.0f * stick));
                if (v > lim) {
                    v = lim;
                }
                mProcVar2 = v;
                target = v;
            } else {
                target = lim;
            }
        } else if (dir == 3 /* DIR_RIGHT */) {
            s16 lim = (s16)gabi::ftoi(-512.0f * stick);
            if (lim < mProcVar2) {
                s16 v = (s16)(mProcVar2 - gabi::ftoi(64.0f * stick));
                if (v < lim) {
                    v = lim;
                }
                mProcVar2 = v;
                target = v;
            } else {
                target = lim;
            }
        }
    }
    cLib_addCalcAngleS(&mProcVar2, target, 3, 0x40, 0x10);
    s16 a = (s16)(shape_angle.y + mProcVar2);
    shape_angle.y = a;
    current.angle.y = a;
    return TRUE;
}
VERIFY(0x0243BA94, &daPy_lk_c::procVomitWait);

/* 0243BC78 */
BOOL daPy_lk_c::procVomitJump() {
    WWHD_FUNC(0x0243BC78, BOOL, this);
    if (mProcVar1 != 0) {
        mProcVar1 = (s16)(mProcVar1 - 1);
        if (mProcVar1 == 0) {
            LK_voiceStart(0);
            speed.y = 46.0f;
            gravity = -2.5f; /* HD: HIO folded */
        }
        lk_stts_clrCcMove(gabi::ea(this));
        return TRUE;
    }
    if (mProcVar0 != 0) {
        lk_stts_clrCcMove(gabi::ea(this));
        mProcVar0 = (s16)(mProcVar0 - 1);
    }
    if (!gabi::call<BOOL>(LK_checkJumpFlower, this)) {
        if (LK_mAcchChkGroundHit()) {
            gabi::call(LK_changeLandProc, this, 1.3f);
            return TRUE;
        }
        s16 sx = shape_angle.x;
        shape_angle.x = (s16)(sx + 0x55F0);
        if (sx < 0 && shape_angle.x > 0) {
            seStartOnlyReverb(0x2849 /* JA_SE_LK_JUMP_FLOWER_OUT */);
        }
        if (mProcVar6 == 1) {
            cLib_addCalc_l(&mNormalSpeed, 12.725f, 0.5f, 2.0f, 0.1f); /* HD: HIO folded */
            return TRUE;
        }
        if (!gabi::call<BOOL>(LK_checkFanGlideProc, this, 1)) {
            f32 stick = mStickDistance; /* kept across the call (GHS) */
            f32 dirScale = 0.0f;
            if (stick > 0.05f) {
                s32 dir = getDirectionFromShapeAngle();
                if (dir == 0 /* DIR_FORWARD */) {
                    dirScale = 1.0f;
                } else if (dir == 1 /* DIR_BACKWARD */) {
                    dirScale = -1.0f;
                }
            }
            cLib_addCalc_l(&mNormalSpeed, gabi::fmuls_ppc(gabi::fmuls_ppc(12.725f, stick), dirScale), 0.5f, 2.0f, 0.1f);
        }
    }
    return TRUE;
}
VERIFY(0x0243BC78, &daPy_lk_c::procVomitJump);

/* 0243BEE4 */
BOOL daPy_lk_c::procVomitLand() {
    WWHD_FUNC(0x0243BEE4, BOOL, this);
    gabi::call(LK_resetFootEffect, this);
    if (mProcVar6 != 0) {
        if (mFrameCtrlUnder[0].getRate() < 0.01f) {
            LK_checkNextMode(0);
        } else if (mFrameCtrlUnder[0].getFrame() > 8.0f) { /* HD: HIO folded */
            LK_checkNextMode(1);
        }
    } else if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        if (mProcVar0 > 0) {
            mProcVar0 = (s16)(mProcVar0 - 1);
        } else {
            gabi::call(LK_setSingleMoveAnime, this, 0x31 /* ANM_LANDDAMAST */, 0.5f, 1.0f, 9, 8.0f);
            mProcVar6 = 1;
        }
    }
    return TRUE;
}
VERIFY(0x0243BEE4, &daPy_lk_c::procVomitLand);

/* ---- hammer ---- */
/* fsel: a >= 0 ? b : c (NaN -> c) */
static inline f32 fsel_l(f32 a, f32 b, f32 c) { return a >= 0.0f ? b : c; }
static inline void lk_JPASetRMtxTVecfromMtx(u32 em) {
    gabi::call(0x028249B0 /* JPASetRMtxTVecfromMtx */, mDoMtx_stack_c::get(), em + 0x1F0, em + 0x22C); /* setGlobalRTMatrix() */
}
/* lfs/stfs copy: bit-exact on PowerPC (signalling NaNs are not quieted), so copy the words */
static inline void lk_copy3f(u32 dst, u32 src) {
    gabi::store<u32>(dst + 0, gabi::load<u32>(src + 0));
    gabi::store<u32>(dst + 4, gabi::load<u32>(src + 4));
    gabi::store<u32>(dst + 8, gabi::load<u32>(src + 8));
}

/* 0243BFC8 */
void daPy_lk_c::setHammerQuake(cBgS_PolyInfo* poly, const cXyz* pos, int mtrl) {
    WWHD_FUNC(0x0243BFC8, void, this, poly, pos, mtrl);
    /* function-local statics (guard words first) */
    const u32 smokeKusaScale = 0x1046D24C, emitterScale = 0x1046D258, emitterTrans = 0x1046D264;
    if (gabi::load<u32>(0x1046D240) == 0) {
        gabi::store<u32>(0x1046D240, 1);
        gabi::store<f32>(smokeKusaScale + 0, 2.0f);
        gabi::store<f32>(smokeKusaScale + 8, 2.0f);
        gabi::store<f32>(smokeKusaScale + 4, 2.0f);
    }
    if (gabi::load<u32>(0x1046D244) == 0) {
        gabi::store<f32>(emitterScale + 8, 1.0f);
        gabi::store<f32>(emitterScale + 0, 1.0f);
        gabi::store<f32>(emitterScale + 4, 0.1f);
        gabi::store<u32>(0x1046D244, 1);
    }
    if (gabi::load<u32>(0x1046D248) == 0) {
        gabi::store<f32>(emitterTrans + 0, 0.0f);
        gabi::store<f32>(emitterTrans + 8, 0.0f);
        gabi::store<u32>(0x1046D248, 1);
        gabi::store<f32>(emitterTrans + 4, 10.0f);
    }
    lk_startShock(6, -0x31);
    setResetFlg0(resetFlg0() | 0x20000 /* daPyRFlg0_HAMMER_QUAKE */);
    if (!(mNoResetFlg0 & 0x10000000)) {
        u32 snd;
        if (mtrl == -1) {
            mGndChkPos.z = mSwordTopPos.z;
            mGndChkPos.x = mSwordTopPos.x;
            mGndChkPos.y = mSwordTopPos.y + 100.0f;
            f64 g = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
            if (g != -1000000000.0) {
                snd = gabi::call<u32>(0x024EECAC /* dBgS::GetMtrlSndId */, dComIfG_Bgsp(), gabi::ea(this) + 0xB28);
            } else {
                snd = mMtrlSndId;
            }
        } else {
            snd = mtrl;
        }
        gabi::call(0x025E1A40 /* mDoAud_seStart */, 0x2855 /* JA_SE_LK_HAMMER_HIT */, &mSwordTopPos, snd, (s32)mReverb);
    }
    if (poly != nullptr && m355C == 0) {
        Mtx34* stk = mDoMtx_stack_c::get();
        PSMTXTrans(stk, pos->x, pos->y, pos->z);
        s16 gx = gabi::call<s16>(LK_getGroundAngle, this, poly, (s32)shape_angle.y);
        s16 ay = shape_angle.y;
        s16 gz = gabi::call<s16>(LK_getGroundAngle, this, poly, (s32)(s16)(ay - 0x4000));
        mDoMtx_ZXYrotM(stk, gx, ay, gz);
        mDoMtx_stack_c::transM(0.0f, 0.0f, -30.0f);
        gabi::Local<cXyz> hit;           /* sp+0x18 (GameCube local_38) */
        gabi::Local<be<f32>> waterY;     /* sp+0x30 */
        gabi::Local<be<s32>> landOut;    /* sp+0x34 */
        u32 s = gabi::ea(stk);
        hit->z = gabi::load<f32>(s + 0x2C); /* multVecZero */
        hit->y = gabi::load<f32>(s + 0x1C);
        hit->x = gabi::load<f32>(s + 0xC);
        s32 attr;
        u32 em;
        if (!gabi::call<BOOL>(0x025D9F70 /* fopAcM_getWaterY */, hit.get(), waterY.get()) || (f32)*waterY < hit->y + 5.0f) {
            dPa_control_c* pa = dComIfGp_getParticle();
            em = gabi::ea(dPa_control_set(pa, 1, 0x214 /* ID_IT_JN_HM_SENKO00 */, hit, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr));
            if (em != 0) {
                lk_JPASetRMtxTVecfromMtx(em);
            }
            pa = dComIfGp_getParticle();
            em = gabi::ea(dPa_control_set(pa, 1, 0x215 /* ID_IT_JN_HM_SENPU00 */, hit, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr));
            if (em != 0) {
                lk_JPASetRMtxTVecfromMtx(em);
            }
            pa = dComIfGp_getParticle();
            em = gabi::ea(dPa_control_set(pa, 1, 0x216 /* ID_IT_JN_HM_SYOGEKI00 */, hit, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr));
            if (em != 0) {
                lk_JPASetRMtxTVecfromMtx(em);
            }
            attr = gabi::call<s32>(0x024EF0F4 /* dBgS::GetAttributeCode */, dComIfG_Bgsp(), poly);
            pa = dComIfGp_getParticle();
            em = gabi::call<u32>(0x025A87C0 /* dPa_control_c::setSimpleLand */, pa, attr, hit.get(), 0, 1.0f, 1.0f, 1.0f,
                                 gabi::ea(this) + 0x110 /* &tevStr */, landOut.get(), 0x1E);
        } else {
            f32 wy = *waterY;
            hit->y = wy;
            PSMTXTrans(stk, hit->x, wy, hit->z);
            mDoMtx_YrotM(stk, shape_angle.y);
            attr = 0x13; /* dBgS_Attr_WATER_e */
            dPa_control_c* pa = dComIfGp_getParticle();
            dPa_control_set(pa, 1, 0x27C /* ID_IT_JN_HM_WP00 */, hit, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
            pa = dComIfGp_getParticle();
            dPa_control_set(pa, 5, 0x3D /* ID_IT_JN_WP_HAMON01 */, hit, nullptr, nullptr, 0xFF,
                            gabi::at<dPa_levelEcallBack>(0x1047B2E4 /* dPa_control_c::mSingleRippleEcallBack */), -1, nullptr, nullptr, nullptr);
            pa = dComIfGp_getParticle();
            em = gabi::call<u32>(0x025A87C0, pa, attr, hit.get(), 0, 1.0f, 1.0f, 1.0f, gabi::ea(this) + 0x110, landOut.get(), 0x1E);
        }
        if (em != 0) {
            lk_JPASetRMtxTVecfromMtx(em);
            if (attr == 0x13 || attr == 4 /* dBgS_Attr_GRASS_e */) {
                gabi::store<f32>(em + 0x58, 1.0f);  /* setSpread() */
                gabi::store<f32>(em + 0x34, 60.0f); /* setRate() */
                if (attr == 0x13) {
                    gabi::store<f32>(em + 0x70, 10.0f);
                    lk_copy3f(em + 0x238, 0x1046CD3C); /* setGlobalParticleScale() */
                } else {
                    gabi::store<f32>(em + 0x70, 40.0f);
                    lk_copy3f(em + 0x238, smokeKusaScale);
                }
            } else {
                gabi::store<f32>(em + 0x58, 0.0f);
                gabi::store<s16>(em + 0x60, 0x28);
                gabi::store<f32>(em + 0x34, 45.0f);
                lk_copy3f(em + 0x8, emitterScale);  /* setEmitterScale() */
                lk_copy3f(em + 0x14, emitterTrans); /* setEmitterTranslation() */
                gabi::store<f32>(em + 0x68, 50.0f);
                lk_copy3f(em + 0x238, smokeKusaScale);
            }
        }
    }
    gabi::Local<cXyz> snd; /* sp+0x24 */
    snd->y = current.pos.y;
    f32 z = current.pos.z;
    snd->x = current.pos.x;
    u32 id = gabi::load<u32>(gabi::ea(this) + 4); /* fopAcM_GetID(this) */
    m355C = 1;
    snd->z = z;
    gabi::call(0x0255F458 /* dKy_Sound_set */, snd.get(), 500, id, 10);
}
VERIFY(0x0243BFC8, &daPy_lk_c::setHammerQuake);

/* 0243C61C */
BOOL daPy_lk_c::procHammerSideSwing() {
    WWHD_FUNC(0x0243C61C, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    m35EC = frameCtrl.getFrame();
    if (frameCtrl.getRate() < 0.01f) {
        mNormalSpeed = 0.0f;
        mDirection = 3; /* DIR_RIGHT */
        LK_checkNextMode(0);
        return TRUE;
    }
    if (frameCtrl.getFrame() > 55.0f) { /* HD: HIO folded */
        f32 sp = mNormalSpeed;
        mNormalSpeed = 0.0f;
        u8 dir = mDirection;
        mDirection = 3;
        if (LK_checkNextMode(1)) {
            return TRUE;
        }
        mDirection = dir;
        mNormalSpeed = sp;
    }
    if (changeCutReverseProc(0x15 /* ANM_CUTREL */)) {
        setHammerQuake(nullptr, nullptr, -1);
        return TRUE;
    }
    if (mpAttnActorLockOn != nullptr) {
        gabi::call(LK_setShapeAngleToAtnActor, this);
        s16 a = shape_angle.y;
        mProcVar2 = a;
        current.angle.y = a;
    } else {
        cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 0x1E, 0x3CDF, 0x1F40); /* HD: HIO folded */
        current.angle.y = shape_angle.y;
    }
    f32 f = frameCtrl.getFrame();
    if (!(f < 25.0f) && f < 36.0f) {
        if (!(mNoResetFlg0 & 0x40 /* daPyFlg0_CUT_AT_FLG */)) {
            setResetFlg0(resetFlg0() | 1);
            seStartSwordCut(0x2857 /* JA_SE_LK_HAMMER_SWING */);
        }
        setResetFlg0(resetFlg0() | 2);
    }
    return TRUE;
}
VERIFY(0x0243C61C, &daPy_lk_c::procHammerSideSwing);

/* 0243C7D8 */
BOOL daPy_lk_c::procHammerFrontSwing_init() {
    WWHD_FUNC(0x0243C7D8, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x53 /* daPyProc_HAMMER_FRONT_SWING_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0xAC /* ANM_HAMSWINGBHIT */, 4.0f, 0.0f, 0x23, 0.0f); /* HD: HIO folded */
    mNormalSpeed = 0.0f;
    current.angle.y = shape_angle.y;
    LK_voiceStart(1);
    if (!LK_checkAttentionLock()) { /* GameCube: both branches end with mProcVar2 = shape_angle.y */
        (void)(f32)mStickDistance;
    }
    mProcVar2 = shape_angle.y;
    gabi::call(LK_setBlurPosResource, this, 0x288 /* _HAMMERFRONT_POS */);
    mProcVar0 = 0;
    LK_FIELD(u8, 0x3AC) = 0x12; /* mCutType = CUT_TYPE_HAMMER_FRONTSWING (a raw u8 in d_a_player.h: not accessible) */
    setResetFlg0((resetFlg0() & ~0x8000000u) | 3);
    lk_swordAnim_changeBckOnly(this, 0x9C /* HAMSWINGBHITA */);
    m35EC = 0.0f;
    return TRUE;
}
VERIFY(0x0243C7D8, &daPy_lk_c::procHammerFrontSwing_init);

/* 0243C954 */
BOOL daPy_lk_c::procHammerFrontSwingReady() {
    WWHD_FUNC(0x0243C954, BOOL, this);
    m35EC = mFrameCtrlUnder[0].getFrame();
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        return procHammerFrontSwing_init();
    }
    if (mpAttnActorLockOn != nullptr) {
        gabi::call(LK_setShapeAngleToAtnActor, this);
        s16 a = shape_angle.y;
        current.angle.y = a;
        mProcVar2 = a;
    } else {
        cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 0x1E, 0x3CDF, 0x1F40);
        current.angle.y = shape_angle.y;
    }
    return TRUE;
}
VERIFY(0x0243C954, &daPy_lk_c::procHammerFrontSwingReady);

/* 0243CA00 */
BOOL daPy_lk_c::procHammerFrontSwingEnd_init() {
    WWHD_FUNC(0x0243CA00, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x54 /* daPyProc_HAMMER_FRONT_SWING_END_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0xAD /* ANM_HAMSWINGBEND */, 0.9f, 5.0f, 0xC, 0.0f); /* HD: HIO folded */
    current.angle.y = shape_angle.y;
    LK_FIELD(u8, 0x3AC) = 0x12; /* mCutType = CUT_TYPE_HAMMER_FRONTSWING (a raw u8 in d_a_player.h: not accessible) */
    mNormalSpeed = 0.0f;
    setResetFlg0(resetFlg0() & ~0x8000000u);
    return TRUE;
}
VERIFY(0x0243CA00, &daPy_lk_c::procHammerFrontSwingEnd_init);

/* 0243CA98 */
BOOL daPy_lk_c::procHammerFrontSwing() {
    WWHD_FUNC(0x0243CA98, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    f32 rate = frameCtrl.getRate();
    f32 frame = frameCtrl.getFrame();
    m35EC = frame;
    if (rate < 0.01f) {
        if (mProcVar0 == 0) {
            return procHammerFrontSwingEnd_init();
        }
        mProcVar0 = (s16)(mProcVar0 - 1);
        return TRUE;
    }
    if (frame > rate && changeCutReverseProc(0x15 /* ANM_CUTREL */)) {
        if (mCurProc != 0x53 /* daPyProc_HAMMER_FRONT_SWING_e */) {
            setHammerQuake(nullptr, nullptr, -1);
            return TRUE;
        }
        gabi::Local<cXyz> gp; /* sp+0x08 */
        {
            f32 y = current.pos.y + 300.0f;
            f32 x = mSwordTopPos.x;
            f32 z = mSwordTopPos.z;
            gp->x = x;
            mGndChkPos.y = y;
            gp->z = z;
            mGndChkPos.x = x;
            gp->y = y;
            mGndChkPos.z = z;
        }
        f64 g = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
        gp->y = (f32)g;
        if (g > (f64)(f32)mSwordTopPos.y) {
            s32 i = 0;
            s32 mtrl = -1;
            for (; i < gabi::load<s32>(mpSwBlur + 0x9C); i++) { /* mSwBlur.field_0x014 */
                u32 pt = mpSwBlur + 0xBC + i * 0xC;      /* mSwBlur.field_0x034[i] */
                f32 px = gabi::load<f32>(pt + 0);
                f32 py = gabi::load<f32>(pt + 4) + 300.0f;
                f32 pz = gabi::load<f32>(pt + 8);
                mGndChkPos.x = px;
                gp->z = pz;
                gp->y = py;
                mGndChkPos.y = py;
                mGndChkPos.z = pz;
                gp->x = px;
                f64 h = gabi::call<f64>(0x02008974, dComIfG_Bgsp(), mGndChk);
                gp->y = (f32)h;
                if (h != -1000000000.0 && (f64)gabi::load<f32>(mpSwBlur + 0xC0 + i * 0xC) > h) {
                    mtrl = gabi::call<s32>(0x024EECAC /* dBgS::GetMtrlSndId */, dComIfG_Bgsp(), gabi::ea(this) + 0xB28);
                    break;
                }
            }
            f32 r = frameCtrl.getRate();
            f32 f5 = gabi::fnmsubs((f32)i * r, 0.1f, frameCtrl.getFrame() - r);
            if (i != 0 && i < gabi::load<s32>(mpSwBlur + 0x9C)) {
                u32 b = mpSwBlur;
                f32 a = gabi::load<f32>(b + 0xB4 + i * 0xC);
                f32 d = gabi::load<f32>(b + 0xC0 + i * 0xC) - a;
                if (std::fabs(d) > 1.0f) {
                    f32 t = (gp->y - a) / d;
                    f5 = gabi::fmadds((1.0f - t) * r, 0.1f, f5);
                }
            }
            f5 = fsel_l(f5, f5, 0.0f);
            frameCtrl.setRate(0.0f);
            frameCtrl.setFrame(f5);
            m35EC = f5;
            mProcVar0 = 10;
            setHammerQuake(gabi::at<cBgS_PolyInfo>(gabi::ea(this) + 0xB28), gp, mtrl);
            return TRUE;
        }
    }
    if (mpAttnActorLockOn != nullptr) {
        gabi::call(LK_setShapeAngleToAtnActor, this);
        s16 a = shape_angle.y;
        current.angle.y = a;
        mProcVar2 = a;
    } else {
        cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 0x1E, 0x3CDF, 0x1F40);
        current.angle.y = shape_angle.y;
    }
    setResetFlg0(resetFlg0() | 2);
    return TRUE;
}
VERIFY(0x0243CA98, &daPy_lk_c::procHammerFrontSwing);

/* 0243CE98 */
BOOL daPy_lk_c::procHammerFrontSwingEnd() {
    WWHD_FUNC(0x0243CE98, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        return LK_checkNextMode(0);
    }
    if (mFrameCtrlUnder[0].getFrame() > 10.0f) { /* HD: HIO folded */
        return LK_checkNextMode(1);
    }
    return TRUE;
}
VERIFY(0x0243CE98, &daPy_lk_c::procHammerFrontSwingEnd);

/* ---- push / pull ---- */
enum : u32 {
    LK_setFrontWallType = 0x023E3850,
    LK_procPushPullWait_init = 0x023ECF74,
};

/* 0243CEE0 */
BOOL daPy_lk_c::setPushPullKeepData(int label) {
    WWHD_FUNC(0x0243CEE0, BOOL, this, label);
    if (label != 0 /* dBgW::PPLABEL_NONE */) {
        if (gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x30) == 0x28 /* checkPowerGloveEquip() */) {
            label |= 8; /* PPLABEL_HEAVY */
        }
        if (mProcVar6 != 0) {
            label |= 4;
        }
    }
    u32 bgs = gabi::ea(dComIfG_Bgsp());
    u32 pp = gabi::call<u32>(0x024EFC54 /* dBgS::PushPullCallBack */, bgs, gabi::ea(this) + 0xCE0 /* mPolyInfo */, this,
                             (s32)shape_angle.y, label);
    if (pp == 0) {
        return FALSE;
    }
    if (label != 0) {
        if (gabi::load<s16>(pp + 0x32A) != mProcVar2) {
            gabi::Local<cXyz> d; /* sp+0x08 */
            cXyz_mi(&current.pos, d, &m370C);
            mDoMtx_stack_c::transS(m370C.x, m370C.y, m370C.z);
            mDoMtx_stack_c::YrotM((s16)(gabi::load<s16>(pp + 0x32A) - mProcVar2));
            PSMTXMultVec(mDoMtx_stack_c::get(), d, &current.pos);
            s16 dd = (s16)(gabi::load<s16>(pp + 0x32A) - mProcVar2);
            shape_angle.y = (s16)(shape_angle.y + dd);
            current.angle.y = (s16)(current.angle.y + dd);
        }
        current.pos.x = current.pos.x + (gabi::load<f32>(pp + 0x314) - m370C.x);
        current.pos.z = current.pos.z + (gabi::load<f32>(pp + 0x31C) - m370C.z);
    }
    lk_xyz_copy(&m370C, gabi::at<cXyz>(pp + 0x314));
    mProcVar2 = gabi::load<s16>(pp + 0x32A);
    return TRUE;
}
VERIFY(0x0243CEE0, &daPy_lk_c::setPushPullKeepData);

/* 0243D06C */
BOOL daPy_lk_c::procPushMove_init() {
    WWHD_FUNC(0x0243D06C, BOOL, this);
    if (!setPushPullKeepData(0)) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x33 /* daPyProc_PUSH_MOVE_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x7F /* ANM_WALKPUSH */, 1.0f, 0.0f, -1, 5.0f); /* HD: HIO folded */
    mProcVar6 = 1;
    dComIfGp_onPlayerStatus0_l(0x4000000 /* daPyStts0_UNK4000000_e */);
    return TRUE;
}
VERIFY(0x0243D06C, &daPy_lk_c::procPushMove_init);

/* 0243D118 */
BOOL daPy_lk_c::procPullMove_init() {
    WWHD_FUNC(0x0243D118, BOOL, this);
    if (!setPushPullKeepData(0)) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x34 /* daPyProc_PULL_MOVE_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x80 /* ANM_WALKPULL */, 1.0f, 0.0f, -1, 5.0f); /* HD: HIO folded */
    mProcVar6 = 1;
    dComIfGp_onPlayerStatus0_l(0x4000000);
    return TRUE;
}
VERIFY(0x0243D118, &daPy_lk_c::procPullMove_init);

/* 0243D1C4 */
BOOL daPy_lk_c::procPushPullWait() {
    WWHD_FUNC(0x0243D1C4, BOOL, this);
    dComIfGp_setDoStatus_l(0x11 /* dActStts_GRAB_e */); /* HD: the do status (GameCube: the R status) */
    gabi::call(LK_setFrontWallType, this);
    if (mProcVar3 != 0) {
        if (gabi::load<u16>(gabi::ea(this) + 0x5888) == 0xFFFF /* checkNoUpperAnime() */) {
            gabi::call(LK_setSingleMoveAnime, this, 0x7E /* ANM_WAITPUSHPULL */, 1.0f, 0.0f, -1, 5.0f); /* HD: HIO folded */
            mProcVar3 = 0;
        }
        return TRUE;
    }
    if (!(mItemButton & 1) /* HD: the do button (GameCube spActionButton) */ || !(resetFlg0() & 8 /* daPyRFlg0_UNK8 */)) {
        LK_checkNextMode(0);
        return TRUE;
    }
    if (mProcVar6 != 0) {
        s16 left = cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 3, 0x800, 0x100);
        current.angle.y = shape_angle.y;
        f64 dx = gabi::call<f64>(0x0200ECD4 /* cLib_addCalc */, &current.pos.x, (f32)m370C.x, 0.5f, 10.0f, 1.0f);
        f64 dz = gabi::call<f64>(0x0200ECD4, &current.pos.z, (f32)m370C.z, 0.5f, 10.0f, 1.0f);
        f32 sum = (f32)(dx + dz);
        if (left == 0 && sum < 5.0f && gabi::load<u16>(gabi::ea(this) + 0x5888) == 0xFFFF) {
            current.pos.x = m370C.x;
            current.pos.z = m370C.z;
            mProcVar6 = 0;
        } else if (mProcVar6 != 0) {
            return TRUE;
        }
    }
    if (mStickDistance > 0.05f) {
        s32 dir = getDirectionFromShapeAngle();
        if (dir == 0 /* DIR_FORWARD */) {
            procPushMove_init();
        } else if (dir == 1 /* DIR_BACKWARD */) {
            procPullMove_init();
        }
    } else {
        gabi::call(LK_setTextureAnime, this, 2 /* mAnmDataTable[ANM_WAITPUSHPULL].mTexAnmIdx */, 0);
    }
    return TRUE;
}
VERIFY(0x0243D1C4, &daPy_lk_c::procPushPullWait);

/* 0243D424 */
BOOL daPy_lk_c::procPushMove() {
    WWHD_FUNC(0x0243D424, BOOL, this);
    dComIfGp_setDoStatus_l(0x11 /* dActStts_GRAB_e */);
    s32 dir = getDirectionFromShapeAngle();
    gabi::call(LK_setFrontWallType, this);
    if (!(resetFlg0() & 8) || (!(mNoResetFlg0 & 0x800 /* daPyFlg0_PUSH_PULL_KEEP */) && !(mItemButton & 1) /* HD: do button */)) {
        LK_checkNextMode(0);
        return TRUE;
    }
    if (!(mNoResetFlg0 & 0x800)) {
        if (mStickDistance > 0.05f) {
            if (dir == 0 /* DIR_FORWARD */) {
                setPushPullKeepData(1 /* PPLABEL_PUSH */);
                return TRUE;
            }
            if (dir == 1 /* DIR_BACKWARD */) {
                return procPullMove_init();
            }
        }
        return gabi::call<BOOL>(LK_procPushPullWait_init, this, 0);
    }
    if (mProcVar6 != 0) {
        LK_voiceStart(19);
    }
    mProcVar6 = 0;
    setPushPullKeepData(1 /* PPLABEL_PUSH */);
    return TRUE;
}
VERIFY(0x0243D424, &daPy_lk_c::procPushMove);

/* 0243D578 */
BOOL daPy_lk_c::procPullMove() {
    WWHD_FUNC(0x0243D578, BOOL, this);
    dComIfGp_setDoStatus_l(0x11 /* dActStts_GRAB_e */);
    s32 dir = getDirectionFromShapeAngle();
    gabi::call(LK_setFrontWallType, this);
    if (!(resetFlg0() & 8) || (!(mNoResetFlg0 & 0x800 /* daPyFlg0_PUSH_PULL_KEEP */) && !(mItemButton & 1) /* HD: do button */)) {
        LK_checkNextMode(0);
        return TRUE;
    }
    if (!(mNoResetFlg0 & 0x800)) {
        if (mStickDistance > 0.05f) {
            if (dir == 1 /* DIR_BACKWARD */) {
                s16 a = shape_angle.y;
                f32 s = cM_ssin(a);
                f32 c = cM_scos(a);
                gabi::Local<cXyz> start; /* sp+0x14 */
                gabi::Local<cXyz> end;   /* sp+0x08 */
                f32 y = current.pos.y + 30.1f;
                start->x = current.pos.x;
                start->y = y;
                start->z = current.pos.z;
                end->x = gabi::fnmsubs(105.0f, s, current.pos.x);
                end->y = y;
                end->z = gabi::fnmsubs(105.0f, c, current.pos.z);
                dBgS_LinChk_Set(mLinkLinChk, start, end, this);
                if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
                    return TRUE;
                }
                f32 y2 = current.pos.y + 89.9f;
                start->y = y2;
                end->y = y2;
                dBgS_LinChk_Set(mLinkLinChk, start, end, this);
                if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
                    return TRUE;
                }
                f32 ex = gabi::fmadds(29.0f, s, end->x);
                f32 ey = end->y;
                f32 ez = gabi::fmadds(29.0f, c, end->z);
                mGndChkPos.y = ey;
                end->x = ex;
                mGndChkPos.x = ex;
                mGndChkPos.z = ez;
                end->z = ez;
                f64 g = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
                if (!((f32)(g - (f64)(f32)current.pos.y) < -30.1f)) {
                    setPushPullKeepData(2 /* PPLABEL_PULL */);
                }
                return TRUE;
            }
            if (dir == 0 /* DIR_FORWARD */) {
                return procPushMove_init();
            }
        }
        return gabi::call<BOOL>(LK_procPushPullWait_init, this, 0);
    }
    if (mProcVar6 == 1) {
        LK_voiceStart(19);
    }
    mProcVar6 = 0;
    setPushPullKeepData(2 /* PPLABEL_PULL */);
    return TRUE;
}
VERIFY(0x0243D578, &daPy_lk_c::procPullMove);

/* ---- bottle ---- */
enum : u32 {
    LK_setShipRidePosUseItem = 0x023E2DC4,
    LK_changeBottleDrinkFace = 0x023EF260,
    LK_setBottleModel = 0x023DF0FC,
    LK_mtxPosFollow_makeEmitterColor = 0x023E47C4, /* unnamed by the matcher: daPy_mtxPosFollowEcallBack_c::makeEmitterColor */
};
#define mpBottleContentsModel LK_FIELD(u32, 0x4970) /* GameCube J3DModel* in 0x974..0x2FAC */
/* dComIfGp_att_getCatghTarget(): dAttLook_c::convPId(play + 0x5934, the catch target id (play + 0x5944)) */
static inline u32 lk_att_getCatchTarget() {
    u32 play = dComIfGp_ea();
    return gabi::call<u32>(0x024EBD10 /* dAttLook_c::convPId */, play + 0x5934, gabi::load<u32>(play + 0x5804 + 0x140));
}

/* 0243D838 */
BOOL daPy_lk_c::procBottleDrink() {
    WWHD_FUNC(0x0243D838, BOOL, this);
    gabi::call(LK_setShipRidePosUseItem, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    u16 idx = gabi::load<u16>(gabi::ea(this) + 0x5848); /* m_anm_heap_under[UNDER_MOVE0_e].mIdx */
    u32 save = 0;
    if (idx == 0x2C /* BINDRINKPRE */) {
        if (frameCtrl.getRate() < 0.01f) {
            gabi::call(LK_setSingleMoveAnime, this, 0xB7 /* ANM_BINDRINKING */, 0.9f, 0.0f, -1, 0.0f); /* HD: HIO folded */
            u16 item = mEquipItem; /* kept across the call when no face is set (GHS) */
            if (gabi::call<BOOL>(LK_changeBottleDrinkFace, this, (u32)item)) {
                gabi::call(LK_setTextureAnime, this, 0x86, 0);
                item = mEquipItem;
            }
            if (item != 0x52 /* dItemNo_GREEN_POTION_e */) {
                /* HD: dComIfGp_setItemLifeCount(dComIfGs_getMaxLife()) accumulates a float */
                f32 maxLife = (f32)gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x20);
                u32 a = dComIfGp_ea() + 0x5B44;
                gabi::store<f32>(a, gabi::load<f32>(a) + maxLife);
            }
            if (mEquipItem != 0x51 /* dItemNo_RED_POTION_e */) {
                u8 maxMagic = gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x33);
                u32 a = dComIfGp_ea() + 0x5B60;
                gabi::store<s16>(a, (s16)(gabi::load<s16>(a) + maxMagic)); /* dComIfGp_setItemMagicCount(dComIfGs_getMaxMagic()) */
                if (mEquipItem == 0x55 /* dItemNo_SOUP_BOTTLE_e */ || mEquipItem == 0x54 /* dItemNo_HALF_SOUP_BOTTLE_e */) {
                    setNoResetFlg1(noResetFlg1() | 0x8000 /* daPyFlg1_SOUP_POWER_UP */);
                }
            }
            resetCurse();
        } else if (frameCtrl.checkPass(65.0f)) {
            LK_voiceStart(39);
        }
    } else if (idx == 0x2B /* BINDRINKING */) {
        save = gabi::load<u32>(0x101F84DC);
        if (gabi::load<u16>(dComIfGp_ea() + 0x5BAC) == gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x22) /* now life == life */ &&
            (u32)(s32)gabi::load<s16>(dComIfGp_ea() + 0x5B62) == (u32)gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x34) /* now magic == magic */) {
            gabi::call(LK_setSingleMoveAnime, this, 0xB8 /* ANM_BINDRINKAFTER */, 1.0f, 0.0f, 0x24, 2.0f); /* HD: HIO folded */
            if (gabi::call<BOOL>(LK_changeBottleDrinkFace, this, (u32)mEquipItem)) {
                gabi::call(LK_setTextureAnime, this, 0x87, 0);
            }
            if (mEquipItem == 0x55 /* dItemNo_SOUP_BOTTLE_e */) {
                gabi::call(LK_setBottleModel, this, 0x54);
                gabi::call(0x025B5578 /* dSv_player_item_c::setEquipBottleItemIn */, gabi::load<u32>(0x101F84DC) + 0x5C, (u32)mReadyItemBtn, 0x54);
            } else {
                mpBottleContentsModel = 0;
                gabi::call(0x025B58B8 /* dSv_player_item_c::setEquipBottleItemEmpty */, gabi::load<u32>(0x101F84DC) + 0x5C, (u32)mReadyItemBtn);
            }
            u16 item = mEquipItem;
            if (item != 0x55 && item != 0x54) {
                u32 mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 0x12 /* CL_JNT_CHIN_JNT_e */);
                u32 color;
                if (item == 0x51) {
                    color = 0x1003617C; /* red */
                } else if (item == 0x52) {
                    color = 0x10036180; /* green */
                } else {
                    color = 0x10036184; /* blue */
                }
                gabi::call(LK_mtxPosFollow_makeEmitterColor, &m33A8, 0x57 /* ID_IT_JN_LK_GEPPU00 */, mtx, &current.pos, &shape_angle,
                           color, 0);
            }
        } else if (frameCtrl.checkPass(10.0f)) {
            LK_voiceStart(39);
        }
    } else if (frameCtrl.getRate() < 0.01f) {
        dComIfGp_event_reset();
        lk_endEventCamera(this);
        gabi::call(LK_endDemoMode, this);
    } else if (frameCtrl.checkPass(8.0f)) {
        LK_voiceStart(40);
    }
    (void)save;
    return TRUE;
}
VERIFY(0x0243D838, &daPy_lk_c::procBottleDrink);

/* 0243E534 */
BOOL daPy_lk_c::procBottleGet_init() {
    WWHD_FUNC(0x0243E534, BOOL, this);
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 0);
    gabi::call(LK_commonProcInit, this, 0xA6 /* daPyProc_BOTTLE_GET_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0xBE /* ANM_BINGET */, 1.0f, 0.0f, 0x32, 4.0f); /* HD: HIO folded */
    u32 a = dComIfGp_ea() + 0x5CDC;
    gabi::store<u32>(a, gabi::load<u32>(a) | 0x1000 /* daPyStts1_UNK1000_e */);
    mGameOverId = 0xFFFFFFFF;
    mProcVar6 = 0;
    u32 cam = gabi::call<u32>(0x024F8044 /* dCam_getBody */);
    gabi::call(0x0253E70C /* dCamera_c::StartEventCamera (varargs) */, cam, 0x12, gabi::load<u32>(gabi::ea(this) + 4),
               0x100361A0 /* "Type" */, &mProcVar6, 0);
    if (mEquipItem == 0x59 /* dItemNo_FOREST_WATER_e */) {
        u32 model = gabi::load<u32>(gabi::ea(this) + 0x4440); /* mpEquipItemModel */
        gabi::call(LK_mtxFollow_makeEmitter, &m32F0, 0x20D /* ID_AK_JN_FORESTWATER00 */, model ? model + 0xC8 : 0, &current.pos, nullptr);
    }
    gabi::call(LK_initShipRideUseItem, this, use, 2);
    return TRUE;
}
VERIFY(0x0243E534, &daPy_lk_c::procBottleGet_init);

/* 0243E634 */
BOOL daPy_lk_c::procBottleSwing() {
    WWHD_FUNC(0x0243E634, BOOL, this);
    gabi::call(LK_setShipRidePosUseItem, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    if (mProcVar6 == 0) {
        if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 6 /* eventInfo.checkCommandCatch() */) {
            mProcVar6 = 1;
            u8 item = gabi::load<u8>(dComIfGp_ea() + 0x52B1); /* dComIfGp_event_getPreItemNo() */
            BOOL skip = FALSE;
            if (item == 0x56 /* dItemNo_WATER_BOTTLE_e */) {
                seStartOnlyReverb(0x287E /* JA_SE_LK_SCOOP_WATER */);
                if (lk_stageNameIs(0x100361A8 /* "Omori" */)) {
                    item = 0x59; /* dItemNo_FOREST_WATER_e */
                    if (gabi::call<BOOL>(0x025B5C80 /* dSv_player_item_c::checkBottle */, gabi::load<u32>(0x101F84DC) + 0x5C, 0x59)) {
                        mProcVar7 = 1;
                        mGameOverId = 0xFFFFFFFF;
                        skip = TRUE;
                    } else {
                        gabi::call(0x025B6120 /* dSv_player_item_record_c::resetTimer */, gabi::load<u32>(0x101F84DC) + 0x86, 0xD2F0); /* HD: 54000 */
                    }
                }
            }
            if (!skip && mProcVar7 == 0) {
                gabi::call(LK_setBottleModel, this, (u32)item);
                gabi::call(0x025B5578 /* dSv_player_item_c::setEquipBottleItemIn */, gabi::load<u32>(0x101F84DC) + 0x5C,
                           (u32)mReadyItemBtn, (u32)gabi::load<u8>(gabi::ea(this) + 0x69B1) /* (u8)mEquipItem */);
                m355E = 0;
            }
            gabi::store<u16>(gabi::ea(this) + 0x420, 5); /* mDemo.setSpecialDemoType() */
        } else {
            f32 f = frameCtrl.getFrame();
            if (!(f < 1.0f) && !(f > 4.0f) && gabi::call<s32>(LK_getReadyItem, this) == 0x50 /* dItemNo_EMPTY_BOTTLE_e */ &&
                (lk_att_getCatchTarget() != 0 || mWaterY > current.pos.y + 10.0f)) {
                gabi::call(0x025D7774 /* fopAcM_orderCatchEvent */, this, lk_att_getCatchTarget());
            }
        }
    }
    if (frameCtrl.getRate() < 0.01f || mProcVar2 != 0) {
        if (mProcVar7 != 0) {
            if (mProcVar2 == 0) {
                mModeFlg = (mModeFlg & ~0x400u) | 0x100;
                gabi::call(LK_setBlendMoveAnime, this, 2.4f); /* HD: HIO folded */
                mProcVar2 = 1;
            }
            if (gabi::call<BOOL>(LK_checkEndMessage, this, 0x14A2)) {
                dComIfGp_event_reset();
                gabi::call(LK_endDemoMode, this);
            }
        } else if (mProcVar6 != 0) {
            procBottleGet_init();
        } else if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
            gabi::call(LK_procShipPaddle_init, this);
        } else {
            LK_checkNextMode(0);
        }
    } else if (frameCtrl.getFrame() > m35A0) {
        if (mProcVar6 == 0 && !dComIfGp_checkPlayerStatus0_l(0x10000)) {
            LK_checkNextMode(1);
        }
    }
    return TRUE;
}
VERIFY(0x0243E634, &daPy_lk_c::procBottleSwing);

/* 0243E948 */
BOOL daPy_lk_c::procBottleGet() {
    WWHD_FUNC(0x0243E948, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    gabi::call(LK_setShipRidePosUseItem, this);
    if (frameCtrl.getRate() < 0.01f && gabi::call<BOOL>(LK_checkEndMessage, this, (u32)mEquipItem + 0x65 /* MSG_NO_FOR_ITEM */)) {
        dComIfGp_event_reset();
        lk_endEventCamera(this);
        gabi::call(LK_deleteEquipItem, this, 0);
        gabi::call(LK_endDemoMode, this);
    } else if (!(frameCtrl.getFrame() < 9.0f)) {
        m355E = 1;
    }
    if (frameCtrl.checkPass(39.0f)) {
        gabi::call(0x025E1988 /* seStartSystem */, 0x8F5 /* JA_SE_ME_ITEM_GET_S */);
        gabi::store<u8>(dComIfGp_ea() + 0x5C20, 2); /* dComIfGp_setMesgBgmOn2() */
    }
    return TRUE;
}
VERIFY(0x0243E948, &daPy_lk_c::procBottleGet);

/* ---- enemy weapons (boko sticks) ---- */
enum : u32 {
    LK_setEnemyWeaponAtParam = 0x023ED2CC,
};

/* 0243EA44 */
BOOL daPy_lk_c::procWeaponNormalSwing() {
    WWHD_FUNC(0x0243EA44, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    if ((mItemButton & 2) /* swordButton() */ && mActorKeepEquip.mActor != nullptr) {
        mNoResetFlg0 = mNoResetFlg0 | 4;
    } else {
        mNoResetFlg0 = mNoResetFlg0 & ~4u;
    }
    if (mActorKeepEquip.mActor == nullptr || frameCtrl.getRate() < 0.01f) {
        if (mActorKeepEquip.mActor == nullptr) {
            mProcVar0 = 0;
            mNoResetFlg0 = mNoResetFlg0 & ~4u;
        }
        if (mProcVar0 > 0) {
            mProcVar0 = (s16)(mProcVar0 - 1);
        } else {
            mNormalSpeed = 0.0f;
            mDirection = 3; /* DIR_RIGHT */
            LK_checkNextMode(0);
            return TRUE;
        }
    } else if (frameCtrl.getFrame() > 27.0f) { /* HD: HIO folded */
        f32 sp = mNormalSpeed;
        mNormalSpeed = 0.0f;
        u8 dir = mDirection;
        mDirection = 3;
        if (LK_checkNextMode(1)) {
            return TRUE;
        }
        mDirection = dir;
        mNormalSpeed = sp;
    }
    if (changeCutReverseProc(0x27 /* ANM_CUTRER */)) {
        return TRUE;
    }
    m34C2 = 1;
    if (mpAttnActorLockOn != nullptr) {
        gabi::call(LK_setShapeAngleToAtnActor, this);
        s16 a = shape_angle.y;
        mProcVar2 = a;
        current.angle.y = a;
    } else {
        cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 0x1E, 0x3CDF, 0x1F40); /* HD: HIO folded */
        current.angle.y = shape_angle.y;
    }
    if (frameCtrl.checkPass(8.0f)) {
        mNormalSpeed = gabi::fmadds(std::fabs((f32)speedF), 0.4f, 11.0f);
    }
    if (frameCtrl.checkPass(6.0f)) {
        frameCtrl.setRate(0.9f);
    }
    f32 f = frameCtrl.getFrame();
    if (!(f < 7.0f) && f < 13.0f) {
        if (!(mNoResetFlg0 & 0x40 /* daPyFlg0_CUT_AT_FLG */)) {
            setResetFlg0(resetFlg0() | 1);
            seStartSwordCut(0x2800 /* JA_SE_LK_SW_KAZEKIRI_S */);
        }
        setResetFlg0(resetFlg0() | 2);
    }
    cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.8f, 2.6f, 0.5f);
    return TRUE;
}
VERIFY(0x0243EA44, &daPy_lk_c::procWeaponNormalSwing);

/* 0243ED14 */
BOOL daPy_lk_c::procWeaponSideSwing() {
    WWHD_FUNC(0x0243ED14, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    if ((mItemButton & 2) /* swordButton() */ && mActorKeepEquip.mActor != nullptr) {
        mNoResetFlg0 = mNoResetFlg0 | 4;
    } else {
        mNoResetFlg0 = mNoResetFlg0 & ~4u;
    }
    if (frameCtrl.getRate() < 0.01f || mActorKeepEquip.mActor == nullptr) {
        mNormalSpeed = 0.0f;
        mDirection = 3; /* DIR_RIGHT */
        LK_checkNextMode(0);
        return TRUE;
    }
    if (frameCtrl.getFrame() > 69.0f) { /* HD: HIO folded */
        f32 sp = mNormalSpeed;
        mNormalSpeed = 0.0f;
        u8 dir = mDirection;
        mDirection = 3;
        if (LK_checkNextMode(1)) {
            return TRUE;
        }
        mDirection = dir;
        mNormalSpeed = sp;
    }
    if (changeCutReverseProc(0x15 /* ANM_CUTREL */)) {
        return TRUE;
    }
    if (mpAttnActorLockOn != nullptr) {
        gabi::call(LK_setShapeAngleToAtnActor, this);
        s16 a = shape_angle.y;
        mProcVar2 = a;
        current.angle.y = a;
    } else {
        cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 0x1E, 0x3CDF, 0x1F40);
        current.angle.y = shape_angle.y;
    }
    f32 f = frameCtrl.getFrame();
    if (!(f < 25.0f) && f < 32.0f) {
        if (!(mNoResetFlg0 & 0x40 /* daPyFlg0_CUT_AT_FLG */)) {
            setResetFlg0(resetFlg0() | 1);
            seStartSwordCut(0x2800 /* JA_SE_LK_SW_KAZEKIRI_S */);
        }
        setResetFlg0(resetFlg0() | 2);
    }
    return TRUE;
}
VERIFY(0x0243ED14, &daPy_lk_c::procWeaponSideSwing);

/* 0243EF08 */
BOOL daPy_lk_c::procWeaponFrontSwing_init() {
    WWHD_FUNC(0x0243EF08, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x4E /* daPyProc_WEAPON_FRONT_SWING_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0xAC /* ANM_HAMSWINGBHIT */, 3.2f, 0.0f, 0xB, 0.0f); /* HD: HIO folded */
    mNormalSpeed = 0.0f;
    current.angle.y = shape_angle.y;
    LK_voiceStart(1);
    seStartSwordCut(0x2857 /* JA_SE_LK_HAMMER_SWING */); /* HD */
    if (!LK_checkAttentionLock()) { /* GameCube: both branches end with mProcVar2 = shape_angle.y */
        (void)(f32)mStickDistance;
    }
    mProcVar2 = shape_angle.y;
    gabi::call(LK_setBlurPosResource, this, 0x288 /* _HAMMERFRONT_POS */);
    gabi::call(LK_setEnemyWeaponAtParam, this, 0);
    mProcVar0 = 0;
    setResetFlg0(resetFlg0() | 3);
    return TRUE;
}
VERIFY(0x0243EF08, &daPy_lk_c::procWeaponFrontSwing_init);

enum : u32 {
    LK_mtxFollow_makeEmitterColor = 0x023D460C,
    LK_setAtParam = 0x023E5600,
    LK_makeFairy = 0x023F73B8,
    LK_HD_0255BE0C = 0x0255BE0C, /* unnamed by the matcher: an HD stage check (d_kankyo) that tints the water particles */
};
/* HD: the water particles take the particle manager's colour (pa + 0xF0..0xF2) as prm and env colour */
static inline void lk_tintWaterEmitter(u32 em) {
    u32 pa = gabi::ea(dComIfGp_getParticle());
    u8 g = gabi::load<u8>(pa + 0xF1), r = gabi::load<u8>(pa + 0xF0), b = gabi::load<u8>(pa + 0xF2);
    gabi::store<u8>(em + 0x244, r);
    gabi::store<u8>(em + 0x245, g);
    gabi::store<u8>(em + 0x246, b);
    u8 g2 = gabi::load<u8>(pa + 0xF1), r2 = gabi::load<u8>(pa + 0xF0), b2 = gabi::load<u8>(pa + 0xF2);
    gabi::store<u8>(em + 0x249, g2);
    gabi::store<u8>(em + 0x248, r2);
    gabi::store<u8>(em + 0x24A, b2);
}

/* 0243DBFC */
BOOL daPy_lk_c::procBottleOpen() {
    WWHD_FUNC(0x0243DBFC, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    gabi::call(LK_setShipRidePosUseItem, this);
    u32 self = gabi::ea(this);
    if (gabi::load<u16>(self + 0x5848) == 0x30 /* BINOPENPRE */) {
        if (frameCtrl.getRate() < 0.01f) {
            if (mEquipItem == 0x57 /* dItemNo_FAIRY_BOTTLE_e */) {
                gabi::call(LK_setSingleMoveAnime, this, 0xBB /* ANM_BINOPENB */, 1.0f, 0.0f, 0x7C, 5.0f); /* HD: HIO folded */
            } else {
                gabi::call(LK_setSingleMoveAnime, this, 0xBA /* ANM_BINOPENA */, 1.0f, 0.0f, 0x45, 5.0f);
            }
        } else if (frameCtrl.checkPass(32.0f)) {
            m355E = 0;
            mRightHandIdx = 8; /* HANDS_JNT_CL_RHANDC_e */
        }
    } else {
        u32 em = gabi::ea(m32E4.mpEmitter.get());
        if (em != 0 && mProcVar2 == 0 && em + 0x1AC != 0 /* getParticleList() */ && gabi::load<u32>(em + 0x1B0) != 0 /* getLast() */) {
            u32 ptcl = gabi::load<u32>(gabi::load<u32>(em + 0x1B0)); /* getLast()->getObject() */
            f32 py = gabi::load<f32>(ptcl + 0x2C);                    /* getGlobalPosition() */
            f32 pz = gabi::load<f32>(ptcl + 0x30);
            f32 px = gabi::load<f32>(ptcl + 0x28);
            gabi::Local<cXyz> pos;   /* sp+0x18 (GameCube sp64) */
            gabi::Local<cXyz> start; /* sp+0x24 (GameCube sp58) */
            gabi::Local<be<f32>> waterY; /* sp+0x3C */
            mGndChkPos.z = pz;
            f32 y100 = py + 100.0f;
            pos->x = px;
            mGndChkPos.x = px;
            mGndChkPos.y = y100;
            pos->y = y100;
            BOOL inWater = FALSE;
            pos->z = pz;
            f64 g1 = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
            lk_xyz_copy(gabi::at<cXyz>(self + 0xCB0), pos); /* mLavaGndChk.SetPos() */
            f64 g2 = gabi::call<f64>(0x02008974, dComIfG_Bgsp(), mGndChk); /* (GameCube bug kept: mGndChk again) */
            f64 top;
            BOOL splash;
            if (gabi::call<BOOL>(0x025D9F70 /* fopAcM_getWaterY */, pos.get(), waterY.get()) && (f64)(f32)*waterY > g1) {
                f32 w = *waterY;
                if ((f64)w > g2) {
                    top = w;
                    inWater = TRUE;
                } else {
                    top = g2;
                }
            } else {
                top = g1 > g2 ? g1 : g2;
            }
            splash = top > (f64)(f32)(py + 15.0f);
            if (splash) {
                pos->y = (f32)top;
                mProcVar2 = 1;
                dPa_control_c* pa = dComIfGp_getParticle();
                u32 em2 = gabi::ea(dPa_control_set(pa, 1, 0x40 /* ID_IT_JN_WP_SHIBUKI */, pos, nullptr, gabi::at<cXyz>(0x10036188) /* splash_scale */,
                                                   0xFF, nullptr, -1, gabi::at<GXColor>(self + 0x1A8), gabi::at<GXColor>(self + 0x1A8), nullptr));
                if (em2 != 0) {
                    gabi::store<u32>(em2 + 0x5C, 0x14); /* setMaxFrame() */
                    gabi::store<f32>(em2 + 0x34, 8.0f); /* setRate() */
                    if (gabi::call<BOOL>(LK_HD_0255BE0C)) {
                        lk_tintWaterEmitter(em2);
                    }
                }
                if (inWater) {
                    dPa_control_c* pa2 = dComIfGp_getParticle();
                    dPa_control_set(pa2, 5, 0x3F /* ID_IT_JN_WP_HAMON03 */, pos, nullptr, gabi::at<cXyz>(0x10036194) /* ripple_scale */, 0xFF,
                                    gabi::at<dPa_levelEcallBack>(0x1047B2E4 /* mSingleRippleEcallBack */), -1, nullptr, nullptr, nullptr);
                    start->x = pos->x;
                    start->y = pos->y;
                    start->z = pos->z;
                } else {
                    f32 x = pos->x, yy = pos->y, z = pos->z;
                    mProcVar0 = 1;
                    m370C.z = z;
                    m370C.x = x;
                    start->x = x;
                    m370C.y = yy;
                    start->y = yy;
                    start->z = z;
                }
                gabi::call(0x02018808 /* cM3dGCps::SetStartEnd */, self + 0x7D6C /* mAtCps[1] */, start.get(), pos.get());
                gabi::store<f32>(self + 0x7CD0, 0.0f); /* mAtCps[1].SetAtVec(0) */
                gabi::store<f32>(self + 0x7D88, 50.0f); /* mAtCps[1].SetR(50.0f) */
                gabi::store<f32>(self + 0x7CD4, 0.0f);
                gabi::store<f32>(self + 0x7CD8, 0.0f);
            } else {
                pos->y = py;
            }
            u32 mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 8 /* CL_JNT_CL_LHANDA_e */);
            gabi::Local<cXyz> d;    /* sp+0x40 */
            gabi::Local<cXyz> vec;  /* sp+0x4C (GameCube sp40) */
            start->x = gabi::load<f32>(mtx + 0xC); /* mDoMtx_multVecZero */
            start->y = gabi::load<f32>(mtx + 0x1C);
            start->z = gabi::load<f32>(mtx + 0x2C);
            cXyz_mi(pos, d, start);
            lk_xyz_copy(vec, d);
            gabi::call(0x02018808, self + 0x7C34 /* mAtCps[0] */, start.get(), pos.get());
            lk_xyz_copy(gabi::at<cXyz>(self + 0x7B98), vec); /* mAtCps[0].SetAtVec() */
            mProcVar3 = 1;
        }
        if (frameCtrl.getRate() < 0.01f) {
            gabi::call(LK_actorKeep_clearData, &mActorKeepRope);
            if (gabi::load<u16>(self + 0x420) == 5 /* checkSpecialDemoMode() */) {
                lk_endEventCamera(this);
                dComIfGp_event_reset();
                gabi::call(LK_endDemoMode, this);
            } else {
                LK_cutEnd();
            }
        } else if (mProcVar4 == 0) {
            u16 item = mEquipItem;
            BOOL water = (item == 0x56 /* dItemNo_WATER_BOTTLE_e */ || item == 0x59 /* dItemNo_FOREST_WATER_e */);
            if (water && frameCtrl.checkPass(9.0f)) {
                seStartOnlyReverb(0x287F /* JA_SE_LK_SPRINKLE_WATER */);
                u32 mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 8 /* CL_JNT_CL_LHANDA_e */);
                gabi::call(LK_mtxFollow_makeEmitterColor, &m32E4, 0x279 /* ID_AK_JN_SPILLWATER00 */, mtx, &current.pos, self + 0x1A8, self + 0x1A8);
                if (gabi::call<BOOL>(LK_HD_0255BE0C) && gabi::load<u32>(self + 0x66FC) != 0) {
                    u32 pa = gabi::ea(dComIfGp_getParticle());
                    u32 e = gabi::load<u32>(self + 0x66FC);
                    u8 g = gabi::load<u8>(pa + 0xF1), r = gabi::load<u8>(pa + 0xF0), b = gabi::load<u8>(pa + 0xF2);
                    gabi::store<u8>(e + 0x244, r);
                    gabi::store<u8>(e + 0x245, g);
                    gabi::store<u8>(e + 0x246, b);
                    u8 g2 = gabi::load<u8>(pa + 0xF1), r2 = gabi::load<u8>(pa + 0xF0);
                    u32 e2 = gabi::load<u32>(self + 0x66FC);
                    u8 b2 = gabi::load<u8>(pa + 0xF2);
                    gabi::store<u8>(e2 + 0x249, g2);
                    gabi::store<u8>(e2 + 0x248, r2);
                    gabi::store<u8>(e2 + 0x24A, b2);
                }
                s32 spl = 0; /* dCcG_At_Spl_UNK0 */
                if (mEquipItem != 0x56) {
                    m3554 = 3;
                    setNoResetFlg1(noResetFlg1() | 0x20000 /* daPyFlg1_FOREST_WATER_USE */);
                    spl = 4; /* dCcG_At_Spl_UNK4 */
                }
                gabi::call(LK_setAtParam, this, 0x100 /* AT_TYPE_WATER */, 0, spl, 0, 0, 0, 5.0f);
            } else {
                item = mEquipItem;
                BOOL empty;
                if (item == 0x57) {
                    empty = !(frameCtrl.getFrame() < 0.0f); /* HD: HIO folded */
                } else if (item == 0x59) {
                    empty = FALSE;
                } else {
                    empty = !(frameCtrl.getFrame() < 15.0f);
                }
                if (empty) {
                    u16 demo = gabi::load<u16>(self + 0x420);
                    mpBottleContentsModel = 0;
                    mProcVar4 = 1;
                    u32 itemSave = gabi::load<u32>(0x101F84DC) + 0x5C;
                    if (demo == 5) {
                        gabi::call(0x025B58B8 /* dSv_player_item_c::setEquipBottleItemEmpty */, itemSave, (u32)mReadyItemBtn);
                    } else if (gabi::load<u8>(dComIfGp_ea() + 0x52B0) == 1 /* dTalkBtn_X_e */) {
                        gabi::call(0x025B58B8, gabi::load<u32>(0x101F84DC) + 0x5C, 0 /* dItemBtn_X_e */);
                    } else {
                        u32 slot = gabi::load<u8>(dComIfGp_ea() + 0x52B0) == 2 /* dTalkBtn_Y_e */ ? 1 : 2;
                        gabi::call(0x025B58B8, gabi::load<u32>(0x101F84DC) + 0x5C, slot);
                    }
                    if (mEquipItem == 0x57) {
                        mModeFlg = mModeFlg | 0x8000000;
                        u32 fairy = gabi::call<u32>(LK_makeFairy, this, self + 0x3F0 /* &mLeftHandPos */, 2);
                        gabi::call(LK_actorKeep_setData, &mActorKeepRope, fairy);
                    } else if (mEquipItem == 0x58 /* dItemNo_FIREFLY_BOTTLE_e */) {
                        fopAcM_create(0x139 /* fpcNm_NH_e */, 1, gabi::at<cXyz>(self + 0x3F0), current.roomNo, &shape_angle, nullptr, -1, 0);
                    }
                }
            }
        } else if (frameCtrl.getFrame() > 40.0f) {
            gabi::call(LK_mtxFollow_end, &m32E4);
        }
    }
    if (mProcVar0 > 0) {
        mProcVar0 = (s16)(mProcVar0 - 1);
        if (mProcVar0 == 0) {
            gabi::Local<cXyz> mark; /* sp+0x30 */
            f32 r1 = cM_rndFX(45.0f);
            f32 x = gabi::fadds_ppc(m370C.x, r1);
            f32 y = m370C.y;
            f32 r2 = cM_rndFX(45.0f);
            f32 z = gabi::fadds_ppc(m370C.z, r2);
            mark->x = x;
            mark->y = y;
            mark->z = z;
            gabi::call(0x025DADA4 /* fopKyM_create */, 0x1D4 /* fpcNm_WATER_MARK_e */, 1, mark.get(), 0, 0);
            mProcVar0 = 3;
        }
    }
    return TRUE;
}
VERIFY(0x0243DBFC, &daPy_lk_c::procBottleOpen);

enum : u32 {
    LK_HD_023D83D8 = 0x023D83D8, /* unnamed by the matcher: HD, stops Link's water-drop emitters (actor_condition bit 4) */
};
/* the tact audio helpers (m_Do_audio): f1 results kept unrounded where the code computes with them */
static inline f64 mDoAud_tact_getBeatFrames_l() { return gabi::call<f64>(0x025E1F08); }
static inline u32 ppc_divwu_l(u32 a, u32 b) { return b ? a / b : 0; }

/* 0243A094 */
BOOL daPy_lk_c::procTactWait() {
    WWHD_FUNC(0x0243A094, BOOL, this);
    u32 self = gabi::ea(this);
    if (actor_condition & 4) { /* HD */
        gabi::call(LK_HD_023D83D8, this);
    }
    gabi::call(LK_setShipRidePosUseItem, this);
    gabi::call(0x025E1F14 /* mDoAud_tact_ambientPlay */);
    s32 p6 = mProcVar6;
    if (p6 == -4) {
        LK_cutEnd();
        return TRUE;
    }
    s16 p1 = mProcVar1;
    if (p1 > 0) {
        p1 = (s16)(p1 - 1);
        mProcVar1 = p1;
        if (mProcVar6 == -5) {
            f32 t = m35A0 + 1.0f;
            p1 = mProcVar1;
            m35A0 = t;
        }
        if (p1 != 0) {
            return TRUE;
        }
        p6 = mProcVar6;
        if (p6 == -5 || p6 == 6 || p6 == 7) {
            LK_cutEnd();
            gabi::store<u8>(dComIfGp_ea() + 0x5BD1, 0); /* dComIfGp_setMetronomeOff() */
            return TRUE;
        }
        if (p6 == -1 || p6 == -3 || p6 >= 0) {
            procTactPlay_init(mProcVar7, p6 == -1, p6 >= 0);
        }
        return TRUE;
    }
    if (p1 < -1) { /* HD: a short count-up before the wait reacts */
        p1 = (s16)(p1 + 1);
        mProcVar1 = p1;
        p6 = mProcVar6;
    }
    if ((u32)p6 == 1 || ((u32)p6 >= 5 && (u32)p6 <= 7)) {
        f32 t = m35AC;
        if (!(t < 0.0f)) {
            m35AC = t - 1.0f;
            p6 = mProcVar6;
        }
        if (p6 == 5) {
            dComIfGp_setBStatus_l(7 /* dActStts_RETURN_e */); /* HD: the B status (GameCube: A) */
        }
        p1 = mProcVar1;
    } else if (p6 != -5) {
        dComIfGp_setBStatus_l(7);
        p1 = mProcVar1;
    }
    BOOL cancel = TRUE;
    if (!checkTactCancelTrigger()) {
        if (!(p1 < -1) || LK_mAcchChkGroundHit()) {
            cancel = FALSE;
        }
    }
    if (p1 == 0 || (cancel && dComIfGp_getBStatus_l() == 7) || !(m35AC > 0.0f)) {
        if (p1 != 0 && mProcVar1 != 0) {
            gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
            gabi::call(LK_resetActAnimeUpper, this, 1 /* UPPER_MOVE1_e */, -1.0f);
            gabi::call(0x025E1988 /* seStartSystem */, 0x870 /* JA_SE_TAKT_USE_CANCEL */);
        }
        gabi::store<u8>(dComIfGp_ea() + 0x5BD1, 0); /* dComIfGp_setMetronomeOff() */
        if (cancel && mProcVar6 == 5) {
            m35AC = -1000.0f;
        }
        if (mProcVar6 == -1) {
            dComIfGp_event_reset();
            lk_endEventCamera(this);
            gabi::call(LK_endDemoMode, this);
        } else {
            LK_cutEnd();
        }
        gabi::call(0x025E1E94 /* mDoAud_tact_reset */);
        return TRUE;
    }
    s32 right, left;
    BOOL count;
    u8 tactOn = gabi::load<u8>(gabi::load<u32>(gabi::load<u32>(0x101F8344) + 0x1DC) + 0x16B); /* HD */
    if (tactOn != 0 && mProcVar6 != -5) {
        right = gabi::call<s32>(0x025E162C /* mDoAud_getTactDirection */, 1, (s32)mProcVar2);
        left = gabi::call<s32>(0x025E162C, 0, (s32)mProcVar3);
        if (left == 0) { /* HD: the GamePad input */
            left = gabi::call<s32>(0x026185A0, gabi::load<u32>(0x101F5088));
        }
        count = m3624 != 0;
        gabi::call(0x025E1EF0 /* mDoAud_tact_setVolume */, mStickDistance * cM_scos(m34DC));
    } else {
        right = mProcVar2;
        left = mProcVar3;
        count = tactOn != 0;
    }
    if (count) { /* HD: GameCube counts every frame */
        m35A0 = m35A0 + 1.0f;
    }
    if ((u32)right != (u32)(s32)mProcVar2) {
        f32 w = m35A4;
        if (!(w > 0.0f) || right != 0) {
            gabi::call(LK_setActAnimeUpper, this, (u32)getTactPlayRightArmAnm(right), 1 /* UPPER_MOVE1_e */, 0.08f, 0.0f, -1, 4.0f);
            if (right == 0 || right == 2 || right == 4) {
                mFrameCtrlUpper[1].setRate(0.8f);
                mFrameCtrlUpper[1].setFrame(mFrameCtrlUnder[0].getFrame());
            }
            s16 r = mProcVar2;
            if (r == 4 || r == 2 || right == 4 || right == 2) {
                gabi::call(0x025E1E94 /* mDoAud_tact_reset */);
                gabi::call(0x025E1EE0 /* mDoAud_tact_setBeat */, right);
                mProcVar5 = 0;
                m35A0 = 0.0f;
                gabi::call(0x025E3F3C /* mDoExt_MtxCalcOldFrame::initOldFrameMorf */, (u32)m_old_fdata, 5.0f, 0, 0x2A);
                m3624 = 0;
                mProcVar0 = -1;
            }
            mProcVar2 = (s16)right;
            lk_anmRatio_set(self + 0x5828, 0.0f); /* mAnmRatioUpper[UPPER_MOVE1_e].setRatio(0.0f) */
            if (mProcVar2 != 0) {
                m35A4 = 2.0f;
            }
        } else {
            m35A4 = w - 1.0f;
        }
    } else if (mProcVar2 != 0) {
        m35A4 = 2.0f;
    }
    BOOL beatSet = FALSE;
    f64 beat;
    f32 cnt;
    if ((u32)left != (u32)(s32)mProcVar3) {
        f32 w = m35A8;
        if (!(w > 0.0f) || left != 0) {
            gabi::call(LK_setActAnimeUpper, this, (u32)getTactPlayLeftArmAnm(left), 2 /* UPPER_MOVE2_e */, 1.0f, 0.0f, -1, 4.0f); /* HD: HIO folded */
            mProcVar3 = (s16)left;
            lk_anmRatio_set(self + 0x5838, 0.0f); /* mAnmRatioUpper[UPPER_MOVE2_e].setRatio(0.0f) */
            gabi::call(0x025E1F48 /* mDoAud_tact_armSoundPlay */, left);
            if (mProcVar3 != 0) {
                m35A8 = 2.0f;
                if (mProcVar5 == 0) { /* HD: the first input starts the beat */
                    m3624 = gabi::call<u32>(0x025E1EFC /* mDoAud_tact_getBeat */);
                    f64 bf = mDoAud_tact_getBeatFrames_l();
                    cnt = (f32)bf;
                    m35A0 = cnt;
                    beat = mDoAud_tact_getBeatFrames_l();
                    beatSet = TRUE;
                }
            }
        } else {
            cnt = m35A0;
            m35A8 = w - 1.0f;
            beat = mDoAud_tact_getBeatFrames_l();
            beatSet = TRUE;
        }
    } else if (mProcVar3 != 0) {
        m35A8 = 2.0f;
    }
    if (!beatSet) {
        cnt = m35A0;
        beat = mDoAud_tact_getBeatFrames_l();
    }
    s16 p0;
    if (!((f64)cnt < beat)) {
        gabi::call(0x025E1F20 /* mDoAud_tact_metronomePlay */, (s32)mProcVar5, (s32)mProcVar3);
        /* mpEquipItemBrk->setFrame(getFrameMax() - 0.001f) (HD brkAnm at +0x48F8, its end frame at +0xA) */
        anm_setFrame_hd(self + 0x48F8, (f32)gabi::load<s16>(self + 0x4902) - 0.001f, 0x10, 0x20);
        mProcVar0 = 3;
        f64 bf = mDoAud_tact_getBeatFrames_l();
        f32 nc = (f32)((f64)(f32)m35A0 - bf);
        u32 m24 = m3624;
        m35A0 = nc;
        if (m24 != 0) { /* HD: a countdown of beats (GameCube toggles 0/1) */
            p0 = mProcVar0;
            m3624 = m24 - 1;
            goto check_p0;
        }
    } else {
        anm_setFrame_hd(self + 0x48F8, 0.0f, 0x10, 0x20); /* mpEquipItemBrk->setFrame(0.0f) */
    }
    p0 = mProcVar0;
check_p0:
    if (p0 >= 0) {
        if (p0 > 0) {
            p0 = (s16)(p0 - 1);
            mProcVar0 = p0;
        }
        if (p0 == 0) {
            s32 judge = gabi::call<s32>(0x025E1F34 /* mDoAud_tact_judge */, (s32)mProcVar5, (s32)mProcVar3);
            mProcVar7 = judge;
            s16 p5 = (s16)(mProcVar5 + 1);
            mProcVar5 = p5;
            s32 nbeat = gabi::call<s32>(0x025E1EFC /* mDoAud_tact_getBeat */);
            u32 rf = resetFlg0();
            s32 song = mProcVar6;
            if (!(p5 < nbeat)) {
                mProcVar5 = 0;
            }
            setResetFlg0(rf | 0x1000000 /* daPyRFlg0_TACT_INPUT */);
            mProcVar0 = -1;
            BOOL match = FALSE;
            if (song >= 0) {
                match = (u32)song == (u32)mProcVar7;
            } else if (song == -1 || song == -3) {
                s32 j = mProcVar7;
                match = j >= 0 && j != 6 && j != 7 &&
                        gabi::call<BOOL>(0x025B7B10 /* dSv_player_collect_c::isTact */, gabi::load<u32>(0x101F84DC) + 0xD4, (u32)(u8)j);
            }
            if (match) {
                mProcVar1 = 0x1E;
                gabi::call(0x025E1988 /* seStartSystem */, 0x896 /* JA_SE_WTAKT_MATCH_SIGNAL */);
                m35A0 = 0.0f;
            } else {
                mProcVar7 = -1;
                if (mProcVar6 == -5) {
                    s16 m4 = (s16)(mProcVar4 + 1);
                    mProcVar4 = m4;
                    if (m4 == 6) {
                        mProcVar1 = 0xF;
                    }
                } else if (mProcVar6 == -2) {
                    s16 m4 = (s16)(mProcVar4 + 1);
                    mProcVar4 = m4;
                    if (m4 == 5) {
                        m35A0 = 0.0f;
                        mProcVar1 = 0x1E;
                    }
                }
            }
        }
    }
    mFrameCtrlUpper[2].setRate(1.0f); /* HD: HIO folded */
    if (mProcVar1 >= 0) {
        return TRUE;
    }
    /* the arm swing frame follows the global frame counter in beats */
    f32 two = (f32)(mDoAud_tact_getBeatFrames_l() * 2.0);
    u32 period;
    if (two < 2147483648.0f) {
        period = (u32)gabi::ftoi(two);
    } else {
        period = (u32)gabi::ftoi(two - 2147483648.0f) + 0x80000000u;
    }
    u32 frm = gabi::load<u32>(0x101FF560);
    f32 phase = (f32)(frm - ppc_divwu_l(frm, period) * period);
    f32 r = (f32)((f64)phase / (f64)(f32)(mDoAud_tact_getBeatFrames_l() * 2.0));
    mFrameCtrlUnder[0].setFrame(r * (f32)mFrameCtrlUnder[0].getEnd());
    mFrameCtrlUpper[1].setFrame(r * (f32)mFrameCtrlUpper[1].getEnd());
    gabi::store<f32>(gabi::load<u32>(self + 0x57FC) /* getNowAnmPackUnder(0) */, mFrameCtrlUnder[0].getFrame());
    u32 up = gabi::load<u32>(self + 0x582C); /* getNowAnmPackUpper(UPPER_MOVE1_e) */
    if (up != 0) {
        gabi::store<f32>(up, mFrameCtrlUpper[1].getFrame());
    }
    return TRUE;
}
VERIFY(0x0243A094, &daPy_lk_c::procTactWait);

enum : u32 {
    LK_procLand_init = 0x0241C714,
    LK_checkJumpRideShip = 0x0241C344,
};
/* clamp a GHS s16 accumulator to +-lim */
static inline s16 lk_clampS(s32 v, s32 lim) { return (s16)(v > lim ? lim : (v < -lim ? -lim : v)); }

/* 02438E78 */
BOOL daPy_lk_c::procFanGlide() {
    WWHD_FUNC(0x02438E78, BOOL, this);
    u32 self = gabi::ea(this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    if (mProcVar1 != 0) {
        mProcVar1 = (s16)(mProcVar1 - 1);
    }
    if (frameCtrl.checkPass(4.0f)) {
        dPa_control_c* pa = dComIfGp_getParticle();
        u32 em = gabi::ea(dPa_control_set(pa, 1, 0x4B /* ID_AK_JN_LEAFPARACHUTE00 */, &current.pos, nullptr, nullptr, 0xFF, nullptr, -1,
                                          nullptr, nullptr, nullptr));
        if (em != 0) {
            u32 mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 0xF /* CL_JNT_HEAD_JNT_e */);
            gabi::call(0x028249B0 /* JPASetRMtxTVecfromMtx */, mtx, em + 0x1F0, em + 0x22C); /* setGlobalRTMatrix() */
        }
    }
    if (frameCtrl.getRate() < 0.01f && mProcVar6 == 0 && gabi::load<u32>(self + 0x66FC) == 0 /* m32E4.getEmitter() */) {
        u32 mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 0xF);
        gabi::call(LK_mtxFollow_makeEmitter, &m32E4, 0x4C /* ID_AK_JN_LEAFPARACHUTE01 */, mtx, &current.pos, nullptr);
    }
    if (!(frameCtrl.getFrame() > 2.0f) && mProcVar6 == 0) {
        return TRUE;
    }
    if (LK_mAcchChkGroundHit()) {
        if (gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
            return gabi::call<BOOL>(LK_changeLandProc, this, 0.6f); /* HD: HIO folded */
        }
        return gabi::call<BOOL>(LK_procLand_init, this, 0.6f, 1);
    }
    if (gabi::call<BOOL>(LK_checkJumpFlower, this) || gabi::call<BOOL>(LK_checkJumpRideShip, this)) {
        return TRUE;
    }
    if (gabi::load<u32>(self + 0x6698) != 0 /* m3280.getEmitter() */) {
        if (mProcVar5 == 0) {
            seStartOnlyReverb(0x2848 /* JA_SE_LK_FAN_CHUTE_WATER */);
            mProcVar5 = 6;
        } else {
            mProcVar5 = (s16)(mProcVar5 - 1);
        }
    }
    if (mProcVar0 <= 0) {
        if (!gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
            maxFallSpeed = -2.0f; /* HD: HIO folded */
            if (mProcVar0 == 0) {
                mProcVar0 = 1;
                mNormalSpeed = 12.0f;
                speed.y = 15.0f;
                return TRUE;
            }
            speed.y = 15.0f;
        }
        mProcVar0 = 1;
        return TRUE;
    }
    if (!dComIfGp_event_runCheck_l()) {
        s32 m7 = mProcVar7 - 1;
        mProcVar7 = m7;
        if (m7 == 0 && gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x34) != 0 /* dComIfGs_getMagic() >= 1 */) {
            u32 a = dComIfGp_ea() + 0x5B60;
            gabi::store<s16>(a, (s16)(gabi::load<s16>(a) - 1)); /* dComIfGp_setItemMagicCount(-1) */
            mProcVar7 = 0x28;
        }
    }
    BOOL close = FALSE;
    if (mProcVar1 == 0) {
        dComIfGp_setDoStatus_l(0x27 /* dActStts_CANCEL_e */);
        if (mProcVar1 == 0 && (mItemTrigger & 3) /* doTrigger() || cancelTrigger() */) {
            close = TRUE;
        }
    }
    if (!close && gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x34) == 0 && mProcVar7 == 0) {
        close = TRUE;
    }
    if (close) {
        f32 x = current.pos.x, y = current.pos.y;
        m3688.x = x;
        f32 z = current.pos.z;
        m3688.y = y;
        m3688.z = z;
        m35F0 = y;
        seStartOnlyReverb(0x2847 /* JA_SE_LK_FAN_CHUTE_CLOSE */);
        if (mItemTrigger & 2 /* swordTrigger() */) {
            gabi::call(LK_setAnimeEquipSword, this, 0);
        }
        return gabi::call<BOOL>(LK_procFall_init, this, 2, 6.0f); /* HD: HIO folded */
    }
    if (speed.y < -gravity && !gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
        gravity = -0.5f;
        maxFallSpeed = -2.0f;
    }
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x34 /* dItemNo_DEKU_LEAF_e */, 1) && frameCtrl.getRate() < 0.01f && mProcVar6 == 0) {
        gabi::call(LK_setSingleMoveAnime, this, 0xA2 /* ANM_USEFANB */, 0.5f, 1.0f, 0xD, 0.0f);
        u32 res = gabi::call<u32>(LK_getItemAnimeResource, this, 0x84 /* FANBA */);
        gabi::call(0x025E4A98 /* mDoExt_McaMorf::setAnm */, gabi::load<u32>(self + 0x44D0) /* mpParachuteFanMorf */, res, 0, 0.0f, 0.5f,
                   1.0f, 13.0f, 0);
        gravity = -2.5f;
        LK_voiceStart(5);
        seStartOnlyReverb(0x2846 /* JA_SE_LK_FAN_CHUTE_RISE */);
        m353A = 5;
    }
    if (LK_mAcchChkWallHit()) {
        for (s32 i = 0; i < 3; i++) {
            u32 cir = self + 0x74C + i * 0x40; /* mAcchCir[i] */
            if ((gabi::load<u32>(cir + 0x10) & 2) /* ChkWallHit() */ &&
                cLib_distanceAngleS(current.angle.y, gabi::load<s16>(cir + 0x3C) /* GetWallAngleY() */) > 0x6000) {
                mNormalSpeed = -mNormalSpeed;
                break;
            }
        }
    }
    if (gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
        cLib_chaseF(&mNormalSpeed, 0.0f, 0.08000000566f); /* HD: HIO folded (0.1f * 0.8f) */
    }
    gabi::Local<be<f32>> windPow; /* sp+0x18 */
    gabi::Local<cXyz> v;          /* sp+0x28 (GameCube local_54) */
    gabi::Local<cXyz> windDir;    /* sp+0x34 (GameCube local_60) */
    gabi::call(0x0257E1B8 /* dKyw_get_AllWind_vec */, self + 0x3D8 /* &mHeadTopPos */, windDir.get(), windPow.get());
    s32 diff = (s16)(m34E8 - shape_angle.y);
    {
        f32 stick = mStickDistance;
        cLib_addCalc_l(&mNormalSpeed, (12.0f * stick) * cM_scos(diff), 0.5f, gabi::fmadds(0.4f, stick, 0.1f), 0.01f); /* HD: HIO folded */
        cLib_addCalcAngleS(&mProcVar2, (s16)gabi::ftoi((512.0f * mStickDistance) * cM_ssin(diff)), 5, 0x40, 8);
    }
    f32 step;
    if (gabi::call<BOOL>(0x025162A4 /* dCcD_GObjInf::ChkTgHit */, self + 0x778C /* mWindCyl */)) {
        lk_xyz_copy(v, gabi::at<cXyz>(self + 0x784C)); /* *mWindCyl.GetTgRVecP() */
        f64 len = gabi::call<f64>(0x028F4384 /* std::sqrtf */, PSVECSquareMag(v));
        if (len > 50.0) {
            PSVECScale(v, v, (f32)(50.0 / len));
        }
        step = 20.0f;
        *windPow = 1.0f;
        if (mProcVar6 == 0) {
            mProcVar6 = 1;
            gabi::call(LK_setSingleMoveAnime, this, 0xA3 /* ANM_USEFANB2 */, 1.0f, 0.0f, -1, 3.0f);
            u32 res = gabi::call<u32>(LK_getItemAnimeResource, this, 0x83 /* FANB2A */);
            gabi::call(0x025E4A98, gabi::load<u32>(self + 0x44D0), res, 0, 3.0f, 1.0f, 0.0f, -1.0f, 0);
        }
    } else {
        f32 p = *windPow * 1.5f;
        *windPow = p;
        if (!(p < 1.0f)) {
            p = 1.0f;
            *windPow = p;
        }
        f32 py = p * 0.5f;
        f32 pxz = p * 9.4f; /* HD: HIO folded */
        v->y = py * windDir->y;
        v->x = pxz * windDir->x;
        v->z = pxz * windDir->z;
        step = 3.0f;
        if (mProcVar6 != 0) {
            mProcVar6 = 0;
            gabi::call(LK_setSingleMoveAnime, this, 0xA2 /* ANM_USEFANB */, 0.5f, 12.999f, 0xD, 5.0f);
            u32 res = gabi::call<u32>(LK_getItemAnimeResource, this, 0x84 /* FANBA */);
            gabi::call(0x025E4A98, gabi::load<u32>(self + 0x44D0), res, 0, 5.0f, 0.5f, 12.999f, 13.0f, 0);
        }
    }
    cLib_addCalc_l(&m3730.x, v->x, 0.5f, step, 1.0f);
    cLib_addCalc_l(&m3730.y, v->y, 0.5f, step, 1.0f);
    cLib_addCalc_l(&m3730.z, v->z, 0.5f, step, 1.0f);
    PSVECAdd(&current.pos, &m3730, &current.pos);
    s16 a = (s16)(shape_angle.y + mProcVar2);
    shape_angle.y = a;
    current.angle.y = a;
    f32 s = cM_ssin(a), c = cM_scos(a);
    f32 mz = m3730.z, mx = m3730.x;
    f32 lx = gabi::fmsubs(mx, c, mz * s);
    f32 lz = gabi::fmadds(mx, s, mz * c);
    f32 f4 = -(lx * 0.02f);
    f32 f3 = lz * 0.02f;
    if (f4 > 1.0f) {
        f4 = 1.0f;
    } else if (f4 < -1.0f) {
        f4 = -1.0f;
    }
    if (f3 > 1.0f) {
        f3 = 1.0f;
    } else if (f3 < -1.0f) {
        f3 = -1.0f;
    }
    s16 targetX = (s16)gabi::ftoi(gabi::fmadds(mNormalSpeed, 0.02f, f3) * 6144.0f);
    s16 targetZ = (s16)gabi::ftoi(gabi::fmadds(f4, 6144.0f, (f32)(-(mProcVar2 * 8))));
    s16 prev = m34F2;
    cLib_addCalcAngleS(&m34F2, targetX, 5, 0x800, 0x100);
    s32 d = (s16)(m34F2 - prev);
    s32 m3 = mProcVar3;
    m34E0 = (s16)((m34E0 + m3) - d);
    mProcVar3 = lk_clampS((s16)(m3 + (d >> 1)), 0x380);
    cLib_addCalcAngleS(&m34E0, 0, 5, 0x80, 0x30);
    cLib_addCalcAngleS(&mProcVar3, 0, 5, 0x50, 0x30);
    if (m34E0 > 0x2000) {
        m34E0 = 0x2000;
    } else if (m34E0 < -0x2000) {
        m34E0 = -0x2000;
    }
    s16 prev2 = m34F4;
    cLib_addCalcAngleS(&m34F4, targetZ, 5, 0x800, 0x100);
    s32 d2 = (s16)(prev2 - m34F4);
    s32 m4 = mProcVar4;
    m34E4 = (s16)((m34E4 + m4) - d2);
    mProcVar4 = lk_clampS((s16)(m4 + d2), 0x380);
    cLib_addCalcAngleS(&m34E4, 0, 5, 0x80, 0x30);
    cLib_addCalcAngleS(&mProcVar4, 0, 5, 0x50, 0x30);
    if (m34E4 > 0x2000) {
        m34E4 = 0x2000;
    } else if (m34E4 < -0x2000) {
        m34E4 = -0x2000;
    }
    f32 pow = *windPow * 0.75f;
    f32 base = pow + 1.5f;
    *windPow = pow;
    f32 s1 = cM_ssin(m355C);
    f64 r1 = gabi::call<f64>(0x02019918 /* cM_rndFX */, 0.3f);
    m3600 = (f32)((f64)(f32)(base + r1) * (f64)s1);
    f32 s2 = cM_ssin(m355C);
    f64 r2 = gabi::call<f64>(0x02019918, 100.0f);
    m355E = (s16)gabi::ftoi((f32)((f64)(f32)(r2 + 600.0) * (f64)s2));
    f32 pw = *windPow;
    f64 r3 = gabi::call<f64>(0x02019788 /* cM_rnd */);
    f32 hi = gabi::fmadds(3000.0f, pw, 12000.0f);
    f32 lo = gabi::fmadds(2000.0f, pw, 6000.0f);
    f32 add = (f32)((f64)hi * r3 + (f64)lo);
    m355C = (s16)gabi::ftoi((f32)((f64)(f32)m355C + (f64)add));
    return TRUE;
}
VERIFY(0x02438E78, &daPy_lk_c::procFanGlide);

/* ---- rope swing (procRopeSwing and its HD turn check) ---- */
/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline s16 cM_rad2s_l(f32 x) { return gabi::call<s16>(0x02019510, x); }
static inline f32 cM_fsin_l(f32 x) { return cM_ssin((u16)cM_rad2s_l(x)); }
static inline f32 cM_fcos_l(f32 x) { return cM_scos((u16)cM_rad2s_l(x)); }
static inline BOOL cLib_chaseS_l(be<s16>* v, s16 target, s16 step) { return gabi::call<BOOL>(0x0200F564, v, target, step); }
static inline void mDoMtx_ZrotS_l(Mtx34* m, s16 z) { gabi::call(0x025F181C, m, z); }
static inline void mDoMtx_ZXYrotS_l(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F1AA4, m, x, y, z); }
/* strcmp(dComIfGp_getStartStageName(), str) == 0 through two sead::SafeString objects (HD) */
static inline BOOL lk_safeStageIs(u32 str) {
    gabi::Local<SafeString> name;
    gabi::Local<SafeString> stage;
    name->__vtbl = 0x10034B24;
    name->mStringTop = str;
    u32 play = dComIfGp_ea();
    stage->__vtbl = 0x10034B24;
    stage->mStringTop = play + 0x5134;
    gabi::call_ptr(gabi::load<u32>(name->__vtbl + 0x14), name.get());
    gabi::call_ptr(gabi::load<u32>(name->__vtbl + 0x14), name.get());
    u32 a = name->mStringTop;
    gabi::call_ptr(gabi::load<u32>(stage->__vtbl + 0x14), stage.get());
    u32 b = stage->mStringTop;
    if (a == b) {
        return TRUE;
    }
    u32 p = name->mStringTop;
    u32 q = stage->mStringTop;
    for (u32 n = 0x40001; n != 0; n--) {
        u8 c = gabi::load<u8>(p);
        if (c != gabi::load<u8>(q)) {
            return FALSE;
        }
        if (c == 0) {
            return TRUE;
        }
        p++;
        q++;
    }
    return FALSE;
}
/* m_pbCalc[PART_UNDER_e]->getRatio(1) (HD: per-joint table) */
static inline f32 lk_underRatio(u32 calc) {
    return gabi::load<f32>(gabi::load<u32>(gabi::load<u32>(calc + 0x7C) + 0x1C));
}
/* GHS function-static cXyz with its guard word */
static inline void lk_staticXyz(u32 guard, u32 v, f32 x, f32 y, f32 z) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        gabi::store<f32>(v, x);
        gabi::store<f32>(v + 4, y);
        gabi::store<f32>(v + 8, z);
    }
}
struct lk_xyz5 { cXyz v[5]; };

enum {
    LK_setBlendRopeMoveAnime = 0x0241CBB0,
};

/* 0243460C  HD-only: would turning the swing (yaw by m6922) hit a wall? Simulates the swing
 * position with the current swing values, sweeps two body-side rays from the rope along the
 * rope to the foot, restores everything and returns FALSE when the ray on the turn side hits. */
BOOL daPy_lk_c::checkRopeSwingTurn(cXyz* foot, f32 swingX, f32 swingZ) {
    WWHD_FUNC(0x0243460C, BOOL, this, foot, swingX, swingZ);
    s16 saveCurAngX = current.angle.x;
    s16 saveVar2 = mProcVar2;
    s16 saveShpX = shape_angle.x;
    f32 savePosX = current.pos.x;
    s16 saveCurAngY = current.angle.y;
    s16 saveCurAngZ = current.angle.z;
    s16 saveShpY = shape_angle.y;
    s16 saveShpZ = shape_angle.z;
    f32 savePosY = current.pos.y;
    f32 savePosZ = current.pos.z;
    s16 maxSwing = 12000;
    if (gabi::call<BOOL>(LK_checkSpecialRope, this)) {
        maxSwing = 1500;
    }
    if (mProcVar2 > maxSwing) {
        mProcVar2 = maxSwing;
    }
    f32 sx = cM_fsin_l(swingX);
    s16 rx = (s16)gabi::ftoi((f32)(-(s32)mProcVar2) * sx);
    f32 sz = cM_fsin_l(swingZ);
    s16 rz = (s16)gabi::ftoi((f32)(s32)mProcVar3 * sz);
    s16 target;
    if (mProcVar0 != 0) {
        f32 s = cM_fsin_l(swingX);
        f32 add = (f32)(s32)mProcVar2 * 0.5f * (f32)(s32)mProcVar0 * lk_underRatio(m_pbCalc[0]);
        target = (s16)gabi::ftoi(gabi::fmadds((f32)(-(s32)mProcVar2), s, add));
    } else {
        f32 s = cM_fsin_l(swingX - 0.62831855f);
        target = (s16)gabi::ftoi((f32)(-(s32)mProcVar2) * s);
    }
    cLib_addCalcAngleS(&shape_angle.x, target, 8, 0xC00, 0x100);
    gabi::Local<cXyz> down; /* sp+0x1C */
    down->x = 0.0f;
    down->y = -m35A0;
    down->z = 0.0f;
    mDoMtx_stack_c::transS(mRopePos.x, mRopePos.y, mRopePos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), rx, shape_angle.y, rz);
    PSMTXMultVec(mDoMtx_stack_c::get(), down, &current.pos);
    gabi::Local<lk_xyz5> line; /* sp+0x68: rope point, then 4 foot points along the swing */
    line->v[0].x = mRopePos.x;
    line->v[0].y = mRopePos.y - m35A0;
    line->v[0].z = mRopePos.z;
    f32 frx = (f32)rx;
    for (int i = 1; i <= 4; i++) {
        f32 t = (f32)i * 0.25f;
        gabi::Local<cXyz> p; /* sp+0x10 */
        mDoMtx_stack_c::transS(mRopePos.x, mRopePos.y, mRopePos.z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), (s16)gabi::ftoi(frx * t), shape_angle.y, rz);
        PSMTXMultVec(mDoMtx_stack_c::get(), down, p);
        mDoMtx_stack_c::transS(p->x, p->y, p->z);
        f32 s = cM_fsin_l(swingX + m35A4);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), (s16)gabi::ftoi((f32)(-(s32)mProcVar2) * s * t), shape_angle.y, rz);
        PSMTXMultVec(mDoMtx_stack_c::get(), foot, &line->v[i]);
    }
    lk_staticXyz(0x1046D22C, 0x1046CDD8, 0.0f, -105.0f, 0.0f);
    lk_staticXyz(0x1046D230, 0x1046CDE4, 15.0f, 0.0f, 0.0f);
    lk_staticXyz(0x1046D234, 0x1046CDF0, 30.0f, 0.0f, 0.0f);
    lk_staticXyz(0x1046D238, 0x1046CDFC, -30.0f, 0.0f, 0.0f);
    cXyz* body = gabi::at<cXyz>(0x1046CDD8);
    cXyz* foot2 = gabi::at<cXyz>(0x1046CDE4);
    cXyz* left = gabi::at<cXyz>(0x1046CDF0);
    cXyz* right = gabi::at<cXyz>(0x1046CDFC);
    gabi::Local<cXyz> l0, r0, l1, r1; /* sp+0x50, 0x5C, 0x28, 0x34 */
    cXyz_pl(body, l0, left);
    cXyz_pl(body, r0, right);
    cXyz_pl(foot2, l1, left);
    cXyz_pl(foot2, r1, right);
    mDoMtx_ZXYrotS_l(mDoMtx_stack_c::get(), 0, shape_angle.y, 0);
    gabi::Local<lk_xyz5> lp; /* sp+0xA4 */
    gabi::Local<lk_xyz5> rp; /* sp+0xE0 */
    PSMTXMultVec(mDoMtx_stack_c::get(), l0, &lp->v[0]);
    PSMTXMultVec(mDoMtx_stack_c::get(), r0, &rp->v[0]);
    for (int i = 1; i <= 4; i++) {
        PSMTXMultVec(mDoMtx_stack_c::get(), l1, &lp->v[i]);
        PSMTXMultVec(mDoMtx_stack_c::get(), r1, &rp->v[i]);
    }
    for (int i = 0; i < 5; i++) {
        PSVECAdd(&lp->v[i], &line->v[i], &lp->v[i]);
        PSVECAdd(&rp->v[i], &line->v[i], &rp->v[i]);
    }
    BOOL hitL = FALSE;
    BOOL hitR = FALSE;
    for (int i = 0; i < 4; i++) {
        dBgS_LinChk_Set(mLinkLinChk, &lp->v[i], &lp->v[i + 1], this);
        hitL |= cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk);
        dBgS_LinChk_Set(mLinkLinChk, &rp->v[i], &rp->v[i + 1], this);
        hitR |= cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk);
    }
    BOOL ret;
    if ((hitL && gabi::fmuls_ppc((f32)(s32)LK_FIELD(s16, 0x6922), m35A8) > 0.0f) ||
        (hitR && gabi::fmuls_ppc((f32)(s32)LK_FIELD(s16, 0x6922), m35A8) < 0.0f)) {
        ret = FALSE;
    } else {
        ret = TRUE;
    }
    shape_angle.x = saveShpX;
    shape_angle.y = saveShpY;
    shape_angle.z = saveShpZ;
    current.angle.x = saveCurAngX;
    current.angle.y = saveCurAngY;
    mProcVar2 = saveVar2;
    current.pos.y = savePosY;
    current.pos.z = savePosZ;
    current.pos.x = savePosX;
    current.angle.z = saveCurAngZ;
    return ret;
}
VERIFY(0x0243460C, &daPy_lk_c::checkRopeSwingTurn);

/* 02435580 */
BOOL daPy_lk_c::procRopeSwing() {
    WWHD_FUNC(0x02435580, BOOL, this);
    if (checkHangRopeActorNull()) {
        return TRUE;
    }
    gabi::Local<cXyz> toe; /* sp+0x38 */
    {
        u32 mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 0x28 /* CL_JNT_RTOE_JNT_e */);
        toe->x = gabi::load<f32>(mtx + 0x0C);
        toe->y = gabi::load<f32>(mtx + 0x1C);
        toe->z = gabi::load<f32>(mtx + 0x2C);
    }
    lk_xyz_copy(&mRopePos, &mActorKeepRope.mActor->current.pos);
    f32 swingX = gabi::fmuls_ppc(m35A8, m35A4);
    f32 swingZ = gabi::fmuls_ppc(m35AC, m35A4);
    mDoMtx_ZrotS_l(mDoMtx_stack_c::get(), (s16)-shape_angle.z);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), (s16)-shape_angle.x);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)-shape_angle.y);
    mDoMtx_stack_transM(-current.pos.x, -current.pos.y, -current.pos.z);
    gabi::Local<cXyz> localToe; /* sp+0x5C */
    PSMTXMultVec(mDoMtx_stack_c::get(), toe, localToe);

    f32 addX;
    f32 addZ;
    if (gabi::call<BOOL>(LK_checkSpecialRope, this)) {
        addX = gabi::fadds_ppc(0.0f, 1.0f);
        addZ = addX;
    } else {
        f32 fx;
        if (!(0.0f > swingX) && !(swingX > 1.5707964f)) {
            fx = gabi::fmuls_ppc(-0.45f, cM_fsin_l(swingX));
        } else if (!(swingX > -1.5707964f)) {
            fx = gabi::fmuls_ppc(0.45f, cM_fsin_l(swingX));
        } else {
            fx = 0.0f;
        }
        if (!(0.0f > swingZ) && !(swingZ > 1.5707964f)) {
            addZ = gabi::fmadds(-0.45f, cM_fsin_l(swingZ), 1.0f);
        } else if (!(swingZ > -1.5707964f)) {
            addZ = gabi::fmadds(0.45f, cM_fsin_l(swingZ), 1.0f);
        } else {
            addZ = gabi::fadds_ppc(0.0f, 1.0f);
        }
        addX = gabi::fadds_ppc(fx, 1.0f);
    }
    f32 scale = m35A4;
    m35A8 = gabi::fadds_ppc(m35A8, addX);
    f32 x = gabi::fmuls_ppc(m35A8, scale);
    m35AC = gabi::fadds_ppc(m35AC, addZ);
    swingZ = gabi::fmuls_ppc(m35AC, scale);
    gabi::Local<be<f32>> sp10; /* sp+0x10 */
    if (!(x < 3.1415927f)) {
        x = gabi::fsubs_ppc(x, 6.2831855f);
        *sp10 = x;
        m35A8 = x / scale;
    } else {
        *sp10 = x;
    }
    if (!(swingZ < 3.1415927f)) {
        swingZ = gabi::fsubs_ppc(swingZ, 6.2831855f);
        m35AC = swingZ / m35A4;
    }
    u32 mode = mModeFlg;
    if ((mode & 0x400) && mFrameCtrlUnder[0].getRate() < 0.01f) {
        mModeFlg = (mode & ~0x400u) | 0x100;
        gabi::call(LK_setTextureAnime, this, 8, 0);
    }
    gabi::call(LK_setBlendRopeMoveAnime, this, 0);

    s16 maxSwing;
    s16 chaseStep;
    s16 addStep;
    if (gabi::call<BOOL>(LK_checkSpecialRope, this)) {
        maxSwing = 1500; /* HD: HIO folded (12000 / 8) */
        chaseStep = 4;
        addStep = 8;
    } else {
        maxSwing = 12000;
        chaseStep = 0x20;
        addStep = 0x40;
    }
    /* HD: no swing input on the stage "ADMumi" */
    if (!lk_safeStageIs(0x1003613C /* "ADMumi" */)) {
        dComIfGp_setRStatus_l(0x13 /* dActStts_STOP_e */);
        /* HD: m6922 = sideways turn speed of the swing */
        if (std::abs((s32)LK_FIELD(s16, 0x6922)) > 0x10) {
            cLib_addCalcAngleS(&LK_FIELD(s16, 0x6922), 0, 5, 0x40, 5);
        } else {
            LK_FIELD(s16, 0x6922) = 0;
        }
        if (mItemButton & 0x40 /* spActionButton() */) {
            s16 step = (s16)(chaseStep * 10);
            cLib_chaseS_l(&mProcVar2, 0, step);
            cLib_chaseS_l(&mProcVar3, 0, step);
        } else {
            cLib_chaseS_l(&mProcVar3, 0, chaseStep);
            f32 absX = std::fabs((f32)*sp10);
            if (mStickDistance > 0.05f) {
                s32 dir = getDirectionFromAngle(m34DC);
                if (dir == 0 || dir == 1) {
                    f32 c = std::fabs(cM_fcos_l(absX));
                    mProcVar2 = (s16)gabi::ftoi(gabi::fmadds((f32)(s32)addStep, c, (f32)(s32)mProcVar2));
                }
                BOOL turning = FALSE;
                s16 target = 0;
                if (dir == 2 /* DIR_LEFT */) {
                    f32 stick = mStickDistance;
                    s16 lim = (s16)gabi::ftoi(170.0f * stick);
                    s16 cur = LK_FIELD(s16, 0x6922);
                    if (lim > cur) {
                        s16 v = (s16)(cur + gabi::ftoi(32.0f * stick));
                        if (v > lim) {
                            v = lim;
                        }
                        LK_FIELD(s16, 0x6922) = v;
                        target = v;
                    } else {
                        target = lim;
                    }
                    turning = TRUE;
                } else if (dir == 3 /* DIR_RIGHT */) {
                    f32 stick = mStickDistance;
                    s16 lim = (s16)gabi::ftoi(-170.0f * stick);
                    s16 cur = LK_FIELD(s16, 0x6922);
                    if (lim < cur) {
                        s16 v = (s16)(cur - gabi::ftoi(32.0f * stick));
                        if (v < lim) {
                            v = lim;
                        }
                        LK_FIELD(s16, 0x6922) = v;
                        target = v;
                    } else {
                        target = lim;
                    }
                    turning = TRUE;
                }
                if (!checkRopeSwingTurn(localToe, *sp10, swingZ)) {
                    LK_FIELD(s16, 0x6922) = 0;
                } else if (turning) {
                    cLib_addCalcAngleS(&LK_FIELD(s16, 0x6922), target, 3, 0x30, 0x10);
                    s16 sy = shape_angle.y;
                    f64 roof;
                    if (LK_FIELD(s16, 0x6922) > 0) {
                        roof = gabi::call<f64>(0x02433E0C /* checkRopeRoofHit */, this, (s32)(s16)(sy + 0x2000));
                    } else {
                        roof = gabi::call<f64>(0x02433E0C /* checkRopeRoofHit */, this, (s32)(s16)(sy - 0x2000));
                    }
                    if ((f32)(roof + -55.0) > current.pos.y) {
                        s16 y = (s16)(shape_angle.y + LK_FIELD(s16, 0x6922));
                        shape_angle.y = y;
                        current.angle.y = y;
                    }
                }
            } else {
                cLib_chaseS_l(&mProcVar2, 0, chaseStep);
            }
        }
    }
    if (mProcVar2 > maxSwing) {
        mProcVar2 = maxSwing;
    }

    gabi::Local<be<s16>> sp14; /* sp+0x14 */
    f32 sx = cM_fsin_l(*sp10);
    *sp14 = (s16)gabi::ftoi((f32)(-(s32)mProcVar2) * sx);
    f32 sz = cM_fsin_l(swingZ);
    s16 rz = (s16)gabi::ftoi((f32)(s32)mProcVar3 * sz);
    s16 target;
    if (mProcVar0 != 0) {
        f32 s = cM_fsin_l(*sp10);
        f32 add = (f32)(s32)mProcVar2 * 0.5f * (f32)(s32)mProcVar0 * lk_underRatio(m_pbCalc[0]);
        target = (s16)gabi::ftoi(gabi::fmadds((f32)(-(s32)mProcVar2), s, add));
    } else {
        f32 s = cM_fsin_l(*sp10 - 0.62831855f);
        target = (s16)gabi::ftoi((f32)(-(s32)mProcVar2) * s);
    }
    cLib_addCalcAngleS(&shape_angle.x, target, 8, 0xC00, 0x100);
    f32 c = cM_fcos_l(swingZ);
    f32 st = cM_fsin_l(swingZ - 1.0995574f);
    s16 tz = (s16)gabi::ftoi((f32)(s32)mProcVar3 * st);
    s16 maxStep = (s16)gabi::ftoi(gabi::fmadds(2048.0f, c, 1024.0f));
    s16 minStep = (s16)gabi::ftoi(gabi::fmadds(128.0f, c, 128.0f));
    cLib_addCalcAngleS(&shape_angle.z, tz, 8, maxStep, minStep);
    gabi::Local<cXyz> down; /* sp+0x44 */
    down->x = 0.0f;
    down->y = -m35A0;
    down->z = 0.0f;
    mDoMtx_stack_c::transS(mRopePos.x, mRopePos.y, mRopePos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), *sp14, shape_angle.y, rz);
    PSMTXMultVec(mDoMtx_stack_c::get(), down, &current.pos);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    f32 s2 = cM_fsin_l(gabi::fadds_ppc(*sp10, m35A4));
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), (s16)gabi::ftoi((f32)(-(s32)mProcVar2) * s2), shape_angle.y, rz);
    gabi::Local<cXyz> sp50; /* sp+0x50 */
    PSMTXMultVec(mDoMtx_stack_c::get(), localToe, sp50);
    if (!checkRopeSwingWall(toe, sp50, (s16*)sp14.get(), (f32*)sp10.get())) {
        lk_staticXyz(0x1046D23C, 0x1046CE08, 0.0f, -105.0f, 0.0f); /* local_height_offset */
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
        PSMTXMultVec(mDoMtx_stack_c::get(), gabi::at<cXyz>(0x1046CE08), sp50);
        if (!checkRopeSwingWall(&current.pos, sp50, (s16*)sp14.get(), (f32*)sp10.get())) {
            gabi::call(LK_resetFootEffect, this);
        }
    }

    f32 ratio = (f32)(s32)mProcVar2 / (f32)(s32)maxSwing;
    if (changeRopeEndProc(0)) {
        f32 r = m35A0 / 5000.0f; /* HD: HIO folded (100 * 50) */
        if (r < 1.0f) {
            r = 1.0f;
        }
        ratio = gabi::fmuls_ppc(ratio, r);
        BOOL slow = FALSE;
        /* HD: slower release on "Asoko" until event bit 0x520 */
        if (lk_safeStageIs(0x10036134 /* "Asoko" */) &&
            !dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), 0x520)) {
            slow = TRUE;
        }
        f32 a = *sp10;
        if (slow) {
            mNormalSpeed = gabi::fmuls_ppc(10.0f, ratio);
        } else {
            mNormalSpeed = gabi::fmuls_ppc(15.0f, ratio);
        }
        if (std::fabs(a) > 1.5707964f) {
            current.angle.y = (s16)(shape_angle.y + 0x8000);
        }
        speed.y = gabi::fmuls_ppc(30.0f, ratio);
        gabi::call(LK_procFall_init, this, 0, 6.0f); /* HD: HIO folded */
        setNoResetFlg1(noResetFlg1() | 0x8000000);
    } else if (mProcVar2 == 0 && mProcVar3 == 0) {
        procRopeHangWait_init(0);
    } else if (ratio > 0.6f) {
        if (cM_fcos_l(*sp10) > 0.77f) {
            mDoAud_seStart(0x201E /* JA_SE_LK_ROPE_SWING_F */, &mRopePos, 0, mReverb);
        }
    }
    return TRUE;
}
VERIFY(0x02435580, &daPy_lk_c::procRopeSwing);
