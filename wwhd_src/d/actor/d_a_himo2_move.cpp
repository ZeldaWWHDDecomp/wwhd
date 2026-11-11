/**
 * d_a_himo2_move.cpp (WWHD)
 * Item - Grappling Hook / Rope: new_himo2_move (the rope state machine and the Helmaroc /
 * Gohma-style camera sequences)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_himo2.cpp, "Nonmatching - retail-only regalloc") to the WWHD
 * layout and code, verified against cking.rpx. "HD:" marks where WWHD's code differs.
 *
 * HD keeps the debug registers (REG0_F/REG0_S/REG8_F) of this TU; they are read through
 * the typed register-bank layout shared by the two source files.
 */
#include "d/actor/d_a_himo2.h"
#include "port_hooks.h"

enum {
    JA_SE_LK_ROPE_LAUNCH = 0x2817,
    JA_SE_LK_ROPE_COIL_1 = 0x2818,
    JA_SE_LK_ROPE_COIL_2 = 0x2819,
    JA_SE_LK_ROPE_COIL_3 = 0x281A,
    JA_SE_LK_ROPE_HOOK_WOOD = 0x281B,
    JA_SE_LK_ROPE_MAXLENGTH = 0x2820,
    JA_SE_LK_ROPE_UNCOIL = 0x2821,
    JA_SE_LK_ROPE_HOOK_BODY = 0x2830,
    JA_SE_LK_ROPE_SWING_ROUND = 0x2842,
    JA_SE_LK_ROPE_UNWIND = 0x201F,
    JA_SE_CV_DRG_ROCKFALL_1 = 0x483F,
    JA_SE_CV_DRG_ROCKFALL_1_2 = 0x4844,
    JA_SE_CV_DRG_ROCKFALL_1_3 = 0x4845,
    JA_SE_CV_DRG_ROCKFALL_2 = 0x4840,
    JA_SE_CV_DRG_SET_ROPE = 0x4841,
    JA_SE_CM_BTD_ROPE_SET = 0x5827,
    JA_SE_CM_BTD_BEF_ROCK_FALL = 0x5829,
    fpcNm_KUI_e = 0xFA,
    fpcNm_BK_e = 0xBD,
};

