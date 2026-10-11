/* daNpc_Sarace_c (Loot the sailor, boating course), WWHD layout.
 *
 * The GameCube header is an empty placeholder (all functions Nonmatching); this layout is measured
 * from the WWHD code. The actor derives from fopNpc_npc_c (0x7DC, shared corrected layout) and has
 * a second morf for the head (resource 0x11) next to the body. Size 0x8B4 (profile 101C6284), actor
 * vtable 10021B28, HIO vtable 10021980. */
#pragma once
#include "gc_api/gc_api.h"

struct daNpc_Sarace_c : fopNpc_npc_c {
    enum Mode_e { MODE_WAIT = 1, MODE_TALK = 2 };
    enum EventOrder_e { ORDER_NONE = 0, ORDER_SPEAK = 1, ORDER_TALK = 2, ORDER_EXPCAM = 3 };
    enum ActionStatus_e { ACTION_END = -1, ACTION_INIT = 0, ACTION_EXEC = 1 };
    enum Resource_e {
        RES_HEAD_ANM = 7,
        RES_BODY_ANM = 9,
        RES_BODY_MODEL = 14,
        RES_HEAD_MODEL = 17
    };
    BOOL initTexPatternAnm(u32 modify);
    BOOL CreateHeap();
    void set_mtx();
    void setAnm(s32 idx, f32 morf);
    BOOL CreateInit();
    s32 _create();
    BOOL _delete();
    void playTexPatternAnm();
    void checkOrder();
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    u8 chkAttention(cXyz* pos, s16 angle);
    u16 next_msgStatus(be<u32>* msgNo);
    u32 getMsg();
    void anmAtr(u16 attr);
    void setAttention();
    void lookBack();
    void wait01();
    void talk01();
    BOOL dummy_action(void* prm);
    BOOL wait_action(void* prm);
    BOOL event_endCheck_action(void* prm);

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ gptr<mDoExt_McaMorf>
        mpHeadMorf;                 /* head model (resource 0x11, animation 7; "headModelData") */
    /* 0x7E8 */ be<u32> mBuoyId[2]; /* process ids of the two course actors (proc 0x1C9) */
    /* 0x7F0 */ gptr<J3DAnmTexPattern> mpTexPattern;
    /* 0x7F4 */ mDoExt_btpAnm mBtpAnm; /* mDoExt_btpAnm (HD 0x74) */
    /* 0x868 */ be<u8> mBtpFrame;
    /* 0x869 */ u8 _869;
    /* 0x86A */ be<s16> mBlinkTimer;
    /* 0x86C */ cXyz mEyePos; /* copied to eyePos (0x37C) */
    /* 0x878 */ cXyz mAttPos; /* copied to attention position (0x390) */
    /* 0x884 */ be<s16> mLookAngle;
    /* 0x886 */ u8 _886[2];
    /* 0x888 */ be<u16> mHomeAngle[3]; /* copy of current.angle */
    /* 0x88E */ u8 _88E[0x894 - 0x88E];
    /* 0x894 */ be<u32> mEventMsgNo;
    /* 0x898 */ be<u8> mbAttention;
    /* 0x899 */ be<u8> mbTalkRequest;
    /* 0x89A */ u8 _89A[0x8A0 - 0x89A];
    /* 0x8A0 */ be<u8> mInitState; // HD: cleared during CreateInit; otherwise unused here.
    /* 0x8A1 */ u8 _8A1[3];
    /* 0x8A4 */ gabi::ptmf<BOOL (daNpc_Sarace_c::*)(void*)> mAction;
    /* 0x8AC */ be<u8> mTexPatternState;
    /* 0x8AD */ be<s8> mAnmIdx;
    /* 0x8AE */ be<s8> mEventOrder;
    /* 0x8AF */ be<s8> mMode;
    /* 0x8B0 */ u8 _8B0[2];
    /* 0x8B2 */ be<s8> mActionStatus;
    /* 0x8B3 */ u8 _8B3;
};
WWHD_OFFSET(daNpc_Sarace_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Sarace_c, mBtpFrame, 0x868);
WWHD_OFFSET(daNpc_Sarace_c, mAction, 0x8A4);
WWHD_OFFSET(daNpc_Sarace_c, mActionStatus, 0x8B2);
WWHD_SIZE(daNpc_Sarace_c, 0x8B4);

