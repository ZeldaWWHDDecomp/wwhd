/**
 * d_a_npc_so.cpp (WWHD)
 * NPC - Fishman: create, heap, node control, searches, orders, execute, HIO and statics
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_so.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_so.h"
#include "d/d_jnt_hit.h"
#include "gc_api/gc_api.h"

const gc_tu_data gc_tu = {.safeString = 0x10021B78,
                          .safeStringAssure = 0x022DDC74,
                          .objAcch = {0x10021B90, 0x10021BB0, 0x10021BA0},
                          .objAcchDt = {0x10021BA0, 0x10021BB0}};
enum : u32 {
    SO_HIO_VTBL = 0x10021BC0,
    SO_BASE_POLY_VTBL = 0x10021960,
    SO_BASE_GROUP_VTBL = 0x10021970,
    SO_ACTOR_VTBL = SO_VTBL
};
static daNpc_So_HIO_c l_HIO;
// HD: serialized GHS descriptors reference methods in the separately verified mode source.
static SoModeData mode_data[16] = {
    {{0x0000FFFF, 0x022E0048, 0x0000FFFF, 0x022E4BA0}, "WAIT"},
    {{0x0000FFFF, 0x022E0084, 0x0000FFFF, 0x022E00B8}, "HIDE"},
    {{0x0000FFFF, 0x022E00D4, 0x0000FFFF, 0x022E01D8}, "JUMP"},
    {{0x0000FFFF, 0x022E0324, 0x0000FFFF, 0x022E03A4}, "SWIM"},
    {{0x0000FFFF, 0x022E0660, 0x0000FFFF, 0x022E06A4}, "NEAR_SWIM"},
    {{0x0000FFFF, 0x022E09B0, 0x0000FFFF, 0x022E09CC}, "EVENT_FIRST_WAIT"},
    {{0x0000FFFF, 0x022E0ADC, 0x0000FFFF, 0x022E0C80}, "EVENT_FIRST"},
    {{0x0000FFFF, 0x022E0D38, 0x0000FFFF, 0x022E0D70}, "EVENT_FIRST_END"},
    {{0x0000FFFF, 0x022E0E28, 0x0000FFFF, 0x022E0E60}, "EVENT_ESA"},
    {{0x0000FFFF, 0x022E0EF4, 0x0000FFFF, 0x022E0F10}, "EVENT_MAPOPEN"},
    {{0x0000FFFF, 0x022E0FD4, 0x0000FFFF, 0x022E100C}, "EVENT_BOW"},
    {{0x0000FFFF, 0x022E116C, 0x0000FFFF, 0x022E11AC}, "TALK"},
    {{0x0000FFFF, 0x022E1204, 0x0000FFFF, 0x022E12B4}, "DISAPPEAR"},
    {{0x0000FFFF, 0x022E136C, 0x0000FFFF, 0x022E4BA4}, "DEBUG"},
    {{0x0000FFFF, 0x022E4BA8, 0x0000FFFF, 0x022E140C}, "GETRUPEE"},
    {{0x0000FFFF, 0x022E1508, 0x0000FFFF, 0x022E1548}, "EVENT_TRIFORCE"}};
static SoModelSystem modelSystem;
static Mtx34 currentMtx;
static dCcD_SrcCyl dNpc_cyl_src;
static dCcD_SrcSph m_sph_src;
static __jnt_hit_data_c search_data[2] = {{0, 4, 2.0, 0x101C62E4}, {0, 8, 2.0, 0x101C62E4}};
static s32 a_anm_bcks_tbl[5] = {9, 6, 7, 5, 8};
static dLib_anm_prm_c a_anm_prm_tbl[6] = {{0, -1, 0, 8.0, 1.0, 2}, {0, -1, 0, 8.0, 1.0, 2},
                                          {1, -1, 0, 8.0, 1.0, 2}, {2, -1, 0, 8.0, 1.0, 2},
                                          {3, -1, 0, 8.0, 1.0, 2}, {4, -1, 0, 8.0, 1.0, 2}};
static const char* a_demo_name_tbl[6] = {"SO_1ST_MEET", "SO_1ST_MEET_END", "SO_MAPOPEN",
                                         "SO_BOW",      "SO_GET_RUPEE",    "SO_TRIFORCE_CHECK"};
static cXyz ripple_scale;
static s32 ripple_guard;
static SoHeaderStatics headerStatics;
static SoDtorRecords headerDtorRecords;
inline SoRipple *soRipple(daNpc_So_c *p) { return reinterpret_cast<SoRipple *>(&p->mAE8); }
inline SoBtpPrefix *soBtp(daNpc_So_c *p) { return reinterpret_cast<SoBtpPrefix *>(&p->mBtpAnm); }
inline SoModelFlags *soFlags(J3DModel *p) { return reinterpret_cast<SoModelFlags *>(p); }
/* inlined into createInit and _create */
static inline bool so_isStartStage(const char *name) {
    gabi::Local<SoString> sa;
    sa->text.v = gabi::guest_addr(name);
    sa->vt.v = gc_tu.safeString;
    const char *stage = dComIfGp_getStartStageName();
    gabi::Local<SoString> sb;
    sb->text = reinterpret_cast<const be<u8> *>(stage);
    sb->vt.v = gc_tu.safeString;
    soTerminate(sa);
    soTerminate(sa);
    const be<u8> *first = sa->text;
    soTerminate(sb);
    const be<u8> *second = sb->text;
    if (first == second)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = first[i];
        if (c != second[i])
            return false;
        if (!c)
            return true;
    }
    return false;
}
inline void *so_getObjectRes(s32 idx) { return dComIfG_getObjectRes("So", idx); }
inline void so_setJointCallBack(J3DModelData *data, u16 idx, J3DJointCallBack cb) {
    data->getJointNodePointer(idx)->setCallBack(cb);
}
inline Mtx34 *so_getAnmMtx(J3DModel *model, s32 n) { return model->getAnmMtx(n); }
inline void so_setAnmMtx(J3DModel *model, s32 n, const Mtx34 *m) {
    mtx_copy(model->getAnmMtx(n), m);
}
inline J3DModel *j3dSys_getModel() { return modelSystem.mModel; }
inline Mtx34 *J3DSys_mCurrentMtx() { return &currentMtx; }
/* 022E4B98 */
static BOOL daNpc_SoIsDelete(daNpc_So_c *i_this) { return TRUE; }

