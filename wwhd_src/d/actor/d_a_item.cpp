/**
 * d_a_item.cpp (WWHD)
 * Item - Field Item
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_item.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_item.h"

enum {
    dItemNo_HEART_e = 0x00,
    dItemNo_HEART_CONTAINER_e = 0x08,
    dItemNo_BOMB_5_e = 0x0B,
    dItemNo_BOMB_30_e = 0x0E,
    dItemNo_ARROW_20_e = 0x11,
    dItemNo_ARROW_30_e = 0x12,
    dItemNo_SMALL_KEY_e = 0x15,
    dItemNo_SWORD_e = 0x38,
    dItemNo_SHIELD_e = 0x3B,
    dItemNo_DROPPED_SWORD_e = 0x3D,
    dItemNo_BLUE_JELLY_e = 0x4B,
};
enum { fpcNm_BST_e = 0xF0, fpcNm_BOOMERANG_e = 0x1B0, fpcNm_HIMO2_e = 0x1BE };
enum { fopAcStts_UNK4000_e = 0x4000, fopAcStts_HOOK_CARRY_e = 0x100000 };
enum { dEvtCnd_CANGETITEM_e = 0x08 };

#define DAITEMBASE_VTBL 0x10012158
#define DAITEM_VTBL 0x10011FF8
#define m_cyl_src gabi::at<dCcD_SrcCyl>(0x10011FB4)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void fopAcM_addAngleY(fopAc_ac_c* a, s16 target, s16 step) { gabi::call(0x025D677C, a, target, step); }
/* fopAcM_SearchByName (HD inline): fopAcIt_Judge(fpcSch_JudgeForPName, &name) */
static inline fopAc_ac_c* fopAcM_SearchByName(s16 name) {
    gabi::Local<be<s16>> key;
    *key = name;
    return fopAcIt_Judge(0x025E121C, key.get());
}
/* 025D7B98 fopAcM_orderItemEvent(actor, u16): HD second argument */
static inline void fopAcM_orderItemEvent(fopAc_ac_c* a, u16 p) { gabi::call(0x025D7B98, a, p); }
/* 025D7DEC fopAcM_createItemForPresentDemo(pos, itemNo, argFlag, itemBitNo, roomNo, angle, scale) */
static inline u32 fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 flag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale) {
    return gabi::call<u32>(0x025D7DEC, pos, itemNo, flag, bitNo, roomNo, angle, scale);
}
static inline BOOL dComIfGp_evmng_endCheckOld(const char* name) {
    return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name);
}
static inline void execItemGet(u8 itemNo) { gabi::call(0x0254DA38, itemNo); }
static inline BOOL checkSpecialEffect(u8 itemNo) { return gabi::call<BOOL>(0x02551C18, itemNo); }
static inline u16 getSpecialEffect(u8 itemNo) { return gabi::call<u16>(0x02551C48, itemNo); }
static inline BOOL isArrow(u8 itemNo) { return gabi::call<BOOL>(0x02550FDC, itemNo); }
/* save data reached through the raw pointer at 0x101F84DC */
static inline u32 dComIfGs_raw() { return gabi::load<u32>(0x101F84DC); }
/* JPABaseEmitter::becomeInvalidEmitter (HD inline) */
static inline void JPABaseEmitter_becomeInvalidEmitter(u32 e) {
    u32 f = gabi::load<u32>(e + 0x254);
    gabi::store<s32>(e + 0x5C, -1);
    gabi::store<u32>(e + 0x254, f | 1);
}
/* virtual remove() of a particle callback (vtable at +0, slot +0x44) */
static inline void ecallBack_remove(void* cb) { gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(cb)) + 0x44), cb); }
static inline f32 item_getH(u8 itemNo) { return (f32)dItem_data::getH(itemNo); }

/* 0217DDDC (the matcher has no name for it; it names 0217E5A4 checkPlayerGet). HD: never while
 * mHD79E (HD action 0xF) is set */
BOOL daItem_c::checkPlayerGet() {
    WWHD_FUNC(0x0217DDDC, BOOL, this);
    if (mHD79E != 0) {
        return FALSE;
    }
    if (m_get_timer < daItem_getData()->mNoGetTime) {
        return FALSE;
    }
    if (mItemStatus == STATUS_BRING_NEZUMI) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x0217DDDC, &daItem_c::checkPlayerGet);

/* HD: mDoAud_seStart(id) without a position (025E1988) and the play-by-name sound (02030B38) */
static inline void mDoAud_seStart_id(u32 id) { gabi::call(0x025E1988, id); }
static inline void mDoAud_seStartName(const char* name) { gabi::call(0x02030B38, name); }
/* dSv_player_get_bag_item_c (save+0xB0): isBeast/onBeast/isBait/onBait */
static inline BOOL dComIfGs_isGetItemBeast(u8 i) { return gabi::call<BOOL>(0x025B7690, dComIfGs_raw() + 0xB0, i); }
static inline void dComIfGs_onGetItemBeast(u8 i) { gabi::call(0x025B7628, dComIfGs_raw() + 0xB0, i); }
static inline BOOL dComIfGs_isGetItemBait(u8 i) { return gabi::call<BOOL>(0x025B7768, dComIfGs_raw() + 0xB0, i); }
static inline void dComIfGs_onGetItemBait(u8 i) { gabi::call(0x025B7700, dComIfGs_raw() + 0xB0, i); }

/* 0217DE18. HD: items that start the get-item demo (status 7) are saved when the demo ends
 * (execMainGetDemoDirection); the others are saved here (fopAcM_onItemForIb inline); item 0x14
 * (HD) plays SE_TNCL_BOTTLE_GET */
void daItem_c::itemGetExecute() {
    WWHD_FUNC(0x0217DE18, void, this);
    if (mItemStatus == STATUS_INIT_NORMAL) {
        return;
    }
    mItemStatus = STATUS_INIT_NORMAL;

    enum { GET, DEMO, BEAST, BAIT, SAVE };
    int kind = SAVE;
    u32 se = 0;
    u8 idx = 0;
    switch (m_itemNo) {
    case 0x00: /* HEART */
    case 0x1E: /* TRIPLE_HEART */
        se = 0x821, kind = GET;
        break;
    case 0x01: /* GREEN_RUPEE */
        se = 0x826, kind = GET;
        break;
    case 0x02: /* BLUE_RUPEE */
    case 0x03: /* YELLOW_RUPEE */
        se = 0x835, kind = GET;
        break;
    case 0x04: /* RED_RUPEE */
    case 0x05: /* PURPLE_RUPEE */
    case 0x06: /* ORANGE_RUPEE */
    case 0x0F: /* SILVER_RUPEE */
        se = 0x836, kind = GET;
        break;
    case 0x07: /* HEART_PIECE */
    case 0x08: /* HEART_CONTAINER */
        mDoAud_seStart_id(0x821);
        mItemStatus = STATUS_INIT_GET_DEMO;
        clrFlag(FLAG_UNK04);
        mCyl.SetTgType(0);
        mCyl.OffCoSPrmBit(1);
        mCyl.ClrTgHit();
        gabi::call(0x0251641C, &mCyl); /* ClrCoHit */
        return;
    case 0x09: /* SMALL_MAGIC */
        se = 0x879, kind = GET;
        break;
    case 0x0A: /* LARGE_MAGIC */
        se = 0x87A, kind = GET;
        break;
    case 0x0B: case 0x0C: case 0x0D: case 0x0E: /* BOMB_* */
    case 0x10: case 0x11: case 0x12:             /* ARROW_* */
        se = 0x827, kind = GET;
        break;
    case 0x14: /* HD */
        mDoAud_seStartName(STR(0x10011E84) /* "SE_TNCL_BOTTLE_GET" */);
        kind = GET;
        break;
    case 0x1F: /* JOY_PENDANT */
        se = 0x87B, kind = BEAST, idx = 7;
        break;
    case dItemNo_SWORD_e: {
        daItem_c* item = (daItem_c*)fopAcM_SearchByName(0xFF /* fpcNm_ITEM_e */);
        if (item && item->m_itemNo == dItemNo_SHIELD_e) {
            item->itemGetExecute();
        }
        kind = DEMO;
        break;
    }
    case dItemNo_SHIELD_e: {
        daItem_c* item = (daItem_c*)fopAcM_SearchByName(0xFF /* fpcNm_ITEM_e */);
        if (item && item->m_itemNo == dItemNo_SWORD_e) {
            item->itemGetExecute();
        }
        kind = DEMO;
        break;
    }
    case dItemNo_SMALL_KEY_e:
    case 0x34: /* DEKU_LEAF */
    case dItemNo_DROPPED_SWORD_e:
        kind = DEMO;
        break;
    case 0x45: /* SKULL_NECKLACE */
        se = 0x87B, kind = BEAST, idx = 0;
        break;
    case 0x46: case 0x47: case 0x48: case 0x49: case 0x4A: case 0x4B: /* the other spoils */
        se = 0x87B, kind = BEAST, idx = m_itemNo - 0x45;
        break;
    case 0x82: /* BIRD_BAIT_5 */
        se = 0x8EC, kind = BAIT, idx = 0;
        break;
    case 0x83: /* HYOI_PEAR */
        se = 0x8EC, kind = BAIT, idx = 1;
        break;
    }

    switch (kind) {
    case GET:
        if (se != 0)
            mDoAud_seStart_id(se);
        execItemGet(m_itemNo);
        break;
    case DEMO:
        mItemStatus = STATUS_INIT_GET_DEMO;
        clrFlag(FLAG_UNK04);
        mCyl.SetTgType(0);
        mCyl.OffCoSPrmBit(1);
        mCyl.ClrTgHit();
        gabi::call(0x0251641C, &mCyl); /* ClrCoHit */
        return;
    case BEAST:
        mDoAud_seStart_id(se);
        if (!dComIfGs_isGetItemBeast(idx)) {
            mItemStatus = STATUS_INIT_GET_DEMO;
            dComIfGs_onGetItemBeast(idx);
        } else {
            execItemGet(m_itemNo);
        }
        break;
    case BAIT:
        mDoAud_seStart_id(se);
        if (!dComIfGs_isGetItemBait(idx)) {
            mItemStatus = STATUS_INIT_GET_DEMO;
            dComIfGs_onGetItemBait(idx);
        } else {
            execItemGet(m_itemNo);
        }
        break;
    }

    if (kind == SAVE || mItemStatus != STATUS_INIT_GET_DEMO) {
        /* fopAcM_onItemForIb(mItemBitNo, m_itemNo, current.roomNo) */
        u8 itemNo = m_itemNo;
        s32 roomNo = current.roomNo;
        u32 save = dComIfGs_raw();
        s32 bitNo = mItemBitNo;
        if (itemNo == dItemNo_BLUE_JELLY_e) {
            gabi::call(0x025B8CE0, save + 0x598, bitNo); /* dSv_memBit_c::onSwitch */
        } else {
            gabi::call(0x025BA384, save + 0x20, bitNo, roomNo); /* dSv_info_c::onItem */
        }
    }

    clrFlag(FLAG_UNK04);
    mCyl.SetTgType(0);
    mCyl.OffCoSPrmBit(1);
    mCyl.ClrTgHit();
    gabi::call(0x0251641C, &mCyl); /* ClrCoHit */
}
VERIFY(0x0217DE18, &daItem_c::itemGetExecute);

