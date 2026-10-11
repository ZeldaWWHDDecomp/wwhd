/**
 * d_a_demo_dk.cpp (WWHD)
 * Demo Helmaroc/dragon (DK) idle model: waits and yawns.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_demo_dk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_demo_dk.h"

const gc_tu_data gc_tu = {.safeString = 0x1000D9FC};
// HD: the archive is named "Demo_Dk", while the GameCube source spells it "DEMO_DK".

enum {
    dRes_INDEX_DEMO_DK_BCK_DK_L_AKUBI1_e = 4,
    dRes_INDEX_DEMO_DK_BCK_DK_L_WAIT1_e = 5,
    dRes_INDEX_DEMO_DK_BMD_DK_L_e = 8,
};

// HD: GHS stores a virtual destructor vtable during placement construction.
demo_dk_class::demo_dk_class() {
    fopAc_ac_c_ct(&actor);
    actor.__vtbl = 0x1000DA14;
}

/* 02122918 */
static BOOL daDEMO_DK_Draw(demo_dk_class* i_this) {
    J3DModel* model = i_this->mpMorf->getModel();

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->actor.current.pos, &i_this->actor.tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->actor.tevStr);
    i_this->mpMorf->updateDL();
    return TRUE;
}

/* 02122980 */
static void anm_init(demo_dk_class* i_this, s32 bckAnmIdx, f32 morf, u8 loopMode, f32 playSpeed, s32 soundIdx) {
    if (soundIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("DEMO_DK", bckAnmIdx);
        void* snd = dComIfG_getObjectRes("DEMO_DK", soundIdx);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, snd);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("DEMO_DK", bckAnmIdx);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, nullptr);
    }
}

/* mode_wait (inlined into Execute) */
static inline void mode_wait(demo_dk_class* i_this) {
    if (i_this->mpMorf->isStop()) {
        f32 tmp = 0.099999995f;

        if (i_this->unk_2C0 > 0) {
            i_this->unk_2C0 = i_this->unk_2C0 - 1;
        }

        if (i_this->unk_2C0 == 0 && cM_rnd() < tmp) {
            anm_init(i_this, dRes_INDEX_DEMO_DK_BCK_DK_L_AKUBI1_e, 5.0f, 0, 1.0f, 0);
            i_this->unk_2B6 = demo_dk_class::MODE_AKUBI_e;
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
        i_this->unk_2B6 = demo_dk_class::MODE_WAIT_e;
    }
}

/* 02122AA8 */
static BOOL daDEMO_DK_Execute(demo_dk_class* i_this) {
    for (s32 i = 0; i < 2; i++) {
        if (i_this->unk_2BC[i] != 0) {
            i_this->unk_2BC[i] = i_this->unk_2BC[i] - 1;
        }
    }

    switch (i_this->unk_2B6) {
    case demo_dk_class::MODE_WAIT_e:
        mode_wait(i_this);
        break;
    case demo_dk_class::MODE_AKUBI_e:
        mode_akubi(i_this);
        break;
    }

    i_this->actor.scale.x = 1.0f;
    i_this->actor.scale.z = i_this->actor.scale.y = 1.0f;
    i_this->actor.current.pos.y = i_this->unk_2C4.y;
    fopAcM_setCullSizeFar(&i_this->actor, 10.0f);

    i_this->mpMorf->play(nullptr, 0, 0);

    J3DModel* model = i_this->mpMorf->getModel();

    mDoMtx_stack_c::transS(i_this->actor.current.pos.x, i_this->actor.current.pos.y, i_this->actor.current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->actor.current.angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->actor.current.angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->actor.current.angle.z);

    model->setBaseScale(i_this->actor.scale);
    model->setBaseTRMtx(mDoMtx_stack_c::get());
    return TRUE;
}

/* 02122D54 */
static BOOL daDEMO_DK_IsDelete(demo_dk_class*) {
    return TRUE;
}

/* 02122D5C */
static BOOL daDEMO_DK_Delete(demo_dk_class* i_this) {
    dComIfG_resDelete(&i_this->mPhase, "DEMO_DK");
    return TRUE;
}

/* 02122D8C */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    demo_dk_class* i_this = (demo_dk_class*)a_this;

    J3DModelData* data = (J3DModelData*)dComIfG_getObjectRes("DEMO_DK", dRes_INDEX_DEMO_DK_BMD_DK_L_e);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("DEMO_DK", dRes_INDEX_DEMO_DK_BCK_DK_L_WAIT1_e);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                            nullptr, 0, 0x11020203);

    if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    return TRUE;
}

/* 02122E78 */
static cPhs_State daDEMO_DK_Create(fopAc_ac_c* a_this) {
    demo_dk_class* i_this = (demo_dk_class*)a_this;

    fopAcM_ct(a_this, demo_dk_class);
    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, "DEMO_DK");
    if (ret == cPhs_COMPLEATE_e) {
        i_this->unk_2C4.copy(a_this->current.pos);
        if (!fopAcM_entrySolidHeap(a_this, useHeapInit, 0x6000)) {
            return cPhs_ERROR_e;
        }

        fopAcM_SetMtx(a_this, i_this->mpMorf->getModel()->getBaseTRMtx());
        fopAcM_SetMin(a_this, -1000.0f, -1000.0f, -1000.0f);
        fopAcM_SetMax(a_this, 1000.0f, 1000.0f, 1000.0f);

        a_this->attention_info.flags = 0;
        a_this->scale.y = 1.0f;
        a_this->scale.z = 1.0f;
        a_this->scale.x = 1.0f;
        anm_init(i_this, dRes_INDEX_DEMO_DK_BCK_DK_L_WAIT1_e, 0.0f, 0, 1.0f, 0);
    }
    return ret;
}

/* 02122FC8 */
static void __sinit_d_a_demo_dk_cpp() {
    sinit_header_statics(0x10463AFC, 0x101B4370);
}

/* 0212305C */
static void SafeString_dt(SafeString* s, s32 flags) {
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}

/* 02123070 */
static void demo_dk_class_dt(demo_dk_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(&i_this->actor);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021230C4 */
static void SafeString_assureTermination(SafeString*) {
}

/* WWHD glue (tools/verify/autoglue.py)
str 1000DA2C "DEMO_DK" renamed
str 1000DA48 "DEMO_DK" renamed in 02122D5C
str 1000DA50 "DEMO_DK" renamed in 02122D8C
str 1000DA60 "DEMO_DK" renamed in 02122E78
ext 025D672C void fopAcM_SetMin(fopAc_ac_c*, f32, f32, f32)
ext 025D673C void fopAcM_SetMax(fopAc_ac_c*, f32, f32, f32)
*/
