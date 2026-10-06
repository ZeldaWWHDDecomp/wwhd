/**
 * d_a_npc_kk1_a.cpp (WWHD)
 * NPC - Mila (poor, Windfall): part A (joint callbacks, heaps, creation, animation, matrices,
 * orders, demo, the TU's static initialisation and compiler-generated functions)
 *
 * The GameCube TU is "Nonmatching": written from the
 * WWHD code (cking.rpx) and verified against it, with the GameCube names.
 */
#define SAFESTRING_VTBL 0x1001C250 /* this TU's sead::SafeString vtable */
#include "d/actor/d_a_npc_kk1.h"

#define KK1_VTBL 0x1001C5C8 /* daNpc_Kk1_c vtable (HD: merged with fopNpc_npc_c's) */
enum { fpcNm_SWC00_e = 0x12A };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0259E6D0 dNpc_PathRun_c::setInf(u8 path, s8 room, bool dir) */
static inline void dNpc_PathRun_setInf(dNpc_PathRun_l* p, u8 path, s8 room, u8 fwd) { gabi::call(0x0259E6D0, p, path, room, fwd); }
/* 0259E778 dNpc_PathRun_c::getPoint(u8): cXyz through a hidden result pointer after `this` */
static inline void dNpc_PathRun_getPoint(dNpc_PathRun_l* p, cXyz* out, u8 idx) { gabi::call(0x0259E778, p, out, idx); }
/* 0259ECFC dNpc_PathRun_c::nextIdx */
static inline BOOL dNpc_PathRun_nextIdx(dNpc_PathRun_l* p) { return gabi::call<BOOL>(0x0259ECFC, p); }
/* 025E19CC mDoAud_seStart(id, pos) (two-argument form) */
static inline void mDoAud_seStart2(u32 id, cXyz* pos) { gabi::call(0x025E19CC, id, pos); }
/* 025E74FC mDoExt_brkAnm::init (the btk/btp argument list) */
static inline s32 mDoExt_brkAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E74FC, anm, d, p, play, attr, rate, start, end, modify, entry);
}
/* 025E8508 mDoExt_bckAnm::init(data, anm, play, mode, rate, start, end, modify) / 025E86B8 entry(data, frame) */
static inline s32 mDoExt_bckAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify) {
    return gabi::call<s32>(0x025E8508, anm, d, p, play, attr, rate, start, end, modify);
}
static inline void mDoExt_bckAnm_entry(void* anm, J3DModelData* d, f32 frame) { gabi::call(0x025E86B8, anm, d, frame); }
/* 0259D24C dNpc_setAnmIDRes(morf, loopMode, morf, speed, bckId, basId, arc) */
static inline BOOL dNpc_setAnmIDRes_l(mDoExt_McaMorf* m, s32 loop, f32 morf, f32 speed, s32 bck, s32 bas, const char* arc) {
    return gabi::call<BOOL>(0x0259D24C, m, loop, morf, speed, bck, bas, arc);
}
/* dDemo (see d_a_npc_ob1.h) */
static inline void* dDemo_object_getActor(u32 obj, u8 id) { return gabi::call<void*>(0x02526E70, obj, id); }
static inline J3DAnmTexPattern* dDemo_actor_getP_BtpData(void* ac, const char* arc) { return gabi::call<J3DAnmTexPattern*>(0x02527828, ac, arc); }
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* m, const char* arc, s32 n, u32 p, u32 q, s8 r) {
    return gabi::call<BOOL>(0x02527028, a, flags, m, arc, n, p, q, r);
}
/* 025F1B48 mDoMtx_ZXYrotM */
static inline void mDoMtx_ZXYrotM_l(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F1B48, m, x, y, z); }
/* the joint node array of a J3DModelData (HD: nodes of 0x1C bytes from +8, the count at +4) */
static inline void setJointCallBack(J3DModelData* md, u32 i, u32 cb) {
    u32 n = gabi::load<u32>(gabi::ea(md) + 4);
    u32 p = gabi::load<u32>(gabi::ea(md) + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}

/* ---- file statics ---- */
/* l_check_cnt / l_check_wrk: the search list filled by searchActor_SWC00 */
static inline u32 l_check_cnt() { return 0x1046766C; }
static inline u32 l_check_wrk() { return 0x104676F8; }
static inline J3DAnmTexPattern* btp_res(daNpc_Kk1_c* a) { return gabi::at<J3DAnmTexPattern>(gabi::load<u32>(gabi::ea(a) + 0x7EC + 0x10)); }
static inline J3DModelData* effModelData(daNpc_Kk1_c* a) { return J3DModel_getModelData_l(gabi::at<J3DModel>((u32)a->mAB4)); }
static inline J3DModel* effModel(daNpc_Kk1_c* a) { return gabi::at<J3DModel>((u32)a->mAB4); }

/* 0226AAF8 */
void daNpc_Kk1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x0226AAF8, void, this, i_node, i_model);
    /* static cXyz a_eye_pos_offst(14, 18, 0): guard 0x10467748, object 0x1046768C */
    cXyz* offst = gabi::at<cXyz>(0x1046768C);
    if (gabi::load<u32>(0x10467748) == 0) {
        gabi::store<u32>(0x10467748, 1);
        offst->z = 0.0f;
        offst->x = 14.0f;
        offst->y = 18.0f;
    }
    u32 jntNo = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jntNo), stk);
    mEyePos.x = (f32)stk->m[0][3];
    mEyePos.y = (f32)stk->m[1][3];
    mEyePos.z = (f32)stk->m[2][3];
    PSMTXMultVec(stk, offst, &mAttPos);
    mDoMtx_XrotM(stk, m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, (s16)-m_jnt.mAngles[0][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), stk);
}
VERIFY(0x0226AAF8, &daNpc_Kk1_c::_nodeCB_Head);

