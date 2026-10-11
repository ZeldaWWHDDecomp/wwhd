/**
 * d_a_ship_exec.cpp (WWHD)
 * King of Red Lions: daShip_c::execute
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ship.cpp) to the WWHD layout and verified against cking.rpx.
 */
#include "d/actor/d_a_ship_exec.h"

static f32 sight_scroll_phase;
static cXyz cannon_top;
struct ShipExecCylinderOffsets {
    be<f32> mOffset[3];
};
static ShipExecCylinderOffsets cyl_offset;
static cXyz sail_offset_storage;
static cXyz sph_offset_storage;
static cXyz top_offset_storage;
static cXyz XZ_top_offset_storage;
static u32 sail_offset_guard;
static u32 sph_offset_guard;
static u32 top_offset_guard;
static u32 XZ_top_offset_guard;

static inline cXyz* ship_exec_staticVec(be<u32>& guard, cXyz& value, f32 x, f32 y, f32 z) {
    if (guard == 0) {
        guard = 1;
        value.set(x, y, z);
    }
    return &value;
}

/* a float copied with lfs/stfs keeps its bits (a host float copy would quiet a signalling NaN) */
static inline void ship_cpf(be<f32>& d, const be<f32>& s) {
    reinterpret_cast<be<u32>&>(d) = reinterpret_cast<const be<u32>&>(s);
}

/* the stop() of the four particle callbacks */
static inline void ship_stopWaves(daShip_c* s) {
    gabi::at<ShipExecCallbackState>(gabi::ea(s->mWaveR))->stop();
    gabi::at<ShipExecCallbackState>(gabi::ea(s->mWaveL))->stop();
    gabi::at<ShipExecCallbackState>(gabi::ea(s->mSplash))->stop();
    gabi::at<ShipExecCallbackState>(gabi::ea(s->mTrack))->stop();
}

/* the cannon-sight end marker: placed at the stack matrix */
static inline void ship_sightMarker(daShip_c* s) {
    ShipExecEffectModels* effects = gabi::at<ShipExecEffectModels>(gabi::ea(s));
    J3DModel* m = effects->mpFallPointModel;
    if (m != nullptr) {
        J3DModel_setBaseTRMtx(m, mDoMtx_now());
        model_setBaseScale1(effects->mpFallPointModel, 1.0f);
    }
    effects->mpFallPointAnm->play(nullptr, 0, 0);
}

// HD: the cannon sight. Simulates the cannonball from the muzzle in 128 steps (mHD1B24), places a
// sight segment every eighth step (offset by a scrolling phase) and stops at the first enemy
// (process names 0xE1, 0xE2, 0xD1) hit by the step colliders (mRopeSph), at a wall (a line check
// every fourth step) or at the water surface, where the end marker is drawn.
static void ship_cannonSight(daShip_c* s) {
    ShipExecSight& sight = ship_exec_sight(s);
    gabi::Local<cXyz> start;
    start->copy(sight.mPosition);
    if ((u8)ship_exec_play()->mEventRun == 0 && ship_isLinkPlayer0() &&
        s->isProc(ShipExecTarget::procCannon)) {
        gabi::Local<dBgS_LinChk> linChk;
        dBgS_LinChk_ct(linChk.get(), SHIP_LINCHK_VT, false);
        gabi::Local<cXyz> previousLinePosition;
        previousLinePosition->copy(sight.mPosition);
        // HD: sight segments scroll by 0.75 per frame over [0, 8).
        f32 ph = (f32)sight_scroll_phase + 0.75f;
        if (!(ph < 7.999f)) {
            ph = ph - 8.0f;
        }
        sight_scroll_phase = ph;
        daShip_sightPacket_setMtx(s->mHDPacket);
        s32 seg = 0;
        f32 k = 0.5f;
        gabi::Local<cXyz> prev;
        gabi::Local<cXyz> pos;
        gabi::Local<cXyz> w;
        gabi::Local<cXyz> d;
        gabi::Local<cXyz> w2;
        gabi::Local<cXyz> t9c;
        gabi::Local<cXyz> ta8;
        gabi::Local<cXyz> t60;
        for (s32 i = 0; i < 0x80; i++) {
            prev->copy(sight.mPosition);
            s->sightStep();
            pos->copy(sight.mPosition);
            dCcD_Sph* sph = &s->mRopeSph[i];
            sph->SetC(pos.get());
            dComIfG_Ccsp_Set(sph);
            if (sph->ChkCoHit()) {
                fopAc_ac_c* ac = sph->GetCoHitAc();
                if (ac && ((s16)ac->mProfName == ShipExecProcess::fpcNm_RD_e ||
                           (s16)ac->mProfName == ShipExecProcess::fpcNm_DAIOCTA_e ||
                           (s16)ac->mProfName == ShipExecProcess::fpcNm_WZ_e)) {
                    mDoMtx_stack_c::transS(pos->x, pos->y, pos->z);
                    ship_sightMarker(s);
                    break;
                }
            }
            if (i - (i & ~3) == 2) {
                dBgS_LinChk_Set(linChk.get(), previousLinePosition.get(), pos.get(), nullptr);
                if (cBgS_LineCross(dComIfG_Bgsp(), linChk.get())) {
                    ShipExecLineHit* hit = gabi::at<ShipExecLineHit>(gabi::ea(linChk.get()));
                    mDoMtx_stack_c::transS(hit->mCross.x, hit->mCross.y, hit->mCross.z);
                    ship_sightMarker(s);
                    break;
                }
                previousLinePosition->copy(sight.mPosition);
            }
            w->copy(*pos);
            s->getMaxWaterY(w.get());
            s32 n = gabi::ftoi((f32)sight_scroll_phase);
            f32 wy = w->y;
            f32 py = pos->y;
            bool hitWater;
            if ((u32)(i - (i & ~7)) == (u32)n) {
                if (py - wy > 120.0f) {
                    cXyz_mi(pos.get(), d.get(), start.get());
                    cXyz_abs(d.get());
                    mDoMtx_stack_c::transS(pos->x, pos->y, pos->z);
                    daShip_sightPacket_setSegMtx(s->mHDPacket, seg, mDoMtx_now());
                    seg++;
                    continue;
                }
                hitWater = !(wy < py);
            } else {
                hitWater = !(wy < py);
            }
            if (!hitWater) {
                continue;
            }
            w2->copy(*prev);
            s->getMaxWaterY(w2.get());
            f32 a = std::fabs(wy - (f32)sight.mPosition.y);
            f32 c = std::fabs(w2->y - prev->y);
            f32 sum = a + c;
            if (sum != 0.0f) {
                k = a / sum;
            }
            cXyz_ml(&sight.mPosition, t9c.get(), 1.0f - k);
            cXyz_ml(prev.get(), ta8.get(), k);
            cXyz_pl(t9c.get(), t60.get(), ta8.get());
            mDoMtx_stack_c::transS(t60->x, t60->y, t60->z);
            ship_sightMarker(s);
            break;
        }
        for (s32 i = 0; i < 0x80; i++) {
            ShipExec_clearCollisionHit(&s->mRopeSph[i]);
        }
        ship_LinChk_dt(linChk.get());
    } else {
        for (s32 i = 0; i < 0x80; i++) {
            ShipExec_removeCollision(dComIfG_Ccsp(), &s->mRopeSph[i]);
            ShipExec_clearCollisionHit(&s->mRopeSph[i]);
        }
    }
    sight.mPosition.copy(*start);
}

