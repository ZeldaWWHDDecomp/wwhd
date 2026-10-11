/**
 * d_a_dr2.cpp (WWHD)
 * NPC - Gohma fight - Valoo (body & tail) + lava pit & ceiling rock
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_dr2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_dr2.h"

#define SAFESTRING_VTBL 0x1000E0D0
#define DR2_VTBL 0x1000E0F8     /* HD: dr2_class vtable */
#define DR2_HIO_VTBL 0x1000E0E8 /* daDr2_HIO_c vtable */

const gc_tu_data gc_tu = {.safeString = SAFESTRING_VTBL, .headerStatics = {0x10463C34, 0x101B47E0, 0}};
static daDr2_HIO_c l_HIO;
static const f32 hsx[6] = {-10.0f, -10.0f, 0.0f, 10.0f, 10.0f, 0.0f};
static const f32 hsz[6] = {-7.0f, 7.0f, 12.0f, 7.0f, -7.0f, -12.0f};
static const s32 hahen_model[6] = {15, 16, 17, 18, 19, 20};

// HD: setAnmMtx uses word copies here; the common float assignment would quiet signaling NaNs.
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void J3DModel_setAnmMtx(J3DModel* model, s32 joint, Mtx34* source) {
    mtx_copy(model->getAnmMtx(joint), source);
}
/* inlined into daDr2_Execute / daDr2_Draw */
static inline f32 cM_fsin(f32 radians) { return cM_ssin(dr2_radiansToAngle(radians)); }
/* inlined into daDr2_Execute / daDr2_Draw */
static inline f32 anm_getFrame(void* animation) {
    return reinterpret_cast<J3DFrameCtrl*>(animation)->mFrame;
}
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* emitter) {
    Dr2Emitter* view = reinterpret_cast<Dr2Emitter*>(emitter);
    u32 flags = view->mFlags;
    view->mMaxFrame = -1;
    view->mFlags = flags | 1;
}
/* inlined into daDr2_Execute / daDr2_Draw */
static inline bool JPABaseEmitter_isEnableDeleteEmitter(JPABaseEmitter* emitter) {
    Dr2Emitter* view = reinterpret_cast<Dr2Emitter*>(emitter);
    return (view->mFlags & 8) && view->mParticleCount + view->mChildCount == 0;
}
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void JPABaseEmitter_setRate(JPABaseEmitter* emitter, f32 rate) {
    reinterpret_cast<Dr2Emitter*>(emitter)->mRate = rate;
}
/* inlined into daDr2_Execute / daDr2_Draw */
static inline JPABaseEmitter* particle_setToon(u16 id, const cXyz* pos, const csXyz* angle,
                                            const cXyz* scale, u8 alpha, dPa_levelEcallBack* cb, s8 setup) {
    dPa_control_c* control = dComIfGp_getParticle();
    return dPa_control_set(control, 2, id, pos, angle, scale, alpha, cb, setup, nullptr, nullptr, nullptr);
}
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void seStartCurrent(fopAc_ac_c* actor, u32 sound, bool checkActor) {
    if (checkActor && actor == nullptr) return;
    if (gabi::ea(&actor->current.pos) != 0)
        mDoAud_seStart(sound, &actor->current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
}
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void StartShock_reg(s32 add, s32 flags, cXyz* pos) {
    dVibration_c* vibration = dComIfGp_getVibration();
    s32 strength = REG0_S(2) + add;
    dr2_shock(vibration, strength, flags, pos);
}
// HD: animation frame, pointed-to resource frame, then functor arguments in GHS load order.
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void anm_setFrame_hd(void* animation, f32 frame, bool brk) {
    be<f32>* resourceFrame;
    gptr<Dr2FrameFunctor>* functorSlot;
    if (brk) {
        Dr2BrkAnimation* view = reinterpret_cast<Dr2BrkAnimation*>(animation);
        resourceFrame = view->mpAnimationFrame;
        view->mFrameCtrl.mFrame = frame;
        *resourceFrame = frame;
        functorSlot = &view->mpFunctor;
    } else {
        Dr2BtkAnimation* view = reinterpret_cast<Dr2BtkAnimation*>(animation);
        resourceFrame = view->mpAnimationFrame;
        view->mFrameCtrl.mFrame = frame;
        *resourceFrame = frame;
        functorSlot = &view->mpFunctor;
    }
    Dr2FrameFunctor* functor = *functorSlot;
    u32 target = functor->mFunction;
    f32 b = functor->mB;
    f32 a = functor->mA;
    u32 argument = functor->mArgument;
    f32 result = gabi::call_ptr<f32>(target, argument, frame, a, b);
    functor->mResult = result;
    dr2_finishFrame(functorSlot);
}
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void btk_setFrame_clamped(void* animation, f32 frame) {
    J3DFrameCtrl* control = reinterpret_cast<J3DFrameCtrl*>(animation);
    if (frame > (f32)control->mEnd) frame = (f32)control->mEnd;
    anm_setFrame_hd(animation, frame, false);
}

// HD: daDr2_HIO_c::daDr2_HIO_c (HD: allocates when this == NULL)
/* 0212D548 */
static daDr2_HIO_c* daDr2_HIO_c_ct(daDr2_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daDr2_HIO_c*)operator_new(0xC);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->mNo = -1;
    i_this->__vtbl = DR2_HIO_VTBL;
    i_this->m08 = 1.0f;
    return i_this;
}

