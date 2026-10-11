/**
 * d_a_npc_md_heap.cpp (WWHD)
 * Player - Medli: callbacks, node callbacks, heap, init, create (02283480..02286B58)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_md.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_md_heap.h"
using namespace MdHeap;

static s16 daNpc_Md_XyCheckCB(void*, int);
static s16 daNpc_Md_XyEventCB(void*, int);
static BOOL CheckCreateHeap(fopAc_ac_c*);
static BOOL nodeCallBack(J3DNode*, int);
static BOOL waistNodeCallBack(J3DNode*, int);
static BOOL armNodeCallBack(J3DNode*, int);
static BOOL hairTopNodeCallBack(J3DNode*, int);
static BOOL hairNodeCallBack(J3DNode*, int);

static SavePointer saveInfo;
static ActorPointer medli;
static u8 m_seaTalk;
static u8 m_flying;
static u8 m_mirror;
static cXyz l_offsetAttPosData;
static cXyz l_offsetEyePosData;
static u32 offsetAttPosGuard;
static u32 offsetEyePosGuard;
static VectorTable waistVecDat;
static MaskTable HairModeMaskData;
static HairNames hairName;
static EventNames event_name_tbl;
static NamePointer l_arc_name;
static NamePointer l_arc_name_ship;
static HeapSizes l_heep_size;
static BtpTable btpAnmTbl;
static dCcD_SrcCyl l_cyl_src;
static dCcD_SrcCyl l_wind_cyl_src;
static dCcD_SrcCyl l_light_cyl_src;
static dCcD_SrcCps l_fan_light_cps_src;
static ActionWords action_shipNpc;
static ActionWords action_harpWaitNpc;
static ActionWords action_waitNpc;
static inline void mdHeap_loadAction(ProcFunc_l* dst, const ActionWords& src) {
    auto* words = reinterpret_cast<ActionWords*>(dst);
    words->mWords[0] = src.mWords[0];
    words->mWords[1] = src.mWords[1];
}

#define MD_SAFESTRING_VTBL SAFE_STRING_VTABLE /* this TU's sead::SafeString vtable */

static inline dSv_event_c* md_event() { return reinterpret_cast<dSv_event_c*>(&saveInfo.mpInfo->mEvent); }
static inline BOOL md_isEventBit(u16 f) { return dSv_event_isEventBit(md_event(), f); }
static inline void md_onEventBit(u16 f) { dSv_event_onEventBit(md_event(), f); }
static inline u8 md_getSelectItem(int btn) { return dComIfGp_getSelectItem(btn); }
/* HD: dRes_control_c::getRes(const SafeString& arc, const SafeString& name)  */
/* inlined into Medli heap callbacks and creation */
static inline void* md_getObjectRes(const char* arc, const char* name) {
    SafeString a;
    SafeString n;
    a.mStringTop = gabi::guest_addr(arc);
    a.__vtbl = MD_SAFESTRING_VTBL;
    n.mStringTop = gabi::guest_addr(name);
    n.__vtbl = MD_SAFESTRING_VTBL;
    return mdHeap_getResource(dComIfG_resControl(), &a, &n);
}
/* the same with the two SafeStrings at given stack addresses */
static inline void* md_getObjectRes_at(SafeString* a, SafeString* n, const char* arc, u32 name) {
    a->mStringTop = gabi::guest_addr(arc);
    a->__vtbl = MD_SAFESTRING_VTBL;
    n->mStringTop = name;
    n->__vtbl = MD_SAFESTRING_VTBL;
    return mdHeap_getResource(dComIfG_resControl(), a, n);
}
static inline J3DModelData* md_getModelData(J3DModel* model) { return model->getModelData(); }
static inline J3DModel* md_morfModel(void* morf) { return reinterpret_cast<MdHeap::Morf*>(morf)->mpModel; }
static inline s32 J3DAnm_getFrameMax(void* animation) {
    auto* object = reinterpret_cast<MdHeap::Animation*>(animation);
    return object->getFrameMax();
}
static inline Mtx34* md_getAnmMtx(J3DModel* model, u32 joint) { return model->getAnmMtx(joint); }
static inline u32 md_jntNo(J3DNode* node) { return ((J3DJoint*)node)->getJntNo(); }
static inline Mtx34* md_mCurrentMtx() { return J3DSys::mCurrentMtx; }
static inline J3DModel* md_j3dSysModel() { return j3dSys.getModel(); }
static inline void PSMTXMultVecArray(const Mtx34* m, const cXyz* src, cXyz* dst, u32 n) { mdHeap_multVecArray(m, src, dst, n); }
/* c_xyz / c_m3d (HD: the cXyz results go through a hidden pointer in r4) */
static inline void cXyz_normZP(const cXyz* a, cXyz* out) { mdHeap_normZP(a, out); }
static inline void cXyz_norm(const cXyz* a, cXyz* out) { mdHeap_norm(a, out); }
static inline void cXyz_outprod(const cXyz* a, cXyz* out, const cXyz* b) { mdHeap_outprod(a, out, b); }
/* cXyz::normalizeZP: normalises in place; HD passes a result slot in r4  */
static inline void cXyz_normalizeZP(cXyz* a, cXyz* out) { mdHeap_normalizeZP(a, out); }
static inline void dKyw_get_AllWind_vec(cXyz* pos, cXyz* dir, be<f32>* pow) { mdHeap_getAllWind(pos, dir, pow); }
static inline void PSMTXRotAxisRad(Mtx34* m, const cXyz* axis, f32 rad) { mdHeap_rotAxisRad(m, axis, rad); }
static inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) { mdHeap_concat(a, b, ab); }
static inline f32 mdHeap_dotProductCall(const cXyz* a, const cXyz* b) { return mdHeap_dotProduct(a, b); }
static inline bool cM3d_UpMtx_Base(const cXyz* a, const cXyz* b, Mtx34* m) { return mdHeap_upMatrix(a, b, m); }
/* cM3dGTri (HD 0x38: the plane normal first) and cM3dGLin (0x1C) as stack objects */
static inline void cM3dGTri_ct(cM3dGTri* t, const cXyz* a, const cXyz* b, const cXyz* c) { mdHeap_constructTriangle(t, a, b, c); }
static inline void cM3dGLin_ct(cM3dGLin* l, const cXyz* a, const cXyz* b) { mdHeap_constructLine(l, a, b); }
static inline bool cM3d_Cross_LinTri(const cM3dGLin* l, const cM3dGTri* t, cXyz* out, bool a, bool b) {
    return mdHeap_crossLineTriangle(l, t, out, a, b);
}
static inline void cM3dGPla_Up(cM3dGTri* t, f32 d) { mdHeap_upTriangle(t, d); } /* (cM3dGTri::Up, unnamed)  */
static inline f32 cM3d_SignedLenPlaAndPos(const cM3dGTri* t, const cXyz* p) { return mdHeap_signedDistance(t, p); }
static inline f32 md_abs(const cXyz* v) { return std_sqrtf(PSVECSquareMag(v)); }
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { mdHeap_xyzRotM(m, x, y, z); }
static inline void McaMorf2_calc(mDoExt_McaMorf2* m) { mdHeap_calcMorf2(m); } /* mDoExt_McaMorf2::calc  */
static inline u32 J3DModelData_getJointName(J3DModelData* data) { return gabi::ea(data->getJointName()); }
static inline s32 JUTNameTab_getIndex(u32 table, u32 name) { return nameTable(table)->getIndex(nameString(name)); }
static inline void md_setJointCallBack(J3DModelData* data, u16 index, u32 callback) { data->getJointNodePointer(index)->mCallBack = callback; }
static inline void md_copyName(u32 dst, u32 src, u32 n) {
    for (u32 i = 0; i < n; i++) {
        bytes(dst)[i] = bytes(src)[i];
    }
}
static inline s32 md_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return mdHeap_initBtp(anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline s32 md_btkAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return mdHeap_initBtk(anm, d, p, play, attr, rate, start, end, modify, entry);
}

