/**
 * d_a_npc_bm1.cpp (WWHD)
 * NPC - Rito (generic Ritos on Dragon Roost Island): setup, animation, particles, events
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_bm1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Part A: 021FA778 .. 021FFFFC (the rest is in d_a_npc_bm1_b.cpp).
 */
#include "d/actor/d_a_npc_bm1.h"

#define SAFESTRING_VTBL 0x10016F80 /* this TU's sead::SafeString vtable */
#define BM1_VTBL 0x10017A18        /* daNpc_Bm1_c vtable (HD: merged with fopNpc_npc_c's) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02606900 HD: dRes_control_c::getRes(const sead::SafeString& arc, const sead::SafeString& name)
 * (dComIfG_getObjectRes(arc, name)) */
static inline void* dComIfG_getObjectRes_s(const char* arc, const char* name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> n;
    a->mStringTop = gabi::ea(arc);
    a->__vtbl = SAFESTRING_VTBL;
    n->mStringTop = gabi::ea(name);
    n->__vtbl = SAFESTRING_VTBL;
    return gabi::call<void*>(0x02606900, dComIfG_resControl(), a.get(), n.get());
}
/* J3DModelData (HD): 027F68FC returns the joint name table header (self-relative offset at
 * +0x10 to the JUTNameTab), 027F3F94 (the matcher calls it __nw) returns the joint tree
 * header (joint count u16 at +8), 027F3F8C returns the model's name header (self-relative
 * offset at +4 to the name) */
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
static inline u32 J3DModelData_getName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F3F8C, d);
    u32 off = gabi::load<u32>(h + 4);
    return off != 0 ? h + 4 + off : 0;
}
/* J3DModel (HD): model data at +0xAC */
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
/* 025E789C mDoExt_btpAnm::init(modelData, anm, anmPlay, attr, rate, start, end, modify, entry) */
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                                     s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
/* 028F040C strcpy */
static inline char* strcpy_g(char* d, const char* s) { return gabi::call<char*>(0x028F040C, d, s); }
/* inline strcmp / strcat (byte loops in the WWHD code) */
static inline s32 strcmp_i(u32 a, u32 b) {
    for (;; a++, b++) {
        u8 c1 = gabi::load<u8>(a), c2 = gabi::load<u8>(b);
        if (c1 != c2 || c1 == 0)
            return (s32)c1 - (s32)c2;
    }
}
static inline void strcat_i(u32 d, u32 s) {
    while (gabi::load<u8>(d) != 0) d++;
    for (;; d++, s++) {
        u8 c = gabi::load<u8>(s);
        gabi::store<u8>(d, c);
        if (c == 0)
            break;
    }
}
/* delete of an mDoExt_McaMorf (HD: virtual deleting destructor, vtable at +0, slot +0xC) */
static inline void McaMorf_delete(mDoExt_McaMorf* morf) {
    if (morf != nullptr) {
        u32 vt = gabi::load<u32>(gabi::ea(morf));
        gabi::call_ptr(gabi::load<u32>(vt + 0xC), morf, 3);
    }
}
/* joint node callback: getJointNodePointer(i)->setCallBack(cb) (HD: node table at modelData+8,
 * count +4, 0x1C bytes per node, callback +8) */
static inline void setJointCallBack(J3DModelData* md, u32 i, u32 cb) {
    u32 n = gabi::load<u32>(gabi::ea(md) + 4);
    u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
    if (i < n)
        joint += i * 0x1C;
    gabi::store<u32>(joint + 8, cb);
}
/* save info: event flags (dSv_event_c) at *(0x101F84DC) + 0x644, re-read at every use */
static inline u32 dComIfGs_save() { return gabi::load<u32>(0x101F84DC); }
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(dComIfGs_save() + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
/* 025B7D90 dSv_player_collect_c::isSymbol (save + 0xD4) */
static inline BOOL dComIfGs_isSymbol(u8 i) { return gabi::call<BOOL>(0x025B7D90, dComIfGs_save() + 0xD4, i); }
/* 025B7840 dSv_player_get_bag_item_c::isReserve (save + 0xB0) */
static inline BOOL dComIfGs_isGetItemReserve(u8 i) { return gabi::call<BOOL>(0x025B7840, dComIfGs_save() + 0xB0, i); }
/* J3DAnmTexPattern::getFrameMax: virtual (vtable pointer at +4, slot +0x14) */
static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
/* 02055B64 cLib_calcTimer<s16> (out of line) */
static inline s16 cLib_calcTimer_g(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* JPABaseEmitter (HD): flags +0x254, max frame +0x5C, direction +0x28, global R/T +0x1F0/+0x22C */
static inline void JPABaseEmitter_becomeImmortalEmitter(JPABaseEmitter* e) {
    u32 a = gabi::ea(e) + 0x254;
    gabi::store<u32>(a, gabi::load<u32>(a) | 0x40);
}
static inline void JPABaseEmitter_quitImmortalEmitter(JPABaseEmitter* e) {
    u32 a = gabi::ea(e) + 0x254;
    gabi::store<u32>(a, gabi::load<u32>(a) & ~0x40u);
}
static inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* e) {
    gabi::store<s32>(gabi::ea(e) + 0x5C, -1);
    u32 a = gabi::ea(e) + 0x254;
    gabi::store<u32>(a, gabi::load<u32>(a) | 1);
}
/* setGlobalRTMatrix(m): 028249B0 JPASetRMtxTVecfromMtx(m, &mGlobalRot, &mGlobalTrs) */
static inline void JPABaseEmitter_setGlobalRTMatrix(JPABaseEmitter* e, Mtx34* m) {
    gabi::call(0x028249B0, m, gabi::ea(e) + 0x1F0, gabi::ea(e) + 0x22C);
}
/* l_HIO (0x1046623C): children[10] at +0xC, 0x54 bytes each (vtable, then hio_prm) */
static inline u32 l_HIO_prm(s32 type) { return 0x1046624C + (type - 1) * 0x54; } /* &l_HIO.children[type - 1].hio_prm */
/* dBgS (play + 0x12A0) */
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
/* 025F19F8 mDoMtx_XYZrotM(m, x, y, z) */
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }
/* 0259E6D0 dNpc_PathRun_c::setInf(u8 pathIdx, s8 roomNo, bool fwd) */
static inline void dNpc_PathRun_setInf(dNpc_PathRun_bm1* p, u32 idx, s8 room, u8 fwd) { gabi::call(0x0259E6D0, p, idx, room, fwd); }
/* dNpc_PathRun_c (HD out of line) */
static inline dPath* dNpc_PathRun_nextPath(dNpc_PathRun_bm1* p, s8 room) { return gabi::call<dPath*>(0x0259E744, p, room); }
static inline void dNpc_PathRun_setInfDrct(dNpc_PathRun_bm1* p, dPath* path) { gabi::call(0x0259E730, p, path); }
/* 02527028 dDemo_setDemoData(actor, flags, morf, arc, n, ids, p6, p7) */
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, morf, arc, n, ids, p6, p7);
}
/* 0259D344 dNpc_setAnmFNDirect(morf, loopMode, morf, speed, const char* bckName, int soundIdx, const char* arc) */
static inline BOOL dNpc_setAnmFNDirect(mDoExt_McaMorf* m, s32 loop, f32 morf, f32 speed, u32 name, s32 snd, const char* arc) {
    return gabi::call<BOOL>(0x0259D344, m, loop, morf, speed, name, snd, arc);
}
static inline u32 dNpc_PathRun_maxPoint(dNpc_PathRun_bm1* p) { return gabi::call<u32>(0x0259EDB8, p); }
static inline void dNpc_PathRun_getPoint(dNpc_PathRun_bm1* p, cXyz* out, u8 idx) { gabi::call(0x0259E778, p, out, idx); }
static inline void dNpc_PathRun_nextIdxAuto(dNpc_PathRun_bm1* p) { gabi::call(0x0259ED58, p); }
static inline void dComIfGp_evmng_setGoal(cXyz* pos) { gabi::call(0x02543714, dComIfGp_getPEvtManager(), pos); }
static inline void J3DModel_setUserArea(J3DModel* m, void* p) { gabi::store<u32>(gabi::ea(m) + 0xB8, gabi::ea(p)); }

enum { fpcNm_NPC_ZL1_e = 0x1AA, fpcNm_NPC_GP1_e = 0x165, fpcNm_NPC_BM2_e = 0x147 };

/* ---- file statics ---- */
static be<s32>& l_check_wrk() { return *gabi::at<be<s32>>(0x104661D0); }
static gptr<fopAc_ac_c>* l_check_inf() { return gabi::at<gptr<fopAc_ac_c>>(0x10466590); } /* [20] */

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
static Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
/* mDoMtx_stack_c::multVecZero(o): the translation column */
static void mDoMtx_stack_multVecZero(cXyz* o) {
    Mtx34* m = mDoMtx_stack_c::get();
    o->x = m->m[0][3];
    o->y = m->m[1][3];
    o->z = m->m[2][3];
}

/* searchActor_*: collect actors into l_check_inf (HD: the actor pointer is checked for NULL) */
static inline bool searchActor_chk(void* i_param_1, s16 name) {
    return l_check_wrk() < 0x14 && fopAc_IsActor(i_param_1) && i_param_1 != nullptr && fpcM_GetName(i_param_1) == name;
}
static inline void searchActor_add(void* i_param_1) {
    s32 n = l_check_wrk();
    l_check_wrk() = n + 1;
    l_check_inf()[n] = (fopAc_ac_c*)i_param_1;
}

/* 021FA778 */
static void* searchActor_Zl(void* i_param_1, void*) {
    WWHD_FUNC(0x021FA778, void*, i_param_1, (void*)nullptr);
    if (searchActor_chk(i_param_1, fpcNm_NPC_ZL1_e)) {
        searchActor_add(i_param_1);
    }
    return nullptr;
}
VERIFY(0x021FA778, searchActor_Zl);

/* 021FA7F8 */
static void* searchActor_Gp(void* i_param_1, void*) {
    WWHD_FUNC(0x021FA7F8, void*, i_param_1, (void*)nullptr);
    if (searchActor_chk(i_param_1, fpcNm_NPC_GP1_e)) {
        searchActor_add(i_param_1);
    }
    return nullptr;
}
VERIFY(0x021FA7F8, searchActor_Gp);

/* 021FA878 */
static void* searchActor_Bm_Skt(void* i_param_1, void*) {
    WWHD_FUNC(0x021FA878, void*, i_param_1, (void*)nullptr);
    if (searchActor_chk(i_param_1, fpcNm_NPC_BM2_e)) {
        if (((daNpc_Bm1_c*)i_param_1)->mSpecificType == daNpc_Bm1_c::SPECIFIC_TYPE_Skett_e) { /* IamSukketo() */
            searchActor_add(i_param_1);
        }
    }
    return nullptr;
}
VERIFY(0x021FA878, searchActor_Bm_Skt);

/* 021FA908 */
static void* searchActor_Bm_Kkt(void* i_param_1, void*) {
    WWHD_FUNC(0x021FA908, void*, i_param_1, (void*)nullptr);
    if (searchActor_chk(i_param_1, fpcNm_NPC_BM2_e)) {
        if (((daNpc_Bm1_c*)i_param_1)->mSpecificType == daNpc_Bm1_c::SPECIFIC_TYPE_Akoot_e) { /* IamKakkuto() */
            searchActor_add(i_param_1);
        }
    }
    return nullptr;
}
VERIFY(0x021FA908, searchActor_Bm_Kkt);

/* 021FA998 */
void daNpc_Bm1_c::nodeWngControl(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x021FA998, void, this, i_node, i_model);
    u32 uVar1 = gabi::load<u16>(gabi::ea(J3DNode_toJoint(i_node)) + 4); /* getJntNo() */
    PSMTXCopy(getAnmMtx(i_model, uVar1), mDoMtx_stack_c::get());
    if (uVar1 == (u32)(s32)m_wngL1_jnt_num) {
        PSMTXCopy(&mLeftArmMtx, j3dSys_mCurrentMtx());
        mtx_copy(getAnmMtx(i_model, uVar1), &mLeftArmMtx); /* setAnmMtx */
    }
    if (uVar1 == (u32)(s32)m_wngL3_jnt_num) {
        mDoMtx_stack_multVecZero(&mWingLPos);
    }
    if (uVar1 == (u32)(s32)m_wngR1_jnt_num) {
        PSMTXCopy(&mRightArmMtx, j3dSys_mCurrentMtx());
        mtx_copy(getAnmMtx(i_model, uVar1), &mRightArmMtx);
    }
    if (uVar1 == (u32)(s32)m_wngR3_jnt_num) {
        mDoMtx_stack_multVecZero(&mWingRPos);
    }
}
VERIFY(0x021FA998, &daNpc_Bm1_c::nodeWngControl);

