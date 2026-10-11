/**
 * d_a_ssk.cpp (WWHD)
 * Object - Tingle-tower style stone tentacle ("Ssk": rock that rises when the player is near).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ssk.cpp) and, where HD changed it, written from the WWHD code;
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * HD changes (summary): m2F2 (wobble direction) and m314[5] (joint positions) are gone; the rock
 * fades in/out through a new amplitude mWobbleAmplitude and an "active" flag mColliderActive that
 * gates the collider; the tentacle model is shown by a translation (scale.y * 320 - 320) instead of
 * a base scale; the joint callbacks are installed once in useHeapInit; case 10 always plays the
 * retract sound and smoke. */
#include "d/actor/d_a_ssk.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1003D2AC,
    .objAcch = {0x1003D2D4, 0x1003D2F4, 0x1003D2E4},
};

static ssk_detail::CylinderSource body_co_cyl;
static ssk_detail::HeaderStatics headerStatics;
static cXyz smokeScale;
static ssk_detail::RegistrationRecord firstRegistration;
static ssk_detail::RegistrationRecord secondRegistration;
static ssk_detail::RegistrationRecord thirdRegistration;
static __jnt_hit_data_c search_data[3] = {
    {8, 1, 50.0f, 0x101D0C84},
    {8, 2, 30.0f, 0x101D0C84},
    {8, 3, 20.0f, 0x101D0C84},
};

// HD: out-of-line culling and constructor/destructor bindings used by this TU.
void sskSetMin(fopAc_ac_c*, f32, f32, f32);
void sskSetMax(fopAc_ac_c*, f32, f32, f32);
JntHit_c* sskCreateJntHit(J3DModel*, __jnt_hit_data_c*, s16);
void sskConstructSmoke(dPa_smokeEcallBack*, u8);
void sskConstructFirstHeaderObject(void*);
void sskConstructSecondHeaderObject(void*);
void sskDestroyAcch(dBgS_Acch*, s32);
void sskDestroyCircle(cM3dGCir_l*, s32);
void sskDestroyActor(fopAc_ac_c*, s32);

// HD: joint resolution precedes the current-model load; no joint position array is updated.
/* 0248D880 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys.getModel();
        ssk_class* i_this = static_cast<ssk_class*>(model->getUserArea());
        s32 jntNo = reinterpret_cast<ssk_detail::Joint*>(joint)->mJointNo;

        if (i_this != nullptr && jntNo < 5) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());

            cMtx_YrotM(calc_mtx(), i_this->m2F4[jntNo].y);
            cMtx_XrotM(calc_mtx(), i_this->m2F4[jntNo].x);
            cMtx_ZrotM(calc_mtx(), i_this->m2F4[jntNo].z);

            Mtx34* dst = model->getAnmMtx(jntNo); /* model->setAnmMtx(jntNo, *calc_mtx) */
            mtx_copy(dst, calc_mtx());
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx); /* J3DSys::mCurrentMtx */
        }
    }
    return TRUE;
}

// HD: callbacks are installed in useHeapInit; visible models share yaw, and scale.y raises the
// tentacle.
/* 0248D9B4 */
static void draw_sub(ssk_class* i_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    J3DModel* pJVar4 = i_this->mpMorf1->getModel();
    J3DModel* pJVar3 = i_this->mpMorf2->getModel();

    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->current.angle.y);

    if (i_this->m350.x > 0.01f) {
        pJVar3->setBaseScale(i_this->m350);
        pJVar3->setBaseTRMtx(mDoMtx_stack_c::get());
    }
    f32 sy = i_this->scale.y;
    if (sy > 0.01f) {
        mDoMtx_stack_c::transM(0.0f, gabi::fmadds(sy, 320.0f, -320.0f), 0.0f);
        pJVar4->setBaseTRMtx(mDoMtx_stack_c::get());
    }

    i_this->mpMorf1->calc();
}