/* daNpc_Md_XyCheckCB (not named by the matcher)  */

/* daNpc_Md_XyEventCB (not named by the matcher)  */

/* hairCross (inlined in hairNodeCallBack): the part after the first cross test */
/* inlined into Medli heap callbacks and creation */
static BOOL hairCross_hit(cM3dGTri* tri, cM3dGLin* lin, cXyz* cross, cXyz* i_r4, cXyz* i_r5) {
    cM3dGPla_Up(tri, 2.0f);
    if (!cM3d_Cross_LinTri(lin, tri, cross, true, false)) {
        return FALSE;
    }
    f32 f31 = cM3d_SignedLenPlaAndPos(tri, i_r4);
    cXyz r1_12c;
    f32 nz = tri->mNormal.z;
    f32 nx = tri->mNormal.x;
    f32 ny = tri->mNormal.y;
    r1_12c.z = nz;
    r1_12c.x = nx;
    r1_12c.y = ny;
    cXyz t38;
    cXyz_ml(&r1_12c, &t38, -f31);
    cXyz r1_120;
    cXyz_pl(i_r4, &r1_120, &t38);
    cXyz r1_e4;
    cXyz_mi(i_r4, &r1_e4, i_r5);
    f32 f30 = md_abs(&r1_e4);
    cXyz r1_108;
    cXyz_mi(cross, &r1_108, &r1_120);
    cXyz_normalizeZP(&r1_108, &t38);
    cXyz_ml(&r1_108, &t38, std_sqrtf(gabi::fmsubs(f30, f30, f31 * f31)));
    r1_108.copy(t38);
    cXyz_pl(&r1_120, &t38, &r1_108);
    i_r5->copy(t38);
    return TRUE;
}
/* inlined into Medli heap callbacks and creation */
static BOOL hairCross(cXyz* i_r3, cXyz* i_r4, cXyz* i_r5) {
    cM3dGTri r1_198;
    cM3dGTri_ct(&r1_198, &i_r3[0], &i_r3[1], &i_r3[2]);
    cM3dGTri r1_160;
    cM3dGTri_ct(&r1_160, &i_r3[1], &i_r3[3], &i_r3[2]);
    cM3dGLin r1_144;
    cM3dGLin_ct(&r1_144, i_r4, i_r5);
    cXyz r1_138;
    if (cM3d_Cross_LinTri(&r1_144, &r1_198, &r1_138, true, false)) {
        return hairCross_hit(&r1_198, &r1_144, &r1_138, i_r4, i_r5);
    } else if (cM3d_Cross_LinTri(&r1_144, &r1_160, &r1_138, true, false)) {
        return hairCross_hit(&r1_160, &r1_144, &r1_138, i_r4, i_r5);
    }
    return FALSE;
}

/* vecChange (inlined in hairNodeCallBack) */
/* inlined into Medli heap callbacks and creation */
static void vecChange(cXyz* i_r3, cXyz* i_r4, s16 i_r5) {
    if (!(std::fabs(md_abs(i_r3)) < 3.814697265625e-06f)) { /* !cM3d_IsZero */
        if (!(std::fabs(md_abs(i_r4)) < 3.814697265625e-06f)) {
            cXyz r1_4c;
            cXyz_norm(i_r3, &r1_4c);
            cXyz r1_40;
            cXyz_norm(i_r4, &r1_40);
            f32 dot = mdHeap_dotProductCall(&r1_4c, &r1_40);
            if (dot < cM_scos(i_r5)) {
                cXyz r1_34;
                cXyz_outprod(&r1_4c, &r1_34, &r1_40);
                PSMTXRotAxisRad(mDoMtx_stack_c::get(), &r1_34, 0.39269909262657166f /* cM_s2rad(0x1000) */);
                PSMTXMultVec(mDoMtx_stack_c::get(), &r1_4c, i_r4);
            } else {
                i_r4->copy(r1_40);
            }
        }
    }
}

/* 02284418 armNodeCallBack (not named by the matcher): copies the arm/wing joint matrices of the
 * body model to the arm model */

/* joint index of a name, with the GameCube assertion (index >= 0) */
static inline s8 md_jntIndex(J3DModelData* md, u32 name, u32 line, u32 msg, be<s8>* dst) {
    s8 idx = (s8)JUTNameTab_getIndex(J3DModelData_getJointName(md), name);
    *dst = idx;
    if (idx < 0) {
        JUT_ASSERT_fail(STR(HEAP_ASSERT_FILE), line, nameString(msg));
    }
    return idx;
}

/* CheckCreateHeap (not named by the matcher)  */

/* sead::SafeString operator== (HD inline, as d_a_mo2): cstr() of both operands through the vtable
 * (slot 0x14; the left one twice), pointer compare, then a bounded strcmp (0x40001 characters) */
/* inlined into Medli heap callbacks and creation */
static inline bool md_isStartStage(u32 lit) {
    SafeString sa;
    sa.mStringTop = lit;
    sa.__vtbl = MD_SAFESTRING_VTBL;
    u32 stage = gabi::ea(dComIfGp_getStartStageName()); /* dComIfGp_getStartStageName() */
    SafeString sb;
    sb.mStringTop = stage;
    sb.__vtbl = MD_SAFESTRING_VTBL;
    safeStringCstr(&sa);
    safeStringCstr(&sa);
    u32 pa = sa.mStringTop;
    safeStringCstr(&sb);
    u32 pb = sb.mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = bytes(pa)[i].get();
        if (ca != bytes(pb)[i].get()) return false;
        if (ca == 0) return true;
    }
    return false;
}

/* 02283584 */
static s16 daNpc_Md_XyCheckCB(void* i_this, int param_1) {
    return static_cast<daNpc_Md_c*>(i_this)->XyCheckCB(param_1);
}

/* 02283480 */
s16 daNpc_Md_c::XyCheckCB(int i_itemBtn) {
    u8 selectItemNo = md_getSelectItem(i_itemBtn);
    if (mType == TYPE_SEA) {
        if (selectItemNo == dItemNo_WIND_WAKER_e /* dItemNo_WIND_WAKER_e */) {
            return TRUE;
        }
        if (selectItemNo == dItemNo_GOLDEN_FEATHER_e /* dItemNo_GOLDEN_FEATHER_e */) {
            if (!md_isEventBit(UNK_2E40) || (md_isEventBit(UNK_2E40) && m_seaTalk != 0)) {
                return TRUE;
            }
        }
    } else if (mType == TYPE_M_DAI || mType == TYPE_EDAICHI) {
        if (selectItemNo == dItemNo_GOLDEN_FEATHER_e && !md_isEventBit(UNK_3B80)) {
            return TRUE;
        }
    }
    if (selectItemNo == dItemNo_GOLDEN_FEATHER_e) {
        md_onEventBit(UNK_2C08);
    }
    return FALSE;
}

/* 02283610 */
static s16 daNpc_Md_XyEventCB(void* i_this, int param_1) {
    return static_cast<daNpc_Md_c*>(i_this)->XyEventCB(param_1);
}

/* 02283588 */
s16 daNpc_Md_c::XyEventCB(int i_itemBtn) {
    u8 selectItemNo = md_getSelectItem(i_itemBtn);
    if (selectItemNo == dItemNo_WIND_WAKER_e) {
        clearStatus(daMdStts_DEFAULT_TALK_XY);
        return mEventIdxTable[5];
    } else if (selectItemNo == dItemNo_GOLDEN_FEATHER_e) {
        setBitStatus(daMdStts_DEFAULT_TALK_XY);
    }
    return -1;
}

