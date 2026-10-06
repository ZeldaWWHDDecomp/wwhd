/* daNpc_Kg2_c (Salvatore, cannon minigame on Windfall), WWHD layout.
 *
 * The GameCube decompilation has no layout for this class (all functions are Nonmatching
 * placeholders); the layout below is measured from the WWHD code of d_a_npc_kg2.
 * - base fopNpc_npc_c (0x7DC, see d_a_npc_ba1.h fopNpc_npc_l)
 * - two mDoExt_btpAnm (HD 0x74 bytes, constructor 025E7820): the held item's and the face's
 * - GHS pointer to member mAction (8 bytes) at 0x910 */
#pragma once
#include "d/actor/d_a_npc_ba1.h"

struct daNpc_Kg2_c : fopNpc_npc_l {
    /* 0x7DC */ be<s8> mItemJntNum;            /* joint the held item hangs from */
    /* 0x7DD */ u8 _7DD[3];
    /* 0x7E0 */ request_of_phase_process_class mPhs;
    /* 0x7E8 */ gptr<J3DModel> mpItemModel;
    /* 0x7EC */ u8 mItemBtpAnm[0x74];          /* mDoExt_btpAnm */
    /* 0x860 */ gptr<J3DAnmTexPattern> mpBtp;
    /* 0x864 */ u8 mBtpAnm[0x74];              /* mDoExt_btpAnm */
    /* 0x8D8 */ be<u8> mBtpFrame;
    /* 0x8D9 */ u8 _8D9;
    /* 0x8DA */ be<s16> mBlinkTimer;
    /* 0x8DC */ cXyz mAttentionPos;            /* head joint offset; copied to eyePos by setAttention */
    /* 0x8E8 */ cXyz mHeadPos;
    /* 0x8F4 */ be<s16> mLookSpeed;
    /* 0x8F6 */ u8 _8F6[2];
    /* 0x8F8 */ be<s16> mInitAngle[3];
    /* 0x8FE */ u8 _8FE[2];
    /* 0x900 */ be<u8> mbAttention;
    /* 0x901 */ be<u8> mbTalkRequest;
    /* 0x902 */ u8 _902[6];
    /* 0x908 */ be<u8> mbGreeted;
    /* 0x909 */ be<u8> mbTicketBought;
    /* 0x90A */ be<u8> mbItemShown;
    /* 0x90B */ be<u8> mItemBtpFrame;
    /* 0x90C */ be<u8> mbNight;
    /* 0x90D */ be<s8> mEvnLoopCount;
    /* 0x90E */ u8 _90E[2];
    /* 0x910 */ ProcFunc_l mAction;
    /* 0x918 */ be<s8> mBtpIdx;
    /* 0x919 */ be<s8> mAnmIdx;
    /* 0x91A */ u8 _91A[2];
    /* 0x91C */ be<u32> mNextMsgNo;
    /* 0x920 */ be<s8> mEventOrder;
    /* 0x921 */ u8 _921[3];
    /* 0x924 */ be<s32> mEventNo;
    /* 0x928 */ be<s16> mEventIds[4];
    /* 0x930 */ be<s8> mMode;
    /* 0x931 */ u8 _931[2];
    /* 0x933 */ be<s8> mActionState;
};
WWHD_OFFSET(daNpc_Kg2_c, mPhs, 0x7E0);
WWHD_OFFSET(daNpc_Kg2_c, mpBtp, 0x860);
WWHD_OFFSET(daNpc_Kg2_c, mBtpFrame, 0x8D8);
WWHD_OFFSET(daNpc_Kg2_c, mAction, 0x910);
WWHD_OFFSET(daNpc_Kg2_c, mEventIds, 0x928);
WWHD_OFFSET(daNpc_Kg2_c, mActionState, 0x933);

/* daNpc_Kg2_HIO_c (0x34 bytes, global at 0x10467628) */
struct daNpc_Kg2_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<f32> mEyeOffsetY;   /* overlays the start of a sub-object built by 0259DA18 */
    /* 0x08 */ be<s16> mJntParam[9];  /* dNpc_JntCtrl_c::setParam arguments */
    /* 0x1A */ be<s16> mLookSpeed;
    /* 0x1C */ be<f32> mEyeHeight;
    /* 0x20 */ be<s16> mAttentionAngle;
    /* 0x22 */ be<u8> field_0x22;
    /* 0x23 */ u8 _23;
    /* 0x24 */ be<f32> mAttentionDist;
    /* 0x28 */ u8 _28[4];
    /* 0x2C */ be<u8> field_0x2C;
    /* 0x2D */ u8 _2D[3];
    /* 0x30 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Kg2_HIO_c, 0x34);
