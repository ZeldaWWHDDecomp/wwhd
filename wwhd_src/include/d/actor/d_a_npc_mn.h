/* daNpcMn_c (Manny, Windfall figure fan), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the constructor 0229BF48 (allocates 0x93C) and the verified
 * functions: fopNpc_npc_c is 0x7DC (d/d_npc.h), so members are +0x118 up to mBtpAnm (0x854);
 * mDoExt_btpAnm grew from 0x14 to 0x74 and mShadowId (GameCube 0x750) is gone (HD shadows), so
 * everything from mPathRun (GameCube 0x754) on is +0x174. Size 0x93C (GameCube 0x7C8). */
#pragma once
#include "gc_api/gc_api.h"

#define MN_SAFESTRING_VTBL gc_tu.safeString /* this TU's sead::SafeString vtable */
#define MN_VTBL 0x1001E914            /* daNpcMn_c vtable (constructor) */

struct sMnAnmDat {
    /* 0x00 */ be<u8> mBckIdx;
    /* 0x01 */ be<u8> mMorf;
    /* 0x02 */ be<s8> mLoopCount;
};
WWHD_SIZE(sMnAnmDat, 3);

struct NpcDatStruct {
    /* 0x00 */ be<s16> mMax_head_x;
    /* 0x02 */ be<s16> mMax_head_y;
    /* 0x04 */ be<s16> mMax_backbone_x;
    /* 0x06 */ be<s16> mMax_backbone_y;
    /* 0x08 */ be<s16> mMin_head_x;
    /* 0x0A */ be<s16> mMin_head_y;
    /* 0x0C */ be<s16> mMin_backbone_x;
    /* 0x0E */ be<s16> mMin_backbone_y;
    /* 0x10 */ be<s16> mMax_turn_step;
    /* 0x12 */ be<s16> field_0x12;
    /* 0x14 */ be<f32> mPlayerEyeOfsY;
    /* 0x18 */ be<f32> mAttnPosOfsY;
    /* 0x1C */ be<f32> mEyePosOfsY;
    /* 0x20 */ be<f32> mAttnDist;
    /* 0x24 */ be<f32> mNearDist;
    /* 0x28 */ be<s16> mAttnAngle;
    /* 0x2A */ be<s16> mLookAtMaxVel;
    /* 0x2C */ be<s16> mWalkLookAtMaxVel;
    /* 0x2E */ be<s16> field_0x2E;
    /* 0x30 */ be<f32> mCylRadius;
    /* 0x34 */ be<f32> mWalkAnmRate;
    /* 0x38 */ be<f32> mWalkSpeed;
    /* 0x3C */ be<s16> field_0x3C;
    /* 0x3E */ be<s16> field_0x3E;
    /* 0x40 */ be<s16> mWaitTimerMin;
    /* 0x42 */ be<s16> mWaitTimerMax;
    /* 0x44 */ be<s16> mTurnWaitMin;
    /* 0x46 */ be<s16> mTurnWaitMax;
    /* 0x48 */ be<s16> mLookResetTimer;
    /* 0x4A */ be<s8> mbAllowBodyTurn;
    /* 0x4B */ be<s8> mbLookOnly;
};
WWHD_SIZE(NpcDatStruct, 0x4C);

struct daNpcMn_c : fopNpc_npc_c {
    enum Prm_e { PRM_SWITCH_W = 8, PRM_SWITCH_S = 8, PRM_SWITCH2_W = 8, PRM_SWITCH2_S = 0x10, PRM_RAIL_W = 8, PRM_RAIL_S = 0x18 };
    enum MoveProcIdx { MOVE_PROC_WAIT = 0, MOVE_PROC_TALK = 1, MOVE_PROC_WALK = 2, MOVE_PROC_TURN = 3, MOVE_PROC_TALK3 = 4 };
    enum LookMode { LOOK_MODE_NONE = 0, LOOK_MODE_ATTN = 1, LOOK_MODE_TURN = 2 };
    enum Talk3State { TALK3_INIT = 0, TALK3_ORDER = 1, TALK3_TALK = 2 };
    enum BckIdx { BCK_WAIT01 = 0, BCK_WAIT02 = 1, BCK_TALK01 = 2, BCK_TALK02 = 3, BCK_WALK = 4, BCK_BIKKURI = 5, BCK_JUMP01 = 6, BCK_JUMP02 = 7, BCK_NULL = 0xFF };

