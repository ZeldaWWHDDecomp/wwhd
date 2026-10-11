/* HD shop selection, item/camera management and cursor rendering.
 * Native inventory025BB5FC..025BD5FF; preceding sea initializer excluded. */
#include "d/d_shop.h"
#include "d/actor/d_a_player.h"
#include "d/actor/d_a_shop_item.h"
#include "gc_api/SSystem/c_lib.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_item.h"
using namespace gabi;
static ShopHeaderStaticsHD shopHeaderStatics;
static u32 shopDestructorRecords[] = {0x00000000, 0x1047C98C, 0x02801024, 0x00000000, 0x1047C988,
                                      0x028ED994, 0x00000000, 0x1047C989, 0x028EADC8};

static u32 Item_set_data_tbl[] = {0x101EB044, 0x101EB054, 0x101EB064, 0x101EB074, 0x101EB084,
                                  0x101EB094, 0x101EB0A4, 0x101EB0A4, 0x00000000};
static u32 select_list_data_tbl[] = {0x101EB0B4, 0x101EB0D4, 0x101EB0F4, 0x101EB114, 0x101EB134,
                                     0x101EB154, 0x101EB174, 0x101EB194, 0x101EB1B4};
static u32 shop_cam_data_tbl[] = {0x101EB1D4, 0x101EB20C, 0x101EB244, 0x101EB27C, 0x101EB2B4,
                                  0x101EB2EC, 0x101EB324, 0x101EB35C, 0x101EB394, 0x101EB3CC,
                                  0x101EB404, 0x101EB43C, 0x101EB474};
static u32 default_select_msg[] = {0x00001DC1, 0x00002777, 0x00002784, 0x00000F3F, 0x00000F3F,
                                   0x00000F3F, 0x00002F48, 0x00002F48, 0x00002866};
static u32 Item_set_pos_data_tbl[] = {0x101EB698, 0x101EB6BC, 0x101EB6E0, 0x101EB704, 0x101EB728,
                                      0x101EB74C, 0x101EB770, 0x101EB794, 0x101EB998};
static s16 angle_z[] = {-8192, 8192, 24576, -24576};

static f32 cursorScaleOne = 1.0f;
static f32 cursorUpper = 30.0f;
static f32 cursorLower = 25.0f;
static f32 shopZero = 0.0f;
static f32 cameraBack = 100.0f;
static f32 cameraFovy = 60.0f;
static f32 cameraFraction = 0.25f;
static f32 cameraMaxStep = 20.0f;
static f32 cursorDelayRange = 5.0f;
static f32 normalCameraX = -136.0f;
static f32 normalCameraY = 102.0f;
static f32 normalCenterZ = -254.0f;
static f32 normalEyeZ = 90.0f;
static f32 itemSpacing = 75.0f;
static f32 shopHalf = 0.5f;
static f32 cursorSqrtTwo = 1.414199948310852f;
static f32 negativePi = -3.1415927410125732f;
static f32 positivePi = 3.1415927410125732f;
static f32 zoomSpeed = 0.05000000074505806f;
static f32 waitSpeed = 0.10000000149011612f;

static void copyVector(u32 dst, u32 src) {
    store<u32>(dst, load<u32>(src));
    store<u32>(dst + 4, load<u32>(src + 4));
    store<u32>(dst + 8, load<u32>(src + 8));
}

static u32 findItem(u32 id) { return ea(fopAcM_SearchByID(id)); }

static u32 activeCamera() {
    s8 index = static_cast<s8>(at<dComIfG_inf_c>(dComIfGp_ea())->play.mPlayerInfo[0].mCameraID);
    return load<u32>(dComIfGp_ea() + 0x5AF8 + (u32)((s32)index * 52));
}

static u32 zeroPosition(u32 dst) {
    if (!dst)
        dst = ea(operator_new(12));
    if (dst)
        at<cXyz>(dst)->set(cXyz::Zero);
    return dst;
}

/* 025BBE98 */
u32 ShopCam_action_c::shop_cam_action_init() {
    u32 p = ea(this),
        player = ea(at<dComIfG_inf_c>(dComIfGp_ea())->play.mPlayerInfo[0].mpPlayer.get());
    at<daPy_py_c>(player)->onNoResetFlg0(daPy_py_c::daPyFlg0_NO_DRAW);
    s16 index = static_cast<s16>(mCamDataIdx);
    u32 slot = ea(&shop_cam_data_tbl[0]) + (u32)((s32)index * 4);
    mCurrActionFunc.mVtableIndex = 0xFFFF;
    mCurrActionFunc.mThisDelta = 0;
    mCurrActionFunc.mFunction = 0x025BBF4C;
    u32 data = load<u32>(slot);
    m18.set(at<ShopItems_c__shop_cam_data>(data)->m00);
    data = load<u32>(slot);
    m24.set(at<ShopItems_c__shop_cam_data>(data)->m0C);
    data = load<u32>(slot);
    f32 fovy = static_cast<f32>(at<ShopItems_c__shop_cam_data>(data)->m18);
    m54 = 0xFFFF;
    m30 = fovy;
    return 1;
}

