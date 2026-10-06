/**
 * d_a_demo_dk.cpp (WWHD)
 * Demo Helmaroc/dragon (DK) idle model: waits and yawns.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_demo_dk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define DEMO_DK_VTBL 0x1000DA14    /* demo_dk_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x1000D9FC /* this TU's sead::SafeString vtable */

enum {
    dRes_INDEX_DEMO_DK_BCK_DK_L_AKUBI1_e = 4,
    dRes_INDEX_DEMO_DK_BCK_DK_L_WAIT1_e = 5,
    dRes_INDEX_DEMO_DK_BMD_DK_L_e = 8,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* fopAcM_SetMin / fopAcM_SetMax: 025D672C / 025D673C (as in bindings.h of newer merges) */
static inline void fopAcM_SetMin_l(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_SetMax_l(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }

struct demo_dk_class : fopAc_ac_c {
    /* 0x3AC */ u8 unk290[0x1C];
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ u8 unk2B4[0x2];
    /* 0x3D2 */ be<u8> unk_2B6;
    /* 0x3D3 */ u8 _3D3;
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ be<s16> unk_2BC[2];
    /* 0x3DC */ be<s16> unk_2C0;
    /* 0x3DE */ u8 _3DE[2];
    /* 0x3E0 */ cXyz unk_2C4;
};
WWHD_SIZE(demo_dk_class, 0x3EC);

/* 02122918 */
static BOOL daDEMO_DK_Draw(demo_dk_class* i_this) {
    WWHD_FUNC(0x02122918, BOOL, i_this);
    J3DModel* model = i_this->mpMorf->getModel();

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    i_this->mpMorf->updateDL();
    return TRUE;
}
VERIFY(0x02122918, daDEMO_DK_Draw);

/* 02122980 */
static void anm_init(demo_dk_class* i_this, s32 bckAnmIdx, f32 morf, u8 loopMode, f32 playSpeed, s32 soundIdx) {
    WWHD_FUNC(0x02122980, void, i_this, bckAnmIdx, morf, loopMode, playSpeed, soundIdx);
    if (soundIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1000DA2C) /* "Demo_Dk" */, bckAnmIdx, SAFESTRING_VTBL);
        void* snd = dComIfG_getObjectRes(STR(0x1000DA2C), soundIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, snd);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1000DA2C), bckAnmIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x02122980, anm_init);

/* mode_wait (inlined into Execute) */
static inline void mode_wait(demo_dk_class* i_this) {
    if (i_this->mpMorf->isStop()) {
        f32 tmp = 0.099999995f;

        if (i_this->unk_2C0 > 0) {
            i_this->unk_2C0 = i_this->unk_2C0 - 1;
        }

        if (i_this->unk_2C0 == 0 && cM_rnd() < tmp) {
            anm_init(i_this, dRes_INDEX_DEMO_DK_BCK_DK_L_AKUBI1_e, 5.0f, 0, 1.0f, 0);
            i_this->unk_2B6 = 1;
        } else {
            anm_init(i_this, dRes_INDEX_DEMO_DK_BCK_DK_L_WAIT1_e, 0.0f, 0, 1.0f, 0);
        }
    }
}

/* mode_akubi (inlined into Execute) */
static inline void mode_akubi(demo_dk_class* i_this) {
    if (i_this->mpMorf->isStop()) {
        anm_init(i_this, dRes_INDEX_DEMO_DK_BCK_DK_L_WAIT1_e, 5.0f, 0, 2.0f, 0);
        i_this->unk_2C0 = 3;
        i_this->unk_2B6 = 0;
    }
}

/* 02122AA8 */
static BOOL daDEMO_DK_Execute(demo_dk_class* i_this) {
    WWHD_FUNC(0x02122AA8, BOOL, i_this);
    for (s32 i = 0; i < 2; i++) {
        if (i_this->unk_2BC[i] != 0) {
            i_this->unk_2BC[i] = i_this->unk_2BC[i] - 1;
        }
    }

    switch (i_this->unk_2B6) {
    case 0:
        mode_wait(i_this);
        break;
    case 1:
        mode_akubi(i_this);
        break;
    }

    i_this->scale.x = 1.0f;
    i_this->scale.z = i_this->scale.y = 1.0f;
    i_this->current.pos.y = i_this->unk_2C4.y;
    i_this->cullSizeFar = 10.0f; /* fopAcM_setCullSizeFar */

    i_this->mpMorf->play(nullptr, 0, 0);

    J3DModel* model = i_this->mpMorf->getModel();

    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->current.angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->current.angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->current.angle.z);

    J3DModel_setBaseScale(model, &i_this->scale);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    return TRUE;
}
VERIFY(0x02122AA8, daDEMO_DK_Execute);

