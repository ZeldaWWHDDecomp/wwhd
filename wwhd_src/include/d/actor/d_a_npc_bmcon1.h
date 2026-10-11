/* daNpcBmcon_c (Willi & Obli, the Flight Control Platform Ritos), WWHD layout, plus the
 * translation-unit statics and local bindings of d_a_npc_bmcon1.
 *
 * The GameCube decompilation has no layout for this class (every function is a "Nonmatching"
 * stub); the layout below is measured from the WWHD code. The GameCube header derives it from
 * fopAc_ac_c, but the constructor (02204CC0) calls fopNpc_npc_c::fopNpc_npc_c (025A1458) and the
 * code uses the fopNpc_npc_c members (m_jnt, mEventCut, mpMorf, mObjAcch, mAcchCir, mStts, mCyl,
 * mCurrMsgNo..): it is an fopNpc_npc_c, like daNpcPeople_c, whose code it closely follows.
 * Size 0x93C (constructor). The HD vtable at 0xB4 is 0x10017CC8. */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"

/* animation table entry (3 bytes, lists ended by 0xFF): bck index, morf frames, loop count
 * (<= 0: loop) */
struct sBmconAnmDat {
    /* 0x00 */ be<u8> mAnmIndex;
    /* 0x01 */ be<u8> mMorf;
    /* 0x02 */ be<s8> mLoopCount;
};
WWHD_SIZE(sBmconAnmDat, 3);

/* per-NPC parameters (l_npc_dat, .rodata 0x101BD1B4, 0x4C bytes per NPC) */
struct daNpcBmcon_c__l_npc_dat {
    /* 0x00 */ be<s16> mMaxHeadX;
    /* 0x02 */ be<s16> mMaxHeadY;
    /* 0x04 */ be<s16> mMaxBackBoneX;
    /* 0x06 */ be<s16> mMaxBackBoneY;
    /* 0x08 */ be<s16> mMinHeadX;
    /* 0x0A */ be<s16> mMinHeadY;
    /* 0x0C */ be<s16> mMinBackBoneX;
    /* 0x0E */ be<s16> mMinBackBoneY;
    /* 0x10 */ be<s16> mMaxTurnStep;
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ be<f32> mPlayerEyeOffset; /* eye offset (dNpc_playerEyePos) */
    /* 0x18 */ be<f32> mAttOffsetY;
    /* 0x1C */ be<f32> mEyeHeight;
    /* 0x20 */ be<f32> mAttDist;       /* attention distance */
    /* 0x24 */ be<f32> mLookDist;      /* look distance */
    /* 0x28 */ be<s16> mAttAngle;      /* attention angle */
    /* 0x2A */ be<s16> mTurnSpeed;     /* turn speed */
    /* 0x2C */ be<s16> mWalkTurnSpeed; /* walk turn speed */
    /* 0x2E */ u8 _2E[2];
    /* 0x30 */ be<f32> mCollisionRadius; /* collision radius */
    /* 0x34 */ be<f32> mWalkAnmRate;     /* animation speed per unit of speedF */
    /* 0x38 */ be<f32> mWalkSpeed;       /* walk speed */
    /* 0x3C */ u8 _3C[0x44 - 0x3C];
    /* 0x44 */ be<s16> mTurnWaitMin; /* turn wait time: minimum */
    /* 0x46 */ be<s16> mTurnWaitMax; /* turn wait time: maximum */
    /* 0x48 */ be<s16> mLookTimer;   /* look timer */
    /* 0x4A */ be<u8> mbTurnOnLook;
    /* 0x4B */ be<u8> mbLookPlayer;
};
WWHD_SIZE(daNpcBmcon_c__l_npc_dat, 0x4C);

struct daNpcBmcon_c : fopNpc_npc_c {
    enum Prm_e {
        PRM_RAIL_ID_W = 0x8,
        PRM_RAIL_ID_S = 0x10,
    };
    enum { NPC_BMCON1 = 0, NPC_BMCON2 = 1 }; /* Willi, Obli */

    u8 getNpcNo() { return mNpcNo; }
    request_of_phase_process_class* getPhaseP() { return &mPhase; }

