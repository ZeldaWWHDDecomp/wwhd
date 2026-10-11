/* Mesa and Abe, part B.
 * HD: the actor tail is shifted by 0x170 after its enlarged texture animation and compact
 * GHS action descriptor. Known GameCube field names are retained in this private view.
 */
#pragma once
#include "d/actor/d_a_npc_ym1.h"
WWHD_SIZE(daNpc_Ym1_c::anm_prm_c, 0x10);
namespace ym1_b {
struct State {
    u8 _pad0[0x7E8];
    /* 0x7E8 */ gptr<J3DModel> m6D0;
    u8 _pad7EC[0x1CA];
    /* 0x9B6 */ csXyz m846;
    u8 _pad9BC[0x18];
    /* 0x9D4 */ cXyz mNbtAttentionPosition;
    u8 _pad9E0[0xC];
    /* 0x9EC */ be<f32> mPreviousMorfFrame;
    /* 0x9F0 */ csXyz mPreviousAngles;
    u8 _pad9F6[0x2];
    /* 0x9F8 */ be<u32> mEyePositionEnabled;
    u8 _pad9FC[0x2];
    /* 0x9FE */ be<s16> mAttentionTimer;
    u8 _padA00[0x8];
    /* 0xA08 */ be<u8> mAnimationStopped;
    /* 0xA09 */ be<u8> mAnimationWrapped;
    /* 0xA0A */ be<u8> mPresentItem;
    u8 _padA0B[0x1];
    /* 0xA0C */ be<u8> m89C;
    u8 _padA0D[0x1];
    /* 0xA0E */ be<u8> m89E;
    u8 _padA0F[0x1];
    /* 0xA10 */ be<u8> m8A0;
    u8 _padA11[0x1];
    /* 0xA12 */ be<s8> mPlayerAttending;
    /* 0xA13 */ be<s8> mTalkRequested;
    /* 0xA14 */ be<s8> m8A4;
    /* 0xA15 */ be<s8> mTurnToPlayer;
    u8 _padA16[0x2];
    /* 0xA18 */ be<s8> mAnimationAttribute;
    /* 0xA19 */ be<s8> mAnimationTag;
    u8 _padA1A[0x1];
    /* 0xA1B */ be<s8> m8AB;
    /* 0xA1C */ be<s8> mActionSubstate;
    /* 0xA1D */ be<s8> mStatus;
    /* 0xA1E */ be<s8> mReturnStatus;
    /* 0xA1F */ be<s8> m8AF;
    u8 _padA20[0x2];
    /* 0xA22 */ be<s8> mActionPhase;
    /* 0xA23 */ be<s8> mMessageAnimationState;
};
WWHD_SIZE(State, 0xA24);
WWHD_OFFSET(State, m6D0, 0x7E8);
WWHD_OFFSET(State, m846, 0x9B6);
WWHD_OFFSET(State, mNbtAttentionPosition, 0x9D4);
WWHD_OFFSET(State, mPreviousMorfFrame, 0x9EC);
WWHD_OFFSET(State, mPreviousAngles, 0x9F0);
WWHD_OFFSET(State, mEyePositionEnabled, 0x9F8);
WWHD_OFFSET(State, mAttentionTimer, 0x9FE);
WWHD_OFFSET(State, mAnimationStopped, 0xA08);
WWHD_OFFSET(State, mAnimationWrapped, 0xA09);
WWHD_OFFSET(State, mPresentItem, 0xA0A);
WWHD_OFFSET(State, m89C, 0xA0C);
WWHD_OFFSET(State, m89E, 0xA0E);
WWHD_OFFSET(State, m8A0, 0xA10);
WWHD_OFFSET(State, mPlayerAttending, 0xA12);
WWHD_OFFSET(State, mTalkRequested, 0xA13);
WWHD_OFFSET(State, m8A4, 0xA14);
WWHD_OFFSET(State, mTurnToPlayer, 0xA15);
WWHD_OFFSET(State, mAnimationAttribute, 0xA18);
WWHD_OFFSET(State, mAnimationTag, 0xA19);
WWHD_OFFSET(State, m8AB, 0xA1B);
WWHD_OFFSET(State, mActionSubstate, 0xA1C);
WWHD_OFFSET(State, mStatus, 0xA1D);
WWHD_OFFSET(State, mReturnStatus, 0xA1E);
WWHD_OFFSET(State, m8AF, 0xA1F);
WWHD_OFFSET(State, mActionPhase, 0xA22);
WWHD_OFFSET(State, mMessageAnimationState, 0xA23);
inline State* state(daNpc_Ym1_c* actor) {
    return reinterpret_cast<State*>(actor);
}
// HD: these non-owning parameter windows overlap the HIO's constructor view.
struct ChildSettings {
    /* 0x00 */ be<s16> mTurnSpeed;
    u8 _pad02[4];
    /* 0x06 */ be<u8> mDebugDraw;
    u8 _pad07[3];
    /* 0x0A */ be<f32> mAreaDistance;
    /* 0x0E */ be<f32> mMorf;
    u8 _pad12[0x1A];
};
WWHD_SIZE(ChildSettings, 0x2C);
WWHD_OFFSET(ChildSettings, mDebugDraw, 6);
WWHD_OFFSET(ChildSettings, mAreaDistance, 0xA);
WWHD_OFFSET(ChildSettings, mMorf, 0xE);
inline ChildSettings* settings(ChildSettings* table, s8 index) {
    return gabi::at<ChildSettings>(gabi::ea(table) + u32(s32(index)) * sizeof(ChildSettings));
}
template <class T> inline T* entry(T* table, s32 index) {
    return gabi::at<T>(gabi::ea(table) + u32(index) * sizeof(T));
}

struct ModelDataView {
    u8 _pad[0x38];
    be<u32> mpAnmTexPattern;
};
WWHD_OFFSET(ModelDataView, mpAnmTexPattern, 0x38);
WWHD_SIZE(ModelDataView, 0x3C);
// HD: the model-data argument may be null; preserve the wrapped guest field address.
inline void removeTexPattern(J3DModelData* data) {
    gabi::store<u32>(gabi::ea(data) + offsetof(ModelDataView, mpAnmTexPattern), 0);
}
struct AcchVtables {
    u8 _pad[0x14];
    be<u32> baseTable;
    u8 _pad18[8];
    be<u32> actorTable;
};
WWHD_OFFSET(AcchVtables, baseTable, 0x14);
WWHD_OFFSET(AcchVtables, actorTable, 0x20);
inline AcchVtables* acchVtables(dBgS_ObjAcch* acch) {
    return reinterpret_cast<AcchVtables*>(acch);
}
struct Parameters {
    be<u8> bytes[0x24];
};
WWHD_SIZE(Parameters, 0x24);
struct ParameterTable {
    Parameters entries[2];
};
struct ColorWord {
    be<u32> rgba;
};
WWHD_SIZE(ColorWord, 4);
enum : u32 {
    CHILD_HIO_VTABLE = 0x10023490,
    HIO_VTABLE = 0x100234A0,
    ACCH_ACTOR_VTABLE = 0x10023470,
    ACCH_BASE_VTABLE = 0x10023480
};
enum Status : s8 {
    STT_KARI = 1,
    STT_WAIT_1 = 2,
    STT_TURN = 3,
    STT_WAIT_2 = 4,
    STT_TALK = 5,
    STT_NBT_WAIT = 6,
    STT_NBT_TALK = 7,
    STT_MESA_WAIT = 8,
    STT_OTHER_TALK = 9,
    STT_MESA_LATE_WAIT = 10,
    STT_SIT_WAIT = 11
};
enum MessageStatus : u16 {
    MSG_TALKING = 6,
    MSG_DISPLAYED = 0xE,
    MSG_CONTINUES = 0xF,
    MSG_ENDS = 0x10,
    MSG_DESTROYED = 0x13
};
} // namespace ym1_b
struct daNpc_Ym1_childHIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 mPrm[0x24]; // HD: copied parameter bytes.
    /* 0x28 */ be<s32> mNo;
};
WWHD_SIZE(daNpc_Ym1_childHIO_c, 0x2C);
WWHD_OFFSET(daNpc_Ym1_childHIO_c, mPrm, 4);
WWHD_OFFSET(daNpc_Ym1_childHIO_c, mNo, 0x28);
struct daNpc_Ym1_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> m08;
    /* 0x0C */ daNpc_Ym1_childHIO_c mChild[2];
};
WWHD_SIZE(daNpc_Ym1_HIO_c, 0x64);
WWHD_OFFSET(daNpc_Ym1_HIO_c, mChild, 0xC);