/* 0217E50C (unnamed by the matcher). HD: also collected by the actor 0xA5 while the player's
 * status bit 0x10000 is set */
static void itemGetCallBack(fopAc_ac_c* item_actor, dCcD_GObjInf*, fopAc_ac_c* collided_actor, dCcD_GObjInf*) {
    WWHD_FUNC(0x0217E50C, void, item_actor, (dCcD_GObjInf*)nullptr, collided_actor, (dCcD_GObjInf*)nullptr);
    daItem_c* item = (daItem_c*)item_actor;
    u32 link = gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYERPTR);
    if (item && item->checkPlayerGet() && collided_actor) {
        if (gabi::ea(collided_actor) == link) {
            item->itemGetExecute();
        }
        if (collided_actor && gabi::load<s16>(gabi::ea(collided_actor) + 0xE) == 0xA5 &&
            (gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0) & 0x10000)) {
            item->itemGetExecute();
        }
    }
}
VERIFY(0x0217E50C, itemGetCallBack);

/* 0217E5A4 (the matcher names it checkPlayerGet) */
BOOL daItem_c::_daItem_draw() {
    WWHD_FUNC(0x0217E5A4, BOOL, this);
    if (chkDraw()) {
        return gabi::call_ptr<BOOL>(vfn(daItemBase_VT_DRAWBASE), this);
    }
    return TRUE;
}
VERIFY(0x0217E5A4, &daItem_c::_daItem_draw);

/* 0217E604 */
static BOOL daItem_Draw(daItem_c* i_this) {
    WWHD_FUNC(0x0217E604, BOOL, i_this);
    return i_this->_daItem_draw();
}
VERIFY(0x0217E604, daItem_Draw);

/* 0217E608. HD: the timers also run while mHD79E is set */
BOOL daItem_c::timeCount() {
    WWHD_FUNC(0x0217E608, BOOL, this);
    m_timer = m_timer + 1;
    if (m_get_timer < 10000 /* m_timer_max */) {
        m_get_timer = m_get_timer + 1;
    }

    if ((checkPlayerGet() && !(gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0)) || mHD79E != 0) {
        if (mWaitTimer > 0) {
            mWaitTimer = mWaitTimer - 1;
        } else if (mDisappearTimer > 0) {
            mDisappearTimer = mDisappearTimer - 1;
        }
    }

    return TRUE;
}
VERIFY(0x0217E608, &daItem_c::timeCount);

/* 0217E6AC */
void daItem_c::itemDefaultRotateY() {
    WWHD_FUNC(0x0217E6AC, void, this);
    s16 rotationSpeed = 0xFFFF / daItem_getData()->mRotateYSpeed;
    fopAcM_addAngleY(this, current.angle.y + rotationSpeed, rotationSpeed);
}
VERIFY(0x0217E6AC, &daItem_c::itemDefaultRotateY);

/* 0217E6D4 */
void daItem_c::mode_proc_call() {
    WWHD_FUNC(0x0217E6D4, void, this);
    /* static ModeFunc mode_proc[] = {mode_wait, mode_wait, mode_water} (0x10011E98) */
    if (mType == 1) {
        itemDefaultRotateY();
    } else {
        ptmf_call(0x10011E98 + mMode * 8, this);
    }

    if (checkFlag(FLAG_BOOMERANG)) {
        fopAc_ac_c* boomerang = fopAcM_SearchByName(fpcNm_BOOMERANG_e);
        if (boomerang) {
            current.pos.copy(boomerang->current.pos);
        } else {
            clrFlag(FLAG_BOOMERANG);
        }
    }

    if (checkFlag(FLAG_HOOK)) {
        fopAc_ac_c* grappling_hook = fopAcM_SearchByName(fpcNm_HIMO2_e);
        if (grappling_hook) {
            current.pos.copy(grappling_hook->current.pos);
        } else {
            clrFlag(FLAG_HOOK);
        }
    }

    if (mType == 1 && ((actor_status & fopAcStts_HOOK_CARRY_e) || checkFlag(FLAG_BOOMERANG))) {
        mType = 3;
    }
}
VERIFY(0x0217E6D4, &daItem_c::mode_proc_call);

/* 0217E850 */
void daItem_c::execBringNezumi() {
    WWHD_FUNC(0x0217E850, void, this);
    if (mType != 1) {
        fopAcM_posMoveF(this, &mStts.m_cc_move);
    }
    mode_proc_call();
}
VERIFY(0x0217E850, &daItem_c::execBringNezumi);

/* 0217E898. HD: an unused dComIfGp_get() first */
BOOL daItem_c::checkGetItem() {
    WWHD_FUNC(0x0217E898, BOOL, this);
    dComIfGp_get();
    if (!checkPlayerGet()) {
        return FALSE;
    }

    if (mCyl.ChkTgHit()) {
        void* hitObj = mCyl.GetTgHitObj();
        if (hitObj) {
            if (cCcD_Obj_ChkAtType(hitObj, 0x2 /* AT_TYPE_SWORD */)) {
                itemGetExecute();
                return TRUE;
            } else if (cCcD_Obj_ChkAtType(hitObj, 0x40 /* AT_TYPE_BOOMERANG */)) {
                setFlag(FLAG_BOOMERANG);
            }
        }
    }

    return FALSE;
}
VERIFY(0x0217E898, &daItem_c::checkGetItem);

/* 0217E938. HD: the event check is skipped for actors with status 0x4000 */
BOOL daItem_c::checkItemDisappear() {
    WWHD_FUNC(0x0217E938, BOOL, this);
    BOOL disappearing = TRUE;
    if (mItemStatus == STATUS_BRING_NEZUMI) {
        disappearing = FALSE;
        show();
    }
    if (checkFlag(FLAG_UNK02)) {
        disappearing = FALSE;
    }
    if (checkFlag(FLAG_UNK10)) {
        disappearing = FALSE;
    }
    if (dItem_data::getFlag(m_itemNo) & 0x01) {
        disappearing = FALSE;
    }
    if (!(actor_status & fopAcStts_UNK4000_e)) {
        if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0) { /* dComIfGp_event_runCheck() */
            disappearing = FALSE;
        }
    }
    if (mItemStatus == STATUS_UNK4) {
        disappearing = FALSE;
    }
    if (checkFlag(FLAG_BOOMERANG) || checkFlag(FLAG_HOOK) || (actor_status & fopAcStts_HOOK_CARRY_e)) {
        disappearing = FALSE;
        show();
    }
    return disappearing;
}
VERIFY(0x0217E938, &daItem_c::checkItemDisappear);

/* 0217EA1C */
void daItem_c::execWaitMain() {
    WWHD_FUNC(0x0217EA1C, void, this);
    checkGetItem();
    if (mType != 1) {
        fopAcM_posMoveF(this, &mStts.m_cc_move);
    }
    mode_proc_call();

    if (!checkFlag(FLAG_UNK02)) {
        f32 scaleStepX = mScaleTarget.x / daItem_getData()->mScaleAnimSpeed;
        f32 scaleStepY = mScaleTarget.y / daItem_getData()->mScaleAnimSpeed;
        f32 scaleStepZ = mScaleTarget.z / daItem_getData()->mScaleAnimSpeed;
        cLib_chaseF(&scale.x, mScaleTarget.x, scaleStepX);
        cLib_chaseF(&scale.y, mScaleTarget.y, scaleStepY);
        cLib_chaseF(&scale.z, mScaleTarget.z, scaleStepZ);
    }

    if (checkItemDisappear() && mWaitTimer == 0) {
        if (mDisappearTimer == 0) {
            fopAcM_delete(this);
        }

        if (m_timer % daItem_getData()->mFlashCycleTime == 0) {
            changeDraw();
        }
    }

    if (!(dItem_data::getFlag(m_itemNo) & 2)) {
        if (mCollideSwitchNo == 0xFF || fopAcM_isSwitch(this, mCollideSwitchNo)) {
            mCyl.SetC(&current.pos);
            dComIfG_Ccsp_Set(&mCyl);
        }
    }
}
VERIFY(0x0217EA1C, &daItem_c::execWaitMain);

/* the player's head top position (HD: daPy_py_c field at +0x3D8) */
static inline u32 daPy_getPlayerActorClass() { return gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER); }

/* 0217EBAC. HD: item 0x14 stays 30 frames longer; status 0x4000 */
void daItem_c::execInitNormalDirection() {
    WWHD_FUNC(0x0217EBAC, void, this);
    u32 player = daPy_getPlayerActorClass();
    f32 hx = gabi::load<f32>(player + 0x3D8);
    f32 hy = gabi::load<f32>(player + 0x3DC) + 15.0f;
    f32 hz = gabi::load<f32>(player + 0x3E0);
    current.pos.x = hx;
    current.pos.y = hy;
    current.pos.z = hz;
    current.angle.z = 0;
    current.angle.x = 0;
    scale.x = mScaleTarget.x;
    scale.y = mScaleTarget.y;
    scale.z = mScaleTarget.z;
    mExtraZRot = 0;

    mSimpleExistTimer = daItem_getData()->mSimpleExistTime;
    if (m_itemNo == 0x14) {
        mSimpleExistTimer = daItem_getData()->mSimpleExistTime + 30;
    }
    actor_status = actor_status | fopAcStts_UNK4000_e;
    speed.x = 0.0f;
    speed.y = daItem_getData()->mGetDemoLaunchSpeed;
    speed.z = 0.0f;
    gravity = daItem_getData()->mGetDemoGravity;

    show();

    mCyl.SetTgType(0);
    mCyl.OffCoSPrmBit(1); /* OffCoSetBit */

    ecallBack_remove(mPtclSmokeCb);
    if (mpParticleEmitter) {
        JPABaseEmitter_becomeInvalidEmitter(gabi::ea(mpParticleEmitter));
        mpParticleEmitter = NULL;
    }

    mItemStatus = STATUS_MAIN_NORMAL;
}
VERIFY(0x0217EBAC, &daItem_c::execInitNormalDirection);

/* 0217ECE0 */
void daItem_c::execMainNormalDirection() {
    WWHD_FUNC(0x0217ECE0, void, this);
    u32 player = daPy_getPlayerActorClass();
    f32 hz = gabi::load<f32>(player + 0x3E0);
    f32 hy = gabi::load<f32>(player + 0x3DC) + 15.0f;
    f32 hx = gabi::load<f32>(player + 0x3D8);
    current.pos.z = hz;
    current.pos.x = hx;
    fopAcM_posMoveF(this, NULL);
    if (current.pos.y < hy) {
        current.pos.y = hy;
    }

    u32 camera = gabi::load<u32>(dComIfGp_ea() + 0x5AF8); /* dComIfGp_getCamera(0) */
    current.angle.x = gabi::load<s16>(camera + 0x234);
    current.angle.y = gabi::load<s16>(camera + 0x236);
    current.angle.z = gabi::load<s16>(camera + 0x238);

    mSimpleExistTimer = mSimpleExistTimer - 1;
    if (mSimpleExistTimer < 0) {
        fopAcM_delete(this);
    }
}
VERIFY(0x0217ECE0, &daItem_c::execMainNormalDirection);

/* 0217EDAC: HD helper (the matcher names it execWaitGetDemoDirection): order the get-item event
 * (heart containers/pieces 7..8 with 0, others with 0xFFFF) */