/* 0226AC70 */
static BOOL nodeCB_Head(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x0226AC70, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys_mModel();
        daNpc_Kk1_c* actor = gabi::at<daNpc_Kk1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (actor != nullptr) {
            actor->_nodeCB_Head(i_node, model);
        }
    }
    return TRUE;
}
VERIFY(0x0226AC70, nodeCB_Head);

/* 0226ACB8 */
void daNpc_Kk1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x0226ACB8, void, this, i_node, i_model);
    u32 jntNo = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jntNo), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, (s16)-m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), stk);
}
VERIFY(0x0226ACB8, &daNpc_Kk1_c::_nodeCB_BackBone);

/* 0226ADDC */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x0226ADDC, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys_mModel();
        daNpc_Kk1_c* actor = gabi::at<daNpc_Kk1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (actor != nullptr) {
            actor->_nodeCB_BackBone(i_node, model);
        }
    }
    return TRUE;
}
VERIFY(0x0226ADDC, nodeCB_BackBone);

/* 0226AE24 (unnamed by the matcher; HD: the one-entry table, the index is not used) */
s32 daNpc_Kk1_c::btpResID(int i_num) {
    WWHD_FUNC(0x0226AE24, s32, this, i_num);
    return gabi::load<s32>(0x1001C360); /* a_btp_tbl[0] */
}
VERIFY(0x0226AE24, &daNpc_Kk1_c::btpResID);

/* 0226AE30 */
BOOL daNpc_Kk1_c::setBtp(s8 i_num, u32 i_modify) {
    WWHD_FUNC(0x0226AE30, BOOL, this, i_num, i_modify);
    J3DModel* model = mpMorf->getModel();
    if (i_num < 0) {
        return false;
    }
    J3DAnmTexPattern* btp = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(mArcName, btpResID(i_num));
    if (btp == nullptr) /* JUT_ASSERT(618, btp != NULL) */
        JUT_ASSERT_fail(STR(0x1001C368), 0x26A, STR(0x1001C378));
    mAC5 = i_num;
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(model), btp, TRUE, 0, 1.0f, 0, -1, i_modify, FALSE) != 0;
}
VERIFY(0x0226AE30, &daNpc_Kk1_c::setBtp);

/* 0226AF1C (a tail call) */
BOOL daNpc_Kk1_c::init_texPttrnAnm(s8 i_num, u32 i_modify) {
    WWHD_FUNC(0x0226AF1C, BOOL, this, i_num, i_modify);
    return setBtp(i_num, i_modify);
}
VERIFY(0x0226AF1C, &daNpc_Kk1_c::init_texPttrnAnm);

/* 0226AF20 */
bool daNpc_Kk1_c::bodyCreateHeap() {
    WWHD_FUNC(0x0226AF20, bool, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 0xD);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(3684, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x1001C390), 0xE64, STR(0x1001C3A0));
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1, nullptr,
                                                  0x80000, 0x11020022);
    mpMorf = morf;
    if (morf == nullptr) {
        return false;
    }
    if (morf->getModel() == nullptr) {
        if (morf != nullptr) {
            /* delete mpMorf (deleting destructor, vtable slot 3) */
            gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(morf)) + 0xC), morf, 3);
        }
        mpMorf = nullptr;
        return false;
    }
    if (!init_texPttrnAnm(0, false)) {
        mpMorf = nullptr;
        return false;
    }
    m_head_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001C388) /* "head" */);
    if (m_head_jnt_num < 0) /* JUT_ASSERT(3706, m_hed_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x1001C390), 0xE7A, STR(0x1001C3B4));
    m_backbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001C3C8) /* "backbone" */);
    if (m_backbone_jnt_num < 0) /* JUT_ASSERT(3708, m_bbone_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x1001C390), 0xE7C, STR(0x1001C3D4));
    setJointCallBack(J3DModel_getModelData_l(mpMorf->getModel()), (u16)(s32)m_head_jnt_num, 0x0226AC70 /* nodeCB_Head */);
    setJointCallBack(J3DModel_getModelData_l(mpMorf->getModel()), (u16)(s32)m_backbone_jnt_num, 0x0226ADDC /* nodeCB_BackBone */);
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    return true;
}
VERIFY(0x0226AF20, &daNpc_Kk1_c::bodyCreateHeap);

