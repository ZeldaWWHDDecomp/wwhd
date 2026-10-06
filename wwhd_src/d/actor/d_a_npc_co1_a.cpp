/**
 * d_a_npc_co1_a.cpp (WWHD)
 * NPC - Prince Komali (before Dragon Roost Cavern): part A (heap, animation, create, matrices)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_co1.cpp) has only "Nonmatching" stubs for this actor, so the functions are
 * written from the WWHD code, verified against cking.rpx.
 */
#include "d/actor/d_a_npc_co1.h"

#define SAFESTRING_VTBL 0x10019380 /* this TU's sead::SafeString vtable */
#define CO1_VTBL 0x10019628        /* daNpc_Co1_c vtable (HD: merged with fopNpc_npc_c's) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* resources by id (HD: sead::SafeString key, per-TU vtable) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->__vtbl = SAFESTRING_VTBL;
    key->mStringTop = gabi::ea(arc);
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
/* J3DModel (HD): model data at +0xAC */
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
/* J3DModelData (HD): 027F68FC returns the joint name table header (self-relative offset at
 * +0x10 to the JUTNameTab), 027F3F94 (the matcher calls it __nw) the joint tree header (joint
 * count u16 at +8) */
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
/* 025E789C mDoExt_btpAnm::init(modelData, anm, anmPlay, attr, rate, start, end, modify, entry) */
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start,
                                     s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
/* 025E7CE0 mDoExt_btkAnm::init(modelData, key, anmPlay, attr, rate, start, end, modify, entry) */
static inline s32 mDoExt_btkAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start,
                                     s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E7CE0, anm, d, p, play, attr, rate, start, end, modify, entry);
}
/* delete a McaMorf (HD: virtual deleting destructor, vtable at +0, slot +0xC) */
static inline void McaMorf_delete(mDoExt_McaMorf* morf) {
    if (morf != nullptr) {
        u32 vt = gabi::load<u32>(gabi::ea(morf));
        gabi::call_ptr(gabi::load<u32>(vt + 0xC), morf, 3);
    }
}

/* HD J3D: j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a model's joint
 * matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    J3DMtxBlock_l* blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(gabi::ea(model) + 0x2C));
    blk->mFlags |= 0x10; /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}

/* J3DAnmTexPattern / J3DAnmTextureSRTKey::getFrameMax: virtual (vtable pointer at +4, slot +0x14) */
static inline s32 J3DAnm_getFrameMax(u32 p) {
    u32 vt = gabi::load<u32>(p + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), gabi::at<u8>(p));
}
/* 02055B64 cLib_calcTimer<s16> (out of line) */
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* 025B7D90 dSv_player_collect_c::isSymbol (collect at save + 0xD4) */
static inline BOOL dComIfGs_isSymbol(u8 i) { return gabi::call<BOOL>(0x025B7D90, gabi::load<u32>(0x101F84DC) + 0xD4, i); }

/* ---- file statics ---- */
/* l_HIO (daNpc_Co1_HIO_c, 0x38 bytes, HD vtable at 0) at 0x10466CB0; fields read here: */
static u32 l_HIO() { return 0x10466CB0; }
static f32 l_HIO_attOffsetY() { return gabi::load<f32>(l_HIO() + 0x20); }

/* pointers to member functions in .data (copied to the stack before set_action) */
enum : u32 { PMF_wait_action1 = 0x10019350 };
static inline void pmf_load(ProcFunc_l* dst, u32 src) {
    gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(src));
    gabi::store<u32>(gabi::ea(dst) + 4, gabi::load<u32>(src + 4));
}
/* (this->*pmf)(arg) */
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