/* 0212AACC */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys.getModel(); /* j3dSys.getModel() */
        dr2_class* i_this = reinterpret_cast<dr2_class*>(model->getUserArea());
        s32 jntNo = reinterpret_cast<Dr2JointData*>(joint)->mJointNo;

        if (i_this != nullptr) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());

            if (i_this->unk_40A == DR2_TAIL_ROCK_SHAKE) {
                /* HD: joints 2..11 (ASHI_L1..KOSHI) */
                if ((u32)(jntNo - 2) <= 9) {
                    cMtx_YrotM(calc_mtx(), i_this->unk_3BC[jntNo]);
                    cMtx_ZrotM(calc_mtx(), i_this->unk_3D4[jntNo]);
                    J3DModel_setAnmMtx(model, jntNo, calc_mtx());
                    PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
                }
            } else if (i_this->unk_40A == DR2_TAIL_PULLED) {
                MtxScale(i_this->unk_40C, 1.0f, 1.0f, true);
                J3DModel_setAnmMtx(model, jntNo, calc_mtx());

                if ((u32)jntNo <= 4) {
                    dr2_rotateY(i_this->unk_3EC, 1);
                    MtxRotZ(i_this->unk_3F0, 1);
                    J3DModel_setAnmMtx(model, jntNo, calc_mtx());
                    PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
                }
            } else if ((u32)(jntNo - 4) <= 7) {
                if (i_this->unk_40A == DR2_TAIL_GRABBED) {
                    cMtx_YrotM(calc_mtx(), i_this->unk_3BC[0]);
                    cMtx_ZrotM(calc_mtx(), i_this->unk_3D4[0]);
                } else {
                    dr2_rotateY(i_this->unk_3EC, 1);
                    MtxRotZ(i_this->unk_3F0, 1);
                }
                J3DModel_setAnmMtx(model, jntNo, calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
            }

            if (jntNo == DR_JNT_J_DR_HANE_L2_e) {
                gabi::Local<cXyz> sp08;
                sp08->x = REG0_F(0) + 215.0f; /* 210.0f + 5.0f */
                sp08->y = REG0_F(1);
                sp08->z = REG0_F(2) + -5.0f; /* -10.0f + 5.0f */
                MtxPosition(sp08.get(), &i_this->unk_3B0);
            }
        }
    }
    return TRUE;
}

/* hahen_draw (inlined) */
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void hahen_draw(dr2_class* i_this) {
    iwa_hahen_s* fragment = i_this->mRockFragments;
    for (s32 i = 0; i < 6; i++, fragment++) {
        if (fragment->unk_04) {
            dScnKy_env_light_c* env = dKy_getEnvlight();
            setLightTevColorType(env, fragment->mpModel, &i_this->tevStr);
            dr2_updateModel(fragment->mpModel);
        }
    }
}

/* iwa_draw (inlined) */
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void iwa_draw(dr2_class* i_this) {
    if (i_this->unk_4BA < 10) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, i_this->unk_418, &i_this->tevStr);
        mDoExt_modelUpdateDL(i_this->unk_418);
        if (i_this->unk_424 != 0) {
            J3DModelData* modelData = J3DModel_getModelData(i_this->unk_41C);
            void* btk = i_this->unk_420;
            dr2_btkEntry(btk, modelData, anm_getFrame(btk));
            env = dKy_getEnvlight();
            setLightTevColorType(env, i_this->unk_41C, &i_this->tevStr);
            mDoExt_modelUpdateDL(i_this->unk_41C);
        }
    }
    hahen_draw(i_this);
}

// HD: the lava floor texture projection follows the view matrix.
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void yuka_texmtx_hd(dr2_class* i_this) {
    Dr2Model* modelView = reinterpret_cast<Dr2Model*>(i_this->unk_42C.get());
    Dr2MaterialNameBlock* names = modelView->mpMaterialNames;
    JUTNameTab* table = names->getNames();
    s32 idx = table->getIndex("test_SC_Yedge_v");
    J3DModel* model = i_this->unk_42C;
    J3DModelData* data = model->mModelData;
    Dr2ShapePacket* shapePacket = reinterpret_cast<Dr2Model*>(model)->mpShapePackets.get() + idx;
    u32 count = data->mMaterialNum;
    Dr2Material* material = dr2_materialBase(data);
    if ((u32)(u16)idx < count) material += (u16)idx;
    Dr2TexGenBlock* textureGeneration = material->mpTexGen;
    Dr2TextureMatrix* textureMatrix = textureGeneration->getTextureMatrix(2);

    gabi::Local<Mtx34> inv;
    dr2_inverse(dr2_viewMatrix(), inv.get());
    dr2_scale(mDoMtx_stack_c::get(), 0.2f, 0.2f, 1.0f);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), 0x4000);
    dr2_concat(mDoMtx_stack_c::get(), inv.get(), mDoMtx_stack_c::get());
    Mtx34* matrix = mDoMtx_stack_c::get();
    // HD: adjacent integer load/store pairs preserve signaling NaN payloads.
    for (int k = 0; k < 11; k++)
        textureMatrix->mProjection[k] = reinterpret_cast<be<u32>*>(matrix)[k];
    f32 m23 = matrix->m[2][3];
    textureMatrix->mRow3 = 0.0f;
    textureMatrix->mRow2 = 1.0f;
    textureMatrix->mRow1 = 0.0f;
    textureMatrix->mRow0 = 0.0f;
    textureMatrix->mTranslation = m23;
    gabi::Local<Mtx34> tmp;
    mtx_copy(tmp.get(), mDoMtx_stack_c::get());
    PSMTXCopy(tmp.get(), &textureMatrix->mEffectMatrix);
    gptr<Mtx34>* slot = dr2_shapeMatrixSlot(shapePacket, 2);
    if (slot != nullptr) *slot = &textureMatrix->mEffectMatrix;
}

/* yuka_draw (inlined) */
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void yuka_draw(dr2_class* i_this) {
    J3DModelData* modelData;

    if (!i_this->unk_43C) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, i_this->unk_428, &i_this->tevStr);
        modelData = J3DModel_getModelData(i_this->unk_428);
        void* btk = i_this->unk_430;
        dr2_btkEntry(btk, modelData, anm_getFrame(btk));
        mDoExt_modelUpdateDL(i_this->unk_428);
    } else {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, i_this->unk_42C, &i_this->tevStr);
        yuka_texmtx_hd(i_this);
        modelData = J3DModel_getModelData(i_this->unk_42C);
        void* btk = i_this->unk_434;
        dr2_btkEntry(btk, modelData, anm_getFrame(btk));
        modelData = J3DModel_getModelData(i_this->unk_42C);
        void* brk = i_this->unk_438;
        dr2_brkEntry(brk, modelData, anm_getFrame(brk));
        mDoExt_modelUpdateDL(i_this->unk_42C);
    }
}