/* 025BBF4C */
u32 ShopCam_action_c::shop_cam_action() {
    u32 p = ea(this), camera = activeCamera() + 0x248;
    shop_camera::Stay(at<void>(camera));
    shop_camera::SetTrimSize(at<void>(camera), 1);
    s16 index = static_cast<s16>(mCamDataIdx), selected = static_cast<s16>(m54);
    u32 data = shopTableValue(shop_cam_data_tbl, static_cast<u32>(static_cast<s32>(index)));
    if (selected >= 0)
        data += 0x1C;
    Local<cXyz> desiredCenter, desiredEye, center, eye;
    desiredCenter->set(at<ShopItems_c__shop_cam_data>(data)->m00);
    desiredEye->set(at<ShopItems_c__shop_cam_data>(data)->m0C);
    f32 fraction = cameraFraction, limit = cameraMaxStep,
        fovy = static_cast<f32>(at<ShopItems_c__shop_cam_data>(data)->m18);
    cLib_addCalcPos2(&m18, *desiredCenter.get(), fraction, limit);
    cLib_addCalcPos2(&m24, *desiredEye.get(), fraction, limit);
    cLib_addCalc2(&m30, fovy, fraction, cursorDelayRange);
    copyVector(ea(center.get()), ea(&m18));
    copyVector(ea(eye.get()), ea(&m24));
    shop_camera::Set(at<void>(camera), center.get(), eye.get(), 0, static_cast<f32>(m30));
    return 1;
}

/* 025BC1B0 */
u32 ShopCam_action_c::rsh_talk_cam_action_init(void* actor, void* desiredCenter, void* desiredEye,
                                               f32 fovy) {
    u32 p = ea(this), a = ea(actor);
    if (static_cast<s16>(mCurrActionFunc.mVtableIndex) == -1 &&
        static_cast<s16>(mCurrActionFunc.mThisDelta) == 0 &&
        static_cast<u32>(mCurrActionFunc.mFunction) == 0x025BC38C)
        return 1;
    u32 player = ea(at<dComIfG_inf_c>(dComIfGp_ea())->play.mPlayerInfo[0].mpPlayer.get());
    at<daPy_py_c>(player)->onNoResetFlg0(daPy_py_c::daPyFlg0_NO_DRAW);
    mCurrActionFunc.mThisDelta = 0;
    mCurrActionFunc.mVtableIndex = 0xFFFF;
    mCurrActionFunc.mFunction = 0x025BC38C;
    PSMTXTrans(mDoMtx_stack_c::get(),
               static_cast<f32>(at<fopAc_ac_c>(a)->attention_info.position.x),
               static_cast<f32>(at<fopAc_ac_c>(a)->attention_info.position.y),
               static_cast<f32>(at<fopAc_ac_c>(a)->attention_info.position.z));
    mDoMtx_stack_c::YrotM(static_cast<s16>(at<fopAc_ac_c>(a)->current.angle.y));
    Local<cXyz> center, eye;
    mDoMtx_stack_c::multVec(static_cast<cXyz*>(desiredCenter), center.get());
    mDoMtx_stack_c::multVec(static_cast<cXyz*>(desiredEye), eye.get());
    copyVector(ea(&m18), ea(center.get()));
    copyVector(ea(&m24), ea(eye.get()));
    m30 = fovy;
    m54 = 0xFFFF;
    return 1;
}

/* 025BC38C */
u32 ShopCam_action_c::rsh_talk_cam_action() {
    u32 p = ea(this), camera = activeCamera() + 0x248;
    shop_camera::Stay(at<void>(camera));
    shop_camera::SetTrimSize(at<void>(camera), 1);
    Local<cXyz> center, eye;
    copyVector(ea(center.get()), ea(&m18));
    copyVector(ea(eye.get()), ea(&m24));
    shop_camera::Set(at<void>(camera), center.get(), eye.get(), 0, static_cast<f32>(m30));
    return 1;
}

/* 025BC438 */
u32 ShopCam_action_c::ds_normal_cam_action_init() {
    u32 p = ea(this),
        player = ea(at<dComIfG_inf_c>(dComIfGp_ea())->play.mPlayerInfo[0].mpPlayer.get());
    at<daPy_py_c>(player)->onNoResetFlg0(daPy_py_c::daPyFlg0_NO_DRAW);
    f32 x = normalCameraX, y = normalCameraY, z = normalCenterZ, ez = normalEyeZ, fovy = cameraFovy;
    mCurrActionFunc.mThisDelta = 0;
    mCurrActionFunc.mFunction = 0x025BC4DC;
    m18.y = y;
    m18.z = z;
    m18.x = x;
    m24.z = ez;
    m54 = 0xFFFF;
    mCurrActionFunc.mVtableIndex = 0xFFFF;
    m30 = fovy;
    m24.x = x;
    m24.y = y;
    return 1;
}

/* 025BC4DC */
u32 ShopCam_action_c::ds_normal_cam_action() {
    u32 p = ea(this), camera = activeCamera() + 0x248;
    shop_camera::Stay(at<void>(camera));
    shop_camera::SetTrimSize(at<void>(camera), 1);
    f32 x = normalCameraX, y = normalCameraY, z = normalCenterZ, ez = normalEyeZ, fovy = cameraFovy;
    m24.y = y;
    m18.y = y;
    m30 = fovy;
    m18.z = z;
    m18.x = x;
    m24.x = x;
    m24.z = ez;
    Local<cXyz> center, eye;
    center->x = x;
    center->y = y;
    center->z = z;
    eye->x = x;
    eye->y = y;
    eye->z = ez;
    shop_camera::Set(at<void>(camera), center.get(), eye.get(), 0, fovy);
    return 1;
}

/* 025BC5B0 */
void ShopCam_action_c::Save() {
    u32 camera = static_cast<u32>(at<dComIfG_inf_c>(dComIfGp_ea())->play.mCameraInfo[0].mpCamera);
    if (!camera)
        return;
    copyVector(ea(&mOrigCenter), ea(&at<ShopCameraViewHD>(camera)->mCenter));
    copyVector(ea(&mOrigEye), ea(&at<ShopCameraViewHD>(camera)->mEye));
    mOrigFovy = static_cast<f32>(at<ShopCameraViewHD>(camera)->mFovy);
}

