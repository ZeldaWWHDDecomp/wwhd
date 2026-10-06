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
#include "d/actor/d_a_mt.h"

/* guest string literals (.rodata): GHS does not pool them */
#define FILE_NAME STR(0x10015774) /* "d_a_mt.cpp" */

/* 021D73C0
 * HD: anm_init(i_this, bck, morf, loopMode, speed, soundFileIdx): soundFileIdx is unused; the
 * animation goes to the head's morf (mpMorf[0]) without a sound table. */
void anm_init(mt_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx) {
    WWHD_FUNC(0x021D73C0, void, i_this, bckFileIdx, morf, loopMode, speed, soundFileIdx);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10015674) /* "Mt" */, bckFileIdx, MT_SAFESTRING_VTBL);
    i_this->mpMorf[0]->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
}
VERIFY(0x021D73C0, anm_init);

/* 021D7480: counts the Magtails (and the ones in state 1) */
static void* mt_a_d_sub(void* param_1, void*) {
    WWHD_FUNC(0x021D7480, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0xD8 /* fpcNm_MT_e */) {
        mt_all_count() += 1;
        if (((mt_class*)param_1)->m570 == 1) {
            mt_fight_count() += 1;
        }
    }
    return nullptr;
}
VERIFY(0x021D7480, mt_a_d_sub);

/* 021D74F8 */
void mt_bg_check(mt_class* i_this) {
    WWHD_FUNC(0x021D74F8, void, i_this);
    f32 offset = REG0_F(3) + 40.0f;
    i_this->actor_status |= 0x400;
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
VERIFY(0x021D74F8, mt_bg_check);

/* tex_anm_set tables (.data): btp resource index and its last frame */
static inline u16 mt_tex_anm_idx(u32 i) { return gabi::load<u16>(0x101BADC0 + 2 * i); }
static inline u16 mt_tex_max_frame(u32 i) { return gabi::load<u16>(0x101BADC4 + 2 * i); }
/* J3DAnmBase (HD): the frame is the first word */
static inline void J3DAnm_setFrame(void* anm, f32 f) { gabi::store<f32>(gabi::ea(anm), f); }

/* 021D75D8 */
void tex_anm_set(mt_class* i_this, u16 idx) {
    WWHD_FUNC(0x021D75D8, void, i_this, idx);
    i_this->mBtpOn = 1;
    J3DAnmTexPattern* btp = (J3DAnmTexPattern*)dComIfG_getObjectRes(STR(0x10015684) /* "Mt" */, mt_tex_anm_idx(idx), MT_SAFESTRING_VTBL);
    i_this->mpBtpRes = btp;
    i_this->mBtpMaxFrame = (u8)mt_tex_max_frame(idx);
    i_this->mBtpFrame = 0;
    J3DAnm_setFrame(btp, 0.0f);
}
VERIFY(0x021D75D8, tex_anm_set);

/* the joint's animation matrix back from calc_mtx, and into J3DSys::mCurrentMtx */
static inline void mt_node_store(J3DModel_l* model, u32 jntNo) {
    mtx_copy(model_getAnmMtx(model, jntNo), calc_mtx());
    PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
}

/* 021D7668 */
static BOOL nodeCallBack_head(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x021D7668, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel_l* model = j3dSys_getModel();
        mt_class* i_this = gabi::at<mt_class>(model->mUserArea);
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != nullptr) {
            PSMTXCopy(model_getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo == 2) {
                mDoMtx_YrotM(calc_mtx(), (s16)-i_this->m75C[0].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m75C[0].z);
            } else if (jntNo == 3) {
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
VERIFY(0x021D7668, nodeCallBack_head);

/* 021D78F4: nodeCallBack_body (not named by the matcher). body_idx() is the segment being
 * calculated: joints 2/3 take m75C[idx+1]/[idx+2], joints 4/5 m7B6[idx+1]/[idx+2], then idx += 2 */
static BOOL nodeCallBack_body(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x021D78F4, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel_l* model = j3dSys_getModel();
        mt_class* i_this = gabi::at<mt_class>(model->mUserArea);
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != nullptr) {
            PSMTXCopy(model_getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo == 2) {
                mDoMtx_YrotM(calc_mtx(), (s16)-i_this->m75C[body_idx() + 1].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m75C[body_idx() + 1].z);
            } else if (jntNo == 3) {
                mDoMtx_YrotM(calc_mtx(), (s16)-i_this->m75C[body_idx() + 2].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m75C[body_idx() + 2].z);
            } else if (jntNo == 4) {
                mDoMtx_YrotM(calc_mtx(), i_this->m7B6[body_idx() + 1].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m7B6[body_idx() + 1].z);
            } else if (jntNo == 5) {
                mDoMtx_YrotM(calc_mtx(), i_this->m7B6[body_idx() + 2].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m7B6[body_idx() + 2].z);
                body_idx() += 2;
            }
            mt_node_store(model, jntNo);
        }
    }
    return TRUE;
}
VERIFY(0x021D78F4, nodeCallBack_body);

/* 021D7CC4: nodeCallBack_tail (not named by the matcher) */
static BOOL nodeCallBack_tail(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x021D7CC4, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel_l* model = j3dSys_getModel();
        mt_class* i_this = gabi::at<mt_class>(model->mUserArea);
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != nullptr) {
            PSMTXCopy(model_getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo == 2) {
                mDoMtx_YrotM(calc_mtx(), (s16)-i_this->m75C[13].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m75C[13].z);
            } else if (jntNo == 3) {
                mDoMtx_YrotM(calc_mtx(), (s16)-i_this->m75C[14].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m75C[14].z);
            } else if (jntNo == 4) {
                mDoMtx_YrotM(calc_mtx(), i_this->m7B6[13].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m7B6[13].z);
            } else if (jntNo == 5) {
                mDoMtx_YrotM(calc_mtx(), i_this->m7B6[14].x);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m7B6[14].z);
            }
            mt_node_store(model, jntNo);
        }
    }
    return TRUE;
}
VERIFY(0x021D7CC4, nodeCallBack_tail);

