#pragma once
#include "d/actor/d_a_shop_item.h"
template <u32 Address> inline constexpr u32 shopCall = Address;
struct ShopClothVtable {
    u8 _00[0x44];
    gfn mClothDraw;
};
WWHD_OFFSET(ShopClothVtable, mClothDraw, 0x44);
struct ShopClothView {
    u8 _00[0xC];
    gptr<ShopClothVtable> mVtable;
    u8 _10[0xC8];
    cXyz mScale;
};
WWHD_OFFSET(ShopClothView, mVtable, 0xC);
WWHD_OFFSET(ShopClothView, mScale, 0xD8);
WWHD_SIZE(ShopClothView, 0xE4);
struct daShopItem_c_m_data {
    cXyz mScale;
    cXyz field_0x0C;
    csXyz field_0x18;
    u8 _1E[2];
};
WWHD_OFFSET(daShopItem_c_m_data, field_0x0C, 0xC);
WWHD_OFFSET(daShopItem_c_m_data, field_0x18, 0x18);
WWHD_SIZE(daShopItem_c_m_data, 0x20);
struct ShopItemResource {
    gptr<const char> mArcname;
    u8 _04[4];
    be<s16> mBmdIdx;
    u8 _0A[0x16];
    be<u16> mHeapSize;
    u8 _22[2];
};
WWHD_OFFSET(ShopItemResource, mHeapSize, 0x20);
WWHD_SIZE(ShopItemResource, 0x24);
struct ShopFieldResource {
    gptr<const char> mArcname;
    be<s16> mBmdIdx;
    u8 _06[0x12];
    be<u16> mHeapSize;
    u8 _1A[2];
};
WWHD_OFFSET(ShopFieldResource, mHeapSize, 0x18);
WWHD_SIZE(ShopFieldResource, 0x1C);
struct ShopJointView {
    u8 _00[0x14];
    be<u32> mMtxCalc;
};
WWHD_OFFSET(ShopJointView, mMtxCalc, 0x14);
struct ShopText {
    char bytes[1];
};
#define SAFESTRING_VTBL 0x1003A844 /* this TU's copy of the sead::SafeString vtable */

#define DASHOPITEM_VTBL 0x1003A8D4
#define DAITEMBASE_VTBL 0x10012158

enum {
    dItemNo_GREEN_RUPEE_e = 0x01,
    dItemNo_SMALL_KEY_e = 0x15,
    dItemNo_BOMB_BAG_e = 0x31,
    dItemNo_SKULL_HAMMER_e = 0x33,
    dItemNo_TOWN_FLOWER_e = 0x8C,
    dItemNo_HEROS_FLAG_e = 0x8F,
    dItemNo_BIG_CATCH_FLAG_e = 0x90,
    dItemNo_BIG_SALE_FLAG_e = 0x91,
    dItemNo_FOUNTAIN_IDOL_e = 0x95,
    dItemNo_POSTMAN_STATUE_e = 0x96,
    dItemNo_SHOP_GURU_STATUE_e = 0x97,
};

enum { TEV_TYPE_BG1_PLIGHT = 0x5C }; /* HD value */

// HD: unit-local bindings retain the verified calling conventions.
/* 025510A8 isUseClothPacket(u8), 02550FAC isDaizaItem(u8) (d_item) */
static inline BOOL isUseClothPacket(u8 itemNo) {
    return gabi::call<BOOL>(shopCall<0x025510A8>, itemNo);
}
static inline BOOL isDaizaItem(u8 itemNo) { return gabi::call<BOOL>(shopCall<0x02550FAC>, itemNo); }
/* 0251B638 dCloth_packet_c::setMtx(Mtx); setScale inline: scale at +0xD8 (HD); cloth_draw is
 * virtual: vtable at +0xC, slot +0x44 */
