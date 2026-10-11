/* daNpc_Tc_c (Tingle, Ankle, David Jr., Knuckle), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the WWHD code of d_a_npc_tc:
 * - base fopNpc_npc_c 0x7DC (d/d_npc.h), so members are +0x118 up to mStatus (0x7E7);
 * - the pointer-to-member mCurrActionFunc is 8 bytes (GHS) instead of 12: mActionStatus 0x7F0;
 * - HD-only s32 at 0x7F4 (a frame counter in statusDemoRescue/help_action: light tev colour);
 * - mSmokePos .. mPhs: +0x118 again (mSmokePos 0x7F8, mSmokeCallBack 0x80C, follow 0x82C/0x840,
 *   mPhs 0x854);
 * - mShadowId (GameCube 0x744) is gone (HD shadows): m_head_tex_pattern 0x85C;
 * - mDoExt_btpAnm grew from 0x14 to 0x74 (constructor 025E7820): everything from mBlinkFrame on
 *   is +0x174.
 * Size 0x990 (profile g_profile_NPC_TC at 0x101C6874, method table 0x101C67EC; GameCube 0x81C).
 * The HD vtable at 0xB4 is 0x10022D20 (stored by _create). */
#pragma once
#include "gc_api/JSystem/JPAEmitter.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
#include "gc_api/f_op/f_op_msg.h"
struct TcActionFunc {
    be<s16> d, i;
    be<u32> f;
};
WWHD_SIZE(TcActionFunc, 8);

struct dPa_smokeEcallBack_tc {      /* 0x00 */
    be<u32> __vtbl;                 /* 0x04 */
    gptr<JPABaseEmitter> mpEmitter; /* 0x08 */
    u8 _08[0x18];
};
WWHD_SIZE(dPa_smokeEcallBack_tc, 0x20);

struct daNpc_Tc_c : fopNpc_npc_c {
    enum Status {
        STATUS_WAIT = 1,
        STATUS_SIT = 2,
        STATUS_TALK = 3,
        STATUS_JUMP = 4,
        STATUS_WALK_TO_JAIL = 5,
        STATUS_TALK_NEAR_JAIL = 6,
        STATUS_WAIT_NEAR_JAIL = 7,
        STATUS_WALK_TO_STOOL = 8,
        STATUS_SIT_TO_STOOL = 9,
        STATUS_DEMO_JUMP = 10,
        STATUS_DEMO_RESCUE = 11,
        STATUS_DEMO_TALK = 12,
        STATUS_PAY_RUPEE = 13,
        STATUS_DEMO_PAY_RUPEE = 14,
        STATUS_GET_RUPEE = 15,
        STATUS_DEMO_GET_RUPEE = 16,
        STATUS_MONUMENT_COMPLETE = 17,
        STATUS_DEMO_MONUMENT_COMPLETE = 18,
    };
    enum ActionStatus { ACTION_STARTING = 0, ACTION_ONGOING = 1, ACTION_ENDING = -1 };
    enum Type { TYPE_NORMAL2 = 0, TYPE_RED = 1, TYPE_WHITE = 2, TYPE_BLUE = 3, TYPE_NORMAL = 4 };

    enum BckIdx {
        BCK_IDX_WAIT01 = 0,
        BCK_IDX_WAIT03 = 1,
        BCK_IDX_WAIT02 = 2,
        BCK_IDX_TALK01 = 3,
        BCK_IDX_WALK01 = 4,
        BCK_IDX_JAMP_A = 5,
        BCK_IDX_JAMP_B = 6,
        BCK_IDX_JAMP_C = 7,
        BCK_IDX_GUARD = 8,
        BCK_IDX_JTBT = 9,
        BCK_IDX_HAPPY = 10,
        BCK_IDX_DANCE01 = 11,
        BCK_IDX_DANCE02 = 12,
        BCK_IDX_GET = 13,
        BCK_IDX_MAWASU = 14,
        BCK_IDX_WAIT04 = 15,
        BCK_IDX_TALK02 = 16,
    };