/* dr_draw (inlined) */
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void dr_draw(dr2_class* i_this) {
    if (i_this->unk_50C) {
        J3DModel* model = i_this->mpMorf2->getModel();
        mDoMtx_stack_c::transS(0.0f, REG0_F(5) + 19720.0f /* HD: 10000.0f + 9720.0f folded */, 0.0f);

        gabi::Local<cXyz> sp08;
        f32 s = REG0_F(6) + 1.0f;
        sp08->x = sp08->y = sp08->z = s;
        J3DModel_setBaseScale(model, sp08.get());
        J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
        setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);

        i_this->mpMorf2->play(nullptr, 0, 0);
        i_this->mpMorf2->calc();
        i_this->mpMorf2->entryDL();
    }
}

/* 0212AEEC */
static BOOL daDr2_Draw(dr2_class* i_this) {
    J3DModel* model = i_this->mpMorf1->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    i_this->mpMorf1->entryDL();
    iwa_draw(i_this);
    yuka_draw(i_this);
    dr_draw(i_this);
    return TRUE;
}

/* 0212B380 */
static void* s_a_d_sub(void* ac1, void* ac2) {
    /* HD: fopAcM_GetName checks the pointer */
    if (fopAc_IsActor(ac1) && ac1 != nullptr && fpcM_GetName(ac1) == fpcNm_BTD_e) {
        return ac1;
    }
    return nullptr;
}

/* hahen_move (inlined) */
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void hahen_move(dr2_class* i_this) {
    iwa_hahen_s* fragment = &i_this->mRockFragments[0];

    for (s32 i = 0; i < 6; i++, fragment++) {
        if (fragment->unk_04) {
            PSVECAdd(&fragment->unk_08, &fragment->unk_14, &fragment->unk_08);
            if (i & 1) {
                fragment->unk_20.z = (s16)(fragment->unk_20.z + (REG0_S(4) + 300));
                fragment->unk_20.x = (s16)(fragment->unk_20.x + (REG0_S(3) + 256));
            } else {
                fragment->unk_20.z = (s16)(fragment->unk_20.z - (REG0_S(4) + 300));
                fragment->unk_20.x = (s16)(fragment->unk_20.x - (REG0_S(3) + 256));
            }
            fragment->unk_14.y = fragment->unk_14.y - (REG0_F(7) + 0.5f);
            MtxTrans(fragment->unk_08.x, fragment->unk_08.y, fragment->unk_08.z, false);
            cMtx_YrotM(calc_mtx(), fragment->unk_20.y);
            cMtx_XrotM(calc_mtx(), fragment->unk_20.x);
            cMtx_ZrotM(calc_mtx(), fragment->unk_20.z);

            J3DModel_setBaseTRMtx(fragment->mpModel, calc_mtx());

            if (fragment->unk_08.y < -100.0f) {
                fragment->unk_04 = false;
            }
        }
    }
}