/* wall_check_sub (inlined in WWHD): a segment that moved into a wall keeps its old x/z */
static inline void wall_check_sub(mt_class* i_this, cXyz* oldPos, cXyz* pos) {
    gabi::Local<cXyz> start;
    gabi::Local<cXyz> end;
    start->x = oldPos->x;
    start->y = oldPos->y + 50.0f;
    start->z = oldPos->z;
    end->x = pos->x;
    end->y = pos->y + 50.0f;
    end->z = pos->z;
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk, MT_LINCHK_VTBLS);
    dBgS_LinChk_Set(linChk, start, end, i_this);
    BOOL hit = cBgS_LineCross(dComIfG_Bgsp(), linChk);
    dBgS_LinChk_dt(linChk, MT_LINCHK_VTBLS);
    if (hit) {
        pos->x = oldPos->x;
        pos->z = oldPos->z;
    }
}

/* 021D7E80 */
void body_wall_check(mt_class* i_this) {
    WWHD_FUNC(0x021D7E80, void, i_this);
    for (int i = 1; i < MT_PART_NUM; i++) {
        wall_check_sub(i_this, &i_this->m61C[i], &i_this->m5BC[i]);
    }
}
VERIFY(0x021D7E80, body_wall_check);

/* br_draw tables (.data, 11 entries): rubble model angle and model index */
static inline s16 mt_br_angle(u32 n) { return gabi::load<s16>(0x101BAEBC + 2 * n); }
static inline u8 mt_br_model(u32 n) { return gabi::load<u8>(0x101BAED4 + n); }