/* dBgS (play + 0x12A0) */
static inline s8 dBgS_GetRoomId_l(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor_l(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }

static mDoExt_McaMorf* prlMorf(daNpc_Co1_c* i_this) { return gabi::at<mDoExt_McaMorf>(i_this->m7E8); } /* pearl McaMorf */

/* 022271C0 */
void daNpc_Co1_c::nodeCo1Control(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x022271C0, void, this, i_node, i_model);
    /* static cXyz a_eye_pos_off(14.0f, 18.0f, 0.0f): guard 0x10466D04, object 0x10466CF8 */
    cXyz* a_eye_pos_off = gabi::at<cXyz>(0x10466CF8);
    if (gabi::load<u32>(0x10466D04) == 0) {
        a_eye_pos_off->z = 0.0f;
        gabi::store<u32>(0x10466D04, 1);
        a_eye_pos_off->x = 14.0f;
        a_eye_pos_off->y = 18.0f;
    }
    J3DJoint* joint = J3DNode_toJoint(i_node);
    u32 jointIdx = gabi::load<u16>(gabi::ea(joint) + 4);
    PSMTXCopy(getAnmMtx(i_model, jointIdx), mDoMtx_stack_c::get());
    if (jointIdx == (u32)(s32)m7E4) { /* head */
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[0][0]);
        PSMTXMultVec(mDoMtx_stack_c::get(), a_eye_pos_off, &m980);
    }
    if (jointIdx == (u32)(s32)m7E5) { /* backbone */
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[1][0]);
    }
    PSMTXCopy(mDoMtx_stack_c::get(), gabi::at<Mtx34>(0x104B4868));
    mtx_copy(getAnmMtx(i_model, jointIdx), mDoMtx_stack_c::get());
}
VERIFY(0x022271C0, &daNpc_Co1_c::nodeCo1Control);

/* 02227360 */
static BOOL nodeCallBack_Co1(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x02227360, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        u32 model = gabi::load<u32>(0x104B462C);
        daNpc_Co1_c* user = gabi::at<daNpc_Co1_c>(gabi::load<u32>(model + 0xB8));
        if (user != nullptr) {
            user->nodeCo1Control(i_node, gabi::at<J3DModel>(model));
        }
    }
    return TRUE;
}
VERIFY(0x02227360, nodeCallBack_Co1);

/* 022273A8 */
J3DModelData* daNpc_Co1_c::create_Anm() {
    WWHD_FUNC(0x022273A8, J3DModelData*, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x100193FC) /* "Co" */, 0x13);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2098, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x10019408), 0x832, STR(0x10019418));
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(STR(0x100193FC), 0x10);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, 1,
                                    nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr) {
        return nullptr;
    }
    if (mpMorf->getModel() == nullptr) {
        McaMorf_delete(mpMorf);
        mpMorf = nullptr;
        return nullptr;
    }
    m7E4 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10019400) /* "head" */);
    if (m7E4 < 0) /* JUT_ASSERT(2120, m_hed_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x10019408), 0x848, STR(0x1001942C));
    m7E5 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10019440) /* "backbone" */);
    if (m7E5 < 0)
        JUT_ASSERT_fail(STR(0x10019408), 0x84B, STR(0x1001944C));
    m7E6 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100193F4) /* "handR" */);
    if (m7E6 < 0)
        JUT_ASSERT_fail(STR(0x10019408), 0x84E, STR(0x10019464));
    return a_mdl_dat;
}
VERIFY(0x022273A8, &daNpc_Co1_c::create_Anm);

/* 022275E0 */
u32 daNpc_Co1_c::btpNum_toResID(int i_btpNum) {
    WWHD_FUNC(0x022275E0, u32, this, i_btpNum);
    /* a_btp_resID_tbl (.data 0x1001947C) */
    return gabi::load<u32>(0x1001947C + i_btpNum * 4);
}
VERIFY(0x022275E0, &daNpc_Co1_c::btpNum_toResID);