    enum AnmPrmIdx {
        ANM_PRM_IDX_NONE = 0,
        ANM_PRM_IDX_WAIT01 = 1,
        ANM_PRM_IDX_WAIT03 = 2,
        ANM_PRM_IDX_WAIT02 = 3,
        ANM_PRM_IDX_TALK01 = 4,
        ANM_PRM_IDX_WALK01 = 5,
        ANM_PRM_IDX_JAMP_A = 6,
        ANM_PRM_IDX_JAMP_B = 7,
        ANM_PRM_IDX_JAMP_C1 = 8,
        ANM_PRM_IDX_JAMP_C2 = 9,
        ANM_PRM_IDX_GUARD = 10,
        ANM_PRM_IDX_JTBT = 11,
        ANM_PRM_IDX_HAPPY = 12,
        ANM_PRM_IDX_DANCE01 = 13,
        ANM_PRM_IDX_DANCE02 = 14,
        ANM_PRM_IDX_GET = 15,
        ANM_PRM_IDX_HAPPY2 = 16, // ANM_PRM_IDX_HAPPY gets set to this
        ANM_PRM_IDX_DANCE01_TO_TALK01 = 17,
        ANM_PRM_IDX_DANCE02_TO_WAIT03 = 18,
        ANM_PRM_IDX_MAWASU = 19,
        ANM_PRM_IDX_WAIT04 = 20,
        ANM_PRM_IDX_TALK02 = 21,
        ANM_PRM_IDX_TALK01_WAIT01 = 22, // loops 3x then -> WAIT01
        ANM_PRM_IDX_TALK01_TALK02 = 23, // loops 5x then -> TALK02
    };

    /* ---- methods (types follow the GameCube header; adjust to what the WWHD code reads) ---- */
    /* main (d_a_npc_tc.cpp) */
    fopAc_ac_c* _searchTower(fopAc_ac_c*);
    BOOL initTexPatternAnm(u32); /* bool, passed on unnormalised */
    void playTexPatternAnm();
    void smoke_set(f32, f32, f32, f32, f32);
    void setAnm();
    u32 setTexAnm();
    u8 chkAttention(cXyz*,
                    s16); /* bool (stored as is); cXyz by value: GHS passes a pointer to a copy */
    void eventOrder();
    void checkOrder();
    void anmAtr(u16);
    void startTower();
    void getArg();
    void createInit();
    static void* searchStoolPos(void*, void*);
    void setAttention();
    void calcMove();
    void lookBack();
    void statusWait();
    void statusSit();
    void statusJump();
    void statusWalkToJail();
    void statusTalkNearJail();
    void statusWaitNearJail();
    void statusWalkToStool();
    void statusSitToStool();
    void statusTalk();
    void statusPayRupee();
    void statusDemoPayRupee();
    void statusGetRupee();
    void statusDemoGetRupee();
    void statusMonumentComplete();
    void statusDemoMonumentComplete();
    void statusDemoJump();
    void statusDemoRescue();
    void statusDemoTalk();
    BOOL help_action(void*);
    BOOL wait_action(void*);
    void calc_sitpos();
    void set_mtx();
    BOOL _draw();
    void setTower();
    BOOL _execute();
    BOOL _delete();
    bool isCreate();
    cPhs_State _create();
    BOOL _createHeap();
    /* messages and cuts (d_a_npc_tc_msg.cpp / d_a_npc_tc_cut.cpp) */
    bool existTcMonument();
    void stopTower();
    u16 next_msgStatusBlue(be<u32>*);
    u16 next_msgStatusNormal(be<u32>*);
    u16 next_msgStatus(be<u32>*);
    u32 setFirstMsg(be<u8>*, u32, u32);
    u32 getMsgNormal();
    u32 getMsgBlue();
    u32 getMsg();
    u16 next_msgStatusNormal2(be<u32>*);
    u32 getMsgNormal2();
    u32 setRupeeSizeMsg();
    bool existUnknownCollectMap();
    int analysisCollectMap();
    bool existKnownCollectMap();
    u16 next_msgStatusWhite(be<u32>*);
    u32 getMsgWhite();
    u16 next_msgStatusRed(be<u32>*);
    u32 setFinishMsg();
    u32 getMsgRed();
    bool checkMonumentFee(u16, u16);
    bool checkAllMonumentFee();
    int checkAllMonumentPay();
    bool checkMonumentPay(u16, u16);
    void cutProc();
    void cutSitToJumpStart(int);
    void cutSitToJumpProc(int);
    void cutPresentStart(int);
    void cutPresentProc(int);
    void cutSetAnmStart(int);
    void cutSetAnmProc(int);
    void cutBackJumpStart(int);
    void cutBackJumpProc(int);
    void cutEffectStart(int);
    void cutEffectProc(int);
    void cutDoorOpenStart(int);
    void cutDoorOpenProc(int);
    void cutDoorCloseStart(int);
    void cutDoorCloseProc(int);
    void cutDoorClose2Start(int);
    void cutDoorClose2Proc(int);
    void cutPayStart(int);
    void cutPayProc(int);

