/**
 * d_a_ssk.cpp (WWHD)
 * Object - Tingle-tower style stone tentacle ("Ssk": rock that rises when the player is near).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ssk.cpp) and, where HD changed it, written from the WWHD code;
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * HD changes (summary): m2F2 (wobble direction) and m314[5] (joint positions) are gone; the rock
 * fades in/out through a new amplitude m7E4 and an "active" flag m7E0 that gates the collider;
 * the tentacle model is shown by a translation (scale.y * 320 - 320) instead of a base scale;
 * the joint callbacks are installed once in useHeapInit; case 10 always plays the retract sound
 * and smoke. */
#include "bindings.h"

#define M_arcname_create STR(0x1003D384) /* "Ssk" (resLoad) */
#define M_arcname_delete STR(0x1003D35C) /* "Ssk" (resDelete) */
#define M_arcname_res STR(0x1003D360)    /* "Ssk" (getRes) */
#define SAFESTRING_VTBL 0x1003D2AC
#define ACT_VTBL 0x1003D304    /* HD: ssk_class vtable */
#define TU_AAB_VTBL 0x1003D2C4 /* this TU's cM3dGAab vtable */
static const dBgS_ObjAcch_vt OBJACCH_VT = {0x1003D2D4, 0x1003D2F4, 0x1003D2E4};
#define body_co_cyl gabi::at<dCcD_SrcCyl>(0x101D0C90)
#define search_data 0x101D0C60 /* __jnt_hit_data_c[3] */

enum {
    dRes_INDEX_SSK_BDL_KTANA_00_e = 3,
    dRes_INDEX_SSK_BDL_TURU_02_e = 4,
};
enum {
    JA_SE_OBJ_JAMA_SHOKU_OUT = 0x694B,
    JA_SE_OBJ_JAMA_SHOKU_IN = 0x694D,
    JA_SE_OBJ_SHOKU_LIFT_MOVE = 0x3823,
    JA_SE_LK_SW_HIT_S = 0x2803,
    JA_SE_LK_W_WEP_HIT = 0x2833,
    JA_SE_LK_MS_WEP_HIT = 0x2834,
    JA_SE_LK_HAMMER_HIT = 0x2855,
    ID_IT_SN_TGSYOKU_ROCK00 = 0x816C,
    ID_IT_ST_TGSYOKU_SMOKE00 = 0xA16D,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD: fopAcM_SetMin / fopAcM_SetMax are out of line (025D672C / 025D673C), as d_a_bita/mo2 */
static inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
/* dStage_roomControl_c::mStayNo (s8 at 0x1047E6C8), as d_a_pt / d_a_cc */
static inline s32 dComIfGp_roomControl_getStayNo() { return gabi::load<s8>(0x1047E6C8); }
/* 027F3F94 (matcher: __nw): J3DModelData's joint tree; joint count (u16) at +8 (as d_a_kb.h) */
static inline u16 J3DModelData_getJointNum(u32 data) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, data) + 8); }
static inline u32 JntHit_create(J3DModel* m, u32 data, s16 num) { return gabi::call<u32>(0x02552B60, m, data, num); }
/* 025A5B18 dPa_smokeEcallBack::dPa_smokeEcallBack(u8) (as d_a_npc_aj1); remove() is virtual (+0x44) */
static inline void dPa_smokeEcallBack_ct(void* cb, u8 a) { gabi::call(0x025A5B18, cb, a); }
static inline void dPa_smokeEcallBack_remove(void* cb) { gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(cb)) + 0x44), cb); }
/* HD J3D (as d_a_kamome): j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; joint
 * matrices in a block at model+0x2C (+0x4 flags, +0x10 matrices) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_l {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_l> mpMtxBlock;
    /* 0x30 */ u8 _30[0xAC - 0x30];
    /* 0xAC */ be<u32> mModelData;
    /* 0xB0 */ u8 _B0[8];
    /* 0xB8 */ be<u32> mUserArea;
};
static Mtx34* getAnmMtx(J3DModel_l* model, s32 jntNo) {
    J3DMtxBlock_l* blk = model->mpMtxBlock;
    blk->mFlags |= 0x10; /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}

