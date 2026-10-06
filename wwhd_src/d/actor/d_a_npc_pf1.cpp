/**
 * d_a_npc_pf1.cpp (WWHD)
 * NPC - Maggie's father (poor)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_pf1.cpp) has only "Nonmatching" stubs for this unit: every function here is
 * written from the WWHD code (cking.rpx) with the GameCube names, and verified against it.
 */
#include "d/actor/d_a_npc_pf1.h"

#define SAFESTRING_VTBL 0x10020A14 /* this TU's sead::SafeString vtable */
#define PF1_VTBL 0x10020C08        /* daNpc_Pf1_c vtable */

/* ---- local bindings (SHARED-CANDIDATE; the same as in d_a_npc_aj1.cpp) ---- */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
/* the arc name is a member (mArcName) in this unit */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
static inline s32 J3DAnm_getFrameMax(void* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* 021E1E78 cLib_getRndValue<int>(base, range) (out-of-line copy in another TU) */
static inline s32 cLib_getRndValue(s32 base, s32 range) { return gabi::call<s32>(0x021E1E78, base, range); }
/* 025E789C mDoExt_btpAnm::init */
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
static inline u32 dBgS_GetMtrlSndId(dBgS* bgs, void* poly) { return gabi::call<u32>(0x024EECAC, bgs, poly); }
static inline void* daNpc_gndPoly(fopNpc_npc_l* a) { return gabi::at<u8>(gabi::ea(&a->mObjAcch) + 0xD4 + 0x14); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
/* HD message manager (*(0x101F4B5C)) */
static inline u32 l_msgMng() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 fopMsgM_getStatus() { return gabi::call<u32>(0x025F795C, gabi::load<u32>(0x101F4B5C)); }
/* 0259E2D4 dNpc_JntCtrl_c::lookAtTarget_2(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16, s16, bool) */
static inline void dNpc_JntCtrl_lookAtTarget_2(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259E2D4, j, outY, target, eye, yrot, vel, headOnly);
}
/* 025D54C4 fopAcM_SearchByID(id, fopAc_ac_c** out) (out of line) */
static inline BOOL fopAcM_SearchByID2(fpc_ProcID id, be<u32>* out) { return gabi::call<BOOL>(0x025D54C4, id, out); }
/* dNpc_PathRun_c */
static inline void dNpc_PathRun_setInf(dNpc_PathRun_pf1* p, u8 path, s8 room, u8 fwd) { gabi::call(0x0259E6D0, p, path, room, fwd); }
static inline void dNpc_PathRun_getPoint(dNpc_PathRun_pf1* p, cXyz* out, u32 idx) { gabi::call(0x0259E778, p, out, idx); }
static inline BOOL dNpc_PathRun_chkPointPass(dNpc_PathRun_pf1* p, cXyz* pos, bool dir) { return gabi::call<BOOL>(0x0259E838, p, pos, dir); }
static inline BOOL dNpc_PathRun_nextIdx(dNpc_PathRun_pf1* p) { return gabi::call<BOOL>(0x0259ECFC, p); }
static inline BOOL dNpc_PathRun_nextIdxAuto(dNpc_PathRun_pf1* p) { return gabi::call<BOOL>(0x0259ED58, p); }
static inline void dNpc_PathRun_setNearPathIndx(dNpc_PathRun_pf1* p, cXyz* pos, f32 dist) { gabi::call(0x0259EE7C, p, pos, dist); }
/* 0200F974 cLib_targetAngleX */
static inline s16 cLib_targetAngleX(cXyz* a, cXyz* b) { return gabi::call<s16>(0x0200F974, a, b); }
/* 028E8DE8 PSVECSquareDistance */
static inline f32 PSVECSquareDistance(const cXyz* a, const cXyz* b) { return gabi::call<f32>(0x028E8DE8, a, b); }
/* 025E19CC mDoAud_seStart(id, pos) (two-argument form, no reverb) */
static inline void mDoAud_seStart2(u32 id, cXyz* pos) { gabi::call(0x025E19CC, id, pos); }
/* 025D5928 fopAcM_fastCreate */
static inline fopAc_ac_c* fopAcM_fastCreate(s16 name, u32 param, cXyz* pos, s32 roomNo, csXyz* angle, cXyz* scale, s8 subtype,
                                            u32 createFunc, void* data) {
    return gabi::call<fopAc_ac_c*>(0x025D5928, name, param, pos, roomNo, angle, scale, subtype, createFunc, data);
}
/* g_Counter.mCounter0 (HD 0x101FF558) */
static inline u32 g_Counter0() { return gabi::load<u32>(0x101FF558); }

/* GHS pointer to member function call */
static inline void pmf_load(ProcFunc_l* dst, u32 src) {
    gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(src));
    gabi::store<u32>(gabi::ea(dst) + 4, gabi::load<u32>(src + 4));
}
static inline void pmf_call(void* self, ProcFunc_l* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}
enum : u32 { PMF_wait_action1 = 0x100209F0 };

/* HD J3D joint matrices (see d_a_npc_ba1.cpp) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    J3DMtxBlock_l* blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(gabi::ea(model) + 0x2C));
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}
static inline u32 jntNo_of(J3DNode* node) { return gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4); }
static inline Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }

/* ---- file statics ---- */
struct daNpc_Pf1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mMaxHeadX;
        /* 0x02 */ be<s16> mMaxHeadY;
        /* 0x04 */ be<s16> mMinHeadX;
        /* 0x06 */ be<s16> mMinHeadY;
        /* 0x08 */ be<s16> mMaxBackBoneX;
        /* 0x0A */ be<s16> mMaxBackBoneY;
        /* 0x0C */ be<s16> mMinBackBoneX;
        /* 0x0E */ be<s16> mMinBackBoneY;
        /* 0x10 */ be<s16> mMaxTurnStep;
        /* 0x12 */ be<s16> mLookVelMax;
        /* 0x14 */ be<f32> mAttPosOffsetY;
        /* 0x18 */ be<u8> mDebugDraw;
        /* 0x19 */ u8 _19;
        /* 0x1A */ be<s16> mTurnScale;
        /* 0x1C */ be<s16> mTurnMaxStep;
        /* 0x1E */ u8 _1E[2];
        /* 0x20 */ be<f32> mWalkAnmRate;
        /* 0x24 */ be<f32> mWalkSpeed;
        /* 0x28 */ be<f32> mWalkAccel;
        /* 0x2C */ be<f32> mAttkAnmRate;
        /* 0x30 */ be<f32> mAttkSpeed;
        /* 0x34 */ be<f32> mAttkAccel;
        /* 0x38 */ be<f32> mStartDist;
        /* 0x3C */ be<f32> mAreaDist;
    };
    /* 0x00 */ be<u32> __vtbl; /* HD: vtable first */
    /* 0x04 */ be<s8> m04;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> m08;
    /* 0x0C */ hio_prm_c mPrm;
};
WWHD_SIZE(daNpc_Pf1_HIO_c, 0x4C);
static daNpc_Pf1_HIO_c& l_HIO() { return *gabi::at<daNpc_Pf1_HIO_c>(0x10468358); }

