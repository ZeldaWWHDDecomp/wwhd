/** Actor manager: steal items, item balls, enemy lookup, terrain and sound checks.
 * WWHD source, verified against the native unit.
 * HD: water checks use SplGrpChk and terrain checks retain this translation unit's vtables. */
#include "f_op/f_op_actor_mng.h"




static constexpr u32 stealItem_CB = 0x025D8FD0;

/* dBgS_GndChk on the stack (0x54 bytes from its start) */

/* fopAcM_isItemForIb (inline): whether the steal/ball item bit was already collected */
static inline BOOL isItemForIb(u32 bit, u32 itemNo, u32 room) {
    u32 save = (u32)saveSingleton;
    if (itemNo == 0x4B) return actMngGuest_dSv_memBit_isSwitch(save + 0x598, bit, (s32)(s8)room);
    return actMngGuest_dSv_info_isItem(save + 0x20, bit, (s32)(s8)room);
}
static inline bool no_bit(u32 bit) {
    return (s32)bit == 0x1F || (s32)bit == -1 || (s32)bit == 0xFF;
}

/* 025D9000 */
static u32 fopAcM_createStealItem(u32 p_pos, u32 i_tblNo, u32 i_roomNo, u32 p_angle, u32 i_itemBitNo) {
    u32 itemNo = actMngGuest_getEmonoItemFromLifeBallTable(i_tblNo & 0xFFFF);
    u32 bit = i_itemBitNo;
    if (actMngGuest_isLimitedItem(itemNo)) {
        if (no_bit(bit) || isItemForIb(bit, itemNo, i_roomNo)) {
            u32 n = actMngGuest_getItemFromLifeBallTableWithoutEmono(i_tblNo & 0xFFFF);
            return actMngGuest_fopAcM_fastCreateItem(p_pos, n, i_roomNo, p_angle, 0, 0.0f, 0.0f, -6.0f,
                                                     (u32)-1, stealItem_CB);
        }
    } else if (actMngGuest_isNonSavedEmono(itemNo)) {
        if (bit != 0) itemNo = actMngGuest_getItemFromLifeBallTableWithoutEmono(i_tblNo & 0xFFFF);
        bit = (u32)-1;
    } else {
        if (itemNo == dItemNo_NONE_e)
            itemNo = actMngGuest_getItemFromLifeBallTableWithoutEmono(i_tblNo & 0xFFFF);
        bit = (u32)-1;
    }
    return actMngGuest_fopAcM_fastCreateItem(p_pos, itemNo, i_roomNo, p_angle, 0, 0.0f, 0.0f, -6.0f, bit,
                                             stealItem_CB);
}

/* strcmp(dComIfGp_getStartStageName(), lit) == 0 through two sead::SafeString temporaries */
static bool stage_is(u32 lit) {
    gabi::Local<SafeString> a, b;
    a->mStringTop = lit;
    a->__vtbl = gc_tu.safeString;
    u32 play = dComIfGp_ea();
    b->__vtbl = gc_tu.safeString;
    b->mStringTop = play + 0x5134;
    gabi::call_ptr((u32)actorMngView<ActorManagerSafeStringVtable>(a->__vtbl)->assureTermination,
                   gabi::ea(a.get()));
    gabi::call_ptr((u32)actorMngView<ActorManagerSafeStringVtable>(a->__vtbl)->assureTermination,
                   gabi::ea(a.get()));
    u32 s1 = a->mStringTop;
    gabi::call_ptr((u32)actorMngView<ActorManagerSafeStringVtable>(b->__vtbl)->assureTermination,
                   gabi::ea(b.get()));
    u32 s2 = b->mStringTop;
    if (s1 == s2) return true;
    for (u32 n = 0; n < 0x40001; n++, s1++, s2++) {
        u8 c = (u8)*actorMngView<be<u8>>(s1);
        if (c != (u8)*actorMngView<be<u8>>(s2)) return false;
        if (c == 0) return true;
    }
    return false;
}