/* 02285380 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daNpc_Md_c*>(i_this)->createHeap();
}

/* 02286084 */
cPhs_State daNpc_Md_c::create() {
    m313D = 0;
    mdHeap_copyString(mModelArcName, l_arc_name.mName.get() /* l_arc_name "Md" */);
    int heapSizeIdx = 0;

    /* fopAcM_ct(this, daNpc_Md_c): the constructor, inlined */
    if (!(actor_condition & 8)) {
        if (this != nullptr) {
            mdHeap_constructNpc(this);
            __vtbl = MD_VTBL;
            mdHeap_constructBtp(m0520);
            mDoExt_btkAnm::ct(&mLightBtkAnm);
            mdHeap_constructArray(mAcchCir, 2, sizeof(dBgS_AcchCir), MdHeap::ACCH_CIRCLE_CONSTRUCTOR);
            auto* line = reinterpret_cast<MdHeap::LinCheck*>(mLinChk);
            mdHeap_constructLinCheck(line);
            line->mFlags[4] = 0;
            line->mFlags[1] = 0;
            line->mFlags[6] = 0;
            line->mpPolyPass = &line->mPolyPassVtable;
            line->mVtable = MdHeap::LIN_CHECK_VTABLE;
            line->mpGroupPass = &line->mGroupPassVtable;
            line->mLineVtable = MdHeap::MIRROR_LINE_VTABLE;
            line->mFlags[5] = 0;
            line->mFlags[3] = 1;
            line->mGroupPassVtable = MdHeap::MIRROR_GROUP_PASS_VTABLE;
            line->mFlags[2] = 0;
            line->mGroup = 0x1F;
            line->mPolyPassVtable = MdHeap::MIRROR_POLY_PASS_VTABLE;
            line->mFlags[0] = 0;
            dCcD_Stts_ct(&mStts);
            // HD: each collision shape keeps this translation unit's AABB vtable.
            dCcD_Cyl_ct(&mCyl1, MdHeap::AABB_VTABLE);
            dCcD_Cyl_ct(&mCyl2, MdHeap::AABB_VTABLE);
            dCcD_Cyl_ct(&mCyl3, MdHeap::AABB_VTABLE);
            auto* capsule = reinterpret_cast<MdHeap::Capsule*>(&mCps);
            mdHeap_constructObjectInfo(&mCps);
            capsule->mAabbVtable = MdHeap::AABB_VTABLE;
            capsule->mShapeVtable = MdHeap::SHAPE_VTABLE;
            mdHeap_constructCapsule(&capsule->mGeometry);
            capsule->mHitVtable = MdHeap::CAPSULE_VTABLE;
            capsule->mGeometry.__vtbl = MdHeap::CAPSULE_GEOMETRY_VTABLE;
            capsule->mShapeVtable = MdHeap::CAPSULE_SHAPE_VTABLE;
            mdHeap_constructJointControl(&mJntCtrl); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
            mdHeap_constructMirror(m0B70);    /* dDlst_mirrorPacket::dDlst_mirrorPacket */
            m304C.__vtbl = MTX_FOLLOW_CALLBACK_VTABLE;         /* daPy_mtxFollowEcallBack_c */
            m3058.__vtbl = FOLLOW_CALLBACK_VTABLE;         /* daNpc_Md_followEcallBack_c */
            mdHeap_constructRipple(&m3074);    /* dPa_rippleEcallBack::dPa_rippleEcallBack */
            /* cBgS_PolyInfo mPolyInfo */
            mPolyInfo.mpBgW = 0;
            mPolyInfo.__vtbl = POLY_INFO_VTABLE;
            mPolyInfo.mPolyIndex = 0xFFFF;
            mPolyInfo.mBgIndex = 0x100;
            mPolyInfo.mProcId = -1;
        }
        actor_condition = actor_condition | 8;
    }

    mType = (u8)(fopAcM_GetParam(this) >> 0x08); /* setTalkType; HD: the (always false) == -2 test is gone */
    if (dComIfGs_isCollect(0, 2)) {
        if (!md_isStartStage(gabi::guest_addr("M_DaiB") /* "M_DaiB" */)) {
            return cPhs_ERROR_e;
        }
        mType = TYPE_M_DAIB;
    } else if (md_isStartStage(gabi::guest_addr("sea") /* "sea" */)) {
        if (md_isEventBit(UNK_2E04) || !md_isEventBit(UNK_1820) || !dComIfGs_isStageBossEnemy(STAGE_DRC)) {
            return cPhs_ERROR_e;
        }
    } else if (md_isStartStage(gabi::guest_addr("Atorizk") /* "Atorizk" */)) {
        if (md_isEventBit(UNK_2E04) || mdHeap_checkLetterPassed()) {
            return cPhs_ERROR_e;
        }
    } else if (md_isStartStage(gabi::guest_addr("Adanmae") /* "Adanmae" */)) {
        if (md_isEventBit(UNK_2E04) || !mdHeap_checkLetterPassed()) {
            return cPhs_ERROR_e;
        }
    } else if (md_isStartStage(gabi::guest_addr("M_Dra09") /* "M_Dra09" */)) {
        if (md_isEventBit(UNK_2E04) || md_isEventBit(UNK_1101)) {
            return cPhs_ERROR_e;
        }
    } else if (md_isStartStage(gabi::guest_addr("Edaichi") /* "Edaichi" */)) {
        if (!md_isEventBit(UNK_2E04) || md_isEventBit(UNK_2920)) {
            return cPhs_ERROR_e;
        }
        mType = TYPE_EDAICHI;
    } else if (md_isStartStage(gabi::guest_addr("M_Dai") /* "M_Dai" */)) {
        if (!md_isEventBit(UNK_2E04) || !md_isEventBit(UNK_2920)) {
            return cPhs_ERROR_e;
        }
        mType = TYPE_M_DAI;
    } else if (md_isStartStage(gabi::guest_addr("M_DaiB") /* "M_DaiB" */)) {
        return cPhs_ERROR_e;
    }

    if (!md_isEventBit(UNK_2E04) && md_isEventBit(UNK_1608)) {
        mType = TYPE_SHIP_RIDE;
        mdHeap_copyString(mModelArcName, l_arc_name_ship.mName.get() /* l_arc_name_ship "Md_ship" */);
        heapSizeIdx = 1;
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhase, mModelArcName);
    m313D = 1;
    if (phase_state == cPhs_COMPLEATE_e) {
        if (dComIfGp_getCb1Player() != nullptr) { /* dComIfGp_getCb1Player() */
            return cPhs_ERROR_e;
        }
        /* static int l_heep_size[] = {0x7660, 0x61C0} */
        if (!mdHeap_entrySolidHeap(this, gabi::guest_fn((const void*)CheckCreateHeap),
                              l_heep_size.values[heapSizeIdx])) {
            mpArmMorf = nullptr;
            mpMorf = nullptr;
            mpWingMorf = nullptr;
            return cPhs_ERROR_e;
        }

        if (mType == TYPE_M_DAI) {
            SaveInfo* sv = saveInfo.mpInfo;
            Priest* priest = &sv->mPriest; /* dSv_player_priest_c: pos, rotate +0xC, roomNo +0xE, flag +0xF */
            if (sv->mPriest.mFlag == 2) {
                s8 roomNo = priest->mRoomNo;
                s16 rot = priest->mRotate;
                mdHeap_setRestartOption(&sv->mRestart, 2, &priest->mPos, rot, roomNo);
                /* HD: the priest position is set again */
                mdHeap_setPriest(&saveInfo.mpInfo->mPriest, 2, &priest->mPos, rot, roomNo);
            }
            mdHeap_checkRestart(this, 2);
        }

        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(md_morfModel(mpMorf.get())));
        /* HD: no l_HIO child (mDoHIO_createChild) */
        if (!init()) {
            return cPhs_ERROR_e;
        }

        setBaseMtx();
        fopAcM_setStageLayer(this);
        medli.mpActor = this; /* HD: a global pointer to Medli */
    }

    return phase_state;
}

