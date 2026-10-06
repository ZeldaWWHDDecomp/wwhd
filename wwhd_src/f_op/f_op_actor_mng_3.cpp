/* f_op_actor_mng part 3: 025D7D40..025D8FFC — item creation. WWHD. See f_op_actor_mng.cpp for the unit's range.
 *
 * Process names: ITEM 0xFF, Demo_Item 0x101, RACEITEM 0x102, ShopItem 0x103, SPC_ITEM01 0x105,
 * NPC_FA1 0x168. Item params as on GameCube: itemNo | bitNo << 8 | switchNo2 << 16 | type << 24 |
 * action << 26. daItem_c (HD): flags +0x783, status +0x785. The item tables are at
 * *(play+0x5D24) + 0x10 (16 entries per table). */
#include "bindings.h"

namespace f_op_actor_mng_3_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
static inline u32 fopAcM_create_l(u32 name, u32 prm, u32 pos, u32 room, u32 angle, u32 scale, u32 arg, u32 fn) {
    return gabi::call<u32>(0x025D5834, name, prm, pos, room, angle, scale, arg, fn);
}
static inline u32 fopAcM_fastCreate_l(u32 name, u32 prm, u32 pos, u32 room, u32 angle, u32 scale, u32 arg, u32 fn, u32 data) {
    return gabi::call<u32>(0x025D5928, name, prm, pos, room, angle, scale, arg, fn, data);
}
static inline u32 check_itemno_l(u32 itemNo) { return gabi::call<u32>(0x02551150, itemNo); }
static inline u32 getItemNoByLife_l(u32 itemNo) { return gabi::call<u32>(0x02551108, itemNo); }
static inline BOOL isHeart_l(u32 itemNo) { return gabi::call<BOOL>(0x025510E8, itemNo); }
static inline f32 cM_rndF_l(f32 x) { return gabi::call<f32>(0x020198D8, x); }
static inline f32 cM_rndFX_l(f32 x) { return gabi::call<f32>(0x02019918, x); }
static inline void csXyz_ct_l(u32 p, s16 x, s16 y, s16 z) { gabi::call(0x0201A478, p, x, y, z); }
static inline void PSVECAdd_l(u32 a, u32 b, u32 out) { gabi::call(0x028E8D88, a, b, out); }
static inline u32 save_sub_l(u32 p) { return gabi::call<u32>(0x027200D0, p); }   /* HD: save+0x12C0 accessor */
static inline BOOL heroMode_l(u32 p) { return gabi::call<BOOL>(0x0271FC5C, p); } /* HD: probably the Hero Mode check */

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }

static inline u32 divw_ppc(u32 a, u32 b) {
    if (b == 0 || (a == 0x80000000u && b == 0xFFFFFFFFu)) return ((s32)a < 0) ? 0xFFFFFFFFu : 0;
    return (u32)((s32)a / (s32)b);
}

static constexpr u32 kZeroSXyz = 0x101FFB14;
static constexpr u32 SAVE = 0x101F84DC; /* -> dSv_info_c; life +0x22, max life +0x20 */

struct csXyz_l { be<u16> x, y, z; };
struct cXyz_l { be<f32> x, y, z; };

/* the GameCube item-number assert: 0 <= itemNo < max && -1 <= bitNo <= 79 || bitNo == 127 */
static inline void item_assert(u32 itemNo, u32 max, u32 bitNo, u32 file, s32 line, u32 msg) {
    if ((itemNo >= max || bitNo + 1 >= 0x51) && bitNo != 0x7F) JUT_ASSERT_l(file, line, msg);
}
static inline void item_assert1(u32 itemNo, u32 file, s32 line, u32 msg) {
    if (itemNo >= 0x100) JUT_ASSERT_l(file, line, msg);
}