/* 022DDBD8 */
static void fopNpc_npc_c_dt(fopNpc_npc_c *i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        soCircleDtor(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        i_this->mObjAcch.dBgS_GrpPassChk::__vtbl = SO_BASE_POLY_VTBL;
        i_this->mObjAcch.dBgS_PolyPassChk::__vtbl = SO_BASE_GROUP_VTBL;
        soAcchDtor(&i_this->mObjAcch, 0);
        soActorDtor(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 022DDC74 */
static void soAssureTermination(void *unused) {}

/* 022DDC78 */
void *daNpc_So_c::_searchEsa(fopAc_ac_c *arg1) {
    if (arg1 != nullptr && fpcM_GetName(arg1) == fpcNm_ESA_e) {
        if (static_cast<SoEsa *>(arg1)->field_0x298 == 0) { /* esa->field_0x298 */
            return arg1;
        }
    }
    return nullptr;
}

/* 022DDCA0 */
static void *searchEsa_CB(void *arg1, void *i_this) {
    return static_cast<daNpc_So_c *>(i_this)->_searchEsa(static_cast<fopAc_ac_c *>(arg1));
}

/* 022DDCB0 */
void daNpc_So_c::_nodeControl(J3DNode *pNode, J3DModel *pModel) {
    s32 jntNo = reinterpret_cast<SoJointIndex *>(J3DNode_toJoint(pNode))->mJntNo; /* getJntNo() */
    PSMTXCopy(so_getAnmMtx(pModel, jntNo), mDoMtx_stack_c::get());
    if (jntNo == (s32)(s8)m_jnt.mHeadJntNum) {
        gabi::Local<cXyz> local_2C;
        gabi::Local<cXyz> local_38;
        local_2C->x = 0.0f;
        local_38->y = -16.0f;
        local_38->x = 24.0f;
        local_2C->y = 0.0f;
        local_38->z = 0.0f;
        local_2C->z = 0.0f;
        /* HD: only for the main model (the second morf shares the joint callbacks) */
        if (pModel == mpMorf->getModel()) {
            PSMTXMultVec(mDoMtx_stack_c::get(), local_2C.get(), &mB60);
            mDoMtx_YrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[0][1]); /* getHead_y() */
            mDoMtx_ZrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[0][0]); /* getHead_x() */
            PSMTXMultVec(mDoMtx_stack_c::get(), local_38.get(), &mB54);
        }
    } else if (jntNo == (s32)(s8)m_jnt.mBackboneJntNum) {
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[1][1]); /* getBackbone_y() */
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[1][0]); /* getBackbone_x() */
    }
    PSMTXCopy(mDoMtx_stack_c::get(), J3DSys_mCurrentMtx());
    so_setAnmMtx(pModel, jntNo, mDoMtx_stack_c::get());
}

/* 022DDED8 */
static BOOL nodeControl_CB(J3DNode *pNode, int calcTiming) {
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel *model = j3dSys_getModel();
        daNpc_So_c *i_this = static_cast<daNpc_So_c *>(model->getUserArea()); /* getUserArea() */
        if (i_this != nullptr) {
            i_this->_nodeControl(pNode, j3dSys_getModel());
        }
    }
    return TRUE;
}

/* 022DDF20 */
void *daNpc_So_c::_searchTagSo(fopAc_ac_c *arg1) {
    if (arg1 != nullptr && fpcM_GetName(arg1) == fpcNm_TAG_SO_e) {
        SoTag *tag = static_cast<SoTag *>(arg1);
        /* mA79 == tag_so->getRndNum() && tag_so->isTag() */
        if (tag->mRndNum == mA79 && tag->mType != 1) {
            mA7C = tag->mJumpRange; /* getJumpRange() */
            u32 x = soWords(&tag->current.pos)->x;
            soWords(&mA80)->x = x;
            u32 y = soWords(&tag->current.pos)->y;
            soWords(&mA80)->y = y;
            u32 z = soWords(&tag->current.pos)->z;
            soWords(&current.pos)->x = x;
            soWords(&current.pos)->z = z;
            soWords(&current.pos)->y = y;
            soWords(&mA80)->z = z;
            return arg1;
        }
    }
    return nullptr;
}

/* 022DDF8C */
static void *searchTagSo_CB(void *arg1, void *i_this) {
    return static_cast<daNpc_So_c *>(i_this)->_searchTagSo(static_cast<fopAc_ac_c *>(arg1));
}

/* 022DDF9C */
void *daNpc_So_c::_searchMinigameTagSo(fopAc_ac_c *arg1) {
    if (arg1 != nullptr && fpcM_GetName(arg1) == fpcNm_TAG_SO_e) {
        SoTag *tag = static_cast<SoTag *>(arg1);
        if (tag->mType == 1) { /* isMinigame() */
            soWords(&mB90)->x = soWords(&tag->current.pos)->x;
            soWords(&mB90)->y = soWords(&tag->current.pos)->y;
            soWords(&mB90)->z = soWords(&tag->current.pos)->z;
            mBAE = 1;
            mB9C = tag->shape_angle.y; /* shape_angle.y */
            return arg1;
        }
    }
    return nullptr;
}

/* 022DDFF4 */
static void *searchMinigameTagSo_CB(void *arg1, void *i_this) {
    return static_cast<daNpc_So_c *>(i_this)->_searchMinigameTagSo(static_cast<fopAc_ac_c *>(arg1));
}

/* 022DE004 */
s16 daNpc_So_c::XyCheckCB(int arg1) {
    if (fopAcM_Search(searchEsa_CB, this) != nullptr) {
        return 0;
    }
    /* HD: no isAnm(2) check */
    if (dComIfGp_checkPlayerStatus0(
            0, daPyStts0_SHIP_RIDE_e) /* checkPlayerStatus0(0, daPyStts0_SHIP_RIDE_e) */
        && dComIfGp_getSelectItem(arg1) == dItemNo_BIRD_BAIT_5_e) {
        return 1;
    }
    return 0;
}

/* 022DE084 */
static s16 daNpc_So_XyCheckCB(void *i_this, int arg2) {
    return static_cast<daNpc_So_c *>(i_this)->XyCheckCB(arg2);
}