/* 022C887C */
void daNpc_Pf1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x022C887C, void, this, i_node, i_model);
    /* static cXyz a_eye_pos_off(20.0f, -30.0f, 0.0f): guard 0x104683A4, object 0x1046834C */
    cXyz* a_eye_pos_off = gabi::at<cXyz>(0x1046834C);
    if (gabi::load<u32>(0x104683A4) == 0) {
        gabi::store<u32>(0x104683A4, 1);
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->x = 20.0f;
        a_eye_pos_off->y = -30.0f;
    }
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    mHeadPos.x = stk->m[0][3];
    mHeadPos.y = stk->m[1][3];
    mHeadPos.z = stk->m[2][3];
    PSMTXMultVec(stk, a_eye_pos_off, &mEyePos);
    mDoMtx_YrotM(stk, -m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[0][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}
VERIFY(0x022C887C, &daNpc_Pf1_c::_nodeCB_Head);

/* node callbacks: if (calcTiming == In && j3dSys.getModel()->getUserArea()) actor->_nodeCB_X(node, model) */
static inline void nodeCB_dispatch(J3DNode* node, int timing, u32 fn) {
    if (timing == 0) {
        u32 model = gabi::load<u32>(0x104B462C);
        u32 user = gabi::load<u32>(model + 0xB8);
        if (user != 0)
            gabi::call(fn, user, node, model);
    }
}

/* 022C89FC */
static BOOL nodeCB_Head(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x022C89FC, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x022C887C);
    return TRUE;
}
VERIFY(0x022C89FC, nodeCB_Head);

/* 022C8A44 */
void daNpc_Pf1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x022C8A44, void, this, i_node, i_model);
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}
VERIFY(0x022C8A44, &daNpc_Pf1_c::_nodeCB_BackBone);

/* 022C8B60 */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x022C8B60, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x022C8A44);
    return TRUE;
}
VERIFY(0x022C8B60, nodeCB_BackBone);

/* 022C8BA8 (unnamed by the matcher) */
int daNpc_Pf1_c::btpResID(int i_btpNum) {
    WWHD_FUNC(0x022C8BA8, int, this, i_btpNum);
    return gabi::load<s32>(0x10020AE4 + i_btpNum * 4);
}
VERIFY(0x022C8BA8, &daNpc_Pf1_c::btpResID);

/* 022C8BBC */
u32 daNpc_Pf1_c::setBtp(s8 i_btpNum, u32 i_bModify) {
    WWHD_FUNC(0x022C8BBC, u32, this, i_btpNum, i_bModify);
    J3DModel* morf_model_p = mpMorf->getModel();
    if (i_btpNum < 0) {
        return false;
    }
    void* a_btp = dComIfG_getObjectIDRes(mArcName, btpResID(i_btpNum));
    if (a_btp == nullptr) /* JUT_ASSERT(465, a_btp != NULL) */
        JUT_ASSERT_fail(STR(0x10020AF0), 0x1D1, STR(0x10020B00));
    mBtpNum = i_btpNum;
    mBtpFrame = i_btpNum == 1 ? 2 : 0;
    mBlinkTimer = 0;
    return mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(morf_model_p), a_btp, TRUE, 0, 1.0f, 0, -1, i_bModify, 0) != 0;
}
VERIFY(0x022C8BBC, &daNpc_Pf1_c::setBtp);

/* 022C8CB8 (tail call) */
u32 daNpc_Pf1_c::init_texPttrnAnm(s8 i_btpNum, u32 i_bModify) {
    WWHD_FUNC(0x022C8CB8, u32, this, i_btpNum, i_bModify);
    return setBtp(i_btpNum, i_bModify);
}
VERIFY(0x022C8CB8, &daNpc_Pf1_c::init_texPttrnAnm);

/* modelData->getJointNodePointer(jnt)->setCallBack(cb) (HD inline: joints are 0x1C-byte records) */
static inline void setJointCallBack(J3DModel* model, s8 jnt, u32 cb) {
    J3DModelData* md = J3DModel_getModelData_l(model);
    u32 idx = (u16)(s16)jnt;
    u32 n = gabi::load<u32>(gabi::ea(md) + 4);
    u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
    if (idx < n)
        joint += idx * 0x1C;
    gabi::store<u32>(joint + 8, cb);
}

/* 022C8CBC */
BOOL daNpc_Pf1_c::bodyCreateHeap() {
    WWHD_FUNC(0x022C8CBC, BOOL, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 8);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2069, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x10020B18), 0x815, STR(0x10020B34));
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr) {
        mDoExt_McaMorf* morf = mpMorf;
        if (morf != nullptr) {
            u32 vt = gabi::load<u32>(gabi::ea(morf));
            gabi::call_ptr(gabi::load<u32>(vt + 0xC), morf, 3); /* delete (virtual deleting destructor) */
        }
        mpMorf = nullptr;
        return FALSE;
    }
    if (!init_texPttrnAnm(0, false)) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_hed_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10020B10) /* "head" */);
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10020B18), 0x82B, STR(0x10020B48));
    m_bbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10020B28) /* "backbone1" */);
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10020B18), 0x82D, STR(0x10020B5C));
    setJointCallBack(mpMorf->getModel(), m_hed_jnt_num, 0x022C89FC /* nodeCB_Head */);
    setJointCallBack(mpMorf->getModel(), m_bbone_jnt_num, 0x022C8B60 /* nodeCB_BackBone */);
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    return TRUE;
}
VERIFY(0x022C8CBC, &daNpc_Pf1_c::bodyCreateHeap);

/* 022C8F7C */
BOOL daNpc_Pf1_c::CreateHeap() {
    WWHD_FUNC(0x022C8F7C, BOOL, this);
    if (!bodyCreateHeap()) {
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 100.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}
VERIFY(0x022C8F7C, &daNpc_Pf1_c::CreateHeap);

/* 022C900C (tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022C900C, BOOL, i_this);
    return static_cast<daNpc_Pf1_c*>(i_this)->CreateHeap();
}
VERIFY(0x022C900C, CheckCreateHeap);

/* 022C9010 */
u8 daNpc_Pf1_c::decideType(int i_type) {
    WWHD_FUNC(0x022C9010, u8, this, i_type);
    if (mType > 0) {
        return true;
    }
    mType = 1;
    mSubType = 0;
    /* strcpy(mArcName, "Pf") (3-byte block copy) */
    for (int i = 0; i < 3; i++)
        gabi::store<u8>(gabi::ea(mArcName) + i, gabi::load<u8>(0x10020B88 + i));
    return mType != -1 && mSubType != -1;
}
VERIFY(0x022C9010, &daNpc_Pf1_c::decideType);

/* 022C9074 (unnamed by the matcher): puts the actor on path point i_idx, facing the next one */
void daNpc_Pf1_c::set_pthPoint(u32 i_idx) {
    WWHD_FUNC(0x022C9074, void, this, i_idx);
    if (mPathRun.mPath.get() == nullptr) {
        return;
    }
    mPathRun.mIdx = i_idx;
    gabi::Local<cXyz> pnt;
    dNpc_PathRun_getPoint(&mPathRun, pnt, i_idx);
    current.pos.copy(*pnt);
    if (dNpc_PathRun_nextIdx(&mPathRun)) {
        gabi::Local<cXyz> next;
        dNpc_PathRun_getPoint(&mPathRun, next, mPathRun.mIdx);
        gabi::Local<cXyz> next_copy;
        next_copy->copy(*next);
        current.angle.y = cLib_targetAngleY(&current.pos, next_copy);
    }
}
VERIFY(0x022C9074, &daNpc_Pf1_c::set_pthPoint);

/* 022C911C */
BOOL daNpc_Pf1_c::set_action(ProcFunc_l* i_newProcFunc, void* i_argsP) {
    WWHD_FUNC(0x022C911C, BOOL, this, i_newProcFunc, i_argsP);
    ProcFunc_l* cur = &mCurrProcFunc;
    s16 newI = i_newProcFunc->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_newProcFunc->d;
        newF = i_newProcFunc->f;
        if (!((u16)cur->d != (u16)newD || cur->f != newF))
            return TRUE;
    } else {
        newF = i_newProcFunc->f;
        newD = i_newProcFunc->d;
        if (cur->i == 0)
            goto set;
    }
    mActState = 9;
    pmf_call(this, cur, i_argsP);
set:
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mActState = 0;
    pmf_call(this, cur, i_argsP);
    return TRUE;
}
VERIFY(0x022C911C, &daNpc_Pf1_c::set_action);

