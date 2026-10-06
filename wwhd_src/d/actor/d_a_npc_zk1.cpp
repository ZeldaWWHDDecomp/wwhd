/**
 * d_a_npc_zk1.cpp (WWHD)
 * NPC - Zuko (Rito postman)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_zk1.cpp) has only "Nonmatching" stubs here, so the functions are written
 * from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 */
#include "d/actor/d_a_npc_zk1.h"

#define SAFESTRING_VTBL 0x10023964 /* this TU's sead::SafeString vtable */
#define ZK1_VTBL 0x10023AB0        /* daNpc_Zk1_c vtable (HD: merged with fopNpc_npc_c's) */

/* ---- local bindings (SHARED-CANDIDATE; the same as in d_a_npc_ba1.cpp / d_a_npc_km1.cpp) ---- */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
/* 025B7D90 dSv_player_collect_c::isSymbol (collect at save + 0xD4) [kf1] */
static inline BOOL dComIfGs_isSymbol(u8 i) { return gabi::call<BOOL>(0x025B7D90, gabi::load<u32>(0x101F84DC) + 0xD4, i); }
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
/* resources by id; the key is the actor's own archive name (mArcName) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* 021E1E78 cLib_getRndValue<int>(base, range) (out-of-line copy in another TU) */
static inline s32 cLib_getRndValue(s32 base, s32 range) { return gabi::call<s32>(0x021E1E78, base, range); }
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                                     s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
/* 0259E2D4 dNpc_JntCtrl_c::lookAtTarget_2(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16 yrot, s16 vel, bool headOnly) */
static inline void dNpc_JntCtrl_lookAtTarget_2(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259E2D4, j, outY, target, eye, yrot, vel, headOnly);
}
/* 0259D8BC dNpc_chkAttn(actor, cXyz pos (pointer to a copy), f32, f32, f32, bool) */
static inline BOOL dNpc_chkAttn(fopAc_ac_c* a, cXyz* pos, f32 r0, f32 r1, f32 h, u32 flag) {
    return gabi::call<BOOL>(0x0259D8BC, a, pos, r0, r1, h, flag);
}
static inline u32 fopMsgM_getStatus() { return gabi::call<u32>(0x025F795C, gabi::load<u32>(0x101F4B5C)); }
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
static inline u32 dBgS_GetMtrlSndId_l(dBgS* bgs, void* poly) { return gabi::call<u32>(0x024EECAC, bgs, poly); }
static inline void* daNpc_gndPoly(fopNpc_npc_l* a) { return gabi::at<u8>(gabi::ea(&a->mObjAcch) + 0xD4 + 0x14); }
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, morf, arc, n, ids, p6, p7);
}
static inline void* dDemo_object_getActor(u32 obj, u8 id) { return gabi::call<void*>(0x02526E70, obj, id); }
static inline J3DAnmTexPattern* dDemo_actor_getP_BtpData(void* ac, const char* arc) { return gabi::call<J3DAnmTexPattern*>(0x02527828, ac, arc); }
/* 025D54C4 fopAcM_SearchByID(id, fopAc_ac_c** out) (out of line) */
static inline BOOL fopAcM_SearchByID_o(u32 id, be<u32>* out) { return gabi::call<BOOL>(0x025D54C4, id, out); }

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
enum : u32 { PMF_wait_action1 = 0x10023948 };

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
/* daNpc_Zk1_HIO_c, HD: vtable at 0 */
struct daNpc_Zk1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mJnt[9];      /* dNpc_JntCtrl_c::setParam arguments */
        /* 0x12 */ be<s16> mMaxTurnStep; /* lookBack */
        /* 0x14 */ be<f32> mAttPosOffsetY;
        /* 0x18 */ be<u8> mDebug;        /* debug colours in _draw */
        /* 0x19 */ u8 _19[3];
    };
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ hio_prm_c mPrmTbl;
};
WWHD_SIZE(daNpc_Zk1_HIO_c, 0x28);
static daNpc_Zk1_HIO_c& l_HIO() { return *gabi::at<daNpc_Zk1_HIO_c>(0x10468ACC); }

/* 022FF9AC */
void daNpc_Zk1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x022FF9AC, void, this, i_node, i_model);
    /* static cXyz a_eye_pos_off(14.0f, 18.0f, 0.0f): guard 0x10468B10, object 0x10468B04 */
    cXyz* a_eye_pos_off = gabi::at<cXyz>(0x10468B04);
    if (gabi::load<u32>(0x10468B10) == 0) {
        gabi::store<u32>(0x10468B10, 1);
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->x = 14.0f;
        a_eye_pos_off->y = 18.0f;
    }
    u32 jntNo = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jntNo), stk);
    mHeadPos.x = stk->m[0][3];
    mHeadPos.y = stk->m[1][3];
    mHeadPos.z = stk->m[2][3];
    PSMTXMultVec(stk, a_eye_pos_off, &mEyePos);
    mDoMtx_XrotM(stk, m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[0][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), stk);
}
VERIFY(0x022FF9AC, &daNpc_Zk1_c::_nodeCB_Head);

/* 022FFB24 */
static BOOL nodeCB_Head(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x022FFB24, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        daNpc_Zk1_c* i_this = gabi::at<daNpc_Zk1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (i_this != nullptr) {
            i_this->_nodeCB_Head(i_node, model);
        }
    }
    return TRUE;
}
VERIFY(0x022FFB24, nodeCB_Head);

