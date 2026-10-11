/**
 * d_a_jbo.cpp (WWHD)
 * Object - Baba Bud
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_jbo.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_jbo.h"
// HD: the native resource archive is spelled "Jbo" (GameCube: "JBO").
const gc_tu_data gc_tu = {.safeString = 0x10012208, .headerStatics = {0x10464868, 0x101B7B38, 0x10464874}};
static dCcD_SrcSph co_sph_src;

#define JBO_VTBL 0x10012220 /* HD: jbo_class vtable */

enum {
    dRes_INDEX_JBO_BCK_IN1_e = 4,
    dRes_INDEX_JBO_BCK_OUT1_e = 5,
    dRes_INDEX_JBO_BCK_UMARERU1_e = 6,
    dRes_INDEX_JBO_BMD_JH_e = 9,
};
enum { JH_JNT_J_JH_ME_E_e = 9 };
enum { JUMP_ANIMATION_TIME = 70 };
enum daJbo_Mode { daJbo_Mode_IDLE_e = 0, daJbo_Mode_WAIT_JUMP_e = 1, daJbo_Mode_SPAWN_e = 2 };
enum daJbo_Type {
    daJbo_Type_NORMAL_e = 0,
    daJbo_Type_POPS_UP_e = 1,
    daJbo_Type_UNK_2_e = 2,
    daJbo_Type_APPEAR_AFTER_DEKU_TREE_e = 3,
};
enum { JA_SE_CM_BV_BASE_POPUP = 0x5843, JA_SE_OBJ_JFLOWER_IN = 0x6946, JA_SE_OBJ_JFLOWER_OUT = 0x6947 };

// HD: the play accessor is called at each field access.
static inline bool jbo_checkPlayerStatus0(u32 mask) {
    return (((JboPlayHD *)dComIfGp_get())->mPlayerStatus & mask) != 0;
}
static inline JPABaseEmitter *jbo_particleSetToon(u16 id, const cXyz *pos) {
    dPa_control_c *pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
}

/* 021843F4 */
static BOOL nodeCallBack(J3DNode *i_node, int i_calcTiming) {
    if (i_calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint *joint = J3DNode_toJoint(i_node);
        J3DModel *model = j3dSys.getModel();
        jbo_class *jbo = (jbo_class *)model->getUserArea();
        s32 jntNo = ((JboJointDataHD *)joint)->mJntNo;
        if (jbo != nullptr && jntNo == JH_JNT_J_JH_ME_E_e) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            gabi::Local<cXyz> pos;
            pos->x = 0.0f;
            pos->y = 0.0f;
            pos->z = 0.0f;
            MtxPosition(pos.get(), &jbo->mParticlePos);
        }
    }
    return TRUE;
}

/* 0218448C */
void jbo_draw_SUB(jbo_class *i_this) {
    J3DModel *model = i_this->mpMorf->getModel();
    model->setBaseScale(i_this->scale);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->shape_angle.y);
    mDoMtx_stack_c::XrotM(i_this->shape_angle.x);
    mDoMtx_stack_c::ZrotM(i_this->shape_angle.z);
    if (i_this->mAnimationSpeed != 0) {
        f32 sx = REG_F(8, 1) + 1.1f;
        f32 sy = REG_F(8, 2) + 1.0f;
        f32 sz = REG_F(8, 3) + 0.9f;
        i_this->mAnimRotation = (s16)(i_this->mAnimRotation + i_this->mAnimationSpeed);
        mDoMtx_stack_c::YrotM(i_this->mAnimRotation);
        mDoMtx_stack_c::scaleM(sx, sy, sz);
        mDoMtx_stack_c::YrotM((s16)-i_this->mAnimRotation);
    }
    model->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 02184644 */
static BOOL daJBO_Draw(jbo_class *i_this) {
    J3DModel *model = i_this->mpMorf->getModel();
    if (i_this->mType == daJbo_Type_APPEAR_AFTER_DEKU_TREE_e)
        return TRUE;
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    g_env_light.setLightTevColorType(model, &i_this->tevStr);
    i_this->mpMorf->updateDL();
    return TRUE;
}