/* br_draw (inlined in WWHD): the two rubble halves around the eye position */
static inline void br_draw(mt_class* i_this) {
    u32 n = (u8)(i_this->m19F0 - 1);
    f32 x = i_this->eyePos.x;
    f32 z = i_this->eyePos.z;
    f32 y = i_this->eyePos.y;
    if (n >= 11) n = 10;
    MtxTrans(x, y, z, 0);
    mDoMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
    mDoMtx_XrotM(calc_mtx(), i_this->shape_angle.x);
    mDoMtx_ZrotM(calc_mtx(), i_this->shape_angle.z);
    f32 scale = l_HIO().m1C * (REG0_F(4) + 2.0f);
    MtxPush();
    mDoMtx_YrotM(calc_mtx(), mt_br_angle(n));
    mDoMtx_XrotM(calc_mtx(), -0x4000);
    MtxScale(scale, scale, scale, 1);
    J3DModel* model = i_this->mpBrModelA[mt_br_model(n)];
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    J3DModel_setBaseTRMtx(model, calc_mtx());
    mDoExt_modelUpdateDL(model);
    MtxPull();
    mDoMtx_ZrotM(calc_mtx(), -0x8000);
    mDoMtx_YrotM(calc_mtx(), mt_br_angle(n));
    mDoMtx_XrotM(calc_mtx(), -0x4000);
    MtxScale(scale, scale, scale, 1);
    model = i_this->mpBrModelB[mt_br_model(n)];
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    J3DModel_setBaseTRMtx(model, calc_mtx());
    mDoExt_modelUpdateDL(model);
}

/* 021D7FFC
 * HD: no shadow (daMt_shadowDraw is gone); the eye btp is re-initialised every frame. */
static BOOL daMt_Draw(mt_class* i_this) {
    WWHD_FUNC(0x021D7FFC, BOOL, i_this);
    gabi::Local<cXyz> zero;
    zero->x = 0.0f;
    zero->y = 0.0f;
    zero->z = 0.0f;
    if (i_this->m3D7 != 0) {
        return TRUE;
    }
    body_idx() = 0;
    for (s32 i = 0; i < MT_PART_NUM; i++) {
        J3DModel* model = i_this->mpMorf[i]->getModel();
        if (i_this->mEnemyIce.mLightShrinkTimer == 0) {
            gabi::Local<cXyz> pos;
            PSMTXMultVec(J3DModel_getBaseTRMtx(model), zero, pos);
            settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, pos, &i_this->tevStr);
            setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
        } else {
            f32 s = i_this->mEnemyIce.mScaleXZ;
            i_this->scale.z = s;
            i_this->scale.y = s;
            i_this->scale.x = s;
            gabi::store<f32>(gabi::ea(model) + 0xC0, s);
            gabi::store<f32>(gabi::ea(model) + 0xBC, s);
            gabi::store<f32>(gabi::ea(model) + 0xC4, s);
            setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
        }
        mDoExt_btkAnm* btk = i_this->mpBtk[i];
        mDoExt_btkAnm_entry(btk, J3DModel_getModelData(model), btk->getFrame());
        mDoExt_brkAnm* brk = i_this->mpBrk[i];
        mDoExt_brkAnm_entry(brk, J3DModel_getModelData(model), ((mDoExt_baseAnm*)brk)->getFrame());
        if (i_this->m400 == 0) {
            s32 f = i_this->m404 + i * l_HIO().m50;
            while (f < 0) f += 41;
            ((mDoExt_baseAnm*)i_this->mpBrk[i].get())->mFrameCtrl.setFrame((f32)f);
            f = i_this->m408 + i * l_HIO().m50;
            while (f < 0) f += 31;
            i_this->mpBtk[i]->mFrameCtrl.setFrame((f32)f);
        } else {
            ((mDoExt_baseAnm*)i_this->mpBrk[i].get())->mFrameCtrl.setFrame((f32)i_this->m404);
            i_this->mpBtk[i]->mFrameCtrl.setFrame((f32)i_this->m408);
        }
        if (i == 0) {
            /* mt_eye_tex_anm */
            J3DAnm_setFrame(i_this->mpBtpRes, (f32)i_this->mBtpFrame);
            mDoExt_btpAnm_init(i_this->mpBtp, J3DModel_getModelData(i_this->mpMorf[0]->getModel()), i_this->mpBtpRes, 1, 2, 1.0f, 0,
                               -1, 1, 0);
            mDoExt_btpAnm_entry(i_this->mpBtp, J3DModel_getModelData(i_this->mpMorf[0]->getModel()), i_this->mBtpFrame);
        }
        i_this->mpMorf[i]->updateDL();
    }
    if (i_this->m19F0 != 0) {
        br_draw(i_this);
    }
    dSnap_RegistFig(0xAF /* DSNAP_TYPE_MT */, i_this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x021D7FFC, daMt_Draw);