/* 022C9248 */
bool daNpc_Pf1_c::init_PF1_0() {
    WWHD_FUNC(0x022C9248, bool, this);
    if (dComIfGs_isEventBit(0x2D01)) {
        return false;
    }
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, PMF_wait_action1);
    set_action(pmf, nullptr);
    return true;
}
VERIFY(0x022C9248, &daNpc_Pf1_c::init_PF1_0);

/* 022C92CC */
void daNpc_Pf1_c::play_btp_anm() {
    WWHD_FUNC(0x022C92CC, void, this);
    u8 frame_max = (u8)J3DAnm_getFrameMax(gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10)));
    if (mBtpNum == 1) {
        return;
    }
    if (mBtpNum == 0 && cLib_calcTimer(&mBlinkTimer) != 0) {
        return;
    }
    u8 frame = (u8)(mBtpFrame + 1);
    mBtpFrame = frame;
    if (frame < frame_max) {
        return;
    }
    if (mBtpNum == 0) {
        mBlinkTimer = (s16)cLib_getRndValue(60, 90);
        frame_max = 0;
    }
    mBtpFrame = frame_max;
}
VERIFY(0x022C92CC, &daNpc_Pf1_c::play_btp_anm);

/* 022C9378 */
void daNpc_Pf1_c::play_animation() {
    WWHD_FUNC(0x022C9378, void, this);
    u32 snd_id = 0;
    play_btp_anm();
    if (mObjAcch.m_flags & 0x20) { /* mObjAcch.ChkGroundHit() */
        snd_id = dBgS_GetMtrlSndId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    }
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    mbMorfAnimStopped = (s8)mpMorf->play(&eyePos, snd_id, (s8)reverb);
    if (mpMorf->getFrame() < mPrevMorfFrame) {
        mbMorfAnimStopped = true;
    }
    mPrevMorfFrame = mpMorf->getFrame();
}
VERIFY(0x022C9378, &daNpc_Pf1_c::play_animation);

/* 022C941C */
void daNpc_Pf1_c::setAttention(u32 i_setEyePos) {
    WWHD_FUNC(0x022C941C, void, this, i_setEyePos);
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    f32 y = mHeadPos.y + l_HIO().mPrm.mAttPosOffsetY;
    attPos->x = mHeadPos.x;
    attPos->y = y;
    attPos->z = mHeadPos.z;
    if (!mbSetEyePos && !i_setEyePos) {
        return;
    }
    eyePos.z = mEyePos.z;
    eyePos.y = mEyePos.y;
    eyePos.x = mEyePos.x;
}
VERIFY(0x022C941C, &daNpc_Pf1_c::setAttention);

/* 022C9470 */
void daNpc_Pf1_c::setMtx(u32 i_setEyePos) {
    WWHD_FUNC(0x022C9470, void, this, i_setEyePos);
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mAngle.x, mAngle.y, mAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    setAttention(i_setEyePos);
}
VERIFY(0x022C9470, &daNpc_Pf1_c::setMtx);

/* 022C9578 */
bool daNpc_Pf1_c::createInit() {
    WWHD_FUNC(0x022C9578, bool, this);
    /* l_evn_tbl (.data 0x101C4D00) */
    const char* name = gabi::at<const char>(gabi::load<u32>(0x101C4D00));
    mEventIDTbl[0] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    mEventCut.setActorInfo2(STR(0x10020B90) /* "Pf1" */, (fopNpc_npc_c*)(void*)this);
    u8 weight = 0xFF;
    u32 path_id = (mParameters >> 16) & 0xFF;
    if (path_id != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, path_id, fopAcM_GetRoomNo(this), TRUE);
        if (mPathRun.mPath.get() == nullptr) {
            return false;
        }
        actor_status &= ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
        weight = 0xD9;
        set_pthPoint(0);
    }
    if (mPathRun.mPath.get() == nullptr) {
        return false;
    }
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xA9); /* attention_info.distances[SPEAK] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags */
    mBckNum = 8;
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xA9); /* attention_info.distances[TALK] */
    gravity = -4.5f;
    bool init_result;
    switch ((u32)(s32)mSubType) {
    case 0:
        init_result = init_PF1_0();
        break;
    default:
        return false;
    }
    if (!init_result) {
        return false;
    }
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    mStts.Init(weight, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    play_animation();
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}
VERIFY(0x022C9578, &daNpc_Pf1_c::createInit);

/* 022C9748 */
cPhs_State daNpc_Pf1_c::_create() {
    WWHD_FUNC(0x022C9748, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Pf1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this);    /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = PF1_VTBL;
            gabi::call(0x025E7820, mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, mArcName);
    mbResLoaded = state == cPhs_COMPLEATE_e;
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    /* a_heap_size_tbl (.data 0x101C4D24) */
    if (!fopAcM_entrySolidHeap(this, 0x022C900C /* CheckCreateHeap */, gabi::load<u32>(0x101C4D24 + mType * 4))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -100.0f, -20.0f, -80.0f, 100.0f, 180.0f, 140.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x022C9748, &daNpc_Pf1_c::_create);

/* 022C989C */
static cPhs_State daNpc_Pf1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022C989C, cPhs_State, i_this);
    return ((daNpc_Pf1_c*)i_this)->_create();
}
VERIFY(0x022C989C, daNpc_Pf1_Create);

/* 022C98A0 (unnamed by the matcher) */
BOOL daNpc_Pf1_c::_delete() {
    WWHD_FUNC(0x022C98A0, BOOL, this);
    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x022C98A0, &daNpc_Pf1_c::_delete);

/* 022C98F4 */
static BOOL daNpc_Pf1_Delete(daNpc_Pf1_c* i_this) {
    WWHD_FUNC(0x022C98F4, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x022C98F4, daNpc_Pf1_Delete);

/* 022C98F8 */
void daNpc_Pf1_c::checkOrder() {
    WWHD_FUNC(0x022C98F8, void, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIDTbl[mEventIndex]) && mEvtState >= 3) {
            mEvtState = 0;
            mMesgAnimeTag = 0xFF;
            mAnmAtr = 0xFF;
        }
    } else if (command == 1 /* checkCommandTalk() */) {
        if (mEvtState == 1 || mEvtState == 2) {
            mEvtState = 0;
            mbTalk = true;
        }
    }
}
VERIFY(0x022C98F8, &daNpc_Pf1_c::checkOrder);

