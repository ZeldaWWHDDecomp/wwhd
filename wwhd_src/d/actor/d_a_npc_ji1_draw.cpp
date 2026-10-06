/**
 * d_a_npc_ji1_draw.cpp (WWHD)
 * NPC - Orca: joint callbacks, draw, heap, matrices, look-back, harpoon.
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* J3D (HD): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, joint matrix block at model+0x2C
 * (flags u16 +4, matrices +0x10), user area at model+0xB8 (as in d_a_kb.h) */
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
    /* 0xAC */ gptr<J3DModelData> mModelData;
    /* 0xB0 */ u8 _B0[8];
    /* 0xB8 */ be<u32> mUserArea;
};
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty) */
static inline Mtx34* getAnmMtx(J3DModel* m, s32 jnt) {
    J3DMtxBlock_l* blk = ((J3DModel_l*)m)->mpMtxBlock;
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30);
}
static inline J3DModel_l* j3dSys_getModel() { return gabi::at<J3DModel_l>(gabi::load<u32>(0x104B462C)); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline u16 J3DJoint_getJntNo(J3DJoint* j) { return gabi::load<u16>(gabi::ea(j) + 4); }
static inline void PSMTXMultVec_(Mtx34* m, cXyz* a, cXyz* b) { gabi::call(0x028E8F64, m, a, b); }
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }
static inline void JPASetRMtxTVecfromMtx(Mtx34* m, u32 r, u32 t) { gabi::call(0x028249B0, m, r, t); }
/* 025BEBB8 dSnap_RegistFig(type, actor, const cXyz& pos, s16 angle, f32, f32, f32) (HD signature) */
static inline void dSnap_RegistFig_pos(s32 type, fopAc_ac_c* a, cXyz* pos, s16 ang, f32 x, f32 y, f32 z) {
    gabi::call(0x025BEBB8, type, a, pos, ang, x, y, z);
}
/* J3DModelData::getJointName()->getIndex(name) (HD: name table at +0x10 relative offset) */
static inline s8 getJointIndex(J3DModelData* d, u32 name) {
    u32 r = gabi::call<u32>(0x027F68FC, d);
    s32 off = gabi::load<s32>(r + 0x10);
    u32 tbl = 0;
    if (off != 0) tbl = r + 0x10 + off;
    return (s8)gabi::call<s32>(0x027DF9B0, tbl, name);
}
/* 027F3F94 (the matcher names it __nw): the model data's joint tree; joint count (u16) at +8 */
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::ea(gabi::call<void*>(0x027F3F94, d)) + 8); }
/* J3DModelData::getJointNodePointer(i)->setCallBack(cb) (HD: nodes of 0x1C bytes from +8; index checked against +4) */
static inline void setJointCallBack(J3DModelData* d, u16 i, u32 cb) {
    u32 data = gabi::ea(d);
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
/* mDoExt_btpAnm::init 025E789C, mDoExt_brkAnm::init 025E8154 (HD) */
static inline BOOL mDoExt_btpAnm_init(void* a, J3DModelData* d, void* btp, s32 anmPlay, s32 mode, f32 rate, s16 start, s16 end,
                                      bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E789C, a, d, btp, anmPlay, mode, rate, start, end, modify, entry);
}
static inline BOOL mDoExt_brkAnm_init(void* a, J3DModelData* d, void* brk, s32 anmPlay, s32 mode, f32 rate, s16 start, s16 end,
                                      bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, brk, anmPlay, mode, rate, start, end, modify, entry);
}
/* 0259D54C dNpc_playerEyePos(f32) -> cXyz (HD: returned through a hidden result pointer) */
static inline void dNpc_playerEyePos_r(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16, s16, bool) */
static inline void lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* y, cXyz* tgt, cXyz* eye, s16 a, s16 b, bool c) {
    gabi::call(0x0259DED0, j, y, tgt, eye, a, b, c);
}

#define STR_JI 0x1001AEE4         /* "Ji" (CreateHeap) */
#define STR_ASSERT_FILE 0x1001AEF0