/* 022275F4 */
/* i_modify (GameCube bool) is passed on unnormalised: typed u32 */
BOOL daNpc_Co1_c::setBtp(u32 i_modify, int i_btpNum) {
    WWHD_FUNC(0x022275F4, BOOL, this, i_modify, i_btpNum);
    J3DModelData* model_data = J3DModel_getModelData_l(mpMorf->getModel());
    u32 res_id = btpNum_toResID(i_btpNum);
    m870 = gabi::ea(dComIfG_getObjectIDRes(STR(0x10019498) /* "Co" */, res_id));
    if (m870 == 0) /* JUT_ASSERT(465, m_hed_tex_pttrn != NULL) */
        JUT_ASSERT_fail(STR(0x1001949C), 0x1D1, STR(0x100194AC));
    s32 r = mDoExt_btpAnm_init(mBtpAnm, model_data, gabi::at<u8>(m870), 1, 2, 1.0f, 0, -1, i_modify, 0);
    bool ok = r == 1;
    if (ok) {
        mBlinkTimer = 0;
        mBlinkFrame = 0;
    }
    return ok;
}
VERIFY(0x022275F4, &daNpc_Co1_c::setBtp);

/* 022276E0 */
/* tail call: setBtp's result register is passed through (typed u32) */
u32 daNpc_Co1_c::iniTexPttrnAnm(u32 i_modify) {
    WWHD_FUNC(0x022276E0, u32, this, i_modify);
    return gabi::call<u32>(0x022275F4, this, i_modify, (s32)mA2C); /* setBtp(i_modify, mBtpNum) */
}
VERIFY(0x022276E0, &daNpc_Co1_c::iniTexPttrnAnm);

/* 022276EC */
J3DModelData* daNpc_Co1_c::create_prl_Anm() {
    WWHD_FUNC(0x022276EC, J3DModelData*, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x100194C4) /* "Co" */, 0x14);
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail(STR(0x100194C8), 0x868, STR(0x100194D8));
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(STR(0x100194C4), 7);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2, 1.0f, 0, -1, 1, nullptr,
                                                  0x80000, 0x15021222);
    m7E8 = gabi::ea(morf);
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->getModel() == nullptr) {
        McaMorf_delete(morf);
        m7E8 = 0;
        return nullptr;
    }
    m7F0 = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100194EC) /* "co_pearl" */);
    if (m7F0 < 0)
        JUT_ASSERT_fail(STR(0x100194C8), 0x882, STR(0x100194F8));
    return a_mdl_dat;
}
VERIFY(0x022276EC, &daNpc_Co1_c::create_prl_Anm);

/* 02227884 */
BOOL daNpc_Co1_c::setBtk(u32 i_modify) {
    WWHD_FUNC(0x02227884, BOOL, this, i_modify);
    J3DModelData* model_data = J3DModel_getModelData_l(prlMorf(this)->getModel());
    m7EC = gabi::ea(dComIfG_getObjectIDRes(STR(0x1001950C) /* "Co" */, 0x15));
    if (m7EC == 0)
        JUT_ASSERT_fail(STR(0x10019510), 0x1ED, STR(0x10019520));
    s32 r = mDoExt_btkAnm_init(mBtkAnm, model_data, gabi::at<u8>(m7EC), 1, 2, 1.0f, 0, -1, i_modify, 0);
    bool ok = r == 1;
    if (ok) {
        m868 = 0;
    }
    return ok;
}
VERIFY(0x02227884, &daNpc_Co1_c::setBtk);

/* 02227964 */
bool daNpc_Co1_c::create_itm_Mdl() {
    WWHD_FUNC(0x02227964, bool, this);
    m86C = 0;
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x10019534) /* "Co" */, 0x12);
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail(STR(0x10019538), 0x894, STR(0x10019548));
    m86C = gabi::ea(mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000002));
    return true;
}
VERIFY(0x02227964, &daNpc_Co1_c::create_itm_Mdl);