/* 025BC620 */
void ShopCam_action_c::Reset() {
    u32 p = ea(this), camera = activeCamera() + 0x248;
    Local<cXyz> center, eye;
    copyVector(ea(center.get()), ea(&mOrigCenter));
    copyVector(ea(eye.get()), ea(&mOrigEye));
    shop_camera::Set(at<void>(camera), center.get(), eye.get(), 0, static_cast<f32>(mOrigFovy));
    shop_camera::Stay(at<void>(camera));
    copyVector(ea(eye.get()), ea(&mOrigEye));
    copyVector(ea(center.get()), ea(&mOrigCenter));
    shop_camera::Reset(at<void>(camera), center.get(), eye.get(), 0, static_cast<f32>(mOrigFovy));
    mCurrActionFunc.mThisDelta = 0;
    mCurrActionFunc.mFunction = 0;
    mCurrActionFunc.mVtableIndex = 0;
}

/* 025BC714 */
void ShopCam_action_c::move() {
    u32 p = ea(this);
    s16 flag = static_cast<s16>(mCurrActionFunc.mVtableIndex);
    if (!flag)
        return;
    u32 receiver = p + (s32) static_cast<s16>(mCurrActionFunc.mThisDelta), target;
    if (flag < 0)
        target = static_cast<u32>(mCurrActionFunc.mFunction);
    else
        target = load<u32>(load<u32>(receiver + (s32)load<s16>(p + 6)) + (u32)((s32)flag * 8) + 4);
    call_ptr<void>(target, at<void>(receiver));
}

/* 025BC810 */
void ShopItems_c::createItem(s32 count, s32 room) {
    u32 p = ea(this);
    s16 data = static_cast<s16>(mItemSetListGlobalIdx);
    Local<csXyz> angle;
    angle->x = load<s16>(0x101FFB14);
    angle->y = load<s16>(0x101FFB16);
    angle->z = load<s16>(0x101FFB18);
    auto itemAt = [&](s16 idx, u32 which) {
        u32 entry = load<u32>(
            shopTableValue(Item_set_data_tbl, static_cast<u32>(static_cast<s32>(idx))) + which * 4);
        return static_cast<u32>(at<ShopItems_c__ItemData>(
                                    static_cast<u32>(at<__shop_items_set_data>(entry)->mpItemData))
                                    ->mItemNo);
    };
    if (data == 0) {
        Local<cXyz> pos;
        copyVector(ea(pos.get()), shopTableValue(Item_set_pos_data_tbl, 0));
        dComIfGs_isEventBit(0xD04);
        dComIfGs_isEventBit(0xD02);
        itemActorProcessId(0) = shop_createShopItem(
            pos.get(), itemAt(static_cast<s16>(mItemSetListGlobalIdx), 0), angle.get(), room, 0, 0);
        f32 spacing = itemSpacing;
        pos->x = fadds_ppc(pos->x, spacing);
        if (dComIfGs_isEventBit(0xD04)) {
            itemActorProcessId(1) =
                shop_createShopItem(pos.get(), itemAt(static_cast<s16>(mItemSetListGlobalIdx), 1),
                                    angle.get(), room, 0, 0);
            pos->x = fadds_ppc(pos->x, spacing);
        }
        if (dComIfGs_isEventBit(0xD02))
            itemActorProcessId(2) =
                shop_createShopItem(pos.get(), itemAt(static_cast<s16>(mItemSetListGlobalIdx), 2),
                                    angle.get(), room, 0, 0);
    } else
        for (s32 i = 0; i < count; ++i) {
            s16 idx = static_cast<s16>(mItemSetListGlobalIdx);
            u32 list = shopTableValue(Item_set_data_tbl, static_cast<u32>(static_cast<s32>(idx)));
            u32 entry = load<u32>(list + (u32)i * 4),
                positions =
                    shopTableValue(Item_set_pos_data_tbl, static_cast<u32>(static_cast<s32>(idx)));
            u32 item =
                static_cast<u32>(at<ShopItems_c__ItemData>(
                                     static_cast<u32>(at<__shop_items_set_data>(entry)->mpItemData))
                                     ->mItemNo);
            itemActorProcessId(i) = shop_createShopItem(at<void>(positions + (u32)i * 12), item,
                                                        angle.get(), room, 0, 0);
        }
    mNumItems = (u32)count;
}

/* 025BCA2C */
u32 ShopItems_c::Item_Select(s32 index) {
    u32 p = ea(this), item = findItem(static_cast<u32>(itemActorProcessId(index)));
    if (!item)
        return 1;
    u32 rotation = ea(at<daShopItem_c>(item)->getRotateP());
    u32 pos = ea(at<daShopItem_c>(item)->getPosP());
    Local<cXyz> home, target, center, difference;
    copyVector(ea(home.get()), ea(&at<fopAc_ac_c>(item)->home.pos));
    copyVector(ea(target.get()), ea(&m30));
    shopItem_getCenter(at<daShopItem_c>(item), center.get());
    shop_PSVECSubtract(target.get(), center.get(), target.get());
    bool zoom = static_cast<s16>(m3C) == 1;
    cXyz_mi(target.get(), difference.get(), home.get());
    f64 length = shop_squareMagnitude(difference.get());
    f64 distance = shop_sqrt(length);
    f32 maxStep = (f32)(distance * (zoom ? zoomSpeed : waitSpeed));
    cLib_addCalcPos2(at<cXyz>(pos), *(zoom ? target.get() : home.get()), shopHalf, maxStep);
    at<csXyz>(rotation)->y = (u32)(s32) static_cast<s16>(at<csXyz>(rotation)->y) + 0x400;
    return 1;
}