void daItem_c::orderItemEvent() {
    WWHD_FUNC(0x0217EDAC, void, this);
    u16 p = 0xFFFF;
    if (m_itemNo >= 7 && m_itemNo <= 8) {
        p = 0;
    }
    fopAcM_orderItemEvent(this, p);
    eventInfo_onCondition(this, dEvtCnd_CANGETITEM_e);
}
VERIFY(0x0217EDAC, &daItem_c::orderItemEvent);

/* 0217EE08. HD: fopAcM_createItemForPresentDemo */
void daItem_c::execInitGetDemoDirection() {
    WWHD_FUNC(0x0217EE08, void, this);
    u32 player = daPy_getPlayerActorClass();
    u32 link = gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYERPTR);

    hide();
    ecallBack_remove(&mPtclFollowCb);

    if (player == link) {
        orderItemEvent();
        mDemoItemBsPcId = fopAcM_createItemForPresentDemo(&current.pos, m_itemNo, 1, -1, current.roomNo, NULL, NULL);
        mItemStatus = STATUS_WAIT_GET_DEMO;
    }
}
VERIFY(0x0217EE08, &daItem_c::execInitGetDemoDirection);

/* 0217EEAC (unnamed by the matcher) */
void daItem_c::execWaitGetDemoDirection() {
    WWHD_FUNC(0x0217EEAC, void, this);
    hide();

    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 4) { /* eventInfo.checkCommandItem() */
        u32 id = mDemoItemBsPcId;
        mItemStatus = STATUS_MAIN_GET_DEMO;
        if (id != fpcM_ERROR_PROCESS_ID_e) {
            gabi::store<u32>(dComIfGp_ea() + 0x52A0, id); /* dComIfGp_event_setItemPartnerId */
        }
    } else {
        orderItemEvent();
    }
}
VERIFY(0x0217EEAC, &daItem_c::execWaitGetDemoDirection);

/* 0217EF28. HD: the item (or Blue Chu Jelly's switch) is saved and the item given here when the
 * event ends; with a parent actor 0x83, event bit 0x2E20 */
void daItem_c::execMainGetDemoDirection() {
    WWHD_FUNC(0x0217EF28, void, this);
    hide();

    if (dComIfGp_evmng_endCheckOld(STR(0x10011EB8) /* "DEFAULT_GETITEM" */)) {
        if (mDemoItemBsPcId != fpcM_ERROR_PROCESS_ID_e) {
            u32 save = dComIfGs_raw();
            if (m_itemNo == dItemNo_BLUE_JELLY_e) {
                gabi::call(0x025B8CE0, save + 0x598, (s32)mItemBitNo); /* dSv_memBit_c::onSwitch */
            } else {
                gabi::call(0x025BA384, save + 0x20, (s32)mItemBitNo, (s32)current.roomNo); /* dSv_info_c::onItem */
            }
            if (parentActorID != fpcM_ERROR_PROCESS_ID_e) {
                fopAc_ac_c* parent = fopAcM_SearchByID(parentActorID);
                if (parent && parent && fpcM_GetName(parent) == 0x83) {
                    gabi::call(0x025B8B68, dComIfGs_raw() + 0x644, 0x2E20); /* dSv_event_c::onEventBit */
                }
            }
            execItemGet(m_itemNo);
        }
        dComIfGp_event_reset();
        fopAcM_delete(this);
    }
}
VERIFY(0x0217EF28, &daItem_c::execMainGetDemoDirection);

/* 0217F030 */
void daItem_c::scaleAnimFromBossItem() {
    WWHD_FUNC(0x0217F030, void, this);
    if (m_get_timer <= 30) {
        if (m_get_timer < 30) {
            f32 s = cM_ssin(m_timer * 0x2000 - 0x4000);
            f32 x = (s + s) / (f32)m_timer + 1.0f;
            if (x < 0.0f) {
                x = 0.0f;
            }
            scale.x = x;
            scale.y = x;
            scale.z = x;
            return;
        }
        /* fopAcM_seStart(this, JA_SE_CM_BOSS_HEART_APPEAR, 0) (HD: no NULL checks here) */
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(0x583D, &eyePos, 0, reverb);
    }
    scale.x = 1.0f;
    scale.y = 1.0f;
    scale.z = 1.0f;
}
VERIFY(0x0217F030, &daItem_c::scaleAnimFromBossItem);

/* 0217F134 */
void daItem_c::execWaitMainFromBoss() {
    WWHD_FUNC(0x0217F134, void, this);
    checkGetItem();
    if (mType != 1) {
        fopAcM_posMoveF(this, &mStts.m_cc_move);
    }
    mode_proc_call();

    if (mItemStatus != STATUS_WAIT_BOSS2) {
        scaleAnimFromBossItem();
    }

    mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);
}
VERIFY(0x0217F134, &daItem_c::execWaitMainFromBoss);

/* 0217F1B4 */
f32 daItem_c::getYOffset() {
    WWHD_FUNC(0x0217F1B4, f32, this);
    switch (m_itemNo) {
    case dItemNo_SHIELD_e:
        return 23.0f;
    case dItemNo_SWORD_e:
        return 20.0f;
    case dItemNo_DROPPED_SWORD_e:
        return 10.0f;
    default:
        return 0.0f;
    }
}
VERIFY(0x0217F1B4, &daItem_c::getYOffset);

/* 0217F200 (pos and rot by value: pointers to the caller's copies). HD: mHDRotY */
void daItem_c::set_mtx_base(J3DModel* pModel, cXyz* pos, csXyz* rot) {
    WWHD_FUNC(0x0217F200, void, this, pModel, pos, rot);
    if (!pModel) {
        return;
    }

    J3DModel_setBaseScale(pModel, &scale);

    f32 x = pos->x;
    f32 yOffset = getYOffset();
    mDoMtx_stack_c::transS(x, pos->y + yOffset, pos->z);

    if (isRupee(m_itemNo)) {
        u8 height = dItem_data::getH(m_itemNo);
        mDoMtx_stack_c::transM(0.0f, (f32)height * 0.5f, 0.0f);
    }

    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), rot->x, rot->y + mHDRotY, rot->z + mExtraZRot);

    if (isRupee(m_itemNo)) {
        u8 height = dItem_data::getH(m_itemNo);
        mDoMtx_stack_c::transM(0.0f, (f32)(-(s32)height) * 0.5f, 0.0f);
    }

    J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
}
VERIFY(0x0217F200, &daItem_c::set_mtx_base);

/* 0217F3F0 */
void daItem_c::set_mtx() {
    WWHD_FUNC(0x0217F3F0, void, this);
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    s16 rx = current.angle.x;
    s16 ry = current.angle.y;
    s16 rz = current.angle.z;
    if (m_itemNo == dItemNo_HEART_CONTAINER_e) {
        ry = shape_angle.y;
    }
    {
        gabi::Local<cXyz> p;
        gabi::Local<csXyz> r;
        p->x = pos->x, p->y = pos->y, p->z = pos->z;
        r->x = rx, r->y = ry, r->z = rz;
        set_mtx_base(mpModel, p, r);
    }

    if (isArrow(m_itemNo)) {
        gabi::Local<cXyz> offset;
        offset->x = 5.0f, offset->y = 0.0f, offset->z = 10.0f;
        /* setArrowTrans(current.angle.y, offset) */
        mDoMtx_YrotS(mDoMtx_stack_c::get(), current.angle.y);
        PSMTXMultVec(mDoMtx_stack_c::get(), offset, offset);
        gabi::Local<cXyz> arrowTrans;
        arrowTrans->x = offset->x, arrowTrans->y = offset->y, arrowTrans->z = offset->z;

        PSVECAdd(pos, arrowTrans, pos);
        {
            gabi::Local<cXyz> p;
            gabi::Local<csXyz> r;
            p->x = pos->x, p->y = pos->y, p->z = pos->z;
            r->x = rx, r->y = ry, r->z = rz;
            set_mtx_base(mpModelArrow[0], p, r);
        }

        PSVECAdd(pos, arrowTrans, pos);
        {
            gabi::Local<cXyz> p;
            gabi::Local<csXyz> r;
            p->x = pos->x, p->y = pos->y, p->z = pos->z;
            r->x = rx, r->y = ry, r->z = rz;
            set_mtx_base(mpModelArrow[1], p, r);
        }
    }
}
VERIFY(0x0217F3F0, &daItem_c::set_mtx);

/* 0217F5B0 */
BOOL daItem_c::_daItem_execute() {
    WWHD_FUNC(0x0217F5B0, BOOL, this);
    if (mSpawnSwitchNo != 0xFF && !fopAcM_isSwitch(this, mSpawnSwitchNo)) {
        return TRUE;
    }
    if (mSpawnSwitchNo != 0xFF && fopAcM_isSwitch(this, mSpawnSwitchNo)) {
        show();
    }

    timeCount();

    f32 y = current.pos.y;
    eyePos.y = y;
    eyePos.z = current.pos.z;
    eyePos.x = current.pos.x;
    gabi::at<cXyz>(gabi::ea(this) + 0x390)->copy(current.pos); /* attention_info.position */
    eyePos.y = gabi::fmadds(item_getH(m_itemNo), 0.5f, y);

    switch (mItemStatus) {
    case STATUS_BRING_NEZUMI:
        execBringNezumi();
        break;
    case STATUS_UNK0:
    case STATUS_UNK1:
        mItemStatus = checkActionNow() != 0 ? STATUS_UNK1 : STATUS_UNK0;
    case STATUS_WAIT_MAIN:
        execWaitMain();
        break;
    case STATUS_INIT_NORMAL:
        execInitNormalDirection();
    case STATUS_MAIN_NORMAL:
        execMainNormalDirection();
        break;
    case STATUS_INIT_GET_DEMO:
        execInitGetDemoDirection();
        break;
    case STATUS_WAIT_GET_DEMO:
        execWaitGetDemoDirection();
        break;
    case STATUS_MAIN_GET_DEMO:
        execMainGetDemoDirection();
        break;
    case STATUS_WAIT_BOSS1:
    case STATUS_WAIT_BOSS2:
        execWaitMainFromBoss();
        break;
    }

    animPlay(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    set_mtx();

    return TRUE;
}
VERIFY(0x0217F5B0, &daItem_c::_daItem_execute);

/* 0217F88C */
static BOOL daItem_Execute(daItem_c* i_this) {
    WWHD_FUNC(0x0217F88C, BOOL, i_this);
    return i_this->_daItem_execute();
}
VERIFY(0x0217F88C, daItem_Execute);

/* 0217F890: _daItem_isdelete inlined */
static BOOL daItem_IsDelete(daItem_c* i_this) {
    WWHD_FUNC(0x0217F890, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0217F890, daItem_IsDelete);

/* 0217F898 */
BOOL daItem_c::_daItem_delete() {
    WWHD_FUNC(0x0217F898, BOOL, this);
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)mPtclRippleCb);
    ecallBack_remove(&mPtclFollowCb);
    ecallBack_remove(mPtclSmokeCb);
    if (mpParticleEmitter) {
        JPABaseEmitter_becomeInvalidEmitter(gabi::ea(mpParticleEmitter));
        mpParticleEmitter = NULL;
    }

    DeleteBase(gabi::at<const char>(gabi::load<u32>(dItem_data::field_item_res(m_itemNo))));

    return TRUE;
}
VERIFY(0x0217F898, &daItem_c::_daItem_delete);