/* 02227A04 */
BOOL daNpc_Co1_c::CreateHeap() {
    WWHD_FUNC(0x02227A04, BOOL, this);
    J3DModelData* anm_model = create_Anm();
    if (!anm_model) {
        return FALSE;
    }
    mA2C = 6;
    if (!iniTexPttrnAnm(false) || !create_prl_Anm()) {
        mpMorf = nullptr;
        return FALSE;
    }
    if (setBtk(false) && create_itm_Mdl()) {
        for (u16 i = 0; i < J3DModelData_getJointNum(anm_model); i++) {
            if ((i == (u32)(s32)m7E4) || (i == (u32)(s32)m7E5)) {
                /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Co1) */
                J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
                u32 n = gabi::load<u32>(gabi::ea(md) + 4);
                u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
                if (i < n)
                    joint += i * 0x1C;
                gabi::store<u32>(joint + 8, 0x02227360 /* nodeCallBack_Co1 */);
            }
        }
        gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
        mAcchCir.SetWall(30.0f, 40.0f);
        mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
        return TRUE;
    }
    mpMorf = nullptr;
    m7E8 = 0;
    return FALSE;
}
VERIFY(0x02227A04, &daNpc_Co1_c::CreateHeap);

/* 02227BB8 */
/* tail call: CreateHeap's result register is passed through */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02227BB8, BOOL, i_this);
    return static_cast<daNpc_Co1_c*>(i_this)->CreateHeap();
}
VERIFY(0x02227BB8, CheckCreateHeap);

/* 02227BBC daNpc_Co1_c::daNpc_Co1_c (fopAcM_ct's constructor, out of line in HD; allocates when
 * this == NULL) */
static daNpc_Co1_c* daNpc_Co1_c_ct(daNpc_Co1_c* i_this) {
    WWHD_FUNC(0x02227BBC, daNpc_Co1_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Co1_c*)operator_new(0xA38);
        if (i_this == nullptr)
            return i_this;
    }
    gabi::call(0x025A1458, i_this); /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
    i_this->__vtbl = CO1_VTBL;
    gabi::call(0x025E7C6C, i_this->mBtkAnm); /* mDoExt_btkAnm::mDoExt_btkAnm */
    gabi::call(0x025E7820, i_this->mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
    gabi::call(0x0259F740, &i_this->mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
    i_this->mA10 = 1.0f;
    return i_this;
}
VERIFY(0x02227BBC, daNpc_Co1_c_ct);

/* 02227C34 */
bool daNpc_Co1_c::charDecide(int) {
    WWHD_FUNC(0x02227C34, bool, this, (int)0);
    mA35 = 0;
    mA34 = 0;
    return true;
}
VERIFY(0x02227C34, &daNpc_Co1_c::charDecide);

/* 02227C48 */
BOOL daNpc_Co1_c::set_action(ProcFunc_l* i_newProcFunc, void* i_argsP) {
    WWHD_FUNC(0x02227C48, BOOL, this, i_newProcFunc, i_argsP);
    ProcFunc_l* cur = &mCurrProcFunc;
    s16 newI = i_newProcFunc->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_newProcFunc->d;
        newF = i_newProcFunc->f;
        if ((u16)cur->d == (u16)newD && cur->f == newF)
            return TRUE;
    } else {
        newF = i_newProcFunc->f;
        newD = i_newProcFunc->d;
        if (cur->i == 0)
            goto set;
    }
    mA36 = 9;
    pmf_call(this, cur, i_argsP);
set:
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mA36 = 0;
    pmf_call(this, cur, i_argsP);
    return TRUE;
}
VERIFY(0x02227C48, &daNpc_Co1_c::set_action);

/* 02227D74 */
bool daNpc_Co1_c::init_CO1_0() {
    WWHD_FUNC(0x02227D74, bool, this);
    bool ret = !dComIfGs_isSymbol(1); /* Din's Pearl not yet obtained */
    if (ret) {
        actor_status = (actor_status & ~0x80u) | 0x4000; /* OffStatus(NOCULLEXEC), OnStatus(0x4000) */
        gabi::Local<ProcFunc_l> pmf;
        pmf_load(pmf, PMF_wait_action1);
        set_action(pmf, nullptr);
    }
    return ret;
}
VERIFY(0x02227D74, &daNpc_Co1_c::init_CO1_0);

