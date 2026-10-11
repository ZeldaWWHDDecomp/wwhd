/* daNpc_P1_c (Gonzo, Senza, Nudge: Tetra's pirates), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_p1: daNpc_P1_c derives from fopAc_ac_c
 * directly (+0x11C). The two pointers to member function are 8 bytes each (GHS) instead of 12, so
 * mPhs..mpTexture are +0x114; mDoExt_btpAnm grew from 0x14 to 0x74, so everything from mBlinkFrame
 * on is +0x174 (mShadowId keeps its slot, HD draws no blob shadow); dBgS_ObjAcch is 0x1C4 and
 * dBgS_AcchCir 0x40, so mAcchCir is +0x178 and the members after it +0x174 again; dNpc_HeadAnm_c
 * is 0x20 (GameCube 0x24), so the members after it are +0x170. Size 0x8B4 (GameCube 0x744). */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/SSystem/c_lib.h"
#include "gc_api/f_op/f_op_actor.h"
#include "JSystem/J3DGraph.h"
#include "d/actor/d_a_npc_ba1.h" /* ProcFunc_l (GHS pointer to member function, measured) */

/* daNpc_P1_childHIO_c (HD: vtable at the end, 0x30) */
struct daNpc_P1_childHIO_c {
    /* 0x00 */ be<s8> unk4;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<f32> mAttnYPosOffset;
    /* 0x08 */ be<s16> mMaxHeadX;
    /* 0x0A */ be<s16> mMinHeadX;
    /* 0x0C */ be<s16> mMaxBackboneX;
    /* 0x0E */ be<s16> mMinBackboneX;
    /* 0x10 */ be<s16> mMaxHeadY;
    /* 0x12 */ be<s16> mMinHeadY;
    /* 0x14 */ be<s16> mMaxBackboneY;
    /* 0x16 */ be<s16> mMinBackboneY;
    /* 0x18 */ be<s16> mMaxTurnStep;
    /* 0x1A */ be<s16> mLookBackTargetY;
    /* 0x1C */ be<s8> mUnused20;
    /* 0x1D */ u8 _1D[3];
    /* 0x20 */ be<f32> mMaxTalkDist;
    /* 0x24 */ be<f32> mMorfBackup;
    /* 0x28 */ be<f32> mUnused2C;
    /* 0x2C */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_P1_childHIO_c, 0x30);