/* 021FAB78 */
static BOOL nodeCallBack_Wng(J3DNode* i_param_1, int i_param_2) {
    WWHD_FUNC(0x021FAB78, BOOL, i_param_1, i_param_2);
    if (i_param_2 == 0) {
        u32 model = gabi::load<u32>(0x104B462C);
        daNpc_Bm1_c* user = gabi::at<daNpc_Bm1_c>(gabi::load<u32>(model + 0xB8));
        if (user != nullptr) {
            user->nodeWngControl(i_param_1, gabi::at<J3DModel>(model));
        }
    }
    return TRUE;
}
VERIFY(0x021FAB78, nodeCallBack_Wng);

/* 021FABC0 */
void daNpc_Bm1_c::nodeArmControl(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x021FABC0, void, this, i_node, i_model);
    u32 uVar1 = gabi::load<u16>(gabi::ea(J3DNode_toJoint(i_node)) + 4); /* getJntNo() */
    PSMTXCopy(getAnmMtx(i_model, uVar1), mDoMtx_stack_c::get());
    if (uVar1 == (u32)(s32)m_armL1_jnt_num) {
        PSMTXCopy(&mLeftArmMtx, j3dSys_mCurrentMtx());
        mtx_copy(getAnmMtx(i_model, uVar1), &mLeftArmMtx);
    }
    if (uVar1 == (u32)(s32)m_armL2_jnt_num) {
        mDoMtx_stack_multVecZero(&mArmLPos);
    }
    if (uVar1 == (u32)(s32)m_armR1_jnt_num) {
        PSMTXCopy(&mRightArmMtx, j3dSys_mCurrentMtx());
        mtx_copy(getAnmMtx(i_model, uVar1), &mRightArmMtx);
    }
    if (uVar1 == (u32)(s32)m_armR2_jnt_num) {
        mDoMtx_stack_multVecZero(&mArmRPos);
    }
}
VERIFY(0x021FABC0, &daNpc_Bm1_c::nodeArmControl);

/* 021FADA0 */
static BOOL nodeCallBack_Arm(J3DNode* i_param_1, int i_param_2) {
    WWHD_FUNC(0x021FADA0, BOOL, i_param_1, i_param_2);
    if (i_param_2 == 0) {
        u32 model = gabi::load<u32>(0x104B462C);
        daNpc_Bm1_c* user = gabi::at<daNpc_Bm1_c>(gabi::load<u32>(model + 0xB8));
        if (user != nullptr) {
            user->nodeArmControl(i_param_1, gabi::at<J3DModel>(model));
        }
    }
    return TRUE;
}
VERIFY(0x021FADA0, nodeCallBack_Arm);

/* 021FADE8 */
void daNpc_Bm1_c::nodeBm1Control(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x021FADE8, void, this, i_node, i_model);
    /* static cXyz a_eye_pos_off(26.0f, 26.0f, 0.0f): guard 0x104665E0, object 0x104661F0 */
    cXyz* a_eye_pos_off = gabi::at<cXyz>(0x104661F0);
    if (gabi::load<u32>(0x104665E0) == 0) {
        a_eye_pos_off->x = 26.0f;
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->y = 26.0f;
        gabi::store<u32>(0x104665E0, 1);
    }
    u32 uVar1 = gabi::load<u16>(gabi::ea(J3DNode_toJoint(i_node)) + 4); /* getJntNo() */
    PSMTXCopy(getAnmMtx(i_model, uVar1), mDoMtx_stack_c::get());
    if (uVar1 == (u32)(s32)m_nec_jnt_num) {
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[0][0]);
    }
    if (uVar1 == (u32)(s32)m_bbone_jnt_num) {
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[1][0]);
    }
    if (uVar1 == (u32)(s32)m_arm_L_jnt_num) {
        PSMTXCopy(mDoMtx_stack_c::get(), &mLeftArmMtx);
    }
    if (uVar1 == (u32)(s32)m_arm_R_jnt_num) {
        PSMTXCopy(mDoMtx_stack_c::get(), &mRightArmMtx);
    }
    if (uVar1 == (u32)(s32)m_hed_jnt_num) {
        PSMTXMultVec(mDoMtx_stack_c::get(), a_eye_pos_off, &mEyePos);
    }
    PSMTXCopy(mDoMtx_stack_c::get(), j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, uVar1), mDoMtx_stack_c::get());
}
VERIFY(0x021FADE8, &daNpc_Bm1_c::nodeBm1Control);

/* 021FAFC8 */
static BOOL nodeCallBack_Bm1(J3DNode* i_param_1, int i_param_2) {
    WWHD_FUNC(0x021FAFC8, BOOL, i_param_1, i_param_2);
    if (i_param_2 == 0) {
        u32 model = gabi::load<u32>(0x104B462C);
        daNpc_Bm1_c* user = gabi::at<daNpc_Bm1_c>(gabi::load<u32>(model + 0xB8));
        if (user != nullptr) {
            user->nodeBm1Control(i_param_1, gabi::at<J3DModel>(model));
        }
    }
    return TRUE;
}
VERIFY(0x021FAFC8, nodeCallBack_Bm1);

/* 021FB010 */
J3DModelData* daNpc_Bm1_c::create_Anm() {
    WWHD_FUNC(0x021FB010, J3DModelData*, this);
    J3DModelData* a_mdl_dat;
    /* HD: types 2..6 (Akoot .. Hoskit) use "bm02.bmt" */
    if ((u32)(s32)mType >= 2 && (u32)(s32)mType <= 6) {
        a_mdl_dat = (J3DModelData*)dComIfG_getObjectRes_s(mArcName, STR(0x100170BC) /* "bm02.bmt" */);
    } else {
        a_mdl_dat = (J3DModelData*)dComIfG_getObjectRes_s(mArcName, STR(0x10017084) /* "bm.bdl" */);
    }
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(0x15DC, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x100170AC), 0x15DC, STR(0x100170C8));
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes_s(mArcName, STR(0x100170DC) /* "bm_dwait.bck" */);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, 1,
                                    nullptr, 0x80000, 0x11020203);
    if (mpMorf.get() == nullptr) {
        return nullptr;
    }
    if (mpMorf->getModel() == nullptr) {
        McaMorf_delete(mpMorf);
        mpMorf = nullptr;
        return nullptr;
    }
    m_hed_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001708C) /* "head" */);
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100170AC), 0x15F6, STR(0x100170EC));
    m_nec_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10017094) /* "neck" */);
    if (m_nec_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100170AC), 0x15F9, STR(0x10017100));
    m_bbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10017114) /* "backbone" */);
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100170AC), 0x15FC, STR(0x10017120));
    m_arm_L_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001709C) /* "armL" */);
    if (m_arm_L_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100170AC), 0x15FF, STR(0x10017138));
    m_arm_R_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100170A4) /* "armR" */);
    if (m_arm_R_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100170AC), 0x1602, STR(0x10017150));
    return a_mdl_dat;
}
VERIFY(0x021FB010, &daNpc_Bm1_c::create_Anm);

/* 021FB354 */
J3DModelData* daNpc_Bm1_c::create_hed_Anm() {
    WWHD_FUNC(0x021FB354, J3DModelData*, this);
    /* a_headBDLName_TBL (.data 0x101BC5B0) */
    const char* name = gabi::at<const char>(gabi::load<u32>(0x101BC5B0 + mSpecificType * 4));
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectRes_s(mArcName, name);
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail(STR(0x10017168), 0x1635, STR(0x10017178));
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes_s(mArcName, STR(0x1001718C) /* "bmhead01_dwait.bck" */);
    mpHeadMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2, 1.0f, 0, -1, 1, nullptr, 0x80000,
                                        0x11020022);
    if (mpHeadMorf.get() == nullptr) {
        return nullptr;
    }
    if (mpHeadMorf->getModel() == nullptr) {
        McaMorf_delete(mpHeadMorf);
        mpHeadMorf = nullptr;
        return nullptr;
    }
    return a_mdl_dat;
}
VERIFY(0x021FB354, &daNpc_Bm1_c::create_hed_Anm);

/* 021FB4C4 */
/* HD: reads a_BTPName_TBL[0] and indexes a_BTPName_TBL_2 by mSpecificType (the argument is unused) */
u32 daNpc_Bm1_c::btpNum_toResID(int i_param_1) {
    WWHD_FUNC(0x021FB4C4, u32, this, i_param_1);
    u32 l_BTPName = 0x1046621C;
    u32 tbl0 = gabi::load<u32>(0x101BC5F0); /* a_BTPName_TBL[0] */
    if (strcmp_i(tbl0, 0x10017228 /* "bmhead01" */) == 0) {
        strcpy_g(gabi::at<char>(l_BTPName), gabi::at<const char>(gabi::load<u32>(0x101BC5F4 + mSpecificType * 4)));
        strcat_i(l_BTPName, 0x10017220 /* ".btp" */);
    } else {
        strcpy_g(gabi::at<char>(l_BTPName), gabi::at<const char>(tbl0));
        strcat_i(l_BTPName, 0x10017220 /* ".btp" */);
    }
    return l_BTPName;
}
VERIFY(0x021FB4C4, &daNpc_Bm1_c::btpNum_toResID);

/* 021FB5B8 */
/* HD: unless the head model is "bmhead08" or "bmhead10", the btp is "<head model name>.btp" */
bool daNpc_Bm1_c::setBtp(u32 i_param_1, int i_btp_num) {
    WWHD_FUNC(0x021FB5B8, bool, this, i_param_1, i_btp_num);
    J3DModelData* model_data = J3DModel_getModelData_l(mpHeadMorf->getModel());
    u32 name = J3DModelData_getName(model_data);
    if (strcmp_i(name, 0x100172A0 /* "bmhead08" */) != 0 && strcmp_i(name, 0x100172AC /* "bmhead10" */) != 0) {
        u32 l_BTPName = 0x1046621C;
        strcpy_g(gabi::at<char>(l_BTPName), gabi::at<const char>(name));
        strcat_i(l_BTPName, 0x10017288 /* ".btp" */);
        m_hed_tex_pttrn = (J3DAnmTexPattern*)dComIfG_getObjectRes_s(mArcName, gabi::at<const char>(l_BTPName));
    } else {
        u32 res_id = btpNum_toResID(i_btp_num);
        m_hed_tex_pttrn = (J3DAnmTexPattern*)dComIfG_getObjectRes_s(mArcName, gabi::at<const char>(res_id));
    }
    if (m_hed_tex_pttrn.get() == nullptr) /* JUT_ASSERT(0x550, m_hed_tex_pttrn != NULL) */
        JUT_ASSERT_fail(STR(0x10017290), 0x550, STR(0x100172B8));
    int iVar1 = mDoExt_btpAnm_init(mHeadBtpAnm, model_data, m_hed_tex_pttrn, 1, 2, 1.0f, 0, -1, i_param_1, 0);
    bool o_retval = iVar1 == 1;
    if (o_retval) {
        mBlinkTimer = 0;
        mBlinkFrame = 0;
    }
    return o_retval;
}
VERIFY(0x021FB5B8, &daNpc_Bm1_c::setBtp);

/* 021FB78C */
/* tail call: setBtp's result register is passed through (typed u32) */
u32 daNpc_Bm1_c::iniTexPttrnAnm(u32 i_param_1) {
    WWHD_FUNC(0x021FB78C, u32, this, i_param_1);
    return gabi::call<u32>(0x021FB5B8, this, i_param_1, (s32)mBtpNum); /* setBtp(i_param_1, mBtpNum) */
}
VERIFY(0x021FB78C, &daNpc_Bm1_c::iniTexPttrnAnm);

/* 021FB798 */
J3DModelData* daNpc_Bm1_c::create_wng_Anm() {
    WWHD_FUNC(0x021FB798, J3DModelData*, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectRes_s(mArcName, STR(0x100172F0) /* "bmwing.bdl" */);
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail(STR(0x100172E0), 0x1659, STR(0x100172FC));
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes_s(mArcName, STR(0x10017310) /* "bmwing_dwait.bck" */);
    mpWingMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2, 1.0f, 0, -1, 1, nullptr, 0x80000,
                                        0x11020203);
    if (mpWingMorf.get() == nullptr) {
        return nullptr;
    }
    if (mpWingMorf->getModel() == nullptr) {
        McaMorf_delete(mpWingMorf);
        mpWingMorf = nullptr;
        return nullptr;
    }
    m_wngL1_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10017324) /* "wingLloc" */);
    if (m_wngL1_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100172E0), 0x1673, STR(0x10017330));
    m_wngL3_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100172D0) /* "wingL3" */);
    if (m_wngL3_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100172E0), 0x1676, STR(0x10017348));
    m_wngR1_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10017360) /* "wingRloc" */);
    if (m_wngR1_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100172E0), 0x1679, STR(0x1001736C));
    m_wngR3_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100172D8) /* "wingR3" */);
    if (m_wngR3_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100172E0), 0x167C, STR(0x10017384));
    return a_mdl_dat;
}
VERIFY(0x021FB798, &daNpc_Bm1_c::create_wng_Anm);

