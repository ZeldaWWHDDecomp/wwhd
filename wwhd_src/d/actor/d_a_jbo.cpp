/**
 * d_a_jbo.cpp (WWHD)
 * Object - Baba Bud
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_jbo.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x10012208
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

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD J3D: j3dSys.mModel at 0x104B462C; a model's joint matrices live in a block at +0x2C
 * (+0x4 flags, +0x10 matrices); user area at +0xB8 (as in d_a_kamome / d_a_kb) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_l {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_l> mpMtxBlock;
    /* 0x30 */ u8 _30[0xB8 - 0x30];
    /* 0xB8 */ be<u32> mUserArea;
};
static inline Mtx34* getAnmMtx(J3DModel_l* model, s32 jntNo) {
    J3DMtxBlock_l* blk = model->mpMtxBlock;
    blk->mFlags |= 0x10; /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}
/* 027F3F94 (the matcher names it __nw): returns the model data's joint tree; joint count (u16) at +8 */
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::ea(gabi::call<void*>(0x027F3F94, d)) + 8); }
/* J3DModelData::getJointNodePointer(i)->setCallBack(cb) (HD: nodes of 0x1C bytes from +8; index checked against +4) */
static inline void setJointCallBack(J3DModelData* d, u16 i, u32 cb) {
    u32 data = gabi::ea(d);
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
/* save events: dSv_event_c at *(0x101F84DC) + 0x644 */
static inline dSv_event_c* dComIfGs_getEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline bool dComIfGp_checkPlayerStatus0(u32 mask) { return (gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0) & mask) != 0; }
/* dComIfGp_setItemMagicCount(n): play+0x5B60 (s16) += n */
static inline void dComIfGp_setItemMagicCount(s16 n) {
    u32 a = dComIfGp_ea() + 0x5B60;
    gabi::store<s16>(a, (s16)(gabi::load<s16>(a) + n));
}
/* JPABaseEmitter::setGlobalAlpha: +0x247 (HD) */
static inline void JPABaseEmitter_setGlobalAlpha(JPABaseEmitter* e, u8 a) { gabi::store<u8>(gabi::ea(e) + 0x247, a); }
/* dComIfGp_particle_setToon: dPa_control_c::set with group 2 */
static inline JPABaseEmitter* dComIfGp_particle_setToon(u16 id, const cXyz* pos) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
}

struct jbo_class : fopAc_ac_c {
    /* 0x3AC */ u8 m290[0x3C8 - 0x3AC];
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D4 */ be<u8> mType;
    /* 0x3D5 */ u8 m2B9;
    /* 0x3D6 */ be<u8> m2BA;
    /* 0x3D7 */ be<u8> m2BB;
    /* 0x3D8 */ be<u8> mMode;
    /* 0x3D9 */ u8 _3D9;
    /* 0x3DA */ be<s16> mFramesUntilJump;
    /* 0x3DC */ be<s16> mAnimRotation;
    /* 0x3DE */ be<s16> mAnimationSpeed;
    /* 0x3E0 */ cXyz mParticlePos;
    /* 0x3EC */ dCcD_Stts mStts;
    /* 0x428 */ dCcD_Sph mCoSph;
};
WWHD_OFFSET(jbo_class, mPhs, 0x3C8);
WWHD_OFFSET(jbo_class, mParticlePos, 0x3E0);
WWHD_OFFSET(jbo_class, mCoSph, 0x428);
WWHD_SIZE(jbo_class, 0x554);

/* 021843F4 */
static BOOL nodeCallBack(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x021843F4, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(i_node);
        J3DModel_l* model = gabi::at<J3DModel_l>(gabi::load<u32>(0x104B462C));
        jbo_class* jbo = gabi::at<jbo_class>(model->mUserArea);
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (jbo != nullptr && jntNo == JH_JNT_J_JH_ME_E_e) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            gabi::Local<cXyz> pos;
            pos->x = 0.0f;
            pos->y = 0.0f;
            pos->z = 0.0f;
            MtxPosition(pos.get(), &jbo->mParticlePos);
        }
    }
    return TRUE;
}
VERIFY(0x021843F4, nodeCallBack);