    /* ---- layout ---- */
    /* 0x7DC */ u8 field_0x6C4[4];
    /* 0x7E0 */ be<s32> field_0x6C8; /* BOOL (anmAtr) */
    /* 0x7E4 */ be<u8> mJtbtTimer;
    /* 0x7E5 */ be<u8> mTalk01Wait01Timer;
    /* 0x7E6 */ be<u8> mTalk01Talk02Timer;
    /* 0x7E7 */ be<s8> mStatus;
    /* 0x7E8 */ TcActionFunc mCurrActionFunc; /* HD: 8 bytes (GameCube 12) */
    /* 0x7F0 */ be<s8> mActionStatus;
    /* 0x7F1 */ u8 _7F1[3];
    /* 0x7F4 */ be<s32> mRescueLightTimer; /* HD-only counter (statusDemoRescue, help_action) */
    /* 0x7F8 */ cXyz mSmokePos;
    /* 0x804 */ csXyz mSmokeAngle;
    /* 0x80A */ u8 _80A[2];
    /* 0x80C */ dPa_smokeEcallBack_tc mSmokeCallBack;
    /* 0x82C */ dPa_followEcallBack field_0x714;
    /* 0x840 */ dPa_followEcallBack field_0x728;
    /* 0x854 */ request_of_phase_process_class mPhs;
    /* 0x85C */ gptr<J3DAnmTexPattern> m_head_tex_pattern; /* HD: no mShadowId before it */
    /* 0x860 */ u8 mBtpAnm[0x74];                          /* mDoExt_btpAnm (HD 0x74) */
    /* 0x8D4 */ be<u8> mBlinkFrame;
    /* 0x8D5 */ u8 _8D5;
    /* 0x8D6 */ be<s16> mBlinkTimer;
    /* 0x8D8 */ cXyz mEyePos;
    /* 0x8E4 */ cXyz mAttPos;
    /* 0x8F0 */ be<u8> mAttnSetCount;
    /* 0x8F1 */ u8 _8F1[3];
    /* 0x8F4 */ cXyz mStoolLookPos;
    /* 0x900 */ be<s16> mMaxHeadTurnVelocity;
    /* 0x902 */ u8 _902[2];
    /* 0x904 */ csXyz field_0x790;
    /* 0x90A */ u8 _90A[2];
    /* 0x90C */ be<s8> field_0x798;
    /* 0x90D */ u8 _90D[3];
    /* 0x910 */ be<f32> mTargetSpeed;
    /* 0x914 */ cXyz mStoolPos;
    /* 0x920 */ cXyz mSitPos;
    /* 0x92C */ cXyz mWalkToStoolPos;
    /* 0x938 */ cXyz mDeltaPos;
    /* 0x944 */ u8 field_0x7D0[0xC];
    /* 0x950 */ cXyz mParticlePos;
    /* 0x95C */ be<u8> mHasAttention;
    /* 0x95D */ be<u8> mTalkingNearJail;
    /* 0x95E */ u8 field_0x7EA[0x965 - 0x95E];
    /* 0x965 */ be<u8> mHasTalkedNearJail;
    /* 0x966 */ be<u8> mHasEnteredSitRadius;
    /* 0x967 */ be<s8> mJumpLandingTimer;
    /* 0x968 */ u8 field_0x7F4[2];
    /* 0x96A */ be<s8> mTexPatternNum;
    /* 0x96B */ be<s8> mTexPatternNumIdx;
    /* 0x96C */ be<s8> mBckIdx;
    /* 0x96D */ be<s8> mAnmPrmIdx;
    /* 0x96E */ be<s8> mOldAnmPrmIdx;
    /* 0x96F */ be<s8> mEventIdx;
    /* 0x970 */ be<u8> mType;
    /* 0x971 */ u8 field_0x7FD[0x978 - 0x971];
    /* 0x978 */ gptr<fopAc_ac_c> m_tower_actor; /* daObjSmplbg::Act_c* */
    /* 0x97C */ be<u8> field_0x808;
    /* 0x97D */ be<u8> field_0x809;
    /* 0x97E */ u8 field_0x80A;
    /* 0x97F */ be<u8> field_0x80B;
    /* 0x980 */ be<u8> field_0x80C;
    /* 0x981 */ be<u8> field_0x80D;
    /* 0x982 */ be<u8> field_0x80E;
    /* 0x983 */ be<u8> field_0x80F;
    /* 0x984 */ be<u8> field_0x810;
    /* 0x985 */ be<u8> field_0x811;
    /* 0x986 */ be<u8> field_0x812;
    /* 0x987 */ be<u8> field_0x813;
    /* 0x988 */ be<u8> field_0x814;
    /* 0x989 */ u8 field_0x815[0x990 - 0x989];
};
WWHD_OFFSET(daNpc_Tc_c, mCurrActionFunc, 0x7E8);
WWHD_OFFSET(daNpc_Tc_c, mSmokeCallBack, 0x80C);
WWHD_OFFSET(daNpc_Tc_c, mPhs, 0x854);
WWHD_OFFSET(daNpc_Tc_c, mBlinkFrame, 0x8D4);
WWHD_OFFSET(daNpc_Tc_c, mParticlePos, 0x950);
WWHD_OFFSET(daNpc_Tc_c, mType, 0x970);
WWHD_OFFSET(daNpc_Tc_c, m_tower_actor, 0x978);
WWHD_SIZE(daNpc_Tc_c, 0x990);