/* 022FFB6C */
void daNpc_Zk1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x022FFB6C, void, this, i_node, i_model);
    u32 jntNo = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jntNo), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), stk);
}
VERIFY(0x022FFB6C, &daNpc_Zk1_c::_nodeCB_BackBone);

/* 022FFC90 */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x022FFC90, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C));
        daNpc_Zk1_c* i_this = gabi::at<daNpc_Zk1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (i_this != nullptr) {
            i_this->_nodeCB_BackBone(i_node, model);
        }
    }
    return TRUE;
}
VERIFY(0x022FFC90, nodeCB_BackBone);

/* 022FFCD8 (unnamed by the matcher) */
s32 daNpc_Zk1_c::btpResID(int) {
    WWHD_FUNC(0x022FFCD8, s32, this, 0);
    /* a_btp_resID_tbl (.data 0x100239D4): one entry, HD reads [0] */
    return gabi::load<s32>(0x100239D4);
}
VERIFY(0x022FFCD8, &daNpc_Zk1_c::btpResID);

/* 022FFCE4 */
bool daNpc_Zk1_c::setBtp(s32 i_btpNum, u32 i_modify) {
    WWHD_FUNC(0x022FFCE4, bool, this, i_btpNum, i_modify);
    J3DModel* model = mpMorf->getModel();
    if (i_btpNum < 0) {
        return false;
    }
    J3DAnmTexPattern* btp = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(mArcName, btpResID(i_btpNum));
    if (btp == nullptr) /* JUT_ASSERT(402, a_btp != NULL) */
        JUT_ASSERT_fail(STR(0x100239DC), 0x192, STR(0x100239EC));
    mBtpNum = (s8)i_btpNum;
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(model), btp, 1, 0 /* EMode_NONE */, 1.0f, 0, -1, i_modify, 0) != 0;
}
VERIFY(0x022FFCE4, &daNpc_Zk1_c::setBtp);

/* 022FFDD0 */
u32 daNpc_Zk1_c::init_texPttrnAnm(s32 i_btpNum, u32 i_modify) {
    WWHD_FUNC(0x022FFDD0, u32, this, i_btpNum, i_modify);
    return gabi::call<u32>(0x022FFCE4, this, i_btpNum, i_modify); /* setBtp(i_btpNum, i_modify) */
}
VERIFY(0x022FFDD0, &daNpc_Zk1_c::init_texPttrnAnm);

/* 022FFDD4 */
BOOL daNpc_Zk1_c::bodyCreateHeap() {
    WWHD_FUNC(0x022FFDD4, BOOL, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 4 /* bdl */);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(1477, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x10023A04), 0x5C5, STR(0x10023A14));
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr) {
        /* delete mpMorf (HD: virtual deleting destructor, vtable at +0, slot +0xC) */
        mDoExt_McaMorf* morf = mpMorf;
        if (morf != nullptr) {
            u32 vt = gabi::load<u32>(gabi::ea(morf));
            gabi::call_ptr(gabi::load<u32>(vt + 0xC), morf, 3);
        }
        mpMorf = nullptr;
        return FALSE;
    }
    if (!init_texPttrnAnm(0, 0)) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_hed_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100239FC) /* "head" */);
    if (m_hed_jnt_num < 0) /* JUT_ASSERT(1499, m_hed_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x10023A04), 0x5DB, STR(0x10023A28));
    m_bbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10023A3C) /* "backbone" */);
    if (m_bbone_jnt_num < 0) /* JUT_ASSERT(1501, m_bbone_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x10023A04), 0x5DD, STR(0x10023A48));
    /* getJointNodePointer(jnt)->setCallBack(...): a joint index out of range uses the table base */
    {
        J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
        u32 i = (u16)(s32)m_hed_jnt_num;
        u32 n = gabi::load<u32>(gabi::ea(md) + 4);
        u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
        if (i < n)
            joint += i * 0x1C;
        gabi::store<u32>(joint + 8, 0x022FFB24 /* nodeCB_Head */);
    }
    {
        J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
        u32 i = (u16)(s32)m_bbone_jnt_num;
        u32 n = gabi::load<u32>(gabi::ea(md) + 4);
        u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
        if (i < n)
            joint += i * 0x1C;
        gabi::store<u32>(joint + 8, 0x022FFC90 /* nodeCB_BackBone */);
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    return TRUE;
}
VERIFY(0x022FFDD4, &daNpc_Zk1_c::bodyCreateHeap);