    u8 getNpcNo() { return mNpcNo; }
    request_of_phase_process_class* getPhaseP() { return &mPhs; }
    void setResFlag(u8 flag) { mResFlag = flag; }

    /* the declarations follow the GameCube header; return types are what the WWHD code returns */
    cPhs_State _create();
    BOOL createHeap();
    cPhs_State createInit();
    bool _delete();
    bool _draw();
    bool _execute();
    u8 executeCommon();
    void executeSetMode(u32); /* u8, not re-extended */
    int executeWaitInit();
    void executeWait();
    int executeTalkInit();
    void executeTalk();
    int executeTalk3Init();
    void executeTalk3();
    int executeWalkInit();
    void executeWalk();
    int executeTurnInit();
    void executeTurn();
    void checkOrder();
    void eventOrder();
    void eventMove();
    void privateCut();
    void eventMesSetInit(int);
    bool eventMesSet();
    void eventGetItemInit();
    void eventWaitInit(int);
    bool eventWait(int);
    void eventSwOnInit(int);
    bool eventSwOn();
    void eventHatchInit();
    bool eventHatch();
    void eventBikkuriInit(int);
    bool eventBikkuri();
    u32 eventTurnInit();
    bool eventTurn(int);
    u32 eventWalkInit();
    bool eventWalk();
    u32 eventLookInit();
    bool eventLook();
    void eventJumpInit(int);
    bool eventJump();
    u16 talk2(int);
    u16 talk3(int);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    u32 getMsg3();
    void setMessage(u32);
    void setAnmFromMsgTag();
    u8 getPrmNpcNo(); /* bool */
    u8 getPrmRailID();
    u8 getPrmSwitchBit();
    u8 getPrmSwitchBit2();
    void setMtx();
    void chkAttention();
    void lookBack();
    BOOL initTexPatternAnm(u32); /* bool, passed on unnormalised */
    void playTexPatternAnm();
    void playAnm();
    void setAnm(u32, int, f32); /* u8 index, full register */
    bool setAnmTbl(sMnAnmDat*);
    s16 XyCheckCB(int);
    int getRand(int);
    void setCollision(dCcD_Cyl*, cXyz*, f32, f32); /* cXyz by value: pointer to a copy */
    BOOL chkEndEvent();
    u8 chkPosNo();
    u8 getPosNo();
    BOOL isChangePos(u32); /* u8, not re-extended */