    /* methods (no GameCube layout or bodies: types from the WWHD code) */
    void nodeArmControl(J3DNode*, J3DModel*);
    cPhs_State _create();
    BOOL createHeap();
    cPhs_State createInit();
    bool _delete();
    bool _draw();
    bool _execute();
    u8 executeCommon();       /* returns mTalk */
    void executeSetMode(u32); /* u8, not re-extended */
    s32 executeWaitInit();
    void executeWait();
    s32 executeTalkInit();
    void executeTalk();
    s32 executeWalkInit();
    void executeWalk();
    s32 executeTurnInit();
    void executeTurn();
    void checkOrder();
    void eventOrder();
    void eventMove();
    void privateCut();
    void eventMesSetInit(int);
    bool eventMesSet();
    void eventGetItemInit();
    u16 talk2(int);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    void chkMsg();
    void setMessage(u32);
    void setAnmFromMsgTag();
    u8 getPrmNpcNo();
    u8 getPrmRailID();
    void setMtx();
    void chkAttention();
    void lookBack();
    void playAnm();
    void setAnm(u32, int, f32); /* u8 index, full register */
    bool setAnmTbl(sBmconAnmDat*);
    s16 XyCheckCB(int);
    void setCollision(dCcD_Cyl*, cXyz*, f32, f32); /* cXyz by value: pointer to a copy */
    void calcFlyDist(cXyz* result);                /* cXyz returned through a hidden pointer (r4) */
    s16 getFlyDistMax();
    void setFlyDistMax(s32); /* s16, used unextended */
    s16 getFlyDistNow();
    void setFlyDistNow(s32); /* s16, used unextended */
    BOOL chkEndEvent();
    BOOL isClear();

    /* ---- layout ---- */
    /* 0x7DC */ Mtx34 mArmLMtx;                         /* armLloc joint matrix (nodeArmControl) */
    /* 0x80C */ Mtx34 mArmRMtx;                         /* armRloc */
    /* 0x83C */ request_of_phase_process_class mPhase;  /* resource phase */
    /* 0x844 */ request_of_phase_process_class mPhase2; /* phase handler (_create) */
    /* 0x84C */ gptr<mDoExt_McaMorf> mpArmMorf;
    /* 0x850 */ gptr<J3DModel> mpEtcModel;
    /* 0x854 */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x858 */ u8 mBtpAnm[0x74]; /* mDoExt_btpAnm (HD 0x74, constructor 025E7820) */
    /* 0x8CC */ dNpc_PathRun_c mPathRun;
    /* 0x8D4 */ u8 _8D4[0x8E0 - 0x8D4];
    /* 0x8E0 */ cXyz mLookAtPos;
    /* 0x8EC */ gptr<sBmconAnmDat> mpAnmDat;
    /* 0x8F0 */ gptr<be<u32>> mpMessageCursor;
    /* 0x8F4 */ be<f32> mTargetSpeed;
    /* 0x8F8 */ be<f32> mMorfOverride; /* < 0: none (setAnm) */
    /* 0x8FC */ be<f32> mAttDist;
    /* 0x900 */ be<u32> mPresentItem;
    /* 0x904 */ be<u8> mbHeadOnly;
    /* 0x905 */ u8 _905;
    /* 0x906 */ be<s16> mEventIdx[2];
    /* 0x90A */ be<s16> mTurnWaitTimer;
    /* 0x90C */ be<s16> mLookTimer;
    /* 0x90E */ be<s16> mAttAngle;
    /* 0x910 */ be<s16> mHomeAngleY;
    /* 0x912 */ be<u16> mPrevMsgStatus;
    /* 0x914 */ be<s16> mUnusedTimer;
    /* 0x916 */ be<s16> mFlyDist;
    /* 0x918 */ u8 _918[2];
    /* 0x91A */ be<s16> mTurnSpeed;
    /* 0x91C */ be<s16> mTurnStep;
    /* 0x91E */ be<s16> mTargetAngleY;
    /* 0x920 */ be<u8> mTalk;
    /* 0x921 */ be<u8> mbAttention;
    /* 0x922 */ be<u8> mOrderEventNum;
    /* 0x923 */ u8 _923;
    /* 0x924 */ be<s8> m_nec_jnt_num;
    /* 0x925 */ be<s8> m_arm_L_jnt_num;
    /* 0x926 */ be<s8> m_arm_R_jnt_num;
    /* 0x927 */ be<s8> m_armLloc_jnt_num;
    /* 0x928 */ be<s8> m_armRloc_jnt_num;
    /* 0x929 */ be<u8>
        mExeMode; /* index into l_execute (returned by the l_execute_init function) */
    /* 0x92A */ be<u8> mResFlag;
    /* 0x92B */ be<u8> mNpcNo;
    /* 0x92C */ be<u8> mPresentFlags;
    /* 0x92D */ be<u8> mAnmIdx;
    /* 0x92E */ be<u8> mAnmFlags;
    /* 0x92F */ be<s8> mAnmLoop;
    /* 0x930 */ be<u8> mCutIndex;
    /* 0x931 */ u8 _931;
    /* 0x932 */ be<u8> mLookMode;    /* 0 none, 1 look at mLookAtPos, 2 turn to mTargetAngleY */
    /* 0x933 */ be<u8> mbTurnOnLook; /* l_npc_dat 0x4A */
    /* 0x934 */ be<u8> mbLookPlayer; /* l_npc_dat 0x4B */
    /* 0x935 */ be<u8> mbFar;
    /* 0x936 */ be<u8> mbStartFlight;
    /* 0x937 */ be<u8> mbChangingStage;
    /* 0x938 */ be<u8> mFlightResult;
    /* 0x939 */ be<u8> mFlightFlags;
    /* 0x93A */ u8 _93A[2];
};
WWHD_OFFSET(daNpcBmcon_c, mPhase, 0x83C);
WWHD_OFFSET(daNpcBmcon_c, mBtpAnm, 0x858);
WWHD_OFFSET(daNpcBmcon_c, mPathRun, 0x8CC);
WWHD_OFFSET(daNpcBmcon_c, mpAnmDat, 0x8EC);
WWHD_OFFSET(daNpcBmcon_c, mEventIdx, 0x906);
WWHD_OFFSET(daNpcBmcon_c, mNpcNo, 0x92B);
WWHD_OFFSET(daNpcBmcon_c, mFlightFlags, 0x939);
WWHD_SIZE(daNpcBmcon_c, 0x93C);

