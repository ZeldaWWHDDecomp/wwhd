/**
 * d_a_npc_kamome_exec.cpp (WWHD)
 * Player - Hyoi Seagull: execute (0225E598)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_kamome.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_kamome.h"

/* this TU's dBgS_GndChk vtables (execute) */
static const dBgS_GndChk_vt KAM_GNDCHK_VT = {0x1001B4AC, 0x1001B4BC, 0x1001B4DC, 0x1001B4CC};
struct kam_chk54 { u8 _[0x54]; };
/* 02445950 daPy_npc_c::checkNowPosMove(const char* staffName) */
static inline BOOL daPy_npc_checkNowPosMove(fopAc_ac_c* a, const char* name) { return gabi::call<BOOL>(0x02445950, a, name); }

/* HD: below its minimum height, or close to the ground (debug-register margins), the seagull is
 * pitched up and lifted; never sinks while touching water. */
static inline void kam_groundCheck(daNpc_kam_c* i_this) {
    gabi::Local<kam_chk54> gnd_chk;
    dBgS_GndChk_ct(gnd_chk, KAM_GNDCHK_VT, false);
    u32 g = gabi::ea(gnd_chk.get());
    f32 x = i_this->current.pos.x;
    f32 y = i_this->current.pos.y;
    f32 y150 = y + 150.0f;
    f32 z = i_this->current.pos.z;
    f32 gy_set = y150 + REG_F(18, 10);
    gabi::store<f32>(g + 0x24, x); /* SetPos */
    gabi::store<f32>(g + 0x28, gy_set);
    gabi::store<f32>(g + 0x2C, z);
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
    gabi::store<u32>(g + 0x20, 0x1001B4BC);
    gabi::store<u32>(g + 0x40, 0x1001B4DC);
    gabi::store<u32>(g + 0x4C, 0x1001B49C);
    gabi::call(0x02008DAC, gnd_chk.get(), 0); /* cBgS_Chk::~cBgS_Chk */
}

/* HD: on the sea in room 44, the seagull is kept out of a circle around one spot (debug-register
 * radius); it is pushed back to the circle's edge */
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

/* 0225E598 HD: nothing moves while the seagull waits hidden; ground and keep-out checks (above);
 * the water splash uses dPa_control_c::set directly; with REG0_S(0) == 0 current.angle follows
 * shape_angle (GameCube: the reverse); no checkOrder; the mass collision replaces the tg sphere */