struct daNpc_Tc_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ dNpc_HIO_c mNpc;
    /* 0x2C */ be<f32> field_0x2C;
    /* 0x30 */ be<f32> field_0x30;
    /* 0x34 */ be<u8> field_0x34;
    /* 0x35 */ u8 _35[3];
    /* 0x38 */ be<f32> field_0x38;
    /* 0x3C */ be<f32> field_0x3C;
    /* 0x40 */ be<f32> field_0x40;
    /* 0x44 */ be<u8> field_0x44;
    /* 0x45 */ u8 _45[3];
    /* 0x48 */ be<f32> field_0x48;
    /* 0x4C */ be<f32> field_0x4C;
    /* 0x50 */ be<f32> field_0x50;
    /* 0x54 */ be<f32> field_0x54;
    /* 0x58 */ be<f32> field_0x58;
    /* 0x5C */ be<f32> field_0x5C;
    /* 0x60 */ cXyz field_0x60;
    /* 0x6C */ cXyz field_0x6C;
    /* 0x78 */ be<s16> field_0x78;
    /* 0x7A */ be<s16> field_0x7A;
    /* 0x7C */ be<u8> field_0x7C;
    /* 0x7D */ be<u8> field_0x7D;
    /* 0x7E */ be<u8> field_0x7E;
    /* 0x7F */ u8 field_0x7F[8];
    /* 0x87 */ u8 field_0x87[8];
    /* 0x8F */ u8 field_0x8F[5];
    /* 0x94 */ be<f32> field_0x94[24];
};
WWHD_SIZE(daNpc_Tc_HIO_c, 0xF4);