/* 021FBA3C */
J3DModelData* daNpc_Bm1_c::create_arm_Anm() {
    WWHD_FUNC(0x021FBA3C, J3DModelData*, this);
    J3DModelData* a_mdl_dat;
    /* HD: types 2..6 use "bmarm02.bmt" */
    if ((u32)(s32)mType >= 2 && (u32)(s32)mType <= 6) {
        a_mdl_dat = (J3DModelData*)dComIfG_getObjectRes_s(mArcName, STR(0x100173D4) /* "bmarm02.bmt" */);
    } else {
        a_mdl_dat = (J3DModelData*)dComIfG_getObjectRes_s(mArcName, STR(0x100173E0) /* "bmarm.bdl" */);
    }
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail(STR(0x100173C4), 0x16AD, STR(0x100173EC));
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes_s(mArcName, STR(0x10017400) /* "bmarm_wait01.bck" */);
    mpArmMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2, 1.0f, 0, -1, 1, nullptr, 0x80000,
                                       0x11020203);
    if (mpArmMorf.get() == nullptr) {
        return nullptr;
    }
    if (mpArmMorf->getModel() == nullptr) {
        McaMorf_delete(mpArmMorf);
        mpArmMorf = nullptr;
        return nullptr;
    }
    m_armL1_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001739C) /* "armLloc" */);
    if (m_armL1_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100173C4), 0x16C7, STR(0x10017414));
    m_armR1_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100173A4) /* "armRloc" */);
    if (m_armR1_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100173C4), 0x16CA, STR(0x1001742C));
    m_armL2_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100173AC) /* "armL2" */);
    if (m_armL2_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100173C4), 0x16CD, STR(0x10017444));
    m_armR2_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100173B4) /* "armR2" */);
    if (m_armR2_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100173C4), 0x16D0, STR(0x1001745C));
    m_hnd_R_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x100173BC) /* "handR" */);
    if (m_hnd_R_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100173C4), 0x16D3, STR(0x10017474));
    return a_mdl_dat;
}
VERIFY(0x021FBA3C, &daNpc_Bm1_c::create_arm_Anm);

/* 021FBD80 */
bool daNpc_Bm1_c::create_itm_Mdl() {
    WWHD_FUNC(0x021FBD80, bool, this);
    mpBagModel = nullptr;
    mpStickModel = nullptr;
    mpKnifeModel = nullptr;
    mpBinderModel = nullptr;
    if (mType == TYPE_Pashli_e || mType == TYPE_Namali_e) {
        J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectRes_s(mArcName, STR(0x1001749C) /* "bm_binder.bdl" */);
        if (a_mdl_dat == nullptr)
            JUT_ASSERT_fail(STR(0x1001748C), 0x16FD, STR(0x100174AC));
        mpBinderModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000002);
        if (mpBinderModel.get() == nullptr) {
            return false;
        }
    }
    if (mType == TYPE_Quill_e || mType == TYPE_Ilari_e || mType == TYPE_Pashli_e) {
        J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectRes_s(mArcName, STR(0x100174C0) /* "bm_bag.bdl" */);
        if (a_mdl_dat == nullptr)
            JUT_ASSERT_fail(STR(0x1001748C), 0x170C, STR(0x100174AC));
        mpBagModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000002);
        if (mpBagModel.get() == nullptr) {
            return false;
        }
    }
    if (mType == TYPE_Akoot_e || mType == TYPE_Skett_e) {
        J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectRes_s(mArcName, STR(0x100174CC) /* "bm_knife.bdl" */);
        if (a_mdl_dat == nullptr)
            JUT_ASSERT_fail(STR(0x1001748C), 0x1719, STR(0x100174AC));
        mpKnifeModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000002);
        if (mpKnifeModel.get() == nullptr) {
            return false;
        }
    }
    if (mType == TYPE_Basht_e || mType == TYPE_Bisht_e || mType == TYPE_Hoskit_e) {
        J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectRes_s(mArcName, STR(0x100174DC) /* "bm_stick.bdl" */);
        if (a_mdl_dat == nullptr)
            JUT_ASSERT_fail(STR(0x1001748C), 0x1727, STR(0x100174AC));
        mpStickModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000002);
        if (mpStickModel.get() == nullptr) {
            return false;
        }
    }
    return true;
}
VERIFY(0x021FBD80, &daNpc_Bm1_c::create_itm_Mdl);

/* 021FBFF0 */
BOOL daNpc_Bm1_c::CreateHeap() {
    WWHD_FUNC(0x021FBFF0, BOOL, this);
    J3DModelData* modeldat;
    J3DModelData* anm_model = create_Anm();
    if (!anm_model) {
        return FALSE;
    }
    J3DModelData* head_modeldat = create_hed_Anm();
    if (!head_modeldat) {
        mpMorf = nullptr;
        return FALSE;
    }
    mBtpNum = 0;
    if (!iniTexPttrnAnm(false) || (modeldat = create_wng_Anm()) == nullptr) {
        mpHeadMorf = nullptr;
        mpMorf = nullptr;
        return FALSE;
    }
    J3DModelData* arm_anmdata = create_arm_Anm();
    if (arm_anmdata != nullptr && create_itm_Mdl()) {
        for (u16 i = 0; i < J3DModelData_getJointNum(modeldat); i++) {
            if ((i == (u32)(s32)m_wngL1_jnt_num) || (i == (u32)(s32)m_wngL3_jnt_num) || (i == (u32)(s32)m_wngR1_jnt_num) ||
                (i == (u32)(s32)m_wngR3_jnt_num)) {
                setJointCallBack(J3DModel_getModelData_l(mpWingMorf->getModel()), i, 0x021FAB78 /* nodeCallBack_Wng */);
            }
        }
        J3DModel_setUserArea(mpWingMorf->getModel(), this);
        for (u16 i = 0; i < J3DModelData_getJointNum(arm_anmdata); i++) {
            if ((i == (u32)(s32)m_armL1_jnt_num) || (i == (u32)(s32)m_armL2_jnt_num) || (i == (u32)(s32)m_armR1_jnt_num) ||
                (i == (u32)(s32)m_armR2_jnt_num)) {
                setJointCallBack(J3DModel_getModelData_l(mpArmMorf->getModel()), i, 0x021FADA0 /* nodeCallBack_Arm */);
            }
        }
        J3DModel_setUserArea(mpArmMorf->getModel(), this);
        for (u16 i = 0; i < J3DModelData_getJointNum(anm_model); i++) {
            if ((i == (u32)(s32)m_hed_jnt_num) || (i == (u32)(s32)m_nec_jnt_num) || (i == (u32)(s32)m_bbone_jnt_num) ||
                (i == (u32)(s32)m_arm_L_jnt_num) || i == (u32)(s32)m_arm_R_jnt_num) {
                setJointCallBack(J3DModel_getModelData_l(mpMorf->getModel()), i, 0x021FAFC8 /* nodeCallBack_Bm1 */);
            }
        }
        J3DModel_setUserArea(mpMorf->getModel(), this);
        mAcchCir.SetWall(30.0f, 50.0f);
        mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
        return TRUE;
    } else {
        mpMorf = nullptr;
        mpWingMorf = nullptr;
        mpHeadMorf = nullptr;
        return FALSE;
    }
}
VERIFY(0x021FBFF0, &daNpc_Bm1_c::CreateHeap);

