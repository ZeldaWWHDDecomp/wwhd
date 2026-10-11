/* daNpc_Tt_c (Tott, the dancing NPC on Windfall), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_tt: daNpc_Tt_c derives from fopAc_ac_c
 * directly, so mHeadAnm is +0x11C (0x3AC). dNpc_HeadAnm_c holds a GHS pointer to member function
 * (8 bytes instead of 12), so it is 0x20 (GameCube 0x24) and mEventIdx..mpMorf are +0x118;
 * mShadowId (GameCube 0x2C4) is gone (HD shadows), so m_head_tex_pattern/mBtpAnm are +0x114;
 * mDoExt_btpAnm grew from 0x14 to 0x74, so mBlinkFrame..mAttnSetCount..mNoticeLinkTimer are
 * +0x174; mCurrActionFunc is an 8-byte pointer to member (GameCube 12), so mDanceStep and
 * mLineKe are +0x170; mDoExt_3DlineMat0_c grew from 0x1C to 0x148, so the members after it
 * are +0x29C. Size 0x1120 (GameCube 0xE84). */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
#include "gc_api/f_op/f_op_msg.h"
struct TtActionFunc {
    be<s16> d, i;
    be<u32> f;
};
WWHD_SIZE(TtActionFunc, 8);

/* mDoExt_3DlineMat0_c (HD 0x148): vtable at +0x130, lines (0x10 each, position array first) at +0x144 */
struct mDoExt_3DlineMat0_tt {
    /* 0x000 */ u8 _000[0x130];
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 _134[0x144 - 0x134];
    /* 0x144 */ be<u32> mpLines;
};
WWHD_SIZE(mDoExt_3DlineMat0_tt, 0x148);

struct tt_ke_s {
    /* 0x00 */ cXyz field_0x00[10];
    /* 0x78 */ cXyz field_0x78[10];
    void ke_control(f32);
    void ke_pos_set(cXyz*);
};
WWHD_SIZE(tt_ke_s, 0xF0);

struct daNpc_Tt_c : fopAc_ac_c {
    enum ActionStatus { ACTION_ENDING = -1, ACTION_STARTING = 0, ACTION_ONGOING = 1 };
    enum TalkStates { TALK_FINISHED = -1, TALK_INIT = 0, TALK_MSG_CREATE = 1, TALK_ACTIVE = 2 };

    u32 ChkOrder(u8 flag) { return (u8)mOrderFlags & flag; }
    void ClrOrder() { mOrderFlags = 0; }
    void SetOrder(u8 flag) { mOrderFlags = (u8)(mOrderFlags | flag); }
    bool chkEvFlag(u8 f) { return (mEvFlags & f) == f; }
    void setEvFlag(u8 f) { mEvFlags = (u8)(mEvFlags | f); }
    void clrEvFlag(u8 f) { mEvFlags = (u8)(mEvFlags & ~f); }
    bool chkFlag(u16 flag) { return (mFlags & flag) == flag; }
    void clrFlag(u16 flag) { mFlags = (u16)(mFlags & ~flag); }
    void setFlag(u16 flag) { mFlags = (u16)(mFlags | flag); }
    f32 curMorf(); /* mDoExt_McaMorf +0xB0 (isMorf: < 1.0) */

    s16 XyCheckCB(int);
    void setAnmStatus();
    void eventOrder();
    void checkOrder();
    u32 next_msgStatus(be<u32>*); /* u16 */
    u32 getMsg();
    void setCollision();
    void talkInit();
    void msgAnm();
    void msgPushButton();
    void msgContinue();
    u16 talk();
    void demoInitChange();
    bool demoProcTact1();
    void demoInitPatten();
    bool demoProcPatten();
    void demoInitSpeak();
    bool demoProcSpeak();
    void demoInitWait();
    bool demoProcWait();
    void demoInitCom();
    void demoProcCom();
    s32 getNowEventAction();
    bool demoProc();
    BOOL init();
    void setAttention(bool);
    void lookBack();
    bool wait01();
    bool talk01();
    bool tact00();
    BOOL wait_action(void*);
    void set_ke_root(int, int, int);
    void ke_execute();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateHeap();
    BOOL initTexPatternAnm(u32); /* bool, passed on unnormalised */
    void playTexPatternAnm();
    void setAnm(int);
    void danceInit(int);
    BOOL danceProc();
    BOOL danceNext();

