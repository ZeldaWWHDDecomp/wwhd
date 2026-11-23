/**
 * d_a_npc_kamome_exec.cpp (WWHD)
 * Player - Hyoi Seagull: execute
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_kamome.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_kamome.h"
#include "d/actor/d_a_npc_kamome_t60.h" /* true60: player per step / NPC whole form */

using KamomeGroundCheck = dBgS_GndChk;
// HD: daPy_npc_c::checkNowPosMove binding.
static inline BOOL daPy_npc_checkNowPosMove(fopAc_ac_c* a, const char* name) { return kam_checkPositionMove(a, name); }

// HD: below its minimum height, or close to the ground (debug-register margins), the seagull is
// pitched up and lifted; never sinks while touching water.

/* inlined into daNpc_kam_c::execute */
static inline void kam_groundCheck(daNpc_kam_c* i_this) {
    gabi::Local<KamomeGroundCheck> gnd_chk;
    dBgS_GndChk_ct(gnd_chk, gc_tu.gndChk, false);
    f32 x = i_this->current.pos.x;
    f32 y = i_this->current.pos.y;
    f32 y150 = y + 150.0f;
    f32 z = i_this->current.pos.z;
    f32 gy_set = y150 + REG_F(18, 10);
    gnd_chk->m_pos.x = x; /* SetPos */
    gnd_chk->m_pos.y = gy_set;
    gnd_chk->m_pos.z = z;
    f32 groundY = cBgS_GroundCross(dComIfG_Bgsp(), gnd_chk);
    f32 limit = (groundY - 10.0f) + REG_F(18, 11);
    f32 py = i_this->current.pos.y;
    if (py < limit) {
        f32 low = (limit - 30.0f) + REG_F(18, 13);
        if (py > low) {
            f32 sy = i_this->speed.y;
            i_this->mTargetAngleX = (s16)-i_this->mAngVelX;
            i_this->mLockAngleXTimer = 30;
            if (sy < 0.0f) {
                i_this->speed.y = 0.0f;
            }
        } else {
            f32 floor = (limit - 90.0f) + REG_F(18, 12);
            if (!(py < floor)) {
                f32 sy = i_this->speed.y;
                i_this->current.pos.y = floor;
                i_this->mLockAngleXTimer = 30;
                i_this->mTargetAngleX = i_this->mAngVelX;
                if (sy > 0.0f) {
                    i_this->speed.y = 0.0f;
                }
            }
        }
    }
    /* ~dBgS_GndChk */
    gnd_chk->cBgS_PolyInfo::__vtbl = gc_tu.gndChkDt[0];
    gnd_chk->dBgS_PolyPassChk::__vtbl = gc_tu.gndChkDt[1];
    gnd_chk->dBgS_GrpPassChk::__vtbl = gc_tu.gndChkDt[2];
    kam_destroyCheck(gnd_chk.get(), 0); /* cBgS_Chk::~cBgS_Chk */
}

// HD: on the sea in room 44, the seagull is kept out of a circle around one spot (debug-register
// radius); it is pushed back to the circle's edge

/* inlined into daNpc_kam_c::execute */
static inline void kam_keepOut(daNpc_kam_c* i_this) {
    f32 dy = 3737.0f - i_this->current.pos.y;
    f32 dx = -200460.0f - i_this->current.pos.x;
    f32 r = REG_F(10, 10) + 200.0f;
    f32 dz = 321951.0f - i_this->current.pos.z;
    gabi::Local<cXyz> delta;
    delta->y = dy;
    delta->x = dx;
    delta->z = dz;
    if (std::fabs(dy) < r) {
        gabi::Local<cXyz> xz;
        xz->z = dz;
        xz->x = dx;
        xz->y = 0.0f;
        f32 dist = std_sqrtf(PSVECSquareMag(xz));
        if (dist < REG_F(10, 11) + 200.0f) {
            Mtx34* m = calc_mtx();
            s16 angle = cM_atan2s(delta->x, delta->z);
            mDoMtx_YrotS(m, angle);
            f32 edge = REG_F(10, 11) + 200.0f;
            delta->y = 0.0f;
            delta->x = 0.0f;
            delta->z = -edge;
            MtxPosition(delta, delta);
            i_this->current.pos.x = delta->x + -200460.0f;
            i_this->current.pos.z = delta->z + 321951.0f;
        }
    }
}

