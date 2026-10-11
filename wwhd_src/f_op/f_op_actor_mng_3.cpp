/** Actor manager: item creation and drop tables.
 * WWHD source, verified against the native unit.
 * HD: Hero Mode and the low-life drop action retain the native checks. */
#include "f_op/f_op_actor_mng.h"




// HD: save+0x12C0 accessor
// HD: probably the Hero Mode check

static inline u32 divw_ppc(u32 a, u32 b) {
    if (b == 0 || (a == 0x80000000u && b == 0xFFFFFFFFu)) return ((s32)a < 0) ? 0xFFFFFFFFu : 0;
    return (u32)((s32)a / (s32)b);
}

/* the GameCube item-number assert: 0 <= itemNo < max && -1 <= bitNo <= 79 || bitNo == 127 */
static inline void item_assert(u32 itemNo, u32 max, u32 bitNo, u32 file, s32 line, u32 msg) {
    if ((itemNo >= max || bitNo + 1 >= 0x51) && bitNo != 0x7F) actMngGuest_JUT_ASSERT(file, line, msg);
}
static inline void item_assert1(u32 itemNo, u32 file, s32 line, u32 msg) {
    if (itemNo >= 0x100) actMngGuest_JUT_ASSERT(file, line, msg);
}

/* 025D7D40 */
static u32 fopAcM_createDemoItem(u32 pos, u32 i_itemNo, u32 i_itemBitNo, u32 i_angle, u32 i_roomNo,
                                 u32 i_scale, u32 i_argFlag) {
    item_assert(i_itemNo, 0x100, i_itemBitNo, gabi::guest_addr("f_op_actor_mng.cpp"), 0xD39,
                gabi::guest_addr("0 <= i_itemNo && i_itemNo < 256 && (-1 <= i_itemBitNo && i_itemBitNo <= "
                                 "79) || i_itemBitNo == 127"));
    if (i_itemNo == dItemNo_NONE_e) return (u32)-1;
    u32 params = (i_itemNo & 0xFF) | ((i_itemBitNo << 8) & 0x7F00) | (i_argFlag << 16);
    return actMngGuest_fopAcM_create(fpcNm_Demo_Item_e, params, pos, i_roomNo, i_angle, i_scale, (u32)-1, 0);
}

/* 025D7DEC */
static u32 fopAcM_createItemForPresentDemo(u32 pos, u32 i_itemNo, u32 argFlag, u32 roomNo, u32 param_5,
                                           u32 angle, u32 scale) {
    item_assert1(i_itemNo, gabi::guest_addr("f_op_actor_mng.cpp"), 0xB8B,
                 gabi::guest_addr("0 <= i_itemNo && i_itemNo < 256"));
    actorMngView<dComIfG_inf_c>(dComIfGp_ea())->play.mEvtCtrl.mGetItemNo =
        (u8)i_itemNo; /* dComIfGp_event_setGtItm */
    if (i_itemNo == dItemNo_NONE_e) return (u32)-1;
    return fopAcM_createDemoItem(pos, i_itemNo, roomNo, angle, param_5, scale,
                                 argFlag); /* as on GameCube: roomNo in the bit slot */
}

/* 025D7E88 */
static u32 fopAcM_createItemForTrBoxDemo(u32 pos, u32 i_itemNo, u32 roomNo, u32 param_5, u32 angle,
                                         u32 scale) {
    item_assert1(i_itemNo, gabi::guest_addr("f_op_actor_mng.cpp"), 0xBB8,
                 gabi::guest_addr("0 <= i_itemNo && i_itemNo < 256"));
    actorMngView<dComIfG_inf_c>(dComIfGp_ea())->play.mEvtCtrl.mGetItemNo = (u8)i_itemNo;
    if (i_itemNo == dItemNo_NONE_e) return (u32)-1;
    return fopAcM_createDemoItem(pos, i_itemNo, roomNo, angle, param_5, scale, 0);
}

