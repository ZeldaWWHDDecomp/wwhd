/**
 * d_a_npc_aj1.cpp (WWHD)
 * NPC - Sturgeon (Outset)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_aj1.cpp) has only "Nonmatching" stubs for this unit: every function here is
 * written from the WWHD code (cking.rpx) with the GameCube names, and verified against it.
 */
#include "d/actor/d_a_npc_aj1.h"

#define SAFESTRING_VTBL 0x10016038 /* this TU's sead::SafeString vtable */

/* ---- local bindings (SHARED-CANDIDATE; the same as in d_a_npc_ls1.cpp) ---- */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGp_getSelectItem(int btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
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
/* 0207A9A0 cLib_calcTimer<unsigned char> */
static inline u8 cLib_calcTimer(be<u8>* t) { return gabi::call<u8>(0x0207A9A0, t); }
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
static inline u32 fopMsgM_getStatus() { return gabi::call<u32>(0x025F795C, gabi::load<u32>(0x101F4B5C)); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16, s16, bool) */
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
/* 028249B0 JPASetRMtxTVecfromMtx(const Mtx, Mtx (rotation), TVec3 (translation)) */
static inline void JPASetRMtxTVecfromMtx(Mtx34* m, u32 r, u32 t) { gabi::call(0x028249B0, m, r, t); }
/* 02529D7C dDetect_c::set_quake (dDetect_c at play+0x5A20) */
static inline void dComIfGp_detect_set_quake(u32 p) { gabi::call(0x02529D7C, dComIfGp_ea() + PLAY_DETECT, p); }
/* 025A5B18 dPa_smokeEcallBack::dPa_smokeEcallBack(u8) */
static inline void dPa_smokeEcallBack_ct(void* p, u8 a) { gabi::call(0x025A5B18, p, a); }

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
enum : u32 { PMF_wait_action1 = 0x10016010, PMF_wait_action2 = 0x10016018 };

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

/* JPABaseEmitter (HD offsets used here) */
static inline void emitter_stop(u32 e) {
    gabi::store<u32>(e + 0x5C, 0xFFFFFFFF);                  /* setMaxFrame(-1)? (life) */
    gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) | 1); /* stopCreateParticle() */
}

/* ---- file statics ---- */
struct daNpc_Aj1_HIO_c {
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
        /* 0x19 */ u8 _19[3];
        /* 0x1C */ be<f32> mFarDist;
        /* 0x20 */ be<s16> mFarAngle;
        /* 0x22 */ u8 _22[2];
        /* 0x24 */ be<f32> mCallDist;
        /* 0x28 */ be<s16> mCallAngle;
        /* 0x2A */ be<s16> mSpprTime;
        /* 0x2C */ be<s16> mLokTime;
        /* 0x2E */ u8 _2E[2];
    };
    /* 0x00 */ be<u32> __vtbl; /* HD: vtable first */
    /* 0x04 */ be<s8> m04;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> m08;
    /* 0x0C */ hio_prm_c mPrm;
};
WWHD_SIZE(daNpc_Aj1_HIO_c, 0x3C);
static daNpc_Aj1_HIO_c& l_HIO() { return *gabi::at<daNpc_Aj1_HIO_c>(0x104659F0); }

/* 021E6F70 */
void daNpc_Aj1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x021E6F70, void, this, i_node, i_model);
    /* static cXyz a_eye_pos_off(24.0f, -16.0f, 0.0f): guard 0x10465A38, object 0x10465A2C */
    cXyz* a_eye_pos_off = gabi::at<cXyz>(0x10465A2C);
    if (gabi::load<u32>(0x10465A38) == 0) {
        gabi::store<u32>(0x10465A38, 1);
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->x = 24.0f;
        a_eye_pos_off->y = -16.0f;
    }
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    mHeadPos.x = stk->m[0][3];
    mHeadPos.y = stk->m[1][3];
    mHeadPos.z = stk->m[2][3];
    mDoMtx_YrotM(stk, -m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[0][0]);
    PSMTXMultVec(stk, a_eye_pos_off, &mEyePos);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}
VERIFY(0x021E6F70, &daNpc_Aj1_c::_nodeCB_Head);

/* node callbacks: if (calcTiming == In && j3dSys.getModel()->getUserArea()) actor->_nodeCB_X(node, model) */
static inline void nodeCB_dispatch(J3DNode* node, int timing, u32 fn) {
    if (timing == 0) {
        u32 model = gabi::load<u32>(0x104B462C);
        u32 user = gabi::load<u32>(model + 0xB8);
        if (user != 0)
            gabi::call(fn, user, node, model);
    }
}

/* 021E70F0 */
static BOOL nodeCB_Head(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x021E70F0, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x021E6F70);
    return TRUE;
}
VERIFY(0x021E70F0, nodeCB_Head);

/* 021E7138 */
void daNpc_Aj1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x021E7138, void, this, i_node, i_model);
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}
VERIFY(0x021E7138, &daNpc_Aj1_c::_nodeCB_BackBone);

/* 021E7254 */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x021E7254, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x021E7138);
    return TRUE;
}
VERIFY(0x021E7254, nodeCB_BackBone);

/* 021E729C (one btp: the index is not used) */
int daNpc_Aj1_c::btpResID(int) {
    WWHD_FUNC(0x021E729C, int, this, 0);
    return gabi::load<s32>(0x100160A8);
}
VERIFY(0x021E729C, &daNpc_Aj1_c::btpResID);

/* 021E72A8 */
u32 daNpc_Aj1_c::init_texPttrnAnm(s8 i_btpNum, u32 i_bModify) {
    WWHD_FUNC(0x021E72A8, u32, this, i_btpNum, i_bModify);
    J3DModel* morf_model_p = mpMorf->getModel();
    if (i_btpNum < 0) {
        return false;
    }
    void* a_btp = dComIfG_getObjectIDRes(STR(0x100160B0) /* "Aj" */, btpResID(i_btpNum));
    if (a_btp == nullptr) /* JUT_ASSERT(534, a_btp != NULL) */
        JUT_ASSERT_fail(STR(0x100160B4), 0x216, STR(0x100160C4));
    mBtpNum = i_btpNum;
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(morf_model_p), a_btp, TRUE, 0, 1.0f, 0, -1, i_bModify, 0) != 0;
}
VERIFY(0x021E72A8, &daNpc_Aj1_c::init_texPttrnAnm);

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

/* 021E7398 */
BOOL daNpc_Aj1_c::bodyCreateHeap() {
    WWHD_FUNC(0x021E7398, BOOL, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x100160E4) /* "Aj" */, 0xA);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2296, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x100160F0), 0x8F8, STR(0x10016100));
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
    m_hed_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100160E8) /* "head" */);
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100160F0), 0x90F, STR(0x10016114));
    m_bbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10016128) /* "backbone" */);
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100160F0), 0x911, STR(0x10016134));
    m_hnd_L_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100160D4) /* "handL" */);
    if (m_hnd_L_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100160F0), 0x913, STR(0x1001614C));
    m_foot_L_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100160DC) /* "footL" */);
    if (m_foot_L_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100160F0), 0x915, STR(0x10016164));
    setJointCallBack(mpMorf->getModel(), m_hed_jnt_num, 0x021E70F0 /* nodeCB_Head */);
    setJointCallBack(mpMorf->getModel(), m_bbone_jnt_num, 0x021E7254 /* nodeCB_BackBone */);
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    return TRUE;
}
VERIFY(0x021E7398, &daNpc_Aj1_c::bodyCreateHeap);

