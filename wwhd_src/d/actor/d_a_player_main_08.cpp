/**
 * d_a_player_main_08.cpp (WWHD)
 * Player - Link (daPy_lk_c), address range #08 (0243F058..02444F20, the end of the translation
 * unit): the Boko-weapon and food procs, the sword procs (d_a_player_sword.inc), the tact
 * accessors, small accessors of d_a_player_main.cpp, __sinit_d_a_player_main_cpp and the per-TU
 * copies of inline constructors, destructors and accessors placed after it.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_player_main.cpp and its .inc files) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Functions of other ranges are called by address (gabi::call), not as C++ members.
 */
#include "d/actor/d_a_player_main.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dComIfGs: the save data object *(0x101F84DC) */
static inline u32 dComIfGs_base_l() { return gabi::load<u32>(0x101F84DC); }
/* play + 0x52B0: dComIfGp_event_getTalkXYBtn() */
static inline u8 dComIfGp_event_getTalkXYBtn_l() { return gabi::load<u8>(dComIfGp_ea() + 0x52B0); }
/* play + 0x5292: dComIfGp_event_runCheck() (event control mMode != 0) */
static inline BOOL dComIfGp_event_runCheck_l() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; }
/* 025B6DA0 dSv_player_bag_item_c::setBaitItemEmpty(u8 btn) on the save data + 0x96 */
static inline void dComIfGs_setReserveBaitEmpty_l(u32 btn) { gabi::call(0x025B6DA0, dComIfGs_base_l() + 0x96, btn); }
/* 024F8044 dCam_getBody(), 0253E860 dCamera_c::EndEventCamera(fpc_ProcID) */
static inline void dCam_EndEventCamera_l(fopAc_ac_c* a) {
    u32 cam = gabi::call<u32>(0x024F8044);
    gabi::call(0x0253E860, cam, gabi::load<u32>(gabi::ea(a) + 4) /* fopAcM_GetID */);
}

/* 0200ECD4 cLib_addCalc(f32* value, f32 target, f32 scale, f32 maxStep, f32 minStep) */
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}

/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline Mtx34* lk_getAnmMtx(J3DModel* m, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(m) + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jnt * 0x30);
}
/* save + 0x2E: the equipped sword; play + 0x5CEA: the HD sword state (checkNormalSwordEquip) */
static inline BOOL lk_checkNormalSwordEquip() {
    return gabi::load<u8>(dComIfGs_base_l() + 0x2E) == 0x38 /* dItemNo_SWORD_e */ || gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 2;
}

/* daPy_lk_c members in the opaque blocks of the shared header (offsets measured in this range) */
#define LK_FIELD(T, off) (*gabi::at<be<T>>(gabi::ea(this) + (off)))
#define mDemoType LK_FIELD(u16, 0x420) /* mDemo.getDemoType() (GameCube 0x304) */
#define mpEquipItemModel LK_FIELD(u32, 0x4440) /* J3DModel* (GameCube 0x2E68 area) */
#define mSwordTopPos (*gabi::at<cXyz>(gabi::ea(this) + 0x3E4)) /* daPy_py_c (GameCube 0x2C8) */
#define m69E8 LK_FIELD(u8, 0x69E8) /* HD-only byte, cleared when procCutTurn hands over */
#define mAtCylR LK_FIELD(f32, 0x79E0) /* mAtCyl.GetRP() */
#define mDemoMode LK_FIELD(s32, 0x430) /* mDemo.getDemoMode() (GameCube 0x314) */
#define mLinkLinChkPoly (gabi::ea(this) + 0x9E4) /* mLinkLinChk's cBgS_PolyInfo */
#define mMaxNormalSpeed LK_FIELD(f32, 0x3C4) /* daPy_py_c (GameCube 0x2A8) */
#define mUpperAnmTransform LK_FIELD(u32, 0x581C) /* mAnmRatioUpper[UPPER_MOVE0_e].getAnmTransform() */
#define mLeftHandPos (*gabi::at<cXyz>(gabi::ea(this) + 0x3F0)) /* daPy_py_c (GameCube 0x2D4) */

/* ---- daPy_lk_c functions of other ranges (by address) ---- */
enum : u32 {
    LK_checkNextMode = 0x023F14E0,
    LK_procWeaponFrontSwing_init = 0x0243EF08,
    LK_setShapeAngleToAtnActor = 0x02419C70,
    LK_commonProcInit = 0x023DFDD8,
    LK_setSingleMoveAnime = 0x023E0A04,
    LK_setEnemyWeaponAtParam = 0x023ED2CC,
    LK_changeCutReverseProc = 0x0243829C,
    LK_seStartSwordCut = 0x024072F4,
    LK_deleteEquipItem = 0x023DC7AC,
    LK_setShipRidePosUseItem = 0x023E2DC4,
    LK_endDemoMode = 0x023F2048,
    LK_setDamagePoint = 0x023F51D0,
    LK_loadTextureAnimeResource = 0x023DC38C, /* unnamed by the matcher */
    LK_setTextureAnimeResource = 0x023DC110, /* unnamed by the matcher */
    LK_loadTextureScrollResource = 0x023DC4DC, /* unnamed by the matcher */
    LK_setTextureScrollResource = 0x023DC43C,
    LK_setDemoTextureAnime = 0x0241FCB4,
    LK_setSpecialBattle = 0x023EC370,
    LK_changeSpecialBattle = 0x023E5400,
    LK_setTextureAnime = 0x023DD768,
    LK_changeLandProc = 0x0241C898,
    LK_setFallVoice = 0x0241D6CC,
    LK_mtxFollow_end = 0x023D4538, /* daPy_mtxFollowEcallBack_c::end */
    LK_mtxFollow_makeEmitter = 0x023D457C,
    LK_endFlameDamageEmitter = 0x023E4E2C,
    LK_procFrontRollCrash_init = 0x0241B904,
    LK_procWait_init = 0x023E2FF4,
    LK_changeSlideProc = 0x023E4CD0,
    LK_getDirectionFromAngle = 0x023E3358,
    LK_setNormalSpeedF = 0x02416230,
    LK_initSeAnime = 0x023E0660,
    LK_procCutTurn_init = 0x023E6380,
    LK_getBlurTopRate = 0x02407178,
    LK_getSwordBlurColor = 0x02407270,
    LK_setJumpCutAtParam = 0x023ED380,
    LK_setHammerQuake = 0x0243BFC8,
    LK_checkHeavyStateOn = 0x023DBC24,
    LK_HD_023ED2B0 = 0x023ED2B0, /* unnamed by the matcher: procJumpCut calls it (this, 0) when it latches the spin-attack follow-up */
    LK_resetFootEffect = 0x023DF9C0,
    LK_checkBoomerangAnime = 0x023D69F8,
};
#define LK_checkNextMode(self, n) gabi::call<BOOL>(LK_checkNextMode, self, n)
/* virtual daPy_lk_c::voiceStart(u32) (vtable slot 0xE4) */
#define LK_voiceStart(n) gabi::call_ptr(gabi::load<u32>(__vtbl + 0xE4), this, (u32)(n))
#define LK_cutEnd() dComIfGp_evmng_cutEnd(mStaffIdx)

/* ---- d_a_player_weapon.inc ---- */

/* 0243F058 */
BOOL daPy_lk_c::procWeaponFrontSwingReady() {
    WWHD_FUNC(0x0243F058, BOOL, this);
    if (mActorKeepEquip.mActor == nullptr) {
        return gabi::call<BOOL>(LK_checkNextMode, this, 0);
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        return gabi::call<BOOL>(LK_procWeaponFrontSwing_init, this);
    }
    if (mpAttnActorLockOn != nullptr) {
        gabi::call(LK_setShapeAngleToAtnActor, this);
        current.angle.y = shape_angle.y;
        mProcVar2 = shape_angle.y;
    } else {
        cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 30, 0x3CDF, 0x1F40); /* HD: HIO folded */
        current.angle.y = shape_angle.y;
    }
    return TRUE;
}
VERIFY(0x0243F058, &daPy_lk_c::procWeaponFrontSwingReady);

/* 0243F12C */
BOOL daPy_lk_c::procWeaponFrontSwingEnd_init() {
    WWHD_FUNC(0x0243F12C, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x4F /* daPyProc_WEAPON_FRONT_SWING_END_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0xAD /* ANM_HAMSWINGBEND */, 0.6f, 5.0f, 8, 10.0f); /* HD: HIO folded */
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    gabi::call(LK_setEnemyWeaponAtParam, this, 0);
    return TRUE;
}
VERIFY(0x0243F12C, &daPy_lk_c::procWeaponFrontSwingEnd_init);

/* 0243F1A8 */
BOOL daPy_lk_c::procWeaponFrontSwing() {
    WWHD_FUNC(0x0243F1A8, BOOL, this);
    if (mActorKeepEquip.mActor == nullptr) {
        return gabi::call<BOOL>(LK_checkNextMode, this, 0);
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        return procWeaponFrontSwingEnd_init();
    }
    if (gabi::call<BOOL>(LK_changeCutReverseProc, this, 0x15 /* ANM_CUTREL */)) {
        return TRUE;
    }
    if (mpAttnActorLockOn != nullptr) {
        gabi::call(LK_setShapeAngleToAtnActor, this);
        current.angle.y = shape_angle.y;
        mProcVar2 = shape_angle.y;
    } else {
        cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 30, 0x3CDF, 0x1F40);
        current.angle.y = shape_angle.y;
    }
    setResetFlg0(resetFlg0() | 2); /* onResetFlg0(daPyRFlg0_UNK2) */
    return TRUE;
}
VERIFY(0x0243F1A8, &daPy_lk_c::procWeaponFrontSwing);

/* 0243F2B0 */
BOOL daPy_lk_c::procWeaponFrontSwingEnd() {
    WWHD_FUNC(0x0243F2B0, BOOL, this);
    if ((mItemButton & 2) /* swordButton() */ && mActorKeepEquip.mActor != nullptr) {
        mNoResetFlg0 = mNoResetFlg0 | 4;
    } else {
        mNoResetFlg0 = mNoResetFlg0 & ~4u;
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f || mActorKeepEquip.mActor == nullptr) {
        return gabi::call<BOOL>(LK_checkNextMode, this, 0);
    }
    if (mFrameCtrlUnder[0].getFrame() > 8.0f /* HD: HIO folded */) {
        return gabi::call<BOOL>(LK_checkNextMode, this, 1);
    }
    return TRUE;
}
VERIFY(0x0243F2B0, &daPy_lk_c::procWeaponFrontSwingEnd);

/* 0243F34C */
BOOL daPy_lk_c::procWeaponThrow() {
    WWHD_FUNC(0x0243F34C, BOOL, this);
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[0];
    if (frameCtrl->getRate() < 0.01f || mActorKeepEquip.mActor == nullptr) {
        return gabi::call<BOOL>(LK_checkNextMode, this, 0);
    }
    if (frameCtrl->getFrame() > 100.0f) {
        return gabi::call<BOOL>(LK_checkNextMode, this, 1);
    }
    if (frameCtrl->checkPass(10.0f)) {
        fopAc_ac_c* lockOn = mpAttnActorLockOn;
        fopAc_ac_c* boko = mActorKeepEquip.mActor;
        s16 angle = (s16)0xF400;
        if (lockOn != nullptr) {
            gabi::Local<cXyz> d;
            cXyz_mi(&lockOn->eyePos, d, &boko->current.pos);
            f64 dist = gabi::call<f64>(0x028F4384 /* std::sqrtf */, gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, d.get()));
            if (!(dist < 1.0f)) {
                gabi::Local<cXyz> xz;
                xz->x = d->x;
                xz->y = 0.0f;
                xz->z = d->z;
                f64 h = gabi::call<f64>(0x028F4384, gabi::call<f64>(0x028E8DD0, xz.get()));
                angle = gabi::call<s16>(0x020195B0 /* cM_atan2s */, -d->y, h);
            }
        }
        /* daBoko_c::setThrow(angle) */
        gabi::store<s16>(gabi::ea(boko) + 0x442, angle);
        gabi::store<u8>(gabi::ea(boko) + 0x432, 1);
        LK_voiceStart(0);
        gabi::call(LK_seStartSwordCut, this, 0x2800); /* HD: an extra sound */
        gabi::call(LK_deleteEquipItem, this, 0);
    }
    return TRUE;
}
VERIFY(0x0243F34C, &daPy_lk_c::procWeaponThrow);

/* ---- d_a_player_food.inc ---- */

/* 0243F4DC */
BOOL daPy_lk_c::procFoodThrow() {
    WWHD_FUNC(0x0243F4DC, BOOL, this);
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[0];
    gabi::call(LK_setShipRidePosUseItem, this);
    if (frameCtrl->getRate() < 0.01f) {
        if (mDemoType != 5 /* !checkSpecialDemoMode() */) {
            LK_cutEnd();
        } else {
            dComIfGp_event_reset();
            dCam_EndEventCamera_l(this);
            gabi::call(LK_endDemoMode, this);
        }
    } else if (frameCtrl->checkPass(8.0f)) {
        seStartOnlyReverb(0x2824 /* JA_SE_LK_ESA_THROW */);
        fopAc_ac_c* esa = gabi::call<fopAc_ac_c*>(0x025D5928 /* fopAcM_fastCreate */, 0xDD /* ESA */, 0xFFFF000Cu,
                                                  &mLeftHandPos, (s32)current.roomNo, &shape_angle, nullptr, -1, 0, 0);
        if (esa != nullptr) {
            m3630 = esa != nullptr ? gabi::load<u32>(gabi::ea(esa) + 4) : 0xFFFFFFFFu;
            if (mDemoType != 5) {
                if (dComIfGp_event_getTalkXYBtn_l() == 1 /* dTalkBtn_X_e */) {
                    dComIfGs_setReserveBaitEmpty_l(0);
                } else if (dComIfGp_event_getTalkXYBtn_l() == 2 /* dTalkBtn_Y_e */) {
                    dComIfGs_setReserveBaitEmpty_l(1);
                } else {
                    dComIfGs_setReserveBaitEmpty_l(2);
                }
            } else {
                dComIfGs_setReserveBaitEmpty_l(mReadyItemBtn);
            }
        }
    }
    return TRUE;
}
VERIFY(0x0243F4DC, &daPy_lk_c::procFoodThrow);