// HD: draw each model only while it is visible.
/* 0248DB4C */
static BOOL daSsk_Draw(ssk_class* i_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (i_this->scale.y > 0.01f) {
        J3DModel* pJVar2 = i_this->mpMorf1->getModel();
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, pJVar2, &i_this->tevStr);
        i_this->mpMorf1->entryDL();
    }
    if (i_this->m350.x > 0.01f) {
        J3DModel* pJVar1 = i_this->mpMorf2->getModel();
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, pJVar1, &i_this->tevStr);
        i_this->mpMorf2->updateDL();
    }
    return TRUE;
}

/* dComIfGp_particle_setToon (inline): dPa_control_c::set on group 2 */
/* inlined into daSsk_Execute */
static inline void particle_setToon(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale,
                                    u8 alpha, void* cb, s8 setup) {
    dPa_control_c* pa = dComIfGp_getParticle();
    dPa_control_set(pa, 2, id, pos, angle, scale, alpha, (dPa_levelEcallBack*)cb, setup, nullptr,
                    nullptr, nullptr);
}

/* inlined into daSsk_Execute: lazily initialized smoke scale */
static cXyz* smoke_scale() {
    cXyz* s = &smokeScale;
    if (headerStatics.mSmokeScaleInitialized == 0) {
        // HD: the guard is stored before the vector components.
        headerStatics.mSmokeScaleInitialized = 1;
        s->x = 0.5f;
        s->y = 0.5f;
        s->z = 0.5f;
    }
    return s;
}

/* nomal_move (inlined into daSsk_Execute) */
static inline void nomal_move(ssk_class* i_this) {
    switch ((u16)i_this->m2BA) {
    case SSK_WAIT:
        if (fopAcM_searchPlayerDistance(i_this) < i_this->m2C0) {
            /* HD: the collider is switched on through mColliderActive; random start angles */
            i_this->mColliderActive = 1;
            i_this->current.angle.y = (s16)gabi::ftoi(cM_rndFX(32768.0f));
            i_this->m2F0 = (s16)gabi::ftoi(cM_rndFX(32768.0f));
            i_this->m2BA = SSK_ROCK_APPEAR;
        }
        break;

    case SSK_ROCK_APPEAR:
        cLib_addCalc2(&i_this->m350.x, 1.0f, 1.0f, 0.3f);
        {
            f32 x = i_this->m350.x;
            i_this->m350.y = x;
            i_this->m350.z = x;
            if (x > 0.99f) {
                i_this->m350.x = 1.0f;
                i_this->m350.y = 1.0f;
                i_this->mWobbleAmplitude = 0.0f;
                i_this->m350.z = 1.0f;
                fopAcM_seStart(i_this, JA_SE_OBJ_JAMA_SHOKU_OUT, 0);
                i_this->m2BA = SSK_RISE;

                dComIfGp_particle_set(ID_IT_SN_TGSYOKU_ROCK00, &i_this->current.pos, nullptr,
                                      nullptr, 0xff, nullptr, -1, &i_this->tevStr.mColorK0,
                                      &i_this->tevStr.mColorK0);
                s8 room = fopAcM_GetRoomNo(i_this);
                particle_setToon(ID_IT_ST_TGSYOKU_SMOKE00, &i_this->current.pos,
                                 &i_this->shape_angle, nullptr, 0xb9, &i_this->m2D0, room);
            }
        }
        break;

    case SSK_RISE:
        cLib_addCalc2(&i_this->scale.y, 1.0f, 1.0f, 0.2f);
        if (i_this->scale.y > 0.99f) {
            i_this->scale.y = 1.0f;
            i_this->m2BA = SSK_ACTIVE; /* HD: no m2F2 */
        }
        break;

    case SSK_RETRACT:
        cLib_addCalc0(&i_this->scale.y, 1.0f, 0.05f);
        cLib_addCalc2(&i_this->mWobbleAmplitude, 0.2f, 1.0f, 0.05f);
        cLib_addCalc0(&i_this->m350.x, 1.0f, 0.04f);
        {
            f32 x = i_this->m350.x;
            i_this->m350.y = x;
            i_this->m350.z = x;
            if (x < 0.01f) {
                i_this->m350.x = 0.0f;
                i_this->m350.y = 0.0f;
                i_this->m350.z = 0.0f;
                i_this->scale.y = 0.0f;
                i_this->m2BA = SSK_WAIT;
                if (i_this->m2B6 != 0xff &&
                    dComIfGs_isSwitch(i_this->m2B6, dComIfGp_roomControl_getStayNo())) {
                    fopAcM_delete(i_this);
                }
            }
        }
        break;

    case SSK_ACTIVE:
        cLib_addCalc2(&i_this->mWobbleAmplitude, 1.0f, 1.0f, 0.1f);
        if ((i_this->m2B4 == 1 && fopAcM_searchPlayerDistance(i_this) > i_this->m2C4) ||
            (i_this->m2B6 != 0xff &&
             dComIfGs_isSwitch(i_this->m2B6, dComIfGp_roomControl_getStayNo()))) {
            /* HD: both ways retract with sound and smoke */
            i_this->mColliderActive = 0;
            i_this->m2BA = SSK_RETRACT;
            fopAcM_seStart(i_this, JA_SE_OBJ_JAMA_SHOKU_IN, 0);
            cXyz* scl = smoke_scale();
            s8 room = fopAcM_GetRoomNo(i_this);
            particle_setToon(ID_IT_ST_TGSYOKU_SMOKE00, &i_this->current.pos, &i_this->shape_angle,
                             scl, 0xb9, &i_this->m2D0, room);
        }
        break;
    }

    /* HD: every state from 2 on wobbles; the amplitude grows with the joint index and
     * mWobbleAmplitude */
    if (i_this->m2BA >= SSK_RISE) {
        i_this->m2F0 = (s16)(i_this->m2F0 + 1);

        for (s32 i = 1; i < 5; i++) {
            f32 amp = gabi::fmadds((f32)i * 1000.0f, i_this->mWobbleAmplitude, 500.0f);
            i_this->m2F4[i].y = (s16)gabi::ftoi(cM_ssin(i_this->m2F0 * 2000 - i * 14000) * amp);
            i_this->m2F4[i].z = (s16)gabi::ftoi(cM_ssin(i_this->m2F0 * 1500 - i * 14000) * amp);
        }

        s16 angle = (s16)cLib_distanceAngleS(i_this->m2F4[1].z, 0x1000);
        if (angle < 0x1000) {
            if (!i_this->m2B7) {
                fopAcM_seStart(i_this, JA_SE_OBJ_SHOKU_LIFT_MOVE, 0);
                i_this->m2B7 = true;
            }
        } else {
            i_this->m2B7 = false;
        }
    }
}