/* 02300094 */
BOOL daNpc_Zk1_c::CreateHeap() {
    WWHD_FUNC(0x02300094, BOOL, this);
    if (!bodyCreateHeap()) {
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 80.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}
VERIFY(0x02300094, &daNpc_Zk1_c::CreateHeap);

/* 02300124 (tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02300124, BOOL, i_this);
    return ((daNpc_Zk1_c*)i_this)->CreateHeap();
}
VERIFY(0x02300124, CheckCreateHeap);

/* 02300128 */
bool daNpc_Zk1_c::decideType(int i_param) {
    WWHD_FUNC(0x02300128, bool, this, i_param); /* the parameter is unused */
    if (mType > 0) {
        return true;
    }
    mType = 1;
    mSpecificType = 0;
    /* HD: the archive name is copied into the actor ("Zk", .rodata 0x10023A68) */
    u8 c0 = gabi::load<u8>(0x10023A68), c1 = gabi::load<u8>(0x10023A69), c2 = gabi::load<u8>(0x10023A6A);
    gabi::store<u8>(gabi::ea(mArcName), c0);
    gabi::store<u8>(gabi::ea(mArcName) + 1, c1);
    gabi::store<u8>(gabi::ea(mArcName) + 2, c2);
    bool ret = false;
    if (mType != -1 && mSpecificType != -1) {
        ret = true;
    }
    return ret;
}
VERIFY(0x02300128, &daNpc_Zk1_c::decideType);

/* 0230018C */
BOOL daNpc_Zk1_c::set_action(ProcFunc_l* i_action, void* i_arg) {
    WWHD_FUNC(0x0230018C, BOOL, this, i_action, i_arg);
    ProcFunc_l* cur = &mCurrProcFunc;
    s16 newI = i_action->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_action->d;
        newF = i_action->f;
        if (!((u16)cur->d != (u16)newD || cur->f != newF))
            return TRUE;
    } else {
        newF = i_action->f;
        newD = i_action->d;
    }
    if (cur->i != 0) {
        mActState = 9;
        pmf_call(this, cur, i_arg);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mActState = 0;
    pmf_call(this, cur, i_arg);
    return TRUE;
}
VERIFY(0x0230018C, &daNpc_Zk1_c::set_action);

/* 023002B8 */
bool daNpc_Zk1_c::init_ZK1_0() {
    WWHD_FUNC(0x023002B8, bool, this);
    if (!dComIfGs_isSymbol(1)) {
        return false;
    }
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, PMF_wait_action1);
    set_action(pmf, nullptr);
    m907 = dComIfGs_isEventBit(0x1802) == 1;
    m908 = dComIfGs_isEventBit(0x1C01) == 1;
    return true;
}
VERIFY(0x023002B8, &daNpc_Zk1_c::init_ZK1_0);

/* 02300388 */
void daNpc_Zk1_c::play_btp_anm() {
    WWHD_FUNC(0x02300388, void, this);
    J3DAnmTexPattern* btp = gabi::at<J3DAnmTexPattern>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10));
    u8 maxFrame = (u8)J3DAnm_getFrameMax(btp);
    if (mBtpNum != 0 || cLib_calcTimer(&mBlinkTimer) == 0) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if (frame >= maxFrame) {
            if (mBtpNum == 0) {
                mBlinkTimer = (s16)cLib_getRndValue(60, 90);
                maxFrame = 0;
            }
            mBlinkFrame = maxFrame;
        }
    }
}
VERIFY(0x02300388, &daNpc_Zk1_c::play_btp_anm);

/* 02300428 */
void daNpc_Zk1_c::play_animation() {
    WWHD_FUNC(0x02300428, void, this);
    u32 mtrlSndId = 0;
    play_btp_anm();
    if (gabi::load<u32>(gabi::ea(&mObjAcch) + 0x28) & 0x20 /* mObjAcch.ChkGroundHit() */) {
        mtrlSndId = dBgS_GetMtrlSndId_l(dComIfG_Bgsp(), daNpc_gndPoly(this));
    }
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    mbMorfAnimStopped = (s8)gabi::call<BOOL>(0x025E535C, mpMorf.get(), &eyePos, mtrlSndId, reverb); /* mpMorf->play() */
    if (mpMorf->getFrame() < mPrevMorfFrame) {
        mbMorfAnimStopped = 1;
    }
    mPrevMorfFrame = mpMorf->getFrame();
}
VERIFY(0x02300428, &daNpc_Zk1_c::play_animation);

/* 023004CC */
void daNpc_Zk1_c::setAttention(u32 i_setEyePos) {
    WWHD_FUNC(0x023004CC, void, this, i_setEyePos);
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    f32 x = current.pos.x;
    f32 y = current.pos.y + l_HIO().mPrmTbl.mAttPosOffsetY;
    attPos->x = x;
    attPos->y = y;
    attPos->z = current.pos.z;
    if (mbSetEyePos == 0 && i_setEyePos == 0) {
        return;
    }
    eyePos.z = mEyePos.z;
    eyePos.y = mEyePos.y;
    eyePos.x = mEyePos.x;
}
VERIFY(0x023004CC, &daNpc_Zk1_c::setAttention);

/* 02300520 */
void daNpc_Zk1_c::setMtx(u32 i_param) {
    WWHD_FUNC(0x02300520, void, this, i_param);
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mAngle.x, mAngle.y, mAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    setAttention(i_param);
}
VERIFY(0x02300520, &daNpc_Zk1_c::setMtx);