/* 021FC368 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021FC368, BOOL, i_this);
    return static_cast<daNpc_Bm1_c*>(i_this)->CreateHeap();
}
VERIFY(0x021FC368, CheckCreateHeap);

/* ---- pointers to member functions in .data (copied to the stack before set_action) ---- */
enum : u32 {
    PMF_demo_action1 = 0x10016F18,
    PMF_wait_action1 = 0x10016F20,
    PMF_wait_action3 = 0x10016F28,
    PMF_wait_action9 = 0x10016F30,
    PMF_wait_action6 = 0x10016F38,
    PMF_wait_action8 = 0x10016F40,
    PMF_wait_action2 = 0x10016F48,
    PMF_wait_action4 = 0x10016F50,
    PMF_wait_action5 = 0x10016F58,
    PMF_wait_actionA = 0x10016F60,
    PMF_wait_action7 = 0x10016F68,
};
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
#define SET_ACTION(name)                       \
    do {                                       \
        gabi::Local<ProcFunc_l> pmf;           \
        pmf_load(pmf, PMF_##name);             \
        set_action(pmf, nullptr);              \
    } while (0)

/* 021FC36C */
bool daNpc_Bm1_c::decideType(int i_type_param, int i_spawn_cond_param) {
    WWHD_FUNC(0x021FC36C, bool, this, i_type_param, i_spawn_cond_param);
    mType = TYPE_Invalid_e;
    mSpawnCondition = 0;
    mSpecificType = SPECIFIC_TYPE_Invalid_e;
    u32 type = (u32)i_type_param;
    switch ((u16)fpcM_GetName(this)) {
    case 0x146: /* fpcNm_NPC_BM1_e */
        mType = TYPE_Quill_e;
        if (type > 4)
            return false;
        mSpecificType = (s8)type; /* SPECIFIC_TYPE_Quill_0_e + type */
        break;
    case 0x147: /* fpcNm_NPC_BM2_e */
        switch (type) {
        case 0: mSpecificType = SPECIFIC_TYPE_Skett_e; mType = TYPE_Skett_e; break;
        case 1: mSpecificType = SPECIFIC_TYPE_Akoot_e; mType = TYPE_Akoot_e; break;
        default: return false;
        }
        break;
    case 0x148: /* fpcNm_NPC_BM3_e */
        switch (type) {
        case 0: mSpecificType = SPECIFIC_TYPE_Basht_e; mType = TYPE_Basht_e; break;
        case 1: mSpecificType = SPECIFIC_TYPE_Bisht_e; mType = TYPE_Bisht_e; break;
        case 2: mSpecificType = SPECIFIC_TYPE_Hoskit_e; mType = TYPE_Hoskit_e; break;
        default: return false;
        }
        break;
    case 0x149: /* fpcNm_NPC_BM4_e */
        switch (type) {
        case 0: mSpecificType = SPECIFIC_TYPE_Ilari_0xA_e; mType = TYPE_Ilari_e; break;
        case 1: mSpecificType = SPECIFIC_TYPE_Ilari_0xB_e; mType = TYPE_Ilari_e; break;
        case 2: mSpecificType = SPECIFIC_TYPE_Ilari_0xC_e; mType = TYPE_Ilari_e; break;
        case 3: mSpecificType = SPECIFIC_TYPE_Pashli_e; mType = TYPE_Pashli_e; break;
        default: return false;
        }
        break;
    case 0x14A: /* fpcNm_NPC_BM5_e */
        switch (type) {
        case 0: mSpecificType = SPECIFIC_TYPE_Namali_e; mType = TYPE_Namali_e; break;
        case 1: mSpecificType = SPECIFIC_TYPE_Kogoli_e; mType = TYPE_Kogoli_e; break;
        default: return false;
        }
        break;
    default:
        return false;
    }
    /* switch (i_spawn_cond_param) { case 0..3: mSpawnCondition = 1..4 } (a table at .rodata 0x100174F8) */
    if ((u32)i_spawn_cond_param <= 3) {
        mSpawnCondition = gabi::load<s8>(0x100174F8 + i_spawn_cond_param);
    }
    s32 st = mSpecificType;
    if (st == SPECIFIC_TYPE_Quill_0_e) {
        /* strcpy(mArcName, "Bm2") */
        for (int i = 0; i < 4; i++) gabi::store<u8>(gabi::ea(mArcName) + i, gabi::load<u8>(0x100174F4 + i));
        return true;
    }
    if ((u32)st <= 0xF) {
        /* strcpy(mArcName, "Bm") */
        for (int i = 0; i < 3; i++) gabi::store<u8>(gabi::ea(mArcName) + i, gabi::load<u8>(0x100174FC + i));
        return true;
    }
    return false;
}
VERIFY(0x021FC36C, &daNpc_Bm1_c::decideType);

/* 021FC5DC */
bool daNpc_Bm1_c::chk_appCnd() {
    WWHD_FUNC(0x021FC5DC, bool, this);
    switch ((u32)(s32)mSpawnCondition) {
    case 0:
        return true;
    case 1:
        if (!dComIfGs_isSymbol(1 /* dSymbol_DIN_e */)) {
            return true;
        }
        break;
    case 2:
        if (dComIfGs_isSymbol(1)) {
            return !dComIfGs_isEventBit(0x1A80);
        }
        break;
    case 3:
        if (!dComIfGs_isSymbol(1)) {
            return false;
        }
        if (dComIfGs_isEventBit(0x1A80)) {
            return dKy_daynight_check() == 0 /* dKy_TIME_DAY_e */;
        }
        break;
    case 4:
        if (!dComIfGs_isSymbol(1)) {
            return false;
        }
        if (dComIfGs_isEventBit(0x1A80)) {
            return dKy_daynight_check() == 1 /* dKy_TIME_NIGHT_e */;
        }
        break;
    }
    return false;
}
VERIFY(0x021FC5DC, &daNpc_Bm1_c::chk_appCnd);

/* 021FC758 */
BOOL daNpc_Bm1_c::set_action(ProcFunc_l* i_newProcFunc, void* i_argsP) {
    WWHD_FUNC(0x021FC758, BOOL, this, i_newProcFunc, i_argsP);
    ProcFunc_l* cur = &mCurrActionFunc;
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
    }
    if (cur->i != 0) {
        m904 = 9;
        pmf_call(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    m904 = 0;
    pmf_call(this, cur, i_argsP);
    return TRUE;
}
VERIFY(0x021FC758, &daNpc_Bm1_c::set_action);

/* 021FC884 */
bool daNpc_Bm1_c::init_PST_0() {
    WWHD_FUNC(0x021FC884, bool, this);
    bool o_retval = false;
    if (!dComIfGs_isEventBit(0x0001)) {
        SET_ACTION(demo_action1);
        o_retval = true;
        actor_status &= ~0x3Fu; /* fopAcM_ClearStatusMap(this) */
        mbInitPostman0 = true;
    }
    return o_retval;
}
VERIFY(0x021FC884, &daNpc_Bm1_c::init_PST_0);

/* 021FC910 */
bool daNpc_Bm1_c::init_PST_1() {
    WWHD_FUNC(0x021FC910, bool, this);
    SET_ACTION(wait_action1);
    return true;
}
VERIFY(0x021FC910, &daNpc_Bm1_c::init_PST_1);

/* 021FC950 */
bool daNpc_Bm1_c::init_PST_2() {
    WWHD_FUNC(0x021FC950, bool, this);
    bool o_retval = false;
    if (!dComIfGs_isEventBit(0x1F40)) {
        SET_ACTION(wait_action3);
        o_retval = true;
    }
    return o_retval;
}
VERIFY(0x021FC950, &daNpc_Bm1_c::init_PST_2);

/* 021FC9CC */
bool daNpc_Bm1_c::init_PST_3() {
    WWHD_FUNC(0x021FC9CC, bool, this);
    bool result = dComIfGs_isSymbol(1 /* dSymbol_DIN_e */) == 0;
    if (result) {
        result = dComIfGs_isEventBit(0x1102) != 0;
        if (result) {
            SET_ACTION(wait_action1);
        }
    }
    return result;
}
VERIFY(0x021FC9CC, &daNpc_Bm1_c::init_PST_3);

/* 021FCA68 */
bool daNpc_Bm1_c::init_PST_4() {
    WWHD_FUNC(0x021FCA68, bool, this);
    bool result = dComIfGs_isEventBit(0x1E80) == 0;
    if (result) {
        m888 = 1;
        gravity = 0.0f;
        SET_ACTION(wait_action9);
        actor_status |= 0x4000; /* fopAcM_OnStatus(this, fopAcStts_UNK4000_e) */
        return true;
    }
    return result;
}
VERIFY(0x021FCA68, &daNpc_Bm1_c::init_PST_4);

/* 021FCB00 (unnamed by the matcher) */
bool daNpc_Bm1_c::init_SKT_0() {
    WWHD_FUNC(0x021FCB00, bool, this);
    actor_status &= ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
    SET_ACTION(wait_action7);
    return true;
}
VERIFY(0x021FCB00, &daNpc_Bm1_c::init_SKT_0);

/* 021FCB4C (unnamed by the matcher) */
u32 daNpc_Bm1_c::init_KKT_0() {
    WWHD_FUNC(0x021FCB4C, u32, this);
    return gabi::call<u32>(0x021FCB00, this); /* init_SKT_0() */
}
VERIFY(0x021FCB4C, &daNpc_Bm1_c::init_KKT_0);

/* 021FCB50 (unnamed by the matcher) */
bool daNpc_Bm1_c::init_BMB_0() {
    WWHD_FUNC(0x021FCB50, bool, this);
    SET_ACTION(wait_action6);
    return true;
}
VERIFY(0x021FCB50, &daNpc_Bm1_c::init_BMB_0);

/* 021FCB90 (unnamed by the matcher) */
u32 daNpc_Bm1_c::init_BMB_1() {
    WWHD_FUNC(0x021FCB90, u32, this);
    return gabi::call<u32>(0x021FCB50, this); /* init_BMB_0() */
}
VERIFY(0x021FCB90, &daNpc_Bm1_c::init_BMB_1);

/* 021FCB94 (unnamed by the matcher) */
bool daNpc_Bm1_c::init_BMB_2() {
    WWHD_FUNC(0x021FCB94, bool, this);
    SET_ACTION(wait_action8);
    return true;
}
VERIFY(0x021FCB94, &daNpc_Bm1_c::init_BMB_2);

/* 021FCBD4 (unnamed by the matcher) */
bool daNpc_Bm1_c::init_BMC_0() {
    WWHD_FUNC(0x021FCBD4, bool, this);
    SET_ACTION(wait_action2);
    return true;
}
VERIFY(0x021FCBD4, &daNpc_Bm1_c::init_BMC_0);

/* 021FCC14 */
bool daNpc_Bm1_c::init_BMC_1() {
    WWHD_FUNC(0x021FCC14, bool, this);
    bool result = false;
    if (!dComIfGs_isEventBit(0x1808)) {
        result = dComIfGs_isEventBit(0x1220) != 0;
        if (result) {
            SET_ACTION(wait_action4);
        }
    }
    return result;
}
VERIFY(0x021FCC14, &daNpc_Bm1_c::init_BMC_1);

/* 021FCCB0 */
bool daNpc_Bm1_c::init_BMC_2() {
    WWHD_FUNC(0x021FCCB0, bool, this);
    bool result = false;
    if (!dComIfGs_isGetItemReserve(0x0F)) {
        result = dComIfGs_isEventBit(0x1808) != 0;
        if (result) {
            SET_ACTION(wait_action5);
        }
    }
    return result;
}
VERIFY(0x021FCCB0, &daNpc_Bm1_c::init_BMC_2);

/* 021FCD4C */
bool daNpc_Bm1_c::init_BMC_3() {
    WWHD_FUNC(0x021FCD4C, bool, this);
    if (mPathRun.mPath.get() != nullptr) {
        SET_ACTION(wait_actionA);
        return true;
    }
    return false;
}
VERIFY(0x021FCD4C, &daNpc_Bm1_c::init_BMC_3);

/* 021FCDAC (unnamed by the matcher) */
u32 daNpc_Bm1_c::init_BMD_0() {
    WWHD_FUNC(0x021FCDAC, u32, this);
    return gabi::call<u32>(0x021FCD4C, this); /* init_BMC_3() */
}
VERIFY(0x021FCDAC, &daNpc_Bm1_c::init_BMD_0);

/* 021FCDB0 */
bool daNpc_Bm1_c::init_BMD_1() {
    WWHD_FUNC(0x021FCDB0, bool, this);
    if (dComIfGs_isEventBit(0x1620)) {
        return false;
    }
    return init_BMC_0();
}
VERIFY(0x021FCDB0, &daNpc_Bm1_c::init_BMD_1);

/* 021FCE10 */
void daNpc_Bm1_c::plyTexPttrnAnm() {
    WWHD_FUNC(0x021FCE10, void, this);
    if (mBtpNum != 0 || !cLib_calcTimer_g(&mBlinkTimer)) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(m_hed_tex_pttrn)) {
            if (mBtpNum != 0) {
                mBlinkFrame = (u8)J3DAnm_getFrameMax(m_hed_tex_pttrn);
            } else {
                s16 t = (s16)gabi::ftoi(cM_rndF(60.0f) + 30.0f);
                mBlinkFrame = 0;
                mBlinkTimer = t;
            }
        }
    }
}
VERIFY(0x021FCE10, &daNpc_Bm1_c::plyTexPttrnAnm);

/* 021FCEE4: the matcher calls 021FD0A0 delPrtcl_Flyaway; this one is (the flyaway emitters) */
void daNpc_Bm1_c::delPrtcl_Flyaway() {
    WWHD_FUNC(0x021FCEE4, void, this);
    if (mpFlyawayEmitterL.get() != nullptr) {
        JPABaseEmitter_quitImmortalEmitter(mpFlyawayEmitterL);
        JPABaseEmitter_becomeInvalidEmitter(mpFlyawayEmitterL);
        mpFlyawayEmitterL = nullptr;
    }
    if (mpFlyawayEmitterR.get() != nullptr) {
        JPABaseEmitter_quitImmortalEmitter(mpFlyawayEmitterR);
        JPABaseEmitter_becomeInvalidEmitter(mpFlyawayEmitterR);
        mpFlyawayEmitterR = nullptr;
    }
}
VERIFY(0x021FCEE4, &daNpc_Bm1_c::delPrtcl_Flyaway);

/* 021FCF54 */
void daNpc_Bm1_c::setPrtcl_Flyaway() {
    WWHD_FUNC(0x021FCF54, void, this);
    if (mAnmNum == 0xB && mpMorf->checkFrame(10.0f)) {
        delPrtcl_Flyaway();
        mpFlyawayEmitterL = dComIfGp_particle_set(0x8275 /* ID_IT_SN_BM_HANEL_FLYAWAY00 */, &mWingLPos, &current.angle, nullptr,
                                                  0xFF, nullptr, fopAcM_GetRoomNo(this), gabi::at<GXColor>(gabi::ea(&tevStr) + 0x98),
                                                  gabi::at<GXColor>(gabi::ea(&tevStr) + 0x98));
        mpFlyawayEmitterR = dComIfGp_particle_set(0x8275, &mWingRPos, &current.angle, nullptr, 0xFF, nullptr,
                                                  fopAcM_GetRoomNo(this), gabi::at<GXColor>(gabi::ea(&tevStr) + 0x98),
                                                  gabi::at<GXColor>(gabi::ea(&tevStr) + 0x98));
        if (mpFlyawayEmitterL.get() != nullptr) {
            JPABaseEmitter_becomeImmortalEmitter(mpFlyawayEmitterL);
        }
        if (mpFlyawayEmitterR.get() != nullptr) {
            JPABaseEmitter_becomeImmortalEmitter(mpFlyawayEmitterR);
            JPABaseEmitter_setDirection(mpFlyawayEmitterR, -1.0f, 1.0f, 0.0f);
        }
    }
}
VERIFY(0x021FCF54, &daNpc_Bm1_c::setPrtcl_Flyaway);

/* 021FD0A0: the matcher calls it delPrtcl_Flyaway; it is delPrtcl_Land0 (the landing emitters) */
void daNpc_Bm1_c::delPrtcl_Land0() {
    WWHD_FUNC(0x021FD0A0, void, this);
    if (mpLandEmitterL.get() != nullptr) {
        JPABaseEmitter_quitImmortalEmitter(mpLandEmitterL);
        JPABaseEmitter_becomeInvalidEmitter(mpLandEmitterL);
        mpLandEmitterL = nullptr;
    }
    if (mpLandEmitterR.get() != nullptr) {
        JPABaseEmitter_quitImmortalEmitter(mpLandEmitterR);
        JPABaseEmitter_becomeInvalidEmitter(mpLandEmitterR);
        mpLandEmitterR = nullptr;
    }
}
VERIFY(0x021FD0A0, &daNpc_Bm1_c::delPrtcl_Land0);