/* inlined into daJBO_Execute */
// HD: mode update is inlined.
static inline void jbo_move(jbo_class *i_this) {
    switch (i_this->mMode) {
    case daJbo_Mode_IDLE_e:
        if (jbo_checkPlayerStatus0(0x80 /* daPyStts0_UNK80_e */) && i_this->mCoSph.ChkCoHit()) {
            J3DAnmTransform *anm =
                (J3DAnmTransform *)dComIfG_getObjectRes("Jbo" /* "Jbo" */, dRes_INDEX_JBO_BCK_IN1_e);
            i_this->mpMorf->setAnm(anm, J3DFrameCtrl::EMode_NONE, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
            fopAcM_seStart(i_this, JA_SE_OBJ_JFLOWER_IN, 0);
            dComIfGp_setItemMagicCount(4);
            i_this->mFramesUntilJump = JUMP_ANIMATION_TIME;
            i_this->mMode = i_this->mMode + 1;
        }
        break;
    case daJbo_Mode_WAIT_JUMP_e:
        i_this->mAnimationSpeed = (s16)((JUMP_ANIMATION_TIME - i_this->mFramesUntilJump) * 200);
        if (i_this->mFramesUntilJump == 1) {
            /* daPy_py_c::onForceVomitJump(): player +0x3BC |= 0x10 */
            JboPlayerHD *player = ((JboPlayHD *)dComIfGp_get())->mpPlayer;
            player->onForceVomitJump();
        }
        if (jbo_checkPlayerStatus0(0x80000000 /* daPyStts0_UNK80000000_e */)) {
            J3DAnmTransform *anm = (J3DAnmTransform *)dComIfG_getObjectRes("Jbo", dRes_INDEX_JBO_BCK_OUT1_e);
            i_this->mpMorf->setAnm(anm, J3DFrameCtrl::EMode_NONE, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
            fopAcM_seStart(i_this, JA_SE_OBJ_JFLOWER_OUT, 0);
            i_this->mAnimationSpeed = 0;
            i_this->mAnimRotation = 0;
            JPABaseEmitter *emitter =
                jbo_particleSetToon(0xA110 /* ID_IT_ST_JMPFLOWER_SMOKE00 */, &i_this->mParticlePos);
            if (emitter != nullptr) {
                ((JboEmitterHD *)emitter)->setGlobalAlpha(100);
            }
            if (i_this->mType == daJbo_Type_UNK_2_e) {
                i_this->mMode = daJbo_Mode_IDLE_e;
                i_this->m2BA = 1;
            } else {
                i_this->mMode = daJbo_Mode_IDLE_e;
            }
        }
        break;
    case daJbo_Mode_SPAWN_e:
        if (i_this->mpMorf->isStop()) {
            i_this->mMode = daJbo_Mode_IDLE_e;
        }
        break;
    }
}

/* 021846B8 */
static BOOL daJBO_Execute(jbo_class *i_this) {
    if (i_this->mType == daJbo_Type_APPEAR_AFTER_DEKU_TREE_e) {
        if (dComIfGs_isEventBit(0x1801)) {
            i_this->mType = daJbo_Type_NORMAL_e;
            i_this->mCoSph.OnCoSPrmBit(1); /* OnCoSetBit */
            return TRUE;
        }
        return TRUE;
    }
    if (i_this->mFramesUntilJump != 0) {
        i_this->mFramesUntilJump = (s16)(i_this->mFramesUntilJump - 1);
    }
    switch (i_this->m2BB) {
    case 0:
        jbo_move(i_this);
        break;
    }
    i_this->mpMorf->play(nullptr, 0, 0);
    gabi::Local<cXyz> pos;
    pos->x = i_this->current.pos.x;
    pos->y = i_this->current.pos.y;
    pos->z = i_this->current.pos.z;
    pos->y = (f32)((f64)(f32)pos->y + 30.0);
    i_this->mCoSph.SetC(pos.get());
    i_this->mCoSph.SetR(55.0f);
    dComIfG_Ccsp_Set(&i_this->mCoSph);
    jbo_draw_SUB(i_this);
    return TRUE;
}

/* 02184A44 */
static BOOL daJBO_IsDelete(jbo_class *) {
    return TRUE;
}

/* 02184A4C */
static BOOL daJBO_Delete(jbo_class *i_this) {
    dComIfG_resDelete(&i_this->mPhs, "Jbo" /* "Jbo" */);
    return TRUE;
}

/* 02184A7C */
static BOOL useHeapInit(fopAc_ac_c *i_this) {
    jbo_class *a_this = (jbo_class *)i_this;
    J3DModelData *data = (J3DModelData *)dComIfG_getObjectRes("Jbo" /* "Jbo" */, dRes_INDEX_JBO_BMD_JH_e);
    J3DAnmTransform *anm = (J3DAnmTransform *)dComIfG_getObjectRes("Jbo", dRes_INDEX_JBO_BCK_IN1_e);
    mDoExt_McaMorf *morf =
        mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_RESET, 0.0f, 0, -1,
                               1, nullptr, 0, 0x11020203);
    a_this->mpMorf = morf;
    if (morf == nullptr || morf->getModel() == nullptr) {
        return FALSE;
    }
    a_this->mpMorf->getModel()->setUserArea((uintptr_t)a_this); /* setUserArea */
    for (u16 i = 0; i < a_this->mpMorf->getModel()->getModelData()->getJointNum(); i++) {
        a_this->mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack);
    }
    return TRUE;
}