// HD: HIO keeps its vtable after the inherited tuning parameters.
struct daNpc_Sarace_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ dNpc_HIO_c mNpc;
    /* 0x2C */ be<f32> mTalkMorf;
    /* 0x30 */ be<f32> mWaitMorf;
    /* 0x34 */ be<u32> __vtbl;
};
WWHD_OFFSET(daNpc_Sarace_HIO_c, mNpc, 4);
WWHD_OFFSET(daNpc_Sarace_HIO_c, mTalkMorf, 0x2C);
WWHD_OFFSET(daNpc_Sarace_HIO_c, __vtbl, 0x34);
WWHD_SIZE(daNpc_Sarace_HIO_c, 0x38);

// HD: these call sites preserve the complete incoming modify/frame registers.
s32 saraceInitTexPattern(mDoExt_btpAnm*, J3DModelData*, J3DAnmTexPattern*, u32, s32, f32, s32, s32,
                         u32, u32);
void saraceEntryTexPattern(mDoExt_btpAnm*, J3DModelData*, u32);
// HD: the joint-name lookup is consumed as a signed byte at this call site.
s8 saraceJointNameIndex(JUTNameTab*, const char*);

// HD: drawing clears this head-model animation entry after both display lists are submitted.
struct SaraceModelDataView {
    /* 0x00 */ u8 _00[0x38];
    /* 0x38 */ be<u32> mTexPatternEntries;
};
WWHD_OFFSET(SaraceModelDataView, mTexPatternEntries, 0x38);
WWHD_SIZE(SaraceModelDataView, 0x3C);

// HD: private call-site signatures retain the verified register widths and stack copies.
void saraceOrderOtherEvent(daNpc_Sarace_c*, const char*, u32, u32);
void saraceMoveF(daNpc_Sarace_c*, dCcD_Stts*);
u8 saraceGroundRoom(dBgS*, cBgS_PolyInfo*);
u8 saraceGroundColor(dBgS*, cBgS_PolyInfo*);
void saraceLookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u32);
void saraceSetNextStage(const char*, s32, s32, s32, f32, s32, s32, s32);
BOOL saraceEventEndCheck(dEvent_manager_c*, const char*);
void saraceNpcHioInit(dNpc_HIO_c*);
void saraceHeaderAngleInit(u8*);
void saraceHeaderCircleInit(u8*);
BOOL daNpc_Sarace_CallbackCreateHeap(daNpc_Sarace_c*);

// Word views preserve signaling NaNs in the initial position copies.
struct SaracePositionWords {
    be<u32> x, y, z;
};
WWHD_SIZE(SaracePositionWords, 12);
// HD: the animation supplies frame maximum through its vtable, not a frame-control field.
struct SaraceTexPatternVtable {
    /* 0x00 */ u8 _00[0x14];
    /* 0x14 */ gabi::gfn mGetFrameMax;
};
struct SaraceTexPatternView {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ gptr<SaraceTexPatternVtable> mpVtable;
    s32 getFrameMax() { return gabi::call_ptr<s32>(mpVtable->mGetFrameMax.get(), this); }
};
WWHD_OFFSET(SaraceTexPatternVtable, mGetFrameMax, 0x14);
WWHD_SIZE(SaraceTexPatternVtable, 0x18);
WWHD_OFFSET(SaraceTexPatternView, mpVtable, 4);
WWHD_SIZE(SaraceTexPatternView, 8);
struct SaraceBuoyView {
    /* 0x000 */ u8 _000[0x591];
    /* 0x591 */ be<u8> mbDeleteRequested;
};
WWHD_OFFSET(SaraceBuoyView, mbDeleteRequested, 0x591);
WWHD_SIZE(SaraceBuoyView, 0x592);
// HD: GHS PTMF dispatch caches index and adjusted receiver before changing action status.
inline void saraceCallAction(const gabi::ptmf<BOOL (daNpc_Sarace_c::*)(void*)>& action, s16 index,
                             u32 receiver) {
    u32 target = action.f;
    if (index >= 0) {
        u32 vtable = gabi::load<u32>(receiver + (s32)(s16)target);
        target = gabi::load<u32>(vtable + (s32)index * 8 + 4);
    }
    gabi::call_ptr(target, receiver, 0);
}
using SaraceActionFunc = BOOL (daNpc_Sarace_c::*)(void*);
// HD: convert the three native member names to the functions stored by the GHS PTMF.
inline u32 saraceActionAddress(SaraceActionFunc action) {
    if (action == &daNpc_Sarace_c::dummy_action)
        return 0x022DD728;
    if (action == &daNpc_Sarace_c::wait_action)
        return 0x022DD750;
    return 0x022DD844; // event_endCheck_action
}

