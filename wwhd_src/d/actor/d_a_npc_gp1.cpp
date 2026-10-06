/**
 * d_a_npc_gp1.cpp (WWHD)
 * NPC - Maggie's father (rich)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_gp1.cpp) has only "Nonmatching" stubs for this unit: every function here is
 * written from the WWHD code (cking.rpx) with the GameCube names, and verified against it.
 */
#include "d/actor/d_a_npc_gp1.h"

#define SAFESTRING_VTBL 0x1001A170 /* this TU's sead::SafeString vtable */
#define GP1_VTBL 0x1001A388        /* daNpc_Gp1_c vtable (HD: merged with fopNpc_npc_c's) */

/* ---- local bindings (SHARED-CANDIDATE; the same as in d_a_npc_aj1.cpp / d_a_npc_km1.cpp) ---- */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGs_getEventReg(u16 r) { return dSv_event_getEventReg(dComIfGs_event(), r); }
static inline void dComIfGs_setEventReg(u16 r, u8 v) { dSv_event_setEventReg(dComIfGs_event(), r, v); }
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
/* 02544980 dEvent_manager_c::CancelPresent */
static inline void dComIfGp_evmng_CancelPresent() { gabi::call(0x02544980, dComIfGp_getPEvtManager()); }
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
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
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
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
static inline u32 fopMsgM_getStatus() { return gabi::call<u32>(0x025F795C, gabi::load<u32>(0x101F4B5C)); }
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
/* 0259E6D0 dNpc_PathRun_c::setInf(pathIdx, roomNo, forwards) */
static inline BOOL dNpc_PathRun_setInf(dNpc_PathRun_gp1* p, u8 path, s8 room, u8 fwd) { return gabi::call<BOOL>(0x0259E6D0, p, path, room, fwd); }
/* 0259E778 dNpc_PathRun_c::getPoint(u8): cXyz through a hidden result pointer (r4) */
static inline void dNpc_PathRun_getPoint(dNpc_PathRun_gp1* p, cXyz* o_pnt, u8 idx) { gabi::call(0x0259E778, p, o_pnt, idx); }
static inline BOOL dNpc_PathRun_chkPointPass(dNpc_PathRun_gp1* p, cXyz* pos, u32 dir) { return gabi::call<BOOL>(0x0259E838, p, pos, dir); }
static inline BOOL dNpc_PathRun_nextIdxAuto(dNpc_PathRun_gp1* p) { return gabi::call<BOOL>(0x0259ED58, p); }
/* 0259D6C8 dNpc_chkArasoi() */
static inline BOOL dNpc_chkArasoi() { return gabi::call<BOOL>(0x0259D6C8); }
/* 025DE508 fpcEx_Search(fpcLyIt_JudgeFunc, void*) */
static inline void* fpcEx_Search(u32 fn, void* data) { return gabi::call<void*>(0x025DE508, fn, data); }
/* 025D54C4 fopAcM_SearchByID(fpc_ProcID, fopAc_ac_c**) */
static inline BOOL fopAcM_SearchByID_l(u32 id, be<u32>* o_actor) { return gabi::call<BOOL>(0x025D54C4, id, o_actor); }
/* 0253F124 dEvt_control_c::getPId(void*) */
static inline u32 dEvt_control_getPId(u32 ctl, void* actor) { return gabi::call<u32>(0x0253F124, ctl, actor); }
/* 025D8AB0 fopAcM_fastCreateItem(pos, itemNo, roomNo, angle, scale, speedF, speedY, gravity, itemBitNo, createFunc) */
static inline fopAc_ac_c* fopAcM_fastCreateItem(cXyz* pos, s32 itemNo, s32 roomNo, csXyz* angle, cXyz* scale, f32 speedF, f32 speedY,
                                                f32 gravity, s32 bitNo, u32 createFunc) {
    return gabi::call<fopAc_ac_c*>(0x025D8AB0, pos, itemNo, roomNo, angle, scale, speedF, speedY, gravity, bitNo, createFunc);
}
/* 025B7570 dSv_player_bag_item_c::checkReserveItem(u8) (bag at save+0x96) */
static inline BOOL dComIfGs_checkReserveItem(u8 item) { return gabi::call<BOOL>(0x025B7570, gabi::load<u32>(0x101F84DC) + 0x96, item); }
/* g_Counter.mCounter0 */
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
enum : u32 { PMF_wait_action1 = 0x1001A158 };

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
struct daNpc_Gp1_HIO_c {
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
        /* 0x1C */ be<f32> mForceTlkDist;
        /* 0x20 */ be<s16> mTurnScale;
        /* 0x22 */ be<s16> mTurnMaxStep;
        /* 0x24 */ be<f32> mAnmSpdRate;
        /* 0x28 */ be<f32> mWalkSpd;
        /* 0x2C */ be<f32> mWalkAccel;
        /* 0x30 */ be<f32> mPassDist;
    };
    /* 0x00 */ be<u32> __vtbl; /* HD: vtable first */
    /* 0x04 */ be<s8> m04;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> m08;
    /* 0x0C */ hio_prm_c mPrm;
};
WWHD_SIZE(daNpc_Gp1_HIO_c, 0x40);
static daNpc_Gp1_HIO_c& l_HIO() { return *gabi::at<daNpc_Gp1_HIO_c>(0x104670A8); }

/* l_check_wrk / l_check_inf[20]: actors found by searchActor_Bm */
#define L_CHECK_WRK 0x10467098
#define L_CHECK_INF 0x10467104

/* 02238460 */
static void* searchActor_Bm(void* i_actor, void*) {
    WWHD_FUNC(0x02238460, void*, i_actor, (void*)nullptr);
    if (gabi::load<s32>(L_CHECK_WRK) < 0x14 && fopAc_IsActor(i_actor) && i_actor != nullptr && fpcM_GetName(i_actor) == 0x149 /* NPC_BM? */) {
        s32 n = gabi::load<s32>(L_CHECK_WRK);
        gabi::store<s32>(L_CHECK_WRK, n + 1);
        gabi::store<u32>(L_CHECK_INF + n * 4, gabi::ea(i_actor));
    }
    return nullptr;
}
VERIFY(0x02238460, searchActor_Bm);

/* 022384E0 */
void daNpc_Gp1_c::nodeGp1Control(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x022384E0, void, this, i_node, i_model);
    /* static cXyz a_eye_pos_off(24.0f, -24.0f, 0.0f): guard 0x10467154, object 0x104670F8 */
    cXyz* a_eye_pos_off = gabi::at<cXyz>(0x104670F8);
    if (gabi::load<u32>(0x10467154) == 0) {
        a_eye_pos_off->z = 0.0f;
        gabi::store<u32>(0x10467154, 1);
        a_eye_pos_off->x = 24.0f;
        a_eye_pos_off->y = -24.0f;
    }
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    if (jnt_no == (u32)(s32)m_hed_jnt_num) {
        mDoMtx_YrotM(stk, -m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(stk, -m_jnt.mAngles[0][0]);
        PSMTXMultVec(stk, a_eye_pos_off, &mEyePos);
    }
    if (jnt_no == (u32)(s32)m_bbone_jnt_num) {
        mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(stk, m_jnt.mAngles[1][0]);
    }
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}
VERIFY(0x022384E0, &daNpc_Gp1_c::nodeGp1Control);

