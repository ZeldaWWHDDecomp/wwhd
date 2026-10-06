/* daShopItem_c (shop display items), WWHD layout. 
 *
 * GameCube -> WWHD: daItemBase_c grew from 0x63C to 0x750 (d_a_itembase.h), so the members
 * move by +0x114; size 0x794 (GameCube 0x680).
 * HD vtable daShopItem_c 0x1003A8D4 (8 bytes per slot, function at +4): 0x0C deleting dtor
 * 02483F4C, 0x14 DrawBase, 0x1C setListStart 02483F48, 0x24 settingBeforeDraw, 0x2C setTevStr,
 * 0x34 animEntry, 0x3C clothCreate. */
#pragma once
#include "d/actor/d_a_itembase.h"

WWHD_OPAQUE(dCloth_packet_c);

struct daShopItem_c : daItemBase_c {
    const char* getShopArcname();
    s16 getShopBmdIdx();
    void CreateInit();
    bool _execute();
    void set_mtx();
    bool _draw();
    void settingBeforeDraw();
    void setTevStr();
    BOOL clothCreate();

    /* d_a_shop_item_static */
    cXyz* getScaleP();
    csXyz* getRotateP();
    cXyz* getPosP();

    /* 0x750 */ request_of_phase_process_class mPhase;   /* GameCube 0x63C */
    /* 0x758 */ gptr<dCloth_packet_c> field_0x644;
    /* 0x75C */ be<u8> field_0x648;
    /* 0x75D */ u8 _75D[3];
    /* 0x760 */ Mtx34 field_0x64C;
    /* 0x790 */ be<s32> mTevType;
};
WWHD_OFFSET(daShopItem_c, mPhase, 0x750);
WWHD_OFFSET(daShopItem_c, field_0x644, 0x758);
WWHD_OFFSET(daShopItem_c, field_0x64C, 0x760);
WWHD_OFFSET(daShopItem_c, mTevType, 0x790);
WWHD_SIZE(daShopItem_c, 0x794);

/* static tables (d_a_shop_item_static, .rodata):
 * mData[255] at 0x1003A930 (0x20 bytes: Vec mScale, Vec field_0x0C, SVec field_0x18),
 * mModelType[255] at 0x1003C910 */
namespace daShopItem_data {
inline u32 mData(u32 no) { return 0x1003A930 + no * 0x20; }
inline u8 mModelType(u32 no) { return gabi::load<u8>(0x1003C910 + no); }
}  // namespace daShopItem_data