/* 022DE088 */
s16 daNpc_So_c::XyEventCB(int arg1) {
    mBDC = dComIfGp_evmng_getEventIdx("SO_ESA_XY" /* "SO_ESA_XY" */, 0xFF);
    return mBDC;
}

/* 022DE0CC */
static s16 daNpc_So_XyEventCB(void *i_this, int arg2) {
    return static_cast<daNpc_So_c *>(i_this)->XyEventCB(arg2);
}

/* 022DE0D0 */
bool daNpc_So_c::jntHitCreateHeap() {
    /* search_data (.data search_data, 2 entries) */
    u32 jh = soCreateJntHit(mpMorf->getModel(), search_data, 2); /* JntHit_create */
    mJntHit = jh;
    if (jh != 0) {
        jntHit = jh; /* jntHit */
        return true;
    }
    return false;
}

/* 022DE410 */
static BOOL createHeap_CB(fopAc_ac_c *i_this) {
    return static_cast<daNpc_So_c *>(i_this)->_createHeap();
}

/* 022DE414 */
void daNpc_So_c::getArg() {
    m6D0 = home.angle.x;
    if ((s32)m6D0 == 0xFFFF ||
        m6D0 == 0) { /* an s16 never equals 0xFFFF: only the 0 test remains */
        m6D0 = 1;
    }
}

/* 022DE6AC */
void daNpc_So_c::offsetZero() {
    mB38.x = -1.0f;
    mB38.y = 20.0f;
    mB38.z = 0.1f;
}

/* 022DEFE8 */
static cPhs_State daNpc_SoCreate(void *i_this) {
    return static_cast<daNpc_So_c *>(i_this)->_create();
}

/* 022DEFEC */
bool daNpc_So_c::_delete() {
    if (dComIfGp_getMiniGameType() == 8) { /* dComIfGp_getMiniGameType() */
        dComIfGp_endMiniGame(0x80);        // HD: the flag bit is distinct from the minigame type.
    }
    dComIfG_resDelete(&mPhase, "So");
    soEndRipple(&mAE8); /* dPa_rippleEcallBack::end */
    return true;
}

/* 022DF05C */
static BOOL daNpc_SoDelete(void *i_this) { return static_cast<daNpc_So_c *>(i_this)->_delete(); }

/* 022DFBD4 */
static BOOL daNpc_SoExecute(void *i_this) { return soExecute(i_this); /* _execute */ }

/* soGetWaterY(pos, acch): GHS passes the f1 result on unrounded */
static inline f64 soGetWaterY(cXyz *pos, dBgS_ObjAcch *acch) { return soWaterY(pos, acch); }
static inline f32 so_faddsD(f64 a, f32 b) { return (f32)(a + (f64)b); }
/* cM_rndF: the f1 result is compared unrounded */
static inline f64 so_rndF_d(f32 max) { return soRandom(max); }

/* 022DE434 */
void daNpc_So_c::setMtx() {
    J3DModel *model = mpMorf->getModel();
    J3DModel *m = model;
    f32 sz = scale.z;
    f32 sx = scale.x;
    f32 sy = scale.y;
    m->mBaseScale.x = sx; /* setBaseScale(scale) */
    m->mBaseScale.y = sy;
    m->mBaseScale.z = sz;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mDoMtx_stack_c::transM(0.0f, mB34, 0.0f);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    /* HD: the second morf is placed on the water surface; flat (y scale 0.1) while the fishman
     * is not tilted */
    f32 x = scale.x;
    s16 ax = shape_angle.x;
    J3DModel *model2 = mA74->getModel();
    f32 z = scale.z;
    J3DModel *m2 = model2;
    if (ax == 0) {
        m2->mBaseScale.x = x;
        m2->mBaseScale.z = z;
        m2->mBaseScale.y = 0.1f;
    } else {
        m2->mBaseScale.x = x;
        m2->mBaseScale.y = scale.y;
        m2->mBaseScale.z = z;
    }
    f32 y = so_faddsD(soGetWaterY(&current.pos, &mAcch), 15.0f);
    mDoMtx_stack_c::transS(current.pos.x, y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(model2, mDoMtx_stack_c::get());
}

/* 022DE6D4 */
void daNpc_So_c::setAnm(s8 anmPrmIdx, u32 force) {
    if (anmPrmIdx != 6) {
        mAnmPrmIdx = anmPrmIdx;
    }
    mDoExt_McaMorf *morf = mpMorf;
    f32 end = gabi::fsubs_ppc((f32)morf->mFrameCtrl.getEnd(), 1.0f); /* getEndFrame() - 1.0f */
    f32 frame = morf->getFrame();                                    /* getFrame() */
    if (!(frame < end)) {
        f64 rnd = so_rndF_d(100.0f);
        if (rnd < (f64)l_HIO.m5C) { /* bge skips on NaN */
            if (mAnmPrmIdx == 5) {
                mAnmPrmIdx = 3;
            } else if (mAnmPrmIdx == 3) {
                mAnmPrmIdx = 5;
            }
        }
    }
    /* HD: dLib_bcks_setAnm also takes the second morf */
    soSetAnm("So", mpMorf.get(), &mBckIdx, &mAnmPrmIdx, &mOldAnmPrmIdx, a_anm_bcks_tbl,
             a_anm_prm_tbl, force, mA74.get());
}

/* 022DE7D8 */
void daNpc_So_c::modeProc(Proc_e mode, int index) {
    if (mode == PROC_INIT_e) {
        m6CC = index;
        (this->*reinterpret_cast<SoModeEntry *>(&mode_data[index])->init)();
    } else if (mode == PROC_RUN_e) {
        (this->*reinterpret_cast<SoModeEntry *>(&mode_data[m6CC])->run)();
    }
}

/* 022DE13C */
BOOL daNpc_So_c::_createHeap() {
    J3DModelData *modelData = (J3DModelData *)so_getObjectRes(dRes_INDEX_SO_BDL_SO_e);
    if (modelData == nullptr) {
        JUT_ASSERT_fail("d_a_npc_so.cpp", 0x208, "modelData != (0)" /* "modelData != 0" */);
    }
    mpMorf =
        mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, -1 /* ~EMode_NONE */,
                               1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020022);
    if (mpMorf == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea(this) */
    /* HD: a second morf of the same model data */
    mA74 = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1,
                                  nullptr, 0x80000, 0x11020022);
    if (mA74 == nullptr || mA74->getModel() == nullptr) {
        return FALSE;
    }
    mA74->getModel()->mUserArea = gabi::ea(this);
    {
        J3DModel *m = mA74->getModel();
        u32 f = soFlags(m)->mFlags & ~6u;
        soFlags(m)->mFlags = f;
        soSetPacketFlags(m, f); /* J3DModel (HD): the flags of every material packet */
    }
    {
        J3DModel *m = mpMorf->getModel();
        u32 f = soFlags(m)->mFlags & ~1u;
        soFlags(m)->mFlags = f;
        soSetPacketFlags(m, f);
    }
    void *btp = so_getObjectRes(dRes_INDEX_SO_BTP_SO_e);
    if (btp == nullptr) {
        JUT_ASSERT_fail("d_a_npc_so.cpp", 0x22B, "btp != 0" /* "btp != 0" */);
    }
    /* mBtpAnm.init(modelData, btp, 1, 0, 1.0f, 0, -1, false, 0) */
    if (!soBtpInit(&mBtpAnm, modelData, btp, 1, 0, 1.0f, 0, -1, 0, 0)) {
        return FALSE;
    }
    m_jnt.mHeadJntNum = SO_JNT_HEAD_e;
    m_jnt.mBackboneJntNum = SO_JNT_BACKBONE_e;
    so_setJointCallBack(modelData, SO_JNT_HEAD_e, nodeControl_CB);
    so_setJointCallBack(modelData, SO_JNT_BACKBONE_e, nodeControl_CB);
    modelData = (J3DModelData *)so_getObjectRes(dRes_INDEX_SO_BDL_SO_FUDE_e);
    if (modelData == nullptr) {
        JUT_ASSERT_fail("d_a_npc_so.cpp", 0x23C, "modelData != (0)");
    }
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (mpModel == nullptr) {
        return FALSE;
    }
    if (!jntHitCreateHeap()) {
        return FALSE;
    }
    return TRUE;
}