/* 021E76FC */
BOOL daNpc_Aj1_c::itemCreateHeap() {
    WWHD_FUNC(0x021E76FC, BOOL, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1001617C) /* "Aj" */, 9);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2361, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x10016180), 0x939, STR(0x10016190));
    mpItemModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    return mpItemModel.get() != nullptr;
}
VERIFY(0x021E76FC, &daNpc_Aj1_c::itemCreateHeap);

/* 021E7798 */
BOOL daNpc_Aj1_c::CreateHeap() {
    WWHD_FUNC(0x021E7798, BOOL, this);
    if (!bodyCreateHeap()) {
        return FALSE;
    }
    if (!itemCreateHeap()) {
        mpMorf = nullptr;
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 60.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}
VERIFY(0x021E7798, &daNpc_Aj1_c::CreateHeap);

/* 021E7854 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021E7854, BOOL, i_this);
    return static_cast<daNpc_Aj1_c*>(i_this)->CreateHeap();
}
VERIFY(0x021E7854, CheckCreateHeap);

/* 021E7858: the item in the Y/X button is 0x48 */
u32 daNpc_Aj1_c::_XyCheckCB(int i_btn) {
    WWHD_FUNC(0x021E7858, u32, this, i_btn);
    return dComIfGp_getSelectItem(i_btn) == 0x48;
}
VERIFY(0x021E7858, &daNpc_Aj1_c::_XyCheckCB);

/* 021E7898 */
static u32 daNpc_Aj1_XyCheck_CB(void* i_this, int i_btn) {
    WWHD_FUNC(0x021E7898, u32, i_this, i_btn);
    return ((daNpc_Aj1_c*)i_this)->_XyCheckCB(i_btn);
}
VERIFY(0x021E7898, daNpc_Aj1_XyCheck_CB);

/* 021E789C */
u8 daNpc_Aj1_c::decideType(int i_type) {
    WWHD_FUNC(0x021E789C, u8, this, i_type);
    m92E = 0;
    s8 type = (u32)i_type <= 2 ? (s8)i_type : (s8)-1;
    mType = type;
    return m92E != -1 && type != -1;
}
VERIFY(0x021E789C, &daNpc_Aj1_c::decideType);

/* 021E7900 */
BOOL daNpc_Aj1_c::set_action(ProcFunc_l* i_newProcFunc, void* i_argsP) {
    WWHD_FUNC(0x021E7900, BOOL, this, i_newProcFunc, i_argsP);
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
    }
    if (cur->i != 0) {
        mActState = 9;
        pmf_call(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mActState = 0;
    pmf_call(this, cur, i_argsP);
    return TRUE;
}
VERIFY(0x021E7900, &daNpc_Aj1_c::set_action);

/* 021E7A2C */
bool daNpc_Aj1_c::init_AJ1_0() {
    WWHD_FUNC(0x021E7A2C, bool, this);
    if (!dComIfGs_isEventBit(0x0E20) && !dComIfGs_isEventBit(0x0502)) {
        gabi::store<u8>(gabi::ea(this) + 0x389, 0x1A); /* attention_info.distances[TALK] */
        gabi::Local<ProcFunc_l> pmf;
        pmf_load(pmf, PMF_wait_action1);
        set_action(pmf, nullptr);
        return true;
    }
    if (mSwitchNo != 0xFF && !dComIfGs_isSwitch(mSwitchNo, fopAcM_GetRoomNo(this))) {
        dComIfGs_onSwitch(mSwitchNo, fopAcM_GetRoomNo(this));
    }
    return false;
}
VERIFY(0x021E7A2C, &daNpc_Aj1_c::init_AJ1_0);

/* 021E7B1C */
bool daNpc_Aj1_c::init_AJ1_1() {
    WWHD_FUNC(0x021E7B1C, bool, this);
    if (dComIfGs_isEventBit(0x0520)) {
        return false;
    }
    dComIfGs_onEventBit(0x0502);
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, PMF_wait_action2);
    set_action(pmf, nullptr);
    actor_status = (actor_status & ~0x80u) | 0x4000u; /* OffStatus(NOCULLEXEC), OnStatus(0x4000) */
    return true;
}
VERIFY(0x021E7B1C, &daNpc_Aj1_c::init_AJ1_1);

/* 021E7BCC */
bool daNpc_Aj1_c::init_AJ1_2() {
    WWHD_FUNC(0x021E7BCC, bool, this);
    if (!dComIfGs_isEventBit(0x0520)) {
        return false;
    }
    gabi::store<u32>(gabi::ea(this) + 0x104, 0x021E7898); /* eventInfo.setXyCheckCB(daNpc_Aj1_XyCheck_CB) */
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, PMF_wait_action2);
    set_action(pmf, nullptr);
    return true;
}
VERIFY(0x021E7BCC, &daNpc_Aj1_c::init_AJ1_2);

/* 021E7C5C */
void daNpc_Aj1_c::play_texPttrnAnm() {
    WWHD_FUNC(0x021E7C5C, void, this);
    if (mBtpNum != 0 || cLib_calcTimer(&mBlinkTimer) == 0) {
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10)))) {
            if (mBtpNum != 0) {
                mBtpFrame = (u8)J3DAnm_getFrameMax(gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10)));
            } else {
                mBlinkTimer = (s16)cLib_getRndValue(60, 90);
                mBtpFrame = 0;
            }
        }
    }
}
VERIFY(0x021E7C5C, &daNpc_Aj1_c::play_texPttrnAnm);

/* 021E7D14 */
void daNpc_Aj1_c::play_animation() {
    WWHD_FUNC(0x021E7D14, void, this);
    u32 snd_id = 0;
    play_texPttrnAnm();
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
VERIFY(0x021E7D14, &daNpc_Aj1_c::play_animation);

/* 021E7DB8 */
void daNpc_Aj1_c::setAttention(u32 i_setEyePos) {
    WWHD_FUNC(0x021E7DB8, void, this, i_setEyePos);
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    f32 y = current.pos.y + l_HIO().mPrm.mAttPosOffsetY;
    attPos->x = current.pos.x;
    attPos->y = y;
    attPos->z = current.pos.z;
    if (!mbSetEyePos && !i_setEyePos) {
        return;
    }
    eyePos.z = mEyePos.z;
    eyePos.y = mEyePos.y;
    eyePos.x = mEyePos.x;
}
VERIFY(0x021E7DB8, &daNpc_Aj1_c::setAttention);

/* 021E7E0C */
void daNpc_Aj1_c::setMtx(u32 i_setEyePos) {
    WWHD_FUNC(0x021E7E0C, void, this, i_setEyePos);
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mAngle.x, mAngle.y, mAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    if (mpItemModel.get()) {
        J3DModel* item = mpItemModel;
        J3DModel_setBaseTRMtx(item, getAnmMtx(mpMorf->getModel(), m_hnd_L_jnt_num));
        J3DModel_calc(mpItemModel);
    }
    setAttention(i_setEyePos);
}
VERIFY(0x021E7E0C, &daNpc_Aj1_c::setMtx);

/* 021E7FB4 */
bool daNpc_Aj1_c::createInit() {
    WWHD_FUNC(0x021E7FB4, bool, this);
    mSwitchNo = (u8)(mParameters >> 8);
    /* l_evn_tbl (.data 0x101BB434): "angry" */
    const char* name = gabi::at<const char>(gabi::load<u32>(0x101BB434));
    mEventIDTbl[0] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    mEventCut.setActorInfo2(STR(0x100161B4) /* "Aj1" */, (fopNpc_npc_c*)(void*)this);
    mBckNum = 9;
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xAD); /* attention_info.distances[SPEAK] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xAD); /* attention_info.distances[TALK] */
    bool init_result;
    switch ((u32)(s32)mType) {
    case 0:
        init_result = init_AJ1_0();
        break;
    case 1:
        init_result = init_AJ1_1();
        break;
    case 2:
        init_result = init_AJ1_2();
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
    gravity = -4.5f;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    play_animation();
    mObjAcch.CrrPos(dComIfG_Bgsp());
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}
VERIFY(0x021E7FB4, &daNpc_Aj1_c::createInit);

/* 021E8184 */
cPhs_State daNpc_Aj1_c::_create() {
    WWHD_FUNC(0x021E8184, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Aj1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this);    /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = 0x10016280;
            gabi::call(0x025E7820, mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dPa_smokeEcallBack_ct(mSmokeCB, 1);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, STR(0x100161CC) /* "Aj" */);
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    if (!fopAcM_entrySolidHeap(this, 0x021E7854 /* CheckCreateHeap */, gabi::load<u32>(0x101BB458))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -60.0f, -20.0f, -60.0f, 80.0f, 260.0f, 100.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x021E8184, &daNpc_Aj1_c::_create);

/* 021E82C8 */
static cPhs_State daNpc_Aj1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021E82C8, cPhs_State, i_this);
    return ((daNpc_Aj1_c*)i_this)->_create();
}
VERIFY(0x021E82C8, daNpc_Aj1_Create);

