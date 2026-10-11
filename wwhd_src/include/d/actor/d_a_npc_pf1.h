/* daNpc_Pf1_c (Maggie's father, poor), WWHD layout. 
 *
 * The GameCube decompilation has no layout (all functions are "Nonmatching" stubs); the members
 * below are measured from the WWHD code of d_a_npc_pf1 and named after their use (the same
 * family as d_a_npc_aj1). The class derives from fopNpc_npc_c (0x7DC, see d_a_npc_ba1.h).
 * Members are read up to 0x92E. */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
#include "gc_api/f_op/f_op_msg.h"
struct Pf1ActionFunc {
    be<s16> d, i;
    be<u32> f;
};
WWHD_SIZE(Pf1ActionFunc, 8);

struct daNpc_Pf1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> bckNum;
        /* 0x01 */ be<s8> btpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> morf;
        /* 0x08 */ be<f32> speed;
        /* 0x0C */ be<s32> loopMode;
    };

    void _nodeCB_Head(J3DNode*, J3DModel*);
    void _nodeCB_BackBone(J3DNode*, J3DModel*);
    bool init_PF1_0();
    bool createInit();
    void play_animation();
    void setMtx(u32);
    int bckResID(int);
    int btpResID(int);
    u32 setBtp(s8, u32);
    u32 init_texPttrnAnm(s8, u32);
    void play_btp_anm();
    void setAnm_anm(anm_prm_c*);
    void setAnm_NUM(int, int);
    void setAnm();
    void chngAnmAtr(u8);
    void ctrlAnmAtr();
    void setAnm_ATR();
    void anmAtr(u16);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_PF1_0();
    u32 getMsg();
    void eventOrder();
    void checkOrder();
    BOOL chk_talk();
    u8 chk_parts_notMov();
    fopAc_ac_c* searchByID(fpc_ProcID, be<s32>*);
    void lookBack();
    u8 chkAttention();
    void setAttention(u32);
    u8 decideType(int);
    void privateCut(int);
    void endEvent();
    s32 isEventEntry();
    void event_proc(int);
    BOOL set_action(Pf1ActionFunc*, void*);
    void setStt(s8);
    void createTama(f32);
    BOOL chk_areaIN(f32, cXyz*);
    BOOL endEvent_check();
    BOOL startEvent_check();
    void set_pthPoint(u32); /* u8, passed on unextended */
    BOOL chk_attn();
    void setBikon(cXyz*);
    BOOL regret();
    BOOL attk_1();
    BOOL walk_1();
    BOOL wait_2();
    BOOL wait_3();
    BOOL talk_1();
    BOOL wait_action1(void*);
    u8 demo();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL bodyCreateHeap();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ char mArcName[6]; /* "Pf" (decideType copies 3 bytes) */
    /* 0x7EC */ u8 mBtpAnm[0x74]; /* mDoExt_btpAnm (HD 0x74); +0x10 the J3DAnmTexPattern */
    /* 0x860 */ be<u8> mBtpFrame;
    /* 0x861 */ u8 _861;
    /* 0x862 */ be<s16> mBlinkTimer;
    /* 0x864 */ Pf1ActionFunc mCurrProcFunc;
    /* 0x86C */ u8 _86C[4];
    /* 0x870 */ be<u32> mLookActorId; /* lookBack mode 4: looks at this actor */
    /* 0x874 */ dNpc_PathRun_c mPathRun;
    /* 0x87C */ cXyz mHomePos;
    /* 0x888 */ csXyz mHomeAngle;
    /* 0x88E */ csXyz mAngle;
    /* 0x894 */ u8 _894[8];
    /* 0x89C */ cXyz mEyePos; /* head joint * (20, -30, 0) */
    /* 0x8A8 */ cXyz mLookPos;
    /* 0x8B4 */ u8 _8B4[0xC];
    /* 0x8C0 */ cXyz mHeadPos; /* the attention arrow is above the head */
    /* 0x8CC */ u8 _8CC[0xC];
    /* 0x8D8 */ cXyz mGndNormal;
    /* 0x8E4 */ be<f32> mPrevMorfFrame;
    /* 0x8E8 */ u8 _8E8[4];
    /* 0x8EC */ be<s16> mActorAngleY;
    /* 0x8EE */ be<s16> mJointHeadY;
    /* 0x8F0 */ be<s16> mJointBackboneY;
    /* 0x8F2 */ u8 _8F2[2];
    /* 0x8F4 */ be<s32> mbSetEyePos;
    /* 0x8F8 */ be<s16> mEventIDTbl[1];
    /* 0x8FA */ be<s16> mEventIndex;
    /* 0x8FC */ u8 _8FC[0xA];
    /* 0x906 */ be<s16> mAttkTimer;
    /* 0x908 */ be<s16> mTimer;
    /* 0x90A */ u8 _90A[4];
    /* 0x90E */ be<s16> mLookAngleY;
    /* 0x910 */ be<s8> mbMorfAnimStopped;
    /* 0x911 */ be<s8> mAtrLoopCnt;
    /* 0x912 */ be<u8> mItemNo;
    /* 0x913 */ u8 _913;
    /* 0x914 */ be<u8> mbMessageEnded; /* set when message 0x1B60 ends */
    /* 0x915 */ be<u8> mbTalked;       /* message 0x1B59 answered */
    /* 0x916 */ u8 _916;
    /* 0x917 */ be<u8> mbResLoaded;
    /* 0x918 */ be<u8> mbWaitForDemo; /* no execute/draw while not in a demo */
    /* 0x919 */ u8 _919;
    /* 0x91A */ be<u8> mbNoShapeAngle;
    /* 0x91B */ be<u8> mbNoDraw;
    /* 0x91C */ be<u8> mbHomeSet;
    /* 0x91D */ be<u8> mbAttention;
    /* 0x91E */ be<u8> mbTalk;
    /* 0x91F */ be<u8> mbHeadOnly;
    /* 0x920 */ be<u8> mbFar;
    /* 0x921 */ be<u8> mbDemo;
    /* 0x922 */ be<s8> mActionIndex;
    /* 0x923 */ be<u8> mAnmAtr;
    /* 0x924 */ be<u8> mMesgAnimeTag;
    /* 0x925 */ be<s8> mBtpNum;
    /* 0x926 */ be<s8> mBckNum;
    /* 0x927 */ be<s8> mEvtState;
    /* 0x928 */ be<s8> mStt;
    /* 0x929 */ be<s8> mPrevStt;
    /* 0x92A */ be<s8> mLookMode;
    /* 0x92B */ be<s8> mType;
    /* 0x92C */ be<s8> mSubType;
    /* 0x92D */ be<s8> mActState;
    /* 0x92E */ be<s8> mAtrSet;
    /* 0x92F */ u8 _92F;
};
WWHD_OFFSET(daNpc_Pf1_c, mArcName, 0x7E6);
WWHD_OFFSET(daNpc_Pf1_c, mBtpAnm, 0x7EC);
WWHD_OFFSET(daNpc_Pf1_c, mCurrProcFunc, 0x864);
WWHD_OFFSET(daNpc_Pf1_c, mPathRun, 0x874);
WWHD_OFFSET(daNpc_Pf1_c, mEyePos, 0x89C);
WWHD_OFFSET(daNpc_Pf1_c, mHeadPos, 0x8C0);
WWHD_OFFSET(daNpc_Pf1_c, mPrevMorfFrame, 0x8E4);
WWHD_OFFSET(daNpc_Pf1_c, mbSetEyePos, 0x8F4);
WWHD_OFFSET(daNpc_Pf1_c, mTimer, 0x908);
WWHD_OFFSET(daNpc_Pf1_c, mbMorfAnimStopped, 0x910);
WWHD_OFFSET(daNpc_Pf1_c, mAtrSet, 0x92E);