/* createInit's common tail (GHS duplicates it after each modeProcInit) */
static inline void so_createInit_tail(daNpc_So_c *t) {
    t->mBE0 = 30;
    t->mAcchCir.SetWall(30.0f, 30.0f);
    t->mAcch.Set(&t->current.pos, &t->old.pos, t, 1, &t->mAcchCir, &t->speed);
    t->mAcch.m_flags = t->mAcch.m_flags | 0xC; /* SetWallNone(), SetRoofNone() */
    J3DModel *model = t->mpMorf->getModel();
    u32 mtx = gabi::ea(model->getBaseTRMtx());
    f32 sx = t->scale.x;
    f32 lo = gabi::fmuls_ppc(-100.0f, sx);
    f32 hi = gabi::fmuls_ppc(100.0f, sx);
    t->cullMtx = mtx; /* model->getBaseTRMtx() */
    fopAcM_setCullSizeBox(t, lo, lo, lo, hi, hi, hi);
    t->attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0x22; /* attention_info.distances[TALK] */
    t->attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] =
        0x22; /* attention_info.distances[SPEAK] */
    t->cullSizeFar = 10.0f;
    t->gravity = -2.5f;
    t->eventInfo.setXyCheckCB(daNpc_So_XyCheckCB); /* eventInfo.setXyCheckCB(daNpc_So_XyCheckCB) */
    t->eventInfo.setXyEventCB(daNpc_So_XyEventCB); /* eventInfo.setXyEventCB(daNpc_So_XyEventCB) */
    t->mEventCut.setActorInfo2("NpcSo" /* "NpcSo" */, t);
}

/* 022DE884 */
void daNpc_So_c::createInit() {
    mBDA = 0;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&dNpc_cyl_src /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    mStts2.Init(0xFF, 0xFF, this);
    mSph.Set(&m_sph_src /* m_sph_src */);
    mSph.SetStts(&mStts2);
    current.pos.y = gabi::fsubs_ppc(current.pos.y, 500.0f);
    setMtx();
    mpMorf->calc();
    mA74->calc(); /* HD */
    u32 z = soWords(&current.pos)->z;
    u32 x = soWords(&current.pos)->x;
    u32 y = soWords(&current.pos)->y;
    soWords(&mAAC)->x = x;
    soWords(&mAAC)->y = y;
    soWords(&mAAC)->z = z;
    offsetZero();
    setAnm(1, 0);
    mA79 = (u8)gabi::ftoi(soRandom(4.9f)); /* cM_rndF(4.9f) */

    bool firstWait = false;
    if (!dComIfGs_isEventBit(0x0901)) {
        if (so_isStartStage("sea") && current.roomNo == dIsleRoom_DragonRoostIsland_e &&
            dComIfGs_isStageBossEnemy(3 /* STAGE_DRC */)) {
            firstWait = true;
        }
    }
    if (firstWait) {
        modeProc(PROC_INIT_e, MODE_EVENT_FIRST_WAIT_e);
        so_createInit_tail(this);
        return;
    }
    bool triforce = false;
    if (so_isStartStage("sea") && current.roomNo == dIsleRoom_GaleIsle_e &&
        dComIfGs_isStageBossEnemy(7 /* STAGE_WT */) &&
        dComIfGs_isCollect(0, 3) /* dComIfGs_isCollect(0, 3) */ && !dComIfGs_isEventBit(0x3A20)) {
        triforce = true;
    }
    if (triforce) {
        modeProc(PROC_INIT_e, MODE_EVENT_TRIFORCE_e);
    } else {
        attention_info.flags =
            fopAc_Attn_TALKFLAG_NOTALK_e | fopAc_Attn_ACTION_SPEAK_e |
            fopAc_Attn_LOCKON_TALK_e; /* attention_info.flags: TALKFLAG_NOTALK | ACTION_SPEAK | LOCKON_TALK */
        modeProc(PROC_INIT_e, MODE_HIDE_e);
    }
    so_createInit_tail(this);
}