/* body_atari_check (inlined into daSsk_Execute) */
static inline void body_atari_check(ssk_class* i_this) {
    if (i_this->m2BC > 0) {
        i_this->m2BC = (s16)(i_this->m2BC - 1);
        return;
    }

    if (!i_this->mCyl.ChkTgHit()) {
        return;
    }

    dCcD_GObjInf* hitObj = static_cast<dCcD_GObjInf*>(i_this->mCyl.GetTgHitObj());
    if (hitObj == nullptr) {
        return;
    }

    i_this->m2BC = 8;

    switch ((u32)hitObj->mAtType) {
    case AT_TYPE_SWORD:
    case AT_TYPE_MACHETE:
    case AT_TYPE_UNK800:
    case AT_TYPE_DARKNUT_SWORD:
    case AT_TYPE_MOBLIN_SPEAR:
    case AT_TYPE_PGANON_SWORD:
        fopAcM_seStart(i_this, JA_SE_LK_SW_HIT_S, 0x44);
        break;

    case AT_TYPE_BOOMERANG:
    case AT_TYPE_BOKO_STICK:
    case AT_TYPE_UNK2000:
    case AT_TYPE_STALFOS_MACE:
        fopAcM_seStart(i_this, JA_SE_LK_W_WEP_HIT, 0x44);
        break;

    case AT_TYPE_SKULL_HAMMER:
        fopAcM_seStart(i_this, JA_SE_LK_HAMMER_HIT, 0x44);
        break;

    case AT_TYPE_NORMAL_ARROW:
    case AT_TYPE_FIRE_ARROW:
    case AT_TYPE_ICE_ARROW:
    case AT_TYPE_LIGHT_ARROW:
    case AT_TYPE_GRAPPLING_HOOK:
        fopAcM_seStart(i_this, JA_SE_LK_MS_WEP_HIT, 0x44);
        break;
    }
}