// HD: item tables 0x1D-0x1F, 0x24, 0x2B drop from item table 0x29 (action 0xD);
// GameCube fopAcM_createItemFromEnemyTable is inlined
/* 025D9154 */
static u32 fopAcM_createIball(u32 p_pos, u32 itemTableIdx, u32 i_roomNo, u32 p_angle, u32 i_itemBitNo) {
    u32 charTbl = dComIfGp_ea() + 0x50A0;
    s32 dropChance = actMngGuest_cDT_GetInf(
        charTbl, (u32)actorMngView<ActorManagerCharacterTable>(dComIfGp_ea() + 0x50A0)->mPercent,
        itemTableIdx & 0xFFFF); /* GetPercent */
    s32 randPercent = gabi::ftoi(cM_rndF(99.999f));
    if (stage_is(gabi::guest_addr("Cave09")) || stage_is(gabi::guest_addr("Cave10")) ||
        stage_is(gabi::guest_addr("Cave11"))) /* Savage Labyrinth */
        return (u32)-1;
    if (itemTableIdx >= 0x1D && (itemTableIdx <= 0x1F || itemTableIdx == 0x24 || itemTableIdx == 0x2B))
        return actMngGuest_fopAcM_createItemFromTable(p_pos, 0x29, i_itemBitNo, i_roomNo, 0, p_angle, 0xD, 0);
    if (dropChance > randPercent) {
        u32 params = (itemTableIdx & 0xFFFF) | ((i_itemBitNo << 16) & 0xFF0000);
        actMngGuest_daIball_remove_old();
        u32 item = actMngGuest_fopAcM_fastCreate(fpcNm_Iball_e, params, p_pos, i_roomNo, 0, 0, (u32)-1, 0, 0);
        return item != 0 ? (u32)actorMngActor(item)->mBsPcId : (u32)-1;
    }
    /* fopAcM_createItemFromEnemyTable */
    s32 itemIdx = gabi::ftoi(cM_rndF(15.999f));
    gabi::Local<cXyz> scale;
    scale->x = (f32)zeroPosition.x;
    scale->y = (f32)zeroPosition.y;
    scale->z = (f32)zeroPosition.z;
    gabi::Local<ActorManagerItemChoices> items;
    for (int i = 0; i < 16; i++) {
        u32 tbl = dComIfGp_ea() + 0x50A0;
        u32 field = (u32)actorMngView<ActorManagerCharacterTable>(dComIfGp_ea() + 0x50A0)
                        ->mNItems[i]; /* GetNITEM0..15 */
        items->v[i] = actMngGuest_cDT_GetInf(tbl, field, itemTableIdx & 0xFFFF);
    }
    u32 itemNo = (u32)items->v[itemIdx] & 0xFF;
    u32 bit = i_itemBitNo;
    if (actMngGuest_isLimitedItem(itemNo)) {
        if (no_bit(bit) || isItemForIb(bit, itemNo, i_roomNo)) {
            u32 n = actMngGuest_getItemNoByLife(3); /* YELLOW_RUPEE */
            f32 sF = cM_rndFX(5.0f);
            f32 sY = gabi::fadds_ppc(cM_rndFX(10.0f), 50.0f);
            u32 item = actMngGuest_fopAcM_fastCreateItem(p_pos, n, i_roomNo, 0, gabi::ea(scale.get()), sF, sY,
                                                         -6.0f, (u32)-1, 0);
            return item != 0 ? (u32)actorMngActor(item)->mBsPcId : (u32)-1;
        }
    } else {
        if (actMngGuest_isNonSavedEmono(itemNo) && bit != 0)
            itemNo = actMngGuest_getItemFromLifeBallTableWithoutEmono(itemTableIdx & 0xFFFF) & 0xFF;
        bit = (u32)-1;
    }
    u32 n = actMngGuest_getItemNoByLife(itemNo);
    f32 sF = cM_rndFX(5.0f);
    f32 sY = gabi::fadds_ppc(cM_rndFX(10.0f), 50.0f);
    u32 item = actMngGuest_fopAcM_fastCreateItem(p_pos, n, i_roomNo, 0, gabi::ea(scale.get()), sF, sY, -6.0f,
                                                 bit, 0);
    return item != 0 ? (u32)actorMngActor(item)->mBsPcId : (u32)-1;
}