/* 021FD110 */
void daNpc_Bm1_c::setPrtcl_Land0() {
    WWHD_FUNC(0x021FD110, void, this);
    if (mAnmNum == 0xD && mpMorf->checkFrame(1.0f)) {
        delPrtcl_Land0();
        mpLandEmitterL = dComIfGp_particle_set(0x8276 /* ID_IT_SN_BM_HANEL_LAND00 */, &mArmLPos, &current.angle, nullptr, 0xFF,
                                               nullptr, fopAcM_GetRoomNo(this), gabi::at<GXColor>(gabi::ea(&tevStr) + 0x98),
                                               gabi::at<GXColor>(gabi::ea(&tevStr) + 0x98));
        mpLandEmitterR = dComIfGp_particle_set(0x8276, &mArmRPos, &current.angle, nullptr, 0xFF, nullptr, fopAcM_GetRoomNo(this),
                                               gabi::at<GXColor>(gabi::ea(&tevStr) + 0x98),
                                               gabi::at<GXColor>(gabi::ea(&tevStr) + 0x98));
        if (mpLandEmitterL.get() != nullptr) {
            JPABaseEmitter_becomeImmortalEmitter(mpLandEmitterL);
        }
        if (mpLandEmitterR.get() != nullptr) {
            JPABaseEmitter_becomeImmortalEmitter(mpLandEmitterR);
            JPABaseEmitter_setDirection(mpLandEmitterR, -1.0f, 0.5f, 0.0f);
        }
    }
}
VERIFY(0x021FD110, &daNpc_Bm1_c::setPrtcl_Land0);

/* 021FD25C (unnamed by the matcher) */
void daNpc_Bm1_c::flwPrtcl_Hane0() {
    WWHD_FUNC(0x021FD25C, void, this);
    if (mpFeatherEmitterL.get() != nullptr) {
        JPABaseEmitter_setGlobalRTMatrix(mpFeatherEmitterL, getAnmMtx(mpWingMorf->getModel(), m_wngL3_jnt_num));
    }
    if (mpFeatherEmitterR.get() != nullptr) {
        JPABaseEmitter_setGlobalRTMatrix(mpFeatherEmitterR, getAnmMtx(mpWingMorf->getModel(), m_wngR3_jnt_num));
    }
}
VERIFY(0x021FD25C, &daNpc_Bm1_c::flwPrtcl_Hane0);

/* 021FD30C (unnamed by the matcher) */
void daNpc_Bm1_c::flwPrtcl_Hane1() {
    WWHD_FUNC(0x021FD30C, void, this);
    if (mpFeather1EmitterL.get() != nullptr) {
        JPABaseEmitter_setGlobalRTMatrix(mpFeather1EmitterL, getAnmMtx(mpWingMorf->getModel(), m_wngL3_jnt_num));
    }
    if (mpFeather1EmitterR.get() != nullptr) {
        JPABaseEmitter_setGlobalRTMatrix(mpFeather1EmitterR, getAnmMtx(mpWingMorf->getModel(), m_wngR3_jnt_num));
    }
}
VERIFY(0x021FD30C, &daNpc_Bm1_c::flwPrtcl_Hane1);

/* 021FD3BC (unnamed by the matcher) */
void daNpc_Bm1_c::setAttention(u32 i_setEyePos) {
    WWHD_FUNC(0x021FD3BC, void, this, i_setEyePos);
    f32 l_hio_offset = gabi::load<f32>(l_HIO_prm(mType) + 0x14); /* mAttPosOffsetY */
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attPos->x = current.pos.x;
    attPos->y = current.pos.y + l_hio_offset;
    attPos->z = current.pos.z;
    if (!mbSetEyePos && !i_setEyePos) {
        return;
    }
    eyePos.x = mEyePos.x;
    eyePos.y = mEyePos.y;
    eyePos.z = mEyePos.z;
}
VERIFY(0x021FD3BC, &daNpc_Bm1_c::setAttention);

/* 021FD41C */
void daNpc_Bm1_c::setMtx(u32 i_param_1) {
    WWHD_FUNC(0x021FD41C, void, this, i_param_1);
    void* gndPoly = gabi::at<u8>(gabi::ea(&mObjAcch) + 0xD4 + 0x14); /* mObjAcch.m_gnd's cBgS_PolyInfo */
    if (mbInDemo == 0) {
        u32 uVar5 = 0;
        plyTexPttrnAnm();
        if (mObjAcch.ChkGroundHit()) {
            uVar5 = dBgS_GetMtrlSndId(dComIfG_Bgsp(), (cBgS_PolyInfo*)gndPoly);
        }
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mbMorfAnimStopped = (s8)mpMorf->play(&eyePos, uVar5, (s8)reverb);
        if (mpMorf->getFrame() < mFrame) {
            mbMorfAnimStopped = true;
        }
        mFrame = mpMorf->getFrame();
        if (mbHasArms) {
            mpArmMorf->play(&eyePos, 0, 0);
        } else {
            mpWingMorf->play(&eyePos, 0, 0);
        }
        mpHeadMorf->play(&eyePos, 0, 0);
        mObjAcch.CrrPos(dComIfG_Bgsp());
        /* HD: with the HD-only flag at 0xA08 set, the actor stays at its old position */
        if (mHD_A08 != 0) {
            current.pos.copy(old.pos);
        }
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), gndPoly);
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), gndPoly)); /* tevStr.mEnvrIdxOverride */
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    if (!mbHasArms) {
        mpWingMorf->calc();
    } else {
        mpArmMorf->calc();
    }
    J3DModel_setBaseTRMtx(mpHeadMorf->getModel(), getAnmMtx(mpMorf->getModel(), m_hed_jnt_num));
    /* HD: calc only with an animation */
    if (mpHeadMorf->getAnm() != nullptr) {
        mpHeadMorf->calc();
    }
    if (mpBinderModel.get() != nullptr) {
        if (mAnmNum == 4) {
            PSMTXCopy(getAnmMtx(mpArmMorf->getModel(), m_hnd_R_jnt_num), mDoMtx_stack_c::get());
            mDoMtx_stack_c::transM(13.5f, 3.0f, -5.5f);
            mDoMtx_XYZrotM(mDoMtx_stack_c::get(), -0x5DDE /* cM_deg2s(228) */, -0x5111 /* cM_deg2s(-114) */,
                           0x671D /* cM_deg2s(-215) */);
            J3DModel_setBaseTRMtx(mpBinderModel, mDoMtx_stack_c::get());
        } else {
            J3DModel_setBaseTRMtx(mpBinderModel, getAnmMtx(mpArmMorf->getModel(), m_hnd_R_jnt_num));
        }
        J3DModel_calc(mpBinderModel);
    }
    if (mpBagModel.get() != nullptr) {
        J3DModel_setBaseTRMtx(mpBagModel, getAnmMtx(mpMorf->getModel(), m_bbone_jnt_num));
        J3DModel_calc(mpBagModel);
    }
    if (mpKnifeModel.get() != nullptr) {
        J3DModel_setBaseTRMtx(mpKnifeModel, getAnmMtx(mpMorf->getModel(), m_bbone_jnt_num));
        J3DModel_calc(mpKnifeModel);
    }
    if (mbHasArms && mpStickModel.get() != nullptr) {
        J3DModel_setBaseTRMtx(mpStickModel, getAnmMtx(mpArmMorf->getModel(), m_hnd_R_jnt_num));
        J3DModel_calc(mpStickModel);
    }
    setPrtcl_Flyaway();
    setPrtcl_Land0();
    flwPrtcl_Hane0();
    flwPrtcl_Hane1();
    setAttention(i_param_1);
}
VERIFY(0x021FD41C, &daNpc_Bm1_c::setMtx);

/* 021FDB44 */
bool daNpc_Bm1_c::createInit() {
    WWHD_FUNC(0x021FDB44, bool, this);
    if (chk_appCnd() == 0) {
        return false;
    }
    /* l_evn_tbl (.data 0x101BC580): "Get_Mo3_Ltr", "Met_Ryu_Islnd", "Get_Rupee", "Skn_Islnd" */
    for (int i = 0; i < 4; i++) {
        const char* name = gabi::at<const char>(gabi::load<u32>(0x101BC580 + i * 4));
        mEventIdTable[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }
    /* a_att_dis_TBL[11][2] (.data 0x101BC674) */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    gabi::store<u8>(gabi::ea(this) + 0x389, gabi::load<u8>(0x101BC674 + mType * 2));     /* distances[TALK] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, gabi::load<u8>(0x101BC674 + mType * 2 + 1)); /* distances[SPEAK] */
    gravity = -4.5f;
    m82C.copy(current.pos);
    s32 iVar5 = 0xFF;
    u32 pathId = (fopAcM_GetParam(this) >> 16) & 0xFF;
    if (pathId != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, pathId, fopAcM_GetRoomNo(this), true);
        /* HD: mPathRun.isPath() is the path pointer test */
        if (mPathRun.mPath.get() == nullptr) {
            return false;
        }
        actor_status &= ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
        iVar5 = 0xF0;
    }
    /* a_staff_tbl (.data 0x101BC634): "Bm1" x5, "Bm2" x2, "Bm3" x3, "Bm4" x4, "Bm5" x2 */
    mEventCut.setActorInfo2(gabi::at<const char>(gabi::load<u32>(0x101BC634 + mSpecificType * 4)), (fopNpc_npc_c*)this);
    mAnmNum = 0x16;
    u32 init_success;
    switch ((u32)(s32)mSpecificType) {
    case SPECIFIC_TYPE_Quill_0_e: init_success = init_PST_0(); break;
    case SPECIFIC_TYPE_Quill_1_e: init_success = init_PST_1(); break;
    case SPECIFIC_TYPE_Quill_2_e: init_success = init_PST_2(); break;
    case SPECIFIC_TYPE_Quill_3_e: init_success = init_PST_3(); break;
    case SPECIFIC_TYPE_Quill_4_e: init_success = init_PST_4(); break;
    case SPECIFIC_TYPE_Akoot_e: init_success = init_KKT_0(); break;
    case SPECIFIC_TYPE_Skett_e: init_success = init_SKT_0(); break;
    case SPECIFIC_TYPE_Basht_e: init_success = init_BMB_0(); break;
    case SPECIFIC_TYPE_Bisht_e: init_success = init_BMB_1(); break;
    case SPECIFIC_TYPE_Hoskit_e: init_success = init_BMB_2(); break;
    case SPECIFIC_TYPE_Ilari_0xA_e: init_success = init_BMC_0(); break;
    case SPECIFIC_TYPE_Ilari_0xB_e: init_success = init_BMC_1(); break;
    case SPECIFIC_TYPE_Ilari_0xC_e: init_success = init_BMC_2(); break;
    case SPECIFIC_TYPE_Pashli_e: init_success = init_BMC_3(); break;
    case SPECIFIC_TYPE_Namali_e: init_success = init_BMD_0(); break;
    case SPECIFIC_TYPE_Kogoli_e: init_success = init_BMD_1(); break;
    default: init_success = false; break;
    }
    if (init_success == 0) {
        return false;
    }
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    mStts.Init(iVar5, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mpMorf->setMorf(0.0f);
    if (mbHasArms) {
        mpArmMorf->setMorf(0.0f);
    } else {
        mpWingMorf->setMorf(0.0f);
    }
    mpHeadMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}
VERIFY(0x021FDB44, &daNpc_Bm1_c::createInit);

/* 021FDF08 */
cPhs_State daNpc_Bm1_c::_create() {
    WWHD_FUNC(0x021FDF08, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Bm1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this); /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
            __vtbl = BM1_VTBL;
            gabi::call(0x025E7820, mHeadBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            gabi::call(0x0259F740, &mEventCut);  /* dNpc_EventCut_c::dNpc_EventCut_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    u32 param = fopAcM_GetParam(this);
    if (!decideType(param & 0xFF, (param >> 8) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, mArcName);
    m87F = state == cPhs_COMPLEATE_e;
    if (!m87F) {
        return state;
    }
    /* a_size_tbl[mType] (.data 0x101BC68C) */
    if (!fopAcM_entrySolidHeap(this, 0x021FC368 /* CheckCreateHeap */, gabi::load<u32>(0x101BC68C + mType * 4))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    if (mSpecificType == SPECIFIC_TYPE_Quill_0_e) {
        fopAcM_setCullSizeBox(this, -250.0f, -20.0f, -200.0f, 250.0f, 500.0f, 200.0f);
    } else {
        fopAcM_setCullSizeBox(this, -70.0f, -20.0f, -70.0f, 70.0f, 220.0f, 70.0f);
    }
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x021FDF08, &daNpc_Bm1_c::_create);

/* 021FE0BC (unnamed by the matcher) */
void daNpc_Bm1_c::delPrtcl_Hane0() {
    WWHD_FUNC(0x021FE0BC, void, this);
    if (mpFeatherEmitterL.get() != nullptr) {
        JPABaseEmitter_becomeInvalidEmitter(mpFeatherEmitterL);
        mpFeatherEmitterL = nullptr;
    }
    if (mpFeatherEmitterR.get() != nullptr) {
        JPABaseEmitter_becomeInvalidEmitter(mpFeatherEmitterR);
        mpFeatherEmitterR = nullptr;
    }
}
VERIFY(0x021FE0BC, &daNpc_Bm1_c::delPrtcl_Hane0);

/* 021FE10C (unnamed by the matcher) */
void daNpc_Bm1_c::delPrtcl_Hane1() {
    WWHD_FUNC(0x021FE10C, void, this);
    if (mpFeather1EmitterL.get() != nullptr) {
        JPABaseEmitter_quitImmortalEmitter(mpFeather1EmitterL);
        JPABaseEmitter_becomeInvalidEmitter(mpFeather1EmitterL);
        mpFeather1EmitterL = nullptr;
    }
    if (mpFeather1EmitterR.get() != nullptr) {
        JPABaseEmitter_quitImmortalEmitter(mpFeather1EmitterR);
        JPABaseEmitter_becomeInvalidEmitter(mpFeather1EmitterR);
        mpFeather1EmitterR = nullptr;
    }
}
VERIFY(0x021FE10C, &daNpc_Bm1_c::delPrtcl_Hane1);

/* 021FE17C */
BOOL daNpc_Bm1_c::_delete() {
    WWHD_FUNC(0x021FE17C, BOOL, this);
    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr) {
        if (mpMorf.get() != nullptr) {
            mpMorf->stopZelAnime();
        }
        if (mpHeadMorf.get() != nullptr) {
            mpHeadMorf->stopZelAnime();
        }
        if (mpWingMorf.get() != nullptr) {
            mpWingMorf->stopZelAnime();
        }
        if (mpArmMorf.get() != nullptr) {
            mpArmMorf->stopZelAnime();
        }
    }
    delPrtcl_Flyaway();
    delPrtcl_Land0();
    delPrtcl_Hane0();
    delPrtcl_Hane1();
    return TRUE;
}
VERIFY(0x021FE17C, &daNpc_Bm1_c::_delete);

/* 021FE218 */
bool daNpc_Bm1_c::partner_srch_sub(u32 i_searchFunc) {
    WWHD_FUNC(0x021FE218, bool, this, i_searchFunc);
    bool o_retval = false;
    mPartnerProcID = 0xFFFFFFFF;
    l_check_wrk() = 0;
    for (int i = 0; i < 20; i++) {
        l_check_inf()[i] = nullptr;
    }
    fpcM_Search(i_searchFunc, this);
    if (l_check_wrk() != 0) {
        mPartnerProcID = fopAcM_GetID(l_check_inf()[0].get());
        o_retval = true;
    }
    return o_retval;
}
VERIFY(0x021FE218, &daNpc_Bm1_c::partner_srch_sub);

/* 021FE2C4: the matcher calls it cLib_calcTimer<s16> (d_a_npc_ac1); it is searchByID */
fopAc_ac_c* daNpc_Bm1_c::searchByID(fpc_ProcID i_procID) {
    WWHD_FUNC(0x021FE2C4, fopAc_ac_c*, this, i_procID);
    gabi::Local<gptr<fopAc_ac_c>> o_actor;
    *o_actor = nullptr;
    gabi::call(0x025D54C4, i_procID, o_actor.get()); /* fopAcM_SearchByID(id, &actor) */
    return *o_actor;
}
VERIFY(0x021FE2C4, &daNpc_Bm1_c::searchByID);

/* 021FE2F8 */
void daNpc_Bm1_c::partner_srch() {
    WWHD_FUNC(0x021FE2F8, void, this);
    bool found_partner = false;
    if (m904 == 1) {
        switch ((u32)(s32)mSpecificType) {
        case SPECIFIC_TYPE_Quill_1_e:
            found_partner = partner_srch_sub(0x021FA778 /* searchActor_Zl */);
            break;
        case SPECIFIC_TYPE_Skett_e:
            found_partner = partner_srch_sub(0x021FA908 /* searchActor_Bm_Kkt */);
            break;
        case SPECIFIC_TYPE_Akoot_e:
            found_partner = partner_srch_sub(0x021FA878 /* searchActor_Bm_Skt */);
            break;
        case SPECIFIC_TYPE_Ilari_0xB_e:
            if (partner_srch_sub(0x021FA7F8 /* searchActor_Gp */)) {
                fopAc_ac_c* actor = searchByID(mPartnerProcID);
                if (actor != nullptr) {
                    current.angle.y = cLib_targetAngleY(&current.pos, &actor->current.pos);
                }
                found_partner = true;
            }
            break;
        }
        if (found_partner) {
            m904 = m904 + 1;
        }
    }
}
VERIFY(0x021FE2F8, &daNpc_Bm1_c::partner_srch);

/* 021FE408 */
void daNpc_Bm1_c::checkOrder() {
    WWHD_FUNC(0x021FE408, void, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIdTable[mEventIdx])) {
            if (mEventIdx == 3) {
                actor_status &= ~0x4000u; /* fopAcM_OffStatus(this, fopAcStts_UNK4000_e) */
            }
            m8FD = 0;
        }
    } else if (command == 1 /* dEvtCmd_INTALK_e */ && (m8FD == 1 || m8FD == 2)) {
        m8FD = 0;
        m895 = true;
    }
}
VERIFY(0x021FE408, &daNpc_Bm1_c::checkOrder);