/* 02283614 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DModel* model = md_j3dSysModel();
        daNpc_Md_c* i_this = static_cast<daNpc_Md_c*>(model->getUserArea());
        if (i_this != nullptr) {
            // HD: retain the interleaved guard and vector stores.
            cXyz* l_offsetAttPos = &l_offsetAttPosData;
            if (offsetAttPosGuard == 0) {
                l_offsetAttPos->x = 0.0f;
                l_offsetAttPos->z = 0.0f;
                offsetAttPosGuard = 1;
                l_offsetAttPos->y = 0.0f;
            }
            cXyz* l_offsetEyePos = &l_offsetEyePosData;
            if (offsetEyePosGuard == 0) {
                l_offsetEyePos->y = 0.0f;
                offsetEyePosGuard = 1;
                l_offsetEyePos->x = 15.0f;
                l_offsetEyePos->z = 0.0f;
            }
            u32 jntNo = md_jntNo(node);
            Mtx34* stk = mDoMtx_stack_c::get();
            PSMTXCopy(md_getAnmMtx(model, jntNo), stk);
            PSMTXMultVec(stk, l_offsetAttPos, &i_this->m3094);
            mDoMtx_XrotM(stk, i_this->mJntCtrl.mAngles[0][1]);
            mDoMtx_ZrotM(stk, -i_this->mJntCtrl.mAngles[0][0]);
            PSMTXMultVec(stk, l_offsetEyePos, &i_this->m3088);
            if (i_this->m312B != 0xFF) {
                i_this->m312B = i_this->m312B + 1;
            }
            PSMTXCopy(stk, md_mCurrentMtx());
            mtx_copy(md_getAnmMtx(model, jntNo), stk);
        }
    }
    return TRUE;
}

/* 022837D0 */
static BOOL waistNodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DModel* model = md_j3dSysModel();
        daNpc_Md_c* i_this = static_cast<daNpc_Md_c*>(model->getUserArea());
        if (i_this != nullptr) {
            u32 jntNo = md_jntNo(node);
            md_getAnmMtx(model, jntNo); /* HD: the dirty flag is set twice */
            Mtx34* r3 = md_getAnmMtx(model, jntNo);
            PSMTXMultVecArray(r3, waistVecDat.values /* waistVecDat */, i_this->m3234, 4);
            Mtx34* stk = mDoMtx_stack_c::get();
            PSMTXCopy(md_getAnmMtx(model, jntNo), stk);
            if (i_this->checkStatus(daNpc_Md_c::daMdStts_UNK4000)) {
                mDoMtx_XrotM(stk, i_this->m3116);
                mDoMtx_ZrotM(stk, i_this->m3114);
            } else {
                mDoMtx_XrotM(stk, i_this->mJntCtrl.mAngles[1][1]);
                mDoMtx_ZrotM(stk, -i_this->mJntCtrl.mAngles[1][0]);
            }
            PSMTXCopy(stk, md_mCurrentMtx());
            mtx_copy(md_getAnmMtx(model, jntNo), stk);
        }
    }
    return TRUE;
}

/* 02284418 */
static BOOL armNodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DModel* armModel = md_j3dSysModel();
        daNpc_Md_c* i_this = static_cast<daNpc_Md_c*>(armModel->getUserArea());
        if (i_this != nullptr) {
            u32 armJntNo = md_jntNo(node);
            s32 bodyJntNo;
            J3DModel* bodyModel = md_morfModel(i_this->mpMorf.get());
            /* HD: the GameCube "else if (armJntNo == ArmLlocJntNum)" is a plain else */
            if (armJntNo == (u32)(s32)i_this->m_armRloc_jnt_num) {
                bodyJntNo = i_this->m_armR_jnt_num;
            } else {
                bodyJntNo = i_this->m_armL_jnt_num;
            }
            PSMTXCopy(md_getAnmMtx(bodyModel, bodyJntNo), md_mCurrentMtx());
            Mtx34* src = md_getAnmMtx(bodyModel, bodyJntNo);
            mtx_copy(md_getAnmMtx(armModel, armJntNo), src);
        }
    }
    return TRUE;
}

/* 022839F0 */
static BOOL hairTopNodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DModel* model = md_j3dSysModel();
        daNpc_Md_c* i_this = static_cast<daNpc_Md_c*>(model->getUserArea());
        if (i_this != nullptr) {
            u32 jntNo = md_jntNo(node);
            Mtx34* mtx = md_getAnmMtx(model, jntNo);
            cXyz* hairTopPos = &i_this->m3174[0];
            f32 x = mtx->m[0][3];
            f32 y = mtx->m[1][3];
            f32 z = mtx->m[2][3];
            hairTopPos->y = y;
            hairTopPos->x = x;
            hairTopPos->z = z;
        }
    }
    return TRUE;
}

/* 02283A78 */
static BOOL hairNodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DModel* model = md_j3dSysModel();
        daNpc_Md_c* i_this = static_cast<daNpc_Md_c*>(model->getUserArea());
        if (i_this != nullptr) {
            u32 jntNo = md_jntNo(node);
            for (int i = 1; i < 8; i++) {
                if ((u32)(s32)i_this->m_hair_jnt_nums[i] != jntNo) {
                    continue;
                }

                Mtx34* mtx = md_getAnmMtx(model, jntNo);
                u8 mask = HairModeMaskData.values[i]; /* HairModeMaskData[i] */
                if (!(u8)(i_this->m3134 & mask)) {
                    i_this->m3134 = i_this->m3134 | mask;
                    cXyz* hairPos = &i_this->m3174[i];
                    f32 y = mtx->m[1][3];
                    f32 x = mtx->m[0][3];
                    f32 z = mtx->m[2][3];
                    hairPos->y = y;
                    hairPos->x = x;
                    hairPos->z = z;
                    cXyz tmp;
                    cXyz_mi(hairPos, &tmp, &i_this->m3174[i - 1]);
                    cXyz* r29 = &i_this->m31D4[i];
                    r29->copy(tmp);
                    i_this->m3264[i] = md_abs(r29);
                    break;
                }

                cXyz* r25 = &i_this->m3174[i - 1];
                cXyz* r28 = &i_this->m3174[i];
                cXyz* r24 = &i_this->m31D4[i];
                if (i <= 1) {
                    r28->x = mtx->m[0][3];
                    r28->y = mtx->m[1][3];
                    r28->z = mtx->m[2][3];
                    cXyz tmp;
                    cXyz_mi(r28, &tmp, r25);
                    r24->copy(tmp);
                    break;
                }

                be<f32>* hairDist = &i_this->m3264[i];
                cXyz* r22 = &i_this->m31D4[i - 1];
                cXyz r1_d0;
                r1_d0.x = r28->x;
                r1_d0.y = r28->y;
                r1_d0.z = r28->z;
                cXyz r1_c4;
                cXyz_mi(&r1_d0, &r1_c4, r25);
                cXyz r1_e8;
                r1_e8.x = 0.0f;
                r1_e8.z = 0.0f;
                r1_e8.y = -1.0f;
                PSVECScale(&r1_e8, &r1_e8, l_HIO().m14C);
                cXyz r1_dc;
                gabi::Local<be<f32>> power;
                dKyw_get_AllWind_vec(&i_this->current.pos, &r1_dc, power);
                cXyz t20;
                cXyz_ml(&r1_dc, &t20, *power);
                cXyz t84;
                cXyz_ml(&t20, &t84, 3.0f);
                PSVECAdd(&r1_e8, &t84, &r1_e8);
                PSVECAdd(&r1_c4, &r1_e8, &r1_c4);
                cXyz_normZP(&r1_c4, &t84);
                cXyz_ml(&t84, &t20, *hairDist);
                r1_c4.copy(t20);
                vecChange(r22, &r1_c4, 0x1000);
                cXyz_ml(&r1_c4, &t20, *hairDist);
                r24->copy(t20);
                cXyz_pl(r24, &t20, r25);
                r28->copy(t20);
                if (hairCross(i_this->m3234, &i_this->m3174[1], r28)) {
                    cXyz t78;
                    cXyz_mi(r28, &t78, r25);
                    r24->copy(t78);
                    cXyz_normalizeZP(r24, &t78);
                    r1_c4.copy(*r24);
                    cXyz t148;
                    cXyz_ml(&r1_c4, &t148, *hairDist);
                    cXyz_pl(&t148, &t78, r25);
                    r28->copy(t78);
                }

                s32 prevHairJntNo = i_this->m_hair_jnt_nums[i - 1];
                cXyz r1_b8;
                cXyz_normZP(r22, &r1_b8);
                Mtx34 r1_124;
                cM3d_UpMtx_Base(&r1_b8, &r1_c4, &r1_124);
                Mtx34 r1_f4;
                PSMTXCopy(md_getAnmMtx(model, prevHairJntNo), &r1_f4);
                r1_f4.m[1][3] = 0.0f;
                r1_f4.m[0][3] = 0.0f;
                r1_f4.m[2][3] = 0.0f;
                PSMTXCopy(&r1_f4, mDoMtx_stack_c::get());
                PSMTXConcat(&r1_124, mDoMtx_stack_c::get(), mDoMtx_stack_c::get()); /* revConcat */
                PSMTXCopy(mDoMtx_stack_c::get(), mtx);
                mtx->m[0][3] = r28->x;
                mtx->m[1][3] = r28->y;
                mtx->m[2][3] = r28->z;
                PSMTXCopy(mtx, md_mCurrentMtx());
                break;
            }
        }
    }
    return TRUE;
}