static inline u32 item_params(u32 itemNo, u32 bitNo, u32 type, u32 action) {
    u32 no = actMngGuest_check_itemno(itemNo);
    return (no | ((bitNo << 8) & 0xFF00) | 0xFF0000 | ((type << 24) & 0x03000000)) | (action << 26);
}

/* 025D7F20 */
static u32 fopAcM_fastCreateItem2(u32 pos, u32 i_itemNo, u32 i_itemBitNo, u32 roomNo, u32 type, u32 angle,
                                  u32 action, u32 scale) {
    item_assert(i_itemNo, 0x100, i_itemBitNo, gabi::guest_addr("f_op_actor_mng.cpp"), 0xDEF,
                gabi::guest_addr("0 <= i_itemNo && i_itemNo < 256 && (-1 <= i_itemBitNo && i_itemBitNo <= "
                                 "79) || i_itemBitNo == 127"));
    if (i_itemNo == dItemNo_NONE_e) return 0;
    gabi::Local<csXyz> prmAngle;
    if (angle != 0) {
        prmAngle->x = (u16)(actorMngView<csXyz>(angle)->x);
        prmAngle->y = (u16)(actorMngView<csXyz>(angle)->y);
    } else {
        actorMngAngleWords(*prmAngle.get()) =
            *actorMngView<be<u32>>(gabi::ea(&zeroAngle)); /* csXyz::Zero x, y */
    }
    prmAngle->z = 0xFF;
    u32 params = item_params(i_itemNo, i_itemBitNo, type, action);
    if (i_itemNo == dItemNo_RECOVER_FAIRY_e) /* RECOVER_FAIRY */
        return actMngGuest_fopAcM_fastCreate(fpcNm_NPC_FA1_e, 1, pos, roomNo, angle, scale, (u32)-1, 0, 0);
    if (i_itemNo == dItemNo_TRIPLE_HEART_e) /* TRIPLE_HEART: two extra hearts */
        for (int i = 0; i < 2; i++)
            actMngGuest_fopAcM_fastCreate(fpcNm_ITEM_e, params, pos, roomNo, gabi::ea(prmAngle.get()), scale,
                                          (u32)-1, 0, 0);
    return actMngGuest_fopAcM_fastCreate(fpcNm_ITEM_e, params, pos, roomNo, gabi::ea(prmAngle.get()), scale,
                                         (u32)-1, 0, 0);
}

/* the life percentage (dComIfGs_getLife() * 100 / (dComIfGs_getMaxLife() & 0xFC)) */
static inline u8 life_percent() {
    u32 save = (u32)saveSingleton;
    u32 life = (u16)(actorMngView<ActorManagerSave>(save)->mSavedata.mPlayer.mPlayerStatusA.mLife);
    u32 max = (u16)(actorMngView<ActorManagerSave>(save)->mSavedata.mPlayer.mPlayerStatusA.mMaxLife) & 0xFC;
    return (u8)divw_ppc(life * 100, max);
}