/* 02227DFC */
void daNpc_Co1_c::plyTexPttrnAnm() {
    WWHD_FUNC(0x02227DFC, void, this);
    /* the pearl's btk loops */
    u8 btkFrame = (u8)(m868 + 1);
    m868 = btkFrame;
    s32 btkMax = J3DAnm_getFrameMax(m7EC);
    s8 btpNum = mA2C;
    if ((s32)btkFrame >= btkMax) {
        m868 = 0;
    }
    if (btpNum == 6 || btpNum == 5 || !cLib_calcTimer(&mBlinkTimer)) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(m870)) {
            if (mA2C == 6 || mA2C == 5) {
                mBlinkFrame = (u8)J3DAnm_getFrameMax(m870);
            } else {
                s16 t = (s16)gabi::ftoi(cM_rndF(60.0f) + 30.0f);
                mBlinkFrame = 0;
                mBlinkTimer = t;
            }
        }
    }
}
VERIFY(0x02227DFC, &daNpc_Co1_c::plyTexPttrnAnm);

/* 02227F1C */
void daNpc_Co1_c::setAttention(u32 i_setEyePos) {
    WWHD_FUNC(0x02227F1C, void, this, i_setEyePos);
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    if (mA2D == 1) {
        mDoMtx_stack_c::transS(x, y, z);
        mDoMtx_stack_c::YrotM(current.angle.y);
        gabi::Local<cXyz> off;
        off->x = 0.0f;
        off->y = 60.0f;
        off->z = -50.0f;
        PSMTXMultVec(mDoMtx_stack_c::get(), off, attPos);
    } else {
        attPos->z = z;
        attPos->x = x;
        attPos->y = y + l_HIO_attOffsetY();
    }
    if (m9E8 == 0 && !i_setEyePos) {
        return;
    }
    eyePos.z = m980.z;
    eyePos.y = m980.y;
    eyePos.x = m980.x;
}
VERIFY(0x02227F1C, &daNpc_Co1_c::setAttention);