/* 022847F8 */
BOOL daNpc_Md_c::createHeap() {
    /* The name SafeStrings of the two wait animations point into the stack name buffers; the
     * harness compares such pointers relative to the SafeString, so the buffers and their
     * SafeStrings share one stack object laid out as GHS's frame (buffers 0x68 above the name) */
    gabi::Local<HeapFrame> frame;
    u32 wait_anim_name = gabi::ea(frame->wait_anim_name);
    u32 arm_wait_anim_name = gabi::ea(frame->arm_wait_anim_name);
    if (isTypeShipRide()) {
        md_copyName(wait_anim_name, gabi::guest_addr("md_shipwait.bck"), 0x10);     /* "md_shipwait.bck" */
        md_copyName(arm_wait_anim_name, gabi::guest_addr("mdarm_shipwait.bck"), 0x13); /* "mdarm_shipwait.bck" */
    } else {
        md_copyName(wait_anim_name, gabi::guest_addr("md_wait01.bck"), 0xE);      /* "md_wait01.bck" */
        md_copyName(arm_wait_anim_name, gabi::guest_addr("mdarm_wait01.bck"), 0x11); /* "mdarm_wait01.bck" */
    }

    J3DModelData* modelData = (J3DModelData*)md_getObjectRes(mModelArcName, "md.bdl" /* "md.bdl" */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(HEAP_ASSERT_FILE), 0x843, STR(MODEL_DATA_ASSERT));
    }
    void* anm = md_getObjectRes_at(&frame->mBodyArc, &frame->mBodyName, mModelArcName, wait_anim_name);
    mpMorf = mdHeap_newMorf2(0u, modelData, 0u, 0u, anm, 0u,
                                          2 /* EMode_LOOP */, 1.0f, 0, -1, 1, 0u, 0x00080000u, 0x11020022u);
    if (mpMorf.get() == nullptr || md_morfModel(mpMorf.get()) == nullptr) {
        return FALSE;
    }

    md_jntIndex(modelData, gabi::guest_addr("backbone1") /* "backbone1" */, 0x856, BACKBONE_FIRST_ASSERT, &m_backbone1_jnt_num);
    md_jntIndex(modelData, gabi::guest_addr("backbone2") /* "backbone2" */, 0x859, BACKBONE_SECOND_ASSERT, &m_backbone2_jnt_num);
    md_jntIndex(modelData, gabi::guest_addr("armR") /* "armR" */, 0x85C, ARM_RIGHT_ASSERT, &m_armR_jnt_num);
    md_jntIndex(modelData, gabi::guest_addr("armL") /* "armL" */, 0x85F, ARM_LEFT_ASSERT, &m_armL_jnt_num);
    md_jntIndex(modelData, gabi::guest_addr("neck") /* "neck" */, 0x862, NECK_ASSERT, &m_neck_jnt_num);

    s16 head_jnt_num = (s16)JUTNameTab_getIndex(J3DModelData_getJointName(modelData), gabi::guest_addr("head") /* "head" */);
    if (head_jnt_num >= 0) {
        md_setJointCallBack(modelData, head_jnt_num, gabi::guest_fn((const void*)nodeCallBack));
    }
    md_setJointCallBack(modelData, (s16)(s8)m_backbone1_jnt_num, gabi::guest_fn((const void*)waistNodeCallBack));
    int i = 0;
    {
        u32 name = gabi::ea(hairName.values[0].get()); /* hairName[0] */
        s8 idx = (s8)JUTNameTab_getIndex(J3DModelData_getJointName(modelData), name);
        m_hair_jnt_nums[i] = idx;
        if (idx >= 0) {
            md_setJointCallBack(modelData, idx, gabi::guest_fn((const void*)hairTopNodeCallBack));
        }
    }
    for (i = 1; i < 8; i++) {
        u32 name = gabi::ea(hairName.values[i].get());
        s8 idx = (s8)JUTNameTab_getIndex(J3DModelData_getJointName(modelData), name);
        m_hair_jnt_nums[i] = idx;
        if (idx >= 0) {
            md_setJointCallBack(modelData, idx, gabi::guest_fn((const void*)hairNodeCallBack));
        }
    }

    md_morfModel(mpMorf.get())->setUserArea((uintptr_t)this);

    modelData = (J3DModelData*)md_getObjectRes(mModelArcName, "mdarm.bdl" /* "mdarm.bdl" */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(HEAP_ASSERT_FILE), 0x8A0, STR(MODEL_DATA_ASSERT));
    }
    anm = md_getObjectRes_at(&frame->mArmArc, &frame->mArmName, mModelArcName, arm_wait_anim_name);
    mpArmMorf = mdHeap_newMorf2(0u, modelData, 0u, 0u, anm, 0u, 2, 1.0f, 0, -1, 0, 0u, 0u, 0x11020203u);
    if (mpArmMorf.get() == nullptr || md_morfModel(mpArmMorf.get()) == nullptr) {
        return FALSE;
    }

    md_jntIndex(modelData, gabi::guest_addr("armRloc") /* "armRloc" */, 0x8AF, ARM_RIGHT_LOCATION_ASSERT, &m_armRloc_jnt_num);
    md_jntIndex(modelData, gabi::guest_addr("armLloc") /* "armLloc" */, 0x8B2, ARM_LEFT_LOCATION_ASSERT, &m_armLloc_jnt_num);
    md_jntIndex(modelData, gabi::guest_addr("handL") /* "handL" */, 0x8B5, HAND_LEFT_ASSERT, &m_handL_jnt_num);

    md_setJointCallBack(modelData, (s16)(s8)m_armRloc_jnt_num, gabi::guest_fn((const void*)armNodeCallBack));
    md_setJointCallBack(modelData, (s16)(s8)m_armLloc_jnt_num, gabi::guest_fn((const void*)armNodeCallBack));

    md_morfModel(mpArmMorf.get())->setUserArea((uintptr_t)this);

    if (!isTypeShipRide()) {
        modelData = (J3DModelData*)md_getObjectRes(mModelArcName, "mdwing.bdl" /* "mdwing.bdl" */);
        if (modelData == nullptr) {
            JUT_ASSERT_fail(STR(HEAP_ASSERT_FILE), 0x8CC, STR(MODEL_DATA_ASSERT));
        }
        anm = md_getObjectRes(mModelArcName, "mdwing_wait01.bck" /* "mdwing_wait01.bck" */);
        mpWingMorf = mdHeap_newMorf(0u, modelData, 0u, 0u, anm,
                                                 2 /* EMode_LOOP */, 1.0f, 0, -1, 0, 0u, 0u, 0x11020203u);
        if (mpWingMorf.get() == nullptr || md_morfModel(mpWingMorf.get()) == nullptr) {
            return FALSE;
        }

        md_jntIndex(modelData, gabi::guest_addr("wingRloc") /* "wingRloc" */, 0x8DB, WING_RIGHT_LOCATION_ASSERT, &m_wingRloc_jnt_num);
        md_jntIndex(modelData, gabi::guest_addr("wingLloc") /* "wingLloc" */, 0x8DE, WING_LEFT_LOCATION_ASSERT, &m_wingLloc_jnt_num);
        md_jntIndex(modelData, gabi::guest_addr("wingR2") /* "wingR2" */, 0x8E1, WING_RIGHT_SECOND_ASSERT, &m_wingR2_jnt_num);
        md_jntIndex(modelData, gabi::guest_addr("wingL2") /* "wingL2" */, 0x8E4, WING_LEFT_SECOND_ASSERT, &m_wingL2_jnt_num);
        md_jntIndex(modelData, gabi::guest_addr("wingR3") /* "wingR3" */, 0x8E7, WING_RIGHT_THIRD_ASSERT, &m_wingR3_jnt_num);
        md_jntIndex(modelData, gabi::guest_addr("wingL3") /* "wingL3" */, 0x8EA, WING_LEFT_THIRD_ASSERT, &m_wingL3_jnt_num);

        md_setJointCallBack(modelData, (s16)(s8)m_wingRloc_jnt_num, gabi::guest_fn((const void*)armNodeCallBack));
        md_setJointCallBack(modelData, (s16)(s8)m_wingLloc_jnt_num, gabi::guest_fn((const void*)armNodeCallBack));

        md_morfModel(mpWingMorf.get())->setUserArea((uintptr_t)this);
    }

    modelData = (J3DModelData*)md_getObjectRes(mModelArcName, "md_harp.bdl" /* "md_harp.bdl" */);
    mpHarpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (mpHarpModel.get() == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)md_getObjectRes(mModelArcName, "ymdhp00.bdl" /* "ymdhp00.bdl" */);
    mpHarpLightModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (mpHarpLightModel.get() == nullptr) {
        return FALSE;
    }

    if (!initLightBtkAnm(false)) {
        return FALSE;
    }
    if (!initTexPatternAnm(0, 0)) {
        return FALSE;
    }

    mAcchCir[0].SetWall(20.0f, 20.0f);
    mAcchCir[1].SetWall(60.0f, 20.0f);
    mAcch.Set(&current.pos, &old.pos, this, 2, mAcchCir, &speed);
    u32 flags = mAcch.m_flags;
    mAcch.m_roof_crr_height = 120.0f;                    /* SetRoofCrrHeight(120.0f) */
    mAcch.m_flags = ((flags & ~8u) | 0x2000) & ~0x400u; /* ClrRoofNone, OnLineCheck, ClrWaterNone */
    return TRUE;
}