/* 0217F938 */
static BOOL daItem_Delete(daItem_c* i_this) {
    WWHD_FUNC(0x0217F938, BOOL, i_this);
    return i_this->_daItem_delete();
}
VERIFY(0x0217F938, daItem_Delete);

/* 0218084C */
void daItem_c::setItemTimer(int timer) {
    WWHD_FUNC(0x0218084C, void, this, timer);
    if (timer == -1) {
        setFlag(FLAG_UNK10);
        return;
    }
    mWaitTimer = timer;
}
VERIFY(0x0218084C, &daItem_c::setItemTimer);

/* 021809EC */
void daItem_c::setTevStr() {
    WWHD_FUNC(0x021809EC, void, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);

    u32 tev = gabi::ea(&tevStr);
    gabi::store<s16>(tev + 0x90, 0x96); /* mColorC0.r */
    gabi::store<s16>(tev + 0x92, 0x96); /* mColorC0.g */
    gabi::store<s16>(tev + 0x94, 0x96); /* mColorC0.b */
    gabi::store<u8>(tev + 0x98, 0xFF);  /* mColorK0.r */
    gabi::store<u8>(tev + 0x99, 0xFF);  /* mColorK0.g */
    gabi::store<u8>(tev + 0x9A, 0xFF);  /* mColorK0.b */
    dScnKy_env_light_c* light = dKy_getEnvlight();
    setLightTevColorType(light, mpModel, &tevStr);

    for (int i = 0; i < 2; i++) {
        if (!mpModelArrow[i]) {
            continue;
        }
        light = dKy_getEnvlight();
        setLightTevColorType(light, mpModelArrow[i], &tevStr);
    }
}
VERIFY(0x021809EC, &daItem_c::setTevStr);

/* ---- not yet written (guest calls only) ---- */

/* float -> u32 (GHS: values >= 2^31 are converted from v - 2^31) */
static inline u32 f2u(f32 v) {
    if (!(v < 2147483648.0f)) {
        return (u32)gabi::ftoi(v - 2147483648.0f) + 0x80000000u;
    }
    return (u32)gabi::ftoi(v);
}

/* 0218086C (unnamed by the matcher). HD: the sound ids come from tables; no NULL checks */
void daItem_c::set_bound_se() {
    WWHD_FUNC(0x0218086C, void, this);
    if (m_get_timer < 10) {
        return;
    }

    f32 a = std::fabs(field_0x650);
    u32 temp = f2u(a + a);
    if (temp > 100) {
        temp = 100;
    }

    u8 itemNo = m_itemNo;
    u32 id;
    if (itemNo >= 1 && itemNo <= 0xE) {
        /* rupees, magic, bombs: JA_SE_OBJ_LUPY_BOUND, JA_SE_OBJ_M_POT_BOUND, JA_SE_CM_BST_BOMB_BOUND... */
        id = gabi::load<u16>(0x10011F26 + itemNo * 2);
    } else if (itemNo >= 0x10 && itemNo <= 0x13) {
        id = gabi::load<u16>(0x10011F00 + itemNo * 2);
    } else if (itemNo >= 0x35 && itemNo <= 0x36) {
        id = 0x6988; /* JA_SE_CM_BST_ARROW_BOUND (magic/light arrows) */
    } else {
        return;
    }
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    mDoAud_seStart(id, &eyePos, temp, reverb);
}
VERIFY(0x0218086C, &daItem_c::set_bound_se);

/* c_xyz out-of-line helpers (results through a hidden pointer: (this, result, arg)) */
static inline void cXyz_normalize(const cXyz* v, cXyz* res) { gabi::call(0x0201B31C, v, res); }

/* 02180AA4 (the matcher names it Reflect: Reflect(wallNorm, &vel, 1.0f, 1.0f) is inlined). HD:
 * item 0x13 sounds when it bounces off a wall */
void daItem_c::checkWall() {
    WWHD_FUNC(0x02180AA4, void, this);
    if (!mAcch.ChkWallHit()) {
        return;
    }

    u32 pla = gabi::ea(dBgS_GetTriPla(dComIfG_Bgsp(), &mAcchCir));
    gabi::Local<cXyz> wallNorm;
    wallNorm->x = gabi::load<f32>(pla + 0);
    wallNorm->y = gabi::load<f32>(pla + 4);
    wallNorm->z = gabi::load<f32>(pla + 8);

    gabi::Local<cXyz> vel;
    vel->x = speedF * cM_ssin(current.angle.y);
    vel->y = speed.y;
    vel->z = speedF * cM_scos(current.angle.y);

    const f32 eps = 3.8146973e-06f; /* G_CM3D_F_ABS_MIN */
    if (!(std::fabs((f32)vel->x) < eps) || !(std::fabs((f32)vel->z) < eps)) {
        /* Reflect(wallNorm, &vel, 1.0f, 1.0f) */
        f32 mag = std_sqrtf(PSVECSquareMag(vel));
        gabi::Local<cXyz> tmp;
        gabi::Local<cXyz> moveNorm;
        gabi::Local<cXyz> surfNorm;
        cXyz_normalize(vel, tmp);
        moveNorm->copy(*tmp);
        cXyz_normalize(wallNorm, tmp);
        surfNorm->copy(*tmp);
        cXyz_ml(moveNorm, tmp, -1.0f);
        moveNorm->copy(*tmp);
        f32 dot = PSVECDotProduct(surfNorm, moveNorm);
        dot = dot + dot;
        gabi::Local<cXyz> scaled;
        cXyz_ml(surfNorm, scaled, dot);
        cXyz_mi(scaled, tmp, moveNorm);
        gabi::Local<cXyz> reflectVec;
        reflectVec->copy(*tmp);
        PSVECScale(reflectVec, reflectVec, mag);
        f32 rz = reflectVec->z;
        f32 rx = reflectVec->x;
        f32 ry = reflectVec->y;
        vel->z = rz;
        vel->y = ry;
        vel->x = rx;
        if (!(std::fabs(rx) < eps)) {
            current.angle.y = cM_atan2s(rx, rz);
            if (m_itemNo == 0x13) {
                f32 speed = std_sqrtf(PSVECSquareMag(vel));
                u32 temp = f2u(speed + speed);
                if (temp > 100) {
                    temp = 100;
                }
                s32 reverb = dComIfGp_getReverb(current.roomNo);
                mDoAud_seStart(0x6A07, &eyePos, temp, reverb);
            }
        }
    }
}
VERIFY(0x02180AA4, &daItem_c::checkWall);

/* 02180D2C: HD (item 0x14, the Tingle Bottle item?): removed with a smoke effect and a sound when
 * the flag at *(0x1018F4AC)+0x4FD4 is set */
void daItem_c::checkHD14Delete() {
    WWHD_FUNC(0x02180D2C, void, this);
    if (m_itemNo == 0x14 && gabi::load<u8>(gabi::load<u32>(0x1018F4AC) + 0x4FD4) != 0) {
        dComIfGp_particle_set(0x1C, &current.pos, NULL, NULL, 0xFF, gabi::at<dPa_levelEcallBack>(0x1047B274));
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(0x6A57, &current.pos, 0, reverb);
        fopAcM_delete(this);
    }
}
VERIFY(0x02180D2C, &daItem_c::checkHD14Delete);

/* |a - b| in the XZ plane (inline in HD) */
static f32 dist_xz(const cXyz* a, const cXyz* b) {
    gabi::Local<cXyz> d;
    cXyz_mi(a, d, b);
    gabi::Local<cXyz> xz;
    xz->x = d->x;
    xz->y = 0.0f;
    xz->z = d->z;
    return std_sqrtf(PSVECSquareMag(xz));
}

/* HD action 0xD (thrown to the player): adjust speedF so that the item lands at mHDTargetPos.
 * The landing height is a stack slot the code never initialises (sp+0x24 of itemActionForRupee) */
static void throwToTarget(daItem_c* i_this, u32 sp) {
    const u32 landing = sp + 0x20; /* cXyz; only x and z are written */
    f32 d1 = dist_xz(&i_this->current.pos, &i_this->mHDTargetPos);
    f32 d2 = dist_xz(&i_this->old.pos, &i_this->mHDTargetPos);
    int n = 0;
    if (d1 < 2000.0f && d2 > 2000.0f) {
        f32 vy = i_this->speed.y;
        f32 groundY = gabi::load<f32>(landing + 4);
        s16 angleY = i_this->current.angle.y;
        f32 spd = i_this->speedF;
        f32 posY = i_this->current.pos.y;
        f32 grav = i_this->gravity;
        if (!(vy < 0.0f) || !(posY < groundY)) {
            gabi::Local<cXyz> p;
            gabi::Local<cXyz> v;
            p->y = posY;
            p->x = i_this->current.pos.x;
            p->z = i_this->current.pos.z;
            v->y = vy;
            v->x = spd * cM_ssin(angleY);
            v->z = spd * cM_scos(angleY);
            bool fail = false;
            if (posY > groundY) {
                do {
                    PSVECAdd(p, v, p);
                    v->y = v->y + grav;
                    n++;
                    if (n > 100) {
                        fail = true;
                        break;
                    }
                } while (p->y > gabi::load<f32>(landing + 4));
            }
            if (!fail) {
                gabi::store<f32>(landing + 0, p->x);
                gabi::store<f32>(landing + 8, p->z);
                if (n > 0) {
                    f32 d3 = dist_xz(&i_this->home.pos, gabi::at<cXyz>(landing));
                    f32 d4 = dist_xz(&i_this->home.pos, &i_this->mHDTargetPos);
                    f32 cur = i_this->speedF;
                    f32 add = 0.0f;
                    if (d3 < d4 - 100.0f || d3 > d4 + 100.0f) {
                        add = (d4 - d3) / (f32)n;
                    }
                    f32 target = cur + add;
                    i_this->mHDTargetSpeedF = target;
                    if (target > 0.0f) {
                        cLib_chaseF(&i_this->speedF, target, 1.0f);
                    }
                    return;
                }
            }
        }
    }
    f32 target = i_this->mHDTargetSpeedF;
    if (target > 0.0f) {
        cLib_chaseF(&i_this->speedF, target, 1.0f);
    }
}

/* 02180DE4. HD: action 0xD (thrown to the player), item 0x14 does not turn on the ground, the
 * HD action 0xF item stops its wait timer when it lands */
BOOL daItem_c::itemActionForRupee() {
    WWHD_FUNC(0x02180DE4, BOOL, this);
    GuestFrame frame(0xA8);
    mAcch.CrrPos(dComIfG_Bgsp());
    checkWall();

    if (mAcch.ChkGroundLanding()) {
        if (mHD79E != 0) {
            mWaitTimer = 0;
        }
        f32 temp2 = field_0x650 * daItem_getData()->mGroundReflect;
        if (temp2 > gravity - 0.5f) {
            fopAcM_SetSpeedF(this, 0.0f);
        } else {
            speed.x = 0.0f;
            speed.y = -temp2;
            speed.z = 0.0f;
        }

        mOnGroundTimer = mOnGroundTimer + 1;
        if (mOnGroundTimer >= 2) {
            clrFlag(FLAG_UNK04);
        }

        set_bound_se();
    } else if (mAcch.ChkGroundHit()) {
        if (m_itemNo != 0x14) {
            itemDefaultRotateY();
        }
        fopAcM_SetSpeedF(this, 0.0f);
        clrFlag(FLAG_UNK04);
        mOnGroundTimer = 1;
    }

    if (speed.y != 0.0f) {
        field_0x650 = speed.y;
    }

    s16 rotateSpeed = daItem_getData()->mRotateXSpeed;
    mRotateSpeed = rotateSpeed;

    s16 target = 0;
    if (mOnGroundTimer == 0) {
        target = current.angle.x + rotateSpeed;
    }
    mTargetAngleX = target;

    if (!checkFlag(FLAG_UNK02)) {
        cLib_chaseAngleS(&current.angle.x, target, mRotateSpeed);
    }

    if (mAction == 0xD) {
        throwToTarget(this, frame.sp);
    }

    checkHD14Delete();
    return TRUE;
}
VERIFY(0x02180DE4, &daItem_c::itemActionForRupee);