/* the sight launch: muzzle position, angles and velocity (mHD1B24) */
static inline void ship_sightLaunch(daShip_c* s) {
    ShipExecSight& sight = ship_exec_sight(s);
    gabi::Local<cXyz> top;
    PSMTXMultVec(model_getAnmMtx(s->mpCannonModel, VFNCN_JNT_CANON2_e), &cannon_top, top.get());
    s16 ax = s->shape_angle.x + s->m0396 - 0x4000;
    sight.mPosition.copy(*top);
    sight.mDelay = 8;
    sight.mAngle.y = s->shape_angle.y + s->m0394;
    sight.mAngle.z = s->shape_angle.z;
    sight.mAngle.x = ax;
}

/* 02477A24 */
BOOL daShip_c::execute() {
    // HD: in the Tower of the Gods the boat waits at a fixed place
    if (daShip_checkSirenInside()) {
        current.pos.x = 4.011528f;
        current.pos.y = -2111.8811f;
        shape_angle.y = 0x4000;
        current.pos.z = -1278.5222f;
        return TRUE;
    }
    // HD: execution waits while Link has the ship-inhibiting state flag.
    if ((u32)gabi::at<ShipExecPlayer>(gabi::ea(dComIfGp_getLinkPlayer()))->mNoResetFlg1 & 0x200) {
        return TRUE;
    }
    /* static cXyz sail_offset(0.5f, 155.0f, 50.0f); sph_offset(-5.0f, 0.0f, 0.0f) */
    ship_exec_staticVec(sail_offset_guard, sail_offset_storage, 0.5f, 155.0f, 50.0f);
    cXyz* sph_offset = ship_exec_staticVec(sph_offset_guard, sph_offset_storage, -5.0f, 0.0f, 0.0f);

    m0404 = 0.0f;
    f32 prev_speedF = speedF;
    m0351 = 0xFF;
    m0428 = nullptr;
    f32 prev_fwdvel = mFwdVel;

    if (mTornadoID != fpcM_ERROR_PROCESS_ID_e) {
        setTornadoActor();
        mWhirlActor = nullptr;
    } else {
        mTornadoActor = nullptr;
        if (mWhirlID != fpcM_ERROR_PROCESS_ID_e) {
            setWhirlActor();
        } else {
            mWhirlActor = nullptr;
        }
    }

    if ((u8)ship_exec_play()->mEventRun != 0) {
        mStickMVal = 0.0f;
        mStickMAng = 0;
        mEvtStaffId = dComIfGp_evmng_getMyStaffId("Ship", this, 0);
        if (mEvtStaffId != -1) {
            u32 cutName =
                ShipExec_getMyNowCutName(ship_exec_play()->mEventManager, (s32)mEvtStaffId);
            if (cutName) {
                const be<u8>* name = gabi::at<be<u8>>(cutName);
                m0351 = (u8)(name[0] * 10 + name[1] - 0x210);
            }
        }
        offStateFlg(daSFLG_UNK2000000_e);
        m03B8 = 0;
    } else {
        // HD: "no control" became "the link player is not the current player"
        if (!ship_isLinkPlayer0()) {
            mStickMVal = 0.0f;
            mStickMAng = 0;
        } else {
            mStickMVal = (f32)ShipExec_getStickValue(0);
            mStickMAng = ShipExec_getStickAngle(0) + 0x8000;
        }
        mEvtStaffId = -1;
        // HD: the crash timer counts down here (GameCube: setCrashData)
        if (m03B6 != 0) {
            m03B6--;
        }
    }
    // HD: also clears the Swift Sail flag (0x80000000)
    offStateFlg(daSFLG_JUMP_e | daSFLG_LAND_e | daSFLG_UNK100_e | daSFLG_JUMP_OK_e |
                daSFLG_SHOOT_CANNON_e | daSFLG_UNK100000_e | daSFLG_UNK8000000_e |
                daSFLG_UNK80000000_e);
    if (m0388 > 0) {
        m0388--;
    }
    if (checkStateFlg(daSFLG_UNK20_e)) {
        offStateFlg(daSFLG_UNK20_e);
        speed.y = 5.0f;
    }
    mpBodyAnm->play(nullptr, 0, 0);
    mpHeadAnm->play(nullptr, 0, 0);

    if (mCurMode != 12 && mCurMode != 15 && mCurMode != MODE_START_MODE_WARP_e &&
        mCurMode != MODE_START_MODE_THROW_e && mCurMode != 7 && mCurMode != MODE_CRANE_UP_e &&
        mCurMode != 8) {
        if (eventInfo.checkCommandTalk()) { /* eventInfo.checkCommandTalk() */
            if (mNextMessageNo == 0x1682) {
                ship_exec_play()->mMenuCollect = 1; /* dComIfGp_onMenuCollect() */
            }
            if (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) {
                procTalkReady_init();
            } else {
                procTalk_init();
            }
        } else if ((ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) && checkOutRange()) {
            if (checkStateFlg(daSFLG_FLY_e)) {
                cLib_addCalcAngleS2(&shape_angle.y, m038C, 8, m038E);
                cLib_addCalcAngleS2(&m038E, 0x1800, 8, 0x200);
                // HD: the boat also stops
                speedF = 0.0f;
                current.angle.y = shape_angle.y;
            } else {
                eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
                fopAcM_orderSpeakEvent(this);
            }
        } else {
            if (checkOutRange()) {
                firstDecrementShipSpeed(0.0f);
            }
            eventInfo_onCondition(this, 1);
        }
    }

    mpGrid = 0;
    mpGrid = gabi::ea(fopAcM_SearchByID(mGridID));

    if (ship_demo_getActor(demoActorID)) {
        procToolDemo_init();
    } else if (m0351 == 2 /* DEMO_SALVAGE_e */) {
        procCraneUp_init();
    } else if (mEvtStaffId != -1 && m0351 != 9 /* DEMO_TORNADO_S_e */) {
        procZevDemo_init();
    }

    s16 sVar26 = m0366;
    if (mAnmTransform) {
        ShipExec_updateFrame(&mFrameCtrl);
        gabi::at<ShipExecAnimation>(gabi::ea(mAnmTransform.get()))->mFrame =
            mFrameCtrl.getFrame(); /* mAnmTransform->setFrame() */
        if (!checkStateFlg(daSFLG_UNK8000_e) && mFrameCtrl.checkState(2)) {
            mAnmTransform = nullptr;
        }
    }
    BOOL r24 = m0351 == 8 /* DEMO_OPEN_e */ || checkStateFlg(daSFLG_UNK4000000_e);

    /* (this->*mProc)() */
    ShipExecProcedure* procedure = gabi::at<ShipExecProcedure>(gabi::ea(this));
    if (procedure->mProc.i != 0) {
        (this->*procedure->mProc)();
    }

    if (m0351 == 8 || checkStateFlg(daSFLG_UNK4000000_e)) {
        m0366 = -0x2000;
        sVar26 = -0x2000;
    } else if (r24) {
        m0366 = 0;
        sVar26 = 0;
    }

    if (!checkStateFlg(daSFLG_FLY_e)) {
        if (mCurMode != 12 && mCurMode != 15 && mCurMode != 7 && mCurMode != MODE_CRANE_UP_e &&
            mCurMode != MODE_START_MODE_WARP_e && mCurMode != MODE_START_MODE_THROW_e &&
            (u8)ship_exec_play()->mEventRun == 0) {
            if (mTornadoActor) {
                s16 target = (s16)gabi::ftoi(gabi::fmadds(m040C, 10430.378f, 20480.0f));
                s16 sVar16 = shape_angle.y;
                cLib_addCalcAngleS(&shape_angle.y, target, 5, 0x2000, 0x200);
                setControllAngle(getAimControllAngle(sVar16));
                current.angle.y = shape_angle.y;
            } else if (mWhirlActor) {
                s16 sVar16 = shape_angle.y;
                f32 a = m040C;
                s16 sVar5;
                if (m0352) {
                    sVar5 = (s16)gabi::ftoi(gabi::fmadds(a, 10430.378f, 20480.0f));
                } else {
                    sVar5 = (s16)gabi::ftoi(gabi::fmadds(a, 10430.378f, 32768.0f));
                }
                cLib_addCalcAngleS(&shape_angle.y, sVar5, 5, 0x2000, 0x200);
                setControllAngle(getAimControllAngle(sVar16));
                current.angle.y = shape_angle.y;
            }
        }
        s16 sVar16 = -m0384;
        s16 sVar5 = (s16)gabi::ftoi((f32)sVar16 * 0.05f);
        if (sVar5 == 0) {
            if (sVar16 < 0) {
                sVar5 = -1;
            } else if (sVar16 > 0) {
                sVar5 = 1;
            }
        }
        m0386 += sVar5;
        m0384 += m0386;
        cLib_addCalcAngleS(&m0386, 0, 30, 0x1000, 1);
    } else {
        cLib_addCalcAngleS(&m0384, -0x800, 3, 0x500, 0x300);
    }

    m045C.copy(current.pos);
    if (!checkStateFlg(daSFLG_UNK2000000_e)) {
        cLib_chaseS(&m03B8, 0, 0x80);
    } else {
        cLib_chaseS(&m03B8, 0x4E8, 0x80);
    }

    if (mCurMode != 12 && mCurMode != 15) {
        if (mCurMode != 7 && mCurMode != MODE_CRANE_UP_e) {
            if (((u8)ship_exec_play()->mEventRun != 0 || !ship_isLinkPlayer0()) &&
                mEvtStaffId == -1 && mCurMode != MODE_TALK_e &&
                mCurMode != MODE_START_MODE_THROW_e && !checkStateFlg(daSFLG_UNK100000_e)) {
                if (checkStateFlg(daSFLG_UNK20000000_e)) {
                    // HD: down to 20 (GameCube 10)
                    if (!(speedF < 20.0f)) {
                        firstDecrementShipSpeed(20.0f);
                    }
                } else {
                    speedF = 0.0f;
                }
            }
            if ((u8)ship_exec_play()->mEventRun == 0 && !checkStateFlg(daSFLG_FLY_e) &&
                (mTornadoActor || mWhirlActor)) {
                speedF = prev_speedF;
                f32 fVar4;
                if (mTornadoActor) {
                    fVar4 = gabi::fmadds(40.0f, m0404, 30.0f);
                    if (!(ship_playerStatus0() & 0x1000 /* daPyStts0_BOW_AIM_e */) ||
                        !((u32)ship_exec_play()->mCameraAttentionStatus &
                          0x20 /* camera attention status */)) {
                        fVar4 *= 1.2f;
                    }
                } else {
                    fVar4 = gabi::fmadds(30.0f, m0404, 10.0f);
                    if ((u8)gabi::at<ShipExecSave>(dSv_base())->mBombNum ==
                            0 /* dComIfGs_getBombNum() */
                        && fopAcM_GetRoomNo(this) == 44) {
                        fVar4 *= 1.2f;
                    }
                }
                firstDecrementShipSpeed(fVar4);
            }
            s16 d = m03B8;
            current.angle.y = (s16)(current.angle.y + d);
            shape_angle.y += d;

            if (checkStateFlg(daSFLG_FLY_e)) {
                if (checkStateFlg(daSFLG_UNK2000000_e)) {
                    cLib_chaseF(&speed.y, 20.0f, 1.0f - gravity);
                    speedF = prev_speedF;
                    // HD: towards 100 (GameCube 55)
                    cLib_chaseF(&speedF, 100.0f, 3.0f);
                    offStateFlg(daSFLG_UNK2000000_e);
                }
                if (speed.y > 0.0f && !(speed.y > -gravity)) {
                    onStateFlg(daSFLG_UNK100_e);
                }
                fopAcM_posMoveF(this, &mStts.m_cc_move);
                m1044.copy(*cXyz_Zero);
            } else if ((mTornadoActor || mWhirlActor) && (u8)ship_exec_play()->mEventRun == 0) {
                f32 step;
                if (mTornadoActor) {
                    if ((ship_playerStatus0() & 0x1000) &&
                        ((u32)ship_exec_play()->mCameraAttentionStatus & 0x20)) {
                        step = 5.0f;
                    } else {
                        step = 25.0f;
                    }
                } else if ((u8)gabi::at<ShipExecSave>(dSv_base())->mBombNum == 0 &&
                           fopAcM_GetRoomNo(this) == 44) {
                    step = 10.0f;
                } else {
                    step = 0.6f;
                }
                cLib_chaseF(&m0400, 0.0f, step);
                f32 r = std::fabs((f32)speedF) / m0400;
                f32 a = m040C + r;
                m0408 = r;
                m040C = a;
                bool r23 = false;
                if (mTornadoActor) {
                    s16 ang = ShipExec_radToAngle(a);
                    current.pos.x = gabi::fmadds(m0400, cM_ssin(ang), mTornadoActor->current.pos.x);
                    s16 ang2 = ShipExec_radToAngle((f32)m040C);
                    current.pos.z =
                        gabi::fmadds(m0400, cM_scos(ang2), mTornadoActor->current.pos.z);
                    r23 = true;
                } else if (m0352) {
                    s16 ang = ShipExec_radToAngle(a);
                    current.pos.x = gabi::fmadds(m0400, cM_ssin(ang), mWhirlActor->current.pos.x);
                    s16 ang2 = ShipExec_radToAngle((f32)m040C);
                    current.pos.z = gabi::fmadds(m0400, cM_scos(ang2), mWhirlActor->current.pos.z);
                    r23 = true;
                } else {
                    f32 y = current.pos.y;
                    fopAcM_posMoveF(this, &mStts.m_cc_move);
                    current.pos.y = y;
                }
                if (r23) {
                    gabi::Local<cXyz> sp114;
                    cXyz_mi(&current.pos, sp114.get(), &old.pos);
                    gabi::Local<cXyz> xz;
                    ship_cpf(xz->x, sp114->x);
                    xz->y = 0.0f;
                    ship_cpf(xz->z, sp114->z);
                    f32 displacementLength = cXyz_abs(xz.get());
                    if (displacementLength > 1.0f) {
                        f32 k = speedF / displacementLength;
                        mStts.m_cc_move.z = 0.0f;
                        mStts.m_cc_move.y = 0.0f;
                        mStts.m_cc_move.x = 0.0f;
                        current.pos.x = gabi::fmadds(sp114->x, k, old.pos.x);
                        current.pos.z = gabi::fmadds(sp114->z, k, old.pos.z);
                    }
                }
            } else {
                speed.x = (speedF * cM_ssin(current.angle.y)) * cM_scos(m0370);
                speed.y = -(speedF * cM_ssin(m0370));
                speed.z = (speedF * cM_scos(current.angle.y)) * cM_scos(m0370);
                fopAcM_posMove(this, &mStts.m_cc_move);
                if ((u8)ship_exec_play()->mEventRun != 0) {
                    m1044.copy(*cXyz_Zero);
                } else {
                    gabi::Local<cXyz> sp108;
                    gabi::Local<be<s32>> sp18;
                    gabi::Local<cXyz> nrm;
                    if (mAcch.GetGroundH() != -1000000000.0f &&
                        ShipExec_getPolyRoomPathVec(
                            gabi::ea(static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd)), sp108.get(),
                            sp18.get())) {
                        ShipExec_normalizeZP(sp108.get(), nrm.get());
                        PSVECScale(sp108.get(), sp108.get(), (f32)((s32)*sp18 >> 1));
                        cLib_addCalcPosXZ(&m1044, sp108.get(), 0.5f, 5.0f, 1.0f);
                    } else {
                        cLib_addCalcPosXZ(&m1044, cXyz_Zero, 0.05f, 0.1f, 0.02f);
                    }
                    PSVECAdd(&current.pos, &m1044, &current.pos);
                }
            }
        } else {
            m1044.copy(*cXyz_Zero);
            if (mEvtStaffId == -1) {
                gabi::Local<cXyz> spFC;
                cXyz_mi(&current.pos, spFC.get(), &old.pos);
                if (checkStateFlg(daSFLG_FLY_e)) {
                    gabi::Local<cXyz> xz;
                    ship_cpf(xz->x, spFC->x);
                    xz->y = 0.0f;
                    ship_cpf(xz->z, spFC->z);
                    speedF = cXyz_abs(xz.get());
                } else {
                    speedF = cXyz_abs(spFC.get());
                }
            } else {
                speed.x = (speedF * cM_ssin(current.angle.y)) * cM_scos(m0370);
                if (m0351 != 7 && m0351 != 10 && m0351 != 11) {
                    speed.y = -(speedF * cM_ssin(m0370));
                }
                speed.z = (speedF * cM_scos(current.angle.y)) * cM_scos(m0370);
                fopAcM_posMove(this, &mStts.m_cc_move);
            }
        }
    }
    /* mStts.ClrCcMove() */
    mStts.m_cc_move.z = 0.0f;
    mStts.m_cc_move.y = 0.0f;
    mStts.m_cc_move.x = 0.0f;
    gabi::Local<cXyz> previousPosition;
    previousPosition->copy(current.pos);
    mAcchCir[3].SetWall(-600.0f - (f32)previousPosition->y, 250.0f);
    mAcch.CrrPos(dComIfG_Bgsp());
    if (mTornadoActor || mWhirlActor) {
        ship_cpf(current.pos.y, previousPosition->y);
        ship_cpf(current.pos.z, previousPosition->z);
        ship_cpf(current.pos.x, previousPosition->x);
    }
    setRoomInfo();
    if (mCurMode == MODE_TACT_WARP_e) {
        ship_cpf(current.pos.x, previousPosition->x);
        ship_cpf(current.pos.z, previousPosition->z);
    }
    setYPos();

    f32 diff = m03F4 - current.pos.y;
    gabi::at<ShipExecTrack>(gabi::ea(mTrack))->mBaseY = m03F4; /* mTrack.setWaterY */
    gabi::at<ShipExecTrack>(gabi::ea(mTrack))->mMinY = m03F8;  /* mTrack.setWaterFlatY */
    gabi::Local<be<s16>> sp12;
    gabi::Local<be<s16>> sp10;
    if (!checkStateFlg(daSFLG_FLY_e)) {
        setWaveAngle(sp12.get(), sp10.get());
    } else {
        *sp10 = m037E;
        *sp12 = m037C;
    }
    diff += 5.0f;
    if (diff > m03D8) {
        m037C = shape_angle.x;
        m03D8 = diff;
        m037E = shape_angle.z;
    } else {
        cLib_chaseF(&m03D8, diff, 3.0f);
        m037C = shape_angle.x;
        m037E = shape_angle.z;
    }
    if (mCurMode == MODE_TACT_WARP_e) {
        shape_angle.x = 0;
        shape_angle.z = 0;
    } else {
        shape_angle.x = m0370 + m0384;
        shape_angle.z = (s16)(m036C + m0372);
    }
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_now(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel* model1 = mpBodyAnm->getModel();
    J3DModel* model2 = mpHeadAnm->getModel();
    J3DModel_setBaseTRMtx(model1, mDoMtx_now());
    mpBodyAnm->calc();
    setHeadAnm();
    J3DModel_setBaseTRMtx(model2, model_getAnmMtx(model1, FN_BODY_JNT_J_FN_GATTAI_e));
    mpHeadAnm->calc();

    if (mPart == PART_CRANE_e) {
        if (m0392 == dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e) {
            f32 rate = getAnglePartRate();
            m0398 = (s16)gabi::ftoi(rate * (f32)(s16)mCraneBaseAngle);
            f32 old039C = (s16)m039C;
            m039C = (s16)gabi::ftoi(old039C * getAnglePartRate());
            incRopeCnt(-20, 0);
        }
        J3DModel_calc(mpSalvageArmModel);
        setRopePos();
        Mtx34* am = model_getAnmMtx(mpSalvageArmModel, VFNCR_JNT_V_CRANE_ROTATION_e);
        ship_cpf(m102C.x, am->m[0][3]);
        ship_cpf(m102C.y, am->m[1][3]);
        ship_cpf(m102C.z, am->m[2][3]);
        if (isProc(ShipExecTarget::procCrane) || isProc(ShipExecTarget::procCraneUp)) {
            ShipExecRopeActor* rope = gabi::at<ShipExecRopeActor>(gabi::ea(this));
            mCraneTop =
                gabi::at<ShipExecRopePositions>(gabi::ea(rope->mpRopePositions))->mpPosition;
        } else {
            mCraneTop = nullptr;
        }
    } else {
        // HD: ended (GameCube remove())
        dPa_rippleEcallBack_end((dPa_rippleEcallBack*)m19C0);
        dPa_rippleEcallBack_end((dPa_rippleEcallBack*)mRipple);
        dPa_followEcallBack_end_l(&m19AC);
        mRopeCnt = 0;
        mCraneTop = nullptr;
        if (mPart == PART_CANNON_e) {
            Mtx34* cm = model_getAnmMtx(mpCannonModel, VFNCN_JNT_CANON2_e);
            ship_cpf(m1038.x, cm->m[0][3]);
            ship_cpf(m1038.y, cm->m[1][3]);
            ship_cpf(m1038.z, cm->m[2][3]);
            if (!isProc(ShipExecTarget::procCannon)) {
                m0396 = (s16)gabi::ftoi(getAnglePartRate() * 16384.0f);
                f32 old0394 = (s16)m0394;
                m0394 = (s16)gabi::ftoi(old0394 * getAnglePartRate());
            }
            J3DModel_calc(mpCannonModel);
            if (m037A == 30) {
                gabi::Local<cXyz> spE4;
                PSMTXMultVec(model_getAnmMtx(mpCannonModel, VFNCN_JNT_CANON2_e), &cannon_top,
                             spE4.get());
                gabi::Local<csXyz> sp1C;
                sp1C->z = shape_angle.z;
                sp1C->x = shape_angle.x + m0396 - 0x4000; /* getCannonAngleX() */
                sp1C->y = shape_angle.y + m0394;          /* getCannonAngleY() */
                u32 prm = ShipExec_makeBombParameter(4, 0, 1);
                fopAc_ac_c* bomb =
                    ShipExec_fastCreate(ShipExecProcess::fpcNm_BOMB_e, prm, spE4.get(),
                                        (s32)tevStr.mRoomNo, sp1C.get(), 0, -1, 0, 0);
                if (bomb) {
                    u32 id = bomb ? (u32)bomb->mBsPcId : 0xFFFFFFFFu;
                    ShipExec_forceCameraLockOn(dCam_getBody(), id);
                    ShipExec_setBombNoGravityTime(bomb, 8);
                    bomb->speedF = cM_scos(sp1C->x) * 110.0f;
                    bomb->speed.y = -(cM_ssin(sp1C->x) * 110.0f);
                    bomb->gravity = -2.5f;
                    seStart(ShipExecSound::JA_SE_LK_SHIP_CANNON_FIRE, &m1038);
                    onStateFlg(daSFLG_SHOOT_CANNON_e);
                    ShipExecPlay* play = ship_exec_play();
                    play->mItemBombNumCount = play->mItemBombNumCount - 1;
                    mpHeadAnm->mFrameCtrl.mRate = 1.0f;
                    mpHeadAnm->mFrameCtrl.mFrame = 0.0f;
                }
                m037A--;
            }
            // HD: the cannon sight
            ship_sightLaunch(this);
            ShipExecSight& sight = ship_exec_sight(this);
            u16 ax = sight.mAngle.x;
            sight.mSpeed = cM_scos(ax) * 110.0f;
            sight.mVelocity.y = -(cM_ssin(ax) * 110.0f);
            sight.mGravity = -2.5f;
            ship_cannonSight(this);
        }
    }

    PSMTXMultVec(model_getAnmMtx(model1, FN_BODY_JNT_J_FN_STEER1_e), &tiller_top_offset,
                 &mTillerTopPos);
    Mtx34* mtx = model_getAnmMtx(model1, FN_BODY_JNT_J_FN_SAIL1_e);
    ship_cpf(m0444.x, mtx->m[0][3]);
    ship_cpf(m0444.y, mtx->m[1][3]);
    u32 grid = mpGrid;
    ship_cpf(m0444.z, mtx->m[2][3]);
    if (grid) {
        /* static cXyz top_offset(0.0f, 0.0f, -365.0f); XZ_top_offset(265.0f, 0.0f, 0.0f) */
        cXyz* top_offset =
            ship_exec_staticVec(top_offset_guard, top_offset_storage, 0.0f, 0.0f, -365.0f);
        cXyz* XZ_top_offset =
            ship_exec_staticVec(XZ_top_offset_guard, XZ_top_offset_storage, 265.0f, 0.0f, 0.0f);
        fopAc_ac_c* g = gabi::at<fopAc_ac_c>(mpGrid);
        g->current.pos.copy(m0444);
        g = gabi::at<fopAc_ac_c>(mpGrid);
        g->current.angle.x = shape_angle.x;
        g->current.angle.y = shape_angle.y;
        g->current.angle.z = shape_angle.z;
        gabi::Local<cXyz> spD8;
        PSMTXMultVecSR(mtx, top_offset, spD8.get());
        gabi::at<fopAc_ac_c>(mpGrid)->scale.y = cXyz_abs(spD8.get()) / 365.0f;
        PSMTXMultVecSR(model_getAnmMtx(model1, 8 /* J_FN_SAIL2 */), XZ_top_offset, spD8.get());
        gabi::at<ShipExecGrid>(grid)->field_0x2200 =
            1.0f - cXyz_abs(spD8.get()) / 265.0f; /* field_0x2200 */
        if (mTornadoActor) {
            /* mpGrid->force_calc_wind_rel_angle(0x3000) */
            gabi::at<ShipExecGrid>(mpGrid)->force_calc_wind_rel_angle(0x3000);
        }
    }

    s16 dv = m0366 - sVar26;
    s32 angleDifferenceMagnitude = dv < 0 ? -dv : dv;
    s32 angleProduct = dv * m0366;
    if (angleProduct > 0 && (f32)angleDifferenceMagnitude > 420.00003f) {
        seStart(ShipExecSound::JA_SE_LK_SHIP_RUDDER_OUT, &mTillerTopPos);
    } else if (angleProduct < 0 && (f32)angleDifferenceMagnitude > 525.0f) {
        seStart(ShipExecSound::JA_SE_LK_SHIP_RUDDER_OUT, &mTillerTopPos);
    }

    if (!checkStateFlg(daSFLG_FLY_e)) {
        // HD: full at speed 100 (GameCube 55)
        f32 shipCruiseSpeed = std::fabs((f32)speedF) / 100.0f;
        if (shipCruiseSpeed > 1.0f) {
            shipCruiseSpeed = 1.0f;
        } else {
            shipCruiseSpeed = shipCruiseSpeed >= 0.0f ? shipCruiseSpeed : 0.0f;
        }
        ShipExec_playCruiseSound(&current.pos, shipCruiseSpeed);
    }

    Mtx34* jnt_mtx = model_getAnmMtx(model2, 0x10 /* J_FN_ME_L */);
    ship_cpf(eyePos.x, jnt_mtx->m[0][3]);
    ship_cpf(eyePos.y, jnt_mtx->m[1][3]);
    ship_cpf(eyePos.z, jnt_mtx->m[2][3]);

    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> spCC;
    cXyz_mi(&link->current.pos, spCC.get(), &current.pos);
    f32 distXz = gabi::fmadds(spCC->x, cM_ssin(shape_angle.y), spCC->z * cM_scos(shape_angle.y));
    ship_cpf(attention_info.position.x, eyePos.x);
    attention_info.position.y = eyePos.y + 30.0f;
    ship_cpf(attention_info.position.z, eyePos.z);
    attention_info.flags = 0;

    bool r23_2 = false;
    gabi::Local<cXyz> spC0;
    if (checkStateFlg(daSFLG_UNK1000000_e)) {
        cXyz_mi(&m1068, spC0.get(), &eyePos);
        offStateFlg(daSFLG_UNK1000000_e);
        r23_2 = true;
    } else if (m0428) {
        cXyz_mi(m0428, spC0.get(), &eyePos);
        r23_2 = true;
    } else if ((mCurMode == MODE_CRANE_e || mCurMode == MODE_CRANE_UP_e) && mCraneTop &&
               mRopeCnt > 0) {
        cXyz_mi(mCraneTop, spC0.get(), &eyePos);
        r23_2 = true;
    } else if (mCurMode == MODE_TALK_e || distXz > 125.0f) {
        if (!(ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) &&
            (!dComIfGs_isEventBit(0x3910) || dComIfGs_isEventBit(0x2D02)) &&
            (u8)ship_exec_play()->mMiniGameType != 1 /* dComIfGp_getMiniGameType() */) {
            if (fopAcM_searchActorDistanceXZ2(this, dComIfGp_getPlayer(0)) < 250000.0f) {
                cXyz_mi(&link->eyePos, spC0.get(), &eyePos);
                attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;
                r23_2 = true;
            }
        }
    } else {
        attention_info.position.copy(current.pos);
        if (!(ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) && dComIfGs_isEventBit(0x0908) &&
            !checkStateFlg(daSFLG_UNK800000_e)) {
            if (dComIfGs_isEventBit(0x1980) || !dComIfGs_isEventBit(0x0902)) {
                attention_info.flags = fopAc_Attn_ACTION_SHIP_e;
            } else if (dComIfGs_isEventBit(0x0A80)) {
                cXyz* windVec = dKyw_get_wind_vec();
                s32 a = cM_atan2s(windVec->x, windVec->z);
                if ((a < 0 ? -a : a) < 0x1000) {
                    attention_info.flags = fopAc_Attn_ACTION_SHIP_e;
                }
            }
        }
    }

    s16 sVar16, sVar5;
    if (r23_2) {
        f32 sx = spC0->x, sy = spC0->y, sz = spC0->z;
        gabi::Local<cXyz> xz;
        ship_cpf(xz->z, spC0->z);
        ship_cpf(xz->x, spC0->x);
        xz->y = 0.0f;
        sVar16 = cM_atan2s(-sy, cXyz_abs(xz.get())) - shape_angle.x;
        if (sVar16 > 0x2000) {
            sVar16 = 0x2000;
        } else if (sVar16 < -0x3000) {
            sVar16 = -0x3000;
        }
        sVar5 = cM_atan2s(sx, sz) - shape_angle.y;
        if (sVar5 > 0x7800) {
            sVar5 = 0x7800;
        } else if (sVar5 < -0x7800) {
            sVar5 = -0x7800;
        }
        sVar16 = sVar16 / 6;
        sVar5 = sVar5 / 6;
    } else {
        sVar16 = 0;
        sVar5 = 0;
    }
    if (sVar16 == 0 || cLib_distanceAngleS(sVar16, m03A0) > 0x100) {
        cLib_addCalcAngleS(&m03A0, sVar16, 10, 0x400, 4);
    }
    if (sVar5 == 0 || cLib_distanceAngleS(sVar5, m03A2) > 0x100) {
        cLib_addCalcAngleS(&m03A2, sVar5, 10, 0x400, 4);
    }

    if (mCurMode != MODE_TALK_e && checkForceMessage()) {
        attention_info.flags = (u32)attention_info.flags & ~(u32)fopAc_Attn_ACTION_SHIP_e;
        gabi::Local<cXyz> spB4;
        cXyz_mi(&attention_info.position, spB4.get(), &link->current.pos);
        bool talk;
        if ((u8)ship_exec_play()->mEventRun == 0 &&
            (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e)) {
            talk = true;
        } else {
            talk = false;
            if (std::fabs(current.pos.y - link->current.pos.y) < 50.0f) {
                gabi::Local<cXyz> xz;
                ship_cpf(xz->x, spB4->x);
                xz->y = 0.0f;
                ship_cpf(xz->z, spB4->z);
                if (PSVECSquareMag(xz.get()) < 62500.0f &&
                    ShipExec_seenActorAngleY(this, dComIfGp_getPlayer(0)) < 0x6000 &&
                    mNextMessageNo != 0xD65) {
                    talk = true;
                }
            }
        }
        if (talk) {
            fopAcM_orderSpeakEvent(this);
            offStateFlg(daSFLG_UNK400000_e);
            attention_info.flags =
                (u32)attention_info.flags | fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;
            eventInfo_onCondition(this, 1);
        }
    }
    if ((dComIfGs_isEventBit(0x2D10) && !ship_checkMasterSwordEquip()) ||
        (dComIfGs_isEventBit(0x3804 /* HYRULE_COURTYARD_CUTSCENE */) &&
         !dComIfGs_isEventBit(0x2D02)) ||
        (dComIfGs_isEventBit(0x3E10) && !dComIfGs_isEventBit(0x3F80))) {
        attention_info.flags = (u32)attention_info.flags & ~(u32)fopAc_Attn_ACTION_SHIP_e;
    }
    if (mCurMode != MODE_TALK_e && (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e)) {
        u32 no = 0;
        if (dComIfGs_isEventBit(0x1E40) && !dComIfGs_isEventBit(0x3840)) {
            no = 0x168c;
        } else if (dComIfGs_isEventBit(0x2D02) && !dComIfGs_isEventBit(0x3201)) {
            no = 0x1645;
        } else if (dComIfGs_isEventBit(0x1820) && !dComIfGs_isEventBit(0x3380)) {
            no = 0x164d;
        } else if (dComIfGs_getTriforceNum() == 8 && !dComIfGs_isEventBit(0x3D04)) {
            no = 0x1682;
        } else if (dComIfGs_isEventBit(0x3E40) && !dComIfGs_isEventBit(0x3E20)) {
            no = 0x1683;
        }
        if (no) {
            mNextMessageNo = no;
            fopAcM_orderSpeakEvent(this);
            attention_info.flags =
                (u32)attention_info.flags | fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;
            offStateFlg(daSFLG_UNK400000_e);
            eventInfo_onCondition(this, 1);
        }
    }

    // HD: the sail texture scroll follows speed / 100 (GameCube 55)
    f32 p = (0.1f * speedF) / 100.0f;
    mTillerAngleRate = (f32)(s16)m0366 * (1.0f / 0x2000);
    f32 t = m03D4;
    if (p > 0.05f) {
        t = t + 0.05f;
    } else {
        if (p < -0.05f) {
            p = -0.05f;
        }
        t = t + p;
    }
    if (t > 1.0f) {
        t = t - 1.0f;
    } else if (t < 0.0f) {
        t = t + 1.0f;
    }
    m03D4 = t;

    gabi::Local<cXyz> spA8;
    cXyz_mi(&current.pos, spA8.get(), &old.pos);
    gabi::Local<cXyz> xz3;
    ship_cpf(xz3->x, spA8->x);
    ship_cpf(xz3->z, spA8->z);
    xz3->y = 0.0f;
    if (!(speedF < 0.0f)) {
        mFwdVel = cXyz_abs(xz3.get());
    } else {
        mFwdVel = -cXyz_abs(xz3.get());
    }
    f32 v = mFwdVel +
            gabi::fmadds(m1044.x, cM_ssin(current.angle.y), m1044.z * cM_scos(current.angle.y));
    mFwdVel = v;
    if (!(std::fabs(v) > 0.01f) || prev_fwdvel * v < 0.0f || v * speedF < 0.0f) {
        ship_stopWaves(this);
    }

    setEffectData(m03F4, *sp10);

    f32 sn = cM_ssin(shape_angle.y);
    f32 cs = cM_scos(shape_angle.y);
    gabi::Local<cXyz> sp9C;
    for (int i = 0; i < 3; i++) {
        f32 off = (f32)cyl_offset.mOffset[i]; /* cyl_offset[i] */
        sp9C->x = gabi::fmadds(off, sn, current.pos.x);
        sp9C->y = current.pos.y - 30.0f;
        sp9C->z = gabi::fmadds(off, cs, current.pos.z);
        mCyl[i].SetC(sp9C.get());
        dComIfG_Ccsp_Set(&mCyl[i]);
        /* SetTgGrp: IsPlayer, or IsPlayer | IsOther */
        u32 grp = (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) ? 4 : 0xC;
        mCyl[i].mObjTg.mSPrm = (mCyl[i].mObjTg.mSPrm & ~0xEu) | grp;
    }
    PSMTXMultVec(model_getAnmMtx(mpHeadAnm->getModel(), FN_HEAD_H_JNT_J_FN_ATAMA_e), sph_offset,
                 sp9C.get());
    {
        u32 grp = (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) ? 4 : 0xC;
        mSph.mObjTg.mSPrm = (mSph.mObjTg.mSPrm & ~0xEu) | grp;
    }
    mSph.SetC(sp9C.get());
    if (mCurMode != MODE_CANNON_e) {
        dComIfG_Ccsp_Set(&mSph);
    } else {
        ShipExec_resetTargetHit(&mSph);
    }
    u8 part = mPart;
    ShipExecPlay* play = ship_exec_play();
    if (part == PART_STEER_e) {
        play->mPlayerStatus1 = play->mPlayerStatus1 | 0x400; /* daPyStts1_SAIL_e */
        ShipExec_setSailSoundState(1);
    } else {
        play->mPlayerStatus1 = play->mPlayerStatus1 & ~0x400u;
        ShipExec_setSailSoundState(0);
    }
    // HD: the shadow texture
    setShadowBtkRate();
    offStateFlg(daSFLG_UNK40000000_e);
    mNextMode = mCurMode;
    return TRUE;
}