    /* 0x3AC */ dNpc_HeadAnm_c mHeadAnm;
    /* 0x3CC */ be<s16> mEventIdx;
    /* 0x3CE */ u8 _3CE[2];
    /* 0x3D0 */ request_of_phase_process_class mPhs;
    /* 0x3D8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3DC */ gptr<J3DAnmTexPattern> m_head_tex_pattern; /* HD: no mShadowId before it */
    /* 0x3E0 */ u8 mBtpAnm[0x74];                          /* mDoExt_btpAnm (HD 0x74) */
    /* 0x454 */ be<u8> mBlinkFrame;
    /* 0x455 */ u8 _455;
    /* 0x456 */ be<s16> mBlinkTimer;
    /* 0x458 */ dBgS_ObjAcch mObjAcch;
    /* 0x61C */ dBgS_AcchCir mAcchCir;
    /* 0x65C */ dCcD_Stts mStts;
    /* 0x698 */ dCcD_Cyl mCyl;
    /* 0x7C8 */ be<s8> m_head_jnt_num;
    /* 0x7C9 */ be<s8> m_backbone_jnt_num;
    /* 0x7CA */ u8 _7CA[2];
    /* 0x7CC */ dNpc_JntCtrl_c m_jnt;
    /* 0x800 */ cXyz mEyePos;
    /* 0x80C */ cXyz mAttnBasePos;
    /* 0x818 */ be<s16> mMaxHeadTurnVelocity;
    /* 0x81A */ u8 _81A[2];
    /* 0x81C */ be<s8> mAnmEnded;
    /* 0x81D */ be<u8> mAttnSetCount;
    /* 0x81E */ u8 _81E[2];
    /* 0x820 */ be<f32> mAnmTimer;
    /* 0x824 */ be<u32> mCurrMsgNo;
    /* 0x828 */ be<u16> mFlags;
    /* 0x82A */ be<s16> mNoticeLinkTimer;
    /* 0x82C */ TtActionFunc mCurrActionFunc;
    /* 0x834 */ be<u8> mDanceStep;
    /* 0x835 */ be<u8> mDanceStepTimer;
    /* 0x836 */ u8 _836[2];
    /* 0x838 */ tt_ke_s mLineKe[8];
    /* 0xFB8 */ mDoExt_3DlineMat0_tt mLineMat;
    /* 0x1100 */ be<s8> mTexPatternIdx;
    /* 0x1101 */ be<s8> mCurrAnmIdx;
    /* 0x1102 */ be<s8> mAnmLoopCount;
    /* 0x1103 */ be<u8> mOrderFlags;
    /* 0x1104 */ be<s8> mState;
    /* 0x1105 */ be<s8> mReturnToState;
    /* 0x1106 */ be<u8> mType;
    /* 0x1107 */ be<s8> mActionStatus;
    /* 0x1108 */ be<s8> mTalkState;
    /* 0x1109 */ be<u8> mMsgAnmIdx;
    /* 0x110A */ u8 _110A[2];
    /* 0x110C */ be<u32> mMtrlSndId;
    /* 0x1110 */ be<s8> mReverb;
    /* 0x1111 */ be<u8> mEvFlags;
    /* 0x1112 */ be<s16> mSoundTimer;
    /* 0x1114 */ be<s16> mTimer;
    /* 0x1116 */ be<u8> field_0xE7A;
    /* 0x1117 */ u8 _1117;
    /* 0x1118 */ be<u32> mMsgNo;
    /* 0x111C */ be<s32> mStaffIdx;
};
WWHD_OFFSET(daNpc_Tt_c, mEventIdx, 0x3CC);
WWHD_OFFSET(daNpc_Tt_c, mBtpAnm, 0x3E0);
WWHD_OFFSET(daNpc_Tt_c, mObjAcch, 0x458);
WWHD_OFFSET(daNpc_Tt_c, mCyl, 0x698);
WWHD_OFFSET(daNpc_Tt_c, m_jnt, 0x7CC);
WWHD_OFFSET(daNpc_Tt_c, mCurrActionFunc, 0x82C);
WWHD_OFFSET(daNpc_Tt_c, mLineKe, 0x838);
WWHD_OFFSET(daNpc_Tt_c, mLineMat, 0xFB8);
WWHD_OFFSET(daNpc_Tt_c, mTexPatternIdx, 0x1100);
WWHD_OFFSET(daNpc_Tt_c, mStaffIdx, 0x111C);
WWHD_SIZE(daNpc_Tt_c, 0x1120);