/* 025D7D40 */
static u32 fopAcM_createDemoItem(u32 pos, u32 i_itemNo, u32 i_itemBitNo, u32 i_angle, u32 i_roomNo, u32 i_scale, u32 i_argFlag) {
    WWHD_FUNC(0x025D7D40, u32, pos, i_itemNo, i_itemBitNo, i_angle, i_roomNo, i_scale, i_argFlag);
    item_assert(i_itemNo, 0x100, i_itemBitNo, 0x100574B4, 0xD39, 0x100574C8);
    if (i_itemNo == 0xFF) return (u32)-1;
    u32 params = (i_itemNo & 0xFF) | ((i_itemBitNo << 8) & 0x7F00) | (i_argFlag << 16);
    return fopAcM_create_l(0x101, params, pos, i_roomNo, i_angle, i_scale, (u32)-1, 0);
}
VERIFY(0x025D7D40, fopAcM_createDemoItem);

/* 025D7DEC */
static u32 fopAcM_createItemForPresentDemo(u32 pos, u32 i_itemNo, u32 argFlag, u32 roomNo, u32 param_5, u32 angle, u32 scale) {
    WWHD_FUNC(0x025D7DEC, u32, pos, i_itemNo, argFlag, roomNo, param_5, angle, scale);
    item_assert1(i_itemNo, 0x1005754C, 0xB8B, 0x1005752C);
    st8(dComIfGp_ea() + 0x52A4, (u8)i_itemNo); /* dComIfGp_event_setGtItm */
    if (i_itemNo == 0xFF) return (u32)-1;
    return fopAcM_createDemoItem(pos, i_itemNo, roomNo, angle, param_5, scale, argFlag); /* as on GameCube: roomNo in the bit slot */
}
VERIFY(0x025D7DEC, fopAcM_createItemForPresentDemo);

/* 025D7E88 */
static u32 fopAcM_createItemForTrBoxDemo(u32 pos, u32 i_itemNo, u32 roomNo, u32 param_5, u32 angle, u32 scale) {
    WWHD_FUNC(0x025D7E88, u32, pos, i_itemNo, roomNo, param_5, angle, scale);
    item_assert1(i_itemNo, 0x10057580, 0xBB8, 0x10057560);
    st8(dComIfGp_ea() + 0x52A4, (u8)i_itemNo);
    if (i_itemNo == 0xFF) return (u32)-1;
    return fopAcM_createDemoItem(pos, i_itemNo, roomNo, angle, param_5, scale, 0);
}
VERIFY(0x025D7E88, fopAcM_createItemForTrBoxDemo);

static inline u32 item_params(u32 itemNo, u32 bitNo, u32 type, u32 action) {
    u32 no = check_itemno_l(itemNo);
    return (no | ((bitNo << 8) & 0xFF00) | 0xFF0000 | ((type << 24) & 0x03000000)) | (action << 26);
}

/* 025D7F20 */
static u32 fopAcM_fastCreateItem2(u32 pos, u32 i_itemNo, u32 i_itemBitNo, u32 roomNo, u32 type, u32 angle, u32 action, u32 scale) {
    WWHD_FUNC(0x025D7F20, u32, pos, i_itemNo, i_itemBitNo, roomNo, type, angle, action, scale);
    item_assert(i_itemNo, 0x100, i_itemBitNo, 0x10057594, 0xDEF, 0x100575A8);
    if (i_itemNo == 0xFF) return 0;
    gabi::Local<csXyz_l> prmAngle;
    if (angle != 0) {
        prmAngle->x = ld16(angle + 0);
        prmAngle->y = ld16(angle + 2);
    } else {
        st(gabi::ea(prmAngle.get()), ld(kZeroSXyz)); /* csXyz::Zero x, y */
    }
    prmAngle->z = 0xFF;
    u32 params = item_params(i_itemNo, i_itemBitNo, type, action);
    if (i_itemNo == 0x16) /* RECOVER_FAIRY */
        return fopAcM_fastCreate_l(0x168, 1, pos, roomNo, angle, scale, (u32)-1, 0, 0);
    if (i_itemNo == 0x1E) /* TRIPLE_HEART: two extra hearts */
        for (int i = 0; i < 2; i++) fopAcM_fastCreate_l(0xFF, params, pos, roomNo, gabi::ea(prmAngle.get()), scale, (u32)-1, 0, 0);
    return fopAcM_fastCreate_l(0xFF, params, pos, roomNo, gabi::ea(prmAngle.get()), scale, (u32)-1, 0, 0);
}
VERIFY(0x025D7F20, fopAcM_fastCreateItem2);