/* 025D9874 */
static u32 fopAcM_createWarpFlower(u32 p_pos, u32 p_angle, u32 i_roomNo, u32 param_4) {
    return actMngGuest_fopAcM_create(fpcNm_WARPFLOWER_e, param_4 & 0x0FFFFFFF, p_pos, i_roomNo, p_angle, 0,
                                     (u32)-1, 0);
}

/* 025D9898 */
static u32 enemySearchJugge(u32 ptr, u32 unused) {
    if (ptr != 0 && actMngGuest_fopAc_IsActor(ptr) && (u8)actorMngActor(ptr)->group == fopAc_ENEMY_e)
        return ptr;
    return 0;
}

// HD: getGrabActorID is a virtual of the player (vtable +0xBC)
/* 025D98E8 */
static u32 fopAcM_myRoomSearchEnemy(u32 roomNo) {
    if ((s32)roomNo < 0)
        actMngGuest_JUT_ASSERT(gabi::guest_addr("f_op_actor_mng.cpp"), 0x1065,
                               gabi::guest_addr("roomNo >= 0"));
    u32 roomProc = actMngGuest_fopScnM_SearchByID((u32)roomStatuses.rooms[roomNo].mProcID);
    if (roomProc == 0)
        actMngGuest_JUT_ASSERT(gabi::guest_addr("f_op_actor_mng.cpp"), 0x1068,
                               gabi::guest_addr("roomProc != (0)"));
    u32 player = (u32)(actorMngView<dComIfG_inf_c>(dComIfGp_ea())->play.mPlayerInfo[0].mpPlayer.v);
    u32 grab = gabi::call_ptr<u32>(
        (u32)actorMngView<ActorManagerPlayerVtable>(actorMngActor(player)->__vtbl)->getGrabActorID, player);
    gabi::Local<ActorManagerGuestID> id;
    id->value = grab;
    if ((s32)grab != -1) {
        u32 enemy = actMngGuest_fopAcIt_Judge(0x025E1234, gabi::ea(id.get()));
        if (enemy != 0 && (u8)actorMngActor(enemy)->group == fopAc_ENEMY_e) return enemy;
    }
    u32 layer = (u32)-1;
    if (actMngGuest_fpcBs_Is_JustOfType((u32)nodeProcessType,
                                        (u32)actorMngView<process_node_class>(roomProc)->mSubType))
        layer = (u32)actorMngView<process_node_class>(roomProc)->mLayer.mLayerID;
    return actMngGuest_fpcM_JudgeInLayer(layer, 0x025D9898, 0);
}

// HD: the parameter bytes are not masked
/* 025D99E8 */
static u32 fopAcM_createDisappearRegister(u32 i_actor, u32 p_pos, u32 i_scale, u32 i_dropType,
                                          u32 i_itemBitNo) {
    u32 params = (i_itemBitNo << 16) | (i_scale << 8) | i_dropType;
    u32 d = actMngGuest_fopAcM_fastCreate(fpcNm_DISAPPEAR_e, params, p_pos,
                                          (u32)(s32)(s8)(u8)actorMngActor(i_actor)->current.roomNo,
                                          i_actor + 0x320, 0, (u32)-1, 0, 0);
    if (d != 0) actorMngActor(d)->itemTableIdx = (u32)actorMngActor(i_actor)->itemTableIdx; /* itemTableIdx */
    return d != 0 ? (u32)actorMngActor(d)->mBsPcId : (u32)-1;
}