/* 02285384 */
BOOL daNpc_Md_c::setAction(ProcFunc_l* pCurrActionFunc, ProcFunc_l* newActionFunc, void* arg) {
    s16 newI = newActionFunc->i;
    s16 newD;
    u32 newF;
    bool endCurrent = true;
    if ((u16)pCurrActionFunc->i == (u16)newI) {
        if (pCurrActionFunc->i == 0) {
            return TRUE;
        }
        newD = newActionFunc->d;
        newF = newActionFunc->f;
        if (!((u16)pCurrActionFunc->d != (u16)newD || pCurrActionFunc->f != newF)) {
            return TRUE;
        }
    } else {
        newF = newActionFunc->f;
        newD = newActionFunc->d;
        if (pCurrActionFunc->i == 0) {
            endCurrent = false;
        }
    }
    if (endCurrent) {
        mActionStatus = ACTION_ENDING;
        md_pmf_call<BOOL>(this, pCurrActionFunc, arg);
    }
    pCurrActionFunc->i = newI;
    pCurrActionFunc->f = newF;
    pCurrActionFunc->d = newD;
    m3150 = 0.0f;
    m314A = 0;
    m3146 = 0;
    m3148 = 0;
    m3144 = 0;
    mActionStatus = ACTION_STARTING;
    md_pmf_call<BOOL>(this, pCurrActionFunc, arg);
    return TRUE;
}

/* 02285520 */
BOOL daNpc_Md_c::shipRideCheck() {
    if (isTypeShipRide()) {
        gabi::Local<ProcFunc_l> fn;
        mdHeap_loadAction(fn, action_shipNpc);
        setNpcAction(fn, nullptr);
        return TRUE;
    }
    return FALSE;
}

/* 022846EC */
BOOL daNpc_Md_c::initTexPatternAnm(u8 btpAnmTblIdx, u32 param_2) {
    BtpEntry* tbl = &btpAnmTbl.values[btpAnmTblIdx]; /* static btpAnmTbl[] {char name[0x20]; u8 m20;} */
    J3DModelData* modelData = md_getModelData(md_morfModel(mpMorf.get()));
    bool ret = false;
    void* eyeTexPtrn = md_getObjectRes(mModelArcName, reinterpret_cast<const char*>(tbl->m00));
    if (eyeTexPtrn == nullptr) {
        JUT_ASSERT_fail(STR(EYE_PATTERN_ASSERT_FILE), 0x1E70, STR(EYE_PATTERN_ASSERT));
    }
    if (md_btpAnm_init(m0520, modelData, eyeTexPtrn, 1, 1 /* EMode_RESET */, 1.0f, 0, -1, param_2, 0)) {
        m3112 = (s16)J3DAnm_getFrameMax(eyeTexPtrn);
        m3133 = 0;
        m3136 = tbl->m20;
        m3137 = btpAnmTblIdx;
        ret = true;
    }
    return ret;
}

/* 0228461C */
BOOL daNpc_Md_c::initLightBtkAnm(u32 param_1) {
    J3DModelData* pModelData = md_getModelData(mpHarpLightModel.get());
    bool ret = false;
    void* a_lightBtk = md_getObjectRes(mModelArcName, "ymdhp00.btk" /* "ymdhp00.btk" */);
    if (a_lightBtk == nullptr) {
        JUT_ASSERT_fail(STR(LIGHT_BTK_ASSERT_FILE), 0x1EA9, STR(LIGHT_BTK_ASSERT));
    }
    if (md_btkAnm_init(&mLightBtkAnm, pModelData, a_lightBtk, 1, 2 /* EMode_LOOP */, 1.0f, 0, -1, param_1, 0)) {
        ret = true;
    }
    return ret;
}

/* 02285984 */
void daNpc_Md_c::setAttention(bool param_1) {
    if (!param_1 && m312B >= 2) {
        return;
    }
    eyePos.y = m3088.y;
    eyePos.z = m3088.z;
    eyePos.x = m3088.x;
    cXyz* attPos = &attention_info.position;
    f32 fVar1;
    if (m_flying != 0) { /* isFlying() */
        fVar1 = 70.0f;
    } else if (dComIfGp_getPlayer(0) == this) {
        fVar1 = 110.0f;
    } else {
        fVar1 = l_HIO().mNpc.mAttnYOffset;
    }
    f32 z = current.pos.z;
    f32 y = current.pos.y + fVar1;
    f32 x = current.pos.x;
    attPos->z = z;
    attPos->x = x;
    attPos->y = y;
}