/* 02300628 */
bool daNpc_Zk1_c::createInit() {
    WWHD_FUNC(0x02300628, bool, this);
    /* l_evn_tbl (.data 0x101C6FB0): one event name */
    const char* name = gabi::at<const char>(gabi::load<u32>(0x101C6FB0));
    mEventIdTable[0] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    mEventCut.setActorInfo2(STR(0x10023A6C) /* "Zk1" */, (fopNpc_npc_c*)(void*)this);
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xA9); /* attention_info.distances[SPEAK] */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xA9); /* attention_info.distances[TALK] */
    mAnmNum = 4;
    gravity = 0.0f;
    bool ok;
    switch (mSpecificType) {
    case 0:
        ok = init_ZK1_0();
        break;
    default:
        ok = false;
        break;
    }
    if (!ok) {
        return false;
    }
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    play_animation();
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    mpMorf->setMorf(0.0f);
    setMtx(1);
    return true;
}
VERIFY(0x02300628, &daNpc_Zk1_c::createInit);

/* 023007BC */
cPhs_State daNpc_Zk1_c::_create() {
    WWHD_FUNC(0x023007BC, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Zk1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this);    /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
            __vtbl = ZK1_VTBL;
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
    /* a_heap_size_tbl (.data 0x101C6FD4)[mType] */
    if (!fopAcM_entrySolidHeap(this, 0x02300124 /* CheckCreateHeap */, gabi::load<u32>(0x101C6FD4 + mType * 4))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -70.0f, -20.0f, -70.0f, 70.0f, 240.0f, 70.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x023007BC, &daNpc_Zk1_c::_create);

/* 02300908 */
static cPhs_State daNpc_Zk1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02300908, cPhs_State, i_this);
    return ((daNpc_Zk1_c*)i_this)->_create();
}
VERIFY(0x02300908, daNpc_Zk1_Create);

/* 0230090C */
BOOL daNpc_Zk1_c::_delete() {
    WWHD_FUNC(0x0230090C, BOOL, this);
    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x0230090C, &daNpc_Zk1_c::_delete);

/* 02300960 */
static BOOL daNpc_Zk1_Delete(daNpc_Zk1_c* i_this) {
    WWHD_FUNC(0x02300960, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x02300960, daNpc_Zk1_Delete);

/* 02300964 */
void daNpc_Zk1_c::checkOrder() {
    WWHD_FUNC(0x02300964, void, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIdTable[mEventIdx]) && mEvtCond >= 3) {
            mEvtCond = 0;
            mMesgAnimeTagInfo = 0xFF;
            mAnmAtr = 0xFF;
        }
    } else if (command == 1 /* checkCommandTalk() */) {
        if (mEvtCond == 1 || mEvtCond == 2) {
            mEvtCond = 0;
            m911 = 1;
        }
    }
}
VERIFY(0x02300964, &daNpc_Zk1_c::checkOrder);

/* 02300A3C */
u8 daNpc_Zk1_c::demo() {
    WWHD_FUNC(0x02300A3C, u8, this);
    u8 id = demoActorID;
    if (id == 0) {
        if (mbInDemo != 0) {
            mbInDemo = 0;
        }
        return mbInDemo;
    }
    if (mbInDemo == 0) {
        m_jnt.mAngles[0][1] = 0;
        m_jnt.mAngles[1][0] = 0;
        mbInDemo = 1;
        m90D = 0;
        m_jnt.mAngles[0][0] = 0;
        id = demoActorID;
        m_jnt.mAngles[1][1] = 0;
    }
    void* actor = nullptr;
    if (id != 0 && id <= 0x20) {
        /* dComIfGp_demo_getActor(id) */
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x100239BC), 0x23A, STR(0x100239AC));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        actor = dDemo_object_getActor(obj, id);
    }
    J3DAnmTexPattern* cur = gabi::at<J3DAnmTexPattern>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10));
    if (cur != nullptr) {
        u8 maxFrame = (u8)J3DAnm_getFrameMax(cur);
        u8 frame = (u8)(mBlinkFrame + 1);
        if (frame < maxFrame) {
            mBlinkFrame = frame;
        } else {
            mBlinkFrame = maxFrame;
        }
    }
    if (actor != nullptr) {
        J3DAnmTexPattern* btp = dDemo_actor_getP_BtpData(actor, mArcName);
        if (btp != nullptr) {
            mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(mpMorf->getModel()), btp, 1, 0, 1.0f, 0, -1, 1, 0);
            mBlinkFrame = 0;
            mBtpNum = 1;
        }
    }
    dDemo_setDemoData(this, 0x6A, mpMorf, mArcName, 0, nullptr, 0, 0);
    return mbInDemo;
}
VERIFY(0x02300A3C, &daNpc_Zk1_c::demo);