/* 0225E598 */
// HD: nothing moves while the seagull waits hidden; ground and keep-out checks (above);
// the water splash uses dPa_control_c::set directly; with REG0_S(0) == 0 current.angle follows
// shape_angle (GameCube: the reverse); no checkOrder; the mass collision replaces the tg sphere
BOOL daNpc_kam_c::execute() {
    kamT60::Form<daNpc_kam_c> t60f(this); /* true60: the frame's form (player: per step; NPC: whole on tick30), from its first step */
    if (t60f.skip()) /* true60: NPC form, a step before the tick30 step: no logic (picture: frame interpolation) */
        return TRUE;

    l_demo_start_chk_cnt = 0;

    /* static JGeometry::TVec3<f32> splash_scale(0.6f, 0.6f, 0.6f), ripple_scale(1.0f, 1.0f, 1.0f) */
    if (splash_guard == 0) {
        splash_guard = 1;
        kam_setVec(&splash_scale, 0.6f, 0.6f, 0.6f);
    }
    if (ripple_guard == 0) {
        ripple_guard = 1;
        kam_setVec(&ripple_scale, 1.0f, 1.0f, 1.0f);
    }

    actor_status = actor_status & ~0x20u; /* fopAcM_OffStatus(this, fopAcStts_SHOWMAP_e) */
    if (t60c::first()) /* true60: a count, on the frame's first step (first-step AI-timer) */
        kam_calcTimerU8(&mDamageFogTimer);

    if (!t60c::held(isReturnLink() != 0, 0x7D, this) && mHidden == 0) { /* true60: a return set on a step acts next frame */
        if (daPy_npc_checkNowPosMove(this, l_staff_name.get())) {
            if (t60c::first()) { /* true60: the speed / turn-rate chases once a frame at dt 1, held (rule (a)) */
            kamT60::Dt1<> t60dt;
            daNpc_kam_HIO1_c& hio = l_HIO.mHio1;
            f32 target = mTargetSpeedF;
            f32 spd = speedF;
            f32 accel = hio.mAccelF;
            mAccelF = accel;
            if (spd < target) {
                spd = gabi::fadds_ppc(spd, accel);
                f32 t2 = mTargetSpeedF;
                speedF = spd;
                if (spd > t2) {
                    speedF = t2;
                }
            } else if (spd > target) {
                spd = gabi::fsubs_ppc(spd, accel);
                f32 t2 = mTargetSpeedF;
                speedF = spd;
                if (spd < t2) {
                    speedF = t2;
                }
            }

            s16 targetY = mTargetAngVelY;
            cLib_addCalcAngleS(&mAngVelY, targetY, hio.mAngVelStepScale, hio.mAngVelMaxStep, hio.mAngVelMinStep);
            cLib_addCalcAngleS(&mAngVelX, mTargetAngVelX, hio.mAngVelStepScale, hio.mAngVelMaxStep, hio.mAngVelMinStep);
            } /* true60 */

            if (t60c::first()) { /* true60: the frame's velocity (and the ground / water / floor checks with their pitch lock) once a
                                  * frame from the frame-start place, held by the later steps (the flight's frame form) */
            f32 sp = speedF;
            f32 sinX = cM_ssin((u16)current.angle.x);
            f32 cosX = cM_scos((u16)current.angle.x);
            f32 cosXSpeed = gabi::fmuls_ppc(sp, cosX);
            speed.y = -gabi::fmuls_ppc(sp, sinX);
            speed.x = gabi::fmuls_ppc(cosXSpeed, cM_ssin((u16)current.angle.y));
            speed.z = gabi::fmuls_ppc(cosXSpeed, cM_scos((u16)current.angle.y));

            if (!isNoBgCheck()) {
                if (current.pos.y < mMinY || isWaterHit()) {
                    if (speed.y < 0.0f) {
                        speed.y = 0.0f;
                    }
                } else {
                    kam_groundCheck(this);
                }
                BOOL special = FALSE;
                if (kam_isStartStage("sea") && current.roomNo == 44) {
                    special = TRUE;
                }
                if (special) {
                    kam_keepOut(this);
                }
            }
            } /* true60 */
            bool t60mv = kamT60::moveExact(this); /* true60: the frame's move exact */
            if (t60mv) {
                t60c::moveBegin(&current.pos.x, &current.pos.z, 0x80);
                t60c::moveBegin(&current.pos.y, &current.pos.y, 0x81); /* (y on its own key: x0 / z0 both y) */
            }
            fopAcM_posMove(this, &mStts.m_cc_move);
            if (t60mv) {
                t60c::moveEnd(&current.pos.x, &current.pos.z, &speed.x, &speed.z, 0x80);
                t60c::moveEnd(&current.pos.y, &current.pos.y, &speed.y, &speed.y, 0x81);
            }
        }

        if (!isNoBgCheck()) {
            mAcch.CrrPos(dComIfG_Bgsp());
            if (!(mAcch.GetGroundH() == -1000000000.0f /* -G_CM3D_F_INF */)) {
                u32 gnd = gabi::ea(static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd)); /* mAcch.m_gnd */
                s8 roomNo = (s8)kam_getGroundRoom(dComIfG_Bgsp(), gnd); /* dBgS::GetRoomId */
                current.roomNo = roomNo;           /* fopAcM_SetRoomNo */
                tevStr.mRoomNo = roomNo; /* tevStr.mRoomNo */
                u8 color = (u8)kam_getGroundColor(dComIfG_Bgsp(), gnd); /* dBgS::GetPolyColor */
                mPolyInfo.mPolyIndex = static_cast<cBgS_PolyInfo&>(mAcch.m_gnd).mPolyIndex;  /* mPolyInfo.SetPolyInfo(mAcch.m_gnd) */
                mPolyInfo.mBgIndex = static_cast<cBgS_PolyInfo&>(mAcch.m_gnd).mBgIndex;
                mPolyInfo.mpBgW = static_cast<cBgS_PolyInfo&>(mAcch.m_gnd).mpBgW;
                mPolyInfo.mProcId = static_cast<cBgS_PolyInfo&>(mAcch.m_gnd).mActorId;
                mStts.mRoomId = (u8)roomNo;        /* mStts.SetRoomId(roomNo) */
                tevStr.mEnvrIdxOverride = color; /* tevStr.mEnvrIdxOverride */
            }
            u32 flags = mHitFlags;
            if (mAcch.m_flags & 0x80000 ) {
                if (!(flags & 8)) {
                    mHitFlags = flags | 8;
                    JPABaseEmitter* splash = dPa_control_set(dComIfGp_getParticle(), 0, 0x40 /* ID_IT_JN_WP_SHIBUKI */,
                                                             &current.pos, nullptr, nullptr, 0xFF, nullptr, -1,
                                                             nullptr, nullptr, nullptr);
                    if (splash != nullptr) {
                        KamomeEmitter* e = reinterpret_cast<KamomeEmitter*>(splash);
                        splash->setRate(15.0f); /* setRate */
                        f32 sx = splash_scale.x;
                        f32 sy = splash_scale.y;
                        f32 sz = splash_scale.z;
                        kam_setVec(&e->mGlobalScale, sx, sy, sz); /* setGlobalScale */
                        kam_setVec(&e->mGlobalScaleOut, sx, sy, sz);
                    }
                    /* dComIfGp_particle_setSingleRipple */
                    JPABaseEmitter* ripple = dPa_control_set(dComIfGp_getParticle(), 5, 0x3D /* ID_IT_JN_WP_HAMON01 */,
                                                             &current.pos, nullptr, nullptr, 0xFF,
                                                             &l_rippleCallback, -1, nullptr,
                                                             nullptr, nullptr);
                    if (ripple != nullptr) {
                        KamomeEmitter* e = reinterpret_cast<KamomeEmitter*>(ripple);
                        f32 sx = ripple_scale.x;
                        f32 sy = ripple_scale.y;
                        f32 sz = ripple_scale.z;
                        kam_setVec(&e->mGlobalScale, sx, sy, sz);
                        kam_setVec(&e->mGlobalScaleOut, sx, sy, sz);
                    }
                }
            } else {
                mHitFlags = flags & ~8u;
            }
        }

        setLineBgCheck();
        if (daPy_npc_checkNowPosMove(this, l_staff_name.get())) {
            if (kamT60::anmPending(this)) /* true60: a requested set pending: the old animation plays on (pose only) */
                kamT60::playOld(this);
            else
            animationPlay();
        }
    }

    kamT60::anmApply(this); /* true60: the tick30 step, after its play, before the dispatch: a pending request's morf set */
    BOOL inEvent = kamT60::eventStep(this, t60f); /* true60: the event branch once a frame on the tick30 step (2026-11-21 (a)) */
    if (!inEvent) {
        if (dComIfGp_getPlayer(0) == this) {
            actor_status = (actor_status & ~0x3Fu) | 0x34; /* fopAcM_SetStatusMap(this, 0x14), SHOWMAP */
            if (t60c::held(isReturnLink() != 0, 0x7E, this)) { /* true60: as above (the frame's first step's answer) */
                mEventState = 0;
            } else {
                playerAction(nullptr);
                if (returnLinkCheck()) {
                    returnLink();
                }
            }
        } else {
            npcAction(nullptr);
        }
        if (kamT60::steerStep()) { /* true60: the steering once a frame on the tick30 step at dt 1 */
        kamT60::Dt1<> t60dt;
        if (REG0_S(0) == 0) {
            cLib_addCalcAngleS2(&current.angle.x, shape_angle.x, 8, 0x400);
            cLib_addCalcAngleS2(&current.angle.z, shape_angle.z, 8, 0x400);
        } else {
            cLib_addCalcAngleS(&shape_angle.x, current.angle.x, 8, 0x2000, 0x400);
            cLib_addCalcAngleS(&shape_angle.z, current.angle.z, 8, 0x2000, 0x400);
        }
        } /* true60 */
    }

    if (t60c::end()) /* true60: the event order once a frame, on the tick30 step */
        eventOrder();
    t60f.model(); /* true60: the frame reached its model step */
    setBaseMtx();

    if (!inEvent && dComIfGp_getPlayer(0) == this && t60c::end()) { /* true60: registered on the tick30 step (one-sweep-per-frame attack) */
        setCollision();
    }

    kam_clearSphereCollision(&mAtSph); /* ClrCoHit */
    mAtSph.ClrTgHit();
    mCps.ClrAtHit();
    if (t60c::end()) { /* true60: the tick30 Move's push, moved dt a step over the frame, cleared on the tick30 step */
    mStts.m_cc_move.z = 0.0f;
    mStts.m_cc_move.x = 0.0f;
    mStts.m_cc_move.y = 0.0f;
    }
    return TRUE;
}

/* WWHD glue (tools/verify/autoglue.py)
str 1001B670 in 0225E598
*/