/* 02249084 */
BOOL nodeCallBack1(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x02249084, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel_l* model = j3dSys_getModel();
        daNpc_Ji1_c* i_this = gabi::at<daNpc_Ji1_c>(model->mUserArea);
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 jntNo = J3DJoint_getJntNo(joint);
        if (i_this) {
            Mtx34* now = mDoMtx_stack_c::get();
            PSMTXCopy(getAnmMtx((J3DModel*)model, jntNo), now);
            if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
                be<s16>* headAnm = (be<s16>*)i_this->mHeadAnm; /* mHeadAnm.field_0x00 (csXyz) */
                mDoMtx_XrotM(now, (s16)(headAnm[1] + i_this->m_jnt.mAngles[0][1]));
                mDoMtx_ZrotM(now, (s16)(headAnm[0] - i_this->m_jnt.mAngles[0][0]));
            }
            if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
                mDoMtx_XrotM(now, i_this->m_jnt.mAngles[1][1]);
                mDoMtx_ZrotM(now, (s16)-i_this->m_jnt.mAngles[1][0]);
                for (int i = 0; i < 3; i++) {
                    PSMTXMultVec_(now, &l_HIO().field_0xC4[i], &i_this->field_0xBD8[i]);
                }
            }
            mtx_copy(getAnmMtx((J3DModel*)model, jntNo), now); /* setAnmMtx */
            PSMTXCopy(now, J3DSys_mCurrentMtx());
        }
    }
    return true;
}
VERIFY(0x02249084, nodeCallBack1);

/* 02249224 */
BOOL nodeCallBack2(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x02249224, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel_l* model = j3dSys_getModel();
        daNpc_Ji1_c* i_this = gabi::at<daNpc_Ji1_c>(model->mUserArea);
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 jntNo = J3DJoint_getJntNo(joint);
        if (i_this) {
            PSMTXCopy(getAnmMtx((J3DModel*)model, jntNo), calc_mtx());
            if (jntNo == (u32)(s32)i_this->hair1JointNo) {
                cMtx_ZrotM(calc_mtx(), (s16)(i_this->field_0xBAA + i_this->field_0xBD2));
                cMtx_YrotM(calc_mtx(), i_this->field_0xBAC);
            } else if (jntNo == (u32)(s32)i_this->hair2JointNo) {
                cMtx_ZrotM(calc_mtx(), (s16)(i_this->field_0xBAE + i_this->field_0xBD4));
                cMtx_YrotM(calc_mtx(), i_this->field_0xBB0);
            } else {
                cMtx_ZrotM(calc_mtx(), (s16)(i_this->field_0xBB2 + i_this->field_0xBD6));
                cMtx_YrotM(calc_mtx(), i_this->field_0xBB4);
            }
            mtx_copy(getAnmMtx((J3DModel*)model, jntNo), calc_mtx());
            PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
        }
    }
    return true;
}
VERIFY(0x02249224, nodeCallBack2);

/* 022494C8 */
BOOL nodeCallBack3(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022494C8, BOOL, node, calcTiming);
    if (calcTiming != 0) {
        return true;
    } else {
        J3DModel_l* model = j3dSys_getModel();
        daNpc_Ji1_c* i_this = gabi::at<daNpc_Ji1_c>(model->mUserArea);
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 jntNo = J3DJoint_getJntNo(joint);
        Mtx34* now = mDoMtx_stack_c::get();
        PSMTXCopy(getAnmMtx((J3DModel*)model, jntNo), now);
        s16 temp = (s16)gabi::ftoi((f32)(s16)i_this->field_0xBA2 * cM_ssin((u16)i_this->field_0xBA4));
        if (jntNo == (u32)(s32)i_this->armLJointNo) {
            mDoMtx_XrotM(now, temp);
        } else if (jntNo == (u32)(s32)i_this->armRJointNo) {
            mDoMtx_XrotM(now, temp);
        }
        mtx_copy(getAnmMtx((J3DModel*)model, jntNo), now);
        PSMTXCopy(now, J3DSys_mCurrentMtx());
        return true;
    }
}
VERIFY(0x022494C8, nodeCallBack3);