WWHD_SIZE(daNpc_Pf1_c, 0x930);

struct daNpc_Pf1_HIO_c {
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
        /* 0x19 */ u8 _19;
        /* 0x1A */ be<s16> mTurnScale;
        /* 0x1C */ be<s16> mTurnMaxStep;
        /* 0x1E */ u8 _1E[2];
        /* 0x20 */ be<f32> mWalkAnmRate;
        /* 0x24 */ be<f32> mWalkSpeed;
        /* 0x28 */ be<f32> mWalkAccel;
        /* 0x2C */ be<f32> mAttkAnmRate;
        /* 0x30 */ be<f32> mAttkSpeed;
        /* 0x34 */ be<f32> mAttkAccel;
        /* 0x38 */ be<f32> mStartDist;
        /* 0x3C */ be<f32> mAreaDist;
    };
    /* 0x00 */ be<u32> __vtbl; /* HD: vtable first */
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> mEntryID;
    /* 0x0C */ hio_prm_c mPrm;
};
WWHD_SIZE(daNpc_Pf1_HIO_c, 0x4C);

// HD: virtual frame count, animation-removal and destructor layouts.
struct Pf1AnmVtable {
    u8 _0[0x14];
    be<u32> mGetFrameMax;
};
struct Pf1Anm {
    u8 _0[4];
    gptr<Pf1AnmVtable> mpVtable;
};
struct Pf1ModelAnimation {
    u8 _0[0x38];
    be<u32> mpBtpAnimation;
};
struct Pf1AcchVtables {
    u8 _0[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mPolyVtable;
};
WWHD_OFFSET(Pf1AnmVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(Pf1Anm, mpVtable, 4);
WWHD_OFFSET(Pf1ModelAnimation, mpBtpAnimation, 0x38);
WWHD_OFFSET(Pf1AcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(Pf1AcchVtables, mPolyVtable, 0x20);
template <class T, class U> inline T* Pf1_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline s32 Pf1_frameMax(J3DAnmTexPattern* p) {
    auto* a = Pf1_view<Pf1Anm>(p);
    auto* t = a->mpVtable.get();
    return gabi::call_ptr<s32>(t->mGetFrameMax, p);
}
inline void Pf1_loadAction(Pf1ActionFunc* dst, Pf1ActionFunc* src) {
    auto* d = Pf1_view<be<u32>>(dst);
    auto* s = Pf1_view<be<u32>>(src);
    d[0] = s[0];
    d[1] = s[1];
}
struct Pf1VirtualAction {
    u8 _0[6];
    be<s16> mVtableOffset;
};
inline void Pf1_callAction(void* self, Pf1ActionFunc* action, void* arg) {
    s16 index = action->i;
    u32 object = gabi::ea(self) + (s32)(s16)action->d;
    if (index < 0) {
        gabi::call_ptr<BOOL>(action->f, object, arg);
    } else {
        s16 offset = Pf1_view<Pf1VirtualAction>(action)->mVtableOffset;
        auto* vtable = gabi::at<be<u32>>(object + offset);
        auto* entry = gabi::at<be<u32>>((u32)*vtable + index * 8 + 4);
        gabi::call_ptr<BOOL>(*entry, object, arg);
    }
}
inline void Pf1_initDebugColour(be<u32>* guard, be<u32>* dst, be<u32>* src) {
    if ((u32)*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, 4);
    }
}

struct Pf1Btp {
    u8 _0[0x10];
    gptr<J3DAnmTexPattern> mpAnm;
};
struct Pf1MorfVtable {
    u8 _0[0xC];
    be<u32> mDestructor;
};
struct Pf1Morf {
    gptr<Pf1MorfVtable> mpVtable;
};

WWHD_OFFSET(Pf1Btp, mpAnm, 0x10);
WWHD_SIZE(Pf1Btp, 0x14);
WWHD_SIZE(Pf1Morf, 4);
WWHD_SIZE(Pf1MorfVtable, 0x10);

struct Pf1Message {
    u8 _0[0x921];
    be<u8> mbWait;
    u8 _922[0x26];
    be<u32> mSelectedAnswer;
};
struct Pf1Tama {
    u8 _0[0x3AC];
    be<u32> mParentID;
    u8 _3B0[0x71C - 0x3B0];
    be<f32> mThrowParam;
};
WWHD_OFFSET(Pf1Message, mbWait, 0x921);
WWHD_OFFSET(Pf1Message, mSelectedAnswer, 0x948);
WWHD_SIZE(Pf1Message, 0x94C);
WWHD_OFFSET(Pf1Tama, mParentID, 0x3AC);
WWHD_OFFSET(Pf1Tama, mThrowParam, 0x71C);
WWHD_SIZE(Pf1Tama, 0x720);
using Pf1GuestWord = be<u32>;
using Pf1GuestInt = be<s32>;
s16 Pf1_calcTimer(be<s16>*);
s32 Pf1_random(s32, s32);
s32 Pf1_btpInit(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32);
bool Pf1_lockonTruth(dAttention_c*);
fopAc_ac_c* Pf1_lockonTarget(dAttention_c*, s32);
fopAc_ac_c* Pf1_actionTarget(dAttention_c*, s32);
s8 Pf1_roomId(dBgS*, cBgS_PolyInfo*);
u8 Pf1_polyColour(dBgS*, cBgS_PolyInfo*);
u32 Pf1_materialSound(dBgS*, cBgS_PolyInfo*);
void Pf1_playerEyePos(cXyz*, f32);
void Pf1_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
BOOL Pf1_searchByID(fpc_ProcID, be<u32>*);
void Pf1_pathPoint(dNpc_PathRun_c*, cXyz*, u32);
void Pf1_sound(u32, cXyz*);
fopAc_ac_c* Pf1_fastCreate(s16, u32, cXyz*, s32, csXyz*, cXyz*, s8, u32, void*);
void Pf1_npcCtor(daNpc_Pf1_c*);
void Pf1_btpCtor(void*);
void* Pf1_demoActor(u32, u8);
void* Pf1_demoBtp(void*, const char*);
void Pf1_demoData(daNpc_Pf1_c*, u32, mDoExt_McaMorf*, const char*, s32, u32, u32, s32);
void Pf1_circleDestroy(void*, s32);
void Pf1_acchDestroy(dBgS_Acch*, s32);
void Pf1_actorDestroy(daNpc_Pf1_c*, s32);
u32 Pf1_messageStatus(void*);

s16 Pf1_targetAngleX(cXyz*, cXyz*);
f32 Pf1_squareDistance(const cXyz*, const cXyz*);
struct Pf1Path {
    u8 _0[5];
    be<u8> mClosed;
};
WWHD_OFFSET(Pf1Path, mClosed, 5);
WWHD_SIZE(Pf1Path, 6);

// HD: NPC contact state cleared each execution and tested by startEvent_check.
struct Pf1NpcContact {
    u8 _0[0x7D6];
    be<u16> mContactState;
};
WWHD_OFFSET(Pf1NpcContact, mContactState, 0x7D6);
WWHD_SIZE(Pf1NpcContact, 0x7D8);

inline void Pf1_deleteMorf(mDoExt_McaMorf* p) {
    auto* table = Pf1_view<Pf1Morf>(p)->mpVtable.get();
    gabi::call_ptr(table->mDestructor, p, 3);
}
inline const char* Pf1_staffName(dNpc_EventCut_c* p) {
    return gabi::at<const char>(p->mpEvtStaffName);
}
inline fopAc_ac_c* Pf1_actorAt(u32 id) { return gabi::at<fopAc_ac_c>(id); }
WWHD_SIZE(Pf1AnmVtable, 0x18);
WWHD_SIZE(Pf1Anm, 8);
WWHD_SIZE(Pf1ModelAnimation, 0x3C);
WWHD_SIZE(Pf1AcchVtables, 0x24);
WWHD_SIZE(Pf1VirtualAction, 8);
WWHD_OFFSET(daNpc_Pf1_c::anm_prm_c, bckNum, 0x00);
WWHD_OFFSET(daNpc_Pf1_c::anm_prm_c, btpNum, 0x01);
WWHD_OFFSET(daNpc_Pf1_c::anm_prm_c, morf, 0x04);
WWHD_OFFSET(daNpc_Pf1_c::anm_prm_c, speed, 0x08);
WWHD_OFFSET(daNpc_Pf1_c::anm_prm_c, loopMode, 0x0C);
WWHD_OFFSET(daNpc_Pf1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Pf1_c, m_hed_jnt_num, 0x7E4);
WWHD_OFFSET(daNpc_Pf1_c, m_bbone_jnt_num, 0x7E5);
WWHD_OFFSET(daNpc_Pf1_c, mArcName, 0x7E6);
WWHD_OFFSET(daNpc_Pf1_c, mBtpAnm, 0x7EC);
WWHD_OFFSET(daNpc_Pf1_c, mBtpFrame, 0x860);
WWHD_OFFSET(daNpc_Pf1_c, mBlinkTimer, 0x862);
WWHD_OFFSET(daNpc_Pf1_c, mCurrProcFunc, 0x864);
WWHD_OFFSET(daNpc_Pf1_c, mLookActorId, 0x870);
WWHD_OFFSET(daNpc_Pf1_c, mPathRun, 0x874);
WWHD_OFFSET(daNpc_Pf1_c, mHomePos, 0x87C);
WWHD_OFFSET(daNpc_Pf1_c, mHomeAngle, 0x888);
WWHD_OFFSET(daNpc_Pf1_c, mAngle, 0x88E);
WWHD_OFFSET(daNpc_Pf1_c, mEyePos, 0x89C);
WWHD_OFFSET(daNpc_Pf1_c, mLookPos, 0x8A8);
WWHD_OFFSET(daNpc_Pf1_c, mHeadPos, 0x8C0);
WWHD_OFFSET(daNpc_Pf1_c, mGndNormal, 0x8D8);
WWHD_OFFSET(daNpc_Pf1_c, mPrevMorfFrame, 0x8E4);
WWHD_OFFSET(daNpc_Pf1_c, mActorAngleY, 0x8EC);
WWHD_OFFSET(daNpc_Pf1_c, mJointHeadY, 0x8EE);
WWHD_OFFSET(daNpc_Pf1_c, mJointBackboneY, 0x8F0);
WWHD_OFFSET(daNpc_Pf1_c, mbSetEyePos, 0x8F4);
WWHD_OFFSET(daNpc_Pf1_c, mEventIDTbl, 0x8F8);
WWHD_OFFSET(daNpc_Pf1_c, mEventIndex, 0x8FA);
WWHD_OFFSET(daNpc_Pf1_c, mAttkTimer, 0x906);
WWHD_OFFSET(daNpc_Pf1_c, mTimer, 0x908);
WWHD_OFFSET(daNpc_Pf1_c, mLookAngleY, 0x90E);
WWHD_OFFSET(daNpc_Pf1_c, mbMorfAnimStopped, 0x910);
WWHD_OFFSET(daNpc_Pf1_c, mAtrLoopCnt, 0x911);
WWHD_OFFSET(daNpc_Pf1_c, mItemNo, 0x912);
WWHD_OFFSET(daNpc_Pf1_c, mbMessageEnded, 0x914);
WWHD_OFFSET(daNpc_Pf1_c, mbTalked, 0x915);
WWHD_OFFSET(daNpc_Pf1_c, mbResLoaded, 0x917);
WWHD_OFFSET(daNpc_Pf1_c, mbWaitForDemo, 0x918);
WWHD_OFFSET(daNpc_Pf1_c, mbNoShapeAngle, 0x91A);
WWHD_OFFSET(daNpc_Pf1_c, mbNoDraw, 0x91B);
WWHD_OFFSET(daNpc_Pf1_c, mbHomeSet, 0x91C);
WWHD_OFFSET(daNpc_Pf1_c, mbAttention, 0x91D);
WWHD_OFFSET(daNpc_Pf1_c, mbTalk, 0x91E);
WWHD_OFFSET(daNpc_Pf1_c, mbHeadOnly, 0x91F);
WWHD_OFFSET(daNpc_Pf1_c, mbFar, 0x920);
WWHD_OFFSET(daNpc_Pf1_c, mbDemo, 0x921);
WWHD_OFFSET(daNpc_Pf1_c, mActionIndex, 0x922);
WWHD_OFFSET(daNpc_Pf1_c, mAnmAtr, 0x923);
WWHD_OFFSET(daNpc_Pf1_c, mMesgAnimeTag, 0x924);
WWHD_OFFSET(daNpc_Pf1_c, mBtpNum, 0x925);
WWHD_OFFSET(daNpc_Pf1_c, mBckNum, 0x926);
WWHD_OFFSET(daNpc_Pf1_c, mEvtState, 0x927);
WWHD_OFFSET(daNpc_Pf1_c, mStt, 0x928);
WWHD_OFFSET(daNpc_Pf1_c, mPrevStt, 0x929);
WWHD_OFFSET(daNpc_Pf1_c, mLookMode, 0x92A);
WWHD_OFFSET(daNpc_Pf1_c, mType, 0x92B);
WWHD_OFFSET(daNpc_Pf1_c, mSubType, 0x92C);
WWHD_OFFSET(daNpc_Pf1_c, mActState, 0x92D);
WWHD_OFFSET(daNpc_Pf1_c, mAtrSet, 0x92E);
WWHD_OFFSET(daNpc_Pf1_HIO_c, mNo, 0x04);
WWHD_OFFSET(daNpc_Pf1_HIO_c, mEntryID, 0x08);
WWHD_OFFSET(daNpc_Pf1_HIO_c, mPrm, 0x0C);

WWHD_SIZE(daNpc_Pf1_c::anm_prm_c, 0x10);