    /* 0x7DC */ u8 field_0x6C4[0x60];
    /* 0x83C */ request_of_phase_process_class mPhs;
    /* 0x844 */ request_of_phase_process_class mPhsMethod;
    /* 0x84C */ gptr<J3DModel> mpModel;
    /* 0x850 */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x854 */ mDoExt_btpAnm mBtpAnm;                  /* mDoExt_btpAnm (HD 0x74, constructor 025E7820) */
    /* 0x8C8 */ dNpc_PathRun_c mPathRun;              /* HD: no mShadowId before it */
    /* 0x8D0 */ cXyz field_0x75C;
    /* 0x8DC */ cXyz mLookAtPos;
    /* 0x8E8 */ gptr<sMnAnmDat> mpAnmDat;
    /* 0x8EC */ gptr<be<u32>> mpMsgNo;
    /* 0x8F0 */ be<f32> mTargetSpeedF;
    /* 0x8F4 */ be<f32> mAnmMorfOverride;
    /* 0x8F8 */ be<f32> mAttnDist;
    /* 0x8FC */ be<f32> mJumpSpeedY;
    /* 0x900 */ be<u32> mItemNo;
    /* 0x904 */ be<u32> mFigureArg;
    /* 0x908 */ be<u8> mHeadOnlyFollow;
    /* 0x909 */ be<u8> field_0x795;
    /* 0x90A */ be<s16> mHatchEventIdx;
    /* 0x90C */ be<s16> mWaitTimer;
    /* 0x90E */ be<s16> mLookResetTimer;
    /* 0x910 */ be<s16> mEventTimer;
    /* 0x912 */ be<s16> mAttnAngle;
    /* 0x914 */ be<s16> mHomeYRot;
    /* 0x916 */ be<u16> mLastMsgStatus;
    /* 0x918 */ be<u16> mEtcFlag;
    /* 0x91A */ be<s16> field_0x7A6;
    /* 0x91C */ be<s16> mBtpTimer;
    /* 0x91E */ be<s16> mLookAtMaxVel;
    /* 0x920 */ be<s16> mTurnVel;
    /* 0x922 */ be<s16> mTargetYRot;
    /* 0x924 */ be<u8> mTalkOrder;
    /* 0x925 */ be<u8> mbPlayerAttention;
    /* 0x926 */ be<u8> mEvtOrderType;
    /* 0x927 */ be<u8> mBtpFrame;
    /* 0x928 */ be<u8> mMoveState;
    /* 0x929 */ be<u8> mResFlag;
    /* 0x92A */ be<u8> mNpcNo;
    /* 0x92B */ be<u8> mSwitchFlag;
    /* 0x92C */ be<u8> mBckIdx;
    /* 0x92D */ be<u8> mAnmFlag;
    /* 0x92E */ be<s8> mAnmLoopCnt;
    /* 0x92F */ be<s8> mActIdx;
    /* 0x930 */ be<u8> field_0x7BC;
    /* 0x931 */ be<s8> mLookMode;
    /* 0x932 */ be<u8> mbAllowBodyTurn;
    /* 0x933 */ be<u8> mbLookOnly;
    /* 0x934 */ be<u8> mbNearPlayer;
    /* 0x935 */ be<u8> mPosFlag;
    /* 0x936 */ be<s8> mShoulderRJoint;
    /* 0x937 */ be<u8> mTalk3State;
    /* 0x938 */ be<u8> mbLookFigure;
    /* 0x939 */ be<u8> mFigureMsgIdx;
    /* 0x93A */ u8 _93A[2];
};
WWHD_OFFSET(daNpcMn_c, mPhs, 0x83C);
WWHD_OFFSET(daNpcMn_c, mBtpAnm, 0x854);
WWHD_OFFSET(daNpcMn_c, mPathRun, 0x8C8);
WWHD_OFFSET(daNpcMn_c, mHomeYRot, 0x914);
WWHD_OFFSET(daNpcMn_c, mNpcNo, 0x92A);
WWHD_OFFSET(daNpcMn_c, mFigureMsgIdx, 0x939);
WWHD_SIZE(daNpcMn_c, 0x93C);

/* ---- translation-unit statics (.data / .rodata addresses) ---- */
extern be<s32> (&l_bmd_ix_tbl)[1];
extern be<s32> (&l_etc_bmd_ix_tbl)[1];
extern be<s32> (&l_btp_ix_tbl)[1];
extern be<s32> (&l_bck_ix_tbl)[8];
extern gptr<char> (&l_arcname_tbl)[1];
extern gptr<char> (&l_npc_staff_id)[1];
extern gptr<char> (&l_room_name)[10];
extern gptr<char> (&cut_name_tbl)[10];
extern be<u16> (&l_figure_comp)[17];
extern be<u32> (&l_msg_mn_1st_talk)[4];
extern be<u32> (&l_msg_mn_2nd_talk)[4];
extern be<u32> (&l_msg_mn_1st_talk_in)[2];
extern be<u32> (&l_msg_mn_2nd_talk_in)[2];
extern be<u32> (&l_msg_mn_3rd_talk_in)[2];
extern be<u32> (&l_msg_mn_comp_1st)[4];
extern be<u32> (&l_msg_mn_comp_2nd)[2];
extern be<u32> (&l_msg_mn_figure)[10];
extern sMnAnmDat (&l_npc_anm_wait)[1];
extern sMnAnmDat (&l_npc_anm_wait2)[1];
extern sMnAnmDat (&l_npc_anm_talk)[1];
extern sMnAnmDat (&l_npc_anm_talk2)[1];
extern sMnAnmDat (&l_npc_anm_walk)[1];
extern sMnAnmDat (&l_npc_anm_bikkuri)[2];
extern sMnAnmDat (&l_npc_anm_jump1)[2];
extern NpcDatStruct (&l_npc_dat)[2];
static inline const char* mn_arcname() { return l_arcname_tbl[0]; }
static inline NpcDatStruct* mn_npc_dat(u32 npcNo) { return &l_npc_dat[npcNo]; }

/* ---- local bindings (SHARED-CANDIDATE; the same as in d_a_npc_people.h unless noted) ---- */

// HD: preserve the additional live argument registers at getIDRes.
void* mn_getResourceWithRegisters(void* control, SafeString* key, s32 id, u32 r6, u32 r7);
static inline void* mn_getObjectIDResWithRegisters(const char* arc, s32 id, u32 r6, u32 r7) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = MN_SAFESTRING_VTBL;
    return mn_getResourceWithRegisters(dComIfG_resControl(), key.get(), id, r6, r7);
}
static inline void mn_set_r(int n, u32 v) { gabi::cpu->r[n] = v; }
s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry);