// clang-format off
/* WWHD glue
 str 1003A5CC "Ship" in 02477A24
 data cyl_offset 101D03E4
 data cannon_top 101D03B4
 data sight_scroll_phase 101D0348
 data XZ_top_offset_guard 1046DDE4
 data XZ_top_offset_storage 1046DD94
 data top_offset_guard 1046DDE0
 data top_offset_storage 1046DD88
 data sph_offset_guard 1046DDDC
 data sph_offset_storage 1046DD7C
 data sail_offset_guard 1046DDD8
 data sail_offset_storage 1046DD70
 ext 0251641C void ShipExec_clearCollisionHit(dCcD_Sph*)
 ext 0200E2F4 void ShipExec_removeCollision(void*, dCcD_Sph*)
 ext 02544830 u32 ShipExec_getMyNowCutName(void*, s32)
 ext 020079B4 f64 ShipExec_getStickValue(u32)
 ext 02007A1C s32 ShipExec_getStickAngle(u32)
 ext 027F2FC4 void ShipExec_updateFrame(void*)
 ext 02019510 s16 ShipExec_radToAngle(f32)
 ext 025AB184 BOOL ShipExec_getPolyRoomPathVec(u32, cXyz*, be<s32>*)
 ext 0201B3C0 void ShipExec_normalizeZP(cXyz*, cXyz*)
 ext 020CB8D8 u32 ShipExec_makeBombParameter(u32, u32, u32)
 ext 025D5928 fopAc_ac_c* ShipExec_fastCreate(u32, u32, cXyz*, s32, csXyz*, u32, s32, u32, u32)
 ext 02515470 void ShipExec_forceCameraLockOn(u32, u32)
 ext 020CB89C void ShipExec_setBombNoGravityTime(fopAc_ac_c*, s32)
 ext 025E1CC4 void ShipExec_playCruiseSound(cXyz*, f32)
 ext 025D68A0 s32 ShipExec_seenActorAngleY(fopAc_ac_c*, fopAc_ac_c*)
 ext 02516264 void ShipExec_resetTargetHit(dCcD_Sph*)
 ext 025E1CD4 void ShipExec_setSailSoundState(u32)
*/