/* daNpc_P1_HIO_c (HD: vtable at the end, 0x9C) */
struct daNpc_P1_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<s32> m8;
    /* 0x08 */ daNpc_P1_childHIO_c children[3];
    /* 0x98 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_P1_HIO_c, 0x9C);

struct daNpc_P1_c : fopAc_ac_c {
    enum { TYPE_P1A_e = 0, TYPE_P1B_e = 1, TYPE_P1C_e = 2 };
    enum { ACTION_STARTING_e = 0, ACTION_ONGOING_e = 1, ACTION_ENDING_e = -1 };

    void setAnimFromMsg();
    BOOL setAnm(int, f32);
    BOOL normalAction(void*);
    BOOL confuseAction(void*);
    BOOL talkAction(void*);
    BOOL p1c_speakAction(void*);
    BOOL speakAction(void*);
    BOOL explainAction(void*);
    u32 getNextMsgNo(int);
    BOOL playTexPatternAnm();
    void demo_end_init();
    BOOL demo_move();
    BOOL event_move();
    BOOL evn_setAnm_init(int);
    BOOL evn_talk_init(int);
    BOOL evn_talk();
    BOOL minigameExplainCut();
    BOOL privateCut();
    BOOL setAttentionPos(cXyz*);
    cPhs_State _create();
    BOOL CreateHeap();
    BOOL _delete();
    u32 getKajiID();
    BOOL kaji_anm();
    BOOL _execute();
    BOOL _draw();
    BOOL lookBack();

    /* 0x3AC */ ProcFunc_l mActionFunc;
    /* 0x3B4 */ ProcFunc_l mPrevAction;
    /* 0x3BC */ request_of_phase_process_class mPhs;
    /* 0x3C4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3C8 */ u8 _3C8[0x3D4 - 0x3C8];
    /* 0x3D4 */ gptr<J3DModel> mpHeadModel;
    /* 0x3D8 */ gptr<J3DModel> mpDoraModel;
    /* 0x3DC */ gptr<J3DAnmTexPattern> mpTexture;
    /* 0x3E0 */ mDoExt_btpAnm mBtp; /* mDoExt_btpAnm (HD 0x74); its J3DAnmTexPattern* at +0x10 */
    /* 0x454 */ be<u8> mBlinkFrame;
    /* 0x455 */ u8 _455;
    /* 0x456 */ be<s16> mBlinkTimer;
    /* 0x458 */ be<u32> mShadowId; /* unused in HD */
    /* 0x45C */ dBgS_ObjAcch mObjAcch;
    /* 0x620 */ dBgS_AcchCir mAcchCir;
    /* 0x660 */ dCcD_Stts mStts;
    /* 0x69C */ dCcD_Cyl mCyl;
    /* 0x7CC */ u8 pad7CC;
    /* 0x7CD */ be<s8> mActionStatus;
    /* 0x7CE */ be<s8> m65A;
    /* 0x7CF */ be<u8> mType;
    /* 0x7D0 */ be<u8> mParam;
    /* 0x7D1 */ u8 _7D1[3];
    /* 0x7D4 */ be<u32> mCurrMesg;
    /* 0x7D8 */ be<u32> mPrevMesg;
    /* 0x7DC */ be<s32> mAnmNum;
    /* 0x7E0 */ be<s32> m66C;
    /* 0x7E4 */ be<u8> m670;
    /* 0x7E5 */ be<u8> m671;
    /* 0x7E6 */ u8 _7E6[2];
    /* 0x7E8 */ be<u32> mKajiId;
    /* 0x7EC */ be<s16> mKajiTimer;
    /* 0x7EE */ be<s16> filler;
    /* 0x7F0 */ dNpc_JntCtrl_c m_jnt;
    /* 0x824 */ dNpc_EventCut_c mEventCut6B0;
    /* 0x890 */ dNpc_HeadAnm_c mHeadAnm;
    /* 0x8B0 */ be<s8> m_handR_jnt_num;
    /* 0x8B1 */ be<u8> mbAttentionFlag;
    /* 0x8B2 */ be<s16> mMaxLookVel;
};
WWHD_OFFSET(daNpc_P1_c, mPhs, 0x3BC);
WWHD_OFFSET(daNpc_P1_c, mBlinkFrame, 0x454);
WWHD_OFFSET(daNpc_P1_c, mObjAcch, 0x45C);
WWHD_OFFSET(daNpc_P1_c, mStts, 0x660);
WWHD_OFFSET(daNpc_P1_c, mCyl, 0x69C);
WWHD_OFFSET(daNpc_P1_c, mActionStatus, 0x7CD);
WWHD_OFFSET(daNpc_P1_c, m_jnt, 0x7F0);
WWHD_OFFSET(daNpc_P1_c, mEventCut6B0, 0x824);
WWHD_OFFSET(daNpc_P1_c, mHeadAnm, 0x890);
WWHD_SIZE(daNpc_P1_c, 0x8B4);

