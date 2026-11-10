/**
 * d_a_bridge_exec.cpp (WWHD)
 * Rope bridge: daBridge_Execute (with bridge_move, control1/2/3, cut_control1/2 and search_aite
 * inlined, as GHS compiled them).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bridge.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bridge.h"

/* functions of d_a_bridge.cpp, called by address */

// HD: Rope and collision helpers preserve the original caller workspaces.
/* 0257DB04 dKyw_get_wind_power (f32*), 0257DAA8 dKyw_get_wind_vec (cXyz*) (as d_a_npc_ji1_exec) */

/* dComIfGp_event_runCheck(): the play object's event flag (u8 at play+0x5292) */
static inline bool dComIfGp_event_runCheck() {
    return gabi::at<BridgeHD::EventState>(dComIfGp_ea())->mRunning != 0;
}
/* 028F43F8 sin (double) */

/* 02518DB0 BridgeHD::SDK::at_power_check(CcAtInfo*) (as d_a_bk_exec) */

/* JPABaseEmitter (HD): setGlobalRTMatrix = JPASetRMtxTVecfromMtx(m, +0x1F0, +0x22C);
 * becomeInvalidEmitter: +0x5C = -1, flags +0x254 |= 1 (as d_a_sss) */

static inline void JPA_becomeInvalidEmitter(u32 e) {
    u32 f = (u32)(gabi::at<BridgeHD::Emitter>(e)->mFlags);
    gabi::at<BridgeHD::Emitter>(e)->mMaxFrame = 0xFFFFFFFF;
    gabi::at<BridgeHD::Emitter>(e)->mFlags = f | 1;
}
/* cM_scos: the cosine of the sin/cos table (8 bytes per entry) */
static inline f32 bridge_getCos(s32 a) { return BridgeHD::getCos(a); }
static inline void copy_words(u32 dst, u32 src) {
    gabi::at<BridgeHD::VectorWords>(dst)->mWords[0] = (u32)(gabi::at<BridgeHD::VectorWords>(src)->mWords[0]);
    gabi::at<BridgeHD::VectorWords>(dst)->mWords[1] = (u32)(gabi::at<BridgeHD::VectorWords>(src)->mWords[1]);
    gabi::at<BridgeHD::VectorWords>(dst)->mWords[2] = (u32)(gabi::at<BridgeHD::VectorWords>(src)->mWords[2]);
}
static inline void copy_floats(cXyz *dst, const cXyz *src) {
    f32 x = src->x;
    dst->x = x;
    f32 y = src->y;
    dst->y = y;
    f32 z = src->z;
    dst->z = z;
}
static inline f32 bridge_getPlankInfluence(int j) { return ita_z_p[5 + j]; }
static inline br_s *br_at(bridge_class *i_this, int i) {
    return gabi::at<br_s>(gabi::ea(&i_this->mBr[0]) + i * 0x680);
}
static inline f32 wind_power() { return (f32)*wp.get(); }
static inline s16 wind_y() { return (s16)(wy); }

/* control1 (inline): the planks from the start, swinging */
static inline void control1(bridge_class *i_this, br_s *pBr) {
    gabi::Local<cXyz> sp3C;
    gabi::Local<cXyz> sp30;
    gabi::Local<cXyz> sp24;
    gabi::Local<cXyz> sp18;
    gabi::Local<cXyz> sp0C;

    pBr++;
    s16 ec = i_this->mWavePhaseH, ee = i_this->mWavePhaseV, f0 = i_this->mWaveStepH, f2 = i_this->mWaveStepV;
    i_this->mWavePhaseH = (s16)(ec + f0);
    i_this->mWavePhaseV = (s16)(ee + f2);
    s16 sVar13 = i_this->mBrCount > 10 ? 4000 : 8000;

    sp3C->y = 0.0f;
    sp3C->z = 0.0f;
    sp3C->x = bridge_getCos(i_this->mWavePhaseH) * i_this->mSwingAmp;
    cMtx_YrotS(calc_mtx(), i_this->home.angle.y);
    MtxPosition(sp3C, sp24);
    sp3C->x = 1.0f;
    MtxPosition(sp3C, sp18);
    sp3C->x = 0.0f;
    sp3C->z = wind_power() * 5.0f;
    cMtx_YrotS(calc_mtx(), wind_y());
    MtxPosition(sp3C, sp0C);
    sp3C->x = 0.0f;
    sp3C->z = 75.0f;

    for (int i = 1; i < i_this->mBrCount; i++, pBr++) {
        br_s *prev = pBr - 1;
        f32 mSwayRate = pBr->mSwayRate;
        f32 tmp = gabi::fmadds(pBr->mSagY, 0.5f, gabi::fmadds(pBr->mDroopY * mSwayRate, 0.5f, pBr->mCtrlPos.y));
        f32 sinA = cM_ssin(i_this->mWavePhaseH + i * sVar13);
        f32 sinB = cM_ssin(i_this->mWavePhaseV + i * (sVar13 + 1000));
        f32 fVar8 = sinA * i_this->mWaveAmpH * mSwayRate;
        f32 x =
            gabi::fmadds(sp24->x, mSwayRate, gabi::fmadds(fVar8, sp18->x, pBr->mCtrlPos.x - prev->mCtrlPos.x)) + sp0C->x;
        f32 y = gabi::fmadds(sinB * i_this->mWaveAmpV, mSwayRate, tmp - prev->mCtrlPos.y);
        f32 z =
            gabi::fmadds(sp24->z, mSwayRate, gabi::fmadds(fVar8, sp18->z, pBr->mCtrlPos.z - prev->mCtrlPos.z)) + sp0C->z;

        s16 atan = cM_atan2s(x, z);
        s16 atan2 = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        cMtx_YrotS(calc_mtx(), atan);
        cMtx_XrotM(calc_mtx(), atan2);
        MtxPosition(sp3C, sp30);
        pBr->mCtrlPos.x = prev->mCtrlPos.x + sp30->x;
        pBr->mCtrlPos.y = prev->mCtrlPos.y + sp30->y;
        pBr->mCtrlPos.z = prev->mCtrlPos.z + sp30->z;
    }
}