/* 022C99D0 */
u8 daNpc_Pf1_c::demo() {
    WWHD_FUNC(0x022C99D0, u8, this);
    u8 id = demoActorID;
    if (id == 0) {
        if (mbDemo) {
            mbDemo = false;
            return 0;
        }
        return mbDemo;
    }
    if (!mbDemo) {
        m_jnt.mAngles[0][1] = 0; /* setHead_y(0) */
        m_jnt.mAngles[1][0] = 0; /* setBackBone_x(0) */
        mbDemo = true;
        mbNoShapeAngle = false;
        m_jnt.mAngles[0][0] = 0; /* setHead_x(0) */
        id = demoActorID;
        m_jnt.mAngles[1][1] = 0; /* setBackBone_y(0) */
    }
    /* dComIfGp_demo_getActor(demoActorID) (HD inline with a range check) */
    void* demo_actor_p = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x10020ACC), 0x23A, STR(0x10020ABC));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor_p = gabi::call<void*>(0x02526E70, obj, id);
    }
    u32 btp = gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10);
    if (btp != 0) {
        u8 frame_max = (u8)J3DAnm_getFrameMax(gabi::at<u8>(btp));
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame < frame_max ? frame : frame_max;
    }
    if (demo_actor_p) {
        void* demo_btp_p = gabi::call<void*>(0x02527828, demo_actor_p, mArcName); /* getP_BtpData */
        if (demo_btp_p) {
            mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(mpMorf->getModel()), demo_btp_p, TRUE, 0, 1.0f, 0, -1, 1, 0);
            mBtpFrame = 0;
            mBtpNum = 2;
        }
    }
    gabi::call(0x02527028, this, 0x6A, mpMorf.get(), mArcName, 0, 0, 0, 0); /* dDemo_setDemoData */
    return mbDemo;
}
VERIFY(0x022C99D0, &daNpc_Pf1_c::demo);

/* 022C9B8C */
s32 daNpc_Pf1_c::isEventEntry() {
    WWHD_FUNC(0x022C9B8C, s32, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x022C9B8C, &daNpc_Pf1_c::isEventEntry);

/* 022C9BCC */
void daNpc_Pf1_c::endEvent() {
    WWHD_FUNC(0x022C9BCC, void, this);
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
    mMesgAnimeTag = 0xFF;
}
VERIFY(0x022C9BCC, &daNpc_Pf1_c::endEvent);

/* 022C9C10 (unnamed by the matcher): no cut of this staff does anything; every cut ends at once */
void daNpc_Pf1_c::privateCut(int i_staffIdx) {
    WWHD_FUNC(0x022C9C10, void, this, i_staffIdx);
    /* a_cut_tbl (.data 0x101C4D2C), one entry */
    if (i_staffIdx == -1) {
        return;
    }
    s8 idx = dComIfGp_evmng_getMyActIdx(i_staffIdx, 0x101C4D2C, 1, TRUE, 0);
    mActionIndex = idx;
    if (idx != -1) {
        dComIfGp_evmng_getIsAddvance(i_staffIdx);
    }
    dComIfGp_evmng_cutEnd(i_staffIdx);
}
VERIFY(0x022C9C10, &daNpc_Pf1_c::privateCut);

/* 022C9CA8 */
void daNpc_Pf1_c::event_proc(int i_staffIdx) {
    WWHD_FUNC(0x022C9CA8, void, this, i_staffIdx);
    if (dComIfGp_evmng_endCheck(mEventIDTbl[mEventIndex])) {
        endEvent();
    } else if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
}
VERIFY(0x022C9CA8, &daNpc_Pf1_c::event_proc);

/* 022C9D4C (matcher: cLib_getRndValue<int> [d_a_npc_aj1]; it is searchByID) */
fopAc_ac_c* daNpc_Pf1_c::searchByID(fpc_ProcID i_id, be<s32>* o_notFound) {
    WWHD_FUNC(0x022C9D4C, fopAc_ac_c*, this, i_id, o_notFound);
    gabi::Local<be<u32>> actor;
    *actor = 0;
    *o_notFound = 0;
    if (!fopAcM_SearchByID2(i_id, actor)) {
        *o_notFound = 1;
    }
    return gabi::at<fopAc_ac_c>(*actor);
}
VERIFY(0x022C9D4C, &daNpc_Pf1_c::searchByID);

/* 022C9DA0 (unnamed by the matcher) */
void daNpc_Pf1_c::lookBack() {
    WWHD_FUNC(0x022C9DA0, void, this);
    gabi::Local<cXyz> look_pos;
    look_pos->set(0.0f, 0.0f, 0.0f);
    s16 target_y = current.angle.y;
    mJointHeadY = m_jnt.mAngles[0][1];
    u8 head_only = mbHeadOnly;
    f32 srcX = current.pos.x;
    f32 srcZ = current.pos.z;
    f32 srcY = eyePos.y;
    mActorAngleY = target_y;
    mJointBackboneY = m_jnt.mAngles[1][1];
    cXyz* look_pos_p = nullptr;
    switch ((u32)(s32)mLookMode) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        mLookPos.copy(*eye);
        look_pos->copy(*eye);
        look_pos_p = look_pos;
        break;
    }
    case 2:
        look_pos->copy(mLookPos);
        look_pos_p = look_pos;
        break;
    case 3:
        target_y = mLookAngleY;
        break;
    case 4: {
        gabi::Local<be<s32>> not_found;
        fopAc_ac_c* actor = searchByID(mLookActorId, not_found);
        if (actor != nullptr && *not_found == 0) {
            mLookPos.copy(actor->current.pos);
            f32 x = mLookPos.x;
            f32 y = actor->eyePos.y;
            look_pos->x = x;
            look_pos->y = y;
            look_pos->z = mLookPos.z;
            mLookPos.y = y;
            look_pos_p = look_pos;
        }
        break;
    }
    }
    gabi::Local<cXyz> cur_pos; /* passed by value: a copy */
    cur_pos->x = srcX;
    cur_pos->y = srcY;
    cur_pos->z = srcZ;
    dNpc_JntCtrl_lookAtTarget_2(&m_jnt, &current.angle.y, look_pos_p, cur_pos, target_y, l_HIO().mPrm.mLookVelMax, head_only);
}
VERIFY(0x022C9DA0, &daNpc_Pf1_c::lookBack);