/* 02300BF8 */
s32 daNpc_Zk1_c::isEventEntry() {
    WWHD_FUNC(0x02300BF8, s32, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x02300BF8, &daNpc_Zk1_c::isEventEntry);

/* 02300C38 */
void daNpc_Zk1_c::endEvent() {
    WWHD_FUNC(0x02300C38, void, this);
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
    mMesgAnimeTagInfo = 0xFF;
}
VERIFY(0x02300C38, &daNpc_Zk1_c::endEvent);

/* 02300C7C (unnamed by the matcher) */
void daNpc_Zk1_c::privateCut(int i_staffIdx) {
    WWHD_FUNC(0x02300C7C, void, this, i_staffIdx);
    if (i_staffIdx == -1) {
        return;
    }
    /* cut_name_tbl (.data 0x101C6FDC): one cut; HD: event_actionInit/event_action are empty */
    s8 actIdx = dComIfGp_evmng_getMyActIdx(i_staffIdx, 0x101C6FDC, 1, TRUE, 0);
    mActIdx = actIdx;
    if (actIdx != -1) {
        dComIfGp_evmng_getIsAddvance(i_staffIdx);
    }
    dComIfGp_evmng_cutEnd(i_staffIdx);
}
VERIFY(0x02300C7C, &daNpc_Zk1_c::privateCut);

/* 02300D14 */
void daNpc_Zk1_c::event_proc(int i_staffIdx) {
    WWHD_FUNC(0x02300D14, void, this, i_staffIdx);
    s16 ev = mEventIdTable[mEventIdx];
    if (dComIfGp_evmng_endCheck(ev)) {
        endEvent();
        return;
    }
    if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
}
VERIFY(0x02300D14, &daNpc_Zk1_c::event_proc);

/* 02300DB8 (the matcher calls it cLib_getRndValue<i> from d_a_npc_aj1) */
fopAc_ac_c* daNpc_Zk1_c::searchByID(fpc_ProcID i_id, be<s32>* o_res) {
    WWHD_FUNC(0x02300DB8, fopAc_ac_c*, this, i_id, o_res);
    gabi::Local<be<u32>> actor;
    *actor = 0;
    *o_res = 0;
    if (!fopAcM_SearchByID_o(i_id, actor)) {
        *o_res = 1;
    }
    return gabi::at<fopAc_ac_c>(*actor);
}
VERIFY(0x02300DB8, &daNpc_Zk1_c::searchByID);

static inline void copy_words(u32 dst, u32 src) {
    u32 x = gabi::load<u32>(src), y = gabi::load<u32>(src + 4), z = gabi::load<u32>(src + 8);
    gabi::store<u32>(dst, x);
    gabi::store<u32>(dst + 4, y);
    gabi::store<u32>(dst + 8, z);
}

/* 02300E0C */
void daNpc_Zk1_c::lookBack() {
    WWHD_FUNC(0x02300E0C, void, this);
    gabi::Local<cXyz> vec1;
    vec1->x = 0.0f;
    vec1->y = 0.0f;
    vec1->z = 0.0f;
    mJointHeadY = m_jnt.mAngles[0][1];
    f32 srcX = current.pos.x;
    s16 targetY = current.angle.y;
    mActorAngleY = targetY;
    u8 headOnly = mHeadOnlyFollow;
    f32 srcZ = current.pos.z;
    f32 srcY = eyePos.y;
    mJointBackboneY = m_jnt.mAngles[1][1];
    cXyz* dstPos = nullptr;
    switch ((u32)(s32)mLookBackState) {
    case 0:
        break;
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        copy_words(gabi::ea(&mLookTarget), gabi::ea(eye.get()));
        copy_words(gabi::ea(vec1.get()), gabi::ea(eye.get()));
        dstPos = vec1;
        break;
    }
    case 2:
        copy_words(gabi::ea(vec1.get()), gabi::ea(&mLookTarget));
        dstPos = vec1;
        break;
    case 3:
        targetY = mTargetYRot;
        break;
    case 4: {
        gabi::Local<be<s32>> res;
        fopAc_ac_c* partner = searchByID(mPartnerID, res);
        if (partner != nullptr && *res == 0) {
            copy_words(gabi::ea(&mLookTarget), gabi::ea(&partner->current.pos));
            f32 x = mLookTarget.x;
            f32 ey = partner->eyePos.y;
            vec1->x = x;
            f32 z = mLookTarget.z;
            vec1->y = ey;
            vec1->z = z;
            dstPos = vec1;
            mLookTarget.y = ey;
        }
        break;
    }
    default:
        break;
    }
    gabi::Local<cXyz> vec2; /* passed by value: a copy */
    vec2->x = srcX;
    vec2->z = srcZ;
    vec2->y = srcY;
    dNpc_JntCtrl_lookAtTarget_2(&m_jnt, &current.angle.y, dstPos, vec2, targetY, l_HIO().mPrmTbl.mMaxTurnStep, headOnly);
}
VERIFY(0x02300E0C, &daNpc_Zk1_c::lookBack);