/* 025BCBAC */
u32 ShopItems_c::Item_Wait(s32 index) {
    u32 item = findItem(static_cast<u32>(itemActorProcessId(index)));
    if (!item)
        return 0;
    u32 rotation = ea(at<daShopItem_c>(item)->getRotateP());
    shop_addCalcAngleS(at<void>(rotation + 2),
                       (s32) static_cast<s16>(at<fopAc_ac_c>(item)->home.angle.y), 4, 0x800, 0x80);
    u32 pos = ea(at<daShopItem_c>(item)->getPosP());
    Local<cXyz> home;
    copyVector(ea(home.get()), ea(&at<fopAc_ac_c>(item)->home.pos));
    cLib_addCalcPos2(at<cXyz>(pos), *home.get(), shopHalf, cameraMaxStep);
    return 1;
}

/* 025BCC7C */
u32 ShopItems_c::Item_ZoomUp(void* pos) {
    m3C = 1;
    copyVector(ea(&m30), ea(pos));
    return 1;
}

/* 025BCCA4 */
u32 ShopItems_c::Item_Move() {
    u32 p = ea(this);
    for (s32 i = 0; i < static_cast<s16>(mNumItems); ++i) {
        if (static_cast<u32>(itemActorProcessId(i)) != 0xFFFFFFFFu) {
            if (i == static_cast<s16>(mSelectedItemIdx))
                Item_Select(i);
            else
                Item_Wait(i);
        }
        if (static_cast<u8>(itemSoldOutFlag((s16)i)) == 1) {
            u32 item = findItem(static_cast<u32>(itemActorProcessId(i)));
            if (item)
                at<daItemBase_c>(item)->hide();
        }
    }
    m3C = 0;
    return 1;
}

/* 025BCFA8 */
u32 ShopItems_c::getSelectItemPos(void* out) {
    u32 p = ea(this), dst = ea(out);
    s16 index = static_cast<s16>(mSelectedItemIdx);
    if (index < 0)
        return zeroPosition(dst);
    u32 item = findItem(static_cast<u32>(itemActorProcessId(index)));
    if (!item)
        return zeroPosition(dst);
    Local<cXyz> center;
    shopItem_getCenter(at<daShopItem_c>(item), center.get());
    u32 pos = ea(at<daShopItem_c>(item)->getPosP());
    return shop_positionAdd(at<void>(pos), out, center.get());
}

/* 025BD0EC */
u32 ShopItems_c::getSelectItemBasePos(void* out) {
    u32 p = ea(this), dst = ea(out);
    s16 index = static_cast<s16>(mSelectedItemIdx);
    if (index < 0)
        return zeroPosition(dst);
    u32 item = findItem(static_cast<u32>(itemActorProcessId(index)));
    if (!item)
        return zeroPosition(dst);
    Local<cXyz> center;
    shopItem_getCenter(at<daShopItem_c>(item), center.get());
    return shop_positionAdd(&at<fopAc_ac_c>(item)->home.pos, out, center.get());
}

/* 025BCD90 */
void ShopItems_c::hideSelectItem() {
    u32 p = ea(this);
    s16 index = static_cast<s16>(mSelectedItemIdx);
    if (index < 0)
        return;
    u32 item = findItem(static_cast<u32>(itemActorProcessId(index)));
    if (item)
        at<daItemBase_c>(item)->hide();
    mbIsHide = 1;
}

/* 025BCECC */
void ShopItems_c::SoldOutItem(s32 index) {
    u32 p = ea(this), item = findItem(static_cast<u32>(itemActorProcessId(index)));
    if (item)
        at<daItemBase_c>(item)->hide();
    itemSoldOutFlag(index) = 1;
}

/* 025BD248 */
u32 ShopItems_c::getItemNo(s32 index) {
    __shop_items_set_data* entry = itemSetData(index);
    return at<ShopItems_c__ItemData>(static_cast<u32>(entry->mpItemData))->itemNoByte();
}

/* 025BCE04 */
void ShopItems_c::showItem() {
    u32 p = ea(this);
    for (s32 i = 0; i < static_cast<s16>(mNumItems); ++i) {
        u32 item = findItem(static_cast<u32>(itemActorProcessId(i)));
        if (!item || static_cast<u8>(itemSoldOutFlag(i)) == 1)
            continue;
        at<daItemBase_c>(item)->show();
        // Source components are captured together before stores, as in the native
        // copy.
        u32 z = load<u32>(item + 0x2F4), x = load<u32>(item + 0x2EC), y = load<u32>(item + 0x2F0);
        store<u32>(item + 0x314, x);
        store<u32>(item + 0x318, y);
        store<u32>(item + 0x31C, z);
        u32 rotation = ea(at<daShopItem_c>(item)->getRotateP());
        at<csXyz>(rotation)->y = static_cast<u16>(at<fopAc_ac_c>(item)->home.angle.y);
    }
    mbIsHide = 0;
}

/* 025BD22C */
u32 ShopItems_c::getSelectItemNo() {
    u32 p = ea(this);
    s16 index = static_cast<s16>(mSelectedItemIdx);
    __shop_items_set_data* entry = itemSetData(index);
    return at<ShopItems_c__ItemData>(static_cast<u32>(entry->mpItemData))->itemNoByte();
}

