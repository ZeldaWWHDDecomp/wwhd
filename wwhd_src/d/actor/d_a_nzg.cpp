/**
 * d_a_nzg.cpp (WWHD)
 * Rat hole: spawns rats (NZ) when the player is near, and the rat shop keeper (NpcNz).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_nzg.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define NZG_VTBL 0x10024598        /* nzg_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x10024570 /* this TU's sead::SafeString vtable */
#define CM3DGAAB_VTBL 0x10024588   /* this TU's cM3dGAab vtable */
#define body_cyl_src gabi::at<dCcD_SrcCyl>(0x101C7830)

enum { dRes_INDEX_NZG_BDL_KANA_00_e = 3 };
enum : s16 { fpcNm_NZ_e = 0xC6, fpcNm_BOMB_e = 0x126, fpcNm_Bomb2_e = 0x127, fpcNm_ITEM_e = 0xFF, fpcNm_ESA_e = 0xDD };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025D5A20 fopAcM_createChild(s16 name, parent, param, pos, roomNo, angle, scale, s8 subtype, createFunc) */
static inline u32 fopAcM_createChild(s16 name, u32 parent, u32 param, cXyz* pos, s32 roomNo, csXyz* angle, cXyz* scale, s8 subtype,
                                     u32 createFunc) {
    return gabi::call<u32>(0x025D5A20, name, parent, param, pos, roomNo, angle, scale, subtype, createFunc);
}
/* 025D5AA4 fopAcM_createChild(const char* name, parent, param, pos, roomNo, angle, scale, createFunc) */
static inline u32 fopAcM_createChild(const char* name, u32 parent, u32 param, cXyz* pos, s32 roomNo, csXyz* angle, cXyz* scale,
                                     u32 createFunc) {
    return gabi::call<u32>(0x025D5AA4, name, parent, param, pos, roomNo, angle, scale, createFunc);
}
static inline f32 REG8_F(int i) { return REG_F(8, i); }

struct nzg_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ gptr<J3DModel> mpModel;
    /* 0x3D4 */ be<u8> m2B8;
    /* 0x3D5 */ be<u8> m2B9;
    /* 0x3D6 */ be<u8> m2BA;
    /* 0x3D7 */ be<u8> m2BB;
    /* 0x3D8 */ u8 m2BC[2];
    /* 0x3DA */ be<s16> m2BE;
    /* 0x3DC */ be<s16> m2C0;
    /* 0x3DE */ be<s16> m2C2[5];
    /* 0x3E8 */ be<f32> m2CC;
    /* 0x3EC */ be<f32> m2D0;
    /* 0x3F0 */ be<u32> m2D4;
    /* 0x3F4 */ gptr<dPath> mpPath;
    /* 0x3F8 */ u8 m2DC[0x405 - 0x3F8];
    /* 0x405 */ be<u8> m2E9;
    /* 0x406 */ u8 _406[2];
    /* 0x408 */ dCcD_Stts mStts;
    /* 0x444 */ dCcD_Cyl mCyl;
};
WWHD_OFFSET(nzg_class, mStts, 0x408);
WWHD_SIZE(nzg_class, 0x574);

/* 023111D8 */
static BOOL daNZG_Draw(nzg_class* i_this) {
    WWHD_FUNC(0x023111D8, BOOL, i_this);
    J3DModel* model = i_this->mpModel;
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    mDoExt_modelUpdateDL(model);
    return TRUE;
}
VERIFY(0x023111D8, daNZG_Draw);