/* 02238680 */
static BOOL nodeCallBack_Gp1(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x02238680, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        u32 model = gabi::load<u32>(0x104B462C); /* j3dSys.getModel() */
        u32 user = gabi::load<u32>(model + 0xB8);
        if (user != 0)
            gabi::call(0x022384E0, user, i_node, model);
    }
    return TRUE;
}
VERIFY(0x02238680, nodeCallBack_Gp1);

/* 022386C8: creates the morf; HD returns the model data (CreateHeap walks its joints) */
J3DModelData* daNpc_Gp1_c::create_Anm() {
    WWHD_FUNC(0x022386C8, J3DModelData*, this);
    const char* arc = STR(0x1001A210); /* "Gp" */
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(arc, 6);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2169, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x1001A21C), 0x879, STR(0x1001A22C));
    void* bck = dComIfG_getObjectIDRes(arc, 5);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, (J3DAnmTransform*)bck, 2, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr) {
        return nullptr;
    }
    if (mpMorf->getModel() == nullptr) {
        mpMorf = nullptr;
        return nullptr;
    }
    m_hed_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001A214) /* "head" */);
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x1001A21C), 0x88E, STR(0x1001A240));
    m_bbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001A254) /* "backbone" */);
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x1001A21C), 0x891, STR(0x1001A260));
    m_hnd_L_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001A208) /* "handL" */);
    if (m_hnd_L_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x1001A21C), 0x894, STR(0x1001A278));
    return a_mdl_dat;
}
VERIFY(0x022386C8, &daNpc_Gp1_c::create_Anm);

/* 022388E4 (unnamed by the matcher; one btp: the index is not used) */
int daNpc_Gp1_c::btpNum_toResID(int) {
    WWHD_FUNC(0x022388E4, int, this, 0);
    return gabi::load<s32>(0x1001A290);
}
VERIFY(0x022388E4, &daNpc_Gp1_c::btpNum_toResID);

/* 022388F0 */
u32 daNpc_Gp1_c::setBtp(u32 i_bModify, int i_btpNum) {
    WWHD_FUNC(0x022388F0, u32, this, i_bModify, i_btpNum);
    J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
    int id = btpNum_toResID(i_btpNum);
    m_hed_tex_pttrn = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(STR(0x1001A294) /* "Gp" */, id);
    if (m_hed_tex_pttrn.get() == nullptr) /* JUT_ASSERT(438, m_hed_tex_pttrn != NULL) */
        JUT_ASSERT_fail(STR(0x1001A298), 0x1B6, STR(0x1001A2A8));
    bool ok = mDoExt_btpAnm_init(mBtpAnm, md, m_hed_tex_pttrn, TRUE, 2, 1.0f, 0, -1, i_bModify, 0) == 1;
    if (ok) {
        mBlinkTimer = 0;
        mBtpFrame = 0;
    }
    return ok;
}
VERIFY(0x022388F0, &daNpc_Gp1_c::setBtp);

/* 022389DC */
u32 daNpc_Gp1_c::iniTexPttrnAnm(u32 i_bModify) {
    WWHD_FUNC(0x022389DC, u32, this, i_bModify);
    return setBtp(i_bModify, mBtpNum);
}
VERIFY(0x022389DC, &daNpc_Gp1_c::iniTexPttrnAnm);