/* 025BD260 */
u32 ShopItems_c::getSelectItemShowMsg() {
    u32 p = ea(this);
    s16 index = static_cast<s16>(mSelectedItemIdx);
    return static_cast<u32>(itemSetData(index)->mShowMsgNo);
}

/* 025BD278 */
u32 ShopItems_c::getSelectItemBuyMsg() {
    u32 p = ea(this);
    s16 index = static_cast<s16>(mSelectedItemIdx);
    return static_cast<u32>(itemSetData(index)->mBuyMsgNo);
}

/* 025BB5FC */
s16 dShop_get_next_select(s32 direction, void* self) {
    u32 p = ea(self);
    s16 data = static_cast<s16>(at<ShopItems_c>(p)->mItemSetListGlobalIdx),
        original = static_cast<s16>(at<ShopItems_c>(p)->mSelectedItemIdx), index = original;
    u32 table = shopTableValue(select_list_data_tbl, static_cast<u32>(static_cast<s32>(data)));
    for (;;) {
        u32 entry = table + (u32)((s32)index * 8) + 8;
        ShopItems_c__select_list_data* neighbors = at<ShopItems_c__select_list_data>(entry);
        switch (direction) {
        case 0:
            index = static_cast<s16>(neighbors->mLeftIdx);
            break;
        case 1:
            index = static_cast<s16>(neighbors->mRightIdx);
            break;
        case 2:
            index = static_cast<s16>(neighbors->mUpIdx);
            break;
        case 3:
            index = static_cast<s16>(neighbors->mDownIdx);
            break;
        }
        if (index == original ||
            static_cast<u32>(at<ShopItems_c>(p)->itemActorProcessId(index)) != 0xFFFFFFFFu)
            return index;
    }
}

/* 025BCF84 */
void ShopItems_c::setItemSetDataList() {
    u32 p = ea(this);
    s16 index = static_cast<s16>(mItemSetListGlobalIdx);
    mpItemSetList = shopTableValue(Item_set_data_tbl, static_cast<u32>(static_cast<s32>(index)));
}

/* 025BCFA0 */
void ShopItems_c::setItemSetDataListExplicit(u32 list) { mpItemSetList = list; }

/* 025BCF44 */
u32 ShopItems_c::isSoldOutItemAll() {
    u32 p = ea(this);
    s16 count = static_cast<s16>(mNumItems);
    for (s16 i = 0; i < count; ++i)
        if (static_cast<u8>(itemSoldOutFlag(i)) != 1)
            return 0;
    return 1;
}

/* 025BB698 */
u32 dShop_now_triggercheck(void* stick, void* shop, void* message, u32 callback, void* arg) {
    u32 m = load<u32>(0x101F4B5C), state = shopMessage_getStatus(at<void>(m));
    if (static_cast<u8>(at<ShopMessageHD>(m)->mSendOn) ||
        (state != 7 && state != 14 && state != 15))
        return 0;
    u32 p = ea(shop);
    s16 original = static_cast<s16>(at<ShopItems_c>(p)->mSelectedItemIdx),
        data = static_cast<s16>(at<ShopItems_c>(p)->mItemSetListGlobalIdx);
    u32 list = static_cast<u32>(at<ShopItems_c>(p)->mpItemSetList);
    shop_checkTrigger(stick);
    s32 left = shop_nextSelectRegister(0, shop), right = shop_nextSelectRegister(1, shop);
    s32 up = shop_nextSelectRegister(2, shop), down = shop_nextSelectRegister(3, shop);
    s32 next = left;
    bool changed = false;
    if (original != left && (shop_checkLeftTrigger(stick) || shop_padLeftTrigger(0)))
        changed = true;
    else if (original != right && (shop_checkRightTrigger(stick) || shop_padRightTrigger(0))) {
        next = right;
        changed = true;
    }
    if (original != up && (shop_checkUpTrigger(stick) || shop_padUpTrigger(0))) {
        next = up;
        changed = true;
    } else if (original != down && (shop_checkDownTrigger(stick) || shop_padDownTrigger(0))) {
        next = down;
        changed = true;
    }
    if (!changed)
        return 0;
    u32 text;
    if (next < 0) {
        text = callback
                   ? call_ptr<u32>(callback, arg)
                   : shopTableValue(default_select_msg, static_cast<u32>(static_cast<s32>(data)));
        *static_cast<be<u32>*>(message) = text;
        at<ShopMessageHD>(m)->mDefaultMsgNo = text;
    } else {
        if (static_cast<u32>(at<ShopMessageHD>(m)->mDefaultMsgNo))
            return 0;
        u32 entry = load<u32>(list + (u32)next * 4);
        text = static_cast<u8>(at<ShopItems_c>(p)->itemSoldOutFlag(next)) == 1
                   ? static_cast<u32>(at<__shop_items_set_data>(entry)->m0C)
                   : static_cast<u32>(at<__shop_items_set_data>(entry)->mShowMsgNo);
        *static_cast<be<u32>*>(message) = text;
    }
    shopMessage_setStatus(at<void>(m), 15);
    at<ShopMessageHD>(m)->mSendOn = 1;
    shopMessage_messageSet(at<void>(m), static_cast<u32>(*static_cast<be<u32>*>(message)), 0);
    at<ShopItems_c>(p)->mSelectedItemIdx = (u32)next;
    shopSound_start(0x80E);
    return 1;
}