/* 021E82CC */
void daNpc_Aj1_c::del_pa(gptr<JPABaseEmitter>* i_emitterP) {
    WWHD_FUNC(0x021E82CC, void, this, i_emitterP);
    JPABaseEmitter* e = *i_emitterP;
    if (e == nullptr) {
        return;
    }
    emitter_stop(gabi::ea(e)); /* becomeInvalidEmitter() */
    *i_emitterP = nullptr;
}
VERIFY(0x021E82CC, &daNpc_Aj1_c::del_pa);

/* 021E82F8 */
BOOL daNpc_Aj1_c::_delete() {
    WWHD_FUNC(0x021E82F8, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x100161CF) /* "Aj" */);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    del_pa(&mpPunEmitter);
    del_pa(&mpAkaEmitter);
    del_pa(&mpDonEmitter);
    dPa_smokeEcallBack_end((dPa_smokeEcallBack*)(void*)mSmokeCB);
    return TRUE;
}
VERIFY(0x021E82F8, &daNpc_Aj1_c::_delete);

/* 021E8374 */
static BOOL daNpc_Aj1_Delete(daNpc_Aj1_c* i_this) {
    WWHD_FUNC(0x021E8374, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x021E8374, daNpc_Aj1_Delete);

/* 021E8378 */
void daNpc_Aj1_c::checkOrder() {
    WWHD_FUNC(0x021E8378, void, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIDTbl[mEventIndex])) {
            if (mEventIndex == 0) {
                actor_status &= ~0x4000u;
            }
            mEvtState = 0;
        }
    } else if (command == 1 /* checkCommandTalk() */) {
        if (mEvtState == 1 || mEvtState == 2) {
            mEvtState = 0;
            mbTalk = true;
        }
    }
}
VERIFY(0x021E8378, &daNpc_Aj1_c::checkOrder);

/* 021E844C */
u8 daNpc_Aj1_c::demo() {
    WWHD_FUNC(0x021E844C, u8, this);
    u8 id = demoActorID;
    if (id == 0) {
        if (mbDemo) {
            mbDemo = false;
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
            JUT_ASSERT_fail(STR(0x10016090), 0x23A, STR(0x10016080));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor_p = gabi::call<void*>(0x02526E70, obj, id);
    }
    if (gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10) != 0) {
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10)))) {
            mBtpFrame = (u8)J3DAnm_getFrameMax(gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10)));
        }
    }
    if (demo_actor_p) {
        void* demo_btp_p = gabi::call<void*>(0x02527828, demo_actor_p, STR(0x100161D2) /* "Aj" */); /* getP_BtpData */
        if (demo_btp_p) {
            mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(mpMorf->getModel()), demo_btp_p, TRUE, 0, 1.0f, 0, -1, 1, 0);
            mBtpFrame = 0;
            mBtpNum = 1;
        }
    }
    gabi::call(0x02527028, this, 0x6A, mpMorf.get(), STR(0x100161D2), 0, 0, 0, 0); /* dDemo_setDemoData */
    return mbDemo;
}
VERIFY(0x021E844C, &daNpc_Aj1_c::demo);

/* 021E8614 */
s32 daNpc_Aj1_c::isEventEntry() {
    WWHD_FUNC(0x021E8614, s32, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x021E8614, &daNpc_Aj1_c::isEventEntry);

/* 021E8654 */
void daNpc_Aj1_c::endEvent() {
    WWHD_FUNC(0x021E8654, void, this);
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
    mMesgAnimeTag = 0xFF;
}
VERIFY(0x021E8654, &daNpc_Aj1_c::endEvent);

/* 021E8698 */
void daNpc_Aj1_c::cut_init_AJ1_TLK() {
    WWHD_FUNC(0x021E8698, void, this);
    mAnmAtr = 0xFF;
    mAtrSet = 0;
    mMesgAnimeTag = 0xFF;
}
VERIFY(0x021E8698, &daNpc_Aj1_c::cut_init_AJ1_TLK);

/* 021E86B0 */
int daNpc_Aj1_c::bckResID(int i_bckNum) {
    WWHD_FUNC(0x021E86B0, int, this, i_bckNum);
    return gabi::load<s32>(0x100161D8 + i_bckNum * 4);
}
VERIFY(0x021E86B0, &daNpc_Aj1_c::bckResID);

static inline JPABaseEmitter* particle_set(u16 id, cXyz* pos, csXyz* angle, u8 alpha, void* cb, u8 grp, s8 roomNo) {
    return dPa_control_set(dComIfGp_getParticle(), grp, id, pos, angle, nullptr, alpha, (dPa_levelEcallBack*)cb, roomNo,
                           nullptr, nullptr, nullptr);
}

/* 021E86C4 */
void daNpc_Aj1_c::set_pa_pun() {
    WWHD_FUNC(0x021E86C4, void, this);
    s8 roomNo = fopAcM_GetRoomNo(this);
    JPABaseEmitter* e = particle_set(0x8113, &current.pos, nullptr, 0xFF, nullptr, 0, roomNo);
    mpPunEmitter = e;
    if (e) {
        mPunTimer = 0;
    }
}
VERIFY(0x021E86C4, &daNpc_Aj1_c::set_pa_pun);

/* 021E874C */
void daNpc_Aj1_c::set_pa_aka() {
    WWHD_FUNC(0x021E874C, void, this);
    JPABaseEmitter* e = mpAkaEmitter;
    if (e) {
        emitter_stop(gabi::ea(e));
    }
    s8 roomNo = fopAcM_GetRoomNo(this);
    mpAkaEmitter = particle_set(0x811F, &current.pos, nullptr, 0xFF, nullptr, 0, roomNo);
}
VERIFY(0x021E874C, &daNpc_Aj1_c::set_pa_aka);

/* 021E87E4 */
void daNpc_Aj1_c::set_pa_don() {
    WWHD_FUNC(0x021E87E4, void, this);
    gabi::Local<cXyz> offset;
    offset->x = 37.3f;
    offset->y = 0.0f;
    offset->z = 13.8f;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), offset, &mDonPos);
    s8 roomNo = fopAcM_GetRoomNo(this);
    mpDonEmitter = particle_set(0x8114, &mDonPos, nullptr, 0xFF, nullptr, 0, roomNo);
}
VERIFY(0x021E87E4, &daNpc_Aj1_c::set_pa_don);