/* 025D9A70 */
static BOOL fopAcM_getGroundAngleRegister(u32 actor, u32 p_angle) {
    gabi::Local<dBgS_GndChk> chk; /* dBgS_GndChk */
    u32 g = gabi::ea(chk.get());
    actMngGuest_cBgS_GndChk_ct(g);
    f32 y = gabi::fadds_ppc((f32)actorMngActor(actor)->current.pos.y, 50.0f);
    f32 z = (f32)actorMngActor(actor)->current.pos.z;
    actorMngView<dBgS_GndChk>(g)->cBgS_Chk::mPolyPassChk = g + 0x40; /* poly pass check */
    actorMngView<dBgS_GndChk>(g)->mbLinkThrough = 0;
    actorMngView<dBgS_GndChk>(g)->cBgS_Chk::__vtbl = gc_tu.gndChk.v10;
    actorMngView<dBgS_GndChk>(g)->mbBombThrough = 0;
    actorMngView<dBgS_GndChk>(g)->m_pos.z = z;
    actorMngView<dBgS_GndChk>(g)->m_pos.y = y;
    actorMngView<dBgS_GndChk>(g)->mbArrowThrough = 0;
    actorMngView<dBgS_GndChk>(g)->cBgS_Chk::mGrpPassChk = g + 0x4C; /* group pass check */
    actorMngView<dBgS_GndChk>(g)->mbRopeThrough = 0;
    actorMngView<dBgS_GndChk>(g)->mbBoomerangThrough = 0;
    actorMngView<dBgS_GndChk>(g)->dBgS_PolyPassChk::__vtbl = gc_tu.gndChk.v40;
    actorMngView<dBgS_GndChk>(g)->mbCamThrough = 0;
    actorMngView<dBgS_GndChk>(g)->cBgS_PolyInfo::__vtbl = gc_tu.gndChk.v20;
    f32 x = (f32)actorMngActor(actor)->current.pos.x;
    actorMngView<dBgS_GndChk>(g)->dBgS_GrpPassChk::__vtbl = gc_tu.gndChk.v4C;
    actorMngView<dBgS_GndChk>(g)->m_pos.x = x;
    actorMngView<dBgS_GndChk>(g)->mbObjThrough = 0;
    actorMngView<dBgS_GndChk>(g)->mGrp = 1;
    BOOL ret = TRUE;
    u32 bgs = dComIfGp_ea() + 0x12A0;
    f32 gy = actMngGuest_cBgS_GroundCross(bgs, g);
    const f32 NONE = -1000000000.0f;
    s16 angleX = 0, angleZ = 0;
    bool done = false;
    if (gy == NONE) {
        ret = FALSE;
        done = true;
    } else {
        f32 y1 = gabi::fadds_ppc(gy, 50.0f);
        f32 z1 = gabi::fadds_ppc(z, 10.0f);
        actorMngView<dBgS_GndChk>(g)->m_pos.x = x;
        actorMngView<dBgS_GndChk>(g)->m_pos.y = y1;
        actorMngView<dBgS_GndChk>(g)->m_pos.z = z1;
        f32 g2 = actMngGuest_cBgS_GroundCross(dComIfGp_ea() + 0x12A0, g);
        f32 x1;
        if (g2 != NONE) {
            angleX = (s16)-actMngGuest_cM_atan2s(gabi::fsubs_ppc(g2, gy), gabi::fsubs_ppc(z1, z));
            x1 = gabi::fadds_ppc(x, 10.0f);
        } else {
            x1 = gabi::fadds_ppc(x, 10.0f);
            ret = FALSE;
        }
        actorMngView<dBgS_GndChk>(g)->m_pos.y = y1;
        actorMngView<dBgS_GndChk>(g)->m_pos.z = z;
        actorMngView<dBgS_GndChk>(g)->m_pos.x = x1;
        f32 g3 = actMngGuest_cBgS_GroundCross(dComIfGp_ea() + 0x12A0, g);
        if (g3 == NONE) {
            ret = FALSE;
            done = true;
        } else {
            angleZ = actMngGuest_cM_atan2s(gabi::fsubs_ppc(g3, gy), gabi::fsubs_ppc(x1, x));
        }
    }
    if (!done && ret) {
        actMngGuest_cLib_addCalcAngleS(p_angle, (u32)(s32)angleX, 4, 0x200, 0x80);
        actMngGuest_cLib_addCalcAngleS(p_angle + 4, (u32)(s32)angleZ, 4, 0x200, 0x80);
    }
    /* ~dBgS_GndChk */
    actorMngView<dBgS_GndChk>(g)->cBgS_PolyInfo::__vtbl = gc_tu.gndChk.v20;
    actorMngView<dBgS_GndChk>(g)->dBgS_PolyPassChk::__vtbl = gc_tu.gndChk.v40;
    actorMngView<dBgS_GndChk>(g)->dBgS_GrpPassChk::__vtbl = actorManagerTu.groupVtable;
    actMngGuest_cBgS_Chk_dt(g, 0);
    return done ? FALSE : ret;
}