struct ssk_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ be<u8> m2B4;
    /* 0x3D1 */ be<u8> m2B5;
    /* 0x3D2 */ be<u8> m2B6;
    /* 0x3D3 */ be<u8> m2B7;
    /* 0x3D4 */ be<s16> m2B8;
    /* 0x3D6 */ be<s16> m2BA;
    /* 0x3D8 */ be<s16> m2BC;
    /* 0x3DA */ u8 _3DA[2];
    /* 0x3DC */ be<f32> m2C0;
    /* 0x3E0 */ be<f32> m2C4;
    /* 0x3E4 */ gptr<mDoExt_McaMorf> mpMorf1;
    /* 0x3E8 */ gptr<mDoExt_McaMorf> mpMorf2;
    /* 0x3EC */ u8 m2D0[0x20]; /* dPa_smokeEcallBack (vtable at +0) */
    /* 0x40C */ be<s16> m2F0;
    /* 0x40E */ csXyz m2F4[5]; /* HD: m2F2 removed */
    /* 0x42C */ cXyz m350;     /* HD: m314[5] removed */
    /* 0x438 */ u8 m35C[0x470 - 0x438];
    /* 0x470 */ u8 mAcchCir[0x40]; /* dBgS_AcchCir */
    /* 0x4B0 */ u8 mAcch[0x1C4];   /* dBgS_ObjAcch */
    /* 0x674 */ dCcD_Stts mStts;
    /* 0x6B0 */ dCcD_Cyl mCyl;
    /* 0x7E0 */ be<u8> m7E0;       /* HD: collider active */
    /* 0x7E1 */ u8 _7E1[3];
    /* 0x7E4 */ be<f32> m7E4;      /* HD: wobble amplitude */
    /* 0x7E8 */ be<u32> m708;      /* JntHit_c* */
};
WWHD_OFFSET(ssk_class, m2B4, 0x3D0);
WWHD_OFFSET(ssk_class, m2F0, 0x40C);
WWHD_OFFSET(ssk_class, m350, 0x42C);
WWHD_OFFSET(ssk_class, mStts, 0x674);
WWHD_OFFSET(ssk_class, mCyl, 0x6B0);
WWHD_OFFSET(ssk_class, m708, 0x7E8);
WWHD_SIZE(ssk_class, 0x7EC);

/* 0248D880. HD: no MtxPosition into m314 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0248D880, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel_l* model = gabi::at<J3DModel_l>(gabi::load<u32>(0x104B462C));
        ssk_class* i_this = gabi::at<ssk_class>(model->mUserArea);
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);

        if (i_this != nullptr && jntNo < 5) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());

            cMtx_YrotM(calc_mtx(), i_this->m2F4[jntNo].y);
            cMtx_XrotM(calc_mtx(), i_this->m2F4[jntNo].x);
            cMtx_ZrotM(calc_mtx(), i_this->m2F4[jntNo].z);

            Mtx34* dst = getAnmMtx(model, jntNo); /* model->setAnmMtx(jntNo, *calc_mtx) */
            mtx_copy(dst, calc_mtx());
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868)); /* J3DSys::mCurrentMtx */
        }
    }
    return TRUE;
}
VERIFY(0x0248D880, nodeCallBack);

/* 0248D9B4. HD: the callbacks are set in useHeapInit; the rock (mpMorf2) is drawn at m350 scale only
 * while visible, the tentacle (mpMorf1) is raised by scale.y instead of scaled */
