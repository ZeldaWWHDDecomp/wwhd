/**
 * d_a_lamp.cpp (WWHD)
 * Object - Generic wall lamp (swinging lamp with a torch flame and a point light).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_lamp.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x10013CD4 /* this TU's sead::SafeString vtable */
#define LAMP_VTBL 0x10013CEC       /* lamp_class vtable (HD virtual destructor) */
#define sph_src gabi::at<dCcD_SrcSph>(0x101B8E68)

enum { dRes_INDEX_LAMP_BMD_LAMP_00_e = 3 };
enum { ID_AK_JN_TORCH = 0x01EA, ID_AK_JP_O_KAGEROU00 = 0x4004 };
enum { AT_TYPE_WIND = 0x200000, AT_TYPE_UNK400000 = 0x400000 };

struct lamp_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ be<u8> mParameters;
    /* 0x3B5 */ u8 _3B5;
    /* 0x3B6 */ be<s16> mCycleCtr;
    /* 0x3B8 */ gptr<J3DModel> mModel;
    /* 0x3BC */ dPa_followEcallBack mPa;
    /* 0x3D0 */ be<u8> mParticleInit;
    /* 0x3D1 */ u8 _3D1[3];
    /* 0x3D4 */ cXyz mPos;
    /* 0x3E0 */ dCcD_Stts mStts;
    /* 0x41C */ dCcD_Sph mSph;
    /* 0x548 */ be<s8> mOto;
    /* 0x549 */ u8 _549[3];
    /* 0x54C */ be<f32> mLength;
    /* 0x550 */ be<s8> mHitTimeoutLeft;
    /* 0x551 */ u8 _551[3];
    /* 0x554 */ be<f32> mHitReactCurZ;
    /* 0x558 */ be<s16> mHitAngle;
    /* 0x55A */ u8 _55A[2];
    /* 0x55C */ LIGHT_INFLUENCE mInf;  /* HD: 0x24 bytes */
    /* 0x580 */ be<f32> mParticlePower;
};
WWHD_OFFSET(lamp_class, mSph, 0x41C);
WWHD_OFFSET(lamp_class, mInf, 0x55C);
WWHD_SIZE(lamp_class, 0x584);

/* 021B1590 */
static BOOL daLamp_Draw(lamp_class* i_this) {
    WWHD_FUNC(0x021B1590, BOOL, i_this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    J3DModel* pModel = i_this->mModel;
    setLightTevColorType(dKy_getEnvlight(), pModel, &i_this->tevStr);
    mDoExt_modelUpdateDL(pModel);
    return TRUE;
}
VERIFY(0x021B1590, daLamp_Draw);


/* 021B15F8 */
static BOOL daLamp_Execute(lamp_class* i_this) {
    WWHD_FUNC(0x021B15F8, BOOL, i_this);
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
        gabi::Local<cXyz> offset;
        offset->x = 0.0f;
        offset->y = 0.0f;
        offset->z = 0.0f;
        MtxPosition(offset, &i_this->mPos);
    }

    if (!i_this->mParticleInit) {
        /* static cXyz fire_scale(0.5f, 0.5f, 0.5f); */
        be<u32>& guard = *gabi::at<be<u32>>(0x10464DD0);
        cXyz* fire_scale = gabi::at<cXyz>(0x10464DC4);
        if (guard == 0) {
            fire_scale->set(0.5f, 0.5f, 0.5f);
            guard = 1;
        }
        dComIfGp_particle_set(ID_AK_JN_TORCH, &i_this->mPos, nullptr, fire_scale, 0xFF,
                              (dPa_levelEcallBack*)(void*)&i_this->mPa);
        i_this->mParticleInit = 1;
        i_this->mParticlePower = 1.0f;
    }

    if (i_this->mPa.getEmitter()) {
        gabi::Local<cXyz> whitePartPos;
        whitePartPos->x = i_this->mPos.x;
        whitePartPos->y = i_this->mPos.y + 20.0f;
        whitePartPos->z = i_this->mPos.z;
        dComIfGp_particle_setSimple(ID_AK_JP_O_KAGEROU00, whitePartPos);
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
            gabi::Local<cXyz> offset;
            gabi::Local<cXyz> rotOffset;
            offset->set(0.0f, 1.0f, i_this->mHitReactCurZ);
            MtxPosition(offset, rotOffset);
            JPABaseEmitter_setDirection(i_this->mPa.getEmitter(), rotOffset->x, rotOffset->y, rotOffset->z);
        }
    }

    return TRUE;
}
VERIFY(0x021B15F8, daLamp_Execute);