/* 022DED84 */
cPhs_State daNpc_So_c::_create() {
    /* fopAcM_ct(this, daNpc_So_c): inline constructor */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this)) {
            soNpcCtor(this); /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
            __vtbl = SO_VTBL;
            dCcD_Stts_ct(&mStts2);
            soSphCtor(&mSph);    /* dCcD_Sph::dCcD_Sph */
            soBtpCtor(&mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dBgS_ObjAcch_ct(&mAcch, gc_tu.objAcch);
            dBgS_AcchCir_ct(&mAcchCir);
            soRippleCtor(&mAE8); /* dPa_rippleEcallBack::dPa_rippleEcallBack */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhase, "So");
    if (state == cPhs_COMPLEATE_e) {
        getArg();
        bool err = false;
        if (so_isStartStage("sea") && current.roomNo == dIsleRoom_ToweroftheGods_e &&
            !dComIfGs_isEventBit(0x1E40)) {
            err = true;
        }
        if (err) {
            return cPhs_ERROR_e;
        }
        if (!dComIfGs_isStageBossEnemy(3 /* STAGE_DRC */)) {
            return cPhs_ERROR_e;
        }
        if (!fopAcM_entrySolidHeap(this, createHeap_CB, 0x1C00 /* m_heapsize */)) {
            return cPhs_ERROR_e;
        }
        createInit();
    }
    return state;
}

/* 022DF060 */
void daNpc_So_c::setScale() {
    /* fopAcM_searchActorDistanceXZ(this, dComIfGp_getPlayer(0)): the f1 result is used unrounded */
    f64 dist = soActorDistance(this, dComIfGp_getPlayer(0));
    f32 v;
    if (dist > (f64)l_HIO.m48) {
        f32 m48 = l_HIO.m48;
        f32 tmp2 = gabi::fsubs_ppc(100000.0f, m48);
        f32 tmp = (f32)(dist - (f64)m48);
        tmp2 = tmp2 / l_HIO.m4C;
        v = gabi::fadds_ppc(tmp / tmp2, 1.0f);
        mB08 = v;
        f32 m4C = l_HIO.m4C;
        if (v > m4C) {
            v = m4C;
            mB08 = v;
        }
    } else {
        v = l_HIO.m44;
        mB08 = v;
    }
    if (m6CC == MODE_EVENT_TRIFORCE_e) {
        scale.y = 1.0f;
        v = 1.0f;
        scale.z = 1.0f;
        scale.x = 1.0f;
        mB08 = 1.0f;
    }
    gabi::Local<cXyz> tmp;
    tmp->z = v;
    tmp->x = v;
    tmp->y = v;
    soAddCalcPos(&scale, tmp.get(), 0.1f, 0.5f); /* cLib_addCalcPos2(&scale, tmp, 0.1f, 0.5f) */
}

/* 022DF148 */
void daNpc_So_c::setAttention() {
    cXyz *att = &attention_info.position; /* attention_info.position */
    f32 y = mB60.y;
    att->y = y;
    att->x = mB60.x;
    att->z = mB60.z;
    att->y = gabi::fadds_ppc(y, l_HIO.mNpc.mAttnYOffset /* mNpc.mAttnYOffset */);
    f64 water = soGetWaterY(att, &mAcch);
    if (!(f32(att->y) > water)) { /* bgt skips on NaN */
        att->y = (f32)water;
    }
    u32 z = soWords(&mB54)->z;
    u32 yy = soWords(&mB54)->y;
    soWords(&eyePos)->z = z;
    soWords(&eyePos)->y = yy;
    soWords(&eyePos)->x = soWords(&mB54)->x;
}

/* dNpc_playerEyePos(offset): the result goes to a hidden out-pointer */
static inline void so_playerEyePos(cXyz *out, f32 ofs) { soPlayerEye(out, ofs); }
static inline void so_copyWords3_yxz(cXyz *dstVector, cXyz *srcVector) {
    SoVectorWords *src = soWords(srcVector);
    SoVectorWords *dst = soWords(dstVector);
    u32 z = src->z;
    u32 x = src->x;
    u32 y = src->y;
    dst->y = y;
    dst->x = x;
    dst->z = z;
}
/* lookBack's tail: m_jnt.lookAtTarget(&shape_angle.y, &mB44, mB54 + (0, 200, 0), shape_angle.y, mB50, mBDA) */
static inline void so_lookAt(daNpc_So_c *t) {
    gabi::Local<cXyz> tmp;
    s16 vel = t->mB50;
    u8 headOnly = t->mBDA;
    f32 y = gabi::fadds_ppc(t->mB54.y, 200.0f);
    tmp->y = y;
    tmp->z = t->mB54.z;
    tmp->x = t->mB54.x;
    soLookAtTarget(&t->m_jnt, &t->shape_angle.y, &t->mB44, tmp.get(), (s32)t->shape_angle.y,
                   (s32)vel, (u32)headOnly);
}

/* 022DF1CC */
void daNpc_So_c::lookBack() {
    dComIfGp_ea();
    if (dComIfGp_event_runCheck() != 0) { /* dComIfGp_event_runCheck() */
        if (mEventCut.mbAttention != 0) { /* getAttnFlag() */
            if (mAnmPrmIdx == 4) {        /* isAnm(4) */
                m_jnt.mbTrn = 0;          /* clrTrn() */
                mB50 = 0;
                so_lookAt(this);
                return;
            }
            f32 x = mEventCut.mPos.x; /* mB44 = getAttnPos() */
            f32 z = mEventCut.mPos.z;
            mB44.x = x;
            s16 turn = mEventCut.mTurnSpeed;
            f32 y = mEventCut.mPos.y;
            mB44.z = z;
            mB44.y = y;
            m_jnt.mbTrn = 1;                         /* setTrn() */
            s16 target = l_HIO.mNpc.mMaxHeadTurnVel; /* mNpc.mMaxHeadTurnVel */
            if (turn != 0)
                target = turn;
            soAddCalcAngle(&mB50, (s32)target, 4, 0x800); /* cLib_addCalcAngleS2 */
            so_lookAt(this);
            return;
        }
        gabi::Local<cXyz> eye;
        so_playerEyePos(eye.get(), l_HIO.mNpc.m04 /* mNpc.m04 */);
        so_copyWords3_yxz(&mB44, eye.get());
    } else {
        m_jnt.mbTrn = 0; /* clrTrn() */
        gabi::Local<cXyz> eye;
        so_playerEyePos(eye.get(), l_HIO.mNpc.m04);
        so_copyWords3_yxz(&mB44, eye.get());
    }
    if (m_jnt.mbTrn != 0) { /* trnChk() */
        s16 turn = mEventCut.mTurnSpeed;
        s16 target = l_HIO.mNpc.mMaxHeadTurnVel;
        if (turn != 0)
            target = turn;
        soAddCalcAngle(&mB50, (s32)target, 4, 0x800); /* cLib_addCalcAngleS2 */
    } else {
        mB50 = 0;
    }
    so_lookAt(this);
}