/* 021FE4DC */
/* returns the mbInDemo byte as is (typed u8) */
u8 daNpc_Bm1_c::demo() {
    WWHD_FUNC(0x021FE4DC, u8, this);
    if (demoActorID == 0) {
        if (mbInDemo != 0) {
            mbInDemo = 0;
        }
        return mbInDemo;
    }
    u8 id = demoActorID;
    mbInDemo = 1;
    /* dComIfGp_demo_getActor(demoActorID): HD inline with a range check and the demo object
     * (0x101D5FFC) asserted */
    void* demo_actor = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x1001706C), 0x23A, STR(0x10017028));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor = gabi::call<void*>(0x02526E70, obj, id); /* dDemo_object_c::getActor */
    }
    if (m_hed_tex_pttrn.get() != nullptr) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(m_hed_tex_pttrn)) {
            mBlinkFrame = (u8)J3DAnm_getFrameMax(m_hed_tex_pttrn);
        }
    }
    /* HD: demo_actor is checked for NULL */
    if (demo_actor != nullptr) {
        J3DAnmTexPattern* demopattern = gabi::call<J3DAnmTexPattern*>(0x02527828, demo_actor, mArcName); /* getP_BtpData */
        if (demopattern != nullptr) {
            m_hed_tex_pttrn = demopattern;
            J3DModelData* md = J3DModel_getModelData_l(mpHeadMorf->getModel());
            if (mDoExt_btpAnm_init(mHeadBtpAnm, md, demopattern, 1, 2, 1.0f, 0, -1, 1, 0)) {
                mBlinkFrame = 0;
                mBtpNum = 1;
            }
        }
    }
    dDemo_setDemoData(this, 0x6A, mpMorf, mArcName, 0, nullptr, 0, 0);
    return mbInDemo;
}
VERIFY(0x021FE4DC, &daNpc_Bm1_c::demo);