/* 0243F670 */
BOOL daPy_lk_c::procFoodSet() {
    WWHD_FUNC(0x0243F670, BOOL, this);
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[0];
    gabi::call(LK_setShipRidePosUseItem, this);
    if (frameCtrl->getRate() > 0.0f && frameCtrl->checkPass(9.0f)) {
        seStartOnlyReverb(0x2885 /* JA_SE_LK_HYOI_SET */);
    }
    if (std::fabs(frameCtrl->getRate()) < 0.01f) {
        if (mDemoType != 5) {
            LK_cutEnd();
        } else if (mProcVar0 > 0) {
            mProcVar0 = mProcVar0 - 1;
        } else if (mProcVar0 < 0) {
            dComIfGp_event_reset();
            gabi::call(LK_deleteEquipItem, this, 0);
            dCam_EndEventCamera_l(this);
            gabi::call(LK_endDemoMode, this);
        } else {
            mProcVar0 = -1;
            frameCtrl->setRate(-1.1f); /* HD: -HIO folded */
        }
    }
    return TRUE;
}
VERIFY(0x0243F670, &daPy_lk_c::procFoodSet);

/* ---- d_a_player_sword.inc ---- */

/* daPy_dmEcallBack_c::checkCurse(): the static curse word 101CEF16 */
static inline BOOL lk_checkCurse() { return gabi::load<u16>(0x101CEF16) == 1; }
/* play + 0x5CD8 / 0x5CDC: dComIfGp_setPlayerStatus0/1(0, flag) */
static inline void lk_onPlayerStatus(u32 off, u32 flag) {
    u32 a = dComIfGp_ea() + off;
    gabi::store<u32>(a, gabi::load<u32>(a) | flag);
}
/* mEquipItem == daPyItem_BOKO_e: hold the upper body animation at its last frame */
#define LK_bokoUpperEnd() do { \
        if (mEquipItem == 0x101 /* daPyItem_BOKO_e */ && mUpperAnmTransform != 0) { \
            mFrameCtrlUpper[0].setFrame((f32)mFrameCtrlUpper[0].getEnd() - 0.001f); \
            gabi::store<f32>(mUpperAnmTransform, mFrameCtrlUpper[0].getFrame()); /* HD: J3DAnmBase frame at +0 */ \
        } \
    } while (0)

/* play + 0x5BB7: dComIfGp_getDoStatus() */
static inline u8 dComIfGp_getDoStatus_l() { return gabi::load<u8>(dComIfGp_ea() + 0x5BB7); }

/* HD: the J3DAnm frames of mpCutfBpk/mpCutfBtk became HD animation objects: the frame (f32 at
 * frameAt), a mirrored frame (through the pointer at ptrAt) and an evaluator object at evAt (whose
 * first word points to {value, a, b, -, fn(ctx, frame, a, b), ctx}; then 027DF40C(evAt)) */
static inline void lk_anmSetFrame(u32 frameAt, u32 ptrAt, u32 evAt, f32 frame) {
    gabi::store<f32>(frameAt, frame);
    gabi::store<f32>(gabi::load<u32>(ptrAt), frame);
    u32 ev = gabi::load<u32>(evAt);
    f32 v = gabi::call_ptr<f32>(gabi::load<u32>(ev + 0x10), gabi::load<u32>(ev + 0x14), frame, gabi::load<f32>(ev + 4), gabi::load<f32>(ev + 8));
    gabi::store<f32>(ev, v);
    gabi::call(0x027DF40C, evAt);
}

/* doTrigger() && dComIfGp_getDoStatus() == dActStts_PARRY_e */
static inline bool lk_checkParry(daPy_lk_c* self) {
    return (self->mItemTrigger & 1) && dComIfGp_getDoStatus_l() == 0x1A;
}
/* frameCtrl.checkPass(passFrame): the speed kick fabs(speedF) * 0.2 + add (HD: HIO folded) */
static inline void lk_cutKick(daPy_lk_c* self, J3DFrameCtrl* frameCtrl, f32 passFrame, f32 add) {
    if (frameCtrl->checkPass(passFrame)) {
        self->mNormalSpeed = gabi::fmadds(std::fabs((f32)self->speedF), 0.2f, add);
    }
}
/* the attack window: the swing sound once, onResetFlg0(daPyRFlg0_UNK2) */
static inline void lk_cutAt(daPy_lk_c* self, J3DFrameCtrl* frameCtrl, f32 start, f32 end) {
    if (frameCtrl->getFrame() >= start && frameCtrl->getFrame() < end) {
        if (!(self->mNoResetFlg0 & 0x40) /* !checkNoResetFlg0(daPyFlg0_CUT_AT_FLG) */) {
            self->setResetFlg0(self->resetFlg0() | 1);
            self->seStartSwordCut(0x2800 /* JA_SE_LK_SW_KAZEKIRI_S */);
        }
        self->setResetFlg0(self->resetFlg0() | 2);
    }
}

/* the common body of procCutA/F/R/L (GameCube: four copies with their own HIO block; HD: HIO folded) */
struct lk_cutParam {
    u8 dir;
    u8 nextCut;     /* m34C5 on a sword trigger */
    f32 endFrame;   /* checkNextMode(1) after this frame */
    f32 passFrame;  /* speed kick */
    f32 kickAdd;    /* fabs(speedF) * 0.2 + kickAdd */
    f32 atStart, atEnd;
    f32 calcMax;    /* cLib_addCalc max step */
    u16 reverseAnm; /* changeCutReverseProc */
};
static inline BOOL lk_procCutCommon(daPy_lk_c* self, J3DFrameCtrl* frameCtrl, const lk_cutParam& p) {
    self->m3522 = 2;
    if (frameCtrl->getRate() < 0.01f) {
        self->mNormalSpeed = 0.0f;
        self->mDirection = p.dir;
        LK_checkNextMode(self, 0);
        return TRUE;
    }
    if (frameCtrl->getFrame() > p.endFrame) {
        f32 speed = self->mNormalSpeed;
        u8 dir = self->mDirection;
        self->mNormalSpeed = 0.0f;
        self->mDirection = p.dir;
        if (LK_checkNextMode(self, 1)) {
            return TRUE;
        }
        self->mDirection = dir;
        self->mNormalSpeed = speed;
    }
    if (gabi::call<BOOL>(LK_changeCutReverseProc, self, (u32)p.reverseAnm)) {
        return TRUE;
    }
    gabi::call(LK_setSpecialBattle, self, 1);
    if (lk_checkParry(self)) {
        return gabi::call<BOOL>(LK_changeSpecialBattle, self);
    }
    if ((self->mItemTrigger & 2) /* swordTrigger() */ && self->m34C5 != 5) {
        self->m34C5 = p.nextCut;
    }
    self->m34C2 = 1;
    if (self->mpAttnActorLockOn != nullptr) {
        gabi::call(LK_setShapeAngleToAtnActor, self);
        self->mProcVar2 = self->shape_angle.y;
    } else {
        cLib_addCalcAngleS(&self->shape_angle.y, self->mProcVar2, 30, 0x3CDF, 0x1F40);
    }
    self->current.angle.y = self->shape_angle.y;
    lk_cutKick(self, frameCtrl, p.passFrame, p.kickAdd);
    lk_cutAt(self, frameCtrl, p.atStart, p.atEnd);
    cLib_addCalc(&self->mNormalSpeed, 0.0f, 0.7f, p.calcMax, 0.5f);
    return TRUE;
}
/* swordButton() && m34C5 == 0: on/offNoResetFlg0(daPyFlg0_UNK4) */
#define LK_cutHoldFlag() do { \
        if ((mItemButton & 2) && m34C5 == 0) mNoResetFlg0 = mNoResetFlg0 | 4; \
        else mNoResetFlg0 = mNoResetFlg0 & ~4u; \
    } while (0)

/* 0243F790 */
BOOL daPy_lk_c::procCutA() {
    WWHD_FUNC(0x0243F790, BOOL, this);
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[0];
    m35EC = frameCtrl->getFrame();
    LK_cutHoldFlag();
    static const lk_cutParam p = {3 /* DIR_RIGHT */, 1, 16.0f, 6.0f, 10.0f, 5.0f, 11.0f, 2.6f, 0x27 /* ANM_CUTRER */};
    return lk_procCutCommon(this, frameCtrl, p);
}
VERIFY(0x0243F790, &daPy_lk_c::procCutA);

/* 0243FAFC */
BOOL daPy_lk_c::procCutF() {
    WWHD_FUNC(0x0243FAFC, BOOL, this);
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[0];
    m35EC = frameCtrl->getFrame();
    LK_cutHoldFlag();
    lk_anmSetFrame(gabi::ea(this) + 0x4984, gabi::ea(this) + 0x4990, gabi::ea(this) + 0x4998, frameCtrl->getFrame()); /* mpCutfBpk->setFrame */
    lk_anmSetFrame(gabi::ea(this) + 0x49F8, gabi::ea(this) + 0x4A5C, gabi::ea(this) + 0x4A04, frameCtrl->getFrame()); /* mpCutfBtk->setFrame */
    static const lk_cutParam p = {3 /* DIR_RIGHT */, 2, 17.0f, 6.0f, 8.0f, 5.0f, 12.0f, 0.95f, 0x27};
    return lk_procCutCommon(this, frameCtrl, p);
}
VERIFY(0x0243FAFC, &daPy_lk_c::procCutF);

/* 0243FF44 */
BOOL daPy_lk_c::procCutR() {
    WWHD_FUNC(0x0243FF44, BOOL, this);
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[0];
    m35EC = frameCtrl->getFrame();
    LK_cutHoldFlag();
    static const lk_cutParam p = {3 /* DIR_RIGHT */, 3, 16.0f, 6.0f, 1.0f, 6.0f, 12.0f, 0.95f, 0x27};
    return lk_procCutCommon(this, frameCtrl, p);
}
VERIFY(0x0243FF44, &daPy_lk_c::procCutR);

/* 024402B0 */
BOOL daPy_lk_c::procCutL() {
    WWHD_FUNC(0x024402B0, BOOL, this);
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[0];
    m35EC = frameCtrl->getFrame();
    LK_cutHoldFlag();
    static const lk_cutParam p = {2 /* DIR_LEFT */, 4, 16.0f, 6.0f, 1.0f, 5.0f, 10.0f, 0.95f, 0x15 /* ANM_CUTREL */};
    return lk_procCutCommon(this, frameCtrl, p);
}
VERIFY(0x024402B0, &daPy_lk_c::procCutL);

/* procCutEA/EB: the end of the rate countdown (mProcVar0 extra frames) */
static inline BOOL lk_procCutE(daPy_lk_c* self, J3DFrameCtrl* frameCtrl, u8 dir, u32 reverseAnm, bool lockOnOnly, f32 kickAdd, f32 calcMax) {
    if (frameCtrl->getRate() < 0.01f) {
        if (self->mProcVar0 > 0) {
            self->mProcVar0 = self->mProcVar0 - 1;
        } else {
            self->mNormalSpeed = 0.0f;
            self->mDirection = dir;
            LK_checkNextMode(self, 0);
            return TRUE;
        }
    }
    if (gabi::call<BOOL>(LK_changeCutReverseProc, self, reverseAnm)) {
        return TRUE;
    }
    if (!lockOnOnly || self->mpAttnActorLockOn != nullptr) {
        gabi::call(LK_setShapeAngleToAtnActor, self);
        self->current.angle.y = self->shape_angle.y;
    }
    gabi::call(LK_setSpecialBattle, self, 1);
    if (lk_checkParry(self)) {
        return gabi::call<BOOL>(LK_changeSpecialBattle, self);
    }
    lk_cutKick(self, frameCtrl, 6.0f, kickAdd);
    lk_cutAt(self, frameCtrl, 5.0f, 11.0f);
    self->m34C2 = 1;
    cLib_addCalc(&self->mNormalSpeed, 0.0f, 0.7f, calcMax, 0.5f);
    return TRUE;
}

/* 0244061C */
BOOL daPy_lk_c::procCutEA() {
    WWHD_FUNC(0x0244061C, BOOL, this);
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[0];
    m35EC = frameCtrl->getFrame();
    LK_cutHoldFlag();
    return lk_procCutE(this, frameCtrl, 3 /* DIR_RIGHT */, 0x15 /* ANM_CUTREL */, true, 15.0f, 4.0f);
}
VERIFY(0x0244061C, &daPy_lk_c::procCutEA);

/* 024408B0 */
BOOL daPy_lk_c::procCutEB() {
    WWHD_FUNC(0x024408B0, BOOL, this);
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[0];
    m35EC = frameCtrl->getFrame();
    LK_cutHoldFlag();
    return lk_procCutE(this, frameCtrl, 2 /* DIR_LEFT */, 0x27 /* ANM_CUTRER */, false, 7.0f, 1.5f);
}
VERIFY(0x024408B0, &daPy_lk_c::procCutEB);

/* procCutExA/Kesa: the rate end, the 0x80 mode flag at passFrame, the sword trigger to nextCut */
static inline BOOL lk_procCutExA(daPy_lk_c* self, J3DFrameCtrl* frameCtrl, s16 m3522, f32 endFrame, f32 passFrame, u8 nextCut,
                                 f32 atStart, f32 atEnd) {
    self->m3522 = m3522;
    if (frameCtrl->getRate() < 0.01f) {
        self->mNormalSpeed = 0.0f;
        LK_checkNextMode(self, 0);
        return TRUE;
    }
    if (frameCtrl->getFrame() > endFrame) {
        f32 speed = self->mNormalSpeed;
        self->mNormalSpeed = 0.0f;
        if (LK_checkNextMode(self, 1)) {
            return TRUE;
        }
        self->mNormalSpeed = speed;
    } else if (frameCtrl->checkPass(passFrame)) {
        self->mModeFlg = self->mModeFlg | 0x80; /* onModeFlg(ModeFlg_00000080) */
    }
    if (gabi::call<BOOL>(LK_changeCutReverseProc, self, 0x15 /* ANM_CUTREL */)) {
        return TRUE;
    }
    gabi::call(LK_setSpecialBattle, self, 1);
    if (lk_checkParry(self)) {
        return gabi::call<BOOL>(LK_changeSpecialBattle, self);
    }
    if ((self->mItemTrigger & 2) && self->m34C5 != 5) {
        self->m34C5 = nextCut;
    }
    if (self->mpAttnActorLockOn != nullptr) {
        gabi::call(LK_setShapeAngleToAtnActor, self);
        self->current.angle.y = self->shape_angle.y;
    }
    lk_cutAt(self, frameCtrl, atStart, atEnd);
    self->m34C2 = 1;
    cLib_addCalc(&self->mNormalSpeed, 0.0f, 0.7f, 4.0f, 0.5f);
    return TRUE;
}