/* 0226B1E0 */
bool daNpc_Kk1_c::effcCreateHeap() {
    WWHD_FUNC(0x0226B1E0, bool, this);
    J3DModelData* md = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 0xE);
    mAB4 = gabi::ea(mDoExt_J3DModel__create(md, 0, 0x11020203));
    if (mAB4 == 0) {
        return true;
    }
    void* brk = dComIfG_getObjectIDRes(mArcName, 0xF);
    if (brk == nullptr) /* JUT_ASSERT(3735) */
        JUT_ASSERT_fail(STR(0x1001C3EC), 0xE97, STR(0x1001C3FC));
    if (!mDoExt_brkAnm_init(mBrkAnm, effModelData(this), brk, TRUE, 0, 0.0f, 0, -1, FALSE, FALSE)) {
        return false;
    }
    void* btk = dComIfG_getObjectIDRes(mArcName, 0x10);
    if (btk == nullptr) /* JUT_ASSERT(3743) */
        JUT_ASSERT_fail(STR(0x1001C3EC), 0xE9F, STR(0x1001C40C));
    if (!mDoExt_btkAnm_init(&mBtkAnm, effModelData(this), btk, TRUE, 0, 0.0f, 0, -1, FALSE, FALSE)) {
        return false;
    }
    void* bck = dComIfG_getObjectIDRes(mArcName, 0);
    if (bck == nullptr) /* JUT_ASSERT(3751) */
        JUT_ASSERT_fail(STR(0x1001C3EC), 0xEA7, STR(0x1001C41C));
    if (!mDoExt_bckAnm_init(&mBckAnm, effModelData(this), bck, TRUE, 0, 0.0f, 0, -1, FALSE)) {
        return false;
    }
    mAAC.y = 0;
    mAAC.x = 0;
    mAAC.z = 0;
    return true;
}
VERIFY(0x0226B1E0, &daNpc_Kk1_c::effcCreateHeap);

/* 0226B3E8 */
bool daNpc_Kk1_c::CreateHeap() {
    WWHD_FUNC(0x0226B3E8, bool, this);
    if (!bodyCreateHeap()) {
        return false;
    }
    if (!effcCreateHeap()) {
        mpMorf = nullptr;
        return false;
    }
    mAcchCir.SetWall(30.0f, 40.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return true;
}
VERIFY(0x0226B3E8, &daNpc_Kk1_c::CreateHeap);

/* 0226B4A4 (a tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0226B4A4, BOOL, i_this);
    return ((daNpc_Kk1_c*)i_this)->CreateHeap();
}
VERIFY(0x0226B4A4, CheckCreateHeap);

/* 0226B4A8 */
static void* searchActor_SWC00(void* i_actor, void* i_this) {
    WWHD_FUNC(0x0226B4A8, void*, i_actor, i_this);
    daNpc_Kk1_c* kk1 = (daNpc_Kk1_c*)i_this;
    if (gabi::load<s32>(l_check_cnt()) < 0x14 && fopAc_IsActor(i_actor) && i_actor != nullptr &&
        fpcM_GetName(i_actor) == fpcNm_SWC00_e) {
        u32 prm = fopAcM_GetParam((fopAc_ac_c*)i_actor);
        if (((prm >> 16) & 3) == 0 && (prm & 0xFF) == (u32)kk1->m925) {
            s32 n = gabi::load<s32>(l_check_cnt());
            gabi::store<s32>(l_check_cnt(), n + 1);
            gabi::store<u32>(l_check_wrk() + n * 4, gabi::ea(i_actor));
        }
    }
    return nullptr;
}
VERIFY(0x0226B4A8, searchActor_SWC00);

/* 0226B550 */
bool daNpc_Kk1_c::decideType(int i_type) {
    WWHD_FUNC(0x0226B550, bool, this, i_type);
    if (mType > 0) {
        return true;
    }
    mType = 1;
    mACC = 0;
    for (int i = 0; i < 3; i++) /* strcpy(mArcName, "Kk") */
        gabi::store<u8>(gabi::ea(mArcName) + i, gabi::load<u8>(0x1001C440 + i));
    bool ok = false;
    if (mType != -1 && mACC != -1) {
        ok = true;
    }
    return ok;
}
VERIFY(0x0226B550, &daNpc_Kk1_c::decideType);

/* 0226B5B4 (unnamed by the matcher): the position of the path's current point; point 0x11 is
 * moved by two debug values */
static void getPathPoint(cXyz* o_pos, dNpc_PathRun_l* i_path) {
    WWHD_FUNC(0x0226B5B4, void, o_pos, i_path);
    gabi::Local<cXyz> pt;
    dNpc_PathRun_getPoint(i_path, pt, i_path->mIdx);
    f32 x = pt->x;
    f32 y = pt->y;
    o_pos->x = x;
    f32 z = pt->z;
    o_pos->y = y;
    o_pos->z = z;
    if (i_path->mIdx == 0x11) {
        o_pos->x = x + ((f32)gabi::load<s16>(0x1047C76C) + 20.0f);
        o_pos->z = o_pos->z - ((f32)gabi::load<s16>(0x1047C76E) + 50.0f);
    }
}
VERIFY(0x0226B5B4, getPathPoint);

/* 0226B690 (unnamed by the matcher) */
void daNpc_Kk1_c::set_pthPoint(u8 i_idx) {
    WWHD_FUNC(0x0226B690, void, this, i_idx);
    if (mPathRun.mPath.get() == nullptr) {
        return;
    }
    mPathRun.mIdx = i_idx;
    getPathPoint(&current.pos, &mPathRun);
    if (dNpc_PathRun_nextIdx(&mPathRun)) {
        gabi::Local<cXyz> next;
        getPathPoint(next, &mPathRun);
        current.angle.y = cLib_targetAngleY(&current.pos, next);
    }
}
VERIFY(0x0226B690, &daNpc_Kk1_c::set_pthPoint);

/* 0226B704 */
BOOL daNpc_Kk1_c::set_action(ProcFunc_l* i_action, void* i_arg) {
    WWHD_FUNC(0x0226B704, BOOL, this, i_action, i_arg);
    ProcFunc_l* cur = &mAction;
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
        mACD = 9;
        pmf_call(this, cur, i_arg);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mACD = 0;
    pmf_call(this, cur, i_arg);
    return TRUE;
}
VERIFY(0x0226B704, &daNpc_Kk1_c::set_action);

/* 0226B830 */
bool daNpc_Kk1_c::init_KK1_0() {
    WWHD_FUNC(0x0226B830, bool, this);
    if (!dComIfGs_isEventBit(0x2D01)) {
        return false;
    }
    if (dKy_daynight_check() && dComIfGs_isEventBit(0xE08)) {
        return false;
    }
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, 0x1001C210 /* &daNpc_Kk1_c::wait_action1 */);
    set_action(pmf, nullptr);
    return true;
}
VERIFY(0x0226B830, &daNpc_Kk1_c::init_KK1_0);