/* iwa_move (inlined) */
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void iwa_move(dr2_class* i_this) {
    fopAc_ac_c* a_this = i_this;

    fopAc_ac_c* btd = i_this->unk_514;
    if (btd == nullptr) {
        btd = (fopAc_ac_c*)fpcM_Search(gabi::guest_fn(reinterpret_cast<const void*>(s_a_d_sub)), i_this);
        i_this->unk_514 = btd;
    }

    if (i_this->unk_4CA != 0) {
        i_this->unk_4CA = (s16)(i_this->unk_4CA - 1);
    }

    switch (i_this->unk_4BA) {
    case DR2_ROCK_RETURN:
        if (std::fabs((f32)(a_this->current.pos.y - a_this->home.pos.y)) > 200.0f) {
            i_this->unk_410 = 0.0f;
        }

        if (i_this->unk_4C8 == 0) {
            cLib_addCalc2(&a_this->current.pos.y, a_this->home.pos.y, 0.5f, 30.0f);

            if (std::fabs((f32)(a_this->current.pos.y - a_this->home.pos.y)) < 1.0f) {
                i_this->unk_4BA = DR2_ROCK_ATTACHED;
            }
        } else {
            i_this->unk_4C8 = (s16)(i_this->unk_4C8 - 1);
        }
        // Fall-through
    case DR2_ROCK_ATTACHED:
        cLib_addCalcAngleS2(&i_this->unk_4B4.x, 0, 1, 0x100);
        cLib_addCalcAngleS2(&i_this->unk_4B4.y, 0, 1, 0x100);

        if (i_this->unk_40A == DR2_TAIL_PULLED || i_this->unk_4CA != 0) {
            if (i_this->unk_4CC == 0) {
                i_this->unk_4CC = (s16)(REG0_S(5) + 3);
                i_this->unk_4CE = (s16)gabi::ftoi(cM_rndFX(REG0_F(7) + 300.0f));
                i_this->unk_4D0 = cM_rndFX(REG0_F(6) + 20.0f);
                if (!i_this->unk_50E) {
                    i_this->unk_510 = 20;
                    i_this->unk_50D = (u8)(REG0_S(5) + 1);
                    i_this->unk_50E = true;
                    seStartCurrent(a_this, JA_SE_CM_BTD_STN_FALL, true);
                }
            } else {
                i_this->unk_4CC = (s16)(i_this->unk_4CC - 1);
            }
        } else {
            i_this->unk_4D0 = 0.0f;
            i_this->unk_4CE = 0;
        }

        cLib_addCalcAngleS2(&i_this->unk_4B4.z, i_this->unk_4CE, 1, (s16)(REG0_S(6) + 0x100));
        cLib_addCalc2(&i_this->unk_4A8.y, (i_this->unk_4D0 + a_this->home.pos.y) - 50.0f, 0.5f, 50.0f);

        if (i_this->unk_4CA == 1) {
            i_this->unk_4BA = (s16)(i_this->unk_4BA + 1);
            s8 roomNo = fopAcM_GetRoomNo(a_this);
            i_this->unk_510 = 1;
            i_this->unk_50D = (u8)(REG0_S(2) + 30);
            i_this->unk_50E = true;
            particle_setToon(dPa_name_ID_AK_ST_BTDSMOKE02, &i_this->unk_4A8, &i_this->unk_4B4, nullptr, 0xB9,
                             (dPa_levelEcallBack*)&i_this->unk_4EC, roomNo);
            seStartCurrent(a_this, JA_SE_CM_BTD_ROCK_FALL, false);
        }
        break;

    case DR2_ROCK_FALL: {
        // HD: assert text names the GameCube variable btd; original guest text is b != (0).
        if (btd == nullptr) /* JUT_ASSERT(0x280, btd != NULL) */
            JUT_ASSERT_fail("d_a_dr2.cpp", 0x280, "btd != NULL");
        f32 spy = a_this->speed.y;
        i_this->unk_4A8.y = i_this->unk_4A8.y + spy;
        a_this->speed.y = spy - (REG0_F(2) + 10.0f);

        f32 fVar11 = reinterpret_cast<Dr2Btd*>(btd)->m02FC[3].y + REG0_F(3);
        if (i_this->unk_4A8.y < fVar11) {
            i_this->unk_4A8.y = fVar11;
            i_this->unk_4BA = DR2_ROCK_RIDE_GOHMA;
            i_this->unk_400[0] = 0x32;
            reinterpret_cast<Dr2Btd*>(btd)->m02F6 = 1;
            seStartCurrent(a_this, JA_SE_CM_BTD_ROCK_HIT, false);
            gabi::Local<cXyz> up;
            up->x = 0.0f;
            up->y = 1.0f;
            up->z = 0.0f;
            StartShock_reg(5, -0x21, up.get());
        }
        break;
    }
    case DR2_ROCK_RIDE_GOHMA: {
        // HD: assert text names the GameCube variable btd; original guest text is b != (0).
        if (btd == nullptr) /* JUT_ASSERT(0x2A5, btd != NULL) */
            JUT_ASSERT_fail("d_a_dr2.cpp", 0x2A5, "btd != NULL");
        i_this->unk_40A = DR2_TAIL_ROCK_SHAKE;
        cLib_addCalc2(&i_this->unk_414, 2000.0f, 0.5f, 100.0f);

        i_this->unk_4A8.y = reinterpret_cast<Dr2Btd*>(btd)->m02FC[3].y + REG0_F(3);
        i_this->unk_4B4.y = i_this->unk_514->current.angle.y;

        f32 lim = REG0_F(6) + 100.0f;
        if (reinterpret_cast<Dr2Btd*>(btd)->m02FC[3].y < lim) {
            i_this->unk_4C0 = lim - reinterpret_cast<Dr2Btd*>(btd)->m02FC[3].y;
            i_this->unk_4C0 = i_this->unk_4C0 * (REG0_F(7) + 0.3f);
        }

        gabi::Local<cXyz> sp4C;
        cXyz_mi(&reinterpret_cast<Dr2Btd*>(btd)->m02FC[3], sp4C.get(), &reinterpret_cast<Dr2Btd*>(btd)->m02FC[4]);
        f32 y = sp4C->y + ((REG0_F(5) + -330.0f) + i_this->unk_4C0);
        f32 x = sp4C->x, z = sp4C->z;
        f32 xz = std_sqrtf(gabi::fmadds(x, x, z * z));
        cLib_addCalcAngleS2(&i_this->unk_4B4.x, (s16)-cM_atan2s(y, xz), 1, 0x200);

        if (!reinterpret_cast<Dr2Btd*>(btd)->m02F6) {
            i_this->unk_4BA = DR2_ROCK_RISE;
            dComIfGp_particle_set(dPa_name_ID_AK_SN_BTDDRIPMAGMA00, &i_this->unk_4A8, &i_this->unk_4B4, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&i_this->unk_4D8);
            i_this->unk_424 = 1;
        }
        break;
    }
    case DR2_ROCK_RISE:
        // HD: assert text names the GameCube variable btd; original guest text is b != (0).
        if (btd == nullptr) /* JUT_ASSERT(0x2E8, btd != NULL) */
            JUT_ASSERT_fail("d_a_dr2.cpp", 0x2E8, "btd != NULL");
        i_this->unk_4A8.y = reinterpret_cast<Dr2Btd*>(btd)->m02FC[8].y + 100.0f + REG0_F(9);
        cLib_addCalcAngleS2(&i_this->unk_4B4.x, 0, 1, 0x100);
        cLib_addCalcAngleS2(&i_this->unk_4B4.y, 0, 1, 0x100);

        if (i_this->unk_4A8.y > a_this->home.pos.y - 1200.0f) {
            a_this->current.pos.y = a_this->current.pos.y + (REG0_F(9) + 100.0f);
            i_this->unk_40A = DR2_TAIL_IDLE;
        }

        if (i_this->unk_4A8.y > a_this->home.pos.y - 50.0f) {
            i_this->unk_4A8.y = a_this->home.pos.y - 50.0f;
            a_this->speed.y = 0.0f;
            i_this->unk_4BA = DR2_ROCK_RETURN;
            i_this->unk_4C0 = 0.0f;
            i_this->unk_4C8 = 0x82;
            dPa_followEcallBack_end(&i_this->unk_4D8); /* HD: unk_4D8.remove() is end() */
            s8 roomNo = fopAcM_GetRoomNo(a_this);
            particle_setToon(dPa_name_ID_AK_ST_BTDSMOKE02, &i_this->unk_4A8, &i_this->unk_4B4, nullptr, 0xB9,
                             (dPa_levelEcallBack*)&i_this->unk_4EC, roomNo);

            i_this->unk_510 = 2;
            i_this->unk_50D = (u8)(REG0_S(2) + 10);
            i_this->unk_50E = true;

            gabi::Local<cXyz> up;
            up->x = 0.0f;
            up->y = 1.0f;
            up->z = 0.0f;
            StartShock_reg(5, -0x21, up.get());
            seStartCurrent(a_this, JA_SE_CM_BTD_ROCK_ATTACH, false);
        }
        break;

    case DR2_ROCK_BREAK: {
        i_this->unk_4BA = (s16)(i_this->unk_4BA + 1);

        iwa_hahen_s* fragment = &i_this->mRockFragments[0];
        for (s32 i = 0; i < 6; i++, fragment++) {
            if (!fragment->unk_04) {
                fragment->unk_04 = true;
                fragment->unk_08.copy(i_this->unk_4A8);
                fragment->unk_20.x = i_this->unk_4B4.x;
                fragment->unk_20.y = i_this->unk_4B4.y;
                fragment->unk_20.z = i_this->unk_4B4.z;
                cMtx_YrotS(calc_mtx(), fragment->unk_20.y);
                cMtx_XrotM(calc_mtx(), fragment->unk_20.x);
                gabi::Local<cXyz> sp58;
                f32 k = REG0_F(2) + 3.0f;
                sp58->x = hsx[i] * k;
                sp58->y = 0.0f;
                sp58->z = hsz[i] * k;
                MtxPosition(sp58.get(), &fragment->unk_14);
            }
        }
    }
        // Fall-through
    case DR2_ROCK_GONE:
        i_this->unk_40A = DR2_TAIL_IDLE;
        break;
    }

    switch (i_this->unk_50E) {
    case 1:
        if (i_this->unk_4D4 != nullptr) {
            JPABaseEmitter_becomeInvalidEmitter(i_this->unk_4D4);
        }

        i_this->unk_4D4 = dComIfGp_particle_set(dPa_name_ID_AK_SN_BTDBROKENROCK00, &i_this->unk_4A8);
        if (i_this->unk_4D4 != nullptr) {
            JPABaseEmitter_setRate(i_this->unk_4D4, (f32)(u8)i_this->unk_50D);
            i_this->unk_50E = (u8)(i_this->unk_50E + 1);
        }
        break;

    case 2: {
        if (i_this->unk_510 != 0) {
            i_this->unk_510 = (s16)(i_this->unk_510 - 1);
            if (i_this->unk_510 == 0 && i_this->unk_4D4 != nullptr) {
                JPABaseEmitter_becomeInvalidEmitter(i_this->unk_4D4);
            }
        }

        JPABaseEmitter* em = i_this->unk_4D4;
        if (em != nullptr) {
            if (JPABaseEmitter_isEnableDeleteEmitter(em)) {
                i_this->unk_50E = false;
                i_this->unk_4D4 = nullptr;
                break;
            }
        }

        Dr2ParticleLink* link = reinterpret_cast<Dr2Emitter*>(em)->mpFirst;
        while (link != nullptr) {
            Dr2Particle* ptcl = link->mpObject;
            gabi::Local<cXyz> sp40;
            f32 py = ptcl->mGlobalPosition.y;
            f32 pz = ptcl->mGlobalPosition.z;
            f32 px = ptcl->mGlobalPosition.x;
            sp40->z = pz;
            sp40->y = py;
            sp40->x = px;
            dComIfGp_particle_setSimple(dPa_name_ID_AK_SN_O_BTDBROKENROCKTAIL00, sp40.get(), 0xB9);
            link = link->mpNext;
        }
        break;
    }
    }

    if (i_this->unk_424 != 0) {
        btk_setFrame_clamped(i_this->unk_420, (f32)i_this->unk_424);
        s16 n = (s16)(i_this->unk_424 + 1);
        if (n > 180) {
            i_this->unk_424 = 0;
        } else {
            i_this->unk_424 = n;
        }
    }

    hahen_move(i_this);
}