/* 02440B38 */
BOOL daPy_lk_c::procCutExA() {
    WWHD_FUNC(0x02440B38, BOOL, this);
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[0];
    m35EC = frameCtrl->getFrame();
    LK_cutHoldFlag();
    return lk_procCutExA(this, frameCtrl, 1, 18.0f, 11.0f, 26, 12.0f, 18.0f);
}
VERIFY(0x02440B38, &daPy_lk_c::procCutExA);

/* 02440E5C */
BOOL daPy_lk_c::procCutExB() {
    WWHD_FUNC(0x02440E5C, BOOL, this);
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[0];
    m35EC = frameCtrl->getFrame();
    LK_cutHoldFlag();
    if (frameCtrl->getRate() < 0.01f) {
        if (mProcVar0 > 0) {
            mProcVar0 = mProcVar0 - 1;
        } else {
            mNormalSpeed = 0.0f;
            LK_checkNextMode(this, 0);
            return TRUE;
        }
    } else if (frameCtrl->checkPass(14.0f)) {
        mModeFlg = mModeFlg | 0x80;
    }
    if (gabi::call<BOOL>(LK_changeCutReverseProc, this, 0x15 /* ANM_CUTREL */)) {
        return TRUE;
    }
    if (mpAttnActorLockOn != nullptr) {
        gabi::call(LK_setShapeAngleToAtnActor, this);
        current.angle.y = shape_angle.y;
    }
    gabi::call(LK_setSpecialBattle, this, 1);
    if (lk_checkParry(this)) {
        return gabi::call<BOOL>(LK_changeSpecialBattle, this);
    }
    lk_cutAt(this, frameCtrl, 14.0f, 20.0f);
    m34C2 = 1;
    cLib_addCalc(&mNormalSpeed, 0.0f, 0.7f, 4.0f, 0.5f);
    return TRUE;
}
VERIFY(0x02440E5C, &daPy_lk_c::procCutExB);

/* 02441104 */
BOOL daPy_lk_c::procCutExMJ() {
    WWHD_FUNC(0x02441104, BOOL, this);
    m35EC = mFrameCtrlUnder[0].getFrame();
    LK_cutHoldFlag();
    if (m34C4 != 0) {
        m3522 = 2;
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        if (mProcVar2 == 0) {
            mProcVar2 = 1;
        } else if (mProcVar2 == 1) {
            gabi::call(LK_setSingleMoveAnime, this, 0xD /* ANM_JMPEDS */, 0.0f, 0.0f, -1, 30.0f);
            mModeFlg = mModeFlg & ~0x400u; /* offModeFlg(ModeFlg_00000400) */
            gabi::call(LK_setTextureAnime, this, 0x37, 0);
            resetSeAnime();
            mProcVar2 = 2;
        }
        if (mProcVar3 == 0x71) {
            if (!(m34EC > 0) || m34EC > 0x4000) {
                m34EC = m34EC - 0x1800;
            } else {
                cLib_addCalcAngleS(&m34EC, 1, 2, 0x1800, 0x800);
            }
        } else if (!(m34EC < 0) || m34EC < -0x4000) {
            m34EC = m34EC + 0x1800;
        } else {
            cLib_addCalcAngleS(&m34EC, -1, 2, 0x1800, 0x800);
        }
        if (mAcch.ChkGroundHit()) {
            return gabi::call<BOOL>(LK_changeLandProc, this, 1.3f); /* HD: HIO folded */
        }
    }
    if (mpAttnActorLockOn != nullptr) {
        gabi::call(LK_setShapeAngleToAtnActor, this);
        current.angle.y = shape_angle.y;
    }
    gabi::call(LK_setSpecialBattle, this, 1);
    if (lk_checkParry(this)) {
        return gabi::call<BOOL>(LK_changeSpecialBattle, this);
    }
    if (m34C4 != 0 && ((mItemTrigger & 2) && m34C5 != 5)) {
        m34C5 = 30;
    }
    if (speed.y < -gravity) {
        gravity = -1.7f;
    }
    gabi::call(LK_setFallVoice, this);
    if (mProcVar2 != 2 && !(mFrameCtrlUnder[0].getFrame() < m35A0)) { /* NaN: in the window */
        if (!(mNoResetFlg0 & 0x40)) {
            setResetFlg0(resetFlg0() | 1);
            seStartSwordCut(0x2800);
        }
        setResetFlg0(resetFlg0() | 2);
    }
    if (current.pos.y < m3688.y - 200.0f) {
        cLib_chaseF(&mNormalSpeed, 0.0f, 0.08000000566244125f); /* HD: 0.1f * HIO (0.8f) folded, 0x3DA3D70B */
    }
    return TRUE;
}
VERIFY(0x02441104, &daPy_lk_c::procCutExMJ);

/* 02441450 */
BOOL daPy_lk_c::procCutKesa() {
    WWHD_FUNC(0x02441450, BOOL, this);
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[0];
    m35EC = frameCtrl->getFrame();
    LK_cutHoldFlag();
    return lk_procCutExA(this, frameCtrl, 2, 32.0f, 16.0f, 31, 22.0f, 26.0f);
}
VERIFY(0x02441450, &daPy_lk_c::procCutKesa);

/* 02441750 */
BOOL daPy_lk_c::procCutTurn() {
    WWHD_FUNC(0x02441750, BOOL, this);
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[0];
    m35EC = frameCtrl->getFrame();
    if ((mProcVar6 != 0 && (mItemButton & 2)) && m34C5 == 0) {
        mNoResetFlg0 = mNoResetFlg0 | 4;
    } else {
        mNoResetFlg0 = mNoResetFlg0 & ~4u;
    }
    if (frameCtrl->getRate() < 0.01f) {
        if (mProcVar0 > 0) {
            mProcVar0 = mProcVar0 - 1;
        } else {
            mDirection = 3; /* DIR_RIGHT */
            LK_checkNextMode(this, 0);
            m69E8 = 0; /* HD */
            return TRUE;
        }
    } else if (frameCtrl->getFrame() > 21.0f) {
        u8 dir = mDirection;
        mDirection = 3;
        if (LK_checkNextMode(this, 1)) {
            m69E8 = 0; /* HD */
            return TRUE;
        }
        mDirection = dir;
    } else if (frameCtrl->getFrame() > 17.0f) {
        gabi::call(LK_mtxFollow_end, &m32E4);
    }
    if (frameCtrl->getFrame() > m35A0) {
        gabi::call(LK_mtxFollow_end, &m32F0);
        gabi::call(0x025A5F88 /* dPa_smokeEcallBack::end */, mSmokeEcallBack);
    }
    gabi::call(LK_setSpecialBattle, this, 1);
    if (lk_checkParry(this)) {
        return gabi::call<BOOL>(LK_changeSpecialBattle, this);
    }
    if (frameCtrl->getFrame() >= 3.0f && frameCtrl->getFrame() < 18.5f) {
        if (!(mNoResetFlg0 & 0x40)) {
            setResetFlg0(resetFlg0() | 1);
            seStartSwordCut(0x282B /* JA_SE_LK_KAITENGIRI */);
        }
        setResetFlg0(resetFlg0() | 2);
        cLib_chaseF(&mAtCylR, m35A4, 18.0f);
    }
    /* m331C/m332C/m333C (dPa_cutTurnEcallBack_c) alpha at +4 */
    if (frameCtrl->checkPass(18.5f)) {
        gabi::store<u8>(gabi::ea(m332C) + 4, 0x80);
        gabi::store<u8>(gabi::ea(m333C) + 4, 0x80);
        gabi::store<u8>(gabi::ea(m331C) + 4, 0x80);
    } else if (gabi::load<u8>(gabi::ea(m331C) + 4) == 0x80) {
        gabi::store<u8>(gabi::ea(m331C) + 4, 0);
        gabi::store<u8>(gabi::ea(m332C) + 4, 0);
        gabi::store<u8>(gabi::ea(m333C) + 4, 0);
    }
    m34C2 = 1;
    return TRUE;
}
VERIFY(0x02441750, &daPy_lk_c::procCutTurn);

/* 02441A44 */
BOOL daPy_lk_c::procCutRoll_init() {
    WWHD_FUNC(0x02441A44, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x56 /* daPyProc_CUT_ROLL_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x2A /* ANM_CUTTURNB */, 1.2f, 4.0f, 6, 1.0f);
    m3578 = 0;
    seStartMapInfo(0x1059 /* JA_SE_LK_V_KAITEN_S */);
    /* dComIfGp_setItemMagicCount(-2) */
    u32 play = dComIfGp_ea();
    gabi::store<s16>(play + 0x5B60, (s16)(gabi::load<s16>(play + 0x5B60) - 2));
    gabi::store<u8>(gabi::ea(this) + 0x3AC, 9); /* mCutType = CUT_TYPE_CUT_ROLL (daPy_py_c, plain u8 in d_a_player.h) */
    gabi::store<u8>(gabi::ea(this) + 0x3AD, 0); /* mCutCount */
    setResetFlg0(resetFlg0() & ~0x08000000u); /* offResetFlg0(daPyRFlg0_NOT_ATTACKING) */
    if (lk_checkNormalSwordEquip()) {
        m35A4 = 180.0f;
        if (noResetFlg1() & 0x8000 /* daPyFlg1_SOUP_POWER_UP */) {
            mAtCyl.mAtp = 4;
        } else {
            mAtCyl.mAtp = 2;
        }
    } else {
        m35A4 = 230.0f;
        if (noResetFlg1() & 0x8000) {
            mAtCyl.mAtp = 8;
        } else {
            mAtCyl.mAtp = 4;
        }
    }
    mAtCyl.SetR(m35A4 * 0.5f);
    mProcVar0 = 90;
    current.angle.y = shape_angle.y;
    mFootEffectPosType = 4; /* setFootEffectPosType(4) */
    u32 st = dComIfGp_ea() + 0x5CD8; /* dComIfGp_setPlayerStatus0(0, daPyStts0_SPIN_ATTACK_e) */
    gabi::store<u32>(st, gabi::load<u32>(st) | 0x40000000);
    gabi::call(LK_endFlameDamageEmitter, this);
    return TRUE;
}
VERIFY(0x02441A44, &daPy_lk_c::procCutRoll_init);

/* 02441C38 */
BOOL daPy_lk_c::procCutRollEnd_init() {
    WWHD_FUNC(0x02441C38, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x57 /* daPyProc_CUT_ROLL_END_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x9D /* ANM_WAITQ */, 1.3f, 0.0f, -1, 2.4f);
    mNormalSpeed = 0.0f;
    current.angle.y = shape_angle.y;
    mProcVar0 = 60;
    /* function-local statics: emitter_trans (guard 1046D270, data 1046D278), particle_scale (guard 1046D274, data 1046D284) */
    const u32 trans = 0x1046D278, scale = 0x1046D284;
    if (gabi::load<u32>(0x1046D270) == 0) {
        gabi::store<f32>(trans + 8, 0.0f);
        gabi::store<u32>(0x1046D270, 1);
        gabi::store<f32>(trans + 4, 55.0f);
        gabi::store<f32>(trans + 0, 0.0f);
    }
    if (gabi::load<u32>(0x1046D274) == 0) {
        gabi::store<u32>(0x1046D274, 1);
        gabi::store<f32>(scale + 0, 0.76f);
        gabi::store<f32>(scale + 8, 0.76f);
        gabi::store<f32>(scale + 4, 0.76f);
    }
    Mtx34* mtx = lk_getAnmMtx(mpCLModel, 15 /* CL_JNT_HEAD_JNT_e */);
    u32 emitter = gabi::call<u32>(LK_mtxFollow_makeEmitter, m33A8, 0x27A /* ID_IT_JN_PIYOPIYO00 */, mtx, &current.pos, nullptr);
    if (emitter != 0) {
        gabi::store<u32>(emitter + 0x254, gabi::load<u32>(emitter + 0x254) | 0x40); /* becomeImmortalEmitter */
        gabi::store<s16>(emitter + 0x64, 5);   /* setVolumeSize(5) */
        gabi::store<s16>(emitter + 0x60, 60);  /* setLifeTime (HD: HIO folded) */
        gabi::store<f32>(emitter + 0x14, gabi::load<f32>(trans + 0)); /* setEmitterTranslation */
        gabi::store<f32>(emitter + 0x18, gabi::load<f32>(trans + 4));
        gabi::store<f32>(emitter + 0x1C, gabi::load<f32>(trans + 8));
        gabi::store<f32>(emitter + 0x238, gabi::load<f32>(scale + 0)); /* setGlobalParticleScale */
        gabi::store<f32>(emitter + 0x23C, gabi::load<f32>(scale + 4));
        gabi::store<f32>(emitter + 0x240, gabi::load<f32>(scale + 8));
    }
    return TRUE;
}
VERIFY(0x02441C38, &daPy_lk_c::procCutRollEnd_init);