/* 021E88B4 */
void daNpc_Aj1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    WWHD_FUNC(0x021E88B4, void, this, i_anmPrmP);
    s8 bck = i_anmPrmP->bckNum;
    if (bck < 0 || mBckNum == bck) {
        return;
    }
    int resID = bckResID(bck);
    s32 loopMode = i_anmPrmP->loopMode;
    f32 speed = i_anmPrmP->speed;
    f32 morf = i_anmPrmP->morf;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, STR(0x10016204) /* "Aj" */);
    s8 new_bck = i_anmPrmP->bckNum;
    m8C9 = 0;
    mPrevMorfFrame = 0.0f;
    mbMorfAnimStopped = false;
    mBckNum = new_bck;
    if (new_bck == 2) {
        set_pa_pun();
        set_pa_aka();
        set_pa_don();
        return;
    }
    JPABaseEmitter* e = mpAkaEmitter;
    if (e) {
        u32 a = gabi::ea(e) + 0x254;
        gabi::store<u32>(a, gabi::load<u32>(a) | 1); /* stopCreateParticle() */
        mbAkaStop = true;
    }
    del_pa(&mpPunEmitter);
    del_pa(&mpDonEmitter);
}
VERIFY(0x021E88B4, &daNpc_Aj1_c::setAnm_anm);

/* 021E89C0 */
void daNpc_Aj1_c::setAnm_NUM(int i_anmNum, int i_setBtp) {
    WWHD_FUNC(0x021E89C0, void, this, i_anmNum, i_setBtp);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BB45C); /* [9] */
    if (i_setBtp) {
        init_texPttrnAnm(a_anm_prm_tbl[i_anmNum].btpNum, true);
    }
    setAnm_anm(&a_anm_prm_tbl[i_anmNum]);
}
VERIFY(0x021E89C0, &daNpc_Aj1_c::setAnm_NUM);

/* 021E8A30 */
void daNpc_Aj1_c::cut_init_INI_ANGRY() {
    WWHD_FUNC(0x021E8A30, void, this);
    setAnm_NUM(0, 1);
    mpMorf->setMorf(8.0f);
}
VERIFY(0x021E8A30, &daNpc_Aj1_c::cut_init_INI_ANGRY);

