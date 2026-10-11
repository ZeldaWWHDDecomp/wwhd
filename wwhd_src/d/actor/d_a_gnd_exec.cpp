/**
 * d_a_gnd_exec.cpp (WWHD)
 * Boss - Ganondorf: execution, with the collision update, sword spheres and
 * demo_camera inlined.
 *
 * The GameCube source of this unit is all "Nonmatching"
 * stubs: written from the WWHD code (cking.rpx) and verified against it.
 */
#include "d/actor/d_a_gnd_exec.h"

// HD: data owned by the Ganondorf translation unit; mapped without host copies.
const gc_tu_data gc_tu = {.safeString = 0x1000FFFC};
// HD: the search callback lives in the raw main fragment, whose VERIFY lacks a name mapping.
static gabi::HostFn l_zeldaSearch((const void*)z_s_sub, 0x0215449C);
static gndExecArchiveName l_cameraArchiveName;
static gndExecActorPtr l_zelda;
static gndExecHIO l_execHIO;
static cXyz l_inactiveSwordPos;
static cXyz l_rippleScale;
static gndExecWord l_rippleScaleInitialized;
static gndExecCameraPoints l_cameraCenters;
static gndExecCameraPoints l_cameraEyes;
static f32 l_cameraFovy[5] = {66.75f, 64.25f, 64.25f, 44.25f, 44.25f};
static s32 l_swordJoints[2] = {4, 7};
static s32 l_swordBaseJoints[2] = {27, 45};
static u32 l_swordAttackTypes[3] = {0, 1, 2};
static s32 l_attackEffectJoints[6] = {26, 44, 4, 7, 26, 44};
static gndExecSigned l_demoFrame;
static gndExecWord l_demoObject;
static gndExecDemoName l_demoName;
static GXColor l_fadeColor;

// HD: dispatch through the player virtuals captured by the caller.
static inline void daPy_setPlayerPosAndAngle(u32 vt, fopAc_ac_c* pl, cXyz* pos, s16 ang) {
    gabi::at<gndExecPlayerVtable>(vt)->setPlayerPosAndAngle(pl, pos, ang);
}

static inline bool gnd_isStop(mDoExt_McaMorf* morf) {
    return (morf->mFrameCtrl.mState & 1) || morf->mFrameCtrl.mRate == 0.0f;
}
/* fopAcM_monsSeStart, inline with the actor and eyePos checks */
static inline void gnd_mons_se(gnd_class* a, u32 id) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gndExec_monsSeStart(id, &a->eyePos, pid, 0, reverb);
    }
}
static inline void gnd_se(gnd_class* a, u32 id) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        mDoAud_seStart(id, &a->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
    }
}
static inline void gnd_shock(s32 strength) {
    gabi::Local<cXyz> p;
    u32 vib = gabi::ea(dComIfGp_getVibration());
    s16 reg = REG0_S(2);
    p->x = 0.0f;
    p->y = 1.0f;
    p->z = 0.0f;
    gndExec_startShock(vib, reg + strength, -0x21, p.get());
}

#define REGF(i) REG0_F(i)

/* inlined into daGnd_Execute */
static inline void gnd_weponSphUpdate(gnd_class* i_this, J3DModel* model, fopAc_ac_c* player, int i,
                                      cXyz* local, cXyz* tmp) {
    dCcD_Sph* sph = &i_this->mWeponSph[i];
    s32 jnt = l_swordJoints[i];
    PSMTXCopy(gnd_getAnmMtx(model, jnt), calc_mtx());
    local->y = 0.0f;
    local->z = 0.0f;
    local->x = 0.0f;
    cXyz* center = (&execFields(i_this)->mSwordJointPos[i]);
    MtxPosition(local, center);
    s8 k = execFields(i_this)->mSwordAttack[i];
    if (k == 0 || (gabi::at<gndExecPlayer>(gabi::ea(player))->mAction == 5 &&
                   execFields(i_this)->mBattlePhase < 2)) {
        execFields(i_this)->mSwordActive[i] = 0;
        sph->mSph.SetC(&l_inactiveSwordPos);
        dComIfG_Ccsp_Set(sph);
        return;
    }
    sph->mGObjAt.mHitMark = 0xD;
    k = execFields(i_this)->mSwordAttack[i];
    cXyz* at = (&execFields(i_this)->mSwordPos[i]);
    if (k == 3) {
        MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
        mDoMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
        s16 idx = (s16)l_execHIO.mSpinAttackType;
        f32 x = REGF(14);
        f32 z = REGF(16) + 300.0f;
        execFields(i_this)->mSwordAttackType[i] = l_swordAttackTypes[idx];
        f32 y = REGF(15) + 100.0f;
        local->z = z;
        sph->SetR(execFields(i_this)->mSwordRadius[i]);
        local->x = x;
        local->y = y;
    } else if (k >= 10) {
        MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
        k = execFields(i_this)->mSwordAttack[i];
        local->x = 0.0f;
        f32 r = REGF(17);
        if (k == 10) {
            local->y = REGF(15) + 60.0f;
        } else {
            local->y = REGF(16) + 230.0f;
        }
        local->z = 0.0f;
        execFields(i_this)->mSwordRadius[i] = r + 100.0f;
        u32 f = (u32)sph->mGObjTg.mSPrm;
        if ((u8)l_execHIO.mSwordShield == 0) {
            sph->mGObjTg.mSPrm = f | 1;
            sph->mGObjTg.mSPrm = (u32)sph->mGObjTg.mSPrm & ~4u;
        } else {
            sph->mGObjTg.mSPrm = f & ~1u;
            sph->mGObjTg.mSPrm = (u32)sph->mGObjTg.mSPrm | 4;
        }
        sph->SetR(execFields(i_this)->mSwordRadius[i]);
    } else {
        s32 jnt2 = l_swordBaseJoints[i];
        PSMTXCopy(gnd_getAnmMtx(model, jnt2), calc_mtx());
        s16 idx = (s16)l_execHIO.mSwordAttackType;
        f32 y = REGF(10);
        execFields(i_this)->mSwordAttackType[i] = l_swordAttackTypes[idx];
        f32 x = REGF(9) + 50.0f;
        f32 z = REGF(11);
        k = execFields(i_this)->mSwordAttack[i];
        local->x = x;
        local->y = y;
        local->z = z;
        if (k == 4)
            sph->mGObjAt.mHitMark = 0xF;
        sph->SetR(execFields(i_this)->mSwordRadius[i]);
    }
    sph->mGObjAt.mSpl = (u8)execFields(i_this)->mSwordAttackType[i];
    MtxPosition(local, at);
    sph->SetC(at);
    mDoMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
    local->x = 0.0f;
    local->y = 0.0f;
    local->z = 1.0f;
    MtxPosition(local, tmp);
    sph->mGObjAt.mVec.copy(*tmp);
    k = execFields(i_this)->mSwordAttack[i];
    u32 at_sprm;
    if (k >= 10) {
        sph->mObjAt.mSPrm = (u32)sph->mObjAt.mSPrm & ~1u;
    } else {
        f32 d = player->current.pos.y - i_this->mAcch.m_ground_h;
        at_sprm = (u32)sph->mObjAt.mSPrm;
        if (d < 50.0f) { /* bge: taken on NaN */
            sph->mObjAt.mSPrm = at_sprm | 1;
        } else {
            sph->mObjAt.mSPrm = at_sprm & ~1u;
        }
    }
    execFields(i_this)->mSwordAttack[i] = 0;
    execFields(i_this)->mSwordRadius[i] = REGF(13) + 150.0f;
    dComIfG_Ccsp_Set(sph);
}