/* 022389E8 */
BOOL daNpc_Gp1_c::CreateHeap() {
    WWHD_FUNC(0x022389E8, BOOL, this);
    J3DModelData* a_mdl_dat = create_Anm();
    if (a_mdl_dat == nullptr) {
        return FALSE;
    }
    mBtpNum = 0;
    if (!iniTexPttrnAnm(false)) {
        mpMorf = nullptr;
        return FALSE;
    }
    for (u16 i = 0; i < J3DModelData_getJointNum(a_mdl_dat); i++) {
        if (i == (u32)(s32)m_hed_jnt_num || i == (u32)(s32)m_bbone_jnt_num) {
            /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Gp1) */
            J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
            u32 n = gabi::load<u32>(gabi::ea(md) + 4);
            u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
            if ((u32)i < n)
                joint += i * 0x1C;
            gabi::store<u32>(joint + 8, 0x02238680);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    mAcchCir.SetWall(30.0f, 70.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}
VERIFY(0x022389E8, &daNpc_Gp1_c::CreateHeap);

/* 02238B44 (tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02238B44, BOOL, i_this);
    return static_cast<daNpc_Gp1_c*>(i_this)->CreateHeap();
}
VERIFY(0x02238B44, CheckCreateHeap);

/* 02238B48 (unnamed by the matcher; one type: the parameter is not used) */
BOOL daNpc_Gp1_c::charDecide(int) {
    WWHD_FUNC(0x02238B48, BOOL, this, 0);
    mType = 0;
    m983 = 0;
    return TRUE;
}
VERIFY(0x02238B48, &daNpc_Gp1_c::charDecide);

/* 02238B5C */
BOOL daNpc_Gp1_c::set_action(ProcFunc_l* i_newProcFunc, void* i_argsP) {
    WWHD_FUNC(0x02238B5C, BOOL, this, i_newProcFunc, i_argsP);
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
VERIFY(0x02238B5C, &daNpc_Gp1_c::set_action);

/* 02238C88 */
bool daNpc_Gp1_c::init_GP1_0() {
    WWHD_FUNC(0x02238C88, bool, this);
    if (dComIfGs_isEventBit(0x2D01)) {
        gabi::Local<ProcFunc_l> pmf;
        pmf_load(pmf, PMF_wait_action1);
        set_action(pmf, nullptr);
        return true;
    }
    return false;
}
VERIFY(0x02238C88, &daNpc_Gp1_c::init_GP1_0);

/* 02238D0C */
void daNpc_Gp1_c::plyTexPttrnAnm() {
    WWHD_FUNC(0x02238D0C, void, this);
    if (mBtpNum == 0 && cLib_calcTimer(&mBlinkTimer) != 0) {
        return;
    }
    u8 frame = (u8)(mBtpFrame + 1);
    mBtpFrame = frame;
    if ((s32)frame < J3DAnm_getFrameMax(m_hed_tex_pttrn)) {
        return;
    }
    if (mBtpNum != 0) {
        mBtpFrame = (u8)J3DAnm_getFrameMax(m_hed_tex_pttrn);
    } else {
        s16 t = (s16)gabi::ftoi(cM_rndF(60.0f) + 30.0f);
        mBtpFrame = 0;
        mBlinkTimer = t;
    }
}
VERIFY(0x02238D0C, &daNpc_Gp1_c::plyTexPttrnAnm);

/* 02238DE0 */
u32 daNpc_Gp1_c::setAnm_tex(s8 i_btpNum) {
    WWHD_FUNC(0x02238DE0, u32, this, i_btpNum);
    if (mBtpNum == i_btpNum) {
        return (u32)gabi::ea(this); /* original: r3 still this */
    }
    mBtpNum = i_btpNum;
    return iniTexPttrnAnm(true);
}
VERIFY(0x02238DE0, &daNpc_Gp1_c::setAnm_tex);

/* 02238E00 (unnamed by the matcher) */
int daNpc_Gp1_c::anmNum_toResID(int i_anmNum) {
    WWHD_FUNC(0x02238E00, int, this, i_anmNum);
    return gabi::load<s32>(0x1001A2CC + i_anmNum * 4);
}
VERIFY(0x02238E00, &daNpc_Gp1_c::anmNum_toResID);

/* 02238E14 */
BOOL daNpc_Gp1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    WWHD_FUNC(0x02238E14, BOOL, this, i_anmPrmP);
    s8 bck = i_anmPrmP->bckNum;
    if (mBckNum == bck) {
        return TRUE;
    }
    mBckNum = bck;
    int resID = anmNum_toResID(bck);
    s32 loopMode = i_anmPrmP->loopMode;
    f32 speed = i_anmPrmP->speed;
    f32 morf = i_anmPrmP->morf;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, STR(0x1001A2F4) /* "Gp" */);
    if (mBckNum == 0) {
        mWaitTimer = (s16)gabi::ftoi(cM_rndF(120.0f) + 180.0f);
    }
    mLoopCnt = 0;
    mPrevMorfFrame = 0.0f;
    mbMorfAnimStopped = 0;
    return TRUE;
}
VERIFY(0x02238E14, &daNpc_Gp1_c::setAnm_anm);

/* 02238EE0 */
void daNpc_Gp1_c::setAnm_NUM(int i_anmNum, int i_setBtp) {
    WWHD_FUNC(0x02238EE0, void, this, i_anmNum, i_setBtp);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BE508); /* [5] */
    if (i_setBtp) {
        setAnm_tex(a_anm_prm_tbl[i_anmNum].btpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[i_anmNum]);
}
VERIFY(0x02238EE0, &daNpc_Gp1_c::setAnm_NUM);

/* 02238F4C */
void daNpc_Gp1_c::ctrl_WAITanm() {
    WWHD_FUNC(0x02238F4C, void, this);
    switch ((u32)(s32)mBckNum) {
    case 0:
        if (cLib_calcTimer(&mWaitTimer) == 0) {
            setAnm_NUM(4, 1);
        }
        break;
    case 4:
        if (mbMorfAnimStopped) {
            setAnm_NUM(0, 1);
            mpMorf->setMorf(0.0f);
        }
        break;
    }
}
VERIFY(0x02238F4C, &daNpc_Gp1_c::ctrl_WAITanm);

/* 02238FFC */
void daNpc_Gp1_c::setAttention(u32 i_setEyePos) {
    WWHD_FUNC(0x02238FFC, void, this, i_setEyePos);
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    f32 y = current.pos.y + l_HIO().mPrm.mAttPosOffsetY;
    attPos->z = current.pos.z;
    attPos->y = y;
    attPos->x = current.pos.x;
    if (!mbSetEyePos && !i_setEyePos) {
        return;
    }
    f32 ey = mEyePos.y + mEyeOffsetY;
    eyePos.x = mEyePos.x;
    eyePos.z = mEyePos.z;
    eyePos.y = ey;
}
VERIFY(0x02238FFC, &daNpc_Gp1_c::setAttention);

/* 02239058 */
void daNpc_Gp1_c::setMtx(u32 i_setEyePos) {
    WWHD_FUNC(0x02239058, void, this, i_setEyePos);
    if (mbDemo == 0) {
        plyTexPttrnAnm();
        mbMorfAnimStopped = (s8)mpMorf->play(&eyePos, 0, 0);
        if (mpMorf->getFrame() < mPrevMorfFrame) {
            mbMorfAnimStopped = 1;
        }
        mPrevMorfFrame = mpMorf->getFrame();
        ctrl_WAITanm();
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(mAngle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    setAttention(i_setEyePos);
}
VERIFY(0x02239058, &daNpc_Gp1_c::setMtx);

/* 022391CC */
bool daNpc_Gp1_c::createInit() {
    WWHD_FUNC(0x022391CC, bool, this);
    /* l_evn_tbl (.data 0x101BE4FC): three event names */
    for (int i = 0; i < 3; i++) {
        const char* name = gabi::at<const char>(gabi::load<u32>(0x101BE4FC + i * 4));
        mEventIDTbl[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }
    mLookPos.copy(current.pos);
    u32 path = (mParameters >> 16) & 0xFF;
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xAB); /* attention_info.distances[TALK] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xAB); /* attention_info.distances[SPEAK] */
    gravity = -4.5f;
    u8 weight = 0xFF;
    if (path != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, path, current.roomNo, 1);
        if (mPathRun.mPath.get() == nullptr) {
            return false;
        }
        actor_status &= ~0x80u; /* OffStatus(fopAcStts_NOCULLEXEC_e) */
        weight = 0xD9;
    }
    mEventCut.setActorInfo2(STR(0x1001A2FC) /* "Gp1" */, (fopNpc_npc_c*)(void*)this);
    mBckNum = 8;
    if (mType != 0) {
        return false;
    }
    if (!init_GP1_0()) {
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
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}
VERIFY(0x022391CC, &daNpc_Gp1_c::createInit);

/* 0223936C */
cPhs_State daNpc_Gp1_c::_create() {
    WWHD_FUNC(0x0223936C, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Gp1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this);       /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = GP1_VTBL;
            gabi::call(0x025E7820, mBtpAnm);    /* mDoExt_btpAnm::mDoExt_btpAnm */
            gabi::call(0x0259F740, &mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, STR(0x1001A310) /* "Gp" */);
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    if (!charDecide(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    if (!fopAcM_entrySolidHeap(this, 0x02238B44 /* CheckCreateHeap */, gabi::load<u32>(0x101BE598))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -70.0f, -20.0f, -70.0f, 50.0f, 240.0f, 50.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x0223936C, &daNpc_Gp1_c::_create);

/* 022394A8 */
static cPhs_State daNpc_Gp1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022394A8, cPhs_State, i_this);
    return ((daNpc_Gp1_c*)i_this)->_create();
}
VERIFY(0x022394A8, daNpc_Gp1_Create);

/* 022394AC */
BOOL daNpc_Gp1_c::_delete() {
    WWHD_FUNC(0x022394AC, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x1001A313) /* "Gp" */);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x022394AC, &daNpc_Gp1_c::_delete);

/* 02239504 */
static BOOL daNpc_Gp1_Delete(daNpc_Gp1_c* i_this) {
    WWHD_FUNC(0x02239504, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x02239504, daNpc_Gp1_Delete);

/* 02239508 (named cLib_calcTimer<s> / d_a_npc_ac1 by the matcher) */
fopAc_ac_c* daNpc_Gp1_c::searchByID(u32 i_id) {
    WWHD_FUNC(0x02239508, fopAc_ac_c*, this, i_id);
    gabi::Local<be<u32>> actor;
    *actor = 0;
    fopAcM_SearchByID_l(i_id, actor);
    return gabi::at<fopAc_ac_c>(*actor);
}
VERIFY(0x02239508, &daNpc_Gp1_c::searchByID);

/* 0223953C */
u8 daNpc_Gp1_c::partner_srch_sub(u32 i_func) {
    WWHD_FUNC(0x0223953C, u8, this, i_func);
    mPartnerID = 0xFFFFFFFF;
    gabi::store<s32>(L_CHECK_WRK, 0);
    for (int i = 0; i < 0x14; i++) {
        gabi::store<u32>(L_CHECK_INF + i * 4, 0);
    }
    fpcEx_Search(i_func, this);
    if (gabi::load<s32>(L_CHECK_WRK) == 0) {
        return false;
    }
    u32 a = gabi::load<u32>(L_CHECK_INF);
    mPartnerID = a != 0 ? gabi::load<u32>(a + 4) /* fopAcM_GetID */ : 0xFFFFFFFF;
    return true;
}
VERIFY(0x0223953C, &daNpc_Gp1_c::partner_srch_sub);

/* 022395E8 */
void daNpc_Gp1_c::partner_srch() {
    WWHD_FUNC(0x022395E8, void, this);
    if (mActState != 1) {
        return;
    }
    if (mStt == 3) {
        if (!partner_srch_sub(0x02238460 /* searchActor_Bm */)) {
            return;
        }
        fopAc_ac_c* partner = searchByID(mPartnerID);
        if (partner != nullptr) {
            current.angle.y = cLib_targetAngleY(&current.pos, &partner->current.pos);
        }
    }
    mActState = mActState + 1;
}
VERIFY(0x022395E8, &daNpc_Gp1_c::partner_srch);

/* 0223967C */
void daNpc_Gp1_c::checkOrder() {
    WWHD_FUNC(0x0223967C, void, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIDTbl[mEventIndex])) {
            mEvtState = 0;
        }
    } else if (command == 1 /* checkCommandTalk() */) {
        if (mEvtState == 1 || mEvtState == 2) {
            mEvtState = 0;
            mbTalk = true;
        }
    }
}
VERIFY(0x0223967C, &daNpc_Gp1_c::checkOrder);

/* 02239738 */
u8 daNpc_Gp1_c::demo() {
    WWHD_FUNC(0x02239738, u8, this);
    if (demoActorID == 0) {
        if (mbDemo) {
            mbDemo = false;
        }
        return mbDemo;
    }
    u8 id = demoActorID;
    mbDemo = true;
    /* dComIfGp_demo_getActor(demoActorID) (HD inline with a range check) */
    void* demo_actor_p = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x1001A1EC), 0x23A, STR(0x1001A1B8));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor_p = gabi::call<void*>(0x02526E70, obj, id);
    }
    if (m_hed_tex_pttrn.get() != nullptr) {
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(m_hed_tex_pttrn)) {
            mBtpFrame = (u8)J3DAnm_getFrameMax(m_hed_tex_pttrn);
        }
    }
    if (demo_actor_p) {
        void* demo_btp_p = gabi::call<void*>(0x02527828, demo_actor_p, STR(0x1001A316) /* "Gp" */); /* getP_BtpData */
        if (demo_btp_p) {
            m_hed_tex_pttrn = (J3DAnmTexPattern*)demo_btp_p;
            if (mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(mpMorf->getModel()), demo_btp_p, TRUE, 2, 1.0f, 0, -1, 1, 0)) {
                mBtpFrame = 0;
                mBtpNum = 1;
            }
        }
    }
    gabi::call(0x02527028, this, 0x6A, mpMorf.get(), STR(0x1001A316), 0, 0, 0, 0); /* dDemo_setDemoData */
    return mbDemo;
}
VERIFY(0x02239738, &daNpc_Gp1_c::demo);

