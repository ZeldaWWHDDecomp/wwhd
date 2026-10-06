/**
 * d_a_syan.cpp (WWHD)
 * Object - Unused - Chandelier (Spaceworld 2001 trailer)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_syan.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1003EE54
#define SYAN_VTBL 0x1003EE6C /* HD: syan_class vtable */

enum {
    dRes_INDEX_SYAN_BCK_SYAN_e = 4,
    dRes_INDEX_SYAN_BDL_SYAN_e = 8,
};

/* b_pos_x/y/z (.data) */
#define b_pos_x(i) gabi::load<f32>(0x101D1658 + 4 * (i))
#define b_pos_y(i) gabi::load<f32>(0x101D1670 + 4 * (i))
#define b_pos_z(i) gabi::load<f32>(0x101D1688 + 4 * (i))

/* GameCube +0x11C throughout; size 0x6D4 (GameCube 0x5B8) */
struct syan_class : fopAc_ac_c {
    enum { kEmtrNum = 6 };
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> morf;
    /* 0x3B8 */ be<s32> frameCount;
    /* 0x3BC */ be<u32> state;
    /* 0x3C0 */ csXyz field_0x2a4[2];
    /* 0x3CC */ csXyz field_0x2b0;
    /* 0x3D2 */ u8 _3D2[2];
    /* 0x3D4 */ be<f32> field_0x2b8;
    /* 0x3D8 */ be<f32> field_0x2bc;
    /* 0x3DC */ be<f32> field_0x2c0;
    /* 0x3E0 */ be<f32> field_0x2c4;
    /* 0x3E4 */ be<f32> field_0x2c8;
    /* 0x3E8 */ be<f32> field_0x2cc;
    /* 0x3EC */ be<f32> field_0x2d0;
    /* 0x3F0 */ be<s16> timer[2];
    /* 0x3F4 */ be<f32> field_0x2d8;
    /* 0x3F8 */ be<f32> field_0x2dc;
    /* 0x3FC */ Mtx34 partMtx[kEmtrNum];
    /* 0x51C */ cXyz partPos[kEmtrNum];
    /* 0x564 */ cXyz partPosOld[kEmtrNum];
    /* 0x5AC */ be<s16> partRotY[kEmtrNum];
    /* 0x5B8 */ dPa_followEcallBack emtrCallBack[kEmtrNum];
    /* 0x630 */ be<f32> partAlpha[kEmtrNum];
    /* 0x648 */ be<f32> field_0x52c[kEmtrNum];
    /* 0x660 */ be<f32> partScale1[kEmtrNum];
    /* 0x678 */ be<f32> field_0x55c[kEmtrNum];
    /* 0x690 */ be<f32> partScale2[kEmtrNum];
    /* 0x6A8 */ be<s16> field_0x58c[kEmtrNum];
    /* 0x6B4 */ be<s16> field_0x598[kEmtrNum];
    /* 0x6C0 */ be<s16> field_0x5a4[kEmtrNum];
    /* 0x6CC */ be<u8> emtrEnabled[kEmtrNum];
    /* 0x6D2 */ u8 _6D2[2];
};
WWHD_OFFSET(syan_class, field_0x2b8, 0x3D4);
WWHD_OFFSET(syan_class, partMtx, 0x3FC);
WWHD_OFFSET(syan_class, emtrCallBack, 0x5B8);
WWHD_OFFSET(syan_class, emtrEnabled, 0x6CC);
WWHD_SIZE(syan_class, 0x6D4);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD J3D: j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a model's joint
 * matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices) (as d_a_kamome) */
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
/* 025F2550 mDoMtx_stack_c::lYrotM(s32) (static) */
static inline void mDoMtx_stack_lYrotM(s16 y) { gabi::call(0x025F2550, y); }
/* 025D672C / 025D673C fopAcM_SetMin / fopAcM_SetMax */
static inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
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
/* GHS array helpers */
static inline void __construct_array(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 flags) { gabi::call(0x028F0164, p, n, size, dtor, flags); }
/* JPABaseEmitter::setGlobalParticleScale (inline): +0x238 (HD) */
static inline void JPABaseEmitter_setGlobalParticleScale(JPABaseEmitter* e, f32 x, f32 y, f32 z) {
    gabi::store<f32>(gabi::ea(e) + 0x238, x);
    gabi::store<f32>(gabi::ea(e) + 0x23C, y);
    gabi::store<f32>(gabi::ea(e) + 0x240, z);
}