/* 025D9D0C */
static void fopAcM_setCarryNow(u32 i_this, u32 stageLayer) {
    fopAc_ac_c* actorView = actorMngActor(i_this);
    actorView->actor_status = (u32)actorView->actor_status | fopAcStts_CARRY_e; /* fopAcStts_CARRY */
    if (stageLayer != 0) actMngGuest_fopAcM_setStageLayer(i_this);
}

/* 025D9D24 */
static void fopAcM_cancelCarryNowRegister(u32 i_this) {
    fopAc_ac_c* actorView = actorMngActor(i_this);
    u32 s = (u32)actorView->actor_status;
    if (s & fopAcStts_CARRY_e) {
        s8 room = (s8)(u8)actorView->current.roomNo;
        actorView->actor_status = s & ~u32(fopAcStts_CARRY_e);
        actMngGuest_fopAcM_setRoomLayer(i_this, room);
        actorView->shape_angle.z = 0;
        if ((u8)(actorMngView<dComIfG_inf_c>(dComIfGp_ea())->play.mEvtCtrl.mMode) != 0 &&
            (u8)actorView->group != fopAc_ENEMY_e)
            actorView->actor_status = (u32)actorView->actor_status | fopAcStts_UNK800_e;
    }
}

// HD: null-safe actor id
/* 025D9DA0 */
static s32 fopAcM_otoCheckRegister(u32 actor, f32 param_2) {
    u32 sound = actMngGuest_dKy_Sound_get();
    u32 id = (u32)actorMngView<ActorManagerSound>(sound)->actor_id;
    if ((s32)id == -1) return 0;
    if (actor != 0 && id == (u32)actorMngActor(actor)->mBsPcId) return 0;
    gabi::Local<cXyz> delta;
    actMngGuest_cXyz_mi(sound, gabi::ea(delta.get()), actor + 0x314);
    f32 d = std_sqrtf(actMngGuest_PSVECSquareMag(gabi::ea(delta.get())));
    if (!(d < param_2)) return 0;
    return (s32)(u32)actorMngView<ActorManagerSound>(sound)->type;
}

/* 025D9E64 */
static u32 fopAcM_findObjectCB(u32 it, u32 i_prm) {
    if (i_prm == 0)
        actMngGuest_JUT_ASSERT(gabi::guest_addr("f_op_actor_mng.cpp"), 0x125D, gabi::guest_addr("Prm"));
    u32 inf = actMngGuest_dStage_searchName((u32)(actorMngView<fopAcM_search_prm>(i_prm)->procname));
    if (inf == 0) return 0;
    s32 procname = (s16)(u16)(actorMngView<ActorManagerNameInfo>(inf)->mProcName);
    s32 name = it != 0 ? (s16)(u16)actorMngActor(it)->mProfName : 0x7FFF;
    if (procname != name) return 0;
    if ((s8)(u8)(actorMngView<ActorManagerNameInfo>(inf)->mArgument) != (s8)(u8)actorMngActor(it)->argument)
        return 0;
    u32 mask = (u32)(actorMngView<fopAcM_search_prm>(i_prm)->prm_mask);
    if (mask != 0 && ((u32)actorMngActor(it)->mParameters & mask) !=
                         (u32)(actorMngView<fopAcM_search_prm>(i_prm)->parameter))
        return 0;
    return it;
}