// HD: with action 0xD a low life (<= 40%) switches any table to the heart tables
/* 025D8120 */
static u32 fopAcM_createItemFromTableRegister(u32 p_pos, u32 i_itemNo, u32 i_itemBitNo, u32 roomNo, u32 type,
                                              u32 p_angle, u32 action, u32 p_scale) {
    item_assert(i_itemNo, 0x40, i_itemBitNo, gabi::guest_addr("f_op_actor_mng.cpp"), 0xBEF,
                gabi::guest_addr("0 <= i_itemNo && i_itemNo < 64 && (-1 <= i_itemBitNo && i_itemBitNo <= 79) "
                                 "|| i_itemBitNo == 127"));
    /* static cXyz fairy_offset_tbl[3] (function-local static, initialised on first use) */
    u32 tbl = gabi::ea(&fairy_offset_tbl);
    if ((u32)fairy_offset_initialized == 0) {
        fairy_offset_tbl.positions[0].y = 0.0f;
        fairy_offset_tbl.positions[2].x = 0.0f;
        fairy_offset_tbl.positions[1].z = 0.0f;
        fairy_offset_tbl.positions[1].x = -40.0f;
        fairy_offset_tbl.positions[2].z = 40.0f;
        fairy_offset_tbl.positions[0].x = 40.0f;
        fairy_offset_initialized = 1;
        fairy_offset_tbl.positions[0].z = 0.0f;
        fairy_offset_tbl.positions[2].y = 0.0f;
        fairy_offset_tbl.positions[1].y = 0.0f;
    }
    gabi::Local<csXyz> angle;
    gabi::Local<cXyz> pos;
    pos->x = 0.0f;
    pos->y = 0.0f;
    pos->z = 0.0f;
    actMngGuest_csXyz_ct(gabi::ea(angle.get()), 0, 0, 0);
    u32 itemNo = i_itemNo;
    u32 tableIdx = i_itemNo - 0x20;
    if (tableIdx < 0x1F) {
        u32 itemTableList = (u32)(actorMngView<dComIfG_inf_c>(dComIfGp_ea())->play.mpItemTable);
        u32 t = tableIdx;
        if (action == 0xD) {
            u8 pct = life_percent();
            if ((u8)(pct - 1) < 0x14)
                t = 4;
            else if ((u8)(pct - 0x15) < 0x14)
                t = 3;
        }
        if (t >= 1 && t <= 4) {
            u8 pct = life_percent();
            u32 sel;
            if ((u8)(pct - 1) < 0x14)
                sel = 4;
            else if ((u8)(pct - 0x15) < 0x14)
                sel = 3;
            else if ((u8)(pct - 0x29) < 0x28)
                sel = 2;
            else if ((u8)(pct - 0x51) < 0x14)
                sel = 1;
            else
                sel = t;
            s32 idx = gabi::ftoi(cM_rndF(15.9999f));
            itemNo = (u8)actorMngView<ActorManagerItemTable>(itemTableList)->items[sel][idx];
        } else if (t >= 0xB && t <= 0x14) {
            if (itemTableList == 0) return (u32)-1;
            u32 p = itemTableList + (t << 4) + 0x10;
            u32 no = (u8)*actorMngView<be<u8>>(p);
            u32 lastItemPID = (u32)-1;
            u32 off = 0;
            for (s32 i = 0; no != dItemNo_NONE_e && i < 0x10; i++) {
                if (p_pos != 0) {
                    actorMngBits(pos->x) = (u32)(actorMngBits(actorMngView<cXyz>(p_pos)->x));
                    actorMngBits(pos->y) = (u32)(actorMngBits(actorMngView<cXyz>(p_pos)->y));
                    actorMngBits(pos->z) = (u32)(actorMngBits(actorMngView<cXyz>(p_pos)->z));
                }
                if (p_angle != 0) {
                    angle->x = (u16)(actorMngView<csXyz>(p_angle)->x);
                    angle->y = (u16)(actorMngView<csXyz>(p_angle)->y);
                    angle->z = (u16)(actorMngView<csXyz>(p_angle)->z);
                }
                if (t == 0x16) { /* never true (GameCube bug: meant itemNo == RECOVER_FAIRY) */
                    actMngGuest_PSVECAdd(gabi::ea(pos.get()), tbl + off, gabi::ea(pos.get()));
                    angle->y = (u16)gabi::ftoi(cM_rndF(32766.0f));
                }
                u32 n = actMngGuest_getItemNoByLife(no & 0xFF);
                u32 item = fopAcM_fastCreateItem2(gabi::ea(pos.get()), n, i_itemBitNo, roomNo, type,
                                                  gabi::ea(angle.get()), 8, 0);
                if (item == 0) return (u32)-1;
                lastItemPID = (u32)actorMngActor(item)->mBsPcId;
                if (lastItemPID == (u32)-1) return lastItemPID;
                p++;
                no = (u8)*actorMngView<be<u8>>(p);
                off += 0xC;
            }
            return lastItemPID;
        } else {
            s32 idx = gabi::ftoi(cM_rndF(15.9999f));
            itemNo = (u8)actorMngView<ActorManagerItemTable>(itemTableList)->items[t][idx];
        }
    }
    if (itemNo == 0x3F || itemNo == dItemNo_NONE_e) return (u32)-1;
    u32 n = actMngGuest_getItemNoByLife(itemNo & 0xFF);
    u32 item = fopAcM_fastCreateItem2(p_pos, n, i_itemBitNo, roomNo, type, p_angle, action, p_scale);
    if (item == 0) return (u32)-1;
    return (u32)actorMngActor(item)->mBsPcId;
}