/* 02441DC0 */
BOOL daPy_lk_c::procCutRoll() {
    WWHD_FUNC(0x02441DC0, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        s16 oldAngle = current.angle.y;
        if (mAcch.ChkWallHit() && mDemoMode != 0x2B /* daPy_demo_c::DEMO_CUT_ROLL_e */) {
            f32 dist = gabi::fadds_ppc(mAcchCir[0].m_wall_r, 25.0f);
            gabi::Local<cXyz> start;
            gabi::Local<cXyz> end;
            start->x = current.pos.x;
            start->y = gabi::fadds_ppc(current.pos.y, mAcchCir[0].m_wall_h);
            start->z = current.pos.z;
            end->x = gabi::fmadds(cM_ssin(current.angle.y), dist, start->x);
            end->y = start->y;
            end->z = gabi::fmadds(cM_scos(current.angle.y), dist, start->z);
            dBgS_LinChk_Set(mLinkLinChk, start, end, this);
            if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
                u32 pla = gabi::call<u32>(0x020084C8 /* cBgS::GetTriPla */, dComIfG_Bgsp(), (u32)gabi::load<u16>(mLinkLinChkPoly + 2),
                                          (u32)gabi::load<u16>(mLinkLinChkPoly + 0));
                if (pla != 0) { /* HD: NULL check */
                    shape_angle.y = cM_atan2s(-gabi::load<f32>(pla + 0), -gabi::load<f32>(pla + 8));
                    current.angle.y = shape_angle.y;
                }
                return gabi::call<BOOL>(LK_procFrontRollCrash_init, this);
            }
        }
        if (mStickDistance > 0.05f) {
            cLib_addCalcAngleS(&current.angle.y, m34E8, 5, 0xBB8, 0x64); /* HD: HIO folded */
            shape_angle.y = current.angle.y;
        }
        s16 turn = (s16)(current.angle.y - oldAngle);
        if (dComIfGp_event_runCheck_l()) {
            if (mDemoMode != 0x2B) {
                mProcVar0 = 0;
            } else {
                mProcVar0 = 5;
            }
        }
        if (mProcVar0 > 0) {
            mProcVar0 = mProcVar0 - 1;
            mNormalSpeed = 30.0f;
            m34EC = m34EC - (0x36B0 + turn);
            if (!(mNoResetFlg0 & 0x40)) {
                setResetFlg0(resetFlg0() | 1);
            }
            setResetFlg0(resetFlg0() | 2);
            cLib_chaseF(&mAtCylR, m35A4, 18.0f);
            seStartMapInfo(0x1059 /* JA_SE_LK_V_KAITEN_S */);
        } else if (gabi::call<f64>(0x0200ECD4 /* cLib_addCalc */, &mNormalSpeed, 0.0f, 0.1f, 2.5f, 1.0f) < 0.5f) {
            shape_angle.y = shape_angle.y + m34EC;
            current.angle.y = shape_angle.y;
            if (dComIfGp_event_runCheck_l() && mDemoMode != 0x2B) {
                gabi::call(LK_procWait_init, this);
            } else {
                procCutRollEnd_init();
            }
        } else {
            f32 rate = mNormalSpeed / 30.0f;
            m34EC = (s16)gabi::ftoi((f32)m34EC - gabi::fmadds((f32)(0x34B0 + turn), rate, 512.0f));
        }
    } else {
        seStartMapInfo(0x1059);
    }
    return TRUE;
}
VERIFY(0x02441DC0, &daPy_lk_c::procCutRoll);

/* 02442134 */
BOOL daPy_lk_c::procCutRollEnd() {
    WWHD_FUNC(0x02442134, BOOL, this);
    seStartOnlyReverb(0x50BC /* JA_SE_CM_MD_PIYO */);
    if (mProcVar0 > 0) {
        mProcVar0 = mProcVar0 - 1;
    } else {
        LK_checkNextMode(this, 0);
    }
    return TRUE;
}
VERIFY(0x02442134, &daPy_lk_c::procCutRollEnd);

/* 02442190 */
BOOL daPy_lk_c::procCutTurnMove_init() {
    WWHD_FUNC(0x02442190, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x59 /* daPyProc_CUT_TURN_MOVE_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x19 /* ANM_CUTTURNPWFB */, 0.0f, 0.0f, -1, 3.0f);
    LK_bokoUpperEnd();
    m3598 = 1.0f;
    mDirection = 0; /* DIR_FORWARD */
    BOOL bit;
    if (dSv_event_isEventBit(gabi::at<dSv_event_c>(dComIfGs_base_l() + 0x644), 0xB20)) {
        bit = TRUE;
    } else if (dSv_event_isEventBit(gabi::at<dSv_event_c>(dComIfGs_base_l() + 0x1178), 0x402) /* dComIfGs_isTmpBit */) {
        bit = TRUE;
    } else {
        bit = FALSE;
    }
    /* HD: checkSwordMiniGame() and dComIfGp_getMiniGameType() != 6 both read play + 0x5CEA */
    if (bit && mEquipItem == 0x103 /* daPyItem_SWORD_e */ && gabi::load<u8>(dComIfGs_base_l() + 0x34) /* getMagic */ >= 2 &&
        gabi::load<u8>(dComIfGp_ea() + 0x5CEA) != 2 && gabi::load<u8>(dComIfGp_ea() + 0x5CEA) != 6) {
        cXyz* scale = lk_checkNormalSwordEquip() ? nullptr : gabi::at<cXyz>(0x101CEED8) /* eff_scale */;
        Mtx34* mtx = lk_getAnmMtx(mpCLModel, 8 /* CL_JNT_CL_LHANDA_e */);
        gabi::call(LK_mtxFollow_makeEmitter, &m32E4, 0x29 /* ID_AK_JN_CHARGEPOWER00 */, mtx, &current.pos, scale);
        mtx = lk_getAnmMtx(mpCLModel, 8);
        gabi::call(LK_mtxFollow_makeEmitter, &m32F0, 0x2A /* ID_AK_JN_CHARGEPOWER01 */, mtx, &current.pos, scale);
        mProcVar0 = 47;
    } else {
        mProcVar0 = -1;
    }
    lk_onPlayerStatus(0x5CD8, 0x40000000 /* daPyStts0_SPIN_ATTACK_e */);
    return TRUE;
}
VERIFY(0x02442190, &daPy_lk_c::procCutTurnMove_init);

/* 0244240C */
BOOL daPy_lk_c::procCutTurnCharge() {
    WWHD_FUNC(0x0244240C, BOOL, this);
    if (!(mItemButton & 2) || lk_checkCurse()) {
        LK_checkNextMode(this, 0);
    } else if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        procCutTurnMove_init();
    }
    return TRUE;
}
VERIFY(0x0244240C, &daPy_lk_c::procCutTurnCharge);

/* 02442470 */
BOOL daPy_lk_c::procCutTurnMove() {
    WWHD_FUNC(0x02442470, BOOL, this);
    gabi::call(LK_setShapeAngleToAtnActor, this);
    m34E6 = shape_angle.y;
    if (0 < mProcVar0) {
        mProcVar0 = mProcVar0 - 1;
        if (mProcVar0 == 0) {
            lk_onPlayerStatus(0x5CDC, 0x20000 /* daPyStts1_UNK20000_e */);
        }
    }
    if (mProcVar0 != -1) {
        gabi::call(0x025E1988 /* seStartSystem */, 0x203B /* JA_SE_LK_SWORD_CHARGE */);
    }
    if (gabi::call<BOOL>(LK_changeSlideProc, this)) {
        return TRUE;
    }
    if (lk_checkCurse()) {
        LK_checkNextMode(this, 0);
    } else if (mItemButton & 2) {
        f32 speed = 0.0f;
        if (mStickDistance > 0.05f) {
            s16 target = m34E8;
            int direction = gabi::call<int>(LK_getDirectionFromAngle, this, (s32)(s16)(target - m34E6));
            u32 anm = 0xFFFF;
            cLib_addCalcAngleS(&current.angle.y, target, 4, 12000, 0x2000);
            if (mDirection == 0 /* DIR_FORWARD */ || mDirection == 1 /* DIR_BACKWARD */) {
                if (direction == 2 /* DIR_LEFT */ || direction == 3 /* DIR_RIGHT */) {
                    anm = 0x1A; /* ANM_CUTTURNPWLR */
                    mNormalSpeed = gabi::fmuls_ppc(mNormalSpeed, 0.5f);
                } else if (mDirection != direction) {
                    mNormalSpeed = gabi::fmuls_ppc(mNormalSpeed, 0.5f);
                }
            } else if (direction == 0 || direction == 1) {
                anm = 0x19; /* ANM_CUTTURNPWFB */
                mNormalSpeed = gabi::fmuls_ppc(mNormalSpeed, 0.5f);
            } else if (mDirection != direction) {
                mNormalSpeed = gabi::fmuls_ppc(mNormalSpeed, 0.5f);
            }
            if (mDirection == direction) {
                speed = gabi::fmuls_ppc(3.5f, mStickDistance); /* HD: HIO folded */
            } else {
                mDirection = direction;
            }
            if (anm != 0xFFFF) {
                gabi::call(LK_setSingleMoveAnime, this, anm, 0.0f, 0.0f, -1, 3.0f);
            }
        }
        gabi::call(LK_setNormalSpeedF, this, speed, 0.5f, 2.0f, 0.25f);
        f32 anmSpeed;
        if (mDirection == 0) {
            anmSpeed = 0.8f;
        } else if (mDirection == 1) {
            anmSpeed = -0.8f;
        } else if (mDirection == 3) {
            anmSpeed = -0.8f;
        } else {
            anmSpeed = 0.8f;
        }
        f32 rate = mNormalSpeed / mMaxNormalSpeed;
        mFrameCtrlUnder[0].setRate(rate * anmSpeed);
        if (anmSpeed >= 0.0f) {
            mFrameCtrlUnder[0].mLoop = mFrameCtrlUnder[0].getStart();
        } else {
            mFrameCtrlUnder[0].mLoop = mFrameCtrlUnder[0].getEnd();
        }
        gabi::call(LK_initSeAnime, this);
        m3598 = gabi::fnmsubs(0.050000011920928955f /* 1.0f - 0.95f */, rate, 1.0f);
        if (mNormalSpeed <= 0.001f || mNormalSpeed != mNormalSpeed) {
            mModeFlg = mModeFlg | 1; /* onModeFlg(ModeFlg_00000001) */
            m3598 = 0.0f;
        } else {
            mModeFlg = mModeFlg & ~1u;
            m3598 = 1.0f;
        }
        LK_bokoUpperEnd();
    } else if (mProcVar0 == 0) {
        procCutRoll_init();
    } else {
        mNormalSpeed = speedF;
        gabi::call(LK_procCutTurn_init, this, 0);
    }
    return TRUE;
}
VERIFY(0x02442470, &daPy_lk_c::procCutTurnMove);

/* 02442878 */
BOOL daPy_lk_c::procCutReverse() {
    WWHD_FUNC(0x02442878, BOOL, this);
    cLib_addCalc(&mNormalSpeed, 0.0f, 0.5f, 1.125f, 0.3f); /* HD: HIO folded */
    if ((mItemButton & 2) && mEquipItem == 0x103 /* daPyItem_SWORD_e */) {
        mNoResetFlg0 = mNoResetFlg0 | 4;
    } else {
        mNoResetFlg0 = mNoResetFlg0 & ~4u;
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        current.angle.y = shape_angle.y;
        LK_checkNextMode(this, 0);
    } else if (mFrameCtrlUnder[0].getFrame() > 11.0f) {
        current.angle.y = shape_angle.y;
        if (!LK_checkNextMode(this, 1)) {
            current.angle.y = shape_angle.y + 0x8000;
        }
    }
    return TRUE;
}
VERIFY(0x02442878, &daPy_lk_c::procCutReverse);

/* 02442990 */
BOOL daPy_lk_c::procJumpCutLand_init() {
    WWHD_FUNC(0x02442990, BOOL, this);
    f32 fall = m35F0 - current.pos.y;
    gabi::call(LK_commonProcInit, this, 0x5C /* daPyProc_JUMP_CUT_LAND_e */);
    if (!dComIfGp_event_runCheck_l()) {
        /* HD: HIO folded (100 * 60, 100 * 20); NaN counts as a long fall */
        if (!(fall < 6000.0f)) {
            gabi::call(LK_setDamagePoint, this, -2.0f);
        } else if (!(fall < 2000.0f)) {
            gabi::call(LK_setDamagePoint, this, -1.0f);
        }
        if (!(fall < 2000.0f)) {
            mDamageWaitTimer = 30;
            mModeFlg = mModeFlg | 8; /* onModeFlg(ModeFlg_DAMAGE) */
        }
    }
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0x2F /* ANM_JATTACKLAND */, 1.1f, 1.0f, 14, 2.0f);
    if (mEquipItem == 0x100 /* daPyItem_NONE_e */) {
        mProcVar6 = 0;
    } else {
        setResetFlg0(resetFlg0() | 2);
        if (!(mNoResetFlg0 & 0x40)) {
            seStartSwordCut(0x2800);
            u32 m = gabi::ea(mpCLModel.get());
            Mtx34* mtx = gabi::at<Mtx34>(m ? m + 0xC8 : 0); /* getBaseTRMtx */
            f64 rate = gabi::call<f64>(LK_getBlurTopRate, this);
            s32 color = gabi::call<s32>(LK_getSwordBlurColor, this);
            gabi::call(0x02407070 /* daPy_swBlur_c::initSwBlur */, mpSwBlur.get(), mtx, 130 /* 10 * HIO folded */, rate, color);
            mNoResetFlg0 = mNoResetFlg0 | 0x40;
        }
        gabi::call(LK_setJumpCutAtParam, this);
        mProcVar6 = 1;
    }
    mProcVar0 = 2;
    current.angle.y = shape_angle.y + 0x8000;
    mFootEffectPosType = 3;
    setResetFlg0(resetFlg0() | 0xC00); /* RIGHT_FOOT_ON_GROUND | LEFT_FOOT_ON_GROUND */
    if (mEquipItem == 0x33 /* dItemNo_SKULL_HAMMER_e */) {
        gabi::call(LK_setHammerQuake, this, 0, 0, -1);
    } else if (gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
        gabi::Local<cXyz> dir;
        dir->x = 0.0f;
        dir->y = 1.0f;
        dir->z = 0.0f;
        dComIfGp_getVibration_StartShock(5, -0x31, dir);
    }
    return TRUE;
}
VERIFY(0x02442990, &daPy_lk_c::procJumpCutLand_init);

/* 02442C1C */
BOOL daPy_lk_c::procJumpCut() {
    WWHD_FUNC(0x02442C1C, BOOL, this);
    /* HD: the spin-attack follow-up of procJumpCutLand is decided here (latched in m69E8) */
    s32 v = m3578;
    if ((v < 0 ? -v : v) > 0xF800 && !lk_checkCurse() && (mEquipItem == 0x103 || mEquipItem == 0x101)) {
        m69E8 = 1;
        gabi::call(LK_HD_023ED2B0, this, 0);
    }
    if (mAcch.ChkGroundHit()) {
        return procJumpCutLand_init();
    }
    if (current.angle.y == shape_angle.y && gabi::call<BOOL>(LK_changeCutReverseProc, this, 0x2B /* ANM_JATTACK */)) {
        current.angle.y = current.angle.y + 0x8000;
        mNormalSpeed = 27.0f;
    } else if (current.angle.y != shape_angle.y) {
        cLib_addCalc(&mNormalSpeed, 5.0f, 0.2f, 1.0f, 0.1f);
    }
    if (mFrameCtrlUnder[0].getFrame() >= 13.0f && mFrameCtrlUnder[0].getFrame() < 15.0f) {
        if (mEquipItem != 0x100) {
            if (!(mNoResetFlg0 & 0x40)) {
                setResetFlg0(resetFlg0() | 1);
                seStartSwordCut(0x2800);
            }
            setResetFlg0(resetFlg0() | 2);
        }
    }
    return TRUE;
}
VERIFY(0x02442C1C, &daPy_lk_c::procJumpCut);