/* nzg_00_move (inlined into Execute) */
static inline void nzg_00_move(nzg_class* i_this) {
    fopAc_ac_c* actor = i_this;

    if (i_this->m2C2[0] != 0 || i_this->m2BE >= i_this->m2C0) {
        return;
    }

    i_this->mCyl.SetC(&actor->current.pos);
    i_this->mCyl.SetH(REG8_F(11) + 40.0f);
    i_this->mCyl.SetR(REG8_F(12) + 40.0f);
    dComIfG_Ccsp_Set(&i_this->mCyl);

    if (i_this->mCyl.ChkCoHit()) {
        fopAc_ac_c* hit_actor = i_this->mCyl.GetCoHitAc();
        if (hit_actor != nullptr) {
            /* fopAcM_GetName (HD: null-checked inline; the checks are dead after the test above) */
            if (fpcM_GetName(hit_actor) != fpcNm_NZ_e && fpcM_GetName(hit_actor) != fpcNm_BOMB_e &&
                fpcM_GetName(hit_actor) != fpcNm_Bomb2_e && fpcM_GetName(hit_actor) != fpcNm_ITEM_e &&
                fpcM_GetName(hit_actor) != fpcNm_ESA_e) {
                i_this->m2D4 = fopAcM_GetID(hit_actor);
                i_this->m2BB = 1;
                return;
            }
        }
    }

    u32 parameters = fopAcM_GetParam(actor) & 0xFF000000;

    if (i_this->m2BA == 1) {
        parameters |= 0x100;
    } else if (i_this->m2BA == 2 && cM_rnd() < 0.5f) {
        parameters |= 0x100;
    }

    if (!(parameters & 0x100)) {
        f32 dist = fopAcM_searchPlayerDistance(actor);
        if (!(dist < i_this->m2CC * 0.5f))
            return;
    }
    f32 dist = fopAcM_searchPlayerDistance(actor);
    if (dist > i_this->m2D0 * 0.5f) {
        parameters |= 1;
        gabi::Local<csXyz> child_angle;
        s16 angleY = actor->current.angle.y;
        child_angle->x = actor->current.angle.x;
        f32 fy = (f32)angleY;
        child_angle->y = angleY;
        child_angle->z = actor->current.angle.z;
        f32 rnd = cM_rndFX(8192.0f);
        child_angle->y = (s16)gabi::ftoi(fy + rnd);
        u32 childProcID = fopAcM_createChild(fpcNm_NZ_e, fopAcM_GetID(actor), parameters, &actor->current.pos,
                                             fopAcM_GetRoomNo(actor), child_angle, &actor->scale, 0, 0);

        if (childProcID != fpcM_ERROR_PROCESS_ID_e) {
            i_this->m2C2[0] = (childProcID & 3) * 10 + 20;
            i_this->m2BE += 1;
        }
    }
}

/* nzg_01_move (inlined into Execute) */
static inline void nzg_01_move(nzg_class* i_this) {
    if (i_this->m2D4 != fpcM_ERROR_PROCESS_ID_e) {
        fopAc_ac_c* ac_id = fopAcM_SearchByID(i_this->m2D4);
        if (ac_id != nullptr) {
            f32 pos_z_diff = i_this->current.pos.z - ac_id->current.pos.z;
            f32 pos_x_diff = i_this->current.pos.x - ac_id->current.pos.x;
            f32 d2 = gabi::fmadds(pos_x_diff, pos_x_diff, pos_z_diff * pos_z_diff);
            if (std_sqrtf(d2) < 80.0f) {
                return;
            }
        }
    }
    i_this->m2D4 = fpcM_ERROR_PROCESS_ID_e;
    i_this->m2BB = 0;
}

/* 02311240 */
static BOOL daNZG_Execute(nzg_class* i_this) {
    WWHD_FUNC(0x02311240, BOOL, i_this);
    i_this->m2D0 = 400.0f;
    for (s32 j = 0; j < 4; j++) {
        if (i_this->m2C2[j] != 0) {
            i_this->m2C2[j] = i_this->m2C2[j] - 1;
        }
    }
    switch (i_this->m2BB) {
    case 0:
        nzg_00_move(i_this);
        break;
    case 1:
        nzg_01_move(i_this);
        break;
    }
    return TRUE;
}
VERIFY(0x02311240, daNZG_Execute);

/* 023115B4 */
static BOOL daNZG_IsDelete(nzg_class*) {
    WWHD_FUNC(0x023115B4, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x023115B4, daNZG_IsDelete);

/* 023115BC: HD: dComIfG_resDelete (GameCube resDeleteDemo) */
static BOOL daNZG_Delete(nzg_class* i_this) {
    WWHD_FUNC(0x023115BC, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x100245C0) /* "Nzg" */);
    return TRUE;
}
VERIFY(0x023115BC, daNZG_Delete);