/* 02228014 */
void daNpc_Co1_c::setMtx(u32 param_1) {
    WWHD_FUNC(0x02228014, void, this, param_1);
    if (m9EF == 0) {
        plyTexPttrnAnm();
        m9D8 = (s8)mpMorf->play(&eyePos, 0, 0);
        prlMorf(this)->mFrameCtrl.mFrame = (f32)(s16)gabi::ftoi(mpMorf->getFrame()); /* the pearl follows the body */
        if (mpMorf->getFrame() < m9A4) {
            m9D8 = 1;
        }
        m9A4 = mpMorf->getFrame();
        mObjAcch.CrrPos(dComIfG_Bgsp());
        switch ((u32)(s32)mA2D) {
        case 4:
            m9E3 = 1;
            break;
        case 8:
            m9E3 = !(mpMorf->getFrame() < 15.0f);
            break;
        default:
            m9E3 = 0;
            break;
        }
    }
    /* the ground polygon: cBgS_PolyInfo at m_gnd + 0x14 */
    void* gndPoly = gabi::at<u8>(gabi::ea(&mObjAcch) + 0xD4 + 0x14);
    tevStr.mRoomNo = dBgS_GetRoomId_l(dComIfG_Bgsp(), gndPoly);
    u8 color = dBgS_GetPolyColor_l(dComIfG_Bgsp(), gndPoly);
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, color); /* tevStr.mEnvrIdxOverride */
    mDoMtx_stack_c::transS(x, y, z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();

    /* the pearl hangs from the backbone */
    PSMTXCopy(getAnmMtx(mpMorf->getModel(), m7E5), mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(prlMorf(this)->getModel(), mDoMtx_stack_c::get());
    prlMorf(this)->calc();
    if (m9E3 != 0) { /* item in the right hand */
        PSMTXCopy(getAnmMtx(mpMorf->getModel(), m7E6), mDoMtx_stack_c::get());
        J3DModel* itm = gabi::at<J3DModel>(m86C);
        J3DModel_setBaseTRMtx(itm, mDoMtx_stack_c::get());
        J3DModel_calc(gabi::at<J3DModel>(m86C));
    }
    PSMTXCopy(getAnmMtx(prlMorf(this)->getModel(), m7F0), mDoMtx_stack_c::get());
    Mtx34* m = mDoMtx_stack_c::get();
    f32 px = m->m[0][3];
    mA14 = px;
    f32 py = m->m[1][3];
    mA18 = py;
    f32 pz = m->m[2][3];
    m9F0.x = px;
    mA1C = pz;
    m9F0.y = py;
    m9F0.z = pz;
    /* pearl glow (HIO) */
    u32 hio = l_HIO();
    m9FC.x = gabi::load<s16>(hio + 0x26);
    m9FC.y = gabi::load<s16>(hio + 0x28);
    m9FC.z = gabi::load<s16>(hio + 0x2A);
    s16 ang = (s16)(mA24 + gabi::load<s16>(hio + 0x34));
    mA24 = ang;
    f32 s = std::fabs(cM_ssin(ang));
    mA20 = s;
    f32 lo = gabi::load<f32>(hio + 0x30);
    f32 d = gabi::load<f32>(hio + 0x2C) - lo;
    f32 range = d >= 0.0f ? d : 0.0f;
    f32 v = gabi::fmadds(range, s, lo);
    mA04 = (v - 0.001f) >= 0.0f ? v : 0.001f;
    setAttention(param_1);
}
VERIFY(0x02228014, &daNpc_Co1_c::setMtx);

/* 0222843C */
bool daNpc_Co1_c::createInit() {
    WWHD_FUNC(0x0222843C, bool, this);
    /* l_evn_tbl (.data 0x101BDC50), 3 names */
    for (int i = 0; i < 3; i++) {
        const char* name = gabi::at<const char>(gabi::load<u32>(0x101BDC50 + i * 4));
        mEventIdx[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }
    m98C.copy(current.pos);
    m9EE = 1;
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0x45); /* attention_info.distances[SPEAK] */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0x46); /* attention_info.distances[TALK] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    gravity = -4.5f;
    mEventCut.setActorInfo2(STR(0x10019584) /* "Co1" */, (fopNpc_npc_c*)(void*)this);
    mA2D = 0xB;
    bool init_success;
    switch (mA35) {
    case 0:
        init_success = init_CO1_0();
        break;
    default:
        init_success = false;
        break;
    }
    if (!init_success) {
        return false;
    }
    shape_angle.z = current.angle.z;
    shape_angle.y = current.angle.y;
    shape_angle.x = current.angle.x;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}
VERIFY(0x0222843C, &daNpc_Co1_c::createInit);

/* 02228598 */
cPhs_State daNpc_Co1_c::_create() {
    WWHD_FUNC(0x02228598, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Co1_c): HD out-of-line constructor */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            daNpc_Co1_c_ct(this);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, STR(0x10019598) /* "Co" */);
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    if (!charDecide(gabi::load<u8>(gabi::ea(this) + 0xB3) /* fopAcM_GetParam(this) & 0xFF */)) {
        return cPhs_ERROR_e;
    }
    /* static int a_size_tbl[] (.data 0x101BDC5C): HD reads [0] */
    if (!fopAcM_entrySolidHeap(this, 0x02227BB8 /* CheckCreateHeap */, gabi::load<u32>(0x101BDC5C))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -40.0f, -20.0f, -100.0f, 40.0f, 100.0f, 60.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x02228598, &daNpc_Co1_c::_create);