/* 022398EC */
s32 daNpc_Gp1_c::isEventEntry() {
    WWHD_FUNC(0x022398EC, s32, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x022398EC, &daNpc_Gp1_c::isEventEntry);

/* 0223992C */
BOOL daNpc_Gp1_c::setAnm() {
    WWHD_FUNC(0x0223992C, BOOL, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BE59C); /* [6] */
    if (a_anm_prm_tbl[mStt].btpNum >= 0) {
        setAnm_tex(a_anm_prm_tbl[mStt].btpNum);
    }
    if (a_anm_prm_tbl[mStt].bckNum >= 0) {
        setAnm_anm(&a_anm_prm_tbl[mStt]);
    }
    return TRUE;
}
VERIFY(0x0223992C, &daNpc_Gp1_c::setAnm);

/* 022399B0 */
void daNpc_Gp1_c::setStt(s8 i_stt) {
    WWHD_FUNC(0x022399B0, void, this, i_stt);
    s8 prev = mStt;
    mStt = i_stt;
    mTalkTimer = 0;
    switch ((u32)(s32)i_stt) {
    case 1:
        mWaitTimer2 = 0x5A;
        break;
    case 2:
        m_jnt.mbTrn = 1; /* setTrn() */
        mAnmAtr = 0xFF;
        mLookMode = 1;
        mPrevStt = prev;
        return;
    case 4: {
        mLookMode = 0;
        mMoveState = 1;
        mWaitTimer2 = (s16)gabi::ftoi(cM_rndF(120.0f) + 180.0f);
        mWalkSpd = l_HIO().mPrm.mWalkSpd;
        mWalkAccel = l_HIO().mPrm.mWalkAccel;
        mPassDist = l_HIO().mPrm.mPassDist;
        break;
    }
    case 5:
        mLookMode = 0;
        break;
    }
    setAnm();
}
VERIFY(0x022399B0, &daNpc_Gp1_c::setStt);

/* 02239ACC */
void daNpc_Gp1_c::endEvent() {
    WWHD_FUNC(0x02239ACC, void, this);
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
}
VERIFY(0x02239ACC, &daNpc_Gp1_c::endEvent);

/* 02239B0C: the event's partner is the Bm found by partner_srch */
void daNpc_Gp1_c::eInit_INI_KAERE_KAERE_() {
    WWHD_FUNC(0x02239B0C, void, this);
    fopAc_ac_c* partner = searchByID(mPartnerID);
    if (partner != nullptr) {
        u32 evt = dComIfGp_ea() + PLAY_EVTCTRL;
        gabi::store<u32>(evt + 0xD0, dEvt_control_getPId(evt, partner)); /* dComIfGp_event_setPt2 */
    }
}
VERIFY(0x02239B0C, &daNpc_Gp1_c::eInit_INI_KAERE_KAERE_);

/* 02239B60 (unnamed by the matcher) */
void daNpc_Gp1_c::eInit_END_KAERE_KAERE_() {
    WWHD_FUNC(0x02239B60, void, this);
    current.angle.y = mHomeAngle.y;
}
VERIFY(0x02239B60, &daNpc_Gp1_c::eInit_END_KAERE_KAERE_);

/* 02239B6C */
void daNpc_Gp1_c::event_actionInit(int i_staffIdx) {
    WWHD_FUNC(0x02239B6C, void, this, i_staffIdx);
    be<s32>* act = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, STR(0x1001A31C) /* "ActNo" */, 3);
    if (act == nullptr) {
        return;
    }
    s8 actNo = (s8)(s32)*act;
    mActNo = actNo;
    mbSetEyePos = 0;
    switch ((u32)(s32)actNo) {
    case 0:
        eInit_INI_KAERE_KAERE_();
        break;
    case 1:
        eInit_END_KAERE_KAERE_();
        break;
    }
}
VERIFY(0x02239B6C, &daNpc_Gp1_c::event_actionInit);

/* 02239C24 (unnamed by the matcher) */
BOOL daNpc_Gp1_c::event_action() {
    WWHD_FUNC(0x02239C24, BOOL, this);
    return TRUE;
}
VERIFY(0x02239C24, &daNpc_Gp1_c::event_action);

/* 02239C2C */
void daNpc_Gp1_c::privateCut(int i_staffIdx) {
    WWHD_FUNC(0x02239C2C, void, this, i_staffIdx);
    /* a_cut_tbl (.data 0x101BE5FC): one cut */
    if (i_staffIdx == -1) {
        return;
    }
    s8 idx = dComIfGp_evmng_getMyActIdx(i_staffIdx, 0x101BE5FC, 1, TRUE, 0);
    mActionIndex = idx;
    if (idx == -1) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(i_staffIdx)) {
        if (mActionIndex == 0) {
            event_actionInit(i_staffIdx);
        }
    }
    BOOL end = TRUE;
    if (mActionIndex == 0) {
        end = event_action();
    }
    if (end) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}