/* 021FE68C */
BOOL daNpc_Bm1_c::isEventEntry() {
    WWHD_FUNC(0x021FE68C, BOOL, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x021FE68C, &daNpc_Bm1_c::isEventEntry);

/* 021FE6CC */
void daNpc_Bm1_c::endEvent() {
    WWHD_FUNC(0x021FE6CC, void, this);
    dComIfGp_event_reset();
    m8F7 = 0xFF;
}
VERIFY(0x021FE6CC, &daNpc_Bm1_c::endEvent);

/* 021FE70C (unnamed by the matcher) */
u32 daNpc_Bm1_c::eInit_DEL_ACTOR_() {
    WWHD_FUNC(0x021FE70C, u32, this);
    return fopAcM_delete(this);
}
VERIFY(0x021FE70C, &daNpc_Bm1_c::eInit_DEL_ACTOR_);

/* 021FE710 */
void daNpc_Bm1_c::eInit_SET_NXT_PTH_INF_() {
    WWHD_FUNC(0x021FE710, void, this);
    if (mPathRun.mPath.get() != nullptr) {
        dPath* path = dNpc_PathRun_nextPath(&mPathRun, fopAcM_GetRoomNo(this));
        if (path != nullptr) {
            dNpc_PathRun_setInfDrct(&mPathRun, path);
        }
    }
}
VERIFY(0x021FE710, &daNpc_Bm1_c::eInit_SET_NXT_PTH_INF_);

/* 021FE768 (unnamed by the matcher) */
void daNpc_Bm1_c::eInit_INI_EVN_1_() {
    WWHD_FUNC(0x021FE768, void, this);
    m888 = 0;
}
VERIFY(0x021FE768, &daNpc_Bm1_c::eInit_INI_EVN_1_);

/* 021FE774 */
u32 daNpc_Bm1_c::setAnm_tex(s8 i_param_1) {
    WWHD_FUNC(0x021FE774, u32, this, i_param_1);
    if (mBtpNum != i_param_1) {
        mBtpNum = i_param_1;
        return iniTexPttrnAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x021FE774, &daNpc_Bm1_c::setAnm_tex);

/* name helpers: strcpy(l_BCKName, tbl[i]); strcat(l_BCKName, ".bck") (l_BCKName at 0x104661FC) */
static u32 bckName(u32 tbl, int i, u32 ext) {
    u32 l_BCKName = 0x104661FC;
    strcpy_g(gabi::at<char>(l_BCKName), gabi::at<const char>(gabi::load<u32>(tbl + i * 4)));
    strcat_i(l_BCKName, ext);
    return l_BCKName;
}

/* 021FE794 (unnamed by the matcher) */
u32 daNpc_Bm1_c::anmNum_toResID(int i_anmNum) {
    WWHD_FUNC(0x021FE794, u32, this, i_anmNum);
    return bckName(0x101BC6B8 /* a_BCKName_TBL */, i_anmNum, 0x10017558 /* ".bck" */);
}
VERIFY(0x021FE794, &daNpc_Bm1_c::anmNum_toResID);

/* 021FE808 (unnamed by the matcher) */
u32 daNpc_Bm1_c::wingAnmNum_toResID(int i_anmNum) {
    WWHD_FUNC(0x021FE808, u32, this, i_anmNum);
    return bckName(0x101BC710 /* "bmwing_dwait", .. */, i_anmNum, 0x10017664 /* ".bck" */);
}
VERIFY(0x021FE808, &daNpc_Bm1_c::wingAnmNum_toResID);

/* 021FE87C (unnamed by the matcher) */
u32 daNpc_Bm1_c::headAnmNum_toResID(int i_anmNum) {
    WWHD_FUNC(0x021FE87C, u32, this, i_anmNum);
    return bckName(0x101BC768 /* "bmhead01_dwait", .. */, i_anmNum, 0x100177C4 /* ".bck" */);
}
VERIFY(0x021FE87C, &daNpc_Bm1_c::headAnmNum_toResID);

/* 021FE8EC */
void daNpc_Bm1_c::setPrtcl_Hane1() {
    WWHD_FUNC(0x021FE8EC, void, this);
    delPrtcl_Hane1();
    mpFeather1EmitterL = dComIfGp_particle_set(0x80DB /* ID_IT_SN_TORIZOKU_HANE01 */, &current.pos, nullptr, nullptr, 0xFF, nullptr,
                                               fopAcM_GetRoomNo(this));
    if (mpFeather1EmitterL.get() != nullptr) {
        JPABaseEmitter_becomeImmortalEmitter(mpFeather1EmitterL);
    }
    mpFeather1EmitterR = dComIfGp_particle_set(0x80DB, &current.pos, nullptr, nullptr, 0xFF, nullptr, fopAcM_GetRoomNo(this));
    if (mpFeather1EmitterR.get() != nullptr) {
        JPABaseEmitter_becomeImmortalEmitter(mpFeather1EmitterR);
    }
}
VERIFY(0x021FE8EC, &daNpc_Bm1_c::setPrtcl_Hane1);

/* 021FE9E0 */
void daNpc_Bm1_c::setPrtcl_Hane0() {
    WWHD_FUNC(0x021FE9E0, void, this);
    delPrtcl_Hane0();
    mpFeatherEmitterL = dComIfGp_particle_set(0x80D6 /* ID_IT_SN_TORIZOKU_HANE00 */, &current.pos, nullptr, nullptr, 0xFF, nullptr,
                                              fopAcM_GetRoomNo(this));
    mpFeatherEmitterR = dComIfGp_particle_set(0x80D6, &current.pos, nullptr, nullptr, 0xFF, nullptr, fopAcM_GetRoomNo(this));
}
VERIFY(0x021FE9E0, &daNpc_Bm1_c::setPrtcl_Hane0);

/* 021FEAB4 */
BOOL daNpc_Bm1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    WWHD_FUNC(0x021FEAB4, BOOL, this, i_anmPrmP);
    s8 anmNum = i_anmPrmP->anmNum;
    if (mAnmNum == anmNum) {
        return TRUE;
    }
    mAnmNum = anmNum;
    u32 name = anmNum_toResID(anmNum);
    dNpc_setAnmFNDirect(mpMorf, i_anmPrmP->loopMode, i_anmPrmP->morf, i_anmPrmP->speed, name, 0, mArcName);
    bool hasArms = i_anmPrmP->hasArms == 1;
    mbHasArms = hasArms;
    f32 morf = i_anmPrmP->morf;
    s32 loopMode = i_anmPrmP->loopMode;
    f32 speed = i_anmPrmP->speed;
    /* HD: the arms or wings take the wing table, the head the head table */
    mDoExt_McaMorf* partsMorf = hasArms ? mpArmMorf.get() : mpWingMorf.get();
    name = wingAnmNum_toResID(mAnmNum);
    dNpc_setAnmFNDirect(partsMorf, loopMode, morf, speed, name, 0, mArcName);
    name = headAnmNum_toResID(mAnmNum);
    dNpc_setAnmFNDirect(mpHeadMorf, i_anmPrmP->loopMode, i_anmPrmP->morf, i_anmPrmP->speed, name, 0, mArcName);
    delPrtcl_Hane0();
    delPrtcl_Hane1();
    switch ((u32)(s32)mAnmNum) {
    case 2:
        setPrtcl_Hane1();
        break;
    case 3:
        setPrtcl_Hane0();
        break;
    }
    mbMorfAnimStopped = false;
    mFrame = 0.0f;
    m87B = 0;
    return TRUE;
}
VERIFY(0x021FEAB4, &daNpc_Bm1_c::setAnm_anm);

/* 021FECF4 */
void daNpc_Bm1_c::setAnm_NUM(int i_param_1, int i_param_2) {
    WWHD_FUNC(0x021FECF4, void, this, i_param_1, i_param_2);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BC7C0); /* [0x17] */
    if (i_param_2 != 0) {
        setAnm_tex(a_anm_prm_tbl[i_param_1].btpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[i_param_1]);
}
VERIFY(0x021FECF4, &daNpc_Bm1_c::setAnm_NUM);

/* 021FED60 (unnamed by the matcher) */
void daNpc_Bm1_c::eInit_SET_ANM_(be<s32>* i_param_1) {
    WWHD_FUNC(0x021FED60, void, this, i_param_1);
    if (i_param_1 != nullptr && (u32)(s32)*i_param_1 < 0x17) {
        setAnm_NUM(*i_param_1, 1);
    }
}
VERIFY(0x021FED60, &daNpc_Bm1_c::eInit_SET_ANM_);

/* 021FED7C */
void daNpc_Bm1_c::eInit_MOV_PTH_POINT_(be<s32>* i_prm_0_p, be<s32>* i_anm_no_p, be<s32>* i_index_p, be<s32>* i_angle_p) {
    WWHD_FUNC(0x021FED7C, void, this, i_prm_0_p, i_anm_no_p, i_index_p, i_angle_p);
    if (mPathRun.mPath.get() == nullptr) {
        return;
    }
    u8 idx = mPathRun.mIdx;
    if (i_index_p != nullptr) {
        u32 i = gabi::load<u8>(gabi::ea(i_index_p) + 3); /* (u8)*i_index_p */
        u32 max = dNpc_PathRun_maxPoint(&mPathRun);
        idx = (u8)(i >= max ? max : i); /* cLib_maxLimit */
        mPathRun.mIdx = idx;
    }
    gabi::Local<cXyz> pos;
    dNpc_PathRun_getPoint(&mPathRun, pos, idx);
    current.pos.copy(*pos);
    dNpc_PathRun_nextIdxAuto(&mPathRun);
    dNpc_PathRun_getPoint(&mPathRun, pos, mPathRun.mIdx);
    gabi::Local<cXyz> pathpos;
    pathpos->copy(*pos);
    current.angle.y = cLib_targetAngleY(&current.pos, pathpos);
    if (i_prm_0_p != nullptr) {
        if (*i_prm_0_p == 1) {
            current.angle.y = cLib_targetAngleY(&current.pos, &dComIfGp_getLinkPlayer()->current.pos);
        }
    } else if (i_angle_p != nullptr) {
        current.angle.y = gabi::load<s16>(gabi::ea(i_angle_p) + 2); /* (s16)*i_angle_p */
    }
    eInit_SET_ANM_(i_anm_no_p);
}
VERIFY(0x021FED7C, &daNpc_Bm1_c::eInit_MOV_PTH_POINT_);

/* 021FEEA0 */
f32 daNpc_Bm1_c::eInit_prmFloat(be<f32>* i_param_1, f32 i_param_2) {
    WWHD_FUNC(0x021FEEA0, f32, this, i_param_1, i_param_2);
    if (i_param_1 != nullptr) {
        return *i_param_1;
    }
    return i_param_2;
}
VERIFY(0x021FEEA0, &daNpc_Bm1_c::eInit_prmFloat);

/* 021FEEB0 */
void daNpc_Bm1_c::bm_setFlyAnm() {
    WWHD_FUNC(0x021FEEB0, void, this);
    setAnm_NUM(m889 ? 9 : 8, 1);
}
VERIFY(0x021FEEB0, &daNpc_Bm1_c::bm_setFlyAnm);

/* 021FEECC */
void daNpc_Bm1_c::eInit_FLY_(be<s32>* i_prm_0_p, be<f32>* i_speed_p, be<f32>* i_spd_y_p, be<f32>* i_accel_p, be<f32>* i_acc_y_p) {
    WWHD_FUNC(0x021FEECC, void, this, i_prm_0_p, i_speed_p, i_spd_y_p, i_accel_p, i_acc_y_p);
    if (i_prm_0_p == nullptr) {
        return;
    }
    m858 = gabi::load<f32>(l_HIO_prm(mType) + 0x30); /* m30 */
    if (*i_prm_0_p != 2) {
        mTargetFlySpeed = eInit_prmFloat(i_speed_p, gabi::load<f32>(l_HIO_prm(mType) + 0x20)); /* m20 */
        mFlySpeedY = eInit_prmFloat(i_spd_y_p, gabi::load<f32>(l_HIO_prm(mType) + 0x28));      /* m28 */
        mTargetFlyStep = eInit_prmFloat(i_accel_p, gabi::load<f32>(l_HIO_prm(mType) + 0x24));  /* m24 */
        mFlyAccelY = eInit_prmFloat(i_acc_y_p, gabi::load<f32>(l_HIO_prm(mType) + 0x2C));      /* m2C */
    }
    m88A = 0;
    mLookBackState = 0;
    switch ((u32)(s32)*i_prm_0_p) {
    case 1:
        m8F4 = 5;
        speedF = mTargetFlySpeed;
        speed.y = mFlySpeedY;
        gravity = 0.0f;
        m889 = 1;
        bm_setFlyAnm();
        break;
    case 3:
        speedF = mTargetFlySpeed;
        break;
    case 4:
        m8F4 = 3;
        mTargetFlySpeed = 0.0f;
    case 5:
        speed.y = 0.0f;
        speedF = mTargetFlySpeed;
        gravity = 0.0f;
        break;
    default:
        m889 = 1;
        m8F4 = 1;
        speed.y = 0.0f;
        speedF = 0.0f;
        gravity = 0.0f;
    case 2:
        break;
    }
}
VERIFY(0x021FEECC, &daNpc_Bm1_c::eInit_FLY_);

/* 021FF0CC */
/* returns the cXyz through the hidden result pointer; HD (GHS): a NULL result pointer allocates */
cXyz* daNpc_Bm1_c::eInit_calcRelativPos(cXyz* o_result, cXyz* i_param_2, be<s32>* arg2) {
    WWHD_FUNC(0x021FF0CC, cXyz*, this, o_result, i_param_2, arg2);
    s16 sVar1;
    if (arg2 != nullptr) {
        sVar1 = (s16)(shape_angle.y + gabi::load<s16>(gabi::ea(arg2) + 2));
    } else {
        sVar1 = shape_angle.y;
    }
    gabi::Local<cXyz> local_1c;
    gabi::Local<cXyz> local_28;
    f32 z = current.pos.z;
    if (i_param_2 != nullptr) {
        local_1c->x = i_param_2->x;
        local_1c->y = i_param_2->y;
        local_1c->z = i_param_2->z;
    } else {
        local_1c->x = 0.0f;
        local_1c->z = 0.0f;
        local_1c->y = 0.0f;
    }
    PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), sVar1);
    PSMTXMultVec(mDoMtx_stack_c::get(), local_1c, local_28);
    cXyz* res = o_result;
    if (res == nullptr) {
        res = (cXyz*)operator_new(0xC);
        if (res == nullptr)
            return res;
    }
    res->x = local_28->x;
    res->y = local_28->y;
    res->z = local_28->z;
    return res;
}
VERIFY(0x021FF0CC, &daNpc_Bm1_c::eInit_calcRelativPos);

/* 021FF210 */
void daNpc_Bm1_c::eInit_SET_PLYER_GOL_(be<s32>* i_prm_0_p, cXyz* i_offset_p, be<s32>* i_angle_p) {
    WWHD_FUNC(0x021FF210, void, this, i_prm_0_p, i_offset_p, i_angle_p);
    if (i_prm_0_p == nullptr) {
        return;
    }
    gabi::Local<cXyz> local_c;
    switch ((u32)(s32)*i_prm_0_p) {
    case 0:
        if (i_offset_p != nullptr) {
            local_c->x = i_offset_p->x;
            local_c->y = i_offset_p->y;
            local_c->z = i_offset_p->z;
            dComIfGp_evmng_setGoal(local_c);
        }
        break;
    case 1: {
        gabi::Local<cXyz> rel;
        eInit_calcRelativPos(rel, i_offset_p, i_angle_p);
        local_c->copy(*rel);
        dComIfGp_evmng_setGoal(local_c);
        break;
    }
    }
}
VERIFY(0x021FF210, &daNpc_Bm1_c::eInit_SET_PLYER_GOL_);

/* 021FF2B8 (unnamed by the matcher) */
void daNpc_Bm1_c::eInit_setLocFlag(be<s32>* i_param_1) {
    WWHD_FUNC(0x021FF2B8, void, this, i_param_1);
    m896 = false;
    if (i_param_1 == nullptr) {
        return;
    }
    switch ((u32)(s32)*i_param_1) {
    case 2:
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        break;
    case 1:
        m896 = true;
        break;
    }
}
VERIFY(0x021FF2B8, &daNpc_Bm1_c::eInit_setLocFlag);

/* 021FF2F8 (unnamed by the matcher) */
void daNpc_Bm1_c::eInit_setShapeAngleY(be<s32>* i_setShapeAngle, s16 i_shapeAngle) {
    WWHD_FUNC(0x021FF2F8, void, this, i_setShapeAngle, i_shapeAngle);
    mbSetShapeAngle = false;
    if (i_setShapeAngle == nullptr) {
        return;
    }
    bool set = *i_setShapeAngle == 1;
    mbSetShapeAngle = set;
    if (!set) {
        return;
    }
    shape_angle.y = i_shapeAngle;
}
VERIFY(0x021FF2F8, &daNpc_Bm1_c::eInit_setShapeAngleY);

/* 021FF328 */
void daNpc_Bm1_c::eInit_setEvTimer(be<s32>* i_param_1) {
    WWHD_FUNC(0x021FF328, void, this, i_param_1);
    m86C = 0;
    if (i_param_1 == nullptr) {
        return;
    }
    m86C = (s16)*i_param_1;
}
VERIFY(0x021FF328, &daNpc_Bm1_c::eInit_setEvTimer);