/* 022CA034 */
void daNpc_Pf1_c::eventOrder() {
    WWHD_FUNC(0x022CA034, void, this);
    s8 state = mEvtState;
    if (state == 1 || state == 2) {
        eventInfo_onCondition(this, 1); /* dEvtCnd_CANTALK_e */
        if (mEvtState == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (state >= 3) {
        s16 idx = (s16)(state - 3);
        mEventIndex = idx;
        fopAcM_orderOtherEventId(this, mEventIDTbl[idx], 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x022CA034, &daNpc_Pf1_c::eventOrder);

/* 022CA0A4 */
BOOL daNpc_Pf1_c::_execute() {
    WWHD_FUNC(0x022CA0A4, BOOL, this);
    if (!mbHomeSet) {
        mHomePos.copy(current.pos);
        mHomeAngle.x = current.angle.x;
        mHomeAngle.y = current.angle.y;
        mHomeAngle.z = current.angle.z;
        mbHomeSet = true;
    }
    daNpc_Pf1_HIO_c::hio_prm_c& prm = l_HIO().mPrm;
    m_jnt.setParam(prm.mMaxBackBoneX, prm.mMaxBackBoneY, prm.mMinBackBoneX, prm.mMinBackBoneY, prm.mMaxHeadX,
                   prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (m918 && demoActorID == 0) {
        return TRUE;
    }
    checkOrder();
    if (!demo()) {
        s32 staff_id;
        if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !eventInfo.checkCommandTalk() */ &&
            (staff_id = isEventEntry()) >= 0) {
            event_proc(staff_id);
        } else {
            pmf_call(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        field_0x7d6 = 0;
        lookBack();
        fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
        if (mObjAcch.GetGroundH() != -1000000000.0f) {
            void* pla = dBgS_GetTriPla(dComIfG_Bgsp(), daNpc_gndPoly(this));
            if (pla != nullptr) {
                mGndNormal.copy(*gabi::at<cXyz>(gabi::ea(pla))); /* ->GetNP() */
            }
        }
        play_animation();
    } else {
        m918 = false;
    }
    eventOrder();
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    if (!mbNoShapeAngle) {
        shape_angle.x = current.angle.x;
        shape_angle.y = current.angle.y;
        shape_angle.z = current.angle.z;
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    setMtx(false);
    if (!mbDemo) {
        setCollision(100.0f, 200.0f);
    }
    return TRUE;
}
VERIFY(0x022CA0A4, &daNpc_Pf1_c::_execute);

/* 022CA394 */
static BOOL daNpc_Pf1_Execute(daNpc_Pf1_c* i_this) {
    WWHD_FUNC(0x022CA394, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x022CA394, daNpc_Pf1_Execute);

/* static initialisation of a 4-byte function-local constant on first use */
static inline void local_static_init(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(dst), gabi::at<u8>(src), 4); /* 028FEAC0 memcpy */
    }
}

/* 022CA398 */
BOOL daNpc_Pf1_c::_draw() {
    WWHD_FUNC(0x022CA398, BOOL, this);
    J3DModel* morf_model_p = mpMorf->getModel();
    J3DModelData* morf_model_info_p = J3DModel_getModelData_l(morf_model_p);
    if (m918 || mbNoDraw) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, morf_model_info_p, mBtpFrame);
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(morf_model_info_p) + 0x38, 0); /* mBtpAnm.remove() */
    /* HD: no shadowDraw() */
    dSnap_RegistFig(0x5B /* DSNAP_TYPE_NPC_PF1 */, this, 1.0f, 1.0f, 1.0f);
    /* debug leftovers: function-local static colors initialised on first use */
    if (l_HIO().mPrm.mDebugDraw) {
        local_static_init(0x101FDA48, 0x101FEBEC, 0x100209F8);
        local_static_init(0x101FDA44, 0x101FEBE8, 0x100209FC);
        local_static_init(0x101FDA50, 0x101FEBF4, 0x10020A00);
        local_static_init(0x101FDAC0, 0x101FEBF8, 0x10020A04);
    }
    return TRUE;
}
VERIFY(0x022CA398, &daNpc_Pf1_c::_draw);

/* 022CA52C */
static BOOL daNpc_Pf1_Draw(daNpc_Pf1_c* i_this) {
    WWHD_FUNC(0x022CA52C, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x022CA52C, daNpc_Pf1_Draw);

/* 022CA530 */
static BOOL daNpc_Pf1_IsDelete(daNpc_Pf1_c*) {
    WWHD_FUNC(0x022CA530, BOOL, (daNpc_Pf1_c*)nullptr);
    return TRUE;
}
VERIFY(0x022CA530, daNpc_Pf1_IsDelete);

/* 022CA538 (unnamed by the matcher) */
int daNpc_Pf1_c::bckResID(int i_bckNum) {
    WWHD_FUNC(0x022CA538, int, this, i_bckNum);
    return gabi::load<s32>(0x10020BB8 + i_bckNum * 4);
}
VERIFY(0x022CA538, &daNpc_Pf1_c::bckResID);

/* 022CA54C */
void daNpc_Pf1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    WWHD_FUNC(0x022CA54C, void, this, i_anmPrmP);
    s8 bck = i_anmPrmP->bckNum;
    if (bck < 0 || mBckNum == bck) {
        return;
    }
    int resID = bckResID(bck);
    s32 loopMode = i_anmPrmP->loopMode;
    f32 morf = i_anmPrmP->morf;
    f32 speed = i_anmPrmP->speed;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, mArcName);
    mbMorfAnimStopped = false;
    mBckNum = i_anmPrmP->bckNum;
    mPrevMorfFrame = 0.0f;
    mAtrLoopCnt = 0;
}
VERIFY(0x022CA54C, &daNpc_Pf1_c::setAnm_anm);

/* 022CA5E4 */
void daNpc_Pf1_c::setAnm_NUM(int i_anmNum, int i_setBtp) {
    WWHD_FUNC(0x022CA5E4, void, this, i_anmNum, i_setBtp);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101C4D30); /* [8] */
    if (i_setBtp) {
        init_texPttrnAnm(a_anm_prm_tbl[i_anmNum].btpNum, true);
    }
    setAnm_anm(&a_anm_prm_tbl[i_anmNum]);
}
VERIFY(0x022CA5E4, &daNpc_Pf1_c::setAnm_NUM);

/* 022CA654 */
void daNpc_Pf1_c::setAnm() {
    WWHD_FUNC(0x022CA654, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101C4DB0); /* [8] */
    init_texPttrnAnm(a_anm_prm_tbl[mStt].btpNum, true);
    setAnm_anm(&a_anm_prm_tbl[mStt]);
}
VERIFY(0x022CA654, &daNpc_Pf1_c::setAnm);

/* 022CA6C4 */
void daNpc_Pf1_c::setAnm_ATR() {
    WWHD_FUNC(0x022CA6C4, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101C4E30); /* [6] */
    init_texPttrnAnm(a_anm_prm_tbl[mAnmAtr].btpNum, true);
    setAnm_anm(&a_anm_prm_tbl[mAnmAtr]);
}
VERIFY(0x022CA6C4, &daNpc_Pf1_c::setAnm_ATR);

/* 022CA72C */
void daNpc_Pf1_c::chngAnmAtr(u8 i_atr) {
    WWHD_FUNC(0x022CA72C, void, this, i_atr);
    if (i_atr == mAnmAtr || i_atr >= 6) {
        return;
    }
    mAnmAtr = i_atr;
    setAnm_ATR();
}
VERIFY(0x022CA72C, &daNpc_Pf1_c::chngAnmAtr);

/* 022CA748 */
void daNpc_Pf1_c::ctrlAnmAtr() {
    WWHD_FUNC(0x022CA748, void, this);
    if (mAnmAtr != 4 || !mbMorfAnimStopped) {
        return;
    }
    s8 cnt = (s8)(mAtrLoopCnt + 1);
    mAtrLoopCnt = cnt;
    if (cnt > 2) {
        mAnmAtr = 6;
        setAnm_NUM(0, 1);
    }
}
VERIFY(0x022CA748, &daNpc_Pf1_c::ctrlAnmAtr);

/* 022CA78C */
void daNpc_Pf1_c::anmAtr(u16 i_msgStatus) {
    WWHD_FUNC(0x022CA78C, void, this, i_msgStatus);
    switch (i_msgStatus) {
    case 6: {
        if (mAtrSet == 0) {
            chngAnmAtr(gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925) /* dComIfGp_getMesgAnimeAttrInfo() */);
            mAtrSet = mAtrSet + 1;
        }
        u8 tag = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgAnimeTagInfo() */
        if (tag != 0xFF && tag != mMesgAnimeTag) {
            gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF);
            mMesgAnimeTag = tag;
            /* chngAnmTag(): empty */
        }
        break;
    }
    case 14:
        mAtrSet = 0;
        break;
    }
    ctrlAnmAtr();
}
VERIFY(0x022CA78C, &daNpc_Pf1_c::anmAtr);