/* 023010A0 */
void daNpc_Zk1_c::eventOrder() {
    WWHD_FUNC(0x023010A0, void, this);
    s8 cond = mEvtCond;
    if (cond == 1 || cond == 2) {
        s8 c = mEvtCond;
        eventInfo_onCondition(this, 1); /* dEvtCnd_CANTALK_e */
        if (c == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (cond >= 3) {
        s16 idx = (s16)(cond - 3);
        mEventIdx = idx;
        fopAcM_orderOtherEventId(this, mEventIdTable[idx], 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x023010A0, &daNpc_Zk1_c::eventOrder);

/* the common tail of _execute's three paths */
static inline void zk1_setAngle(daNpc_Zk1_c* i_this) {
    s16 z = i_this->current.angle.z, y = i_this->current.angle.y, x = i_this->current.angle.x;
    i_this->mAngle.x = x;
    i_this->mAngle.y = y;
    i_this->mAngle.z = z;
    if (i_this->m90D == 0) {
        i_this->shape_angle.z = i_this->current.angle.z;
        i_this->shape_angle.y = i_this->current.angle.y;
        i_this->shape_angle.x = i_this->current.angle.x;
    }
}

/* 02301110 */
BOOL daNpc_Zk1_c::_execute() {
    WWHD_FUNC(0x02301110, BOOL, this);
    if (mbRanExecute == 0) {
        mInitAngle.y = current.angle.y;
        mInitAngle.z = current.angle.z;
        copy_words(gabi::ea(&mInitPos), gabi::ea(&current.pos));
        mInitAngle.x = current.angle.x;
        mbRanExecute = 1;
    }
    daNpc_Zk1_HIO_c::hio_prm_c& prm = l_HIO().mPrmTbl;
    m_jnt.setParam(prm.mJnt[4], prm.mJnt[5], prm.mJnt[6], prm.mJnt[7], prm.mJnt[0], prm.mJnt[1], prm.mJnt[2], prm.mJnt[3],
                   prm.mJnt[8]);
    if (m90C != 0 && demoActorID == 0) {
        return TRUE;
    }
    checkOrder();
    if (demo() == 0) {
        s32 staffIdx = -1;
        bool evt = false;
        if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !eventInfo.checkCommandTalk() */) {
            staffIdx = isEventEntry();
            if (staffIdx >= 0)
                evt = true;
        }
        if (evt || m90A != 0) {
            event_proc(staffIdx);
        } else {
            pmf_call(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        lookBack();
        fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
        play_animation();
        eventOrder();
        zk1_setAngle(this);
    } else {
        m90C = 0;
        eventOrder();
        zk1_setAngle(this);
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    setMtx(0);
    if (mbInDemo == 0) {
        setCollision(80.0f, 220.0f);
    }
    return TRUE;
}
VERIFY(0x02301110, &daNpc_Zk1_c::_execute);

/* 023013E0 */
static BOOL daNpc_Zk1_Execute(daNpc_Zk1_c* i_this) {
    WWHD_FUNC(0x023013E0, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x023013E0, daNpc_Zk1_Execute);

/* static initialisation of a 4-byte function-local constant on first use */
static inline void local_static_init(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(dst), gabi::at<u8>(src), 4); /* 028FEAC0 memcpy */
    }
}

/* 023013E4 */
BOOL daNpc_Zk1_c::_draw() {
    WWHD_FUNC(0x023013E4, BOOL, this);
    J3DModel* model = mpMorf->getModel();
    J3DModelData* model_data = J3DModel_getModelData_l(model);
    if (m90C != 0 || m90E != 0) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, model_data, mBlinkFrame);
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(model_data) + 0x38, 0); /* mBtpAnm.remove(model_data) */
    /* HD: no shadowDraw() */
    dSnap_RegistFig(0x8C, this, 1.0f, 1.0f, 1.0f);
    /* debug leftovers: function-local static colours initialised on first use */
    if (l_HIO().mPrmTbl.mDebug != 0) {
        local_static_init(0x101FDA50, 0x101FEBF4, 0x10023950);
        local_static_init(0x101FDAC0, 0x101FEBF8, 0x10023954);
    }
    return TRUE;
}
VERIFY(0x023013E4, &daNpc_Zk1_c::_draw);

/* 02301518 */
static BOOL daNpc_Zk1_Draw(daNpc_Zk1_c* i_this) {
    WWHD_FUNC(0x02301518, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x02301518, daNpc_Zk1_Draw);

/* 0230151C */
static BOOL daNpc_Zk1_IsDelete(daNpc_Zk1_c*) {
    WWHD_FUNC(0x0230151C, BOOL, (daNpc_Zk1_c*)nullptr);
    return TRUE;
}
VERIFY(0x0230151C, daNpc_Zk1_IsDelete);

/* 02301524 (unnamed by the matcher) */
s32 daNpc_Zk1_c::bckResID(int i_idx) {
    WWHD_FUNC(0x02301524, s32, this, i_idx);
    /* a_bck_resID_tbl (.data 0x10023A8C) */
    return gabi::load<s32>(0x10023A8C + i_idx * 4);
}
VERIFY(0x02301524, &daNpc_Zk1_c::bckResID);

/* 02301538 */
void daNpc_Zk1_c::setAnm_anm(anm_prm_c* i_prm) {
    WWHD_FUNC(0x02301538, void, this, i_prm);
    s8 num = i_prm->mAnmNum;
    if (num < 0 || mAnmNum == num) {
        return;
    }
    s32 resID = bckResID(num);
    s32 loopMode = i_prm->mLoopMode;
    f32 morf = i_prm->mMorf;
    f32 speed = i_prm->mSpeed;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, mArcName);
    mbMorfAnimStopped = 0;
    mAnmNum = i_prm->mAnmNum;
    mPrevMorfFrame = 0.0f;
    m905 = 0;
}
VERIFY(0x02301538, &daNpc_Zk1_c::setAnm_anm);

/* 023015D0 */
void daNpc_Zk1_c::setAnm() {
    WWHD_FUNC(0x023015D0, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101C6FE0); /* [3] */
    init_texPttrnAnm(a_anm_prm_tbl[mStatus].mBtpNum, 1);
    setAnm_anm(&a_anm_prm_tbl[mStatus]);
}
VERIFY(0x023015D0, &daNpc_Zk1_c::setAnm);

/* 02301640 */
void daNpc_Zk1_c::setAnm_ATR() {
    WWHD_FUNC(0x02301640, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101C7010); /* [4] */
    init_texPttrnAnm(a_anm_prm_tbl[mAnmAtr].mBtpNum, 1);
    setAnm_anm(&a_anm_prm_tbl[mAnmAtr]);
}
VERIFY(0x02301640, &daNpc_Zk1_c::setAnm_ATR);

/* 023016A8 */
void daNpc_Zk1_c::chngAnmAtr(u32 i_atr) {
    WWHD_FUNC(0x023016A8, void, this, i_atr);
    if (i_atr == mAnmAtr || i_atr >= 4) {
        return;
    }
    mAnmAtr = (u8)i_atr;
    setAnm_ATR();
}
VERIFY(0x023016A8, &daNpc_Zk1_c::chngAnmAtr);

/* 023016C4 */
void daNpc_Zk1_c::anmAtr(u32 i_msgStatus) {
    WWHD_FUNC(0x023016C4, void, this, i_msgStatus);
    if (i_msgStatus == 6 /* fopMsgStts_MSG_TYPING_e */) {
        if (m921 == 0) {
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo());
            m921 = m921 + 1;
        }
        u8 tag = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgAnimeTagInfo() */
        if (tag != 0xFF && tag != mMesgAnimeTagInfo) {
            gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF); /* dComIfGp_clearMesgAnimeTagInfo() */
            mMesgAnimeTagInfo = tag;
            /* chngAnmTag(): empty */
        }
    } else if (i_msgStatus == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
        m921 = 0;
    }
    /* ctrlAnmAtr(), ctrlAnmTag(): empty */
}
VERIFY(0x023016C4, &daNpc_Zk1_c::anmAtr);

/* 02301794 */
u16 daNpc_Zk1_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x02301794, u16, this, pMsgNo);
    u16 ret = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    switch ((u32)*pMsgNo) {
    case 0x17A2:
        *pMsgNo = 0x17A3;
        break;
    case 0x17A3:
        *pMsgNo = 0x17A4;
        break;
    case 0x17A4:
        *pMsgNo = 0x17A5;
        break;
    case 0x17A5:
        *pMsgNo = 0x17A6;
        break;
    case 0x17A8:
        *pMsgNo = 0x17A9;
        break;
    case 0x17A9:
        *pMsgNo = 0x17AA;
        break;
    case 0x17AA:
        *pMsgNo = 0x17AB;
        break;
    case 0x17AD:
        if (dKy_daynight_check()) {
            *pMsgNo = 0x17AF;
        } else {
            *pMsgNo = 0x17AE;
        }
        break;
    default:
        ret = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return ret;
}
VERIFY(0x02301794, &daNpc_Zk1_c::next_msgStatus);