static inline void dCloth_packet_setMtx(dCloth_packet_c *c, Mtx34 *m) {
    gabi::call(shopCall<0x0251B638>, c, m);
}
static inline void dCloth_packet_setScale(dCloth_packet_c *c, const cXyz *s) {
    f32 y = s->y, x = s->x, z = s->z;
    auto *cloth = reinterpret_cast<ShopClothView *>(c);
    cloth->mScale.x = x;
    cloth->mScale.y = y;
    cloth->mScale.z = z;
}
static inline void dCloth_packet_cloth_draw(dCloth_packet_c *c) {
    auto *cloth = reinterpret_cast<ShopClothView *>(c);
    gabi::call_ptr(cloth->mVtable->mClothDraw.get(), c);
}
/* 0252CA4C dDlst_texSpecmapST(cXyz* eye, dKy_tevstr_c*, J3DModel*, f32): HD passes the model
 * (copy of the d_a_itembase local binding) */
static inline void dDlst_texSpecmapST(cXyz *eye, dKy_tevstr_c *tev, J3DModel *model, f32 scale) {
    gabi::call(shopCall<0x0252CA4C>, eye, tev, model, scale);
}
/* mDoLib_clipper::getFar(): the float at 0x1048D04C */
static inline f32 mDoLib_clipper_getFar() { return gabi::object_at<be<f32>>(0x1048D04C); }
/* d_item_data: the solid heap sizes (item_resource +0x20, field_item_res +0x18) */
static inline u16 dItem_data_getHeapSize(u32 no) {
    return gabi::at<ShopItemResource>(dItemDataView::item_resource(no))->mHeapSize;
}
static inline u16 dItem_data_getFieldHeapSize(u32 no) {
    return gabi::at<ShopFieldResource>(dItemDataView::field_item_res(no))->mHeapSize;
}

/* HD: preserve the original's whole stack frame. set_mtx and
 * clothCreate index four-entry stack arrays with a u8 field they (re)load after calls; any index
 * is possible under clobbering, and the original then reads the rest of its frame (padding, the
 * saved GPRs) or the caller's frame. The candidate keeps its arrays at the original's offsets in
 * a frame of the same size, with the saved GPRs (entry values of rFIRST..r31) and the LR save
 * word (entry SP + 4) the original writes in its prologue. */
struct OrigFrame : GuestFrame {
    OrigFrame(u32 n, int firstGpr, u32 gprOff) : GuestFrame(n) {
        const u32 entry = sp + n;
        gabi::store<u32>(sp, entry);
        gabi::store<u32>(entry + 4, gabi::cpu->lr);
        for (int r = firstGpr; r < 32; r++)
            gabi::store<u32>(sp + gprOff + 4 * (r - firstGpr), gabi::cpu->r[r]);
    }
};

inline BOOL shopDrawTail(void *actor) { return gabi::call<BOOL>(shopCall<0x02483B28>, actor); }
inline BOOL shopExecuteTail(void *actor) { return gabi::call<BOOL>(shopCall<0x02483BBC>, actor); }
inline void shopDestroyItemBase(daShopItem_c *actor) { gabi::call(shopCall<0x021832F4>, actor, 0); }

namespace shopAbi {
enum : u32 {
    FTEX03 = 0x20,
    FTEX04 = 0x21,
    FTEX05 = 0x22,
    FTEX07 = 0x23,
    CLOTHTOON = 3,
    ClothVobj03 = 0x0251BFA4,
    ClothVobj04 = 0x0251C134,
    ClothVobj05 = 0x0251C2D0,
    ClothVobj07 = 0x0251C478,
    AcchVtable = 0x1003A86C,
    AcchVtable2 = 0x1003A88C,
    AcchVtable3 = 0x1003A87C,
    CylVtable = 0x1003A85C,
    CheckItemCreateHeap = 0x021841D0,
    CheckFieldItemCreateHeap = 0x0218422C,
    HeaderStatics = 0x1046DE08,
    HeaderStaticsData = 0x101D051C,
    StaticHeaderStatics = 0x1046DE24,
    StaticHeaderStaticsData = 0x101D0570
};
}