/* 02184BE8 */
static cPhs_State daJBO_Create(fopAc_ac_c *i_this) {
    jbo_class *a_this = (jbo_class *)i_this;
    /* fopAcM_ct(i_this, jbo_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = JBO_VTBL;
            dCcD_Stts_ct(&a_this->mStts);
            __ct(&a_this->mCoSph); /* dCcD_Sph::dCcD_Sph */
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&a_this->mPhs, "Jbo" /* "Jbo" */);
    if (state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(i_this, useHeapInit, 0x1C20)) {
            return cPhs_ERROR_e;
        }
        a_this->mType = (u8)fopAcM_GetParam(a_this);
        if (a_this->mType == 0xFF) {
            a_this->mType = daJbo_Type_NORMAL_e;
        }
        s16 reg = REG_S(8, 9);
        if (reg != 0) {
            a_this->mType = (u8)reg;
        }
        fopAcM_SetMtx(i_this, a_this->mpMorf->getModel()->getBaseTRMtx()); /* fopAcM_SetMtx */
        i_this->attention_info.flags = 0;                                  /* attention_info.flags = 0 */
        a_this->mStts.Init(0xFF, 0xFF, i_this);
        a_this->mCoSph.Set(&co_sph_src);
        a_this->mCoSph.SetStts(&a_this->mStts);
        if (a_this->mType == daJbo_Type_APPEAR_AFTER_DEKU_TREE_e) {
            i_this->actor_status &= ~(u32)fopAcStts_SHOWMAP_e; /* fopAcM_OffStatus(SHOWMAP) */
            a_this->mCoSph.OffCoSPrmBit(1);                    /* ClrCoSet */
        }
        if (a_this->mType == daJbo_Type_POPS_UP_e) {
            J3DAnmTransform *anm =
                (J3DAnmTransform *)dComIfG_getObjectRes("Jbo", dRes_INDEX_JBO_BCK_UMARERU1_e);
            a_this->mpMorf->setAnm(anm, J3DFrameCtrl::EMode_NONE, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
            /* fopAcM_seStart: HD inline, only the eyePos check remains (this is known non-null) */
            if (gabi::ea(&i_this->eyePos) != 0)
                mDoAud_seStart(JA_SE_CM_BV_BASE_POPUP, &i_this->eyePos, 0,
                               dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
            a_this->mMode = daJbo_Mode_SPAWN_e;
        }
        i_this->gbaName = 0x21;
        jbo_draw_SUB(a_this);
    }
    return state;
}

/* 02184E34 */
static void __sinit_d_a_jbo_cpp() {
    __sinit_header_statics();
}

// HD: deleting destructor of a class with a trivial destructor
/* 02184EC8 */
static void trivial_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: jbo_class deleting destructor (HD)
/* 02184EDC */
static void jbo_class_dt(jbo_class *p, s32 flags) {
    if (p != nullptr) {
        JboSph_destroy(&p->mCoSph, 2); /* dCcD_Sph::~dCcD_Sph */
        __dt(&p->mStts);
        __dt(p); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}

// HD: empty virtual
/* 02184F48 */
static void empty_virtual(void *p) {}

/* WWHD glue
data co_sph_src 101B7AF8
str 10012204 in 021846B8 # "Jbo"
str 10012254 in 02184A4C # "Jbo"
str 10012258 in 02184A7C # "Jbo"
str 1001225C in 02184BE8 # "Jbo"
ext 025166F0 void __ct(dCcD_Sph*)
ext 02515AE8 void JboSph_destroy(dCcD_Sph*, s32)
*/
