/**
 * d_a_ff.cpp (WWHD)
 * Object - Firefly (not collectable)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ff.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * HD: each firefly is a point light (LIGHT_INFLUENCE at the end of the class); the glow model
 * and the z-buffer visibility test (z_check, GXPeekZ) are gone from Draw.
 */
#include "d/actor/d_a_ff.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/f_pc/f_pc.h"

#define FF_VTBL 0x1000EB2C         /* ff_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x1000EAC4 /* this TU's sead::SafeString vtable */
static dCcD_SrcSph cc_sph_src;
static s32 ff_count = 0;
// HD: literal table entries let autoglue check them against the archive indices.
// dRes_INDEX_FF_BMD_HO_e, dRes_INDEX_FF_BMD_HOP_e
static u32 ho_bmd[] = {5, 6};
// dRes_INDEX_FF_BRK_HO_e, dRes_INDEX_FF_BRK_HOP_e
static u32 ho_brk[] = {12, 13};
enum { fpcNm_FF_e = 0xBB };
#define REG13_F(i) REG_F(13, i)

// HD: nullable BRK construction returns the entire guest result, not just the input pointer.
mDoExt_brkAnm* ffBrkConstruct(mDoExt_brkAnm*);
fopAcM_prm_class* ffCreateAppend();
u32 ffRequestCreate(layer_class*, s16, u32, u32, fopAcM_prm_class*);
void ffModelUpdate(J3DModel*);
void ffGroundCheckDestruct(cBgS_Chk*, s32);
void ffSphereConstruct(dCcD_Sph*);
void ffSphereDestruct(dCcD_Sph*, s32);
void ffActorDestruct(fopAc_ac_c*, s32);
static constexpr u32 GROUND_POLY_VTBL = 0x1000EAFC;
static constexpr u32 GROUND_POLYPASS_VTBL = 0x1000EB1C;
static constexpr u32 GROUND_GRPPASS_VTBL = 0x1000EADC;
static constexpr u32 HEADER_STATICS_PI = 0x10463D14;
static constexpr u32 HEADER_STATICS_DTORS = 0x101B4C44;

static u32 ffCreate(s16 name, fopAcM_prm_class* append) {
    layer_class* layer = fpcLy_CurrentLayer();
    return ffRequestCreate(layer, name, 0, 0, append);
}

/* 02132F3C */
static BOOL daFf_Draw(ff_class* i_this) {
    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, false);
    f32 s = i_this->mScale;
    MtxScale(s, s, s, true);
    J3DModel_setBaseTRMtx(i_this->mpModel[0], calc_mtx());
    if (i_this->mScale > 0.01f) {
        i_this->mBrkAnm[0]->play();
        J3DModel* m = i_this->mpModel[0];
        mDoExt_brkAnm* brk = i_this->mBrkAnm[0];
        brk->entry(m->getModelData(), brk->mFrameCtrl.mFrame);
        dComIfGd_setListMaskOff();
        ffModelUpdate(i_this->mpModel[0]);
        dComIfGd_setList();
    }
    return TRUE;
}

/* the dBgS_GndChk on fire_fly_move's stack (this TU's vtables) */
static const dBgS_GndChk_vt l_gnd_vt = {0x1000EAEC, 0x1000EAFC, 0x1000EB1C, 0x1000EB0C};

/* inlined into daFf_Execute */
static inline void ff_groundGlow(ff_class* i_this) {
    f32 g = i_this->mGroundY;
    f32 y = i_this->current.pos.y;
    if (y < g + 12.5f) {
        if (y < g)
            i_this->current.pos.y = g;
        cLib_addCalc2(&i_this->mGlowScaleY, 0.5f, 0.2f, 0.1f);
    } else {
        cLib_addCalc2(&i_this->mGlowScaleY, 1.0f, 0.2f, 0.1f);
    }
}

