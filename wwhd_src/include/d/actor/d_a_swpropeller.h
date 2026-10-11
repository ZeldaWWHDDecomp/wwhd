#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/gc_api.h"
void daObj_HitSeStart(const cXyz *, s32, const dCcD_GObjInf *, u32);
// HD: this sound entry point has no reverb argument.
void mDoAud_seStart3(u32, cXyz *, u32);
// HD: nodeCallBack reads the already-resolved native joint data.
struct SwPropJointDataHD {
    u8 _0[4];
    be<u16> mJntNo;
};
struct daSwProp_c : fopAc_ac_c {
    bool _delete();
    BOOL CreateHeap();
    void CreateInit();
    cPhs_State _create();
    void set_mtx();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs; /* GameCube 0x290 (+0x11C) */
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ u8 m408[0x558 - 0x524];
    /* 0x558 */ dBgS_ObjAcch mAcch;
    /* 0x71C */ dBgS_AcchCir mAcchCir;
    /* 0x75C */ be<s16> mRotY;
    /* 0x75E */ be<s16> mRotYVel;
    /* 0x760 */ be<s16> mRotYVelTarget;
    /* 0x762 */ u8 _762[2];
    /* 0x764 */ be<u32> m648;    /* GameCube m648 */
    /* 0x768 */ be<u8> mWindHit; /* wind hit in the previous frame */
    /* 0x769 */ be<u8> mType;
    /* 0x76A */ be<u8> mSpinDown; /* turning after a weapon hit */
    /* 0x76B */ u8 _76B;
};
WWHD_OFFSET(daSwProp_c, mStts, 0x3B8);
WWHD_OFFSET(daSwProp_c, mCyl, 0x3F4);
WWHD_OFFSET(daSwProp_c, mAcch, 0x558);
WWHD_OFFSET(daSwProp_c, mAcchCir, 0x71C);
WWHD_OFFSET(daSwProp_c, mRotY, 0x75C);
WWHD_OFFSET(daSwProp_c, mType, 0x769);