/* 021D8530 */
void bakuha(mt_class* i_this) {
    WWHD_FUNC(0x021D8530, void, i_this);
    fopAcM_createDisappear(i_this, &i_this->eyePos, 10, 0, 0xFF);
    if (i_this->m3D2 == 0 && i_this->m3D5 != 0) {
        dComIfGs_onSwitch(i_this->m3D5, fopAcM_GetRoomNo(i_this));
    }
    i_this->m570 = 3;
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
    dComIfGp_particle_set(0x8096, &i_this->current.pos);
}
VERIFY(0x021D8530, bakuha);

/* 021D86F8 */
void water_damage_se_set(mt_class* i_this) {
    WWHD_FUNC(0x021D86F8, void, i_this);
    if (i_this != nullptr && gabi::ea(&i_this->eyePos) != 0) {
        mDoAud_seStart(0x5820 /* JA_SE_OBJ_FALL_WATER_S */, &i_this->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
        /* fopAcM_monsSeStart (HD inline) */
        if (gabi::ea(&i_this->eyePos) != 0) {
            u32 pid = i_this != nullptr ? fopAcM_GetID(i_this) : 0xFFFFFFFF;
            s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(i_this));
            gabi::call(0x025E1AA4, 0x4842 /* JA_SE_CV_MT_DAMAGE */, &i_this->eyePos, pid, 0, reverb);
        }
    }
    i_this->m464 = 1;
}
VERIFY(0x021D86F8, water_damage_se_set);

/* 021DC62C */
static BOOL daMt_IsDelete(mt_class*) {
    WWHD_FUNC(0x021DC62C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021DC62C, daMt_IsDelete);

/* 021DC634
 * HD: a Magtail removed by mt_bg_check (fell out of the room) is re-created at its home
 * position, unless its switch (m3D6) is set. */
static BOOL daMt_Delete(mt_class* i_this) {
    WWHD_FUNC(0x021DC634, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x1001574C) /* "Mt" */);
    if (i_this->mpEmitter != nullptr) {
        JPABaseEmitter_becomeInvalidEmitter(i_this->mpEmitter);
        u32 e = gabi::ea(i_this->mpEmitter.get());
        gabi::store<s32>(e + 0x5C, -1);
        gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) | 1);
        i_this->mpEmitter = nullptr;
    }
    for (int i = 0; i < MT_PART_NUM; i++) {
        i_this->mFollowCb[i].remove();
    }
    if (i_this->m1DD4 != 0 && i_this->m3D2 != 0 && i_this->m3D6 != 0 &&
        !dComIfGs_isSwitch(i_this->m3D6, fopAcM_GetRoomNo(i_this))) {
        u8* app = fopAcM_CreateAppend();
        u32 a = gabi::ea(app);
        for (u32 k = 0; k < 12; k += 4) gabi::store<u32>(a + 4 + k, gabi::load<u32>(gabi::ea(&i_this->home.pos) + k));
        gabi::store<s16>(a + 0x10, i_this->home.angle.x);
        gabi::store<s16>(a + 0x12, i_this->home.angle.y);
        gabi::store<s16>(a + 0x14, i_this->home.angle.z);
        gabi::store<u32>(a + 0, fopAcM_GetParam(i_this));
        gabi::store<s8>(a + 0x21, i_this->current.roomNo);
        fpcSCtRq_Request(fpcLy_CurrentLayer(), 0xD8 /* fpcNm_MT_e */, 0, 0, app);
    }
    return TRUE;
}
VERIFY(0x021DC634, daMt_Delete);