// HD: fire_fly_move inlined
/* 02133070 */
static BOOL daFf_Execute(ff_class* i_this) {
    for (int i = 0; i < 5; i++) {
        if (i_this->mTimers[i] != 0) {
            i_this->mTimers[i] -= 1;
        }
    }

    // HD: fire_fly_move is inlined; preserve fused sums and operand/evaluation order.
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    dBgS_GndChk chk;
    dBgS_GndChk_ct(&chk, l_gnd_vt, false);
    i_this->mLiveTimer += 1;
    if (i_this->mTimers[2] == 0) {
        i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(50.0f) + 40.0f);
        i_this->mTimers[2] = (s16)gabi::ftoi(cM_rndF(200.0f) + 100.0f);
    }
    if (i_this->mTimers[1] == 0) {
        i_this->mScaleTarget =
            gabi::fmadds(cM_ssin(i_this->mLiveTimer * 1000) * 0.15f, 0.25f, 0.225f);
    } else {
        i_this->mScaleTarget = 0.0f;
    }
    cLib_addCalc2(&i_this->mScale, i_this->mScaleTarget, 0.1f, 0.05f);

    u32 mode = (u32)(s32)i_this->mMode;
    bool move = false;
    if (mode == 0 || mode == 1) {
        if (mode == 0) {
            // HD: load x/y/z first, then store ground-check z/x/y.
            f32 x = i_this->current.pos.x, y = i_this->current.pos.y + 250.0f,
                z = i_this->current.pos.z;
            chk.m_pos.z = z;
            chk.m_pos.x = x;
            chk.m_pos.y = y;
            i_this->mGroundY = cBgS_GroundCross(dComIfG_Bgsp(), &chk) + 12.5f;
            if (i_this->mbNoUseGroundY == 0) {
                i_this->current.pos.y = i_this->mGroundY;
            }
            i_this->mHomePos.y = i_this->current.pos.y;
            i_this->mHomePos.x = i_this->current.pos.x;
            i_this->mHomePos.z = i_this->current.pos.z;
            i_this->mMode += 1;
        }
        /* case 1 */
        cLib_addCalc2(&i_this->current.pos.x, i_this->mHomePos.x, 0.1f,
                      std::fabs((f32)i_this->speed.x));
        cLib_addCalc2(&i_this->current.pos.y, i_this->mHomePos.y, 0.1f,
                      std::fabs((f32)i_this->speed.y));
        cLib_addCalc2(&i_this->current.pos.z, i_this->mHomePos.z, 0.1f,
                      std::fabs((f32)i_this->speed.z));
        f32 yd = player->current.pos.y - i_this->current.pos.y;
        f32 xd = player->current.pos.x - i_this->current.pos.x;
        f32 zd = player->current.pos.z - i_this->current.pos.z;
        f32 dist = std_sqrtf(gabi::fmadds(zd, zd, gabi::fmadds(xd, xd, yd * yd)));
        if (dist < 250.0f) {
            i_this->mMode += 1;
            i_this->mTimers[3] = (s16)gabi::ftoi(cM_rndF(100.0f) + 1000.0f);
            i_this->current.angle.x = -0x3000;
            i_this->speedF = 10.0f;
        }
        ff_groundGlow(i_this);
    } else if (mode == 2) {
        if (i_this->mTimers[0] == 0) {
            i_this->mScatterPos.x = cM_rndFX(750.0f) + i_this->mHomePos.x;
            i_this->mScatterPos.z = cM_rndFX(750.0f) + i_this->mHomePos.z;
            f32 ry = cM_rndFX(225.0f);
            i_this->mScatterPos.y = (i_this->mHomePos.y + ry) + 137.5f;
            i_this->mRotVel = 0.0f;
            i_this->mVelocityFwdTarget = cM_rndF(20.0f) + 10.0f;
            f32 xd = i_this->mScatterPos.x - i_this->current.pos.x;
            f32 yd = i_this->mScatterPos.y - i_this->current.pos.y;
            f32 zd = i_this->mScatterPos.z - i_this->current.pos.z;
            f32 xx = xd * xd, zz = zd * zd;
            f32 dist = std_sqrtf(gabi::fmadds(yd, yd, xx) + zz);
            i_this->mTimers[0] = (s16)gabi::ftoi(dist / i_this->mVelocityFwdTarget);
            i_this->mTargetRotY = cM_atan2s(xd, zd);
            f32 xz_dist = std_sqrtf(xx + zz);
            i_this->mTargetRotX = (s16)-cM_atan2s(yd, xz_dist);
        }
        if (i_this->mTimers[3] == 0) {
            i_this->mScatterPos.x = i_this->mHomePos.x;
            i_this->mScatterPos.y = i_this->mHomePos.y;
            i_this->mScatterPos.z = i_this->mHomePos.z;
            i_this->mRotVel = 0.0f;
            i_this->mMode += 1;
        }
        move = true;
    } else if (mode == 3) {
        f32 xd = i_this->mScatterPos.x - i_this->current.pos.x;
        f32 zd = i_this->mScatterPos.z - i_this->current.pos.z;
        f32 yd = i_this->mScatterPos.y - i_this->current.pos.y;
        i_this->mTargetRotY = cM_atan2s(xd, zd);
        f32 xx = xd * xd, zz = zd * zd;
        f32 xz_dist = std_sqrtf(xx + zz);
        i_this->mTargetRotX = (s16)-cM_atan2s(yd, xz_dist);
        if (gabi::fmadds(yd, yd, xx) + zz < 2500.0f) {
            i_this->mMode = 1;
        }
        cLib_addCalc2(&i_this->mRotVel, 10.0f, 1.0f, 0.15f);
        move = true;
    } else if (mode == 4) {
        if (i_this->mTimers[4] == 0) {
            i_this->mMode = 2;
            i_this->mTimers[3] = (s16)gabi::ftoi(cM_rndF(100.0f) + 1000.0f);
        } else {
            PSVECAdd(&i_this->mVelImpulse, &i_this->m2C0,
                     &i_this->mVelImpulse); /* mVelImpulse += m2C0 */
            if (cM_rndF(1.0f) < REG13_F(0) + 0.1f) {
                i_this->m2C0.x = cM_rndFX(REG13_F(1) + 1.0f);
            }
            if (cM_rndF(1.0f) < REG13_F(0) + 0.1f) {
                i_this->m2C0.z = cM_rndFX(REG13_F(1) + 1.0f);
            }
            i_this->m2C0.y = REG13_F(2) + 2.0f;
            f32 lim = REG13_F(3) + 10.0f;
            if (i_this->mVelImpulse.y > lim) {
                i_this->mVelImpulse.y = lim;
            }
        }
        move = true;
    } else {
        ff_groundGlow(i_this);
    }

    if (move) {
        f32 step = 500.0f;
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mTargetRotY, 10,
                            (s16)gabi::ftoi(step * i_this->mRotVel));
        cLib_addCalcAngleS2(&i_this->current.angle.x, i_this->mTargetRotX, 10,
                            (s16)gabi::ftoi(step * i_this->mRotVel));
        cLib_addCalc2(&i_this->mRotVel, 1.0f, 1.0f, 0.1f);
        cLib_addCalc2(&i_this->speedF, i_this->mVelocityFwdTarget, 1.0f, 3.0f);
        cXyz local_cc;
        local_cc.x = 0.0f;
        local_cc.y = 0.0f;
        local_cc.z = i_this->speedF * 0.25f;
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        MtxPosition(&local_cc, &i_this->speed);
        cXyz sum;
        cXyz_pl(&i_this->speed, &sum, &i_this->mVelImpulse);
        PSVECAdd(&i_this->current.pos, &sum, &i_this->current.pos);
        cLib_addCalc0(&i_this->mVelImpulse.x, 1.0f, 0.1f);
        cLib_addCalc0(&i_this->mVelImpulse.y, 1.0f, 0.1f);
        cLib_addCalc0(&i_this->mVelImpulse.z, 1.0f, 0.1f);
        ff_groundGlow(i_this);
    }

    /* HD: the firefly's point light follows it; its power follows the scale */
    {
        auto from = reinterpret_cast<FfPositionWords*>(&i_this->current.pos);
        auto to = reinterpret_cast<FfPositionWords*>(&i_this->mInf.mPos);
        u32 x = from->x, y = from->y, z = from->z;
        f32 power = 255.0f * i_this->mScale;
        to->x = x;
        to->y = y;
        to->z = z;
        i_this->mInf.mColorR = 100;
        i_this->mInf.mColorG = 300;
        i_this->mInf.mColorB = 300;
        i_this->mInf.mPower = power;
    }
    /* ~dBgS_GndChk */
    {
        chk.cBgS_PolyInfo::__vtbl = GROUND_POLY_VTBL;
        chk.dBgS_PolyPassChk::__vtbl = GROUND_POLYPASS_VTBL;
        chk.dBgS_GrpPassChk::__vtbl = GROUND_GRPPASS_VTBL;
        ffGroundCheckDestruct(static_cast<cBgS_Chk*>(&chk), 0);
    }

    if (i_this->mSph.ChkTgHit() != 0) {
        i_this->mMode = 4;
        i_this->mTimers[4] = (s16)gabi::ftoi((cM_rndF(20.0f) + 30.0f) + REG13_F(9));
    }
    i_this->mSph.SetC(&i_this->current.pos);
    dComIfG_Ccsp_Set(&i_this->mSph);
    return TRUE;
}