/* 025BB974 */
u32 dShop_maxCheck(s32 item, s32 unused) {
    if (item == dItemNo_BIRD_BAIT_5_e || item == dItemNo_HYOI_PEAR_e)
        return !dComIfGs_checkBaitItemEmpty();
    // HD: the town-flower case returns false directly.
    if (item == shop_item::dItemNo_TOWN_FLOWER_e)
        return 0;
    dSv_info_c* save;
    if ((u32)item - dItemNo_ARROW_10_e < 3) {
        save = dComIfGs_info();
        return static_cast<u8>(save->mSavedata.mPlayer.mItemRecord.mItemRecord2.mArrowNum) ==
               static_cast<u8>(save->mSavedata.mPlayer.mItemMax.mItemMax2.mArrowNum);
    }
    if ((u32)item - shop_item::dItemNo_BOMB_5_e < 4) {
        save = dComIfGs_info();
        return static_cast<u8>(save->mSavedata.mPlayer.mItemRecord.mItemRecord2.mBombNum) ==
               static_cast<u8>(save->mSavedata.mPlayer.mItemMax.mItemMax2.mBombNum);
    }
    return 0;
}

/* 025BBA1C */
u32 dShop_BoughtErrorStatus(void* self, s32 buy, s32 cost) {
    u32 p = ea(self);
    s16 index = static_cast<s16>(at<ShopItems_c>(p)->mSelectedItemIdx);
    __shop_items_set_data* entry = at<ShopItems_c>(p)->itemSetData(index);
    ShopItems_c__ItemData* data = at<ShopItems_c__ItemData>(static_cast<u32>(entry->mpItemData));
    u8 flags = static_cast<u8>(data->mBuyConditions);
    u32 item = static_cast<u32>(data->mItemNo), error = 0;
    if (flags & Shop_BuyCond_MUST_OWN_ITEM_e) {
        u32 prerequisite = static_cast<u32>(data->mMustOwnItemNo);
        if (prerequisite == shop_item::dItemNo_BOW_e) {
            if (dComIfGs_getItem(dInvSlot_BOW_e) == dItemNo_NONE_e)
                error = Shop_BuyCond_MUST_OWN_ITEM_e;
        } else if (!dComIfGs_checkGetItem(prerequisite & 255))
            error = Shop_BuyCond_MUST_OWN_ITEM_e;
    }
    if ((flags & Shop_BuyCond_MUST_NOT_OWN_ITEM_e) &&
        dComIfGs_checkGetItem(data->mustNotOwnItemByte()))
        error |= Shop_BuyCond_MUST_NOT_OWN_ITEM_e;
    if (flags & Shop_BuyCond_ANY_BOTTLE_e) {
        dSv_info_c* save = dComIfGs_info();
        u32 count = 0;
        // Four bottle inventory slots are 14..17. Generic inlined item lookup's
        // other slot ranges cannot be selected by this bounded loop.
        for (u32 i = 0; i < 4; ++i)
            if (static_cast<u8>(save->mSavedata.mPlayer.mPlayerItem.mItems[14 + i]) !=
                dItemNo_NONE_e)
                ++count;
        if (!count)
            error |= Shop_BuyCond_ANY_BOTTLE_e;
    }
    if ((flags & Shop_BuyCond_EMPTY_BOTTLE_e) && !dComIfGs_checkEmptyBottle())
        error |= Shop_BuyCond_EMPTY_BOTTLE_e;
    if ((flags & Shop_BuyCond_MAXIMUM_e) && shop_maxCheckRegister((s32)item, 0))
        error |= Shop_BuyCond_MAXIMUM_e;
    if (flags & Shop_BuyCond_RUPEES_e) {
        u32 rupees = dComIfGs_getRupee();
        if (cost == -1)
            cost = 0;
        if ((s32)rupees < cost)
            error |= Shop_BuyCond_RUPEES_e;
        else if (buy == 1 && !error) {
            u32 play = dComIfGp_ea();
            at<dComIfG_inf_c>(play)->play.mItemRupeeCount =
                static_cast<u32>(at<dComIfG_inf_c>(play)->play.mItemRupeeCount) - (u32)cost;
            execItemGet(item & 255);
            return error;
        }
    }
    if (buy == 1 && !error)
        execItemGet(item & 255);
    return error;
}

/* 025BBC54 */
void* ShopCursor_c_ct(void* self, void* modelData, void* animation, f32 scale) {
    // HD: no GC material Z-mode loop; initialize the larger embedded brk animation.
    u32 p = ea(self);
    if (!p)
        p = ea(operator_new(0xB8));
    if (!p)
        return nullptr;
    mDoExt_brkAnm::ct(&at<ShopCursor_c>(p)->mBrkAnm);
    for (u32 i = 0; i < 4; ++i)
        at<ShopCursor_c>(p)->mpModels[i] =
            ea(mDoExt_J3DModel__create(static_cast<J3DModelData*>(modelData), 0, 0x11020203));
    at<ShopCursor_c>(p)->mpBrkData = ea(animation);
    f32 one = cursorScaleOne;
    if (!shop_brkInitHD(&at<ShopCursor_c>(p)->mBrkAnm, modelData, animation, 1, 2, 0, -1, 0, 0,
                        one))
        at<ShopCursor_c>(p)->mpBrkData = 0;
    at<ShopCursor_c>(p)->m48 = scale;
    at<ShopCursor_c>(p)->m54 = 0;
    f32 upper = cursorUpper, lower = cursorLower;
    at<ShopCursor_c>(p)->m38 = upper;
    at<ShopCursor_c>(p)->m3C = lower;
    at<ShopCursor_c>(p)->m40 = upper;
    at<ShopCursor_c>(p)->m4C = one;
    at<ShopCursor_c>(p)->m44 = 15;
    at<ShopCursor_c>(p)->m50 = one;
    return at<void>(p);
}

