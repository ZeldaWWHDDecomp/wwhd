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
    void setTevType(s32 type) { mTevType = type; } /* [v nz] */

    /* 0x750 */ request_of_phase_process_class mPhase;   /* GameCube 0x63C */
    /* 0x758 */ gptr<dCloth_packet_c> mpCloth;  /* was field_0x644: clothCreate 02483C10 stores the dCloth_packet_c made for the flag items (nullptr for the others); set_mtx 02483508 sets its scale and matrix; _draw 02483B28 draws it (dCloth_packet_cloth_draw) (tww the same) */
    /* 0x75C */ be<u8> mClothType;  /* was field_0x648: clothCreate 02483C10 stores the cloth kind of the item (0-3, by item number), the index into clothRes / clothFunc; set_mtx 02483508 reads it again (tww the same) */
    /* 0x75D */ u8 _75D[3];
    /* 0x760 */ Mtx34 mMtx;  /* was field_0x64C: set_mtx 02483508 copies the model matrix (mDoMtx_stack_c::get()) into it; CreateInit 024836F4 sets cullMtx to it (fopAcM_SetMtx) (tww the same) */
    /* 0x790 */ be<s32> mTevType;
};
WWHD_OFFSET(daShopItem_c, mPhase, 0x750);
WWHD_OFFSET(daShopItem_c, mpCloth, 0x758);
WWHD_OFFSET(daShopItem_c, mMtx, 0x760);
WWHD_OFFSET(daShopItem_c, mTevType, 0x790);
WWHD_SIZE(daShopItem_c, 0x794);

/* static tables (d_a_shop_item_static, .rodata):
 * mData[255] at 0x1003A930 (0x20 bytes: Vec mScale, Vec field_0x0C, SVec field_0x18),
 * mModelType[255] at 0x1003C910 */
namespace daShopItem_data {
inline u32 mData(u32 no) { return 0x1003A930 + no * 0x20; }
inline u8 mModelType(u32 no) { return gabi::load<u8>(0x1003C910 + no); }
}  // namespace daShopItem_data