/* 0226B8E4 */
void daNpc_Kk1_c::play_btp_anm() {
    WWHD_FUNC(0x0226B8E4, void, this);
    u8 frameMax = (u8)J3DAnm_getFrameMax(btp_res(this));
    if (mAC5 == 0 && cLib_calcTimer(&mBlinkTimer) != 0) {
        return;
    }
    u8 frame = (u8)(mBtpFrame + 1);
    mBtpFrame = frame;
    if (frame < frameMax) {
        return;
    }
    if (mAC5 == 0) {
        mBlinkTimer = (s16)cLib_getRndValue(0x3C, 0x5A);
        frameMax = 0;
    }
    mBtpFrame = frameMax;
}
VERIFY(0x0226B8E4, &daNpc_Kk1_c::play_btp_anm);

/* 0226B984 */
void daNpc_Kk1_c::setBikon(cXyz* i_offset) {
    WWHD_FUNC(0x0226B984, void, this, i_offset);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    gabi::Local<cXyz> pos;
    PSMTXMultVec(mDoMtx_stack_c::get(), i_offset, pos);
    if (dPa_control_set(dComIfGp_getParticle(), 0, 0x8152, pos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr)) {
        mDoAud_seStart2(0x58BD, &current.pos);
    }
}
VERIFY(0x0226B984, &daNpc_Kk1_c::setBikon);

/* 0226BA48 */
void daNpc_Kk1_c::play_eff_anm() {
    WWHD_FUNC(0x0226BA48, void, this);
    s16 brkFrame = (s16)(mAAC.x + 1);
    void* btk = gabi::at<void>(gabi::load<u32>(gabi::ea(this) + 0x9AC + 0x68));
    if (brkFrame < 30) {
        mAAC.x = brkFrame;
    } else {
        mAAC.x = 29;
    }
    s32 btkMax = J3DAnm_getFrameMax(btk);
    s16 btkFrame = (s16)(mAAC.y + 1);
    if (btkFrame < btkMax) {
        mAAC.y = btkFrame;
    } else {
        mAAC.y = (s16)(btkMax - 1);
    }
    s32 bckMax = J3DAnm_getFrameMax(gabi::at<void>(gabi::load<u32>(gabi::ea(this) + 0xA20 + 0x88)));
    s16 bckFrame = (s16)(mAAC.z + 1);
    if (bckFrame < bckMax) {
        mAAC.z = bckFrame;
    } else {
        mAAC.z = 0x3B;
    }
}
VERIFY(0x0226BA48, &daNpc_Kk1_c::play_eff_anm);

/* 0226BB5C */
void daNpc_Kk1_c::play_animation() {
    WWHD_FUNC(0x0226BB5C, void, this);
    u32 sndId = 0;
    play_btp_anm();
    if (gabi::load<u32>(gabi::ea(&mObjAcch) + 0x28) & 0x20) { /* mObjAcch.ChkGroundHit() */
        sndId = gabi::call<u32>(0x024EECAC, dComIfG_Bgsp(), gabi::ea(this) + 0x538); /* dBgS::GetMtrlSndId(gnd poly) */
    }
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    m922 = (u8)gabi::call<BOOL>(0x025E535C, mpMorf.get(), &eyePos, sndId, reverb); /* mpMorf->play */
    if (mpMorf->getFrame() < m8D8) {
        m922 = 1;
    }
    m8D8 = mpMorf->getFrame();
    switch ((s32)(s8)mAC6) {
    case 8:
        if (mpMorf->checkFrame(4.0f)) {
            gabi::Local<cXyz> ofs;
            ofs->x = 0.0f;
            ofs->y = -50.0f;
            ofs->z = -15.0f;
            setBikon(ofs);
        }
        break;
    case 9:
        if (mpMorf->checkFrame(4.0f)) {
            gabi::Local<cXyz> ofs;
            ofs->x = 0.0f;
            ofs->y = -50.0f;
            ofs->z = 0.0f;
            setBikon(ofs);
        }
        break;
    }
    play_eff_anm();
}
VERIFY(0x0226BB5C, &daNpc_Kk1_c::play_animation);

