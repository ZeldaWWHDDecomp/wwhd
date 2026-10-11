/* daNpc_Ac1_c (Prince Komali after Dragon Roost Cavern), WWHD layout.
 *
 * GameCube -> WWHD, measured from the verified functions of d_a_npc_ac1 (all GameCube functions
 * are "Nonmatching" stubs, so the names below follow the GameCube header's offsets and the
 * usage in WWHD's code):
 * - fopNpc_npc_c is 0x7DC (shared d/d_npc.h), so the members start at 0x7DC (GameCube 0x6C4,
 *   +0x118).
 * - the GameCube word at 0x6D0 (mShadowID in the sibling NPCs) is gone: m_handR_jnt_num and the
 *   item model are at 0x7E8/0x7EC (GameCube 0x6D0/0x6D4 area), +0x114 from the tex pattern on.
 * - mDoExt_btpAnm grew from 0x14 to 0x74, so everything after it is +0x174; the pointer to
 *   member mCurrProcFunc is 8 bytes (GHS) instead of 12, so from the matrices on +0x170.
 * - size 0x9EC (profile; GameCube 0x87C + 0x170). */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"

/* GHS pointer to member function (8 bytes): this adjustment, virtual index (0: null, < 0: not
 * virtual), then the function address, or (virtual) the vtable pointer's offset at +6 */
struct Ac1ActionFunc {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
WWHD_SIZE(Ac1ActionFunc, 8);

struct daNpc_Ac1_c : fopNpc_npc_c {
    /* HD: 0x14 bytes (the GameCube struct is empty in the decompilation) */
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;
        /* 0x01 */ be<s8> mBtpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
        /* 0x10 */ be<s32> mbArm; /* 1: the arm animation set (arm morf), else the wing set */
    };