/* move (inlined) */
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void move(dr2_class* i_this) {
    fopAc_ac_c* a_this = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* foundActor = fopAcM_SearchByID(i_this->unk_3FC);

    if (i_this->unk_40A == DR2_TAIL_ROCK_SHAKE) {
        for (s32 i = 2; i < 12; i++) {
            i_this->unk_3BC[i] = (s16)gabi::ftoi(cM_ssin(i_this->unk_2BC * 2800 + i * 3000) * i_this->unk_414);
            i_this->unk_3D4[i] = (s16)gabi::ftoi(cM_scos(i_this->unk_2BC * 2300 + i * 2000) * i_this->unk_414);
        }
    } else {
        i_this->unk_40A = DR2_TAIL_IDLE;
        if (foundActor != nullptr && foundActor->health == 3) {
            // HD: copy the left hand position before the subtraction call.
            gabi::Local<cXyz> hand;
            cXyz* lh = &reinterpret_cast<Dr2Player*>(player)->mLeftHandPosition;
            hand->x = lh->x;
            hand->y = lh->y;
            hand->z = lh->z;
            gabi::Local<cXyz> diff;
            cXyz_mi(hand.get(), diff.get(), &foundActor->current.pos);
            gabi::Local<cXyz> sp30;
            sp30->copy(*diff);
            gabi::Local<cXyz> sp24;

            cMtx_YrotS(calc_mtx(), (s16)-player->shape_angle.y);
            MtxPosition(sp30.get(), sp24.get());

            s32 atan = cM_atan2s(sp24->z, -sp24->y);
            i_this->current.angle.y = player->shape_angle.y;
            s16 atan2 = (s16)gabi::ftoi((f32)atan * (REG0_F(3) + -0.125f));

            cLib_addCalcAngleS2(&i_this->unk_3BC[0], atan2, 0x10, 0x40);
            cLib_addCalcAngleS2(&i_this->unk_3D4[0], 0, 0x10, 0x40);
            i_this->unk_40A = DR2_TAIL_GRABBED;
        } else if (foundActor != nullptr && foundActor->health == 2) {
            i_this->unk_40A = DR2_TAIL_PULLED;
            s16 ang = fopAcM_searchPlayerAngleY(a_this);
            i_this->current.angle.y = ang;
            player->shape_angle.y = (s16)(ang - -0x8000);
            player->current.angle.y = (s16)(i_this->current.angle.y - -0x8000);

            cLib_addCalc2(&i_this->unk_3EC, REG0_F(9) + -0.175f, 0.5f, REG0_F(10) + 0.0125f);
            cLib_addCalc0(&i_this->unk_3F0, 0.5f, 0.10000000149011612f);

            if (i_this->unk_3EC < -0.1f) {
                cLib_addCalc2(&i_this->unk_40C, 1.04f, 0.1f, REG0_F(14) + 0.005f);
            }
        } else {
            f32 t = i_this->unk_3F4 + 0.04f;
            i_this->current.angle.y = i_this->home.angle.y;
            i_this->unk_40C = 0.98f;
            i_this->unk_3F4 = t;
            if (t > 6.2831855f) {
                i_this->unk_3EC = i_this->unk_3EC - 6.2831855f;
            }

            t = i_this->unk_3F8 + 0.05f;
            i_this->unk_3F8 = t;
            if (t > 6.2831855f) {
                i_this->unk_3F0 = i_this->unk_3F0 - 6.2831855f;
            }

            i_this->unk_3EC = cM_fsin(i_this->unk_3F4) * i_this->unk_410;
            i_this->unk_3F0 = cM_fsin(i_this->unk_3F8) * i_this->unk_410;

            if (foundActor != nullptr && foundActor->health == 1) {
                cLib_addCalc2(&i_this->unk_410, 0.0f, 0.5f, 0.001f);
                i_this->unk_4CC = 20;
            } else {
                cLib_addCalc2(&i_this->unk_410, 0.048f, 0.5f, 0.001f);
            }
        }
    }

    i_this->mpMorf1->play(&a_this->eyePos, 0, 0);
    i_this->mpMorf1->calc();
}

