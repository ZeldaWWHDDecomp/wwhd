/**
 * d_a_mt.cpp (WWHD)
 * Enemy - Magtail
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_mt.cpp) has only "Nonmatching" placeholders for this unit, so every function
 * here is written from the WWHD code (cking.rpx) and verified against it. GameCube names are
 * kept where the WWHD function matches the GameCube symbol; functions the GameCube had out of
 * line but WWHD inlines (mt_check, mt_eye_tex_anm, wall_check_sub, br_draw, ...) are inline
 * helpers here.
 */
#include "d/actor/d_a_mt_main.h"

const gc_tu_data gc_tu = {.safeString = MT_SAFESTRING_VTBL};
static u32 followEcallBack_ct(dPa_followEcallBack *);
static mt_class *mt_class_ct(mt_class *);
static void followEcallBack_dt(void *, s32);
static dCcD_SrcSph eye_sph_src = {};
static dCcD_SrcSph at_sph_src = {};
static u16 l_tex_anm_idx[2] = {54, 55};
static u16 l_tex_max_frame[2] = {6, 1};
static s16 l_br_angle[11] = {-13000, -9000, -4000, 0, 0, 0, 0, 0, 0, 0, 0};
static u8 l_br_model[11] = {0, 1, 1, 2, 2, 2, 1, 0, 0, 0, 0};
static u32 l_bmd_idx[8] = {20, 19, 19, 19, 19, 19, 19, 21};
static u32 l_btk_idx[8] = {46, 44, 44, 44, 44, 44, 44, 48};
static u32 l_brk_idx[8] = {34, 32, 32, 32, 32, 32, 32, 36};
static f32 l_part_scale[8] = {1.0f, 1.0f, 1.0f, 0.975f, 0.925f, 0.825f, 0.75f, 0.525f};
static u32 l_br_bmd_idx[3] = {13, 14, 15};

// HD: GHS constructor wrapper allocates when this is NULL.
/* 021DD330 */
static daMt_HIO_c *daMt_HIO_c_ct(daMt_HIO_c *self) {
    if (self == nullptr) {
        self = (daMt_HIO_c *)operator_new(sizeof(daMt_HIO_c));
        if (self == nullptr)
            return self;
    }
    self->__vtbl = MT_HIO_VTBL;
    self->mNo = 0;
    self->m05 = 0;
    self->m06 = 0;
    self->m07 = 0;
    self->m08 = 0x157C;
    self->m0C = -7500.0f;
    self->m10 = 15;
    self->m14 = 0.5f;
    self->m18 = 21.0f;
    self->m1C = 0.8f;
    self->m20 = 0.8f;
    self->m24 = 450.0f;
    self->m28 = 350.0f;
    self->m2C = 30.0f;
    self->m30 = 0x11;
    self->m32 = 0x28;
    self->m34 = 400.0f;
    self->m38 = 0x2D;
    self->m3A = 0x46;
    self->m3C = 0x2B;
    self->m3E = 0x2F;
    self->m40 = 55.0f;
    self->m44 = 30.0f;
    self->m48 = 25.0f;
    self->m4C = 0.65f;
    self->m50 = -5;
    self->m52 = 0x2AF8;
    self->m54 = 600;
    self->m58 = 1.0f;
    self->m5C = 1.0f;
    return self;
}

// HD: soundFileIdx is unused; animation goes to the head morf without a sound table.
/* 021D73C0 */
void anm_init(mt_class *i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed,
              int soundFileIdx) {
    J3DAnmTransform *anm =
        (J3DAnmTransform *)dComIfG_getObjectRes("Mt", bckFileIdx, MT_SAFESTRING_VTBL);
    i_this->mpMorf[0]->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
}

/* 021D7480 */
static void *mt_a_d_sub(void *param_1, void *) {
    if (fopAc_IsActor(param_1) && param_1 != nullptr &&
        fpcM_GetName(param_1) == cc_uty::fpcNm_MT_e) {
        mt_all_count() += 1;
        if (((mt_class *)param_1)->m570 == MT_STATE_FIGHT) {
            mt_fight_count() += 1;
        }
    }
    return nullptr;
}

/* 021D74F8 */
void mt_bg_check(mt_class *i_this) {
    f32 offset = REG0_F(3) + 40.0f;
    i_this->actor_status |= fopAcStts_FREEZE_e;
    i_this->current.pos.y -= offset;
    i_this->old.pos.y -= offset;
    dBgS_Acch_CrrPos(&i_this->mAcch, dComIfG_Bgsp());
    f32 y = i_this->current.pos.y + offset;
    i_this->old.pos.y += offset;
    i_this->current.pos.y = y;
    if (i_this->home.pos.y - y > 1000.0f) {
        if (y - i_this->mAcch.m_ground_h > 5000.0f) {
            i_this->m1DD4 = 1;
            fopAcM_delete(i_this);
        }
    }
}

