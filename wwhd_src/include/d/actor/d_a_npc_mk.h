/* daNpc_Mk_c (Ivan, Outset hide-and-seek boy), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_mk: daNpc_Mk_c derives from
 * fopAc_ac_c directly, so its members are +0x11C up to mpMorf; mShadowId (GameCube 0x29C) is
 * gone (HD shadows), so m_head_tex_pattern/mBtpAnm are +0x118; mDoExt_btpAnm grew from 0x14 to
 * 0x74, so everything from mBlinkFrame on is +0x178 up to mCurrActionFunc; the pointer to member
 * function is 8 bytes (GHS) instead of 12, so the members after it are +0x174. Size 0x87C
 * (GameCube 0x708). */
#pragma once
#include "gc_api/gc_api.h"
#include "d/actor/d_a_npc_mk_static.h"

enum dRes_INDEX_MK {
    /* BCKS */
    dRes_INDEX_MK_BCK_MK_DA_e=0x5,
    dRes_INDEX_MK_BCK_MK_JIDA01_e=0x6,
    dRes_INDEX_MK_BCK_MK_JIDA02_e=0x7,
    dRes_INDEX_MK_BCK_MK_JUMP_e=0x8,
    dRes_INDEX_MK_BCK_MK_KEIKAI_e=0x9,
    dRes_INDEX_MK_BCK_MK_LAND_e=0xA,
    dRes_INDEX_MK_BCK_MK_RUN_e=0xB,
    dRes_INDEX_MK_BCK_MK_TALK01_e=0xC,
    dRes_INDEX_MK_BCK_MK_TALK02_e=0xD,
    dRes_INDEX_MK_BCK_MK_TIRABARE_e=0xE,
    dRes_INDEX_MK_BCK_MK_WAIT_e=0xF,
    dRes_INDEX_MK_BCK_MK_WALK_e=0x10,
    /* BDLM */
    dRes_INDEX_MK_BDL_MK_e=0x13,
    /* BTP  */
    dRes_INDEX_MK_BTP_MK_MABA_e=0x16,
};

struct daNpc_Mk_c : fopAc_ac_c {
    using ActionFunc = gabi::ptmf<BOOL (daNpc_Mk_c::*)(void*)>;
    enum TalkStates { TALK_FINISHED = -1, TALK_INIT = 0, TALK_MSG_CREATE = 1, TALK_ACTIVE = 2 };
    enum ActionStatus { ACTION_STARTING = 0, ACTION_ONGOING = 1, ACTION_ENDING = -1 };
    enum VisitModes {
        VISIT_START = 0,
        VISIT_TALK = 1,
        VISIT_WALK_PATH = 2,
        VISIT_RUN_LINK = 3,
        VISIT_WALK_AROUND_LINK = 4,
        VISIT_REACHED_LINK = 5,
        VISIT_NOTICE_LINK = 6,
        VISIT_LEFT_PATH = 7,
        VISIT_WALK_PATH_FAST = 8,
        VISIT_WALK_PATH_IGNORE_LINK = 9,
    };
    enum States {
        STATE_WAIT = 0,
        STATE_TALK01 = 1,
        STATE_TALK02 = 2,
        STATE_VISIT = 3,
        STATE_CLIMB01 = 4,
        STATE_DROP01 = 5,
        STATE_RUNAWAY = 6,
        STATE_JUMP = 7,
        STATE_JITANDA01 = 8,
        STATE_JITANDA02 = 9,
        STATE_DEMO01 = 10,
        STATE_DEMO02 = 11,
        STATE_DEMO03 = 12,
        STATE_13 = 13,
    };
    enum Types { TYPE_NONE = 0, TYPE_NORMAL = 1, TYPE_MINIGAME = 2 };

    u32 ChkOrder(u8 flag) { return (u8)mOrderFlags & flag; }
    void ClrOrder() { mOrderFlags = 0; }
    void SetOrder(u8 v) { mOrderFlags = (u8)(mOrderFlags | v); }
    bool chkFlag(u16 flag) { return (mFlags & flag) == flag; }
    void clrFlag(u16 flag) { mFlags = (u16)(mFlags & ~flag); }
    void setFlag(u16 flag) { mFlags = (u16)(mFlags | flag); }
    bool isMorf(); /* mDoExt_McaMorf::isMorf: mCurMorf (+0xB0) < 1.0 */
    void setAction(u32 actionFunc); /* inline setAction(&daNpc_Mk_c::xxx_action, NULL) */

