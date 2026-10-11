// Zunari shop posts and curtains. HD: actor fields shift from GC 0x290 to 0x3AC.
#pragma once
#include "gc_api/gc_api.h"

// HD: expose only the collision callback slot needed by this unit.
struct RotenBgW : dBgW {
    u8 mPadding[0xA8 - sizeof(dBgW)];
    be<u32> mpCrrFunc;
    void SetCrrFunc(u32 callback) { mpCrrFunc = callback; }
};
WWHD_OFFSET(RotenBgW, mpCrrFunc, 0xA8);

class daObj_Roten_c : public fopAc_ac_c {
public:
    daObj_Roten_c();
    BOOL CreateHeap();
    void set_mtx();
    s32 CreateInit();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ u8 field_0x29C[0x30];
    /* 0x3E8 */ gptr<RotenBgW> mpBgW;
    /* 0x3EC */ be<u8> mType;
    /* 0x3ED */ be<u8> field_0x2D1;
    /* 0x3EE */ u8 mPadding[2];
};
WWHD_OFFSET(daObj_Roten_c, mpModel, 0x3B4);
WWHD_OFFSET(daObj_Roten_c, mpBgW, 0x3E8);
WWHD_OFFSET(daObj_Roten_c, mType, 0x3EC);
WWHD_SIZE(daObj_Roten_c, 0x3F0);

struct daObj_Roten_HIO_c {
    // HD: GHS places this HIO's vtable last, unlike GameCube JORReflexible.
    /* 0x00 */ be<s8> mNo;
    u8 mPadding[3];
    /* 0x04 */ be<f32> field_0x08;
    /* 0x08 */ be<s16> field_0x0C;
    u8 mPadding2[2];
    /* 0x0C */ be<u32> mVtable;
};
WWHD_SIZE(daObj_Roten_HIO_c, 0x10);

enum {
    dRes_INDEX_ROTEN_BDL_ROTEN02_e = 4,
    dRes_INDEX_ROTEN_BDL_ROTEN03_e = 5,
    dRes_INDEX_ROTEN_BDL_ROTEN04_e = 6,
    dRes_INDEX_ROTEN_DZB_ROTEN02_e = 9,
    dRes_INDEX_ROTEN_DZB_ROTEN03_e = 10,
    dRes_INDEX_ROTEN_DZB_ROTEN04_e = 11,
};
BOOL dSv_player_get_bag_item_isReserve(dSv_player_get_bag_item_c* items, u32 index);
