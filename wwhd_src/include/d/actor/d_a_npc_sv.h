// Salvage Corp. members, verified WWHD layout. GameCube source is nonmatching.
// HD: NPC base 0x7DC, actor size 0x860, virtuals in the actor vtable at 0xB4.
#pragma once
#include "gc_api/d/d_npc.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"

/* animation script entry (3 bytes) */
struct sSvAnmDat {
    /* 0x0 */ be<u8> mAnm;   /* 0xFF: end */
    /* 0x1 */ be<u8> mMorf;
    /* 0x2 */ be<s8> mLoop;  /* > 0: play n times, then the next entry */
};
WWHD_SIZE(sSvAnmDat, 3);

/* per-member parameters, l_npc_dat (.data 101C6668, 0x44 per npc number) */
struct sSvNpcDat {
    /* 0x00 */ be<s16> mMaxHeadX;
    /* 0x02 */ be<s16> mMaxHeadY;
    /* 0x04 */ be<s16> mMaxBackboneX;
    /* 0x06 */ be<s16> mMaxBackboneY;
    /* 0x08 */ be<s16> mMinHeadX;
    /* 0x0A */ be<s16> mMinHeadY;
    /* 0x0C */ be<s16> mMinBackboneX;
    /* 0x0E */ be<s16> mMinBackboneY;
    /* 0x10 */ be<s16> mMaxTurnStep;
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ be<f32> mOffsetX, mOffsetY, mOffsetZ;        /* model offset (setMtx) */
    /* 0x20 */ be<f32> mEyeOffset;  /* dNpc_playerEyePos */
    /* 0x24 */ be<f32> mAttnY;      /* attention position height */
    /* 0x28 */ be<f32> mEyeY;       /* eye position height */
    /* 0x2C */ be<f32> mTalkDist;   /* m824 */
    /* 0x30 */ be<f32> mLookDist;
    /* 0x34 */ be<s16> mLookAngle;  /* m83A */
    /* 0x36 */ be<s16> mLookSpeed;  /* m842 */
    /* 0x38 */ be<f32> mCylR;
    /* 0x3C */ be<f32> mCylH;
    /* 0x40 */ be<s16> mLookTimer;  /* m838 */
    /* 0x42 */ be<u8> mTurnToTarget;          /* mTurnToTarget */
    /* 0x43 */ be<u8> mFollowPlayer;          /* mFollowPlayer */
};
WWHD_SIZE(sSvNpcDat, 0x44);

struct daNpcSv_c : fopNpc_npc_c {
    cPhs_State _create();
    BOOL createHeap();
    s32 createInit();
    BOOL _delete();
    BOOL _draw();
    BOOL _execute();
    s32 executeCommon();
    void executeSetMode(int mode);
    s32 executeWaitInit();
    void executeWait();
    u8 executeTalkInit();
    void executeTalk();
    void checkOrder();
    void eventOrder();
    void eventMove();
    void privateCut();
    void eventMesSetInit(int staffId);
    BOOL eventMesSet();
    void eventGetItemInit(int staffId);
    void eventSetAngleInit();
    void eventAttentionInit(int staffId);
    BOOL eventAttention();
    void eventTurnOkInit();
    u16 talk2(int mode, fopAc_ac_c* actor);
    u16 next_msgStatus(be<u32>* pMsgNo);
    u32 getMsg();
    void setMessage(u32 msgNo);
    void setAnmFromMsgTag();
    u8 getPrmNpcNo();
    void setMtx();
    void chkAttention();
    void lookBack();
    void playAnm();
    void setAnm(u8 anm, int loopMode, f32 morf);
    BOOL setAnmTbl(sSvAnmDat* p);
    void setCollision(dCcD_Cyl* cyl, cXyz* pos, f32 r, f32 h); /* cXyz by value: pointer to a copy */
    u8 getTalkNo();
    BOOL isTalkOK();

