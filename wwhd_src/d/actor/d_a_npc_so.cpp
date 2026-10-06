/**
 * d_a_npc_so.cpp (WWHD)
 * NPC - Fishman: create, heap, node control, searches, orders, execute, HIO and statics
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_so.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_so.h"

/* ---- this TU's statics (HD addresses) ---- */
#define SO_SAFESTRING_VTBL 0x10021B78 /* this TU's sead::SafeString vtable */
#define SO_ARC_NAME 0x100223AC        /* m_arc_name "So" */
#define SO_ASSERT_FILE STR(0x10021BEC)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline BOOL dComIfGs_isStageBossEnemy(s32 no) { return gabi::call<BOOL>(0x02520A84, no); }
/* dComIfGp_getSelectItem(btn): play + 0x5BBB + btn */
static inline u8 dComIfGp_getSelectItem(s32 btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }
/* J3D (HD): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868 */
static inline J3DModel* j3dSys_getModel() { return gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
/* J3DModel::getAnmMtx(jnt) / setAnmMtx(jnt, m): the joint matrices are marked dirty (HD) */
static inline Mtx34* so_getAnmMtx(J3DModel* model, s32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30);
}
static inline void so_setAnmMtx(J3DModel* model, s32 jntNo, const Mtx34* m) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    mtx_copy(gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30), m);
}
/* HD: getJointNodePointer(idx) is bounds-checked (count +4, array +8, 0x1C each); setCallBack at +8 */
static inline void so_setJointCallBack(J3DModelData* d, u16 idx, u32 cb) {
    u32 count = gabi::load<u32>(gabi::ea(d) + 4);
    u32 p = gabi::load<u32>(gabi::ea(d) + 8);
    if (idx < count) p += idx * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0, HD: sead::SafeString operator== */
static inline bool so_isStartStage(u32 lit) {
    gabi::Local<SafeString> sa;
    sa->mStringTop = lit;
    sa->__vtbl = SO_SAFESTRING_VTBL;
    u32 stage = dComIfGp_ea() + 0x5134;
    gabi::Local<SafeString> sb;
    sb->mStringTop = stage;
    sb->__vtbl = SO_SAFESTRING_VTBL;
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    u32 pa = sa->mStringTop;
    gabi::call_ptr(gabi::load<u32>(sb->__vtbl + 0x14), sb.get());
    u32 pb = sb->mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(pa + i);
        if (ca != gabi::load<u8>(pb + i)) return false;
        if (ca == 0) return true;
    }
    return false;
}
static inline void* so_getObjectRes(s32 idx) { return dComIfG_getObjectRes(STR(SO_ARC_NAME), idx, SO_SAFESTRING_VTBL); }

/* 022E4B98 */
static BOOL daNpc_SoIsDelete(daNpc_So_c* i_this) {
    WWHD_FUNC(0x022E4B98, BOOL, i_this);
    return TRUE;
}
VERIFY(0x022E4B98, daNpc_SoIsDelete);

/* 022DDBD8: fopNpc_npc_c deleting destructor (this TU's copy) */
static void fopNpc_npc_c_dt(fopNpc_npc_c* i_this, s32 flags) {
    WWHD_FUNC(0x022DDBD8, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x10021960);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x10021970);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x022DDBD8, fopNpc_npc_c_dt);

/* 022DDC74: empty virtual (this TU's copy) */
static void so_empty_022DDC74(void*) {
    WWHD_FUNC(0x022DDC74, void, (void*)nullptr);
}
VERIFY(0x022DDC74, so_empty_022DDC74);

/* 022DDC78 (unnamed by the matcher): the esa (bait) has not been eaten */
void* daNpc_So_c::_searchEsa(fopAc_ac_c* arg1) {
    WWHD_FUNC(0x022DDC78, void*, this, arg1);
    if (arg1 != nullptr && fpcM_GetName(arg1) == 0xDD /* fpcNm_ESA_e */) {
        if (gabi::load<u8>(gabi::ea(arg1) + 0x3B4) == 0) { /* esa->field_0x298 */
            return arg1;
        }
    }
    return nullptr;
}
VERIFY(0x022DDC78, &daNpc_So_c::_searchEsa);

/* 022DDCA0 searchEsa_CB */
static void* searchEsa_CB(void* arg1, void* i_this) {
    WWHD_FUNC(0x022DDCA0, void*, arg1, i_this);
    return static_cast<daNpc_So_c*>(i_this)->_searchEsa(static_cast<fopAc_ac_c*>(arg1));
}
VERIFY(0x022DDCA0, searchEsa_CB);