/* control2 (inline): the planks from the end */
static inline void control2(bridge_class *i_this, br_s *pBr) {
    gabi::Local<cXyz> sp18;
    gabi::Local<cXyz> sp0C;
    pBr += i_this->mBrCount - 2;
    sp18->x = 0.0f;
    sp18->y = 0.0f;
    sp18->z = 75.0f;
    for (int i = 0; i < i_this->mBrCount - 1; i++, pBr--) {
        br_s *next = pBr + 1;
        f32 z = pBr->mCtrlPos.z - next->mCtrlPos.z;
        f32 tmp = gabi::fmadds(pBr->mSagY, 0.5f, gabi::fmadds(pBr->mDroopY * pBr->mSwayRate, 0.5f, pBr->mCtrlPos.y));
        f32 x = pBr->mCtrlPos.x - next->mCtrlPos.x;
        f32 y = tmp - next->mCtrlPos.y;
        s16 atan = cM_atan2s(x, z);
        s16 atan2 = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        next->mRotation.y = atan;
        next->mRotation.x = atan2;
        cMtx_YrotS(calc_mtx(), atan);
        cMtx_XrotM(calc_mtx(), atan2);
        MtxPosition(sp18, sp0C);
        pBr->mCtrlPos.x = next->mCtrlPos.x + sp0C->x;
        pBr->mCtrlPos.y = next->mCtrlPos.y + sp0C->y;
        pBr->mCtrlPos.z = next->mCtrlPos.z + sp0C->z;
    }
}

/* control3 (inline): the first plank's angle */
static inline void control3(bridge_class *i_this, br_s *pBr) {
    f32 x = pBr->mCtrlPos.x - pBr[1].mCtrlPos.x;
    f32 z = pBr->mCtrlPos.z - pBr[1].mCtrlPos.z;
    f32 y = pBr->mCtrlPos.y - pBr[1].mCtrlPos.y;
    pBr->mRotation.y = cM_atan2s(x, z);
    pBr->mRotation.x = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
}

/* cut_control1 (inline): the half hanging from the start after the bridge broke */
static inline void cut_control1(bridge_class *i_this, br_s *pBr) {
    gabi::Local<cXyz> sp24;
    gabi::Local<cXyz> sp18;
    gabi::Local<cXyz> spC;
    pBr++;
    cMtx_YrotS(calc_mtx(), i_this->home.angle.y);
    sp24->z = 1.0f;
    sp24->x = 0.0f;
    sp24->y = 0.0f;
    MtxPosition(sp24, spC);
    sp24->z = 75.0f;

    for (int i = 1; i < i_this->mBreakBrNo; i++, pBr++) {
        br_s *prev = pBr - 1;
        f32 fVar1 = pBr->mGroundY + 30.0f;
        f32 fVar2 = pBr->mCtrlPos.y + pBr->mDroopY;
        if (fVar2 < fVar1) {
            pBr->mGroundHitCount = pBr->mGroundHitCount + 1;
            fVar2 = fVar1;
        }
        f32 x = (pBr->mCtrlPos.x - prev->mCtrlPos.x) + spC->x;
        f32 z = (pBr->mCtrlPos.z - prev->mCtrlPos.z) + spC->z;
        f32 y = fVar2 - prev->mCtrlPos.y;
        s16 atan = cM_atan2s(x, z);
        s16 atan2 = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        s16 ry = (s16)(atan + 0x8000);
        s16 rx = (s16)-atan2;
        prev->mRotation.y = ry;
        prev->mRotation.x = rx;
        if ((u32)i == (u32)(i_this->mBreakBrNo - 1)) {
            pBr->mRotation.y = ry;
            pBr->mRotation.x = rx;
        }
        cMtx_YrotS(calc_mtx(), atan);
        cMtx_XrotM(calc_mtx(), atan2);
        MtxPosition(sp24, sp18);
        pBr->mCtrlPos.x = prev->mCtrlPos.x + sp18->x;
        pBr->mCtrlPos.y = prev->mCtrlPos.y + sp18->y;
        pBr->mCtrlPos.z = prev->mCtrlPos.z + sp18->z;
    }
}