// HD: GHS member descriptors, virtual message slots and player flag used by this actor.
struct BmconAction {
    be<s16> d, i;
    be<u32> f;
};
struct BmconVirtualAction {
    u8 _0[6];
    be<s16> mVtableOffset;
};
struct BmconMessage {
    u8 _0[0x948];
    be<u32> mSelectNum;
};
struct BmconVirtualNpc {
    u8 _0[0xB4];
    gptr<be<u32>> mpVtable;
};
struct BmconPlayerFlag {
    u8 _0[0x3BC];
    be<u32> mStatus;
};
struct BmconModelScale {
    u8 _0[0xBC];
    cXyz mScale;
};
struct BmconAcchVtables {
    u8 _0[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mPolyVtable;
};
typedef be<f32> BmconFloat;
typedef be<s16> BmconAngle;
template <class T, class U> inline T* Bmcon_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline s32 Bmcon_callMode(void* self, BmconAction* action) {
    s16 index = action->i;
    u32 object = gabi::ea(self) + (s32)(s16)action->d;
    if (index < 0)
        return gabi::call_ptr<s32>(action->f, object);
    s16 offset = Bmcon_view<BmconVirtualAction>(action)->mVtableOffset;
    auto* vt = gabi::at<be<u32>>(object + offset);
    auto* entry = gabi::at<be<u32>>((u32)*vt + index * 8 + 4);
    return gabi::call_ptr<s32>(*entry, object);
}
inline u32 Bmcon_virtualFunction(void* self, u32 slot) {
    return Bmcon_view<BmconVirtualNpc>(self)->mpVtable.get()[slot / 4];
}
inline u32 Bmcon_nextMessage(daNpcBmcon_c* self, be<u32>* msg) {
    return gabi::call_ptr<u32>(Bmcon_virtualFunction(self, 0x14), self, msg);
}
inline u32 Bmcon_getMessage(daNpcBmcon_c* self) {
    return gabi::call_ptr<u32>(Bmcon_virtualFunction(self, 0x1C), self);
}
inline void Bmcon_messageAnimation(daNpcBmcon_c* self, u16 status) {
    gabi::call_ptr(Bmcon_virtualFunction(self, 0x24), self, status);
}
inline void Bmcon_copyWords(cXyz* dst, cXyz* src) {
    auto* d = Bmcon_view<be<u32>>(dst);
    auto* s = Bmcon_view<be<u32>>(src);
    d[0] = s[0];
    d[1] = s[1];
    d[2] = s[2];
}
WWHD_SIZE(BmconAction, 8);
WWHD_OFFSET(BmconAction, d, 0);
WWHD_OFFSET(BmconAction, i, 2);
WWHD_OFFSET(BmconAction, f, 4);
WWHD_SIZE(BmconVirtualAction, 8);
WWHD_OFFSET(BmconVirtualAction, mVtableOffset, 6);
WWHD_OFFSET(BmconMessage, mSelectNum, 0x948);
WWHD_SIZE(BmconMessage, 0x94C);
WWHD_OFFSET(BmconVirtualNpc, mpVtable, 0xB4);
WWHD_SIZE(BmconVirtualNpc, 0xB8);
WWHD_OFFSET(BmconPlayerFlag, mStatus, 0x3BC);
WWHD_SIZE(BmconPlayerFlag, 0x3C0);
WWHD_OFFSET(BmconModelScale, mScale, 0xBC);
WWHD_SIZE(BmconModelScale, 0xC8);
WWHD_OFFSET(BmconAcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(BmconAcchVtables, mPolyVtable, 0x20);
WWHD_SIZE(BmconAcchVtables, 0x24);

s16 Bmcon_calcTimer(be<s16>*);
void Bmcon_playerEyePos(cXyz*, f32);
void Bmcon_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
void Bmcon_distanceAngle(cXyz*, cXyz*, be<f32>*, be<s16>*);
cPhs_State Bmcon_phaseHandler(request_of_phase_process_class*, void*, void*);
void Bmcon_npcCtor(void*);
void Bmcon_btpCtor(void*);
BOOL Bmcon_orderChangeEvent(fopAc_ac_c*, fopAc_ac_c*, s16, u16, u16);
BOOL Bmcon_isCreating(u32);
u32 Bmcon_messageStatus(void*);
void Bmcon_setMessageStatus(void*, u32);
u32 Bmcon_messageSet(void*, u32, cXyz*);
void Bmcon_sound(u32);
void Bmcon_bgmStart(u32);
void Bmcon_bgmStop(u32);
u32 Bmcon_presentItem(cXyz*, s32, u8, s32, s32, csXyz*, cXyz*);
void Bmcon_cutEnd(dEvent_manager_c*, s32);
BOOL Bmcon_cutAdvance(dEvent_manager_c*, s32);
void Bmcon_subtract(const cXyz*, const cXyz*, cXyz*);
void Bmcon_nextStage(const char*, s16, s8, s8, f32, u32, s32, s8);
void Bmcon_circleDestroy(void*, s32);
void Bmcon_acchDestroy(dBgS_Acch*, s32);
void Bmcon_actorDestroy(daNpcBmcon_c*, s32);
void Bmcon_shock(dVibration_c*, s32, s32, cXyz*);

// HD: this copy addresses the embedded matrix even for a zero model address.
struct BmconModelMatrix {
    u8 _0[0xC8];
    Mtx34 mBaseMatrix;
};
WWHD_OFFSET(BmconModelMatrix, mBaseMatrix, 0xC8);
WWHD_SIZE(BmconModelMatrix, 0xF8);

WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mMaxHeadX, 0x00);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mMaxHeadY, 0x02);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mMaxBackBoneX, 0x04);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mMaxBackBoneY, 0x06);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mMinHeadX, 0x08);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mMinHeadY, 0x0A);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mMinBackBoneX, 0x0C);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mMinBackBoneY, 0x0E);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mMaxTurnStep, 0x10);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mPlayerEyeOffset, 0x14);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mAttOffsetY, 0x18);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mEyeHeight, 0x1C);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mAttDist, 0x20);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mLookDist, 0x24);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mAttAngle, 0x28);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mTurnSpeed, 0x2A);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mWalkTurnSpeed, 0x2C);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mCollisionRadius, 0x30);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mWalkAnmRate, 0x34);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mWalkSpeed, 0x38);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mTurnWaitMin, 0x44);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mTurnWaitMax, 0x46);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mLookTimer, 0x48);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mbTurnOnLook, 0x4A);
WWHD_OFFSET(daNpcBmcon_c__l_npc_dat, mbLookPlayer, 0x4B);
