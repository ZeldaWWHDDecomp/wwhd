/* daNpc_Zk1_c (Zuko, the Rito postman at the Dragon Roost aerie), WWHD layout.
 *
 * GameCube -> WWHD, measured from the verified functions of d_a_npc_zk1 (the GameCube source is
 * all "Nonmatching" stubs; only m72C/m750 are named there): fopNpc_npc_c is 0x7DC (see
 * d_a_npc_ba1.h / d/d_npc.h). The archive name is a member (mArcName, "Zk", copied by
 * decideType) and is used as the resource key everywhere. mDoExt_btpAnm is 0x74 (constructor
 * 025E7820) and the pointer to member function 8 bytes, so the members after it are +0x150
 * from GameCube (m72C -> m72C 0x87C). No dNpc_EventCut_c member: the fopNpc_npc_c one (0x3E0)
 * is used. Size 0x924 (profile g_profile_NPC_ZK1 at 0x101C7098; GameCube 0x7B4). */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
struct Zk1ActionFunc {
    be<s16> d, i;
    be<u32> f;
};
WWHD_SIZE(Zk1ActionFunc, 8);

struct daNpc_Zk1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;
        /* 0x01 */ be<s8> mBtpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    void _nodeCB_Head(J3DNode*, J3DModel*);
    void _nodeCB_BackBone(J3DNode*, J3DModel*);
    s32 btpResID(int);
    bool setBtp(s32, u32);          /* (s8, bool): r4 is tested as a word, r5 passed on */
    u32 init_texPttrnAnm(s32, u32); /* tail call to setBtp: registers passed through */
    BOOL bodyCreateHeap();
    BOOL CreateHeap();
    bool decideType(int);
    BOOL set_action(Zk1ActionFunc*, void*);
    bool init_ZK1_0();
    void play_btp_anm();
    void play_animation();
    void setAttention(u32); /* bool, tested as a word */
    void setMtx(u32);       /* bool, passed on */
    bool createInit();
    cPhs_State _create();
    BOOL _delete();
    void checkOrder();
    u8 demo(); /* bool */
    s32 isEventEntry();
    void endEvent();
    void privateCut(int);
    void event_proc(int);
    fopAc_ac_c* searchByID(fpc_ProcID, be<s32>*);
    void lookBack();
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    s32 bckResID(int);
    void setAnm_anm(anm_prm_c*);
    void setAnm();
    void setAnm_ATR();
    void chngAnmAtr(u32); /* u8, compared as a word */
    void anmAtr(u32);     /* u16, compared as a word */
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_ZK1_0();
    u32 getMsg();
    bool chk_talk();
    u8 chk_parts_notMov(); /* bool */
    u8 chkAttention();     /* bool */
    void setStt(u32);      /* s8, compared as a word */
    BOOL wait_1();
    u32 talk_1(); /* returns chk_parts_notMov()'s register */
    BOOL wait_action1(void*);

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ char mArcName[3]; /* HD: "Zk" (decideType) */
    /* 0x7E9 */ u8 _7E9[3];
    /* 0x7EC */ u8 mBtpAnm[0x74]; /* mDoExt_btpAnm (HD 0x74); +0x10 the J3DAnmTexPattern* */
    /* 0x860 */ be<u8> mBlinkFrame;
    /* 0x861 */ u8 _861;
    /* 0x862 */ be<s16> mBlinkTimer;
    /* 0x864 */ Zk1ActionFunc mCurrProcFunc;
    /* 0x86C */ u8 _86C[4];
    /* 0x870 */ be<u32> mPartnerID; /* lookBack state 4 */
    /* 0x874 */ u8 _874[8];
    /* 0x87C */ cXyz m72C; /* GameCube m72C */
    /* 0x888 */ csXyz mInitAngle;
    /* 0x88E */ csXyz mAngle; /* model rotation (setMtx) */
    /* 0x894 */ u8 _894[8];
    /* 0x89C */ cXyz mEyePos; /* head joint * a_eye_pos_off */
    /* 0x8A8 */ cXyz mLookTarget;
    /* 0x8B4 */ u8 _8B4[0xC];
    /* 0x8C0 */ cXyz mHeadPos; /* head joint translation */
    /* 0x8CC */ u8 _8CC[0xC];
    /* 0x8D8 */ be<f32> mPrevMorfFrame;
    /* 0x8DC */ u8 _8DC[4];
    /* 0x8E0 */ be<s16> mActorAngleY;
    /* 0x8E2 */ be<s16> mJointHeadY;
    /* 0x8E4 */ be<s16> mJointBackboneY;
    /* 0x8E6 */ u8 _8E6[2];
    /* 0x8E8 */ be<s32> mbSetEyePos; /* wait_1 / talk_1 result */
    /* 0x8EC */ be<s16> mEventIdTable[1];
    /* 0x8EE */ be<s16> mEventIdx;
    /* 0x8F0 */ u8 _8F0[8];
    /* 0x8F8 */ be<s16> mTimer;
    /* 0x8FA */ u8 _8FA[8];
    /* 0x902 */ be<s16> mTargetYRot;
    /* 0x904 */ be<s8> mbMorfAnimStopped;
    /* 0x905 */ be<u8> mAnimationLoopCount;
    /* 0x906 */ be<u8> mItemNo;
    /* 0x907 */ be<u8> mFirstConversationDone;  /* event bit 0x1802 (init_ZK1_0) */
    /* 0x908 */ be<u8> mSecondConversationDone; /* event bit 0x1C01 (init_ZK1_0) */
    /* 0x909 */ u8 _909;
    /* 0x90A */ be<u8> mRunEvent;       /* run event_proc */
    /* 0x90B */ be<u8> mbResLoaded;     /* _create: resLoad == complete */
    /* 0x90C */ be<u8> mHidden;         /* no execute/draw (unless in a demo) */
    /* 0x90D */ be<u8> mKeepShapeAngle; /* keep shape_angle */
    /* 0x90E */ be<u8> mDrawDisabled;   /* no draw */
    /* 0x90F */ be<u8> mbRanExecute;
    /* 0x910 */ be<u8> mbAttention;
    /* 0x911 */ be<u8> mTalkOrdered; /* talk ordered (checkOrder) */
    /* 0x912 */ be<u8> mHeadOnlyFollow;
    /* 0x913 */ be<u8> mReturnToInitialAngle; /* turn back to mInitAngle */
    /* 0x914 */ be<u8> mbInDemo;
    /* 0x915 */ be<s8> mActIdx;
    /* 0x916 */ be<u8> mAnmAtr;
    /* 0x917 */ be<u8> mMesgAnimeTagInfo;
    /* 0x918 */ be<s8> mBtpNum;
    /* 0x919 */ be<s8> mAnmNum;
    /* 0x91A */ be<s8> mEvtCond; /* 1/2 talk, >= 3 event mEventIdTable[n - 3] */
    /* 0x91B */ be<s8> mStatus;
    /* 0x91C */ be<s8> mPrevStatus;
    /* 0x91D */ be<s8> mLookBackState;
    /* 0x91E */ be<s8> mType;
    /* 0x91F */ be<s8> mSpecificType;
    /* 0x920 */ be<s8> mActState; /* set_action: 0 init, 9 leaving */
    /* 0x921 */ be<s8> mMessageAnimationStarted;
    /* 0x922 */ u8 _922[2];
};
WWHD_OFFSET(daNpc_Zk1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Zk1_c, mBtpAnm, 0x7EC);
WWHD_OFFSET(daNpc_Zk1_c, mCurrProcFunc, 0x864);
WWHD_OFFSET(daNpc_Zk1_c, m72C, 0x87C);
WWHD_OFFSET(daNpc_Zk1_c, mEyePos, 0x89C);
WWHD_OFFSET(daNpc_Zk1_c, mPrevMorfFrame, 0x8D8);
WWHD_OFFSET(daNpc_Zk1_c, mbSetEyePos, 0x8E8);
WWHD_OFFSET(daNpc_Zk1_c, mTargetYRot, 0x902);
WWHD_OFFSET(daNpc_Zk1_c, mbInDemo, 0x914);
WWHD_OFFSET(daNpc_Zk1_c, mMessageAnimationStarted, 0x921);
WWHD_SIZE(daNpc_Zk1_c, 0x924);