/* 022CA84C */
u16 daNpc_Pf1_c::next_msgStatus(be<u32>* o_msgNoP) {
    WWHD_FUNC(0x022CA84C, u16, this, o_msgNoP);
    u16 msg_status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    u32 mng = l_msgMng();
    switch (*o_msgNoP) {
    case 0x1B59: {
        u32 sel = gabi::load<u32>(mng + 0x948); /* the selected answer */
        if (sel < 1) {
            *o_msgNoP = 0x1B5A;
        } else if (sel == 1) {
            *o_msgNoP = 0x1B5D;
        }
        mbTalked = true;
        break;
    }
    case 0x1B5A:
        *o_msgNoP = 0x1B61;
        break;
    case 0x1B5B:
        *o_msgNoP = 0x1B62;
        break;
    case 0x1B5C:
        *o_msgNoP = 0x1B5F;
        break;
    case 0x1B5E:
        *o_msgNoP = 0x1B60;
        break;
    case 0x1B60:
        m914 = true;
        msg_status = 0x10;
        break;
    case 0x1B61:
        *o_msgNoP = 0x1B5B;
        break;
    case 0x1B62:
        dComIfGs_onEventBit(0x0B04);
        msg_status = 0x10;
        break;
    default:
        msg_status = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msg_status;
}
VERIFY(0x022CA84C, &daNpc_Pf1_c::next_msgStatus);

/* 022CA970 */
u32 daNpc_Pf1_c::getMsg_PF1_0() {
    WWHD_FUNC(0x022CA970, u32, this);
    if (dComIfGs_isEventBit(0x0B04)) {
        return 0x1B5C;
    }
    return mbTalked ? 0x1B5E : 0x1B59;
}
VERIFY(0x022CA970, &daNpc_Pf1_c::getMsg_PF1_0);

/* 022CA9E0 */
u32 daNpc_Pf1_c::getMsg() {
    WWHD_FUNC(0x022CA9E0, u32, this);
    u32 msg = 0;
    if (mSubType == 0) {
        msg = getMsg_PF1_0();
    }
    return msg;
}
VERIFY(0x022CA9E0, &daNpc_Pf1_c::getMsg);

/* 022CAA18 */
BOOL daNpc_Pf1_c::chk_talk() {
    WWHD_FUNC(0x022CAA18, BOOL, this);
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mItemNo = dComIfGp_event_getPreItemNo();
            return true;
        }
        return false;
    }
    mItemNo = 0xFF;
    return true;
}
VERIFY(0x022CAA18, &daNpc_Pf1_c::chk_talk);

/* 022CAAB0: true when the head, backbone or body turned this frame (aj1's test, inverted) */
u8 daNpc_Pf1_c::chk_parts_notMov() {
    WWHD_FUNC(0x022CAAB0, u8, this);
    return !(mJointHeadY == m_jnt.mAngles[0][1] && mJointBackboneY == m_jnt.mAngles[1][1] && mActorAngleY == current.angle.y);
}
VERIFY(0x022CAAB0, &daNpc_Pf1_c::chk_parts_notMov);

