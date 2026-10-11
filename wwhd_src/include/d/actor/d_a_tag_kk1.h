/* Tag - start of Mila's chase. GameCube names with WWHD layouts. Local use only. */
#pragma once
#include "gc_api/d/d_npc.h"
#include "gc_api/d/d_com_inf_game.h"
struct daTag_Kk1_HIO_c {
    struct hio_prm_c {
        union {
            struct { be<f32> mHorizontalDistance, mVerticalDistance; be<u8> field_0x10; u8 _09[3]; };
            be<u32> mWords[3];
        };
    };
    // HD: the HIO vtable follows its fields; GameCube has it at the front.
    be<s8> mNo; u8 _01[3]; hio_prm_c prm; be<u32> __vtbl;
    enum : u32 { __vt = 0x1003F6C0 };
};
WWHD_SIZE(daTag_Kk1_HIO_c::hio_prm_c, 0xC);
WWHD_OFFSET(daTag_Kk1_HIO_c, prm, 4);
WWHD_OFFSET(daTag_Kk1_HIO_c, __vtbl, 0x10);
WWHD_SIZE(daTag_Kk1_HIO_c, 0x14);
struct daTag_Kk1_c : fopNpc_npc_c {
    // HD: fopNpc_npc_c is 0x7DC; the GameCube fields at 0x6C4 follow it here.
    be<u8> padding_0x6C4, mTagSet, mNameIsWrong; u8 _7DF;
    enum : u32 { __vt = 0x1003F698 };
    daTag_Kk1_c() { __ct(static_cast<fopNpc_npc_c*>(this)); __vtbl = __vt; }
    BOOL _draw(); BOOL _execute(); BOOL _delete(); cPhs_State _create();
};
WWHD_OFFSET(daTag_Kk1_c, mTagSet, 0x7DD);
WWHD_OFFSET(daTag_Kk1_c, mNameIsWrong, 0x7DE);
WWHD_SIZE(daTag_Kk1_c, 0x7E0);
enum { fpcNm_TAG_KK1_e = 0x19F };
f32 tagKk1_squareDistance(const cXyz* first, const cXyz* second);