/* 022286C0 */
static cPhs_State daNpc_Co1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022286C0, cPhs_State, i_this);
    return ((daNpc_Co1_c*)i_this)->_create();
}
VERIFY(0x022286C0, daNpc_Co1_Create);

/* 022286C4 */
BOOL daNpc_Co1_c::_delete() {
    WWHD_FUNC(0x022286C4, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x1001959B) /* "Co" */);
    dKy_plight_cut((LIGHT_INFLUENCE*)(void*)&m9F0); /* HD: the pearl's point light */
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x022286C4, &daNpc_Co1_c::_delete);

/* 02228724 */
static BOOL daNpc_Co1_Delete(daNpc_Co1_c* i_this) {
    WWHD_FUNC(0x02228724, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x02228724, daNpc_Co1_Delete);

/* 02228728 */
void daNpc_Co1_c::checkOrder() {
    WWHD_FUNC(0x02228728, void, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt */) {
        if (dComIfGp_evmng_startCheck(mEventIdx[m9C8])) {
            switch ((u32)(s32)m9C8) {
            case 1:
                mA30 = 0;
                actor_status &= ~0x4000u;
                break;
            case 2:
                mA30 = 0;
                mA33 = 0;
                break;
            default:
                mA30 = 0;
                break;
            }
        }
    } else if (command == 1 /* checkCommandTalk */) {
        if (mA30 == 1 || mA30 == 2) {
            mA30 = 0;
            m9ED = 1;
        }
    }
}
VERIFY(0x02228728, &daNpc_Co1_c::checkOrder);

/* 02228848 */
/* returns the in-demo byte as is (typed u8) */
u8 daNpc_Co1_c::demo() {
    WWHD_FUNC(0x02228848, u8, this);
    if (demoActorID == 0) {
        if (m9EF != 0) {
            m9EF = 0;
            return 0;
        }
        return m9EF;
    }
    u8 id = demoActorID;
    m9EF = 1;
    /* dComIfGp_demo_getActor(demoActorID): HD inline with a range check and the demo object
     * (0x101D5FFC) asserted */
    void* demo_actor = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x100193D8), 0x23A, STR(0x100193C8));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor = gabi::call<void*>(0x02526E70, obj, id); /* dDemo_object_c::getActor */
    }
    if (m870 != 0) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(m870)) {
            mBlinkFrame = (u8)J3DAnm_getFrameMax(m870);
        }
    }
    /* HD: demo_actor is checked for NULL */
    if (demo_actor != nullptr) {
        u32 demopattern = gabi::call<u32>(0x02527828, demo_actor, STR(0x1001959E) /* "Co" */); /* getP_BtpData */
        if (demopattern != 0) {
            m870 = demopattern;
            J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
            if (mDoExt_btpAnm_init(mBtpAnm, md, gabi::at<u8>(demopattern), 1, 2, 1.0f, 0, -1, 1, 0)) {
                mBlinkFrame = 0;
                mA2C = 7;
            }
        }
    }
    gabi::call<BOOL>(0x02527028, this, 0x6A, mpMorf.get(), STR(0x1001959E), 0, 0, 0, 0); /* dDemo_setDemoData */
    return m9EF;
}
VERIFY(0x02228848, &daNpc_Co1_c::demo);