/* 022CAAF0 */
u8 daNpc_Pf1_c::chkAttention() {
    WWHD_FUNC(0x022CAAF0, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x022CAAF0, &daNpc_Pf1_c::chkAttention);

/* 022CAB78 */
void daNpc_Pf1_c::setStt(s8 i_stt) {
    WWHD_FUNC(0x022CAB78, void, this, i_stt);
    s8 prev = mStt;
    mStt = i_stt;
    switch ((u32)(s32)i_stt) {
    case 1:
    case 4:
    case 6:
    case 7:
        mEvtState = 0;
        speedF = 0.0f;
        break;
    case 2:
        mEvtState = 0;
        speedF = 0.0f;
        mAtrSet = 0;
        mMesgAnimeTag = 0xFF;
        mPrevStt = prev;
        mAnmAtr = 0xFF;
        break;
    case 3:
        mEvtState = 0;
        mTimer = (s16)cLib_getRndValue(180, 90);
        break;
    case 5:
        mTimer = 0;
        mEvtState = 0;
        mAttkTimer = 30;
        break;
    }
    setAnm();
}
VERIFY(0x022CAB78, &daNpc_Pf1_c::setStt);

/* 022CAC64: throws a "tama" (actor 0x1D0) from the eyes towards the player */
void daNpc_Pf1_c::createTama(f32 i_param) {
    WWHD_FUNC(0x022CAC64, void, this, i_param);
    gabi::Local<csXyz> angle;
    csXyz_ct(angle, 0, 0, 0);
    gabi::Local<cXyz> start;
    start->x = eyePos.x;
    start->y = eyePos.y + 15.0f;
    start->z = eyePos.z;
    gabi::Local<cXyz> eye;
    dNpc_playerEyePos_l(eye, -20.0f);
    gabi::Local<cXyz> target;
    target->copy(*eye);
    cXyz_mi(target, eye, &eyePos);
    gabi::Local<cXyz> xz;
    xz->x = eye->x;
    xz->y = 0.0f;
    xz->z = eye->z;
    std_sqrtf(PSVECSquareMag(xz)); /* unused */
    angle->y = cLib_targetAngleY(start, target);
    angle->x = cLib_targetAngleX(start, target);
    fopAc_ac_c* tama = fopAcM_fastCreate(0x1D0, 0, &eyePos, fopAcM_GetRoomNo(this), angle, nullptr, -1, 0, nullptr);
    if (tama != nullptr) {
        gabi::store<f32>(gabi::ea(tama) + 0x71C, i_param);
        gabi::store<u32>(gabi::ea(tama) + 0x3AC, gabi::load<u32>(gabi::ea(this) + 4)); /* fopAcM_GetID(this) */
        tama->speedF = 50.0f;
    }
}
VERIFY(0x022CAC64, &daNpc_Pf1_c::createTama);

/* 022CADB8 (cXyz by value: a pointer to the caller's copy) */
BOOL daNpc_Pf1_c::chk_areaIN(f32 i_dist, cXyz* i_pos) {
    WWHD_FUNC(0x022CADB8, BOOL, this, i_dist, i_pos);
    gabi::Local<cXyz> diff;
    cXyz_mi(&dComIfGp_getLinkPlayer()->current.pos, diff, i_pos);
    gabi::Local<cXyz> xz;
    f32 dx = diff->x, dz = diff->z;
    xz->y = 0.0f;
    xz->x = dx;
    xz->z = dz;
    u8 in = std_sqrtf(PSVECSquareMag(xz)) < i_dist;
    if (in && g_Counter0() % 3 == 0) {
        createTama(i_dist);
    }
    return in;
}
VERIFY(0x022CADB8, &daNpc_Pf1_c::chk_areaIN);

/* 022CAE8C */
BOOL daNpc_Pf1_c::endEvent_check() {
    WWHD_FUNC(0x022CAE8C, BOOL, this);
    if (mbTalked) {
        return !dComIfGs_isEventBit(0x0B04);
    }
    return false;
}
VERIFY(0x022CAE8C, &daNpc_Pf1_c::endEvent_check);

/* 022CAEE4 */
BOOL daNpc_Pf1_c::startEvent_check() {
    WWHD_FUNC(0x022CAEE4, BOOL, this);
    gabi::Local<cXyz> pos;
    pos->x = mHomePos.x;
    pos->y = mHomePos.y + 100.0f;
    pos->z = mHomePos.z;
    if (chk_areaIN(l_HIO().mPrm.mStartDist, pos)) {
        if (std_sqrtf(PSVECSquareDistance(&current.pos, &dComIfGp_getLinkPlayer()->current.pos)) < 210.0f || field_0x7d6 != 0) {
            return true;
        }
    }
    return false;
}
VERIFY(0x022CAEE4, &daNpc_Pf1_c::startEvent_check);

/* 022CAF98 */
BOOL daNpc_Pf1_c::chk_attn() {
    WWHD_FUNC(0x022CAF98, BOOL, this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, &dComIfGp_getLinkPlayer()->current.pos);
    gabi::Local<cXyz> xz;
    f32 dx = diff->x, dz = diff->z;
    xz->y = 0.0f;
    xz->x = dx;
    xz->z = dz;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    f32 dy = current.pos.y - player->current.pos.y;
    s16 angle = (s16)(cLib_targetAngleY(&current.pos, &dComIfGp_getLinkPlayer()->current.pos) - current.angle.y);
    f32 max_angle = mLookMode == 1 ? 90.0f : 60.0f;
    u8 ret = false;
    if (dist < 200.0f) {
        if ((f32)abs((int)angle) / 182.04445f < max_angle && std::fabs(dy) < 300.0f) {
            ret = true;
        }
    }
    return ret;
}
VERIFY(0x022CAF98, &daNpc_Pf1_c::chk_attn);

/* 022CB1F4 (cXyz by value: a pointer to the caller's copy) */
void daNpc_Pf1_c::setBikon(cXyz* i_offset) {
    WWHD_FUNC(0x022CB1F4, void, this, i_offset);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    gabi::Local<cXyz> pos;
    PSMTXMultVec(mDoMtx_stack_c::get(), i_offset, pos);
    dPa_control_set(dComIfGp_getParticle(), 0, 0x8152, pos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
}
VERIFY(0x022CB1F4, &daNpc_Pf1_c::setBikon);

/* 022CB2A4 */
BOOL daNpc_Pf1_c::regret() {
    WWHD_FUNC(0x022CB2A4, BOOL, this);
    if (mbTalk) {
        if (chk_talk()) {
            setStt(2);
        }
        return TRUE;
    }
    if (!endEvent_check()) {
        mEvtState = 0;
        if (startEvent_check()) {
            setStt(5);
        }
    } else {
        mEvtState = 2;
    }
    mLookMode = 0;
    mbHeadOnly = true;
    if (mbMorfAnimStopped) {
        setStt(3);
    }
    return TRUE;
}
VERIFY(0x022CB2A4, &daNpc_Pf1_c::regret);

/* turn to the player and run (attk_1) */
static inline void pf1_chase(daNpc_Pf1_c* i_this) {
    daNpc_Pf1_HIO_c::hio_prm_c& prm = l_HIO().mPrm;
    s16 ang = cLib_targetAngleY(&i_this->current.pos, &dComIfGp_getLinkPlayer()->current.pos);
    cLib_addCalcAngleS(&i_this->current.angle.y, ang, prm.mTurnScale, prm.mTurnMaxStep, 0x80);
    cLib_chaseF(&i_this->speedF, prm.mAttkSpeed, prm.mAttkAccel);
    f32 rate = i_this->speedF * prm.mAttkAnmRate;
    i_this->mpMorf->setPlaySpeed(rate - 0.5f >= 0.0f ? rate : 0.5f);
}

/* 022CB378 */
BOOL daNpc_Pf1_c::attk_1() {
    WWHD_FUNC(0x022CB378, BOOL, this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, &dComIfGp_getLinkPlayer()->current.pos);
    gabi::Local<cXyz> xz;
    f32 dx = diff->x, dz = diff->z;
    xz->y = 0.0f;
    xz->x = dx;
    xz->z = dz;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    if (mbTalk) {
        if (chk_talk()) {
            setStt(6);
            setStt(2);
        }
        return TRUE;
    }
    mLookMode = 0;
    mbHeadOnly = true;
    if (mBckNum == 0) {
        if (mTimer == 0) {
            s16 ang = cLib_targetAngleY(&current.pos, &dComIfGp_getLinkPlayer()->current.pos);
            cLib_addCalcAngleS(&current.angle.y, ang, 4, 0x1000, 0x80);
            if (abs((s16)(current.angle.y - ang)) < 0x800) {
                mDoAud_seStart2(0x58BD, &current.pos);
                gabi::Local<cXyz> offset;
                offset->x = 0.0f;
                offset->y = -40.0f;
                offset->z = 60.0f;
                setBikon(offset);
                mTimer = 20;
            }
        } else if (cLib_calcTimer(&mTimer) == 0) {
            setAnm_NUM(2, 1);
        }
        return TRUE;
    }
    if (mEvtState != 1 && dist < 210.0f) {
        mEvtState = 1;
    }
    if (cLib_calcTimer(&mAttkTimer) == 0) {
        gabi::Local<cXyz> home;
        home->x = mHomePos.x;
        home->y = mHomePos.y;
        home->z = mHomePos.z;
        if (!chk_areaIN(l_HIO().mPrm.mAreaDist, home)) {
            setStt(6);
            return TRUE;
        }
    }
    pf1_chase(this);
    return TRUE;
}
VERIFY(0x022CB378, &daNpc_Pf1_c::attk_1);

/* 022CB66C */
BOOL daNpc_Pf1_c::walk_1() {
    WWHD_FUNC(0x022CB66C, BOOL, this);
    dPath* path = mPathRun.mPath;
    if (path == nullptr || !(gabi::load<u8>(gabi::ea(path) + 5) & 1)) {
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    if (dNpc_PathRun_chkPointPass(&mPathRun, pos, mPathRun.mbDir != 0)) {
        dNpc_PathRun_nextIdxAuto(&mPathRun);
    }
    gabi::Local<cXyz> pnt;
    dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
    gabi::Local<cXyz> pnt_copy;
    pnt_copy->copy(*pnt);
    daNpc_Pf1_HIO_c::hio_prm_c& prm = l_HIO().mPrm;
    cLib_addCalcAngleS(&current.angle.y, cLib_targetAngleY(&current.pos, pnt_copy), prm.mTurnScale, prm.mTurnMaxStep, 0x80);
    f32 target = prm.mWalkSpeed;
    if (cLib_calcTimer(&mTimer) == 0 || mbTalk) {
        target = 0.0f;
    }
    if (!endEvent_check() && startEvent_check()) {
        speedF = 0.0f;
        mTimer = 0;
        setStt(5);
        mLookMode = 0;
        mbHeadOnly = true;
        return TRUE;
    }
    cLib_chaseF(&speedF, target, prm.mWalkAccel);
    f32 rate = speedF * prm.mWalkAnmRate;
    s32 itarget = gabi::ftoi(target + 0.5f);
    mpMorf->setPlaySpeed(rate - 0.5f >= 0.0f ? rate : 0.5f);
    if (itarget == 0 && gabi::ftoi(speedF) == 0) {
        if (mbTalk) {
            if (chk_talk()) {
                setStt(2);
            }
            return TRUE;
        }
        setStt(4);
        return TRUE;
    }
    mEvtState = 0;
    if (endEvent_check()) {
        mEvtState = 2;
    }
    mLookMode = 0;
    mbHeadOnly = true;
    return TRUE;
}
VERIFY(0x022CB66C, &daNpc_Pf1_c::walk_1);

/* 022CB8C8 */
BOOL daNpc_Pf1_c::wait_2() {
    WWHD_FUNC(0x022CB8C8, BOOL, this);
    if (mbTalk) {
        if (chk_talk()) {
            setStt(2);
        }
        return TRUE;
    }
    bool stay = false;
    if (dComIfGs_isEventBit(0x0B04)) {
        gabi::Local<cXyz> home;
        home->x = mHomePos.x;
        home->y = mHomePos.y;
        home->z = mHomePos.z;
        stay = chk_areaIN(l_HIO().mPrm.mAreaDist, home) != 0;
    }
    if (!stay) {
        dNpc_PathRun_setNearPathIndx(&mPathRun, &current.pos, 0.0f);
        setStt(3);
        return TRUE;
    }
    mEvtState = 2;
    mbHeadOnly = true;
    mLookMode = chk_attn() ? 1 : 0;
    return TRUE;
}
VERIFY(0x022CB8C8, &daNpc_Pf1_c::wait_2);

/* 022CB9D0 */
BOOL daNpc_Pf1_c::wait_3() {
    WWHD_FUNC(0x022CB9D0, BOOL, this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, &dComIfGp_getLinkPlayer()->current.pos);
    gabi::Local<cXyz> xz;
    xz->x = diff->x;
    xz->y = 0.0f;
    xz->z = diff->z;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    if (mbTalk) {
        if (chk_talk()) {
            setStt(2);
            mbFar = false;
        }
        return TRUE;
    }
    mbFar = dist > 300.0f;
    if (mbFar) {
        setStt(3);
        mLookMode = 0;
        mbHeadOnly = true;
        return TRUE;
    }
    mEvtState = 2;
    mbHeadOnly = true;
    mLookMode = chk_attn() ? 1 : 0;
    return TRUE;
}
VERIFY(0x022CB9D0, &daNpc_Pf1_c::wait_3);

/* 022CBAE0 */
BOOL daNpc_Pf1_c::talk_1() {
    WWHD_FUNC(0x022CBAE0, BOOL, this);
    BOOL res = chk_parts_notMov();
    s16 ang = cLib_targetAngleY(&current.pos, &dComIfGp_getLinkPlayer()->current.pos);
    cLib_addCalcAngleS(&current.angle.y, ang, 4, l_HIO().mPrm.mLookVelMax, 0x80);
    u16 status = talk(1);
    if (!mbHasMsg) {
        return TRUE;
    }
    if (status == 10 && mbMorfAnimStopped && mCurrMsgNo == 0x1B60) {
        gabi::store<u8>(l_msgMng() + 0x921, 1);
    }
    /* HD: the message status comes from the message manager */
    if (fopMsgM_getStatus() == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
        mItemNo = 0xFF;
        mbTalk = false;
        setStt(mPrevStt);
        endEvent();
    }
    return res;
}
VERIFY(0x022CBAE0, &daNpc_Pf1_c::talk_1);

/* 022CBBE8 */
BOOL daNpc_Pf1_c::wait_action1(void*) {
    WWHD_FUNC(0x022CBBE8, BOOL, this, (void*)nullptr);
    switch ((u32)(s32)mActState) {
    case 0:
        setStt(3);
        mActState = mActState + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch ((u32)(s32)mStt) {
        case 1:
            mbSetEyePos = TRUE; /* wait_1(): returns TRUE */
            break;
        case 2:
            mbSetEyePos = talk_1();
            break;
        case 3:
            mbSetEyePos = walk_1();
            break;
        case 4:
            mbSetEyePos = regret();
            break;
        case 5:
            mbSetEyePos = attk_1();
            break;
        case 6:
            mbSetEyePos = wait_2();
            break;
        case 7:
            mbSetEyePos = wait_3();
            break;
        }
        break;
    }
    return TRUE;
}
VERIFY(0x022CBBE8, &daNpc_Pf1_c::wait_action1);

/* 022CBD1C daNpc_Pf1_HIO_c::daNpc_Pf1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Pf1_HIO_c* daNpc_Pf1_HIO_c_ct(daNpc_Pf1_HIO_c* i_this) {
    WWHD_FUNC(0x022CBD1C, daNpc_Pf1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Pf1_HIO_c*)operator_new(0x4C);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10020AAC;
    memcpy_g(&i_this->mPrm, gabi::at<u8>(0x101C4E90), 0x40); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->m04 = -1;
    i_this->m08 = -1;
    return i_this;
}
VERIFY(0x022CBD1C, daNpc_Pf1_HIO_c_ct);

/* 022CBD88: static initialisation of the translation unit */
static void __sinit_d_a_npc_pf1_cpp() {
    WWHD_FUNC(0x022CBD88, void);
    sinit_header_statics(0x10468330, 0x101C4ED0);
    daNpc_Pf1_HIO_c_ct(&l_HIO()); /* static daNpc_Pf1_HIO_c l_HIO */
}
VERIFY(0x022CBD88, __sinit_d_a_npc_pf1_cpp);

/* 022CBE28: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x022CBE28, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x022CBE28, SafeString_dt);

/* 022CBE3C: daNpc_Pf1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Pf1_c_dt(daNpc_Pf1_c* i_this, s32 flags) {
    WWHD_FUNC(0x022CBE3C, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x10020A4C);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x10020A5C);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x022CBE3C, daNpc_Pf1_c_dt);

/* 022CBED8: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x022CBED8, void, (SafeString*)nullptr);
}
VERIFY(0x022CBED8, SafeString_assureTerminationImpl);
