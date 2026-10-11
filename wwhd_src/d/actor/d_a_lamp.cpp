/**
 * d_a_lamp.cpp (WWHD)
 * Object - Generic wall lamp (swinging lamp with a torch flame and a point light).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_lamp.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#define SAFESTRING_VTBL 0x10013CD4
#include "d/actor/d_a_lamp.h"

static dCcD_SrcSph sph_src;

/* 021B1590 */
static BOOL daLamp_Draw(lamp_class* i_this) {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    J3DModel* pModel = i_this->mModel;
    g_env_light.setLightTevColorType(pModel, &i_this->tevStr);
    mDoExt_modelUpdateDL(pModel);
    return TRUE;
}


/* 021B15F8 */
static BOOL daLamp_Execute(lamp_class* i_this) {
    i_this->mCycleCtr += 1;
    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
    mDoMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    if (i_this->mParameters == 0) {
        i_this->mLength = 0.1f;
    } else {
        if (dComIfGp_getVibration_CheckQuake() || fopAcM_otoCheck(i_this, 1000.0f) > 400) {
            i_this->mOto = 0x14;
        }
        if (i_this->mOto) {
            i_this->mOto--;
            cLib_addCalc2(&i_this->mLength, 0.15f, 1.0f, 0.015f);
        } else {
            cLib_addCalc2(&i_this->mLength, 0.01f, 1.0f, 0.0005f);
        }
    }
    f32 oppDist = cM_ssin(i_this->mCycleCtr * 0x320) * i_this->mLength;
    MtxRotX(oppDist, 1);
    f32 adjDist = cM_scos(i_this->mCycleCtr * 0x2bc) * i_this->mLength;
    MtxRotZ(adjDist, 1);
    MtxScale(0.4f, 0.4f, 0.4f, 1);
    J3DModel_setBaseTRMtx(i_this->mModel, calc_mtx());
    MtxTrans(10.0f, -140.0f, -15.0f, 1);

    {
        cXyz offset;
        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = 0.0f;
        MtxPosition(&offset, &i_this->mPos);
    }

    if (!i_this->mParticleInit) {
        static cXyz fire_scale(0.5f, 0.5f, 0.5f);
        dComIfGp_particle_set(ID_AK_JN_TORCH, &i_this->mPos, nullptr, &fire_scale, 0xFF,
                              (dPa_levelEcallBack*)(void*)&i_this->mPa);
        i_this->mParticleInit = 1;
        i_this->mParticlePower = 1.0f;
    }

    if (i_this->mPa.getEmitter()) {
        cXyz whitePartPos;
        whitePartPos.x = i_this->mPos.x;
        whitePartPos.y = i_this->mPos.y + 20.0f;
        whitePartPos.z = i_this->mPos.z;
        dComIfGp_particle_setSimple(ID_AK_JP_O_KAGEROU00, &whitePartPos);
        cLib_addCalc2(&i_this->mParticlePower, cM_rndF(0.2f) + 1.0f, 0.5f, 0.02f);
    } else {
        i_this->mParticlePower = 0.0f;
    }
    i_this->mInf.mPos.copy(i_this->mPos);
    i_this->mInf.mColorR = 600;
    i_this->mInf.mColorG = 400;
    i_this->mInf.mColorB = 120;

    s16 power = (s16)gabi::ftoi(i_this->mParticlePower * 150.0f);
    i_this->mInf.mPower = power;
    i_this->mInf.mFluctuation = 250.0f;

    i_this->mSph.SetC(&i_this->mPos);
    dComIfG_Ccsp_Set(&i_this->mSph);
    if (!i_this->mHitTimeoutLeft) {
        if (i_this->mSph.ChkTgHit()) {
            void* pHitObj = i_this->mSph.GetTgHitObj();
            if (pHitObj && cCcD_Obj_ChkAtType(pHitObj, AT_TYPE_WIND | AT_TYPE_UNK400000)) {
                i_this->mHitAngle = dComIfGp_getPlayer(0)->shape_angle.y;
                i_this->mHitTimeoutLeft = 0x28;
            }
        }
    } else {
        i_this->mHitTimeoutLeft--;
        if (i_this->mPa.getEmitter()) {
            f32 tgtZ;
            if (i_this->mHitTimeoutLeft > 10) {
                tgtZ = 4.0f;
            } else {
                tgtZ = 0.0f;
            }
            cLib_addCalc2(&i_this->mHitReactCurZ, tgtZ, 1.0f, 0.5f);
            cMtx_YrotS(calc_mtx(), i_this->mHitAngle);
            cXyz offset;
            cXyz rotOffset;
            offset.set(0.0f, 1.0f, i_this->mHitReactCurZ);
            MtxPosition(&offset, &rotOffset);
            JPABaseEmitter_setDirection(i_this->mPa.getEmitter(), rotOffset.x, rotOffset.y, rotOffset.z);
        }
    }

    return TRUE;
}