/* 023115EC */
static BOOL useHeapInit(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x023115EC, BOOL, i_ac);
    nzg_class* nzg_this = (nzg_class*)i_ac;

    mDoMtx_stack_c::transS(nzg_this->current.pos.x, nzg_this->current.pos.y, nzg_this->current.pos.z);
    mDoMtx_stack_c::YrotM(nzg_this->shape_angle.y);

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x100245C4) /* "Nzg" */, dRes_INDEX_NZG_BDL_KANA_00_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(630, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x100245C8), 0x276, STR(0x100245D4));
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);

    if (model == nullptr) {
        return FALSE;
    }

    nzg_this->mpModel = model;
    J3DModel_setBaseScale(model, &nzg_this->scale);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    nzg_this->cullMtx = gabi::ea(model) + 0xC8; /* fopAcM_SetMtx(actor, model->getBaseTRMtx()) */
    return TRUE;
}
VERIFY(0x023115EC, useHeapInit);

/* 0231175C */
static cPhs_State daNZG_Create(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x0231175C, cPhs_State, i_ac);
    nzg_class* nzg_this = (nzg_class*)i_ac;
    /* fopAcM_ct_Retail(i_ac, nzg_class): inline constructors of mStts and mCyl */
    if (!fopAcM_CheckCondition(i_ac, fopAcCnd_INIT_e)) {
        if (i_ac != nullptr) {
            fopAc_ac_c_ct(i_ac);
            i_ac->__vtbl = NZG_VTBL;
            dCcD_Stts_ct(&nzg_this->mStts);
            dCcD_Cyl_ct(&nzg_this->mCyl, CM3DGAAB_VTBL);
        }
        fopAcM_OnCondition(i_ac, fopAcCnd_INIT_e);
    }
    cPhs_State phase_state = dComIfG_resLoad(&nzg_this->mPhs, STR(0x100245F4) /* "Nzg" */);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(nzg_this, 0x023115EC /* useHeapInit */, 0x680)) {
            return cPhs_ERROR_e;
        }
        u32 prm = fopAcM_GetParam(nzg_this);
        nzg_this->m2B8 = prm;
        nzg_this->m2B9 = prm >> 8;
        nzg_this->m2BA = prm >> 16;
        nzg_this->m2E9 = prm >> 24;
        if ((u8)(prm >> 24) != 0xFF) {
            nzg_this->mpPath = dPath_GetRoomPath(prm >> 24, fopAcM_GetRoomNo(i_ac));
        }
        nzg_this->shape_angle.y = nzg_this->current.angle.y;
        nzg_this->m2CC = (f32)nzg_this->m2B8 * 10.0f;
        nzg_this->m2C0 = 1;
        if (nzg_this->m2B9 != 0) {
            nzg_this->m2C0 = nzg_this->m2B9;
        }

        if (nzg_this->m2BA > 2) {
            nzg_this->m2BA = 0;
        }

        nzg_this->mCyl.Set(body_cyl_src);
        nzg_this->mCyl.SetStts(&nzg_this->mStts);
        if (nzg_this->m2BA == 0) {
            gabi::Local<csXyz> child_angle;
            child_angle->y = nzg_this->current.angle.y;
            child_angle->z = nzg_this->current.angle.z;
            child_angle->x = nzg_this->current.angle.x;
            fopAcM_createChild(STR(0x100245F8) /* "NpcNz" */, fopAcM_GetID(nzg_this), 0xffffffff, &nzg_this->current.pos,
                               fopAcM_GetRoomNo(nzg_this), child_angle, &nzg_this->scale, 0);
        }
    }
    return phase_state;
}
VERIFY(0x0231175C, daNZG_Create);

/* 023119A0: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_nzg_cpp() {
    WWHD_FUNC(0x023119A0, void, (u32)0);
    sinit_header_statics(0x10468EAC, 0x101C7874);
}
VERIFY(0x023119A0, __sinit_d_a_nzg_cpp);

/* 02311A34: sead::SafeString deleting destructor (this TU's copy; vtable 0x10024570 +0xC) */
static void SafeString_dt(SafeString* s, s32 flags) {
    WWHD_FUNC(0x02311A34, void, s, flags);
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}
VERIFY(0x02311A34, SafeString_dt);

/* 02311A48: nzg_class deleting destructor (compiler-generated, HD virtual destructor) */
static void nzg_class_dt(nzg_class* i_this, s32 flags) {
    WWHD_FUNC(0x02311A48, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02311A48, nzg_class_dt);

/* 02311AB4: sead::SafeString virtual at vtable +0x14 (assureTerminationImpl_, empty in this copy) */
static void SafeString_assureTermination(SafeString*) {
    WWHD_FUNC(0x02311AB4, void, (u32)0);
}
VERIFY(0x02311AB4, SafeString_assureTermination);