/* 022DDCB0 */
void daNpc_So_c::_nodeControl(J3DNode* pNode, J3DModel* pModel) {
    WWHD_FUNC(0x022DDCB0, void, this, pNode, pModel);
    s32 jntNo = gabi::load<u16>(gabi::ea(J3DNode_toJoint(pNode)) + 4); /* getJntNo() */
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
VERIFY(0x022DDCB0, &daNpc_So_c::_nodeControl);

/* 022DDED8 */
static BOOL nodeControl_CB(J3DNode* pNode, int calcTiming) {
    WWHD_FUNC(0x022DDED8, BOOL, pNode, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = j3dSys_getModel();
        daNpc_So_c* i_this = gabi::at<daNpc_So_c>(gabi::load<u32>(gabi::ea(model) + 0xB8)); /* getUserArea() */
        if (i_this != nullptr) {
            i_this->_nodeControl(pNode, j3dSys_getModel());
        }
    }
    return TRUE;
}
VERIFY(0x022DDED8, nodeControl_CB);

/* 022DDF20 (unnamed by the matcher) */
void* daNpc_So_c::_searchTagSo(fopAc_ac_c* arg1) {
    WWHD_FUNC(0x022DDF20, void*, this, arg1);
    if (arg1 != nullptr && fpcM_GetName(arg1) == 0x23 /* fpcNm_TAG_SO_e */) {
        u32 tag = gabi::ea(arg1);
        /* mA79 == tag_so->getRndNum() && tag_so->isTag() */
        if (gabi::load<u8>(tag + 0x3AC) == mA79 && gabi::load<u8>(tag + 0x3B4) != 1) {
            gabi::store<f32>(gabi::ea(&mA7C), gabi::load<f32>(tag + 0x3B0)); /* getJumpRange() */
            u32 x = gabi::load<u32>(tag + 0x314);
            gabi::store<u32>(gabi::ea(&mA80), x);
            u32 y = gabi::load<u32>(tag + 0x318);
            gabi::store<u32>(gabi::ea(&mA80) + 4, y);
            u32 z = gabi::load<u32>(tag + 0x31C);
            gabi::store<u32>(gabi::ea(&current.pos), x);
            gabi::store<u32>(gabi::ea(&current.pos) + 8, z);
            gabi::store<u32>(gabi::ea(&current.pos) + 4, y);
            gabi::store<u32>(gabi::ea(&mA80) + 8, z);
            return arg1;
        }
    }
    return nullptr;
}
VERIFY(0x022DDF20, &daNpc_So_c::_searchTagSo);

/* 022DDF8C searchTagSo_CB */
static void* searchTagSo_CB(void* arg1, void* i_this) {
    WWHD_FUNC(0x022DDF8C, void*, arg1, i_this);
    return static_cast<daNpc_So_c*>(i_this)->_searchTagSo(static_cast<fopAc_ac_c*>(arg1));
}
VERIFY(0x022DDF8C, searchTagSo_CB);

/* 022DDF9C (unnamed by the matcher) */
void* daNpc_So_c::_searchMinigameTagSo(fopAc_ac_c* arg1) {
    WWHD_FUNC(0x022DDF9C, void*, this, arg1);
    if (arg1 != nullptr && fpcM_GetName(arg1) == 0x23 /* fpcNm_TAG_SO_e */) {
        u32 tag = gabi::ea(arg1);
        if (gabi::load<u8>(tag + 0x3B4) == 1) { /* isMinigame() */
            gabi::store<u32>(gabi::ea(&mB90), gabi::load<u32>(tag + 0x314));
            gabi::store<u32>(gabi::ea(&mB90) + 4, gabi::load<u32>(tag + 0x318));
            gabi::store<u32>(gabi::ea(&mB90) + 8, gabi::load<u32>(tag + 0x31C));
            mBAE = 1;
            mB9C = gabi::load<s16>(tag + 0x32A); /* shape_angle.y */
            return arg1;
        }
    }
    return nullptr;
}
VERIFY(0x022DDF9C, &daNpc_So_c::_searchMinigameTagSo);

/* 022DDFF4 searchMinigameTagSo_CB */
static void* searchMinigameTagSo_CB(void* arg1, void* i_this) {
    WWHD_FUNC(0x022DDFF4, void*, arg1, i_this);
    return static_cast<daNpc_So_c*>(i_this)->_searchMinigameTagSo(static_cast<fopAc_ac_c*>(arg1));
}
VERIFY(0x022DDFF4, searchMinigameTagSo_CB);

/* 022DE004 */
s16 daNpc_So_c::XyCheckCB(int arg1) {
    WWHD_FUNC(0x022DE004, s16, this, arg1);
    if (fopAcIt_Judge(0x022DDCA0 /* searchEsa_CB */, this) != nullptr) {
        return 0;
    }
    /* HD: no isAnm(2) check */
    if ((gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x10000) /* checkPlayerStatus0(0, daPyStts0_SHIP_RIDE_e) */ &&
        dComIfGp_getSelectItem(arg1) == 0x82 /* dItemNo_BIRD_BAIT_5_e */) {
        return 1;
    }
    return 0;
}
VERIFY(0x022DE004, &daNpc_So_c::XyCheckCB);

/* 022DE084 daNpc_So_XyCheckCB (tail call) */
static s16 daNpc_So_XyCheckCB(void* i_this, int arg2) {
    WWHD_FUNC(0x022DE084, s16, i_this, arg2);
    return static_cast<daNpc_So_c*>(i_this)->XyCheckCB(arg2);
}
VERIFY(0x022DE084, daNpc_So_XyCheckCB);

/* 022DE088 */
s16 daNpc_So_c::XyEventCB(int arg1) {
    WWHD_FUNC(0x022DE088, s16, this, arg1);
    mBDC = dComIfGp_evmng_getEventIdx(STR(0x10021BDC) /* "SO_ESA_XY" */, 0xFF);
    return mBDC;
}
VERIFY(0x022DE088, &daNpc_So_c::XyEventCB);

/* 022DE0CC daNpc_So_XyEventCB (tail call) */
static s16 daNpc_So_XyEventCB(void* i_this, int arg2) {
    WWHD_FUNC(0x022DE0CC, s16, i_this, arg2);
    return static_cast<daNpc_So_c*>(i_this)->XyEventCB(arg2);
}
VERIFY(0x022DE0CC, daNpc_So_XyEventCB);

/* 022DE0D0 */
bool daNpc_So_c::jntHitCreateHeap() {
    WWHD_FUNC(0x022DE0D0, bool, this);
    /* search_data (.data 0x101C62CC, 2 entries) */
    u32 jh = gabi::call<u32>(0x02552B60, mpMorf->getModel(), 0x101C62CC, 2); /* JntHit_create */
    mJntHit = jh;
    if (jh != 0) {
        gabi::store<u32>(gabi::ea(this) + 0x36C, jh); /* jntHit */
        return true;
    }
    return false;
}
VERIFY(0x022DE0D0, &daNpc_So_c::jntHitCreateHeap);

/* 022DE410 createHeap_CB (tail call) */
static BOOL createHeap_CB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022DE410, BOOL, i_this);
    return static_cast<daNpc_So_c*>(i_this)->_createHeap();
}
VERIFY(0x022DE410, createHeap_CB);

/* 022DE414 getArg (unnamed by the matcher) */
void daNpc_So_c::getArg() {
    WWHD_FUNC(0x022DE414, void, this);
    m6D0 = home.angle.x;
    if ((s32)m6D0 == 0xFFFF || m6D0 == 0) { /* an s16 never equals 0xFFFF: only the 0 test remains */
        m6D0 = 1;
    }
}
VERIFY(0x022DE414, &daNpc_So_c::getArg);

/* 022DE6AC */
void daNpc_So_c::offsetZero() {
    WWHD_FUNC(0x022DE6AC, void, this);
    mB38.x = -1.0f;
    mB38.y = 20.0f;
    mB38.z = 0.1f;
}
VERIFY(0x022DE6AC, &daNpc_So_c::offsetZero);

/* 022DEFE8 */
static cPhs_State daNpc_SoCreate(void* i_this) {
    WWHD_FUNC(0x022DEFE8, cPhs_State, i_this);
    return static_cast<daNpc_So_c*>(i_this)->_create();
}
VERIFY(0x022DEFE8, daNpc_SoCreate);