static void draw_sub(ssk_class* i_this) {
    WWHD_FUNC(0x0248D9B4, void, i_this);
    J3DModel* pJVar4 = i_this->mpMorf1->getModel();
    J3DModel* pJVar3 = i_this->mpMorf2->getModel();

    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->current.angle.y);

    if (i_this->m350.x > 0.01f) {
        J3DModel_setBaseScale(pJVar3, &i_this->m350);
        J3DModel_setBaseTRMtx(pJVar3, mDoMtx_stack_c::get());
    }
    f32 sy = i_this->scale.y;
    if (sy > 0.01f) {
        mDoMtx_stack_c::transM(0.0f, gabi::fmadds(sy, 320.0f, -320.0f), 0.0f);
        J3DModel_setBaseTRMtx(pJVar4, mDoMtx_stack_c::get());
    }

    i_this->mpMorf1->calc();
}
VERIFY(0x0248D9B4, draw_sub);

/* 0248DB4C. HD: each model is drawn only while visible */
static BOOL daSsk_Draw(ssk_class* i_this) {
    WWHD_FUNC(0x0248DB4C, BOOL, i_this);
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
VERIFY(0x0248DB4C, daSsk_Draw);

/* dComIfGp_particle_setToon (inline): dPa_control_c::set on group 2 */
static inline void particle_setToon(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha,
                                    void* cb, s8 setup) {
    dPa_control_c* pa = dComIfGp_getParticle();
    dPa_control_set(pa, 2, id, pos, angle, scale, alpha, (dPa_levelEcallBack*)cb, setup, nullptr, nullptr, nullptr);
}

/* function-local static cXyz (0.5, 0.5, 0.5), initialised on first use (guard 0x1046DF88) */
static cXyz* smoke_scale() {
    cXyz* s = gabi::at<cXyz>(0x1046DFA0);
    if (gabi::load<u32>(0x1046DF88) == 0) {
        gabi::store<u32>(0x1046DF88, 1);
        s->x = 0.5f;
        s->y = 0.5f;
        s->z = 0.5f;
    }
    return s;
}

/* nomal_move (inlined into daSsk_Execute) */
static inline void nomal_move(ssk_class* i_this) {
    switch ((u16)i_this->m2BA) {
    case 0:
        if (fopAcM_searchPlayerDistance(i_this) < i_this->m2C0) {
            /* HD: the collider is switched on through m7E0; random start angles */
            i_this->m7E0 = 1;
            i_this->current.angle.y = (s16)gabi::ftoi(cM_rndFX(32768.0f));
            i_this->m2F0 = (s16)gabi::ftoi(cM_rndFX(32768.0f));
            i_this->m2BA = 1;
        }
        break;

    case 1:
        cLib_addCalc2(&i_this->m350.x, 1.0f, 1.0f, 0.3f);
        {
            f32 x = i_this->m350.x;
            i_this->m350.y = x;
            i_this->m350.z = x;
            if (x > 0.99f) {
                i_this->m350.x = 1.0f;
                i_this->m350.y = 1.0f;
                i_this->m7E4 = 0.0f;
                i_this->m350.z = 1.0f;
                fopAcM_seStart(i_this, JA_SE_OBJ_JAMA_SHOKU_OUT, 0);
                i_this->m2BA = 2;

                dComIfGp_particle_set(ID_IT_SN_TGSYOKU_ROCK00, &i_this->current.pos, nullptr, nullptr, 0xff, nullptr, -1,
                                      gabi::at<GXColor>(gabi::ea(i_this) + 0x1A8) /* tevStr.mColorK0 */,
                                      gabi::at<GXColor>(gabi::ea(i_this) + 0x1A8));
                s8 room = fopAcM_GetRoomNo(i_this);
                particle_setToon(ID_IT_ST_TGSYOKU_SMOKE00, &i_this->current.pos, &i_this->shape_angle, nullptr, 0xb9,
                                 i_this->m2D0, room);
            }
        }
        break;

    case 2:
        cLib_addCalc2(&i_this->scale.y, 1.0f, 1.0f, 0.2f);
        if (i_this->scale.y > 0.99f) {
            i_this->scale.y = 1.0f;
            i_this->m2BA = 10; /* HD: no m2F2 */
        }
        break;

    case 3:
        cLib_addCalc0(&i_this->scale.y, 1.0f, 0.05f);
        cLib_addCalc2(&i_this->m7E4, 0.2f, 1.0f, 0.05f);
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
                i_this->m2BA = 0;
                if (i_this->m2B6 != 0xff && dComIfGs_isSwitch(i_this->m2B6, dComIfGp_roomControl_getStayNo())) {
                    fopAcM_delete(i_this);
                }
            }
        }
        break;

    case 10:
        cLib_addCalc2(&i_this->m7E4, 1.0f, 1.0f, 0.1f);
        if ((i_this->m2B4 == 1 && fopAcM_searchPlayerDistance(i_this) > i_this->m2C4) ||
            (i_this->m2B6 != 0xff && dComIfGs_isSwitch(i_this->m2B6, dComIfGp_roomControl_getStayNo()))) {
            /* HD: both ways retract with sound and smoke */
            i_this->m7E0 = 0;
            i_this->m2BA = 3;
            fopAcM_seStart(i_this, JA_SE_OBJ_JAMA_SHOKU_IN, 0);
            cXyz* scl = smoke_scale();
            s8 room = fopAcM_GetRoomNo(i_this);
            particle_setToon(ID_IT_ST_TGSYOKU_SMOKE00, &i_this->current.pos, &i_this->shape_angle, scl, 0xb9,
                             i_this->m2D0, room);
        }
        break;
    }

    /* HD: every state from 2 on wobbles; the amplitude grows with the joint index and m7E4 */
    if (i_this->m2BA >= 2) {
        i_this->m2F0 = (s16)(i_this->m2F0 + 1);

        for (s32 i = 1; i < 5; i++) {
            f32 amp = gabi::fmadds((f32)i * 1000.0f, i_this->m7E4, 500.0f);
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

    void* hitObj = i_this->mCyl.GetTgHitObj();
    if (hitObj == nullptr) {
        return;
    }

    i_this->m2BC = 8;

    switch (gabi::load<u32>(gabi::ea(hitObj) + 0x10) /* GetAtType() */) {
    case 0x2:
    case 0x400:
    case 0x800:
    case 0x4000000:
    case 0x10000000:
    case 0x20000000:
        fopAcM_seStart(i_this, JA_SE_LK_SW_HIT_S, 0x44);
        break;

    case 0x40:
    case 0x80:
    case 0x2000:
    case 0x1000000:
        fopAcM_seStart(i_this, JA_SE_LK_W_WEP_HIT, 0x44);
        break;

    case 0x10000:
        fopAcM_seStart(i_this, JA_SE_LK_HAMMER_HIT, 0x44);
        break;

    case 0x4000:
    case 0x40000:
    case 0x80000:
    case 0x100000:
    case 0x8000000:
        fopAcM_seStart(i_this, JA_SE_LK_MS_WEP_HIT, 0x44);
        break;
    }
}

/* 0248DC20 */
static BOOL daSsk_Execute(ssk_class* i_this) {
    WWHD_FUNC(0x0248DC20, BOOL, i_this);
    switch (i_this->m2B8) {
    case 0:
        nomal_move(i_this);
        break;
    }

    /* HD: the collider is set only while active; no SetH/SetR */
    if (i_this->m7E0) {
        i_this->mCyl.SetC(&i_this->current.pos);
        dComIfG_Ccsp_Set(&i_this->mCyl);
    }
    body_atari_check(i_this);
    i_this->m2C4 = i_this->m2C0 + 100.0f;
    draw_sub(i_this);
    return TRUE;
}
VERIFY(0x0248DC20, daSsk_Execute);

/* 0248E4FC */
static BOOL daSsk_IsDelete(ssk_class*) {
    WWHD_FUNC(0x0248E4FC, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0248E4FC, daSsk_IsDelete);

/* 0248E504 */
static BOOL daSsk_Delete(ssk_class* i_this) {
    WWHD_FUNC(0x0248E504, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, M_arcname_delete); /* dComIfG_resDeleteDemo */
    dPa_smokeEcallBack_remove(i_this->m2D0);
    return TRUE;
}
VERIFY(0x0248E504, daSsk_Delete);

/* 0248E554. HD: the joint callbacks are installed here (GameCube: in draw_sub) */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x0248E554, BOOL, a_this);
    ssk_class* i_this = (ssk_class*)a_this;

    J3DModelData* md = (J3DModelData*)dComIfG_getObjectRes(M_arcname_res, dRes_INDEX_SSK_BDL_TURU_02_e, SAFESTRING_VTBL);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, md, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1, nullptr,
                                                  0x80000, 0x11000022);
    i_this->mpMorf1 = morf;

    if (morf == nullptr || morf->getModel() == nullptr) {
        return FALSE;
    }

    for (u16 i = 0; i < J3DModelData_getJointNum(gabi::load<u32>(gabi::ea(morf->getModel()) + 0xAC)); i++) {
        /* getJointNodePointer(i)->setCallBack(nodeCallBack) */
        u32 data = gabi::load<u32>(gabi::ea(i_this->mpMorf1->getModel()) + 0xAC);
        u32 n = gabi::load<u32>(data + 4);
        u32 p = gabi::load<u32>(data + 8);
        if (i < n)
            p += i * 0x1C;
        gabi::store<u32>(p + 8, 0x0248D880);
        morf = i_this->mpMorf1;
    }

    md = (J3DModelData*)dComIfG_getObjectRes(M_arcname_res, dRes_INDEX_SSK_BDL_KTANA_00_e, SAFESTRING_VTBL);
    morf = mDoExt_McaMorf::create(nullptr, md, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11000022);
    i_this->mpMorf2 = morf;
    if (morf == nullptr || morf->getModel() == nullptr) {
        return FALSE;
    }

    gabi::store<u32>(gabi::ea(i_this->mpMorf1->getModel()) + 0xB8, gabi::ea(i_this)); /* setUserArea */

    u32 jntHit = JntHit_create(i_this->mpMorf1->getModel(), search_data, 3);
    i_this->m708 = jntHit;
    if (jntHit != 0) {
        i_this->jntHit = jntHit;
    } else {
        return FALSE;
    }

    return TRUE;
}
VERIFY(0x0248E554, useHeapInit);