/* 023018C0 */
u32 daNpc_Zk1_c::getMsg_ZK1_0() {
    WWHD_FUNC(0x023018C0, u32, this);
    if (!dComIfGs_isEventBit(0x1A80)) {
        return dComIfGs_isEventBit(0x1802) ? 0x17A7 : 0x17A2;
    }
    if (m907 == 0) {
        return dComIfGs_isEventBit(0x1802) ? 0x17AC : 0x17A8;
    }
    if (m908 == 0) {
        return dComIfGs_isEventBit(0x1C01) ? 0x17B0 : 0x17AD;
    }
    return dComIfGs_isEventBit(0x1D80) ? 0x17B2 : 0x17B1;
}
VERIFY(0x023018C0, &daNpc_Zk1_c::getMsg_ZK1_0);

/* 023019D8 */
u32 daNpc_Zk1_c::getMsg() {
    WWHD_FUNC(0x023019D8, u32, this);
    u32 ret = 0;
    if (mSpecificType == 0) {
        ret = getMsg_ZK1_0();
    }
    return ret;
}
VERIFY(0x023019D8, &daNpc_Zk1_c::getMsg);

/* 02301A10 */
bool daNpc_Zk1_c::chk_talk() {
    WWHD_FUNC(0x02301A10, bool, this);
    if (dComIfGp_event_chkTalkXY()) {
        if (!dComIfGp_evmng_ChkPresentEnd()) {
            return false;
        }
        mItemNo = dComIfGp_event_getPreItemNo();
        return true;
    }
    mItemNo = 0xFF;
    return true;
}
VERIFY(0x02301A10, &daNpc_Zk1_c::chk_talk);

/* 02301AA8 */
u8 daNpc_Zk1_c::chk_parts_notMov() {
    WWHD_FUNC(0x02301AA8, u8, this);
    /* HD: true when a part moved */
    return !(mJointHeadY == m_jnt.mAngles[0][1] && mJointBackboneY == m_jnt.mAngles[1][1] && mActorAngleY == current.angle.y);
}
VERIFY(0x02301AA8, &daNpc_Zk1_c::chk_parts_notMov);