/* 024A4298 (not named by the matcher) */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x024A4298, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel_l* model = gabi::at<J3DModel_l>(gabi::load<u32>(0x104B462C));
        syan_class* i_this = gabi::at<syan_class>(model->mUserArea);
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != nullptr) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo == 2 /* SYAN_JNT_JOINT5_e */) {
                cMtx_YrotM(calc_mtx(), i_this->field_0x2a4[0].y);
                cMtx_XrotM(calc_mtx(), -i_this->field_0x2a4[0].x);
                cMtx_ZrotM(calc_mtx(), i_this->field_0x2a4[0].z);
                mtx_copy(getAnmMtx(model, jntNo), calc_mtx()); /* model->setAnmMtx */
                PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868)); /* J3DSys::mCurrentMtx */
            } else if (jntNo == 5 /* SYAN_JNT_JOINT8_e */) {
                cMtx_YrotM(calc_mtx(), i_this->field_0x2b0.y);
                cMtx_XrotM(calc_mtx(), -(i_this->field_0x2a4[1].x + i_this->field_0x2b0.x));
                cMtx_ZrotM(calc_mtx(), i_this->field_0x2b0.z);
                mtx_copy(getAnmMtx(model, jntNo), calc_mtx());
                PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868));

                gabi::Local<cXyz> b_pos;
                for (s32 i = 0; i < 6; i++) {
                    b_pos->set(b_pos_x(i), b_pos_y(i), b_pos_z(i));
                    i_this->partPosOld[i].copy(i_this->partPos[i]);
                    MtxPosition(b_pos.get(), &i_this->partPos[i]);
                }
            }
        }
    }
    return TRUE;
}
VERIFY(0x024A4298, nodeCallBack);

/* 024A450C. syan_draw inlined; HD: no alpha models (dComIfGd_setAlphaModel) and no colour */
static BOOL daSyan_Draw(syan_class* i_this) {
    WWHD_FUNC(0x024A450C, BOOL, i_this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    mDoExt_McaMorf* morf = i_this->morf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, morf->getModel(), &i_this->tevStr);
    i_this->morf->entryDL();
    for (s32 i = 0; i < syan_class::kEmtrNum; i++) {
        MtxTrans(i_this->partPos[i].x, i_this->partPos[i].y, i_this->partPos[i].z, false);
        f32 scale = i_this->partScale1[i] * i_this->partScale2[i];
        MtxScale(scale, scale, scale, true);
        cMtx_YrotM(calc_mtx(), i_this->partRotY[i]);
        PSMTXCopy(calc_mtx(), &i_this->partMtx[i]);
    }
    return TRUE;
}
VERIFY(0x024A450C, daSyan_Draw);

/* the tail of each switch case (GHS duplicated it) */
static inline void syan_set_mtx(syan_class* i_this) {
    J3DModel* model = i_this->morf->getModel();
    i_this->scale.set(0.25f, 0.25f, 0.25f);
    J3DModel_setBaseScale(model, &i_this->scale);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_lYrotM(i_this->current.angle.y);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
}