/* 02249638 HD: no blob shadow; dSnap_RegistFig takes the position by reference */
BOOL daNpc_Ji1_c::_draw() {
    WWHD_FUNC(0x02249638, BOOL, this);
    if (mHide) {
        return true;
    }
    J3DModel* pOrcaModel = mpOrcaMorf->getModel();
    J3DModelData* pOrcaModelData = J3DModel_getModelData(pOrcaModel);
    J3DModel* pSpearModel = mpSpearMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pOrcaModel, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pSpearModel, &tevStr);
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)&mBlinkAnim, pOrcaModelData, (u8)mBlinkFrame);
    mpOrcaMorf->entryDL();
    if (field_0xD84 == 1) {
        mpSpearMorf->entryDL();
    } else if (field_0xD84 == 2) {
        mpSpearMorf->updateDL();
    }
    {
        f32 f = (f32)(s16)gabi::ftoi(mCryBtkFrame);
        mDoExt_btkAnm_entry((mDoExt_btkAnm*)&mCryBtk, J3DModel_getModelData(mpTearsModel), f);
    }
    {
        f32 f = (f32)(s16)gabi::ftoi(mCryBrkFrame);
        mDoExt_brkAnm_entry((mDoExt_brkAnm*)&mCryBrk, J3DModel_getModelData(mpTearsModel), f);
    }
    mDoExt_modelUpdateDL(mpTearsModel);
    dSnap_RegistFig_pos(0x4D /* DSNAP_TYPE_NPC_JI1 */, this, &current.pos, current.angle.y, 1.0f, 1.0f, 1.0f);
    return true;
}
VERIFY(0x02249638, &daNpc_Ji1_c::_draw);