/* 02133AB8 */
static BOOL daFf_IsDelete(ff_class* i_this) {
    return TRUE;
}

/* 02133AC0 */
static BOOL daFf_Delete(ff_class* i_this) {
    dComIfG_resDelete(&i_this->mPhs, "Ff"); /* resDeleteDemo */
    ff_count = 0;
    dKy_plight_cut(&i_this->mInf); /* HD */
    return TRUE;
}

/* 02133B10 */
static BOOL useHeapInit(fopAc_ac_c* i_ac) {
    ff_class* a_this = (ff_class*)i_ac;
    for (int i = 0; i < 2; i++) {
        u32 bmd = ho_bmd[i];
        J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Ff", bmd, SAFESTRING_VTBL);
        if (modelData == nullptr) { /* JUT_ASSERT(741, modelData != NULL) */
            JUT_ASSERT_fail("d_a_ff.cpp", 0x2E5, "modelData != NULL");
            return FALSE;
        }
        a_this->mpModel[i] = mDoExt_J3DModel__create(modelData, 0x10000, 0x11020203);
        if (a_this->mpModel[i] == nullptr) {
            return FALSE;
        }
        /* new mDoExt_brkAnm() */
        mDoExt_brkAnm* brk = static_cast<mDoExt_brkAnm*>(operator_new(sizeof(mDoExt_brkAnm)));
        if (brk != nullptr)
            brk = ffBrkConstruct(brk);
        a_this->mBrkAnm[i] = brk;
        if (a_this->mBrkAnm[i] == nullptr) {
            return FALSE;
        }
        J3DModel* model = a_this->mpModel[i];
        u32 brkIdx = ho_brk[i];
        J3DAnmTevRegKey* key =
            static_cast<J3DAnmTevRegKey*>(dComIfG_getObjectRes("Ff", brkIdx, SAFESTRING_VTBL));
        f32 rate = cM_rndF(0.15f) + 0.9f;
        int iVar6 = a_this->mBrkAnm[i]->init(model->getModelData(), key, true,
                                             J3DFrameCtrl::EMode_LOOP, rate, 0, -1, false, 0);
        if (iVar6 == 0) {
            return FALSE;
        }
    }
    return TRUE;
}