/* 0218448C */
void jbo_draw_SUB(jbo_class* i_this) {
    WWHD_FUNC(0x0218448C, void, i_this);
    J3DModel* model = i_this->mpMorf->getModel();
    J3DModel_setBaseScale(model, &i_this->scale);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.z);
    if (i_this->mAnimationSpeed != 0) {
        f32 sx = REG_F(8, 1) + 1.1f;
        f32 sy = REG_F(8, 2) + 1.0f;
        f32 sz = REG_F(8, 3) + 0.9f;
        i_this->mAnimRotation = (s16)(i_this->mAnimRotation + i_this->mAnimationSpeed);
        mDoMtx_stack_c::YrotM(i_this->mAnimRotation);
        mDoMtx_stack_scaleM(sx, sy, sz);
        mDoMtx_stack_c::YrotM((s16)-i_this->mAnimRotation);
    }
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
}
VERIFY(0x0218448C, jbo_draw_SUB);

/* 02184644 */
static BOOL daJBO_Draw(jbo_class* i_this) {
    WWHD_FUNC(0x02184644, BOOL, i_this);
    J3DModel* model = i_this->mpMorf->getModel();
    if (i_this->mType == daJbo_Type_APPEAR_AFTER_DEKU_TREE_e)
        return TRUE;
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    i_this->mpMorf->updateDL();
    return TRUE;
}
VERIFY(0x02184644, daJBO_Draw);