/* the life percentage (dComIfGs_getLife() * 100 / (dComIfGs_getMaxLife() & 0xFC)) */
static inline u8 life_percent() {
    u32 save = ld(SAVE);
    u32 life = ld16(save + 0x22);
    u32 max = ld16(save + 0x20) & 0xFC;
    return (u8)divw_ppc(life * 100, max);
}

/* 025D8120: HD: with action 0xD a low life (<= 40%) switches any table to the heart tables */
static u32 fopAcM_createItemFromTable(u32 p_pos, u32 i_itemNo, u32 i_itemBitNo, u32 roomNo, u32 type, u32 p_angle, u32 action, u32 p_scale) {
    WWHD_FUNC(0x025D8120, u32, p_pos, i_itemNo, i_itemBitNo, roomNo, type, p_angle, action, p_scale);
    item_assert(i_itemNo, 0x40, i_itemBitNo, 0x10057618, 0xBEF, 0x1005762C);
    /* static cXyz fairy_offset_tbl[3] (function-local static, initialised on first use) */
    const u32 tbl = 0x10487494;
    if (ld(0x10487508) == 0) {
        stf(tbl + 0x04, 0.0f);
        stf(tbl + 0x18, 0.0f);
        stf(tbl + 0x14, 0.0f);
        stf(tbl + 0x0C, -40.0f);
        stf(tbl + 0x20, 40.0f);
        stf(tbl + 0x00, 40.0f);
        st(0x10487508, 1);
        stf(tbl + 0x08, 0.0f);
        stf(tbl + 0x1C, 0.0f);
        stf(tbl + 0x10, 0.0f);
    }
    gabi::Local<csXyz_l> angle;
    gabi::Local<cXyz_l> pos;
    pos->x = 0.0f;
    pos->y = 0.0f;
    pos->z = 0.0f;
    csXyz_ct_l(gabi::ea(angle.get()), 0, 0, 0);
    u32 itemNo = i_itemNo;
    u32 tableIdx = i_itemNo - 0x20;
    if (tableIdx < 0x1F) {
        u32 itemTableList = ld(dComIfGp_ea() + 0x5D24);
        u32 t = tableIdx;
        if (action == 0xD) {
            u8 pct = life_percent();
            if ((u8)(pct - 1) < 0x14) t = 4;
            else if ((u8)(pct - 0x15) < 0x14) t = 3;
        }
        if (t >= 1 && t <= 4) {
            u8 pct = life_percent();
            u32 sel;
            if ((u8)(pct - 1) < 0x14) sel = 4;
            else if ((u8)(pct - 0x15) < 0x14) sel = 3;
            else if ((u8)(pct - 0x29) < 0x28) sel = 2;
            else if ((u8)(pct - 0x51) < 0x14) sel = 1;
            else sel = t;
            s32 idx = gabi::ftoi(cM_rndF_l(15.9999f));
            itemNo = ld8(itemTableList + (sel << 4) + idx + 0x10);
        } else if (t >= 0xB && t <= 0x14) {
            if (itemTableList == 0) return (u32)-1;
            u32 p = itemTableList + (t << 4) + 0x10;
            u32 no = ld8(p);
            u32 lastItemPID = (u32)-1;
            u32 off = 0;
            for (s32 i = 0; no != 0xFF && i < 0x10; i++) {
                if (p_pos != 0) {
                    st(gabi::ea(pos.get()) + 0, ld(p_pos + 0));
                    st(gabi::ea(pos.get()) + 4, ld(p_pos + 4));
                    st(gabi::ea(pos.get()) + 8, ld(p_pos + 8));
                }
                if (p_angle != 0) {
                    angle->x = ld16(p_angle + 0);
                    angle->y = ld16(p_angle + 2);
                    angle->z = ld16(p_angle + 4);
                }
                if (t == 0x16) { /* never true (GameCube bug: meant itemNo == RECOVER_FAIRY) */
                    PSVECAdd_l(gabi::ea(pos.get()), tbl + off, gabi::ea(pos.get()));
                    angle->y = (u16)gabi::ftoi(cM_rndF_l(32766.0f));
                }
                u32 n = getItemNoByLife_l(no & 0xFF);
                u32 item = fopAcM_fastCreateItem2(gabi::ea(pos.get()), n, i_itemBitNo, roomNo, type, gabi::ea(angle.get()), 8, 0);
                if (item == 0) return (u32)-1;
                lastItemPID = ld(item + 4);
                if (lastItemPID == (u32)-1) return lastItemPID;
                p++;
                no = ld8(p);
                off += 0xC;
            }
            return lastItemPID;
        } else {
            s32 idx = gabi::ftoi(cM_rndF_l(15.9999f));
            itemNo = ld8(itemTableList + (t << 4) + idx + 0x10);
        }
    }
    if (itemNo == 0x3F || itemNo == 0xFF) return (u32)-1;
    u32 n = getItemNoByLife_l(itemNo & 0xFF);
    u32 item = fopAcM_fastCreateItem2(p_pos, n, i_itemBitNo, roomNo, type, p_angle, action, p_scale);
    if (item == 0) return (u32)-1;
    return ld(item + 4);
}
VERIFY(0x025D8120, fopAcM_createItemFromTable);