// HD: ff_class::ff_class (HD: out of line; allocates when this == NULL)
/* 02133CF0 */
static ff_class* ff_class_ct(ff_class* i_this) {
    if (i_this == nullptr) {
        i_this = (ff_class*)operator_new(0x5C0);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = FF_VTBL;
    dCcD_Stts_ct(&i_this->mStts);
    ffSphereConstruct(&i_this->mSph); /* dCcD_Sph::dCcD_Sph */
    i_this->mInf.mHD20 = 1.0f;        /* HD: LIGHT_INFLUENCE constructor */
    return i_this;
}

/* 02133D80 */
static cPhs_State daFf_Create(fopAc_ac_c* i_this) {
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr)
            ff_class_ct((ff_class*)i_this);
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    ff_class* a_this = (ff_class*)i_this;
    cPhs_State phase_state = dComIfG_resLoad(&a_this->mPhs, "Ff");
    if (phase_state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(a_this, useHeapInit, 0x2320)) {
            int uVar1 = fopAcM_GetParam(a_this) & 0xFF;
            if (uVar1 != 0) {
                a_this->mParameters = fopAcM_GetParam(a_this) & 0xFF00;
                for (int i = 0; i < uVar1; i++) {
                    fopAcM_prm_class* p = ffCreateAppend();
                    f32 rx = cM_rndFX(500.0f);
                    p->base.position.x = a_this->current.pos.x + rx;
                    p->base.position.y = a_this->current.pos.y;
                    f32 rz = cM_rndFX(500.0f);
                    p->base.angle.z = 0;
                    p->base.angle.y = 0;
                    p->base.angle.x = 0;
                    p->base.position.z = a_this->current.pos.z + rz;
                    p->base.parameters = fopAcM_GetParam(a_this);
                    ffCreate(fpcNm_FF_e, p);
                }
            }
            a_this->mbNoUseGroundY = fopAcM_GetParam(a_this) >> 8;
            ff_count += 1;
            a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(a_this->mpModel[0]));
            a_this->mHomePos.copy(a_this->current.pos);
            a_this->m2EC.copy(a_this->mHomePos);
            a_this->mLiveTimer = (s16)gabi::ftoi(cM_rndF(32768.0f));
            a_this->mTimers[2] = (s16)gabi::ftoi(cM_rndF(100.0f));
            a_this->mStts.Init(200, 0, a_this);
            a_this->mSph.Set(&cc_sph_src);
            /* HD: the point light starts at the home position */
            a_this->mInf.mPos.copy(a_this->m2EC);
            a_this->mInf.mPower = 0.0f;
            a_this->mSph.SetStts(&a_this->mStts);
            a_this->mInf.mFluctuation = 250.0f;
            a_this->mInf.mColorR = 100;
            a_this->mInf.mColorG = 300;
            a_this->mInf.mColorB = 300;
            dKy_plight_set(&a_this->mInf);
        } else {
            return cPhs_ERROR_e;
        }
    }
    return phase_state;
}