/* 021811F4. HD: hearts thrown by action 0xD are collected after 30 frames */
BOOL daItem_c::itemActionForHeart() {
    WWHD_FUNC(0x021811F4, BOOL, this);
    f32 origSpeedY = speed.y;
    if (origSpeedY < 0.0f) {
        gravity = 0.0f;
        speed.z = 0.0f;
        speed.y = daItem_getData()->mHeartFallSpeed;
        speed.x = 0.0f;
    }

    mAcch.CrrPos(dComIfG_Bgsp());

    if (mAcch.m_flags & (dBgS_Acch::GROUND_LANDING | dBgS_Acch::GROUND_HIT)) {
        speed.x = 0.0f;
        clrFlag(FLAG_UNK04);
        speed.z = 0.0f;
        speedF = 0.0f;
        mExtraZRot = 0;
        speed.y = -1.0f;
        itemDefaultRotateY();
    } else if (origSpeedY < 0.0f) {
        f32 amplitude = daItem_getData()->mHeartAmplitude;
        speedF = cM_ssin(m_timer * daItem_getData()->mHeartFallCycleTime) * amplitude;
    }

    if (mAction == 0xD && m_timer > 30) {
        itemGetExecute();
    }
    return TRUE;
}
VERIFY(0x021811F4, &daItem_c::itemActionForHeart);

/* 02181338 */
BOOL daItem_c::itemActionForKey() {
    WWHD_FUNC(0x02181338, BOOL, this);
    mAcch.CrrPos(dComIfG_Bgsp());
    checkWall();

    if (mAcch.ChkGroundLanding()) {
        f32 temp2 = field_0x650 * daItem_getData()->mGroundReflect;
        if (temp2 > gravity - 0.5f) {
            speedF = 0.0f;
            mRotateSpeed = 0;
            current.angle.x = 0x4000;
            mTargetAngleX = 0x4000;
        } else {
            speed.z = 0.0f;
            speed.x = 0.0f;
            speed.y = -temp2;
        }

        mOnGroundTimer = mOnGroundTimer + 1;
        if (mOnGroundTimer >= 2) {
            clrFlag(FLAG_UNK04);
        }
    } else if (mAcch.ChkGroundHit()) {
        current.angle.x = 0;
        mOnGroundTimer = 1;
        mTargetAngleX = 0;
        clrFlag(FLAG_UNK04);
        itemDefaultRotateY();
    }

    if (speed.y != 0.0f) {
        field_0x650 = speed.y;
    }

    s16 rotateSpeed = daItem_getData()->mRotateXSpeed;
    mRotateSpeed = rotateSpeed;

    s16 target = 0;
    if (mOnGroundTimer == 0) {
        target = current.angle.x + rotateSpeed;
    }
    s16 step = mRotateSpeed;
    mTargetAngleX = target;
    cLib_chaseAngleS(&current.angle.x, target, step);

    return TRUE;
}
VERIFY(0x02181338, &daItem_c::itemActionForKey);

/* 021814C0 */
BOOL daItem_c::itemActionForEmono() {
    WWHD_FUNC(0x021814C0, BOOL, this);
    mAcch.CrrPos(dComIfG_Bgsp());

    if (mAcch.ChkGroundLanding()) {
        f32 temp2 = field_0x650 * daItem_getData()->mGroundReflect;
        if (temp2 > gravity - 0.5f) {
            speedF = 0.0f;
        } else {
            speed.x = 0.0f;
            speed.z = 0.0f;
            speed.y = -temp2;
        }

        set_bound_se();
    } else if (mAcch.ChkGroundHit()) {
        s16 rotationSpeed = 0xFFFF / daItem_getData()->mRotateYSpeed;
        rotationSpeed = rotationSpeed / 2;
        fopAcM_addAngleY(this, current.angle.y + rotationSpeed, rotationSpeed);
        speedF = 0.0f;
    }

    if (speed.y != 0.0f) {
        field_0x650 = speed.y;
    }

    return TRUE;
}
VERIFY(0x021814C0, &daItem_c::itemActionForEmono);

/* 021815E8. HD: JPABaseEmitter::setGlobalTranslation inline (emitters of version 7+ flip y) */
BOOL daItem_c::itemActionForArrow() {
    WWHD_FUNC(0x021815E8, BOOL, this);
    mAcch.CrrPos(dComIfG_Bgsp());

    if (mOnGroundTimer == 0 && mpParticleEmitter && fopAcM_SearchByName(fpcNm_BST_e)) { /* Gohdan */
        f32 z = current.pos.z;
        f32 y = current.pos.y;
        u32 e = gabi::ea(mpParticleEmitter);
        f32 x = current.pos.x;
        u8 version = gabi::load<u8>(e + 0x262);
        gabi::store<f32>(e + 0x22C, x);
        gabi::store<f32>(e + 0x230, y);
        gabi::store<f32>(e + 0x234, z);
        if (version >= 7) {
            gabi::store<f32>(e + 0x230, -gabi::load<f32>(e + 0x230));
        }
    }

    if (mAcch.ChkGroundLanding()) {
        f32 temp_f3 = field_0x650 * daItem_getData()->mGroundReflect;
        if (temp_f3 > gravity - 0.5f) {
            mOnGroundTimer = mOnGroundTimer + 1;
            speedF = 0.0f;
        } else {
            f32 spd = speedF;
            speed.z = 0.0f;
            speed.x = 0.0f;
            speed.y = -temp_f3;
            mOnGroundTimer = mOnGroundTimer + 1;
            speedF = spd * 0.5f;
        }

        if (mOnGroundTimer == 1 && fopAcM_SearchByName(fpcNm_BST_e)) { /* Gohdan */
            s8 roomNo = current.roomNo;
            JPABaseEmitter* emitter = dComIfGp_particle_set(0xA1E2 /* ID_AK_ST_BSTDROPARROWSMOKE00 */, &current.pos, NULL, NULL,
                                                            0xFF, (dPa_levelEcallBack*)mPtclSmokeCb, roomNo);
            if (emitter) {
                gabi::store<s32>(gabi::ea(emitter) + 0x5C, 1); /* setMaxFrame(1) */
            }
        }

        if (mpParticleEmitter) {
            JPABaseEmitter_becomeInvalidEmitter(gabi::ea(mpParticleEmitter));
            mpParticleEmitter = NULL;
        }

        set_bound_se();
    } else if (mAcch.ChkGroundHit()) {
        speedF = 0.0f;
        if (m_itemNo != dItemNo_HEART_CONTAINER_e) {
            itemDefaultRotateY();
        }
    }

    if (m_itemNo == dItemNo_HEART_CONTAINER_e) {
        if (mOnGroundTimer != 0) {
            s16 rotationSpeed = 0xFFFF / daItem_getData()->mRotateYSpeed;
            cLib_addCalcAngleS(&mRotateSpeed, rotationSpeed, 10, 0x400, 0x100);
        }

        cLib_chaseAngleS(&shape_angle.y, shape_angle.y + mRotateSpeed, mRotateSpeed);
    }

    if (speed.y != 0.0f) {
        field_0x650 = speed.y;
    }

    return TRUE;
}
VERIFY(0x021815E8, &daItem_c::itemActionForArrow);

/* 0218188C (GameCube: Nonmatching) */
BOOL daItem_c::itemActionForSword() {
    WWHD_FUNC(0x0218188C, BOOL, this);
    mAcch.CrrPos(dComIfG_Bgsp());

    bool isQuake = gabi::call<BOOL>(0x02529CE8, gabi::at<dDetect_c>(dComIfGp_ea() + PLAY_DETECT), &current.pos) != 0; /* chk_quake */
    if (isQuake && !checkFlag(FLAG_QUAKE) && mAcch.ChkGroundHit()) {
        speed.x = 0.0f;
        speed.y = 21.0f;
        speed.z = 0.0f;
        gravity = -3.5f;
    }

    if (mAcch.ChkGroundLanding()) {
        f32 temp = field_0x650 * 0.9f;
        speed.x = 0.0f;
        speed.y = -temp;
        speed.z = 0.0f;

        if (m_itemNo == dItemNo_SWORD_e) {
            if (m_get_timer > 15) {
                s32 reverb = dComIfGp_getReverb(current.roomNo);
                mDoAud_seStart(0x6971 /* JA_SE_OBJ_LNK_SWORD_FALL */, &eyePos, 0, reverb);
            }
        } else if (m_itemNo == dItemNo_SHIELD_e) {
            if (m_get_timer > 15) {
                s32 reverb = dComIfGp_getReverb(current.roomNo);
                mDoAud_seStart(0x6972 /* JA_SE_OBJ_LNK_SHIELD_FALL */, &eyePos, 0, reverb);
            }
        }
    }

    gabi::Local<dBgS_GndChk> gndChk; /* dBgS_ObjGndChk */
    const dBgS_GndChk_vt vt = {0x10011D94, 0x10011DA4, 0x10011DC4, 0x10011DB4};
    dBgS_GndChk_ct(gndChk, vt, true);
    gabi::Local<cXyz> bottomPos;
    gabi::Local<cXyz> topPos;
    bottomPos->x = 0.0f, bottomPos->y = 0.0f, bottomPos->z = 0.0f;
    topPos->x = 0.0f, topPos->y = 50.0f, topPos->z = 0.0f;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    PSMTXMultVec(mDoMtx_stack_c::get(), bottomPos, bottomPos);
    PSMTXMultVec(mDoMtx_stack_c::get(), topPos, topPos);
    dBgS_GndChk_SetPos(gndChk, bottomPos);
    f32 groundY = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
    dBgS_GndChk_SetPos(gndChk, topPos);
    f32 temp3 = groundY - bottomPos->y;
    groundY = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
    f32 temp4 = groundY - topPos->y;

    s16 rot;
    if (field_0x666 == 1) {
        rot = 0x2000;
        field_0x660 = rot;
    } else {
        rot = field_0x660;
    }

    if (temp3 > 0.0f || temp4 > 0.0f) {
        rot = (s16)gabi::ftoi((f32)rot * -0.8f);
        u8 n = field_0x666;
        field_0x660 = rot;
        field_0x666 = n + 1;
    }

    if (rot == 0) {
        field_0x666 = 0;
    } else {
        cLib_addCalcAngleS2(&current.angle.x, rot + 0x4000, 10, 0x800);
    }

    if (isQuake) {
        setFlag(FLAG_QUAKE);
    } else {
        clrFlag(FLAG_QUAKE);
    }

    /* ~dBgS_ObjGndChk (inline): this TU's vtables, then cBgS_Chk::~cBgS_Chk */
    u32 g = gabi::ea(gndChk.get());
    gabi::store<u32>(g + 0x40, 0x10011D84);
    gabi::store<u32>(g + 0x20, 0x10011D64);
    gabi::store<u32>(g + 0x4C, 0x10011D44);
    gabi::call(0x02008DAC, g, 0);
    return TRUE;
}
VERIFY(0x0218188C, &daItem_c::itemActionForSword);