WWHD_OFFSET(daNpc_Sarace_c, mpHeadMorf, 0x7E4);
WWHD_OFFSET(daNpc_Sarace_c, mBuoyId, 0x7E8);
WWHD_OFFSET(daNpc_Sarace_c, mpTexPattern, 0x7F0);
WWHD_OFFSET(daNpc_Sarace_c, mBtpAnm, 0x7F4);
WWHD_OFFSET(daNpc_Sarace_c, mBlinkTimer, 0x86A);
WWHD_OFFSET(daNpc_Sarace_c, mEyePos, 0x86C);
WWHD_OFFSET(daNpc_Sarace_c, mAttPos, 0x878);
WWHD_OFFSET(daNpc_Sarace_c, mLookAngle, 0x884);
WWHD_OFFSET(daNpc_Sarace_c, mHomeAngle, 0x888);
WWHD_OFFSET(daNpc_Sarace_c, mEventMsgNo, 0x894);
WWHD_OFFSET(daNpc_Sarace_c, mbAttention, 0x898);
WWHD_OFFSET(daNpc_Sarace_c, mbTalkRequest, 0x899);
WWHD_OFFSET(daNpc_Sarace_c, mInitState, 0x8A0);
WWHD_OFFSET(daNpc_Sarace_c, mTexPatternState, 0x8AC);
WWHD_OFFSET(daNpc_Sarace_c, mAnmIdx, 0x8AD);
WWHD_OFFSET(daNpc_Sarace_c, mEventOrder, 0x8AE);
WWHD_OFFSET(daNpc_Sarace_c, mMode, 0x8AF);

// Private mapped data layouts.
struct SaraceHioLabel {
    char text[23];
};
struct SaraceSignedTable {
    be<s32> values[3];
};
struct SaraceFloatTable {
    be<f32> values[3];
};
struct SaraceBuoyPositions {
    cXyz positions[2];
};
struct SaraceHeaderZeros {
    be<u32> words[4];
};
struct SaraceHeaderStatics {
    be<f32> minAngle;
    be<f32> maxAngle;
    u8 angle;
    u8 circle;
    u8 _0A[2];
};
struct SaracePiLimits {
    be<f32> minAngle, maxAngle;
};
struct SaraceBuoyInitialPos {
    be<f32> values[5];
};
WWHD_SIZE(SaraceSignedTable, 12);
WWHD_SIZE(SaraceFloatTable, 12);
WWHD_SIZE(SaraceBuoyPositions, 24);
WWHD_SIZE(SaraceHioLabel, 23);
WWHD_SIZE(SaraceBuoyInitialPos, 20);
WWHD_SIZE(SaraceHeaderZeros, 16);
WWHD_OFFSET(SaraceHeaderStatics, angle, 8);
WWHD_OFFSET(SaraceHeaderStatics, circle, 9);
WWHD_SIZE(SaraceHeaderStatics, 12);
WWHD_SIZE(SaracePiLimits, 8);