/* 02249884 */
BOOL daNpc_Ji1_c::CreateHeap() {
    WWHD_FUNC(0x02249884, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(STR_JI), 0x42 /* BDL_JI */, SAFESTRING_VTBL);
    void* anm = dComIfG_getObjectRes(STR(STR_JI), 0x3F /* BCK_WAIT01 */, SAFESTRING_VTBL);
    void* bas = dComIfG_getObjectRes(STR(STR_JI), 0x21 /* BAS_WAIT01 */, SAFESTRING_VTBL);
    mpOrcaMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, (J3DAnmTransform*)anm, J3DFrameCtrl::EMode_LOOP,
                                        1.0f, 0, -1, 1, bas, 0, 0x11020203);
    if (mpOrcaMorf.get() == nullptr || mpOrcaMorf->getModel() == nullptr) {
        return false;
    }
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpOrcaMorf->getModel(), mDoMtx_stack_c::get());

    s8 n;
    n = getJointIndex(modelData, 0x1001AEE8 /* "head" */);
    m_jnt.mHeadJntNum = n;
    if (n < 0) JUT_ASSERT_fail(STR(STR_ASSERT_FILE), 0x1629, STR(0x1001AFC0));
    n = getJointIndex(modelData, 0x1001AF30 /* "backbone1" */);
    m_jnt.mBackboneJntNum = n;
    if (n < 0) JUT_ASSERT_fail(STR(STR_ASSERT_FILE), 0x162E, STR(0x1001AFDC));
    n = getJointIndex(modelData, 0x1001AEB4 /* "armL1" */);
    armLJointNo = n;
    if (n < 0) JUT_ASSERT_fail(STR(STR_ASSERT_FILE), 0x1632, STR(0x1001AFFC));
    n = getJointIndex(modelData, 0x1001AEBC /* "armR1" */);
    armRJointNo = n;
    if (n < 0) JUT_ASSERT_fail(STR(STR_ASSERT_FILE), 0x1635, STR(0x1001B010));
    n = getJointIndex(modelData, 0x1001AEC4 /* "handR" */);
    handRJointNo = n;
    if (n < 0) JUT_ASSERT_fail(STR(STR_ASSERT_FILE), 0x1638, STR(0x1001AF3C));

    J3DModelData* yariData = (J3DModelData*)dComIfG_getObjectRes(STR(STR_JI), 0x43 /* BDL_JI_YARI */, SAFESTRING_VTBL);
    void* yariAnm = dComIfG_getObjectRes(STR(STR_JI), 0x3D /* BCK_JIYARI_TATEATTACK */, SAFESTRING_VTBL);
    mpSpearMorf = mDoExt_McaMorf::create(nullptr, yariData, nullptr, nullptr, (J3DAnmTransform*)yariAnm, J3DFrameCtrl::EMode_NONE,
                                         0.0f, 0, -1, 1, nullptr, 0, 0x11020203);
    if (mpSpearMorf.get() == nullptr || mpSpearMorf->getModel() == nullptr) {
        return false;
    }

    J3DModelData* modelData2 = (J3DModelData*)dComIfG_getObjectRes(STR(STR_JI), 0x44 /* BDL_YJITR00 */, SAFESTRING_VTBL);
    mpTearsModel = mDoExt_J3DModel__create(modelData2, 0, 0x11020203);
    void* a_brk = dComIfG_getObjectRes(STR(STR_JI), 0x47 /* BRK_YJITR00 */, SAFESTRING_VTBL);
    if (a_brk == nullptr) JUT_ASSERT_fail(STR(STR_ASSERT_FILE), 0x165E, STR(0x1001B024));
    void* a_btk = dComIfG_getObjectRes(STR(STR_JI), 0x4A /* BTK_YJITR00 */, SAFESTRING_VTBL);
    if (a_btk == nullptr) JUT_ASSERT_fail(STR(STR_ASSERT_FILE), 0x1661, STR(0x1001B034));
    BOOL temp1 = mDoExt_brkAnm_init(&mCryBrk, modelData2, a_brk, false, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0);
    BOOL temp2 = mCryBtk.init(modelData2, (J3DAnmTextureSRTKey*)a_btk, false, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0);
    if (mpTearsModel.get() == nullptr || temp1 == 0 || temp2 == 0) {
        return false;
    }

    headTexPattern = (J3DAnmTexPattern*)dComIfG_getObjectRes(STR(STR_JI), 0x4D /* BTP_JI */, SAFESTRING_VTBL);
    if (headTexPattern.get() == nullptr) JUT_ASSERT_fail(STR(STR_ASSERT_FILE), 0x1669, STR(0x1001AF50));
    if (mDoExt_btpAnm_init(&mBlinkAnim, modelData, headTexPattern.get(), TRUE, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0) == 0) {
        return false;
    }

    n = getJointIndex(modelData, 0x1001AECC /* "hair1" */);
    hair1JointNo = n;
    if (n < 0) JUT_ASSERT_fail(STR(STR_ASSERT_FILE), 0x1671, STR(0x1001AF68));
    n = getJointIndex(modelData, 0x1001AED4 /* "hair2" */);
    hair2JointNo = n;
    if (n < 0) JUT_ASSERT_fail(STR(STR_ASSERT_FILE), 0x1673, STR(0x1001AF7C));
    n = getJointIndex(modelData, 0x1001AEDC /* "hair3" */);
    hair3JointNo = n;
    if (n < 0) JUT_ASSERT_fail(STR(STR_ASSERT_FILE), 0x1675, STR(0x1001AF90));

    for (u16 i = 0; i < J3DModelData_getJointNum(modelData); i++) {
        if (i == hair1JointNo || i == hair2JointNo || i == hair3JointNo) {
            setJointCallBack(modelData, i, 0x02249224 /* nodeCallBack2 */);
        }
    }
    for (u16 i = 0; i < J3DModelData_getJointNum(modelData); i++) {
        if (i == m_jnt.mHeadJntNum || i == m_jnt.mBackboneJntNum) {
            setJointCallBack(modelData, i, 0x02249084 /* nodeCallBack1 */);
        } else if (i == armLJointNo || i == armRJointNo) {
            setJointCallBack(modelData, i, 0x022494C8 /* nodeCallBack3 */);
        }
    }

    ((J3DModel_l*)mpOrcaMorf->getModel())->mUserArea = gabi::ea(this);
    mAcchCir.SetWall(60.0f, 50.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
    mAcch.OnLineCheck();

    static const u32 names[0x12] = {
        0x1001B044, 0x1001AFA4, 0x1001B054, 0x1001B064, 0x1001B074, 0x1001B084, 0x1001B090, 0x1001B0A0, 0x1001B0B0,
        0x1001AF00, 0x1001B0C4, 0x1001B0D8, 0x1001AF10, 0x1001B0E8, 0x1001B0F8, 0x1001AF1C, 0x1001B108, 0x1001AFB0,
    };
    for (int i = 0; i < 0x12; i++) {
        mEventIdx[i] = dComIfGp_evmng_getEventIdx(STR(names[i]), 0xFF);
    }

    if (dComIfGs_isEventBit(0x0001) && !dComIfGs_isEventBit(0x0108)) {
        gabi::store<s16>(gabi::ea(this) + 0xFC, mEventIdx[0x11]); /* eventInfo.setEventId */
    }
    return true;
}
VERIFY(0x02249884, &daNpc_Ji1_c::CreateHeap);