/* 0226BCD4 */
void daNpc_Kk1_c::flwAse() {
    WWHD_FUNC(0x0226BCD4, void, this);
    if (mABC == 0) {
        return;
    }
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(mpMorf->getModel(), m_head_jnt_num), stk);
    u32 emitter = mABC;
    f32 x = stk->m[0][3];
    f32 y = stk->m[1][3];
    f32 z = stk->m[2][3];
    if (gabi::load<u8>(emitter + 0x262) >= 7) {
        y = -y;
    }
    gabi::store<f32>(emitter + 0x230, y); /* emitter->setGlobalTranslation */
    gabi::store<f32>(emitter + 0x22C, x);
    gabi::store<f32>(emitter + 0x234, z);
}
VERIFY(0x0226BCD4, &daNpc_Kk1_c::flwAse);

/* 0226BD78 */
void daNpc_Kk1_c::setAttention(u32 i_force) {
    WWHD_FUNC(0x0226BD78, void, this, i_force);
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attPos->x = current.pos.x;
    attPos->y = current.pos.y + l_HIO_kk1().mAttentionYOffset;
    attPos->z = current.pos.z;
    if (m8E8 == 0 && !i_force) {
        return;
    }
    eyePos.z = (f32)mAttPos.z;
    eyePos.y = (f32)mAttPos.y;
    eyePos.x = (f32)mAttPos.x;
}
VERIFY(0x0226BD78, &daNpc_Kk1_c::setAttention);

/* 0226BDCC */
void daNpc_Kk1_c::setMtx(u32 i_force) {
    WWHD_FUNC(0x0226BDCC, void, this, i_force);
    J3DModel* model = mpMorf->getModel();
    f32 sy = scale.y;
    f32 sx = scale.x;
    f32 sz = scale.z;
    gabi::store<f32>(gabi::ea(model) + 0xBC, sx); /* model->setBaseScale(scale) */
    gabi::store<f32>(gabi::ea(model) + 0xC0, sy);
    gabi::store<f32>(gabi::ea(model) + 0xC4, sz);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM_l(mDoMtx_stack_c::get(), m88E.x, m88E.y, m88E.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    mDoExt_bckAnm_entry(&mBckAnm, effModelData(this), (f32)(s16)mAAC.z);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mDoMtx_stack_c::transM(0.0f, 120.0f, 30.0f);
    J3DModel_setBaseTRMtx(effModel(this), mDoMtx_stack_c::get());
    J3DModel_calc(effModel(this));
    /* mBckAnm.remove(): the joint tree's animation */
    u32 md = gabi::ea(effModelData(this));
    gabi::store<u32>(gabi::load<u32>(md + 8) + 0x14, 0);
    flwAse();
    setAttention(i_force);
}
VERIFY(0x0226BDCC, &daNpc_Kk1_c::setMtx);

/* 0226BFD0 */
bool daNpc_Kk1_c::createInit() {
    WWHD_FUNC(0x0226BFD0, bool, this);
    /* event names (.data 0x101BF2AC, 8 entries) */
    be<s16>* evtIdx = &m8EC;
    for (int i = 0; i < 8; i++) {
        u32 name = gabi::load<u32>(0x101BF2AC + i * 4);
        evtIdx[i] = dComIfGp_evmng_getEventIdx(gabi::at<const char>(name), 0xFF);
    }
    mEventCut.setActorInfo2(STR(0x1001C464) /* "Kk1" */, this);
    u32 prm = fopAcM_GetParam(this);
    u8 path = (prm >> 16) & 0xFF;
    m925 = (prm >> 8) & 0xFF;
    if (path != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, path, current.roomNo, 1);
        if (mPathRun.mPath.get() == nullptr) {
            return false;
        }
        gabi::store<u32>(gabi::ea(this) + 0x2E0, gabi::load<u32>(gabi::ea(this) + 0x2E0) & ~0x80u); /* actor_status */
        set_pthPoint(0);
    }
    if (mPathRun.mPath.get() == nullptr) {
        return false;
    }
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xA9); /* attention_info.distances */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xA9);
    gravity = -4.5f;
    mAC6 = 0xC;
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags */
    if (mACC != 0) {
        return false;
    }
    if (!init_KK1_0()) {
        return false;
    }
    s16 az = current.angle.z;
    s16 ax = current.angle.x;
    m88E.z = az; /* m88E: the model rotation */
    s16 ay = current.angle.y;
    shape_angle.x = ax;
    shape_angle.y = ay;
    shape_angle.z = az;
    m88E.y = ay;
    m88E.x = ax;
    /* dBgS_GndChk on the stack: put the actor on the ground */
    gabi::Local<dBgS_GndChk> gndChk;
    dBgS_GndChk_vt vt = {0x1001C2A0, 0x1001C2B0, 0x1001C2D0, 0x1001C2C0};
    dBgS_GndChk_ct(gndChk, vt, false);
    u32 b = gabi::ea(gndChk.get());
    gabi::store<f32>(b + 0x24, current.pos.x);
    gabi::store<f32>(b + 0x28, current.pos.y + 50.0f);
    gabi::store<f32>(b + 0x2C, current.pos.z);
    f32 groundY = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
    if (groundY != -1000000000.0f) {
        old.pos.y = groundY;
        current.pos.y = groundY;
    }
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    play_animation();
    gabi::store<s8>(gabi::ea(&tevStr) + 0xB9, dBgS_GetRoomId(dComIfG_Bgsp(), gabi::at<void>(gabi::ea(this) + 0x538)));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), gabi::at<void>(gabi::ea(this) + 0x538)));
    mpMorf->setMorf(0.0f);
    setMtx(true);
    /* ~dBgS_GndChk */
    gabi::store<u32>(b + 0x20, 0x1001C2B0);
    gabi::store<u32>(b + 0x40, 0x1001C2D0);
    gabi::store<u32>(b + 0x4C, 0x1001C290);
    gabi::call(0x02008DAC, gndChk.get(), 0); /* cBgS_Chk::~cBgS_Chk */
    return true;
}
VERIFY(0x0226BFD0, &daNpc_Kk1_c::createInit);

