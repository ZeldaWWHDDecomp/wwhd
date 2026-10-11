/**
 * d_a_kamome.cpp (WWHD): kamome_auto_move.
 * Automatic movement and actor destruction.
 */
#include "d/actor/d_a_kamome.h"

/* ---- functions of d_a_kamome.cpp (verified there) ---- */
static void anm_init(kamome_class* i_this, int a, float m, unsigned char l, float s, int snd) { kamomeAuto_animation(i_this, a, m, l, s, snd); }
static fopAc_ac_c* search_esa(kamome_class* i_this) { return kamomeAuto_bait(i_this); }
static void kamome_pos_move(kamome_class* i_this) { kamomeAuto_flight(i_this); }
static void kamome_bgcheck(kamome_class* i_this) { kamomeAuto_background(i_this); }
static void kamome_ground_pos_move(kamome_class* i_this) { kamomeAuto_groundMove(i_this); }

/* ---- bindings used here ---- */

/* mDoExt_McaMorf frame control (HD: J3DFrameCtrl at +0x98: rate, +0x9C frame, +0xA7 state) */
static s32 getFrame(mDoExt_McaMorf* m) { return gabi::ftoi(m->getFrame()); }
static bool isStop(mDoExt_McaMorf* m) {
    return m->isStop();
}
static f32 GetGroundH(kamome_class* i_this) { return i_this->mAcch.GetGroundH(); }
static bool ChkWallHit(kamome_class* i_this) { return (i_this->mAcch.m_flags & 0x10) != 0; }

static f32 REG6_F(int i) { return REG_F(6, i); }
static s16 REG6_S(int i) { return REG_S(6, i); }

/* l_kamomeHIO (d_a_kamome.cpp) */


/* esa_class fields used */
static void esa_set_field_0x298(fopAc_ac_c* esa, u8 v) { kamomeBait(esa)->field_0x298 = v; }


// HD: dBgS_GndChk on the stack (0x54 bytes; sub-object vtables after the members)





/* fly towards a point 2000 ahead and 1000 up */
/* inlined into kamome_auto_move */
static void set_fly_away_target(kamome_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    MtxTrans(a_this->current.pos.x, a_this->current.pos.y, a_this->current.pos.z, 0);
    cXyz sp30;
    sp30.set(0.0f, 1000.0f, 2000.0f);
    cMtx_YrotM(calc_mtx(), a_this->current.angle.y);
    MtxPosition(&sp30, &i_this->mTargetPos);
}

