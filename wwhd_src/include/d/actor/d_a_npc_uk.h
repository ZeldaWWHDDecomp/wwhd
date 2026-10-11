/* daNpc_Uk_c (Jin, Jan and Jun-Roberto, the Killer Bees of Windfall), WWHD layout.
 *
 * The GameCube decompilation of this TU has only "Nonmatching" placeholders and an empty class,
 * so the whole layout is measured from the WWHD functions of d_a_npc_uk. daNpc_Uk_c derives from
 * fopAc_ac_c directly (members from 0x3AC); its HD vtable (0x10023050) holds only the virtual
 * destructor. Size 0x874 (profile g_profile_NPC_UK at 0x101C69C8). Names follow the sister
 * actor daNpc_Tt_c and the GameCube header's accessors. */
#pragma once
#include "gc_api/gc_api.h"
#include "gc_api/JSystem/JPAEmitter.h"

enum dRes_INDEX_UK {
    /* BCKS */
    dRes_INDEX_UK_BCK_UK_DA_e=0x7,
    dRes_INDEX_UK_BCK_UK_JIDA01_e=0x8,
    dRes_INDEX_UK_BCK_UK_JIDA02_e=0x9,
    dRes_INDEX_UK_BCK_UK_JUMP_e=0xA,
    dRes_INDEX_UK_BCK_UK_KEIKAI_e=0xB,
    dRes_INDEX_UK_BCK_UK_LAND_e=0xC,
    dRes_INDEX_UK_BCK_UK_RUN_e=0xD,
    dRes_INDEX_UK_BCK_UK_TALK01_e=0xE,
    dRes_INDEX_UK_BCK_UK_TALK02_e=0xF,
    dRes_INDEX_UK_BCK_UK_WAIT_e=0x10,
    dRes_INDEX_UK_BCK_UK_WALK_e=0x11,
    /* BDL  */
    dRes_INDEX_UK_BDL_UKHEAD_B_e=0x14,
    dRes_INDEX_UK_BDL_UKHEAD_D_e=0x15,
    /* BDLM */
    dRes_INDEX_UK_BDL_UK_e=0x18,
    dRes_INDEX_UK_BDL_UKHEAD_C_e=0x19,
    /* BMT  */
    dRes_INDEX_UK_BMT_UK_C_e=0x1C,
    dRes_INDEX_UK_BMT_UK_D_e=0x1D,
    /* BTP  */
    dRes_INDEX_UK_BTP_UK_MABA_C_e=0x20,
    dRes_INDEX_UK_BTP_UK_MABA_C_1_e=0x21,
};
struct daNpc_Uk_c;
using UkActionFunc = gabi::ptmf<BOOL(daNpc_Uk_c::*)(void*)>;
WWHD_SIZE(UkActionFunc, 8);
#include "d/actor/d_a_npc_mk_static.h" /* daNpc_Mk_Static_c, MkPath_l (dNpc_PathRun_c) */

struct daNpc_Uk_c : fopAc_ac_c {
    enum States {
        STATE_WAIT = 0,
        STATE_TALK = 1,
        STATE_VISIT = 2,
        STATE_DEMO = 3,
        STATE_EVENT = 4,
        STATE_HIDDEN = 5,
        STATE_WARNING_B = 6,
        STATE_WARNING_C = 7,
        STATE_WARNING_D = 8,
        STATE_FOUND = 9,
        STATE_RUNAWAY = 10,
        STATE_JUMP = 11,
        STATE_JITANDA01 = 12,
        STATE_JITANDA02 = 13,
    };
    enum Flags {
        FLAG_TALKING = 0x1,
        FLAG_WARNING = 0x2,
        FLAG_ATTENTION = 0x4,
        FLAG_PUSHED = 0x8,
        FLAG_HIDDEN = 0x10,
        FLAG_NO_HEAD_TRACK = 0x20,
        FLAG_LOOK_PARTNER = 0x40,
        FLAG_SET_TALK_PARTNER = 0x80,
        FLAG_KEEP_SHAPE_ANGLE = 0x100,
        FLAG_AIRBORNE = 0x200,
        FLAG_NO_MSG_ANIMATION = 0x400,
    };
    enum TalkStates {
        TALK_FINISHED=-1,
        TALK_INIT=0,
        TALK_MSG_CREATE=1,
        TALK_ACTIVE=2,
    };

    enum ActionStatus {
        ACTION_ENDING = -1,
        ACTION_STARTING = 0,
        ACTION_ONGOING = 1,
    };

    u32 ChkOrder(u8 flag) { return (u8)mOrderFlags & flag; }
    void ClrOrder() { mOrderFlags = 0; }
    void SetOrder(u8 flag) { mOrderFlags = (u8)(mOrderFlags | flag); }
    bool chkFlag(u16 flag) { return (mFlags & flag) != 0; }
    void clrFlag(u16 flag) { mFlags = (u16)(mFlags & ~flag); }
    void setFlag(u16 flag) { mFlags = (u16)(mFlags | flag); }
    f32 curMorf() { return mpMorf->mCurMorf; } /* mDoExt_McaMorf::mCurMorf */
    BOOL isMorf() { return curMorf() < 1.0f; }