/* cut_control2 (inline): the half hanging from the end */
static inline void cut_control2(bridge_class *i_this, br_s *pBr) {
    gabi::Local<cXyz> sp24;
    gabi::Local<cXyz> sp18;
    gabi::Local<cXyz> spC;
    pBr += i_this->mBrCount - 2;
    cMtx_YrotS(calc_mtx(), i_this->home.angle.y);
    sp24->y = 0.0f;
    sp24->z = -1.0f;
    sp24->x = 0.0f;
    MtxPosition(sp24, spC);
    sp24->z = 75.0f;

    for (int i = 0; i < (i_this->mBrCount - 1) - i_this->mBreakBrNo; i++, pBr--) {
        br_s *next = pBr + 1;
        f32 fVar2 = pBr->mCtrlPos.y + pBr->mDroopY;
        f32 fVar1 = pBr->mGroundY + 30.0f;
        if (fVar2 < fVar1) {
            pBr->mGroundHitCount = pBr->mGroundHitCount + 1;
            fVar2 = fVar1;
        }
        f32 x = (pBr->mCtrlPos.x - next->mCtrlPos.x) + spC->x;
        f32 z = (pBr->mCtrlPos.z - next->mCtrlPos.z) + spC->z;
        f32 y = fVar2 - next->mCtrlPos.y;
        s16 atan = cM_atan2s(x, z);
        s16 atan2 = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        next->mRotation.y = atan;
        next->mRotation.x = atan2;
        if ((u32)i == (u32)(((i_this->mBrCount - 1) - i_this->mBreakBrNo) - 1)) {
            pBr->mRotation.y = atan;
            pBr->mRotation.x = atan2;
        }
        cMtx_YrotS(calc_mtx(), atan);
        cMtx_XrotM(calc_mtx(), atan2);
        MtxPosition(sp24, sp18);
        pBr->mCtrlPos.x = next->mCtrlPos.x + sp18->x;
        pBr->mCtrlPos.y = next->mCtrlPos.y + sp18->y;
        pBr->mCtrlPos.z = next->mCtrlPos.z + sp18->z;
    }
}