/* CallbackCreateHeap tables (.data, per segment) */
static inline u32 mt_bmd_idx(u32 i) { return gabi::load<u32>(0x101BAEE0 + 4 * i); }
static inline u32 mt_btk_idx(u32 i) { return gabi::load<u32>(0x101BADF4 + 4 * i); }
static inline u32 mt_brk_idx(u32 i) { return gabi::load<u32>(0x101BADD4 + 4 * i); }
static inline f32 mt_part_scale(u32 i) { return gabi::load<f32>(0x101BAF00 + 4 * i); }
static inline u32 mt_br_bmd_idx(u32 i) { return gabi::load<u32>(0x101BAF20 + 4 * i); }
static inline void setJointCallBack(J3DModelData* md, u32 i, u32 cb) { gabi::store<u32>(modelData_getJointNode(md, i) + 8, cb); }

/* 021DC780 */
static BOOL CallbackCreateHeap(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x021DC780, BOOL, a_this);
    mt_class* i_this = (mt_class*)a_this;
    for (u32 i = 0; i < MT_PART_NUM; i++) {
        J3DModelData* bmd = (J3DModelData*)dComIfG_getObjectRes(STR(0x1001574F) /* "Mt" */, mt_bmd_idx(i), MT_SAFESTRING_VTBL);
        i_this->mpMorf[i] = mDoExt_McaMorf::create(nullptr, bmd, nullptr, nullptr, nullptr, 2, 1.0f, 0, -1, 1, nullptr, 0x80000,
                                                   0x37440402);
        if (i_this->mpMorf[i] == nullptr || i_this->mpMorf[i]->getModel() == nullptr) {
            return FALSE;
        }
        J3DModel* model = i_this->mpMorf[i]->getModel();
        J3DModelData* modelData = J3DModel_getModelData(model);

        mDoExt_btkAnm* btk = (mDoExt_btkAnm*)operator_new(0x74);
        if (btk != nullptr) btk = gabi::call<mDoExt_btkAnm*>(0x025E7C6C, btk); /* mDoExt_btkAnm::mDoExt_btkAnm */
        i_this->mpBtk[i] = btk;
        if (btk == nullptr) JUT_ASSERT_fail(FILE_NAME, 0x123E, STR(0x10015754) /* "actor->btk[i]" */);
        J3DAnmTextureSRTKey* btkRes = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(STR(0x1001574F), mt_btk_idx(i), MT_SAFESTRING_VTBL);
        if (!i_this->mpBtk[i]->init(J3DModel_getModelData(model), btkRes, true, 2, 1.0f, 0, -1, false, 0)) {
            return FALSE;
        }

        mDoExt_brkAnm* brk = (mDoExt_brkAnm*)operator_new(0x78);
        if (brk != nullptr) brk = mDoExt_brkAnm_ct(brk);
        i_this->mpBrk[i] = brk;
        if (brk == nullptr) JUT_ASSERT_fail(FILE_NAME, 0x124B, STR(0x10015764) /* "actor->brk[i]" */);
        J3DAnmTevRegKey* brkRes = (J3DAnmTevRegKey*)dComIfG_getObjectRes(STR(0x1001574F), mt_brk_idx(i), MT_SAFESTRING_VTBL);
        if (!mDoExt_brkAnm_init(i_this->mpBrk[i], J3DModel_getModelData(model), brkRes, 1, 2, 1.0f, 0, -1, 0, 0)) {
            return FALSE;
        }

        if (i == 0) {
            anm_init(i_this, 10, 20.0f, 2, 1.0f, 0);
            J3DAnmTexPattern* btp = nullptr;
            for (u32 j = 0; j < 2; j++) {
                btp = (J3DAnmTexPattern*)dComIfG_getObjectRes(STR(0x1001574F), mt_tex_anm_idx(j), MT_SAFESTRING_VTBL);
                /* J3DAnmTexPattern::searchUpdateMaterialID(J3DModelData*): virtual, vtable at +4, slot 0x1C */
                gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(btp) + 4) + 0x1C), btp, J3DModel_getModelData(model));
            }
            mDoExt_btpAnm* btpAnm = mDoExt_btpAnm_ct(nullptr);
            i_this->mpBtp = btpAnm;
            mDoExt_btpAnm_init(btpAnm, J3DModel_getModelData(i_this->mpMorf[0]->getModel()), btp, 1, 2, 1.0f, 0, -1, 0, 0);
            tex_anm_set(i_this, 0);
        }

        ((J3DModel_l*)model)->mUserArea = gabi::ea(i_this);
        if (i == 0) {
            for (u16 j = 0; j < gabi::load<u16>(modelData_getJointTree(modelData) + 8); j++) {
                if ((u32)(j - 2) < 4) setJointCallBack(modelData, j, 0x021D7668 /* nodeCallBack_head */);
            }
        } else {
            for (u16 j = 0; j < gabi::load<u16>(modelData_getJointTree(modelData) + 8); j++) {
                if ((u32)(j - 2) < 4) {
                    if (i == 7) {
                        setJointCallBack(modelData, j, 0x021D7CC4 /* nodeCallBack_tail */);
                    } else {
                        setJointCallBack(modelData, j, 0x021D78F4 /* nodeCallBack_body */);
                    }
                }
            }
        }
        i_this->m71C[i] = mt_part_scale(i);
    }

    for (u32 i = 0; i < 3; i++) {
        J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1001574F), mt_br_bmd_idx(i), MT_SAFESTRING_VTBL);
        if (modelData == nullptr) JUT_ASSERT_fail(FILE_NAME, 0x12FF, STR(0x10015780) /* "modelData != 0" */);
        i_this->mpBrModelA[i] = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        if (i_this->mpBrModelA[i] == nullptr) JUT_ASSERT_fail(FILE_NAME, 0x1302, STR(0x10015794));
        i_this->mpBrModelB[i] = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        if (i_this->mpBrModelB[i] == nullptr) JUT_ASSERT_fail(FILE_NAME, 0x1304, STR(0x100157B0));
        /* J3DModel::setBaseScale(scale) */
        u32 m = gabi::ea(i_this->mpBrModelA[i].get());
        f32 sx = i_this->scale.x, sz = i_this->scale.z, sy = i_this->scale.y;
        gabi::store<f32>(m + 0xBC, sx);
        gabi::store<f32>(m + 0xC4, sz);
        gabi::store<f32>(m + 0xC0, sy);
        m = gabi::ea(i_this->mpBrModelB[i].get());
        sy = i_this->scale.y;
        sz = i_this->scale.z;
        sx = i_this->scale.x;
        gabi::store<f32>(m + 0xC4, sz);
        gabi::store<f32>(m + 0xC0, sy);
        gabi::store<f32>(m + 0xBC, sx);
    }
    return TRUE;
}
VERIFY(0x021DC780, CallbackCreateHeap);