/* 0248DC20 */
static BOOL daSsk_Execute(ssk_class* i_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    switch (i_this->m2B8) {
    case 0:
        nomal_move(i_this);
        break;
    }

    /* HD: the collider is set only while active; no SetH/SetR */
    if (i_this->mColliderActive) {
        i_this->mCyl.SetC(&i_this->current.pos);
        dComIfG_Ccsp_Set(&i_this->mCyl);
    }
    body_atari_check(i_this);
    i_this->m2C4 = i_this->m2C0 + 100.0f;
    draw_sub(i_this);
    return TRUE;
}

/* 0248E4FC */
static BOOL daSsk_IsDelete(ssk_class*) {
    gabi::AutoFrame frame{gabi::current_function()};
    return TRUE;
}

/* 0248E504 */
static BOOL daSsk_Delete(ssk_class* i_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    dComIfG_resDelete(&i_this->mPhase, "Ssk"); /* dComIfG_resDeleteDemo */
    i_this->m2D0.remove();
    return TRUE;
}

// HD: install the joint callbacks during heap initialization.
/* 0248E554 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    ssk_class* i_this = (ssk_class*)a_this;

    J3DModelData* md =
        (J3DModelData*)dComIfG_getObjectRes("Ssk", dRes_INDEX_SSK_BDL_TURU_02_e, gc_tu.safeString);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, md, nullptr, nullptr, nullptr, -1, 1.0f,
                                                  0, -1, 1, nullptr, 0x80000, 0x11000022);
    i_this->mpMorf1 = morf;

    if (morf == nullptr || morf->getModel() == nullptr) {
        return FALSE;
    }

    for (u16 i = 0; i < morf->getModel()->getModelData()->getJointNum(); i++) {
        i_this->mpMorf1->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(
            nodeCallBack);
        morf = i_this->mpMorf1;
    }

    md =
        (J3DModelData*)dComIfG_getObjectRes("Ssk", dRes_INDEX_SSK_BDL_KTANA_00_e, gc_tu.safeString);
    morf = mDoExt_McaMorf::create(nullptr, md, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1,
                                  nullptr, 0x80000, 0x11000022);
    i_this->mpMorf2 = morf;
    if (morf == nullptr || morf->getModel() == nullptr) {
        return FALSE;
    }

    i_this->mpMorf1->getModel()->setUserArea((uintptr_t)i_this);

    JntHit_c* jntHit = sskCreateJntHit(i_this->mpMorf1->getModel(), search_data, 3);
    i_this->m708 = jntHit;
    if (jntHit != nullptr) {
        i_this->jntHit = gabi::ea(jntHit);
    } else {
        return FALSE;
    }

    return TRUE;
}

/* 0248E770 */
static cPhs_State daSsk_Create(fopAc_ac_c* a_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    ssk_class* i_this = (ssk_class*)a_this;

    /* fopAcM_ct(a_this, ssk_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = ssk_class::__vt;
            sskConstructSmoke(&i_this->m2D0, 1);
            dBgS_AcchCir_ct(&i_this->mAcchCir);
            dBgS_ObjAcch_ct(&i_this->mAcch, gc_tu.objAcch);
            dCcD_Stts_ct(&i_this->mStts);
            dCcD_Cyl_ct(&i_this->mCyl, ssk_class::__aabVt);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State PVar2 = dComIfG_resLoad(&i_this->mPhase, "Ssk");
    if (PVar2 == cPhs_COMPLEATE_e) {
        i_this->m2D0.m11 = 0;
        u32 prm = fopAcM_GetParam(a_this);
        i_this->m2B4 = (u8)prm;
        i_this->m2B5 = (u8)(prm >> 8);
        i_this->m2B6 = (u8)(prm >> 0x18);

        if (i_this->m2B5 == 0xff) {
            i_this->m2B5 = 0;
        }

        if (i_this->m2B6 != 0xff &&
            dComIfGs_isSwitch(i_this->m2B6, dComIfGp_roomControl_getStayNo())) {
            return cPhs_ERROR_e;
        }

        f32 r = (f32)i_this->m2B5 * 10.0f;
        if (r == 0.0f) {
            r = 400.0f;
        }
        i_this->m2C0 = r;

        if (!fopAcM_entrySolidHeap(a_this, useHeapInit, 0x1300)) {
            return cPhs_ERROR_e;
        }

        J3DModel* model = i_this->mpMorf2->getModel();
        fopAcM_SetMtx(a_this, model->getBaseTRMtx());
        sskSetMin(a_this, -800.0f, -200.0f, -400.0f);
        sskSetMax(a_this, 500.0f, 1000.0f, 1000.0f);
        a_this->attention_info.flags = 0;
        i_this->mCyl.Set(reinterpret_cast<dCcD_SrcCyl*>(&body_co_cyl));
        i_this->mCyl.SetStts(&i_this->mStts);
        i_this->mStts.Init(0xfe, 200, a_this);
        i_this->m2B8 = 0;

        if (i_this->m2B4 == 0xff) {
            /* HD: mColliderActive instead of the At bits; random start angles instead of m2F2 */
            i_this->mColliderActive = 1;
            a_this->scale.set(1.0f, 1.0f, 1.0f);
            i_this->m350.set(1.0f, 1.0f, 1.0f);
            a_this->current.angle.y = (s16)gabi::ftoi(cM_rndFX(32768.0f));
            i_this->m2F0 = (s16)gabi::ftoi(cM_rndFX(32768.0f));
            i_this->m2BA = SSK_ACTIVE;
        } else {
            a_this->scale.set(0.0f, 0.0f, 0.0f);
            i_this->m350.set(0.0f, 0.0f, 0.0f);
        }

        draw_sub(i_this);
    }
    return PVar2;
}