s8 JUTNameTab_getIndex(u32 tab, const char* name);

/* HD J3D joint matrices: J3DModel +0x2C is the matrix block (flags u16 at +4, matrices at +0x10) */

/* sead::SafeString equality (HD form of strcmp(a, b) == 0): the left string is terminated
 * through its vtable twice, the right one once, then compared by pointer, then byte by byte (at
 * most 0x40001 bytes) */
struct MnVirtualSlot {
    u8 _00[0x14];
    be<u32> mFunction;
};
struct MnActorVirtualTable {
    u8 _00[0x14];
    be<u32> next_msgStatus;
    u8 _18[4];
    be<u32> getMsg;
    u8 _20[4];
    be<u32> anmAtr;
};
struct MnAnimationView {
    u8 _00[4];
    gptr<MnVirtualSlot> mVtable;
};

static inline bool mn_SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(reinterpret_cast<MnVirtualSlot*>((uintptr_t)(u32)a->__vtbl)->mFunction, a);
    gabi::call_ptr(reinterpret_cast<MnVirtualSlot*>((uintptr_t)(u32)a->__vtbl)->mFunction, a);
    gabi::call_ptr(reinterpret_cast<MnVirtualSlot*>((uintptr_t)(u32)b->__vtbl)->mFunction, b);
    u32 pa = a->mStringTop;
    u32 pb = b->mStringTop;
    if (pa == pb) {
        return true;
    }
    for (u32 n = 0; n < 0x40001; n++) {
        u8 ca = reinterpret_cast<be<u8>*>((uintptr_t)pa)[n];
        if (ca != reinterpret_cast<be<u8>*>((uintptr_t)pb)[n]) {
            return false;
        }
        if (ca == 0) {
            return true;
        }
    }
    return false;
}
/* strcmp(dComIfGp_getNextStageName(), name) == 0 (play + 0x5140) */
static inline bool mn_isNextStage(const char* name) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::guest_addr(name);
    a->__vtbl = MN_SAFESTRING_VTBL;
    dComIfG_inf_c* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    gabi::Local<SafeString> b;
    b->mStringTop = gabi::ea(game->play.mNextStage.mName);
    b->__vtbl = MN_SAFESTRING_VTBL;
    return mn_SafeString_eq(a.get(), b.get());
}
cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class* p, u32 tbl, void* self);
void fopNpc_npc_c_ct(void* p);
void mDoExt_btpAnm_ct(void* p);
/* 025BD64C dSnap_GetFigRoomId(int) */
s32 dSnap_GetFigRoomId(s32 fig);

/* dComIfGp_event_onEventFlag(f): the event control's flag word (play + 0x52B8) |= f */

static inline s8 mn_getMyActIdx(s32 staffId, gptr<char>* tbl, s32 n, BOOL force, s32 nameType) {
    return static_cast<s8>(dComIfGp_evmng_getMyActIdx(staffId, tbl, n, force, nameType));
}
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz a, cXyz b (pointers to copies), f32* dist, s16* angY) */
void dNpc_calc_DisXZ_AngY(cXyz* a, cXyz* b, be<f32>* dist, be<s16>* ang);
void mn_playerEyePos(cXyz* out, f32 offs);
void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly);
fopAc_ac_c* fopAcM_searchFromName(const char* name, u32 mask, u32 prm);
BOOL fopAcM_orderChangeEventId(fopAc_ac_c* a, fopAc_ac_c* b, s16 ev, u16 flag, u16 hind);
BOOL fopAcM_orderPotentialEvent(fopAc_ac_c* a, u16 type, u16 flag, u16 p);
u32 fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 flag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale);
s16 cLib_calcTimer(be<s16>* t);
static inline s32 J3DAnm_getFrameMax(void* p) {
    MnVirtualSlot* vt = reinterpret_cast<MnAnimationView*>(p)->mVtable;
    return gabi::call_ptr<s32>(vt->mFunction, p);
}
/* GHS pointer-to-member call without arguments, returning r3 */

/* HD: the message functions are virtual (vtable at +0xB4): slot 0x14 next_msgStatus, 0x1C
 * getMsg, 0x24 anmAtr(u16) */
