/**
 * d_a_syan.cpp (WWHD)
 * Object - Unused - Chandelier (Spaceworld 2001 trailer)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_syan.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_syan.h"
const gc_tu_data gc_tu = {.safeString = 0x1003EE54};
static f32 b_pos_x[6] = {870.0f, 420.0f, -420.0f, -880.0f, -400.0f, 440.0f};
static f32 b_pos_y[6] = {-560.0f, -760.0f, -630.0f, -710.0f, -670.0f, -690.0f};
static f32 b_pos_z[6] = {0.0f, 750.0f, 750.0f, 0.0f, -720.0f, -750.0f};

/* 024A4298 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys.getModel();
        syan_class* i_this = (syan_class*)model->getUserArea();
        s32 jntNo = ((SyanJointData*)joint)->mJntNo;
        if (i_this != nullptr) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            if (jntNo == SYAN_JNT_JOINT5_e) {
                cMtx_YrotM(calc_mtx(), i_this->field_0x2a4[0].y);
                cMtx_XrotM(calc_mtx(), -i_this->field_0x2a4[0].x);
                cMtx_ZrotM(calc_mtx(), i_this->field_0x2a4[0].z);
                model->setAnmMtx(jntNo, calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
            } else if (jntNo == SYAN_JNT_JOINT8_e) {
                cMtx_YrotM(calc_mtx(), i_this->field_0x2b0.y);
                cMtx_XrotM(calc_mtx(), -(i_this->field_0x2a4[1].x + i_this->field_0x2b0.x));
                cMtx_ZrotM(calc_mtx(), i_this->field_0x2b0.z);
                model->setAnmMtx(jntNo, calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);

                cXyz b_pos;
                for (s32 i = 0; i < 6; i++) {
                    b_pos.set(b_pos_x[i], b_pos_y[i], b_pos_z[i]);
                    i_this->partPosOld[i].copy(i_this->partPos[i]);
                    MtxPosition(&b_pos, &i_this->partPos[i]);
                }
            }
        }
    }
    return TRUE;
}

// HD: syan_draw is inlined; alpha-model submission and colour setup are absent.
/* 024A450C */
static BOOL daSyan_Draw(syan_class* i_this) {
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

/* inlined into daSyan_Execute */
static inline void syan_set_mtx(syan_class* i_this) {
    SyanModel* model = (SyanModel*)i_this->morf->getModel();
    i_this->scale.set(0.25f, 0.25f, 0.25f);
    model->setBaseScale(i_this->scale);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    syan_lYrotM(i_this->current.angle.y);
    model->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 024A45E0 */
static BOOL daSyan_Execute(syan_class* i_this) {
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
                static cXyz fire_scale(0.7f, 0.7f, 0.7f);
                dComIfGp_particle_set(dPa_name::ID_AK_JN_TORCH, &i_this->partPos[i], nullptr, &fire_scale, 0xFF,
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
                    emtr->setDirection(dx, 0.1f, dz);

                    f32 dirMag = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
                    f32 scaleY = gabi::fmadds(dirMag, REG0_F(12) + 2.0f, 1.0f);
                    f32 lim = REG0_F(13) + 4.0f;
                    if (scaleY > lim)
                        scaleY = lim;
                    ((SyanEmitter*)i_this->emtrCallBack[i].getEmitter())->setGlobalParticleScale(1.0f, scaleY, 1.0f);

                    cXyz pos;
                    pos.x = i_this->partPos[i].x;
                    pos.y = i_this->partPos[i].y + REG0_F(7) + 20.0f;
                    pos.z = i_this->partPos[i].z;
                    dComIfGp_particle_setSimple(dPa_name::ID_AK_JP_O_KAGEROU00, &pos);
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

/* 024A5074 */
static BOOL daSyan_IsDelete(syan_class* i_this) {
    return TRUE;
}

/* 024A507C */
static BOOL daSyan_Delete(syan_class* i_this) {
    dComIfG_resDelete(&i_this->mPhs, "Syan");
    for (s32 i = 0; i < syan_class::kEmtrNum; i++)
        i_this->emtrCallBack[i].remove();
    return TRUE;
}

/* 024A50E4 */
static BOOL daSyan_solidHeapCB(fopAc_ac_c* i_ac) {
    syan_class* i_this = (syan_class*)i_ac;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Syan", dRes_INDEX_SYAN_BDL_SYAN_e);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Syan", dRes_INDEX_SYAN_BCK_SYAN_e);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                                  nullptr, 0x80000, 0x11000002);
    i_this->morf = morf;
    return morf != nullptr;
}

/* 024A51AC */
static void emtrCallBack_ct(dPa_followEcallBack* p) {
    dPa_followEcallBack_ct(p, 0, 0);
}

// HD: out-of-line constructor allocates on a null this and constructs the callback array.
/* 024A51B8 */
static syan_class* syan_class_ct(syan_class* i_this) {
    if (i_this == nullptr) {
        i_this = (syan_class*)operator_new(0x6D4);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = 0x1003EE6C;
    syan_constructArray(i_this->emtrCallBack, syan_class::kEmtrNum, sizeof(dPa_followEcallBack), emtrCallBack_ct);
    return i_this;
}

/* 024A5224 */
static cPhs_State daSyan_Create(fopAc_ac_c* i_ac) {
    /* fopAcM_ct(i_ac, syan_class) */
    if (!fopAcM_CheckCondition(i_ac, fopAcCnd_INIT_e)) {
        if (i_ac != nullptr)
            syan_class_ct((syan_class*)i_ac);
        fopAcM_OnCondition(i_ac, fopAcCnd_INIT_e);
    }
    syan_class* i_this = (syan_class*)i_ac;
    cPhs_State rt = dComIfG_resLoad(&i_this->mPhs, "Syan");
    if (rt == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, daSyan_solidHeapCB, 0x3E40) != 0) {
            J3DModel* model = i_this->morf->getModel();
            for (u16 i = 0; i < model->getModelData()->getJointNum(); i++) {
                model->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack);
            }
            fopAcM_SetMin(i_this, -1000.0f, -5000.0f, -1000.0f);
            fopAcM_SetMax(i_this, 1000.0f, 5000.0f, 1000.0f);
            fopAcM_SetMtx(i_this, i_this->morf->getModel()->getBaseTRMtx());
            model->setUserArea((uintptr_t)i_this);
            i_this->field_0x2b8 = 200.0f;

            static s16 fire_time[] = {0, 5, 10, 15, 20, 25};
            for (s32 i = 0; i < syan_class::kEmtrNum; i++)
                i_this->field_0x5a4[i] = fire_time[i];
        } else {
            rt = cPhs_ERROR_e;
        }
    }
    return rt;
}

// HD: compiler-generated and template code of this translation unit.
/* 024A53A8 */
static void __sinit_d_a_syan_cpp() {
    sinit_header_statics(0x1046E1F0, 0x101D16CC);
}

/* 024A543C */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 024A5450 */
static void followEcallBack_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 024A5464 */
static void syan_class_dt(syan_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        syan_destroyArray(i_this->emtrCallBack, syan_class::kEmtrNum, sizeof(dPa_followEcallBack), followEcallBack_dt, 0);
        __dt((fopAc_ac_c*)i_this);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024A54D8 */
static void SafeString_assureTerminationImpl(void*) {
}

/* WWHD glue (tools/verify/autoglue.py): per-TU data and existing local bindings.
data b_pos_x 101D1658
data b_pos_y 101D1670
data b_pos_z 101D1688
data fire_time 101D16C0
data fire_scale 1046E20C guard 1046E218
str 1003EEF8 in 024A507C
str 1003EF00 in 024A50E4
str 1003EF14 in 024A5224
ext 025F2550 void syan_lYrotM(s16)
ext 025D672C void fopAcM_SetMin(fopAc_ac_c*, f32, f32, f32)
ext 025D673C void fopAcM_SetMax(fopAc_ac_c*, f32, f32, f32)
*/