BOOL daNpc_kam_c::execute() {
    WWHD_FUNC(0x0225E598, BOOL, this);
    gabi::store<s16>(KAM_L_DEMO_CHK_CNT, 0);

    /* static JGeometry::TVec3<f32> splash_scale(0.6f, 0.6f, 0.6f), ripple_scale(1.0f, 1.0f, 1.0f) */
    if (gabi::load<u32>(KAM_GUARD_SPLASH_SCALE) == 0) {
        gabi::store<u32>(KAM_GUARD_SPLASH_SCALE, 1);
        kam_setVec(KAM_L_SPLASH_SCALE, 0.6f, 0.6f, 0.6f);
    }
    if (gabi::load<u32>(KAM_GUARD_RIPPLE_SCALE) == 0) {
        gabi::store<u32>(KAM_GUARD_RIPPLE_SCALE, 1);
        kam_setVec(KAM_L_RIPPLE_SCALE, 1.0f, 1.0f, 1.0f);
    }

    actor_status = actor_status & ~0x20u; /* fopAcM_OffStatus(this, fopAcStts_SHOWMAP_e) */
    kam_calcTimerU8(&mDamageFogTimer);    /* executeDamageFog() */

    if (!isReturnLink() && mHidden == 0) {
        if (daPy_npc_checkNowPosMove(this, gabi::at<const char>(gabi::load<u32>(KAM_L_STAFF_NAME)))) {
            daNpc_kam_HIO1_l& hio = kam_l_HIO().mHio1;
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
                if (kam_isStartStage(0x1001B670 /* "sea" */) && current.roomNo == 44) {
                    special = TRUE;
                }
                if (special) {
                    kam_keepOut(this);
                }
            }
            fopAcM_posMove(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        }

        if (!isNoBgCheck()) {
            mAcch.CrrPos(dComIfG_Bgsp());
            if (!(mAcch.GetGroundH() == -1000000000.0f /* -G_CM3D_F_INF */)) {
                u32 gnd = gabi::ea(this) + 0x6FC; /* mAcch.m_gnd */
                s8 roomNo = (s8)gabi::call<s32>(0x024EF130, dComIfG_Bgsp(), gnd); /* dBgS::GetRoomId */
                current.roomNo = roomNo;           /* fopAcM_SetRoomNo */
                gabi::store<s8>(gabi::ea(this) + 0x1C9, roomNo); /* tevStr.mRoomNo */
                u8 color = (u8)gabi::call<u32>(0x024EEEB8, dComIfG_Bgsp(), gnd); /* dBgS::GetPolyColor */
                mPolyInfo.mPolyIndex = gabi::load<u16>(gnd);  /* mPolyInfo.SetPolyInfo(mAcch.m_gnd) */
                mPolyInfo.mBgIndex = gabi::load<u16>(gnd + 2);
                mPolyInfo.mpBgW = gabi::load<u32>(gnd + 4);
                mPolyInfo.mProcId = gabi::load<s32>(gnd + 8);
                mStts.mRoomId = (u8)roomNo;        /* mStts.SetRoomId(roomNo) */
                gabi::store<u8>(gabi::ea(this) + 0x1CA, color); /* tevStr.mEnvrIdxOverride */
            }
            u32 flags = mHitFlags;
            if (mAcch.m_flags & 0x80000 /* ChkSeaIn() */) {
                if (!(flags & 8)) {
                    mHitFlags = flags | 8; /* onWaterHit() */
                    JPABaseEmitter* splash = dPa_control_set(dComIfGp_getParticle(), 0, 0x40 /* ID_IT_JN_WP_SHIBUKI */,
                                                             &current.pos, nullptr, nullptr, 0xFF, nullptr, -1,
                                                             nullptr, nullptr, nullptr);
                    if (splash != nullptr) {
                        u32 e = gabi::ea(splash);
                        gabi::store<f32>(e + 0x34, 15.0f); /* setRate */
                        f32 sx = gabi::load<f32>(KAM_L_SPLASH_SCALE);
                        f32 sy = gabi::load<f32>(KAM_L_SPLASH_SCALE + 4);
                        f32 sz = gabi::load<f32>(KAM_L_SPLASH_SCALE + 8);
                        kam_setVec(e + 0x220, sx, sy, sz); /* setGlobalScale */
                        kam_setVec(e + 0x238, sx, sy, sz);
                    }
                    /* dComIfGp_particle_setSingleRipple */
                    JPABaseEmitter* ripple = dPa_control_set(dComIfGp_getParticle(), 5, 0x3D /* ID_IT_JN_WP_HAMON01 */,
                                                             &current.pos, nullptr, nullptr, 0xFF,
                                                             gabi::at<dPa_levelEcallBack>(0x1047B2E4), -1, nullptr,
                                                             nullptr, nullptr);
                    if (ripple != nullptr) {
                        u32 e = gabi::ea(ripple);
                        f32 sx = gabi::load<f32>(KAM_L_RIPPLE_SCALE);
                        f32 sy = gabi::load<f32>(KAM_L_RIPPLE_SCALE + 4);
                        f32 sz = gabi::load<f32>(KAM_L_RIPPLE_SCALE + 8);
                        kam_setVec(e + 0x220, sx, sy, sz);
                        kam_setVec(e + 0x238, sx, sy, sz);
                    }
                }
            } else {
                mHitFlags = flags & ~8u; /* offWaterHit() */
            }
        }

        setLineBgCheck();
        if (daPy_npc_checkNowPosMove(this, gabi::at<const char>(gabi::load<u32>(KAM_L_STAFF_NAME)))) {
            animationPlay();
        }
    }

    BOOL inEvent = eventProc();
    if (!inEvent) {
        if (dComIfGp_getPlayer(0) == this) {
            actor_status = (actor_status & ~0x3Fu) | 0x34; /* fopAcM_SetStatusMap(this, 0x14), SHOWMAP */
            if (isReturnLink()) {
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
        if (REG0_S(0) == 0) {
            cLib_addCalcAngleS2(&current.angle.x, shape_angle.x, 8, 0x400);
            cLib_addCalcAngleS2(&current.angle.z, shape_angle.z, 8, 0x400);
        } else {
            cLib_addCalcAngleS(&shape_angle.x, current.angle.x, 8, 0x2000, 0x400);
            cLib_addCalcAngleS(&shape_angle.z, current.angle.z, 8, 0x2000, 0x400);
        }
    }

    eventOrder();
    setBaseMtx();

    if (!inEvent && dComIfGp_getPlayer(0) == this) {
        setCollision();
    }

    gabi::call(0x0251641C, &mAtSph); /* ClrCoHit */
    mAtSph.ClrTgHit();
    mCps.ClrAtHit();
    mStts.m_cc_move.z = 0.0f; /* mStts.ClrCcMove() */
    mStts.m_cc_move.x = 0.0f;
    mStts.m_cc_move.y = 0.0f;
    return TRUE;
}
VERIFY(0x0225E598, &daNpc_kam_c::execute);