/* 024A45E0 */
static BOOL daSyan_Execute(syan_class* i_this) {
    WWHD_FUNC(0x024A45E0, BOOL, i_this);
    dComIfGp_get(); /* HD: an unused accessor call */
    for (s32 i = 0; i < 2; i++)
        if (i_this->timer[i] != 0)
            i_this->timer[i] = i_this->timer[i] - 1;

    i_this->morf->calc();
    i_this->frameCount = i_this->frameCount + 1;
    i_this->field_0x2c4 = i_this->field_0x2c4 + i_this->field_0x2cc;
    i_this->field_0x2c8 = i_this->field_0x2c8 + i_this->field_0x2d0;
    i_this->field_0x2a4[0].x = (s16)gabi::ftoi(i_this->field_0x2bc * cM_ssin(gabi::ftoi(i_this->field_0x2c4)));
    i_this->field_0x2a4[1].x = (s16)gabi::ftoi(i_this->field_0x2c0 * cM_ssin(gabi::ftoi(i_this->field_0x2c8)));
    i_this->field_0x2b0.y = (s16)gabi::ftoi(i_this->field_0x2b8 * cM_ssin((s16)i_this->frameCount * (REG0_S(8) + 800)));
    i_this->field_0x2b0.z = (s16)gabi::ftoi(i_this->field_0x2b8 * cM_ssin((s16)i_this->frameCount * (REG0_S(9) + 700)));

    switch ((u32)i_this->state) {
    case 0:
        break;
    case 1:
        cLib_addCalcAngleS2(&i_this->field_0x2b0.x, REG0_S(1) + 2500, 2, REG0_S(2) + 250);
        cLib_addCalc2(&i_this->field_0x2bc, gabi::fmadds(REG0_F(1), 100.0f, 1500.0f), 0.5f, gabi::fmadds(REG0_F(12), 100.0f, 150.0f));
        cLib_addCalc2(&i_this->field_0x2c0, 2500.0f, 0.5f, 250.0f);
        cLib_addCalc2(&i_this->field_0x2b8, 2000.0f, 0.5f, 200.0f);
        if (i_this->timer[0] == 0)
            i_this->state = i_this->state + 1;
        break;
    case 2: {
        cLib_addCalc2(&i_this->field_0x2cc, gabi::fmadds(REG0_F(4), 100.0f, 500.0f), 0.1f, gabi::fmadds(REG0_F(5), 100.0f, 50.0f));
        cLib_addCalc2(&i_this->field_0x2d0, gabi::fmadds(REG0_F(6), 100.0f, 1000.0f), 0.1f, gabi::fmadds(REG0_F(7), 100.0f, 20.0f));
        f32 t = i_this->field_0x2dc * gabi::fmadds(REG0_F(8), 100.0f, 300.0f);
        cLib_addCalc2(&i_this->field_0x2bc, t, 0.1f, gabi::fmadds(REG0_F(9), 100.0f, 10.0f));
        t = i_this->field_0x2dc * gabi::fmadds(REG0_F(10), 100.0f, 300.0f);
        cLib_addCalc2(&i_this->field_0x2c0, t, 0.1f, gabi::fmadds(REG0_F(11), 100.0f, 10.0f));
        cLib_addCalcAngleS2(&i_this->field_0x2b0.x, REG0_S(1) + 2500, 2, REG0_S(2) + 2000);
        cLib_addCalc2(&i_this->field_0x2b8, i_this->field_0x2dc * 500.0f, 0.2f, REG0_F(13) + 5.0f);
        break;
    }
    }

    syan_set_mtx(i_this);

    for (s32 i = 0; i < syan_class::kEmtrNum; i++) {
        if (i_this->field_0x58c[i] == 0) {
            f32 r = cM_rndF(REG0_F(2) + 10.0f);
            i_this->field_0x58c[i] = (s16)gabi::ftoi(r + 5.0f + REG0_F(3));
            r = cM_rndF(REG0_F(6) + 4.0f);
            i_this->field_0x52c[i] = r + 4.0f + REG0_F(7);
        }
        if (i_this->field_0x598[i] == 0) {
            i_this->field_0x598[i] = (s16)gabi::ftoi(cM_rndF(6.0f) + 3.0f);
            i_this->field_0x55c[i] = cM_rndF(0.075f) + 0.75f;
        }
        cLib_addCalc2(&i_this->partAlpha[i], i_this->field_0x52c[i], 1.0f, REG0_F(4) + 0.1f);
        cLib_addCalc2(&i_this->partScale1[i], i_this->field_0x55c[i], 0.4f, 0.04f);
        i_this->partRotY[i] = i_this->partRotY[i] + 0x100;
        if (i_this->field_0x5a4[i] == 0) {
            cLib_addCalc2(&i_this->partScale2[i], REG0_F(9) + 0.75f, 0.5f, 0.05f);
            if (!i_this->emtrEnabled[i]) {
                i_this->emtrEnabled[i] = true;
                /* static cXyz fire_scale(0.7f, 0.7f, 0.7f): guard 0x1046E218, object 0x1046E20C */
                cXyz* fire_scale = gabi::at<cXyz>(0x1046E20C);
                if (gabi::load<u32>(0x1046E218) == 0) {
                    gabi::store<u32>(0x1046E218, 1);
                    fire_scale->set(0.7f, 0.7f, 0.7f);
                }
                dComIfGp_particle_set(0x1EA /* ID_AK_JN_TORCH */, &i_this->partPos[i], nullptr, fire_scale, 0xFF,
                                      (dPa_levelEcallBack*)(void*)&i_this->emtrCallBack[i]);
            } else {
                JPABaseEmitter* emtr = i_this->emtrCallBack[i].getEmitter();
                if (emtr != nullptr) {
                    f32 k = REG0_F(3) + -0.03f;
                    f32 dx = (i_this->partPos[i].x - i_this->partPosOld[i].x) * k;
                    if (dx > 1.0f) dx = 1.0f;
                    else if (dx < -1.0f) dx = -1.0f;
                    f32 dz = (i_this->partPos[i].z - i_this->partPosOld[i].z) * k;
                    if (dz > 1.0f) dz = 1.0f;
                    else if (dz < -1.0f) dz = -1.0f;
                    JPABaseEmitter_setDirection(emtr, dx, 0.1f, dz);

                    f32 dirMag = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
                    f32 scaleY = gabi::fmadds(dirMag, REG0_F(12) + 2.0f, 1.0f);
                    f32 lim = REG0_F(13) + 4.0f;
                    if (scaleY > lim)
                        scaleY = lim;
                    JPABaseEmitter_setGlobalParticleScale(i_this->emtrCallBack[i].getEmitter(), 1.0f, scaleY, 1.0f);

                    gabi::Local<cXyz> pos;
                    pos->x = i_this->partPos[i].x;
                    pos->y = i_this->partPos[i].y + REG0_F(7) + 20.0f;
                    pos->z = i_this->partPos[i].z;
                    dComIfGp_particle_setSimple(0x4004 /* ID_AK_JP_O_KAGEROU00 */, pos.get());
                }
            }
        }

        if (i_this->field_0x58c[i] != 0)
            i_this->field_0x58c[i] = i_this->field_0x58c[i] - 1;
        if (i_this->field_0x598[i] != 0)
            i_this->field_0x598[i] = i_this->field_0x598[i] - 1;
        if (i_this->field_0x5a4[i] != 0)
            i_this->field_0x5a4[i] = i_this->field_0x5a4[i] - 1;
    }

    return TRUE;
}
VERIFY(0x024A45E0, daSyan_Execute);

