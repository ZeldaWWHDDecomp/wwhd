/* daNpc_Sarace_c (Loot the sailor, boating course), WWHD layout.
 *
 * The GameCube header is an empty placeholder (all functions Nonmatching); this layout is measured
 * from the WWHD code. The actor derives from fopNpc_npc_c (0x7DC, see d_a_npc_ba1.h) and has a
 * second morf for the head (resource 0x11) next to the body. Size 0x8B4 (profile
 * 101C6284), actor vtable 10021B28, HIO vtable 10021980. */
#pragma once
#include "d/actor/d_a_npc_ba1.h"

struct daNpc_Sarace_c : fopNpc_npc_l {
    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ gptr<mDoExt_McaMorf> mpHeadMorf;  /* head model (resource 0x11, animation 7; "headModelData") */
    /* 0x7E8 */ be<u32> mBuoyId[2];               /* process ids of the two course actors (proc 0x1C9) */
    /* 0x7F0 */ gptr<J3DAnmTexPattern> mpTexPattern;
    /* 0x7F4 */ u8 mBtpAnm[0x74];                 /* mDoExt_btpAnm (HD 0x74) */
    /* 0x868 */ be<u8> mBtpFrame;
    /* 0x869 */ u8 _869;
    /* 0x86A */ be<s16> mBlinkTimer;
    /* 0x86C */ cXyz mEyePos;                     /* copied to eyePos (0x37C) */
    /* 0x878 */ cXyz mAttPos;                     /* copied to attention position (0x390) */
    /* 0x884 */ be<s16> mLookAngle;
    /* 0x886 */ u8 _886[2];
    /* 0x888 */ be<u16> mHomeAngle[3];            /* copy of current.angle */
    /* 0x88E */ u8 _88E[0x894 - 0x88E];
    /* 0x894 */ be<u32> mEventMsgNo;
    /* 0x898 */ be<u8> mbAttention;
    /* 0x899 */ be<u8> mbTalkRequest;
    /* 0x89A */ u8 _89A[0x8A0 - 0x89A];
    /* 0x8A0 */ be<u8> field_0x8A0;
    /* 0x8A1 */ u8 _8A1[3];
    /* 0x8A4 */ ProcFunc_l mAction;
    /* 0x8AC */ be<u8> field_0x8AC;
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