/* dr_move (inlined) */
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void dr_move(dr2_class* i_this) {
    if (i_this->unk_4C6 != 0) {
        i_this->unk_4C6 = (s16)(i_this->unk_4C6 - 1);
    }

    switch (i_this->unk_4C4) {
    case 0:
        if (i_this->unk_50C) {
            i_this->unk_4C6 = 0;
            J3DAnmTransform* pAnimRes =
                (J3DAnmTransform*)dComIfG_getObjectRes("Dr2", dRes_INDEX_DR2_BCK_DR_BOSS_DEMO1_e, SAFESTRING_VTBL);
            i_this->mpMorf2->setAnm(pAnimRes, 0, 1.0f, 1.0f, 0.0f, -1.0f, nullptr);
            i_this->unk_4C4 = 1;
        }
        break;

    case 1:
        if (!i_this->unk_50C) {
            i_this->unk_4C4 = false;
        }
        break;
    }
}

/* yuka_move (inlined) */
/* inlined into daDr2_Execute / daDr2_Draw */
static inline void yuka_move(dr2_class* i_this) {
    // HD: the stage boss flag is read through the save object before the stage name.
    if (dComIfGs_isStageBossEnemy() &&
        *reinterpret_cast<const be<u8>*>(dComIfGp_getStartStageName()) != 'X' /* dComIfGp_getStartStageName()[0] */) {
        i_this->unk_43C = true;
        anm_setFrame_hd(i_this->unk_434, 299.0f, false);
        anm_setFrame_hd(i_this->unk_438, 299.0f, true);
    }

    gabi::Local<Mtx34> sp08;
    f32 s = REG0_F(9) + 1.0f;
    dr2_scale(sp08.get(), s, s, s);

    if (i_this->unk_43C) {
        MtxTrans(0.0f, -10000.0f, 0.0f, false);
    } else {
        MtxTrans(0.0f, 0.0f, 0.0f, false);
        dr2_playAnimation(i_this->unk_430);
    }

    J3DModel_setBaseTRMtx(i_this->unk_428, calc_mtx());
    dr2_concat(calc_mtx(), sp08.get(), &i_this->unk_440);

    dBgW_Move(i_this->mpBgW1);
    if (!i_this->unk_43C) {
        MtxTrans(0.0f, -10000.0f, 0.0f, false);
    } else {
        MtxTrans(0.0f, 0.0f, 0.0f, false);
        dr2_playAnimation(i_this->unk_434);
        dr2_playAnimation(i_this->unk_438);
    }

    J3DModel_setBaseTRMtx(i_this->unk_42C, calc_mtx());
    dr2_concat(calc_mtx(), sp08.get(), &i_this->unk_470);
    dBgW_Move(i_this->mpBgW2);
}

/* 0212B3D0 */
static BOOL daDr2_Execute(dr2_class* i_this) {
    i_this->unk_2BC = (s16)(i_this->unk_2BC + 1);

    for (s32 i = 0; i < 5; i++) {
        if (i_this->unk_400[i] != 0) {
            i_this->unk_400[i] = (s16)(i_this->unk_400[i] - 1);
        }
    }

    move(i_this);

    fopAcM_SearchByID(i_this->unk_3FC);
    f32 s = l_HIO.m08;
    i_this->scale.z = s;
    i_this->scale.y = s;
    i_this->scale.x = s;

    J3DModel* model = i_this->mpMorf1->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    J3DModel_setBaseScale(model, &i_this->scale);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->current.angle.y);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

    iwa_move(i_this);

    mDoMtx_stack_c::transS(i_this->unk_4A8.x, i_this->unk_4A8.y, i_this->unk_4A8.z);

    mDoMtx_stack_c::YrotM(i_this->unk_4B4.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->unk_4B4.x);
    mDoMtx_stack_c::YrotM(i_this->home.angle.y);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->unk_4B4.z);
    mDoMtx_stack_c::YrotM((s16)-i_this->unk_4B4.y);

    J3DModel_setBaseTRMtx(i_this->unk_418, mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(i_this->unk_41C, mDoMtx_stack_c::get());

    dr_move(i_this);
    yuka_move(i_this);
    return TRUE;
}

/* 0212CD08 */
static BOOL daDr2_IsDelete(dr2_class*) {
    return TRUE;
}

/* 0212CD10 */
static BOOL daDr2_Delete(dr2_class* i_this) {
    dComIfG_resDelete(&i_this->mPhase, "Dr2");
    mDoHIO_deleteChild(l_HIO.mNo);
    i_this->unk_4D8.remove();
    dr2_removeSmoke(&i_this->unk_4EC); /* unk_4EC.remove() */
    mDoAud_seDeleteObject(&i_this->unk_4A8);
    dBgS* bgs = dComIfG_Bgsp();
    cBgS_Release(bgs, i_this->mpBgW1);
    bgs = dComIfG_Bgsp();
    cBgS_Release(bgs, i_this->mpBgW2);
    return TRUE;
}

/* inlined into daDr2_Execute / daDr2_Draw */
static inline void* new_btkAnm() {
    void* p = operator_new(0x74);
    if (p != nullptr) p = dr2_btkCtor(p); /* mDoExt_btkAnm::mDoExt_btkAnm */
    return p;
}
/* inlined into daDr2_Execute / daDr2_Draw */
static inline BOOL btkAnm_init(void* a, J3DModelData* d, void* key, s32 mode) {
    return dr2_btkInit(a, d, key, (u32)1, mode, 1.0f, (s16)0, (s16)-1, (u32)0, (s32)0);
}