    BOOL initTexPatternAnm(u32); /* bool */
    void playTexPatternAnm();
    void setAnm(s8);
    void setAnmStatus();
    bool chkAttentionLocal();
    void chkAttention();
    void eventOrder();
    void checkOrder();
    u32 next_msgStatus(be<u32>*); /* u16 */
    u32 getMsg();
    void setCollision();
    void msgAnm(u8);
    void talkInit();
    void msgPushButton();
    u16 talk();
    BOOL checkDemoStart();
    s32 getNowEventAction();
    BOOL init();
    u8 getType();
    u8 getPath();
    u8 nextVisitMode();
    void visitInit(u8);
    void visitProc();
    void runawayInit();
    void walkPath(u8);
    void runLink();
    void aroundLink();
    void remove_Um2();
    void demoInitCom();
    bool demoProc();
    void setAttention(bool);
    u8 getLookBackMode();
    void lookBack();
    bool wait01();
    bool talk01();
    bool talk02();
    u8 visitTalkInit();
    void visitSetEvent();
    bool visit01();
    bool climb01();
    bool drop01();
    bool runaway();
    bool jump();
    bool jitanda01();
    bool jitanda02();
    bool demo01();
    bool demo02();
    bool demo03();
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
    /* 0x3B8 */ gptr<J3DAnmTexPattern> m_head_tex_pattern; /* HD: no mShadowId before it */
    /* 0x3BC */ mDoExt_btpAnm mBtpAnm;                           /* mDoExt_btpAnm (HD 0x74) */
    /* 0x430 */ be<u8> mBlinkFrame;
    /* 0x431 */ u8 _431;
    /* 0x432 */ be<s16> mBlinkTimer;
    /* 0x434 */ dBgS_ObjAcch mObjAcch;
    /* 0x5F8 */ dBgS_AcchCir mAcchCir;
    /* 0x638 */ dCcD_Stts mStts;
    /* 0x674 */ dCcD_Cyl mCyl;
    /* 0x7A4 */ dNpc_JntCtrl_c m_jnt;
    /* 0x7D8 */ cXyz mEyePos;
    /* 0x7E4 */ cXyz mAttnBasePos;
    /* 0x7F0 */ be<s16> mMaxHeadTurnVelocity;
    /* 0x7F2 */ be<s8> mAnmEnded;
    /* 0x7F3 */ be<u8> mAttnSetCount;
    /* 0x7F4 */ be<f32> mAnmTimer;
    /* 0x7F8 */ be<u32> mCurrMsgNo;
    /* 0x7FC */ be<u16> mFlags;
    /* 0x7FE */ be<u8> mAttentionTimer;
    /* 0x7FF */ be<u8> mMsgSelectNum;
    /* 0x800 */ MkPath_l field_0x688; /* dNpc_PathRun_c */
    /* 0x808 */ MkPath_l field_0x690;
    /* 0x810 */ MkPath_l field_0x698;
    /* 0x818 */ be<u8> mVisitMode;
    /* 0x819 */ be<u8> mWaitTimer;
    /* 0x81A */ be<u8> mRunAroundLinkTimer;
    /* 0x81B */ be<u8> mTimerToReachLink;
    /* 0x81C */ be<u16> field_0x6A4;
    /* 0x81E */ u8 _81E[2];
    /* 0x820 */ ActionFunc mCurrActionFunc;
    /* 0x828 */ be<s8> mTexPatternIdx;
    /* 0x829 */ be<s8> mAnmIdx;
    /* 0x82A */ u8 _82A[2];
    /* 0x82C */ be<s32> mStaffIdx;
    /* 0x830 */ be<s32> mEventAction;
    /* 0x834 */ be<u8> mOrderFlags;
    /* 0x835 */ be<s8> mState;
    /* 0x836 */ be<s8> mPrevState;
    /* 0x837 */ be<s8> mType;
    /* 0x838 */ be<s8> mActionStatus;
    /* 0x839 */ be<s8> mTalkState;
    /* 0x83A */ be<u8> mMsgAnmIdx;
    /* 0x83B */ u8 _83B;
    /* 0x83C */ be<u32> mMtrlSndId;
    /* 0x840 */ be<s8> mReverb;
    /* 0x841 */ u8 _841[3];
    /* 0x844 */ cXyz field_0x6D0;
    /* 0x850 */ cXyz field_0x6DC;
    /* 0x85C */ be<s16> field_0x6E8;
    /* 0x85E */ be<s16> mEventIdx;
    /* 0x860 */ be<u32> mpName;
    /* 0x864 */ be<s16> field_0x6F0;
    /* 0x866 */ u8 _866[2];
    /* 0x868 */ be<u32> mMsgNo;
    /* 0x86C */ daNpc_Mk_Static_c mMkStatic;
};
WWHD_OFFSET(daNpc_Mk_c, mBtpAnm, 0x3BC);
WWHD_OFFSET(daNpc_Mk_c, mObjAcch, 0x434);
WWHD_OFFSET(daNpc_Mk_c, mAcchCir, 0x5F8);
WWHD_OFFSET(daNpc_Mk_c, mStts, 0x638);
WWHD_OFFSET(daNpc_Mk_c, mCyl, 0x674);
WWHD_OFFSET(daNpc_Mk_c, m_jnt, 0x7A4);
WWHD_OFFSET(daNpc_Mk_c, mFlags, 0x7FC);
WWHD_OFFSET(daNpc_Mk_c, field_0x688, 0x800);
WWHD_OFFSET(daNpc_Mk_c, mCurrActionFunc, 0x820);
WWHD_OFFSET(daNpc_Mk_c, mOrderFlags, 0x834);
WWHD_OFFSET(daNpc_Mk_c, mMtrlSndId, 0x83C);
WWHD_OFFSET(daNpc_Mk_c, mpName, 0x860);
WWHD_OFFSET(daNpc_Mk_c, mMkStatic, 0x86C);
WWHD_SIZE(daNpc_Mk_c, 0x87C);