/* 021E8A74 */
void daNpc_Aj1_c::cut_init_INVIT() {
    WWHD_FUNC(0x021E8A74, void, this);
    mDoAud_seStart(0x4893, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
}
VERIFY(0x021E8A74, &daNpc_Aj1_c::cut_init_INVIT);

/* 021E8ABC */
u32 daNpc_Aj1_c::cut_move_AJ1_TLK() {
    WWHD_FUNC(0x021E8ABC, u32, this);
    if (talk(1) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        mMesgAnimeTag = 0xFF;
        mAtrSet = 0;
        mAnmAtr = 0xFF;
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x021E8ABC, &daNpc_Aj1_c::cut_move_AJ1_TLK);

/* 021E8B24 */
u32 daNpc_Aj1_c::cut_move_VIVRATE() {
    WWHD_FUNC(0x021E8B24, u32, this);
    if (mbMorfAnimStopped) {
        mDoAud_seStart(0x58A2, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        gabi::Local<cXyz> up;
        up->x = 0.0f;
        up->y = 1.0f;
        up->z = 0.0f;
        dComIfGp_getVibration_StartShock(5, -0x11, up);
        setAnm_NUM(3, 1);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x021E8B24, &daNpc_Aj1_c::cut_move_VIVRATE);

/* 021E8BD8 */
u32 daNpc_Aj1_c::cut_move_JMP() {
    WWHD_FUNC(0x021E8BD8, u32, this);
    if (mbMorfAnimStopped) {
        dComIfGp_detect_set_quake(0);
        mCutTimer = 0x14;
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x021E8BD8, &daNpc_Aj1_c::cut_move_JMP);

/* 021E8C40 */
u32 daNpc_Aj1_c::cut_move_SPPRISE() {
    WWHD_FUNC(0x021E8C40, u32, this);
    if (cLib_calcTimer(&mCutTimer) == 0) {
        setAnm_NUM(4, 1);
        mCutTimer = l_HIO().mPrm.mSpprTime;
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x021E8C40, &daNpc_Aj1_c::cut_move_SPPRISE);

/* 021E8CB0 */
u32 daNpc_Aj1_c::cut_move_LOK() {
    WWHD_FUNC(0x021E8CB0, u32, this);
    if (mbMorfAnimStopped && cLib_calcTimer(&mCutTimer) == 0) {
        setAnm_NUM(5, 1);
        mDoAud_seStart(0x4894, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        mCutTimer = l_HIO().mPrm.mLokTime;
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x021E8CB0, &daNpc_Aj1_c::cut_move_LOK);

/* 021E8D4C */
u32 daNpc_Aj1_c::cut_move_DAN() {
    WWHD_FUNC(0x021E8D4C, u32, this);
    if (mbMorfAnimStopped && cLib_calcTimer(&mCutTimer) == 0) {
        setAnm_NUM(2, 1);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x021E8D4C, &daNpc_Aj1_c::cut_move_DAN);

/* 021E8DBC */
void daNpc_Aj1_c::privateCut(int i_staffIdx) {
    WWHD_FUNC(0x021E8DBC, void, this, i_staffIdx);
    /* a_cut_tbl (.data 0x101BB4EC): "AJ1_TLK", "INI_ANGRY", "VIVRATE", "JMP", "SPPRISE", "LOK", "DAN", "INVITE" */
    if (i_staffIdx == -1) {
        return;
    }
    s8 idx = dComIfGp_evmng_getMyActIdx(i_staffIdx, 0x101BB4EC, 8, TRUE, 0);
    mActionIndex = idx;
    if (idx == -1) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(i_staffIdx)) {
        switch ((u32)(s32)mActionIndex) {
        case 0:
            cut_init_AJ1_TLK();
            break;
        case 1:
            cut_init_INI_ANGRY();
            break;
        case 7:
            cut_init_INVIT();
            break;
        }
    }
    u32 cut_end;
    switch ((u32)(s32)mActionIndex) {
    case 0:
        cut_end = cut_move_AJ1_TLK();
        break;
    case 2:
        cut_end = cut_move_VIVRATE();
        break;
    case 3:
        cut_end = cut_move_JMP();
        break;
    case 4:
        cut_end = cut_move_SPPRISE();
        break;
    case 5:
        cut_end = cut_move_LOK();
        break;
    case 6:
        cut_end = cut_move_DAN();
        break;
    default:
        cut_end = TRUE;
        break;
    }
    if (cut_end) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}
VERIFY(0x021E8DBC, &daNpc_Aj1_c::privateCut);

/* 021E8F80 */
void daNpc_Aj1_c::lookBack() {
    WWHD_FUNC(0x021E8F80, void, this);
    u8 head_only = mbHeadOnly;
    gabi::Local<cXyz> look_pos;
    look_pos->set(0.0f, 0.0f, 0.0f);
    s16 target_y = current.angle.y;
    mJointHeadY = m_jnt.mAngles[0][1];
    mActorAngleY = target_y;
    f32 srcX = current.pos.x;
    f32 srcY = eyePos.y;
    f32 srcZ = current.pos.z;
    mJointBackboneY = m_jnt.mAngles[1][1];
    cXyz* look_pos_p = nullptr;
    switch ((u32)(s32)mLookMode) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
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
    }
    cLib_addCalcAngleS2(&mLookVel, l_HIO().mPrm.mLookVelMax, 4, 0x800);
    if (!m_jnt.mbTrn) {
        mLookVel = 0;
    }
    gabi::Local<cXyz> cur_pos; /* passed by value: a copy */
    cur_pos->x = srcX;
    cur_pos->y = srcY;
    cur_pos->z = srcZ;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, look_pos_p, cur_pos, target_y, mLookVel, head_only);
}
VERIFY(0x021E8F80, &daNpc_Aj1_c::lookBack);

/* 021E91A8 */
void daNpc_Aj1_c::event_proc(int i_staffIdx) {
    WWHD_FUNC(0x021E91A8, void, this, i_staffIdx);
    if (dComIfGp_evmng_endCheck(mEventIDTbl[mEventIndex])) {
        if (mEventIndex == 0) {
            dComIfGs_onEventBit(0x0508);
            dComIfGs_onEventBit(0x0504);
        }
        endEvent();
    } else {
        if (!mEventCut.cutProc()) {
            privateCut(i_staffIdx);
        }
        lookBack();
    }
}
VERIFY(0x021E91A8, &daNpc_Aj1_c::event_proc);

/* 021E9284 */
void daNpc_Aj1_c::eventOrder() {
    WWHD_FUNC(0x021E9284, void, this);
    s8 state = mEvtState;
    if (state == 1 || state == 2) {
        eventInfo_onCondition(this, 1);    /* dEvtCnd_CANTALK_e */
        if (mType == 2) {
            eventInfo_onCondition(this, 0x20); /* dEvtCnd_CANTALKITEM_e */
        }
        if (mEvtState == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (state >= 3) {
        mEventIndex = (s16)(state - 3);
        fopAcM_orderOtherEventId(this, mEventIDTbl[mEventIndex], 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x021E9284, &daNpc_Aj1_c::eventOrder);

/* 021E930C */
void daNpc_Aj1_c::flw_pa_pun() {
    WWHD_FUNC(0x021E930C, void, this);
    JPABaseEmitter* e = mpPunEmitter;
    if (e == nullptr) {
        return;
    }
    JPASetRMtxTVecfromMtx(getAnmMtx(mpMorf->getModel(), m_hed_jnt_num), gabi::ea(e) + 0x1F0, gabi::ea(e) + 0x22C);
    if (cLib_calcTimer(&mPunTimer) == 0) {
        mDoAud_seStart(0x58A3, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        mPunTimer = 5;
    }
}
VERIFY(0x021E930C, &daNpc_Aj1_c::flw_pa_pun);

/* 021E93B0 */
void daNpc_Aj1_c::del_pa_aka() {
    WWHD_FUNC(0x021E93B0, void, this);
    if (!mbAkaStop) {
        return;
    }
    JPABaseEmitter* e = mpAkaEmitter;
    if (e) {
        u32 ea = gabi::ea(e);
        if (gabi::load<u32>(ea + 0x1B4) + gabi::load<u32>(ea + 0x1C0) != 0) { /* getParticleNumber() != 0 */
            return;
        }
        emitter_stop(ea);
        mbAkaStop = false;
        mpAkaEmitter = nullptr;
    } else {
        mbAkaStop = false;
    }
}
VERIFY(0x021E93B0, &daNpc_Aj1_c::del_pa_aka);

/* 021E9404 */
void daNpc_Aj1_c::flw_pa_aka() {
    WWHD_FUNC(0x021E9404, void, this);
    JPABaseEmitter* e = mpAkaEmitter;
    if (e == nullptr) {
        return;
    }
    JPASetRMtxTVecfromMtx(getAnmMtx(mpMorf->getModel(), m_hed_jnt_num), gabi::ea(e) + 0x1F0, gabi::ea(e) + 0x22C);
}
VERIFY(0x021E9404, &daNpc_Aj1_c::flw_pa_aka);

/* 021E9448 */
void daNpc_Aj1_c::set_pa_smk() {
    WWHD_FUNC(0x021E9448, void, this);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(mpMorf->getModel(), m_foot_L_jnt_num), stk);
    mSmokePos.x = stk->m[0][3];
    mSmokePos.y = stk->m[1][3];
    mSmokePos.z = stk->m[2][3];
    dPa_smokeEcallBack_end((dPa_smokeEcallBack*)(void*)mSmokeCB);
    s8 roomNo = fopAcM_GetRoomNo(this);
    /* dComIfGp_particle_setToon(0x2027, &mSmokePos, &current.angle, NULL, 0xC8, &mSmokeCB, roomNo) */
    JPABaseEmitter* e = particle_set(0x2027, &mSmokePos, &current.angle, 0xC8, mSmokeCB, 2, roomNo);
    mpSmokeEmitter = e;
    if (e) {
        u32 a = gabi::ea(e);
        gabi::store<f32>(a + 0x240, 1.0f); /* setGlobalParticleScale(0.3, 0.3, 1.0) */
        gabi::store<f32>(a + 0x238, 0.3f);
        gabi::store<f32>(a + 0x23C, 0.3f);
        a = gabi::ea(mpSmokeEmitter.get());
        gabi::store<f32>(a + 0x228, 0.1f);
        gabi::store<f32>(a + 0x224, 0.1f);
        gabi::store<f32>(a + 0x220, 0.1f);
        gabi::store<u16>(gabi::ea(mpSmokeEmitter.get()) + 0x60, 0x28);
        gabi::store<f32>(gabi::ea(mpSmokeEmitter.get()) + 0x34, 3.0f);
        gabi::store<u32>(gabi::ea(mpSmokeEmitter.get()) + 0x5C, 1);
        gabi::store<f32>(gabi::ea(mpSmokeEmitter.get()) + 0x70, 120.0f);
        a = gabi::ea(mpSmokeEmitter.get());
        gabi::store<f32>(a + 0x8, 1.0f);
        gabi::store<f32>(a + 0xC, 0.1f);
        gabi::store<f32>(a + 0x10, 1.0f);
        /* mSmokeCB.setSmokeColor({0xA0, 0xA0, 0x80, 0xC8}) */
        u32 c = gabi::ea(mSmokeCB) + 0x16;
        gabi::store<u8>(c + 0, 0xA0);
        gabi::store<u8>(c + 1, 0xA0);
        gabi::store<u8>(c + 2, 0x80);
        gabi::store<u8>(c + 3, 0xC8);
    }
}
VERIFY(0x021E9448, &daNpc_Aj1_c::set_pa_smk);

/* 021E95D4 */
void daNpc_Aj1_c::setSmoke() {
    WWHD_FUNC(0x021E95D4, void, this);
    if (mBckNum != 2) {
        return;
    }
    if (mpMorf->checkFrame(0.0f) || mpMorf->checkFrame(9.0f)) {
        set_pa_smk();
    }
    if (mpMorf->checkFrame(0.0f) || mpMorf->checkFrame(10.0f)) {
        mDoAud_seStart(0x58A4, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    }
}
VERIFY(0x021E95D4, &daNpc_Aj1_c::setSmoke);

/* 021E96BC */
BOOL daNpc_Aj1_c::_execute() {
    WWHD_FUNC(0x021E96BC, BOOL, this);
    if (!mbHomeSet) {
        mHomePos.copy(current.pos);
        mHomeAngle.x = current.angle.x;
        mHomeAngle.y = current.angle.y;
        mHomeAngle.z = current.angle.z;
        mbHomeSet = true;
    }
    daNpc_Aj1_HIO_c::hio_prm_c& prm = l_HIO().mPrm;
    m_jnt.setParam(prm.mMaxBackBoneX, prm.mMaxBackBoneY, prm.mMinBackBoneX, prm.mMinBackBoneY, prm.mMaxHeadX,
                   prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (m8CE && demoActorID == 0) {
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
        fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        play_animation();
        mObjAcch.CrrPos(dComIfG_Bgsp());
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
    flw_pa_pun();
    del_pa_aka();
    flw_pa_aka();
    setSmoke();
    if (!mbDemo) {
        setCollision(60.0f, 140.0f);
    }
    return TRUE;
}
VERIFY(0x021E96BC, &daNpc_Aj1_c::_execute);

/* 021E991C */
static BOOL daNpc_Aj1_Execute(daNpc_Aj1_c* i_this) {
    WWHD_FUNC(0x021E991C, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x021E991C, daNpc_Aj1_Execute);

/* static initialisation of a 4-byte function-local constant on first use */
static inline void local_static_init(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(dst), gabi::at<u8>(src), 4); /* 028FEAC0 memcpy */
    }
}

/* 021E9920 */
BOOL daNpc_Aj1_c::_draw() {
    WWHD_FUNC(0x021E9920, BOOL, this);
    J3DModel* morf_model_p = mpMorf->getModel();
    J3DModelData* morf_model_info_p = J3DModel_getModelData_l(morf_model_p);
    if (m8CE || mbNoDraw) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, morf_model_info_p, mBtpFrame);
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(morf_model_info_p) + 0x38, 0); /* mBtpAnm.remove() */
    if (mpItemModel.get()) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpItemModel, &tevStr);
        mDoExt_modelEntryDL(mpItemModel);
    }
    /* debug leftovers: function-local static colors initialised on first use */
    if (l_HIO().mPrm.mDebugDraw) {
        local_static_init(0x101FDA50, 0x101FEBF4, 0x10016020);
        local_static_init(0x101FDAC0, 0x101FEBF8, 0x10016024);
        local_static_init(0x101FDA44, 0x101FEBE8, 0x10016028);
    }
    /* HD: no shadowDraw() */
    dSnap_RegistFig(0x4C /* DSNAP_TYPE_NPC_AJ1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x021E9920, &daNpc_Aj1_c::_draw);

/* 021E9AA8 */
static BOOL daNpc_Aj1_Draw(daNpc_Aj1_c* i_this) {
    WWHD_FUNC(0x021E9AA8, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x021E9AA8, daNpc_Aj1_Draw);

/* 021E9AAC */
static BOOL daNpc_Aj1_IsDelete(daNpc_Aj1_c*) {
    WWHD_FUNC(0x021E9AAC, BOOL, (daNpc_Aj1_c*)nullptr);
    return TRUE;
}
VERIFY(0x021E9AAC, daNpc_Aj1_IsDelete);

/* 021E9AB4: true when the head, backbone and body did not turn this frame */
u8 daNpc_Aj1_c::chk_parts_notMov() {
    WWHD_FUNC(0x021E9AB4, u8, this);
    return mJointHeadY == m_jnt.mAngles[0][1] && mJointBackboneY == m_jnt.mAngles[1][1] && mActorAngleY == current.angle.y;
}
VERIFY(0x021E9AB4, &daNpc_Aj1_c::chk_parts_notMov);

/* 021E9AF4 */
void daNpc_Aj1_c::ctrl_WAITanm() {
    WWHD_FUNC(0x021E9AF4, void, this);
    switch ((u32)(s32)mBckNum) {
    case 0:
        if (cLib_calcTimer(&mWaitTimer) == 0 && mbMorfAnimStopped && chk_parts_notMov()) {
            setAnm_NUM(1, 1);
        }
        break;
    case 1:
        if (mbMorfAnimStopped) {
            setAnm_NUM(0, 1);
            mWaitTimer = (s16)cLib_getRndValue(90, 180);
        }
        break;
    }
}
VERIFY(0x021E9AF4, &daNpc_Aj1_c::ctrl_WAITanm);

/* 021E9BC0 */
void daNpc_Aj1_c::ctrl_TIREanm() {
    WWHD_FUNC(0x021E9BC0, void, this);
    if (mBckNum == 6) {
        if (mbMorfAnimStopped && cLib_calcTimer(&mTireTimer) == 0) {
            setAnm_NUM(0, 1);
            mpMorf->setMorf(20.0f);
        }
    } else {
        mTireTimer = 0;
    }
}
VERIFY(0x021E9BC0, &daNpc_Aj1_c::ctrl_TIREanm);

/* 021E9C50 */
void daNpc_Aj1_c::setAnm() {
    WWHD_FUNC(0x021E9C50, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BB50C); /* [5] */
    init_texPttrnAnm(a_anm_prm_tbl[mStt].btpNum, true);
    setAnm_anm(&a_anm_prm_tbl[mStt]);
}
VERIFY(0x021E9C50, &daNpc_Aj1_c::setAnm);

/* 021E9CC0 */
void daNpc_Aj1_c::setAnm_ATR() {
    WWHD_FUNC(0x021E9CC0, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BB55C); /* [9] */
    init_texPttrnAnm(a_anm_prm_tbl[mAnmAtr].btpNum, true);
    setAnm_anm(&a_anm_prm_tbl[mAnmAtr]);
}
VERIFY(0x021E9CC0, &daNpc_Aj1_c::setAnm_ATR);

/* 021E9D28 */
void daNpc_Aj1_c::chngAnmAtr(u8 i_atr) {
    WWHD_FUNC(0x021E9D28, void, this, i_atr);
    if (i_atr == mAnmAtr || i_atr >= 9) {
        return;
    }
    mAnmAtr = i_atr;
    setAnm_ATR();
    if (mAnmAtr == 8) {
        mTireTimer = 3;
    }
}
VERIFY(0x021E9D28, &daNpc_Aj1_c::chngAnmAtr);

/* 021E9D84 */
void daNpc_Aj1_c::ctrlAnmAtr() {
    WWHD_FUNC(0x021E9D84, void, this);
    switch (mAnmAtr) {
    case 2:
        ctrl_WAITanm();
        break;
    case 5:
        if (mbMorfAnimStopped) {
            mAnmAtr = 0;
            setAnm_NUM(0, 1);
        }
        break;
    }
    ctrl_TIREanm();
}
VERIFY(0x021E9D84, &daNpc_Aj1_c::ctrlAnmAtr);

/* 021E9E08 */
void daNpc_Aj1_c::anmAtr(u16 i_msgStatus) {
    WWHD_FUNC(0x021E9E08, void, this, i_msgStatus);
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
VERIFY(0x021E9E08, &daNpc_Aj1_c::anmAtr);

/* 021E9EC8 */
u16 daNpc_Aj1_c::next_msgStatus(be<u32>* o_msgNoP) {
    WWHD_FUNC(0x021E9EC8, u16, this, o_msgNoP);
    u16 msg_status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    switch (*o_msgNoP) {
    case 0x9C6:
        *o_msgNoP = 0x9C7;
        break;
    case 0x9C7:
        *o_msgNoP = 0x9C8;
        break;
    case 0x9C8:
    case 0x9CA:
        if (dComIfGs_isEventBit(0x0001) && !dComIfGs_isEventBit(0x3704)) {
            *o_msgNoP = 0x9DA;
        } else {
            msg_status = 0x10;
        }
        break;
    case 0x9C9:
        *o_msgNoP = 0x9CA;
        break;
    case 0x9CB:
    case 0x9DC:
        *o_msgNoP = 0x9CC;
        break;
    case 0x9CD:
        *o_msgNoP = 0x9CE;
        break;
    case 0x9CF:
        if (dComIfGs_isEventBit(0x0001) && !dComIfGs_isEventBit(0x3704)) {
            *o_msgNoP = 0x9DD;
        } else {
            msg_status = 0x10;
        }
        break;
    case 0x9D0:
        if (!dComIfGs_isEventBit(0x2A20)) {
            *o_msgNoP = 0x9D1;
        } else {
            *o_msgNoP = dKy_daynight_check() ? 0x9D3 : 0x9D2;
        }
        break;
    case 0x9DB:
        if (dComIfGs_isEventBit(0x0504)) {
            *o_msgNoP = 0x9CF;
        } else {
            *o_msgNoP = dComIfGs_isEventBit(0x2A80) ? 0x9DC : 0x9CB;
        }
        break;
    default:
        msg_status = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msg_status;
}
VERIFY(0x021E9EC8, &daNpc_Aj1_c::next_msgStatus);

/* 021EA0E0 */
u32 daNpc_Aj1_c::getMsg_AJ1_0() {
    WWHD_FUNC(0x021EA0E0, u32, this);
    return dComIfGs_isEventBit(0x0510) ? 0x9C9 : 0x9C6;
}
VERIFY(0x021EA0E0, &daNpc_Aj1_c::getMsg_AJ1_0);

/* 021EA12C */
u32 daNpc_Aj1_c::getMsg_AJ1_1() {
    WWHD_FUNC(0x021EA12C, u32, this);
    if (dComIfGs_isEventBit(0x0E20) && !dComIfGs_isEventBit(0x3702)) {
        dComIfGs_onEventBit(0x3702);
        return 0x9DB;
    }
    if (dComIfGs_isEventBit(0x0504)) {
        return 0x9CF;
    }
    return dComIfGs_isEventBit(0x2A80) ? 0x9DC : 0x9CB;
}
VERIFY(0x021EA12C, &daNpc_Aj1_c::getMsg_AJ1_1);

/* 021EA200 */
u32 daNpc_Aj1_c::getMsg_AJ1_2() {
    WWHD_FUNC(0x021EA200, u32, this);
    if (mItemNo == 0x48) {
        if (!dComIfGs_isEventBit(0x3701)) {
            return 0x9D7;
        }
        return dComIfGs_isEventBit(0x0B20) ? 0x9D8 : 0x9D9;
    }
    if (!dComIfGs_isEventBit(0x3708)) {
        return 0x9D0;
    }
    if (!dComIfGs_isEventBit(0x2A20)) {
        return 0x9D4;
    }
    return dKy_daynight_check() ? 0x9D6 : 0x9D5;
}
VERIFY(0x021EA200, &daNpc_Aj1_c::getMsg_AJ1_2);

/* 021EA308 */
u32 daNpc_Aj1_c::getMsg() {
    WWHD_FUNC(0x021EA308, u32, this);
    switch ((u32)(s32)mType) {
    case 0:
        return getMsg_AJ1_0();
    case 1:
        return getMsg_AJ1_1();
    case 2:
        return getMsg_AJ1_2();
    }
    return 0;
}
VERIFY(0x021EA308, &daNpc_Aj1_c::getMsg);

/* 021EA360 */
u8 daNpc_Aj1_c::chk_talk() {
    WWHD_FUNC(0x021EA360, u8, this);
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
VERIFY(0x021EA360, &daNpc_Aj1_c::chk_talk);

/* 021EA3F8 */
u8 daNpc_Aj1_c::chkAttention() {
    WWHD_FUNC(0x021EA3F8, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x021EA3F8, &daNpc_Aj1_c::chkAttention);

/* 021EA480 */
void daNpc_Aj1_c::setStt(s8 i_stt) {
    WWHD_FUNC(0x021EA480, void, this, i_stt);
    s8 prev = mStt;
    mStt = i_stt;
    switch ((u32)(s32)i_stt) {
    case 1:
        mEvtState = 0;
        mFarTimer = (s16)cLib_getRndValue(90, 120);
        break;
    case 2:
        mEvtState = 0;
        break;
    case 3:
        mEvtState = 0;
        mAnmAtr = 0xFF;
        mPrevStt = prev;
        mMesgAnimeTag = 0xFF;
        mAtrSet = 0;
        break;
    case 4:
        mEvtState = 0;
        mWaitTimer = (s16)cLib_getRndValue(90, 180);
        if (mBckNum == 1 || mBckNum == 6) {
            return;
        }
        break;
    }
    setAnm();
}
VERIFY(0x021EA480, &daNpc_Aj1_c::setStt);

/* 021EA570 (cXyz by value: a pointer to the caller's copy) */
u8 daNpc_Aj1_c::chk_areaIN(f32 i_dist, s16 i_angle, cXyz* i_pos) {
    WWHD_FUNC(0x021EA570, u8, this, i_dist, i_angle, i_pos);
    gabi::Local<cXyz> diff;
    cXyz_mi(&dComIfGp_getPlayer(0)->current.pos, diff, i_pos);
    gabi::Local<cXyz> xz;
    f32 dx = diff->x, dz = diff->z;
    xz->x = dx;
    xz->y = 0.0f;
    xz->z = dz;
    f32 abs_xz = std_sqrtf(PSVECSquareMag(xz));
    f32 dy = dComIfGp_getPlayer(0)->current.pos.y - i_pos->y;
    s16 angle = (s16)(cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos) - mHomeAngle.y);
    if (abs_xz < i_dist && std::fabs(dy) < 500.0f && abs((int)angle) < (int)i_angle) {
        return true;
    }
    return false;
}
VERIFY(0x021EA570, &daNpc_Aj1_c::chk_areaIN);

/* 021EA6DC */
BOOL daNpc_Aj1_c::FARwai() {
    WWHD_FUNC(0x021EA6DC, BOOL, this);
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    if (chk_areaIN(l_HIO().mPrm.mFarDist, l_HIO().mPrm.mFarAngle, pos)) {
        setStt(2);
        mLookMode = 1;
        mbHeadOnly = false;
        m_jnt.mbTrn = 1; /* setTrn() */
        return TRUE;
    }
    if (mBckNum == 8) {
        if (mbMorfAnimStopped) {
            setAnm_NUM(0, 1);
            mpMorf->setMorf(8.0f);
        }
        return TRUE;
    }
    if (cLib_calcTimer(&mFarTimer) == 0) {
        setAnm_NUM(8, 1);
        mFarTimer = (s16)cLib_getRndValue(90, 120);
    }
    return TRUE;
}
VERIFY(0x021EA6DC, &daNpc_Aj1_c::FARwai);

/* 021EA7D8 */
BOOL daNpc_Aj1_c::call_1() {
    WWHD_FUNC(0x021EA7D8, BOOL, this);
    if (mbTalk) {
        if (chk_talk()) {
            setStt(3);
            mbHeadOnly = false;
            m_jnt.mbTrn = 1; /* setTrn() */
            mLookMode = 1;
        }
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    pos->z = current.pos.z;
    pos->y = current.pos.y;
    pos->x = current.pos.x;
    if (!chk_areaIN(l_HIO().mPrm.mFarDist, l_HIO().mPrm.mFarAngle, pos)) {
        setStt(1);
        mpMorf->setMorf(15.0f);
        mLookMode = 3;
        m_jnt.mbTrn = 1;
        mbHeadOnly = false;
        mLookAngleY = mHomeAngle.y;
        return TRUE;
    }
    m_jnt.mbTrn = 1;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    if (chk_areaIN(l_HIO().mPrm.mCallDist, l_HIO().mPrm.mCallAngle, pos)) {
        mEvtState = 2;
        if (mbAttention) {
            if (mBckNum != 0) {
                setAnm_NUM(0, 1);
                mpMorf->setMorf(15.0f);
            }
        } else if (mBckNum != 7) {
            setAnm_NUM(7, 1);
        }
    }
    return TRUE;
}
VERIFY(0x021EA7D8, &daNpc_Aj1_c::call_1);

/* 021EA97C */
BOOL daNpc_Aj1_c::wait_1() {
    WWHD_FUNC(0x021EA97C, BOOL, this);
    cLib_addCalcAngleS(&current.angle.y, mHomeAngle.y, 4, 0x400, 0);
    ctrl_WAITanm();
    if (mEvtState == 1 || mEvtState >= 3) {
        return TRUE;
    }
    ctrl_TIREanm();
    if (mbTalk) {
        if (chk_talk()) {
            setStt(3);
            mLookMode = 1;
            mbHeadOnly = false;
            m_jnt.mbTrn = 1;
        }
        return TRUE;
    }
    mEvtState = 2;
    if (mBckNum != 1 && mbAttention) {
        mLookMode = 1;
        return TRUE;
    }
    mLookMode = 3;
    mLookAngleY = mHomeAngle.y;
    return TRUE;
}
VERIFY(0x021EA97C, &daNpc_Aj1_c::wait_1);

/* 021EAA68 */
BOOL daNpc_Aj1_c::talk_1() {
    WWHD_FUNC(0x021EAA68, BOOL, this);
    BOOL res = chk_parts_notMov();
    talk(1);
    /* HD: the message status comes from the message manager */
    if (fopMsgM_getStatus() == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
        switch (mCurrMsgNo) {
        case 0x9C8:
            dComIfGs_onEventBit(0x0510);
            break;
        case 0x9D1:
        case 0x9D2:
        case 0x9D3:
            dComIfGs_onEventBit(0x3708);
            break;
        case 0x9D7:
            dComIfGs_onEventBit(0x3701);
            break;
        case 0x9DA:
            dComIfGs_onEventBit(0x3704);
            dComIfGs_onEventBit(0x0510);
            break;
        case 0x9DD:
            dComIfGs_onEventBit(0x3704);
            break;
        }
        mItemNo = 0xFF;
        mbTalk = false;
        setStt(mPrevStt);
        mTalkEndTimer = (s16)cLib_getRndValue(15, 30);
        endEvent();
    }
    return res;
}
VERIFY(0x021EAA68, &daNpc_Aj1_c::talk_1);

/* 021EAC18 */
BOOL daNpc_Aj1_c::wait_action1(void*) {
    WWHD_FUNC(0x021EAC18, BOOL, this, (void*)nullptr);
    switch ((u32)(s32)mActState) {
    case 0:
        setStt(1);
        mActState = mActState + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch ((u32)(s32)mStt) {
        case 1:
            mbSetEyePos = FARwai();
            break;
        case 2:
            mbSetEyePos = call_1();
            break;
        case 3:
            mbSetEyePos = talk_1();
            break;
        }
        lookBack();
        break;
    }
    return TRUE;
}
VERIFY(0x021EAC18, &daNpc_Aj1_c::wait_action1);

/* sead::SafeString operator== (HD inline: assureTermination, then strcmp bounded by 0x40001) */
static inline bool SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(a) + 4) + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(a) + 4) + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(b) + 4) + 0x14), b);
    u32 s1 = gabi::load<u32>(gabi::ea(a));
    u32 s2 = gabi::load<u32>(gabi::ea(b));
    if (s1 == s2)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c1 = gabi::load<u8>(s1 + i);
        u8 c2 = gabi::load<u8>(s2 + i);
        if (c1 != c2)
            return false;
        if (c1 == 0)
            return true;
    }
    return false;
}

/* 021EACEC */
BOOL daNpc_Aj1_c::wait_action2(void*) {
    WWHD_FUNC(0x021EACEC, BOOL, this, (void*)nullptr);
    switch ((u32)(s32)mActState) {
    case 0: {
        setStt(4);
        /* in the stage "Ojhous2" (Sturgeon's house), type 1 starts the "angry" event until event 0x508 */
        gabi::Local<SafeString> house;
        house->mStringTop = 0x1001626C; /* "Ojhous2" */
        house->__vtbl = SAFESTRING_VTBL;
        gabi::Local<SafeString> stage;
        stage->__vtbl = SAFESTRING_VTBL;
        stage->mStringTop = dComIfGp_ea() + 0x5134; /* dComIfGp_getStartStageName() */
        bool start = false;
        if (SafeString_eq(house, stage) && mType == 1) {
            start = true;
        }
        if (start && !dComIfGs_isEventBit(0x0508)) {
            mEvtState = 3;
        }
        mActState = mActState + 1;
        break;
    }
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch ((u32)(s32)mStt) {
        case 3:
            mbSetEyePos = talk_1();
            break;
        case 4:
            mbSetEyePos = wait_1();
            break;
        }
        lookBack();
        break;
    }
    return TRUE;
}
VERIFY(0x021EACEC, &daNpc_Aj1_c::wait_action2);

/* 021EAEA8 daNpc_Aj1_HIO_c::daNpc_Aj1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Aj1_HIO_c* daNpc_Aj1_HIO_c_ct(daNpc_Aj1_HIO_c* i_this) {
    WWHD_FUNC(0x021EAEA8, daNpc_Aj1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Aj1_HIO_c*)operator_new(0x3C);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10016070;
    memcpy_g(&i_this->mPrm, gabi::at<u8>(0x101BB5EC), 0x30); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->m04 = -1;
    i_this->m08 = -1;
    return i_this;
}
VERIFY(0x021EAEA8, daNpc_Aj1_HIO_c_ct);

/* 021EAF14: static initialisation of the translation unit */
static void __sinit_d_a_npc_aj1_cpp() {
    WWHD_FUNC(0x021EAF14, void);
    sinit_header_statics(0x104659D4, 0x101BB61C);
    daNpc_Aj1_HIO_c_ct(&l_HIO()); /* static daNpc_Aj1_HIO_c l_HIO */
}
VERIFY(0x021EAF14, __sinit_d_a_npc_aj1_cpp);

/* 021EAFB4: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x021EAFB4, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x021EAFB4, SafeString_dt);

/* 021EAFC8: daNpc_Aj1_c deleting destructor (compiler-generated, HD virtual destructor; the
 * btpAnm and smoke callback members have no destructor calls) */
static void daNpc_Aj1_c_dt(daNpc_Aj1_c* i_this, s32 flags) {
    WWHD_FUNC(0x021EAFC8, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x10016050);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x10016060);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021EAFC8, daNpc_Aj1_c_dt);

/* 021EB064: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x021EB064, void, (SafeString*)nullptr);
}
VERIFY(0x021EB064, SafeString_assureTerminationImpl);