/* 022289FC */
s32 daNpc_Co1_c::isEventEntry() {
    WWHD_FUNC(0x022289FC, s32, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x022289FC, &daNpc_Co1_c::isEventEntry);

/* 02228A3C */
u32 daNpc_Co1_c::setAnm_tex(s8 i_btpNum) {
    WWHD_FUNC(0x02228A3C, u32, this, i_btpNum);
    if (mA2C != i_btpNum) {
        mA2C = i_btpNum;
        return iniTexPttrnAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x02228A3C, &daNpc_Co1_c::setAnm_tex);

/* 02228A5C */
u32 daNpc_Co1_c::anmNum_toResID(int i_anmNum) {
    WWHD_FUNC(0x02228A5C, u32, this, i_anmNum);
    /* a_bck_resID_tbl (.data 0x100195A4) */
    return gabi::load<u32>(0x100195A4 + i_anmNum * 4);
}
VERIFY(0x02228A5C, &daNpc_Co1_c::anmNum_toResID);

/* 02228A70 */
u32 daNpc_Co1_c::anmNum_toResID_prl(int i_anmNum) {
    WWHD_FUNC(0x02228A70, u32, this, i_anmNum);
    /* the pearl's bck table (.data 0x100195D0) */
    return gabi::load<u32>(0x100195D0 + i_anmNum * 4);
}
VERIFY(0x02228A70, &daNpc_Co1_c::anmNum_toResID_prl);

/* 02228A84 */
BOOL daNpc_Co1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    WWHD_FUNC(0x02228A84, BOOL, this, i_anmPrmP);
    s8 anmNum = i_anmPrmP->mAnmNum;
    if (mA2D == anmNum) {
        return TRUE;
    }
    mA2D = anmNum;
    u32 resID = anmNum_toResID(anmNum);
    {
        mDoExt_McaMorf* morf = mpMorf;
        f32 morfF = i_anmPrmP->mMorf;
        f32 speed = i_anmPrmP->mSpeed;
        s32 loopMode = i_anmPrmP->mLoopMode;
        dNpc_setAnmIDRes(morf, loopMode, morfF, speed, resID, -1, STR(0x100195FC) /* "Co" */);
    }
    u32 prlID = anmNum_toResID_prl(mA2D);
    {
        mDoExt_McaMorf* morf = prlMorf(this);
        s32 loopMode = i_anmPrmP->mLoopMode;
        dNpc_setAnmIDRes(morf, loopMode, 0.0f, 0.0f, prlID, -1, STR(0x100195FC));
    }
    m9A4 = 0.0f;
    m9D8 = 0;
    m9D9 = 0;
    return TRUE;
}
VERIFY(0x02228A84, &daNpc_Co1_c::setAnm_anm);

/* 02228B70 */
bool daNpc_Co1_c::setAnm() {
    WWHD_FUNC(0x02228B70, bool, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BDC60);
    if (a_anm_prm_tbl[mA31].mBtpNum >= 0) {
        setAnm_tex(a_anm_prm_tbl[mA31].mBtpNum);
    }
    if (a_anm_prm_tbl[mA31].mAnmNum >= 0) {
        setAnm_anm(&a_anm_prm_tbl[mA31]);
    }
    return true;
}
VERIFY(0x02228B70, &daNpc_Co1_c::setAnm);

/* 02228BF4 */
void daNpc_Co1_c::setStt(s8 i_status) {
    WWHD_FUNC(0x02228BF4, void, this, i_status);
    m9CC = 0;
    mA31 = i_status;
    switch ((u32)(s32)i_status) {
    case 2:
        mA2A = 0xFF;
        mA33 = 1;
        break;
    case 7:
        mA33 = 0;
        break;
    }
    setAnm();
}
VERIFY(0x02228BF4, &daNpc_Co1_c::setStt);

/* 02228C38 */
void daNpc_Co1_c::endEvent() {
    WWHD_FUNC(0x02228C38, void, this);
    dComIfGp_event_reset();
    mA2A = 0xFF;
}
VERIFY(0x02228C38, &daNpc_Co1_c::endEvent);

/* 02228C78 */
void daNpc_Co1_c::setAnm_NUM(int i_anmNum, int i_setTex) {
    WWHD_FUNC(0x02228C78, void, this, i_anmNum, i_setTex);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BDCE0);
    if (i_setTex != 0) {
        setAnm_tex(a_anm_prm_tbl[i_anmNum].mBtpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[i_anmNum]);
}
VERIFY(0x02228C78, &daNpc_Co1_c::setAnm_NUM);