// HD: in Hero Mode (probably) a heart becomes item 1
/* 025D85E4 */
static u32 fopAcM_createRaceItem(u32 pos, u32 i_itemNo, u32 i_itemBitNo, u32 angle, u32 roomNo, u32 scale,
                                 u32 param_7) {
    item_assert(i_itemNo, 0x100, i_itemBitNo, gabi::guest_addr("f_op_actor_mng.cpp"), 0xCFB,
                gabi::guest_addr("0 <= i_itemNo && i_itemNo < 256 && (-1 <= i_itemBitNo && i_itemBitNo <= "
                                 "79) || i_itemBitNo == 127"));
    if (i_itemNo == dItemNo_NONE_e) return (u32)-1;
    u32 itemNo = actMngGuest_check_itemno(i_itemNo);
    u32 x = actMngGuest_save_sub((u32)saveSingleton + 0x12C0);
    if (actMngGuest_heroMode(x) != 0 && actMngGuest_isHeart(itemNo & 0xFF) != 0) itemNo = 1;
    u32 params = (itemNo & 0xFF) | ((i_itemBitNo << 8) & 0x7F00) | ((param_7 << 15) & 0x78000);
    return actMngGuest_fopAcM_create(fpcNm_RACEITEM_e, params, pos, roomNo, angle, scale, (u32)-1, 0);
}

/* 025D86E0 */
static u32 fopAcM_createRaceItemFromTable(u32 pos, u32 i_itemNo, u32 i_itemBitNo, u32 i_roomNo, u32 angle,
                                          u32 scale, u32 param_7) {
    item_assert(i_itemNo, 0x40, i_itemBitNo, gabi::guest_addr("f_op_actor_mng.cpp"), 0xC94,
                gabi::guest_addr("0 <= i_itemNo && i_itemNo < 64 && (-1 <= i_itemBitNo && i_itemBitNo <= 79) "
                                 "|| i_itemBitNo == 127"));
    u32 itemNo = i_itemNo;
    if (i_itemNo - 0x20 < 0x1F) {
        u32 t =
            (u32)(actorMngView<dComIfG_inf_c>(dComIfGp_ea())->play.mpItemTable) + ((i_itemNo - 0x20) << 4);
        s32 idx = gabi::ftoi(cM_rndF(15.9999f));
        itemNo = (u8)actorMngView<be<u8>>(t + 0x10)[idx];
    }
    if (itemNo == 0x3F || itemNo == dItemNo_NONE_e) return (u32)-1;
    u32 n = actMngGuest_getItemNoByLife(itemNo & 0xFF);
    return fopAcM_createRaceItem(pos, n, i_itemBitNo, angle, i_roomNo, scale, param_7);
}

/* 025D87E4 */
static u32 fopAcM_createShopItem(u32 pos, u32 i_itemNo, u32 angle, u32 roomNo, u32 scale, u32 createFunc) {
    item_assert1(i_itemNo, gabi::guest_addr("f_op_actor_mng.cpp"), 0xCCD,
                 gabi::guest_addr("0 <= i_itemNo && i_itemNo < 256"));
    if (i_itemNo == dItemNo_NONE_e) return (u32)-1;
    return actMngGuest_fopAcM_create(fpcNm_ShopItem_e, i_itemNo, pos, roomNo, angle, scale, (u32)-1,
                                     createFunc);
}