/* 021DCD04: dPa_followEcallBack array element constructor (__construct_array) */
static u32 followEcallBack_ct(dPa_followEcallBack* p) {
    WWHD_FUNC(0x021DCD04, u32, p);
    return gabi::call<u32>(0x025A5894, p, (u8)0, (u8)0);
}
VERIFY(0x021DCD04, followEcallBack_ct);

static const dBgS_ObjAcch_vt MT_OBJACCH_VT = {0x1001559C, 0x100155BC, 0x100155AC};
static inline void __construct_array(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr(void* p, u32 n, u32 size, u32 dtor) { gabi::call(0x028F0164, p, n, size, dtor, 0, 0); }

/* 021DCD10: mt_class::mt_class (HD: allocates when this == NULL) */
static mt_class* mt_class_ct(mt_class* self) {
    WWHD_FUNC(0x021DCD10, mt_class*, self);
    if (self == nullptr) {
        self = (mt_class*)operator_new(sizeof(mt_class));
        if (self == nullptr) return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = MT_VTBL;
    __construct_array(self->mFollowCb, MT_PART_NUM, sizeof(dPa_followEcallBack), 0x021DCD04);
    dBgS_AcchCir_ct(&self->mAcchCir);
    dBgS_ObjAcch_ct(&self->mAcch, MT_OBJACCH_VT);
    dCcD_Stts_ct(&self->mStts);
    __construct_array(self->mSph, MT_PART_NUM, sizeof(dCcD_Sph), 0x025166F0 /* dCcD_Sph::dCcD_Sph */);
    gabi::call(0x025166F0, &self->mAtSph);
    dCcD_Stts_ct(&self->mEnemyIce.mStts);
    dCcD_Cyl_ct(&self->mEnemyIce.mCyl, MT_AAB_VTBL);
    dBgS_AcchCir_ct(&self->mEnemyIce.mBgAcchCir);
    dBgS_ObjAcch_ct(&self->mEnemyIce.mBgAcch, MT_OBJACCH_VT);
    return self;
}
VERIFY(0x021DCD10, mt_class_ct);

/* daMt_Create's sphere sources (.data) */
#define MT_EYE_SPH_SRC gabi::at<dCcD_SrcSph>(0x101BAF2C)
#define MT_AT_SPH_SRC gabi::at<dCcD_SrcSph>(0x101BAF6C)

/* 021DCE84 */
static cPhs_State daMt_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x021DCE84, cPhs_State, a_this);
    mt_class* i_this = (mt_class*)a_this;
    /* fopAcM_SetupActor(a_this, mt_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) mt_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State res = dComIfG_resLoad(&i_this->mPhs, STR(0x100157E0) /* "Mt" */);
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

    a_this->itemTableIdx = dComIfGp_CharTbl_GetNameIndex(STR(0x100157D8) /* "magtail" */, 0);

    if (!fopAcM_entrySolidHeap(a_this, 0x021DC780 /* CallbackCreateHeap */, 0x1BFC8)) {
        return cPhs_ERROR_e;
    }

    i_this->mBtHeight = 162.5f;
    i_this->mBtBodyR = 200.0f;
    if (i_this->m3D0 >= 10) {
        switch (i_this->m3D1) {
        case 1: i_this->m5A4 = 1000; break;
        case 2: i_this->m5A4 = 500; break;
        case 3: i_this->m5A4 = 250; break;
        case 11: i_this->m5A4 = -1000; break;
        case 12: i_this->m5A4 = -500; break;
        case 13: i_this->m5A4 = -250; break;
        }
    } else if (i_this->m3D3 != 0xFF) {
        i_this->ppd = dPath_GetRoomPath(i_this->m3D3, fopAcM_GetRoomNo(a_this));
        if (i_this->ppd == nullptr) {
            return cPhs_ERROR_e;
        }
        i_this->m3D8 = i_this->m3D3 + 1;
        i_this->m3DA = 1;
        u32 pnt = gabi::load<u32>(gabi::ea(i_this->ppd.get()) + 8); /* m_points[0].m_position */
        i_this->m598.x = gabi::load<f32>(pnt + 4);
        i_this->m598.y = gabi::load<f32>(pnt + 8);
        i_this->m598.z = gabi::load<f32>(pnt + 0xC);
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
    dBgS_Acch_Set(&i_this->mAcch, &a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed, nullptr, nullptr);
    dBgS_AcchCir_SetWall(&i_this->mAcchCir, 50.0f, REG0_F(0) + 19.0f);
    i_this->mStts.Init(250, 2, a_this);
    for (int i = 0; i < MT_PART_NUM; i++) {
        i_this->mSph[i].Set(MT_EYE_SPH_SRC);
        i_this->mSph[i].SetStts(&i_this->mStts);
    }
    i_this->mSph[0].SetAtAtp(2);
    i_this->mAtSph.Set(MT_AT_SPH_SRC);
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
VERIFY(0x021DCE84, daMt_Create);

/* 021DD330 */
static daMt_HIO_c* daMt_HIO_c_ct(daMt_HIO_c* self) {
    WWHD_FUNC(0x021DD330, daMt_HIO_c*, self);
    if (self == nullptr) {
        self = (daMt_HIO_c*)operator_new(sizeof(daMt_HIO_c));
        if (self == nullptr) return self;
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
VERIFY(0x021DD330, daMt_HIO_c_ct);

/* 021DD478: static initialisation (header statics, then l_HIO) */
static void __sinit_d_a_mt_cpp() {
    WWHD_FUNC(0x021DD478, void, (u32)0);
    sinit_header_statics(0x10465854, 0x101BAFAC);
    daMt_HIO_c_ct(&l_HIO());
}
VERIFY(0x021DD478, __sinit_d_a_mt_cpp);

/* 021DD518: out-of-line copy of fopAcM_seStart(actor, se, param) (HD inline) */
static void mt_fopAcM_seStart(fopAc_ac_c* a, u32 id, u32 param) {
    WWHD_FUNC(0x021DD518, void, a, id, param);
    fopAcM_seStart(a, id, param);
}
VERIFY(0x021DD518, mt_fopAcM_seStart);

/* 021DD584: out-of-line copy of fopAcM_monsSeStart(actor, se, param) (HD inline):
 * 025E1AA4 mDoAud_monsSeStart(id, pos, actorId, param, reverb) */
static void mt_fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    WWHD_FUNC(0x021DD584, void, a, id, param);
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        u32 pid = a != nullptr ? fopAcM_GetID(a) : 0xFFFFFFFF;
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, param, reverb);
    }
}
VERIFY(0x021DD584, mt_fopAcM_monsSeStart);