/* 02181C4C */
void daItem_c::mode_wait_init() {
    WWHD_FUNC(0x02181C4C, void, this);
    mMode = MODE_WAIT;
    gravity = daItem_getData()->mGravity;
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)mPtclRippleCb);
}
VERIFY(0x02181C4C, &daItem_c::mode_wait_init);

/* water height under the item: the sea's wave height in the sea area, else the water check's */
static inline f32 acch_waterH(daItem_c* i_this) { return gabi::load<f32>(gabi::ea(&i_this->mAcch) + 0x1BC); } /* m_wtr height */

/* 02181C68: HD (items 0x13/0x14 in the water; action 0xE: follows the actor at play+0x5B3C) */
void daItem_c::waterFloat() {
    WWHD_FUNC(0x02181C68, void, this);
    mHD79C = 0;
    f32 x = current.pos.x, z = current.pos.z;
    f32 waterY = acch_waterH(this);
    if (daSea_ChkArea(x, z)) {
        waterY = daSea_calcWave(current.pos.x, current.pos.z);
    }

    if (mAction == 0xE) {
        u32 player = gabi::ea(dComIfGp_getPlayer(0));
        if (player != 0) {
            gabi::Local<cXyz> pos;
            pos->x = gabi::load<f32>(player + 0x314);
            pos->y = gabi::load<f32>(player + 0x318);
            pos->z = gabi::load<f32>(player + 0x31C);
            if (!gabi::call<BOOL>(0x02037F80, gabi::load<u32>(0x1018F450), pos.get())) {
                waterY = waterY - 50.0f;
            }
        }
    }

    if (mAcch.GetGroundH() > waterY) {
        speedF = 0.0f;
        mHD79C = 1;
        return;
    }

    u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C);
    if (ship != 0 && home.pos.y < 50.0f) {
        fopAc_ac_c* shipAc = gabi::at<fopAc_ac_c>(ship);
        gabi::Local<cXyz> d;
        gabi::Local<cXyz> xz;
        cXyz_mi(&shipAc->current.pos, d, &home.pos);
        xz->x = d->x, xz->y = 0.0f, xz->z = d->z;
        f32 d1 = PSVECSquareMag(xz);
        cXyz_mi(&current.pos, d, &home.pos);
        gabi::Local<cXyz> xz2;
        xz2->x = d->x, xz2->y = 0.0f, xz2->z = d->z;
        f32 d2 = PSVECSquareMag(xz2);
        if (d1 > 6250000.0f || d2 > 6250000.0f) {
            cLib_addCalc0(&speedF, 0.1f, 0.5f);
        } else {
            gabi::Local<dBgS_LinChk> linChk;
            const dBgS_LinChk_vt vt = {0x10011E44, 0x10011E54, 0x10011E74, 0x10011E64};
            dBgS_LinChk_ct(linChk, vt, false);
            dBgS_LinChk_Set(linChk, &current.pos, &shipAc->current.pos, this);
            if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                cLib_addCalc2(&speedF, 0.075f * shipAc->speedF, 0.1f, 0.5f);
            } else {
                speedF = 0.0f;
            }
            /* ~dBgS_LinChk (inline) */
            u32 c = gabi::ea(linChk.get());
            gabi::store<u32>(c + 0x58, 0x10011E74);
            gabi::store<u32>(c + 0x64, 0x10011D44);
            gabi::store<u32>(c + 0x20, 0x10011D34);
            cBgS_LinChk_dt(linChk, 0);
        }
        s16 angleY = current.angle.y;
        current.angle.y = cLib_targetAngleY(&current.pos, &shipAc->current.pos);
        fopAcM_posMoveF(this, NULL);
        current.angle.y = angleY;
    }

    f32 g = gravity;
    f32 py = current.pos.y;
    f32 v = (std::fabs(g) * (waterY - py)) / 7.5f;
    mHD768 = v;
    if (mAction == 0xE && v < 0.0f && waterY < py + 15.0f) {
        v = v + v;
        mHD768 = v;
    }

    f32 vy = speed.y + v;
    if (waterY < py) {
        if (vy < -10.0f) {
            vy = -10.0f;
        } else if (vy > 6.0f) {
            vy = 6.0f;
        }
    } else {
        if (vy < -6.0f) {
            vy = -6.0f;
        } else if (vy > 10.0f) {
            vy = 10.0f;
        }
    }
    if (waterY + 20.0f < py) {
        if (vy > 0.0f) {
            vy = 0.0f;
        }
    }
    if (py < waterY - 20.0f && vy < 0.0f) {
        speed.y = 0.0f;
    } else {
        speed.y = vy;
    }
}
VERIFY(0x02181C68, &daItem_c::waterFloat);

/* 0218214C. HD: item 0x14 grows by 1.2 and bobs (waterFloat) a random number of frames on the
 * first entry; the HD action 0xF item stops its wait timer */
void daItem_c::mode_water_init() {
    WWHD_FUNC(0x0218214C, void, this);
    f32 z = current.pos.z, x = current.pos.x;
    if (mHD79E != 0) {
        mWaitTimer = 0;
    }
    mMode = MODE_WATER;

    if (daSea_ChkArea(x, z)) {
        f32 seaH = daSea_calcWave(current.pos.x, current.pos.z);
        if (seaH > current.pos.y) {
            current.pos.y = seaH;
        }
    } else {
        if (!mAcch.ChkWaterHit() || acch_waterH(this) < current.pos.y) {
            mode_wait_init();
        }
        current.pos.y = acch_waterH(this);
    }

    mExtraZRot = 0;
    mRotateSpeed = 0;
    speed.x = 0.0f;
    speed.y = 0.0f;
    speed.z = 0.0f;
    speedF = 0.0f;
    current.angle.z = 0;
    current.angle.x = 0;
    clrFlag(FLAG_UNK04);

    if (m_itemNo == 0x14) {
        PSVECScale(&mScaleTarget, &mScaleTarget, 1.2f);
        f32 k = scale.x * 1.2f;
        f32 height = item_getH(m_itemNo) * k;
        f32 radius = (f32)dItem_data::getR(m_itemNo) * k;
        mCyl.SetR(radius);
        mCyl.SetH(height);
    }

    f32 sx = mScaleTarget.x;
    scale.x = sx;
    scale.y = mScaleTarget.y;
    scale.z = mScaleTarget.z;

    gabi::Local<cXyz> particleScale;
    f32 temp3 = (f32)gabi::load<u8>(dItem_data::item_info(m_itemNo)) / (f32)gabi::load<u8>(dItem_data::item_info(1 /* GREEN_RUPEE */));
    temp3 *= sx;
    particleScale->x = temp3;
    particleScale->y = temp3;
    particleScale->z = temp3;

    /* dComIfGp_particle_setShipTail(ID_AK_JN_HAMON00, ...) */
    dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &current.pos, NULL, particleScale, 0xFF,
                    (dPa_levelEcallBack*)mPtclRippleCb, -1, NULL, NULL, NULL);

    if (m_itemNo == 0x14) {
        gravity = -0.2f;
        if (mHD79D == 0) {
            s16 r = (s16)gabi::ftoi(cM_rndFX(80.0f));
            u8 action = mAction;
            current.angle.x = r + 0x400;
            if (action != 0xE) {
                s32 n = gabi::ftoi(cM_rndF(20.0f));
                if (n > 0) {
                    do {
                        speed.y = speed.y + -0.2f;
                        waterFloat();
                        current.pos.y = current.pos.y + speed.y;
                    } while (--n != 0);
                }
            }
            mHD79D = 1;
        }
    }
    gabi::store<f32>(gabi::ea(mPtclRippleCb) + 0x10, 0.0f); /* mPtclRippleCb.mRate */
}
VERIFY(0x0218214C, &daItem_c::mode_water_init);

/* dBgS_ObjGndChk_Yogan (lava) inline constructor / destructor */
static void yoganChk_ct(dBgS_GndChk* c) {
    const dBgS_GndChk_vt vt = {0x10011DD4, 0x10011DE4, 0x10011E04, 0x10011DF4};
    dBgS_GndChk_ct(c, vt, true);
    gabi::store<u32>(gabi::ea(c) + 0x50, 4);
}
static void gndChk_dt(dBgS_GndChk* c) {
    u32 g = gabi::ea(c);
    gabi::store<u32>(g + 0x20, 0x10011D64);
    gabi::store<u32>(g + 0x40, 0x10011D84);
    gabi::store<u32>(g + 0x4C, 0x10011D44);
    gabi::call(0x02008DAC, g, 0); /* cBgS_Chk::~cBgS_Chk */
}

/* 02182460: HD item 0x13 (a state machine in mHD787, timer mHD788: thrown up, falls, splashes
 * into the water or vanishes in smoke) */