static inline u32 mn_v_next_msgStatus(daNpcMn_c* self, be<u32>* msgNo) { return gabi::call_ptr<u32>(reinterpret_cast<MnActorVirtualTable*>((uintptr_t)(u32)self->__vtbl)->next_msgStatus, self, msgNo); }
static inline u32 mn_v_getMsg(daNpcMn_c* self) { return gabi::call_ptr<u32>(reinterpret_cast<MnActorVirtualTable*>((uintptr_t)(u32)self->__vtbl)->getMsg, self); }
static inline void mn_v_anmAtr(daNpcMn_c* self, u16 status) { gabi::call_ptr(reinterpret_cast<MnActorVirtualTable*>((uintptr_t)(u32)self->__vtbl)->anmAtr, self, status); }
/* HD message manager (*(0x101F4B5C)): 025F795C status, 025F74D0 setStatus, 025F7DB0
 * messageSet(msgNo, cXyz* pos), 025F7E68 scopeMessageSet(msgNo) */
extern be<u32>& l_messageManager;
static inline u32 mn_msgManager() { return l_messageManager; }
u32 msgMng_getStatus(u32 mgr);
void msgMng_setStatus(u32 mgr, u32 st);
u32 msgMng_messageSet(u32 mgr, u32 msgNo, cXyz* pos);
u32 msgMng_scopeMessageSet(u32 mgr, u32 msgNo);
/* play + 0x5BB3: scope message status */

/* dComIfGp_checkPlayerStatus0(0, daPyStts0_TELESCOPE_LOOK_e): play + 0x5CD8 & 0x200000 */
static inline BOOL mn_checkTelescopeLook() { return dComIfGp_checkPlayerStatus0(0, 0x200000) != 0; }

using MnExecuteInitSignature = s32(daNpcMn_c::*)();
struct MnExecuteInit {
    gabi::ptmf<MnExecuteInitSignature> action;
};
using MnMoveProcSignature = void(daNpcMn_c::*)();
struct MnMoveProc {
    gabi::ptmf<MnMoveProcSignature> action;
};

struct MnFigureView : fopAc_ac_c {
    u8 _3AC[0xA32 - 0x3AC];
    be<u8> mbDisplay;
};
struct MnModelDataView {
    u8 _00[0x38];
    be<u32> mpTexPattern;
};
using MnAnimationPointer = gptr<sMnAnmDat>;
extern MnAnimationPointer (&l_msgAnm)[4];
u32 mn_setAnmTblResult(daNpcMn_c* self, sMnAnmDat* anm);

enum {
    dRes_ID_MN_BDL_MN_e = 2, dRes_ID_MN_BDL_MN_BAG_e = 1,
    dRes_ID_MN_BTP_MN_MABA_e = 3,
    dRes_ID_MN_BCK_MN_WAIT01_e = 6, dRes_ID_MN_BCK_MN_WAIT02_e = 10,
    dRes_ID_MN_BCK_MN_TALK01_e = 4, dRes_ID_MN_BCK_MN_TALK02_e = 5,
    dRes_ID_MN_BCK_MN_WALK_e = 11, dRes_ID_MN_BCK_MN_BIKKURI_e = 7,
    dRes_ID_MN_BCK_MN_JUMP01_e = 8, dRes_ID_MN_BCK_MN_JUMP02_e = 9,
};
enum { DSNAP_TYPE_NPC_MN = 0xA8 };
enum MnEventFlag {
    MN_EVENT_FIRST_TALK = 0x2F08, MN_EVENT_FIRST_ROOM_TALK = 0x2F04,
    MN_EVENT_COMPLETE = 0x3D08, MN_EVENT_COMPLETE_TALK = 0x3120,
    MN_EVENT_ALLOW_ROOM_CHANGE = 0x3A01, MN_EVENT_ROOM = 0x870F,
};
WWHD_OFFSET(MnFigureView, mbDisplay, 0xA32);
WWHD_SIZE(MnFigureView, 0xA33);
WWHD_OFFSET(MnModelDataView, mpTexPattern, 0x38);
WWHD_SIZE(MnModelDataView, 0x3C);
WWHD_SIZE(MnVirtualSlot, 0x18);
WWHD_SIZE(MnActorVirtualTable, 0x28);
WWHD_SIZE(MnAnimationView, 8);
WWHD_SIZE(MnExecuteInit, 8);
WWHD_SIZE(MnMoveProc, 8);