/* 02442DC0 */
BOOL daPy_lk_c::procJumpCutLand() {
    WWHD_FUNC(0x02442DC0, BOOL, this);
    current.angle.y = shape_angle.y;
    gabi::call(LK_resetFootEffect, this);
    if (mProcVar6 != 0 && gabi::call<BOOL>(LK_changeCutReverseProc, this, 0x27 /* ANM_CUTRER */)) {
        return TRUE;
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        if (mProcVar0 > 0) {
            mProcVar0 = mProcVar0 - 1;
            LK_checkNextMode(this, 1);
        } else {
            LK_checkNextMode(this, 0);
        }
    } else if (mFrameCtrlUnder[0].getFrame() > 12.0f) {
        LK_checkNextMode(this, 1);
    } else if (mProcVar6 != 0) {
        mProcVar6 = 0;
        if (m69E8 != 0) { /* HD: latched by procJumpCut */
            gabi::call(LK_procCutTurn_init, this, 1);
        } else {
            setResetFlg0(resetFlg0() | 2);
        }
    }
    return TRUE;
}
VERIFY(0x02442DC0, &daPy_lk_c::procJumpCutLand);

/* ---- d_a_player_boomerang.inc ---- */

/* 02442ED8 */
BOOL daPy_lk_c::returnBoomerang() {
    WWHD_FUNC(0x02442ED8, BOOL, this);
    u32 st = dComIfGp_ea() + 0x5CD8; /* dComIfGp_clearPlayerStatus0(0, daPyStts0_BOOMERANG_WAIT_e) */
    gabi::store<u32>(st, gabi::load<u32>(st) & ~0x00400000u);
    if (mEquipItem == 0x100 /* daPyItem_NONE_e */ &&
        gabi::load<u32>(dComIfGp_ea() + 0x5B2C) == gabi::ea(this) /* !checkNoControll() */ &&
        !dComIfGp_event_runCheck_l() && mDemoType == 0 /* !checkPlayerDemoMode() */ &&
        (gabi::load<u16>(gabi::ea(this) + 0x5888) == 0xFFFF /* checkNoUpperAnime() */ ||
         gabi::call_ptr<BOOL>(gabi::load<u32>(__vtbl + 0x3C), this) /* checkPlayerGuard() (virtual) */ ||
         gabi::load<u16>(gabi::ea(this) + 0x5888) == 0x64 /* checkDashDamageAnime() */ ||
         gabi::call<BOOL>(LK_checkBoomerangAnime, this)) &&
        (!(mModeFlg & 0x01FD2810) || mCurProc == 0x8B /* daPyProc_SHIP_BOOMERANG_e */)) {
        u32 id = mActorKeepThrow.mID;
        u32 actor = gabi::ea(mActorKeepThrow.mActor.get());
        mActorKeepEquip.mID = id;
        gabi::store<u32>(gabi::ea(&mActorKeepEquip) + 4, actor);
        gabi::call(0x023DC63C /* daPy_actorKeep_c::clearData */, &mActorKeepThrow);
        mEquipItem = 0x2D; /* dItemNo_BOOMERANG_e */
        mNoResetFlg0 = mNoResetFlg0 | 0x20;
        return TRUE;
    }
    gabi::call(0x023DC63C, &mActorKeepThrow);
    return FALSE;
}
VERIFY(0x02442ED8, &daPy_lk_c::returnBoomerang);

/* ---- d_a_player_ship.inc ---- */