/* 021B1C04 */
static BOOL daLamp_IsDelete(lamp_class* i_this) {
    WWHD_FUNC(0x021B1C04, BOOL, i_this);
    i_this->mPa.remove();
    return TRUE;
}
VERIFY(0x021B1C04, daLamp_IsDelete);

/* 021B1C34 */
static BOOL daLamp_Delete(lamp_class* i_this) {
    WWHD_FUNC(0x021B1C34, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x10013D78) /* "Lamp" */);
    dKy_plight_cut(&i_this->mInf);
    return TRUE;
}
VERIFY(0x021B1C34, daLamp_Delete);

/* 021B1C78: also the solid heap callback (HD: daLamp_solidHeapCB folded into it) */
static BOOL useHeapInit(lamp_class* i_this) {
    WWHD_FUNC(0x021B1C78, BOOL, i_this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10013CCC), dRes_INDEX_LAMP_BMD_LAMP_00_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x170, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x10013CFC), 0x170, STR(0x10013D0C));

    i_this->mModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->mModel == nullptr) {
        return FALSE;
    } else {
        return TRUE;
    }
}
VERIFY(0x021B1C78, useHeapInit);

/* 021B1D14: lamp_class::lamp_class (HD: out of line; allocates when this == NULL) */
static lamp_class* lamp_class_ct(lamp_class* i_this) {
    WWHD_FUNC(0x021B1D14, lamp_class*, i_this);
    if (i_this == nullptr) {
        i_this = (lamp_class*)operator_new(0x584);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = LAMP_VTBL;
    dPa_followEcallBack_ct(&i_this->mPa, 0, 0);
    dCcD_Stts_ct(&i_this->mStts);
    gabi::call(0x025166F0, &i_this->mSph); /* dCcD_Sph::dCcD_Sph */
    i_this->mInf.mHD20 = 1.0f;            /* HD: LIGHT_INFLUENCE constructor */
    return i_this;
}
VERIFY(0x021B1D14, lamp_class_ct);

/* 021B1DB4 */
static cPhs_State daLamp_Create(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x021B1DB4, cPhs_State, i_ac);
    /* fopAcM_ct(i_ac, lamp_class) */
    if (!fopAcM_CheckCondition(i_ac, fopAcCnd_INIT_e)) {
        if (i_ac != nullptr)
            lamp_class_ct((lamp_class*)i_ac);
        fopAcM_OnCondition(i_ac, fopAcCnd_INIT_e);
    }
    lamp_class* i_this = (lamp_class*)i_ac;

    cPhs_State phase_state = dComIfG_resLoad(&i_this->mPhs, STR(0x10013D84) /* "Lamp" */);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, 0x021B1C78 /* useHeapInit */, 0x6040)) {
            i_this->mParameters = fopAcM_GetParam(i_this);
            if (i_this->mParameters == 0xFF) {
                i_this->mParameters = 0;
            }
            i_this->mStts.Init(0xff, 0xff, i_this);
            i_this->mSph.Set(sph_src);
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
VERIFY(0x021B1DB4, daLamp_Create);

/* 021B1F2C */
static void __sinit_d_a_lamp_cpp() {
    WWHD_FUNC(0x021B1F2C, void, (u32)0);
    sinit_header_statics(0x10464DA8, 0x101B8EA8);
}
VERIFY(0x021B1F2C, __sinit_d_a_lamp_cpp);

/* ---- leftover functions of the translation unit ---- */

/* 021B1FC0 sead::SafeStringBase<char>::~SafeStringBase (deleting; nothing to destroy); vtable slot 10013CE0 */
static void lamp_SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021B1FC0, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x021B1FC0, lamp_SafeString_dt);

/* 021B1FD4 lamp_class::~lamp_class (deleting; vtable slot 10013CF8) */
static void daLamp_c_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021B1FD4, void, p, flags);
    if (p != nullptr) {
        u32 t = gabi::ea(p);
        gabi::call(0x02515AE8, t + 0x41C, 2); /* dCcD_Sph */
        gabi::call(0x02515860, t + 0x3E0, 2); /* dCcD_Stts */
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x021B1FD4, daLamp_c_dt);

/* 021B2040 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10013CE8, after the destructor 021B1FC0 */
static void lamp_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x021B2040, void, p);
}
VERIFY(0x021B2040, lamp_SafeString_assureTermination);