/* 025D85E4: HD: in Hero Mode (probably) a heart becomes item 1 */
static u32 fopAcM_createRaceItem(u32 pos, u32 i_itemNo, u32 i_itemBitNo, u32 angle, u32 roomNo, u32 scale, u32 param_7) {
    WWHD_FUNC(0x025D85E4, u32, pos, i_itemNo, i_itemBitNo, angle, roomNo, scale, param_7);
    item_assert(i_itemNo, 0x100, i_itemBitNo, 0x10057690, 0xCFB, 0x100576A4);
    if (i_itemNo == 0xFF) return (u32)-1;
    u32 itemNo = check_itemno_l(i_itemNo);
    u32 x = save_sub_l(ld(SAVE) + 0x12C0);
    if (heroMode_l(x) != 0 && isHeart_l(itemNo & 0xFF) != 0) itemNo = 1;
    u32 params = (itemNo & 0xFF) | ((i_itemBitNo << 8) & 0x7F00) | ((param_7 << 15) & 0x78000);
    return fopAcM_create_l(0x102, params, pos, roomNo, angle, scale, (u32)-1, 0);
}
VERIFY(0x025D85E4, fopAcM_createRaceItem);

/* 025D86E0 */
static u32 fopAcM_createRaceItemFromTable(u32 pos, u32 i_itemNo, u32 i_itemBitNo, u32 i_roomNo, u32 angle, u32 scale, u32 param_7) {
    WWHD_FUNC(0x025D86E0, u32, pos, i_itemNo, i_itemBitNo, i_roomNo, angle, scale, param_7);
    item_assert(i_itemNo, 0x40, i_itemBitNo, 0x10057708, 0xC94, 0x1005771C);
    u32 itemNo = i_itemNo;
    if (i_itemNo - 0x20 < 0x1F) {
        u32 t = ld(dComIfGp_ea() + 0x5D24) + ((i_itemNo - 0x20) << 4);
        s32 idx = gabi::ftoi(cM_rndF_l(15.9999f));
        itemNo = ld8(t + idx + 0x10);
    }
    if (itemNo == 0x3F || itemNo == 0xFF) return (u32)-1;
    u32 n = getItemNoByLife_l(itemNo & 0xFF);
    return fopAcM_createRaceItem(pos, n, i_itemBitNo, angle, i_roomNo, scale, param_7);
}
VERIFY(0x025D86E0, fopAcM_createRaceItemFromTable);