/* 022DEFEC */
bool daNpc_So_c::_delete() {
    WWHD_FUNC(0x022DEFEC, bool, this);
    if (gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 8) { /* dComIfGp_getMiniGameType() */
        /* dComIfGp_endMiniGame(8) */
        u32 play = dComIfGp_ea();
        u16 f = gabi::load<u16>(play + 0x5CE8);
        gabi::store<u8>(play + 0x5CEE, 0);
        gabi::store<u8>(play + 0x5CEA, 0);
        gabi::store<u16>(play + 0x5CE8, (u16)(f ^ 0x80));
    }
    dComIfG_resDelete(&mPhase, STR(SO_ARC_NAME));
    gabi::call(0x025A9270, gabi::ea(&mAE8)); /* dPa_rippleEcallBack::end */
    return true;
}
VERIFY(0x022DEFEC, &daNpc_So_c::_delete);

/* 022DF05C */
static BOOL daNpc_SoDelete(void* i_this) {
    WWHD_FUNC(0x022DF05C, BOOL, i_this);
    return static_cast<daNpc_So_c*>(i_this)->_delete();
}
VERIFY(0x022DF05C, daNpc_SoDelete);

/* 022DFBD4 */
static BOOL daNpc_SoExecute(void* i_this) {
    WWHD_FUNC(0x022DFBD4, BOOL, i_this);
    return gabi::call<BOOL>(0x022DF6EC, i_this); /* _execute */
}
VERIFY(0x022DFBD4, daNpc_SoExecute);

/* ---- l_HIO (daNpc_So_HIO_c, HD layout: vtable at 0, dNpc_HIO_c at 8, members from 0x2C on at their
 * GameCube offsets) ---- */
#define SO_L_HIO 0x104686CC
static inline f32 so_hioF(u32 off) { return gabi::load<f32>(SO_L_HIO + off); }
static inline s16 so_hioS(u32 off) { return gabi::load<s16>(SO_L_HIO + off); }
static inline u8 so_hioB(u32 off) { return gabi::load<u8>(SO_L_HIO + off); }
/* dLib_getWaterY(pos, acch): GHS passes the f1 result on unrounded */
static inline f64 dLib_getWaterY(cXyz* pos, dBgS_ObjAcch* acch) { return gabi::call<f64>(0x025871F8, pos, acch); }
static inline f32 so_faddsD(f64 a, f32 b) { return (f32)(a + (f64)b); }
/* cM_rndF: the f1 result is compared unrounded */
static inline f64 so_rndF_d(f32 max) { return gabi::call<f64>(0x020198D8, max); }