/* tex_anm_set tables (.data): btp resource index and its last frame */
// HD: J3DAnmBase stores the frame before its vtable.
static inline void J3DAnm_setFrame(void *anm, f32 f) { ((MtAnmView *)anm)->mFrame = f; }

/* 021D75D8 */
void tex_anm_set(mt_class *i_this, u16 idx) {
    i_this->mBtpOn = 1;
    J3DAnmTexPattern *btp =
        (J3DAnmTexPattern *)dComIfG_getObjectRes("Mt", l_tex_anm_idx[idx], MT_SAFESTRING_VTBL);
    i_this->mpBtpRes = btp;
    i_this->mBtpMaxFrame = (u8)l_tex_max_frame[idx];
    i_this->mBtpFrame = 0;
    J3DAnm_setFrame(btp, 0.0f);
}

/* the joint's animation matrix back from calc_mtx, and into J3DSys::mCurrentMtx */
static inline void mt_node_store(J3DModel_l *model, u32 jntNo) {
    mtx_copy(model_getAnmMtx(model, jntNo), calc_mtx());
    PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
}

/* 021D7668 */
static BOOL nodeCallBack_head(J3DNode *node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DJoint *joint = J3DNode_toJoint(node);
        J3DModel_l *model = j3dSys_getModel();
        mt_class *i_this = ((MtModelView *)model)->mpActor;
        u32 jntNo = ((MtJointView *)joint)->mJntNo;
        if (i_this != nullptr) {
            PSMTXCopy(model_getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo == MT_JNT_BEND_FIRST) {
                mDoMtx_YrotM(calc_mtx(), (s16)-i_this->m75C[0].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m75C[0].z);
            } else if (jntNo == MT_JNT_BEND_SECOND) {
                mDoMtx_YrotM(calc_mtx(), i_this->m7B6[0].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m7B6[0].z);
            } else {
                MtxScale(i_this->mHeadScale, i_this->mHeadScale, i_this->mHeadScale, 1);
            }
            mt_node_store(model, jntNo);
        }
    }
    return TRUE;
}

/* nodeCallBack_body (not named by the matcher). body_idx() is the segment being
 * calculated: joints 2/3 take m75C[idx+1]/[idx+2], joints 4/5 m7B6[idx+1]/[idx+2], then idx += 2 */
/* 021D78F4 */
static BOOL nodeCallBack_body(J3DNode *node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DJoint *joint = J3DNode_toJoint(node);
        J3DModel_l *model = j3dSys_getModel();
        mt_class *i_this = ((MtModelView *)model)->mpActor;
        u32 jntNo = ((MtJointView *)joint)->mJntNo;
        if (i_this != nullptr) {
            PSMTXCopy(model_getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo == MT_JNT_BEND_FIRST) {
                mDoMtx_YrotM(calc_mtx(), (s16)-i_this->m75C[body_idx() + 1].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m75C[body_idx() + 1].z);
            } else if (jntNo == MT_JNT_BEND_SECOND) {
                mDoMtx_YrotM(calc_mtx(), (s16)-i_this->m75C[body_idx() + 2].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m75C[body_idx() + 2].z);
            } else if (jntNo == MT_JNT_TWIST_FIRST) {
                mDoMtx_YrotM(calc_mtx(), i_this->m7B6[body_idx() + 1].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m7B6[body_idx() + 1].z);
            } else if (jntNo == MT_JNT_TWIST_SECOND) {
                mDoMtx_YrotM(calc_mtx(), i_this->m7B6[body_idx() + 2].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m7B6[body_idx() + 2].z);
                body_idx() += 2;
            }
            mt_node_store(model, jntNo);
        }
    }
    return TRUE;
}

/* 021D7CC4 */
static BOOL nodeCallBack_tail(J3DNode *node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DJoint *joint = J3DNode_toJoint(node);
        J3DModel_l *model = j3dSys_getModel();
        mt_class *i_this = ((MtModelView *)model)->mpActor;
        u32 jntNo = ((MtJointView *)joint)->mJntNo;
        if (i_this != nullptr) {
            PSMTXCopy(model_getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo == MT_JNT_BEND_FIRST) {
                mDoMtx_YrotM(calc_mtx(), (s16)-i_this->m75C[13].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m75C[13].z);
            } else if (jntNo == MT_JNT_BEND_SECOND) {
                mDoMtx_YrotM(calc_mtx(), (s16)-i_this->m75C[14].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m75C[14].z);
            } else if (jntNo == MT_JNT_TWIST_FIRST) {
                mDoMtx_YrotM(calc_mtx(), i_this->m7B6[13].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m7B6[13].z);
            } else if (jntNo == MT_JNT_TWIST_SECOND) {
                mDoMtx_YrotM(calc_mtx(), i_this->m7B6[14].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m7B6[14].z);
            }
            mt_node_store(model, jntNo);
        }
    }
    return TRUE;
}