/* 0224AA24 */
BOOL daNpc_Ji1_c::chkAttention(cXyz* param_1, s16 param_2) {
    WWHD_FUNC(0x0224AA24, BOOL, this, param_1, param_2);
    fopAc_ac_c* player = daPy_getPlayerActorClass();
    f32 temp = 800.0f;
    s32 temp2 = l_HIO().field_0x10 + l_HIO().field_0x12;
    f32 x = player->current.pos.x - param_1->x;
    f32 z = player->current.pos.z - param_1->z;
    f32 dist = std_sqrtf(gabi::fmadds(x, x, z * z));
    s16 angle = cM_atan2s(x, z);
    if (field_0xD7A != 0) {
        temp += 40.0f;
        temp2 += 0x71C;
    }
    angle -= param_2;
    s32 a = angle < 0 ? -angle : angle;
    field_0xD7A = temp2 > a && !(temp <= dist);
    return temp2 > a && !(temp <= dist);
}
VERIFY(0x0224AA24, &daNpc_Ji1_c::chkAttention);

/* 0224ABA4 */
BOOL daNpc_Ji1_c::lookBack() {
    WWHD_FUNC(0x0224ABA4, BOOL, this);
    BOOL ret = false;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> temp;
    cXyz_mi(&player->current.pos, temp, &current.pos);
    gabi::Local<cXyz> xz;
    xz->x = (f32)temp->x;
    xz->y = 0.0f;
    xz->z = (f32)temp->z;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    bool temp2 = true;
    {
        gabi::Local<cXyz> pos;
        pos->x = (f32)current.pos.x;
        pos->y = (f32)current.pos.y;
        pos->z = (f32)current.pos.z;
        chkAttention(pos, current.angle.y);
    }
    gabi::Local<cXyz> attnPos;
    if (mEventCut.mbAttention) {
        attnPos->x = (f32)mEventCut.mPos.x;
        attnPos->y = (f32)mEventCut.mPos.y;
        attnPos->z = (f32)mEventCut.mPos.z;
    }
    cXyz* dstPos;
    if (field_0xD7E) {
        /* daPy_py_c::getLeftHandPos(): the player's cXyz at +0x3F0 */
        cXyz* lh = gabi::at<cXyz>(gabi::ea(player) + 0x3F0);
        attnPos->x = (f32)lh->x;
        attnPos->y = (f32)lh->y;
        attnPos->z = (f32)lh->z;
        dstPos = attnPos;
    } else {
        if ((field_0xD7A != 0 || isGuardAnim()) ||
            (ptmf_eq(mAction, ACT_kaitenwaitAction) && !dComIfGs_isEventBit(0x0501)) ||
            (!ptmf_eq(mAction, ACT_normalAction) && dist < 600.0f)) {
            if (mAnimation == 0xD) {
                dstPos = nullptr;
            } else {
                gabi::Local<cXyz> eye;
                dNpc_playerEyePos_r(eye, 0.0f);
                attnPos->copy(*eye);
                dstPos = attnPos;
            }
        } else {
            dstPos = nullptr;
        }
    }
    s16 maxHeadRot = l_HIO().field_0x10;
    s16 maxSpineRot = l_HIO().field_0x12;
    if (mAnimation == 0x10 || mAnimation == 0x11) {
        maxHeadRot = 0x3E80;
        maxSpineRot = 0x1F40;
    }
    m_jnt.setParam(l_HIO().field_0x0E, maxSpineRot, -l_HIO().field_0x0E, -maxHeadRot, l_HIO().field_0x0C, maxHeadRot,
                   -l_HIO().field_0x0C, -maxHeadRot, l_HIO().field_0x14);
    if (m_jnt.mbTrn) {
        cLib_addCalcAngleS2(&field_0xC88, l_HIO().field_0x18, l_HIO().field_0x16, 0x800);
        temp2 = false;
        ret = true;
    } else {
        field_0xC88 = 0;
    }
    gabi::Local<cXyz> eye;
    eye->x = (f32)eyePos.x;
    eye->y = (f32)eyePos.y;
    eye->z = (f32)eyePos.z;
    lookAtTarget(&m_jnt, &current.angle.y, dstPos, eye, current.angle.y, field_0xC88, temp2);
    return ret;
}
VERIFY(0x0224ABA4, &daNpc_Ji1_c::lookBack);

