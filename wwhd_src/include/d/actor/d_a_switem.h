// Hit-trigger item dropper: WWHD actor layout.
#pragma once
#include "gc_api/gc_api.h"
#include "d/actor/d_a_item.h"
enum {
    AT_TYPE_SWORD = 0x2, AT_TYPE_BOMB = 0x20, AT_TYPE_BOOMERANG = 0x40, AT_TYPE_BOKO_STICK = 0x80,
    AT_TYPE_MACHETE = 0x400, AT_TYPE_SKULL_HAMMER = 0x10000, AT_TYPE_FIRE_ARROW = 0x4000,
    AT_TYPE_ICE_ARROW = 0x40000, AT_TYPE_LIGHT_ARROW = 0x80000, AT_TYPE_NORMAL_ARROW = 0x100000,
    AT_TYPE_STALFOS_MACE = 0x1000000, AT_TYPE_DARKNUT_SWORD = 0x4000000,
};
enum { JA_SE_OBJ_LUPY_OUT = 0x69E9, JA_SE_OBJ_ITEM_OUT = 0x69ED };
enum { daItemType_0_e = 0, daItemAct_1_e = 1 };

class daSwItem_c : public fopAc_ac_c {
public:
    daSwItem_c() {
        fopAc_ac_c_ct(this); __vtbl = 0x1003EBF4;
        dCcD_Stts_ct(&mStts); dCcD_Cyl_ct(&mCyl, 0x1003EBE4);
    } // HD: virtual destructor and inline collision constructors

    BOOL CreateInit();
    cPhs_State _create();
    bool _execute();
    BOOL isRupeeInAllCreateTable(int tbl);

    /* 0x3AC */ u8 m3AC[0x3B8 - 0x3AC];
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ be<u8> mAtTypeTrigger;
    /* 0x525 */ be<u8> mSpawnedItem;
    /* 0x526 */ u8 m526[2];
};
WWHD_OFFSET(daSwItem_c, mCyl, 0x3F4);
WWHD_OFFSET(daSwItem_c, mAtTypeTrigger, 0x524);
WWHD_SIZE(daSwItem_c, 0x528);

namespace daSwItem_prm {
inline u32 getAtType(daSwItem_c* i_this) { return (fopAcM_GetParam(i_this) >> 0x00) & 0xFF; }
inline u32 getItemTbl(daSwItem_c* i_this) { return (fopAcM_GetParam(i_this) >> 0x08) & 0x3F; }
inline u32 getItemBitNo(daSwItem_c* i_this) { return (fopAcM_GetParam(i_this) >> 0x0E) & 0x7F; }
}  // namespace daSwItem_prm


struct ItemTableList {
    u8 mMagic[0xA];
    be<s16> mEntryCount;
    be<u32> mPadding;
    be<u8> mItemTables[0x1E][0x10]; // HD: guest byte fields
};