/* 021FF344 */
/* HD: the look angle starts as shape_angle.y (GameCube: uninitialised) */
void daNpc_Bm1_c::eInit_ATTENTION_(be<s32>* i_prm_0_p, be<s32>* i_prm_1_p, be<s32>* i_prm_2_p, cXyz* i_offset_p, be<s32>* i_angle_p,
                                   be<s32>* i_index_p, be<s32>* i_timer_p) {
    WWHD_FUNC(0x021FF344, void, this, i_prm_0_p, i_prm_1_p, i_prm_2_p, i_offset_p, i_angle_p, i_index_p, i_timer_p);
    if (i_prm_0_p == nullptr) {
        return;
    }
    s16 angle = shape_angle.y;
    switch ((u32)(s32)*i_prm_0_p) {
    case 0:
        break;
    case 1:
        mLookBackState = 1;
        angle = cLib_targetAngleY(&current.pos, &dComIfGp_getLinkPlayer()->current.pos);
        break;
    case 2:
        if (i_offset_p == nullptr) {
            return;
        }
        mLookBackState = 2;
        angle = cLib_targetAngleY(&current.pos, i_offset_p);
        break;
    case 3: {
        mLookBackState = 2;
        gabi::Local<cXyz> rel;
        eInit_calcRelativPos(rel, i_offset_p, i_angle_p);
        m82C.copy(*rel);
        angle = cLib_targetAngleY(&current.pos, &m82C);
        break;
    }
    case 4:
        if (i_angle_p == nullptr) {
            return;
        }
        mLookBackState = 3;
        angle = gabi::load<s16>(gabi::ea(i_angle_p) + 2); /* (s16)*i_angle_p */
        break;
    case 5: {
        if (mPathRun.mPath.get() == nullptr) {
            return;
        }
        u8 pointIndex = mPathRun.mIdx;
        if (i_index_p != nullptr) {
            pointIndex = gabi::load<u8>(gabi::ea(i_index_p) + 3);
        }
        gabi::Local<cXyz> pt;
        dNpc_PathRun_getPoint(&mPathRun, pt, pointIndex);
        mLookBackState = 2;
        m82C.copy(*pt);
        break;
    }
    case 6: {
        fopAc_ac_c* actor = searchByID(mPartnerProcID);
        if (actor == nullptr) {
            return;
        }
        mLookBackState = 2;
        m82C.copy(actor->eyePos);
        angle = cLib_targetAngleY(&current.pos, &m82C);
        break;
    }
    default:
        mLookBackState = 0;
        break;
    }
    eInit_setLocFlag(i_prm_1_p);
    eInit_setShapeAngleY(i_prm_2_p, angle);
    eInit_setEvTimer(i_timer_p);
}
VERIFY(0x021FF344, &daNpc_Bm1_c::eInit_ATTENTION_);

/* 021FF5D0 */
void daNpc_Bm1_c::eInit_WLK_(be<s32>* i_prm_0_p, be<f32>* i_speed_p, be<f32>* i_accel_p, cXyz* i_offset_p, be<s32>* i_angle_p,
                             be<s32>* i_index_p, be<s32>* i_timer_p) {
    WWHD_FUNC(0x021FF5D0, void, this, i_prm_0_p, i_speed_p, i_accel_p, i_offset_p, i_angle_p, i_index_p, i_timer_p);
    if (i_prm_0_p == nullptr) {
        return;
    }
    switch ((u32)(s32)*i_prm_0_p) {
    case 0:
        if (i_offset_p == nullptr) {
            return;
        }
        mTargetPos.x = i_offset_p->x;
        mTargetPos.y = i_offset_p->y;
        mTargetPos.z = i_offset_p->z;
        break;
    case 1: {
        gabi::Local<cXyz> rel;
        eInit_calcRelativPos(rel, i_offset_p, i_angle_p);
        mTargetPos.copy(*rel);
        break;
    }
    case 2: {
        if (mPathRun.mPath.get() == nullptr) {
            return;
        }
        u8 pointIdx = mPathRun.mIdx;
        if (i_index_p != nullptr) {
            pointIdx = gabi::load<u8>(gabi::ea(i_index_p) + 3);
        }
        gabi::Local<cXyz> pt;
        dNpc_PathRun_getPoint(&mPathRun, pt, pointIdx);
        mTargetPos.copy(*pt);
        break;
    }
    default:
        return;
    }
    eInit_setEvTimer(i_timer_p);
    mTargetFlySpeed = eInit_prmFloat(i_speed_p, gabi::load<f32>(l_HIO_prm(mType) + 0x3C)); /* m3C */
    mTargetFlyStep = eInit_prmFloat(i_accel_p, gabi::load<f32>(l_HIO_prm(mType) + 0x40));  /* m40 */
    f32 m44 = gabi::load<f32>(l_HIO_prm(mType) + 0x44);
    mLookBackState = 0;
    m858 = m44;
    setAnm_NUM(0xE, 1);
}
VERIFY(0x021FF5D0, &daNpc_Bm1_c::eInit_WLK_);

/* 021FF888 */
void daNpc_Bm1_c::event_actionInit(int arg0) {
    WWHD_FUNC(0x021FF888, void, this, arg0);
    cXyz* offset_p = (cXyz*)dComIfGp_evmng_getMySubstanceP(arg0, STR(0x100178F0) /* "Offst" */, 1);
    be<s32>* act_no_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, STR(0x100178F8) /* "ActNo" */, 3);
    be<s32>* prm_0_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, STR(0x10017900) /* "prm_0" */, 3);
    be<s32>* prm_1_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, STR(0x10017908) /* "prm_1" */, 3);
    be<s32>* prm_2_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, STR(0x10017910) /* "prm_2" */, 3);
    be<s32>* angle_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, STR(0x10017918) /* "Angle" */, 3);
    be<s32>* index_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, STR(0x10017920) /* "Index" */, 3);
    be<s32>* timer_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, STR(0x10017928) /* "Timer" */, 3);
    be<s32>* anmno_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(arg0, STR(0x10017930) /* "AnmNo" */, 3);
    be<f32>* speed_p = (be<f32>*)dComIfGp_evmng_getMySubstanceP(arg0, STR(0x10017938) /* "Speed" */, 0);
    be<f32>* accel_p = (be<f32>*)dComIfGp_evmng_getMySubstanceP(arg0, STR(0x10017940) /* "Accel" */, 0);
    be<f32>* spd_y_p = (be<f32>*)dComIfGp_evmng_getMySubstanceP(arg0, STR(0x10017948) /* "Spd_y" */, 0);
    be<f32>* acc_y_p = (be<f32>*)dComIfGp_evmng_getMySubstanceP(arg0, STR(0x10017950) /* "Acc_y" */, 0);
    if (act_no_p == nullptr) {
        return;
    }
    s8 actNo = (s8)*act_no_p;
    mActNo = actNo;
    switch ((u32)(s32)actNo) {
    case 4:
        eInit_DEL_ACTOR_();
        break;
    case 0x10:
        eInit_SET_NXT_PTH_INF_();
        break;
    case 0x14:
        eInit_INI_EVN_1_();
        break;
    case 0x15:
        eInit_MOV_PTH_POINT_(prm_0_p, anmno_p, index_p, angle_p);
        break;
    case 0x16:
    case 0x17:
        eInit_FLY_(prm_0_p, speed_p, spd_y_p, accel_p, acc_y_p);
        break;
    case 0x18:
        eInit_SET_PLYER_GOL_(prm_0_p, offset_p, angle_p);
        break;
    case 0x19:
        eInit_ATTENTION_(prm_0_p, prm_1_p, prm_2_p, offset_p, angle_p, index_p, timer_p);
        break;
    case 0x1A:
        eInit_SET_ANM_(anmno_p);
        break;
    case 0x1B:
        eInit_WLK_(prm_0_p, speed_p, accel_p, offset_p, angle_p, index_p, timer_p);
        break;
    }
}
VERIFY(0x021FF888, &daNpc_Bm1_c::event_actionInit);

/* 021FFC48 */
void daNpc_Bm1_c::cut_init_360_TRN(int i_staffIdx) {
    WWHD_FUNC(0x021FFC48, void, this, i_staffIdx); /* the argument is unused */
    setAnm_NUM(0x13, 1);
    mLookBackState = 0;
}
VERIFY(0x021FFC48, &daNpc_Bm1_c::cut_init_360_TRN);

/* 021FFC84 (unnamed by the matcher) */
bool daNpc_Bm1_c::eMove_FLY_() {
    WWHD_FUNC(0x021FFC84, bool, this);
    m88A = 0;
    return m8F4 == 0;
}
VERIFY(0x021FFC84, &daNpc_Bm1_c::eMove_FLY_);

/* 021FFCA0 (unnamed by the matcher) */
u8 daNpc_Bm1_c::eMove_KMA_FLY_() {
    WWHD_FUNC(0x021FFCA0, u8, this);
    return m88A;
}
VERIFY(0x021FFCA0, &daNpc_Bm1_c::eMove_KMA_FLY_);

/* 021FFCA8 (unnamed by the matcher) */
u32 daNpc_Bm1_c::eMove_ATTENTION_() {
    WWHD_FUNC(0x021FFCA8, u32, this);
    if (m86C >= 0) {
        return cLib_calcTimer_g(&m86C) == 0;
    }
    return (u8)(m_jnt.mbTrn ^ 1); /* m_jnt.trnChk() == 0 */
}
VERIFY(0x021FFCA8, &daNpc_Bm1_c::eMove_ATTENTION_);

/* 021FFCFC */
bool daNpc_Bm1_c::eMove_WLK_() {
    WWHD_FUNC(0x021FFCFC, bool, this);
    bool is_terminate = cLib_calcTimer_g(&m86C) == 0;
    if (is_terminate) {
        setAnm_NUM(4, 1);
        speedF = 0.0f;
        mTargetFlySpeed = 0.0f;
    }
    return is_terminate;
}
VERIFY(0x021FFCFC, &daNpc_Bm1_c::eMove_WLK_);

/* 021FFD64 */
/* the eMove_* results are returned as is: typed u32 */
u32 daNpc_Bm1_c::event_action() {
    WWHD_FUNC(0x021FFD64, u32, this);
    switch ((u32)(s32)mActNo) {
    case 0x16:
        return gabi::call<u32>(0x021FFC84, this); /* eMove_FLY_() */
    case 0x17:
        return gabi::call<u32>(0x021FFCA0, this); /* eMove_KMA_FLY_() */
    case 0x19:
        return gabi::call<u32>(0x021FFCA8, this); /* eMove_ATTENTION_() */
    case 0x1B:
        return gabi::call<u32>(0x021FFCFC, this); /* eMove_WLK_() */
    default:
        return 1;
    }
}
VERIFY(0x021FFD64, &daNpc_Bm1_c::event_action);

/* 021FFDCC */
bool daNpc_Bm1_c::cut_move_360_TRN() {
    WWHD_FUNC(0x021FFDCC, bool, this);
    if (mbMorfAnimStopped) {
        current.angle.y = current.angle.y + -0x8000;
        setAnm_NUM(4, 1);
        mpMorf->setMorf(0.0f);
        if (mbHasArms) {
            mpArmMorf->setMorf(0.0f);
        } else {
            mpWingMorf->setMorf(0.0f);
        }
        mpHeadMorf->setMorf(0.0f);
        return true;
    }
    return false;
}
VERIFY(0x021FFDCC, &daNpc_Bm1_c::cut_move_360_TRN);

/* 021FFED0 */
void daNpc_Bm1_c::privateCut(int arg0) {
    WWHD_FUNC(0x021FFED0, void, this, arg0);
    /* static char* a_cut_tbl[] = {"ACTION", "360_TRN"} (.data 0x101BC98C) */
    if (arg0 == -1) {
        return;
    }
    s8 idx = gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), arg0, 0x101BC98C, 2, 1, 0); /* getMyActIdx */
    mActionIndex = idx;
    u32 evmng = gabi::ea(dComIfGp_getPEvtManager());
    if (idx == -1) {
        gabi::call(0x02543280, evmng, arg0); /* dComIfGp_evmng_cutEnd */
        return;
    }
    if (gabi::call<BOOL>(0x025447C8, evmng, arg0)) { /* dComIfGp_evmng_getIsAddvance */
        switch ((u32)(s32)mActionIndex) {
        case 0:
            event_actionInit(arg0);
            break;
        case 1:
            cut_init_360_TRN(arg0);
            break;
        }
    }
    u32 cVar3;
    switch ((u32)(s32)mActionIndex) {
    case 0:
        cVar3 = event_action();
        break;
    case 1:
        cVar3 = cut_move_360_TRN();
        break;
    default:
        cVar3 = 1;
        break;
    }
    if (cVar3) {
        dComIfGp_evmng_cutEnd(arg0);
    }
}
VERIFY(0x021FFED0, &daNpc_Bm1_c::privateCut);

/* 021FE0B8 */
static cPhs_State daNpc_Bm1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021FE0B8, cPhs_State, i_this);
    return ((daNpc_Bm1_c*)i_this)->_create();
}
VERIFY(0x021FE0B8, daNpc_Bm1_Create);

/* 021FE214 */
static BOOL daNpc_Bm1_Delete(daNpc_Bm1_c* i_this) {
    WWHD_FUNC(0x021FE214, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x021FE214, daNpc_Bm1_Delete);