/* 0224AEF0 */
void daNpc_Ji1_c::set_mtx() {
    WWHD_FUNC(0x0224AEF0, void, this);
    J3DModel* pModel = mpOrcaMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
    mpOrcaMorf->calc();
    {
        Mtx34* src = getAnmMtx(pModel, m_jnt.mHeadJntNum);
        J3DModel_setBaseTRMtx(mpTearsModel, src);
    }
    JPABaseEmitter* e = field_0x430;
    if (e) {
        Mtx34* m = getAnmMtx(pModel, m_jnt.mHeadJntNum);
        JPASetRMtxTVecfromMtx(m, gabi::ea(e) + 0x1F0, gabi::ea(e) + 0x22C); /* setGlobalRTMatrix */
    }
    if (field_0xD84 == 1) {
        PSMTXCopy(getAnmMtx(pModel, handRJointNo), mDoMtx_stack_c::get());
        mDoMtx_XYZrotM(mDoMtx_stack_c::get(), field_0xD5C.x, field_0xD5C.y, field_0xD5C.z);
        mDoMtx_stack_c::transM(field_0xD50.x, field_0xD50.y, field_0xD50.z);
        J3DModel_setBaseTRMtx(mpSpearMorf->getModel(), mDoMtx_stack_c::get());
    }
    mpSpearMorf->calc();
    if (field_0xD84) {
        gabi::Local<cXyz> temp;
        gabi::Local<cXyz> temp2;
        temp->set(0.0f, 0.0f, 100.0f);
        temp2->set(0.0f, 0.0f, -100.0f);
        if (mAnimation == 0x14) {
            temp->z = 70.0f;
        }
        field_0xB90.copy(field_0xB78);
        PSMTXCopy(getAnmMtx(mpSpearMorf->getModel(), 1 /* JI_YARI_JNT_JI_YARI_e */), mDoMtx_stack_c::get());
        PSMTXMultVec_(mDoMtx_stack_c::get(), temp, &field_0xB78);
        PSMTXMultVec_(mDoMtx_stack_c::get(), temp2, &field_0xB84);
    }
}
VERIFY(0x0224AEF0, &daNpc_Ji1_c::set_mtx);