/* 025D8870 */
static u32 fopAcM_createItem(u32 pos, u32 i_itemNo, u32 i_itemBitNo, u32 roomNo, u32 type, u32 angle,
                             u32 action, u32 scale) {
    item_assert(i_itemNo, 0x100, i_itemBitNo, gabi::guest_addr("f_op_actor_mng.cpp"), 0xD9F,
                gabi::guest_addr("0 <= i_itemNo && i_itemNo < 256 && (-1 <= i_itemBitNo && i_itemBitNo <= "
                                 "79) || i_itemBitNo == 127"));
    if (i_itemNo == dItemNo_NONE_e) return (u32)-1;
    gabi::Local<csXyz> prmAngle;
    if (angle != 0) {
        prmAngle->x = (u16)(actorMngView<csXyz>(angle)->x);
        prmAngle->y = (u16)(actorMngView<csXyz>(angle)->y);
    } else {
        actorMngAngleWords(*prmAngle.get()) = *actorMngView<be<u32>>(gabi::ea(&zeroAngle));
    }
    prmAngle->z = 0xFF;
    u32 params = item_params(i_itemNo, i_itemBitNo, type, action);
    if (i_itemNo == dItemNo_RECOVER_FAIRY_e)
        return actMngGuest_fopAcM_create(fpcNm_NPC_FA1_e, 1, pos, roomNo, angle, scale, (u32)-1, 0);
    if (i_itemNo == dItemNo_TRIPLE_HEART_e)
        for (int i = 0; i < 2; i++)
            actMngGuest_fopAcM_create(fpcNm_ITEM_e, params, pos, roomNo, gabi::ea(prmAngle.get()), scale,
                                      (u32)-1, 0);
    return actMngGuest_fopAcM_create(fpcNm_ITEM_e, params, pos, roomNo, gabi::ea(prmAngle.get()), scale,
                                     (u32)-1, 0);
}

/* 025D8A5C */
static u32 fopAcM_createItemForBoss(u32 pos, u32 unused, u32 roomNo, u32 angle, u32 scale, u32 param_6) {
    if (param_6 == 1) return fopAcM_createItem(pos, 8, (u32)-1, roomNo, 3, angle, 0xC, scale);
    return fopAcM_createItem(pos, 8, (u32)-1, roomNo, 3, angle, 5, scale);
}

/* 025D8AB0 */
static u32 fopAcM_fastCreateItem(u32 pos, u32 i_itemNo, u32 roomNo, u32 angle, u32 scale, f32 speedF,
                                 f32 speedY, f32 gravity, u32 i_itemBitNo, u32 createFunc) {
    item_assert1(i_itemNo, gabi::guest_addr("f_op_actor_mng.cpp"), 0xEBE,
                 gabi::guest_addr("0 <= i_itemNo && i_itemNo < 256"));
    if (i_itemNo == dItemNo_NONE_e) return 0;
    u32 no = actMngGuest_check_itemno(i_itemNo);
    u32 params = (no | ((i_itemBitNo << 8) & 0xFF00)) | 0x28FF0000;
    if (actMngGuest_isHeart(i_itemNo & 0xFF) != 0) speedF = gabi::fadds_ppc(speedF, speedF);
    if (i_itemNo == dItemNo_RECOVER_FAIRY_e)
        return actMngGuest_fopAcM_fastCreate(fpcNm_NPC_FA1_e, 1, pos, roomNo, angle, scale, (u32)-1, 0, 0);
    gabi::Local<csXyz> prmAngle;
    if (i_itemNo == dItemNo_TRIPLE_HEART_e) {
        for (int i = 0; i < 2; i++) {
            u32 src = angle != 0 ? angle : gabi::ea(&zeroAngle);
            u16 ax = (u16)(actorMngView<csXyz>(src)->x);
            s16 ay = (s16)(u16)(actorMngView<csXyz>(src)->y);
            prmAngle->x = ax;
            prmAngle->z = 0xFF;
            prmAngle->y = (u16)ay;
            s32 r = gabi::ftoi(cM_rndFX(8192.0f));
            prmAngle->y = (u16)(ay + (s16)r);
            u32 item = actMngGuest_fopAcM_fastCreate(fpcNm_ITEM_e, params, pos, roomNo,
                                                     gabi::ea(prmAngle.get()), scale, (u32)-1, createFunc, 0);
            if (item != 0) {
                f32 a = gabi::fadds_ppc(cM_rndFX(0.3f), 1.0f);
                actorMngActor(item)->speedF = gabi::fmuls_ppc(speedF, a);
                f32 b = gabi::fadds_ppc(cM_rndFX(0.2f), 1.0f);
                actorMngActor(item)->speed.y = gabi::fmuls_ppc(speedY, b);
                actorMngActor(item)->gravity = gravity;
            }
        }
    }
    u32 src = angle != 0 ? angle : gabi::ea(&zeroAngle);
    prmAngle->y = (u16)(actorMngView<csXyz>(src)->y);
    prmAngle->x = (u16)(actorMngView<csXyz>(src)->x);
    prmAngle->z = 0xFF;
    u32 item = actMngGuest_fopAcM_fastCreate(fpcNm_ITEM_e, params, pos, roomNo, gabi::ea(prmAngle.get()),
                                             scale, (u32)-1, createFunc, 0);
    if (item != 0) {
        actorMngActor(item)->speedF = speedF;
        actorMngActor(item)->speed.y = speedY;
        actorMngActor(item)->gravity = gravity;
    }
    return item;
}