    /* 0x7DC */ request_of_phase_process_class mPhs;      /* dComIfG_resLoad */
    /* 0x7E4 */ request_of_phase_process_class mPhsHandler; /* dComLbG_PhaseHandler */
    /* 0x7EC */ be<u32> mKeepLookTimer;              /* chkAttention: != 0 keeps the look timer */
    /* 0x7F0 */ u8 _7F0[0x800 - 0x7F0];
    /* 0x800 */ cXyz mLookPos;             /* look target (mode 1) */
    /* 0x80C */ u8 _80C[0x818 - 0x80C];
    /* 0x818 */ gptr<sSvAnmDat> mpAnmTbl;  /* current animation script entry */
    /* 0x81C */ be<u32> mpMsgTbl;          /* u32* message script */
    /* 0x820 */ be<f32> mNextMorf;         /* < 0: none */
    /* 0x824 */ be<f32> mTalkDist;
    /* 0x828 */ be<s32> mItemNo;
    /* 0x82C */ be<s32> mFrame;            /* from the ship (+0x1438) */
    /* 0x830 */ be<u8> mHeadOnly;          /* lookAtTarget's last argument */
    /* 0x831 */ u8 _831;
    /* 0x832 */ be<s16> mEventIdx[3];      /* SV_TALK_P1_1ST, SV_TALK_P1_2ND, SV_TALK_P4_1ST */
    /* 0x838 */ be<s16> mLookTimer;
    /* 0x83A */ be<s16> mLookAngle;
    /* 0x83C */ be<s16> mHomeAngleY;
    /* 0x83E */ be<s16> mAttnTimer;        /* event ATTENTION "Timer" */
    /* 0x840 */ be<s16> mEvtAngleY;        /* event SET_ANGLE */
    /* 0x842 */ be<s16> mLookSpeed;
    /* 0x844 */ be<s16> mLookVel;
    /* 0x846 */ be<s16> mLookAngleY;       /* look mode 2 */
    /* 0x848 */ be<u16> mFlags;
    /* 0x84A */ be<u8> mTalk;
    /* 0x84B */ be<u8> mAttn;
    /* 0x84C */ be<u8> mOrder;             /* 1: can talk, 2: speak */
    /* 0x84D */ be<u8> mMode;              /* 0 wait, 1 talk */
    /* 0x84E */ be<u8> mCreated;
    /* 0x84F */ be<u8> mNpcNo;
    /* 0x850 */ be<u8> mEvtFlags;
    /* 0x851 */ be<u8> mAnm;
    /* 0x852 */ be<u8> mAnmFlags;
    /* 0x853 */ be<s8> mAnmLoop;
    /* 0x854 */ be<s8> mActIdx;
    /* 0x855 */ be<s8> mLookMode;          /* 0 none, 1 position, 2 angle */
    /* 0x856 */ be<u8> mTalkEnabled;
    /* 0x857 */ be<u8> mTurnToTarget;
    /* 0x858 */ be<u8> mFollowPlayer;
    /* 0x859 */ be<u8> mPlayerNearby;
    /* 0x85A */ be<u8> mTalkNo;
    /* 0x85B */ be<u8> mMsgAnm;
    /* 0x85C */ be<u8> mAttentionAnimationStarted;
    /* 0x85D */ be<u8> mLookAtPlayer;
    /* 0x85E */ be<u8> mPreviousPlayerNearby;
    /* 0x85F */ u8 _85F;
};
WWHD_OFFSET(daNpcSv_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpcSv_c, mLookPos, 0x800);
WWHD_OFFSET(daNpcSv_c, mpAnmTbl, 0x818);
WWHD_OFFSET(daNpcSv_c, mEventIdx, 0x832);
WWHD_OFFSET(daNpcSv_c, mFlags, 0x848);
WWHD_OFFSET(daNpcSv_c, mNpcNo, 0x84F);
WWHD_OFFSET(daNpcSv_c, mPreviousPlayerNearby, 0x85E);
WWHD_SIZE(daNpcSv_c, 0x860);