u32 p1_msgGetStatus(msg_class*);
void p1_msgSetStatus(msg_class*, u32);
u32 p1_msgSet(msg_class*, u32, cXyz*);
s32 p1ActionIndex(dEvent_manager_c*, s32, u32, s32, s32, s32);
void p1BtpEntry(mDoExt_btpAnm*, J3DModelData*, u32);
void p1RegisterFigure(u32, daNpc_P1_c*, cXyz*, s32, f32, f32, f32);
u32 p1DemoActor(u32, u32);
J3DAnmTexPattern* p1DemoBtp(u32, const char*);
s32 p1c_btpInit(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32);
u32 p1GroundSound(cXyz*, f32);
BOOL p1DemoData(daNpc_P1_c*, s32, mDoExt_McaMorf*, const char*, s32, s32, u32, s32);
s16 p1BlinkTimer(be<s16>*);
BOOL p1EndCheck(dEvent_manager_c*, const char*);
u32 p1MaterialSound(dBgS*, void*);
s8 p1RoomId(dBgS*, void*);
u8 p1PolyColor(dBgS*, void*);
void p1ConstructArray(daNpc_P1_childHIO_c*, s32, s32, u32);
void p1CircleDtor(void*, s32);
void p1AcchDtor(dBgS_ObjAcch*, s32);
void p1ActorDtor(daNpc_P1_c*, s32);
u32 p1JointNameHeader(J3DModelData*);
s8 p1c_getIndex(u32, const char*);
u32 p1JointRegistry(J3DModelData*);
void p1c_setActorInfo(dNpc_EventCut_c*, const char*, fopAc_ac_c*);
void p1c_JntCtrl_ct(void*);
void p1c_EventCut_ct(void*);
void p1c_btpAnm_ct(void*);
void p1LookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s32, s32, u32);
void p1_cLib_addCalcPos2(cXyz*, cXyz*, f32, f32);
BOOL p1OrderOtherEvent(fopAc_ac_c*, u32, u32, u32);
void p1NextStage(u32, s32, s32, s32, f32, u32, s32, u32);
namespace p1_cpp {
// HD: P1 resource indices follow the expanded HD archive.
enum {
    dRes_INDEX_P1_BCK_WAIT_e = 40,
    dRes_INDEX_P1_BCK_TALK_e = 35,
    dRes_INDEX_P1_BCK_TALK02_e = 36,
    dRes_INDEX_P1_BCK_TALK03_e = 37,
    dRes_INDEX_P1_BCK_WAIT02_e = 41,
    dRes_INDEX_P1_BCK_OMOKJ_e = 34,
    dRes_INDEX_P1_BCK_TORIKJ_e = 39,
    dRes_INDEX_P1_BCK_ANGRY_e = 26,
    dRes_INDEX_P1_BCK_TALK04_e = 38,
    dRes_INDEX_P1_BCK_CHECK01_e = 31,
    dRes_INDEX_P1_BCK_CHECK02_e = 32,
    dRes_INDEX_P1_BCK_LOOK_e = 33,
    dRes_INDEX_P1_BCK_C_WAIT_e = 30,
    dRes_INDEX_P1_BCK_C_TALK01_e = 28,
    dRes_INDEX_P1_BCK_C_TALK02_e = 29,
    dRes_INDEX_P1_BCK_C_STOP_e = 27,
    dRes_INDEX_P1_BDL_P1_e = 0x11,
    dRes_INDEX_P1_BDL_P1B_HD_e = 7,
    dRes_INDEX_P1_BDL_P1C_HD_e = 8,
    dRes_INDEX_P1_BDL_P1A_HEAD_e = 0x15,
    dRes_INDEX_P1_BDL_P1B_HEAD_e = 0x16,
    dRes_INDEX_P1_BDL_P1C_HEAD_e = 0x17,
    dRes_INDEX_P1_BTP_MABA_A_e = 0xB,
    dRes_INDEX_P1_BTP_MABA_B_e = 0xC,
    dRes_INDEX_P1_BTP_MABA_C_e = 0xD,
    dRes_INDEX_P1_BTP_P1A_DUMMY_TEX_ALL_e = 0xE,
    dRes_INDEX_P1_BDL_DORA_STICK_e = 0x12,
    DSNAP_TYPE_NPC_P1A = 115,
    DSNAP_TYPE_NPC_P1B = 116,
    DSNAP_TYPE_NPC_P1C = 117,
    fopMsgStts_MSG_DISPLAYED_e = 0xE,
    fopMsgStts_MSG_CONTINUES_e = 0xF,
    fopMsgStts_MSG_ENDS_e = 0x10,
    fopMsgStts_BOX_CLOSED_e = 0x12,
    fopMsgStts_MSG_DESTROYED_e = 0x13,
};

struct VectorWords {
    be<u32> x, y, z;
};
inline VectorWords* words(cXyz* p) {
    return reinterpret_cast<VectorWords*>(p);
}
struct CameraFlags {
    u8 _00[0x348];
    be<u8> mSkipPosition, mSkipTarget, mSkipFov;
};
inline CameraFlags* camera(u32 p) {
    return gabi::at<CameraFlags>(p);
}
struct MessageManagerView {
    u8 _00[0x948];
    be<s32> mSelectNum;
};
inline MessageManagerView* message(msg_class* p) {
    return reinterpret_cast<MessageManagerView*>(p);
}
struct PirateShipView {
    u8 _00[0x490];
    be<u32> mKajiId;
};
struct KajiView {
    u8 _00[0x3B8];
    gptr<mDoExt_McaMorf> mpMorf;
};
struct AnimationVtable {
    u8 _00[0x14];
    be<u32> mGetFrameMax;
};
struct AnimationView {
    u8 _00[4];
    gptr<AnimationVtable> mpVtable;
};
inline s32 frameMax(u32 p) {
    auto* v = gabi::at<AnimationView>(p);
    return gabi::call_ptr<s32>(v->mpVtable->mGetFrameMax, p);
}
struct AcchVtables {
    u8 _00[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mActorVtable;
};
inline AcchVtables* acch(dBgS_ObjAcch* p) {
    return reinterpret_cast<AcchVtables*>(p);
}
inline void* circle(dBgS_AcchCir* p) {
    return gabi::at<void>(gabi::ea(p) + 0x14);
}
struct CylinderSource {
    u8 _00[0x44];
};
struct JointNameHeader {
    u8 _00[0x10];
    be<u32> mNameOffset;
};
struct JointRegistry {
    u8 _00[8];
    be<u16> mJointCount;
};
struct JointNode {
    u8 _00[8];
    be<u32> mCallback;
    u8 _0C[0x10];
};
struct ModelDataView {
    u8 _00[4];
    be<u32> mJointCount;
    gptr<JointNode> mpJoints;
};
struct DemoActorView {
    u8 _00[4];
    be<u16> mFlags;
};
template <class T> inline T& entry(T* p, s32 i) {
    return *gabi::at<T>(gabi::ea(p) + (u32)i * sizeof(T));
}
WWHD_SIZE(VectorWords, 12);
WWHD_OFFSET(CameraFlags, mSkipFov, 0x34A);
WWHD_OFFSET(MessageManagerView, mSelectNum, 0x948);
WWHD_OFFSET(PirateShipView, mKajiId, 0x490);
WWHD_OFFSET(KajiView, mpMorf, 0x3B8);
WWHD_SIZE(JointNode, 0x1C);
WWHD_SIZE(CylinderSource, 0x44);
WWHD_OFFSET(AcchVtables, mActorVtable, 0x20);
} // namespace p1_cpp

extern daNpc_P1_HIO_c& l_HIO;
extern be<u32>& l_msgId;
extern gptr<msg_class>& l_messageManager;
// HD: exact ABI adapters shared by the five source files of this translation unit.
#define P1_SAFESTRING_VTBL 0x1001F87Cu /* this TU's sead::SafeString vtable */
#define P1_VTBL 0x1001F8D4u            /* daNpc_P1_c vtable (HD virtual destructor) */
#define P1_AAB_VTBL 0x1001F894u        /* this TU's cM3dGAab vtable copy */
enum { fpcNm_Obj_Pirateship_e = 0x39, fpcNm_Kaji_e = 0x3C };

/* file statics: l_HIO (0x10467F40); l_msgId (0x10467F20; HD: l_msg is gone, the message is
 * reached through the HD message manager) */

static inline daNpc_P1_childHIO_c* p1_child(u32 type) {
    return gabi::at<daNpc_P1_childHIO_c>(gabi::ea(&l_HIO.children[0]) + type * sizeof(daNpc_P1_childHIO_c));
}

// HD: message manager replaces the GameCube l_msg object.
/* HD message manager (*(0x101F4B5C)): 025F795C status, 025F74D0 setStatus, 025F7DB0
 * messageSet(msgNo, cXyz* pos) -> id; its select number (GameCube msg_class::mSelectNum) at +0x948 */

/* dComIfGp_onCameraAttentionStatus(0, 4) / off: play + 0x5B00 */
static inline void p1_onCameraAttentionStatus4() {
    auto* info = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    info->play.mCameraInfo[0].mCameraAttentionStatus |= 4;
}
static inline void p1_offCameraAttentionStatus4() {
    auto* info = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    info->play.mCameraInfo[0].mCameraAttentionStatus &= ~4u;
}
/* dComIfGp_getCamera(0)->mCamera.SkipSmoother() (inline: three flags of the camera at
 * *(play + 0x5AF8)); the store order differs per site */
static inline u32 p1_camera0() {
    return gabi::ea(dComIfGp_getCamera(0));
}

static inline void* p1_getRes(const char* arc, s32 idx) {
    return dComIfG_getObjectRes(arc, idx, P1_SAFESTRING_VTBL);
}
static inline s32 p1_getMyActIdx(s32 staffId, u32 tbl, s32 n, s32 force, s32 nameType) {
    return p1ActionIndex(dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* mDoExt_McaMorf::checkFrame(getEndFrame() - d): the end frame is the frame control's s16 end */
static inline BOOL p1_checkEnd(mDoExt_McaMorf* m, f32 d) {
    f32 e = (f32)(s32)m->mFrameCtrl.mEnd;
    return m->mFrameCtrl.checkPass(e - d);
}
/* |(a - b).xz| through cXyz::operator- and PSVECSquareMag/sqrtf on {x, 0, z} (word copies) */
static inline f32 p1_absXZ_mi(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> xz;
    cXyz_mi(a, diff.get(), b);
    xz->y = 0.0f;
    p1_cpp::words(xz.get())->x = (u32)p1_cpp::words(diff.get())->x;
    p1_cpp::words(xz.get())->z = (u32)p1_cpp::words(diff.get())->z;
    f32 sq = PSVECSquareMag(xz.get());
    return std_sqrtf(sq);
}
static inline void p1_copy12(u32 dst, u32 src) {
    auto* s = gabi::at<p1_cpp::VectorWords>(src);
    auto* d = gabi::at<p1_cpp::VectorWords>(dst);
    u32 x = s->x, y = s->y, z = s->z;
    d->x = x;
    d->y = y;
    d->z = z;
}
/* matrix assignment: GHS loads all twelve values into FPRs (lfs), then stores them (stfs); the
 * recompiled lfs quiets a signalling NaN here, so the copy goes through f32 */
static inline void p1_mtx_copy(u32 dst, u32 src) {
    J3DModel::mtx_assign(gabi::at<Mtx34>(dst), gabi::at<Mtx34>(src));
}
/* J3DModel::getAnmMtx (HD: joint matrix block at +0x2C, dirty flag 0x10 at +4, matrices at +0x10) */
static inline u32 p1_getAnmMtx(J3DModel* model, s32 jntNo) {
    return gabi::ea(model->getAnmMtx(jntNo));
}
/* GHS pointer to member function call */
static inline void p1_pmf_call(void* self, ProcFunc_l* pmf, void* arg) {
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
/* checkAction(&daNpc_P1_c::fn) */
static inline bool p1_checkAction(daNpc_P1_c* a, u32 fn) {
    return a->mActionFunc.i == -1 && a->mActionFunc.d == 0 && a->mActionFunc.f == fn;
}
/* setAction(&daNpc_P1_c::fn, NULL, 0) (inline) */
static inline void p1_setAction(daNpc_P1_c* a, u32 fn) {
    ProcFunc_l* cur = &a->mActionFunc;
    s16 i = cur->i;
    if (i == -1 && cur->d == 0 && cur->f == fn)
        return;
    if (i != 0) {
        a->mActionStatus = -1; /* ACTION_ENDING */
        p1_pmf_call(a, cur, nullptr);
    }
    s16 oi = cur->i;
    s16 od = cur->d;
    u32 of = cur->f;
    a->mPrevAction.d = od;
    a->mPrevAction.i = oi;
    a->mPrevAction.f = of;
    cur->d = 0;
    cur->i = -1;
    cur->f = fn;
    a->mActionStatus = 0; /* ACTION_STARTING */
    gabi::call_ptr<BOOL>(cur->f, a, (u32)0);
}
/* setAction(mPrevAction, NULL, 0) (inline, generic pointer to member) */
static inline void p1_setActionPrev(daNpc_P1_c* a) {
    ProcFunc_l* cur = &a->mActionFunc;
    s16 ti = a->mPrevAction.i;
    u32 tf = a->mPrevAction.f;
    s16 td = a->mPrevAction.d;
    s16 ci = cur->i;
    if (ci == ti && (ci == 0 || (cur->d == td && cur->f == tf)))
        return;
    if (ci != 0) {
        a->mActionStatus = -1;
        p1_pmf_call(a, cur, nullptr);
    }
    s16 oi = cur->i;
    s16 od = cur->d;
    u32 of = cur->f;
    a->mPrevAction.d = od;
    a->mPrevAction.i = oi;
    a->mPrevAction.f = of;
    cur->d = td;
    cur->i = ti;
    cur->f = tf;
    a->mActionStatus = 0;
    p1_pmf_call(a, cur, nullptr);
}

extern p1_cpp::CylinderSource& l_cyl_src;
static inline u8 p1_eventMode() {
    return dComIfGp_getEvent()->mMode;
}

static inline bool p1_isStage_Demo17() {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> b;
    a->__vtbl = P1_SAFESTRING_VTBL;
    a->mStringTop = 0x1001F9D0; /* "Demo17" */
    b->mStringTop = 0x1047E6B8;
    b->__vtbl = P1_SAFESTRING_VTBL;
    gabi::call(0x022B5AE8, a.get());
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
    u32 s2 = b->mStringTop;
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

static inline bool p1c_isStartStage(u32 str) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> b;
    a->mStringTop = str;
    a->__vtbl = P1_SAFESTRING_VTBL;
    u32 play = dComIfGp_ea();
    b->__vtbl = P1_SAFESTRING_VTBL;
    b->mStringTop = play + 0x5134;
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
    u32 s2 = b->mStringTop;
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

static inline bool p1c_isOutsetLayerA(daNpc_P1_c* a) {
    if (!p1c_isStartStage(0x1001F92C /* "sea" */))
        return false;
    if (a->current.roomNo != 0x2C /* dIsleRoom_OutsetIsland_e */)
        return false;
    return dComIfGp_getStartStageLayer() == 0xA;
}

static inline void p1_skipSmoother_a(u32 cam) {
    p1_cpp::camera(cam)->mSkipFov = 1;
    p1_cpp::camera(cam)->mSkipPosition = 1;
    p1_cpp::camera(cam)->mSkipTarget = 1;
}

static inline f32 p1_absXZ_f(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> xz;
    cXyz_mi(a, diff.get(), b);
    f32 x = diff->x;
    f32 z = diff->z;
    xz->x = x;
    xz->y = 0.0f;
    xz->z = z;
    f32 sq = PSVECSquareMag(xz.get());
    return std_sqrtf(sq);
}

static inline mDoExt_McaMorf* p1_kajiMorf(fopAc_ac_c* kaji) {
    return reinterpret_cast<p1_cpp::KajiView*>(kaji)->mpMorf.get();
}

static inline void p1_kajiSetAnm(fopAc_ac_c* kaji, s32 i_anm, f32 frame) {
    s32 bckIdx, basIdx;
    switch ((u32)i_anm) {
    case 0:
        bckIdx = 0xE;
        basIdx = 8;
        break; /* KJ_WAIT */
    case 1:
        bckIdx = 0xC;
        basIdx = 6;
        break; /* KJ_OMO */
    case 2:
        bckIdx = 0xD;
        basIdx = 7;
        break; /* KJ_TORI */
    case 3:
        bckIdx = 0xB;
        basIdx = 5;
        break; /* KJ_ANG */
    default:
        return;
    }
    J3DAnmTransform* bck = (J3DAnmTransform*)p1_getRes(gabi::at<const char>(0x1001F874), bckIdx);
    void* bas = p1_getRes(gabi::at<const char>(0x1001F874), basIdx);
    p1_kajiMorf(kaji)->setAnm(bck, 2 /* EMode_LOOP */, 0.0f, 1.0f, 0.0f, -1.0f, bas);
    if (!(frame < 0.0f)) {
        p1_kajiMorf(kaji)->setFrame(frame);
    }
}

// HD: GHS direct member-pointer targets shared by this split translation unit.
enum : u32 {
    P1_normalAction = 0x022B43EC,
    P1_confuseAction = 0x022B4764,
    P1_talkAction = 0x022B48D0,
    P1_speakAction = 0x022B4E54,
    P1_p1c_speakAction = 0x022B53A0,
    P1_explainAction = 0x022B5744,
};

WWHD_SIZE(p1_cpp::CameraFlags, 0x34B);
WWHD_SIZE(p1_cpp::MessageManagerView, 0x94C);
WWHD_SIZE(p1_cpp::PirateShipView, 0x494);
WWHD_SIZE(p1_cpp::KajiView, 0x3BC);
WWHD_OFFSET(p1_cpp::AnimationVtable, mGetFrameMax, 0x14);
WWHD_SIZE(p1_cpp::AnimationVtable, 0x18);
WWHD_OFFSET(p1_cpp::AnimationView, mpVtable, 4);
WWHD_SIZE(p1_cpp::AnimationView, 8);
WWHD_SIZE(p1_cpp::AcchVtables, 0x24);
WWHD_OFFSET(p1_cpp::JointNameHeader, mNameOffset, 0x10);
WWHD_SIZE(p1_cpp::JointNameHeader, 0x14);
WWHD_OFFSET(p1_cpp::JointRegistry, mJointCount, 8);
WWHD_SIZE(p1_cpp::JointRegistry, 0xA);
WWHD_OFFSET(p1_cpp::ModelDataView, mpJoints, 8);
WWHD_SIZE(p1_cpp::ModelDataView, 0xC);
WWHD_OFFSET(p1_cpp::DemoActorView, mFlags, 4);
WWHD_SIZE(p1_cpp::DemoActorView, 6);