// HD: exact-width private bindings used by this actor.
BOOL mk_fopAcM_orderChangeEventId(fopAc_ac_c* a, u32 idx, u16 flag, u16 hind);
BOOL mk_fopAcM_orderOtherEventId_r(fopAc_ac_c* a, u32 idx, u8 tool, u16 p3, u16 prio, u16 flag);
fopAc_ac_c* mk_fopAcM_searchFromName(const char* name, u32 mask, u32 prm);
fopAc_ac_c* mk_fopAcM_getTalkEventPartner(fopAc_ac_c* a);
s32 mk_fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 flag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale);
void mDoAud_seStart_noPos(u32 id);
u16 msgMgr_getStatus(u32 m);
void msgMgr_setStatus(u32 m, u32 st);
u32 msgMgr_messageSet(u32 m, u32 msgNo, cXyz* pos);
void dNpc_PathRun_setInf(MkPath_l* p, u8 path, s8 room, u8 fwd);
u32 dNpc_PathRun_nextPath(MkPath_l* p, s8 room);
void dNpc_PathRun_setInfDrct(MkPath_l* p, u32 path);
BOOL dNpc_PathRun_chkInside(MkPath_l* p, cXyz* pos);
void dNpc_PathRun_setNearPathIndx(MkPath_l* p, cXyz* pos, f32 r);
s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry);
bool dAttention_LockonTruth(dAttention_c* a);
fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i);
fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i);
void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly);
s8 JUTNameTab_getIndex(u32 tab, const char* name);
s8 dBgS_GetRoomId(dBgS* bgs, void* poly);
u8 dBgS_GetPolyColor(dBgS* bgs, void* poly);
u32 dBgS_GetMtrlSndId_l(dBgS* bgs, void* poly);
void McaMorf_entry(mDoExt_McaMorf* m);

// HD: model-data animation owner and the player flag used by this actor.
struct MkModelDataView {
    /* 0x00 */ u8 pad[0x38];
    /* 0x38 */ be<u32> mpTexNoAnimator;
};
WWHD_OFFSET(MkModelDataView, mpTexNoAnimator, 0x38);
WWHD_SIZE(MkModelDataView, 0x3C);
struct MkPlayerView {
    /* 0x000 */ u8 pad[0x3C0];
    /* 0x3C0 */ be<u32> mPlayerFlag;
};
WWHD_OFFSET(MkPlayerView, mPlayerFlag, 0x3C0);
WWHD_SIZE(MkPlayerView, 0x3C4);
struct MkMessageManager {
    /* 0x000 */ u8 pad[0x948];
    /* 0x948 */ be<u32> mSelectNum;
};
WWHD_OFFSET(MkMessageManager, mSelectNum, 0x948);
WWHD_SIZE(MkMessageManager, 0x94C);
struct MkEventInteger {
    /* 0x00 */ be<s16> mHighHalf;
    /* 0x02 */ be<s16> mLowHalf;
};
WWHD_OFFSET(MkEventInteger, mLowHalf, 2);
WWHD_SIZE(MkEventInteger, 4);
void mk_setEventName(dEvt_info_c*, const char*);
void mk_constructJntCtrl(dNpc_JntCtrl_c*);
s32 mk_getMyActIdx(dEvent_manager_c*, s32, u32, s32, s32, s32);
u32 mk_getEventIdx(dEvent_manager_c*, const char*, u8);

// HD: the texture animation retains its virtual frame-count query.
struct MkTexPatternVtable {
    /* 0x00 */ u8 pad[0x14];
    /* 0x14 */ gptr<void> mGetFrameMax;
};
WWHD_OFFSET(MkTexPatternVtable, mGetFrameMax, 0x14);
WWHD_SIZE(MkTexPatternVtable, 0x18);
struct MkTexPatternView {
    /* 0x00 */ u8 pad[4];
    /* 0x04 */ gptr<MkTexPatternVtable> mpVtable;
    s32 getFrameMax() { return gabi::call_ptr<s32>(gabi::ea(mpVtable->mGetFrameMax.get()), this); }
};
WWHD_OFFSET(MkTexPatternView, mpVtable, 4);
WWHD_SIZE(MkTexPatternView, 8);
WWHD_SIZE(daNpc_Mk_c::ActionFunc, 8);