// HD: the bit number is not masked to 16 bits
/* 025D8E6C */
static u32 fopAcM_createItemForKP2(u32 pos, u32 i_itemNo, u32 roomNo, u32 angle, u32 scale, f32 speedF,
                                   f32 speedY, f32 gravity, u32 i_itemBitNo) {
    item_assert1(i_itemNo, gabi::guest_addr("f_op_actor_mng.cpp"), 0xE62,
                 gabi::guest_addr("0 <= i_itemNo && i_itemNo < 256"));
    if (i_itemNo == dItemNo_NONE_e) return 0;
    u32 ac = actMngGuest_fopAcM_fastCreate(fpcNm_SPC_ITEM01_e, i_itemNo | (i_itemBitNo << 8), pos, roomNo,
                                           angle, scale, (u32)-1, 0, 0);
    if (ac != 0) {
        actorMngActor(ac)->speed.y = speedY;
        actorMngActor(ac)->speedF = speedF;
        actorMngActor(ac)->gravity = gravity;
    }
    return ac;
}

/* 025D8F90 */
static u32 fopAcM_createItemForSimpleDemo(u32 pos, u32 i_itemNo, u32 roomNo, u32 angle, u32 scale, f32 speedF,
                                          f32 speedY) {
    u32 item = fopAcM_fastCreateItem(pos, i_itemNo, roomNo, angle, scale, speedF, speedY, -7.0f, (u32)-1, 0);
    if (item != 0) actorMngView<ActorManagerItem>(item)->mStatus = 5; /* setStatus(STATUS_INIT_NORMAL) */
    return item;
}

/* 025D8FD0 */
static BOOL stealItem_CB(u32 actor) {
    if (actor != 0) {
        actorMngView<ActorManagerItem>(actor)->mFlag =
            (u8)actorMngView<ActorManagerItem>(actor)->mFlag | 0x40; /* FLAG_HOOK */
        actorMngActor(actor)->scale.x = 1.0f;
        actorMngActor(actor)->scale.y = 1.0f;
        actorMngActor(actor)->scale.z = 1.0f;
    }
    return TRUE;
}

/* WWHD glue
str 100574B4 in 025D7D40
str 100574C8 in 025D7D40
str 1005752C in 025D7DEC
str 1005754C in 025D7DEC
str 10057560 in 025D7E88
str 10057580 in 025D7E88
str 10057594 in 025D7F20
str 100575A8 in 025D7F20
str 10057618 in 025D8120
str 1005762C in 025D8120
str 10057690 in 025D85E4
str 100576A4 in 025D85E4
str 10057708 in 025D86E0
str 1005771C in 025D86E0
str 10057780 in 025D87E4
str 100577A0 in 025D87E4
str 100577B4 in 025D8870
str 100577C8 in 025D8870
str 1005783C in 025D8AB0
str 1005785C in 025D8AB0
str 10057874 in 025D8E6C
str 10057894 in 025D8E6C
*/