/* bridge_move, case 3: the planks follow the ropes, react to riders and hits */
static inline void bridge_move_normal(bridge_class *i_this, fopAc_ac_c *player) {
    br_s *pBr = &i_this->mBr[0];
    gabi::Local<cXyz> sp2C;
    i_this->mAnmPhase = (s16)(i_this->mAnmPhase + 3000);
    copy_words(gabi::ea(&pBr->mCtrlPos), gabi::ea(&i_this->home.pos));

    if ((i_this->mTypeBits & 1) == 1) {
        gabi::Local<cXyz> sp38;
        cMtx_YrotS(calc_mtx(), i_this->home.angle.y);
        sp38->z = 0.0f;
        sp38->x = bridge_getCos(i_this->mWavePhaseH) * i_this->mSwingAmp * -2.0f;
        sp38->y = 0.0f;
        MtxPosition(sp38, sp2C);
        PSVECAdd(&pBr->mCtrlPos, sp2C, &pBr->mCtrlPos);
    }

    control1(i_this, pBr);

    copy_words(gabi::ea(&br_at(i_this, i_this->mBrCount - 1)->mCtrlPos), gabi::ea(&i_this->mEndPos));
    if ((i_this->mTypeBits & 1) == 1) {
        cXyz *last = &br_at(i_this, i_this->mBrCount - 1)->mCtrlPos;
        BridgeHD::SDK::subtractVector(last, sp2C.get(), last); /* PSVECSubtract */
    }

    control2(i_this, pBr);
    control3(i_this, pBr);

    gabi::Local<cXyz> sp14;
    cXyz_mi(&i_this->home.pos, sp14, &pBr->mCtrlPos);
    f32 dz = sp14->z, dy = sp14->y, dx = sp14->x;
    copy_words(gabi::ea(&i_this->current.pos), gabi::ea(&pBr->mCtrlPos));
    i_this->current.angle.x = pBr->mRotation.x;
    i_this->current.angle.y = pBr->mRotation.y;
    i_this->current.angle.z = pBr->mRotation.z;

    for (int i = 0; i < i_this->mBrCount; i++, pBr++) {
        f32 py = pBr->mCtrlPos.y;
        f32 pz = pBr->mCtrlPos.z;
        pBr->mPosition.y = py;
        f32 px = pBr->mCtrlPos.x;
        pBr->mPosition.z = pz;
        pBr->mPosition.x = px;
        s32 cnt = i_this->mBrCount;
        f32 tmpf = ((f32)(cnt - i) / (f32)cnt) * 0.75f;
        pBr->mPosition.x = gabi::fmadds(dx, tmpf, px);
        pBr->mPosition.y = gabi::fmadds(dy, tmpf, py);
        pBr->mPosition.z = gabi::fmadds(dz, tmpf, pz);

        if (pBr->mPlankRideTimer != 0) {
            for (int j = -5; j <= 5; j++) {
                if ((i + j) < 0 || (i + j) >= i_this->mBrCount) {
                    continue;
                }
                s16 my_tgt = (s16)gabi::ftoi((f32)pBr->mRideRollTarget * bridge_getPlankInfluence(j) * pBr[j].mSwayRate);
                cLib_addCalcAngleS2(&pBr[j].mRideRoll, my_tgt, 4, 0x800);
                cLib_addCalc2(&pBr[j].mSagY, pBr->mRideSag * bridge_getPlankInfluence(j), 1.0f, 10.0f);
            }
        }

        if ((pBr->mRopeFlags & 4) != 0) {
            if ((pBr->mRopeFlags & 3) != 3) {
                f32 fVar14 = 0.0f;
                f32 fVar2 = -80.0f;
                if ((pBr->mRopeFlags & 3) == 1) {
                    fVar14 = 7000.0f;
                    fVar2 = -30.0f;
                } else if ((pBr->mRopeFlags & 3) == 2) {
                    fVar14 = -7000.0f;
                    fVar2 = -30.0f;
                }
                for (int j = -5; j <= 5; j++) {
                    if ((i + j) < 0 || (i + j) >= i_this->mBrCount) {
                        continue;
                    }
                    s16 my_tgt = (s16)gabi::ftoi(fVar14 * bridge_getPlankInfluence(j) * pBr[j].mSwayRate);
                    cLib_addCalcAngleS2(&pBr[j].mCutRoll, my_tgt, 4, 0x800);
                    cLib_addCalc2(&pBr[j].mSagY, fVar2 * bridge_getPlankInfluence(j), 1.0f, 15.0f);

                    if ((pBr->mRopeFlags & 3) == 0 && (i_this->mTypeBits & 4) == 0 && (u32)(j + 2) < 5 &&
                        pBr[j].mPlankRideTimer != 0) {
                        if (dComIfGp_event_runCheck()) {
                            i_this->mBreakCount = 0;
                        } else {
                            s32 v = i_this->mBreakCount + 2;
                            i_this->mBreakCount = v;
                            /* HD: faster with the player flag 0x02000000 (+0x3B8) */
                            if ((u32)(gabi::at<BridgeHD::PlayerFlags>(gabi::ea(player))->mNoResetFlg0) &
                                BridgeHD::HeavyRider) {
                                v += 8;
                                i_this->mBreakCount = v;
                            }
                            if (v > 100) {
                                i_this->mMoveProcMode = 4;
                                i_this->mBreakBrNo = i + j;
                                if (i_this->mRideTimer != 0) {
                                    gabi::Local<cXyz> dir;
                                    dVibration_c *vib = dComIfGp_getVibration();
                                    dir->x = 0.0f;
                                    dir->y = 1.0f;
                                    dir->z = 0.0f;
                                    vib->StartShock(
                                        5, -0x21,
                                        dir.get()); /* StartShock(REG0_S(2) + 5, -0x21, cXyz(0, 1, 0)) */
                                }
                                break;
                            }
                        }
                    }
                    if (pBr[j].mRideSag < -200.0f) {
                        i_this->mBreakBrNo = i + j;
                        i_this->mMoveProcMode = 4;
                        break;
                    }
                }
            }
            if ((pBr->mRopeFlags & 4) != 0) {
                s16 a0 = pBr->mRopeHitTimer[0];
                s16 a1 = pBr->mRopeHitTimer[1];
                if (a0 != 0 || a1 != 0) {
                    f32 f = (f32)(a0 | a1) * 150.0f;
                    f32 fVar14 = cM_ssin(i_this->mAnmPhase * 4) * f;
                    for (int j = -5; j <= 5; j++) {
                        if ((i + j) < 0 || (i + j) >= i_this->mBrCount) {
                            continue;
                        }
                        s16 my_tgt = (s16)gabi::ftoi(fVar14 * bridge_getPlankInfluence(j) * pBr[j].mSwayRate);
                        pBr[j].mCutRoll = (s16)(pBr[j].mCutRoll + my_tgt);
                    }
                }
            }
        }

        u8 mPlankRideTimer = pBr->mPlankRideTimer;
        s16 mCutRoll = pBr->mCutRoll;
        s16 mRideRoll = pBr->mRideRoll;
        if (mPlankRideTimer != 0) {
            pBr->mPlankRideTimer = mPlankRideTimer - 1;
        }
        pBr->mRideRollTarget = 0;
        pBr->mRotation.z = (s16)(mRideRoll + mCutRoll);
        cLib_addCalcAngleS2(&pBr->mRideRoll, 0, 4, 0x400);
        cLib_addCalcAngleS2(&pBr->mCutRoll, 0, 4, 0x400);
        cLib_addCalc2(&pBr->mDroopY, -15.0f, 1.0f, 5.0f);
        cLib_addCalc0(&pBr->mSagY, 1.0f, 5.0f);
    }

    s32 mBreakCount = i_this->mBreakCount;
    f32 e0 = i_this->mWavePower;
    f32 e4 = i_this->mSwingPower;
    if (mBreakCount != 0) {
        i_this->mBreakCount = mBreakCount - 1;
    }
    i_this->mWaveAmpV = e0;
    i_this->mWaveAmpH = e0;
    i_this->mSwingAmp = e4;
    i_this->mWaveStepV = 3000;
    i_this->mWaveStepH = 0x578;

    /* GameCube: *wp > 0.1f ? 2.0f : 0.0f (HD: fsel on 0.1f - *wp) */
    f32 tmpf2 = (0.1f - wind_power()) >= 0.0f ? 0.0f : 2.0f;
    cLib_addCalc2(&i_this->mWavePower, tmpf2, 0.1f, 0.1f);
    cLib_addCalc2(&i_this->mSwingPower, tmpf2 * 0.3f, 0.1f, 0.05f);
}