VERIFY(0x02239C2C, &daNpc_Gp1_c::privateCut);

/* 02239D00 */
void daNpc_Gp1_c::lookBack() {
    WWHD_FUNC(0x02239D00, void, this);
    mJointBackboneY = m_jnt.mAngles[1][1];
    s16 target_y = current.angle.y;
    gabi::Local<cXyz> look_pos;
    look_pos->y = 0.0f;
    look_pos->x = 0.0f;
    f32 srcX = current.pos.x;
    look_pos->z = 0.0f;
    mActorAngleY = target_y;
    s8 mode = mLookMode;
    f32 srcY = eyePos.y;
    u8 head_only = mbHeadOnly;
    cXyz* look_pos_p = nullptr;
    f32 srcZ = current.pos.z;
    mJointHeadY = m_jnt.mAngles[0][1];
    switch ((u32)(s32)mode) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        look_pos->copy(*eye);
        srcZ = current.pos.z;
        srcX = current.pos.x;
        srcY = eyePos.y;
        look_pos_p = look_pos;
        break;
    }
    case 2:
        look_pos->copy(mLookPos);
        srcZ = current.pos.z;
        srcX = current.pos.x;
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
VERIFY(0x02239D00, &daNpc_Gp1_c::lookBack);

/* 02239F40 */
void daNpc_Gp1_c::event_proc(int i_staffIdx) {
    WWHD_FUNC(0x02239F40, void, this, i_staffIdx);
    if (dComIfGp_evmng_endCheck(mEventIDTbl[mEventIndex])) {
        switch ((u32)(s32)mEventIndex) {
        case 0:
            dComIfGs_onEventBit(0x1808);
            actor_status |= 0x80u; /* OnStatus(fopAcStts_NOCULLEXEC_e) */
            setStt(1);
            mWaitTimer2 = 0;
            mHairTimer = (s16)((g_Counter0() & 3) + 2);
            break;
        case 1:
            dComIfGs_onEventBit(0x1420);
            mbAfterEvent = 1;
            mEvtState = 1;
            break;
        case 2:
            setStt(1);
            break;
        }
        endEvent();
    } else {
        if (!mEventCut.cutProc()) {
            privateCut(i_staffIdx);
        }
        lookBack();
    }
}
VERIFY(0x02239F40, &daNpc_Gp1_c::event_proc);

/* 0223A0E0 */
void daNpc_Gp1_c::gp_clcMovSpd() {
    WWHD_FUNC(0x0223A0E0, void, this);
    s16 angle = cLib_targetAngleY(&current.pos, &mPathPos);
    cLib_addCalcAngleS(&current.angle.y, angle, l_HIO().mPrm.mTurnScale, l_HIO().mPrm.mTurnMaxStep, 0);
    cLib_chaseF(&speedF, mWalkSpd, mWalkAccel);
}
VERIFY(0x0223A0E0, &daNpc_Gp1_c::gp_clcMovSpd);

/* 0223A144 (unnamed by the matcher): 0 moving, 1 point reached, 2 end of the path */
int daNpc_Gp1_c::gp_movPass() {
    WWHD_FUNC(0x0223A144, int, this);
    dPath* path = mPathRun.mPath;
    if (path != nullptr && (gabi::load<u8>(gabi::ea(path) + 5) & 1) /* dPath_ChkClose(path) */) {
        gabi::Local<cXyz> pos;
        pos->x = current.pos.x;
        pos->y = current.pos.y;
        pos->z = current.pos.z;
        if (dNpc_PathRun_chkPointPass(&mPathRun, pos, mPathRun.mbDir != 0)) {
            dNpc_PathRun_nextIdxAuto(&mPathRun);
            return 1;
        }
        return 0;
    }
    gabi::Local<cXyz> diff;
    cXyz_mi(&mPathPos, diff, &current.pos);
    gabi::Local<cXyz> xz;
    xz->x = diff->x;
    xz->y = 0.0f;
    xz->z = diff->z;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    int ret = 0;
    if (!(dist > mPassDist)) {
        ret = 1;
        if (mPathRun.mPath.get() != nullptr && !dNpc_PathRun_nextIdxAuto(&mPathRun)) {
            ret = 2;
        }
    }
    return ret;
}
VERIFY(0x0223A144, &daNpc_Gp1_c::gp_movPass);

/* 0223A244 */
void daNpc_Gp1_c::gp_nMove() {
    WWHD_FUNC(0x0223A244, void, this);
    if (mbPathEnd) {
        mbNoMove = 1;
        return;
    }
    if (mMoveState != 1) {
        return;
    }
    gp_clcMovSpd();
    f32 rate = speedF * l_HIO().mPrm.mAnmSpdRate;
    if (rate < 0.5f) {
        rate = 0.5f;
    }
    mpMorf->setPlaySpeed(rate);
    switch (gp_movPass()) {
    case 1:
        mbPathEnd = 1;
        break;
    case 2:
        mbPathEnd = 1;
        mMoveState = 0;
        break;
    }
}
VERIFY(0x0223A244, &daNpc_Gp1_c::gp_nMove);