/* 0226C28C */
cPhs_State daNpc_Kk1_c::_create() {
    WWHD_FUNC(0x0226C28C, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Kk1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = KK1_VTBL;
            gabi::call(0x025E7820, mBtpAnm);  /* mDoExt_btpAnm::mDoExt_btpAnm */
            gabi::call(0x025E7480, mBrkAnm);  /* mDoExt_brkAnm::mDoExt_brkAnm */
            gabi::call(0x025E7C6C, &mBtkAnm); /* mDoExt_btkAnm::mDoExt_btkAnm (matcher: init) */
            /* inline mDoExt_bckAnm::mDoExt_bckAnm() */
            u32 b = gabi::ea(&mBckAnm);
            gabi::call(0x027F2BC0, b, 0); /* J3DFrameCtrl::init */
            gabi::store<u32>(b + 0x10, 0x1016E54C);
            gabi::call(0x027DA984, b + 0x14);
            gabi::store<u32>(b + 0x80, 0);
            gabi::store<u32>(b + 0x48, 0x1016D820);
            gabi::store<u32>(b + 0x58, 0);
            gabi::store<u32>(b + 0x84, 0);
            gabi::store<u32>(b + 0x7C, 0);
            gabi::store<u32>(b + 0x88, 0);
            gabi::store<u32>(b + 0x10, 0x1001C268); /* this TU's vtable */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State result = dComIfG_resLoad(&mPhs, mArcName);
    m92C = result == cPhs_COMPLEATE_e;
    if (result != cPhs_COMPLEATE_e) {
        return result;
    }
    /* a_heap_size_tbl (.data 0x101BF2EC) */
    if (!fopAcM_entrySolidHeap(this, 0x0226B4A4 /* CheckCreateHeap */, gabi::load<u32>(0x101BF2EC + (s32)mType * 4))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, -20.0f, -50.0f, 50.0f, 140.0f, 50.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return result;
}
VERIFY(0x0226C28C, &daNpc_Kk1_c::_create);

/* 0226C438 */
void daNpc_Kk1_c::delAse() {
    WWHD_FUNC(0x0226C438, void, this);
    u32 emitter = mABC;
    if (emitter == 0) {
        return;
    }
    /* emitter->becomeInvalidEmitter() */
    gabi::store<s32>(emitter + 0x5C, -1);
    gabi::store<u32>(emitter + 0x254, gabi::load<u32>(emitter + 0x254) | 1);
    mABC = 0;
}
VERIFY(0x0226C438, &daNpc_Kk1_c::delAse);

/* 0226C464 */
BOOL daNpc_Kk1_c::_delete() {
    WWHD_FUNC(0x0226C464, BOOL, this);
    /* HD: no HIO child deletion */
    dComIfG_resDelete(&mPhs, mArcName);
    delAse();
    if (gabi::load<u32>(gabi::ea(this) + 0xF4) != 0 && mpMorf.get() != nullptr) { /* heap */
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x0226C464, &daNpc_Kk1_c::_delete);

/* 0226C4C4 */
bool daNpc_Kk1_c::partner_search_sub(u32 i_func) {
    WWHD_FUNC(0x0226C4C4, bool, this, i_func);
    bool found = false;
    m86C = (u32)-1;
    gabi::store<s32>(l_check_cnt(), 0);
    for (int i = 0; i < 0x14; i++)
        gabi::store<u32>(l_check_wrk() + i * 4, 0);
    gabi::call<void*>(0x025DE508, i_func, this); /* fpcEx_Search */
    if (gabi::load<s32>(l_check_cnt()) != 0) {
        u32 a = gabi::load<u32>(l_check_wrk());
        u32 id = (u32)-1;
        if (a != 0)
            id = gabi::load<u32>(a + 4); /* fopAcM_GetID */
        found = true;
        m86C = id;
    }
    return found;
}
VERIFY(0x0226C4C4, &daNpc_Kk1_c::partner_search_sub);

/* 0226C570 */
void daNpc_Kk1_c::partner_search() {
    WWHD_FUNC(0x0226C570, void, this);
    if ((s8)mACD != 1) {
        return;
    }
    if (mACC == 0 && !partner_search_sub(0x0226B4A8 /* searchActor_SWC00 */)) {
        return;
    }
    mACD = (s8)mACD + 1;
}
VERIFY(0x0226C570, &daNpc_Kk1_c::partner_search);

/* 0226C5DC (unnamed by the matcher) */
s32 daNpc_Kk1_c::bckResID(int i_num) {
    WWHD_FUNC(0x0226C5DC, s32, this, i_num);
    return gabi::load<s32>(0x1001C470 + i_num * 4); /* a_bck_tbl */
}
VERIFY(0x0226C5DC, &daNpc_Kk1_c::bckResID);

/* 0226C5F0 */
void daNpc_Kk1_c::setAse() {
    WWHD_FUNC(0x0226C5F0, void, this);
    delAse();
    mABC = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 0, 0x819E, &current.pos, &current.angle, nullptr, 0xFF, nullptr, -1,
                                    nullptr, nullptr, nullptr));
}
VERIFY(0x0226C5F0, &daNpc_Kk1_c::setAse);

/* 0226C660 */
void daNpc_Kk1_c::setAnm_anm(anm_prm_c* i_prm) {
    WWHD_FUNC(0x0226C660, void, this, i_prm);
    s8 num = i_prm->mAnmNo;
    if (num < 0 || (s8)mAC6 == num) {
        return;
    }
    s32 bck = bckResID(num);
    dNpc_setAnmIDRes_l(mpMorf, i_prm->mLoopMode, i_prm->mMorf, i_prm->mSpeed, bck, -1, mArcName);
    mAC6 = (u8)(s8)i_prm->mAnmNo;
    delAse();
    m92E = 1;
    if ((s8)mAC6 == 1) {
        setAse();
        mAAC.y = 0;
        mAAC.x = 0;
        mAAC.z = 0;
        m92E = 0;
    }
    m923 = 0;
    m922 = 0;
    m8D8 = 0.0f;
}
VERIFY(0x0226C660, &daNpc_Kk1_c::setAnm_anm);

/* 0226C730 */
void daNpc_Kk1_c::setAnm_NUM(int i_num, int i_tex) {
    WWHD_FUNC(0x0226C730, void, this, i_num, i_tex);
    anm_prm_c* tbl = gabi::at<anm_prm_c>(0x101BF2F4); /* a_anm_prm_tbl */
    if (i_tex != 0) {
        init_texPttrnAnm(tbl[i_num].mTexNo, true);
    }
    setAnm_anm(&tbl[i_num]);
}
VERIFY(0x0226C730, &daNpc_Kk1_c::setAnm_NUM);

/* 0226C7A0 */
bool daNpc_Kk1_c::checkCommandTalk() {
    WWHD_FUNC(0x0226C7A0, bool, this);
    bool ret = false;
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 1) { /* eventInfo.checkCommandTalk() */
        ret = true;
        if ((s8)mAC2 == 5 && m914 != 0) {
            ret = false;
        }
    }
    return ret;
}
VERIFY(0x0226C7A0, &daNpc_Kk1_c::checkCommandTalk);