/* inlined into body_wall_check */
static inline void wall_check_sub(mt_class *i_this, cXyz *oldPos, cXyz *pos) {
    cXyz start;
    cXyz end;
    start.x = oldPos->x;
    start.y = oldPos->y + 50.0f;
    start.z = oldPos->z;
    end.x = pos->x;
    end.y = pos->y + 50.0f;
    end.z = pos->z;
    dBgS_LinChk_l linChk;
    dBgS_LinChk_ct(&linChk, MT_LINCHK_VTBLS);
    dBgS_LinChk_Set(&linChk, &start, &end, i_this);
    BOOL hit = cBgS_LineCross(dComIfG_Bgsp(), &linChk);
    dBgS_LinChk_dt(&linChk, MT_LINCHK_VTBLS);
    if (hit) {
        pos->x = oldPos->x;
        pos->z = oldPos->z;
    }
}

/* 021D7E80 */
void body_wall_check(mt_class *i_this) {
    for (int i = 1; i < MT_PART_NUM; i++) {
        wall_check_sub(i_this, &i_this->m61C[i], &i_this->m5BC[i]);
    }
}

/* inlined into daMt_Draw */

/* inlined into daMt_Draw */
static inline void br_draw(mt_class *i_this) {
    u32 n = (u8)(i_this->m19F0 - 1);
    f32 x = i_this->eyePos.x;
    f32 z = i_this->eyePos.z;
    f32 y = i_this->eyePos.y;
    if (n >= 11)
        n = 10;
    MtxTrans(x, y, z, 0);
    mDoMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
    mDoMtx_XrotM(calc_mtx(), i_this->shape_angle.x);
    mDoMtx_ZrotM(calc_mtx(), i_this->shape_angle.z);
    f32 scale = l_HIO().m1C * (REG0_F(4) + 2.0f);
    MtxPush();
    mDoMtx_YrotM(calc_mtx(), l_br_angle[n]);
    mDoMtx_XrotM(calc_mtx(), -0x4000);
    MtxScale(scale, scale, scale, 1);
    J3DModel *model = i_this->mpBrModelA[l_br_model[n]];
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    J3DModel_setBaseTRMtx(model, calc_mtx());
    mDoExt_modelUpdateDL(model);
    MtxPull();
    mDoMtx_ZrotM(calc_mtx(), -0x8000);
    mDoMtx_YrotM(calc_mtx(), l_br_angle[n]);
    mDoMtx_XrotM(calc_mtx(), -0x4000);
    MtxScale(scale, scale, scale, 1);
    model = i_this->mpBrModelB[l_br_model[n]];
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    J3DModel_setBaseTRMtx(model, calc_mtx());
    mDoExt_modelUpdateDL(model);
}