// Finishing-blow camera waypoints.
#define DC_CENTER(n) (&l_cameraCenters.mPoints[n])
#define DC_EYE(n) (&l_cameraEyes.mPoints[n])
#define DC_FOVY(n) l_cameraFovy[n]

// HD: the GameCube demo_camera implementation is unavailable; preserve the HD cuts.
static inline void gnd_demo_order(gnd_class* i_this, u16 type) {
    gnd_orderPotentialEvent(i_this, type, 0xFFFF, 0);
    i_this->eventInfo.mCondition = (u16)(i_this->eventInfo.mCondition | dEvtCnd_UNK2_e);
}
static inline bool gnd_cmdDemo(gnd_class* i_this) {
    return i_this->eventInfo.mCommand == dEvtCmd_INDEMO_e;
}
static inline void gnd_camStart(gnd_class* i_this, u32 camc, s16 m) {
    execFields(i_this)->mCameraCut = m + 1;
    gnd_cam_Stop(camc);
    gnd_cam_SetTrimSize(camc, 2);
}
static inline fopAc_ac_c* gnd_zelda() { return l_zelda.get(); }

/* inlined into daGnd_Execute */
static inline void demo_camera(gnd_class* i_this) {
    fopAc_ac_c* pl = dComIfGp_getPlayer(0);
    s8 camIdx = gabi::at<gndExecPlayView>(dComIfGp_ea())->mCameraId;
    u32 play = dComIfGp_ea();
    fopAc_ac_c* zelda = gnd_zelda();
    fopAc_ac_c* zelda0 = zelda;
    s16 m = execFields(i_this)->mCameraCut;
    u32 cam = gabi::ea(gabi::at<gndExecPlayView>(play + camIdx * 0x34)->mpCamera.get());
    u32 camc = gabi::ea(&gabi::at<gndExecCamera>(cam)->mController);
    gabi::Local<cXyz> zero;
    zero->x = 0.0f;
    zero->z = 0.0f;
    zero->y = 0.0f;
    f32 fovAdd = 0.0f;
    gabi::Local<cXyz> l38;
    gabi::Local<cXyz> l50;
    gabi::Local<cXyz> lsum;
    s16 t;
    s8 n;

    // HD: some cuts test the cached cut, others reload it after guest calls.
    enum class CameraUpdate { Skip, ReloadCut, CachedCut, Force };
    CameraUpdate update = [&]() -> CameraUpdate {
        switch ((u32)(s32)m) {
        case CAMERA_INTRO_START:
            if (!gnd_cmdDemo(i_this)) {
                gnd_demo_order(i_this, 2);
                return CameraUpdate::Skip;
            }
            gnd_camStart(i_this, camc, m);
            execFields(i_this)->mCameraFovy = 55.0f;
            execFields(i_this)->mCameraBlend = 0.0f;
            execFields(i_this)->mCameraTimer = 0;
            gabi::at<gndExecPlayer>(gabi::ea(pl))->mDemoTimer = 0;
            gabi::at<gndExecPlayer>(gabi::ea(pl))->mDemoMode = 3;
            execFields(i_this)->mCameraShake = 0.0f;
            execFields(i_this)->mCameraFovy = REGF(6) + 55.0f;
            execFields(i_this)->mCameraEyeStep.x = REGF(0) + 400.0f;
            execFields(i_this)->mCameraEyeStep.y = REGF(1) + 100.0f;
            execFields(i_this)->mCameraEyeStep.z = REGF(2) - 200.0f;
            execFields(i_this)->mCameraCenterStep.x = REGF(3);
            execFields(i_this)->mCameraCenterStep.y = REGF(4) + 200.0f;
            execFields(i_this)->mCameraCenterStep.z = REGF(5);
            zelda = gnd_zelda();
            /* fall through */
        case CAMERA_INTRO_ORBIT: {
            f32 dx = gabi::fsubs_ppc(i_this->current.pos.x, zelda->current.pos.x);
            f32 dz = gabi::fsubs_ppc(i_this->current.pos.z, zelda->current.pos.z);
            s16 ang = cM_atan2s(dx, dz);
            s16 v = (s16)(ang + 0x4000 + REG0_S(0));
            i_this->shape_angle.y = v;
            i_this->current.angle.y = v;
            if (execFields(i_this)->mCameraTimer > REG0_S(1) + 0x19) {
                execFields(i_this)->mDemoWaitTimer = 0x82;
                cLib_addCalc2(&execFields(i_this)->mCameraEyeStep.x, REGF(6) + 130.0f, 0.6f,
                              gabi::fmuls_ppc(270.0f, execFields(i_this)->mCameraBlend));
                cLib_addCalc2(&execFields(i_this)->mCameraEyeStep.y, REGF(7) + 230.0f, 0.6f,
                              gabi::fmuls_ppc(80.0f, execFields(i_this)->mCameraBlend));
                cLib_addCalc2(&execFields(i_this)->mCameraEyeStep.z, REGF(8), 0.6f,
                              gabi::fmuls_ppc(100.0f, execFields(i_this)->mCameraBlend));
                cLib_addCalc2(&execFields(i_this)->mCameraCenterStep.y, REGF(9) + 280.0f, 0.6f,
                              gabi::fmuls_ppc(80.0f, execFields(i_this)->mCameraBlend));
                cLib_addCalc2(&execFields(i_this)->mCameraBlend, 1.0f, 1.0f,
                              gabi::fmuls_ppc(0.02f, REGF(7) + 1.0f));
            } else {
                execFields(i_this)->mCameraEyeStep.x = REGF(0) + 400.0f;
                execFields(i_this)->mCameraEyeStep.y = REGF(1) + 150.0f;
                execFields(i_this)->mCameraEyeStep.z = REGF(2);
                execFields(i_this)->mCameraCenterStep.x = REGF(3);
                execFields(i_this)->mCameraCenterStep.y = REGF(4) + 200.0f;
                execFields(i_this)->mCameraCenterStep.z = REGF(5);
            }
            MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
            mDoMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
            MtxPosition((&execFields(i_this)->mCameraEyeStep), (&execFields(i_this)->mCameraEye));
            MtxPosition((&execFields(i_this)->mCameraCenterStep),
                        (&execFields(i_this)->mCameraCenter));
            if (execFields(i_this)->mCameraTimer > REG0_S(2) + 0x3C) {
                s16 nm = execFields(i_this)->mCameraCut + 1;
                execFields(i_this)->mCameraCut = nm;
                execFields(i_this)->mCameraTimer = 0;
                execFields(i_this)->mDemoWaitTimer = 0x32;
                m = nm;
                return CameraUpdate::CachedCut;
            }
            return CameraUpdate::ReloadCut;
        }
        case CAMERA_INTRO_ZELDA: {
            if (execFields(i_this)->mCameraTimer == 10) {
                execFields(i_this)->mMode = execFields(i_this)->mMode + 1;
                zelda = gnd_zelda();
            }
            MtxTrans(zelda->current.pos.x, zelda->current.pos.y, zelda->current.pos.z, 0);
            mDoMtx_YrotM(calc_mtx(), gnd_zelda()->shape_angle.y);
            execFields(i_this)->mCameraEyeStep.x = REGF(10) + 100.0f;
            execFields(i_this)->mCameraEyeStep.y = REGF(11) + 50.0f;
            execFields(i_this)->mCameraEyeStep.z = REGF(12) - 300.0f;
            MtxPosition((&execFields(i_this)->mCameraEyeStep), (&execFields(i_this)->mCameraEye));
            execFields(i_this)->mCameraCenter.x = i_this->current.pos.x;
            f32 py = i_this->current.pos.y;
            f32 pz = i_this->current.pos.z;
            execFields(i_this)->mCameraCenter.y = py;
            execFields(i_this)->mCameraCenter.z = pz;
            execFields(i_this)->mCameraCenter.y = gabi::fadds_ppc(py, REGF(13) + 150.0f);
            u32 vt = pl->__vtbl;
            s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
            daPy_setPlayerPosAndAngle(vt, pl, &pl->current.pos, (s16)(ang + 0x8000));
            m = execFields(i_this)->mCameraCut;
            return CameraUpdate::CachedCut;
        }
        case CAMERA_INTRO_LOOK_UP:
            cLib_addCalc2(&execFields(i_this)->mCameraCenter.y, 100.0f, 0.8f, 10.0f);
            if (execFields(i_this)->mCameraTimer < 0xF)
                return CameraUpdate::ReloadCut;
            execFields(i_this)->mCameraCut = 5;
            execFields(i_this)->mCameraTimer = 0;
            execFields(i_this)->mCameraFovy = 75.0f;
            /* fall through */
        case CAMERA_INTRO_LINK: {
            mDoMtx_YrotS(calc_mtx(), pl->shape_angle.y);
            l38->x = REGF(4) + -50.0f;
            l38->y = REGF(5) + 50.0f;
            l38->z = REGF(6) + 100.0f;
            MtxPosition(l38, l50);
            cXyz_pl(&pl->current.pos, lsum, l50);
            (&execFields(i_this)->mCameraEye)->copy(*lsum);
            execFields(i_this)->mCameraCenter.x = pl->current.pos.x;
            f32 py = pl->current.pos.y;
            execFields(i_this)->mCameraCenter.y = py;
            execFields(i_this)->mCameraCenter.z = pl->current.pos.z;
            execFields(i_this)->mCameraCenter.y = gabi::fadds_ppc(py, REGF(7) + 90.0f);
            cLib_addCalc2(&execFields(i_this)->mCameraFovy, 55.0f, 0.8f, 10.0f);
            execFields(i_this)->mDemoWaitTimer = 0x32;
            t = execFields(i_this)->mCameraTimer;
            if ((u8)l_execHIO.mLongIntro == 0) {
                if (t == 5) {
                    u32 vt = pl->__vtbl;
                    gabi::at<gndExecPlayer>(gabi::ea(pl))->mDemoAction = 0x31;
                    gabi::at<gndExecPlayerVtable>(vt)->voiceStart(pl);
                    t = execFields(i_this)->mCameraTimer;
                }
                if (t != 0x19)
                    return CameraUpdate::ReloadCut;
            } else {
                if (t == 5) {
                    gabi::at<gndExecPlayer>(gabi::ea(pl))->mDemoAction = 0x32;
                    t = execFields(i_this)->mCameraTimer;
                }
                if (t == 0x3C) {
                    gabi::at<gndExecPlayer>(gabi::ea(pl))->mDemoAction = 0x1D;
                    t = execFields(i_this)->mCameraTimer;
                }
                if (t != 0x96)
                    return CameraUpdate::ReloadCut;
            }
            {
                J3DModel* model = i_this->mpMorf->getModel();
                void* res = dComIfG_getObjectRes(l_cameraArchiveName.mName, 0x63, gc_tu.safeString);
                gnd_btkAnm_init(i_this->mpBtk, J3DModel_getModelData(model), res, 1, 0, 1.0f, 0, -1,
                                true, 0);
            }
            {
                s16 a = i_this->shape_angle.y;
                execFields(i_this)->mAction = 0;
                i_this->current.angle.y = a;
                execFields(i_this)->mMode = 0;
                execFields(i_this)->mTimers[1] = (s16)l_execHIO.mAttackWait;
                execFields(i_this)->mBattlePhase = 1;
                execFields(i_this)->mCameraCut = 0x96;
                m = 0x96;
            }
            return CameraUpdate::CachedCut;
        }
        case CAMERA_ZELDA_START:
            if (execFields(i_this)->mAction != 0)
                return CameraUpdate::Skip;
            if (!checkGround(i_this, 0.0f))
                return CameraUpdate::Skip;
            if (!gnd_cmdDemo(i_this)) {
                gnd_demo_order(i_this, 2);
                return CameraUpdate::Skip;
            }
            gnd_camStart(i_this, camc, m);
            execFields(i_this)->mCameraFovy = 55.0f;
            execFields(i_this)->mCameraTimer = 0;
            execFields(i_this)->mCameraBlend = 0.0f;
            gabi::at<gndExecPlayer>(gabi::ea(pl))->mDemoMode = 3;
            gabi::at<gndExecPlayer>(gabi::ea(pl))->mDemoTimer = 0;
            mDoMtx_YrotS(calc_mtx(), gnd_zelda()->shape_angle.y);
            l38->x = REGF(4) + 150.0f;
            l38->y = REGF(5) + 50.0f;
            l38->z = REGF(6) + 150.0f;
            MtxPosition(l38, l50);
            cXyz_pl(&gnd_zelda()->current.pos, lsum, l50);
            (&execFields(i_this)->mCameraEye)->copy(*lsum);
            (&execFields(i_this)->mCameraCenter)->copy(gnd_zelda()->eyePos);
            execFields(i_this)->mCameraCenter.y =
                gabi::fadds_ppc(execFields(i_this)->mCameraCenter.y, REGF(7) - 30.0f);
            gabi::at<gndExecZelda>(gabi::ea(zelda0))->mDemoAction = 2;
            execFields(i_this)->mAction = 0x14;
            execFields(i_this)->mCameraShake = 0.0f;
            zelda = gnd_zelda();
            /* fall through */
        case CAMERA_ZELDA_FOLLOW: {
            cLib_addCalc2(&execFields(i_this)->mCameraCenter.x, zelda->eyePos.x, 0.1f, 20.0f);
            f32 y = gabi::fsubs_ppc(gabi::fadds_ppc(gnd_zelda()->eyePos.y, REGF(7)), 30.0f);
            if (y < 40.0f)
                y = 40.0f;
            cLib_addCalc2(&execFields(i_this)->mCameraCenter.y, y, 0.1f, 20.0f);
            cLib_addCalc2(&execFields(i_this)->mCameraCenter.z, gnd_zelda()->eyePos.z, 0.1f, 20.0f);
            if (execFields(i_this)->mCameraTimer >= REG0_S(0) + 100) {
                s16 a = i_this->shape_angle.y;
                execFields(i_this)->mAction = 0;
                execFields(i_this)->mMode = 0;
                i_this->current.angle.y = a;
                execFields(i_this)->mTimers[1] = (s16)l_execHIO.mAttackWait;
                execFields(i_this)->mCameraCut = 0x96;
                gabi::at<gndExecZelda>(gabi::ea(zelda0))->mResumeBattle = 1;
                execFields(i_this)->mBattlePhase = 2;
                m = execFields(i_this)->mCameraCut;
                return CameraUpdate::CachedCut;
            }
            return CameraUpdate::ReloadCut;
        }
        case CAMERA_HIT_START:
            if (!gnd_cmdDemo(i_this)) {
                gnd_demo_order(i_this, 1);
                return CameraUpdate::Skip;
            }
            gnd_camStart(i_this, camc, m);
            execFields(i_this)->mCameraFovy = 55.0f;
            execFields(i_this)->mCameraBlend = 0.0f;
            execFields(i_this)->mCameraTimer = 0;
            gabi::at<gndExecPlayer>(gabi::ea(pl))->mDemoTimer = 0;
            gabi::at<gndExecPlayer>(gabi::ea(pl))->mDemoMode = 3;
            gnd_shock(8);
            execFields(i_this)->mDemoWaitTimer = 0x64;
            execFields(i_this)->mCameraShake = REGF(18) + 5.0f;
            /* fall through */
        case CAMERA_HIT_LINK: {
            execFields(i_this)->mCameraCenter.x = pl->current.pos.x;
            f32 py = pl->current.pos.y;
            execFields(i_this)->mCameraCenter.y = py;
            execFields(i_this)->mCameraCenter.z = pl->current.pos.z;
            execFields(i_this)->mCameraCenter.y = gabi::fadds_ppc(py, REG_F(8, 10) + 50.0f);
            mDoMtx_YrotS(calc_mtx(), pl->shape_angle.y);
            l38->x = REG_F(8, 1) + 200.0f;
            l38->y = REG_F(8, 2) + 30.0f;
            l38->z = REG_F(8, 3) + 300.0f;
            MtxPosition(l38, l50);
            cXyz_pl(l50, lsum, &pl->current.pos);
            (&execFields(i_this)->mCameraEye)->copy(*lsum);
            if (execFields(i_this)->mCameraTimer > (s16)(REG_S(12, 0) + 0xF)) {
                execFields(i_this)->mCameraCut = execFields(i_this)->mCameraCut + 1;
                execFields(i_this)->mCameraTimer = 0;
            } else {
                fovAdd = REG_F(8, 13);
                m = execFields(i_this)->mCameraCut;
                return CameraUpdate::CachedCut;
            }
        }
            /* fall through */
        case CAMERA_HIT_GANONDORF:
            cLib_addCalc2(&execFields(i_this)->mCameraCenter.x, i_this->current.pos.x, 0.5f,
                          REG_F(8, 6) + 70.0f);
            cLib_addCalc2(&execFields(i_this)->mCameraCenter.y,
                          execFields(i_this)->mHeadPos.y + REG_F(8, 4), 0.1f, 50.0f);
            cLib_addCalc2(&execFields(i_this)->mCameraCenter.z, i_this->current.pos.z + REG_F(8, 5),
                          0.5f, REG_F(8, 6) + 70.0f);
            mDoMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
            l38->x = REG_F(8, 7) + -300.0f;
            l38->y = REG_F(8, 8) + 100.0f;
            l38->z = REG_F(8, 9) + 500.0f;
            MtxPosition(l38, l50);
            cXyz_pl(l50, lsum, &i_this->current.pos);
            (&execFields(i_this)->mCameraEye)->copy(*lsum);
            if (execFields(i_this)->mCameraTimer > 0x1E) {
                s16 e = execFields(i_this)->mAction;
                execFields(i_this)->mCameraTimer = 0;
                if (e != 0xB) {
                    s16 a = i_this->shape_angle.y;
                    execFields(i_this)->mAction = 0;
                    execFields(i_this)->mMode = 0;
                    i_this->current.angle.y = a;
                    execFields(i_this)->mTimers[1] = (s16)l_execHIO.mAttackWait;
                }
                execFields(i_this)->mCameraCut = 0x96;
                return CameraUpdate::Force;
            }
            return CameraUpdate::ReloadCut;
        case CAMERA_FINISH_START:
            if (!gnd_cmdDemo(i_this)) {
                gnd_demo_order(i_this, 2);
                return CameraUpdate::Skip;
            }
            gnd_camStart(i_this, camc, m);
            /* fall through */
        case CAMERA_FINISH_SETUP: {
            execFields(i_this)->mCameraFovy = 55.0f;
            execFields(i_this)->mCameraTimer = 0;
            execFields(i_this)->mCameraBlend = 0.0f;
            gabi::at<gndExecPlayer>(gabi::ea(pl))->mDemoMode = 3;
            gabi::at<gndExecPlayer>(gabi::ea(pl))->mDemoTimer = 0;
            i_this->mCyl.mObjCo.mSPrm = i_this->mCyl.mObjCo.mSPrm & ~1u; /* mCyl: Co off */
            anm_init(i_this, 0x35, 1.0f, 0, 1.0f, -1);
            gabi::at<gndExecPlayer>(gabi::ea(pl))->mDemoAction = 0x46;
            gabi::at<gndExecZelda>(gabi::ea(zelda0))->mFinishDemo = 1;
            execFields(i_this)->mCameraCut = execFields(i_this)->mCameraCut + 1;
            daPy_setPlayerPosAndAngle(pl->__vtbl, pl, zero, 0);
            i_this->current.pos.copy(*zero);
            i_this->shape_angle.y = 0;
            i_this->current.angle.y = 0;
            gnd_zelda()->current.pos.copy(*zero);
            fopAc_ac_c* z = gnd_zelda();
            z->current.pos.x = gabi::fadds_ppc(z->current.pos.x, 400.0f);
            execFields(i_this)->mFinishEmitters[0] =
                gabi::ea(dComIfGp_particle_set(0x8384, &i_this->current.pos));
            execFields(i_this)->mFinishEmitters[1] =
                gabi::ea(dComIfGp_particle_set(0x8385, &i_this->current.pos));
            execFields(i_this)->mCameraWaypoint = 0;
        }
            /* fall through */
        case CAMERA_FINISH_FOLLOW: {
            for (int i = 0; i < 2; i++) {
                u32 e = execFields(i_this)->mFinishEmitters[i];
                if (e == 0)
                    continue;
                mDoExt_McaMorf* morf = i_this->mpMorf;
                if (gnd_isStop(morf)) {
                    gnd_becomeInvalidEmitter(e);
                    execFields(i_this)->mFinishEmitters[i] = 0;
                } else {
                    gnd_JPASetRMtxTVecfromMtx(gnd_getAnmMtx(morf->getModel(), 0x30), e + 0x1F0,
                                              e + 0x22C);
                }
            }
            t = execFields(i_this)->mCameraTimer;
            bool soundActorValid = true;
            if (t == 0x22) {
                soundActorValid = i_this != nullptr && gabi::ea(&i_this->eyePos) != 0;
                if (soundActorValid) {
                    gnd_mons_se(i_this, 0x4957);
                    t = execFields(i_this)->mCameraTimer;
                }
            }
            if (soundActorValid && t == 0x4A) {
                soundActorValid = i_this != nullptr && gabi::ea(&i_this->eyePos) != 0;
                if (soundActorValid) {
                    gnd_mons_se(i_this, 0x4958);
                    t = execFields(i_this)->mCameraTimer;
                }
            }
            if (soundActorValid && t == 0x1E) {
                gnd_se(i_this, 0x2806);
                gnd_shock(4);
                t = execFields(i_this)->mCameraTimer;
            }
            bool advanceWaypoint = soundActorValid && t == 0x13;
            if (!advanceWaypoint) {
                advanceWaypoint = (s32)t == REG0_S(4) + 0x27 || (s32)t == REG0_S(5) + 0x43 ||
                                  (s32)t == REG0_S(6) + 0x45;
            }
            if (advanceWaypoint) {
                n = (s8)(execFields(i_this)->mCameraWaypoint + 1);
                execFields(i_this)->mCameraWaypoint = n;
                if (n == 2 || n == 4) {
                    execFields(i_this)->mCameraCenterStep.x =
                        fabsf(gabi::fsubs_ppc(DC_CENTER(n)->x, DC_CENTER(n - 1)->x));
                    n = execFields(i_this)->mCameraWaypoint;
                    execFields(i_this)->mCameraCenterStep.y =
                        fabsf(gabi::fsubs_ppc(DC_CENTER(n)->y, DC_CENTER(n - 1)->y));
                    execFields(i_this)->mCameraCenterStep.z =
                        fabsf(gabi::fsubs_ppc(DC_CENTER(n)->z, DC_CENTER(n - 1)->z));
                    execFields(i_this)->mCameraEyeStep.x =
                        fabsf(gabi::fsubs_ppc(DC_EYE(n)->x, DC_EYE(n - 1)->x));
                    execFields(i_this)->mCameraEyeStep.y =
                        fabsf(gabi::fsubs_ppc(DC_EYE(n)->y, DC_EYE(n - 1)->y));
                    execFields(i_this)->mCameraEyeStep.z =
                        fabsf(gabi::fsubs_ppc(DC_EYE(n)->z, DC_EYE(n - 1)->z));
                    execFields(i_this)->mCameraFovyStep =
                        fabsf(gabi::fsubs_ppc(DC_FOVY(n), DC_FOVY(n - 1)));
                }
            } else {
                n = execFields(i_this)->mCameraWaypoint;
            }
            if (n == 0 || n == 1 || n == 3) {
                (&execFields(i_this)->mCameraCenter)->copy(*DC_CENTER(n));
                n = execFields(i_this)->mCameraWaypoint;
                (&execFields(i_this)->mCameraEye)->copy(*DC_EYE(n));
                execFields(i_this)->mCameraFovy = DC_FOVY(n);
            } else {
                n = execFields(i_this)->mCameraWaypoint;
                f32 r = REGF(0) + 0.1f;
                execFields(i_this)->mCameraBlend = r;
                f32 step = gabi::fmuls_ppc(execFields(i_this)->mCameraCenterStep.x, r);
                f32 k = REG0_F(1) + 0.4f;
                cLib_addCalc2(&execFields(i_this)->mCameraCenter.x, DC_CENTER(n)->x, k, step);
                n = execFields(i_this)->mCameraWaypoint;
                cLib_addCalc2(&execFields(i_this)->mCameraCenter.y, DC_CENTER(n)->y, k,
                              gabi::fmuls_ppc(execFields(i_this)->mCameraCenterStep.y,
                                              execFields(i_this)->mCameraBlend));
                n = execFields(i_this)->mCameraWaypoint;
                cLib_addCalc2(&execFields(i_this)->mCameraCenter.z, DC_CENTER(n)->z, k,
                              gabi::fmuls_ppc(execFields(i_this)->mCameraCenterStep.z,
                                              execFields(i_this)->mCameraBlend));
                n = execFields(i_this)->mCameraWaypoint;
                cLib_addCalc2(&execFields(i_this)->mCameraEye.x, DC_EYE(n)->x, k,
                              gabi::fmuls_ppc(execFields(i_this)->mCameraEyeStep.x,
                                              execFields(i_this)->mCameraBlend));
                n = execFields(i_this)->mCameraWaypoint;
                cLib_addCalc2(&execFields(i_this)->mCameraEye.y, DC_EYE(n)->y, k,
                              gabi::fmuls_ppc(execFields(i_this)->mCameraEyeStep.y,
                                              execFields(i_this)->mCameraBlend));
                n = execFields(i_this)->mCameraWaypoint;
                cLib_addCalc2(&execFields(i_this)->mCameraEye.z, DC_EYE(n)->z, k,
                              gabi::fmuls_ppc(execFields(i_this)->mCameraEyeStep.z,
                                              execFields(i_this)->mCameraBlend));
                n = execFields(i_this)->mCameraWaypoint;
                cLib_addCalc2(&execFields(i_this)->mCameraFovy, DC_FOVY(n), k,
                              gabi::fmuls_ppc(execFields(i_this)->mCameraFovyStep,
                                              execFields(i_this)->mCameraBlend));
            }
            t = execFields(i_this)->mCameraTimer;
            if (t == 0x4A) {
                gnd_se(i_this, 0x595D);
                gnd_bgmStop(10);
                gnd_shock(8);
                t = execFields(i_this)->mCameraTimer;
                if (t == 0x4A) {
                    gnd_fadeOut((s8)(REG0_S(8) + 0x32));
                    gnd_setFadeColor(gabi::ea(&l_fadeColor));
                    t = execFields(i_this)->mCameraTimer;
                }
            }
            if (t == 0x7C) {
                gnd_setNextStage("GTower", 2, 0, 9, 0.0f, 0, 1, 7);
                m = execFields(i_this)->mCameraCut;
                return CameraUpdate::CachedCut;
            }
            return CameraUpdate::ReloadCut;
        }
        case CAMERA_END:
            gnd_cam_SetTrimSize(camc, 0);
            gnd_cam_Start(camc);
            gnd_play5C22_on();
            dComIfGp_event_reset();
            execFields(i_this)->mCameraCut = 0;
            gabi::at<gndExecPlayer>(gabi::ea(pl))->mDemoMode = 2;
            gabi::at<gndExecPlayer>(gabi::ea(pl))->mDemoAction = 1;
            execFields(i_this)->mDemoWaitTimer = 1;
            return CameraUpdate::ReloadCut;
        default:
            return CameraUpdate::CachedCut;
        }
    }();
    if (update == CameraUpdate::Skip)
        return;
    if (update == CameraUpdate::ReloadCut)
        m = execFields(i_this)->mCameraCut;
    if (update != CameraUpdate::Force && m == 0)
        return;
    {
        s16 tt = execFields(i_this)->mCameraTimer;
        f32 amp = execFields(i_this)->mCameraShake;
        f32 s1 = gabi::fmuls_ppc(cM_ssin(tt * 0x3500), amp);
        f32 c2 = gabi::fmuls_ppc(cM_scos(tt * 0x3900), amp);
        f32 c3 = gabi::fmuls_ppc(cM_scos((s16)execFields(i_this)->mCounter * 0x1C00), amp);
        gabi::Local<cXyz> center;
        gabi::Local<cXyz> eye;
        center->x = gabi::fadds_ppc(execFields(i_this)->mCameraCenter.x, s1);
        center->y = gabi::fadds_ppc(execFields(i_this)->mCameraCenter.y, c2);
        center->z = execFields(i_this)->mCameraCenter.z;
        eye->x = gabi::fadds_ppc(execFields(i_this)->mCameraEye.x, s1);
        eye->y = gabi::fadds_ppc(execFields(i_this)->mCameraEye.y, c2);
        eye->z = execFields(i_this)->mCameraEye.z;
        s16 roll = (s16)gabi::ftoi(gabi::fmadds(c3, 7.5f, fovAdd));
        gnd_cam_Set(camc, center, eye, roll, execFields(i_this)->mCameraFovy);
        cLib_addCalc0(&execFields(i_this)->mCameraShake, 1.0f, REGF(16) + 1.0f);
        gndExec_reportCamera(0x19A, 0x1AE, "K SUB  COUNT  %d",
                             (s32)execFields(i_this)->mCameraTimer);
        execFields(i_this)->mCameraTimer = execFields(i_this)->mCameraTimer + 1;
    }
}