/* 025D87E4 */
static u32 fopAcM_createShopItem(u32 pos, u32 i_itemNo, u32 angle, u32 roomNo, u32 scale, u32 createFunc) {
    WWHD_FUNC(0x025D87E4, u32, pos, i_itemNo, angle, roomNo, scale, createFunc);
    item_assert1(i_itemNo, 0x100577A0, 0xCCD, 0x10057780);
    if (i_itemNo == 0xFF) return (u32)-1;
    return fopAcM_create_l(0x103, i_itemNo, pos, roomNo, angle, scale, (u32)-1, createFunc);
}
VERIFY(0x025D87E4, fopAcM_createShopItem);

/* 025D8870 */
static u32 fopAcM_createItem(u32 pos, u32 i_itemNo, u32 i_itemBitNo, u32 roomNo, u32 type, u32 angle, u32 action, u32 scale) {
    WWHD_FUNC(0x025D8870, u32, pos, i_itemNo, i_itemBitNo, roomNo, type, angle, action, scale);
    item_assert(i_itemNo, 0x100, i_itemBitNo, 0x100577B4, 0xD9F, 0x100577C8);
    if (i_itemNo == 0xFF) return (u32)-1;
    gabi::Local<csXyz_l> prmAngle;
    if (angle != 0) {
        prmAngle->x = ld16(angle + 0);
        prmAngle->y = ld16(angle + 2);
    } else {
        st(gabi::ea(prmAngle.get()), ld(kZeroSXyz));
    }
    prmAngle->z = 0xFF;
    u32 params = item_params(i_itemNo, i_itemBitNo, type, action);
    if (i_itemNo == 0x16) return fopAcM_create_l(0x168, 1, pos, roomNo, angle, scale, (u32)-1, 0);
    if (i_itemNo == 0x1E)
        for (int i = 0; i < 2; i++) fopAcM_create_l(0xFF, params, pos, roomNo, gabi::ea(prmAngle.get()), scale, (u32)-1, 0);
    return fopAcM_create_l(0xFF, params, pos, roomNo, gabi::ea(prmAngle.get()), scale, (u32)-1, 0);
}
VERIFY(0x025D8870, fopAcM_createItem);

/* 025D8A5C */
static u32 fopAcM_createItemForBoss(u32 pos, u32 unused, u32 roomNo, u32 angle, u32 scale, u32 param_6) {
    WWHD_FUNC(0x025D8A5C, u32, pos, unused, roomNo, angle, scale, param_6);
    if (param_6 == 1) return fopAcM_createItem(pos, 8, (u32)-1, roomNo, 3, angle, 0xC, scale);
    return fopAcM_createItem(pos, 8, (u32)-1, roomNo, 3, angle, 5, scale);
}
VERIFY(0x025D8A5C, fopAcM_createItemForBoss);