/* 024A5074 */
static BOOL daSyan_IsDelete(syan_class* i_this) {
    WWHD_FUNC(0x024A5074, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024A5074, daSyan_IsDelete);

/* 024A507C */
static BOOL daSyan_Delete(syan_class* i_this) {
    WWHD_FUNC(0x024A507C, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x1003EEF8) /* "Syan" */);
    for (s32 i = 0; i < syan_class::kEmtrNum; i++)
        i_this->emtrCallBack[i].remove();
    return TRUE;
}
VERIFY(0x024A507C, daSyan_Delete);

/* 024A50E4 */
static BOOL daSyan_solidHeapCB(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x024A50E4, BOOL, i_ac);
    syan_class* i_this = (syan_class*)i_ac;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1003EF00), dRes_INDEX_SYAN_BDL_SYAN_e, SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1003EF00), dRes_INDEX_SYAN_BCK_SYAN_e, SAFESTRING_VTBL);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                                  nullptr, 0x80000, 0x11000002);
    i_this->morf = morf;
    return morf != nullptr;
}
VERIFY(0x024A50E4, daSyan_solidHeapCB);

/* 024A51AC: array element constructor of emtrCallBack (dPa_followEcallBack(0, 0)) */
static void emtrCallBack_ct(dPa_followEcallBack* p) {
    WWHD_FUNC(0x024A51AC, void, p);
    dPa_followEcallBack_ct(p, 0, 0);
}
VERIFY(0x024A51AC, emtrCallBack_ct);