/* 02301AE8 */
u8 daNpc_Zk1_c::chkAttention() {
    WWHD_FUNC(0x02301AE8, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x02301AE8, &daNpc_Zk1_c::chkAttention);

/* 02301B70 */
void daNpc_Zk1_c::setStt(u32 i_status) {
    WWHD_FUNC(0x02301B70, void, this, i_status);
    s8 old = mStatus;
    mStatus = (s8)i_status;
    switch (i_status) {
    case 1:
        mEvtCond = 0;
        break;
    case 2:
        mEvtCond = 0;
        mMesgAnimeTagInfo = 0xFF;
        mAnmAtr = 0xFF;
        mPrevStatus = old;
        m921 = 0;
        break;
    }
    setAnm();
}
VERIFY(0x02301B70, &daNpc_Zk1_c::setStt);

/* 02301BBC */
BOOL daNpc_Zk1_c::wait_1() {
    WWHD_FUNC(0x02301BBC, BOOL, this);
    s16 diff = 0;
    if (m913 != 0) {
        cLib_addCalcAngleS(&current.angle.y, mInitAngle.y, 4, 0x800, 0x80);
        diff = (s16)(mInitAngle.y - current.angle.y);
    }
    if (m911 != 0) {
        if (chk_talk()) {
            setStt(2);
            mHeadOnlyFollow = 0;
            mLookBackState = 1;
            m913 = 0;
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        }
    } else {
        mLookBackState = 0;
        mEvtCond = 2;
        mHeadOnlyFollow = 1;
        if (diff == 0) {
            if (mbAttention != 0) {
                mTimer = (s16)cLib_getRndValue(15, 30);
            }
            if (cLib_calcTimer(&mTimer) != 0) {
                mLookBackState = 1;
            }
            fopAc_ac_c* player = dComIfGp_getPlayer(0);
            gabi::Local<cXyz> pos; /* passed by value: a copy */
            pos->x = player->current.pos.x;
            pos->y = player->current.pos.y;
            pos->z = player->current.pos.z;
            if (!dNpc_chkAttn(this, pos, 200.0f, 50.0f, 76.0f, mLookBackState == 1)) {
                mLookBackState = 0;
                m913 = 1;
            }
        }
    }
    return TRUE;
}
VERIFY(0x02301BBC, &daNpc_Zk1_c::wait_1);

/* 02301D34 */
u32 daNpc_Zk1_c::talk_1() {
    WWHD_FUNC(0x02301D34, u32, this);
    u32 ret = chk_parts_notMov();
    talk(1);
    /* HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus) */
    if (mbHasMsg != 0 && fopMsgM_getStatus() == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
        u32 msgNo = mCurrMsgNo;
        if (msgNo == 0x17A6 || msgNo == 0x17AB) {
            dComIfGs_onEventBit(0x1802);
        } else if (msgNo < 0x17AE) {
        } else if (msgNo <= 0x17AF) {
            dComIfGs_onEventBit(0x1C01);
        } else if (msgNo == 0x17B1) {
            dComIfGs_onEventBit(0x1D80);
        }
        mItemNo = 0xFF;
        m911 = 0;
        setStt(mPrevStatus);
        mTimer = (s16)cLib_getRndValue(15, 30);
        endEvent();
    }
    return ret;
}
VERIFY(0x02301D34, &daNpc_Zk1_c::talk_1);

/* 02301E80 */
BOOL daNpc_Zk1_c::wait_action1(void*) {
    WWHD_FUNC(0x02301E80, BOOL, this, (void*)nullptr);
    s8 state = mActState;
    if (state == 0) {
        setStt(1);
        m913 = 1;
        mActState = mActState + 1;
        return TRUE;
    }
    if ((u32)(s32)state > 3) {
        return TRUE;
    }
    mbAttention = chkAttention();
    switch ((u32)(s32)mStatus) {
    case 1:
        mbSetEyePos = wait_1();
        break;
    case 2:
        mbSetEyePos = talk_1();
        break;
    }
    return TRUE;
}
VERIFY(0x02301E80, &daNpc_Zk1_c::wait_action1);

/* 02301F38 daNpc_Zk1_HIO_c::daNpc_Zk1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Zk1_HIO_c* daNpc_Zk1_HIO_c_ct(daNpc_Zk1_HIO_c* i_this) {
    WWHD_FUNC(0x02301F38, daNpc_Zk1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Zk1_HIO_c*)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1002399C;
    memcpy_g(&i_this->mPrmTbl, gabi::at<u8>(0x101C7050), 0x1C); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}
VERIFY(0x02301F38, daNpc_Zk1_HIO_c_ct);

/* 02301FA4: static initialisation of the translation unit */
static void __sinit_d_a_npc_zk1_cpp() {
    WWHD_FUNC(0x02301FA4, void);
    sinit_header_statics_z(0x10468AC0, 0x101C706C, 0x10468AF4);
    daNpc_Zk1_HIO_c_ct(&l_HIO()); /* static daNpc_Zk1_HIO_c l_HIO */
}
VERIFY(0x02301FA4, __sinit_d_a_npc_zk1_cpp);

/* 02302044: sead::SafeString deleting destructor (this TU's copy; vtable 0x10023964 slot +0xC) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x02302044, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x02302044, SafeString_dt);

/* 02302058: daNpc_Zk1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Zk1_c_dt(daNpc_Zk1_c* i_this, s32 flags) {
    WWHD_FUNC(0x02302058, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x1002397C);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x1002398C);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02302058, daNpc_Zk1_c_dt);

/* 023020F4: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x023020F4, void, (SafeString*)nullptr);
}
VERIFY(0x023020F4, SafeString_assureTerminationImpl);