// HD: virtual frame count, animation-removal and destructor layouts.
struct TtAnmVtable {
    u8 _0[0x14];
    be<u32> mGetFrameMax;
};
struct TtAnm {
    u8 _0[4];
    gptr<TtAnmVtable> mpVtable;
};
struct TtModelAnimation {
    u8 _0[0x38];
    be<u32> mpBtpAnimation;
};
struct TtAcchVtables {
    u8 _0[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mPolyVtable;
};
WWHD_OFFSET(TtAnmVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(TtAnm, mpVtable, 4);
WWHD_OFFSET(TtModelAnimation, mpBtpAnimation, 0x38);
WWHD_OFFSET(TtAcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(TtAcchVtables, mPolyVtable, 0x20);
template <class T, class U> inline T* Tt_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline s32 Tt_frameMax(J3DAnmTexPattern* p) {
    auto* a = Tt_view<TtAnm>(p);
    auto* t = a->mpVtable.get();
    return gabi::call_ptr<s32>(t->mGetFrameMax, p);
}
inline void Tt_loadAction(TtActionFunc* dst, TtActionFunc* src) {
    auto* d = Tt_view<be<u32>>(dst);
    auto* s = Tt_view<be<u32>>(src);
    d[0] = s[0];
    d[1] = s[1];
}
struct TtVirtualAction {
    u8 _0[6];
    be<s16> mVtableOffset;
};
inline void Tt_callAction(void* self, TtActionFunc* action, void* arg) {
    s16 index = action->i;
    u32 object = gabi::ea(self) + (s32)(s16)action->d;
    if (index < 0) {
        gabi::call_ptr<BOOL>(action->f, object, arg);
    } else {
        s16 offset = Tt_view<TtVirtualAction>(action)->mVtableOffset;
        auto* vtable = gabi::at<be<u32>>(object + offset);
        auto* entry = gabi::at<be<u32>>((u32)*vtable + index * 8 + 4);
        gabi::call_ptr<BOOL>(*entry, object, arg);
    }
}
inline void Tt_initDebugColour(be<u32>* guard, be<u32>* dst, be<u32>* src) {
    if ((u32)*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, 4);
    }
}

struct TtBtp {
    u8 _0[0x10];
    gptr<J3DAnmTexPattern> mpAnm;
};
struct TtMorfVtable {
    u8 _0[0xC];
    be<u32> mDestructor;
};
struct TtMorf {
    gptr<TtMorfVtable> mpVtable;
};

WWHD_OFFSET(TtBtp, mpAnm, 0x10);
WWHD_SIZE(TtBtp, 0x14);
WWHD_SIZE(TtMorf, 4);
WWHD_SIZE(TtMorfVtable, 0x10);

// HD: line records, material sorting, player virtual dispatch and register views.
struct TtLine {
    gptr<cXyz> mpPositions;
    u8 _4[12];
};
struct TtLineVtable {
    u8 _0[0x14];
    be<u32> mMaterialID;
};
struct TtPlayerVtable {
    u8 _0[0x34];
    be<u32> mTactTimerCancel;
};
struct TtPlayer {
    u8 _0[0xB4];
    gptr<TtPlayerVtable> mpVtable;
    u8 _B8[0x300];
    be<u32> mNoDrawFlags;
};
struct TtMessage {
    u8 _0[0x921];
    be<u8> mbSendOn;
};
struct TtModelMatrix {
    u8 _0[0xC8];
    Mtx34 mBaseMatrix;
};
struct TtDebugRegisters {
    u8 _0[0x5A8];
    be<f32> mHairDamping, mHairGravity;
};
WWHD_SIZE(TtLine, 0x10);
WWHD_SIZE(TtLineVtable, 0x18);
WWHD_SIZE(TtPlayerVtable, 0x38);
WWHD_OFFSET(TtPlayer, mpVtable, 0xB4);
WWHD_OFFSET(TtPlayer, mNoDrawFlags, 0x3B8);
WWHD_SIZE(TtPlayer, 0x3BC);
WWHD_OFFSET(TtMessage, mbSendOn, 0x921);
WWHD_SIZE(TtMessage, 0x922);
WWHD_OFFSET(TtModelMatrix, mBaseMatrix, 0xC8);
WWHD_SIZE(TtModelMatrix, 0xF8);
WWHD_OFFSET(TtDebugRegisters, mHairDamping, 0x5A8);
WWHD_OFFSET(TtDebugRegisters, mHairGravity, 0x5AC);
WWHD_SIZE(TtDebugRegisters, 0x5B0);
inline u32 Tt_word(const be<f32>& f) { return *Tt_view<be<u32>>(const_cast<be<f32>*>(&f)); }
inline void Tt_word(be<f32>& f, u32 v) { *Tt_view<be<u32>>(&f) = v; }
inline cXyz* Tt_linePosition(mDoExt_3DlineMat0_tt* l, s32 i) {
    return gabi::at<TtLine>(l->mpLines)[i].mpPositions;
}
inline u32 Tt_tactTimerCancel(fopAc_ac_c* p) {
    auto* vt = Tt_view<TtPlayer>(p)->mpVtable.get();
    return gabi::call_ptr<u32>(vt->mTactTimerCancel, p);
}
inline s32 Tt_lineMaterialID(mDoExt_3DlineMat0_tt* l) {
    auto* vt = gabi::at<TtLineVtable>(l->__vtbl);
    return gabi::call_ptr<s32>(vt->mMaterialID, l);
}
// HD: twelve loads precede stores, with GHS float-store NaN quieting even for unaligned words.
inline void Tt_copyMatrix(Mtx34* dst, const Mtx34* src) {
    u32 words[12];
    auto* s = Tt_view<be<u32>>(const_cast<Mtx34*>(src));
    for (int i = 0; i < 12; i++)
        words[i] = s[i];
    for (int i = 0; i < 12; i++) {
        u32 v = words[i];
        if ((v & 0x7F800000u) == 0x7F800000u && (v & 0x003FFFFFu) != 0 && !(v & 0x00400000u))
            v |= 0x00400000u;
        gmem_stf32(gabi::ea(dst) + i * 4, v);
    }
}
BOOL Tt_orderChangeEvent(fopAc_ac_c*, fopAc_ac_c*, s16, u16, u16);
u16 Tt_messageStatus(void*);
void Tt_setMessageStatus(void*, u32);
u32 Tt_messageSet(void*, u32, cXyz*);
BOOL Tt_setAnm(mDoExt_McaMorf*, s32, f32, f32, s32, s32, const char*);
s16 Tt_calcTimer(be<s16>*);
s32 Tt_btpInit(void*, J3DModelData*, J3DAnmTexPattern*, s32, s32, f32, s32, s32, u32, s32);
void Tt_playerEyePos(cXyz*, f32);
void Tt_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
s8 Tt_roomId(dBgS*, cBgS_PolyInfo*);
u8 Tt_polyColour(dBgS*, cBgS_PolyInfo*);
u32 Tt_materialSound(dBgS*, cBgS_PolyInfo*);
void Tt_XrotS(Mtx34*, s16);
void Tt_lineCtor(mDoExt_3DlineMat0_tt*);
void Tt_lineDestroy(mDoExt_3DlineMat0_tt*, s32);
BOOL Tt_lineInit(mDoExt_3DlineMat0_tt*, u16, u16, BOOL);
void Tt_lineUpdate(mDoExt_3DlineMat0_tt*, u16, f32, void*, u16, dKy_tevstr_c*);
void Tt_sortLine(void*, mDoExt_3DlineMat0_tt*);
void Tt_actorCtor(daNpc_Tt_c*);
void Tt_btpCtor(void*);
void Tt_jointCtor(dNpc_JntCtrl_c*);
void Tt_circleDestroy(void*, s32);
void Tt_acchDestroy(dBgS_Acch*, s32);
void Tt_actorDestroy(daNpc_Tt_c*, s32);
enum TtActionAddress : u32 { TtWaitAction = 0x022F0664 };
inline void Tt_callDirect(daNpc_Tt_c* p, u32 f) { gabi::call_ptr<BOOL>(f, p, (void*)nullptr); }
inline void* Tt_linePacket(mDoExt_3DlineMat0_tt* p) {
    u32 packet = dComIfGp_ea() + 0x5FB4;
    s32 id = Tt_lineMaterialID(p);
    return gabi::at<u8>(packet + id * 0x9C);
}

WWHD_OFFSET(daNpc_Tt_c, mHeadAnm, 0x3AC);
WWHD_OFFSET(daNpc_Tt_c, mEventIdx, 0x3CC);
WWHD_OFFSET(daNpc_Tt_c, mPhs, 0x3D0);
WWHD_OFFSET(daNpc_Tt_c, mpMorf, 0x3D8);
WWHD_OFFSET(daNpc_Tt_c, m_head_tex_pattern, 0x3DC);
WWHD_OFFSET(daNpc_Tt_c, mBtpAnm, 0x3E0);
WWHD_OFFSET(daNpc_Tt_c, mBlinkFrame, 0x454);
WWHD_OFFSET(daNpc_Tt_c, mBlinkTimer, 0x456);
WWHD_OFFSET(daNpc_Tt_c, mObjAcch, 0x458);
WWHD_OFFSET(daNpc_Tt_c, mAcchCir, 0x61C);
WWHD_OFFSET(daNpc_Tt_c, mStts, 0x65C);
WWHD_OFFSET(daNpc_Tt_c, mCyl, 0x698);
WWHD_OFFSET(daNpc_Tt_c, m_head_jnt_num, 0x7C8);
WWHD_OFFSET(daNpc_Tt_c, m_backbone_jnt_num, 0x7C9);
WWHD_OFFSET(daNpc_Tt_c, m_jnt, 0x7CC);
WWHD_OFFSET(daNpc_Tt_c, mEyePos, 0x800);
WWHD_OFFSET(daNpc_Tt_c, mAttnBasePos, 0x80C);
WWHD_OFFSET(daNpc_Tt_c, mMaxHeadTurnVelocity, 0x818);
WWHD_OFFSET(daNpc_Tt_c, mAnmEnded, 0x81C);
WWHD_OFFSET(daNpc_Tt_c, mAttnSetCount, 0x81D);
WWHD_OFFSET(daNpc_Tt_c, mAnmTimer, 0x820);
WWHD_OFFSET(daNpc_Tt_c, mCurrMsgNo, 0x824);
WWHD_OFFSET(daNpc_Tt_c, mFlags, 0x828);
WWHD_OFFSET(daNpc_Tt_c, mNoticeLinkTimer, 0x82A);
WWHD_OFFSET(daNpc_Tt_c, mCurrActionFunc, 0x82C);
WWHD_OFFSET(daNpc_Tt_c, mDanceStep, 0x834);
WWHD_OFFSET(daNpc_Tt_c, mDanceStepTimer, 0x835);
WWHD_OFFSET(daNpc_Tt_c, mLineKe, 0x838);
WWHD_OFFSET(daNpc_Tt_c, mLineMat, 0xFB8);
WWHD_OFFSET(daNpc_Tt_c, mTexPatternIdx, 0x1100);
WWHD_OFFSET(daNpc_Tt_c, mCurrAnmIdx, 0x1101);
WWHD_OFFSET(daNpc_Tt_c, mAnmLoopCount, 0x1102);
WWHD_OFFSET(daNpc_Tt_c, mOrderFlags, 0x1103);
WWHD_OFFSET(daNpc_Tt_c, mState, 0x1104);
WWHD_OFFSET(daNpc_Tt_c, mReturnToState, 0x1105);
WWHD_OFFSET(daNpc_Tt_c, mType, 0x1106);
WWHD_OFFSET(daNpc_Tt_c, mActionStatus, 0x1107);
WWHD_OFFSET(daNpc_Tt_c, mTalkState, 0x1108);
WWHD_OFFSET(daNpc_Tt_c, mMsgAnmIdx, 0x1109);
WWHD_OFFSET(daNpc_Tt_c, mMtrlSndId, 0x110C);
WWHD_OFFSET(daNpc_Tt_c, mReverb, 0x1110);
WWHD_OFFSET(daNpc_Tt_c, mEvFlags, 0x1111);
WWHD_OFFSET(daNpc_Tt_c, mSoundTimer, 0x1112);
WWHD_OFFSET(daNpc_Tt_c, mTimer, 0x1114);
WWHD_OFFSET(daNpc_Tt_c, field_0xE7A, 0x1116);
WWHD_OFFSET(daNpc_Tt_c, mMsgNo, 0x1118);
WWHD_OFFSET(daNpc_Tt_c, mStaffIdx, 0x111C);
WWHD_OFFSET(mDoExt_3DlineMat0_tt, __vtbl, 0x130);
WWHD_OFFSET(mDoExt_3DlineMat0_tt, mpLines, 0x144);
WWHD_OFFSET(tt_ke_s, field_0x00, 0);
WWHD_OFFSET(tt_ke_s, field_0x78, 0x78);