/* 0248E770 */
static cPhs_State daSsk_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x0248E770, cPhs_State, a_this);
    ssk_class* i_this = (ssk_class*)a_this;

    /* fopAcM_ct(a_this, ssk_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = ACT_VTBL;
            dPa_smokeEcallBack_ct(i_this->m2D0, 1);
            dBgS_AcchCir_ct((dBgS_AcchCir*)i_this->mAcchCir);
            dBgS_ObjAcch_ct((dBgS_Acch*)i_this->mAcch, OBJACCH_VT);
            dCcD_Stts_ct(&i_this->mStts);
            dCcD_Cyl_ct(&i_this->mCyl, TU_AAB_VTBL);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State PVar2 = dComIfG_resLoad(&i_this->mPhase, M_arcname_create);
    if (PVar2 == cPhs_COMPLEATE_e) {
        gabi::store<u8>(gabi::ea(i_this->m2D0) + 0x11, 0); /* m2D0.setRateOff(0) */
        u32 prm = fopAcM_GetParam(a_this);
        i_this->m2B4 = (u8)prm;
        i_this->m2B5 = (u8)(prm >> 8);
        i_this->m2B6 = (u8)(prm >> 0x18);

        if (i_this->m2B5 == 0xff) {
            i_this->m2B5 = 0;
        }

        if (i_this->m2B6 != 0xff && dComIfGs_isSwitch(i_this->m2B6, dComIfGp_roomControl_getStayNo())) {
            return cPhs_ERROR_e;
        }

        f32 r = (f32)i_this->m2B5 * 10.0f;
        if (r == 0.0f) {
            r = 400.0f;
        }
        i_this->m2C0 = r;

        if (!fopAcM_entrySolidHeap(a_this, 0x0248E554 /* useHeapInit */, 0x1300)) {
            return cPhs_ERROR_e;
        }

        J3DModel* model = i_this->mpMorf2->getModel();
        a_this->cullMtx = model != nullptr ? gabi::ea(model) + 0xC8 : 0; /* fopAcM_SetMtx(getBaseTRMtx()) */
        fopAcM_SetMin(a_this, -800.0f, -200.0f, -400.0f);
        fopAcM_SetMax(a_this, 500.0f, 1000.0f, 1000.0f);
        gabi::store<u32>(gabi::ea(a_this) + 0x39C, 0); /* attention_info.flags = 0 */
        i_this->mCyl.Set(body_co_cyl);
        i_this->mCyl.SetStts(&i_this->mStts);
        i_this->mStts.Init(0xfe, 200, a_this);
        i_this->m2B8 = 0;

        if (i_this->m2B4 == 0xff) {
            /* HD: m7E0 instead of the At bits; random start angles instead of m2F2 */
            i_this->m7E0 = 1;
            a_this->scale.set(1.0f, 1.0f, 1.0f);
            i_this->m350.set(1.0f, 1.0f, 1.0f);
            a_this->current.angle.y = (s16)gabi::ftoi(cM_rndFX(32768.0f));
            i_this->m2F0 = (s16)gabi::ftoi(cM_rndFX(32768.0f));
            i_this->m2BA = 10;
        } else {
            a_this->scale.set(0.0f, 0.0f, 0.0f);
            i_this->m350.set(0.0f, 0.0f, 0.0f);
        }

        draw_sub(i_this);
    }
    return PVar2;
}
VERIFY(0x0248E770, daSsk_Create);