/* 021DD60C: sead::SafeString deleting destructor (this TU's copy; vtable 0x10015494 slot +0xC) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021DD60C, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x021DD60C, SafeString_dt);

/* 021DD620: JMath::TSinCosTable::sinShort (out-of-line copy; entries {sin, cos}) */
static f32 TSinCosTable_sinShort(void* table, s16 angle) {
    WWHD_FUNC(0x021DD620, f32, table, angle);
    return gabi::load<f32>(gabi::ea(table) + ((u16)angle >> 3) * 8);
}
VERIFY(0x021DD620, TSinCosTable_sinShort);

/* 021DFDDC: JMath::TSinCosTable::cosShort (out-of-line copy) */
static f32 TSinCosTable_cosShort(void* table, s16 angle) {
    WWHD_FUNC(0x021DFDDC, f32, table, angle);
    return gabi::load<f32>(gabi::ea(table) + ((u16)angle >> 3) * 8 + 4);
}
VERIFY(0x021DFDDC, TSinCosTable_cosShort);

/* 021DFDF4: dPa_followEcallBack array element deleting destructor (__destroy_arr) */
static void followEcallBack_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021DFDF4, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x021DFDF4, followEcallBack_dt);

static inline void mt_ObjAcch_dt(dBgS_ObjAcch* a) {
    gabi::store<u32>(gabi::ea(a) + 0x20, MT_OBJACCH_VT.v20);
    gabi::store<u32>(gabi::ea(a) + 0x14, MT_OBJACCH_VT.v14);
    gabi::call(0x024EFD9C, a, 0); /* dBgS_Acch::~dBgS_Acch */
}