/* eventInfo (HD): mCommand u16 at +0xF8, mCondition u16 at +0xFA */
static inline u16 so_evCommand(fopAc_ac_c *a) { return a->eventInfo.mCommand; }
static inline void so_evOnCondition(fopAc_ac_c *a, u16 c) { a->eventInfo.onCondition(c); }
static inline BOOL so_event_chkTalkXY() {
    return u32(reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get())->play.mEvtCtrl.mTalkButton - 1) <=
           3;
}

/* 022DF3F0 */
void daNpc_So_c::checkOrder() {
    u16 cmd = so_evCommand(this);
    if (cmd == dEvtCmd_INDEMO_e) { /* checkCommandDemoAccrpt() */
        mB70 = 0;
    } else if (cmd == dEvtCmd_INTALK_e && (mB70 == 1 || mB70 == 2)) {
        if (so_event_chkTalkXY()) {
            modeProc(PROC_INIT_e, MODE_EVENT_ESA_e);
        }
        mB70 = 0;
    }
}

// Event names follow the GameCube table.
static inline const char *so_demoName(s32 i) { return a_demo_name_tbl[i]; }

/* 022DF46C */
void daNpc_So_c::eventOrder() {
    u8 b = mB70;
    if (b == 1 || b == 2) {
        so_evOnCondition(this,
                         dEvtCnd_CANTALK_e |
                             dEvtCnd_CANTALKITEM_e); /* dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e */
        if (mB70 == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if ((u32)(b - 4) <= 2) { /* mB70 == 5 || mB70 == 4 || mB70 == 6 */
        soOrderChangeEvent(this, so_demoName(b - 3), 0, 0xFFFF); /* fopAcM_orderChangeEvent */
    } else if (b == 7) {
        if (REG_S(12, 9) == 0) {                                 /* REG12_S(9) */
            soOrderChangeEvent(this, so_demoName(4), 0, 0xFFFF); /* fopAcM_orderChangeEvent */
            so_evOnCondition(this, dEvtCnd_CANGETITEM_e);        /* dEvtCnd_CANGETITEM_e */
        } else {
            soOrderOtherEvent(this, so_demoName(4), 1, 0xFFFF); /* fopAcM_orderOtherEvent2 */
        }
    } else if (b >= 3) {
        if ((u32)(b - 3) >= 6) {
            JUT_ASSERT_fail("d_a_npc_so.cpp", 0x73E,
                            "m_order_event_num-ORDER_DEMO < (sizeof(a_demo_name_tbl) / "
                            "sizeof(a_demo_name_tbl[0]))"); /* HD: index range assert */
        }
        soOrderOtherEvent(this, so_demoName((u8)mB70 - 3), 1, 0xFFFF); /* fopAcM_orderOtherEvent2 */
    }
}

/* cLib_minLimit<f32>(v, lo) as GHS emits it: fsel(v - lo, v, lo) (NaN -> lo) */
static inline f32 so_minLimit(f32 v, f32 lo) {
    f32 d = gabi::fsubs_ppc(v, lo);
    return d >= 0.0f ? v : lo;
}

/* 022DF60C */
void daNpc_So_c::setAnmSwimSpeed() {
    if (mAnmPrmIdx == 2) { /* isAnm(2) */
        gabi::Local<cXyz> d;
        soSubtract(&current.pos, d.get(), &old.pos); /* current.pos - old.pos */
        f64 sq = soSquareMagnitude(d.get());         /* PSVECSquareMag */
        f64 len = soSqrt(sq);                        /* std::sqrtf: f1 unrounded */
        f32 abs = (f32)(len / 10.0);                 /* HD: no debug register in the divisor */
        f32 v;
        if (!(abs > 0.0f)) { /* bgt skips on NaN */
            v = gabi::fmuls_ppc(0.0f, l_HIO.m34);
        } else {
            f32 c = abs;
            if (!(abs < 1.0f))
                c = 1.0f; /* blt skips on NaN */
            v = gabi::fmuls_ppc(c, l_HIO.m34);
        }
        v = so_minLimit(v, l_HIO.m38);
        mpMorf->setPlaySpeed(v); /* setPlaySpeed */
        mA74->setPlaySpeed(v);   /* HD: the second morf too */
    }
}

static inline s32 so_calcTimerI(be<s32> *t) { return soCalcTimer(t); } /* cLib_calcTimer<int> */

/* the blink timer of the eye texture (HD: after the tag search, both branches) */
static inline void so_btpTimer(daNpc_So_c *t) {
    if (so_calcTimerI(&t->m868) == 0) {
        s16 n = (s16)(t->m86C + 1);
        f32 a = (f32)n;
        f32 b = (f32)s16(soBtp(t)->mEndFrame); /* mBtpAnm.getEndFrame() */
        t->m86C = n;
        if (a > b) {
            f32 r = gabi::fadds_ppc(cM_rndF(100.0f), 100.0f);
            t->m86C = 0;
            t->m868 = (s16)gabi::ftoi(r);
        }
    }
}

// HD: preserve guard-first initialization of the ripple scale.
static inline cXyz *so_rippleScale() {
    if (ripple_guard == 0) {
        ripple_guard = 1;
        ripple_scale.x = 0.8f;
        ripple_scale.y = 0.8f;
        ripple_scale.z = 0.8f;
    }
    return &ripple_scale;
}

/* 022DF6EC */
bool daNpc_So_c::_execute() {
    f32 sx = scale.x;
    f32 lo = gabi::fmuls_ppc(-100.0f, sx);
    f32 hi = gabi::fmuls_ppc(100.0f, sx);
    fopAcM_setCullSizeBox(this, lo, lo, lo, hi, hi, hi);

    if (dComIfGp_event_runCheck() == 0 /* !dComIfGp_event_runCheck() */ &&
        mAcch.ChkGroundHit() /* mAcch.ChkGroundHit() */) {
        current.pos.y = 0.0f;
        speedF = 0.0f;
        mAFC = 0.0f;
        speed.y = 0.0f;
        modeProc(PROC_INIT_e, MODE_HIDE_e);
        return true;
    }

    m_jnt.setParam(l_HIO.mNpc.mMaxBackboneX, l_HIO.mNpc.mMaxBackboneY, l_HIO.mNpc.mMinBackboneX,
                   l_HIO.mNpc.mMinBackboneY, l_HIO.mNpc.mMaxHeadX, l_HIO.mNpc.mMaxHeadY,
                   l_HIO.mNpc.mMinHeadX, l_HIO.mNpc.mMinHeadY, l_HIO.mNpc.mMaxTurnStep);
    current.angle.y = shape_angle.y;
    if (mA7C == 0.0f) {
        fopAcM_Search(searchTagSo_CB, this); /* fopAcM_Search */
    } else if (m6CC == MODE_HIDE_e) {
        modeProc(PROC_INIT_e, MODE_SWIM_e);
    }
    so_btpTimer(this);

    setScale();
    setAttention();
    cLib_addCalc2(&speedF, mAFC, 0.3f, 4.0f);
    cLib_addCalc2(&mB34, mB38.x, mB38.z, mB38.y);
    lookBack();
    checkOrder();
    modeProc(PROC_RUN_e, 0x10);
    eventOrder();

    s16 target = 0;
    f64 water = soGetWaterY(&current.pos, &mAcch);
    if ((f64)current.pos.y < water) { /* the position is read after the call */
        current.pos.y = (f32)soGetWaterY(&current.pos, &mAcch);
        if (mB34 > 0.0f &&
            soRipple(this)->mpBaseEmitter.v.get() == 0 /* mAE8.mpBaseEmitter == NULL */) {
            cXyz *scl = so_rippleScale();
            /* dComIfGp_particle_setShipTail(dPa_name::ID_AK_JN_HAMON00, &current.pos, NULL, &ripple_scale, 0xFF, &mAE8) */
            dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &current.pos, nullptr, scl, 0xFF,
                            reinterpret_cast<dPa_levelEcallBack *>(&mAE8), -1, nullptr, nullptr,
                            nullptr);
            if (soRipple(this)->mpBaseEmitter.v.get() != 0) {
                soRipple(this)->mRate = 0.0f; /* mAE8.setRate(0.0f) */
            }
        }
    } else {
        f32 b = mB00;
        f32 q = gabi::fmuls_ppc(b, 0.25f);
        f32 h = 0.5f;
        if (speed.y < -q) {
            if (speed.y < -gabi::fmuls_ppc(b, h)) {
                target = l_HIO.m64;
            } else {
                target = l_HIO.m66;
            }
        } else if (speed.y > q) {
            if (speed.y > gabi::fmuls_ppc(b, h)) {
                target = l_HIO.m68;
            } else {
                target = l_HIO.m6A;
            }
        } else {
            target = 0;
        }
        soEndRipple(&mAE8); /* mAE8.end() */
    }
    cLib_addCalcAngleS(&shape_angle.x, target, 4, 0x800,
                       0x80); /* HD: S with a minimum step (GameCube S2) */

    if (m6CC != MODE_HIDE_e && m6CC != MODE_EVENT_FIRST_WAIT_e && !mBDB &&
        so_calcTimerI(&mBE0) == 0) {
        fopAcM_posMoveF(this, nullptr);
        dBgS_Acch_CrrPos(&mAcch, dComIfG_Bgsp());
    }
    mpMorf->play(nullptr, 0, 0);
    mpMorf->calc();
    mA74->play(nullptr, 0, 0); /* HD: the second morf */
    mA74->calc();
    setMtx();
    setAnm(6, 0);
    setAnmSwimSpeed();
    current.angle.y = shape_angle.y;
    return false;
}

// HD: compiler-generated and template code of this translation unit.
/* 022E48B4 */
static u32 daNpc_So_HIO_ct(daNpc_So_HIO_c *h) {
    if (h == 0) {
        h = static_cast<daNpc_So_HIO_c *>(operator_new(sizeof(daNpc_So_HIO_c)));
        if (h == 0)
            return gabi::ea(h);
    }
    h->__vtbl = SO_HIO_VTBL;     /* vtable */
    soNpcHIOCtor(&h->mNpc);      /* dNpc_HIO_c::dNpc_HIO_c */
    soJntHitHIOCtor(&h->jntHit); /* JntHit_HIO_c::JntHit_HIO_c */
    h->m5C = 30.0f;
    h->m38 = 0.9f;
    h->m64 = (s16)0x1f40;
    h->m48 = 2000.0f;
    h->m34 = 3.0f;
    h->m50 = 150.0f;
    h->m4C = 2.0f;
    h->m40 = 2000.0f;
    h->m54 = 20000.0f;
    h->m2E = 0;
    h->m31 = 0;
    h->m6C = 10.0f;
    h->m3C = 100.0f;
    h->m30 = 0;
    h->m2F = 0;
    h->m68 = (s16)-0x1f40;
    h->m44 = 0.7f;
    h->mNpc.mMaxHeadX = (s16)0x1ffe;
    h->mNpc.m04 = -33.0f;
    h->m66 = (s16)0xfa0;
    h->m2C = 0;
    h->mNpc.mMaxHeadY = (s16)0x1000;
    h->m6A = (s16)-0xfa0;
    h->m60 = 700.0f;
    h->m74 = 300.0f;
    h->mNpc.mMaxBackboneY = (s16)0x2000;
    h->mNpc.mMaxBackboneX = (s16)0x1000;
    h->m58 = 250.0f;
    h->mNpc.mMaxAttnDistXZ = 400.0f;
    h->mNpc.mAttnYOffset = 50.0f;
    h->mNpc.mMinHeadX = (s16)-0xbb8;
    h->m70 = 600.0f;
    h->mNpc.m22 = 0;
    h->mNpc.mMaxHeadTurnVel = (s16)0x150;
    h->m7C = (s16)0x5;
    h->m88 = 120.0f;
    h->m80 = -1.4f;
    h->m78 = 100.0f;
    h->m90 = 15.0f;
    h->mNpc.mMinHeadY = (s16)-0x1000;
    h->m94 = 0;
    h->m84 = 18.0f;
    h->mNpc.mMaxAttnAngleY = (s16)0x7fff;
    h->mNpc.mMaxTurnStep = (s16)0x250;
    h->mNpc.mMinBackboneY = (s16)-0x2000;
    h->m2D = 0;
    h->m8C = 300.0f;
    h->mNpc.mMinBackboneX = (s16)-0x1000;

    return gabi::ea(h);
}

/* 022E4AB4 */
static void __sinit_d_a_npc_so_cpp() {
    for (int i = 0; i < 4; i++)
        headerStatics.mZero[i] = 0;
    __register_global_object(gabi::ea(&headerDtorRecords));
    headerStatics.mPiMin = -3.1415927f;
    headerStatics.mPiMax = 3.1415927f;
    soFirstHeaderCtor(&headerStatics.mFirst);
    __register_global_object(gabi::ea(&headerDtorRecords.records[12]));
    soSecondHeaderCtor(&headerStatics.mSecond);
    __register_global_object(gabi::ea(&headerDtorRecords.records[24]));
    headerStatics.mCenterY = 50000.0f;
    headerStatics.mEyeY = 10000.0f;
    headerStatics.mCenterZ = 50000.0f;
    headerStatics.mEyeZ = 10000.0f;
    daNpc_So_HIO_ct(&l_HIO);
}

/* 022E4B84 */
static void so_empty_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 022E4BA0 */
static void soEmptyModeWait(void *unused) {}
/* 022E4BA4 */
static void soEmptyModeDebug(void *unused) {}
/* 022E4BA8 */
static void soEmptyModeGetRupeeInit(void *unused) {}
/* 022E4BAC */
static void soEmptyDraw(void *unused) {}
/* 022E4BB0 */
static void soEmptyDebug(void *unused) {}
/* 022E4BB4 */
static void soEmptyEvent(void *unused) {}
/* 022E4C9C */
static void soEmptyDestructor(void *unused) {}

/* 022E4BB8 */
static void daNpc_So_c_dt(daNpc_So_c *i_this, s32 flags) {
    if (i_this != nullptr) {
        soCircleDtor(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        i_this->mAcch.dBgS_GrpPassChk::__vtbl =
            gc_tu.objAcchDt[0]; /* ~dBgS_ObjAcch: sub-object vtables */
        i_this->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcchDt[1];
        soAcchDtor(&i_this->mAcch, 0); /* dBgS_Acch::~dBgS_Acch */
        soSphDtor(&i_this->mSph, 2);   /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mStts2, 2);
        /* ~fopNpc_npc_c (inline) */
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        soCircleDtor(&static_cast<fopNpc_npc_c *>(i_this)->mAcchCir.m_cir,
                     2); /* fopNpc_npc_c::mAcchCir.m_cir */
        i_this->mObjAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcchDt[0];
        i_this->mObjAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcchDt[1];
        soAcchDtor(&i_this->mObjAcch, 0);
        soActorDtor(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
/* WWHD glue (tools/verify/autoglue.py)
ext 025871F8 f64 soWaterY(cXyz*, dBgS_ObjAcch*)
ext 020198D8 f64 soRandom(f32)
ext 02552B60 u32 soCreateJntHit(J3DModel*, const __jnt_hit_data_c*, s32)
ext 025A9270 void soEndRipple(void*)
ext 022DF6EC BOOL soExecute(void*)
ext 02587A0C void soSetAnm(const char*, mDoExt_McaMorf*, be<s8>*, be<s8>*, be<s8>*, const be<s32>*, const dLib_anm_prm_c*, u32, mDoExt_McaMorf*)
ext 027F596C void soSetPacketFlags(J3DModel*, u32)
ext 025E789C BOOL soBtpInit(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, u32)
ext 025A1458 void soNpcCtor(daNpc_So_c*)
ext 025166F0 void soSphCtor(dCcD_Sph*)
ext 025E7820 void soBtpCtor(void*)
ext 025A9084 void soRippleCtor(void*)
ext 025D6958 f64 soActorDistance(fopAc_ac_c*, fopAc_ac_c*)
ext 0200F164 void soAddCalcPos(cXyz*, cXyz*, f32, f32)
ext 0259D54C void soPlayerEye(cXyz*, f32)
ext 0259DED0 void soLookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s32, s32, u32)
ext 0200F428 void soAddCalcAngle(be<s16>*, s32, s32, s32)
ext 025D78FC void soOrderChangeEvent(fopAc_ac_c*, const char*, u32, u32)
ext 025D77DC void soOrderOtherEvent(fopAc_ac_c*, const char*, u32, u32)
ext 0201ADE0 void soSubtract(cXyz*, cXyz*, cXyz*)
ext 028E8DD0 f64 soSquareMagnitude(cXyz*)
ext 028F4384 f64 soSqrt(f64)
ext 0211D2F8 s32 soCalcTimer(be<s32>*)
ext 0259DA18 void soNpcHIOCtor(dNpc_HIO_c*)
ext 02552BE8 void soJntHitHIOCtor(SoJntHitHIO*)
ext 028ED6F8 void soFirstHeaderCtor(be<u8>*)
ext 028EAB2C void soSecondHeaderCtor(be<u8>*)
ext 02018034 void soCircleDtor(void*, s32)
ext 024EFD9C void soAcchDtor(dBgS_Acch*, s32)
ext 025D50BC void soActorDtor(fopAc_ac_c*, s32)
ext 02515AE8 void soSphDtor(dCcD_Sph*, s32)
str 100223AC
str 10021BEC in 022DE13C
str 10021BDC
str 10021BFC
str 10021C10
str 10021EC8
str 10021EE0 in 022DF46C
str 10021EF0
str 10021EC4 in 022DE884
str 10021ED0 in 022DED84
data l_HIO 104686CC
data mode_data 10021CB0
data modelSystem 104B45F0
data currentMtx 104B4868
data dNpc_cyl_src 101EA190
data m_sph_src 100223B0
data search_data 101C62CC
data a_anm_bcks_tbl 10021C3C
data a_anm_prm_tbl 10021C50
data a_demo_name_tbl 101C62FC
data ripple_scale 104686C0
data ripple_guard 10468790
data headerStatics 10468694
data headerDtorRecords 101C6368
*/