BOOL daItem_c::itemActionForHD13() {
    WWHD_FUNC(0x02182460, BOOL, this);
    switch (mHD787) {
    case 0:
        scale.x = 1.0f, scale.y = 1.0f, scale.z = 1.0f;
        gravity = 0.0f;
        speed.x = 0.0f, speed.y = 0.0f, speed.z = 0.0f;
        speedF = 0.0f;
        break;
    case 1: {
        scale.x = 1.0f;
        field_0x650 = 50.0f;
        scale.y = 1.0f, scale.z = 1.0f;
        speedF = 30.0f;
        u32 player = gabi::ea(dComIfGp_getPlayer(0));
        if (player != 0) {
            current.angle.y = gabi::load<s16>(player + 0x32A); /* shape_angle.y */
        }
        gravity = daItem_getData()->mGravity * 0.75f;
        speed.x = 0.0f, speed.y = 50.0f, speed.z = 0.0f;
        mRotateSpeed = (s16)gabi::ftoi((f32)daItem_getData()->mRotateXSpeed * 0.5f);
        player = gabi::ea(dComIfGp_getPlayer(0));
        if (player != 0) {
            gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(player + 0xB4) + 0xE4), player, 0x11); /* player virtual (+0xE4)(0x11) */
        }
        mHD787 = 2;
        mHD788 = 90;
        break;
    }
    case 2: {
        mAcch.CrrPos(dComIfG_Bgsp());
        checkWall();
        s16 step = mRotateSpeed;
        s16 target = current.angle.x + step;
        mTargetAngleX = target;
        cLib_chaseAngleS(&current.angle.x, target, step);
        u8 onGround = mOnGroundTimer;
        scale.x = 1.0f, scale.y = 1.0f, scale.z = 1.0f;
        bool vanish = false;
        if (onGround != 0 && (!(field_0x650 > -gravity) || mAcch.ChkGroundHit())) {
            vanish = true;
        } else {
            mHD788 = mHD788 - 1;
            if (mHD788 == 0 || mAcch.ChkRoofHit()) {
                vanish = true;
            }
        }
        if (vanish) {
            mHD787 = 3;
            break;
        }
        if ((mAcch.ChkWaterHit() && acch_waterH(this) > current.pos.y) ||
            (daSea_ChkArea(current.pos.x, current.pos.z) && daSea_calcWave(current.pos.x, current.pos.z) > current.pos.y)) {
            mHD787 = 6;
            break;
        }
        gabi::Local<dBgS_GndChk> lavaChk;
        yoganChk_ct(lavaChk);
        u32 c = gabi::ea(lavaChk.get());
        gabi::store<f32>(c + 0x28, old.pos.y);
        gabi::store<f32>(c + 0x24, old.pos.x);
        gabi::store<f32>(c + 0x2C, old.pos.z);
        f32 lavaY = cBgS_GroundCross(dComIfG_Bgsp(), lavaChk);
        if (lavaY != -1e9f && lavaY > current.pos.y) {
            gabi::store<u32>(c + 0x4C, 0x10011D44);
            gabi::store<u32>(c + 0x40, 0x10011D84);
            gabi::store<u32>(c + 0x20, 0x10011D64);
            mHD787 = 3;
            gabi::call(0x02008DAC, c, 0);
            break;
        }
        if (mAcch.ChkGroundLanding()) {
            f32 f = field_0x650 * 0.75f;
            mOnGroundTimer = mOnGroundTimer + 1;
            speed.x = 0.0f;
            speed.y = -f;
            speed.z = 0.0f;
            set_bound_se();
        }
        gndChk_dt(lavaChk);
        break;
    }
    case 3: {
        hide();
        gravity = 0.0f;
        speed.x = 0.0f, speed.y = 0.0f;
        s16 ax = current.angle.x;
        speed.z = 0.0f;
        mTargetAngleX = ax;
        mRotateSpeed = 0;
        speedF = 0.0f;
        dComIfGp_particle_set(0x1C, &current.pos, NULL, NULL, 0xFF, gabi::at<dPa_levelEcallBack>(0x1047B274));
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(0x6A57, &current.pos, 0, reverb);
        mHD788 = 0x1C;
        mHD787 = 4;
        break;
    }
    case 4:
        mHD788 = mHD788 - 1;
        if (mHD788 == 0) {
            mHD787 = 5;
        }
        break;
    case 5:
        fopAcM_delete(this);
        break;
    case 6: {
        s16 angleY = current.angle.y;
        f32 spd = speedF;
        mode_water_init();
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(0x6918, &eyePos, 0, reverb);
        fopKyM_createWpillar(&current.pos, 0.5f, 1.0f, 0);
        current.angle.y = angleY;
        f32 f = field_0x650;
        mHDRotY = 0;
        speedF = spd * 0.75f;
        speed.x = 0.0f;
        speed.y = f * 0.07f;
        speed.z = 0.0f;
        f32 r = cM_rndF(256.0f);
        mMode = MODE_WAIT;
        mHD787 = 7;
        s16 ax = (s16)gabi::ftoi(r) + 0x900;
        current.angle.x = ax;
        mTargetAngleX = ax;
        mHD788 = 0x2D;
        mRotateSpeed = 0;
        gravity = -0.5f;
        break;
    }
    case 7: {
        f32 x = current.pos.x, z = current.pos.z;
        f32 waterY = acch_waterH(this);
        f32 py = current.pos.y;
        if (daSea_ChkArea(x, z)) {
            waterY = daSea_calcWave(current.pos.x, current.pos.z);
        }
        f32 g = gravity;
        f32 spd = speedF;
        f32 v = (std::fabs(g) * (waterY - py)) / 7.5f;
        speedF = spd * 0.95f;
        mHD768 = v;
        speed.y = (speed.y + v) * 0.95f;
        mAcch.CrrPos(dComIfG_Bgsp());
        cLib_chaseAngleS(&mHDRotY, mHDRotY + 0x888, 0x888);
        u16 rs = (u16)mRotateSpeed;
        f32 sp = speedF;
        s16 k = (s16)gabi::ftoi(cM_ssin(rs));
        u8 timer = mHD788;
        s16 target = mTargetAngleX;
        mRotateSpeed = rs + 0xCCC;
        timer = timer - 1;
        mHD788 = timer;
        current.angle.x = target * k;
        if (!(sp > 3.0f) || timer == 0) {
            mHD787 = 3;
        }
        break;
    }
    }

    if (speed.y != 0.0f) {
        field_0x650 = speed.y;
    }
    return TRUE;
}
VERIFY(0x02182460, &daItem_c::itemActionForHD13);

/* 02182B70. HD: items 0x13/0x14 bob (waterFloat); no turning while mHD79C */
void daItem_c::mode_water() {
    WWHD_FUNC(0x02182B70, void, this);
    if (m_itemNo == 0x14) {
        waterFloat();
    }
    mAcch.CrrPos(dComIfG_Bgsp());

    if (m_itemNo != 0x14) {
        if (daSea_ChkArea(current.pos.x, current.pos.z)) {
            f32 seaH = daSea_calcWave(current.pos.x, current.pos.z);
            if (seaH < current.pos.y) {
                mode_wait_init();
            } else {
                current.pos.y = seaH;
            }
        } else if (!mAcch.ChkWaterHit() || acch_waterH(this) < current.pos.y) {
            mode_wait_init();
        } else {
            current.pos.y = acch_waterH(this);
        }
    }

    if (mHD79C == 0) {
        s16 rotationSpeed = 0xFFFF / daItem_getData()->mRotateYSpeed;
        fopAcM_addAngleY(this, current.angle.y + rotationSpeed, rotationSpeed);
    }
    checkHD14Delete();
}
VERIFY(0x02182B70, &daItem_c::mode_water);

/* 02182C80 */
void daItem_c::mode_wait() {
    WWHD_FUNC(0x02182C80, void, this);
    if (checkFlag(FLAG_UNK04) && gabi::call<BOOL>(0x02551B90, (u8)m_itemNo) /* checkAppearEffect */) {
        u16 appearEffect = gabi::call<u16>(0x02551BBC, (u8)m_itemNo); /* getAppearEffect */
        dComIfGp_particle_setSimple(appearEffect, &current.pos);
    }

    switch (m_itemNo) {
    case 0x00: /* HEART */
    case 0x1E: /* TRIPLE_HEART */
        itemActionForHeart();
        break;
    case 0x07: case 0x08:             /* HEART_PIECE, HEART_CONTAINER */
    case 0x0B: case 0x0C: case 0x0D: case 0x0E: /* BOMB_* */
    case 0x10: case 0x11: case 0x12:  /* ARROW_* */
    case 0x35: case 0x36:             /* MAGIC_ARROW, LIGHT_ARROW */
        itemActionForArrow();
        break;
    case dItemNo_SMALL_KEY_e:
        itemActionForKey();
        break;
    case 0x09: case 0x0A:             /* SMALL_MAGIC, LARGE_MAGIC */
    case 0x1F:                        /* JOY_PENDANT */
    case 0x45: case 0x46: case 0x47: case 0x48: case 0x49: case 0x4A: case 0x4B: /* spoils */
        itemActionForEmono();
        break;
    case dItemNo_SWORD_e:
    case dItemNo_SHIELD_e:
    case dItemNo_DROPPED_SWORD_e:
        itemActionForSword();
        break;
    case 0x13: /* HD */
        itemActionForHD13();
        return;
    default:
        itemActionForRupee();
        break;
    }

    if ((mAcch.ChkWaterHit() && acch_waterH(this) > current.pos.y) ||
        (daSea_ChkArea(current.pos.x, current.pos.z) && daSea_calcWave(current.pos.x, current.pos.z) > current.pos.y)) {
        mode_water_init();
    }

    gabi::Local<dBgS_GndChk> lavaChk; /* dBgS_ObjGndChk_Yogan */
    yoganChk_ct(lavaChk);
    u32 c = gabi::ea(lavaChk.get());
    gabi::store<f32>(c + 0x2C, old.pos.z);
    gabi::store<f32>(c + 0x28, old.pos.y);
    gabi::store<f32>(c + 0x24, old.pos.x);
    f32 lavaY = cBgS_GroundCross(dComIfG_Bgsp(), lavaChk);
    if (lavaY != -1e9f && lavaY > current.pos.y) {
        fopAcM_delete(this);
    }
    gndChk_dt(lavaChk);
}
VERIFY(0x02182C80, &daItem_c::mode_wait);

/* 0217F93C. HD: y_speed starts at 0 (the GameCube reads an uninitialised register for some
 * actions); action 0xC (boss item) and 0xD (thrown to the player: computes speed, gravity and
 * the target position); item 0x13 starts its state machine without collision */