/* 025BD290 */
void ShopCursor_c::anm_play() {
    u32 p = ea(this);
    mBrkAnm.play();
    s16 countdown = static_cast<s16>(m44);
    m44 = (u16)(countdown - 1);
    if (countdown > 0)
        return;
    s32 delay = ftoi(cM_rndF(cursorDelayRange));
    f32 upper = static_cast<f32>(m38), lower = static_cast<f32>(m3C),
        current = static_cast<f32>(m40);
    f32 compare = fmsubs(fadds_ppc(upper, lower), shopHalf, current);
    m44 = (u32)delay + 15;
    m40 = compare >= 0 ? upper : lower;
}

/* 025BD338 */
void ShopCursor_c::draw() {
    u32 p = ea(this),
        camera = static_cast<u32>(at<dComIfG_inf_c>(dComIfGp_ea())->play.mCameraInfo[0].mpCamera);
    s32 yaw = shop_targetAngleY(&at<ShopCameraViewHD>(camera)->mCenter,
                                &at<ShopCameraViewHD>(camera)->mEye);
    s16 pitch = (s16)(0u - (u32)shop_targetAngleX(&at<ShopCameraViewHD>(camera)->mCenter,
                                                  &at<ShopCameraViewHD>(camera)->mEye));
    if (!static_cast<u8>(m54))
        return;
    f32 mid = fmsubs(fadds_ppc(static_cast<f32>(m38), static_cast<f32>(m3C)), shopHalf,
                     static_cast<f32>(m40));
    f32 scaleY = mid >= 0 ? static_cast<f32>(m50) : static_cast<f32>(m4C);
    store<u32>(0x104B4634, load<u32>(dComIfGp_ea() + 0x5D84));
    u32 second = load<u32>(dComIfGp_ea() + 0x5D88);
    f32 zero = shopZero, sqrtTwo = cursorSqrtTwo;
    store<u32>(0x104B4638, second);
    for (u32 i = 0; i < 4; ++i) {
        PSMTXTrans(mDoMtx_stack_c::get(), static_cast<f32>(mPos.x), static_cast<f32>(mPos.y),
                   static_cast<f32>(mPos.z));
        shop_XYZrotM(mDoMtx_stack_c::get(), (s32)pitch, yaw, 0);
        mDoMtx_stack_c::ZrotM(angle_z[i]);
        mDoMtx_stack_c::transM(zero, fmuls_ppc(static_cast<f32>(m40), sqrtTwo), zero);
        f32 scale = static_cast<f32>(m48);
        mDoMtx_stack_c::scaleM(scale, fmuls_ppc(scale, scaleY), scale);
        f32 matrix[12];
        for (u32 j = 0; j < 12; ++j)
            matrix[j] = mDoMtx_stack_c::get()->m[j / 4][j % 4];
        u32 model = static_cast<u32>(at<ShopCursor_c>(p)->mpModels[i]);
        for (u32 j = 0; j < 12; ++j)
            at<J3DModel>(model)->mBaseTransformMtx.m[j / 4][j % 4] = matrix[j];
    }
    u32 model = static_cast<u32>(at<ShopCursor_c>(p)->mpModels[0]);
    mBrkAnm.entry(at<J3DModel>(model)->getModelData(), mBrkAnm.getFrame());
    for (u32 i = 0; i < 4; ++i)
        mDoExt_modelUpdateDL(at<J3DModel>(static_cast<u32>(at<ShopCursor_c>(p)->mpModels[i])), 0);
    dComIfGd_setList();
}

/* 025BD31C */
void ShopCursor_c::setPos(void* pos) { copyVector(ea(this), ea(pos)); }

/* 025BBD7C */
void* ShopCursor_create(void* modelData, void* animation, f32 scale) {
    // HD: return null safely when construction fails.
    u32 p = shop_cursorConstruct((void*)nullptr, modelData, animation, scale);
    if (!p)
        return nullptr;
    for (u32 i = 0; i < 4; ++i)
        if (!static_cast<u32>(at<ShopCursor_c>(p)->mpModels[i]))
            return nullptr;
    if (!static_cast<u32>(at<ShopCursor_c>(p)->mpBrkData))
        return nullptr;
    return at<void>(p);
}

// HD: compiler-generated and template code of this translation unit.

/* 025BBDF0 */
void* ShopCam_action_c_ct(void* self) {
    u32 p = ea(self);
    if (!p)
        p = ea(operator_new(0x50));
    if (!p)
        return nullptr;
    bzero(&at<ShopCam_action_c>(p)->mCurrActionFunc, u32(8));
    bzero(&at<ShopCam_action_c>(p)->mPreviousAction, u32(8));
    f32 zero = shopZero, back = cameraBack, fovy = cameraFovy;
    at<ShopCam_action_c>(p)->m18.y = zero;
    at<ShopCam_action_c>(p)->m54 = 0xFFFF;
    at<ShopCam_action_c>(p)->m24.y = zero;
    at<ShopCam_action_c>(p)->mCamDataIdx = 0;
    at<ShopCam_action_c>(p)->m52 = 0;
    at<ShopCam_action_c>(p)->m50 = 0;
    at<ShopCam_action_c>(p)->m24.z = back;
    at<ShopCam_action_c>(p)->m18.x = zero;
    at<ShopCam_action_c>(p)->m30 = fovy;
    at<ShopCam_action_c>(p)->m18.z = zero;
    at<ShopCam_action_c>(p)->mOrigFovy = zero;
    at<ShopCam_action_c>(p)->m24.x = zero;
    return at<void>(p);
}