/* 025D9F38 */
static u32 fopAcM_searchFromName(u32 pProcName, u32 paramMask, u32 parameter) {
    gabi::Local<fopAcM_search_prm> prm;
    prm->procname = pProcName;
    prm->prm_mask = paramMask;
    prm->parameter = parameter;
    return actMngGuest_fopAcIt_Judge(0x025D9E64, gabi::ea(prm.get()));
}

// HD: dBgS::SplGrpChk is the water check
/* 025D9F70 */
static BOOL fopAcM_getWaterY(u32 pPos, u32 pDstWaterY) {
    const u32 wc = gabi::ea(&water_check); /* static dBgS_WtrChk water_check */
    if ((u32)water_check_initialized == 0) {
        water_check_initialized = 1;
        actMngGuest_dBgS_WtrChk_ct(wc);
        actMngGuest___register_global_object(actorManagerTu.destructorRecords[0]);
    }
    *actorMngView<be<f32>>(pDstWaterY) = -1000000000.0f;
    f32 y = gabi::fsubs_ppc((f32)(actorMngView<cXyz>(pPos)->y), 500.0f);
    f32 x = (f32)(actorMngView<cXyz>(pPos)->x);
    f32 z = (f32)(actorMngView<cXyz>(pPos)->z);
    actorMngView<ActorManagerWaterCheck>(wc)->m_pos.x = x;
    actorMngView<ActorManagerWaterCheck>(wc)->m_pos.y = y;
    actorMngView<ActorManagerWaterCheck>(wc)->m_pos.z = z;
    actorMngView<ActorManagerWaterCheck>(wc)->mTop = gabi::fadds_ppc(y, 1000.0f);
    BOOL ret = FALSE;
    if (actMngGuest_dBgS_SplGrpChk(dComIfGp_ea() + 0x12A0, wc)) {
        *actorMngView<be<f32>>(pDstWaterY) = (f32)actorMngView<ActorManagerWaterCheck>(wc)->m_height;
        ret = TRUE;
    }
    if (daSea_ChkArea((f32)(actorMngView<cXyz>(pPos)->x), (f32)(actorMngView<cXyz>(pPos)->z))) {
        f32 wave = daSea_calcWave((f32)(actorMngView<cXyz>(pPos)->x), (f32)(actorMngView<cXyz>(pPos)->z));
        if (wave > (f32)*actorMngView<be<f32>>(pDstWaterY)) *actorMngView<be<f32>>(pDstWaterY) = wave;
        ret = TRUE;
    }
    return ret;
}

/* 025DA088 */
static void fopAcM_setGbaName(u32 i_this, u32 itemNo, u32 gbaName0, u32 gbaName1) {
    bool got;
    if (actMngGuest_dComIfGs_checkGetItem(itemNo))
        got = true;
    else if (itemNo == 0x27)
        got = actMngGuest_dComIfGs_checkGetItem(0x35) ||
              actMngGuest_dComIfGs_checkGetItem(0x36); /* BOW: magic/light arrows */
    else if (itemNo == 0x35)
        got = actMngGuest_dComIfGs_checkGetItem(0x36) != 0;
    else
        got = false;
    actorMngActor(i_this)->gbaName = (u8)(got ? gbaName1 : gbaName0);
}