/* 022DE434 setMtx (unnamed by the matcher) */
void daNpc_So_c::setMtx() {
    WWHD_FUNC(0x022DE434, void, this);
    J3DModel* model = mpMorf->getModel();
    u32 m = gabi::ea(model);
    f32 sz = scale.z;
    f32 sx = scale.x;
    f32 sy = scale.y;
    gabi::store<f32>(m + 0xBC, sx); /* setBaseScale(scale) */
    gabi::store<f32>(m + 0xC0, sy);
    gabi::store<f32>(m + 0xC4, sz);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mDoMtx_stack_c::transM(0.0f, mB34, 0.0f);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    /* HD: the second morf is placed on the water surface; flat (y scale 0.1) while the fishman
     * is not tilted */
    f32 x = scale.x;
    s16 ax = shape_angle.x;
    J3DModel* model2 = mA74->getModel();
    f32 z = scale.z;
    u32 m2 = gabi::ea(model2);
    if (ax == 0) {
        gabi::store<f32>(m2 + 0xBC, x);
        gabi::store<f32>(m2 + 0xC4, z);
        gabi::store<f32>(m2 + 0xC0, 0.1f);
    } else {
        gabi::store<f32>(m2 + 0xBC, x);
        gabi::store<f32>(m2 + 0xC0, scale.y);
        gabi::store<f32>(m2 + 0xC4, z);
    }
    f32 y = so_faddsD(dLib_getWaterY(&current.pos, &mAcch), 15.0f);
    mDoMtx_stack_c::transS(current.pos.x, y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(model2, mDoMtx_stack_c::get());
}
VERIFY(0x022DE434, &daNpc_So_c::setMtx);

/* 022DE6D4 */
void daNpc_So_c::setAnm(s8 anmPrmIdx, u32 force) {
    WWHD_FUNC(0x022DE6D4, void, this, anmPrmIdx, force);
    if (anmPrmIdx != 6) {
        mAnmPrmIdx = anmPrmIdx;
    }
    u32 morf = gabi::ea(mpMorf.get());
    f32 end = gabi::fsubs_ppc((f32)gabi::load<s16>(morf + 0xA2), 1.0f); /* getEndFrame() - 1.0f */
    f32 frame = gabi::load<f32>(morf + 0x9C);                           /* getFrame() */
    if (!(frame < end)) {
        f64 rnd = so_rndF_d(100.0f);
        if (rnd < (f64)so_hioF(0x5C)) { /* bge skips on NaN */
            if (mAnmPrmIdx == 5) {
                mAnmPrmIdx = 3;
            } else if (mAnmPrmIdx == 3) {
                mAnmPrmIdx = 5;
            }
        }
    }
    /* HD: dLib_bcks_setAnm also takes the second morf */
    gabi::call(0x02587A0C, STR(SO_ARC_NAME), mpMorf.get(), &mBckIdx, &mAnmPrmIdx, &mOldAnmPrmIdx,
               0x10021C3C /* a_anm_bcks_tbl */, 0x10021C50 /* a_anm_prm_tbl */, force, mA74.get());
}
VERIFY(0x022DE6D4, &daNpc_So_c::setAnm);

/* 022DE7D8: mode_tbl (.data 0x10021CB0, 0x14 per entry: init and run pointers to member, name) */
#define SO_MODE_TBL 0x10021CB0
static inline void so_pmf_tailcall(u32 self, u32 pmf) {
    u32 thisp = self + (s32)gabi::load<s16>(pmf);
    s16 i = gabi::load<s16>(pmf + 2);
    if (i < 0) {
        gabi::call_ptr(gabi::load<u32>(pmf + 4), thisp);
    } else {
        u32 vt = gabi::load<u32>(thisp + gabi::load<s16>(pmf + 6));
        gabi::call_ptr(gabi::load<u32>(vt + i * 8 + 4), thisp);
    }
}
void daNpc_So_c::modeProc(Proc_e mode, int index) {
    WWHD_FUNC(0x022DE7D8, void, this, mode, index);
    if (mode == PROC_INIT_e) {
        m6CC = index;
        so_pmf_tailcall(gabi::ea(this), SO_MODE_TBL + index * 0x14);
    } else if (mode == PROC_RUN_e) {
        so_pmf_tailcall(gabi::ea(this), SO_MODE_TBL + m6CC * 0x14 + 8);
    }
}
VERIFY(0x022DE7D8, &daNpc_So_c::modeProc);

/* 022DE13C */
BOOL daNpc_So_c::_createHeap() {
    WWHD_FUNC(0x022DE13C, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)so_getObjectRes(0xC /* dRes_INDEX_SO_BDL_SO_e */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(SO_ASSERT_FILE, 0x208, STR(0x10021BFC) /* "modelData != 0" */);
    }
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, -1 /* ~EMode_NONE */, 1.0f, 0, -1, 1,
                                    nullptr, 0x80000, 0x11020022);
    if (mpMorf == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    /* HD: a second morf of the same model data */
    mA74 = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1, nullptr, 0x80000,
                                  0x11020022);
    if (mA74 == nullptr || mA74->getModel() == nullptr) {
        return FALSE;
    }
    gabi::store<u32>(gabi::ea(mA74->getModel()) + 0xB8, gabi::ea(this));
    {
        u32 m = gabi::ea(mA74->getModel());
        u32 f = gabi::load<u32>(m + 0x74) & ~6u;
        gabi::store<u32>(m + 0x74, f);
        gabi::call(0x027F596C, m, f); /* J3DModel (HD): the flags of every material packet */
    }
    {
        u32 m = gabi::ea(mpMorf->getModel());
        u32 f = gabi::load<u32>(m + 0x74) & ~1u;
        gabi::store<u32>(m + 0x74, f);
        gabi::call(0x027F596C, m, f);
    }
    void* btp = so_getObjectRes(0x10 /* dRes_INDEX_SO_BTP_SO_e */);
    if (btp == nullptr) {
        JUT_ASSERT_fail(SO_ASSERT_FILE, 0x22B, STR(0x10021C10) /* "btp != 0" */);
    }
    /* mBtpAnm.init(modelData, btp, 1, 0, 1.0f, 0, -1, false, 0) */
    if (!gabi::call<BOOL>(0x025E789C, gabi::ea(&mBtpAnm), modelData, btp, 1, 0, 1.0f, 0, -1, 0, 0)) {
        return FALSE;
    }
    m_jnt.mHeadJntNum = 11;
    m_jnt.mBackboneJntNum = 1;
    so_setJointCallBack(modelData, 11, 0x022DDED8 /* nodeControl_CB */);
    so_setJointCallBack(modelData, 1, 0x022DDED8);
    modelData = (J3DModelData*)so_getObjectRes(0xD /* dRes_INDEX_SO_BDL_SO_FUDE_e */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(SO_ASSERT_FILE, 0x23C, STR(0x10021BFC));
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
VERIFY(0x022DE13C, &daNpc_So_c::_createHeap);

/* createInit's common tail (GHS duplicates it after each modeProcInit) */
static inline void so_createInit_tail(daNpc_So_c* t) {
    t->mBE0 = 30;
    t->mAcchCir.SetWall(30.0f, 30.0f);
    t->mAcch.Set(&t->current.pos, &t->old.pos, t, 1, &t->mAcchCir, &t->speed);
    t->mAcch.m_flags = t->mAcch.m_flags | 0xC; /* SetWallNone(), SetRoofNone() */
    J3DModel* model = t->mpMorf->getModel();
    u32 mtx = model != nullptr ? gabi::ea(model) + 0xC8 : 0;
    f32 sx = t->scale.x;
    f32 lo = gabi::fmuls_ppc(-100.0f, sx);
    f32 hi = gabi::fmuls_ppc(100.0f, sx);
    t->cullMtx = mtx; /* model->getBaseTRMtx() */
    fopAcM_setCullSizeBox(t, lo, lo, lo, hi, hi, hi);
    gabi::store<u8>(gabi::ea(t) + 0x389, 0x22); /* attention_info.distances[TALK] */
    gabi::store<u8>(gabi::ea(t) + 0x38B, 0x22); /* attention_info.distances[SPEAK] */
    t->cullSizeFar = 10.0f;
    t->gravity = -2.5f;
    gabi::store<u32>(gabi::ea(t) + 0x104, 0x022DE084); /* eventInfo.setXyCheckCB(daNpc_So_XyCheckCB) */
    gabi::store<u32>(gabi::ea(t) + 0x100, 0x022DE0CC); /* eventInfo.setXyEventCB(daNpc_So_XyEventCB) */
    t->mEventCut.setActorInfo2(STR(0x10021EC8) /* "NpcSo" */, t);
}

/* 022DE884 */
void daNpc_So_c::createInit() {
    WWHD_FUNC(0x022DE884, void, this);
    mBDA = 0;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    mStts2.Init(0xFF, 0xFF, this);
    mSph.Set(gabi::at<dCcD_SrcSph>(0x100223B0) /* m_sph_src */);
    mSph.SetStts(&mStts2);
    current.pos.y = gabi::fsubs_ppc(current.pos.y, 500.0f);
    setMtx();
    mpMorf->calc();
    mA74->calc(); /* HD */
    u32 z = gabi::load<u32>(gabi::ea(&current.pos) + 8);
    u32 x = gabi::load<u32>(gabi::ea(&current.pos));
    u32 y = gabi::load<u32>(gabi::ea(&current.pos) + 4);
    gabi::store<u32>(gabi::ea(&mAAC), x);
    gabi::store<u32>(gabi::ea(&mAAC) + 4, y);
    gabi::store<u32>(gabi::ea(&mAAC) + 8, z);
    offsetZero();
    setAnm(1, 0);
    mA79 = (u8)gabi::ftoi(gabi::call<f64>(0x020198D8, 4.9f)); /* cM_rndF(4.9f) */

    bool firstWait = false;
    if (!dComIfGs_isEventBit(0x0901)) {
        if (so_isStartStage(0x10021EC4 /* "sea" */) && current.roomNo == 0xD /* dIsleRoom_DragonRoostIsland_e */ &&
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
    if (so_isStartStage(0x10021EC4 /* "sea" */) && current.roomNo == 4 /* dIsleRoom_GaleIsle_e */ &&
        dComIfGs_isStageBossEnemy(7 /* STAGE_WT */) &&
        gabi::call<BOOL>(0x025B7A2C, gabi::load<u32>(0x101F84DC) + 0xD4, 0, 3) /* dComIfGs_isCollect(0, 3) */ &&
        !dComIfGs_isEventBit(0x3A20)) {
        triforce = true;
    }
    if (triforce) {
        modeProc(PROC_INIT_e, MODE_EVENT_TRIFORCE_e);
    } else {
        gabi::store<u32>(gabi::ea(this) + 0x39C, 0x0200000A); /* attention_info.flags: TALKFLAG_NOTALK | ACTION_SPEAK | LOCKON_TALK */
        modeProc(PROC_INIT_e, MODE_HIDE_e);
    }
    so_createInit_tail(this);
}
VERIFY(0x022DE884, &daNpc_So_c::createInit);

/* 022DED84 */
cPhs_State daNpc_So_c::_create() {
    WWHD_FUNC(0x022DED84, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_So_c): inline constructor */
    if (!fopAcM_CheckCondition(this, 8 /* fopAcCnd_INIT_e */)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this); /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
            __vtbl = SO_VTBL;
            dCcD_Stts_ct(&mStts2);
            gabi::call(0x025166F0, &mSph); /* dCcD_Sph::dCcD_Sph */
            gabi::call(0x025E7820, gabi::ea(&mBtpAnm)); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dBgS_ObjAcch_ct(&mAcch, dBgS_ObjAcch_vt{0x10021B90, 0x10021BB0, 0x10021BA0});
            dBgS_AcchCir_ct(&mAcchCir);
            gabi::call(0x025A9084, gabi::ea(&mAE8)); /* dPa_rippleEcallBack::dPa_rippleEcallBack */
        }
        fopAcM_OnCondition(this, 8);
    }
    cPhs_State state = dComIfG_resLoad(&mPhase, STR(SO_ARC_NAME));
    if (state == cPhs_COMPLEATE_e) {
        getArg();
        bool err = false;
        if (so_isStartStage(0x10021ED0 /* "sea" */) && current.roomNo == 0x1A /* dIsleRoom_ToweroftheGods_e */ &&
            !dComIfGs_isEventBit(0x1E40)) {
            err = true;
        }
        if (err) {
            return cPhs_ERROR_e;
        }
        if (!dComIfGs_isStageBossEnemy(3 /* STAGE_DRC */)) {
            return cPhs_ERROR_e;
        }
        if (!fopAcM_entrySolidHeap(this, 0x022DE410 /* createHeap_CB */, 0x1C00 /* m_heapsize */)) {
            return cPhs_ERROR_e;
        }
        createInit();
    }
    return state;
}
VERIFY(0x022DED84, &daNpc_So_c::_create);

/* 022DF060 */
void daNpc_So_c::setScale() {
    WWHD_FUNC(0x022DF060, void, this);
    /* fopAcM_searchActorDistanceXZ(this, dComIfGp_getPlayer(0)): the f1 result is used unrounded */
    f64 dist = gabi::call<f64>(0x025D6958, this, gabi::load<u32>(dComIfGp_ea() + 0x5B2C));
    f32 v;
    if (dist > (f64)so_hioF(0x48)) {
        f32 m48 = so_hioF(0x48);
        f32 tmp2 = gabi::fsubs_ppc(100000.0f, m48);
        f32 tmp = (f32)(dist - (f64)m48);
        tmp2 = tmp2 / so_hioF(0x4C);
        v = gabi::fadds_ppc(tmp / tmp2, 1.0f);
        mB08 = v;
        f32 m4C = so_hioF(0x4C);
        if (v > m4C) {
            v = m4C;
            mB08 = v;
        }
    } else {
        v = so_hioF(0x44);
        mB08 = v;
    }
    if (m6CC == 0xF) {
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
    gabi::call(0x0200F164, &scale, tmp.get(), 0.1f, 0.5f); /* cLib_addCalcPos2(&scale, tmp, 0.1f, 0.5f) */
}
VERIFY(0x022DF060, &daNpc_So_c::setScale);

/* 022DF148 */
void daNpc_So_c::setAttention() {
    WWHD_FUNC(0x022DF148, void, this);
    u32 att = gabi::ea(this) + 0x390; /* attention_info.position */
    f32 y = mB60.y;
    gabi::store<f32>(att + 4, y);
    gabi::store<f32>(att, mB60.x);
    gabi::store<f32>(att + 8, mB60.z);
    gabi::store<f32>(att + 4, gabi::fadds_ppc(y, so_hioF(0x1C) /* mNpc.mAttnYOffset */));
    f64 water = dLib_getWaterY(gabi::at<cXyz>(att), &mAcch);
    if (!(gabi::load<f32>(att + 4) > water)) { /* bgt skips on NaN */
        gabi::store<f32>(att + 4, (f32)water);
    }
    u32 z = gabi::load<u32>(gabi::ea(&mB54) + 8);
    u32 yy = gabi::load<u32>(gabi::ea(&mB54) + 4);
    gabi::store<u32>(gabi::ea(&eyePos) + 8, z);
    gabi::store<u32>(gabi::ea(&eyePos) + 4, yy);
    gabi::store<u32>(gabi::ea(&eyePos), gabi::load<u32>(gabi::ea(&mB54)));
}
VERIFY(0x022DF148, &daNpc_So_c::setAttention);

/* dNpc_playerEyePos(offset): the result goes to a hidden out-pointer */
static inline void so_playerEyePos(cXyz* out, f32 ofs) { gabi::call(0x0259D54C, out, ofs); }
static inline void so_copyWords3_yxz(u32 dst, u32 src) {
    u32 z = gabi::load<u32>(src + 8);
    u32 x = gabi::load<u32>(src);
    u32 y = gabi::load<u32>(src + 4);
    gabi::store<u32>(dst + 4, y);
    gabi::store<u32>(dst, x);
    gabi::store<u32>(dst + 8, z);
}
/* lookBack's tail: m_jnt.lookAtTarget(&shape_angle.y, &mB44, mB54 + (0, 200, 0), shape_angle.y, mB50, mBDA) */
static inline void so_lookAt(daNpc_So_c* t) {
    gabi::Local<cXyz> tmp;
    s16 vel = t->mB50;
    u8 headOnly = t->mBDA;
    f32 y = gabi::fadds_ppc(t->mB54.y, 200.0f);
    tmp->y = y;
    tmp->z = t->mB54.z;
    tmp->x = t->mB54.x;
    gabi::call(0x0259DED0, &t->m_jnt, &t->shape_angle.y, &t->mB44, tmp.get(), (s32)t->shape_angle.y, (s32)vel, (u32)headOnly);
}

/* 022DF1CC */
void daNpc_So_c::lookBack() {
    WWHD_FUNC(0x022DF1CC, void, this);
    dComIfGp_ea();
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0) { /* dComIfGp_event_runCheck() */
        if (mEventCut.mbAttention != 0) {          /* getAttnFlag() */
            if (mAnmPrmIdx == 4) {                 /* isAnm(4) */
                m_jnt.mbTrn = 0;                   /* clrTrn() */
                mB50 = 0;
                so_lookAt(this);
                return;
            }
            f32 x = mEventCut.mPos.x;               /* mB44 = getAttnPos() */
            f32 z = mEventCut.mPos.z;
            mB44.x = x;
            s16 turn = mEventCut.mTurnSpeed;
            f32 y = mEventCut.mPos.y;
            mB44.z = z;
            mB44.y = y;
            m_jnt.mbTrn = 1;                       /* setTrn() */
            s16 target = so_hioS(0x1A);            /* mNpc.mMaxHeadTurnVel */
            if (turn != 0) target = turn;
            gabi::call(0x0200F428, &mB50, (s32)target, 4, 0x800); /* cLib_addCalcAngleS2 */
            so_lookAt(this);
            return;
        }
        gabi::Local<cXyz> eye;
        so_playerEyePos(eye.get(), so_hioF(0x4) /* mNpc.m04 */);
        so_copyWords3_yxz(gabi::ea(&mB44), gabi::ea(eye.get()));
    } else {
        m_jnt.mbTrn = 0; /* clrTrn() */
        gabi::Local<cXyz> eye;
        so_playerEyePos(eye.get(), so_hioF(0x4));
        so_copyWords3_yxz(gabi::ea(&mB44), gabi::ea(eye.get()));
    }
    if (m_jnt.mbTrn != 0) { /* trnChk() */
        s16 turn = mEventCut.mTurnSpeed;
        s16 target = so_hioS(0x1A);
        if (turn != 0) target = turn;
        gabi::call(0x0200F428, &mB50, (s32)target, 4, 0x800); /* cLib_addCalcAngleS2 */
    } else {
        mB50 = 0;
    }
    so_lookAt(this);
}
VERIFY(0x022DF1CC, &daNpc_So_c::lookBack);

/* eventInfo (HD): mCommand u16 at +0xF8, mCondition u16 at +0xFA */
static inline u16 so_evCommand(fopAc_ac_c* a) { return gabi::load<u16>(gabi::ea(a) + 0xF8); }
static inline void so_evOnCondition(fopAc_ac_c* a, u16 c) {
    gabi::store<u16>(gabi::ea(a) + 0xFA, (u16)(gabi::load<u16>(gabi::ea(a) + 0xFA) | c));
}
static inline BOOL so_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }

/* 022DF3F0 */
void daNpc_So_c::checkOrder() {
    WWHD_FUNC(0x022DF3F0, void, this);
    u16 cmd = so_evCommand(this);
    if (cmd == 2) { /* checkCommandDemoAccrpt() */
        mB70 = 0;
    } else if (cmd == 1 /* checkCommandTalk() */ && (mB70 == 1 || mB70 == 2)) {
        if (so_event_chkTalkXY()) {
            modeProc(PROC_INIT_e, MODE_EVENT_ESA_e);
        }
        mB70 = 0;
    }
}
VERIFY(0x022DF3F0, &daNpc_So_c::checkOrder);

/* a_demo_name_tbl[6] ("SO_1ST_MEET" ...), .data 0x101C62FC */
static inline u32 so_demoName(s32 i) { return gabi::load<u32>(0x101C62FC + i * 4); }

/* 022DF46C */
void daNpc_So_c::eventOrder() {
    WWHD_FUNC(0x022DF46C, void, this);
    u8 b = mB70;
    if (b == 1 || b == 2) {
        so_evOnCondition(this, 0x21); /* dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e */
        if (mB70 == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if ((u32)(b - 4) <= 2) { /* mB70 == 5 || mB70 == 4 || mB70 == 6 */
        gabi::call(0x025D78FC, this, so_demoName(b - 3), 0, 0xFFFF); /* fopAcM_orderChangeEvent */
    } else if (b == 7) {
        if (gabi::load<s16>(0x1047BD5A) == 0) { /* REG12_S(9) */
            gabi::call(0x025D78FC, this, so_demoName(4), 0, 0xFFFF); /* fopAcM_orderChangeEvent */
            so_evOnCondition(this, 8);            /* dEvtCnd_CANGETITEM_e */
        } else {
            gabi::call(0x025D77DC, this, so_demoName(4), 1, 0xFFFF); /* fopAcM_orderOtherEvent2 */
        }
    } else if (b >= 3) {
        if ((u32)(b - 3) >= 6) {
            JUT_ASSERT_fail(STR(0x10021EE0), 0x73E, STR(0x10021EF0)); /* HD: index range assert */
        }
        gabi::call(0x025D77DC, this, so_demoName((u8)mB70 - 3), 1, 0xFFFF); /* fopAcM_orderOtherEvent2 */
    }
}
VERIFY(0x022DF46C, &daNpc_So_c::eventOrder);

/* cLib_minLimit<f32>(v, lo) as GHS emits it: fsel(v - lo, v, lo) (NaN -> lo) */
static inline f32 so_minLimit(f32 v, f32 lo) {
    f32 d = gabi::fsubs_ppc(v, lo);
    return d >= 0.0f ? v : lo;
}

/* 022DF60C */
void daNpc_So_c::setAnmSwimSpeed() {
    WWHD_FUNC(0x022DF60C, void, this);
    if (mAnmPrmIdx == 2) { /* isAnm(2) */
        gabi::Local<cXyz> d;
        gabi::call(0x0201ADE0, &current.pos, d.get(), &old.pos); /* current.pos - old.pos */
        f64 sq = gabi::call<f64>(0x028E8DD0, d.get());           /* PSVECSquareMag */
        f64 len = gabi::call<f64>(0x028F4384, sq);               /* std::sqrtf: f1 unrounded */
        f32 abs = (f32)(len / 10.0); /* HD: no debug register in the divisor */
        f32 v;
        if (!(abs > 0.0f)) { /* bgt skips on NaN */
            v = gabi::fmuls_ppc(0.0f, so_hioF(0x34));
        } else {
            f32 c = abs;
            if (!(abs < 1.0f)) c = 1.0f; /* blt skips on NaN */
            v = gabi::fmuls_ppc(c, so_hioF(0x34));
        }
        v = so_minLimit(v, so_hioF(0x38));
        gabi::store<f32>(gabi::ea(mpMorf.get()) + 0x98, v); /* setPlaySpeed */
        gabi::store<f32>(gabi::ea(mA74.get()) + 0x98, v);   /* HD: the second morf too */
    }
}
VERIFY(0x022DF60C, &daNpc_So_c::setAnmSwimSpeed);

static inline s32 so_calcTimerI(be<s32>* t) { return gabi::call<s32>(0x0211D2F8, t); } /* cLib_calcTimer<int> */

/* the blink timer of the eye texture (HD: after the tag search, both branches) */
static inline void so_btpTimer(daNpc_So_c* t) {
    if (so_calcTimerI(&t->m868) == 0) {
        s16 n = (s16)(t->m86C + 1);
        f32 a = (f32)n;
        f32 b = (f32)gabi::load<s16>(gabi::ea(&t->mBtpAnm) + 0xA); /* mBtpAnm.getEndFrame() */
        t->m86C = n;
        if (a > b) {
            f32 r = gabi::fadds_ppc(cM_rndF(100.0f), 100.0f);
            t->m86C = 0;
            t->m868 = (s16)gabi::ftoi(r);
        }
    }
}

/* the ripple's static scale (function-local static: guard 0x10468790, cXyz at 0x104686C0) */
static inline u32 so_rippleScale() {
    if (gabi::load<s32>(0x10468790) == 0) {
        gabi::store<s32>(0x10468790, 1);
        gabi::store<f32>(0x104686C0, 0.8f);
        gabi::store<f32>(0x104686C4, 0.8f);
        gabi::store<f32>(0x104686C8, 0.8f);
    }
    return 0x104686C0;
}

/* 022DF6EC */
bool daNpc_So_c::_execute() {
    WWHD_FUNC(0x022DF6EC, bool, this);
    f32 sx = scale.x;
    f32 lo = gabi::fmuls_ppc(-100.0f, sx);
    f32 hi = gabi::fmuls_ppc(100.0f, sx);
    fopAcM_setCullSizeBox(this, lo, lo, lo, hi, hi, hi);

    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0 /* !dComIfGp_event_runCheck() */ &&
        (gabi::load<u32>(gabi::ea(&mAcch) + 0x28) & 0x20) /* mAcch.ChkGroundHit() */) {
        current.pos.y = 0.0f;
        speedF = 0.0f;
        mAFC = 0.0f;
        speed.y = 0.0f;
        modeProc(PROC_INIT_e, MODE_HIDE_e);
        return true;
    }

    m_jnt.setParam(so_hioS(0xA), so_hioS(0xE), so_hioS(0x12), so_hioS(0x16), so_hioS(0x8), so_hioS(0xC),
                   so_hioS(0x10), so_hioS(0x14), so_hioS(0x18));
    current.angle.y = shape_angle.y;
    if (mA7C == 0.0f) {
        fopAcIt_Judge(0x022DDF8C /* searchTagSo_CB */, this); /* fopAcM_Search */
    } else if (m6CC == 1) {
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
    f64 water = dLib_getWaterY(&current.pos, &mAcch);
    if ((f64)current.pos.y < water) { /* the position is read after the call */
        current.pos.y = (f32)dLib_getWaterY(&current.pos, &mAcch);
        if (mB34 > 0.0f && gabi::load<u32>(gabi::ea(&mAE8) + 4) == 0 /* mAE8.mpBaseEmitter == NULL */) {
            u32 scl = so_rippleScale();
            /* dComIfGp_particle_setShipTail(dPa_name::ID_AK_JN_HAMON00, &current.pos, NULL, &ripple_scale, 0xFF, &mAE8) */
            dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &current.pos, nullptr, gabi::at<cXyz>(scl), 0xFF,
                            gabi::at<dPa_levelEcallBack>(gabi::ea(&mAE8)), -1, nullptr, nullptr, nullptr);
            if (gabi::load<u32>(gabi::ea(&mAE8) + 4) != 0) {
                gabi::store<f32>(gabi::ea(&mAE8) + 0x10, 0.0f); /* mAE8.setRate(0.0f) */
            }
        }
    } else {
        f32 b = mB00;
        f32 q = gabi::fmuls_ppc(b, 0.25f);
        f32 h = 0.5f;
        if (speed.y < -q) {
            if (speed.y < -gabi::fmuls_ppc(b, h)) {
                target = so_hioS(0x64);
            } else {
                target = so_hioS(0x66);
            }
        } else if (speed.y > q) {
            if (speed.y > gabi::fmuls_ppc(b, h)) {
                target = so_hioS(0x68);
            } else {
                target = so_hioS(0x6A);
            }
        } else {
            target = 0;
        }
        gabi::call(0x025A9270, gabi::ea(&mAE8)); /* mAE8.end() */
    }
    cLib_addCalcAngleS(&shape_angle.x, target, 4, 0x800, 0x80); /* HD: S with a minimum step (GameCube S2) */

    if (m6CC != 1 && m6CC != 5 && !mBDB && so_calcTimerI(&mBE0) == 0) {
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
VERIFY(0x022DF6EC, &daNpc_So_c::_execute);

/* 022E48B4 daNpc_So_HIO_c::daNpc_So_HIO_c (HD layout: vtable at 0, dNpc_HIO_c at 4, JntHit_HIO_c at 0x98;
 * the member values are the GameCube ones plus the HD-only ones at 0x70..0x94) */
static u32 daNpc_So_HIO_ct(u32 h) {
    WWHD_FUNC(0x022E48B4, u32, h);
    if (h == 0) {
        h = gabi::ea(operator_new(0xC4));
        if (h == 0) return h;
    }
    gabi::store<u32>(h, 0x10021BC0); /* vtable */
    gabi::call(0x0259DA18, h + 4);    /* dNpc_HIO_c::dNpc_HIO_c */
    gabi::call(0x02552BE8, h + 0x98); /* JntHit_HIO_c::JntHit_HIO_c */
    gabi::store<f32>(h + 0x5C, 30.0f);
    gabi::store<f32>(h + 0x38, 0.9f);
    gabi::store<s16>(h + 0x64, (s16)0x1f40);
    gabi::store<f32>(h + 0x48, 2000.0f);
    gabi::store<f32>(h + 0x34, 3.0f);
    gabi::store<f32>(h + 0x50, 150.0f);
    gabi::store<f32>(h + 0x4C, 2.0f);
    gabi::store<f32>(h + 0x40, 2000.0f);
    gabi::store<f32>(h + 0x54, 20000.0f);
    gabi::store<u8>(h + 0x2E, 0);
    gabi::store<u8>(h + 0x31, 0);
    gabi::store<f32>(h + 0x6C, 10.0f);
    gabi::store<f32>(h + 0x3C, 100.0f);
    gabi::store<u8>(h + 0x30, 0);
    gabi::store<u8>(h + 0x2F, 0);
    gabi::store<s16>(h + 0x68, (s16)-0x1f40);
    gabi::store<f32>(h + 0x44, 0.7f);
    gabi::store<s16>(h + 0x8, (s16)0x1ffe);
    gabi::store<f32>(h + 0x4, -33.0f);
    gabi::store<s16>(h + 0x66, (s16)0xfa0);
    gabi::store<u8>(h + 0x2C, 0);
    gabi::store<s16>(h + 0xC, (s16)0x1000);
    gabi::store<s16>(h + 0x6A, (s16)-0xfa0);
    gabi::store<f32>(h + 0x60, 700.0f);
    gabi::store<f32>(h + 0x74, 300.0f);
    gabi::store<s16>(h + 0xE, (s16)0x2000);
    gabi::store<s16>(h + 0xA, (s16)0x1000);
    gabi::store<f32>(h + 0x58, 250.0f);
    gabi::store<f32>(h + 0x24, 400.0f);
    gabi::store<f32>(h + 0x1C, 50.0f);
    gabi::store<s16>(h + 0x10, (s16)-0xbb8);
    gabi::store<f32>(h + 0x70, 600.0f);
    gabi::store<u8>(h + 0x22, 0);
    gabi::store<s16>(h + 0x1A, (s16)0x150);
    gabi::store<s16>(h + 0x7C, (s16)0x5);
    gabi::store<f32>(h + 0x88, 120.0f);
    gabi::store<f32>(h + 0x80, -1.4f);
    gabi::store<f32>(h + 0x78, 100.0f);
    gabi::store<f32>(h + 0x90, 15.0f);
    gabi::store<s16>(h + 0x14, (s16)-0x1000);
    gabi::store<u8>(h + 0x94, 0);
    gabi::store<f32>(h + 0x84, 18.0f);
    gabi::store<s16>(h + 0x20, (s16)0x7fff);
    gabi::store<s16>(h + 0x18, (s16)0x250);
    gabi::store<s16>(h + 0x16, (s16)-0x2000);
    gabi::store<u8>(h + 0x2D, 0);
    gabi::store<f32>(h + 0x8C, 300.0f);
    gabi::store<s16>(h + 0x12, (s16)-0x1000);

    return h;
}
VERIFY(0x022E48B4, daNpc_So_HIO_ct);

/* 022E4AB4 __sinit_d_a_npc_so_cpp: the per-TU header statics, four camera floats, l_HIO */
static void __sinit_d_a_npc_so_cpp() {
    WWHD_FUNC(0x022E4AB4, void);
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x104686B0 + 4 * i, 0);
    __register_global_object(0x101C6368);
    gabi::store<f32>(0x10468694, -3.1415927f);
    gabi::store<f32>(0x10468698, 3.1415927f);
    gabi::call(0x028ED6F8, 0x104686AC);
    __register_global_object(0x101C6374);
    gabi::call(0x028EAB2C, 0x104686AD);
    __register_global_object(0x101C6380);
    gabi::store<f32>(0x1046869C, 50000.0f);
    gabi::store<f32>(0x104686A8, 10000.0f);
    gabi::store<f32>(0x104686A0, 50000.0f);
    gabi::store<f32>(0x104686A4, 10000.0f);
    daNpc_So_HIO_ct(SO_L_HIO);
}
VERIFY(0x022E4AB4, __sinit_d_a_npc_so_cpp);

/* 022E4B84: deleting destructor of an empty class (this TU's copy) */
static void so_empty_dt(void* p, s32 flags) {
    WWHD_FUNC(0x022E4B84, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x022E4B84, so_empty_dt);

/* 022E4BA0..022E4BB4, 022E4C9C: empty inline virtuals (this TU's copies) */
static void so_empty_022E4BA0(void*) {
    WWHD_FUNC(0x022E4BA0, void, (void*)nullptr);
}
VERIFY(0x022E4BA0, so_empty_022E4BA0);
static void so_empty_022E4BA4(void*) {
    WWHD_FUNC(0x022E4BA4, void, (void*)nullptr);
}
VERIFY(0x022E4BA4, so_empty_022E4BA4);
static void so_empty_022E4BA8(void*) {
    WWHD_FUNC(0x022E4BA8, void, (void*)nullptr);
}
VERIFY(0x022E4BA8, so_empty_022E4BA8);
static void so_empty_022E4BAC(void*) {
    WWHD_FUNC(0x022E4BAC, void, (void*)nullptr);
}
VERIFY(0x022E4BAC, so_empty_022E4BAC);
static void so_empty_022E4BB0(void*) {
    WWHD_FUNC(0x022E4BB0, void, (void*)nullptr);
}
VERIFY(0x022E4BB0, so_empty_022E4BB0);
static void so_empty_022E4BB4(void*) {
    WWHD_FUNC(0x022E4BB4, void, (void*)nullptr);
}
VERIFY(0x022E4BB4, so_empty_022E4BB4);
static void so_empty_022E4C9C(void*) {
    WWHD_FUNC(0x022E4C9C, void, (void*)nullptr);
}
VERIFY(0x022E4C9C, so_empty_022E4C9C);

/* 022E4BB8: daNpc_So_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_So_c_dt(daNpc_So_c* i_this, s32 flags) {
    WWHD_FUNC(0x022E4BB8, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mAcch) + 0x20, 0x10021BA0);  /* ~dBgS_ObjAcch: sub-object vtables */
        gabi::store<u32>(gabi::ea(&i_this->mAcch) + 0x14, 0x10021BB0);
        gabi::call(0x024EFD9C, &i_this->mAcch, 0);                      /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x02515AE8, &i_this->mSph, 2);                       /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mStts2, 2);
        /* ~fopNpc_npc_c (inline) */
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&static_cast<fopNpc_npc_c*>(i_this)->mAcchCir) + 0x14, 2); /* fopNpc_npc_c::mAcchCir.m_cir */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x10021BA0);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x10021BB0);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x022E4BB8, daNpc_So_c_dt);