    u8 getType();
    u8 getPath();
    u32 getShapeType();
    u16 getCaughtFlag();
    u16 getFoundFlag();
    u16 getFirstTalk();
    BOOL chkGameStart();
    BOOL chkPositioning(f32, f32, f32, s16, s16);
    u8 nextVisitMode();
    void approachRun(fopAc_ac_c*);
    void aroundWalk(fopAc_ac_c*);
    void surrender();
    void runawayInit();
    BOOL initTexPatternAnm(u32); /* bool, passed on unnormalised */
    void playTexPatternAnm();
    void setAnm(s8, u8);
    BOOL chkAttentionLocal();
    void chkAttention();
    void setAnmStatus();
    void eventOrder();
    void checkOrder();
    u32 next_msgStatus(be<u32>*); /* u16 */
    u32 getMsg();
    void setCollision();
    void msgAnm(u32); /* u8, compared unnormalised */
    void talkInit();
    u16 talk();
    BOOL init();
    void setAttention(u32); /* bool, tested unnormalised */
    u32 getLookBackMode();
    void lookBack();
    u32 getStaffName();
    BOOL checkDemoStart();
    s32 getNowEventAction();
    void visitInit(u32); /* u8, compared unnormalised */
    void visitProc();
    BOOL wait01();
    BOOL talk01();
    BOOL talk02();
    u32 visitTalkInit();
    void visitSetEvent();
    BOOL visit01();
    BOOL warningB();
    BOOL warningC();
    BOOL warningD();
    BOOL found();
    BOOL runaway();
    BOOL jump();
    BOOL jitanda01();
    BOOL jitanda02();
    BOOL demo01();
    BOOL demo02();
    void demoInitCom();
    BOOL demoProc();
    BOOL wait_action(void*);
    BOOL seek_action(void*);
    BOOL hind_action(void*);
    BOOL visit_action(void*);
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateHeap();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3B8 */ gptr<J3DModel> mpHeadModel;             /* head model, placed on the head joint */
    /* 0x3BC */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x3C0 */ mDoExt_btpAnm mBtpAnm;                       /* mDoExt_btpAnm (HD 0x74) */
    /* 0x434 */ be<u8> mBlinkFrame;
    /* 0x435 */ u8 _435;
    /* 0x436 */ be<s16> mBlinkTimer;
    /* 0x438 */ dBgS_ObjAcch mObjAcch;
    /* 0x5FC */ dBgS_AcchCir mAcchCir;
    /* 0x63C */ dCcD_Stts mStts;
    /* 0x678 */ dCcD_Cyl mCyl;
    /* 0x7A8 */ dNpc_JntCtrl_c m_jnt;
    /* 0x7DC */ cXyz mEyePos;
    /* 0x7E8 */ cXyz mAttnBasePos;
    /* 0x7F4 */ be<s16> mMaxHeadTurnVelocity;
    /* 0x7F6 */ be<s8> mAnmEnded;
    /* 0x7F7 */ be<u8> mAttnSetCount;
    /* 0x7F8 */ be<f32> mAnmTimer;
    /* 0x7FC */ be<u32> mCurrMsgNo;
    /* 0x800 */ be<u16> mFlags;
    /* 0x802 */ be<u8> mTalkWait;          /* frames without attention after a talk */
    /* 0x803 */ u8 _803;
    /* 0x804 */ be<u32> mPartnerId;        /* process id of the partner actor */
    /* 0x808 */ be<f32> mWalkSpeed;
    /* 0x80C */ be<f32> mRunSpeed;
    /* 0x810 */ MkPath_l mPath;            /* dNpc_PathRun_c */
    /* 0x818 */ UkActionFunc mCurrActionFunc;
    /* 0x820 */ be<s8> mTexPatternIdx;
    /* 0x821 */ be<s8> mCurrAnmIdx;
    /* 0x822 */ u8 _822[2];
    /* 0x824 */ be<s32> mStaffIdx;
    /* 0x828 */ be<s32> mEventAction;
    /* 0x82C */ be<u8> mOrderFlags;
    /* 0x82D */ be<s8> mState;
    /* 0x82E */ be<s8> mReturnToState;
    /* 0x82F */ be<u8> mVisitMode;
    /* 0x830 */ be<u8> mVisitTimer;
    /* 0x831 */ be<u8> mVisitTimer2;
    /* 0x832 */ be<u8> mApproachTimer;
    /* 0x833 */ be<s8> mType;
    /* 0x834 */ be<s8> mActionStatus;
    /* 0x835 */ be<s8> mTalkState;
    /* 0x836 */ be<u8> mMsgAnmIdx;
    /* 0x837 */ u8 _837;
    /* 0x838 */ be<u32> mMtrlSndId;
    /* 0x83C */ be<s8> mReverb;
    /* 0x83D */ u8 _83D[3];
    /* 0x840 */ cXyz mDemoStartPos;
    /* 0x84C */ cXyz mDemoTargetPos;
    /* 0x858 */ be<s16> mDemoTimer;
    /* 0x85A */ be<s16> mEventIdx;
    /* 0x85C */ be<s16> mDemoTargetAngle;
    /* 0x85E */ u8 _85E[2];
    /* 0x860 */ be<u32> mMsgNo;
    /* 0x864 */ daNpc_Mk_Static_c mMk;
};
WWHD_OFFSET(daNpc_Uk_c, mPhs, 0x3AC);
WWHD_OFFSET(daNpc_Uk_c, mBtpAnm, 0x3C0);
WWHD_OFFSET(daNpc_Uk_c, mObjAcch, 0x438);
WWHD_OFFSET(daNpc_Uk_c, mAcchCir, 0x5FC);
WWHD_OFFSET(daNpc_Uk_c, mStts, 0x63C);
WWHD_OFFSET(daNpc_Uk_c, mCyl, 0x678);
WWHD_OFFSET(daNpc_Uk_c, m_jnt, 0x7A8);
WWHD_OFFSET(daNpc_Uk_c, mFlags, 0x800);
WWHD_OFFSET(daNpc_Uk_c, mCurrActionFunc, 0x818);
WWHD_OFFSET(daNpc_Uk_c, mOrderFlags, 0x82C);
WWHD_OFFSET(daNpc_Uk_c, mDemoStartPos, 0x840);
WWHD_OFFSET(daNpc_Uk_c, mMsgNo, 0x860);
WWHD_OFFSET(daNpc_Uk_c, mMk, 0x864);
WWHD_SIZE(daNpc_Uk_c, 0x874);