/* bridge_move (inline) */
static inline void bridge_move(bridge_class *i_this) {
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    br_s *pBr = &i_this->mBr[0];

    switch (i_this->mMoveProcMode) {
    case 0:
        i_this->mRopePhaseOfs = 0;
        i_this->mMoveProcMode = 2;
        /* HD: no fopAcM_OffStatus(CULL) */
        /* fallthrough */
    case 2:
        for (int i = 0; i < i_this->mBrCount; i++) {
            br_s *b = br_at(i_this, i);
            if ((i_this->mTypeBits & 1) == 1) {
                b->mSwayRate = 1.0f;
                b->mRope1Hp = 3;
                b->mRope0Hp = 3;
            } else {
                f32 tmpf = 3.1415927f * ((f32)i / (f32)(i_this->mBrCount - 1));
                f32 s = (f32)(BridgeHD::SDK::sin_d(tmpf) * 1.0);
                b->mRope1Hp = 3;
                b->mRope0Hp = 3;
                b->mSwayRate = fabsf(s);
            }
        }
        i_this->mMoveProcMode = 3;
        /* fallthrough */
    case 3:
        bridge_move_normal(i_this, player);
        break;

    case 4: {
        gabi::Local<cXyz> sp38;
        for (int i = 0; i < i_this->mBrCount; i++, pBr++) {
            pBr->mDroopY = 0.0f;
            u32 mBreakBrNo = i_this->mBreakBrNo;
            if ((u32)i == mBreakBrNo || (u32)i == mBreakBrNo - 1 || (u32)i == mBreakBrNo + 1) {
                for (int k = 0; k < 2; k++) {
                    f32 x = pBr->mPosition.x;
                    sp38->x = x;
                    sp38->y = pBr->mPosition.y;
                    sp38->z = pBr->mPosition.z;
                    sp38->x = x + cM_rndFX(50.0f);
                    sp38->z = sp38->z + cM_rndFX(50.0f);
                    BridgeHD::SDK::kikuzu_set(i_this, sp38);
                }
            }
        }
        i_this->mMoveProcMode = 5;
        i_this->mBreakTimer = 50;
        fopAcM_seStart(player, 0x6933 /* JA_SE_OBJ_SBRIDGE_BREAK */, 0);
    }
        /* fallthrough */
    case 5: {
        pBr = &i_this->mBr[0];
        s16 mBreakTimer = i_this->mBreakTimer;
        s16 mAnmPhase = i_this->mAnmPhase;
        if (mBreakTimer != 0) {
            i_this->mBreakTimer = mBreakTimer - 1;
        }
        i_this->mAnmPhase = (s16)(mAnmPhase + 4000);
        copy_words(gabi::ea(&pBr->mCtrlPos), gabi::ea(&i_this->home.pos));
        cut_control1(i_this, pBr);
        copy_words(gabi::ea(&br_at(i_this, i_this->mBrCount - 1)->mCtrlPos), gabi::ea(&i_this->mEndPos));
        cut_control2(i_this, pBr);
        for (int i = 0; i < i_this->mBrCount; i++, pBr++) {
            copy_words(gabi::ea(&pBr->mPosition), gabi::ea(&pBr->mCtrlPos));
            cLib_addCalc2(&pBr->mDroopY, -50.0f, 1.0f, 5.0f);
            pBr->mGroundY = -10000.0f;
        }
        break;
    }
    default:
        break;
    }
}

/* the rope hit checks of one plank (type 0 bridges: cuttable ropes) */
static inline void rope_hit(bridge_class *i_this, br_s *pBr, fopAc_ac_c *a_player, int side, u32 *soundId,
                            bool *bVar1) {
    dCcD_Cyl *cyl = &pBr->mCyl[side];
    if (pBr->mRopeHitTimer[side] < 10) {
        be<u32> &flags = cyl->mGObjTg.mSPrm; /* OnTgNoConHit */
        flags = (u32)flags | 2;
        pBr->mRopeHitTimer[side] = 20;
        *soundId = 0x2838; /* JA_SE_LK_CUT_SBRIDGE_ROPE */
        gabi::Local<BridgeHD::SDK::CcAtInfo> atInfo;
        atInfo->mpObj = gabi::ea(cyl->GetTgHitObj());
        BridgeHD::SDK::at_power_check(atInfo);
        u8 dmg = atInfo->mDamage;
        if (dmg > 1) {
            dmg = 4;
            atInfo->mDamage = dmg;
        }
        be<s8> &hp = side == 0 ? pBr->mRope0Hp : pBr->mRope1Hp;
        s8 left = (s8)(hp - dmg);
        hp = left;
        if (left <= 0) {
            pBr->mRopeFlags = pBr->mRopeFlags & (side == 0 ? 0xE : 0xD);
        } else {
            *bVar1 = true;
        }
        dComIfGp_particle_set(0xD /* dPa_name::ID_AK_JN_OK */, &pBr->mRopeMidPos[side], &a_player->shape_angle);
        BridgeHD::SDK::kikuzu_set(i_this, &pBr->mRopeMidPos[side]);
    }
}

/* the chain hit checks (type 1/8 bridges) */
static inline void chain_hit(br_s *pBr, fopAc_ac_c *a_player, int side, u32 *soundId) {
    dCcD_Cyl *cyl = &pBr->mCyl[side];
    if (pBr->mRopeHitTimer[side] < 10) {
        be<u32> &flags = cyl->mGObjTg.mSPrm; /* OnTgNoConHit */
        flags = (u32)flags | 2;
        pBr->mRopeHitTimer[side] = 15;
        *soundId = 0x2839; /* JA_SE_LK_HIT_SBRIDGE_CHAIN */
        dComIfGp_particle_set(0xC /* dPa_name::ID_AK_JN_NG */, cyl->GetTgHitPosP(), &a_player->shape_angle,
                              &BridgeHD::getHeaderState()->mChainParticleScale /* spB4 */);
    }
}