BOOL daItem_c::initAction() {
    WWHD_FUNC(0x0217F93C, BOOL, this);
    const daItemBase_c_m_data* data = daItem_getData();
    f32 y_speed = 0.0f;

    if (checkFlag(FLAG_UNK02)) {
        u8 action = mAction;
        scale.x = mScaleTarget.x;
        f32 ty = mScaleTarget.y, tz = mScaleTarget.z;
        scale.y = ty;
        scale.z = tz;

        switch (action) {
        case 4: {
            current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
            f32 temp = data->field_0x2C + cM_rndF(5.0f);
            speedF = cM_rndF(data->field_0x30);
            speed.x = 0.0f;
            speed.y = temp;
            speed.z = 0.0f;
            break;
        }
        case 5: /* daItemAct_BOSS_DISAPPEAR_e */
            speed.x = 0.0f, speed.y = 0.0f;
            actor_status = actor_status | fopAcStts_UNK4000_e;
            speed.z = 0.0f;
            mRotateSpeed = 0x4A8;
            speedF = 0.0f;
            scale.x = 0.0f, scale.y = 0.0f, scale.z = 0.0f;
            mItemStatus = STATUS_WAIT_BOSS1;
            break;
        case 0xC: /* daItemAct_BOSS_e */
            mRotateSpeed = 0x4A8;
            actor_status = actor_status | fopAcStts_UNK4000_e;
            scale.x = 1.0f, scale.y = 1.0f, scale.z = 1.0f;
            mItemStatus = STATUS_WAIT_BOSS2;
            break;
        }

        u8 flag = mFlag;
        mMode = MODE_WAIT;
        gravity = data->mGravity;
        mFlag = flag & ~FLAG_UNK04;
        return TRUE;
    }

    switch (mAction) {
    case 1: {
        f32 r = cM_rndFX(5.0f);
        f32 speedH = data->mSpeedH;
        speedF = speedH / 10.0f;
        y_speed = data->mLaunchSpeed + r;
        if (gabi::call<f32>(0x020079B4, 0) != 0.0f) { /* CPad_GET_STICK_VALUE(0) */
            speedF = speedH;
        }
        current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
        break;
    }
    case 3:
        y_speed = 25.0f;
        current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
        speedF = data->mVelocityScale;
        break;
    case 7:
        speedF = data->mVelocityScale * 1.5f;
        current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
        y_speed = data->mLaunchSpeed + cM_rndFX(5.0f);
        break;
    case 2:
    case 4:
    case 9:
        speedF = 0.0f;
        current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
        y_speed = data->mLaunchSpeed + cM_rndFX(5.0f);
        break;
    case 8: {
        current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
        f32 r = cM_rndFX(5.0f);
        f32 velocity = data->mVelocityScale;
        y_speed = data->field_0x44 + r;
        speedF = velocity;
        break;
    }
    case 0xA:
        scale.x = 0.0f;
        gravity = data->mGravity;
        mMode = MODE_WAIT;
        scale.y = 0.0f, scale.z = 0.0f;
        break;
    case 0xB:
        current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
        speedF = 0.0f;
        y_speed = 0.0f;
        break;
    case 6:
        y_speed = data->mLaunchSpeed + cM_rndFX(5.0f);
        break;
    case 0xD: /* HD: thrown to the player */
        if (isHeart(m_itemNo)) {
            speedF = 0.0f;
            current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
            y_speed = data->mLaunchSpeed + cM_rndFX(5.0f);
            mHDTargetSpeedF = -1.0f;
        } else {
            mAcch.CrrPos(dComIfG_Bgsp());
            if (mAcch.ChkWaterHit() && acch_waterH(this) > current.pos.y) {
                current.pos.y = acch_waterH(this) + 30.0f;
            } else if (daSea_ChkArea(current.pos.x, current.pos.z) &&
                       daSea_calcWave(current.pos.x, current.pos.z) > current.pos.y) {
                current.pos.y = daSea_calcWave(current.pos.x, current.pos.z) + 30.0f;
            }
            gabi::Local<csXyz> unused;
            csXyz_ct(unused, 0, 0, 0);
            u32 player = gabi::ea(dComIfGp_getPlayer(0));
            gabi::Local<cXyz> target;
            f32 px = gabi::load<f32>(player + 0x314);
            target->x = px;
            f32 py = gabi::load<f32>(player + 0x318);
            target->y = py;
            f32 pz = gabi::load<f32>(player + 0x31C);
            target->z = pz;
            f32 tx = gabi::fmadds(gabi::load<f32>(player + 0x33C), 45.0f, px);
            f32 ty = py + 100.0f;
            target->x = tx;
            target->y = ty;
            f32 tz = gabi::fmadds(gabi::load<f32>(player + 0x344), 45.0f, pz);
            mHDTargetPos.x = tx;
            mHDTargetPos.y = ty;
            mHDTargetPos.z = tz;
            target->z = tz;
            current.angle.y = gabi::call<s16>(0x0200F958, &current.pos, target.get()); /* cLib_targetAngleY */
            gabi::Local<cXyz> d;
            cXyz_mi(target, d, &current.pos);
            gabi::Local<cXyz> xz;
            xz->x = d->x, xz->y = 0.0f, xz->z = d->z;
            f32 dist = std_sqrtf(PSVECSquareMag(xz));
            s32 n = 45;
            f32 spd = dist / 45.0f;
            if (spd > 30.0f) {
                spd = 30.0f;
                n = gabi::ftoi(dist / 30.0f);
            }
            f32 dy = target->y - current.pos.y;
            u32 sum = 0; /* wraps like the console's 32-bit add */
            for (s32 i = 1; i <= n; i++) {
                sum += (u32)i;
            }
            f32 fn = (f32)n;
            f32 fsum = (f32)(s32)sum;
            mHDTargetSpeedF = -1.0f;
            speed.y = 30.0f;
            speedF = spd;
            gravity = gabi::fnmsubs(30.0f, fn, dy) / fsum;
        }
        break;
    case 0:
    case 5:
    default:
        break;
    }

    mExtraZRot = 0;

    if (isHeart(m_itemNo)) {
        speedF = speedF + speedF;
        mExtraZRot = (s16)gabi::ftoi(cM_rndFX((f32)data->mHeartTilt));
    }

    u8 action = mAction;
    if (m_itemNo == 0x13) {
        mHD787 = 0;
        actor_status = actor_status | fopAcStts_UNK4000_e;
        mCyl.SetTgType(0);
        mCyl.OffCoSPrmBit(1);
    }
    if (action != 0xD || isHeart(m_itemNo)) {
        gravity = data->mGravity;
        speed.x = 0.0f;
        speed.y = y_speed;
        speed.z = 0.0f;
    }
    scale.x = 0.0f, scale.y = 0.0f;
    u8 flag = mFlag;
    scale.z = 0.0f;
    mMode = MODE_WAIT;
    mFlag = flag | FLAG_UNK04;

    return TRUE;
}
VERIFY(0x0217F93C, &daItem_c::initAction);

/* 021800F0. HD: action 0xF (an item that cannot be collected, disappears after 30 frames and
 * becomes action 4); item 0x14 gets a random orientation; item 0x13 has a roof correction and no
 * Gohdan sparkle */
void daItem_c::CreateInit() {
    WWHD_FUNC(0x021800F0, void, this);
    mAcchCir.SetWall(30.0f, 30.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
    mAcch.m_flags = mAcch.m_flags & ~(u32)(dBgS_Acch::WATER_NONE | dBgS_Acch::ROOF_NONE); /* ClrWaterNone, ClrRoofNone */

    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */

    mStts.Init(0, 0xFF, this);
    mCyl.Set(m_cyl_src);
    mCyl.SetStts(&mStts);
    mCyl.mGObjCo.mHitCallback = 0x0217E50C; /* SetCoHitCallback(itemGetCallBack) */

    f32 height = item_getH(m_itemNo);
    f32 radius = (f32)dItem_data::getR(m_itemNo);

    if (scale.x > 1.0f) {
        radius *= scale.x;
        height *= scale.x;
    }
    mCyl.SetR(radius);
    mCyl.SetH(height);

    u32 param = fopAcM_GetParam(this);
    mWaitTimer = daItem_getData()->mWaitTime;
    field_0x650 = speed.y;
    mDisappearTimer = daItem_getData()->mDisappearTime;
    u8 type = daItem_prm::getType(this);
    mItemStatus = STATUS_UNK0;
    mType = type;
    mHDRotY = 0;
    mHD787 = 0;
    if (type == 3 || type == 1) {
        setFlag(FLAG_UNK02);
    }
    mAction = param >> 0x1A;
    mHD79E = 0;
    if ((param >> 0x1A) == 0xF) { /* HD */
        mAction = 4;
        mHD79E = 1;
        clrFlag(FLAG_UNK02);
        actor_status = actor_status | fopAcStts_UNK4000_e;
        mDisappearTimer = 30;
    }

    show();

    if (checkSpecialEffect(m_itemNo) && (m_itemNo != dItemNo_SMALL_KEY_e || checkFlag(FLAG_UNK02))) {
        u16 particleID = getSpecialEffect(m_itemNo);
        dComIfGp_particle_set(particleID, &current.pos, NULL, NULL, 0xFF, (dPa_levelEcallBack*)&mPtclFollowCb);
    }

    if (m_itemNo >= dItemNo_BOMB_5_e && m_itemNo <= dItemNo_BOMB_30_e) {
        mScaleTarget.x = 0.6f, mScaleTarget.y = 0.6f, mScaleTarget.z = 0.6f;
    } else {
        mScaleTarget.x = 1.0f, mScaleTarget.y = 1.0f, mScaleTarget.z = 1.0f;
    }

    s32 sw = daItem_prm::getSwitchNo2(this);
    mSpawnSwitchNo = sw;
    if (sw != 0xFF && !fopAcM_isSwitch(this, sw)) {
        hide();
        setFlag(FLAG_UNK02);
    }
    mCollideSwitchNo = daItem_prm::getSwitchNo(this);

    current.angle.z = 0;
    home.angle.z = 0;
    initAction();

    switch (m_itemNo) {
    case dItemNo_SWORD_e:
    case dItemNo_SHIELD_e:
        actor_status = actor_status | fopAcStts_UNK4000_e;
        break;
    case dItemNo_DROPPED_SWORD_e:
        current.angle.x = 0x4000;
        break;
    }

    if (m_itemNo == 0x14) {
        current.angle.x = (s16)gabi::ftoi(cM_rndF(4096.0f));
        current.angle.y = (s16)gabi::ftoi(cM_rndF(32768.0f));
        current.angle.z = (s16)gabi::ftoi(cM_rndF(4096.0f));
    } else if (m_itemNo == 0x13) {
        mAcch.SetRoofCrrHeight(30.0f);
    }
    set_mtx();
    animPlay(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    if (m_itemNo == 0x13) {
        return;
    }
    if (fopAcM_SearchByName(fpcNm_BST_e)) { /* Gohdan */
        mpParticleEmitter = dComIfGp_particle_set(0x81E1 /* ID_AK_SN_BSTKIRAKIRAARROW00 */, &current.pos);
    }
}
VERIFY(0x021800F0, &daItem_c::CreateInit);

/* 021805CC. HD: no hearts when 027200D0(save+0x12C0)/0271FC5C report so (Hero Mode) */
cPhs_State daItem_c::_daItem_create() {
    WWHD_FUNC(0x021805CC, cPhs_State, this);
    /* fopAcM_ct(this, daItem_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = DAITEMBASE_VTBL;
            const dBgS_ObjAcch_vt acchVt = {0x10011E14, 0x10011E34, 0x10011E24};
            dBgS_ObjAcch_ct(&mAcch, acchVt);
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x10011D24);
            __vtbl = DAITEM_VTBL;
            gabi::call(0x025A9084, mPtclRippleCb);             /* dPa_rippleEcallBack::dPa_rippleEcallBack */
            dPa_followEcallBack_ct(&mPtclFollowCb, 0, 0);
            gabi::call(0x025A5B18, mPtclSmokeCb, 1);           /* dPa_smokeEcallBack::dPa_smokeEcallBack(1) */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    m_itemNo = daItem_prm::getItemNo(this);

    if (gabi::load<u32>(dItem_data::field_item_res(m_itemNo)) == 0) { /* getFieldArc */
        setLoadError();
        return cPhs_ERROR_e;
    }

    mItemBitNo = daItem_prm::getItemBitNo(this);
    if (m_itemNo != dItemNo_BLUE_JELLY_e) {
        mItemBitNo = mItemBitNo & 0x7F;
        if (fopAcM_isItem(this, mItemBitNo) && mItemBitNo != 0x7F) {
            setLoadError();
            return cPhs_ERROR_e;
        }
    }
    /* HD */
    u32 mode = gabi::call<u32>(0x027200D0, dComIfGs_raw() + 0x12C0);
    if (gabi::call<BOOL>(0x0271FC5C, mode) && isHeart(m_itemNo)) {
        setLoadError();
        return cPhs_ERROR_e;
    }

    cPhs_State phase_state =
        dComIfG_resLoad(&mPhs, gabi::at<const char>(gabi::load<u32>(dItem_data::field_item_res(m_itemNo))));
    if (phase_state == cPhs_COMPLEATE_e) {
        u32 heap_size = gabi::load<u16>(dItem_data::field_item_res(m_itemNo) + 0x18); /* getFieldHeapSize */
        if (!fopAcM_entrySolidHeap(this, 0x0218422C /* CheckFieldItemCreateHeap */, heap_size)) {
            return cPhs_ERROR_e;
        }
        CreateInit();
    }

    return phase_state;
}
VERIFY(0x021805CC, &daItem_c::_daItem_create);

/* 02180848 */
static cPhs_State daItem_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02180848, cPhs_State, i_this);
    return ((daItem_c*)i_this)->_daItem_create();
}
VERIFY(0x02180848, daItem_Create);

/* 02182FB0 */
static void __sinit_d_a_item_cpp() {
    WWHD_FUNC(0x02182FB0, void, (u32)0);
    sinit_header_statics(0x104647F8, 0x101B7A18);
}
VERIFY(0x02182FB0, __sinit_d_a_item_cpp);

/* 02183044: daItem_c deleting destructor (the particle callbacks have trivial destructors) */
static void daItem_c_dt(daItem_c* i_this, s32 flags) {
    WWHD_FUNC(0x02183044, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x021832F4, i_this, 0); /* daItemBase_c::~daItemBase_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02183044, daItem_c_dt);