// HD: compiler-generated and template code of this translation unit.
/* 0248EAF4 */
static void __sinit_d_a_ssk_cpp() {
    gabi::AutoFrame frame{gabi::current_function()};
    for (int i = 0; i < 4; i++)
        headerStatics.mZero[i] = 0;
    __register_global_object(gabi::ea(&firstRegistration));
    headerStatics.mNegativePi = -3.1415927f;
    headerStatics.mPositivePi = 3.1415927f;
    sskConstructFirstHeaderObject(&headerStatics.mFirstObject);
    __register_global_object(gabi::ea(&secondRegistration));
    sskConstructSecondHeaderObject(&headerStatics.mSecondObject);
    __register_global_object(gabi::ea(&thirdRegistration));
}

/* 0248EB88 */
static void trivial_dt(void* p, s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 0248EB9C */
static void ssk_class_dt(ssk_class* i_this, s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        i_this->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
        i_this->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
        sskDestroyAcch(&i_this->mAcch, 0);            /* dBgS_Acch::~dBgS_Acch */
        sskDestroyCircle(&i_this->mAcchCir.m_cir, 2); /* mAcchCir's cM3dGCir */
        sskDestroyActor(i_this, 0);                   /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0248EC38 */
static void empty_virtual(void* p) { gabi::AutoFrame frame{gabi::current_function()}; }

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data and bindings
str 1003D384 "Ssk" in 0248E770
str 1003D35C "Ssk" in 0248E504
str 1003D360 "Ssk" in 0248E554
data body_co_cyl 101D0C90
data search_data 101D0C60
data headerStatics 1046DF80
data smokeScale 1046DFA0
data firstRegistration 101D0CD4
data secondRegistration 101D0CE0
data thirdRegistration 101D0CEC
ext 025D672C void sskSetMin(fopAc_ac_c*, f32, f32, f32)
ext 025D673C void sskSetMax(fopAc_ac_c*, f32, f32, f32)
ext 02552B60 JntHit_c* sskCreateJntHit(J3DModel*, __jnt_hit_data_c*, s16)
ext 025A5B18 void sskConstructSmoke(dPa_smokeEcallBack*, u8)
ext 028ED6F8 void sskConstructFirstHeaderObject(void*)
ext 028EAB2C void sskConstructSecondHeaderObject(void*)
ext 024EFD9C void sskDestroyAcch(dBgS_Acch*, s32)
ext 02018034 void sskDestroyCircle(cM3dGCir_l*, s32)
ext 025D50BC void sskDestroyActor(fopAc_ac_c*, s32)
*/