/* 024A51B8: syan_class::syan_class (HD: out of line, allocates when this == NULL) */
static syan_class* syan_class_ct(syan_class* i_this) {
    WWHD_FUNC(0x024A51B8, syan_class*, i_this);
    if (i_this == nullptr) {
        i_this = (syan_class*)operator_new(0x6D4);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = SYAN_VTBL;
    __construct_array(i_this->emtrCallBack, 6, 0x14, 0x024A51AC);
    return i_this;
}
VERIFY(0x024A51B8, syan_class_ct);

/* 024A5224 */
static cPhs_State daSyan_Create(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x024A5224, cPhs_State, i_ac);
    /* fopAcM_ct(i_ac, syan_class) */
    if (!fopAcM_CheckCondition(i_ac, fopAcCnd_INIT_e)) {
        if (i_ac != nullptr)
            syan_class_ct((syan_class*)i_ac);
        fopAcM_OnCondition(i_ac, fopAcCnd_INIT_e);
    }
    syan_class* i_this = (syan_class*)i_ac;
    cPhs_State rt = dComIfG_resLoad(&i_this->mPhs, STR(0x1003EF14) /* "Syan" */);
    if (rt == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, 0x024A50E4 /* daSyan_solidHeapCB */, 0x3E40) != 0) {
            J3DModel_l* model = (J3DModel_l*)(void*)i_this->morf->getModel();
            for (u16 i = 0; i < J3DModelData_getJointNum(gabi::at<J3DModelData>(model->mModelData)); i++) {
                setJointCallBack(gabi::at<J3DModelData>(model->mModelData), i, 0x024A4298 /* nodeCallBack */);
            }
            fopAcM_SetMin(i_this, -1000.0f, -5000.0f, -1000.0f);
            fopAcM_SetMax(i_this, 1000.0f, 5000.0f, 1000.0f);
            i_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->morf->getModel())); /* fopAcM_SetMtx */
            model->mUserArea = gabi::ea(i_this);
            i_this->field_0x2b8 = 200.0f;

            /* static s16 fire_time[] = { 0, 5, 10, 15, 20, 25 } (.data 0x101D16C0) */
            for (s32 i = 0; i < syan_class::kEmtrNum; i++)
                i_this->field_0x5a4[i] = gabi::load<s16>(0x101D16C0 + 2 * i);
        } else {
            rt = cPhs_ERROR_e;
        }
    }
    return rt;
}
VERIFY(0x024A5224, daSyan_Create);

/* 024A53A8 */
static void __sinit_d_a_syan_cpp() {
    WWHD_FUNC(0x024A53A8, void, (u32)0);
    sinit_header_statics(0x1046E1F0, 0x101D16CC);
}
VERIFY(0x024A53A8, __sinit_d_a_syan_cpp);

/* 024A543C: sead::SafeString deleting destructor (this TU's vtable slot) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024A543C, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x024A543C, SafeString_dt);

/* 024A5450: dPa_followEcallBack deleting destructor (this TU's copy, trivial) */
static void followEcallBack_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024A5450, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x024A5450, followEcallBack_dt);

/* 024A5464: syan_class deleting destructor (compiler-generated, vtable +0xC) */
static void syan_class_dt(syan_class* i_this, s32 flags) {
    WWHD_FUNC(0x024A5464, void, i_this, flags);
    if (i_this != nullptr) {
        __destroy_arr(i_this->emtrCallBack, 6, 0x14, 0x024A5450, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024A5464, syan_class_dt);

/* 024A54D8: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x024A54D8, void, (u32)0);
}
VERIFY(0x024A54D8, SafeString_assureTerminationImpl);