/* 025BC760 */
void* ShopItems_c_ct(void* self) {
    u32 p = ea(self);
    if (!p)
        p = ea(operator_new(0x44));
    if (!p)
        return nullptr;
    at<ShopItems_c>(p)->mSelectedItemIdx = 0;
    bzero(&at<ShopItems_c>(p)->mItemActorProcessIds[0], u32(0x20));
    at<ShopItems_c>(p)->mpItemSetList = 0;
    bzero(&at<ShopItems_c>(p)->mItemIsSoldOut[0], u32(8));
    at<ShopItems_c>(p)->m3C = 0;
    at<ShopItems_c>(p)->mbIsHide = 0;
    at<ShopItems_c>(p)->mSelectedItemIdx = 0xFFFF;
    at<ShopItems_c>(p)->mItemSetListGlobalIdx = 0;
    at<ShopItems_c>(p)->mNumItems = 0;
    for (u32 i = 0; i < 8; ++i) {
        at<ShopItems_c>(p)->itemActorProcessId(i) = 0xFFFFFFFFu;
        at<ShopItems_c>(p)->itemSoldOutFlag(i) = 0;
    }
    return at<void>(p);
}

/* 025BD56C */
void __sinit_d_shop_cpp() {
    shopHeaderStatics.zero[2] = 0;
    shopHeaderStatics.zero[0] = 0;
    shopHeaderStatics.zero[3] = 0;
    shopHeaderStatics.zero[1] = 0;
    shop_registerDestructor(&shopDestructorRecords[0]);
    shopHeaderStatics.negativePi = negativePi;
    shopHeaderStatics.positivePi = positivePi;
    shop_initializeByte(&shopHeaderStatics.firstObject);
    shop_registerDestructor(&shopDestructorRecords[3]);
    shop_initializeFlag(&shopHeaderStatics.secondObject);
    shop_registerDestructor(&shopDestructorRecords[6]);
}

/* WWHD glue
data shopHeaderStatics 1047C980
data shopDestructorRecords 101EB818
ext 025E8154 u32 shop_brkInitHD(void*, void*, void*, s32, s32, s32, s32, s32, s32, f32)
ext 025F19F8 void shop_XYZrotM(void*, s32, s32, s32)
ext 0200F974 s32 shop_targetAngleX(void*, void*)
ext 0200F93C s32 shop_targetAngleY(void*, void*)
ext 025D87E4 u32 shop_createShopItem(void*, u32, void*, s32, s32, s32)
ext 025B8B94 u32 shop_isEventBit(void*, s32)
ext 02007790 u32 shop_padDownTrigger(s32)
ext 02007764 u32 shop_padUpTrigger(s32)
ext 020077E8 u32 shop_padRightTrigger(s32)
ext 020077BC u32 shop_padLeftTrigger(s32)
ext 02588B5C u32 shop_checkDownTrigger(void*)
ext 02588AEC u32 shop_checkUpTrigger(void*)
ext 02588A7C u32 shop_checkRightTrigger(void*)
ext 02588A0C u32 shop_checkLeftTrigger(void*)
ext 025BB5FC s32 shop_nextSelectRegister(s32, void*)
ext 0258873C void shop_checkTrigger(void*)
ext 028F4384 f64 shop_sqrt(f64)
ext 028E8DD0 f64 shop_squareMagnitude(cXyz*)
ext 028E8DAC void shop_PSVECSubtract(cXyz*, cXyz*, cXyz*)
ext 0200F378 void shop_addCalcAngleS(void*, s32, s32, s32, s32)
ext 025BBC54 u32 shop_cursorConstruct(void*, void*, void*, f32)
ext 025BB974 u32 shop_maxCheckRegister(s32, s32)
ext 0201AD78 u32 shop_positionAdd(void*, void*, cXyz*)
ext 028EAB2C void shop_initializeFlag(void*)
ext 028ED6F8 void shop_initializeByte(void*)
ext 028F026C void shop_registerDestructor(void*)
ext 025F795C u32 shopMessage_getStatus(void*)
ext 025F74D0 void shopMessage_setStatus(void*, u32)
ext 025F7DB0 void shopMessage_messageSet(void*, u32, u32)
ext 025E1988 void shopSound_start(u32)
data Item_set_data_tbl 101EB674
data select_list_data_tbl 101EB7B8
data shop_cam_data_tbl 101EB7DC
data default_select_msg 101EB4AC
data Item_set_pos_data_tbl 101EB974
data angle_z 101EB810
data cursorScaleOne 100545C4
data cursorUpper 100545C8
data cursorLower 100545CC
data shopZero 100545D0
data cameraBack 100545D4
data cameraFovy 100545D8
data cameraFraction 100545DC
data cameraMaxStep 100545E0
data cursorDelayRange 100545E4
data normalCameraX 100545E8
data normalCameraY 100545EC
data normalCenterZ 100545F0
data normalEyeZ 100545F4
data itemSpacing 100545F8
data shopHalf 10054604
data cursorSqrtTwo 10054608
data negativePi 10054610
data positivePi 10054614
data zoomSpeed 100545FC
data waitSpeed 10054600
ext 021842B8 void daItemBase_c::hide()
ext 021842C8 void daItemBase_c::show()
ext 02483FAC csXyz* daShopItem_c::getRotateP()
ext 02483FB4 cXyz* daShopItem_c::getPosP()
ext 02483FBC void shopItem_getCenter(daShopItem_c*, cXyz*)
*/