// HD: private bindings preserve the original argument/result widths.
BOOL fopAcM_orderChangeEventId_raw(fopAc_ac_c* a, u32 ev, u16 flag, u16 hind);
BOOL fopAcM_orderOtherEventId_raw(fopAc_ac_c* a, u32 ev, u8 mapToolId, u16 p3, u16 prio, u16 flag);
fopAc_ac_c* uk_fopAcM_getTalkEventPartner(fopAc_ac_c* a);
fopAc_ac_c* uk_fopAcM_searchFromName(const char* name, u32 mask, u32 prm);
void mDoAud_seStart_1(u32 id);
u16 msgMgr_getStatus(u32 m);
void msgMgr_setStatus(u32 m, u32 st);
u32 msgMgr_messageSet(u32 m, u32 msgNo, cXyz* pos);
BOOL dNpc_PathRun_setInf(MkPath_l* p, u8 path, s8 room, u8 fwd);
s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry);
void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly);
s8 JUTNameTab_getIndex(u32 tab, const char* name);
s8 dBgS_GetRoomId(dBgS* bgs, void* poly);
u8 dBgS_GetPolyColor(dBgS* bgs, void* poly);
u32 dBgS_GetMtrlSndId_l(dBgS* bgs, void* poly);
bool dAttention_LockonTruth(u32 a);
fopAc_ac_c* dAttention_LockonTarget(u32 a, s32 i);
fopAc_ac_c* dAttention_ActionTarget(u32 a, s32 i);
u32 Mk_runAwayProc(daNpc_Mk_Static_c* m, fopAc_ac_c* a, MkPath_l* p, void* cyl, be<s16>* ang);

void uk_constructJntCtrl(dNpc_JntCtrl_c*);

// HD: private views of fields used by this actor.
struct UkModelDataView {
    /* 0x00 */ u8 pad[0x38];
    /* 0x38 */ be<u32> mpTexNoAnimator;
};
WWHD_OFFSET(UkModelDataView, mpTexNoAnimator, 0x38);
WWHD_SIZE(UkModelDataView, 0x3C);
struct UkPartnerView {
    /* 0x000 */ u8 pad[0x818];
    /* 0x818 */ be<u8> mVisitMode;
};
WWHD_OFFSET(UkPartnerView, mVisitMode, 0x818);
WWHD_SIZE(UkPartnerView, 0x819);
struct UkEventInteger {
    /* 0x00 */ be<s16> mHighHalf;
    /* 0x02 */ be<s16> mLowHalf;
};
WWHD_OFFSET(UkEventInteger, mLowHalf, 2);
WWHD_SIZE(UkEventInteger, 4);
struct UkTexPatternVtable {
    /* 0x00 */ u8 pad[0x14];
    /* 0x14 */ gptr<void> mGetFrameMax;
};
WWHD_OFFSET(UkTexPatternVtable, mGetFrameMax, 0x14);
WWHD_SIZE(UkTexPatternVtable, 0x18);
struct UkTexPatternView {
    /* 0x00 */ u8 pad[4];
    /* 0x04 */ gptr<UkTexPatternVtable> mpVtable;
    s32 getFrameMax() { return gabi::call_ptr<s32>(gabi::ea(mpVtable->mGetFrameMax.get()), this); }
};
WWHD_OFFSET(UkTexPatternView, mpVtable, 4);
WWHD_SIZE(UkTexPatternView, 8);
u32 uk_getEventIdx(dEvent_manager_c*, const char*, u8);
void uk_setEventName(dEvt_info_c*, const char*);