/* 025DA124 */
static void fpoAcM_absolutePos(u32 i_this, u32 relPos, u32 absPos) {
    fopAc_ac_c* actorView = actorMngActor(i_this);
    f32 px = (f32)actorView->current.pos.x;
    actorMngView<cXyz>(absPos)->x = px;
    f32 py = (f32)actorView->current.pos.y;
    actorMngView<cXyz>(absPos)->y = py;
    f32 pz = (f32)actorView->current.pos.z;
    actorMngView<cXyz>(absPos)->z = pz;
    u32 t = gabi::ea(&sincosTable) + ((u32)((u16)actorView->shape_angle.y >> 3) << 3);
    f32 rx = (f32)(actorMngView<cXyz>(relPos)->x);
    f32 c = (f32)(actorMngView<ActorManagerSinCos>(t)->cos);
    f32 rz = (f32)(actorMngView<cXyz>(relPos)->z);
    f32 s = (f32)(actorMngView<ActorManagerSinCos>(t)->sin);
    actorMngView<cXyz>(absPos)->x = gabi::fadds_ppc(px, gabi::fmadds(s, rz, gabi::fmuls_ppc(c, rx)));
    actorMngView<cXyz>(absPos)->y = gabi::fadds_ppc(py, (f32)(actorMngView<cXyz>(relPos)->y));
    t = gabi::ea(&sincosTable) + ((u32)((u16)actorView->shape_angle.y >> 3) << 3);
    s = (f32)(actorMngView<ActorManagerSinCos>(t)->sin);
    rx = (f32)(actorMngView<cXyz>(relPos)->x);
    rz = (f32)(actorMngView<cXyz>(relPos)->z);
    c = (f32)(actorMngView<ActorManagerSinCos>(t)->cos);
    actorMngView<cXyz>(absPos)->z = gabi::fadds_ppc(pz, gabi::fmsubs(c, rz, gabi::fmuls_ppc(s, rx)));
}

// HD: compiler-generated and template code of this translation unit
/* 025DA1B4 */
static void __sinit_f_op_actor_mng_cpp() {
    for (int i = 0; i < 4; i++)
        headerStatics.mZeros[i] = 0;
    actMngGuest___register_global_object(actorManagerTu.destructorRecords[1]);
    headerStatics.mMinPi = -3.1415927f;
    headerStatics.mMaxPi = 3.1415927f;
    actorMngRandConstructor(gabi::ea(&headerStatics.mHeaderObjects[1]));
    actMngGuest___register_global_object(actorManagerTu.destructorRecords[2]);
    actorMngListConstructor(gabi::ea(&headerStatics.mHeaderObjects[2]));
    actMngGuest___register_global_object(actorManagerTu.destructorRecords[3]);
    actMngGuest___register_global_object(actorManagerTu.destructorRecords[4]);
}

/* a per-TU inline deleting destructor without members */
/* 025DA254 */
static void actorManagerRandDestructor(u32 self, u32 flags) {
    if (self == 0) return;
    if (!(flags & 1)) return;
    actMngGuest_operator_delete(self);
}

/* dBgS_WtrChk::~dBgS_WtrChk (this unit's copy) */
/* 025DA268 */
static void dBgS_WtrChk_dt(u32 self, u32 flags) {
    if (self == 0) return;
    actorMngView<ActorManagerWaterCheck>(self)->mCheck.__vtbl = actorManagerTu.waterVtable;
    actorMngView<ActorManagerWaterCheck>(self)->mPolyPassVtable = actorManagerTu.waterPolyVtable;
    actorMngView<ActorManagerWaterCheck>(self)->mGrpPassVtable = actorManagerTu.groupVtable;
    actorMngSplGroupDestructor(self + 0x10, 0);
    if (flags & 1) actMngGuest_operator_delete(self);
}

/* a per-TU inline deleting destructor without members */
/* 025DA2E0 */
static void actorManagerListDestructor(u32 self, u32 flags) {
    if (self == 0) return;
    if (!(flags & 1)) return;
    actMngGuest_operator_delete(self);
}

/* an empty per-TU inline (called on SafeString temporaries by fopAcM_entrySolidHeap) */
/* 025DA2F4 */
static void actorManagerAssureTermination(u32 self) {
    (void)self;
}

/* WWHD glue
str 100578BC in 025D9154
str 100578C4 in 025D9154
str 100578CC in 025D9154
str 100578D4 in 025D98E8
str 100578E4 in 025D98E8
str 100578F0 in 025D98E8
str 1005790C in 025D9E64
str 10057910 in 025D9E64
*/