    void nodeWngControl(J3DNode*, J3DModel*);
    void nodeArmControl(J3DNode*, J3DModel*);
    void nodeAc1Control(J3DNode*, J3DModel*);
    J3DModelData* create_Anm();
    int btpNum_toResID(int);
    bool setBtp(u32, int);
    u32 iniTexPttrnAnm(u32);
    J3DModelData* create_wng_Anm();
    J3DModelData* create_arm_Anm();
    bool create_itm_Mdl();
    BOOL CreateHeap();
    bool charDecide(int);
    BOOL set_action(Ac1ActionFunc*, void*);
    bool init_AC1_0();
    void plyTexPttrnAnm();
    void setAttention(u32);
    void setMtx(u32);
    bool createInit();
    cPhs_State _create();
    BOOL _delete();
    void checkOrder();
    u8 demo();
    s32 isEventEntry();
    void event_actionInit(int);
    bool event_action();
    void privateCut(int);
    void lookBack();
    void event_proc(int);
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    int anmNum_toResID(int);
    int armAnmNum_toResID(int);
    u32 wingAnmNum_toResID(int);
    u32 setAnm_tex(s8);
    BOOL setAnm_anm(anm_prm_c*);
    bool setAnm();
    void setAnm_ATR(int);
    void chg_anmAtr(u8);
    void anmAtr(u16);
    bool chk_talk();
    u8 chk_partsNotMove();
    u16 next_msgStatus(be<u32>*);
    u32 getBitMask();
    u32 getMsg_AC1_0();
    u32 getMsg();
    u8 chkAttention();
    void endEvent();
    void setStt(s8);
    BOOL wait_1();
    BOOL talk_1();
    BOOL wait_action1(void*);

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m6CC;
    /* 0x7E5 */ be<s8> m6CD;
    /* 0x7E6 */ be<s8> m6CE;
    /* 0x7E7 */ be<s8> m6CF;
    /* 0x7E8 */ be<s8> m_handR_jnt_num;
    /* 0x7E9 */ u8 _7E9[3];
    /* 0x7EC */ gptr<J3DModel> m6D4;         /* GameCube m6D4 */
    /* 0x7F0 */ gptr<J3DAnmTexPattern> m6DC; /* GameCube m6DC */
    /* 0x7F4 */ u8 mBtpAnim[0x74];           /* mDoExt_btpAnm (HD 0x74) */
    /* 0x868 */ be<u8> m6F4;                 /* GameCube m6F4 */
    /* 0x869 */ u8 _869;
    /* 0x86A */ be<s16> mBlinkTimer;
    /* 0x86C */ u8 _86C[4];
    /* 0x870 */ gptr<mDoExt_McaMorf> m6FC; /* GameCube m6FC */
    /* 0x874 */ be<s8> m700;               /* "wingL_loc" */
    /* 0x875 */ be<s8> m701;               /* "wingR_loc" */
    /* 0x876 */ u8 _876[6];
    /* 0x87C */ gptr<mDoExt_McaMorf> m708; /* GameCube m708 */
    /* 0x880 */ be<s8> m_armL_loc_jnt_num; /* "armL_loc" */
    /* 0x881 */ be<s8> m_armR_loc_jnt_num; /* "armR_loc" */
    /* 0x882 */ u8 _882[2];
    /* 0x884 */ Ac1ActionFunc mCurrProcFunc;
    /* 0x88C */ Mtx34 mArmLMtx;            /* the body's armL joint matrix */
    /* 0x8BC */ Mtx34 mArmRMtx;            /* the body's armR joint matrix */
    /* 0x8EC */ dNpc_PathRun_c mPathRun;   /* GameCube 0x77C */
    /* 0x8F4 */ dNpc_EventCut_c mEventCut; /* hides fopNpc_npc_c::mEventCut (as on GameCube) */
    /* 0x960 */ u8 _960[4];
    /* 0x964 */ cXyz m7F4;  /* GameCube m7F4 */
    /* 0x970 */ csXyz m800; /* GameCube m800 */
    /* 0x976 */ u8 _976[2];
    /* 0x978 */ cXyz mTransformedEyePos;
    /* 0x984 */ cXyz m814; /* GameCube m814: the position at creation */
    /* 0x990 */ u8 _990[0x99C - 0x990];
    /* 0x99C */ be<f32> m82C; /* GameCube m82C */
    /* 0x9A0 */ u8 _9A0[0x9B4 - 0x9A0];
    /* 0x9B4 */ be<s16> mJointHeadY;
    /* 0x9B6 */ be<s16> mJointBackboneY;
    /* 0x9B8 */ be<s16> mActorAngleY;
    /* 0x9BA */ u8 _9BA[2];
    /* 0x9BC */ be<s16> m84C; /* GameCube m84C */
    /* 0x9BE */ u8 _9BE[4];
    /* 0x9C2 */ be<s16> m852;       /* GameCube m852 */
    /* 0x9C4 */ be<s16> m854;       /* GameCube m854 */
    /* 0x9C6 */ be<u16> mMsgStatus; /* HD: the message status seen by talk_1 */
    /* 0x9C8 */ be<s8> m858;        /* GameCube m858 */
    /* 0x9C9 */ be<u8> mAnimationLoopCount;
    /* 0x9CA */ u8 _9CA;
    /* 0x9CB */ be<u8> mItemNo;
    /* 0x9CC */ u8 _9CC;
    /* 0x9CD */ be<u8> m85D; /* GameCube m85D: no execute/draw */
    /* 0x9CE */ be<u8> m85E; /* GameCube m85E: the arm animation set is active */
    /* 0x9CF */ be<u8> m85F; /* GameCube m85F */
    /* 0x9D0 */ be<u8> m860; /* GameCube m860 */
    /* 0x9D1 */ be<u8> m861; /* GameCube m861 */
    /* 0x9D2 */ u8 _9D2;
    /* 0x9D3 */ be<u8> m863; /* GameCube m863 */
    /* 0x9D4 */ be<s32> mbSetEyePos;
    /* 0x9D8 */ be<u8> mbAttention;
    /* 0x9D9 */ be<u8> m869; /* GameCube m869 */
    /* 0x9DA */ be<u8> mHeadOnlyFollow;
    /* 0x9DB */ be<u8> m86B; /* GameCube m86B */
    /* 0x9DC */ be<s8> mActionIndex;
    /* 0x9DD */ be<s8> mActNo;
    /* 0x9DE */ be<u8> m86E; /* GameCube m86E */
    /* 0x9DF */ be<u8> mMesgAnimeTagInfo;
    /* 0x9E0 */ be<s8> m870; /* GameCube m870 */
    /* 0x9E1 */ be<s8> m871; /* GameCube m871 */
    /* 0x9E2 */ u8 _9E2[2];
    /* 0x9E4 */ be<s8> m874; /* GameCube m874 */
    /* 0x9E5 */ be<s8> m875; /* GameCube m875 */
    /* 0x9E6 */ be<s8> m876; /* GameCube m876 */
    /* 0x9E7 */ be<s8> m877; /* GameCube m877 */
    /* 0x9E8 */ be<s8> mType;
    /* 0x9E9 */ be<s8> m879;      /* GameCube m879 */
    /* 0x9EA */ be<s8> mActState; /* set_action: 9 in the old action, 0 in the new */
    /* 0x9EB */ be<s8> mMessageAnimationStarted;
};
WWHD_OFFSET(daNpc_Ac1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Ac1_c, mBtpAnim, 0x7F4);
WWHD_OFFSET(daNpc_Ac1_c, m6FC, 0x870);
WWHD_OFFSET(daNpc_Ac1_c, mCurrProcFunc, 0x884);
WWHD_OFFSET(daNpc_Ac1_c, mPathRun, 0x8EC);
WWHD_OFFSET(daNpc_Ac1_c, mEventCut, 0x8F4);
WWHD_OFFSET(daNpc_Ac1_c, m7F4, 0x964);
WWHD_OFFSET(daNpc_Ac1_c, m82C, 0x99C);
WWHD_OFFSET(daNpc_Ac1_c, m84C, 0x9BC);
WWHD_OFFSET(daNpc_Ac1_c, mbSetEyePos, 0x9D4);
WWHD_OFFSET(daNpc_Ac1_c, mMessageAnimationStarted, 0x9EB);
WWHD_SIZE(daNpc_Ac1_c, 0x9EC);

