// Helmaroc King idle demo actor, WWHD layout.
#pragma once
#include "gc_api/gc_api.h"

void fopAcM_SetMin(fopAc_ac_c*, f32, f32, f32);
void fopAcM_SetMax(fopAc_ac_c*, f32, f32, f32);

class demo_dk_class {
public:
    demo_dk_class();
    enum Mode_e { MODE_WAIT_e, MODE_AKUBI_e };
    /* 0x000 */ fopAc_ac_c actor;
    /* 0x3AC */ u8 unk290[0x1C];
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ u8 unk2B4[0x2];
    /* 0x3D2 */ be<u8> unk_2B6;
    /* 0x3D3 */ u8 _3D3;
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ be<s16> unk_2BC[2];
    /* 0x3DC */ be<s16> unk_2C0;
    /* 0x3DE */ u8 _3DE[2];
    /* 0x3E0 */ cXyz unk_2C4;
};
WWHD_OFFSET(demo_dk_class, mPhase, 0x3C8);
WWHD_OFFSET(demo_dk_class, mpMorf, 0x3D4);
WWHD_OFFSET(demo_dk_class, unk_2C4, 0x3E0);
WWHD_SIZE(demo_dk_class, 0x3EC);