/* the rope fire of one side */
static inline void rope_fire(bridge_class *i_this, br_s *pBr, int side, bool bGotFlamePos, cXyz *spC0,
                             cXyz *spA8) {
    cXyz *top = side == 0 ? &pBr->mSide0Pos[1] : &pBr->mSide1Pos[1];
    be<s16> &timer = side == 0 ? pBr->mRope0FireTimer : pBr->mRope1FireTimer;
    gptr<JPABaseEmitter> &emitter = side == 0 ? pBr->mpRope0FireEmitter : pBr->mpRope1FireEmitter;
    copy_floats(spA8, top);
    if ((pBr->mRopeFlags & (side == 0 ? 1 : 2)) == 0) {
        spA8->y = spA8->y - 10000.0f;
    }
    pBr->mCyl[side].SetC(spA8);

    if (timer != 0) {
        timer = timer - 1;
        if (emitter != nullptr) {
            mDoMtx_stack_c::transS(top->x, top->y + 100.0f, top->z);
            BridgeHD::SDK::JPA_setGlobalRTMatrix(gabi::ea(emitter.get()), mDoMtx_stack_c::get());
            if (timer == 0) {
                JPA_becomeInvalidEmitter(gabi::ea(emitter.get()));
                emitter = nullptr;
            }
        }
        if (timer == 0) {
            pBr->mRopeFlags = pBr->mRopeFlags & (side == 0 ? 0xE : 0xD);
        }
    } else if (bGotFlamePos && (i_this->mTypeBits & 9) == 0) {
        spA8->y = spA8->y + 100.0f;
        gabi::Local<cXyz> d;
        cXyz_mi(spA8, d, spC0);
        gabi::Local<cXyz> sp48;
        sp48->x = d->x;
        sp48->z = d->z;
        sp48->y = d->y * 0.4f;
        if (std_sqrtf(PSVECSquareMag(sp48)) < 50.0f) {
            timer = 30;
            emitter = dComIfGp_particle_set(0x80EA /* dPa_name::ID_AK_SN_BRIDGEROPEFIRE00 */, spA8);
        }
    }
}

/* the side rope's segment at this plank: hanging from the plank, or cut */
static inline void side_rope_segment(bridge_class *i_this, br_s *pBr, int line, u32 seg, u32 idx,
                                     cXyz *sp84) {
    u32 s = seg + idx * 0xC;
    gabi::Local<cXyz> d;
    cXyz_mi(gabi::at<cXyz>(s - 0xC), d, gabi::at<cXyz>(s + 0xC));
    gabi::Local<cXyz> sp30;
    sp30->z = d->z;
    sp30->y = d->y;
    sp30->x = d->x;
    f32 sn = cM_ssin(i_this->mAnmPhase * 6);
    f32 a0 = (f32)pBr->mRopeHitTimer[line];
    f32 x = gabi::fmadds(sp30->x, 0.5f, (f32)gabi::at<cXyz>(s)[1].x) + sp84->x;
    gabi::at<cXyz>(s)->x = x;
    f32 y = gabi::fmadds(sn, a0, gabi::fmadds(sp30->y, 0.5f, (f32)gabi::at<cXyz>(s)[1].y)) - 10.0f;
    gabi::at<cXyz>(s)->y = y;
    f32 z = gabi::fmadds(sp30->z, 0.5f, (f32)gabi::at<cXyz>(s)[1].z) + sp84->z;
    gabi::at<cXyz>(s)->z = z;

    u32 lines = (u32)(*reinterpret_cast<be<u32> *>(
        &gabi::at<BridgeLineMaterial>(gabi::ea(&pBr->mLineMat1))->mpLines));
    u32 segment00 = gabi::ea(gabi::at<BridgeLine>(lines)[line].mpSegments.get());
    u32 segment01 = gabi::ea(gabi::at<BridgeLine>(lines)[(line + 2)].mpSegments.get());
    gabi::at<cXyz>(segment00)->x = (f32)gabi::at<cXyz>(seg)[idx].x;
    gabi::at<cXyz>(segment00)->y = (f32)gabi::at<cXyz>(seg)[idx].y;
    gabi::at<cXyz>(segment00)->z = z;
    BridgeHD::SDK::himo_cut_control1(gabi::at<cXyz>(segment00));
    copy_words(segment01, gabi::ea(line == 0 ? &pBr->mSide0Pos[1] : &pBr->mSide1Pos[1]));
    BridgeHD::SDK::himo_cut_control1(gabi::at<cXyz>(segment01));
}