/* 0212CDAC */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    dr2_class* i_this = (dr2_class*)a_this;

    J3DModelData* md = (J3DModelData*)dComIfG_getObjectRes("Dr2", dRes_INDEX_DR2_BMD_DR_SIPPO_e, SAFESTRING_VTBL);
    i_this->mpMorf1 = mDoExt_McaMorf::create(nullptr, md, nullptr, nullptr, nullptr, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0,
                                             nullptr, 0, 0x11020203);

    mDoExt_McaMorf* morf = i_this->mpMorf1;
    if ((morf == nullptr) || (morf->getModel() == nullptr)) {
        return FALSE;
    }

    for (u16 i = 0; i < J3DModel_getModelData(i_this->mpMorf1->getModel())->getJointNum(); i++) {
        J3DModel_getModelData(i_this->mpMorf1->getModel())->getJointNodePointer(i)->setCallBack(nodeCallBack);
    }
    i_this->mpMorf1->getModel()->mUserArea = gabi::ea(i_this); /* setUserArea */

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Dr2", dRes_INDEX_DR2_BDL_IWA00_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(1404, modelData != NULL) */
        JUT_ASSERT_fail("d_a_dr2.cpp", 0x57C, "modelData != NULL");
    i_this->unk_418 = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->unk_418 == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)dComIfG_getObjectRes("Dr2", dRes_INDEX_DR2_BMD_GAN_MAGMA_e, SAFESTRING_VTBL);
    if (modelData == nullptr)
        JUT_ASSERT_fail("d_a_dr2.cpp", 0x585, "modelData != NULL");
    i_this->unk_41C = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->unk_41C == nullptr) {
        return FALSE;
    }

    i_this->unk_420 = new_btkAnm();
    if (i_this->unk_420 == nullptr) {
        return FALSE;
    }
    {
        J3DModel* m = i_this->unk_41C;
        void* srtKey = dComIfG_getObjectRes("Dr2", dRes_INDEX_DR2_BTK_GAN_MAGMA_e, SAFESTRING_VTBL);
        if (!btkAnm_init(i_this->unk_420, J3DModel_getModelData(m), srtKey, J3DFrameCtrl::EMode_NONE)) {
            return FALSE;
        }
    }

    for (s32 i = 0; i < 6; i++) {
        s32 idx = hahen_model[i];
        modelData = (J3DModelData*)dComIfG_getObjectRes("Dr2", idx, SAFESTRING_VTBL);
        i_this->mRockFragments[i].mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        if (i_this->mRockFragments[i].mpModel == nullptr) {
            return FALSE;
        }
    }

    {
        J3DModelData* d2 = (J3DModelData*)dComIfG_getObjectRes("Dr2", dRes_INDEX_DR2_BMD_DR_e, SAFESTRING_VTBL);
        J3DAnmTransform* a2 = (J3DAnmTransform*)dComIfG_getObjectRes("Dr2", dRes_INDEX_DR2_BCK_DR_BOSS_DEMO1_e, SAFESTRING_VTBL);
        i_this->mpMorf2 = mDoExt_McaMorf::create(nullptr, d2, nullptr, nullptr, a2, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0,
                                                 nullptr, 0, 0x11020203);
    }
    morf = i_this->mpMorf2;
    if (morf == nullptr || morf->getModel() == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)dComIfG_getObjectRes("Dr2", dRes_INDEX_DR2_BMD_MBYO1_e, SAFESTRING_VTBL);
    if (modelData == nullptr)
        JUT_ASSERT_fail("d_a_dr2.cpp", 0x5C0, "modelData != NULL");
    i_this->unk_428 = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->unk_428 == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)dComIfG_getObjectRes("Dr2", dRes_INDEX_DR2_BMD_MBYO2_e, SAFESTRING_VTBL);
    if (modelData == nullptr)
        JUT_ASSERT_fail("d_a_dr2.cpp", 0x5C7, "modelData != NULL");
    i_this->unk_42C = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->unk_42C == nullptr) {
        return FALSE;
    }

    i_this->unk_430 = new_btkAnm();
    if (i_this->unk_430 == nullptr) {
        return FALSE;
    }
    {
        J3DModel* m = i_this->unk_428;
        void* srtKey = dComIfG_getObjectRes("Dr2", dRes_INDEX_DR2_BTK_MBYO1_e, SAFESTRING_VTBL);
        if (!btkAnm_init(i_this->unk_430, J3DModel_getModelData(m), srtKey, J3DFrameCtrl::EMode_LOOP)) {
            return FALSE;
        }
    }

    i_this->unk_434 = new_btkAnm();
    if (i_this->unk_434 == nullptr) {
        return FALSE;
    }
    {
        J3DModel* m = i_this->unk_42C;
        void* srtKey = dComIfG_getObjectRes("Dr2", dRes_INDEX_DR2_BTK_MBYO2_e, SAFESTRING_VTBL);
        if (!btkAnm_init(i_this->unk_434, J3DModel_getModelData(m), srtKey, J3DFrameCtrl::EMode_NONE)) {
            return FALSE;
        }
    }

    {
        void* p = operator_new(0x78);
        if (p != nullptr) p = dr2_brkCtor(p); /* mDoExt_brkAnm::mDoExt_brkAnm */
        i_this->unk_438 = p;
    }
    if (i_this->unk_438 == nullptr) {
        return FALSE;
    }
    {
        J3DModel* m = i_this->unk_42C;
        void* tevRegKey = dComIfG_getObjectRes("Dr2", dRes_INDEX_DR2_BRK_MBYO2_e, SAFESTRING_VTBL);
        if (!dr2_brkInit(i_this->unk_438.get(), J3DModel_getModelData(m), tevRegKey, (u32)1, (s32)0, 1.0f, (s16)0,
                              (s16)-1, (u32)0, (s32)0)) {
            return FALSE;
        }
    }

    i_this->mpBgW1 = new_dBgW();
    if (i_this->mpBgW1 == nullptr) {
        return FALSE;
    }

    i_this->mpBgW2 = new_dBgW();
    if (i_this->mpBgW2 == nullptr) {
        return FALSE;
    }

    cBgD_t* cBgD = (cBgD_t*)dComIfG_getObjectRes("Dr2", dRes_INDEX_DR2_DZB_MBYO1_e, SAFESTRING_VTBL);
    cBgW_Set(i_this->mpBgW1, cBgD, cBgW_MOVE_BG_e, &i_this->unk_440);
    dr2_setMoveBgCorrection(i_this->mpBgW1); /* SetCrrFunc(dBgS_MoveBGProc_Typical) */

    cBgD = (cBgD_t*)dComIfG_getObjectRes("Dr2", dRes_INDEX_DR2_DZB_MBYO2_e, SAFESTRING_VTBL);
    cBgW_Set(i_this->mpBgW2, cBgD, cBgW_MOVE_BG_e, &i_this->unk_470);
    dr2_setMoveBgCorrection(i_this->mpBgW2);

    return TRUE;

}