// HD: unit-local adapters retain the original argument and result widths.
static inline u32 mv_LockonTruth(u32 att) { return himo2_LockonTruth(att); }
// dAttention_c::LockonTarget(s32) (matcher: ActionTarget)
static inline u32 mv_LockonTarget(u32 att, s32 i) { return himo2_LockonTarget(att, i); }
static inline u32 mv_padTrigX() { return himo2_padTrigX(0); }
static inline u32 mv_padTrigY() { return himo2_padTrigY(0); }
static inline u32 mv_padTrigZ() { return himo2_padTrigZ(0); }
static inline f32 mv_subStickY() { return himo2_subStickY(0); }
static inline f32 mv_subStickX() { return himo2_subStickX(0); }
static inline void mv_camStop(himo2_camera_c* cam) { himo2_cameraStop(cam->mCamera); }
static inline void mv_camStart(himo2_camera_c* cam) { himo2_cameraStart(cam->mCamera); }
static inline void mv_camSetTrimSize(himo2_camera_c* cam, s32 s) {
    himo2_cameraTrim(cam->mCamera, s);
}
static inline void mv_camReset(himo2_camera_c* cam, cXyz* c, cXyz* e) {
    himo2_cameraReset(cam->mCamera, c, e);
}
static inline void mv_camSet(himo2_camera_c* cam, cXyz* c, cXyz* e, s16 fovy_bank, f32 f) {
    himo2_cameraSet(cam->mCamera, c, e, fovy_bank, f);
}
static inline void mv_colset(f32 f) {
    himo2_customColorSet(0, 4, f);
} /* dKy_custom_colset(0, 4, f) */
static inline void mv_orderPotentialEvent(fopAc_ac_c* a, u16 flag) {
    himo2_orderPotentialEvent(a, flag, (u16)0xFFFF, 0);
}
static inline fopAc_ac_c* mv_player() { return dComIfGp_getPlayer(0); }
/* fopAcM_seStart (HD: checks &eyePos != NULL; the actor pointer itself is not checked) */
static inline void mv_seStart(fopAc_ac_c* a, u32 id) {
    if (gabi::ea(a) + 0x37C != 0)
        mDoAud_seStart(id, &a->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* fopAcM_seStart(player, ...) (HD: checks the player and its eyePos) */
static inline void mv_seStartP(fopAc_ac_c* a, u32 id) {
    if (a != nullptr && gabi::ea(a) + 0x37C != 0)
        mDoAud_seStart(id, &a->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* fopAcM_seStartCurrent (HD: checks the actor and its position) */
static inline void mv_seStartCurrent(fopAc_ac_c* a, u32 id) {
    if (a != nullptr && gabi::ea(a) + 0x314 != 0)
        mDoAud_seStart(id, &a->current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline void mv_seStart0(u32 id) { mDoAud_seStart(id, nullptr, 0, 0); }
/* btd (0x10464678) and dr (0x1046467C) fields used here */
static inline void btd_setBlur(u8 v) { himo2_btd->mBlurRate = v; } /* btd->mBlurRate */
static inline void btd_onStatus4000() {
    himo2_btd_c* b = himo2_btd;
    b->actor_status |= fopAcStts_UNK4000_e;
}
static inline void btd_offStatus4000() {
    himo2_btd_c* b = himo2_btd;
    b->actor_status &= ~(u32)fopAcStts_UNK4000_e;
}
/* daYkgr_c::hide / show: the ykgr instance at 0x10475628, flags +0x254 */
static inline void ykgr_hide() {
    himo2_guideEmitter_c* emitter = l_guideEmitter;
    if (emitter)
        emitter->mStatus |= 4;
}
static inline void ykgr_show() {
    himo2_guideEmitter_c* emitter = l_guideEmitter;
    if (emitter)
        emitter->mStatus &= ~4u;
}
static inline f32 mv_abs(cXyz* v) { return std_sqrtf(PSVECSquareMag(v)); }
static inline void mv_pl_pos_add(himo2_class* i_this) { himo2_playerPosExternal(i_this); }
static inline BOOL mv_bg_check(himo2_class* i_this) { return himo2_bgCheckExternal(i_this); }
static inline void mv_posMove(himo2_class* i_this) {
    /* speed = Rotation(angle) * (0, 0, speedF); pos += speed */
    gabi::Local<cXyz> sp130;
    sp130->x = 0.0f;
    sp130->y = 0.0f;
    sp130->z = i_this->speedF;
    MtxPosition(sp130, &i_this->speed);
    PSVECAdd(&i_this->current.pos, &i_this->speed, &i_this->current.pos); /* PSVECAdd */
}
/* |pos - sp100| (cXyz::operator- then abs of the copy) */
static inline f32 mv_dist(himo2_class* i_this, cXyz* sp100) {
    gabi::Local<cXyz> tmp;
    *tmp.get() = i_this->current.pos - *sp100;
    gabi::Local<cXyz> sp130;
    *sp130.get() = *tmp.get();
    return mv_abs(sp130);
}
/* fopAcM_SearchByID (HD inline): fopAcIt_Judge(fpcSch_JudgeByID, &id) unless id == -1 */
static inline fopAc_ac_c* mv_searchByID(u32 id) {
    gabi::Local<be<u32>> pid;
    *pid.get() = id;
    if (id == 0xFFFFFFFF)
        return nullptr;
    return fopAcIt_Judge(0x025E1234, pid.get());
}

/* cases 5/8/9: pull the hook back to the hand */
static inline bool mv_pull_back(himo2_class* i_this, cXyz* sp100) {
    f32 step = (l_himo2Regs.mChild[18].mFloatRegs[13] + 400.0f) * i_this->mReturnSpeedRate;
    cLib_addCalcPos2(&i_this->current.pos, *sp100, 1.0f, step);
    cLib_addCalc2(&i_this->mReturnSpeedRate, 1.0f, 1.0f, l_himo2Regs.mChild[18].mFloatRegs[14] + 0.1f);
    mv_pl_pos_add(i_this);
    return mv_dist(i_this, sp100) > 5.0f;
}

/* the rope wound in the spin (cases 0 and 1) */
static inline void mv_spin(himo2_class* i_this, fopAc_ac_c* player, cXyz* sp100) {
    mDoMtx_YrotS(calc_mtx(), player->shape_angle.y);
    mDoMtx_ZrotM(calc_mtx(), (s16)(l_himo2Regs.mChild[0].mShortRegs[2] + -12000));
    mDoMtx_YrotM(calc_mtx(),
                 (s16)((s32)i_this->mFrameCounter * (l_himo2Regs.mChild[0].mShortRegs[3] + 0x2000)));
    gabi::Local<cXyz> sp130;
    sp130->x = 0.0f;
    sp130->y = 0.0f;
    sp130->z = l_himo2Regs.mChild[0].mFloatRegs[11] + 70.0f;
    MtxPosition(sp130, &i_this->current.pos);
    PSVECAdd(&i_this->current.pos, sp100, &i_this->current.pos); /* PSVECAdd */
    i_this->mRootSegment = l_himo2Regs.mChild[0].mShortRegs[5] + 0x60;
}

/* 0216F7FC */
static void new_himo2_move(himo2_class* i_this) {
    daPy_py_c* player = (daPy_py_c*)mv_player();
    s8 camId = dComIfGp_getPlayerCameraID(0); /* dComIfGp_getPlayerCameraID(0) */
    dComIfG_inf_c* play = (dComIfG_inf_c*)dComIfGp_get();
    f32 sz = i_this->mHandPos[0].z;
    f32 sx = i_this->mHandPos[0].x;
    f32 sy = i_this->mHandPos[0].y;
    himo2_camera_c* camera = gabi::at<himo2_camera_c>(play->play.mCameraInfo[camId].mpCamera);
    gabi::Local<cXyz> sp100;
    sp100->x = sx;
    sp100->y = sy;
    sp100->z = sz;
    f32 dy = i_this->current.pos.y - sy;
    f32 dx = i_this->current.pos.x - sx;
    f32 dz = i_this->current.pos.z - sz;
    i_this->mHandDist = std_sqrtf(gabi::fmadds(dz, dz, gabi::fmadds(dx, dx, dy * dy)));
    if (i_this->mStateTimer != 0)
        i_this->mStateTimer = i_this->mStateTimer - 1;
    if (i_this->mHitWait != 0)
        i_this->mHitWait = i_this->mHitWait - 1;
    if (i_this->mDemoTimer != 0)
        i_this->mDemoTimer = i_this->mDemoTimer - 1;
    if (i_this->mHideTimer != 0)
        i_this->mHideTimer = i_this->mHideTimer - 1;
    if (i_this->mCancelWait != 0)
        i_this->mCancelWait = i_this->mCancelWait - 1;

    u32 att = dComIfGp_ea() + 0x5804; /* dComIfGp_getAttention() */
    bool lockon = true;
    if (mv_LockonTruth(att) == 0 &&
        ((u32)gabi::at<himo2_attention_c>(att)->mFlags & 0x20000000) == 0)
        lockon = false;
    bool r30 = !lockon; /* attention->Lockon() == false */
    bool r27 = false;   /* a lock-on target */
    bool r25 = false;
    fopAc_ac_c* r24 = nullptr;
    if (r30) {
        rope_scale = l_himo2HIO.mRopeScale;
    } else {
        rope_scale = l_himo2HIO.mRopeScaleLockOn;
        i_this->mHideTimer = 0;
    }
    if (mv_LockonTruth(att) != 0) {
        r24 = gabi::at<fopAc_ac_c>(mv_LockonTarget(att, 0));
        if (r24 != nullptr) {
            i_this->mTargetPos = r24->eyePos;
            r27 = true;
        }
    }
    if (i_this->mState != HIMO2_WAIT && i_this->mDemoState < HIMO2_DEMO_SET_ROCK) {
        daPy_py_c* link = daPy_getPlayerLinkActorClass(); /* daPy_getPlayerLinkActorClass() */
        /* checkRopeForceEnd() && !eventInfo.checkCommandDemoAccrpt() */
        if (((u32)link->mResetFlg0 & daPy_py_c::daPyRFlg0_ROPE_FORCE_END) &&
            !i_this->eventInfo.checkCommandDemoAccrpt()) {
            i_this->mDemoState = HIMO2_DEMO_INACTIVE;
            i_this->mStateTimer = 30;
            s32 dc = i_this->mState;
            i_this->mParameters = 0;
            i_this->mWrapDrawMode = 0;
            if (dc == 11) {
                mv_seStart(i_this, JA_SE_LK_ROPE_UNCOIL);
                i_this->mState = HIMO2_WAIT;
            } else {
                i_this->mState = HIMO2_REBOUND;
            }
            fopAc_ac_c* t = i_this->mpTarget;
            if (t != nullptr && fpcM_GetName(t) == fpcNm_KUI_e)
                t->health = 0;
            i_this->mpTarget = nullptr;
        }
    }

    const u32 ropeState = i_this->mState;
    switch (ropeState) {
    case HIMO2_WAIT: {
        i_this->speedF = 0.0f;
        if (!r30 && gabi::call_ptr<u32>(himo2_playerVtable(player)->checkRopeReadyAnime, player) !=
                        0) { /* checkRopeReadyAnime */
            mv_spin(i_this, player, sp100);
            if ((i_this->mFrameCounter & 7) == 0)
                mv_seStart(i_this, JA_SE_LK_ROPE_COIL_1);
        } else {
            i_this->mRootSegment = 0x61 - gabi::ftoi(i_this->mHandDist *
                                              (l_himo2Regs.mChild[0].mFloatRegs[3] + 0.060000002f));
            cLib_addCalc2(&i_this->current.pos.x, sp100->x, 1.0f, 200.0f * i_this->mReturnSpeedRate);
            cLib_addCalc2(&i_this->current.pos.y, sp100->y, 1.0f, 200.0f * i_this->mReturnSpeedRate);
            cLib_addCalc2(&i_this->current.pos.z, sp100->z, 1.0f, 200.0f * i_this->mReturnSpeedRate);
            cLib_addCalc2(&i_this->mReturnSpeedRate, 1.0f, 1.0f, 0.005f);
            if (mv_dist(i_this, sp100) > 5.0f)
                mv_seStartP(player, JA_SE_LK_ROPE_UNWIND);
        }
        if (i_this->mParameters == 1) {
            i_this->mSagY = 0.0f;
            i_this->mReturnSpeedRate = 0.0f;
            i_this->mHideTimer = 0;
            i_this->mStateTimer = l_himo2Regs.mChild[18].mShortRegs[0] + 0x14;
            i_this->mCancelWait = l_himo2Regs.mChild[0].mShortRegs[9] + 10;
            i_this->mHitWait = 3;
            himo2_camera_c* cam2 =
                (himo2_camera_c*)dComIfGp_getCamera(0); /* dComIfGp_getCamera(0) */
            f32 f26 = i_this->mTargetPos.y - (f32)cam2->eye.y;
            f32 f25 = i_this->mTargetPos.z - (f32)cam2->eye.z;
            f32 f27 = i_this->mTargetPos.x - (f32)cam2->eye.x;
            f32 f24 = f27 * f27;
            f32 f23 = f25 * f25;
            gabi::Local<cXyz> sp130;
            sp130->z = gabi::fmadds(l_himo2Regs.mChild[0].mFloatRegs[15], 100.0f, 1000.0f);
            if (std_sqrtf(gabi::fmadds(f26, f26, f24) + f23) > sp130->z) {
                s16 r24_2 = cM_atan2s(f27, f25);
                s16 r22 = (s16)-cM_atan2s(f26, std_sqrtf(f24 + f23));
                mDoMtx_YrotS(calc_mtx(), r24_2);
                mDoMtx_XrotM(calc_mtx(), r22);
                sp130->x = 0.0f;
                sp130->y = 0.0f;
                gabi::Local<cXyz> sp124;
                MtxPosition(sp130, sp124);
                i_this->mTargetPos.x = (f32)cam2->eye.x + sp124->x;
                i_this->mTargetPos.y = (f32)cam2->eye.y + sp124->y;
                i_this->mTargetPos.z = (f32)cam2->eye.z + sp124->z;
            }
            if (r30) {
                i_this->mState = HIMO2_LAUNCH_FREE;
            } else {
                i_this->mState = HIMO2_SPIN;
                i_this->mStateTimer = l_himo2Regs.mChild[0].mShortRegs[4] + 2;
            }
            i_this->mSwayAmp = l_himo2Regs.mChild[18].mFloatRegs[14] + 4.0f;
            i_this->current.pos = *sp100.get();
            i_this->mRootSegment = l_himo2Regs.mChild[0].mShortRegs[6] + 0x5A;
            himo2_s* phVar18 = i_this->mSegments;
            for (int iVar8 = 0; iVar8 < 100; iVar8++, phVar18++)
                phVar18->mPos = *sp100.get();
        }
        break;
    }
    case 1: {
        mv_spin(i_this, player, sp100);
        if ((i_this->mFrameCounter & 7) == 0)
            mv_seStart(i_this, JA_SE_LK_ROPE_SWING_ROUND);
        s16 r4 = (s16)((s32)i_this->mFrameCounter * (l_himo2Regs.mChild[0].mShortRegs[3] + 0x2000));
        s16 r8 = l_himo2Regs.mChild[0].mShortRegs[7];
        s16 lo = (s16)(r8 + -20000);
        if (r4 >= lo && r4 < (s16)(r8 + -0x2E20))
            r25 = true;
        if (i_this->mStateTimer == 0 && r25) {
            i_this->mState = HIMO2_LAUNCH_TARGET;
            if (r27) {
                i_this->mpTarget = r24;
                i_this->mStateTimer = l_himo2Regs.mChild[0].mShortRegs[4] + 70;
                i_this->mTargetId = fopAcM_GetID(r24); /* fopAcM_GetID */
            } else {
                i_this->mTargetId = 0xFFFFFFFF;
                i_this->mStateTimer = l_himo2Regs.mChild[0].mShortRegs[2] + 20;
            }
            gabi::Local<cXyz> tmp;
            *tmp.get() = i_this->mTargetPos - i_this->current.pos; /* cXyz::operator- */
            gabi::Local<cXyz> sp130;
            f32 y = tmp->y, x = tmp->x, z = tmp->z;
            sp130->x = x;
            sp130->z = z;
            sp130->y = y;
            i_this->current.angle.y = cM_atan2s(x, z);
            f32 zz = sp130->z;
            f32 xx = sp130->x;
            f32 d = std_sqrtf(gabi::fmadds(xx, xx, zz * zz));
            i_this->current.angle.x = (s16)-cM_atan2s(sp130->y, d);
            mv_seStart(i_this, JA_SE_LK_ROPE_LAUNCH);
        }
        break;
    }
    case 2: {
        i_this->mRootSegment = l_himo2Regs.mChild[0].mShortRegs[0];
        f32 f27, f26, f25;
        fopAc_ac_c* t = i_this->mpTarget;
        if (t != nullptr) {
            /* HD: always the target's position (GameCube: mTargetPos for plain kui) */
            f32 tz = t->current.pos.z, tx = t->current.pos.x;
            s16 ay = i_this->current.angle.y;
            f32 ty = t->current.pos.y;
            mDoMtx_YrotS(calc_mtx(), ay);
            mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
            gabi::Local<cXyz> sp130;
            sp130->y = 0.0f;
            sp130->x = 0.0f;
            sp130->z = gabi::fmadds(l_himo2Regs.mChild[0].mFloatRegs[14], 100.0f, 100.0f);
            gabi::Local<cXyz> sp124;
            MtxPosition(sp130, sp124);
            f27 = (tx + sp124->x) - i_this->current.pos.x;
            f25 = (tz + sp124->z) - i_this->current.pos.z;
            f26 = (ty + sp124->y) - i_this->current.pos.y;
        } else {
            f27 = i_this->mTargetPos.x - i_this->current.pos.x;
            f26 = i_this->mTargetPos.y - i_this->current.pos.y;
            f25 = i_this->mTargetPos.z - i_this->current.pos.z;
        }
        f32 f24 = f27 * f27;
        f32 f23 = f25 * f25;
        f32 f22 = std_sqrtf(gabi::fmadds(f26, f26, f24) + f23);
        f32 f27_2 = std_sqrtf(f24 + f23);
        s16 r31 = (s16)gabi::ftoi(f22 * (l_himo2Regs.mChild[0].mFloatRegs[3] + -5.0f));
        s16 lim = (s16)(l_himo2Regs.mChild[0].mShortRegs[2] + -3000);
        if (r31 < lim)
            r31 = lim;
        if (i_this->mpTarget != nullptr || f22 > i_this->speedF * 10.0f) {
            i_this->current.angle.y = cM_atan2s(f27, f25);
            i_this->current.angle.x = (s16)-cM_atan2s(f26, f27_2);
        }
        i_this->speedF = l_himo2Regs.mChild[18].mFloatRegs[0] + 50.0f; /* HD: 50 (GameCube 20) */
        mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        mDoMtx_XrotM(calc_mtx(), (s16)(i_this->current.angle.x + r31));
        mv_posMove(i_this);
        mv_pl_pos_add(i_this);
        if (i_this->mpTarget != nullptr) {
            /* HD: also when mStateTimer <= 10 (GameCube: mStateTimer == 0) */
            if (f22 < i_this->speedF * 10.0f || i_this->mStateTimer <= 10) {
                i_this->mState = HIMO2_HOOKED;
                i_this->mDemoState = HIMO2_DEMO_REQUEST;
                i_this->mWrapDrawMode = 1;
                if (himo2_btd != 0)
                    btd_offStatus4000();
                break;
            }
            if (i_this->mHitWait == 0 && i_this->mSph.ChkAtHit()) {
                i_this->mStateTimer = 0x28;
                i_this->mpTarget = nullptr;
                i_this->speedF = -i_this->speedF;
                i_this->mState = HIMO2_REBOUND;
                break;
            }
        } else if (!mv_bg_check(i_this)) {
            if (i_this->mHitWait == 0 && i_this->mSph.ChkAtHit()) {
                i_this->mStateTimer = 0x28;
                i_this->mState = HIMO2_REBOUND;
                i_this->speedF = -i_this->speedF;
                break;
            }
            if (i_this->mStateTimer == 0) {
                i_this->mState = HIMO2_MAX_LENGTH;
                i_this->mStateTimer = 50;
                mv_seStart(i_this, JA_SE_LK_ROPE_MAXLENGTH);
                break;
            }
        } else {
            break;
        }
        if ((mv_padTrigX() || mv_padTrigY() || mv_padTrigZ()) && i_this->mCancelWait == 0) {
            i_this->mState = HIMO2_MAX_LENGTH;
            i_this->mStateTimer = 0x28;
            mv_seStart(i_this, JA_SE_LK_ROPE_MAXLENGTH);
            i_this->mpTarget = nullptr;
        }
        break;
    }
    case 3: {
        u32 id = i_this->mTargetId;
        i_this->mRootSegment = l_himo2Regs.mChild[0].mShortRegs[0];
        fopAc_ac_c* r4_r27 = mv_searchByID(id);
        bool r26 = false;
        if (r4_r27 != nullptr) {
            gabi::Local<cXyz> tmp;
            *tmp.get() = r4_r27->eyePos - i_this->current.pos; /* cXyz::operator- */
            gabi::Local<cXyz> sp130;
            f32 x = tmp->x, y = tmp->y, z = tmp->z;
            sp130->x = x;
            sp130->y = y;
            sp130->z = z;
            s16 a = cM_atan2s(x, z);
            cLib_addCalcAngleS2(&i_this->current.angle.y, a, 2,
                                (s16)(l_himo2Regs.mChild[18].mShortRegs[9] + 0x1000));
            f32 zz = sp130->z;
            f32 xx = sp130->x;
            f32 d = std_sqrtf(gabi::fmadds(xx, xx, zz * zz));
            s16 b = cM_atan2s(sp130->y, d);
            cLib_addCalcAngleS2(&i_this->current.angle.x, (s16)-b, 2,
                                (s16)(l_himo2Regs.mChild[18].mShortRegs[9] + 0x1000));
            if (mv_abs(sp130) < l_himo2Regs.mChild[0].mFloatRegs[3] + 50.0f)
                r26 = true;
        }
        i_this->speedF = l_himo2Regs.mChild[18].mFloatRegs[1] + 50.0f; /* HD: 50 (GameCube 30) */
        mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        mv_posMove(i_this);
        mv_pl_pos_add(i_this);
        BOOL bg = mv_bg_check(i_this);
        s16 a2 = i_this->mHitWait;
        if (bg) {
            r26 = true;
            i_this->mReturnSpeedRate = 0.1f;
        }
        if (a2 == 0 && i_this->mSph.ChkAtHit()) {
            r26 = true;
            i_this->mReturnSpeedRate = 0.1f;
        }
        if (i_this->mStateTimer == 0) {
            r26 = true;
            i_this->mReturnSpeedRate = 0.02f;
        }
        if (((mv_padTrigX() || mv_padTrigY() || mv_padTrigZ()) && i_this->mCancelWait == 0) || r26)
            i_this->mState = HIMO2_PULL_BACK;
        break;
    }
    case 4: {
        fopAc_ac_c* r4_r27 = mv_searchByID(i_this->mTargetId);
        if (r4_r27 == nullptr) {
            i_this->mState = HIMO2_PULL_BACK;
        } else {
            mDoMtx_YrotS(calc_mtx(), (s16)(i_this->mFrameCounter << 13));
            gabi::Local<cXyz> sp130;
            sp130->y = 0.0f;
            sp130->z = i_this->mWrapRadius;
            sp130->x = 0.0f;
            gabi::Local<cXyz> sp124;
            MtxPosition(sp130, sp124);
            gabi::Local<cXyz> tmp;
            *tmp.get() = r4_r27->eyePos + *sp124.get(); /* cXyz::operator+ */
            i_this->current.pos = *tmp.get();
            cLib_addCalc0(&i_this->mWrapRadius, 1.0f, 5.0f);
            if (i_this->mWrapRadius < 0.1f)
                i_this->mParameters = 0;
            s32 r3 = 0x61 - gabi::ftoi(i_this->mHandDist *
                                       (l_himo2Regs.mChild[0].mFloatRegs[5] + 0.060000002f));
            if (r3 < i_this->mRootSegment || i_this->mWrapRadius < 0.1f)
                i_this->mRootSegment = r3;
            if (mv_padTrigX() || mv_padTrigY() || mv_padTrigZ()) {
                i_this->mState = HIMO2_PULL_BACK;
                if (r4_r27 != nullptr && fpcM_GetName(r4_r27) == fpcNm_BK_e) {
                    /* bk->dr.mAction = 0; bk->dr.mMode = 0 */
                    ((himo2_bk_c*)r4_r27)->dr.mAction = 0;
                    ((himo2_bk_c*)r4_r27)->dr.mMode = 0;
                }
            }
        }
        i_this->mSagY = -5.0f;
        break;
    }
    case 5: {
        /* HD: no mRootSegment = REG0_S(0); the pull uses cLib_addCalcPos2 */
        if (mv_pull_back(i_this, sp100)) {
            mv_seStartP(player, JA_SE_LK_ROPE_UNWIND);
        } else {
            i_this->mParameters = 0;
            mv_seStart(i_this, JA_SE_LK_ROPE_UNCOIL);
            i_this->mpTarget = nullptr;
            i_this->mWrapDrawMode = 0;
            i_this->mState = HIMO2_WAIT;
        }
        break;
    }
    case 8:
        cLib_addCalc2(&i_this->speedF, -50.0f, 1.0f, l_himo2Regs.mChild[18].mFloatRegs[12] + 6.0f);
        /* fallthrough */
    case 9: {
        if (mv_pull_back(i_this, sp100)) {
            mv_seStartP(player, JA_SE_LK_ROPE_UNWIND);
        } else {
            i_this->mParameters = 0;
            i_this->current.pos = *sp100.get();
            i_this->mState = HIMO2_WAIT;
            i_this->mHideTimer = l_himo2Regs.mChild[0].mShortRegs[6] + 0x14;
        }
        break;
    }
    case 10:
    case 11: {
        if (ropeState == 10) {
            i_this->mRootSegment = 100 - gabi::ftoi(i_this->mHandDist *
                                             (l_himo2Regs.mChild[0].mFloatRegs[1] + 0.060000002f));
            i_this->current.pos = i_this->mHangPos;
            cLib_addCalc2(&i_this->mSagY, -6.25f, 1.0f, 0.375f);
            if (i_this->mParameters == 3) {
                i_this->mState = HIMO2_HANG;
                if ((fopAcM_GetParam(i_this->mpTarget) & 0xF0) != 0)
                    dComIfGs_onEventBit(0x540);
                else
                    dComIfGs_onEventBit(0x580);
            }
            i_this->mpTarget->health = 1;
        } else {
            i_this->current.pos = i_this->mHangPos;
            i_this->mRootSegment = 100 - gabi::ftoi(i_this->mHandDist *
                                             (l_himo2Regs.mChild[0].mFloatRegs[2] + 0.060000002f));
            i_this->mpTarget->health = 3;
        }
        if (i_this->mParameters == 4) {
            i_this->mParameters = 0;
            mv_seStart(i_this, JA_SE_LK_ROPE_UNCOIL);
            fopAc_ac_c* t = i_this->mpTarget;
            i_this->mState = HIMO2_WAIT;
            t->health = 0;
            s8 d9 = i_this->mDemoState;
            i_this->mpTarget = nullptr;
            if (d9 != 0) {
                i_this->mDemoState = HIMO2_DEMO_JUMP;
                i_this->mDemoTimer = 120;
            }
            i_this->mWrapDrawMode = 0;
        }
        break;
    }
    default:
        break;
    }

    f32 f27 = 0.0f; /* the mWrapOffsetZ step (case 2) */
    f32 f26 = 0.0f; /* the sub stick Y (case 5) */
    s8 d9 = i_this->mDemoState;
    bool stickBlock = false;
    switch (d9) {
    case HIMO2_DEMO_REQUEST: {
        if (!i_this->eventInfo.checkCommandDemoAccrpt()) { /* !eventInfo.checkCommandDemoAccrpt() */
            mv_orderPotentialEvent(i_this, 1 /* dEvtFlag_NOPARTNER_e */);
            i_this->eventInfo.onCondition(dEvtCnd_UNK2_e);
            break;
        }
        i_this->mCamFovy = l_himo2Regs.mChild[18].mFloatRegs[11] + 50.0f;
        mv_camStop(camera);
        mv_camSetTrimSize(camera, 2);
        s8 mWrapMirror = i_this->mWrapMirror;
        i_this->mDemoState = HIMO2_DEMO_COIL;
        f32 b4 = l_himo2Regs.mChild[18].mFloatRegs[5] + -400.0f;
        fopAc_ac_c* t = i_this->mpTarget;
        i_this->mWrapAngleStep = 0;
        i_this->mHookAngleX = 0;
        i_this->mWrapOffsetZ = b4;
        i_this->mWrapSpeed = 0.0f;
        i_this->mWrapCount = 0;
        s16 ay = t->current.angle.y;
        if (mWrapMirror != 0)
            i_this->mCamAngleY = (s16)(ay + -0x4000);
        else
            i_this->mCamAngleY = (s16)(ay + 0x4000);
        if ((fopAcM_GetParam(t) & 0xF0) != 0) {
            i_this->mCamEyeSignX = -1.0f;
            i_this->mWrapOffsetX = l_himo2Regs.mChild[0].mFloatRegs[6] + -90.0f;
            i_this->mCamEyeSignZ = l_himo2Regs.mChild[0].mFloatRegs[7] + 1.0f;
            i_this->mCamEyeDist = l_himo2Regs.mChild[0].mFloatRegs[16] + 3.5f;
        } else {
            /* HD: no l_himo2HIO.mSwingDirMode choice: always (-1, -1) */
            i_this->mWrapOffsetX = 0.0f;
            i_this->mCamEyeSignX = -1.0f;
            i_this->mCamEyeSignZ = -1.0f;
            i_this->mCamEyeDist = l_himo2Regs.mChild[18].mFloatRegs[6] + 3.0f;
        }
        i_this->mCamCenter = i_this->mpTarget->current.pos;
    }
        /* fallthrough */
    case HIMO2_DEMO_COIL: {
        cLib_addCalc0(&i_this->mWrapOffsetX, 1.0f, l_himo2Regs.mChild[18].mFloatRegs[4] + 3.0f);
        if ((fopAcM_GetParam(i_this->mpTarget) & 0xF0) == 0)
            f27 = l_himo2Regs.mChild[18].mFloatRegs[3] + 10.0f;
        /* HD: step 10 (+ REG + 10 for plain kui) */
        cLib_addCalc2(&i_this->mWrapOffsetZ, -144.0f, 1.0f, f27 + 10.0f);
        if (i_this->mWrapOffsetZ > -145.0f) {
            s32 bc = i_this->mWrapCount;
            if (bc < 400) {
                f32 f1 = gabi::fnmsubs(i_this->mpTarget->scale.y - 1.0f, 17.0f, 20.0f);
                f32 c4 = i_this->mWrapSpeed;
                if (f1 < 0.5f)
                    f1 = 0.5f;
                s32 r3 = gabi::ftoi(c4 / f1);
                s32 r5 = r3 + 1;
                i_this->mOldWrapCount = bc;
                i_this->mWrapCount = i_this->mWrapCount + r5;
                i_this->mWrapSpeed = c4 + (l_himo2Regs.mChild[18].mFloatRegs[2] +
                                      2.0f); /* HD: + REG + 2 (GameCube + 1) */
                if (bc == 0) {
                    mv_seStart(i_this, JA_SE_LK_ROPE_COIL_1);
                } else if (bc >= 13) {
                    s32 nbc = i_this->mWrapCount;
                    if (nbc <= r5 + 13) {
                        mv_seStart(i_this, JA_SE_LK_ROPE_COIL_2);
                    } else if (bc >= 26) {
                        if (nbc <= r5 + 26 || (bc >= 39 && nbc <= r5 + 39))
                            mv_seStart(i_this, JA_SE_LK_ROPE_COIL_3);
                    }
                }
            }
            fopAc_ac_c* t = i_this->mpTarget;
            u32 prm = fopAcM_GetParam(t);
            f32 f3 = t->scale.y;
            if ((prm & 0x0F) == 3)
                f3 = (f3 - 0.14f) + (f32)l_himo2Regs.mChild[8].mFloatRegs[7]; /* REG8_F(7) */
            s16 r4 = (s16)gabi::ftoi(gabi::fnmsubs(2000.0f, f3 - 1.0f, 4000.0f));
            cLib_addCalcAngleS2(&i_this->mWrapAngleStep, r4, 1, (s16)(r4 / 8));
            if (i_this->mWrapCount > 49) {
                s16 ca = i_this->mHookAngleX;
                if (ca < 9000) {
                    i_this->mHookAngleX = 9000;
                    if (gabi::ea(i_this) + 0x37C != 0) {
                        u32 p = fopAcM_GetParam(i_this->mpTarget);
                        mDoAud_seStart(
                            (p & 0xF0) != 0 ? JA_SE_LK_ROPE_HOOK_BODY : JA_SE_LK_ROPE_HOOK_WOOD,
                            &i_this->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
                    }
                } else if (ca == 9000) {
                    i_this->mHookAngleX = 0x238C;
                }
            }
        }
        cLib_addCalc2(&i_this->mCamCenter.x, i_this->mpTarget->current.pos.x, 0.3f, 100.0f);
        cLib_addCalc2(&i_this->mCamCenter.y, i_this->mpTarget->current.pos.y, 0.3f, 100.0f);
        cLib_addCalc2(&i_this->mCamCenter.z, i_this->mpTarget->current.pos.z, 0.3f, 100.0f);
        i_this->mCamEye = i_this->mpTarget->current.pos;
        mDoMtx_YrotS(calc_mtx(), i_this->mCamAngleY);
        gabi::Local<cXyz> sp130;
        f32 z4c = l_himo2Regs.mChild[18].mFloatRegs[9] + 120.0f;
        f32 x44 = l_himo2Regs.mChild[18].mFloatRegs[7] + 10.0f;
        sp130->y = l_himo2Regs.mChild[18].mFloatRegs[8] + -85.0f;
        sp130->x = x44 * i_this->mCamEyeSignX;
        sp130->z = z4c * i_this->mCamEyeSignZ;
        gabi::Local<cXyz> sp124;
        MtxPosition(sp130, sp124);
        f32 fc = i_this->mCamEyeDist;
        f32 nx = gabi::fmadds(sp124->x, fc, i_this->mCamEye.x);
        f32 ny = gabi::fmadds(sp124->y, fc, i_this->mCamEye.y);
        f32 nz = gabi::fmadds(sp124->z, fc, i_this->mCamEye.z);
        i_this->mCamEye.x = nx;
        i_this->mCamEye.y = ny;
        i_this->mCamEye.z = nz;
        if ((fopAcM_GetParam(i_this->mpTarget) & 0xF0) != 0)
            cLib_addCalc2(&i_this->mCamEyeDist, l_himo2Regs.mChild[0].mFloatRegs[6] + 1.8f,
                          l_himo2Regs.mChild[0].mFloatRegs[4] + 0.2f,
                          l_himo2Regs.mChild[0].mFloatRegs[5] + 0.1f);
        else
            cLib_addCalc2(&i_this->mCamEyeDist, l_himo2Regs.mChild[18].mFloatRegs[10] + 1.2f,
                          l_himo2Regs.mChild[18].mFloatRegs[4] + 0.2f,
                          l_himo2Regs.mChild[18].mFloatRegs[5] + 0.2f);
        break;
    }
    case HIMO2_DEMO_SET_ROCK: {
        i_this->mpTarget->health = 2;
        mDoMtx_YrotS(calc_mtx(), i_this->mpTarget->current.angle.y);
        gabi::Local<cXyz> sp130;
        sp130->x = l_himo2Regs.mChild[0].mFloatRegs[11] + 900.0f;
        sp130->y = (i_this->mpTarget->home.pos.y + l_himo2Regs.mChild[0].mFloatRegs[12]) + 2000.0f;
        sp130->z = l_himo2Regs.mChild[0].mFloatRegs[13] + 1000.0f;
        gabi::Local<cXyz> sp124;
        MtxPosition(sp130, sp124);
        cLib_addCalc2(&i_this->mCamEye.x, sp124->x, 0.3f, 200.0f);
        cLib_addCalc2(&i_this->mCamEye.y, sp124->y, 0.3f, 200.0f);
        cLib_addCalc2(&i_this->mCamEye.z, sp124->z, 0.3f, 200.0f);
        cLib_addCalc2(&i_this->mCamCenter.y,
                      (i_this->mpTarget->home.pos.y + 2500.0f) + l_himo2Regs.mChild[0].mFloatRegs[14],
                      0.3f, 100.0f);
        cLib_addCalc2(&i_this->mCamFovy, l_himo2Regs.mChild[0].mFloatRegs[13] + 80.0f, 1.0f, 2.0f);
        if (i_this->mDemoTimer <= 30) {
            if (i_this->mDemoTimer < 30)
                btd_setBlur(0xB4);
            if (i_this->mDemoTimer == 30)
                mv_seStartCurrent(himo2_dr, JA_SE_CM_BTD_ROPE_SET);
        }
        if (gabi::load<u8>(gabi::ea(dComIfGp_getStartStageName())) !=
            'X') { /* dComIfGp_getStartStageName()[0] */
            if (i_this->mDemoTimer > 1)
                break;
            if (!dComIfGs_isEventBit(0x420))
                mv_colset(1.0f);
        }
        if (i_this->mDemoTimer != 0 || l_himo2Regs.mChild[0].mShortRegs[8] != 0)
            break;
        btd_setBlur(1);
        i_this->mDemoState = HIMO2_DEMO_DROP_ROCK;
        if (gabi::load<u8>(gabi::ea(dComIfGp_getStartStageName())) == 'X' ||
            dComIfGs_isEventBit(0x420)) {
            i_this->mRockfallSeStep = 0;
            fopAc_ac_c* t = i_this->mpTarget;
            i_this->mDemoTimer = 0;
            t->health = 0;
        } else {
            dComIfGs_onEventBit(0x420);
            i_this->mDemoTimer = l_himo2Regs.mChild[0].mShortRegs[2] + 0x3E;
            ykgr_hide();
            fopAc_ac_c* t = i_this->mpTarget;
            i_this->mRockfallSeStep = 0;
            t->health = 0;
        }
        if (himo2_btd != 0)
            btd_onStatus4000();
    }
        /* fallthrough */
    case HIMO2_DEMO_DROP_ROCK: {
        mv_colset(1.0f);
        if ((u32)(s32)i_this->mDemoTimer == (u32)(s32)(s16)(l_himo2Regs.mChild[0].mShortRegs[2] + 0x39))
            mv_seStart0(JA_SE_CV_DRG_SET_ROPE);
        bool big = false;
        if (i_this->mDemoTimer <= 1) {
            mv_colset(0.0f);
            if (i_this->mDemoTimer == 0 && l_himo2Regs.mChild[0].mShortRegs[8] == 0)
                big = true;
        }
        if (big) {
            himo2_dr->mShowDragon = 0; /* dr->mShowDragon */
            mv_colset(0.0f);
            i_this->mRockfallSeStep = 0;
            i_this->mDemoTimer = 50;
            i_this->mDemoState = HIMO2_DEMO_CONTROL;
            i_this->mParameters = 2;
            dComIfGp_getEvent()->reset();
            i_this->mControlEndWait = 20;
            s16 ang = fopAcM_searchPlayerAngleY(i_this); /* fopAcM_searchPlayerAngleY */
            s16 a = (s16)(ang + 0x8000);
            i_this->mCamAngleY = a;
            i_this->mCamFovy = 65.0f;
            i_this->mCamAngleYTarget = a;
            i_this->mCamFovyTarget = 65.0f;
            mDoMtx_YrotS(calc_mtx(), a);
            gabi::Local<cXyz> sp130;
            sp130->x = l_himo2Regs.mChild[0].mFloatRegs[7] + 300.0f;
            sp130->y = (player->current.pos.y + 700.0f) + l_himo2Regs.mChild[0].mFloatRegs[8];
            sp130->z = l_himo2Regs.mChild[0].mFloatRegs[9] + -500.0f;
            MtxPosition(sp130, &i_this->mCamEye);
            i_this->mCamEye.x =
                gabi::fmadds(player->current.pos.x, l_himo2Regs.mChild[0].mFloatRegs[15] + 0.55f,
                             i_this->mCamEye.x);
            i_this->mCamEye.z =
                gabi::fmadds(player->current.pos.z, l_himo2Regs.mChild[0].mFloatRegs[15] + 0.55f,
                             i_this->mCamEye.z);
            i_this->mCamCenter.x = player->current.pos.x;
            f32 py = player->current.pos.y;
            i_this->mCamCenter.y = py;
            i_this->mCamCenter.z = player->current.pos.z;
            i_this->mCamCenter.y = py - 50.0f;
            ykgr_show();
        } else {
            i_this->mCamFovy = l_himo2Regs.mChild[0].mFloatRegs[12] + 50.0f;
            himo2_dr->mShowDragon = 1;
            i_this->mCamEye.x = l_himo2Regs.mChild[0].mFloatRegs[14] + -5000.0f;
            i_this->mCamEye.y = l_himo2Regs.mChild[0].mFloatRegs[15] + 21720.0f;
            i_this->mCamEye.z = l_himo2Regs.mChild[0].mFloatRegs[16] + 4000.0f;
            i_this->mCamCenter.x = l_himo2Regs.mChild[0].mFloatRegs[17];
            i_this->mCamCenter.y = l_himo2Regs.mChild[0].mFloatRegs[18] + 21420.0f;
            i_this->mCamCenter.z = l_himo2Regs.mChild[0].mFloatRegs[19];
        }
        break;
    }
    case HIMO2_DEMO_CONTROL: {
        if (i_this->mDemoTimer == 0) {
            s8 e0 = i_this->mRockfallSeStep;
            if (e0 == 0) {
                mv_seStart0(JA_SE_CV_DRG_ROCKFALL_1);
                i_this->mDemoTimer = 50;
                i_this->mRockfallSeStep = (s8)(i_this->mRockfallSeStep + 1);
            } else if (e0 == 1) {
                mv_seStart0(JA_SE_CV_DRG_ROCKFALL_1_2);
                i_this->mDemoTimer = 50;
                i_this->mRockfallSeStep = (s8)(i_this->mRockfallSeStep + 1);
            } else {
                if (e0 >= 2) {
                    mv_seStart0(JA_SE_CV_DRG_ROCKFALL_1_3);
                    e0 = 2;
                }
                i_this->mRockfallSeStep = (s8)(e0 + 1);
                i_this->mDemoTimer = 50;
            }
        }
        f26 = PORT_CAMERA_F(0x02171B1C, nullptr, mv_subStickY()); /* the port's camera controls (port_hooks.h) */
        stickBlock = true;
        break;
    }
    case HIMO2_DEMO_JUMP: {
        if (!((u32)player->mResetFlg0 & daPy_py_c::daPyRFlg0_ROPE_JUMP_LAND) &&
            i_this->mDemoTimer != 0) { /* !getRopeJumpLand() */
            if (!i_this->eventInfo.checkCommandDemoAccrpt()) {
                mv_orderPotentialEvent(i_this, 2 /* dEvtFlag_STAFF_ALL_e */);
                i_this->eventInfo.onCondition(dEvtCnd_UNK2_e);
            }
            himo2_dr->mRockFallTimer = 0x5A; /* dr->mRockFallTimer */
            cLib_addCalc2(&i_this->mCamFovyTarget, l_himo2Regs.mChild[0].mFloatRegs[8] + 20.0f, 1.0f,
                          l_himo2Regs.mChild[0].mFloatRegs[9] + 2.0f);
            stickBlock = true; // Keep the sub-stick step at zero.
            break;
        }
        i_this->mDemoState = HIMO2_DEMO_RESET;
        i_this->mDemoTimer = 10;
        break;
    }
    case HIMO2_DEMO_RESET: {
        if (i_this->mDemoTimer != 0)
            break;
        if ((s16)himo2_dr->mRockState >= 10) { /* dr->mRockState */
            gabi::Local<cXyz> c;
            gabi::Local<cXyz> e;
            c->x = i_this->mCamCenter.x;
            c->y = i_this->mCamCenter.y;
            e->z = i_this->mCamEye.z;
            e->x = i_this->mCamEye.x;
            c->z = i_this->mCamCenter.z;
            i_this->mDemoState = HIMO2_DEMO_INACTIVE;
            e->y = i_this->mCamEye.y;
            mv_camReset(camera, c, e);
            mv_camStart(camera);
            mv_camSetTrimSize(camera, 0);
            dComIfGp_getEvent()->reset();
            break;
        }
        i_this->mCamFovy = l_himo2Regs.mChild[0].mFloatRegs[12] + 50.0f;
        btd_onStatus4000();
        i_this->mDemoState = HIMO2_DEMO_BOSS_FALL;
        i_this->mCamCenter.x = 0.0f;
        i_this->mCamCenter.y = gabi::fmadds(l_himo2Regs.mChild[0].mFloatRegs[5], 0.1f, 3000.0f);
        i_this->mCamCenter.z = l_himo2Regs.mChild[0].mFloatRegs[6] * 0.1f;
        mDoMtx_YrotS(calc_mtx(), (s16)himo2_btd->current.angle.y);
        {
            gabi::Local<cXyz> sp130;
            sp130->x = l_himo2Regs.mChild[0].mFloatRegs[7] * 0.1f;
            sp130->y = gabi::fmadds(l_himo2Regs.mChild[0].mFloatRegs[8], 0.1f, 1000.0f);
            sp130->z = gabi::fmadds(l_himo2Regs.mChild[0].mFloatRegs[9], 0.1f, 1400.0f);
            MtxPosition(sp130, &i_this->mCamEye);
        }
        i_this->mDemoTimer = 210;
        i_this->mCamFovy = l_himo2Regs.mChild[0].mFloatRegs[13] + 80.0f;
        {
            gabi::Local<dBgS_LinChk> lin_chk;
            dBgS_LinChk_ct(lin_chk, dBgS_LinChk_vt{0x10010EA4, 0x10010EB4, 0x10010ED4, 0x10010EC4},
                           false);
            lin_chk->Set(&i_this->mCamCenter, &i_this->mCamEye, i_this); /* dBgS_LinChk::Set */
            if (dComIfG_Bgsp()->LineCross(lin_chk.get()) != 0)    /* cBgS::LineCross */
                i_this->mCamEye = lin_chk->mLin.mEnd;               /* GetCross() */
            /* ~dBgS_LinChk */
            lin_chk->dBgS_PolyPassChk::__vtbl = 0x10010ED4;
            lin_chk->dBgS_GrpPassChk::__vtbl = 0x10010E24;
            lin_chk->cBgS_PolyInfo::__vtbl = 0x10010E14;
            himo2_destroyLineCheck(lin_chk.get(), 0);
        }
    }
        /* fallthrough */
    case HIMO2_DEMO_BOSS_FALL: {
        himo2_dr->mTailState = 3; /* dr->mTailState */
        if (i_this->mDemoTimer == 200)
            mv_seStart0(JA_SE_CV_DRG_ROCKFALL_2);
        if (i_this->mDemoTimer == 170)
            mv_seStartCurrent(himo2_dr, JA_SE_CM_BTD_BEF_ROCK_FALL);
        himo2_dr_c* dr = himo2_dr;
        f32 f1 = i_this->mDemoTimer > 150 ? 2000.0f : 100.0f;
        cLib_addCalc2(&dr->mShakeAmplitude, f1, 0.5f, 100.0f); /* dr->mShakeAmplitude */
        if (i_this->mDemoTimer > 120)
            break;
        cLib_addCalc2(&i_this->mCamCenter.y,
                      gabi::fmadds(l_himo2Regs.mChild[0].mFloatRegs[5], 0.1f, 650.0f), 1.0f,
                      100.0f);
        cLib_addCalc2(&i_this->mCamEye.y, 0.0f, 0.5f, 100.0f);
        if ((u32)(s32)i_this->mDemoTimer ==
            (u32)(s32)(s16)(l_himo2Regs.mChild[0].mShortRegs[4] + 0x6E)) {
            i_this->mCamShake = l_himo2Regs.mChild[0].mFloatRegs[11] + 50.0f;
            btd_setBlur(0xB4);
        }
        if (i_this->mDemoTimer > 80)
            break;
        cLib_addCalc2(&i_this->mCamFovy, l_himo2Regs.mChild[0].mFloatRegs[14] + 40.0f, 0.5f, 2.0f);
        if (i_this->mDemoTimer == 2)
            btd_setBlur(1);
        if (i_this->mDemoTimer != 0)
            break;
        if ((u8)himo2_btd->mCrackStage >= 3) { /* btd->mCrackStage */
            i_this->mDemoState = HIMO2_DEMO_BOSS_END;
            i_this->mDemoTimer = 220;
            himo2_dr->mTailState = 0;
        } else {
            i_this->mDemoState = HIMO2_DEMO_INACTIVE;
            f32 ey = player->eyePos.y;
            f32 ex = player->eyePos.x;
            f32 ez = player->eyePos.z;
            gabi::Local<cXyz> c;
            gabi::Local<cXyz> spDC;
            c->x = ex;
            c->y = player->eyePos.y;
            c->z = player->eyePos.z;
            spDC->x = ex * 0.9f;
            spDC->y = ey;
            spDC->z = ez * 0.9f;
            mv_camReset(camera, c, spDC);
            mv_camStart(camera);
            mv_camSetTrimSize(camera, 0);
            btd_offStatus4000();
            dComIfGp_getEvent()->reset();
            himo2_dr->mTailState = 0;
        }
        break;
    }
    case HIMO2_DEMO_BOSS_END: {
        cLib_addCalc2(&i_this->mCamFovy, l_himo2Regs.mChild[0].mFloatRegs[4] + 50.0f, 0.5f, 3.0f);
        f32 by = (f32)himo2_btd->eyePos.y; /* btd->actor.eyePos.y */
        cLib_addCalc2(&i_this->mCamCenter.y,
                      gabi::fmadds(l_himo2Regs.mChild[0].mFloatRegs[5], 0.1f, by) + 200.0f, 0.2f,
                      1000.0f);
        mDoMtx_YrotS(calc_mtx(), (s16)himo2_btd->current.angle.y);
        gabi::Local<cXyz> sp130;
        sp130->x = gabi::fmadds(l_himo2Regs.mChild[0].mFloatRegs[7], 0.1f, -300.0f);
        sp130->y = gabi::fmadds(l_himo2Regs.mChild[0].mFloatRegs[8], 0.1f, 100.0f);
        sp130->z = gabi::fmadds(l_himo2Regs.mChild[0].mFloatRegs[9], 0.1f, 2000.0f);
        gabi::Local<cXyz> sp124;
        MtxPosition(sp130, sp124);
        cLib_addCalc2(&i_this->mCamEye.x, sp124->x, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->mCamEye.y, sp124->y, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->mCamEye.z, sp124->z, 0.1f, 50.0f);
        if (i_this->mDemoTimer == 99)
            btd_setBlur(0xB4);
        if (i_this->mDemoTimer == 40)
            btd_setBlur(1);
        if (i_this->mDemoTimer == 0) {
            i_this->mDemoState = HIMO2_DEMO_INACTIVE;
            f32 ex = player->eyePos.x;
            f32 ez = player->eyePos.z;
            f32 ey = player->eyePos.y;
            gabi::Local<cXyz> c;
            gabi::Local<cXyz> spD0;
            c->x = ex;
            c->y = player->eyePos.y;
            c->z = player->eyePos.z;
            spD0->x = ex * 0.9f;
            spD0->y = ey;
            spD0->z = ez * 0.9f;
            mv_camReset(camera, c, spD0);
            mv_camStart(camera);
            mv_camSetTrimSize(camera, 0);
            btd_offStatus4000();
            dComIfGp_getEvent()->reset();
        }
        break;
    }
    default:
        break;
    }

    if (stickBlock) {
        /* label_1d50 */
        f32 f2 = mv_subStickX();
        s32 add = gabi::ftoi(f2 * (l_himo2Regs.mChild[0].mFloatRegs[6] + 1000.0f));
        i_this->mCamAngleYTarget = (s16)(i_this->mCamAngleYTarget + add);
        cLib_addCalcAngleS2(&i_this->mCamAngleY, i_this->mCamAngleYTarget, 4, 0x1000);
        if (!(f26 > -0.1f)) {
            cLib_addCalc2(&i_this->mCamFovyTarget, l_himo2Regs.mChild[0].mFloatRegs[7] + 70.0f, 1.0f,
                          std::fabs(f26) * 3.0f);
        } else if (!(f26 < 0.1f)) {
            cLib_addCalc2(&i_this->mCamFovyTarget, l_himo2Regs.mChild[0].mFloatRegs[8] + 20.0f, 1.0f,
                          std::fabs(f26) * 3.0f);
        }
        cLib_addCalc2(&i_this->mCamFovy, i_this->mCamFovyTarget, 0.1f, 10.0f);
        mDoMtx_YrotS(calc_mtx(), i_this->mCamAngleY);
        gabi::Local<cXyz> sp130;
        sp130->x = l_himo2Regs.mChild[0].mFloatRegs[7] + 300.0f;
        sp130->y = (player->current.pos.y + 700.0f) + l_himo2Regs.mChild[0].mFloatRegs[8];
        sp130->z = l_himo2Regs.mChild[0].mFloatRegs[9] + -500.0f;
        MtxPosition(sp130, &i_this->mCamEye);
        f32 nx = gabi::fmadds(player->current.pos.x, l_himo2Regs.mChild[0].mFloatRegs[15] + 0.55f,
                              i_this->mCamEye.x);
        f32 oz = i_this->mCamEye.z;
        i_this->mCamEye.x = nx;
        i_this->mCamEye.z =
            gabi::fmadds(player->current.pos.z, l_himo2Regs.mChild[0].mFloatRegs[15] + 0.55f, oz);
        cLib_addCalc2(&i_this->mCamCenter.x, player->current.pos.x, 0.3f, 100.0f);
        cLib_addCalc2(&i_this->mCamCenter.y,
                      (player->current.pos.y - 50.0f) + l_himo2Regs.mChild[0].mFloatRegs[10], 0.3f,
                      100.0f);
        cLib_addCalc2(&i_this->mCamCenter.z, player->current.pos.z, 0.3f, 100.0f);
        if (i_this->mControlEndWait == 0 && gabi::call_ptr<u32>(himo2_playerVtable(player)->checkPlayerFly,
                                                      player) == 0) { /* !checkPlayerFly() */
            mv_camStart(camera);
            i_this->mDemoState = HIMO2_DEMO_INACTIVE;
            i_this->mWrapDrawMode = 0;
            i_this->mpTarget = nullptr;
            i_this->mState = HIMO2_WAIT;
            i_this->mParameters = 0;
            dComIfGp_getEvent()->reset();
        }
    }

    if (i_this->mDemoState > HIMO2_DEMO_INACTIVE) {
        u32 d8 = i_this->mFrameCounter;
        f32 mCamShake = i_this->mCamShake;
        f32 f27_3 = cM_ssin((s32)(d8 * 0x3300)) * mCamShake;
        f32 f1 = cM_scos((s32)(d8 * 0x3000)) * mCamShake;
        f32 c3 = cM_scos((s32)(d8 * 0x1C00));
        gabi::Local<cXyz> spB8;
        gabi::Local<cXyz> spC4;
        f32 dcx = i_this->mCamEye.x;
        f32 e8x = i_this->mCamCenter.x;
        f32 e8y = i_this->mCamCenter.y;
        f32 e8z = i_this->mCamCenter.z;
        spB8->y = e8y + f1;
        spB8->z = e8z;
        spB8->x = e8x + f27_3;
        spC4->x = dcx + f27_3;
        spC4->y = i_this->mCamEye.y + f1;
        s16 r23 = (s16)gabi::ftoi((c3 * mCamShake) * 7.5f);
        spC4->z = i_this->mCamEye.z;
        mv_camSet(camera, spB8, spC4, r23, i_this->mCamFovy);
        cLib_addCalc0(&i_this->mCamShake, 1.0f, l_himo2Regs.mChild[0].mFloatRegs[16] + 2.0f);
        fopAc_ac_c* t = i_this->mpTarget;
        s16 r23_2 = 0;
        s32 bc = i_this->mWrapCount;
        if (t != nullptr && (fopAcM_GetParam(t) & 0xF0) != 0)
            r23_2 = -50;
        if (bc > (s16)(r23_2 + 0x82) && l_himo2Regs.mChild[0].mShortRegs[8] == 0) {
            if ((fopAcM_GetParam(t) & 0xF0) != 0) {
                if (i_this->mDemoState == HIMO2_DEMO_COIL) {
                    if ((s16)himo2_dr->mRockState >= 10) {
                        i_this->mDemoTimer = 0;
                        i_this->mDemoState = HIMO2_DEMO_DROP_ROCK;
                        i_this->mWrapDrawMode = 2;
                        if (himo2_btd != 0)
                            btd_onStatus4000();
                    } else {
                        i_this->mDemoState = HIMO2_DEMO_SET_ROCK;
                        i_this->mDemoTimer = l_himo2Regs.mChild[0].mShortRegs[2] + 0x41;
                    }
                    i_this->mWrapDrawMode = 2;
                }
            } else {
                mv_camStart(camera);
                i_this->mDemoState = HIMO2_DEMO_INACTIVE;
                i_this->mParameters = 2;
                i_this->mWrapDrawMode = 2;
                dComIfGp_getEvent()->reset();
            }
        }
    }

    if (i_this->mRootSegment < 2)
        i_this->mRootSegment = 2;
}

// HD: compiler-generated functions of this translation unit.

// daHimo2_c::~daHimo2_c (deleting; vtable slot 10010F00): members in reverse order, then fopAc_ac_c
/* 021726E0 */
static void daHimo2_c_dt(void* p, s32 flags) {
    if (p != nullptr) {
        himo2_class* i_this = (himo2_class*)p;
        __dt(&i_this->mAcch);
        __dt(&i_this->mAcchCir);
        himo2_destroyPacket(&i_this->mPackets[3], 2);
        himo2_destroyPacket(&i_this->mPackets[2], 2);
        himo2_destroyPacket(&i_this->mPackets[1], 2);
        himo2_destroyPacket(&i_this->mPackets[0], 2);
        himo2_destroySphere(&i_this->mSph, 2);
        __dt(&i_this->mStts);
        __dt((mDoExt_3DlineMat1_c*)&i_this->mHandLine);
        __dt((mDoExt_3DlineMat1_c*)&i_this->mCoilLine);
        __dt((mDoExt_3DlineMat1_c*)&i_this->mRopeLine);
        __dt((fopAc_ac_c*)p);
        if (flags & 1)
            operator_delete(p);
    }
}

// sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10010E00, after the
// destructor 0216F7E8
/* 021727D0 */
static void himo2_SafeString_assureTermination(void* p) {}

/* WWHD glue
ext 02082DDC void himo2_destroyPacket(himo2_modelPacket_c*, s32)
ext 02515AE8 void himo2_destroySphere(dCcD_Sph*, s32)
ext 020078E8 u32 himo2_padTrigX(u32)
ext 02007914 u32 himo2_padTrigY(u32)
ext 02007814 u32 himo2_padTrigZ(u32)
ext 02007B20 f32 himo2_subStickY(u32)
ext 02007AFC f32 himo2_subStickX(u32)
ext 02514F2C void himo2_cameraStop(void*)
ext 02514F38 void himo2_cameraStart(void*)
ext 02515280 void himo2_cameraTrim(void*, s32)
ext 0251510C void himo2_cameraReset(void*, cXyz*, cXyz*)
ext 02514FE8 void himo2_cameraSet(void*, cXyz*, cXyz*, s16, f32)
ext 0255FDA4 void himo2_customColorSet(u32, u32, f32)
ext 025D7B24 void himo2_orderPotentialEvent(fopAc_ac_c*, u16, u16, u32)
ext 0216E144 void himo2_playerPosExternal(himo2_class*)
ext 0216DE7C BOOL himo2_bgCheckExternal(himo2_class*)
ext 024EDFCC u32 himo2_LockonTruth(u32)
ext 024EC8D0 u32 himo2_LockonTarget(u32, s32)
ext 02008B4C void himo2_destroyLineCheck(dBgS_LinChk*, s32)
*/