/* 02189270 */
static void kamome_auto_move(kamome_class* i_this) {

    fopAc_ac_c* a_this = &i_this->actor;
    dComIfGp_get(); /* fopAc_ac_c* player = dComIfGp_getPlayer(0): unused */
    s8 moveType = 0;
    s32 frame;
    f32 x, y, z;

    switch (i_this->mAnimState) {
    case 0:
        frame = getFrame(i_this->mpMorf);
        if ((i_this->mTimers[0] == 0) && (frame == REG0_S(0) + 9)) {
            i_this->mAnimState = 1;
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT1_e, REG0_F(0) + 12.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KAMOME_BAS_KA_WAIT1_e);
        }
        break;

    case 1:
        if ((i_this->mGlobalTimer & 0x3f) == 0 && cM_rndF(1.0f) < 0.5f) {
            s16 t = (s16)gabi::ftoi(cM_rndF(10000.0f));
            i_this->mAnimState = 2;
            i_this->mGlobalTimer = t;
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_SING1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, 0);
        } else if ((i_this->mTimers[0] == 0) && (a_this->current.pos.y < i_this->mTargetPos.y)) {
            i_this->mAnimState = 0;
            i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT2_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KAMOME_BAS_KA_WAIT2_e);
        }
        break;

    case 2:
        frame = getFrame(i_this->mpMorf);
        if (frame == 8) {
            fopAcM_seStart(a_this, JA_SE_CV_KAMOME, 0);
        }
        if (isStop(i_this->mpMorf)) {
            i_this->mAnimState = 1;
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KAMOME_BAS_KA_WAIT1_e);
        }
        break;

    case 10:
        break;

    case 20:
        if (isStop(i_this->mpMorf)) {
            i_this->mAnimState = 0;
            i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT2_e, 0.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KAMOME_BAS_KA_WAIT2_e);
        }
        break;
    }

    switch (i_this->mMoveState) {
    case 0:
        if (i_this->mTimers[1] == 0) {
            // HD: wander radius 500 (GameCube 1000)
            f32 rx = cM_rndFX(500.0f);
            x = (a_this->home.pos.x + rx) - a_this->current.pos.x;
            f32 rz = cM_rndFX(500.0f);
            z = (a_this->home.pos.z + rz) - a_this->current.pos.z;
            if (std_sqrtf(gabi::fmadds(x, x, z * z)) > 200.0f) {
                i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(150.0f) + 50.0f);
                i_this->mTargetPos.x = x + a_this->current.pos.x;
                // HD: height range 300 (GameCube 500)
                f32 ry = cM_rndF(300.0f);
                i_this->mTargetPos.y = a_this->home.pos.y + ry;
                i_this->mTargetPos.z = z + a_this->current.pos.z;
                i_this->mRotVelFade = 0.0f;

                if (a_this->current.pos.y < i_this->mTargetPos.y) {
                    i_this->mVelocityFwdTarget = REG0_F(10) + 20.0f;
                    i_this->mVelocityFwdTargetMaxVel = REG0_F(11) + 0.2f;
                } else {
                    i_this->mVelocityFwdTarget = REG0_F(12) + 36.0f;
                    i_this->mVelocityFwdTargetMaxVel = REG0_F(13) + 0.5f;
                }

                i_this->mRotVel = cM_rndF(300.0f) + 200.0f;

                if (i_this->mTimers[3] == 0) {
                    fopAc_ac_c* iVar3 = search_esa(i_this);
                    if (iVar3 != nullptr) {
                        i_this->mMoveState = 10;
                        i_this->mEsaProcID = fopAcM_GetID(iVar3);
                        i_this->mRotVel = 1000.0f;
                    }
                }
            }
        }
        break;

    case 10: {
        fopAc_ac_c* pfVar4 = fopAcM_SearchByID(i_this->mEsaProcID);
        if (pfVar4 != nullptr) {
            cXyz sp30;
            sp30.x = 0.0f;
            sp30.y = 0.0f;
            sp30.z = gabi::fmadds(REG6_F(16), 10.0f, -100.0f);
            cMtx_YrotS(calc_mtx(), a_this->current.angle.y);
            cXyz sp24;
            MtxPosition(&sp30, &sp24);
            i_this->mTargetPos.x = pfVar4->current.pos.x + sp24.x;
            // HD: 70 + REG6_F(8) (GameCube 80 + REG6_F(8) * 10)
            i_this->mTargetPos.y = pfVar4->current.pos.y + 70.0f + REG6_F(8);
            i_this->mTargetPos.z = pfVar4->current.pos.z + sp24.z;
            i_this->mVelocityFwdTarget = 20.0f;
            x = i_this->mTargetPos.x - a_this->current.pos.x;
            y = i_this->mTargetPos.y - a_this->current.pos.y;
            z = i_this->mTargetPos.z - a_this->current.pos.z;
            // HD: REG6_F(9) + 100 (GameCube REG6_F(9) * 10 + 300)
            if (std_sqrtf(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y))) < REG6_F(9) + 100.0f) {
                i_this->mMoveState = 0x14;
                anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_LAND1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_KAMOME_BAS_KA_LAND1_e);
                i_this->mAnimState = 10;
                a_this->speed.y = 0.0f;
                s16 r = (s16)gabi::ftoi(cM_rndFX(15000.0f));
                i_this->m2FC = a_this->current.angle.y + r;
                i_this->mTimers[2] = 0x3c;
                moveType = -1;
            }
        } else {
            i_this->mMoveState = 0;
            i_this->mGroundKeepTimer = 0x3c; // HD: keep above ground for 60 steps
        }
        break;
    }

    case 0x14: {
        moveType = -1;
        frame = getFrame(i_this->mpMorf);
        a_this->current.pos.y += a_this->speed.y;
        if (frame > REG6_S(2) + 0xf) {
            a_this->speed.y -= REG6_F(7) + 0.8f;
            cLib_addCalcAngleS2(&a_this->current.angle.y, i_this->m2FC, 2, 1000);
        }
        cLib_addCalcAngleS2(&a_this->current.angle.x, 0, 5, 0x800);
        cLib_addCalcAngleS2(&a_this->current.angle.z, 0, 5, 0x800);
        kamome_bgcheck(i_this);

        // HD: landing on water (sea or a water polygon above the ground) aborts the landing
        bool on_ground;
        if (daSea_ChkArea(a_this->current.pos.x, a_this->current.pos.z)) {
            f32 h = daSea_calcWave(a_this->current.pos.x, a_this->current.pos.z);
            on_ground = GetGroundH(i_this) > h;
        } else {
            cXyz sp1c;
            auto p = Vec3f{a_this->current.pos.x, a_this->current.pos.y + 100.0f, a_this->current.pos.z};
            sp1c = p;
            f32 h = dBgS_GetWaterHeight(&sp1c);
            on_ground = h == -1000000000.0f || GetGroundH(i_this) > h;
        }
        if (!on_ground) {
            i_this->mMoveState = 0;
            i_this->mAnimState = 0;
            i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT2_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KAMOME_BAS_KA_WAIT2_e);
            break;
        }

        if (i_this->mAcch.ChkGroundHit()) {
            a_this->speed.y = -1.0f;
        } else {
            cLib_addCalc2(&a_this->current.pos.x, i_this->mTargetPos.x, 0.1f, std::fabs((f32)a_this->speed.x));
            cLib_addCalc2(&a_this->current.pos.z, i_this->mTargetPos.z, 0.1f, std::fabs((f32)a_this->speed.z));
        }

        if (isStop(i_this->mpMorf)) {
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT3_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, 0);
        }

        if (i_this->mTimers[2] == 0) {
            i_this->mMoveState = 0x16;
        }
        break;
    }

    case 0x15: {
        moveType = 1;
        fopAc_ac_c* pfVar4 = search_esa(i_this);
        if (pfVar4 == nullptr) {
            i_this->mMoveState = 0x19;
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_FLY1_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_KAMOME_BAS_KA_FLY1_e);
        } else {
            i_this->mEsaProcID = fopAcM_GetID(pfVar4);
            i_this->mMoveState = 0x16;
        }
        break;
    }

    case 0x16:
        moveType = 1;
        i_this->mVelocityFwdTarget = 0.0f;
        i_this->actor.speedF = 0.0f;
        i_this->mRotVel = 0.0f;
        if (i_this->mTimers[2] == 0) {
            i_this->mMoveState = 0x17;
            fopAc_ac_c* pfVar4 = fopAcM_SearchByID(i_this->mEsaProcID);
            if (pfVar4 != nullptr) {
                i_this->mTargetPos.x = pfVar4->current.pos.x;
                i_this->mTargetPos.z = pfVar4->current.pos.z;
                anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_MOVE1_e, 3.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KAMOME_BAS_KA_MOVE1_e);
            } else {
                i_this->mMoveState = 0x19;
                anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_FLY1_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_KAMOME_BAS_KA_FLY1_e);
            }
        }
        break;

    case 0x13:
        moveType = 1;
        if (isStop(i_this->mpMorf)) {
            i_this->mMoveState = 0x15;
            i_this->mTimers[2] = (s16)gabi::ftoi(cM_rndF(30.0f) + 20.0f);
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT3_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, 0);
        }
        i_this->mVelocityFwdTarget = 0.0f;
        a_this->speedF = 0.0f;
        break;

    case 0x17:
        moveType = 1;
        i_this->m2AB = 1;
        frame = getFrame(i_this->mpMorf);
        if ((frame >= l_kamomeHIO.m06) && (frame <= l_kamomeHIO.m08)) {
            i_this->mVelocityFwdTarget = i_this->mScale * 5.0f;
            a_this->speedF = i_this->mScale * 5.0f;
            i_this->mRotVel = 4000.0f;
            i_this->mRotVelFade = 1.0f;
        } else {
            i_this->mVelocityFwdTarget = 0.0f;
            a_this->speedF = 0.0f;
            i_this->mRotVel = 0.0f;
            x = i_this->mTargetPos.x - a_this->current.pos.x;
            z = i_this->mTargetPos.z - a_this->current.pos.z;
            if (std_sqrtf(gabi::fmadds(x, x, z * z)) < 85.0f) {
                anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_EAT1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, 0);
                i_this->mMoveState = 0x18;
            }

            if (ChkWallHit(i_this)) {
                i_this->mMoveState = 0x19;
                anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_FLY1_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_KAMOME_BAS_KA_FLY1_e);
            }
        }
        break;

    case 0x18:
        moveType = 1;
        i_this->mVelocityFwdTarget = 0.0f;
        a_this->speedF = 0.0f;
        frame = getFrame(i_this->mpMorf);
        if (frame == 9) {
            fopAc_ac_c* pfVar4 = fopAcM_SearchByID(i_this->mEsaProcID);
            if (pfVar4 != nullptr) {
                fopAcM_delete(pfVar4);
            }
        }

        if (isStop(i_this->mpMorf)) {
            if (cM_rndF(1.0f) < 0.2f) {
                i_this->mMoveState = 0x13;
                anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_SING2_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, 0);
            } else {
                i_this->mMoveState = 0x15;
                i_this->mTimers[2] = (s16)gabi::ftoi(cM_rndF(20.0f));
                anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT3_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, 0);
            }
        }
        break;

    case 0x19:
        moveType = 1;
        a_this->speed.y = 0.0f;
        frame = getFrame(i_this->mpMorf);
        if (frame < 5) {
            i_this->mVelocityFwdTarget = 0.0f;
            i_this->mVelocityFwdTargetMaxVel = 1.0f;
        } else {
            i_this->mMoveState = 0;
            i_this->mAnimState = 20;
            i_this->mTimers[3] = (s16)gabi::ftoi(cM_rndF(100.0f) + 100.0f);
            i_this->mRiseTimer = REG0_S(4) + 10;
            if (i_this->mPathIdx != 0xff) {
                i_this->mbUsePathMovement = i_this->mPathIdx + 1;
                i_this->m308 = 1;
            } else {
                i_this->mTimers[1] = 0x32;
                i_this->mRotVel = gabi::fmadds(REG0_F(4), 10.0f, 1000.0f);
                i_this->mVelocityFwdTarget = 25.0f;
                i_this->mVelocityFwdTargetMaxVel = 2.0f;
                a_this->speedF = 0.0f;
                set_fly_away_target(i_this);
            }
        }
        break;
    }

    switch (moveType) {
    case 0:
        kamome_pos_move(i_this);
        // HD: while mGroundKeepTimer runs, stay at least 50 + REG18_F(3) above the ground
        if (i_this->mGroundKeepTimer != 0) {
            dBgS_GndChk_l gndChk;
            dBgS_GndChk_ct(&gndChk);
            auto p = Vec3f{a_this->current.pos.x, a_this->current.pos.y + 200.0f, a_this->current.pos.z};
            gndChk.m_pos = p;
            f32 gy = cBgS_GroundCross(dComIfG_Bgsp(), &gndChk) + 50.0f + REG_F(18, 3);
            if (!(a_this->current.pos.y > gy)) {
                a_this->current.pos.y = gy;
            }
            dBgS_GndChk_dt(&gndChk);
        }
        break;

    case 1:
        kamome_ground_pos_move(i_this);
        if (i_this->mMoveState != 0x19) {
            if (a_this->current.pos.y - GetGroundH(i_this) > 200.0f ||
                fopAcM_searchActorDistance(a_this, dComIfGp_getPlayer0()) < l_kamomeHIO.m0C) {
                i_this->mMoveState = 0;
                fopAc_ac_c* pfVar5 = fopAcM_SearchByID(i_this->mEsaProcID);
                if (pfVar5 != nullptr) {
                    esa_set_field_0x298(pfVar5, 0);
                }
                i_this->mAnimState = 0;
                i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
                anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT2_e, 3.0f, J3DFrameCtrl::EMode_LOOP, 1.5f, dRes_INDEX_KAMOME_BAS_KA_WAIT2_e);
                if (i_this->mPathIdx != 0xff) {
                    i_this->mbUsePathMovement = i_this->mPathIdx + 1;
                    s16 t = (s16)gabi::ftoi(cM_rndF(250.0f) + 250.0f);
                    i_this->m308 = 1;
                    i_this->mTimers[3] = t;
                } else {
                    i_this->mTimers[1] = 0x32;
                    i_this->mRotVel = gabi::fmadds(REG0_F(4), 10.0f, 5000.0f);
                    i_this->mVelocityFwdTarget = 30.0f;
                    i_this->mVelocityFwdTargetMaxVel = 3.0f;
                    a_this->speedF = 0.0f;
                    set_fly_away_target(i_this);
                    i_this->mTimers[3] = (s16)gabi::ftoi(cM_rndF(250.0f) + 250.0f);
                }
            }
        }
        break;
    }
}