/* 02285A6C */
void daNpc_Md_c::setBaseMtx() {
    J3DModel* model = md_morfModel(mpMorf.get());
    Mtx34* now = mDoMtx_stack_c::get();
    u32 status = m30F0; /* HD: read before the matrix calls */
    if (actor_status & fopAcStts_CARRY_e) { /* fopAcM_checkCarryNow(this) */
        PSMTXTrans(now, current.pos.x, current.pos.y, current.pos.z); /* transS */
        if (status & daMdStts_CARRY_ACTION) { /* isNoCarryAction() */
            mDoMtx_ZXYrotM(now, shape_angle.x, shape_angle.y, shape_angle.z);
            mDoMtx_stack_transM(0.0f, -l_HIO().m05C.m4, 0.0f);
        } else {
            mDoMtx_YrotM(now, shape_angle.y);
            mDoMtx_stack_transM(m3298.x, m3298.y, m3298.z);
            mDoMtx_XrotM(now, shape_angle.x);
            mDoMtx_ZrotM(now, shape_angle.z);
        }
    } else if (status & daMdStts_SHIP_RIDE) { /* isShipRide() */
        actor_status = actor_status | fopAcStts_UNK4000_e; /* fopAcM_OnStatus(this, fopAcStts_UNK4000_e) */
        fopAc_ac_c* ship = dComIfGp_getShipActor(); /* dComIfGp_getShipActor() */
        /* ship->getBodyMtx(): the body morf's model base matrix */
        J3DModel* shipModel = md_morfModel(reinterpret_cast<Ship*>(ship)->mpMorf.get());
        PSMTXCopy(J3DModel_getBaseTRMtx(shipModel), now);
        mDoMtx_stack_transM(43.0f, 14.65f, 13.99f);
        mDoMtx_XYZrotM(now, 0, -0x45B0, 0);
        f32 z = now->m[2][3];
        f32 x = now->m[0][3];
        f32 y = now->m[1][3];
        current.pos.x = x;
        current.pos.y = y;
        current.pos.z = z;
        s16 sVar4 = ship->shape_angle.y + -0x4000;
        current.angle.y = sVar4;
        shape_angle.y = sVar4;
        setAttention(true);
        tevStr.mRoomNo = ship->tevStr.mRoomNo;
        tevStr.mEnvrIdxOverride = ship->tevStr.mEnvrIdxOverride;
    } else {
        PSMTXTrans(now, current.pos.x, current.pos.y + 95.0f, current.pos.z);
        mDoMtx_YrotM(now, shape_angle.y);
        mDoMtx_stack_transM(m3298.x, m3298.y, m3298.z);
        mDoMtx_XrotM(now, shape_angle.x);
        mDoMtx_ZrotM(now, shape_angle.z);
        mDoMtx_stack_transM(0.0f, -95.0f, 0.0f);
    }
    J3DModel_setBaseTRMtx(model, now);
    McaMorf2_calc(mpMorf.get());
    if (!isTypeShipRide()) {
        McaMorf_calc((mDoExt_McaMorf_c*)(void*)mpWingMorf.get());
    }
    McaMorf2_calc(mpArmMorf.get());
    if (checkStatus(daMdStts_UNK80)) {
        J3DModel* armModel = md_morfModel(mpArmMorf.get());
        PSMTXCopy(md_getAnmMtx(armModel, (s32)m_handL_jnt_num), now);
        mDoMtx_stack_transM(0.37f, 3.81f, -11.13f);
        mDoMtx_XYZrotM(now, 0x1B24, -0x2913, -0x1982);
    } else {
        J3DModel* bodyModel = md_morfModel(mpMorf.get());
        PSMTXCopy(md_getAnmMtx(bodyModel, (s32)m_backbone2_jnt_num), now);
        mDoMtx_stack_transM(2.2f, -11.681f, 1.3f);
        mDoMtx_XYZrotM(now, 0x3F11, 0x2894, 0x7F7E);
    }
    J3DModel_setBaseTRMtx(mpHarpModel.get(), now);
    J3DModel* lightModel = mpHarpLightModel.get();
    J3DModel_setBaseTRMtx(lightModel, J3DModel_getBaseTRMtx(mpHarpModel.get()));
}

/* 02285580 */
BOOL daNpc_Md_c::init() {
    u8 uVar1 = fopAcM_GetParam(this) & 0xFF;
    m3100 = (fopAcM_GetParam(this) >> 0x10) & 0xFF;
    m313E = 0;
    m30F4 = JA_SE_CM_MD_HARP_CN4; /* HD: JA_SE_CM_MD_HARP_CN4 */
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;  /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0x21;  /* distances[TALK] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0x21;  /* distances[SPEAK] */
    attention_info.distances[fopAc_Attn_TYPE_CARRY_e] = 0x27;  /* distances[CARRY] */
    mCurEventMode = 0;
    mCurEvent = 0;
    for (int i = 0; i < 6; i++) {
        m0508[i] = 0;
    }
    m30AC.x = 0.0f;
    m30AC.y = 0.0f;
    m30AC.z = 50.0f;
    m313C = 0;
    m30F0 = 0;
    gravity = l_HIO().m0F4;
    gabi::Local<ProcFunc_l> fn;
    if (mType == TYPE_M_DAIB) {
        mdHeap_loadAction(fn, action_harpWaitNpc);
    } else {
        mdHeap_loadAction(fn, action_waitNpc);
    }
    setNpcAction(fn, nullptr);
    m3094.copy(current.pos);
    m3088.copy(current.pos);
    for (s32 i = 0; i < 4; i++) {
        m3234[i].set(0.0f, 0.0f, 0.0f);
    }
    if ((uVar1 & 1) != 0) {
        m30F0 = daMdStts_UNK8;
    }
    m3135 = 0;
    mCurEvent = -1;
    m_flying = 0;
    m_mirror = 0;
    mRunRate = 0.0f;
    m30F8 = 120.0f;
    reinterpret_cast<Morf*>(mpMorf.get())->mAnmRate = 0.0f; /* mpMorf->setAnmRate(mRunRate) */
    reinterpret_cast<Morf*>(mpArmMorf.get())->mAnmRate = mRunRate;
    if (!md_isEventBit(UNK_1620)) {
        mNoResetFlg1 = mNoResetFlg1 | 0x40; /* onNpcNotChange() */
    } else if (shipRideCheck()) {
        mdHeap_setMorf(mpMorf.get(), 0.0f); /* mDoExt_McaMorf2::setMorf */
        mdHeap_setMorf(mpArmMorf.get(), 0.0f);
    }
    if (mType == TYPE_M_DAI || mType == TYPE_EDAICHI) {
        mStts.Init(0xFE, 0xFF, this);
    } else {
        mStts.Init(0xFF, 0xFF, this);
    }
    mCyl1.Set(&l_cyl_src); /* l_cyl_src */
    mCyl1.SetStts(&mStts);
    mCyl2.Set(&l_wind_cyl_src); /* l_wind_cyl_src */
    mCyl2.SetStts(&mStts);
    mCyl3.Set(&l_light_cyl_src); /* l_light_cyl_src */
    mCyl3.SetStts(&mStts);
    mdHeap_setCapsule(&mCps, &l_fan_light_cps_src); /* mCps.Set(l_fan_light_cps_src) */
    mCps.SetAtType(AT_TYPE_LIGHT); /* AT_TYPE_LIGHT */
    mCps.SetStts(&mStts);
    reinterpret_cast<Capsule*>(&mCps)->mGeometry.mRadius = 20.0f;
    void* pBti = md_getObjectRes(mModelArcName, "md_spot.bti" /* "md_spot.bti" */);
    mdHeap_initMirror(m0B70, pBti); /* dDlst_mirrorPacket::init */
    if (mType == TYPE_EDAICHI) {
        mNoResetFlg1 = mNoResetFlg1 | 2; /* onNpcCallCommand() */
    }
    for (s32 i = 0; i < 10; i++) {
        mEventIdxTable[i] = dComIfGp_evmng_getEventIdx(event_name_tbl.values[i].get() /* event_name_tbl */, 0xFF);
    }
    eventInfo.setXyCheckCB(daNpc_Md_XyCheckCB);
    eventInfo.setXyEventCB(daNpc_Md_XyEventCB);
    return TRUE;
}