// HD: private ABI adapters retain full-register message results and destructor ordering.
void Sv_npcCtor(void*);
cPhs_State Sv_phaseHandler(request_of_phase_process_class*, u32, void*);
s32 Sv_createPresent(cXyz*, s32, u8, s32, s32, csXyz*, cXyz*);
void Sv_setMessageStatus(u32, u32);
u32 Sv_messageStatus(u32);
u32 Sv_messageSet(u32, u32, cXyz*);
void Sv_circleDt(void*, s32);
void Sv_acchDt(void*, s32);
void Sv_actorDt(void*, s32);
struct SvActorVtable {
    u8 _00[0x14];
    be<u32> mNextMessage;
    u8 _18[4];
    be<u32> mGetMessage;
    u8 _20[4];
    be<u32> mAnimationAttribute;
};
WWHD_OFFSET(SvActorVtable, mNextMessage, 0x14);
WWHD_OFFSET(SvActorVtable, mGetMessage, 0x1C);
WWHD_OFFSET(SvActorVtable, mAnimationAttribute, 0x24);
struct SvAcchDestructor { u8 _00[0x14]; be<u32> mCheckVtable; u8 _18[8]; be<u32> mActorCheckVtable; };
WWHD_OFFSET(SvAcchDestructor, mCheckVtable, 0x14);
WWHD_OFFSET(SvAcchDestructor, mActorCheckVtable, 0x20);
template<class T, class U> inline T* Sv_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline be<u32>* Sv_messagePointer(u32 address) { return gabi::at<be<u32>>(address); }
inline SvActorVtable* Sv_vtable(fopNpc_npc_c* actor) { return gabi::at<SvActorVtable>(actor->__vtbl); }
inline u32 Sv_getMessage(fopNpc_npc_c* actor) { return gabi::call_ptr<u32>(Sv_vtable(actor)->mGetMessage, actor); }
inline u32 Sv_nextMessage(fopNpc_npc_c* actor, be<u32>* message) { return gabi::call_ptr<u32>(Sv_vtable(actor)->mNextMessage, actor, message); }
inline void Sv_animationAttribute(fopNpc_npc_c* actor, u16 status) { gabi::call_ptr(Sv_vtable(actor)->mAnimationAttribute, actor, status); }


void Sv_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);

void Sv_cutEnd(dEvent_manager_c*, s32);
BOOL Sv_advance(dEvent_manager_c*, s32);

WWHD_SIZE(SvActorVtable, 0x28);
WWHD_SIZE(SvAcchDestructor, 0x24);

fopAc_ac_c* Sv_searchFromName(const char*, u32, u32);
void Sv_soundStart(u32);
void Sv_xyzRotate(Mtx34*, s16, s16, s16);
BOOL Sv_searchByID(fpc_ProcID, be<u32>*);
struct SvShip : fopAc_ac_c {
    u8 _3AC[0x40C - 0x3AC]; be<s32> mMode;
    u8 _410[0x590 - 0x410]; cXyz mRopeEndPos;
    u8 _59C[0x1438 - 0x59C]; be<s32> mAnimationFrame;
    u8 _143C[0x14C2 - 0x143C]; be<u8> mPlayerNearby; u8 _14C3;
    be<u32> mCraneSide;
};
WWHD_OFFSET(SvShip, mMode, 0x40C);
WWHD_OFFSET(SvShip, mRopeEndPos, 0x590);
WWHD_OFFSET(SvShip, mAnimationFrame, 0x1438);
WWHD_OFFSET(SvShip, mPlayerNearby, 0x14C2);
WWHD_OFFSET(SvShip, mCraneSide, 0x14C4);
WWHD_SIZE(SvShip, 0x14C8);
inline SvShip* Sv_ship(u32 address) { return gabi::at<SvShip>(address); }
inline fopAc_ac_c* Sv_actor(u32 address) { return gabi::at<fopAc_ac_c>(address); }
inline sSvAnmDat* Sv_animation(u32 address) { return gabi::at<sSvAnmDat>(address); }
struct SvTimerArgument { be<s16> mHigh; be<s16> mTimer; };
WWHD_OFFSET(SvTimerArgument, mTimer, 2);


WWHD_OFFSET(sSvNpcDat, mOffsetX, 0x14);
WWHD_OFFSET(sSvNpcDat, mCylR, 0x38);
WWHD_SIZE(SvTimerArgument, 4);