// HD: HIO vtable remains first.
struct daNpc_Zk1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mJnt[9];      /* dNpc_JntCtrl_c::setParam arguments */
        /* 0x12 */ be<s16> mMaxTurnStep; /* lookBack */
        /* 0x14 */ be<f32> mAttPosOffsetY;
        /* 0x18 */ be<u8> mDebug; /* debug colours in _draw */
        /* 0x19 */ u8 _19[3];
    };
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ hio_prm_c mPrmTbl;
};
WWHD_SIZE(daNpc_Zk1_HIO_c, 0x28);

// HD: virtual frame count, animation-removal and destructor layouts.
struct Zk1AnmVtable {
    u8 _0[0x14];
    be<u32> mGetFrameMax;
};
struct Zk1Anm {
    u8 _0[4];
    gptr<Zk1AnmVtable> mpVtable;
};
struct Zk1ModelAnimation {
    u8 _0[0x38];
    be<u32> mpBtpAnimation;
};
struct Zk1AcchVtables {
    u8 _0[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mPolyVtable;
};
WWHD_OFFSET(Zk1AnmVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(Zk1Anm, mpVtable, 4);
WWHD_OFFSET(Zk1ModelAnimation, mpBtpAnimation, 0x38);
WWHD_OFFSET(Zk1AcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(Zk1AcchVtables, mPolyVtable, 0x20);
template <class T, class U> inline T* Zk1_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline s32 Zk1_frameMax(J3DAnmTexPattern* p) {
    auto* a = Zk1_view<Zk1Anm>(p);
    auto* t = a->mpVtable.get();
    return gabi::call_ptr<s32>(t->mGetFrameMax, p);
}
inline void Zk1_loadAction(Zk1ActionFunc* dst, Zk1ActionFunc* src) {
    auto* d = Zk1_view<be<u32>>(dst);
    auto* s = Zk1_view<be<u32>>(src);
    d[0] = s[0];
    d[1] = s[1];
}
struct Zk1VirtualAction {
    u8 _0[6];
    be<s16> mVtableOffset;
};
inline void Zk1_callAction(void* self, Zk1ActionFunc* action, void* arg) {
    s16 index = action->i;
    u32 object = gabi::ea(self) + (s32)(s16)action->d;
    if (index < 0) {
        gabi::call_ptr<BOOL>(action->f, object, arg);
    } else {
        s16 offset = Zk1_view<Zk1VirtualAction>(action)->mVtableOffset;
        auto* vtable = gabi::at<be<u32>>(object + offset);
        auto* entry = gabi::at<be<u32>>((u32)*vtable + index * 8 + 4);
        gabi::call_ptr<BOOL>(*entry, object, arg);
    }
}
inline void Zk1_initDebugColour(be<u32>* guard, be<u32>* dst, be<u32>* src) {
    if ((u32)*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, 4);
    }
}

s8 Zk1_roomId(dBgS*, cBgS_PolyInfo*);
u8 Zk1_polyColour(dBgS*, cBgS_PolyInfo*);

struct Zk1BtpAnm {
    u8 _0[0x10];
    gptr<J3DAnmTexPattern> mpAnimation;
};
WWHD_OFFSET(Zk1BtpAnm, mpAnimation, 0x10);
WWHD_SIZE(Zk1BtpAnm, 0x14);
struct Zk1MorfVtable {
    u8 _0[0xC];
    be<u32> mDestroy;
};
struct Zk1MorfObject {
    gptr<Zk1MorfVtable> mpVtable;
};
WWHD_OFFSET(Zk1MorfVtable, mDestroy, 0xC);
WWHD_SIZE(Zk1MorfVtable, 0x10);
WWHD_OFFSET(Zk1MorfObject, mpVtable, 0);
WWHD_SIZE(Zk1MorfObject, 4);
inline void Zk1_destroyMorf(mDoExt_McaMorf* p) {
    gabi::call_ptr(Zk1_view<Zk1MorfObject>(p)->mpVtable->mDestroy, p, 3);
}
inline const char* Zk1_staffName(dNpc_EventCut_c* cut) {
    return gabi::at<const char>(cut->mpEvtStaffName);
}
inline void Zk1_copyWords(cXyz* dst, const cXyz* src) {
    auto* s = Zk1_view<be<u32>>(src);
    auto* d = Zk1_view<be<u32>>(dst);
    u32 x = s[0], y = s[1], z = s[2];
    d[0] = x;
    d[1] = y;
    d[2] = z;
}
inline be<u8>* Zk1_bytes(void* p) { return gabi::at<be<u8>>(gabi::ea(p)); }
WWHD_SIZE(Zk1AnmVtable, 0x18);
WWHD_SIZE(Zk1Anm, 8);
WWHD_SIZE(Zk1ModelAnimation, 0x3C);
WWHD_SIZE(Zk1AcchVtables, 0x24);
WWHD_OFFSET(Zk1VirtualAction, mVtableOffset, 6);
WWHD_SIZE(Zk1VirtualAction, 8);
WWHD_OFFSET(Zk1ActionFunc, d, 0);
WWHD_OFFSET(Zk1ActionFunc, i, 2);
WWHD_OFFSET(Zk1ActionFunc, f, 4);
WWHD_OFFSET(daNpc_Zk1_HIO_c, __vtbl, 0);
WWHD_OFFSET(daNpc_Zk1_HIO_c, mPrmTbl, 0xC);
WWHD_SIZE(daNpc_Zk1_HIO_c::hio_prm_c, 0x1C);
WWHD_OFFSET(daNpc_Zk1_HIO_c::hio_prm_c, mAttPosOffsetY, 0x14);
WWHD_OFFSET(daNpc_Zk1_HIO_c::hio_prm_c, mDebug, 0x18);
WWHD_SIZE(daNpc_Zk1_c::anm_prm_c, 0x10);
WWHD_OFFSET(daNpc_Zk1_c::anm_prm_c, mMorf, 4);
WWHD_OFFSET(daNpc_Zk1_c::anm_prm_c, mSpeed, 8);
WWHD_OFFSET(daNpc_Zk1_c::anm_prm_c, mLoopMode, 0xC);
s8 Zk1_roomId(dBgS*, cBgS_PolyInfo*);
u8 Zk1_polyColour(dBgS*, cBgS_PolyInfo*);
s16 Zk1_calcTimer(be<s16>* t);
s32 Zk1_btpInit(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                s32 start, s32 end, u32 modify, s32 entry);
bool Zk1_lockonTruth(dAttention_c* a);
fopAc_ac_c* Zk1_lockonTarget(dAttention_c* a, s32 i);
fopAc_ac_c* Zk1_actionTarget(dAttention_c* a, s32 i);
void Zk1_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel,
                      u8 headOnly);
BOOL Zk1_demoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids,
                  u32 p6, s8 p7);
void* Zk1_demoGetActor(u32 obj, u8 id);
void Zk1_npcCtor(daNpc_Zk1_c* p);
void Zk1_btpCtor(void* p);
void Zk1_circleDestroy(void* p, s32 flags);
void Zk1_acchDestroy(dBgS_Acch* p, s32 flags);
void Zk1_actorDestroy(daNpc_Zk1_c* p, s32 flags);
J3DAnmTexPattern* Zk1_demoBtp(void*, const char*);
s32 Zk1_getRndValue(s32, s32);
BOOL Zk1_checkAttention(fopAc_ac_c*, cXyz*, f32, f32, f32, u32);
u32 Zk1_materialSound(dBgS*, cBgS_PolyInfo*);
BOOL Zk1_searchById(u32, be<u32>*);
u32 Zk1_setBtpResult(daNpc_Zk1_c*, s32, u32);
BOOL Zk1_playMorf(mDoExt_McaMorf*, cXyz*, u32, s32);
u32 Zk1_messageStatus(void*);

inline fopAc_ac_c* Zk1_actorFromIdResult(u32 p) { return gabi::at<fopAc_ac_c>(p); }