/* 0223A330 (unnamed by the matcher) */
void daNpc_Gp1_c::eventOrder() {
    WWHD_FUNC(0x0223A330, void, this);
    s8 state = mEvtState;
    if (state == 1 || state == 2) {
        eventInfo_onCondition(this, 0x21); /* dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e */
        if (mEvtState == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (state >= 3) {
        mEventIndex = (s16)(state - 3);
        fopAcM_orderOtherEventId(this, mEventIDTbl[mEventIndex], 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x0223A330, &daNpc_Gp1_c::eventOrder);

/* 0223A3A0 */
BOOL daNpc_Gp1_c::_execute() {
    WWHD_FUNC(0x0223A3A0, BOOL, this);
    if (!mbHomeSet) {
        mHomeAngle.y = current.angle.y;
        mHomePos.copy(current.pos);
        mHomeAngle.x = current.angle.x;
        mHomeAngle.z = current.angle.z;
        mbHomeSet = true;
    }
    daNpc_Gp1_HIO_c::hio_prm_c& prm = l_HIO().mPrm;
    m_jnt.setParam(prm.mMaxBackBoneX, prm.mMaxBackBoneY, prm.mMinBackBoneX, prm.mMinBackBoneY, prm.mMaxHeadX,
                   prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (m968 && demoActorID == 0) {
        return TRUE;
    }
    mbNoMove = 0;
    m968 = 0;
    partner_srch();
    checkOrder();
    if (!demo()) {
        s32 staff_id;
        if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !eventInfo.checkCommandTalk() */ &&
            (staff_id = isEventEntry()) >= 0) {
            event_proc(staff_id);
        } else {
            pmf_call(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        if (!mbNoMove) {
            gp_nMove();
            fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        }
        if (!mbNoShapeAngle) {
            mAngle.x = current.angle.x;
            mAngle.y = current.angle.y;
            mAngle.z = current.angle.z;
            shape_angle.x = current.angle.x;
            shape_angle.y = current.angle.y;
            shape_angle.z = current.angle.z;
        }
    }
    eventOrder();
    setMtx(false);
    if (!mbDemo) {
        setCollision(70.0f, 230.0f);
    }
    return TRUE;
}
VERIFY(0x0223A3A0, &daNpc_Gp1_c::_execute);

/* 0223A5A8 */
static BOOL daNpc_Gp1_Execute(daNpc_Gp1_c* i_this) {
    WWHD_FUNC(0x0223A5A8, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x0223A5A8, daNpc_Gp1_Execute);

/* static initialisation of a 4-byte function-local constant on first use */
static inline void local_static_init(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(dst), gabi::at<u8>(src), 4); /* 028FEAC0 memcpy */
    }
}

/* 0223A5AC */
BOOL daNpc_Gp1_c::_draw() {
    WWHD_FUNC(0x0223A5AC, BOOL, this);
    J3DModel* morf_model_p = mpMorf->getModel();
    J3DModelData* morf_model_info_p = J3DModel_getModelData_l(morf_model_p);
    if (m968 || mbNoDraw) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, morf_model_info_p, mBtpFrame);
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(morf_model_info_p) + 0x38, 0); /* mBtpAnm.remove() */
    /* debug leftovers: function-local static colors initialised on first use (the third twice) */
    if (l_HIO().mPrm.mDebugDraw) {
        local_static_init(0x101FDA50, 0x101FEBF4, 0x1001A160);
        local_static_init(0x101FDAC0, 0x101FEBF8, 0x1001A164);
        if (gabi::load<u32>(0x101FDA48) == 0) {
            gabi::store<u32>(0x101FDA48, 1);
            memcpy_g(gabi::at<u8>(0x101FEBEC), gabi::at<u8>(0x1001A168), 4);
            local_static_init(0x101FDA48, 0x101FEBEC, 0x1001A168);
        }
    }
    /* HD: no shadowDraw() */
    dSnap_RegistFig(0x5B /* DSNAP_TYPE_NPC_GP1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x0223A5AC, &daNpc_Gp1_c::_draw);

/* 0223A73C */
static BOOL daNpc_Gp1_Draw(daNpc_Gp1_c* i_this) {
    WWHD_FUNC(0x0223A73C, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x0223A73C, daNpc_Gp1_Draw);

/* 0223A740 */
static BOOL daNpc_Gp1_IsDelete(daNpc_Gp1_c*) {
    WWHD_FUNC(0x0223A740, BOOL, (daNpc_Gp1_c*)nullptr);
    return TRUE;
}
VERIFY(0x0223A740, daNpc_Gp1_IsDelete);

/* 0223A748 */
void daNpc_Gp1_c::setAnm_ATR(int i_setBtp) {
    WWHD_FUNC(0x0223A748, void, this, i_setBtp);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BE600); /* [7] */
    if (i_setBtp) {
        setAnm_tex(a_anm_prm_tbl[mAnmAtr].btpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[mAnmAtr]);
}
VERIFY(0x0223A748, &daNpc_Gp1_c::setAnm_ATR);

/* 0223A7B8 */
void daNpc_Gp1_c::chg_anmAtr(u8 i_atr) {
    WWHD_FUNC(0x0223A7B8, void, this, i_atr);
    if (i_atr >= 7 || i_atr == mAnmAtr) {
        return;
    }
    mAnmAtr = i_atr;
    setAnm_ATR(1);
}
VERIFY(0x0223A7B8, &daNpc_Gp1_c::chg_anmAtr);

/* create_rupee's stack frame: the angle table follows the position (a negative g_Counter makes
 * the table index negative, and the original then reads pos.y / pos.z) */
struct gp1_rupee_frame_l {
    /* 0x00 */ cXyz pos;
    /* 0x0C */ be<f32> a_angle_tbl[3]; /* {-30, 0, 30} (.rodata 0x1001A360) */
};

/* 0223A7DC: throws mRupeeNum rupees from the left hand */
BOOL daNpc_Gp1_c::create_rupee() {
    WWHD_FUNC(0x0223A7DC, BOOL, this);
    gabi::Local<csXyz> angle;
    csXyz_ct(angle, 0, 0, 0);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(mpMorf->getModel(), m_hnd_L_jnt_num), stk);
    gabi::Local<gp1_rupee_frame_l> frm;
    frm->pos.x = stk->m[0][3];
    frm->pos.y = stk->m[1][3];
    frm->pos.z = stk->m[2][3];
    s32 cnt = (s32)g_Counter0();
    for (int i = 0; i < mRupeeNum; i++) {
        u32 tbl = gabi::ea(&frm->a_angle_tbl[0]);
        gabi::store<u32>(tbl, gabi::load<u32>(0x1001A360));
        gabi::store<u32>(tbl + 4, gabi::load<u32>(0x1001A364));
        gabi::store<u32>(tbl + 8, gabi::load<u32>(0x1001A368));
        f32 r = cM_rndF(30.0f);
        f32 base = gabi::load<f32>(tbl + (cnt % 3) * 4);
        s16 deg = (s16)gabi::ftoi((r - 15.0f) + base);
        f32 a = (f32)deg * 182.04445f; /* cM_deg2s */
        angle->y = (s16)(current.angle.y + (s16)gabi::ftoi(a));
        f32 spd = cM_rndFX(3.0f) + 10.0f;
        f32 spdY = cM_rndFX(6.0f) + 33.0f;
        fopAc_ac_c* item = fopAcM_fastCreateItem(&frm->pos, 4 /* dItem_BLUE_RUPEE_e */, current.roomNo, nullptr, nullptr, spd, spdY, -2.0f, -1, 0);
        if (item == nullptr) {
            break;
        }
        item->actor_status |= 0x4000u;
        item->scale.x = 0.2f;
        item->scale.y = 0.2f;
        item->scale.z = 0.2f;
        item->current.angle.x = angle->x;
        item->current.angle.y = angle->y;
        item->current.angle.z = angle->z;
        item->shape_angle.x = angle->x;
        item->shape_angle.y = angle->y;
        item->shape_angle.z = angle->z;
        cnt++;
    }
    return TRUE;
}
VERIFY(0x0223A7DC, &daNpc_Gp1_c::create_rupee);

/* 0223AB20 */
void daNpc_Gp1_c::control_anmAtr() {
    WWHD_FUNC(0x0223AB20, void, this);
    switch (mAnmAtr) {
    case 1:
        if (!mbMorfAnimStopped) {
            return;
        }
        break;
    case 5:
        if (!mbMorfAnimStopped) {
            if (mpMorf->checkFrame(80.0f)) {
                create_rupee();
                mDoAud_seStart(0x69E9, nullptr, 0, dComIfGp_getReverb(current.roomNo));
                mRupeeNum = 0;
            }
            return;
        }
        break;
    case 6:
        if (!mbMorfAnimStopped) {
            return;
        }
        mLoopCnt = mLoopCnt + 1;
        if (mLoopCnt < 2) {
            return;
        }
        break;
    default:
        return;
    }
    mAnmAtr = 0;
    setAnm_NUM(0, 1);
}
VERIFY(0x0223AB20, &daNpc_Gp1_c::control_anmAtr);

/* 0223AC3C */
void daNpc_Gp1_c::anmAtr(u16 i_msgStatus) {
    WWHD_FUNC(0x0223AC3C, void, this, i_msgStatus);
    switch (i_msgStatus) {
    case 6: {
        if (mAtrSet == 0) {
            mAnmAtr = 0xFF;
            chg_anmAtr(gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925) /* dComIfGp_getMesgAnimeAttrInfo() */);
            mAtrSet = mAtrSet + 1;
        }
        u8 tag = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgAnimeTagInfo() */
        gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF);   /* HD: cleared unconditionally */
        if (tag != 0xFF && tag != mMesgAnimeTag) {
            mMesgAnimeTag = tag;
            /* chg_anmTag(): empty */
        }
        break;
    }
    case 14:
        mAtrSet = 0;
        break;
    }
    control_anmAtr();
}
VERIFY(0x0223AC3C, &daNpc_Gp1_c::anmAtr);

/* 0223AD04 */
u8 daNpc_Gp1_c::chk_talk() {
    WWHD_FUNC(0x0223AD04, u8, this);
    u8 ret = true;
    mItemNo = 0xFF;
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mItemNo = dComIfGp_event_getPreItemNo();
            mItemNo = dComIfGp_event_getPreItemNo(); /* HD: twice */
        } else {
            ret = false;
            mItemNo = dComIfGp_event_getPreItemNo();
        }
    }
    return ret;
}
VERIFY(0x0223AD04, &daNpc_Gp1_c::chk_talk);

/* 0223AD98: true when the head, backbone and body did not turn this frame */
u8 daNpc_Gp1_c::chk_partsNotMove() {
    WWHD_FUNC(0x0223AD98, u8, this);
    return mJointHeadY == m_jnt.mAngles[0][1] && mJointBackboneY == m_jnt.mAngles[1][1] && mActorAngleY == current.angle.y;
}
VERIFY(0x0223AD98, &daNpc_Gp1_c::chk_partsNotMove);

/* 0223ADD8: the player is near and at about the same height */
u8 daNpc_Gp1_c::chk_forceTlkArea() {
    WWHD_FUNC(0x0223ADD8, u8, this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&dComIfGp_getPlayer(0)->current.pos, diff, &current.pos);
    gabi::Local<cXyz> xz;
    xz->x = diff->x;
    xz->z = diff->z;
    xz->y = 0.0f;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    f32 dy = dComIfGp_getPlayer(0)->current.pos.y - current.pos.y;
    return -1.0f < dy && !(dy > 100.0f) && dist < l_HIO().mPrm.mForceTlkDist;
}
VERIFY(0x0223ADD8, &daNpc_Gp1_c::chk_forceTlkArea);

/* 0223AEAC */
u16 daNpc_Gp1_c::next_msgStatus(be<u32>* o_msgNoP) {
    WWHD_FUNC(0x0223AEAC, u16, this, o_msgNoP);
    u16 msg_status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    u32 msgNo = *o_msgNoP;
    u8 num = gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0xBC); /* save info: the number of skull necklaces? */
    switch (msgNo) {
    case 0x1E15:
        *o_msgNoP = 0x1E16;
        break;
    case 0x1E16:
        *o_msgNoP = 0x1E17;
        break;
    case 0x1E17:
        *o_msgNoP = 0x1E18;
        break;
    case 0x1E19:
        *o_msgNoP = 0x1E1A;
        break;
    case 0x1E1A:
        *o_msgNoP = 0x1E1B;
        break;
    case 0x1E1B:
        *o_msgNoP = 0x1E1C;
        break;
    case 0x1E20:
        *o_msgNoP = 0x1E21;
        break;
    case 0x1E21:
        *o_msgNoP = num < 0x14 ? 0x1E30 : 0x1E22;
        break;
    case 0x1E22:
    case 0x1E28:
        *o_msgNoP = 0x1E24;
        mbPresent = 1;
        mBuyNum = 0x14;
        break;
    case 0x1E24:
        *o_msgNoP = 0x1E25;
        break;
    case 0x1E26:
        *o_msgNoP = 0x1E27;
        break;
    case 0x1E29:
        *o_msgNoP = 0x1E2A;
        mbPresent = 1;
        mBuyNum = num;
        break;
    case 0x1E2A:
        if (mBuyNum >= 0x10) {
            *o_msgNoP = 0x1E2D;
            mRupeeNum = 3;
        } else if (mBuyNum >= 6) {
            *o_msgNoP = 0x1E2C;
            mRupeeNum = 2;
        } else {
            *o_msgNoP = 0x1E2B;
            mRupeeNum = 1;
        }
        break;
    case 0x1E30:
        *o_msgNoP = 0x1E23;
        break;
    case 0x1EE6:
        *o_msgNoP = 0x1EE7;
        break;
    default:
        msg_status = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msg_status;
}
VERIFY(0x0223AEAC, &daNpc_Gp1_c::next_msgStatus);

/* 0223B08C */
u32 daNpc_Gp1_c::getMsg_GP1_0() {
    WWHD_FUNC(0x0223B08C, u32, this);
    u8 item = mItemNo;
    if (item == 0x45 /* skull necklace shown */) {
        if (dComIfGs_isEventBit(0x1420)) {
            return 0x1E29;
        }
        if (dComIfGs_isEventBit(0x1804)) {
            return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0xBC) >= 0x14 ? 0x1E28 : 0x1E30;
        }
        return 0x1E20;
    }
    if (item != 0xFF) {
        return 0x1E2F;
    }
    if (mbAfterEvent) {
        mbAfterEvent = 0;
        return 0x1E26;
    }
    if (!dComIfGs_isEventBit(0x1501)) {
        return 0x1E15;
    }
    if (dComIfGs_isEventBit(0x1808)) {
        if (dComIfGs_checkReserveItem(0x9B)) {
            return 0x1E1D;
        }
        return dComIfGs_getEventReg(0xC5FF) >= 0xA ? 0x1E1E : 0x1E1F;
    }
    return dComIfGs_isEventBit(0x1920) ? 0x1E1B : 0x1E19;
}
VERIFY(0x0223B08C, &daNpc_Gp1_c::getMsg_GP1_0);