// HD: no shadow (daMt_shadowDraw is gone); the eye btp is re-initialised every frame.
/* 021D7FFC */
static BOOL daMt_Draw(mt_class *i_this) {
    cXyz zero;
    zero.x = 0.0f;
    zero.y = 0.0f;
    zero.z = 0.0f;
    if (i_this->m3D7 != 0) {
        return TRUE;
    }
    body_idx() = 0;
    for (s32 i = 0; i < MT_PART_NUM; i++) {
        J3DModel *model = i_this->mpMorf[i]->getModel();
        if (i_this->mEnemyIce.mLightShrinkTimer == 0) {
            cXyz pos;
            PSMTXMultVec(J3DModel_getBaseTRMtx(model), &zero, &pos);
            settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &pos, &i_this->tevStr);
            setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
        } else {
            f32 s = i_this->mEnemyIce.mScaleXZ;
            i_this->scale.z = s;
            i_this->scale.y = s;
            i_this->scale.x = s;
            model->mBaseScale.y = s;
            model->mBaseScale.x = s;
            model->mBaseScale.z = s;
            setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
        }
        mDoExt_btkAnm *btk = i_this->mpBtk[i];
        mDoExt_btkAnm_entry(btk, J3DModel_getModelData(model), btk->getFrame());
        mDoExt_brkAnm *brk = i_this->mpBrk[i];
        mDoExt_brkAnm_entry(brk, J3DModel_getModelData(model), ((mDoExt_baseAnm *)brk)->getFrame());
        if (i_this->m400 == 0) {
            s32 f = i_this->m404 + i * l_HIO().m50;
            while (f < 0)
                f += 41;
            ((mDoExt_baseAnm *)i_this->mpBrk[i].get())->mFrameCtrl.setFrame((f32)f);
            f = i_this->m408 + i * l_HIO().m50;
            while (f < 0)
                f += 31;
            i_this->mpBtk[i]->mFrameCtrl.setFrame((f32)f);
        } else {
            ((mDoExt_baseAnm *)i_this->mpBrk[i].get())->mFrameCtrl.setFrame((f32)i_this->m404);
            i_this->mpBtk[i]->mFrameCtrl.setFrame((f32)i_this->m408);
        }
        if (i == 0) {
            /* mt_eye_tex_anm */
            J3DAnm_setFrame(i_this->mpBtpRes, (f32)i_this->mBtpFrame);
            mDoExt_btpAnm_init(i_this->mpBtp, J3DModel_getModelData(i_this->mpMorf[0]->getModel()),
                               i_this->mpBtpRes, 1, 2, 1.0f, 0, -1, 1, 0);
            mDoExt_btpAnm_entry(i_this->mpBtp, J3DModel_getModelData(i_this->mpMorf[0]->getModel()),
                                i_this->mBtpFrame);
        }
        i_this->mpMorf[i]->updateDL();
    }
    if (i_this->m19F0 != 0) {
        br_draw(i_this);
    }
    dSnap_RegistFig(MT_SNAP_TYPE, i_this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 021D8530 */
void bakuha(mt_class *i_this) {
    fopAcM_createDisappear(i_this, &i_this->eyePos, 10, 0, 0xFF);
    if (i_this->m3D2 == 0 && i_this->m3D5 != 0) {
        dComIfGs_onSwitch(i_this->m3D5, fopAcM_GetRoomNo(i_this));
    }
    i_this->m570 = MT_STATE_DEAD;
    i_this->m576 = REG0_S(0) + 0x39;
    for (int i = 1; i < MT_PART_NUM; i++) {
        i_this->m6AC[i].x = cM_rndFX(REG0_F(4) + 30.0f);
        i_this->m6AC[i].y = cM_rndF(10.0f) + 20.0f + REG0_F(5);
        i_this->m6AC[i].z = cM_rndFX(REG0_F(4) + 30.0f);
        s16 t = (s16)gabi::ftoi(cM_rndF(3.0f));
        i_this->m70C[i] = t;
        if (i_this->m1A14 == 3) {
            i_this->m70C[i] = t + 5;
        }
    }
    dComIfGp_particle_set(MT_BURST_EFFECT, &i_this->current.pos);
}

/* 021D86F8 */
void water_damage_se_set(mt_class *i_this) {
    if (i_this != nullptr && gabi::ea(&i_this->eyePos) != 0) {
        mDoAud_seStart(MT_SE_FALL_WATER, &i_this->eyePos, 0,
                       dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
        // HD: inlined fopAcM_monsSeStart keeps the full actor ID and reverb words.
        if (gabi::ea(&i_this->eyePos) != 0) {
            u32 pid = i_this != nullptr ? fopAcM_GetID(i_this) : 0xFFFFFFFF;
            s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(i_this));
            mt_monsSeStart(MT_SE_DAMAGE, &i_this->eyePos, pid, 0, reverb);
        }
    }
    i_this->m464 = 1;
}

/* 021DC62C */
static BOOL daMt_IsDelete(mt_class *) { return TRUE; }

// HD: a Magtail removed by mt_bg_check (fell out of the room) is re-created at its home
// position, unless its switch (m3D6) is set.
/* 021DC634 */
static BOOL daMt_Delete(mt_class *i_this) {
    dComIfG_resDelete(&i_this->mPhs, "Mt");
    if (i_this->mpEmitter != nullptr) {
        MtEmitterView *e = (MtEmitterView *)i_this->mpEmitter.get();
        e->mStatus &= ~0x40u;
        e->mMaxFrame = -1;
        e->mStatus |= 1;
        i_this->mpEmitter = nullptr;
    }
    for (int i = 0; i < MT_PART_NUM; i++) {
        i_this->mFollowCb[i].remove();
    }
    if (i_this->m1DD4 != 0 && i_this->m3D2 != 0 && i_this->m3D6 != 0 &&
        !dComIfGs_isSwitch(i_this->m3D6, fopAcM_GetRoomNo(i_this))) {
        u8 *app = fopAcM_CreateAppend();
        MtAppendView *a = (MtAppendView *)app;
        a->mPosition = i_this->home.pos;
        a->mAngle.x = i_this->home.angle.x;
        a->mAngle.y = i_this->home.angle.y;
        a->mAngle.z = i_this->home.angle.z;
        a->mParameters = fopAcM_GetParam(i_this);
        a->mRoomNo = i_this->current.roomNo;
        fpcSCtRq_Request(fpcLy_CurrentLayer(), cc_uty::fpcNm_MT_e, 0, 0, app);
    }
    return TRUE;
}

/* CallbackCreateHeap tables (.data, per segment) */
static inline void setJointCallBack(J3DModelData *md, u32 i, u32 cb) {
    MtModelDataView *data = (MtModelDataView *)md;
    MtJointNodeView *nodes = data->mpNodes;
    nodes[i < data->mNodeNum ? i : 0].mCallBack = cb;
}

/* 021DC780 */
static BOOL CallbackCreateHeap(fopAc_ac_c *a_this) {
    mt_class *i_this = (mt_class *)a_this;
    for (u32 i = 0; i < MT_PART_NUM; i++) {
        J3DModelData *bmd =
            (J3DModelData *)dComIfG_getObjectRes("Mt", l_bmd_idx[i], MT_SAFESTRING_VTBL);
        i_this->mpMorf[i] = mDoExt_McaMorf::create(nullptr, bmd, nullptr, nullptr, nullptr, 2, 1.0f,
                                                   0, -1, 1, nullptr, 0x80000, 0x37440402);
        if (i_this->mpMorf[i] == nullptr || i_this->mpMorf[i]->getModel() == nullptr) {
            return FALSE;
        }
        J3DModel *model = i_this->mpMorf[i]->getModel();
        J3DModelData *modelData = J3DModel_getModelData(model);

        mDoExt_btkAnm *btk = (mDoExt_btkAnm *)operator_new(0x74);
        if (btk != nullptr)
            btk = mt_btkAnm_ct(btk); /* mDoExt_btkAnm::mDoExt_btkAnm */
        i_this->mpBtk[i] = btk;
        if (btk == nullptr)
            JUT_ASSERT_fail("d_a_mt.cpp", 0x123E, "actor->btk[i]");
        J3DAnmTextureSRTKey *btkRes =
            (J3DAnmTextureSRTKey *)dComIfG_getObjectRes("Mt", l_btk_idx[i], MT_SAFESTRING_VTBL);
        if (!i_this->mpBtk[i]->init(J3DModel_getModelData(model), btkRes, true, 2, 1.0f, 0, -1,
                                    false, 0)) {
            return FALSE;
        }

        mDoExt_brkAnm *brk = (mDoExt_brkAnm *)operator_new(0x78);
        if (brk != nullptr)
            brk = mDoExt_brkAnm_ct(brk);
        i_this->mpBrk[i] = brk;
        if (brk == nullptr)
            JUT_ASSERT_fail("d_a_mt.cpp", 0x124B, "actor->brk[i]");
        J3DAnmTevRegKey *brkRes =
            (J3DAnmTevRegKey *)dComIfG_getObjectRes("Mt", l_brk_idx[i], MT_SAFESTRING_VTBL);
        if (!mDoExt_brkAnm_init(i_this->mpBrk[i], J3DModel_getModelData(model), brkRes, 1, 2, 1.0f,
                                0, -1, 0, 0)) {
            return FALSE;
        }

        if (i == 0) {
            anm_init(i_this, MT_INITIAL_BCK, 20.0f, 2, 1.0f, 0);
            J3DAnmTexPattern *btp = nullptr;
            for (u32 j = 0; j < 2; j++) {
                btp = (J3DAnmTexPattern *)dComIfG_getObjectRes("Mt", l_tex_anm_idx[j],
                                                               MT_SAFESTRING_VTBL);
                // HD: searchUpdateMaterialID is dispatched through the animation vtable.
                MtAnmView *pattern = (MtAnmView *)btp;
                pattern->searchUpdateMaterialID(btp, J3DModel_getModelData(model));
            }
            mDoExt_btpAnm *btpAnm = mDoExt_btpAnm_ct(nullptr);
            i_this->mpBtp = btpAnm;
            mDoExt_btpAnm_init(btpAnm, J3DModel_getModelData(i_this->mpMorf[0]->getModel()), btp, 1,
                               2, 1.0f, 0, -1, 0, 0);
            tex_anm_set(i_this, 0);
        }

        ((MtModelView *)model)->mpActor = i_this;
        if (i == 0) {
            for (u16 j = 0; j < mtJointTree(modelData)->mJointNum; j++) {
                if ((u32)(j - 2) < 4)
                    setJointCallBack(modelData, j, gabi::guest_fn((const void *)nodeCallBack_head));
            }
        } else {
            for (u16 j = 0; j < mtJointTree(modelData)->mJointNum; j++) {
                if ((u32)(j - 2) < 4) {
                    if (i == 7) {
                        setJointCallBack(modelData, j,
                                         gabi::guest_fn((const void *)nodeCallBack_tail));
                    } else {
                        setJointCallBack(modelData, j,
                                         gabi::guest_fn((const void *)nodeCallBack_body));
                    }
                }
            }
        }
        i_this->m71C[i] = l_part_scale[i];
    }

    for (u32 i = 0; i < 3; i++) {
        J3DModelData *modelData =
            (J3DModelData *)dComIfG_getObjectRes("Mt", l_br_bmd_idx[i], MT_SAFESTRING_VTBL);
        if (modelData == nullptr)
            JUT_ASSERT_fail("d_a_mt.cpp", 0x12FF, "modelData != (0)");
        i_this->mpBrModelA[i] = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        if (i_this->mpBrModelA[i] == nullptr)
            JUT_ASSERT_fail("d_a_mt.cpp", 0x1302, "actor->br_modelL[i] != (0)");
        i_this->mpBrModelB[i] = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        if (i_this->mpBrModelB[i] == nullptr)
            JUT_ASSERT_fail("d_a_mt.cpp", 0x1304, "actor->br_modelR[i] != (0)");
        /* J3DModel::setBaseScale(scale) */
        J3DModel *m = i_this->mpBrModelA[i];
        f32 sx = i_this->scale.x, sz = i_this->scale.z, sy = i_this->scale.y;
        m->mBaseScale.x = sx;
        m->mBaseScale.z = sz;
        m->mBaseScale.y = sy;
        m = i_this->mpBrModelB[i];
        sy = i_this->scale.y;
        sz = i_this->scale.z;
        sx = i_this->scale.x;
        m->mBaseScale.z = sz;
        m->mBaseScale.y = sy;
        m->mBaseScale.x = sx;
    }
    return TRUE;
}

/* daMt_Create's sphere sources (.data) */

/* 021DCE84 */
static cPhs_State daMt_Create(fopAc_ac_c *a_this) {
    mt_class *i_this = (mt_class *)a_this;
    /* fopAcM_SetupActor(a_this, mt_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            mt_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State res = dComIfG_resLoad(&i_this->mPhs, "Mt");
    if (res != cPhs_COMPLEATE_e) {
        return res;
    }

    a_this->gbaName = 5;
    u8 type = fopAcM_GetParam(a_this) & 0xFF;
    u32 param = fopAcM_GetParam(a_this);
    i_this->m3D0 = type != 0xFF ? type : 0;
    i_this->m3D1 = (param >> 8) & 0x7F;
    i_this->m3D2 = (param >> 15) & 1;
    i_this->m3D3 = (param >> 16) & 0xFF;
    i_this->m3D4 = param >> 24;
    u8 sw = (u8)a_this->current.angle.z;
    if (i_this->m3D2 == 0) {
        i_this->m3D5 = sw;
        if (sw != 0 && dComIfGs_isSwitch(sw, fopAcM_GetRoomNo(a_this))) {
            return cPhs_ERROR_e;
        }
    } else {
        i_this->m3D6 = sw;
    }
    a_this->current.angle.z = 0;

    a_this->itemTableIdx = dComIfGp_CharTbl_GetNameIndex("magtail", 0);

    if (!fopAcM_entrySolidHeap(a_this, gabi::guest_fn((const void *)CallbackCreateHeap), 0x1BFC8)) {
        return cPhs_ERROR_e;
    }

    i_this->mBtHeight = 162.5f;
    i_this->mBtBodyR = 200.0f;
    if (i_this->m3D0 >= 10) {
        switch (i_this->m3D1) {
        case 1:
            i_this->m5A4 = 1000;
            break;
        case 2:
            i_this->m5A4 = 500;
            break;
        case 3:
            i_this->m5A4 = 250;
            break;
        case 11:
            i_this->m5A4 = -1000;
            break;
        case 12:
            i_this->m5A4 = -500;
            break;
        case 13:
            i_this->m5A4 = -250;
            break;
        }
    } else if (i_this->m3D3 != 0xFF) {
        i_this->ppd = dPath_GetRoomPath(i_this->m3D3, fopAcM_GetRoomNo(a_this));
        if (i_this->ppd == nullptr) {
            return cPhs_ERROR_e;
        }
        i_this->m3D8 = i_this->m3D3 + 1;
        i_this->m3DA = 1;
        MtPathPoint *pnt = ((MtPathView *)i_this->ppd.get())->mpPoints;
        i_this->m598.x = pnt->mPosition.x;
        i_this->m598.y = pnt->mPosition.y;
        i_this->m598.z = pnt->mPosition.z;
    }
    if (i_this->m3D4 != 0xFF) {
        i_this->m3D7 = i_this->m3D4 + 1;
    }

    a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf[1]->getModel()));
    fopAcM_SetMin(a_this, -200.0f, -200.0f, -200.0f);
    fopAcM_SetMax(a_this, 200.0f, 200.0f, 200.0f);
    a_this->gravity = -3.0f;
    i_this->m586 = (s16)gabi::ftoi(cM_rndF(32768.0f));
    for (int i = 0; i < MT_JOINT_NUM; i++) {
        i_this->m810[i].copy(a_this->current.pos);
        i_this->mB10[i].x = a_this->current.angle.x;
        i_this->mB10[i].y = a_this->current.angle.y;
        i_this->mB10[i].z = a_this->current.angle.z;
    }
    dBgS_Acch_Set(&i_this->mAcch, &a_this->current.pos, &a_this->old.pos, a_this, 1,
                  &i_this->mAcchCir, &a_this->speed, nullptr, nullptr);
    dBgS_AcchCir_SetWall(&i_this->mAcchCir, 50.0f, REG0_F(0) + 19.0f);
    i_this->mStts.Init(250, 2, a_this);
    for (int i = 0; i < MT_PART_NUM; i++) {
        i_this->mSph[i].Set(&eye_sph_src);
        i_this->mSph[i].SetStts(&i_this->mStts);
    }
    i_this->mSph[0].SetAtAtp(2);
    i_this->mAtSph.Set(&at_sph_src);
    i_this->mAtSph.SetStts(&i_this->mStts);
    a_this->max_health = 8;
    i_this->mEnemyIce.mDeathSwitch = i_this->m3D5;
    i_this->mHeadScale = 1.0f;
    i_this->m1A17 = 2;
    a_this->health = 8;
    i_this->mEnemyIce.mpActor = a_this;
    daMt_Execute(i_this);
    return res;
}

// HD: compiler-generated and template code of this translation unit.

/* 021DCD04 */
static u32 followEcallBack_ct(dPa_followEcallBack *p) {
    return mt_followEcallBack_ct(p, (u8)0, (u8)0);
}

static const dBgS_ObjAcch_vt MT_OBJACCH_VT = {0x1001559C, 0x100155BC, 0x100155AC};

// HD: actor constructor wrapper allocates when this is NULL.
/* 021DCD10 */
static mt_class *mt_class_ct(mt_class *self) {
    if (self == nullptr) {
        self = (mt_class *)operator_new(sizeof(mt_class));
        if (self == nullptr)
            return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = MT_VTBL;
    mt_construct_array(self->mFollowCb, MT_PART_NUM, sizeof(dPa_followEcallBack),
                       gabi::guest_fn((const void *)followEcallBack_ct));
    dBgS_AcchCir_ct(&self->mAcchCir);
    dBgS_ObjAcch_ct(&self->mAcch, MT_OBJACCH_VT);
    dCcD_Stts_ct(&self->mStts);
    mt_construct_array(self->mSph, MT_PART_NUM, sizeof(dCcD_Sph), MT_SPH_CONSTRUCTOR);
    mt_Sph_ct(&self->mAtSph);
    dCcD_Stts_ct(&self->mEnemyIce.mStts);
    dCcD_Cyl_ct(&self->mEnemyIce.mCyl, MT_AAB_VTBL);
    dBgS_AcchCir_ct(&self->mEnemyIce.mBgAcchCir);
    dBgS_ObjAcch_ct(&self->mEnemyIce.mBgAcch, MT_OBJACCH_VT);
    return self;
}

/* 021DD478 */
static void __sinit_d_a_mt_cpp() {
    sinit_header_statics(0x10465854, 0x101BAFAC);
    daMt_HIO_c_ct(&l_HIO());
}

/* 021DD518 */
static void mt_fopAcM_seStart(fopAc_ac_c *a, u32 id, u32 param) { fopAcM_seStart(a, id, param); }

/* out-of-line copy of fopAcM_monsSeStart(actor, se, param) (HD inline):
 * mDoAud_monsSeStart(id, pos, actorId, param, reverb) */
/* 021DD584 */
static void mt_fopAcM_monsSeStart(fopAc_ac_c *a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        u32 pid = a != nullptr ? fopAcM_GetID(a) : 0xFFFFFFFF;
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
        mt_monsSeStart(id, &a->eyePos, pid, param, reverb);
    }
}

/* 021DD60C */
static void SafeString_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 021DD620 */
static f32 TSinCosTable_sinShort(void *table, s16 angle) {
    return ((MtSinCosEntry *)table)[(u16)angle >> 3].sin;
}

/* 021DFDDC */
static f32 TSinCosTable_cosShort(void *table, s16 angle) {
    return ((MtSinCosEntry *)table)[(u16)angle >> 3].cos;
}

/* 021DFDF4 */
static void followEcallBack_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

static inline void mt_ObjAcch_dt(dBgS_ObjAcch *a) {
    ((MtAcchView *)a)->mSecondaryVtable = MT_OBJACCH_VT.v20;
    ((MtAcchView *)a)->mVtable = MT_OBJACCH_VT.v14;
    mt_Acch_dt(a, 0); /* dBgS_Acch::~dBgS_Acch */
}

/* 021DFE08 */
static void mt_class_dt(mt_class *self, s32 flags) {
    if (self == nullptr)
        return;
    mt_ObjAcch_dt(&self->mEnemyIce.mBgAcch);
    mt_Cir_dt(&((MtAcchCirView *)&self->mEnemyIce.mBgAcchCir)->mCircle, 2); /* ~cM3dGCir */
    dCcD_Cyl_dt(&self->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&self->mEnemyIce.mStts, 2);
    mt_Sph_dt(&self->mAtSph, 2); /* dCcD_Sph::~dCcD_Sph */
    mt_destroy_arr(self->mSph, MT_PART_NUM, sizeof(dCcD_Sph), MT_SPH_DESTRUCTOR, 0, 0);
    dCcD_Stts_dt(&self->mStts, 2);
    mt_ObjAcch_dt(&self->mAcch);
    mt_Cir_dt(&((MtAcchCirView *)&self->mAcchCir)->mCircle, 2);
    mt_destroy_arr(self->mFollowCb, MT_PART_NUM, sizeof(dPa_followEcallBack),
                   gabi::guest_fn((const void *)followEcallBack_dt), 0, 0);
    mt_Actor_dt(self, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1)
        operator_delete(self);
}

/* 021DFF2C */
static void SafeString_assureTerminationImpl(SafeString *) {}
/* WWHD glue (tools/verify/autoglue.py)
data l_tex_anm_idx 101BADC0
data l_tex_max_frame 101BADC4
data l_br_angle 101BAEBC
data l_br_model 101BAED4
data l_bmd_idx 101BAEE0
data l_btk_idx 101BADF4
data l_brk_idx 101BADD4
data l_part_scale 101BAF00
data l_br_bmd_idx 101BAF20
data eye_sph_src 101BAF2C nocheck
data at_sph_src 101BAF6C nocheck
str 10015674 in 021D73C0
str 10015684 in 021D75D8
str 1001574C in 021DC634
str 1001574F in 021DC780
str 10015754 "actor->btk[i]"
str 10015764 "actor->brk[i]"
str 10015780 "modelData != (0)"
str 10015794 "actor->br_modelL[i] != (0)"
str 100157B0 "actor->br_modelR[i] != (0)"
str 100157E0 in 021DCE84
str 100157D8 "magtail"
str 10015774 "d_a_mt.cpp"
ext 027F3F94 MtJointTreeView* mtJointTree(J3DModelData*)
ext 025E1AA4 void mt_monsSeStart(u32 id, cXyz* pos, u32 actorId, u32 param, s32 reverb)
ext 025E7C6C mDoExt_btkAnm* mt_btkAnm_ct(mDoExt_btkAnm* p)
ext 025A5894 u32 mt_followEcallBack_ct(dPa_followEcallBack* p, u8 a, u8 b)
ext 028EFFD0 void mt_construct_array(void* p, u32 n, u32 size, u32 ctor)
ext 028F0164 void mt_destroy_arr(void* p, u32 n, u32 size, u32 dtor, s32 a, s32 b)
ext 025166F0 void mt_Sph_ct(dCcD_Sph* p)
ext 02515AE8 void mt_Sph_dt(dCcD_Sph* p, s32 flags)
ext 024EFD9C void mt_Acch_dt(dBgS_ObjAcch* p, s32 flags)
ext 025D50BC void mt_Actor_dt(mt_class* p, s32 flags)
ext 02018034 void mt_Cir_dt(void* p, s32 flags)
*/