// HD: HIO layout and compiler-generated virtual dispatch.
struct daNpc_Ac1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mMaxHeadX;
        /* 0x02 */ be<s16> mMaxHeadY;
        /* 0x04 */ be<s16> mMinHeadX;
        /* 0x06 */ be<s16> mMinHeadY;
        /* 0x08 */ be<s16> mMaxBackboneX;
        /* 0x0A */ be<s16> mMaxBackboneY;
        /* 0x0C */ be<s16> mMinBackboneX;
        /* 0x0E */ be<s16> mMinBackboneY;
        /* 0x10 */ be<s16> mMaxTurnStep;
        /* 0x12 */ be<s16> mCalcAngleTarget;
        /* 0x14 */ be<f32> mAttPosOffsetY;
        /* 0x18 */ be<u8> mDebug;
        /* 0x19 */ u8 _19[0x30 - 0x19];
    };
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ hio_prm_c mPrmTbl;
};
WWHD_SIZE(daNpc_Ac1_HIO_c, 0x3C);
// HD: virtual frame count, animation-removal and destructor layouts.
struct Ac1AnmVtable {
    u8 _0[0x14];
    be<u32> mGetFrameMax;
};
struct Ac1Anm {
    u8 _0[4];
    gptr<Ac1AnmVtable> mpVtable;
};
struct Ac1ModelAnimation {
    u8 _0[0x38];
    be<u32> mpBtpAnimation;
};
struct Ac1AcchVtables {
    u8 _0[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mPolyVtable;
};
WWHD_OFFSET(Ac1AnmVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(Ac1Anm, mpVtable, 4);
WWHD_OFFSET(Ac1ModelAnimation, mpBtpAnimation, 0x38);
WWHD_OFFSET(Ac1AcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(Ac1AcchVtables, mPolyVtable, 0x20);
template <class T, class U> inline T* Ac1_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline s32 Ac1_frameMax(J3DAnmTexPattern* p) {
    auto* a = Ac1_view<Ac1Anm>(p);
    auto* t = a->mpVtable.get();
    return gabi::call_ptr<s32>(t->mGetFrameMax, p);
}
inline void Ac1_loadAction(Ac1ActionFunc* dst, Ac1ActionFunc* src) {
    auto* d = Ac1_view<be<u32>>(dst);
    auto* s = Ac1_view<be<u32>>(src);
    d[0] = s[0];
    d[1] = s[1];
}
struct Ac1VirtualAction {
    u8 _0[6];
    be<s16> mVtableOffset;
};
inline void Ac1_callAction(void* self, Ac1ActionFunc* action, void* arg) {
    s16 index = action->i;
    u32 object = gabi::ea(self) + (s32)(s16)action->d;
    if (index < 0) {
        gabi::call_ptr<BOOL>(action->f, object, arg);
    } else {
        s16 offset = Ac1_view<Ac1VirtualAction>(action)->mVtableOffset;
        auto* vtable = gabi::at<be<u32>>(object + offset);
        auto* entry = gabi::at<be<u32>>((u32)*vtable + index * 8 + 4);
        gabi::call_ptr<BOOL>(*entry, object, arg);
    }
}
inline void Ac1_initDebugColour(be<u32>* guard, be<u32>* dst, be<u32>* src) {
    if ((u32)*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, 4);
    }
}

s8 Ac1_roomId(dBgS*, cBgS_PolyInfo*);
u8 Ac1_polyColour(dBgS*, cBgS_PolyInfo*);

struct Ac1MorfVtable {
    u8 _0[0xC];
    be<u32> mDestroy;
};
struct Ac1MorfObject {
    gptr<Ac1MorfVtable> mpVtable;
};
WWHD_OFFSET(Ac1MorfVtable, mDestroy, 0xC);
WWHD_SIZE(Ac1MorfVtable, 0x10);
WWHD_OFFSET(Ac1MorfObject, mpVtable, 0);
WWHD_SIZE(Ac1MorfObject, 4);
inline void Ac1_destroyMorf(mDoExt_McaMorf* p) {
    gabi::call_ptr(Ac1_view<Ac1MorfObject>(p)->mpVtable->mDestroy, p, 3);
}
inline const char* Ac1_staffName(dNpc_EventCut_c* p) {
    return gabi::at<const char>(p->mpEvtStaffName);
}
void* Ac1_getNamedResource(dRes_control_c*, SafeString*, SafeString*);
inline void* Ac1_objectResource(const char* arc, const char* name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> n;
    a->mStringTop = gabi::guest_addr(arc);
    a->__vtbl = gc_tu.safeString;
    n->mStringTop = gabi::guest_addr(name);
    n->__vtbl = gc_tu.safeString;
    return Ac1_getNamedResource(dComIfG_resControl(), a.get(), n.get());
}
WWHD_SIZE(Ac1AnmVtable, 0x18);
WWHD_SIZE(Ac1Anm, 8);
WWHD_SIZE(Ac1ModelAnimation, 0x3C);
WWHD_SIZE(Ac1AcchVtables, 0x24);
WWHD_OFFSET(Ac1VirtualAction, mVtableOffset, 6);
WWHD_SIZE(Ac1VirtualAction, 8);
WWHD_OFFSET(Ac1ActionFunc, d, 0);
WWHD_OFFSET(Ac1ActionFunc, i, 2);
WWHD_OFFSET(Ac1ActionFunc, f, 4);
WWHD_OFFSET(daNpc_Ac1_HIO_c, __vtbl, 0);
WWHD_OFFSET(daNpc_Ac1_HIO_c, mPrmTbl, 0xC);
WWHD_SIZE(daNpc_Ac1_HIO_c::hio_prm_c, 0x30);
WWHD_OFFSET(daNpc_Ac1_HIO_c::hio_prm_c, mAttPosOffsetY, 0x14);
WWHD_OFFSET(daNpc_Ac1_HIO_c::hio_prm_c, mDebug, 0x18);
WWHD_SIZE(daNpc_Ac1_c::anm_prm_c, 0x14);
WWHD_OFFSET(daNpc_Ac1_c::anm_prm_c, mMorf, 4);
WWHD_OFFSET(daNpc_Ac1_c::anm_prm_c, mSpeed, 8);
WWHD_OFFSET(daNpc_Ac1_c::anm_prm_c, mLoopMode, 0xC);
WWHD_OFFSET(daNpc_Ac1_c::anm_prm_c, mbArm, 0x10);
s16 Ac1_calcTimer(be<s16>* t);
s32 Ac1_btpInit(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                s32 start, s32 end, u32 modify, s32 entry);
bool Ac1_lockonTruth(dAttention_c* a);
fopAc_ac_c* Ac1_lockonTarget(dAttention_c* a, s32 i);
fopAc_ac_c* Ac1_actionTarget(dAttention_c* a, s32 i);
void Ac1_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel,
                      u8 headOnly);
BOOL Ac1_demoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids,
                  u32 p6, s8 p7);
void* Ac1_demoGetActor(u32 obj, u8 id);
void Ac1_npcCtor(daNpc_Ac1_c* p);
void Ac1_btpCtor(void* p);
void Ac1_eventCutCtor(dNpc_EventCut_c* p);
void Ac1_circleDestroy(void* p, s32 flags);
void Ac1_acchDestroy(dBgS_Acch* p, s32 flags);
void Ac1_actorDestroy(daNpc_Ac1_c* p, s32 flags);
J3DAnmTexPattern* Ac1_demoBtp(void*, const char*);
u32 Ac1_setBtpResult(daNpc_Ac1_c*, u32, s32);
BOOL Ac1_setWingAnimation(mDoExt_McaMorf*, s32, f32, f32, u32, s32, const char*);
u32 Ac1_messageStatus(void*);
BOOL Ac1_eventAdvanced(dEvent_manager_c*, s32);
void Ac1_cutEnd(dEvent_manager_c*, s32);