// HD: virtual frame count, animation-removal and destructor layouts.
struct TcAnmVtable {
    u8 _0[0x14];
    be<u32> mGetFrameMax;
};
struct TcAnm {
    u8 _0[4];
    gptr<TcAnmVtable> mpVtable;
};
struct TcModelAnimation {
    u8 _0[0x38];
    be<u32> mpBtpAnimation;
};
struct TcAcchVtables {
    u8 _0[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mPolyVtable;
};
WWHD_OFFSET(TcAnmVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(TcAnm, mpVtable, 4);
WWHD_OFFSET(TcModelAnimation, mpBtpAnimation, 0x38);
WWHD_OFFSET(TcAcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(TcAcchVtables, mPolyVtable, 0x20);
template <class T, class U> inline T* Tc_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline s32 Tc_frameMax(J3DAnmTexPattern* p) {
    auto* a = Tc_view<TcAnm>(p);
    auto* t = a->mpVtable.get();
    return gabi::call_ptr<s32>(t->mGetFrameMax, p);
}
struct TcVirtualAction {
    u8 _0[6];
    be<s16> mVtableOffset;
};
inline void Tc_initDebugColour(be<u32>* guard, be<u32>* dst, be<u32>* src) {
    if ((u32)*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, 4);
    }
}

struct TcTower {
    u8 _0[0x3F0];
    be<u8> mStop;
};
WWHD_OFFSET(TcTower, mStop, 0x3F0);
WWHD_SIZE(TcTower, 0x3F1);
struct TcSmokeVtable {
    u8 _0[0x44];
    be<u32> mRemove;
};
WWHD_OFFSET(TcSmokeVtable, mRemove, 0x44);
WWHD_SIZE(TcSmokeVtable, 0x48);
struct TcMainEmitter {
    u8 _0[0x68];
    be<f32> mInitialVelOmni;
    u8 _6c[4];
    be<f32> mInitialVelDir;
    u8 _74[0x220 - 0x74];
    cXyz mGlobalScale;
    u8 _22c[12];
    cXyz mGlobalParticleScale;
};
WWHD_OFFSET(TcMainEmitter, mInitialVelOmni, 0x68);
WWHD_OFFSET(TcMainEmitter, mInitialVelDir, 0x70);
WWHD_OFFSET(TcMainEmitter, mGlobalScale, 0x220);
WWHD_OFFSET(TcMainEmitter, mGlobalParticleScale, 0x238);
WWHD_SIZE(TcMainEmitter, 0x244);
struct TcEnvironment {
    u8 _0[0x10A4];
    be<u8> mColourOverride;
};
WWHD_OFFSET(TcEnvironment, mColourOverride, 0x10A4);
WWHD_SIZE(TcEnvironment, 0x10A5);
struct TcRescueState {
    u8 _0[0x243];
    be<u8> mbRescued;
};
WWHD_OFFSET(TcRescueState, mbRescued, 0x243);
WWHD_SIZE(TcRescueState, 0x244);
struct TcObjectName {
    be<char> mName[8];
    be<s16> procname;
    be<s8> argument;
};
WWHD_OFFSET(TcObjectName, procname, 8);
WWHD_OFFSET(TcObjectName, argument, 10);
WWHD_SIZE(TcObjectName, 11);
using TcGuestByte = be<u8>;
using TcGuestFloat = be<f32>;
inline void Tc_removeSmoke(dPa_smokeEcallBack_tc* p) {
    auto* table = gabi::at<TcSmokeVtable>(p->__vtbl);
    gabi::call_ptr(table->mRemove, p);
}
inline void Tc_callAction(void* self, TcActionFunc* action, void* arg) {
    s16 index = action->i;
    u32 object = gabi::ea(self) + (s32)(s16)action->d;
    if (index < 0)
        gabi::call_ptr<BOOL>(action->f, object, arg);
    else {
        s16 offset = Tc_view<TcVirtualAction>(action)->mVtableOffset;
        auto* vtable = gabi::at<be<u32>>(object + offset);
        auto* entry = gabi::at<be<u32>>((u32)*vtable + index * 8 + 4);
        gabi::call_ptr<BOOL>(*entry, object, arg);
    }
}

/* dLib_anm_prm_c (0x10) */
struct TcAnmPrm {
    /* 0x00 */ be<s8> mBckIdx;
    /* 0x01 */ be<s8> mNextPrmIdx;
    /* 0x02 */ be<s16> field_0x02;
    /* 0x04 */ be<f32> mMorf;
    /* 0x08 */ be<f32> mPlaySpeed;
    /* 0x0C */ be<s32> mLoopMode;
};
WWHD_SIZE(TcAnmPrm, 0x10);

void Tc_npcHioCtor(dNpc_HIO_c*);
s16 Tc_calcTimer(be<s16>*);
s32 Tc_btpInit(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32);
BOOL Tc_orderOtherEvent(fopAc_ac_c*, const char*, u16, u16);
BOOL Tc_isStageTbox(s32, s32);
TcObjectName* Tc_searchName(const char*);
BOOL Tc_endCheck(dEvent_manager_c*, const char*);
BOOL Tc_playerInCircle(cXyz*, f32, f32);
void Tc_addCalcPosXZ(cXyz*, cXyz*, f32, f32);
void Tc_playerEyePos(cXyz*, f32);
void Tc_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
void Tc_snapshot(u8, fopAc_ac_c*, cXyz*, s16, f32, f32, f32);
void Tc_npcCtor(daNpc_Tc_c*);
void Tc_smokeCtor(dPa_smokeEcallBack_tc*, s32);
void Tc_btpCtor(void*);
void Tc_circleDestroy(void*, s32);
void Tc_acchDestroy(dBgS_Acch*, s32);
void Tc_actorDestroy(daNpc_Tc_c*, s32);
void Tc_btpEntry(void*, J3DModelData*, u32);
u32 Tc_materialSound(dBgS*, cBgS_PolyInfo*);
s8 Tc_roomId(dBgS*, cBgS_PolyInfo*);
u8 Tc_polyColour(dBgS*, cBgS_PolyInfo*);
void Tc_monsterSound(u32, cXyz*, u32, u32, s32);
void Tc_setBcks(const char*, mDoExt_McaMorf*, be<s8>*, be<s8>*, be<s8>*, be<s32>*, TcAnmPrm*, s32,
                s32);
void Tc_cutProc(daNpc_Tc_c*);
inline BOOL tower_isStop(fopAc_ac_c* p) { return Tc_view<TcTower>(p)->mStop; }
inline void tower_offStop(fopAc_ac_c* p) { Tc_view<TcTower>(p)->mStop = 0; }
inline s16 Tc_profileName(void* p) {
    return p ? Tc_view<base_process_class>(p)->mProfName : (s16)0x7FFF;
}

void Tc_headerByteCtor0(be<u8>*);
void Tc_headerByteCtor1(be<u8>*);

struct TcAnimationTable {
    TcAnmPrm mEntries[24];
};
WWHD_SIZE(TcAnimationTable, 0x180);

WWHD_SIZE(TcAnmVtable, 0x18);
WWHD_SIZE(TcAnm, 8);
WWHD_SIZE(TcModelAnimation, 0x3C);
WWHD_SIZE(TcAcchVtables, 0x24);
WWHD_SIZE(TcVirtualAction, 8);
WWHD_OFFSET(daNpc_Tc_HIO_c, mNpc, 4);

WWHD_OFFSET(daNpc_Tc_c, field_0x6C4, 0x7DC);
WWHD_OFFSET(daNpc_Tc_c, field_0x6C8, 0x7E0);
WWHD_OFFSET(daNpc_Tc_c, mJtbtTimer, 0x7E4);
WWHD_OFFSET(daNpc_Tc_c, mTalk01Wait01Timer, 0x7E5);
WWHD_OFFSET(daNpc_Tc_c, mTalk01Talk02Timer, 0x7E6);
WWHD_OFFSET(daNpc_Tc_c, mStatus, 0x7E7);
WWHD_OFFSET(daNpc_Tc_c, mActionStatus, 0x7F0);
WWHD_OFFSET(daNpc_Tc_c, mRescueLightTimer, 0x7F4);
WWHD_OFFSET(daNpc_Tc_c, mSmokePos, 0x7F8);
WWHD_OFFSET(daNpc_Tc_c, mSmokeAngle, 0x804);
WWHD_OFFSET(daNpc_Tc_c, field_0x714, 0x82C);
WWHD_OFFSET(daNpc_Tc_c, field_0x728, 0x840);
WWHD_OFFSET(daNpc_Tc_c, m_head_tex_pattern, 0x85C);
WWHD_OFFSET(daNpc_Tc_c, mBtpAnm, 0x860);
WWHD_OFFSET(daNpc_Tc_c, mBlinkTimer, 0x8D6);
WWHD_OFFSET(daNpc_Tc_c, mEyePos, 0x8D8);
WWHD_OFFSET(daNpc_Tc_c, mAttPos, 0x8E4);
WWHD_OFFSET(daNpc_Tc_c, mAttnSetCount, 0x8F0);
WWHD_OFFSET(daNpc_Tc_c, mStoolLookPos, 0x8F4);
WWHD_OFFSET(daNpc_Tc_c, mMaxHeadTurnVelocity, 0x900);
WWHD_OFFSET(daNpc_Tc_c, field_0x790, 0x904);
WWHD_OFFSET(daNpc_Tc_c, field_0x798, 0x90C);
WWHD_OFFSET(daNpc_Tc_c, mTargetSpeed, 0x910);
WWHD_OFFSET(daNpc_Tc_c, mStoolPos, 0x914);
WWHD_OFFSET(daNpc_Tc_c, mSitPos, 0x920);
WWHD_OFFSET(daNpc_Tc_c, mWalkToStoolPos, 0x92C);
WWHD_OFFSET(daNpc_Tc_c, mDeltaPos, 0x938);
WWHD_OFFSET(daNpc_Tc_c, field_0x7D0, 0x944);
WWHD_OFFSET(daNpc_Tc_c, mHasAttention, 0x95C);
WWHD_OFFSET(daNpc_Tc_c, mTalkingNearJail, 0x95D);
WWHD_OFFSET(daNpc_Tc_c, field_0x7EA, 0x95E);
WWHD_OFFSET(daNpc_Tc_c, mHasTalkedNearJail, 0x965);
WWHD_OFFSET(daNpc_Tc_c, mHasEnteredSitRadius, 0x966);
WWHD_OFFSET(daNpc_Tc_c, mJumpLandingTimer, 0x967);
WWHD_OFFSET(daNpc_Tc_c, field_0x7F4, 0x968);
WWHD_OFFSET(daNpc_Tc_c, mTexPatternNum, 0x96A);
WWHD_OFFSET(daNpc_Tc_c, mTexPatternNumIdx, 0x96B);
WWHD_OFFSET(daNpc_Tc_c, mBckIdx, 0x96C);
WWHD_OFFSET(daNpc_Tc_c, mAnmPrmIdx, 0x96D);
WWHD_OFFSET(daNpc_Tc_c, mOldAnmPrmIdx, 0x96E);
WWHD_OFFSET(daNpc_Tc_c, mEventIdx, 0x96F);
WWHD_OFFSET(daNpc_Tc_c, field_0x7FD, 0x971);
WWHD_OFFSET(daNpc_Tc_c, field_0x808, 0x97C);
WWHD_OFFSET(daNpc_Tc_c, field_0x809, 0x97D);
WWHD_OFFSET(daNpc_Tc_c, field_0x80A, 0x97E);
WWHD_OFFSET(daNpc_Tc_c, field_0x80B, 0x97F);
WWHD_OFFSET(daNpc_Tc_c, field_0x80C, 0x980);
WWHD_OFFSET(daNpc_Tc_c, field_0x80D, 0x981);
WWHD_OFFSET(daNpc_Tc_c, field_0x80E, 0x982);
WWHD_OFFSET(daNpc_Tc_c, field_0x80F, 0x983);
WWHD_OFFSET(daNpc_Tc_c, field_0x810, 0x984);
WWHD_OFFSET(daNpc_Tc_c, field_0x811, 0x985);
WWHD_OFFSET(daNpc_Tc_c, field_0x812, 0x986);
WWHD_OFFSET(daNpc_Tc_c, field_0x813, 0x987);
WWHD_OFFSET(daNpc_Tc_c, field_0x814, 0x988);
WWHD_OFFSET(daNpc_Tc_c, field_0x815, 0x989);