/* 021B1C04 */
static BOOL daLamp_IsDelete(lamp_class* i_this) {
    i_this->mPa.remove();
    return TRUE;
}

/* 021B1C34 */
static BOOL daLamp_Delete(lamp_class* i_this) {
    dComIfG_resDelete(&i_this->mPhs, "Lamp" /* "Lamp" */);
    dKy_plight_cut(&i_this->mInf);
    return TRUE;
}

/* 021B1C78 */
// HD: the solid-heap callback wrapper is folded into useHeapInit.
static BOOL useHeapInit(lamp_class* i_this) {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Lamp", dRes_INDEX_LAMP_BMD_LAMP_00_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x170, modelData != NULL) */
        JUT_ASSERT_fail("d_a_lamp.cpp", 0x170, "modelData != (0)");

    i_this->mModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->mModel == nullptr) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/* 021B1D14 */
static lamp_class* lamp_class_ct(lamp_class* i_this) {
    if (i_this == nullptr) {
        i_this = (lamp_class*)operator_new(0x584);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = LAMP_VTBL;
    dPa_followEcallBack_ct(&i_this->mPa, 0, 0);
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Sph__ct(&i_this->mSph);
    i_this->mInf.mHD20 = 1.0f;            /* HD: LIGHT_INFLUENCE constructor */
    return i_this;
}

/* 021B1DB4 */
static cPhs_State daLamp_Create(fopAc_ac_c* i_ac) {
    /* fopAcM_ct(i_ac, lamp_class) */
    if (!fopAcM_CheckCondition(i_ac, fopAcCnd_INIT_e)) {
        if (i_ac != nullptr)
            lamp_class_ct((lamp_class*)i_ac);
        fopAcM_OnCondition(i_ac, fopAcCnd_INIT_e);
    }
    lamp_class* i_this = (lamp_class*)i_ac;

    cPhs_State phase_state = dComIfG_resLoad(&i_this->mPhs, "Lamp" /* "Lamp" */);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, reinterpret_cast<BOOL (*)(fopAc_ac_c*)>(useHeapInit), 0x6040)) {
            i_this->mParameters = fopAcM_GetParam(i_this);
            if (i_this->mParameters == 0xFF) {
                i_this->mParameters = 0;
            }
            i_this->mStts.Init(0xff, 0xff, i_this);
            i_this->mSph.Set(&sph_src);
            i_this->mSph.SetStts(&i_this->mStts);

            i_this->mCycleCtr = (s16)gabi::ftoi(cM_rndFX(32768.0f));

            for (int i = 0; i < 2; i++) {
                daLamp_Execute(i_this);
            }

            dKy_plight_set(&i_this->mInf);
        } else {
            phase_state = cPhs_ERROR_e;
        }
    }

    return phase_state;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */
/* 021B1F2C */
static void __sinit_d_a_lamp_cpp() {
    sinit_header_statics(0x10464DA8, 0x101B8EA8);
}

/* ---- leftover functions of the translation unit ---- */

/* 021B1FC0 */
static void lamp_SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) operator_delete(p);
}

/* 021B1FD4 */
static void daLamp_c_dt(void* p, s32 flags) {
    if (p != nullptr) {
        lamp_class* i_this = static_cast<lamp_class*>(p);
        dCcD_Sph__dt(&i_this->mSph, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1) operator_delete(p);
    }
}

/* 021B2040 */
static void lamp_SafeString_assureTermination(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str 10013D78 in 021B1C34
str 10013CCC in 021B1C78
str 10013CFC in 021B1C78
str 10013D0C in 021B1C78
str 10013D84 in 021B1DB4
data sph_src 101B8E68
data fire_scale 10464DC4 guard 10464DD0
ext 025166F0 void dCcD_Sph__ct(dCcD_Sph*)
ext 02515AE8 void dCcD_Sph__dt(dCcD_Sph*, s32)
*/