/* 0244300C */
BOOL daPy_lk_c::shipSpecialDemoStart() {
    WWHD_FUNC(0x0244300C, BOOL, this);
    if (gabi::call<BOOL>(0x02540310 /* dEvt_control_c::compulsory */, dComIfGp_ea() + 0x51D0, this, 0, 0xFFFF)) {
        mDemoType = 5; /* mDemo.setSpecialDemoType() */
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0244300C, &daPy_lk_c::shipSpecialDemoStart);

/* ---- d_a_player_tact.inc ---- */

/* 0244307C */
BOOL daPy_lk_c::checkEndTactMusic() {
    WWHD_FUNC(0x0244307C, BOOL, this);
    if (mCurProc == 0x9B /* daPyProc_TACT_PLAY_e */ && mProcVar0 != 0) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0244307C, &daPy_lk_c::checkEndTactMusic);

/* 024430C0 */
f32 daPy_lk_c::getTactMetronomeRate() {
    WWHD_FUNC(0x024430C0, f32, this);
    if (mCurProc == 0x9A /* daPyProc_TACT_WAIT_e */) {
        f64 beat = gabi::call<f64>(0x025E1F08 /* mDoAud_tact_getBeatFrames */);
        return (f32)((f64)(f32)m35A0 / beat);
    }
    return -1.0f;
}
VERIFY(0x024430C0, &daPy_lk_c::getTactMetronomeRate);

/* 0244311C */
BOOL daPy_lk_c::checkTactLastInput() {
    WWHD_FUNC(0x0244311C, BOOL, this);
    if (mCurProc == 0x9A && mProcVar1 >= 0) { /* HD: >= 0 (GameCube != -1) */
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0244311C, &daPy_lk_c::checkTactLastInput);

/* 02443144 */
BOOL daPy_lk_c::getTactTopPos(cXyz* out) {
    WWHD_FUNC(0x02443144, BOOL, this, out);
    if (mEquipItem != 0x22 /* dItemNo_WIND_WAKER_e */ || mpEquipItemModel == 0) {
        return FALSE;
    }
    u32 m = mpEquipItemModel;
    PSMTXMultVec(gabi::at<Mtx34>(m ? m + 0xC8 : 0) /* getBaseTRMtx */, gabi::at<cXyz>(0x10034FF0) /* l_tact_top */, out);
    return TRUE;
}
VERIFY(0x02443144, &daPy_lk_c::getTactTopPos);

/* 024431AC */
BOOL daPy_lk_c::getTactNormalWait() {
    WWHD_FUNC(0x024431AC, BOOL, this);
    if (mCurProc == 0x9A && mProcVar6 == -1 && mProcVar1 < 0) { /* HD: < 0 (GameCube == -1) */
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x024431AC, &daPy_lk_c::getTactNormalWait);

/* 024431E0 (HD) */
BOOL daPy_lk_c::checkTactPlayMelody() {
    WWHD_FUNC(0x024431E0, BOOL, this);
    if (mCurProc == 0x9B /* daPyProc_TACT_PLAY_e */ && mProcVar5 == 0) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x024431E0, &daPy_lk_c::checkTactPlayMelody);

/* ---- d_a_player_particle.inc ---- */

/* 02443208 */
BOOL daPy_lk_c::setItemWaterEffect(fopAc_ac_c* actor, BOOL inWater, BOOL triggerOnExit) {
    WWHD_FUNC(0x02443208, BOOL, actor, inWater, triggerOnExit); /* static: no this */
    /* function-local statics: eff_scale0 (guard 1046D290), eff_scale2 (guard 1046D294), direction (guard 1046D298) */
    const u32 scale0 = 0x1046CD84, scale2 = 0x1046CD90, direction = 0x1046CD9C;
    if (gabi::load<u32>(0x1046D290) == 0) {
        gabi::store<u32>(0x1046D290, 1);
        gabi::store<f32>(scale0 + 0, 0.5f);
        gabi::store<f32>(scale0 + 8, 0.5f);
        gabi::store<f32>(scale0 + 4, 0.5f);
    }
    if (gabi::load<u32>(0x1046D294) == 0) {
        gabi::store<u32>(0x1046D294, 1);
        gabi::store<f32>(scale2 + 0, 0.13f);
        gabi::store<f32>(scale2 + 8, 0.13f);
        gabi::store<f32>(scale2 + 4, 0.13f);
    }
    if (gabi::load<u32>(0x1046D298) == 0) {
        gabi::store<f32>(direction + 0, 0.0f);
        gabi::store<f32>(direction + 8, 0.0f);
        gabi::store<u32>(0x1046D298, 1);
        gabi::store<f32>(direction + 4, 1.0f);
    }
    gabi::Local<be<f32>> waterY;
    if (gabi::call<BOOL>(0x025D9F70 /* fopAcM_getWaterY */, &actor->current.pos, waterY.get()) &&
        ((inWater && actor->current.pos.y > *waterY) || (!inWater && !(*waterY < actor->current.pos.y)))) {
        /* the item entered or left the water */
        inWater ^= 1;
        if (!inWater && !triggerOnExit) {
            return inWater;
        }
        f32 posY = actor->current.pos.y;
        f32 deltaY = std::fabs(actor->old.pos.y - posY);
        f32 waterDistY = std::fabs(*waterY - posY);
        gabi::Local<cXyz> pos;
        if (deltaY < 1.0f) {
            pos->x = actor->current.pos.x;
            pos->y = posY;
            pos->z = actor->current.pos.z;
        } else {
            f32 t = waterDistY / deltaY;
            if (t > 1.0f) {
                t = 1.0f;
            }
            gabi::Local<cXyz> a;
            gabi::Local<cXyz> b;
            gabi::Local<cXyz> sum;
            cXyz_ml(&actor->old.pos, a, t);
            cXyz_ml(&actor->current.pos, b, 1.0f - t);
            cXyz_pl(a, sum, b);
            for (u32 k = 0; k < 12; k += 4) gabi::store<u32>(gabi::ea(pos.get()) + k, gabi::load<u32>(gabi::ea(sum.get()) + k));
        }
        /* dComIfGp_particle_setSingleRipple(ID_IT_JN_WP_HAMON01/03) */
        dPa_control_set(dComIfGp_getParticle(), 5, 0x3D, pos, nullptr, gabi::at<cXyz>(scale0), 0xFF,
                        gabi::at<dPa_levelEcallBack>(0x1047B2E4) /* single ripple callback */, -1, nullptr, nullptr, nullptr);
        dPa_control_set(dComIfGp_getParticle(), 5, 0x3F, pos, nullptr, gabi::at<cXyz>(scale0), 0xFF,
                        gabi::at<dPa_levelEcallBack>(0x1047B2E4), -1, nullptr, nullptr, nullptr);
        gabi::Local<GXColor> amb;
        gabi::Local<GXColor> dif;
        gabi::call(0x025602F0 /* dKy_get_seacolor */, amb.get(), dif.get());
        u32 emitter = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 1, 0x23 /* ID_AK_JN_ELEMENTSHIBUKI00 */, pos, nullptr,
                                               gabi::at<cXyz>(scale2), 0xFF, nullptr, -1, amb.get(), nullptr, nullptr));
        if (emitter != 0) {
            gabi::store<f32>(emitter + 0x58, 0.0f);  /* setSpread */
            gabi::store<s16>(emitter + 0x60, 20);    /* setLifeTime */
            gabi::store<f32>(emitter + 0x70, 80.0f); /* setDirectionalSpeed */
            gabi::store<f32>(emitter + 0x6C, 20.0f); /* setAwayFromAxisSpeed */
            gabi::store<s32>(emitter + 0x5C, 1);     /* setMaxFrame */
            gabi::store<f32>(emitter + 0x34, 40.0f); /* setRate */
            gabi::store<f32>(emitter + 0x28, gabi::load<f32>(direction + 0)); /* setDirection */
            gabi::store<f32>(emitter + 0x2C, gabi::load<f32>(direction + 4));
            gabi::store<f32>(emitter + 0x30, gabi::load<f32>(direction + 8));
        }
    }
    return inWater;
}
VERIFY(0x02443208, &daPy_lk_c::setItemWaterEffect);

/* ---- d_a_player_hook.inc ---- */

/* 024435FC (unnamed by the matcher) */
BOOL daPy_lk_c::checkHookshotReturn() {
    WWHD_FUNC(0x024435FC, BOOL, this);
    u32 hookshot = gabi::ea(mActorKeepEquip.mActor.get());
    if (mEquipItem != 0x2F /* dItemNo_HOOKSHOT_e */ || hookshot == 0) {
        return FALSE;
    }
    /* checkPull() || checkReturn(): HD mode word at +0xB0 */
    u32 mode = gabi::load<u32>(hookshot + 0xB0);
    return mode == 3 || mode == 2;
}
VERIFY(0x024435FC, &daPy_lk_c::checkHookshotReturn);

/* ---- d_a_player_sword.inc / main ---- */

/* 0244363C */
BOOL daPy_lk_c::checkCutRollChange() {
    WWHD_FUNC(0x0244363C, BOOL, this);
    if (mCurProc == 0x59 /* daPyProc_CUT_TURN_MOVE_e */ && mProcVar0 >= 0) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0244363C, &daPy_lk_c::checkCutRollChange);

/* 02443664 */
BOOL daPy_lk_c::checkGameOverStart() {
    WWHD_FUNC(0x02443664, BOOL, this);
    return mCurProc == 0xB2 /* daPyProc_DEMO_DEAD_e */ && m35A0 < 0.38f;
}
VERIFY(0x02443664, &daPy_lk_c::checkGameOverStart);

/* 02443694 */
int daPy_lk_c::getTactTimerCancel() {
    WWHD_FUNC(0x02443694, int, this);
    /* HD: no Korok (grab actor) check; 1 above -100, else 2 */
    if (mCurProc == 0x9A && !(m35AC > 0.0f)) {
        return m35AC > -100.0f ? 1 : 2;
    }
    return 0;
}
VERIFY(0x02443694, &daPy_lk_c::getTactTimerCancel);

/* 024436D8 */
s32 daPy_lk_c::getTactMusic() {
    WWHD_FUNC(0x024436D8, s32, this);
    if (mCurProc == 0x9B) {
        return mProcVar6;
    }
    if (mCurProc == 0x9A) {
        return mProcVar7;
    }
    return -1;
}
VERIFY(0x024436D8, &daPy_lk_c::getTactMusic);

/* 02443704 */
fopAc_ac_c* daPy_lk_c::getGrabMissActor() {
    WWHD_FUNC(0x02443704, fopAc_ac_c*, this);
    if (mCurProc != 0x6E && mCurProc != 0x70) {
        return nullptr;
    }
    return mActorKeepRope.mActor;
}
VERIFY(0x02443704, &daPy_lk_c::getGrabMissActor);

/* 02443768 (unnamed by the matcher) */
void daPy_lk_c::setTactZev(u32 tactZevPartnerId, int r30, u32 r31) {
    WWHD_FUNC(0x02443768, void, this, tactZevPartnerId, r30, r31);
    if (tactZevPartnerId != 0xFFFFFFFF) {
        gabi::call(0x025E1988 /* seStartSystem */, 0x8A7 /* JA_SE_PRE_TAKT */);
    }
    m34CC = (u8)r30;
    m3494 = r31;
    mTactZevPartnerId = tactZevPartnerId;
}
VERIFY(0x02443768, &daPy_lk_c::setTactZev);

/* 024437D0 */
BOOL daPy_lk_c::getBokoFlamePos(cXyz* outPos) {
    WWHD_FUNC(0x024437D0, BOOL, this, outPos);
    if (mEquipItem == 0x101 /* daPyItem_BOKO_e */) {
        fopAc_ac_c* boko = fopAcM_SearchByID(mActorKeepEquip.mID);
        if (boko != nullptr && gabi::load<s16>(gabi::ea(boko) + 0x43C) /* getFlameTimer */ != 0) {
            for (u32 k = 0; k < 12; k += 4) gabi::store<u32>(gabi::ea(outPos) + k, gabi::load<u32>(gabi::ea(&mSwordTopPos) + k));
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x024437D0, &daPy_lk_c::getBokoFlamePos);

/* 02443880 (unnamed by the matcher) */
void daPy_lk_c::voiceStart(u32 id) {
    WWHD_FUNC(0x02443880, void, this, id);
    u32 vowel = gabi::call<u32>(0x025E1CB4 /* mDoAud_getLinkVoiceVowel */, id);
    gabi::call(0x025E1C90 /* mDoAud_linkVoiceStart */, id, &eyePos, vowel, (s32)mReverb);
}
VERIFY(0x02443880, &daPy_lk_c::voiceStart);

/* 024438D4 */
void daPy_lk_c::setOutPower(f32 power, s16 angle, int flag) {
    WWHD_FUNC(0x024438D4, void, this, power, angle, flag);
    f32 cur = m3644;
    if (cur < 0.1f) {
        m3640 = angle;
        m3644 = power;
    } else {
        u16 old = (u16)m3640;
        f32 x = gabi::fmadds(cur, cM_ssin(old), gabi::fmuls_ppc(power, cM_ssin((u16)angle)));
        f32 z = gabi::fmadds(cur, cM_scos(old), gabi::fmuls_ppc(power, cM_scos((u16)angle)));
        m3640 = cM_atan2s(x, z);
        m3644 = std_sqrtf(gabi::fmadds(x, x, gabi::fmuls_ppc(z, z)));
    }
    if (flag != 0) {
        setNoResetFlg1(noResetFlg1() | 0x10000000); /* onNoResetFlg1(daPyFlg1_UNK10000000) */
    }
}
VERIFY(0x024438D4, &daPy_lk_c::setOutPower);

/* 024439D4 (unnamed by the matcher) */
BOOL daPy_lk_c::setHookshotCarryOffset(u32 carryActorID, const cXyz* offset) {
    WWHD_FUNC(0x024439D4, BOOL, this, carryActorID, offset);
    u32 hookshot = gabi::ea(mActorKeepEquip.mActor.get());
    if (mEquipItem != 0x2F || hookshot == 0) {
        return FALSE;
    }
    if (carryActorID != gabi::load<u32>(hookshot + 0xD774) /* getCarryActorID() (HD) */) {
        return FALSE;
    }
    /* setCarryOffset(offset): HD at +0xD768 */
    for (u32 k = 0; k < 12; k += 4) gabi::store<u32>(hookshot + 0xD768 + k, gabi::load<u32>(gabi::ea(offset) + k));
    return TRUE;
}
VERIFY(0x024439D4, &daPy_lk_c::setHookshotCarryOffset);

/* the debug copies of the player position and angles (HD release keeps them) */
static inline void lk_setDebugKeep(daPy_lk_c* self) {
    u32 t = gabi::ea(self);
    for (u32 k = 0; k < 12; k += 4) gabi::store<u32>(0x1046CD48 + k, gabi::load<u32>(t + 0x314 + k)); /* l_debug_keep_pos */
    for (u32 k = 0; k < 6; k += 2) gabi::store<u16>(0x1046CD10 + k, gabi::load<u16>(t + 0x328 + k));  /* l_debug_shape_angle */
    for (u32 k = 0; k < 6; k += 2) gabi::store<u16>(0x1046CD08 + k, gabi::load<u16>(t + 0x320 + k));  /* l_debug_current_angle */
}
/* current.pos = old.pos = *pos (GHS integer copy, in the original's load/store order: pos may alias) */
static inline void lk_setPos(daPy_lk_c* self, const cXyz* pos) {
    u32 t = gabi::ea(self), p = gabi::ea(pos);
    u32 x = gabi::load<u32>(p + 0);
    gabi::store<u32>(t + 0x314, x);
    u32 y = gabi::load<u32>(p + 4);
    gabi::store<u32>(t + 0x318, y);
    u32 z = gabi::load<u32>(p + 8);
    gabi::store<u32>(t + 0x300, x);
    gabi::store<u32>(t + 0x31C, z);
    gabi::store<u32>(t + 0x304, y);
    gabi::store<u32>(t + 0x308, z);
}

/* 02443A28 */
void daPy_lk_c::setPlayerPosAndAngle(cXyz* pos, s16 angle) {
    WWHD_FUNC(0x02443A28, void, this, pos, angle);
    if (!dComIfGp_event_runCheck_l() && gabi::load<u8>(dComIfGp_ea() + 0x5BCF) /* dComIfGp_getScopeType() */ != 1) {
        return;
    }
    if (pos != nullptr) {
        lk_setPos(this, pos);
    }
    shape_angle.y = angle;
    current.angle.y = angle;
    m34DE = angle;
    lk_setDebugKeep(this);
}
VERIFY(0x02443A28, (void (daPy_lk_c::*)(cXyz*, s16)) & daPy_lk_c::setPlayerPosAndAngle);

/* 02443B14 */
void daPy_lk_c::setPlayerPosAndAngle(cXyz* pos, csXyz* angle) {
    WWHD_FUNC(0x02443B14, void, this, pos, angle);
    if (!dComIfGp_event_runCheck_l()) {
        return;
    }
    if (pos != nullptr) {
        lk_setPos(this, pos);
    }
    if (angle != nullptr) {
        shape_angle.x = angle->x;
        shape_angle.y = angle->y;
        shape_angle.z = angle->z;
        current.angle.y = shape_angle.y;
        m34DE = shape_angle.y;
    }
    lk_setDebugKeep(this);
}
VERIFY(0x02443B14, (void (daPy_lk_c::*)(cXyz*, csXyz*)) & daPy_lk_c::setPlayerPosAndAngle);

/* 02443C0C */
void daPy_lk_c::setPlayerPosAndAngle(Mtx34* mtx) {
    WWHD_FUNC(0x02443C0C, void, this, mtx);
    if (dComIfGp_event_runCheck_l()) {
        u32 m = gabi::ea(mtx);
        current.pos.x = gabi::load<f32>(m + 0x0C);
        current.pos.y = gabi::load<f32>(m + 0x1C);
        current.pos.z = gabi::load<f32>(m + 0x2C);
        old.pos.x = current.pos.x;
        old.pos.y = current.pos.y;
        old.pos.z = current.pos.z;
        gabi::call(0x025F232C /* mDoMtx_MtxToRot */, mtx, &shape_angle);
        current.angle.y = shape_angle.y;
        m34DE = shape_angle.y;
        lk_setDebugKeep(this);
    }
}
VERIFY(0x02443C0C, (void (daPy_lk_c::*)(Mtx34*)) & daPy_lk_c::setPlayerPosAndAngle);

/* 02443CE0 */
BOOL daPy_lk_c::setThrowDamage(cXyz* pos, s16 angle, f32 speed, f32 speedY, int damage) {
    WWHD_FUNC(0x02443CE0, BOOL, this, pos, angle, speed, speedY, damage);
    /* virtual setPlayerPosAndAngle(cXyz*, s16) (vtable slot 0x114) */
    gabi::call_ptr(gabi::load<u32>(__vtbl + 0x114), this, pos, (s32)angle);
    this->speed.y = speedY;
    mNormalSpeed = speed;
    gabi::call(LK_setDamagePoint, this, (f32)(s32)(0u - (u32)damage));
    mNoResetFlg0 = mNoResetFlg0 | 0x1000000; /* onNoResetFlg0(daPyFlg0_UNK1000000) */
    return TRUE;
}
VERIFY(0x02443CE0, &daPy_lk_c::setThrowDamage);

/* 02443DA0 */
void daPy_lk_c::changeTextureAnime(u16 btpIdx, u16 btkIdx, int r7) {
    WWHD_FUNC(0x02443DA0, void, this, btpIdx, btkIdx, r7);
    if (!dComIfGp_event_runCheck_l()) {
        return;
    }
    if (r7 == -1) {
        /* m_tex_anm_heap / m_tex_scroll_heap: field_0x4 (index), field_0x6 */
        u32 tah = gabi::ea(m_tex_anm_heap), tsh = gabi::ea(m_tex_scroll_heap);
        if (gabi::load<u16>(tah + 4) != btpIdx || gabi::load<u16>(tah + 6) != 0xFFFE) {
            gabi::store<u16>(tah + 4, btpIdx);
            gabi::store<u16>(tah + 6, 0xFFFE);
            u32 res = gabi::call<u32>(LK_loadTextureAnimeResource, this, (u32)btpIdx, 0);
            gabi::call(LK_setTextureAnimeResource, this, res, 0);
        }
        if (gabi::load<u16>(tsh + 4) != btkIdx || gabi::load<u16>(tsh + 6) != 0xFFFE) {
            gabi::store<u16>(tsh + 4, btkIdx);
            gabi::store<u16>(tsh + 6, 0xFFFE);
            u32 res = gabi::call<u32>(LK_loadTextureScrollResource, this, (u32)btkIdx, 0);
            gabi::call(LK_setTextureScrollResource, this, res, 0);
        }
    } else {
        gabi::call(LK_setDemoTextureAnime, this, (u32)btpIdx, (u32)btkIdx, 0, (u32)(u16)r7);
    }
}
VERIFY(0x02443DA0, &daPy_lk_c::changeTextureAnime);

/* ---- __sinit_d_a_player_main_cpp (matcher: "daPy_lk_c::draw", a matcher error) ---- */
static inline void st32_l(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void stf_l(u32 a, f32 v) { gabi::store<f32>(a, v); }

/* 02443EC4 */
static void __sinit_d_a_player_main_cpp() {
    WWHD_FUNC(0x02443EC4, void);
    /* the per-TU header statics */
    st32_l(0x1046CD24, 0);
    st32_l(0x1046CD1C, 0);
    st32_l(0x1046CD28, 0);
    st32_l(0x1046CD20, 0);
    __register_global_object(0x101CEEE4);
    stf_l(0x1046CCF0, -3.1415927f);
    stf_l(0x1046CCF4, 3.1415927f);
    gabi::call(0x028ED6F8, 0x1046CD18u);
    __register_global_object(0x101CEEF0);
    gabi::call(0x028EAB2C, 0x1046CD19u);
    __register_global_object(0x101CEEFC);
    /* sead::SafeString-like {const char*, vtable 10034B24} pairs (resource names) */
    st32_l(0x1046CCA8, 0x10034B24);
    st32_l(0x1046CCA4, 0x10036200);
    st32_l(0x1046CCAC, 0x100361F8);
    st32_l(0x1046CCB0, 0x10034B24);
    st32_l(0x1046CCC8, 0x10034B24);
    st32_l(0x1046CCD0, 0x10034B24);
    st32_l(0x1046CCC4, 0x10036218);
    st32_l(0x1046CCCC, 0x10036220);
    stf_l(0x1046CCE0, 50.0f);
    st32_l(0x1046CCD8, 0x10034B24);
    stf_l(0x1046CD3C, 0.67f);
    stf_l(0x1046CCDC, -5.0f);
    stf_l(0x101CEC00, 50.0f);
    stf_l(0x101CEC0C, 50.0f);
    stf_l(0x1046CCE4, 9.99f);
    stf_l(0x1046CD40, 0.67f);
    stf_l(0x1046CD44, 0.67f);
    stf_l(0x101CEBC4, 10.0f);
    stf_l(0x101CEB94, 3.25f);
    stf_l(0x101CEBB4, 11.25f);
    st32_l(0x1046CCB8, 0x10034B24);
    st32_l(0x1046CCB4, 0x10036208);
    st32_l(0x1046CCD4, 0x100361F4);
    stf_l(0x101CEBD0, 10.0f);
    stf_l(0x101CEBA8, 11.25f);
    stf_l(0x101CEBAC, 18.75f);
    st32_l(0x1046CCC0, 0x10034B24);
    stf_l(0x101CEBDC, 10.0f);
    stf_l(0x101CEBF4, 50.0f);
    st32_l(0x1046CCBC, 0x10036210);
    stf_l(0x101CEBA0, 3.25f);
    stf_l(0x101CEBE8, 10.0f);
    stf_l(0x101CEC18, 50.0f);
    stf_l(0x101CEC20, 50.0f);
    stf_l(0x101CEC24, 50.0f);
    stf_l(0x101CEC2C, 50.0f);
    stf_l(0x1046CCF8, 50000.0f);
    stf_l(0x101CEC5C, 30.0f);
    stf_l(0x101CEE3C, 70.0f);
    stf_l(0x1046CD00, 10000.0f);
    stf_l(0x101CEDA4, 75.0f);
    st32_l(0x1046CD30, 0x10034B24);
    st32_l(0x1046CD2C, 0x10036228);
    st32_l(0x1046CD38, 0x10034B24);
    st32_l(0x1046CD34, 0x10036230);
    stf_l(0x1046CCFC, 50000.0f);
    stf_l(0x101CEC50, -50.0f);
    stf_l(0x101CEC38, -50.0f);
    stf_l(0x1046CD04, 10000.0f);
    stf_l(0x101CED18, 30.0f);
    stf_l(0x101CED5C, 30.0f);
    stf_l(0x101CED60, 125.0f);
    stf_l(0x101CED1C, 125.0f);
    stf_l(0x101CEC44, 50.0f);
    /* a static ground check object at 1046D29C (dBgS_GndChk-like: cBgS_GndChk at +8, inline constructor) */
    const u32 g = 0x1046D29C;
    st32_l(g, 0x10037AF8);
    gabi::call(0x02008E0C /* cBgS_GndChk::cBgS_GndChk */, g + 8);
    st32_l(g + 0x08, g + 0x48);
    st32_l(g + 0x0C, g + 0x54);
    st32_l(g + 0x58, 1);
    for (u32 k = 0x4D; k <= 0x52; k++) gabi::store<u8>(g + k, 0);
    st32_l(g + 0x18, 0x10034CB4);
    st32_l(g + 0x28, 0x10034CC4);
    st32_l(g + 0x54, 0x10034CD4);
    st32_l(g + 0x48, 0x10034CE4);
    gabi::store<u8>(g + 0x4C, 1);
    __register_global_object(0x101CEF08);
    stf_l(0x1046CCE8, 67.5f);
    stf_l(0x1046CCEC, 95.0f);
}
VERIFY(0x02443EC4, __sinit_d_a_player_main_cpp);

/* ---- per-TU copies of inline constructors, destructors and accessors (after __sinit) ----
 * GHS emits one copy per translation unit; names are by content where the matcher has none. */
#define LK_NEW(size) gabi::call<u32>(0x0273AD10 /* operator new */, (u32)(size))
#define LK_DELETE(p) gabi::call(0x0273AF40 /* __dl */, (u32)(p))

/* 0244426C: an empty deleting destructor */
static void lk_emptyDtor_0244426C(u32 self, s32 flags) {
    WWHD_FUNC(0x0244426C, void, self, flags);
    if (self != 0 && (flags & 1)) LK_DELETE(self);
}
VERIFY(0x0244426C, lk_emptyDtor_0244426C);

/* 02444280: constructor of the 0x24 HD objects at daPy_lk_c + 0x48C */
static u32 lk_ct_02444280(u32 self) {
    WWHD_FUNC(0x02444280, u32, self);
    if (self == 0) {
        self = LK_NEW(0x24);
        if (self == 0) return 0;
    }
    gabi::store<u8>(self + 8, 0);
    gabi::store<u32>(self + 0x20, 0);
    return self;
}
VERIFY(0x02444280, lk_ct_02444280);

/* 024442C0: constructor of a 0x10 object {f32 0, 0, 0, 0} */
static u32 lk_ct_024442C0(u32 self) {
    WWHD_FUNC(0x024442C0, u32, self);
    if (self == 0) {
        self = LK_NEW(0x10);
        if (self == 0) return 0;
    }
    gabi::store<u32>(self + 0xC, 0);
    gabi::store<f32>(self + 0, 0.0f);
    gabi::store<u32>(self + 4, 0);
    gabi::store<u32>(self + 8, 0);
    return self;
}
VERIFY(0x024442C0, lk_ct_024442C0);

/* 02444310: J3DFrameCtrl::J3DFrameCtrl() */
static u32 J3DFrameCtrl_ct_l(u32 self) {
    WWHD_FUNC(0x02444310, u32, self);
    if (self == 0) {
        self = LK_NEW(0x10);
        if (self == 0) return 0;
    }
    gabi::call(0x027F2BC0 /* J3DFrameCtrl::init */, self, 0);
    return self;
}
VERIFY(0x02444310, J3DFrameCtrl_ct_l);

/* the 0x254 HD animation object (constructor 0244435C / 0244463C, destructor 02444988 & co.) */
static inline u32 lk_ct254(u32 self) {
    if (self == 0) {
        self = LK_NEW(0x254);
        if (self == 0) return 0;
    }
    gabi::call(0x027B5BD8, self + 4);
    gabi::call(0x027BF734, self + 0x158);
    gabi::store<u32>(self + 0x250, 0);
    gabi::store<u32>(self + 0x24C, 0);
    return self;
}
/* 0244435C */
static u32 lk_ct254_0244435C(u32 self) {
    WWHD_FUNC(0x0244435C, u32, self);
    return lk_ct254(self);
}
VERIFY(0x0244435C, lk_ct254_0244435C);

/* 024443B8: constructor of an empty 0x10 object */
static u32 lk_ct_024443B8(u32 self) {
    WWHD_FUNC(0x024443B8, u32, self);
    if (self == 0) {
        self = LK_NEW(0x10);
    }
    return self;
}
VERIFY(0x024443B8, lk_ct_024443B8);

/* 024443E4: daPy_footEffect_c::daPy_footEffect_c() */
static u32 daPy_footEffect_c_ct(u32 self) {
    WWHD_FUNC(0x024443E4, u32, self);
    if (self == 0) {
        self = LK_NEW(0x4C);
        if (self == 0) return 0;
    }
    gabi::call(0x025A5B18 /* dPa_smokeEcallBack::dPa_smokeEcallBack */, self, 1);
    gabi::call(0x025A5894 /* dPa_followEcallBack::dPa_followEcallBack */, self + 0x20, 0, 0);
    return self;
}
VERIFY(0x024443E4, daPy_footEffect_c_ct);

/* 02444440 / 02444480 / 024444C0: constructors that only store their vtable */
static inline u32 lk_ctVtbl(u32 self, u32 size, u32 vtbl) {
    if (self == 0) {
        self = LK_NEW(size);
        if (self == 0) return 0;
    }
    gabi::store<u32>(self, vtbl);
    return self;
}
static u32 lk_ct_02444440(u32 self) {
    WWHD_FUNC(0x02444440, u32, self);
    return lk_ctVtbl(self, 0x28, 0x10037C30);
}
VERIFY(0x02444440, lk_ct_02444440);
static u32 lk_ct_02444480(u32 self) {
    WWHD_FUNC(0x02444480, u32, self);
    return lk_ctVtbl(self, 0xC, 0x10037C70);
}
VERIFY(0x02444480, lk_ct_02444480);
static u32 lk_ct_024444C0(u32 self) {
    WWHD_FUNC(0x024444C0, u32, self);
    return lk_ctVtbl(self, 0x10, 0x10037CB0);
}
VERIFY(0x024444C0, lk_ct_024444C0);

/* 02444500: constructor of a 0x118 ground-check object (cBgS_GndChk at +0x34, probably dBgS_LinkGndChk) */
static u32 lk_ct_02444500(u32 self) {
    WWHD_FUNC(0x02444500, u32, self);
    if (self == 0) {
        self = LK_NEW(0x118);
        if (self == 0) return 0;
    }
    gabi::call(0x02008E0C /* cBgS_GndChk::cBgS_GndChk */, self + 0x34);
    gabi::store<u8>(self + 0x79, 0);
    gabi::store<u8>(self + 0x7A, 1);
    gabi::store<u32>(self + 0x84, 1);
    gabi::store<u8>(self + 0x78, 0);
    gabi::store<u32>(self + 0x38, self + 0x80);
    gabi::store<u32>(self + 0x74, 0x10034CA4);
    gabi::store<u32>(self + 0x54, 0x10034C84);
    gabi::store<u8>(self + 0x7D, 0);
    gabi::store<u32>(self + 0x44, 0x10034C74);
    gabi::store<u8>(self + 0x7C, 0);
    gabi::store<u32>(self + 0x80, 0x10034C94);
    gabi::store<u32>(self + 0x34, self + 0x74);
    gabi::store<u8>(self + 0x7E, 0);
    gabi::store<u8>(self + 0x7B, 0);
    return self;
}
VERIFY(0x02444500, lk_ct_02444500);

/* 024445B0: dCcD_Cps::dCcD_Cps() (0x138) */
static u32 dCcD_Cps_ct_l(u32 self) {
    WWHD_FUNC(0x024445B0, u32, self);
    if (self == 0) {
        self = LK_NEW(0x138);
        if (self == 0) return 0;
    }
    gabi::call(0x02515FB8 /* dCcD_GObjInf::dCcD_GObjInf */, self);
    gabi::store<u32>(self + 0x114, 0x100015A8);
    gabi::store<u32>(self + 0x110, 0x10034B9C);
    gabi::call(0x02018150 /* cM3dGCps::cM3dGCps */, self + 0x118);
    gabi::store<u32>(self + 0x3C, 0x1004AF18);
    gabi::store<u32>(self + 0x130, 0x1004AF60);
    gabi::store<u32>(self + 0x114, 0x1004AF70);
    return self;
}
VERIFY(0x024445B0, dCcD_Cps_ct_l);

/* 0244463C: the 0x254 animation object constructor (second copy) */
static u32 lk_ct254_0244463C(u32 self) {
    WWHD_FUNC(0x0244463C, u32, self);
    return lk_ct254(self);
}
VERIFY(0x0244463C, lk_ct254_0244463C);

/* 02444698: terminate a {char* buf, ?, size} string buffer (buf[size - 1] = 0) */
static void lk_bufTerminate_02444698(u32 self) {
    WWHD_FUNC(0x02444698, void, self);
    gabi::store<u8>(gabi::load<u32>(self + 0) + gabi::load<u32>(self + 8) - 1, 0);
}
VERIFY(0x02444698, lk_bufTerminate_02444698);

/* 024446B0: an empty deleting destructor */
static void lk_emptyDtor_024446B0(u32 self, s32 flags) {
    WWHD_FUNC(0x024446B0, void, self, flags);
    if (self != 0 && (flags & 1)) LK_DELETE(self);
}
VERIFY(0x024446B0, lk_emptyDtor_024446B0);

/* 024446C4: 2 if bit 2 of the word at (*(this + 8)) + 0xC is set, else 0 */
static s32 lk_024446C4(u32 self) {
    WWHD_FUNC(0x024446C4, s32, self);
    return (gabi::load<u32>(gabi::load<u32>(self + 8) + 0xC) & 4) ? 2 : 0;
}
VERIFY(0x024446C4, lk_024446C4);

/* 024446E0: the s16 at (*(this + 8)) + 0x12 */
static s16 lk_024446E0(u32 self) {
    WWHD_FUNC(0x024446E0, s16, self);
    return gabi::load<s16>(gabi::load<u32>(self + 8) + 0x12);
}
VERIFY(0x024446E0, lk_024446E0);

/* 024446EC / 02444740: deleting destructors over 027F13DC (base destructor) */
static inline void lk_dtorBase(u32 self, s32 flags, u32 base, u32 baseOff, s32 baseFlags) {
    if (self != 0) {
        gabi::call(base, self + baseOff, baseFlags);
        if (flags & 1) LK_DELETE(self);
    }
}
static void lk_dtor_024446EC(u32 self, s32 flags) {
    WWHD_FUNC(0x024446EC, void, self, flags);
    lk_dtorBase(self, flags, 0x027F13DC, 0, 0);
}
VERIFY(0x024446EC, lk_dtor_024446EC);
static void lk_dtor_02444740(u32 self, s32 flags) {
    WWHD_FUNC(0x02444740, void, self, flags);
    lk_dtorBase(self, flags, 0x027F13DC, 0, 0);
}
VERIFY(0x02444740, lk_dtor_02444740);

/* 02444794: deleting destructor over 02018034(this + 0x14, 2) */
static void lk_dtor_02444794(u32 self, s32 flags) {
    WWHD_FUNC(0x02444794, void, self, flags);
    lk_dtorBase(self, flags, 0x02018034, 0x14, 2);
}
VERIFY(0x02444794, lk_dtor_02444794);

/* the 0x254 animation object's deleting destructor (copies 024447E8, 02444988, 02444CC4) */
static inline void lk_dtor254(u32 self, s32 flags) {
    if (self != 0) {
        gabi::call(0x027BF880, self + 0x158, 2);
        gabi::call(0x027B5CBC, self + 4, 2);
        if (flags & 1) LK_DELETE(self);
    }
}
static void lk_dtor254_024447E8(u32 self, s32 flags) {
    WWHD_FUNC(0x024447E8, void, self, flags);
    lk_dtor254(self, flags);
}
VERIFY(0x024447E8, lk_dtor254_024447E8);

/* 02444848..0244485C: six empty functions (empty virtuals / callbacks) */
static void lk_empty_02444848() { WWHD_FUNC(0x02444848, void); }
VERIFY(0x02444848, lk_empty_02444848);
static void lk_empty_0244484C() { WWHD_FUNC(0x0244484C, void); }
VERIFY(0x0244484C, lk_empty_0244484C);
static void lk_empty_02444850() { WWHD_FUNC(0x02444850, void); }
VERIFY(0x02444850, lk_empty_02444850);
static void lk_empty_02444854() { WWHD_FUNC(0x02444854, void); }
VERIFY(0x02444854, lk_empty_02444854);
static void lk_empty_02444858() { WWHD_FUNC(0x02444858, void); }
VERIFY(0x02444858, lk_empty_02444858);
static void lk_empty_0244485C() { WWHD_FUNC(0x0244485C, void); }
VERIFY(0x0244485C, lk_empty_0244485C);

/* 02444860: *(this + 4) = v (a setter, probably an emitter callback's setup) */
static void lk_set4_02444860(u32 self, u32 v) {
    WWHD_FUNC(0x02444860, void, self, v);
    gabi::store<u32>(self + 4, v);
}
VERIFY(0x02444860, lk_set4_02444860);

/* 02444868 / 024448F4: deleting destructors of dBgS ground-check objects (vtables back to the base, then ~cBgS_Chk) */
static inline void lk_dtorGndChk(u32 self, s32 flags, u32 off) {
    if (self != 0) {
        gabi::store<u32>(self + off + 0x20, 0x10034C44);
        gabi::store<u32>(self + off + 0x40, 0x10034C64);
        gabi::store<u32>(self + off + 0x4C, 0x10034C24);
        gabi::call(0x02008DAC /* cBgS_Chk::~cBgS_Chk */, self + off, 0);
        if (flags & 1) LK_DELETE(self);
    }
}
static void lk_dtor_02444868(u32 self, s32 flags) {
    WWHD_FUNC(0x02444868, void, self, flags);
    lk_dtorGndChk(self, flags, 0x34);
}
VERIFY(0x02444868, lk_dtor_02444868);

/* 024448E0: an empty deleting destructor */
static void lk_emptyDtor_024448E0(u32 self, s32 flags) {
    WWHD_FUNC(0x024448E0, void, self, flags);
    if (self != 0 && (flags & 1)) LK_DELETE(self);
}
VERIFY(0x024448E0, lk_emptyDtor_024448E0);

static void lk_dtor_024448F4(u32 self, s32 flags) {
    WWHD_FUNC(0x024448F4, void, self, flags);
    lk_dtorGndChk(self, flags, 8);
}
VERIFY(0x024448F4, lk_dtor_024448F4);

/* 0244496C: daPy_fanSwingEcallBack_c::setup(JPABaseEmitter*, const cXyz*, const csXyz*, s8) */
static void daPy_fanSwingEcallBack_c_setup(u32 self, u32 emitter) {
    WWHD_FUNC(0x0244496C, void, self, emitter);
    gabi::store<u32>(emitter + 0x254, gabi::load<u32>(emitter + 0x254) | 0x40); /* becomeImmortalEmitter */
    gabi::store<u32>(self + 4, 0);
    gabi::store<u32>(self + 8, emitter);
}
VERIFY(0x0244496C, daPy_fanSwingEcallBack_c_setup);

static void lk_dtor254_02444988(u32 self, s32 flags) {
    WWHD_FUNC(0x02444988, void, self, flags);
    lk_dtor254(self, flags);
}
VERIFY(0x02444988, lk_dtor254_02444988);

static void lk_dtor254_02444CC4(u32 self, s32 flags) {
    WWHD_FUNC(0x02444CC4, void, self, flags);
    lk_dtor254(self, flags);
}
VERIFY(0x02444CC4, lk_dtor254_02444CC4);

/* the 0x254 animation object's destructor body as inlined into 024449E8: release its resource
 * (the heap holding *(+0x250) frees it: 02755FEC finds the heap, virtual free slot 0x3C) */
static inline void lk_dtor254_inline(u32 obj) {
    gabi::call(0x027BF7E8, obj + 0x158);
    u32 res = gabi::load<u32>(obj + 0x250);
    gabi::store<u32>(obj + 0, 0);
    if (res != 0) {
        u32 heap = gabi::call<u32>(0x02755FEC /* JKRHeap::findFromRoot */, gabi::load<u32>(0x101F8B4C), res);
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(heap + 0xC) + 0x3C), heap, gabi::load<u32>(obj + 0x250));
        gabi::store<u32>(obj + 0x24C, 0);
        gabi::store<u32>(obj + 0x250, 0);
    }
}

/* 024449E8: deleting destructor of a large HD object (vtable 10037BE8 at +0xC; 120 animation objects of
 * 0x254 at +0x660, a counted array of 0x23C entries at +0x11DD8, sub-objects up to +0x12214) */
static void lk_dtor_024449E8(u32 self, s32 flags) {
    WWHD_FUNC(0x024449E8, void, self, flags);
    if (self == 0) {
        return;
    }
    u32 arr = self + 0x660;
    gabi::store<u32>(self + 0xC, 0x10037BE8);
    for (u32 i = 0; i < 0x3C; i++) {
        u32 o = arr + i * 0x4A8;
        lk_dtor254_inline(o);
        lk_dtor254_inline(o + 0x254);
    }
    gabi::store<u32>(arr + 0x11770, 0);
    u32 b = self + 0x11DD8;
    for (s32 i = 0; i < (s32)gabi::load<u32>(b); i++) {
        u32 n = gabi::load<u32>(b);
        u32 e = gabi::load<u32>(b + 4);
        if ((u32)i < n) e += i * 0x23C;
        for (u32 k = 0; k < 2; k++) gabi::call(0x027BEBEC, e + 0x10 + k * 0x1C);
    }
    for (u32 k = 0; k < 2; k++) gabi::call(0x027BEBEC, b + 0x1C + k * 0x1C);
    for (u32 k = 0; k < 2; k++) gabi::call(0x027BEBEC, b + 0xC4 + k * 0x1C);
    gabi::call(0x027BE2B0, self + 0x12214, 2);
    gabi::call(0x027B54A0, self + 0x121F4, 2);
    gabi::call(0x027FB528, b + 0xB4, 0);
    gabi::call(0x027FB528, b + 0xC, 0);
    gabi::call(0x027FD764, b, 2);
    if (arr != 0) {
        for (u32 i = 0; i < 0x3C; i++) {
            u32 o = arr + i * 0x4A8;
            lk_dtor254_inline(o);
            lk_dtor254_inline(o + 0x254);
        }
        gabi::store<u32>(arr + 0x11770, 0);
        gabi::call(0x028F0164 /* __destroy_arr */, arr, 0x78, 0x254, 0x02444988u /* 0x254 destructor */, 0, 0);
    }
    gabi::call(0x027F13DC, self, 0);
    if (flags & 1) LK_DELETE(self);
}
VERIFY(0x024449E8, lk_dtor_024449E8);

/* 02444D24 / 02444D38 / 02444D4C: empty deleting destructors */
static void lk_emptyDtor_02444D24(u32 self, s32 flags) {
    WWHD_FUNC(0x02444D24, void, self, flags);
    if (self != 0 && (flags & 1)) LK_DELETE(self);
}
VERIFY(0x02444D24, lk_emptyDtor_02444D24);
static void lk_emptyDtor_02444D38(u32 self, s32 flags) {
    WWHD_FUNC(0x02444D38, void, self, flags);
    if (self != 0 && (flags & 1)) LK_DELETE(self);
}
VERIFY(0x02444D38, lk_emptyDtor_02444D38);
static void lk_emptyDtor_02444D4C(u32 self, s32 flags) {
    WWHD_FUNC(0x02444D4C, void, self, flags);
    if (self != 0 && (flags & 1)) LK_DELETE(self);
}
VERIFY(0x02444D4C, lk_emptyDtor_02444D4C);

/* ---- daPy_lk_c inline accessors (per-TU copies) ---- */

/* 02444D60: the f32 at +0x8A0 (mAcch: probably GetGroundH()) */
f32 daPy_lk_c::getGroundH_l() {
    WWHD_FUNC(0x02444D60, f32, this);
    return gabi::load<f32>(gabi::ea(this) + 0x8A0);
}
VERIFY(0x02444D60, &daPy_lk_c::getGroundH_l);

/* 02444D68 / 02444D88: mpCLModel->getAnmMtx(8) / getAnmMtx(12) (probably getLeftHandMatrix / getRightHandMatrix) */
Mtx34* daPy_lk_c::getAnmMtx8_l() {
    WWHD_FUNC(0x02444D68, Mtx34*, this);
    return lk_getAnmMtx(mpCLModel, 8);
}
VERIFY(0x02444D68, &daPy_lk_c::getAnmMtx8_l);
Mtx34* daPy_lk_c::getAnmMtx12_l() {
    WWHD_FUNC(0x02444D88, Mtx34*, this);
    return lk_getAnmMtx(mpCLModel, 12);
}
VERIFY(0x02444D88, &daPy_lk_c::getAnmMtx12_l);

/* 02444DA8: checkModeFlg(0x10452822) */
u32 daPy_lk_c::checkModeFlg_10452822_l() {
    WWHD_FUNC(0x02444DA8, u32, this);
    return mModeFlg & 0x10452822;
}
VERIFY(0x02444DA8, &daPy_lk_c::checkModeFlg_10452822_l);

/* 02444DBC: checkModeFlg(ModeFlg_00000002) || (checkModeFlg(0x40000) && mCurProc == 0x36) */
BOOL daPy_lk_c::check_02444DBC_l() {
    WWHD_FUNC(0x02444DBC, BOOL, this);
    u32 f = mModeFlg;
    if (f & 2) return TRUE;
    if ((f & 0x40000) && mCurProc == 0x36) return TRUE;
    return FALSE;
}
VERIFY(0x02444DBC, &daPy_lk_c::check_02444DBC_l);

/* 02444DEC / 02444E00 / 02444E14: mCurProc == 0x1E / 0xA5 / 0x59 */
BOOL daPy_lk_c::checkProc1E_l() {
    WWHD_FUNC(0x02444DEC, BOOL, this);
    return mCurProc == 0x1E;
}
VERIFY(0x02444DEC, &daPy_lk_c::checkProc1E_l);
BOOL daPy_lk_c::checkProcA5_l() {
    WWHD_FUNC(0x02444E00, BOOL, this);
    return mCurProc == 0xA5;
}
VERIFY(0x02444E00, &daPy_lk_c::checkProcA5_l);
BOOL daPy_lk_c::checkCutTurnMoveProc_l() {
    WWHD_FUNC(0x02444E14, BOOL, this);
    return mCurProc == 0x59; /* daPyProc_CUT_TURN_MOVE_e */
}
VERIFY(0x02444E14, &daPy_lk_c::checkCutTurnMoveProc_l);

/* 02444E28 / 02444E30: mFrameCtrlUnder[UNDER_MOVE0_e].getRate() / getFrame() */
f32 daPy_lk_c::getBaseAnimeRate_l() {
    WWHD_FUNC(0x02444E28, f32, this);
    return mFrameCtrlUnder[0].getRate();
}
VERIFY(0x02444E28, &daPy_lk_c::getBaseAnimeRate_l);
f32 daPy_lk_c::getBaseAnimeFrame_l() {
    WWHD_FUNC(0x02444E30, f32, this);
    return mFrameCtrlUnder[0].getFrame();
}
VERIFY(0x02444E30, &daPy_lk_c::getBaseAnimeFrame_l);

/* 02444E38 / 02444E40 / 02444E48: mActorKeepEquip / Throw / Grab .getID() */
u32 daPy_lk_c::getEquipActorID_l() {
    WWHD_FUNC(0x02444E38, u32, this);
    return mActorKeepEquip.mID;
}
VERIFY(0x02444E38, &daPy_lk_c::getEquipActorID_l);
u32 daPy_lk_c::getThrowActorID_l() {
    WWHD_FUNC(0x02444E40, u32, this);
    return mActorKeepThrow.mID;
}
VERIFY(0x02444E40, &daPy_lk_c::getThrowActorID_l);
u32 daPy_lk_c::getGrabActorID_l() {
    WWHD_FUNC(0x02444E48, u32, this);
    return mActorKeepGrab.mID;
}
VERIFY(0x02444E48, &daPy_lk_c::getGrabActorID_l);

/* 02444E50: checkGrabBarrelSearch(1) (tail call) */
BOOL daPy_lk_c::checkGrabBarrel_l() {
    WWHD_FUNC(0x02444E50, BOOL, this);
    return gabi::call<BOOL>(0x023DCF08 /* daPy_lk_c::checkGrabBarrelSearch */, this, 1);
}
VERIFY(0x02444E50, &daPy_lk_c::checkGrabBarrel_l);

/* 02444E58 */
BOOL daPy_lk_c::checkPlayerNoDraw() {
    WWHD_FUNC(0x02444E58, BOOL, this);
    s32 idx = mCameraInfoIdx;
    return (gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5B00) & 2) /* dComIfGp_checkCameraAttentionStatus(idx, 2) */ ||
           (mNoResetFlg0 & 0x08000000);
}
VERIFY(0x02444E58, &daPy_lk_c::checkPlayerNoDraw);

/* 02444EC4: mActorKeepEquip.getActor() == NULL */
BOOL daPy_lk_c::checkNoEquipActor_l() {
    WWHD_FUNC(0x02444EC4, BOOL, this);
    return mActorKeepEquip.mActor == nullptr;
}
VERIFY(0x02444EC4, &daPy_lk_c::checkNoEquipActor_l);

/* 02444ED4: checkUpperAnime(0xE4) (m_anm_heap_upper[UPPER_MOVE2_e].mIdx) */
BOOL daPy_lk_c::checkUpperAnimeE4_l() {
    WWHD_FUNC(0x02444ED4, BOOL, this);
    return gabi::load<u16>(gabi::ea(this) + 0x5888) == 0xE4;
}
VERIFY(0x02444ED4, &daPy_lk_c::checkUpperAnimeE4_l);

/* 02444EE8: mpCLModel->getAnmMtx(jnt) */
Mtx34* daPy_lk_c::getModelJointMtx_l(s32 jnt) {
    WWHD_FUNC(0x02444EE8, Mtx34*, this, jnt);
    return lk_getAnmMtx(mpCLModel, jnt);
}
VERIFY(0x02444EE8, &daPy_lk_c::getModelJointMtx_l);

/* 02444F0C: m3620 = v; onNoResetFlg0(daPyFlg0_UNK8) */
void daPy_lk_c::setFlg8_02444F0C_l(u32 v) {
    WWHD_FUNC(0x02444F0C, void, this, v);
    m3620 = v;
    mNoResetFlg0 = mNoResetFlg0 | 8;
}
VERIFY(0x02444F0C, &daPy_lk_c::setFlg8_02444F0C_l);

/* 02444F20 */
BOOL daPy_lk_c::checkComboCutTurn() {
    WWHD_FUNC(0x02444F20, BOOL, this);
    return mCurProc == 0x55 /* daPyProc_CUT_TURN_e */ && mProcVar6 != 0;
}
VERIFY(0x02444F20, &daPy_lk_c::checkComboCutTurn);

/* ---- leftover functions of the translation unit ---- */

/* 02444F48 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10034B38 after the destructor 0244426C, also called directly */
static void lk_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x02444F48, void, p);
}
VERIFY(0x02444F48, lk_SafeString_assureTermination);

/* 02444F4C sead::FixedSafeString<N>::~FixedSafeString (deleting; vtable slot 10034B48, next slot the buffered assureTerminationImpl_ 02444698) */
static void lk_FixedSafeString_dt_02444F4C(void* p, s32 flags) {
    WWHD_FUNC(0x02444F4C, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x02444F4C, lk_FixedSafeString_dt_02444F4C);

/* 02444F60 sead::FixedSafeString<N>::~FixedSafeString (deleting; vtable slot 10034B78, next slot 02444698) */
static void lk_FixedSafeString_dt_02444F60(void* p, s32 flags) {
    WWHD_FUNC(0x02444F60, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x02444F60, lk_FixedSafeString_dt_02444F60);