/* 0226C7DC */
void daNpc_Kk1_c::checkOrder() {
    WWHD_FUNC(0x0226C7DC, void, this);
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 2) { /* eventInfo.checkCommandDemoAccrpt() */
        be<s16>* evtIdx = &m8EC;
        if (dComIfGp_evmng_startCheck(evtIdx[m8FC]) && (s8)mAC7 >= 3) {
            if (m8FC == 4) {
                setAnm_NUM(0, 1);
            }
            mAC7 = 0;
            mAC4 = 0xFF;
            mAC3 = 0xFF;
        }
    } else if (checkCommandTalk()) {
        if ((s8)mAC7 == 1 || (s8)mAC7 == 2) {
            mAC7 = 0;
            m933 = 1;
        }
    }
}
VERIFY(0x0226C7DC, &daNpc_Kk1_c::checkOrder);

/* 0226C8D8 */
u8 daNpc_Kk1_c::demo() {
    WWHD_FUNC(0x0226C8D8, u8, this);
    u8 id = demoActorID;
    if (id == 0) {
        if (m936 != 0) {
            m936 = 0;
        }
        return 0;
    }
    if (m936 == 0) {
        m_jnt.mAngles[0][1] = 0;
        m_jnt.mAngles[1][0] = 0;
        m936 = 1;
        m92F = 0;
        m_jnt.mAngles[0][0] = 0;
        id = demoActorID;
        m_jnt.mAngles[1][1] = 0;
    }
    void* demoAc = nullptr;
    if (id != 0 && id <= 0x20) {
        /* dComIfGp_demo_getActor(id) (HD inline with a range check) */
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x1001C348), 0x23A, STR(0x1001C310));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demoAc = dDemo_object_getActor(obj, id);
    }
    J3DAnmTexPattern* btp = btp_res(this);
    if (btp != nullptr) {
        u8 frameMax = (u8)J3DAnm_getFrameMax(btp);
        u8 frame = (u8)(mBtpFrame + 1);
        if (frame < frameMax) {
            mBtpFrame = frame;
        } else {
            mBtpFrame = frameMax;
        }
    }
    if (demoAc != nullptr) {
        J3DAnmTexPattern* demoBtp = dDemo_actor_getP_BtpData(demoAc, mArcName);
        if (demoBtp != nullptr) {
            mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(mpMorf->getModel()), demoBtp, TRUE, 0, 1.0f, 0, -1, TRUE, FALSE);
            mBtpFrame = 0;
            mAC5 = 1;
        }
    }
    dDemo_setDemoData(this, 0x6A, mpMorf, mArcName, 0, 0, 0, 0);
    return m936;
}
VERIFY(0x0226C8D8, &daNpc_Kk1_c::demo);