/* 0223B268 */
u32 daNpc_Gp1_c::getMsg() {
    WWHD_FUNC(0x0223B268, u32, this);
    if (mType == 0) {
        return getMsg_GP1_0();
    }
    return 0;
}
VERIFY(0x0223B268, &daNpc_Gp1_c::getMsg);

/* 0223B2A0 */
u8 daNpc_Gp1_c::chkAttention() {
    WWHD_FUNC(0x0223B2A0, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x0223B2A0, &daNpc_Gp1_c::chkAttention);

/* 0223B328 */
BOOL daNpc_Gp1_c::wait_1() {
    WWHD_FUNC(0x0223B328, BOOL, this);
    mWaitTimer = (s16)gabi::ftoi(cM_rndF(120.0f) + 180.0f);
    if (mbTalk && mBckNum != 4) {
        if (chk_talk()) {
            setStt(2);
            mbHeadOnly = false;
        }
        return TRUE;
    }
    if (mEvtState != 1 && mEvtState < 3) {
        mEvtState = 2;
    }
    if (chk_forceTlkArea() || mbAttention) {
        mTalkTimer = 0x3C;
    }
    if (cLib_calcTimer(&mTalkTimer)) {
        mLookMode = 1;
        return TRUE;
    }
    if (cLib_calcTimer(&mWaitTimer2) == 0) {
        setStt(4);
        return TRUE;
    }
    mLookMode = 2;
    m_jnt.mbTrn = 1; /* setTrn() */
    mLookPos.x = -2.0f;
    mLookPos.y = eyePos.y;
    mLookPos.z = 940.0f;
    return TRUE;
}
VERIFY(0x0223B328, &daNpc_Gp1_c::wait_1);

static inline void talk_end(daNpc_Gp1_c* i_this) {
    s8 prev = i_this->mPrevStt;
    i_this->mbTalk = false;
    i_this->mItemNo = 0xFF;
    i_this->setStt(prev);
    i_this->mTalkTimer = 0x3C;
    i_this->endEvent();
}

/* 0223B47C */
u8 daNpc_Gp1_c::talk_1() {
    WWHD_FUNC(0x0223B47C, u8, this);
    u8 res = chk_partsNotMove();
    talk(1);
    if (!mbHasMsg) {
        return res;
    }
    /* HD: the message status comes from the message manager */
    u32 status = fopMsgM_getStatus();
    if (status == 2 || status == 6) {
        if (mbPresent) {
            dComIfGp_evmng_CancelPresent();
            if (mBuyNum != 0) {
                /* hand the necklaces over: event register 0xC5FF counts them (up to 127) */
                u8 reg = dComIfGs_getEventReg(0xC5FF);
                s16 n = mBuyNum;
                u32 a = dComIfGp_ea() + 0x5B70; /* the item count to take (dComIfGp_setItem...Count) */
                gabi::store<s16>(a, (s16)(gabi::load<s16>(a) - n));
                s32 total = reg + mBuyNum;
                if (total > 0x7F) {
                    total = 0x7F;
                }
                dComIfGs_setEventReg(0xC5FF, (u8)total);
            }
            mbPresent = 0;
        }
    } else if (status == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
        u32 msgNo = mCurrMsgNo;
        switch (msgNo) {
        case 0x1E18:
            dComIfGs_onEventBit(0x1501);
            break;
        case 0x1E1C:
            dComIfGs_onEventBit(0x1920);
            break;
        case 0x1E23:
            dComIfGs_onEventBit(0x1804);
            break;
        case 0x1E25:
            mEvtState = 4;
            break;
        case 0x1E2B:
        case 0x1E2C:
        case 0x1E2D:
            mEvtState = 5;
            break;
        }
        talk_end(this);
    }
    return res;
}
VERIFY(0x0223B47C, &daNpc_Gp1_c::talk_1);

/* 0223B740 */
BOOL daNpc_Gp1_c::walk_1() {
    WWHD_FUNC(0x0223B740, BOOL, this);
    if (mbTalk || chk_forceTlkArea() || cLib_calcTimer(&mWaitTimer2) == 0) {
        mWalkSpd = 0.0f;
    }
    if (mEvtState != 1 && mEvtState < 3) {
        mEvtState = 2;
    }
    if (mbPathEnd) {
        mbPathEnd = 0;
        if (mMoveState == 0) {
            mPathRun.mIdx = 0;
            mMoveState = 1;
        }
    }
    gabi::Local<cXyz> pnt;
    dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
    mPathPos.copy(*pnt);
    if (gabi::ftoi(mWalkSpd) == 0 && gabi::ftoi(speedF) == 0) {
        if (mbTalk || cLib_calcTimer(&mHairTimer) != 0) {
            setStt(1);
            mWalkAccel = 0.0f;
            mMoveState = 0;
            speedF = 0.0f;
        } else {
            setStt(5);
            mWalkAccel = 0.0f;
            mMoveState = 0;
            speedF = 0.0f;
            mHairTimer = (s16)((g_Counter0() & 3) + 2);
        }
    }
    return TRUE;
}
VERIFY(0x0223B740, &daNpc_Gp1_c::walk_1);

/* 0223B8D4 */
BOOL daNpc_Gp1_c::hair_1() {
    WWHD_FUNC(0x0223B8D4, BOOL, this);
    u8 talk = mbTalk;
    if (mbMorfAnimStopped) {
        if (talk || chk_forceTlkArea()) {
            setStt(1);
        } else {
            setStt(4);
        }
        return TRUE;
    }
    if (!talk && mEvtState != 1 && mEvtState < 3) {
        mEvtState = 2;
    }
    return TRUE;
}
VERIFY(0x0223B8D4, &daNpc_Gp1_c::hair_1);

/* 0223B978 */
BOOL daNpc_Gp1_c::wait_2() {
    WWHD_FUNC(0x0223B978, BOOL, this);
    if (mEvtState != 1 && mEvtState < 3) {
        f32 dy = dComIfGp_getPlayer(0)->current.pos.y - current.pos.y;
        if (dy > -1.0f) {
            mEvtState = 3;
        } else {
            mEvtState = 0;
        }
    }
    return TRUE;
}
VERIFY(0x0223B978, &daNpc_Gp1_c::wait_2);

/* 0223B9F8 */
BOOL daNpc_Gp1_c::wait_action1(void*) {
    WWHD_FUNC(0x0223B9F8, BOOL, this, (void*)nullptr);
    s8 state = mActState;
    if (state == 0) {
        if (dNpc_chkArasoi()) {
            actor_status &= ~0x80u;
            setStt(3);
        } else {
            actor_status |= 0x80u;
            setStt(1);
            mHairTimer = (s16)((g_Counter0() & 3) + 2);
        }
        mActState = mActState + 1;
        return TRUE;
    }
    if ((u32)(s32)state > 3) {
        return TRUE;
    }
    mbAttention = chkAttention();
    switch ((u32)(s32)mStt) {
    case 1:
        mbSetEyePos = wait_1();
        break;
    case 2:
        mbSetEyePos = talk_1();
        break;
    case 3:
        mbSetEyePos = wait_2();
        break;
    case 4:
        mbSetEyePos = walk_1();
        break;
    case 5:
        mbSetEyePos = hair_1();
        break;
    }
    if (mEvtState != 1 && mEvtState < 3 && !dComIfGs_isEventBit(0x1501) && !mbTalk && chk_forceTlkArea()) {
        mEvtState = 1;
    }
    lookBack();
    return TRUE;
}
VERIFY(0x0223B9F8, &daNpc_Gp1_c::wait_action1);

/* 0223BBC4 daNpc_Gp1_HIO_c::daNpc_Gp1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Gp1_HIO_c* daNpc_Gp1_HIO_c_ct(daNpc_Gp1_HIO_c* i_this) {
    WWHD_FUNC(0x0223BBC4, daNpc_Gp1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Gp1_HIO_c*)operator_new(0x40);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001A1A8;
    memcpy_g(&i_this->mPrm, gabi::at<u8>(0x101BE670), 0x34); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->m04 = -1;
    i_this->m08 = -1;
    return i_this;
}
VERIFY(0x0223BBC4, daNpc_Gp1_HIO_c_ct);

/* 0223BC30: static initialisation of the translation unit */
static void __sinit_d_a_npc_gp1_cpp() {
    WWHD_FUNC(0x0223BC30, void);
    sinit_header_statics_z(0x1046709C, 0x101BE6A4, 0x104670E8);
    daNpc_Gp1_HIO_c_ct(&l_HIO()); /* static daNpc_Gp1_HIO_c l_HIO */
}
VERIFY(0x0223BC30, __sinit_d_a_npc_gp1_cpp);

/* 0223BCD0: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x0223BCD0, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x0223BCD0, SafeString_dt);

/* 0223BCE4: daNpc_Gp1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Gp1_c_dt(daNpc_Gp1_c* i_this, s32 flags) {
    WWHD_FUNC(0x0223BCE4, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x1001A188);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x1001A198);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0223BCE4, daNpc_Gp1_c_dt);

/* 0223BD80: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x0223BD80, void, (SafeString*)nullptr);
}
VERIFY(0x0223BD80, SafeString_assureTerminationImpl);