/* 021DFE08: mt_class deleting destructor (HD virtual destructor) */
static void mt_class_dt(mt_class* self, s32 flags) {
    WWHD_FUNC(0x021DFE08, void, self, flags);
    if (self == nullptr) return;
    mt_ObjAcch_dt(&self->mEnemyIce.mBgAcch);
    gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&self->mEnemyIce.mBgAcchCir) + 0x14), 2); /* ~cM3dGCir */
    dCcD_Cyl_dt(&self->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&self->mEnemyIce.mStts, 2);
    gabi::call(0x02515AE8, &self->mAtSph, 2); /* dCcD_Sph::~dCcD_Sph */
    __destroy_arr(self->mSph, MT_PART_NUM, sizeof(dCcD_Sph), 0x02515AE8);
    dCcD_Stts_dt(&self->mStts, 2);
    mt_ObjAcch_dt(&self->mAcch);
    gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&self->mAcchCir) + 0x14), 2);
    __destroy_arr(self->mFollowCb, MT_PART_NUM, sizeof(dPa_followEcallBack), 0x021DFDF4);
    gabi::call(0x025D50BC, self, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}
VERIFY(0x021DFE08, mt_class_dt);

/* 021DFF2C: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x021DFF2C, void, (u32)0);
}
VERIFY(0x021DFF2C, SafeString_assureTerminationImpl);