/* jbo_move (inlined into daJBO_Execute in HD) */
static inline void jbo_move(jbo_class* i_this) {
    switch (i_this->mMode) {
    case daJbo_Mode_IDLE_e:
        if (dComIfGp_checkPlayerStatus0(0x80 /* daPyStts0_UNK80_e */) && i_this->mCoSph.ChkCoHit()) {
            J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10012204) /* "JBO" */, dRes_INDEX_JBO_BCK_IN1_e, SAFESTRING_VTBL);
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
            u32 player = gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER);
            gabi::store<u32>(player + 0x3BC, gabi::load<u32>(player + 0x3BC) | 0x10);
        }
        if (dComIfGp_checkPlayerStatus0(0x80000000 /* daPyStts0_UNK80000000_e */)) {
            J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10012204), dRes_INDEX_JBO_BCK_OUT1_e, SAFESTRING_VTBL);
            i_this->mpMorf->setAnm(anm, J3DFrameCtrl::EMode_NONE, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
            fopAcM_seStart(i_this, JA_SE_OBJ_JFLOWER_OUT, 0);
            i_this->mAnimationSpeed = 0;
            i_this->mAnimRotation = 0;
            JPABaseEmitter* emitter = dComIfGp_particle_setToon(0xA110 /* ID_IT_ST_JMPFLOWER_SMOKE00 */, &i_this->mParticlePos);
            if (emitter != nullptr) {
                JPABaseEmitter_setGlobalAlpha(emitter, 100);
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
static BOOL daJBO_Execute(jbo_class* i_this) {
    WWHD_FUNC(0x021846B8, BOOL, i_this);
    if (i_this->mType == daJbo_Type_APPEAR_AFTER_DEKU_TREE_e) {
        if (dSv_event_isEventBit(dComIfGs_getEvent(), 0x1801)) {
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
VERIFY(0x021846B8, daJBO_Execute);

/* 02184A44 */
static BOOL daJBO_IsDelete(jbo_class*) {
    WWHD_FUNC(0x02184A44, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02184A44, daJBO_IsDelete);

/* 02184A4C */
static BOOL daJBO_Delete(jbo_class* i_this) {
    WWHD_FUNC(0x02184A4C, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x10012254) /* "JBO" */);
    return TRUE;
}
VERIFY(0x02184A4C, daJBO_Delete);

/* 02184A7C */
static BOOL useHeapInit(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02184A7C, BOOL, i_this);
    jbo_class* a_this = (jbo_class*)i_this;
    J3DModelData* data = (J3DModelData*)dComIfG_getObjectRes(STR(0x10012258) /* "JBO" */, dRes_INDEX_JBO_BMD_JH_e, SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10012258), dRes_INDEX_JBO_BCK_IN1_e, SAFESTRING_VTBL);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_RESET, 0.0f, 0, -1, 1,
                                                  nullptr, 0, 0x11020203);
    a_this->mpMorf = morf;
    if (morf == nullptr || morf->getModel() == nullptr) {
        return FALSE;
    }
    gabi::store<u32>(gabi::ea(a_this->mpMorf->getModel()) + 0xB8, gabi::ea(a_this)); /* setUserArea */
    for (u16 i = 0; i < J3DModelData_getJointNum(J3DModel_getModelData(a_this->mpMorf->getModel())); i++) {
        setJointCallBack(J3DModel_getModelData(a_this->mpMorf->getModel()), i, 0x021843F4 /* nodeCallBack */);
    }
    return TRUE;
}
VERIFY(0x02184A7C, useHeapInit);

/* 02184BE8 */
static cPhs_State daJBO_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02184BE8, cPhs_State, i_this);
    jbo_class* a_this = (jbo_class*)i_this;
    /* fopAcM_ct(i_this, jbo_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = JBO_VTBL;
            dCcD_Stts_ct(&a_this->mStts);
            gabi::call(0x025166F0, &a_this->mCoSph); /* dCcD_Sph::dCcD_Sph */
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&a_this->mPhs, STR(0x1001225C) /* "JBO" */);
    if (state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(i_this, 0x02184A7C /* useHeapInit */, 0x1C20)) {
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
        i_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(a_this->mpMorf->getModel())); /* fopAcM_SetMtx */
        gabi::store<u32>(gabi::ea(i_this) + 0x388 + 0x14, 0); /* attention_info.flags = 0 */
        a_this->mStts.Init(0xFF, 0xFF, i_this);
        a_this->mCoSph.Set(gabi::at<dCcD_SrcSph>(0x101B7AF8) /* co_sph_src */);
        a_this->mCoSph.SetStts(&a_this->mStts);
        if (a_this->mType == daJbo_Type_APPEAR_AFTER_DEKU_TREE_e) {
            i_this->actor_status &= ~0x20u; /* fopAcM_OffStatus(SHOWMAP) */
            a_this->mCoSph.OffCoSPrmBit(1); /* ClrCoSet */
        }
        if (a_this->mType == daJbo_Type_POPS_UP_e) {
            J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1001225C), dRes_INDEX_JBO_BCK_UMARERU1_e, SAFESTRING_VTBL);
            a_this->mpMorf->setAnm(anm, J3DFrameCtrl::EMode_NONE, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
            /* fopAcM_seStart: HD inline, only the eyePos check remains (this is known non-null) */
            if (gabi::ea(&i_this->eyePos) != 0)
                mDoAud_seStart(JA_SE_CM_BV_BASE_POPUP, &i_this->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
            a_this->mMode = daJbo_Mode_SPAWN_e;
        }
        i_this->gbaName = 0x21;
        jbo_draw_SUB(a_this);
    }
    return state;
}
VERIFY(0x02184BE8, daJBO_Create);

/* 02184E34 */
static void __sinit_d_a_jbo_cpp() {
    WWHD_FUNC(0x02184E34, void, (u32)0);
    sinit_header_statics(0x10464868, 0x101B7B38);
}
VERIFY(0x02184E34, __sinit_d_a_jbo_cpp);

/* 02184EC8: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02184EC8, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02184EC8, trivial_dt);

/* 02184EDC: jbo_class deleting destructor (HD) */
static void jbo_class_dt(jbo_class* p, s32 flags) {
    WWHD_FUNC(0x02184EDC, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x02515AE8, &p->mCoSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&p->mStts, 2);
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x02184EDC, jbo_class_dt);

/* 02184F48: empty virtual */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02184F48, void, p);
}
VERIFY(0x02184F48, empty_virtual);