// HD: preserve the independently pooled archive-name copy used by the inlined camera.
/* 02154EE0 */
BOOL daGnd_Execute(gnd_class* i_this) {
    fopAc_ac_c* player0 = dComIfGp_getPlayer(0);
    if (execFields(i_this)->mBgmParam == 0)
        execFields(i_this)->mAction = 0x14;

    if (i_this->demoActorID != 0) {
        /* demo */
        execFields(i_this)->mDemoActive = 1;
        bool skip = false;
        if ((s32)l_demoFrame == 0x6A5) {
            gabi::Local<SafeString> a;
            gabi::Local<SafeString> b;
            a->__vtbl = gc_tu.safeString;
            b->mStringTop = gabi::guest_addr(l_demoName.mName);
            a->mStringTop = gabi::guest_addr("Demo42");
            b->__vtbl = gc_tu.safeString;
            gndExec_assureTermination(a.get());
            gabi::at<gndExecStringVtable>(a->__vtbl)->assureTermination(a.get());
            u32 s1 = a->mStringTop;
            gabi::at<gndExecStringVtable>(b->__vtbl)->assureTermination(b.get());
            if (s1 == b->mStringTop) {
                skip = true;
            } else {
                u32 p = a->mStringTop, q = b->mStringTop;
                for (u32 k = 0; k < 0x40001; k++) {
                    u8 c = gabi::load<u8>(p + k);
                    if (c != gabi::load<u8>(q + k))
                        break;
                    if (c == 0) {
                        skip = true;
                        break;
                    }
                }
            }
        }
        if (!skip) {
            u8 id = i_this->demoActorID;
            void* act = nullptr;
            if (id != 0 && id <= 0x20) {
                u32 obj = (u32)l_demoObject;
                if (obj == 0) {
                    JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
                    obj = (u32)l_demoObject;
                }
                act = gnd_dDemo_getActor(obj, id);
            }
            if (act != nullptr) {
                void* btp = gnd_dDemo_getP_BtpData(act, "Gnd");
                if (btp != nullptr) {
                    J3DModel* m = i_this->mpMorf->getModel();
                    gnd_btpAnm_init(i_this->mpBtp, J3DModel_getModelData(m), btp, 1, 0, 1.0f, 0, -1,
                                    true, 0);
                }
                void* btk = gnd_dDemo_getP_BtkData(act, "Gnd");
                if (btk != nullptr) {
                    J3DModel* m = i_this->mpMorf->getModel();
                    gnd_btkAnm_init(i_this->mpBtk, J3DModel_getModelData(m), btk, 1, 0, 1.0f, 0, -1,
                                    true, 0);
                }
                void* brk = gnd_dDemo_getP_BrkData(act, "Gnd");
                if (brk != nullptr) {
                    J3DModel* m = i_this->mpMorf->getModel();
                    gnd_brkAnm_init(i_this->mpBrk, J3DModel_getModelData(m), brk, 1, 2, 1.0f, 0, -1,
                                    true, 0);
                }
            }
            gnd_dDemo_setDemoData(i_this, 0x6A, i_this->mpMorf, "Gnd", 0, nullptr, 0, 0);
        }
        if (execFields(i_this)->mDemoActive != 0) {
            J3DModel* model = i_this->mpMorf->getModel();
            f32 sz = i_this->scale.z;
            f32 sx = i_this->scale.x;
            f32 sy = i_this->scale.y;
            model->mBaseScale.x = sx;
            model->mBaseScale.y = sy;
            model->mBaseScale.z = sz;
            mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y,
                                   i_this->current.pos.z);
            mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x, i_this->shape_angle.y,
                           i_this->shape_angle.z);
            J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
            i_this->mpMorf->calc();
            mDoExt_baseAnm_play(i_this->mpBrk);
            mDoExt_baseAnm_play(i_this->mpBtk);
            mDoExt_baseAnm_play(i_this->mpBtp);
            ke_move(i_this);
            return TRUE;
        }
    } else if (execFields(i_this)->mDemoActive != 0) {
        execFields(i_this)->mDemoActive = 0;
    }

    /* the water surface (attribute 0x13): ripples */
    if (checkGround(i_this, 0.0f)) {
        if ((i_this->mAcch.m_flags & 0x20) &&
            gnd_GetAttributeCode(dComIfG_Bgsp(),
                                 static_cast<cBgS_PolyInfo*>(&i_this->mAcch.m_gnd)) == 0x13 &&
            execFields(i_this)->mRippleEmitter == 0) {
            // HD: keep the guarded ripple scale initialization and its store order.
            if ((u32)l_rippleScaleInitialized == 0) {
                l_rippleScaleInitialized = 1;
                l_rippleScale.x = 1.0f;
                l_rippleScale.y = 1.0f;
                l_rippleScale.z = 1.0f;
            }
            dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &i_this->current.pos, nullptr,
                            &l_rippleScale, 0xFF, (dPa_levelEcallBack*)i_this->mRipple, -1, nullptr,
                            nullptr, nullptr);
            if (execFields(i_this)->mRippleEmitter != 0) {
                execFields(i_this)->mRippleHeight = 0.0f;
            }
        }
    } else {
        dPa_rippleEcallBack_end((dPa_rippleEcallBack*)i_this->mRipple);
    }
    if (gabi::ea(l_zelda.get()) == 0) {
        l_zelda = (fopAc_ac_c*)fpcM_Search(gabi::guest_fn((const void*)z_s_sub), i_this);
    }
    {
        u8 hioC = (u8)l_execHIO.mPhaseOverride;
        u8 st = (u8)i_this->stealItemBitNo;
        if (hioC != 0)
            execFields(i_this)->mBattlePhase = hioC - 1;
        if (st == 0x23) {
            i_this->stealItemBitNo = 0x24;
            gnd_changeBgmStatus(2);
        }
        u8 b3 = (u8)((u32)i_this->mParameters & 0xFF);
        s16 cnt = execFields(i_this)->mCounter;
        if (b3 == 0x23) {
            i_this->mParameters = 0;
            execFields(i_this)->mAction = 0x15;
            execFields(i_this)->mCameraCut = 0x14;
        }
        execFields(i_this)->mCounter = cnt + 1;
        for (int k = 0; k < 5; k++) {
            s16 v = execFields(i_this)->mTimers[k];
            if (v != 0)
                execFields(i_this)->mTimers[k] = v - 1;
        }
        be<s16>* timers[4] = {&execFields(i_this)->mDamageTimer, &execFields(i_this)->mHitTimer,
                              &execFields(i_this)->mAttackTimer, &execFields(i_this)->mEffectTimer};
        for (int k = 0; k < 4; k++) {
            s16 v = *timers[k];
            if (v != 0)
                *timers[k] = v - 1;
        }
    }
    if ((u8)l_execHIO.mFreeze == 0) {
        gndExec_move(i_this);
        i_this->mAcch.CrrPos(dComIfG_Bgsp());
        settingTevStruct(dKy_getEnvlight(), 2, &i_this->current.pos,
                         &execFields(i_this)->mBattleTevStr);
        i_this->mpMorf->play(&i_this->eyePos, 0, 0);
        if (gnd_isStop(i_this->mpMorf))
            attack_eff_remove(i_this);
        mDoExt_baseAnm_play(i_this->mpBrk);
        mDoExt_baseAnm_play(i_this->mpBtk);
        mDoExt_baseAnm_play(i_this->mpBtp);
    }

    /* the model matrix */
    J3DModel* model = i_this->mpMorf->getModel();
    {
        f32 s = (f32)l_execHIO.mScale;
        i_this->scale.z = s;
        i_this->scale.y = s;
        i_this->scale.x = s;
        model->mBaseScale.x = s;
        model->mBaseScale.y = s;
        model->mBaseScale.z = s;
    }
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    cLib_addCalc0(&execFields(i_this)->mModelYawOffset, REG0_F(17) + 0.5f, REG0_F(18) + 6053.5f);
    mDoMtx_YrotM(mDoMtx_stack_c::get(),
                 (s16)(i_this->shape_angle.y + gabi::ftoi(execFields(i_this)->mModelYawOffset)));
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    i_this->mpMorf->calc();

    /* eye, attention and the body collision */
    gabi::Local<cXyz> zero;
    zero->y = 0.0f;
    zero->z = 0.0f;
    zero->x = 0.0f;
    PSMTXCopy(gnd_getAnmMtx(model, 0x30), calc_mtx());
    MtxPosition(zero, (&execFields(i_this)->mHeadPos));
    {
        i_this->eyePos.x = execFields(i_this)->mHeadPos.x;
        f32 ey = execFields(i_this)->mHeadPos.y;
        i_this->eyePos.y = ey;
        i_this->eyePos.z = execFields(i_this)->mHeadPos.z;
        f32 px = i_this->current.pos.x;
        f32 reg = REG_F(10, 5);
        i_this->attention_info.position.x = px;
        f32 off = gabi::fadds_ppc(reg, 80.0f);
        f32 py = i_this->current.pos.y;
        f32 pz = i_this->current.pos.z;
        f32 eyY = gabi::fsubs_ppc(ey, off);
        i_this->attention_info.position.z = pz;
        f32 ay = gabi::fadds_ppc(py, 290.0f);
        i_this->eyePos.y = eyY;
        i_this->attention_info.position.y = ay;
    }
    i_this->mCyl.SetC(&i_this->current.pos);
    i_this->mCyl.SetR(80.0f);
    i_this->mCyl.SetH(220.0f);
    dComIfG_Ccsp_Set(&i_this->mCyl);
    i_this->mHeadSph.SetC((&execFields(i_this)->mHeadPos));
    i_this->mHeadSph.SetR(
        gabi::fmuls_ppc(gabi::fadds_ppc(REG0_F(11), 70.0f), (f32)l_execHIO.mScale));
    dComIfG_Ccsp_Set(&i_this->mHeadSph);
    gabi::Local<cXyz> tmp;
    PSMTXCopy(gnd_getAnmMtx(model, 9), calc_mtx());
    MtxPosition(zero, tmp);
    tmp->y = gabi::fsubs_ppc(tmp->y, gabi::fadds_ppc(REG0_F(10), 20.0f));
    i_this->mChestSph.SetC(tmp);
    i_this->mChestSph.SetR(
        gabi::fmuls_ppc(gabi::fadds_ppc(REG0_F(11), 170.0f), (f32)l_execHIO.mScale));
    dComIfG_Ccsp_Set(&i_this->mChestSph);

    /* Disable body target collision during the invulnerability timer. */
    {
        s8 t = execFields(i_this)->mInvulnerabilityTimer;
        if (t != 0) {
            i_this->mHeadSph.mObjTg.mSPrm = i_this->mHeadSph.mObjTg.mSPrm & ~1u;
            execFields(i_this)->mInvulnerabilityTimer = t - 1;
            i_this->mCyl.mObjTg.mSPrm = i_this->mCyl.mObjTg.mSPrm & ~1u;
        } else {
            i_this->mCyl.mObjTg.mSPrm = i_this->mCyl.mObjTg.mSPrm | 1;
            i_this->mHeadSph.mObjTg.mSPrm = i_this->mHeadSph.mObjTg.mSPrm | 1;
        }
    }
    for (int i = 0; i < 2; i++) {
        gnd_weponSphUpdate(i_this, model, player0, i, zero, tmp);
    }

    /* the sword's attack type for the player's guard (fopEn_enemy_c) */
    {
        s8 a = execFields(i_this)->mGuardTimer;
        if (a != 0) {
            execFields(i_this)->mGuardTimer = a - 1;
            execFields(i_this)->mGuardAttackType = 1;
        }
        s8 b = execFields(i_this)->mGuardTimerAlt;
        if (b != 0) {
            execFields(i_this)->mGuardTimerAlt = b - 1;
            execFields(i_this)->mGuardAttackType = 2;
        }
        i_this->mBtNowFrame = 15.0f;
        u8 type = execFields(i_this)->mGuardAttackType;
        i_this->mBtStartFrame = 10.0f;
        i_this->mBtEndFrame = 20.0f;
        i_this->mBtMaxDis = 10000.0f;
        i_this->mBtAttackType = type;
        i_this->mBtMaxDis = REG0_F(4) + 600.0f;
        execFields(i_this)->mGuardAttackType = 0;
        if ((u8)l_execHIO.mStartZeldaDemo != 0) {
            l_execHIO.mStartZeldaDemo = 0;
            execFields(i_this)->mCameraCut = 10;
        }
    }

    demo_camera(i_this);
    ke_move(i_this);

    /* Attack effects follow their model joints. */
    model = i_this->mpMorf->getModel();
    for (int j = 0; j < 6; j++) {
        u32 e = execFields(i_this)->mAttackEmitters[j];
        if (e != 0) {
            s32 jnt = l_attackEffectJoints[j];
            gnd_JPASetRMtxTVecfromMtx(gnd_getAnmMtx(model, jnt), e + 0x1F0, e + 0x22C);
        }
    }
    body_flash(i_this);
    return TRUE;
}
/* WWHD glue (tools/verify/autoglue.py): execute fragment of the Ganondorf TU
ext 02526E70 void* gnd_dDemo_getActor(u32 obj, u8 id)
ext 02527828 void* gnd_dDemo_getP_BtpData(void* ac, const char* arc)
ext 025279C8 void* gnd_dDemo_getP_BtkData(void* ac, const char* arc)
ext 02527A98 void* gnd_dDemo_getP_BrkData(void* ac, const char* arc)
ext 02527028 BOOL gnd_dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7)
ext 024EF0F4 s32 gnd_GetAttributeCode(dBgS* bgs, void* poly)
ext 028249B0 void gnd_JPASetRMtxTVecfromMtx(Mtx34* m, u32 r, u32 t)
ext 025D7B24 BOOL gnd_orderPotentialEvent(fopAc_ac_c* a, u16 type, u16 flag, u16 p)
ext 02514F2C void gnd_cam_Stop(u32 cam)
ext 02514F38 void gnd_cam_Start(u32 cam)
ext 02515280 void gnd_cam_SetTrimSize(u32 cam, s32 s)
ext 02514FE8 void gnd_cam_Set(u32 cam, cXyz* center, cXyz* eye, s16 bank, f32 fovy)
ext 0252012C void gnd_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 lastSpeed, u32 lastMode, s32 setPoint, s8 wipe)
ext 025E1904 void gnd_bgmStop(u32 frames)
ext 025E1950 void gnd_changeBgmStatus(s32 st)
ext 025F05E8 void gnd_fadeOut(s8 rate)
ext 025F05F8 void gnd_setFadeColor(u32 color)
ext 0259169C void gnd_play5C22_on()
data l_zelda 10464008
data l_execHIO 104640BC
data l_inactiveSwordPos 1046402C
data l_rippleScale 10464038
data l_rippleScaleInitialized 10464014
data l_cameraCenters 10464044
data l_cameraEyes 10464080
data l_cameraFovy 101B554C
data l_swordJoints 101B57D0
data l_swordBaseJoints 101B57C8
data l_swordAttackTypes 101B57D8
data l_attackEffectJoints 101B54DC
data l_demoFrame 101D600C
data l_demoObject 101D5FFC
data l_demoName 1047E6B8
data l_fadeColor 101D5E9C
ext 0215D268 void gndExec_assureTermination(SafeString*)
ext 02159FC8 void gndExec_move(gnd_class*)
ext 025CB374 BOOL gndExec_startShock(u32, s32, s32, cXyz*)
ext 025E1AA4 void gndExec_monsSeStart(u32, cXyz*, u32, s32, s32)
ext 027EC9E8 void gndExec_reportCamera(s32, s32, const char*, s32)
data l_cameraArchiveName 1000FFE8
str 1000FFE0 "Gnd"
str 1000FFEC "Demo42"
str 1000FFF4 "GTower"
str 1001009C "d_demo.h"
str 10010074 "m_object != (0)"
str 100100A8 "K SUB  COUNT  %d"
*/