// HD: static initialisation of the translation unit (header statics only)
// HD: compiler-generated and template code of this translation unit.
/* 02133FF0 */
static void __sinit_d_a_ff_cpp() {
    sinit_header_statics(HEADER_STATICS_PI, HEADER_STATICS_DTORS);
}

// HD: sead::SafeString deleting destructor (this TU's vtable 0x1000EAC4)
/* 02134084 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

// HD: ff_class deleting destructor (compiler-generated, HD virtual destructor)
/* 02134098 */
static void ff_class_dt(ff_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        ffSphereDestruct(&i_this->mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mStts, 2);
        ffActorDestruct(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
/* 02134104 */
static void SafeString_assureTerminationImpl(void* p) {
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
data ff_count 101B4BD0
data ho_bmd 101B4BF4
data ho_brk 101B4BFC
data cc_sph_src 101B4C04
str 1000EBAC "Ff" in 02133AC0
str 1000EBB4 "Ff" in 02133B10
str 1000EBDC "Ff" in 02133D80
str 1000EBB8
str 1000EBC4 "modelData != NULL"
ext 025E80D0 mDoExt_brkAnm* ffBrkConstruct(mDoExt_brkAnm*)
ext 025D5600 fopAcM_prm_class* ffCreateAppend()
ext 025E14A8 u32 ffRequestCreate(layer_class*,s16,u32,u32,fopAcM_prm_class*)
ext 025E2DA8 void ffModelUpdate(J3DModel*)
ext 02008DAC void ffGroundCheckDestruct(cBgS_Chk*,s32)
ext 025166F0 void ffSphereConstruct(dCcD_Sph*)
ext 02515AE8 void ffSphereDestruct(dCcD_Sph*,s32)
ext 025D50BC void ffActorDestruct(fopAc_ac_c*,s32)
*/
// clang-format on