/* 0212D394 */
static cPhs_State daDr2_Create(fopAc_ac_c* a_this) {
    dr2_class* i_this = (dr2_class*)a_this;

    /* fopAcM_ct(&i_this->actor, dr2_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = DR2_VTBL;
            dPa_followEcallBack_ct(&i_this->unk_4D8, 0, 0);
            dr2_smokeCtor(&i_this->unk_4EC, (u8)1); /* dPa_smokeEcallBack::dPa_smokeEcallBack(1) */
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, "Dr2");
    if (ret == cPhs_COMPLEATE_e) {
        i_this->unk_4EC.mFollowOff = 1; /* unk_4EC.setFollowOff() */
        if (!fopAcM_entrySolidHeap(a_this, gabi::guest_fn(reinterpret_cast<const void*>(useHeapInit)), 0xDFE0)) {
            return cPhs_ERROR_e;
        }

        dBgS* bgs = dComIfG_Bgsp();
        if (dBgS_Regist(bgs, i_this->mpBgW1, a_this)) {
            return cPhs_ERROR_e;
        }

        bgs = dComIfG_Bgsp();
        if (dBgS_Regist(bgs, i_this->mpBgW2, a_this)) {
            return cPhs_ERROR_e;
        }

        // HD: glue retains the original Shift-JIS HIO label bytes.
        l_HIO.mNo = mDoHIO_createChild("Dragon Tail", &l_HIO);
        /* fopAcM_Create(fpcNm_KUI_e, NULL, params) (HD inline) */
        Dr2CreateAppend* params = dr2_createAppend(); /* fopAcM_CreateAppend */
        params->mParameter = 0x511;
        params->mRoomNo = a_this->current.roomNo;
        u32 layer = dr2_currentLayer(); /* fpcLy_CurrentLayer */
        i_this->unk_3FC = dr2_createRequest(layer, (s16)fpcNm_KUI_e, (u32)0, (u32)0, params); /* fpcSCtRq_Request */
        i_this->unk_4A8.x = a_this->home.pos.x;
        i_this->unk_4A8.y = a_this->home.pos.y + REG0_F(7) - 50.0f;
        i_this->unk_4A8.z = a_this->home.pos.z;
    }
    return ret;
}

// HD: compiler-generated and template code of this translation unit.
/* 0212D59C */
static void __sinit_d_a_dr2_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
    daDr2_HIO_c_ct(&l_HIO); /* static daDr2_HIO_c l_HIO */
}

// HD: deleting destructor of a class with a trivial destructor (daDr2_HIO_c)
/* 0212D63C */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: dr2_class deleting destructor (HD)
/* 0212D650 */
static void dr2_class_dt(dr2_class* p, s32 flags) {
    if (p != nullptr) {
        dr2_actorDtor(p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}

// HD: empty virtual
/* 0212D6A4 */
static void empty_virtual(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py)
ext 0200FBA4 void dr2_rotateY(f32 radians, u8 concatenate)
ext 025E2DA8 void dr2_updateModel(J3DModel* model)
ext 02019510 s16 dr2_radiansToAngle(f32 radians)
ext 028E91EC void dr2_inverse(const Mtx34* source, Mtx34* result)
ext 028E945C void dr2_scale(Mtx34* matrix, f32 x, f32 y, f32 z)
ext 028E9108 void dr2_concat(const Mtx34* left, const Mtx34* right, Mtx34* result)
ext 025E83FC void dr2_brkEntry(void* animation, J3DModelData* data, f32 frame)
ext 025E7FC4 void dr2_btkEntry(void* animation, J3DModelData* data, f32 frame)
ext 025CB374 void dr2_shock(dVibration_c* vibration, s32 strength, s32 flags, cXyz* position)
ext 025E742C void dr2_playAnimation(void* animation)
ext 027DF40C void dr2_finishFrame(gptr<Dr2FrameFunctor>* functor)
ext 027FA678 gptr<Mtx34>* dr2_shapeMatrixSlot(Dr2ShapePacket* packet, s32 index)
ext 025E7C6C void* dr2_btkCtor(void* animation)
ext 025E7CE0 BOOL dr2_btkInit(void* animation, J3DModelData* data, void* key, u32 play, s32 mode, f32 rate, s16 start, s16 end, u32 modify, s32 entry)
ext 025E80D0 void* dr2_brkCtor(void* animation)
ext 025E8154 BOOL dr2_brkInit(void* animation, J3DModelData* data, void* key, u32 play, s32 mode, f32 rate, s16 start, s16 end, u32 modify, s32 entry)
ext 025A5B18 void dr2_smokeCtor(Dr2SmokeCallback* callback, u8 mode)
ext 025D5600 Dr2CreateAppend* dr2_createAppend()
ext 025DED64 u32 dr2_currentLayer()
ext 025E14A8 u32 dr2_createRequest(u32 layer, s16 process, u32 callback, u32 data, Dr2CreateAppend* append)
ext 025D50BC void dr2_actorDtor(fopAc_ac_c* actor, s32 flags)
str 1000E108 "test_SC_Yedge_v" in 0212AEEC
str 1000E0CC "Dr2" in 0212B3D0
str 1000E1D8 "Dr2" in 0212CD10
str 1000E1DC "Dr2" in 0212CDAC
str 1000E200 "Dr2" in 0212D394
str 1000E1E0 "d_a_dr2.cpp" in 0212CDAC
str 1000E118 "d_a_dr2.cpp" in 0212B3D0
str 1000E124 "btd != NULL" renamed in 0212B3D0
str 1000E1EC "modelData != NULL" in 0212CDAC
str 1000E204 "Dragon Tail" renamed in 0212D394
data l_HIO 10463C50 nocheck
data hsx 101B4778
data hsz 101B4790
data hahen_model 101B47C8
*/
