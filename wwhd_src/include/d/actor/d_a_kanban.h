/* kanban_class (cuttable sign), WWHD layout. 
 *
 * GameCube -> WWHD: fopAc_ac_c +0x11C, and every member +0x11C up to the eleven models; the
 * shadow id m570 is gone (HD shadows: daKanban_Draw sets none), so the dCcD_Stts and dCcD_Cyl
 * are +0x118. Size 0x7F8 (GameCube 0x6E0); offsets from daKanban_Create (inline constructors)
 * and the deleting destructor 0218CF60. */
#pragma once
#include "bindings.h"

/* dPa_rippleEcallBack (0x14): the emitter rate at +0x10 (setRate) */
struct dPa_rippleEcallBack_l {
    /* 0x00 */ u8 _00[0x10];
    /* 0x10 */ be<f32> mRate;
};
WWHD_SIZE(dPa_rippleEcallBack_l, 0x14);

struct kanban_class {
    /* 0x000 */ fopAc_ac_c actor;
    /* 0x3AC */ be<u8> m290;            /* bool: a cut-off part */
    /* 0x3AD */ u8 _3AD[3];
    /* 0x3B0 */ be<u32> m294;           /* parts mask */
    /* 0x3B4 */ be<u8> m298;
    /* 0x3B5 */ u8 _3B5;
    /* 0x3B6 */ be<u8> m29A;
    /* 0x3B7 */ be<u8> m29B;
    /* 0x3B8 */ be<u8> m29C;
    /* 0x3B9 */ u8 _3B9[3];
    /* 0x3BC */ be<f32> m2A0;
    /* 0x3C0 */ be<f32> m2A4;
    /* 0x3C4 */ be<f32> m2A8;
    /* 0x3C8 */ csXyz m2AC;
    /* 0x3CE */ be<s16> m2B2[5];
    /* 0x3D8 */ be<s16> m2BC;
    /* 0x3DA */ be<s16> m2BE;
    /* 0x3DC */ be<s16> m2C0;
    /* 0x3DE */ be<s16> m2C2;
    /* 0x3E0 */ be<s16> m2C4;
    /* 0x3E2 */ be<s16> m2C6;
    /* 0x3E4 */ be<u32> m2C8;           /* fpc_ProcID of the mother sign */
    /* 0x3E8 */ cXyz m2CC;
    /* 0x3F4 */ cXyz m2D8;
    /* 0x400 */ be<f32> m2E4;
    /* 0x404 */ be<f32> m2E8;
    /* 0x408 */ be<f32> m2EC;
    /* 0x40C */ csXyz m2F0;
    /* 0x412 */ u8 _412[2];
    /* 0x414 */ cXyz m2F8;
    /* 0x420 */ cXyz m304;
    /* 0x42C */ dBgS_AcchCir m310;
    /* 0x46C */ dBgS_ObjAcch m350;
    /* 0x630 */ dPa_rippleEcallBack_l m514;
    /* 0x644 */ cXyz m528;
    /* 0x650 */ u8 m534[0x658 - 0x650];
    /* 0x658 */ request_of_phase_process_class mPhase;
    /* 0x660 */ gptr<J3DModel> m544[11];
    /* 0x68C */ dCcD_Stts m574;         /* HD: m570 (shadow id) removed */
    /* 0x6C8 */ dCcD_Cyl m5B0;
};
WWHD_OFFSET(kanban_class, m2B2, 0x3CE);
WWHD_OFFSET(kanban_class, m2C8, 0x3E4);
WWHD_OFFSET(kanban_class, m2F8, 0x414);
WWHD_OFFSET(kanban_class, m350, 0x46C);
WWHD_OFFSET(kanban_class, m514, 0x630);
WWHD_OFFSET(kanban_class, mPhase, 0x658);
WWHD_OFFSET(kanban_class, m574, 0x68C);
WWHD_OFFSET(kanban_class, m5B0, 0x6C8);
WWHD_SIZE(kanban_class, 0x7F8);
