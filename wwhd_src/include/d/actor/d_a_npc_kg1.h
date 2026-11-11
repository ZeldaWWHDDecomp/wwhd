/* Salvatore, WWHD layout. */
#pragma once
#include "d/actor/d_a_npc_ba1.h"
struct daNpc_Kg1_c : fopNpc_npc_l {
    /* 0x7DC */ gptr<J3DModel> mpPropModel;
    /* 0x7E0 */ u8 _7E0[0x854-0x7E0];
    /* 0x854 */ request_of_phase_process_class mPhs;
    /* 0x85C */ ProcFunc_l mAction;
    /* 0x864 */ u8 _864[4];
    /* 0x868 */ gptr<J3DAnmTexPattern> m_eye_tex_pattern;  /* was mpTexPattern (misnamed): the eye pattern; the core class initTexPatternAnm 0226586C asserts it as "m_eye_tex_pattern != (0)" (kg1_core::daNpc_Kg1_c::m_eye_tex_pattern) */
    /* 0x86C */ u8 mEyeBtpAnm[0x74];  /* was mBtpAnm (misnamed): the eye pattern animation (kg1_core::daNpc_Kg1_c::mEyeBtpAnm, set up with m_eye_tex_pattern); mBtpAnm is the prop model's pattern at 0x7E0 (tww mBtpAnm at GameCube 0x6C8) */
    /* 0x8E0 */ u8 _8E0[0x954-0x8E0];
    /* 0x954 */ be<u8> mBtpFrame;
    /* 0x955 */ u8 _955;
    /* 0x956 */ be<s16> mBlinkTimer;
    /* 0x958 */ u8 _958[2];
    /* 0x95A */ be<u8> mTalking;  /* was mTalkActive (misnamed): kg1_core::daNpc_Kg1_c::mTalking (checkOrder 022663A4 sets it on the talk command; same code as daNpc_Ba1_c::mTalking); one name per field */
    /* 0x95B */ u8 _95B;
    /* 0x95C */ be<u8> mOrderEvent;  /* was mEventOrder (misnamed): kg1_core::daNpc_Kg1_c::mOrderEvent (eventOrder 022664A4 orders by it; same code as daNpc_Ba1_c::mOrderEvent); one name per field */
    /* 0x95D */ u8 _95D[0x960-0x95D];
    /* 0x960 */ cXyz mEyePos;
    /* 0x96C */ cXyz mHeadPos;
    /* 0x978 */ be<s8> mAnimation;
    /* 0x979 */ be<s8> mRequestedAnimation;
    /* 0x97A */ be<s8> mPreviousAnimation;
    /* 0x97B */ be<u8> mSpecialAnimation;
    /* 0x97C */ u8 _97C[0x99E - 0x97C];
    /* 0x99E */ be<u8> mSequenceFlags[6];
    /* 0x9A4 */ u8 _9A4[8];
    /* 0x9AC */ be<s32> mWaitMode;
    /* 0x9B0 */ be<s16> mEventIds[3];
    /* 0x9B6 */ u8 _9B6[2];
};
WWHD_OFFSET(daNpc_Kg1_c, mPhs, 0x854);
WWHD_OFFSET(daNpc_Kg1_c, mBtpFrame, 0x954);
WWHD_OFFSET(daNpc_Kg1_c, mWaitMode, 0x9AC);
WWHD_SIZE(daNpc_Kg1_c, 0x9B8);