/* ---- leftover functions of the translation unit ---- */

/* kamome_class::~kamome_class (deleting; vtable slot 10012408) */
/* 0218A4A8 */
static void daKamome_c_dt(void* p, s32 flags) {

    if (p != nullptr) {
        kamome_class* actor = static_cast<kamome_class*>(p);
        kamomeAuto_sphereDtor(&actor->mSph, 2); /* dCcD_Sph */
        dCcD_Stts_dt(&actor->mStts,2); /* dCcD_Stts */
        actor->mAcch.dBgS_GrpPassChk::__vtbl = KAMOME_ACCH_VTABLE;
        actor->mAcch.dBgS_PolyPassChk::__vtbl = KAMOME_POLY_VTABLE;
        kamomeAuto_acchDtor(&actor->mAcch, 0); /* line check */
        kamomeAuto_circleDtor(&actor->mAcchCir.m_cir, 2);
        kamomeAuto_actorDtor(&actor->actor, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}

/* sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10012318, after the destructor 0218925C */
/* 0218A544 */
static void kamome_SafeString_assureTermination(void* p) {

}

/* WWHD glue
ext 02185558 void kamomeAuto_animation(kamome_class* actor, int resource, f32 blend, u8 mode, f32 speed, int sound)
ext 02185808 fopAc_ac_c* kamomeAuto_bait(kamome_class* actor)
ext 02185B78 void kamomeAuto_flight(kamome_class* actor)
ext 02185DF0 void kamomeAuto_background(kamome_class* actor)
ext 02185E80 void kamomeAuto_groundMove(kamome_class* actor)
ext 02515AE8 void kamomeAuto_sphereDtor(dCcD_Sph* sphere,s32 flags)
ext 024EFD9C void kamomeAuto_acchDtor(dBgS_ObjAcch* acch,s32 flags)
ext 02018034 void kamomeAuto_circleDtor(void* circle,s32 flags)
ext 025D50BC void kamomeAuto_actorDtor(fopAc_ac_c* actor,s32 flags)
*/
