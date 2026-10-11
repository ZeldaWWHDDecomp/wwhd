/* Item tag: GameCube members move +0x11C with the WWHD actor base.
 * The unnamed field names follow tww. */
#pragma once
#include "gc_api/gc_api.h"

class daTagKbItem_c : public fopAc_ac_c {
public:
    daTagKbItem_c();
    enum : u32 { __vt = 0x1003F54C }; // HD: virtual destructor
    bool _delete();
    void CreateInit();
    cPhs_State _create();

    /* 0x3AC */ request_of_phase_process_class mPhase; /* unused */
    /* 0x3B4 */ be<u8> field_0x298;
    /* 0x3B5 */ be<u8> field_0x299;
    /* 0x3B6 */ u8 _3B6[2];
    /* 0x3B8 */ be<s32> mItemBitNo;
    /* 0x3BC */ be<u8> mItemNo;
    /* 0x3BD */ be<u8> mEnemyKind;
    /* 0x3BE */ u8 _3BE[2];
    /* 0x3C0 */ be<s32> mSwBitNo;
    /* 0x3C4 */ gptr<fopAc_ac_c> mpActor;
};
WWHD_OFFSET(daTagKbItem_c, mItemBitNo, 0x3B8);
WWHD_OFFSET(daTagKbItem_c, mpActor, 0x3C4);

WWHD_SIZE(daTagKbItem_c, 0x3C8);