/* 0248EAF4: the header statics, in this TU at {-pi, pi} P, objects P+0xC / P+0xD, zeroed 16 bytes P+0x10 */
static void __sinit_d_a_ssk_cpp() {
    WWHD_FUNC(0x0248EAF4, void, (u32)0);
    const u32 P = 0x1046DF80, D = 0x101D0CD4;
    for (int i = 0; i < 4; i++) gabi::store<u32>(P + 0x10 + 4 * i, 0);
    __register_global_object(D);
    gabi::store<f32>(P, -3.1415927f);
    gabi::store<f32>(P + 4, 3.1415927f);
    gabi::call(0x028ED6F8, P + 0xC);
    __register_global_object(D + 0xC);
    gabi::call(0x028EAB2C, P + 0xD);
    __register_global_object(D + 0x18);
}
VERIFY(0x0248EAF4, __sinit_d_a_ssk_cpp);

/* 0248EB88: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0248EB88, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0248EB88, trivial_dt);

/* 0248EB9C: ssk_class deleting destructor (inline member destructors) */
static void ssk_class_dt(ssk_class* i_this, s32 flags) {
    WWHD_FUNC(0x0248EB9C, void, i_this, flags);
    if (i_this != nullptr) {
        u32 b = gabi::ea(i_this);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::store<u32>(b + 0x4D0, OBJACCH_VT.v20);
        gabi::store<u32>(b + 0x4C4, OBJACCH_VT.v14);
        gabi::call(0x024EFD9C, i_this->mAcch, 0);        /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x02018034, gabi::at<u8>(b + 0x484), 2); /* mAcchCir's cM3dGCir */
        gabi::call(0x025D50BC, i_this, 0);               /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0248EB9C, ssk_class_dt);

/* 0248EC38: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x0248EC38, void, p);
}
VERIFY(0x0248EC38, empty_virtual);