/* 020E11E8 */
BOOL daBridge_Execute(bridge_class *i_this) {
    /* HD: no distance culling (GameCube: mbStopDraw); nothing happens during an event while the
     * rumble timer runs */
    if (dComIfGp_event_runCheck() && i_this->mRideTimer != 0) {
        return TRUE;
    }
    fopAc_ac_c *a_player = dComIfGp_getPlayer(0);
    if (i_this->mRideTimer != 0) {
        i_this->mRideTimer = i_this->mRideTimer - 1;
    }

    cXyz *windDirection = BridgeHD::SDK::dKyw_get_wind_vec();
    *reinterpret_cast<be<u32> *>(&wind_vec) = gabi::ea(windDirection);
    wy = cM_atan2s(windDirection->x, windDirection->z);
    *reinterpret_cast<be<u32> *>(&wp) = BridgeHD::SDK::dKyw_get_wind_power();
    i_this->mFrameCounter = i_this->mFrameCounter + 1;

    if ((i_this->mTypeBits & 2) != 0 && i_this->mpAite == nullptr) {
        i_this->mpAite =
            (bridge_class *)fpcM_Search(BridgeHD::SearchPartner /* s_a_b_sub */, i_this); /* search_aite */
    }

    bridge_move(i_this);

    i_this->mSideRopeNum = 0;
    gabi::Local<cXyz> spC0;
    br_s *pBr = &i_this->mBr[0];
    bool bGotFlamePos = false;
    /* daPy_py_c::getBokoFlamePos (virtual, vtable +0x74) */
    if (gabi::call_ptr<BOOL>(
            reinterpret_cast<BridgeHD::ActorVirtual *>(a_player)->mVtable->getBokoFlamePos.get(), a_player,
            spC0.get()) != FALSE) {
        bGotFlamePos = true;
    }

    /* HD: the rope tops: GameCube +1000 (chains) / +200, HD 1000 - 300 sin (chains) / 230 - 110 sin
     * over the bridge (half a period for joined bridges) */
    f32 topBase, topSwing;
    if (i_this->mTypeBits & 1) {
        topBase = 1000.0f;
        topSwing = 300.0f;
    } else {
        topBase = 230.0f;
        topSwing = 110.0f;
    }

    gabi::Local<cXyz> eyeDir;
    gabi::Local<cXyz> spA8;
    gabi::Local<cXyz> sp84;
    for (int i = 0; i < i_this->mBrCount; i++, pBr++) {
        MtxTrans(pBr->mPosition.x, pBr->mPosition.y, pBr->mPosition.z, 0);
        cMtx_YrotM(calc_mtx(), pBr->mRotation.y);
        cMtx_XrotM(calc_mtx(), pBr->mRotation.x);
        mDoMtx_ZrotM(calc_mtx(), pBr->mRotation.z);

        s32 mBreakBrNo = i_this->mBreakBrNo;
        if (mBreakBrNo != 0) {
            MtxTrans(0.0f, 0.0f, i > mBreakBrNo ? 30.0f : -30.0f, 1);
        }

        f32 sx = pBr->mScale.x;
        eyeDir->y = 0.0f;
        eyeDir->z = 0.0f;
        eyeDir->x = 99.0f * sx;
        MtxPosition(eyeDir, &pBr->mSide0Pos[1]);
        eyeDir->x = -eyeDir->x; /* GameCube: *= -1.0f (fneg) */
        MtxPosition(eyeDir, &pBr->mSide1Pos[1]);
        eyeDir->y = -30.0f; /* REG0_F(4) + -30.0f */
        MtxPosition(eyeDir, &pBr->mSide1Pos[2]);
        eyeDir->x = -eyeDir->x;
        MtxPosition(eyeDir, &pBr->mSide0Pos[2]);

        if ((pBr->mRopeFlags & 4) != 0) {
            s16 mRopeTopTimer = pBr->mRopeTopTimer;
            if (mRopeTopTimer != 0) {
                if (mRopeTopTimer > 0) {
                    pBr->mRopeTopTimer = mRopeTopTimer - 1;
                }
                f32 ratio = (f32)i / (f32)(i_this->mBrCount - 1);
                f64 s;
                if (i_this->mTypeBits & 2) {
                    s = BridgeHD::SDK::sin_d(1.5707964f * ratio);
                } else {
                    s = BridgeHD::SDK::sin_d(3.1415927f * ratio);
                }
                f32 add = (f32)(s * (f64)(-topSwing) + (f64)topBase); /* fmadds with the double sine */
                f32 x1 = pBr->mSide0Pos[1].x, y1 = pBr->mSide0Pos[1].y, z1 = pBr->mSide0Pos[1].z;
                f32 x0 = pBr->mSide1Pos[1].x, y0 = pBr->mSide1Pos[1].y, z0 = pBr->mSide1Pos[1].z;
                pBr->mSide0Pos[0].z = z1;
                pBr->mSide0Pos[0].x = x1;
                pBr->mSide1Pos[0].x = x0;
                pBr->mSide0Pos[0].y = y1 + add;
                pBr->mSide1Pos[0].y = y0 + add;
                pBr->mSide1Pos[0].z = z0;
                if ((i_this->mTypeBits & 2) != 0 && (u32)i == (u32)(i_this->mActiveBrCount - 1)) {
                    copy_words(gabi::ea(&i_this->mEndRopePos1), gabi::ea(&pBr->mSide0Pos[0]));
                    copy_words(gabi::ea(&i_this->mEndRopePos0), gabi::ea(&pBr->mSide1Pos[0]));
                }
            }

            u32 soundId = 0;
            bool bVar1 = false;
            if ((i_this->mTypeBits & 9) == 0) {
                if (i_this->mBrCount - i < 3) {
                    pBr->mRopeFlags = 0;
                }
                if (i_this->mBreakBrNo != 0) {
                    pBr->mRopeFlags = pBr->mRopeFlags & 0xC;
                }
                if (pBr->mCyl[0].ChkTgHit()) {
                    rope_hit(i_this, pBr, a_player, 0, &soundId, &bVar1);
                }
                if (pBr->mCyl[1].ChkTgHit()) {
                    rope_hit(i_this, pBr, a_player, 1, &soundId, &bVar1);
                }
            } else {
                /* HD: spB4 is a function-local static (2, 2, 2) */
                be<u32> &guard = BridgeHD::getHeaderState()->mScaleGuard;
                if (guard == 0) {
                    BridgeHD::getHeaderState()->mChainParticleScale.y = 2.0f;
                    BridgeHD::getHeaderState()->mChainParticleScale.z = 2.0f;
                    guard = 1;
                    BridgeHD::getHeaderState()->mChainParticleScale.x = 2.0f;
                }
                if (pBr->mCyl[0].ChkTgHit()) {
                    chain_hit(pBr, a_player, 0, &soundId);
                }
                if (pBr->mCyl[1].ChkTgHit()) {
                    chain_hit(pBr, a_player, 1, &soundId);
                }
            }

            if (soundId != 0) {
                if ((i_this->mTypeBits & 8) != 0 || bVar1) {
                    soundId = 0x2837; /* JA_SE_LK_HIT_SBRIDGE_ROPE */
                }
                mDoAud_seStart(soundId, &pBr->mCtrlPos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
            }

            rope_fire(i_this, pBr, 0, bGotFlamePos, spC0, spA8);
            rope_fire(i_this, pBr, 1, bGotFlamePos, spC0, spA8);

            dComIfG_Ccsp_Set(&pBr->mCyl[0]);
            dComIfG_Ccsp_Set(&pBr->mCyl[1]);
        }

        for (int k = 0; k < 2; k++) {
            if (pBr->mRopeHitTimer[k] != 0) {
                pBr->mRopeHitTimer[k] = pBr->mRopeHitTimer[k] - 1;
            }
        }

        cMtx_YrotM(calc_mtx(), pBr->mRotationYExtra);
        if (i >= i_this->mActiveBrCount) {
            pBr->mScale.z = 0.0f;
            pBr->mScale.y = 0.0f;
            pBr->mScale.x = 0.0f;
            J3DModel *m = pBr->mpModel;
            m->mBaseScale.x = 0.0f;
            m->mBaseScale.y = 0.0f;
            m->mBaseScale.z = 0.0f;
            pBr->mRopeFlags = 0;
        }
        J3DModel_setBaseTRMtx(pBr->mpModel, calc_mtx());

        if ((i_this->mTypeBits & 1) == 0 && (pBr->mRopeFlags & 4) != 0) {
            u32 lines = (u32)(*reinterpret_cast<be<u32> *>(
                &gabi::at<BridgeLineMaterial>(gabi::ea(&i_this->mLineMat))->mpLines));
            u32 segment0 = gabi::ea(gabi::at<BridgeLine>(lines)[0].mpSegments.get());
            u32 segment1 = gabi::ea(gabi::at<BridgeLine>(lines)[1].mpSegments.get());
            u32 idx = i_this->mSideRopeNum + 1;

            eyeDir->x = 0.0f;
            s16 tmpS = (s16)(i_this->mRopePhaseOfs * 0x5DC);
            /* HD: the angle is summed in float, then converted */
            s32 ang = gabi::ftoi((f32)(i_this->mFrameCounter * 0x578) + (f32)tmpS);
            f32 sn = cM_ssin(ang);
            eyeDir->z = (wind_power() + 0.3f) * ((sn + sn) + 5.0f);
            cMtx_YrotS(calc_mtx(), wind_y());
            MtxPosition(eyeDir, sp84);

            if (pBr->mRopeFlags & 1) {
                copy_words(segment0 + idx * 0xC, gabi::ea(&pBr->mSide0Pos[0]));
            } else {
                side_rope_segment(i_this, pBr, 0, segment0, idx, sp84);
            }
            if (pBr->mRopeFlags & 2) {
                copy_words(segment1 + idx * 0xC, gabi::ea(&pBr->mSide1Pos[0]));
            } else {
                side_rope_segment(i_this, pBr, 1, segment1, idx, sp84);
            }
            i_this->mSideRopeNum = i_this->mSideRopeNum + 1;
        }
    }

    dBgWSv_CopyBackVtx(i_this->mpBgW);
    u32 vtxTbl = (u32)(*reinterpret_cast<be<u32> *>(
        &gabi::at<BridgeHD::Background>(gabi::ea(i_this->mpBgW.get()))->pm_vtx_tbl));
    int other_i = 0;
    gabi::Local<cXyz> spCC;
    for (int i = 0; i < (s32)gabi::at<BridgeHD::Background>(gabi::ea(i_this->mpBgW.get()))->pm_bgd->m_v_num;
         i++) {
        int sw = i & 3;
        int idx = i >> 2;
        u32 v = vtxTbl + i * 0xC;
        if (idx < i_this->mActiveBrCount) {
            pBr = br_at(i_this, idx);
            switch (sw) {
            case 0:
                copy_words(v, gabi::ea(&pBr->mSide0Pos[2]));
                break;
            case 1:
                copy_words(v, gabi::ea(&pBr->mSide1Pos[2]));
                break;
            case 2:
                copy_words(v, gabi::ea(&pBr->mSide0Pos[1]));
                break;
            case 3:
                copy_words(v, gabi::ea(&pBr->mSide1Pos[1]));
                break;
            }
            if (idx == 0 || (u32)idx == (u32)(i_this->mActiveBrCount - 1)) {
                cMtx_YrotS(calc_mtx(), pBr->mRotation.y);
                cMtx_XrotM(calc_mtx(), pBr->mRotation.x);
                eyeDir->y = 0.0f;
                eyeDir->x = 0.0f;
                if (idx == 0) {
                    eyeDir->z = 50.0f;
                } else if ((u32)i_this->mActiveBrCount == (u32)i_this->mBrCount) {
                    eyeDir->z = -50.0f;
                } else {
                    eyeDir->z = -40.0f;
                }
                MtxPosition(eyeDir, spCC);
                gabi::at<cXyz>(v)->x = (f32)(gabi::at<cXyz>(v)->x) + spCC->x;
                gabi::at<cXyz>(v)->y = (f32)(gabi::at<cXyz>(v)->y) + spCC->y;
                gabi::at<cXyz>(v)->z = (f32)(gabi::at<cXyz>(v)->z) + spCC->z;
            }
            other_i = i;
        } else {
            copy_words(v, vtxTbl + other_i * 0xC);
        }
        if (i_this->mMoveProcMode >= 4) {
            gabi::at<cXyz>(v)->y = 10000.0f;
        }
    }

    dBgW_Move(i_this->mpBgW);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    return TRUE;
}

/* WWHD glue
 */