/* ---- harpoon (local bindings, SHARED-CANDIDATE) ---- */
#define ZeroQuat gabi::at<Quaternion_l>(0x101E9C38)
#define cXyz_BaseY gabi::at<cXyz>(0x101FFBC0)
static inline void PSQUATMultiply(Quaternion_l* a, Quaternion_l* b, Quaternion_l* ab) { gabi::call(0x028E8B48, a, b, ab); }
static inline void PSQUATNormalize(Quaternion_l* a, Quaternion_l* out) { gabi::call(0x028E8C0C, a, out); }
static inline void C_QUATSlerp(Quaternion_l* p, Quaternion_l* q, Quaternion_l* r, f32 t) { gabi::call(0x028E9BC0, p, q, r, t); }
static inline void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* out) { gabi::call(0x028E8DAC, a, b, out); }
static inline void PSMTXConcat(Mtx34* a, Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
static inline void mDoMtx_stack_quatM(Quaternion_l* q) { gabi::call(0x025F25CC, q); }
/* 023127F8 daObj::quat_rotVec(Quaternion*, const cXyz&, const cXyz&) */
static inline void daObj_quat_rotVec(Quaternion_l* q, cXyz* a, cXyz* b) { gabi::call(0x023127F8, q, a, b); }
static inline BOOL cXyz_normalizeRS(cXyz* v) { return gabi::call<BOOL>(0x0201B47C, v); }
/* 025E1A04 mDoAud_seStart(id, pos, param) (HD: no reverb argument) */
static inline void mDoAud_seStart3(u32 id, cXyz* pos, u32 param) { gabi::call(0x025E1A04, id, pos, param); }
/* (u32)f for f >= 0 (GHS: through 2^31) */
static inline u32 f2u(f32 f) {
    if (f < 2147483648.0f) return (u32)gabi::ftoi(f);
    return (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000u;
}
static inline void quat_copy_neg(Quaternion_l* dst, Quaternion_l* src) {
    dst->x = -src->x;
    dst->y = (f32)src->y;
    dst->z = -src->z;
    dst->w = (f32)src->w;
}

/* 0224DE3C */
void daNpc_Ji1_c::harpoonRelease(cXyz* param_1) {
    WWHD_FUNC(0x0224DE3C, void, this, param_1);
    PSMTXCopy(getAnmMtx(mpSpearMorf->getModel(), 1 /* JI_YARI_JNT_JI_YARI_e */), &field_0xCA0);
    mpSpearMorf->setFrame(0.0f);
    field_0xCD0.x = field_0xCA0.m[0][3];
    field_0xCD0.y = field_0xCA0.m[1][3];
    field_0xCD0.z = field_0xCA0.m[2][3];
    field_0xCA0.m[0][3] = 0.0f;
    field_0xCA0.m[1][3] = 0.0f;
    field_0xCA0.m[2][3] = 0.0f;
    if (param_1) {
        field_0xCDC.copy(*param_1);
    } else {
        field_0xCDC.set(0.0f, 5.0f, 0.0f);
    }
    field_0xD14 = 0x400;
    field_0xD04.x = cM_ssin((u16)field_0xD14) * cM_scos((u16)current.angle.y);
    field_0xD04.y = 0.0f;
    field_0xD04.z = cM_ssin((u16)field_0xD14) * cM_ssin((u16)current.angle.y);
    field_0xD04.w = cM_scos((u16)field_0xD14);
    /* field_0xCF4 = ZeroQuat (integer word copy) */
    u32 q = gabi::ea(&field_0xCF4), z = gabi::ea(ZeroQuat);
    gabi::store<u32>(q + 0x0, gabi::load<u32>(z + 0x0));
    gabi::store<u32>(q + 0x4, gabi::load<u32>(z + 0x4));
    gabi::store<u32>(q + 0x8, gabi::load<u32>(z + 0x8));
    gabi::store<u32>(q + 0xC, gabi::load<u32>(z + 0xC));
    field_0xD84 = 2;
}
VERIFY(0x0224DE3C, &daNpc_Ji1_c::harpoonRelease);

/* 0224A2B8 */
void daNpc_Ji1_c::harpoonMove() {
    WWHD_FUNC(0x0224A2B8, void, this);
    gabi::Local<cXyz> temp, temp2, temp3, temp4;
    temp3->x = (f32)field_0xB78.x;
    temp4->z = (f32)field_0xB84.z;
    temp2->z = -100.0f;
    temp->z = 100.0f;
    temp->x = 0.0f;
    temp->y = 0.0f;
    temp2->x = 0.0f;
    temp2->y = 0.0f;
    temp4->x = (f32)field_0xB84.x;
    temp4->y = (f32)field_0xB84.y;
    temp3->z = (f32)field_0xB78.z;
    temp3->y = (f32)field_0xB78.y;
    f32 hio_bc = l_HIO().field_0xBC;
    f32 hio_c0 = l_HIO().field_0xC0;

    PSQUATMultiply(&field_0xCF4, &field_0xD04, &field_0xCF4);
    PSQUATNormalize(&field_0xCF4, &field_0xCF4);
    PSVECAdd(&field_0xCD0, &field_0xCDC, &field_0xCD0);
    field_0xCDC.y = field_0xCDC.y + hio_c0;
    PSVECScale(&field_0xCDC, &field_0xCDC, 0.95f);
    mDoMtx_stack_c::transS(field_0xCD0.x, field_0xCD0.y, field_0xCD0.z);
    mDoMtx_stack_quatM(&field_0xCF4);
    PSMTXConcat(mDoMtx_stack_c::get(), &field_0xCA0, mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(mpSpearMorf->getModel(), mDoMtx_stack_c::get());
    PSMTXMultVec_(mDoMtx_stack_c::get(), temp, &field_0xB78);
    PSMTXMultVec_(mDoMtx_stack_c::get(), temp2, &field_0xB84);

    if (field_0xB78.y < 0.0f || field_0xB84.y < 0.0f) {
        gabi::Local<cXyz> temp5, temp6;
        cXyz_mi(&field_0xB78, temp5, &field_0xCD0);
        cXyz_mi(&field_0xB84, temp6, &field_0xCD0);
        PSVECSubtract(temp3, &field_0xCD0, temp3);
        PSVECSubtract(temp4, &field_0xCD0, temp4);
        gabi::Local<Quaternion_l> temp7, temp8, temp11;
        daObj_quat_rotVec(temp7, temp3, temp5);
        daObj_quat_rotVec(temp8, temp4, temp6);

        cXyz* sePos = nullptr;
        gabi::Local<cXyz> temp12;
        if (field_0xB78.y < 0.0f) {
            if (!(field_0xB84.y < 0.0f)) {
                f32 b78y = field_0xB78.y;
                field_0xB84.y = field_0xB84.y - b78y;
                field_0xCD0.y = field_0xCD0.y - b78y;
                field_0xB78.y = 0.0f;
                quat_copy_neg(temp7, &field_0xD04);
                gabi::Local<cXyz> temp10;
                temp10->x = (f32)temp6->x;
                temp10->y = 0.0f;
                temp10->z = (f32)temp6->z;
                daObj_quat_rotVec(temp11, temp6, temp10);
                C_QUATSlerp(temp8, temp11, temp8, 0.05f);
                C_QUATSlerp(temp7, ZeroQuat, temp7, 0.1f);
                C_QUATSlerp(temp8, temp7, &field_0xD04, 0.5f);
                cXyz_mi(temp5, temp12, temp3);
                sePos = &field_0xB78;
                goto bound;
            }
        } else if (field_0xB84.y < 0.0f) {
            f32 b84y = field_0xB84.y;
            field_0xCD0.y = field_0xCD0.y - b84y;
            field_0xB78.y = field_0xB78.y - b84y;
            field_0xB84.y = 0.0f;
            quat_copy_neg(temp8, &field_0xD04);
            gabi::Local<cXyz> temp10;
            temp10->x = (f32)temp5->x;
            temp10->y = 0.0f;
            temp10->z = (f32)temp5->z;
            daObj_quat_rotVec(temp11, temp5, temp10);
            C_QUATSlerp(temp7, temp11, temp7, 0.1f);
            C_QUATSlerp(temp8, ZeroQuat, temp8, 0.1f);
            C_QUATSlerp(temp7, temp8, &field_0xD04, 0.5f);
            cXyz_mi(temp6, temp12, temp4);
            sePos = &field_0xB84;
            goto bound;
        }
        {
            gabi::Local<cXyz> temp10;
            temp10->x = (f32)temp5->x;
            temp10->y = 0.0f;
            temp10->z = (f32)temp5->z;
            daObj_quat_rotVec(temp7, temp5, temp10);
            field_0xCD0.y = gabi::fnmsubs(field_0xB84.y + field_0xB78.y, 0.5f, field_0xCD0.y);
            C_QUATSlerp(&field_0xD04, temp7, &field_0xD04, 0.25f);
            field_0xCDC.y = field_0xCDC.y * hio_bc;
        }
        goto tail;
    bound:
        if (temp12->y < -3.0f && cXyz_normalizeRS(temp12)) {
            u32 temp13 = f2u(std::fabs(temp12->y * 100.0f));
            u32 temp14 = temp13 > 100 ? 100 : temp13;
            mDoAud_seStart3(0x5807 /* JA_SE_CM_LANCE_BOUND */, sePos, temp14);
            f32 d = PSVECDotProduct(temp12, cXyz_BaseY);
            f32 k = hio_bc * d;
            field_0xCDC.y = field_0xCDC.y * k;
        } else {
            field_0xCDC.y = 0.0f;
        }
    tail:
        gabi::Local<cXyz> temp9;
        cXyz_mi(&field_0xB78, temp9, &field_0xB84);
        gabi::Local<cXyz> up;
        up->set(0.0f, 1.0f, 0.0f);
        f32 dot = PSVECDotProduct(temp9, up);
        if (dot < 1.0f) {
            gabi::Local<cXyz> temp10;
            temp10->x = (f32)temp5->x;
            temp10->y = 0.0f;
            temp10->z = (f32)temp5->z;
            daObj_quat_rotVec(temp7, temp5, temp10);
            field_0xCD0.y = gabi::fnmsubs(field_0xB84.y + field_0xB78.y, 0.5f, field_0xCD0.y);
            C_QUATSlerp(&field_0xD04, temp7, &field_0xD04, 0.25f);
            field_0xCDC.y = field_0xCDC.y * hio_bc;
        }
    }
}
VERIFY(0x0224A2B8, &daNpc_Ji1_c::harpoonMove);
