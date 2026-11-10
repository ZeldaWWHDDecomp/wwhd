/* Mesa/Abe part A views. The common actor layout stays unchanged for parts A2 and B.
 * HD: SafeString virtual slots, the enlarged texture animation and HIO stride are measured
 * from the verified WWHD functions. */
#pragma once
#include "d/actor/d_a_npc_ym1.h"

struct Ym1ChildHIO {
    /* 0x00 */ be<f32> mAttentionOffset;
    /* 0x04 */ u8 _04[0x28];
};
WWHD_SIZE(Ym1ChildHIO, 0x2C);
WWHD_OFFSET(Ym1ChildHIO, mAttentionOffset, 0);
struct Ym1Text {
    be<u8> bytes[8];
};
struct Ym1StageName {
    be<u8> bytes[8];
};
WWHD_SIZE(Ym1Text, 8);
WWHD_SIZE(Ym1StageName, 8);
struct Ym1StringVtable {
    /* 0x00 */ u8 _00[0x14];
    /* 0x14 */ gfn assure;
};
WWHD_OFFSET(Ym1StringVtable, assure, 0x14);
WWHD_SIZE(Ym1StringVtable, 0x18);
inline Ym1StringVtable *ym1StringVtable(SafeString *s) {
    return gabi::at<Ym1StringVtable>(s->__vtbl);
}
struct Ym1BtpAnm {
    /* 0x00 */ u8 _00[0x10];
    /* 0x10 */ gptr<void> mpAnm;
    /* 0x14 */ u8 _14[0x60];
};
WWHD_OFFSET(Ym1BtpAnm, mpAnm, 0x10);
WWHD_SIZE(Ym1BtpAnm, 0x74);
inline void ym1CopyAction(ProcFunc_l *dst, const ProcFunc_l *src) {
    // HD: GHS copies the two words, preserving the load/store order.
    auto *out = reinterpret_cast<be<u32> *>(dst);
    auto *in = reinterpret_cast<const be<u32> *>(src);
    out[0] = in[0];
    out[1] = in[1];
}
u32 ym1CreateHeapResult(fopAc_ac_c *);
void ym1AssureTermination(SafeString *);

// HD: a non-owning view of the members used by part A; the shared actor remains byte-identical.
struct Ym1PartAState {
    /* 0x000 */ u8 _000[0x7E8];
    /* 0x7E8 */ gptr<J3DModel> mItemModel;
    /* 0x7EC */ u8 _7EC[0x9B6 - 0x7EC];
    /* 0x9B6 */ csXyz mModelAngle;
    /* 0x9BC */ cXyz mEyePosition;
    /* 0x9C8 */ u8 _9C8[12];
    /* 0x9D4 */ cXyz mNbtAttentionPosition;
    /* 0x9E0 */ cXyz mHeadPosition;
    /* 0x9EC */ be<f32> mPreviousMorfFrame;
    /* 0x9F0 */ u8 _9F0[8];
    /* 0x9F8 */ be<u32> mEyePositionEnabled;
    /* 0x9FC */ u8 _9FC[12];
    /* 0xA08 */ be<u8> mAnimationStopped;  /* was mMorfAnimationStopped (misnamed): view of daNpc_Ym1_c mAnimationStopped (d_a_npc_ym1.h); one name per field. play_animation 022F79D0 sets it from mpMorf->play() and also when the frame wraps, and the actions test it as the animation's end, so the class / ym1_b::State name stays */
    /* 0xA09 */ u8 _A09[0x11];
    /* 0xA1A */ be<s8> mBtpNum;
    /* 0xA1B */ be<s8> mAnmNum;  /* was mAnimationAttribute (misnamed): view of daNpc_Ym1_c mAnmNum (d_a_npc_ym1.h), the animation number setAnm_anm 022F9170 stores; the message animation attribute is daNpc_Ym1_c mAnimationAttribute at 0xA18 */
    /* 0xA1C */ u8 _A1C[6];
    /* 0xA22 */ be<s8> mActionPhase;  /* was mActionState (misnamed): view of daNpc_Ym1_c mActionPhase (d_a_npc_ym1.h); one name per field. set_action 022F7388 sets 9 / 0 and wait_action1 022FA828 .. step it from setup (0) to running (1), a phase of the current action, so the class / ym1_b::State name stays */
    /* 0xA23 */ u8 _A23;
};
WWHD_OFFSET(Ym1PartAState, mItemModel, 0x7E8);
WWHD_OFFSET(Ym1PartAState, mModelAngle, 0x9B6);
WWHD_OFFSET(Ym1PartAState, mEyePosition, 0x9BC);
WWHD_OFFSET(Ym1PartAState, mNbtAttentionPosition, 0x9D4);
WWHD_OFFSET(Ym1PartAState, mHeadPosition, 0x9E0);
WWHD_OFFSET(Ym1PartAState, mPreviousMorfFrame, 0x9EC);
WWHD_OFFSET(Ym1PartAState, mEyePositionEnabled, 0x9F8);
WWHD_OFFSET(Ym1PartAState, mAnimationStopped, 0xA08);
WWHD_OFFSET(Ym1PartAState, mBtpNum, 0xA1A);
WWHD_OFFSET(Ym1PartAState, mAnmNum, 0xA1B);
WWHD_OFFSET(Ym1PartAState, mActionPhase, 0xA22);
WWHD_SIZE(Ym1PartAState, 0xA24);
inline Ym1PartAState *ym1State(daNpc_Ym1_c *actor) {
    return reinterpret_cast<Ym1PartAState *>(actor);
}