/* 02286B58 */
static cPhs_State daNpc_Md_Create(fopAc_ac_c* i_this) {
    return static_cast<daNpc_Md_c*>(i_this)->create();
}

/* WWHD glue
ext 02606900 void* mdHeap_getResource(void* control, SafeString* arc, SafeString* name)
ext 028E8FB8 void mdHeap_multVecArray(const Mtx34* matrix, const cXyz* src, cXyz* dst, u32 count)
ext 0201B12C void mdHeap_normZP(const cXyz* src, cXyz* result)
ext 0201B084 void mdHeap_norm(const cXyz* src, cXyz* result)
ext 0201B080 void mdHeap_outprod(const cXyz* src, cXyz* result, const cXyz* other)
ext 0201B3C0 void mdHeap_normalizeZP(cXyz* src, cXyz* result)
ext 0257E1B8 void mdHeap_getAllWind(cXyz* pos, cXyz* dir, be<f32>* power)
ext 028E9838 void mdHeap_rotAxisRad(Mtx34* matrix, const cXyz* axis, f32 radians)
ext 028E9108 void mdHeap_concat(const Mtx34* first, const Mtx34* second, Mtx34* result)
ext 028E8F44 f32 mdHeap_dotProduct(const cXyz* first, const cXyz* second)
ext 02017374 bool mdHeap_upMatrix(const cXyz* first, const cXyz* second, Mtx34* matrix)
ext 020190B8 void mdHeap_constructTriangle(cM3dGTri* triangle, const cXyz* a, const cXyz* b, const cXyz* c)
ext 02018780 void mdHeap_constructLine(cM3dGLin* line, const cXyz* start, const cXyz* end)
ext 02012AE8 bool mdHeap_crossLineTriangle(const cM3dGLin* line, const cM3dGTri* triangle, cXyz* result, bool front, bool back)
ext 020191B4 void mdHeap_upTriangle(cM3dGTri* triangle, f32 distance)
ext 02010C50 f32 mdHeap_signedDistance(const cM3dGTri* triangle, const cXyz* pos)
ext 025F19F8 void mdHeap_xyzRotM(Mtx34* matrix, s16 x, s16 y, s16 z)
ext 025E66E4 void mdHeap_calcMorf2(mDoExt_McaMorf2* morf)
ext 025E789C s32 mdHeap_initBtp(void* animation, J3DModelData* data, void* resource, s32 play, s32 mode, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 025E7CE0 s32 mdHeap_initBtk(void* animation, J3DModelData* data, void* resource, s32 play, s32 mode, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 025E6220 mDoExt_McaMorf2* mdHeap_newMorf2(u32 object, J3DModelData* data, u32 callback1, u32 callback2, void* animation, u32 animation2, s32 mode, f32 rate, s32 start, s32 end, s32 sound, u32 info, u32 flags, u32 materials)
ext 025E4F64 mDoExt_McaMorf* mdHeap_newMorf(u32 object, J3DModelData* data, u32 callback1, u32 callback2, void* animation, s32 mode, f32 rate, s32 start, s32 end, s32 sound, u32 info, u32 flags, u32 materials)
ext 025E5D1C void mdHeap_setMorf(mDoExt_McaMorf2* morf, f32 value)
ext 025164C0 void mdHeap_setCapsule(dCcD_Cps* capsule, const dCcD_SrcCps* source)
ext 0252D6E8 void mdHeap_initMirror(void* mirror, void* texture)
ext 028F040C void mdHeap_copyString(char* dst, const char* src)
ext 024450B4 void mdHeap_constructNpc(daNpc_Md_c* actor)
ext 025E7820 void mdHeap_constructBtp(void* animation)
ext 028EFFD0 void mdHeap_constructArray(void* array, u32 count, u32 stride, u32 constructor)
ext 02008FEC void mdHeap_constructLinCheck(void* check)
ext 02515FB8 void mdHeap_constructObjectInfo(void* object)
ext 02018150 void mdHeap_constructCapsule(void* capsule)
ext 0259DAA0 void mdHeap_constructJointControl(dNpc_JntCtrl_c* control)
ext 0252CE74 void mdHeap_constructMirror(void* mirror)
ext 025A9084 void mdHeap_constructRipple(dPa_rippleEcallBack_md* callback)
ext 0259D734 BOOL mdHeap_checkLetterPassed()
ext 025D63E8 BOOL mdHeap_entrySolidHeap(fopAc_ac_c* actor, u32 callback, u32 size)
ext 025B9834 void mdHeap_setRestartOption(void* restart, s32 flag, const cXyz* pos, s16 rotation, s8 room)
ext 025B8880 void mdHeap_setPriest(void* priest, s32 flag, const cXyz* pos, s16 rotation, s8 room)
ext 02445784 void mdHeap_checkRestart(daNpc_Md_c* actor, s32 flag)
data saveInfo 101F84DC
data medli 101CEF74
data m_seaTalk 101D5F40
data m_flying 101D5F3E
data m_mirror 101D5F3F
data l_offsetAttPosData 10467A6C
data l_offsetEyePosData 10467A78
data offsetAttPosGuard 10467C5C
data offsetEyePosGuard 10467C60
data waistVecDat 101C02AC
data HairModeMaskData 101C0270
data hairName 101C02DC
data event_name_tbl 101C0284
data l_arc_name 101C0278
data l_arc_name_ship 101C027C
data l_heep_size 101C07FC
data btpAnmTbl 101C05EC
data l_cyl_src 101C031C
data l_wind_cyl_src 101C03F0
data l_light_cyl_src 101C0360
data l_fan_light_cps_src 101C03A4
data action_shipNpc 1001D4B0
data action_harpWaitNpc 1001D580
data action_waitNpc 1001D490
str 1001D91C "ymdhp00.btk"
str 1001D9DC "md_shipwait.bck"
str 1001DB18 "mdarm_shipwait.bck"
str 1001DA70 "md_wait01.bck"
str 1001DB2C "mdarm_wait01.bck"
str 1001D994 "md.bdl"
str 1001DA98 "mdarm.bdl"
str 1001DBE4 "mdwing.bdl"
str 1001DAA4 "mdwing_wait01.bck"
str 1001DA58 "md_harp.bdl"
str 1001DA64 "ymdhp00.bdl"
str 1001DC14 "md_spot.bti"
str 1001DA80 "backbone1"
str 1001DA8C "backbone2"
str 1001D99C "armR"
str 1001D9A4 "armL"
str 1001D9AC "neck"
str 1001D9B4 "head"
str 1001D97C "armRloc"
str 1001D984 "armLloc"
str 1001D98C "handL"
str 1001DBF0 "wingRloc"
str 1001DBFC "wingLloc"
str 1001D9BC "wingR2"
str 1001D9C4 "wingL2"
str 1001D9CC "wingR3"
str 1001D9D4 "wingL3"
str 1001DC80 "M_DaiB"
str 1001DC74 "sea"
str 1001DC54 "Atorizk"
str 1001DC5C "Adanmae"
str 1001DC64 "M_Dra09"
str 1001DC6C "Edaichi"
str 1001DC78 "M_Dai"
*/