/* 0226CA94 */
s32 daNpc_Kk1_c::isEventEntry() {
    WWHD_FUNC(0x0226CA94, s32, this);
    return dComIfGp_evmng_getMyStaffId(gabi::at<const char>(mEventCut.mpEvtStaffName), nullptr, 0);
}
VERIFY(0x0226CA94, &daNpc_Kk1_c::isEventEntry);

/* 0226CAD4 */
void daNpc_Kk1_c::setAnm() {
    WWHD_FUNC(0x0226CAD4, void, this);
    anm_prm_c* tbl = gabi::at<anm_prm_c>(0x101BF3D4); /* a_anm_prm_tbl by status */
    init_texPttrnAnm(tbl[(s8)mAC8].mTexNo, true);
    setAnm_anm(&tbl[(s8)mAC8]);
}
VERIFY(0x0226CAD4, &daNpc_Kk1_c::setAnm);

/* 0226CB44 */
void daNpc_Kk1_c::setStt(s8 i_stt) {
    WWHD_FUNC(0x0226CB44, void, this, i_stt);
    s8 old = (s8)mAC8;
    mAC8 = i_stt;
    switch ((u32)(s32)i_stt) {
    case 1:
    case 4:
    case 6:
    case 7:
        m914 = 0;
        mAC7 = 0;
        speedF = 0.0f;
        break;
    case 2:
        mAC9 = old;
        mAC3 = 0xFF;
        mACE = 0;
        mAC7 = 0;
        m912 = 0;
        mAC4 = 0xFF;
        break;
    case 3:
        mAC7 = 0;
        break;
    case 5:
        mAC7 = 0;
        mAC2 = 0;
        m926 = 1;
        mAC1 = 2;
        break;
    }
    setAnm();
}
VERIFY(0x0226CB44, &daNpc_Kk1_c::setStt);

/* 0226CBE8 */
void daNpc_Kk1_c::endEvent() {
    WWHD_FUNC(0x0226CBE8, void, this);
    dComIfGp_event_reset();
    mAC3 = 0xFF;
    mAC4 = 0xFF;
}
VERIFY(0x0226CBE8, &daNpc_Kk1_c::endEvent);

/* 0226CC2C (matcher: cLib_getRndValue<int>) */
fopAc_ac_c* daNpc_Kk1_c::searchByID(fpc_ProcID i_id, be<s32>* o_res) {
    WWHD_FUNC(0x0226CC2C, fopAc_ac_c*, this, i_id, o_res);
    gabi::Local<be<u32>> actor;
    *actor = 0;
    *o_res = 0;
    if (!gabi::call<BOOL>(0x025D54C4, i_id, actor.get())) { /* fopAcM_SearchByID */
        *o_res = 1;
    }
    return gabi::at<fopAc_ac_c>((u32)*actor);
}
VERIFY(0x0226CC2C, &daNpc_Kk1_c::searchByID);

/* ---- the TU's static initialisation and compiler-generated functions ---- */

/* 022711F4 daNpc_Kk1_HIO_c::daNpc_Kk1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Kk1_HIO_c* daNpc_Kk1_HIO_c_ct(daNpc_Kk1_HIO_c* i_this) {
    WWHD_FUNC(0x022711F4, daNpc_Kk1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Kk1_HIO_c*)operator_new(0x60);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001C300;
    memcpy_g(i_this->mPrm, gabi::at<u8>(0x101BF5EC), 0x54); /* a_prm_tbl */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}
VERIFY(0x022711F4, daNpc_Kk1_HIO_c_ct);

/* 02271260: static initialisation of the translation unit */
static void __sinit_d_a_npc_kk1_cpp() {
    WWHD_FUNC(0x02271260, void);
    sinit_header_statics_z(0x10467670, 0x101BF640, 0x1046767C);
    daNpc_Kk1_HIO_c_ct(&l_HIO_kk1()); /* static daNpc_Kk1_HIO_c l_HIO */
}
VERIFY(0x02271260, __sinit_d_a_npc_kk1_cpp);

/* 02271300: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x02271300, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x02271300, SafeString_dt);

/* 02271314: daNpc_Kk1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Kk1_c_dt(daNpc_Kk1_c* i_this, s32 flags) {
    WWHD_FUNC(0x02271314, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x027F3628, gabi::ea(&i_this->mBckAnm) + 0x10, 0); /* mBckAnm's J3DAnm object (+0x10) */
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x1001C2E0);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x1001C2F0);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02271314, daNpc_Kk1_c_dt);

/* 022713BC: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x022713BC, void, (SafeString*)nullptr);
}
VERIFY(0x022713BC, SafeString_assureTerminationImpl);