/* 02122D54 */
static BOOL daDEMO_DK_IsDelete(demo_dk_class*) {
    WWHD_FUNC(0x02122D54, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02122D54, daDEMO_DK_IsDelete);

/* 02122D5C: HD: dComIfG_resDelete (GameCube resDeleteDemo) */
static BOOL daDEMO_DK_Delete(demo_dk_class* i_this) {
    WWHD_FUNC(0x02122D5C, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x1000DA48) /* "Demo_Dk" */);
    return TRUE;
}
VERIFY(0x02122D5C, daDEMO_DK_Delete);

/* 02122D8C */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02122D8C, BOOL, a_this);
    demo_dk_class* i_this = (demo_dk_class*)a_this;

    J3DModelData* data = (J3DModelData*)dComIfG_getObjectRes(STR(0x1000DA50) /* "Demo_Dk" */, dRes_INDEX_DEMO_DK_BMD_DK_L_e, SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1000DA50), dRes_INDEX_DEMO_DK_BCK_DK_L_WAIT1_e, SAFESTRING_VTBL);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                            nullptr, 0, 0x11020203);

    if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x02122D8C, useHeapInit);

/* 02122E78 */
static cPhs_State daDEMO_DK_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02122E78, cPhs_State, a_this);
    demo_dk_class* i_this = (demo_dk_class*)a_this;

    /* fopAcM_ct(&i_this->actor, demo_dk_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = DEMO_DK_VTBL;
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, STR(0x1000DA60) /* "Demo_Dk" */);
    if (ret == cPhs_COMPLEATE_e) {
        i_this->unk_2C4.copy(a_this->current.pos);
        if (!fopAcM_entrySolidHeap(a_this, 0x02122D8C /* useHeapInit */, 0x6000)) {
            return cPhs_ERROR_e;
        }

        a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel())); /* fopAcM_SetMtx */
        fopAcM_SetMin_l(a_this, -1000.0f, -1000.0f, -1000.0f);
        fopAcM_SetMax_l(a_this, 1000.0f, 1000.0f, 1000.0f);

        gabi::store<u32>(gabi::ea(a_this) + 0x39C, 0); /* attention_info.flags */
        a_this->scale.y = 1.0f;
        a_this->scale.z = 1.0f;
        a_this->scale.x = 1.0f;
        anm_init(i_this, dRes_INDEX_DEMO_DK_BCK_DK_L_WAIT1_e, 0.0f, 0, 1.0f, 0);
    }
    return ret;
}
VERIFY(0x02122E78, daDEMO_DK_Create);

/* 02122FC8: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_demo_dk_cpp() {
    WWHD_FUNC(0x02122FC8, void, (u32)0);
    sinit_header_statics(0x10463AFC, 0x101B4370);
}
VERIFY(0x02122FC8, __sinit_d_a_demo_dk_cpp);

/* 0212305C: sead::SafeString deleting destructor (this TU's copy; vtable 0x1000D9FC +0xC) */
static void SafeString_dt(SafeString* s, s32 flags) {
    WWHD_FUNC(0x0212305C, void, s, flags);
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}
VERIFY(0x0212305C, SafeString_dt);

/* 02123070: demo_dk_class deleting destructor (compiler-generated, HD virtual destructor) */
static void demo_dk_class_dt(demo_dk_class* i_this, s32 flags) {
    WWHD_FUNC(0x02123070, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02123070, demo_dk_class_dt);

/* 021230C4: sead::SafeString virtual at vtable +0x14 (assureTerminationImpl_, empty in this copy) */
static void SafeString_assureTermination(SafeString*) {
    WWHD_FUNC(0x021230C4, void, (u32)0);
}
VERIFY(0x021230C4, SafeString_assureTermination);