/* 025D8AB0 */
static u32 fopAcM_fastCreateItem(u32 pos, u32 i_itemNo, u32 roomNo, u32 angle, u32 scale, f32 speedF, f32 speedY, f32 gravity, u32 i_itemBitNo, u32 createFunc) {
    WWHD_FUNC(0x025D8AB0, u32, pos, i_itemNo, roomNo, angle, scale, speedF, speedY, gravity, i_itemBitNo, createFunc);
    item_assert1(i_itemNo, 0x1005785C, 0xEBE, 0x1005783C);
    if (i_itemNo == 0xFF) return 0;
    u32 no = check_itemno_l(i_itemNo);
    u32 params = (no | ((i_itemBitNo << 8) & 0xFF00)) | 0x28FF0000;
    if (isHeart_l(i_itemNo & 0xFF) != 0) speedF = gabi::fadds_ppc(speedF, speedF);
    if (i_itemNo == 0x16) return fopAcM_fastCreate_l(0x168, 1, pos, roomNo, angle, scale, (u32)-1, 0, 0);
    gabi::Local<csXyz_l> prmAngle;
    if (i_itemNo == 0x1E) {
        for (int i = 0; i < 2; i++) {
            u32 src = angle != 0 ? angle : kZeroSXyz;
            u16 ax = ld16(src + 0);
            s16 ay = (s16)ld16(src + 2);
            prmAngle->x = ax;
            prmAngle->z = 0xFF;
            prmAngle->y = (u16)ay;
            s32 r = gabi::ftoi(cM_rndFX_l(8192.0f));
            prmAngle->y = (u16)(ay + (s16)r);
            u32 item = fopAcM_fastCreate_l(0xFF, params, pos, roomNo, gabi::ea(prmAngle.get()), scale, (u32)-1, createFunc, 0);
            if (item != 0) {
                f32 a = gabi::fadds_ppc(cM_rndFX_l(0.3f), 1.0f);
                stf(item + 0x370, gabi::fmuls_ppc(speedF, a));
                f32 b = gabi::fadds_ppc(cM_rndFX_l(0.2f), 1.0f);
                stf(item + 0x340, gabi::fmuls_ppc(speedY, b));
                stf(item + 0x374, gravity);
            }
        }
    }
    u32 src = angle != 0 ? angle : kZeroSXyz;
    prmAngle->y = ld16(src + 2);
    prmAngle->x = ld16(src + 0);
    prmAngle->z = 0xFF;
    u32 item = fopAcM_fastCreate_l(0xFF, params, pos, roomNo, gabi::ea(prmAngle.get()), scale, (u32)-1, createFunc, 0);
    if (item != 0) {
        stf(item + 0x370, speedF);
        stf(item + 0x340, speedY);
        stf(item + 0x374, gravity);
    }
    return item;
}
VERIFY(0x025D8AB0, fopAcM_fastCreateItem);

/* 025D8E6C: HD: the bit number is not masked to 16 bits */
static u32 fopAcM_createItemForKP2(u32 pos, u32 i_itemNo, u32 roomNo, u32 angle, u32 scale, f32 speedF, f32 speedY, f32 gravity, u32 i_itemBitNo) {
    WWHD_FUNC(0x025D8E6C, u32, pos, i_itemNo, roomNo, angle, scale, speedF, speedY, gravity, i_itemBitNo);
    item_assert1(i_itemNo, 0x10057894, 0xE62, 0x10057874);
    if (i_itemNo == 0xFF) return 0;
    u32 ac = fopAcM_fastCreate_l(0x105, i_itemNo | (i_itemBitNo << 8), pos, roomNo, angle, scale, (u32)-1, 0, 0);
    if (ac != 0) {
        stf(ac + 0x340, speedY);
        stf(ac + 0x370, speedF);
        stf(ac + 0x374, gravity);
    }
    return ac;
}
VERIFY(0x025D8E6C, fopAcM_createItemForKP2);

/* 025D8F90 */
static u32 fopAcM_createItemForSimpleDemo(u32 pos, u32 i_itemNo, u32 roomNo, u32 angle, u32 scale, f32 speedF, f32 speedY) {
    WWHD_FUNC(0x025D8F90, u32, pos, i_itemNo, roomNo, angle, scale, speedF, speedY);
    u32 item = fopAcM_fastCreateItem(pos, i_itemNo, roomNo, angle, scale, speedF, speedY, -7.0f, (u32)-1, 0);
    if (item != 0) st8(item + 0x785, 5); /* setStatus(STATUS_INIT_NORMAL) */
    return item;
}
VERIFY(0x025D8F90, fopAcM_createItemForSimpleDemo);

/* 025D8FD0 */
static BOOL stealItem_CB(u32 actor) {
    WWHD_FUNC(0x025D8FD0, BOOL, actor);
    if (actor != 0) {
        st8(actor + 0x783, ld8(actor + 0x783) | 0x40); /* FLAG_HOOK */
        stf(actor + 0x330, 1.0f);
        stf(actor + 0x334, 1.0f);
        stf(actor + 0x338, 1.0f);
    }
    return TRUE;
}
VERIFY(0x025D8FD0, stealItem_CB);

} // namespace f_op_actor_mng_3_cpp
