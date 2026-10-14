/* HD-only menu item part. State names follow the binary's StateID strings;
 * other names describe observed operations. */
#include "d/hd_screen_menu_item.h"
#include "d/hd_screen_btn_item_icon.h"
#include "d/d_save.h"
#include "d/hd_save_holders.h"
#include "d/hd_input_eventmgr.h"
#include "d/d_meter_01.h"
static constexpr u32 ITEM_PROCESS_NAME_JUDGE = 0x025E121C; // fpcSch_JudgeForPName
static hdPartsVtbl_c sMenuVtable;
static u32 sMenuHolderVtable;
static u32 sMenuSecondaryVtable;
static hdPartsStateId_c sMenuOffState;
static hdPartsStateId_c sPartsDisplayState;
static hdPartsStateId_c sDragState;
static hdPartsStateId_c sKeyState;
static hdPartsStateId_c sCursorState;
static hdPartsStateId_c sMainDecideState;
static hdPartsStateId_c sMainFloatState;
static hdPartsStateId_c sMainFloatCancelState;
static MeterUpdateSystem* sItemSystem;
static u32 hdMenuItem_isVariantItem(u32);
u32 hdItem_findReceiverChild(hdParts_c*, u32);
Pair32 hdItem_itemPosition(hdParts_c*, u32);
static hdPartsStateId_c sMainBlankState;
static hdPartsStateId_c sMainButtonInState;
static hdPartsStateId_c sMainButtonOutState;
static hdPartsStateId_c sMainSelectTouchState;
static hdPartsStateId_c sMainSelectCursorState;
static hdPartsStateId_c sMainUnselectState;
static hdPartsStateId_c sMainOnTouchState;
void hdItemButton_markTouchRelease();
u32 hdItemButton_canDispatch(u32);
void hdUiReceiverList_pushBack(u32 list, u32 node);
static hdUiEvent* hdItem_queuedEvent(cking::ui::input::EventMgr* manager) { return &((hdItemQueuedEvent_l*)manager)->mEvent; }
hdUiEvent* hdItem_copyEvent(hdUiEvent*, hdUiEvent*);
u32 hdItemButton_inputEnabled();
static void hdMenuItem_playDecideSound();
static void hdMenuItem_playSetSound();
static void hdMenuItem_playDragReleaseSound();
static u32 hdMenuItem_isEquipped(hdMenuItem_c*, u32);
void hdItem_movePoint(hdItemPoint_c*, hdItemPoint_c*, f32);
void hdItem_playDragSe(const char*, f32, f32);
static hdItemIcon_c* sMenuIcon;
static hdUiSafeString sMainLayoutName;
static hdMenuPaneNames_c sMainPaneNames;
u32 hdItem_collectPanes(void*, hdItemPaneArray_c*, hdUiSafeString*);
u32 hdItem_collectPictures(void*, hdItemPaneArray_c*, hdUiSafeString*);
u32 hdItem_collectTexts(void*, hdItemPaneArray_c*, hdUiSafeString*);
void hdItemAnim_resetSlot(hdAnimSet*, u32, u32);
static hdItemPoint_c sTouchIconOffset;
void hdItemPane_getPosition(hdItemPosition3_c*, hdPartsPaneSRT_c*);
// The event-manager header and d_save_readable.h pull incompatible item-data declarations.
// Reuse the public save layout and expose only the HD holder extension needed here.

static hdItemSaveGlobal_l* sMenuSave;
static u32 sMenuEquipmentFlags;
static cking::ui::input::EventMgr* sMenuEventManager;

void hdItem_playSe(const char* name);

static hdItemCountModes_c sCountModes;
static gfn sCountTextVtable;
static hdUiStringTable sItemSafeStringVtable;
void hdItemMessage_setLabel(HdMsgText*, hdUiSafeString*, hdUiSafeString*);

static u16 sCountFormat;
void hdItem_formatCount(HdMsgString*, be<u16>*, u32);



f32 hdItem_splitFraction(be<f32>* integerPart, f32 value);
u32 hdItemSave_getTimer(dSv_player_item_record_c* record);
void hdItemAnim_clearSlot(hdAnimSet* animation, u32 slot);

static hdItemMappingTable_c sItemMappings;
void hdItemFader_update(hdItemFader_l* fader);
void hdItemPane_setAlpha(hdPartsPaneSRT_c* pane, u32 alpha);
void hdItemPane_getGlobalPosition(hdItemPosition3_c* out, hdPartsPaneSRT_c* pane);


static void* sMainHeapManager;
void hdItemFader_construct(hdItemFader_l* fader, f32 value);
void hdItemFader_set(hdItemFader_l* fader, u32 duration, f32 target);
void hdItemAnim_attach(hdAnimSet* set, u32 entry, u32 slot);


static hdMenuAnimationNames_c sMenuAnimations;
static hdMenuGroupNames_c sMenuGroups;
static hdMenuGroupRemap_c sMenuGroupRemap;
static hdScreenUiSystem* sMenuUiSystem;
static cking::ui::input::InputManager* sMenuInput;
u32 hdItem_atanIndex(f32);
void hdItemAnimation_bind(hdAnimSet* set, u32 entry, hdUiSafeString* animation, hdUiSafeString* group);

/* 0262F6E4 */
static s32 hdMenuItem_stateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 0262F7E4 */
void hdMenuItem_c::destroy(hdMenuItem_c* self, s32 flags) {
    if (!self) return;
    self->mButton.mStateHolder.mHolderVtable = &sMenuHolderVtable;
    self->mButton.mSecondaryVtable = &sMenuSecondaryVtable;
    hdBasicButton_c::destroy(self, 0);
    if (flags & 1) operator_delete(self);
}
/* 0262F850 */
static u32 hdMenuItem_isPairedVariant(u32 item) {
    return (item >= 0x43 && item <= 0x44) || (item >= 0x46 && item <= 0x47);
}
/* 0262F880 */
static u32 hdMenuItem_itemForSlot(hdMenuItem_c*, u32 slot) {
    for (u32 i = 0; i < HD_ITEM_MAPPING_COUNT; ++i) if (sItemMappings.mEntries[i].mSlot == slot) return sItemMappings.mEntries[i].mItem;
    return HD_ITEM_NO_ITEM;
}
/* 0262F8B0 */
void hdMenuItem_c::updateArrowVariants(u32 slot) {
    bool magic = slot == 0x35;
    bool light = slot == 0x36;
    hdPartsPaneSRT_c* pane = mPaneGroups[1].at(0);
    pane->mFlags = (pane->mFlags & ~u32(HD_ITEM_PANE_VISIBLE)) | (magic || light);
    pane = mPaneGroups[1].at(1);
    pane->mFlags = (pane->mFlags & ~u32(HD_ITEM_PANE_VISIBLE)) | (magic || light);
    pane = mPaneGroups[1].at(2);
    pane->mFlags = (pane->mFlags & ~u32(HD_ITEM_PANE_VISIBLE)) | light;
    if (magic || light) {
        hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_ARROW_FIRE, HD_ITEM_SLOT_ARROW_FIRE, 1.0f);
        hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_ARROW_ICE, HD_ITEM_SLOT_ARROW_ICE, 1.0f);
        if (light) hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_ARROW_LIGHT, HD_ITEM_SLOT_ARROW_LIGHT, 1.0f);
        else hdItemAnim_clearSlot(mButton.animSet(), HD_ITEM_SLOT_ARROW_LIGHT);
    } else {
        hdItemAnim_clearSlot(mButton.animSet(), HD_ITEM_SLOT_ARROW_FIRE);
        hdItemAnim_clearSlot(mButton.animSet(), HD_ITEM_SLOT_ARROW_ICE);
        hdItemAnim_clearSlot(mButton.animSet(), HD_ITEM_SLOT_ARROW_LIGHT);
    }
}
// GHS converts nonnegative timer values to u32 in two signed conversion ranges.
static u32 hdItem_timerInteger(f32 value) {
    if (!(value < 2147483648.0f)) return u32(gabi::ftoi(f32(value - 2147483648.0f))) + 0x80000000u;
    return u32(gabi::ftoi(value));
}
/* 0262FB24 */
void hdMenuItem_c::drawTimer() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::FrameLocal<be<f32>> minutes(0x10);
    *minutes = 0.0f;
    hdItem_splitFraction(minutes, f32(mLimit) / 60.0f);
    u32 seconds = hdItem_timerInteger(mLimit) % 60;
    hdTextPane_c* pane = (hdTextPane_c*)mPaneGroups[2].at(1);
    // The shared message model describes the same embedded conversion workspace.
    HdMsgText* message = (HdMsgText*)&pane->mWork;
    gabi::FrameLocal<hdUiSafeString> set(0x14);
    gabi::FrameLocal<hdUiSafeString> label(0x1C);
    set->mVtable = &sItemSafeStringVtable;
    label->mVtable = &sItemSafeStringVtable;
    set->mText.v = gabi::guest_str("MenuItem_00");
    label->mText.v = gabi::guest_str("L_BtnItemIcon_00-T_Time_00");
    hdItemMessage_setLabel(message, set, label);
    message->mLineFlags[0] = hdItem_timerInteger(*minutes);
    message->mLineFlags[1] = seconds;
    ((hdTextPane_c*)mPaneGroups[2].at(1))->prepareMessage((PaneWork_l*)message, 1);
}
/* 0262FCD4 */
void hdMenuItem_c::updateTimer(u32 item) {
    if (item == HD_ITEM_TIMED_INVENTORY_ITEM) {
        gabi::AutoFrame automatic{gabi::Activation::current};
        f32 timer = f32(hdItemSave_getTimer(&sMenuSave->mInfo.mSavedata.mPlayer.mItemRecord));
        gabi::FrameLocal<be<f32>> seconds(8);
        *seconds = 0.0f;
        hdItem_splitFraction(seconds, timer / 30.0f);
        if (mLimit != *seconds) { mLimit = *seconds; drawTimer(); }
    }
}
/* 0262FDD4 */
u32 hdMenuItem_c::currentCount() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    dSv_player_c& player = sMenuSave->mInfo.mSavedata.mPlayer;
    if (mItem == 0x12) {
        u32 count = player.mItemRecord.mItemRecord2.mArrowNum;
        if (mFlag || (mDragFlag && (sMenuEquipmentFlags & 0x01000000))) {
            gabi::FrameLocal<be<u16>> process(8);
            *process = 0x76;
            hdItemProjectile_l* projectile = (hdItemProjectile_l*)fopAcIt_Judge(ITEM_PROCESS_NAME_JUDGE, process);
            if (projectile) { mFlag = 1; return u8(10 - projectile->mUsedCapacity); }
        }
        return count;
    }
    if (mItem == 0x15) return player.mItemRecord.mItemRecord2.mBombNum;
    if (mItem == 0xC || mItem == 0xD) {
        hdSaveHolders_c* holders = &sMenuSave->mHdSave;
        return ((dPictureStoreHD_c*)holders->getScratchPicture())->mSelectedPhotoSlot;
    }
    switch (u32(mItem)) {
    case 0x1C: return player.mBagItemRecord.mBeastNum[7];
    case 0x1D: return player.mBagItemRecord.mBeastNum[1];
    case 0x1E: return player.mBagItemRecord.mBeastNum[3];
    case 0x1F: return player.mBagItemRecord.mBeastNum[2];
    case 0x20: return player.mBagItemRecord.mBeastNum[0];
    case 0x21: return player.mBagItemRecord.mBeastNum[4];
    case 0x22: return player.mBagItemRecord.mBeastNum[5];
    case 0x23: return player.mBagItemRecord.mBeastNum[6];
    case 0x1B:
        // HD: the selection supplies an unchecked guest-byte index into the bait storage.
        return *gabi::at<be<u8>>(gabi::ea(&player.mBagItem.mBait[2]) + u32(mAnimationSlot));
    default: return 0;
    }
}
/* 0262FF78 */
u32 hdMenuItem_c::maximumCount() {
    dSv_player_c& player = sMenuSave->mInfo.mSavedata.mPlayer;
    if (mItem == 0x12) {
        u32 count = player.mItemMax.mItemMax2.mArrowNum;
        if (mFlag || (mDragFlag && (sMenuEquipmentFlags & 0x01000000))) return 10;
        return count;
    }
    if (mItem == 0x15) return player.mItemMax.mItemMax2.mBombNum;
    if (mItem == 0xC || mItem == 0xD) return 12;
    if (u32(mItem) - 0x1C <= 7) return 99;
    if (mItem == 0x1B) return 3;
    return 0;
}
/* 02630018 */
u32 hdMenuItem_c::updateCountPhase() {
    u32 hasCount = 0, phase = 2;
    if (mVisible) {
        u32 mode = *gabi::at<be<u32>>(gabi::ea(&sCountModes) + u32(mItem) * 4);
        if (mode == 1 || (mode == 2 && currentCount() != 0)) {
            hasCount = 1;
            phase = 0;
            u32 count = currentCount();
            if (count == maximumCount()) phase = 1;
        }
    }
    if (mPhase != phase) {
        mPhase = phase;
        hdAnimSet_setFrame(mButton.animSet(), HD_ITEM_ANIM_COUNT, HD_ITEM_SLOT_COUNT, f32(phase));
        mButton.setPaneEnabled(1);
    }
    return hasCount;
}
static bool hdItem_currentStateEquals(hdMenuItem_c* self, hdPartsStateId_c* state) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(self->mButton.stateMachine());
    hdPartsStateIdVtbl_c* comparisonVtable = state->mVtable;
    u32 currentNumber = current->getKey();
    return currentNumber == gabi::call_ptr<u32>(comparisonVtable->mGetKey.get(), state);
}
/* 0263286C */
void hdMenuItem_c::updateEquippedSelection() {
    bool equipped = hdMenuItem_isEquipped(this, mAnimationSlot);
    if (mMode != HD_ITEM_MODE_NORMAL) {
        if (equipped) return;
        mMode = HD_ITEM_MODE_NORMAL;
        if (mVisible) { hdBasicButton_c::decideEnd(); return; }
        hdItemAnim_resetSlot(mButton.animSet(), 3, HD_ITEM_SLOT_BUTTON);
        mButton.setPaneEnabled(1);
        return;
    }
    if ((hdItem_currentStateEquals(this, &sMenuOffState) || hdItem_currentStateEquals(this, &sCursorState)) && equipped) {
        mMode = HD_ITEM_MODE_EQUIPPED;
        hdItemAnim_resetSlot(mButton.animSet(), 2, HD_ITEM_SLOT_BUTTON);
        mButton.setPaneEnabled(1);
        mButton.changeState(&sMenuOffState);
    }
}
/* 026329E0 */
void hdMenuItem_c::updateItem() {
    if (updateCountPhase()) {
        u32 count = currentCount();
        if (mObservedValue != count) { mObservedValue = count; updateCountText(); }
    }
}

/* 0262FD70 */
void hdMenuItem_c::setTimerItem(u32 item) {
    mLimit = HD_ITEM_TIMER_UNINITIALIZED;
    hdPartsPaneSRT_c* pane = mPaneGroups[2].at(1);
    if (item == HD_ITEM_TIMED_INVENTORY_ITEM) {
        pane->mFlags = (pane->mFlags & ~u32(HD_ITEM_PANE_VISIBLE)) | 1;
        updateTimer(HD_ITEM_TIMED_INVENTORY_ITEM);
    } else pane->mFlags = pane->mFlags & ~u32(HD_ITEM_PANE_VISIBLE);
}
/* 02630110 */
void hdMenuItem_c::updateCountText() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::FrameLocal<hdItemCountText_c> text(8);
    text->mBuffer[0] = 0;
    text->mString.mpBuffer = (char*)&text->mBuffer[0];
    text->mString.mCapacity = 3;
    text->mString.mpVTable = &sCountTextVtable;
    text->mBuffer[2] = 0;
    hdItem_formatCount(&text->mString, &sCountFormat, mObservedValue);
    ((hdTextPane_c*)mPaneGroups[2].mpData.get()[0].get())->setStringPrepared((PaneString_l*)&text->mString, NULL);
}
/* 0263018C */
void hdMenuItem_c::refreshSelection(u32 animationSlot, u32 slot) {
    mAnimationSlot = animationSlot;
    mSlot = slot;
    u32 item = hdMenuItem_itemForSlot(this, slot);
    if (item == HD_ITEM_NO_ITEM) { vSetVisible(0); return; }
    vSetVisible(1);
    mItem = item;
    hdAnimSet_setFrame(mButton.animSet(), HD_ITEM_ANIM_ICON_CHANGE, HD_ITEM_SLOT_ICON_CHANGE, f32(item));
    mButton.setPaneEnabled(1);
    updateArrowVariants(slot);
    setTimerItem(slot);
    mPhase = HD_ITEM_COUNT_UNINITIALIZED;
    updateCountPhase();
    mObservedValue = currentCount();
    updateCountText();
}
/* 026302B8 */
void hdMenuItem_c::beginCursorSelection(hdItemPoint_c* point) {
    mPoints[1].x = f32(point->x) + f32(sTouchIconOffset.x);
    mPoints[1].y = f32(point->y) + f32(sTouchIconOffset.y);
    mPoints[0].x = f32(point->x) + f32(sTouchIconOffset.x);
    mPoints[0].y = f32(point->y) + f32(sTouchIconOffset.y);
    sMenuIcon->setSelection(mAnimationSlot, mSlot);
    sMenuIcon->showFromCursor(&mPoints[0]);
    gabi::call_ptr(menuVtable()->mTranslatePoint.get(), this, &mPoints[0]);
    if (mVisible && mPaneGroups[0].mpData.get()[0].get()) {
        hdPartsPaneSRT_c* pane = mPaneGroups[0].mpData.get()[0];
        pane->mFlags = pane->mFlags & ~u32(HD_ITEM_PANE_VISIBLE);
    }
    if (hdItemButton_inputEnabled() && mButton.mTouchEnabled) sMenuEventManager->setModeDefault(0);
    hdMenuItem_playDecideSound();
    mButton.changeState(&sMainFloatState);
}
/* 02630414 */
void hdMenuItem_c::offExecute() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!sMenuEventManager) return;
    gabi::FrameLocal<hdUiEvent> event(8);
    hdItem_copyEvent(event, hdItem_queuedEvent(sMenuEventManager));
    u32 id = mButton.getReceiverId();
    if (u32(event->mId) == id) {
        if (gabi::call_ptr<u32>(menuVtable()->mCanEnterTouch.get(), this, u32(event->mKind))) {
            if (hdItemButton_inputEnabled() && mButton.mTouchEnabled) mButton.clearFocus();
            gabi::call_ptr(menuVtable()->mSelectAction.get(), this);
            cking::ui::input::Receiver* receiver = (cking::ui::input::Receiver*)mButton.mpReceiver.get();
            if (gabi::call_ptr<u32>(receiver->mpVtable->mCanDrag.get(), receiver)) mButton.changeState(&sMainSelectTouchState);
        } else if (event->mKind == 3) {
            cking::ui::input::Receiver* receiver = (cking::ui::input::Receiver*)mButton.mpReceiver.get();
            if (!gabi::call_ptr<u32>(receiver->mpVtable->mCanDrag.get(), receiver)) return;
            if (hdItemButton_inputEnabled() && mButton.mTouchEnabled) mButton.clearFocus();
            gabi::call_ptr(menuVtable()->mSelectAction.get(), this);
            mButton.changeState(&sMainSelectTouchState);
        } else if (hdMenuItem_isPairedVariant(event->mKind)) {
            beginCursorSelection((hdItemPoint_c*)&sMenuEventManager->mTouchCurrent);
        }
    } else if (hdItemButton_inputEnabled() && mButton.mTouchEnabled && sMenuEventManager->getMode() == gabi::ea(mButton.mpReceiver.get())) {
        mButton.handleTouchRelease();
        mButton.changeState(&sMainSelectCursorState);
    }
}
/* 026306D8 */
void hdMenuItem_c::onTouchExecute() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!sMenuEventManager) return;
    gabi::FrameLocal<hdUiEvent> event(8);
    hdItem_copyEvent(event, hdItem_queuedEvent(sMenuEventManager));
    u32 id = mButton.getReceiverId();
    if (u32(event->mId) == id) {
        if (gabi::call_ptr<u32>(menuVtable()->mShouldUnselectTouch.get(), this, u32(event->mKind))) mButton.changeState(&sMainUnselectState);
        else if (hdMenuItem_isPairedVariant(event->mKind)) beginCursorSelection((hdItemPoint_c*)&sMenuEventManager->mTouchCurrent);
    } else if (event->mId == -1) {
        if (hdItemButton_inputEnabled() && mButton.mTouchEnabled && sMenuEventManager->getMode() == gabi::ea(mButton.mpReceiver.get()) &&
            hdItemButton_canDispatch(event->mKind) && gabi::call_ptr<u32>(menuVtable()->mCanLeaveCursor.get(), this, u32(event->mKind))) {
            hdItemButton_markTouchRelease();
            mButton.changeState(&sMainUnselectState);
        }
    } else if (event->mId == 0) mButton.changeState(&sMainUnselectState);
}
/* 02630850 */
void hdMenuItem_c::onCursorExecute() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!sMenuEventManager) return;
    gabi::FrameLocal<hdUiEvent> event(8);
    hdItem_copyEvent(event, hdItem_queuedEvent(sMenuEventManager));
    u32 id = mButton.getReceiverId();
    if (u32(event->mId) == id) {
        if (gabi::call_ptr<u32>(menuVtable()->mCanEnterTouch.get(), this, u32(event->mKind))) {
            cking::ui::input::Receiver* receiver = (cking::ui::input::Receiver*)mButton.mpReceiver.get();
            if (gabi::call_ptr<u32>(receiver->mpVtable->mCanDrag.get(), receiver)) {
                gabi::call_ptr(menuVtable()->mSelectAction.get(), this);
                mButton.changeState(&sMainSelectTouchState);
            }
        } else if (gabi::call_ptr<u32>(menuVtable()->mShouldUnselectTouch.get(), this, u32(event->mKind))) mButton.changeState(&sMainUnselectState);
        else if (hdMenuItem_isPairedVariant(event->mKind)) beginCursorSelection((hdItemPoint_c*)&sMenuEventManager->mTouchCurrent);
    } else if (event->mId == -1) {
        if (hdItemButton_inputEnabled() && mButton.mTouchEnabled && sMenuEventManager->getMode() == gabi::ea(mButton.mpReceiver.get()) &&
            hdItemButton_canDispatch(event->mKind) && gabi::call_ptr<u32>(menuVtable()->mCanLeaveCursor.get(), this, u32(event->mKind))) {
            hdItemButton_markTouchRelease();
            mButton.changeState(&sMainUnselectState);
        }
    } else {
        if (hdItemButton_inputEnabled() && mButton.mTouchEnabled && sMenuEventManager->getMode() == gabi::ea(mButton.mpReceiver.get())) return;
        mButton.changeState(&sMainUnselectState);
    }
}
/* 02630620 */
void hdMenuItem_c::prepareTouchSelection() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::FrameLocal<hdItemPosition3_c> position(0x10);
    hdItemPane_getPosition(position, (hdPartsPaneSRT_c*)mButton.mpLayout->lyt()->mpRootPane.get());
    gabi::FrameLocal<hdItemPoint_c> target(8);
    target->x = f32(position->x) + f32(sTouchIconOffset.x);
    target->y = f32(position->y) + f32(sTouchIconOffset.y);
    sMenuIcon->setSelection(mAnimationSlot, mSlot);
    sMenuIcon->showFromTouch(target);
    if (mVisible && mPaneGroups[0].mpData.get()[0].get()) {
        hdPartsPaneSRT_c* pane = mPaneGroups[0].mpData.get()[0];
        pane->mFlags = pane->mFlags & ~u32(HD_ITEM_PANE_VISIBLE);
    }
}
/* 026302A0 */
void hdMenuItem_c::setSelection(u32 animationSlot, u32 slot) {
    mOtherSlot = animationSlot;
    mPreviousSlot = slot;
    refreshSelection( animationSlot, slot);
}
/* 026302AC */
static void hdMenuItem_playDecideSound() { hdItem_playSe("SE_MENU_DECIDE"); }
/* 026313B4 */
void hdMenuItem_c::finishDragSelection() {
    // The original reuses each FP load for two stores, retaining its NaN conversion.
    f32 x = mPanePosition.x, y = mPanePosition.y;
    mPoints[1].x = x;
    mPoints[1].y = y;
    mPoints[0].x = x;
    mPoints[0].y = y;
    sMenuIcon->positionAtPoint(&mPoints[0]);
    sMenuIcon->hideAfterCancel();
    gabi::call_ptr(menuVtable()->mTranslatePoint.get(), this, &mPoints[0]);
    if (mVisible && mPaneGroups[0].mpData.get()[0].get()) {
        hdPartsPaneSRT_c* pane = mPaneGroups[0].mpData.get()[0];
        pane->mFlags = (pane->mFlags & ~u32(HD_ITEM_PANE_VISIBLE)) + 1;
    }
    if (hdItemButton_inputEnabled() && mButton.mTouchEnabled) sMenuEventManager->setModeDefault(gabi::ea(mButton.mpReceiver.get()));
    hdMenuItem_playSetSound();
    mMode = HD_ITEM_MODE_TOUCH;
    mButton.changeState(&sMainDecideState);
}
/* 0263198C */
void hdMenuItem_c::finishFloatSelection() {
    // The original reuses each FP load for two stores, retaining its NaN conversion.
    f32 x = mPanePosition.x, y = mPanePosition.y;
    mPoints[1].x = x;
    mPoints[1].y = y;
    mPoints[0].x = x;
    mPoints[0].y = y;
    sMenuIcon->positionAtPoint(&mPoints[0]);
    sMenuIcon->hideAfterFloat();
    gabi::call_ptr(menuVtable()->mTranslatePoint.get(), this, &mPoints[0]);
    if (mVisible && mPaneGroups[0].mpData.get()[0].get()) {
        hdPartsPaneSRT_c* pane = mPaneGroups[0].mpData.get()[0];
        pane->mFlags = (pane->mFlags & ~u32(HD_ITEM_PANE_VISIBLE)) + 1;
    }
    if (hdItemButton_inputEnabled() && mButton.mTouchEnabled) sMenuEventManager->setModeDefault(gabi::ea(mButton.mpReceiver.get()));
    hdMenuItem_playDragReleaseSound();
    mButton.changeState(&sMainFloatCancelState);
}
/* 026314A8 */
void hdMenuItem_c::updateDragSound(hdItemPoint_c* delta) {
    f32 x = delta->x;
    f32 y = delta->y;
    f32 squared = gabi::fmadds(x, x, gabi::fmuls_ppc(y, y));
    f32 length;
    if (squared > 0.0f) {
        // Preserve PPC NaN operand selection and the estimate second-operand rounding.
        f64 estimate = gabi::frsqrte(squared);
        f32 estimateSquared = f32(estimate * round25(estimate));
        f32 halfEstimate = f32(estimate * 0.5f);
        f32 inverse = gabi::fmuls_ppc(gabi::fnmsubs(estimateSquared, squared, 3.0f), halfEstimate);
        length = gabi::fmuls_ppc(inverse, squared);
    } else length = gabi::fmuls_ppc(0.0f, squared);
    // HD's fsel rejects non-finite length after testing length - length.
    if (!std::isfinite(length)) length = 0.0f;
    f32 amount = gabi::fmuls_ppc(length, 0.01f);
    if (!(amount <= 1.0f)) amount = 1.0f;
    hdItemFader_set(&mInterpolator, 3, gabi::fmuls_ppc(amount, amount));
    f32 volume = gabi::fmuls_ppc(mInterpolator.mValue, 0.5f);
    hdItem_playDragSe("SE_MENU_ICON_DRAG", volume, f32(volume + 1.0f));
}
/* 026315AC */
void hdMenuItem_c::moveDragSelection(hdItemPoint_c* point) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    // The recompiled paired FP loads quiet signaling payloads before storing the previous point.
    f32 previousX = mPoints[0].x, previousY = mPoints[0].y;
    mPoints[1].x = previousX;
    mPoints[1].y = previousY;
    f32 x = point->x;
    f32 offsetX = sTouchIconOffset.x, offsetY = sTouchIconOffset.y;
    f32 nextX = x + offsetX;
    f32 y = point->y;
    f32 nextY = y + offsetY;
    mPoints[0].x = nextX;
    mPoints[0].y = nextY;
    sMenuIcon->positionAtPoint(&mPoints[0]);
    gabi::call_ptr(menuVtable()->mTranslatePoint.get(), this, &mPoints[0]);
    gabi::FrameLocal<hdItemPoint_c> delta(0x20);
    delta->x = f32(mPoints[0].x) - f32(mPoints[1].x);
    delta->y = f32(mPoints[0].y) - f32(mPoints[1].y);
    updateDragSound(delta);
}
/* 0263168C */
void hdMenuItem_c::changeItemByDragExecute() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    mButton.synchronizeFocus();
    gabi::FrameLocal<hdUiEvent> event(0x18);
    hdItem_copyEvent(event, hdItem_queuedEvent(sMenuEventManager));
    mButton.getReceiverId();
    if (mPoints[3].x == mPoints[2].x && mPoints[3].y == mPoints[2].y) {
        finishDragSelection();
        return;
    }
    hdItem_movePoint(&mPoints[3], &mPoints[2], mScale);
    gabi::FrameLocal<hdItemPoint_c> point(0x10);
    point->x = f32(mPoints[3].x) - f32(sTouchIconOffset.x);
    point->y = f32(mPoints[3].y) - f32(sTouchIconOffset.y);
    moveDragSelection(point);
}
/* 026318BC */
void hdMenuItem_c::beginDragSelection(hdItemPoint_c* from, hdItemPoint_c* to) {
    // Copies preserve the source payload; arithmetic consumes the converted FP values.
    u32 startXBits = from->xBits;
    mPoints[3].xBits = startXBits;
    f32 startX = gabi::f32_from_bits(startXBits);
    u32 startYBits = from->yBits;
    mPoints[3].yBits = startYBits;
    f32 startY = gabi::f32_from_bits(startYBits);
    // This single lfs/stfs copy folds before the subsequent FP reload.
    mPoints[2].xBits = to->xBits;
    f32 deltaX = f32(mPoints[2].x) - startX;
    f32 destinationY = to->y;
    f32 deltaY = destinationY - startY;
    f32 squared = gabi::fmadds(deltaX, deltaX, gabi::fmuls_ppc(deltaY, deltaY));
    mPoints[2].y = destinationY;
    f32 inverse = 0.0f;
    if (squared > 0.0f) {
        // Preserve PPC NaN operand selection and the estimate second-operand rounding.
        f64 estimate = gabi::frsqrte(squared);
        f32 estimateSquared = f32(estimate * round25(estimate));
        f32 halfEstimate = f32(estimate * 0.5f);
        inverse = gabi::fmuls_ppc(gabi::fnmsubs(estimateSquared, squared, 3.0f), halfEstimate);
    }
    mScale = gabi::fmuls_ppc(gabi::fmuls_ppc(inverse, squared), 0.5f);
    mButton.changeState(&sDragState);
}
/* 02632BBC */
void hdMenuItem_c::beginKeySelection(hdItemPoint_c* to) {
    if (!mVisible) return;
    if (mButton.animSet()->mEntries.at(13)->get()->mController->mPlaying) {
        restorePaneVisible();
        hdItemAnim_resetSlot(mButton.animSet(), HD_ITEM_ANIM_FLOAT_CANCEL, HD_ITEM_SLOT_FLOAT);
    }
    // Paired FP loads retain the original conversion before either store.
    f32 startX = mPanePosition.x, startY = mPanePosition.y;
    mPoints[3].x = startX;
    mPoints[3].y = startY;
    // This single lfs/stfs copy folds before the subsequent FP reload.
    mPoints[2].xBits = to->xBits;
    f32 deltaX = f32(mPoints[2].x) - startX;
    f32 destinationY = to->y;
    f32 deltaY = destinationY - startY;
    f32 squared = gabi::fmadds(deltaX, deltaX, gabi::fmuls_ppc(deltaY, deltaY));
    mPoints[2].y = destinationY;
    f32 inverse = 0.0f;
    if (squared > 0.0f) {
        // Preserve PPC NaN operand selection and the estimate second-operand rounding.
        f64 estimate = gabi::frsqrte(squared);
        f32 estimateSquared = f32(estimate * round25(estimate));
        f32 halfEstimate = f32(estimate * 0.5f);
        inverse = gabi::fmuls_ppc(gabi::fnmsubs(estimateSquared, squared, 3.0f), halfEstimate);
    }
    mScale = gabi::fmuls_ppc(gabi::fmuls_ppc(inverse, squared), 0.125f);
    hdMenuItem_playSetSound();
    mButton.changeState(&sKeyState);
}
/* 026313A8 */
static void hdMenuItem_playSetSound() { hdItem_playSe("SE_MENU_ITEM_SET"); }
/* 02631980 */
static void hdMenuItem_playDragReleaseSound() { hdItem_playSe("SE_MENU_ICON_DRAG_RELEASE"); }
/* 026306D4 */
void hdMenuItem_c::beginTouchSelection() { prepareTouchSelection(); }
/* 02630AC8 */
void hdMenuItem_c::selectTouchExecute() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!sMenuEventManager) return;
    if (mSelectPhase == 0) {
        gabi::FrameLocal<hdUiEvent> event(8);
        hdItem_copyEvent(event, hdItem_queuedEvent(sMenuEventManager));
        u32 id = mButton.getReceiverId();
        if (u32(event->mId) == id) {
            if (gabi::call_ptr<u32>(menuVtable()->mShouldUnselectTouch.get(), this, u32(event->mKind))) {
                mSelectPhase = 3;
                mButton.changeState(&sMainUnselectState);
                return;
            }
            if (hdMenuItem_isPairedVariant(event->mKind)) {
                beginCursorSelection((hdItemPoint_c*)&sMenuEventManager->mTouchCurrent);
                return;
            }
        } else if (event->mId == -1) {
            if (hdItemButton_inputEnabled() && mButton.mTouchEnabled && sMenuEventManager->getMode() == gabi::ea(mButton.mpReceiver.get()) &&
                hdItemButton_canDispatch(event->mKind) && gabi::call_ptr<u32>(menuVtable()->mCanLeaveCursor.get(), this, u32(event->mKind))) {
                hdItemButton_markTouchRelease();
                mSelectPhase = 3;
                mButton.changeState(&sMainUnselectState);
                return;
            }
        } else if (event->mId == 0) {
            mSelectPhase = 3;
            mButton.changeState(&sMainUnselectState);
            return;
        }
    }
    mButton.changeState(mSelectPhase == 3 ? &sMainUnselectState : &sMainOnTouchState);
}
/* 02630A64 */
void hdMenuItem_c::selectTouchEnter() {
    prepareTouchSelection();
    hdItemAnim_operationD4(mButton.animSet(), 1, HD_ITEM_SLOT_BUTTON);
    hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_FLOAT, HD_ITEM_SLOT_FLOAT, 1.0f);
    mSelectPhase = 0;
}
/* 02630CB8 */
void hdMenuItem_c::selectCursorEnter() {
    hdAnimSet_play(mButton.animSet(), 0, HD_ITEM_SLOT_BUTTON, 1.0f);
    mSelectPhase = 1;
}
/* 02630D04 */
void hdMenuItem_c::selectCursorExecute() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!sMenuEventManager) return;
    if (mSelectPhase == 1) {
        gabi::FrameLocal<hdUiEvent> event(8);
        hdItem_copyEvent(event, hdItem_queuedEvent(sMenuEventManager));
        u32 id = mButton.getReceiverId();
        if (u32(event->mId) == id) {
            if (gabi::call_ptr<u32>(menuVtable()->mCanEnterTouch.get(), this, u32(event->mKind))) {
                cking::ui::input::Receiver* receiver = (cking::ui::input::Receiver*)mButton.mpReceiver.get();
                if (gabi::call_ptr<u32>(receiver->mpVtable->mCanDrag.get(), receiver)) {
                    gabi::call_ptr(menuVtable()->mSelectAction.get(), this);
                    mSelectPhase = 0;
                }
            } else if (gabi::call_ptr<u32>(menuVtable()->mShouldUnselectTouch.get(), this, u32(event->mKind))) mSelectPhase = 3;
            else if (hdMenuItem_isPairedVariant(event->mKind)) {
                beginCursorSelection((hdItemPoint_c*)&sMenuEventManager->mTouchCurrent);
                return;
            }
        } else if (event->mId == -1) {
            if (hdItemButton_inputEnabled() && mButton.mTouchEnabled && sMenuEventManager->getMode() == gabi::ea(mButton.mpReceiver.get()) &&
                hdItemButton_canDispatch(event->mKind) && gabi::call_ptr<u32>(menuVtable()->mCanLeaveCursor.get(), this, u32(event->mKind))) {
                hdItemButton_markTouchRelease();
                mSelectPhase = 3;
            }
        } else {
            if (!(hdItemButton_inputEnabled() && mButton.mTouchEnabled && sMenuEventManager->getMode() == gabi::ea(mButton.mpReceiver.get()))) mSelectPhase = 3;
        }
    }
    if (hdAnimSet_isEnd(mButton.animSet(), 0)) {
        if (mSelectPhase == 0) mButton.changeState(&sMainSelectTouchState);
        else if (mSelectPhase == 3) mButton.changeState(&sMainUnselectState);
        else mButton.changeState(&sCursorState);
    }
}
/* 02630F60 */
void hdMenuItem_c::restorePaneVisible() {
    sMenuIcon->hideFromUnselect();
    if (mVisible) {
        hdPartsPaneSRT_c* pane = mPaneGroups[0].mpData.get()[0];
        if (pane) pane->mFlags = (pane->mFlags & ~u32(HD_ITEM_PANE_VISIBLE)) | 1;
    }
}
/* 02630FC4 */
void hdMenuItem_c::unselectEnter() {
    if (mButton.animSet()->mEntries.at(13)->get()->mController->mPlaying) {
        restorePaneVisible();
        hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_FLOAT_CANCEL, HD_ITEM_SLOT_FLOAT, 1.0f);
    } else hdAnimSet_play(mButton.animSet(), 1, HD_ITEM_SLOT_BUTTON, 1.0f);
}
/* 0263105C */
void hdMenuItem_c::unselectExecute() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::FrameLocal<hdUiEvent> event(8);
    hdItem_copyEvent(event, hdItem_queuedEvent(sMenuEventManager));
    u32 id = mButton.getReceiverId();
    if (u32(event->mId) == id && hdMenuItem_isPairedVariant(event->mKind)) {
        beginCursorSelection((hdItemPoint_c*)&sMenuEventManager->mTouchCurrent);
        return;
    }
    if (hdAnimSet_isEnd(mButton.animSet(), HD_ITEM_ANIM_FLOAT_CANCEL) && hdAnimSet_isEnd(mButton.animSet(), 1)) mButton.changeState(&sMenuOffState);
}
/* 0263112C */
void hdMenuItem_c::decideEnter() {
    mButton.mpReceiver->mEnabled = 0;
    mButton.synchronizeFocus();
    hdAnimSet* animation = mButton.animSet();
    if (mMode == HD_ITEM_MODE_TOUCH) {
        hdItemAnim_resetSlot(animation, 14, 4);
        hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_TOUCH_SLIDE_DECIDE, HD_ITEM_SLOT_BUTTON, 1.0f);
    } else hdAnimSet_play(animation, 2, 0, 1.0f);
}
/* 026311B8 */
void hdMenuItem_c::decideExecute() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (sMenuEventManager) {
        mButton.synchronizeFocus();
        gabi::FrameLocal<hdUiEvent> event(8);
        hdItem_copyEvent(event, hdItem_queuedEvent(sMenuEventManager));
        mButton.getReceiverId();
        if (hdAnimSet_isEnd(mButton.animSet(), HD_ITEM_ANIM_TOUCH_SLIDE_DECIDE) && hdAnimSet_isEnd(mButton.animSet(), 2)) mButton.changeState(&sMenuOffState);
    }
}
/* 02631A78 */
void hdMenuItem_c::floatExecute() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!sMenuEventManager) return;
    gabi::FrameLocal<hdUiEvent> event(0x10);
    hdItem_copyEvent(event, hdItem_queuedEvent(sMenuEventManager));
    u32 receiverId = mButton.getReceiverId();
    if (u32(event->mId) == receiverId) {
        if (hdMenuItem_isVariantItem(event->mKind)) {
            hdParts_c* hud = gabi::at<hdParts_c>(sItemSystem->mHudContext);
            hdParts_c* itemScreen = *hud->childSlot(0x2D);
            u32 hover = sMenuEventManager->getHover();
            if (hover) {
                cking::ui::input::Receiver* receiver = gabi::at<cking::ui::input::Receiver>(hover);
                u32 index = hdItem_findReceiverChild(hud, receiver->mId);
                Pair32 position{hdItem_itemPosition(itemScreen, index)};
                gabi::FrameLocal<hdItemPoint_c> destination(8);
                union FloatBits { u32 bits; f32 value; };
                FloatBits x{position.r3}, y{position.r4};
                destination->x = x.value;
                destination->y = y.value;
                beginDragSelection(&mPoints[0], destination);
                return;
            }
        } else {
            moveDragSelection((hdItemPoint_c*)&sMenuEventManager->mTouchCurrent);
            return;
        }
    }
    finishFloatSelection();
}
/* 02631BFC */
void hdMenuItem_c::floatCancelExecute() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::FrameLocal<hdUiEvent> event(8);
    hdItem_copyEvent(event, hdItem_queuedEvent(sMenuEventManager));
    u32 id = mButton.getReceiverId();
    if (u32(event->mId) == id && hdMenuItem_isPairedVariant(event->mKind)) {
        beginCursorSelection((hdItemPoint_c*)&sMenuEventManager->mTouchCurrent);
        return;
    }
    if (hdAnimSet_isEnd(mButton.animSet(), HD_ITEM_ANIM_FLOAT_CANCEL)) mButton.changeState(&sMenuOffState);
}
/* 026312C8 */
void hdMenuItem_c::changeItemByKeyExecute() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    mButton.synchronizeFocus();
    gabi::FrameLocal<hdUiEvent> event(8);
    hdItem_copyEvent(event, hdItem_queuedEvent(sMenuEventManager));
    mButton.getReceiverId();
    if (mPoints[3].x == mPoints[2].x && mPoints[3].y == mPoints[2].y) {
        mMode = HD_ITEM_MODE_EQUIPPED;
        mButton.changeState(&sMainDecideState);
        return;
    }
    hdItem_movePoint(&mPoints[3], &mPoints[2], mScale);
    sMenuIcon->showAtPoint(&mPoints[3]);
}
/* 026317F8 */
void hdMenuItem_c::arrowLoopEnter() {
    hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_ARROW_FIRE, HD_ITEM_SLOT_ARROW_FIRE, 1.0f);
    hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_ARROW_ICE, HD_ITEM_SLOT_ARROW_ICE, 1.0f);
    hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_ARROW_LIGHT, HD_ITEM_SLOT_ARROW_LIGHT, 1.0f);
}
/* 02631258 */
void hdMenuItem_c::decideExit() { mButton.mpReceiver->mEnabled = 1; }
/* 02631268 */
void hdMenuItem_c::changeItemByKeyEnter() {
    mButton.mpReceiver->mEnabled = 0;
    mButton.synchronizeFocus();
    sMenuIcon->setSelection(mAnimationSlot, mSlot);
    sMenuIcon->animateToPoint(&mPoints[3]);
}
/* 0263137C */
void hdMenuItem_c::changeItemByKeyExit() {
    u32 x = mPanePosition.xBits, y = mPanePosition.yBits;
    mPoints[3].xBits = x;
    mPoints[3].yBits = y;
    sMenuIcon->hideAfterKey();
}
/* 02631398 */
void hdMenuItem_c::changeItemByDragEnter() {
    mButton.mpReceiver->mEnabled = 0;
    mButton.synchronizeFocus();
}
/* 02631760 */
void hdMenuItem_c::changeItemByDragExit() {
    u32 x = mPanePosition.xBits, y = mPanePosition.yBits;
    mPoints[3].xBits = x;
    mPoints[3].yBits = y;
}
/* 02631774 */
void hdMenuItem_c::touchSlideDecideEnter() { hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_TOUCH_SLIDE_DECIDE, HD_ITEM_SLOT_TOUCH_DECIDE, 1.0f); }
/* 02631790 */
void hdMenuItem_c::touchSlideDecideExecute() { gabi::tail_ptr(mButton.mVtable->mCalcLayout.get(), this); }
/* 026317A0 */
void hdMenuItem_c::touchSlideDecideCmdEnter() { hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_COMMAND, HD_ITEM_SLOT_TOUCH_DECIDE, 1.0f); }
/* 026317BC */
void hdMenuItem_c::touchSlideDecideCmdExecute() { gabi::tail_ptr(mButton.mVtable->mCalcLayout.get(), this); }
/* 026317CC */
void hdMenuItem_c::iconChangeEnter() { hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_ICON_CHANGE, HD_ITEM_SLOT_ICON_CHANGE, 1.0f); }
/* 026317E8 */
void hdMenuItem_c::iconChangeExecute() { gabi::tail_ptr(mButton.mVtable->mCalcLayout.get(), this); }
/* 02631888 */
void hdMenuItem_c::arrowLoopExecute() { gabi::tail_ptr(mButton.mVtable->mCalcLayout.get(), this); }
/* 02631898 */
void hdMenuItem_c::floatEnter() { hdItemAnim_attach(mButton.animSet(), HD_ITEM_ANIM_FLOAT, HD_ITEM_SLOT_FLOAT); }
/* 026318AC */
static u32 hdMenuItem_isVariantItem(u32 item) { return item == 0x45; }
/* 02631BA4 */
void hdMenuItem_c::floatCancelEnter() {
    hdItemAnim_operationD4(mButton.animSet(), 1, HD_ITEM_SLOT_BUTTON);
    hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_FLOAT_CANCEL, HD_ITEM_SLOT_FLOAT, 1.0f);
}
/* 02631CB4 */
void hdMenuItem_c::numEnter() { hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_COUNT, HD_ITEM_SLOT_COUNT, 1.0f); }
/* 02631CD0 */
void hdMenuItem_c::numExecute() { gabi::tail_ptr(mButton.mVtable->mCalcLayout.get(), this); }
/* 02631CE0 */
void hdMenuItem_c::switchEnter() { hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_SWITCH, HD_ITEM_SLOT_TOUCH_DECIDE, 1.0f); }
/* 02631CFC */
void hdMenuItem_c::switchExecute() { gabi::tail_ptr(mButton.mVtable->mCalcLayout.get(), this); }
/* 02631D0C */
hdMenuItem_c* hdMenuItem_c::construct(hdMenuItem_c* self) {
    if (!self) self = (hdMenuItem_c*)operator_new(sizeof(hdMenuItem_c));
    if (!self) return self;
    hdPartsBasicButton_construct(&self->mButton);
    self->mButton.mVtable = &sMenuVtable;
    self->mButton.mStateHolder.mHolderVtable = &sMenuHolderVtable;
    self->mButton.mSecondaryVtable = &sMenuSecondaryVtable;
    // HD: the GHS embedded-object constructors retain their allocating NULL path.
    for (u32 i = 0; i < HD_ITEM_PANE_GROUP_COUNT; ++i) {
        hdItemPaneArray_c* group = gabi::at<hdItemPaneArray_c>(gabi::ea(self) + offsetof(hdMenuItem_c, mPaneGroups) + i * sizeof(hdItemPaneArray_c));
        if (!group) group = (hdItemPaneArray_c*)operator_new(sizeof(hdItemPaneArray_c));
        if (group) { group->mpData = NULL; group->mCount = 0; }
    }
    for (u32 i = 0; i < 6; ++i) {
        hdItemPoint_c* point = gabi::at<hdItemPoint_c>(gabi::ea(self) + offsetof(hdMenuItem_c, mPanePosition) + i * sizeof(hdItemPoint_c));
        if (!point) point = (hdItemPoint_c*)operator_new(sizeof(hdItemPoint_c));
        if (point) { point->y = 0.0f; point->x = 0.0f; }
    }
    self->mFlag = 0;
    self->mMode = HD_ITEM_MODE_NORMAL;
    self->mPhase = HD_ITEM_COUNT_UNINITIALIZED;
    self->mAnimationSlot = HD_ITEM_INVALID_SLOT;
    self->mObservedValue = HD_ITEM_UNKNOWN_COUNT;
    self->mVisible = 1;
    self->mOtherSlot = HD_ITEM_INVALID_SLOT;
    self->mItem = HD_ITEM_NO_ITEM;
    self->mDragFlag = 0;
    self->mPreviousSlot = HD_ITEM_INVALID_SLOT;
    self->mSlot = HD_ITEM_INVALID_SLOT;
    self->mLimit = HD_ITEM_TIMER_UNINITIALIZED;
    self->mScale = 1.0f;
    hdItemFader_construct(&self->mInterpolator, 1.0f);
    hdItemFader_set(&self->mInterpolator, 0, 0.0f);
    return self;
}

/* 02631F30 */
u32 hdMenuItem_c::preparePanes() {
    hdScreenHeap* heap = sMenuUiSystem->mHeap;
    const u32 counts[] = {1, 3, 2};
    for (u32 groupIndex = 0; groupIndex < HD_ITEM_PANE_GROUP_COUNT; ++groupIndex) {
        hdScreenHeap* allocationHeap = heap;
        if (!allocationHeap) allocationHeap = hdParts_getCurrentHeap(sMainHeapManager);
        void* storage = allocationHeap->allocate(counts[groupIndex] * 4);
        hdItemPaneArray_c& group = mPaneGroups[groupIndex];
        if (storage) { group.mpData = (gptr<hdPartsPaneSRT_c>*)storage; group.mCount = counts[groupIndex]; }
        s32 count = group.mCount;
        for (s32 i = 0; i < count; ++i) {
            *gabi::at<gptr<hdPartsPaneSRT_c>>(gabi::ea(group.mpData.get()) + u32(i) * 4) = NULL;
        }
        u32 complete;
        if (groupIndex == 0) complete = hdItem_collectPanes(mButton.mpLayout.get(), &group, &sMainPaneNames.mRoot);
        else if (groupIndex == 1) complete = hdItem_collectPictures(mButton.mpLayout.get(), &group, &sMainPaneNames.mPictures[0]);
        else complete = hdItem_collectTexts(mButton.mpLayout.get(), &group, &sMainPaneNames.mTexts[0]);
        if (!complete) return 0;
    }
    return 1;
}
/* 0263211C */
u32 hdMenuItem_c::load(void* resource, u32 arg1, u32 arg2) {
    if (!hdPartsBasicButton_init(&mButton, &sMainLayoutName, gabi::ea(resource), arg1, (hdPartsDesc_c*)gabi::at<void>(arg2))) return 0;
    if (!preparePanes()) return 0;
    vCachePosition();
    mButton.mpReceiver->mEnabled = 1;
    setSelection(HD_ITEM_INVALID_SLOT, 0x97);
    return 1;
}
/* 02632754 */
static u32 hdMenuItem_isEquipped(hdMenuItem_c*, u32 slot) {
    dSv_player_status_a_c& status = sMenuSave->mInfo.mSavedata.mPlayer.mPlayerStatusA;
    for (u32 i = 0; i < HD_ITEM_PANE_GROUP_COUNT; ++i) {
        u32 equipped = status.mSelectItem[i];
        if (slot == 4) {
            if (equipped == 4 || equipped - 0x18 < 8) return 1;
        } else {
            if (slot == equipped) return 1;
            if (slot == 0xB && equipped - 0x24 < 8) return 1;
            if (slot == 0x12 && equipped - 0x30 < 8) return 1;
        }
    }
    return 0;
}
/* 026327E8 */
void hdMenuItem_c::updateEquippedMode() {
    if (hdMenuItem_isEquipped(this, mAnimationSlot)) {
        mMode = HD_ITEM_MODE_EQUIPPED;
        hdItemAnim_resetSlot(mButton.animSet(), 2, HD_ITEM_SLOT_BUTTON);
        mButton.setPaneEnabled(1);
    } else {
        mMode = HD_ITEM_MODE_NORMAL;
        hdItemAnim_resetSlot(mButton.animSet(), 3, HD_ITEM_SLOT_BUTTON);
        mButton.setPaneEnabled(1);
    }
}
/* 026321CC */
void hdMenuItem_c::releasePanes() {
    for (s32 groupIndex = 2; groupIndex >= 0; --groupIndex) {
        hdItemPaneArray_c& group = mPaneGroups[groupIndex];
        s32 count = group.mCount;
        for (s32 i = 0; i < count; ++i) {
            *gabi::at<gptr<hdPartsPaneSRT_c>>(gabi::ea(group.mpData.get()) + u32(i) * 4) = NULL;
        }
        if (count > 0 || group.mpData.get() != NULL) {
            hdScreenHeap* heap = hdParts_findContainHeap(sMainHeapManager, group.mpData.get());
            heap->free(group.mpData.get());
            group.mCount = 0;
            group.mpData = NULL;
        }
    }
}
/* 0263235C */
void hdMenuItem_c::finalize() { releasePanes(); hdBasicButton_c::unload(); }
/* 026323C4 */
void hdMenuItem_c::updateVisible() {
    if (mAnimationSlot == 4 || mAnimationSlot == 0xB || mAnimationSlot == 0x12) {
        if (!hdItem_currentStateEquals(this, &sDragState)) hdBasicButton_c::registerReceiver();
        return;
    }
    if (hdItem_currentStateEquals(this, &sMainBlankState) ||
        hdItem_currentStateEquals(this, &sMainButtonInState) ||
        hdItem_currentStateEquals(this, &sMainButtonOutState) ||
        hdItem_currentStateEquals(this, &sDragState)) return;
    if (sMenuEventManager && mButton.mpReceiver.get()) hdUiReceiverList_pushBack(gabi::ea(sMenuEventManager), gabi::ea(mButton.mpReceiver.get()));
}
/* 026325C8 */
void hdMenuItem_c::resetSelectionAnimation() {
    if (hdItem_currentStateEquals(this, &sMainFloatState)) {
        mPoints[0].xBits = mPanePosition.xBits;
        mPoints[1].yBits = mPanePosition.yBits;
        mPoints[1].xBits = mPanePosition.xBits;
        mPoints[0].yBits = mPanePosition.yBits;
        sMenuIcon->positionAtPoint(&mPoints[0]);
        sMenuIcon->hideAfterFloat();
        gabi::call_ptr(menuVtable()->mTranslatePoint.get(), this, &mPoints[0]);
        if (mVisible && mPaneGroups[0].mpData.get()[0].get()) {
            hdPartsPaneSRT_c* pane = mPaneGroups[0].mpData.get()[0];
            pane->mFlags = (pane->mFlags & ~u32(HD_ITEM_PANE_VISIBLE)) + 1;
        }
    }
    hdItemAnim_resetSlot(mButton.animSet(), HD_ITEM_ANIM_FLOAT_CANCEL, HD_ITEM_SLOT_FLOAT);
    hdItemAnim_resetSlot(mButton.animSet(), 1, HD_ITEM_SLOT_BUTTON);
    mButton.setPaneEnabled(1);
    mButton.changeState(&sMenuOffState);
}
/* 02632390 */
void hdMenuItem_c::executeButton() { hdBasicButton_c::update(); hdItemFader_update(&mInterpolator); }
/* 02632704 */
void hdMenuItem_c::setActive(u32 active) { mButton.changeState(active ? &sMenuOffState : &sPartsDisplayState); }
/* 02632724 */
void hdMenuItem_c::storeSelection(u32 animationSlot, u32 slot) { mOtherSlot = animationSlot; mPreviousSlot = slot; }
/* 02632730 */
void hdMenuItem_c::applySelection() {
    if (mAnimationSlot != mOtherSlot || mSlot != mPreviousSlot) refreshSelection( mOtherSlot, mPreviousSlot);
}
/* 02632A38 */
void hdMenuItem_c::updateAnimation() { updateTimer(mSlot); }
/* 02632A40 */
u32 hdMenuItem_c::isHighlightPlaying() { return mButton.animSet()->mEntries.at(13)->get()->mController->mPlaying; }
/* 02632A6C */
u32 hdMenuItem_c::isDragState() {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(mButton.stateMachine());
    hdPartsStateIdVtbl_c* comparisonVtable = sDragState.mVtable;
    u32 currentNumber = current->getKey();
    return currentNumber == gabi::call_ptr<u32>(comparisonVtable->mGetKey.get(), &sDragState);
}
/* 02632ADC */
u32 hdMenuItem_c::isKeyState() {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(mButton.stateMachine());
    hdPartsStateIdVtbl_c* comparisonVtable = sKeyState.mVtable;
    u32 currentNumber = current->getKey();
    return currentNumber == gabi::call_ptr<u32>(comparisonVtable->mGetKey.get(), &sKeyState);
}
/* 02632B4C */
u32 hdMenuItem_c::isCursorState() {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(mButton.stateMachine());
    hdPartsStateIdVtbl_c* comparisonVtable = sCursorState.mVtable;
    u32 currentNumber = current->getKey();
    return currentNumber == gabi::call_ptr<u32>(comparisonVtable->mGetKey.get(), &sCursorState);
}
/* 02633020 */
void hdMenuItem_c::updateAnimationOptions() {
    bool inactive = false;
    hdPartsStateId_c* states[] = {&sMenuOffState, &sCursorState, &sPartsDisplayState};
    for (u32 i = 0; i < HD_ITEM_PANE_GROUP_COUNT; ++i) {
        hdPartsStateId_c* current = hdParts_getCurrentStateId(mButton.stateMachine());
        hdPartsStateId_c* comparison = states[i];
        hdPartsStateIdVtbl_c* comparisonVtable = comparison->mVtable;
        u32 currentNumber = current->getKey();
        if (currentNumber == gabi::call_ptr<u32>(comparisonVtable->mGetKey.get(), comparison)) { inactive = true; break; }
    }
    if (!inactive || mSlot == 0x35 || mSlot == 0x36) {
        mButton.setPaneEnabled(1);
        mButton.setTextEnabled(1);
    }
}
/* 0263315C */
u32 hdMenuItem_c::classifyDirection(u32 direction, u32 first, u32 second, u32 third, u32 fourth) {
    if (direction == 5 || direction == 8 || (direction >= 11 && direction <= 12)) return direction;
    f32 x = sMenuInput->mPointer.mStick.value.x;
    f32 y = sMenuInput->mPointer.mStick.value.y;
    f32 squared = gabi::fmadds(x, x, gabi::fmuls_ppc(y, y));
    f32 length;
    if (squared > 0.0f) {
        f64 estimate = gabi::frsqrte(squared);
        f32 correction = gabi::fnmsubs(f32(estimate * round25(estimate)), squared, 3.0f);
        length = gabi::fmuls_ppc(gabi::fmuls_ppc(correction, f32(estimate * 0.5)), squared);
    } else length = gabi::fmuls_ppc(0.0f, squared);
    if (!(length > 0.0f) || (x == 0.0f && y == 0.0f)) return direction;
    u32 angle;
    if (x < 0.0f) {
        if (y < 0.0f) {
            if (x > y) angle = 0xC0000000u - hdItem_atanIndex(f32(x / y));
            else angle = 0x80000000u + hdItem_atanIndex(f32(y / x));
        } else {
            if (-x < y) angle = 0x40000000u + hdItem_atanIndex(f32(-(x / y)));
            else angle = 0x80000000u - hdItem_atanIndex(f32(-(y / x)));
        }
    } else {
        if (y < 0.0f) {
            if (x < -y) angle = 0xC0000000u + hdItem_atanIndex(f32(-(x / y)));
            else angle = 0u - hdItem_atanIndex(f32(-(y / x)));
        } else {
            if (x < y) angle = 0x40000000u - hdItem_atanIndex(f32(x / y));
            else angle = hdItem_atanIndex(f32(y / x));
        }
    }
    if (angle <= first) return angle == 0 ? direction : 12;
    if (angle <= second) return 5;
    if (angle <= third) return 11;
    if (angle <= fourth) return 8;
    return 12;
}
/* 026333A8 */
void hdMenuItem_c::updateDirection(u32 direction) {
    u32 selected = classifyDirection(direction, 0x20000000, 0x60000000, 0xA0000000, 0xE0000000);
    mButton.requestFocus(selected);
}
/* 02632CFC */
s32 hdMenuItem_c::selectedTexture() {
    u32 slot = mSlot;
    if (slot != mPreviousSlot) slot = mPreviousSlot;
    for (u32 i = 0; i < HD_ITEM_MAPPING_COUNT; ++i) if (sItemMappings.mEntries[i].mSlot == slot) return sItemMappings.mEntries[i].mSelectedTexture;
    return -1;
}
/* 02632D40 */
s32 hdMenuItem_c::currentTexture() {
    u32 slot = mSlot;
    for (u32 i = 0; i < HD_ITEM_MAPPING_COUNT; ++i) if (sItemMappings.mEntries[i].mSlot == slot) return sItemMappings.mEntries[i].mCurrentTexture;
    return -1;
}
/* 02632D74 */
void hdMenuItem_c::preparePane() { hdItemPane_setAlpha(mPaneGroups[2].at(1), 0x20); }
/* 02632D94 */
static u32 hdMenuItem_inventoryItem(s32 slot) {
    dSv_player_c& player = sMenuSave->mInfo.mSavedata.mPlayer;
    if (u32(slot) - 0x18 < 8) return player.mBagItem.mBeast[u32(slot) - 0x18];
    if (u32(slot) - 0x24 < 8) return player.mBagItem.mBait[u32(slot) - 0x24];
    if (u32(slot) - 0x30 < 8) return player.mBagItem.mReserve[u32(slot) - 0x30];
    if (slot < 0x15) {
        // HD: negative indices are not rejected; retain guest-address wraparound.
        return *gabi::at<be<u8>>(gabi::ea(&player.mPlayerItem.mItems[0]) + u32(slot));
    }
    return HD_ITEM_INVALID_SLOT;
}
/* 02632E64 */
static u32 hdMenuItem_isUnclaimedPointerConfirm(hdButtonEvent_l* event) {
    if (sMenuEventManager->getId() != 0) return 0;
    u32 button = event->mButton;
    return (button == 3 || button == 4 || button == 14) && event->mType == HD_BUTTON_EVENT_TOUCH_e;
}
/* 026333F0 */
void hdMenuItem_c::setVisible(u32 visible) {
    if (mVisible != visible) {
        mVisible = visible;
        hdPartsPaneSRT_c* pane = mPaneGroups[0].mpData.get()[0];
        if (pane) pane->mFlags = (pane->mFlags & ~u32(HD_ITEM_PANE_VISIBLE)) | visible;
    }
    if (hdBasicButton_c::isEnabled() != visible) vSetActive(visible);
}
/* 026334E8 */
void hdMenuItem_c::translatePoint(hdItemPoint_c* point) {
    f32 dx = point->x - mPanePosition.x;
    f32 dy = point->y - mPanePosition.y;
    hdPartsPaneSRT_c* pane = rootPane();
    f32 y = mPaneTranslation.y + dy;
    f32 x = mPaneTranslation.x + dx;
    u32 z = pane->mTranslationZBits;
    hdItemPaneSetTranslateY(pane, y);
    hdItemPaneSetTranslateX(pane, x);
    pane->mFlags = pane->mFlags | 0x10;
    pane->mTranslationZBits = z;
}
/* 02633480 */
void hdMenuItem_c::cachePosition() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    hdPartsPaneSRT_c* pane = rootPane();
    gabi::FrameLocal<hdItemPosition3_c> position(8);
    hdItemPane_getGlobalPosition(position, pane);
    u32 x = position->xBits, y = position->yBits;
    mPanePosition.xBits = x;
    mPanePosition.yBits = y;
    u32 translateX = pane->mTranslationXBits, translateY = pane->mTranslationYBits;
    mPaneTranslation.xBits = translateX;
    mPaneTranslation.yBits = translateY;
}
/* 02632F08 */
u32 hdMenuItem_c::loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), sMenuUiSystem->mHeap, 4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mButton.mpLayout = layout;
    if (!layout || !layout->vLoad(name, arg1, arg2, arg3, HD_ITEM_ANIMATION_COUNT, 9, desc)) return 0;
    for (u32 i = 0; i < HD_ITEM_EXTRA_ANIMATION_COUNT; ++i) {
        u32 groupIndex = sMenuGroupRemap.mIndices[i];
        hdUiSafeString* group = &sMenuGroups.mNames[groupIndex];
        hdItemAnimation_bind(mButton.animSet(), i + HD_ITEM_ANIM_TOUCH_SLIDE_DECIDE, &sMenuAnimations.mNames[i], group);
    }
    return 1;
}

static hdItemHeaderZero_c sMainHeaderZero;
static hdItemHeaderDescriptors_c sMainHeaderDescriptors;
static hdItemPoint_c sMainPiBounds;
static hdItemPoint_c sMainPiConstants;
static u8 sMainPrimaryHeaderObject;
static u8 sMainSecondaryHeaderObject;
void hdMenuHeader_constructPrimary(void*);
void hdMenuHeader_constructSecondary(void*);
static hdItemHeaderLimits_c sMainHeaderLimits;
static f32 sMainFarLimit;
static f32 sMainNearLimit;
static u32 sMainStateCounterReady;
static u32 sMainStateCounter;
static hdPartsStateIdVtbl_c sMainStateVtable;
static hdStateId sMainButtonOffState;
static hdStateId sMainButtonOnTouchState;
static hdStateId sMainButtonOnCursorState;
static hdStateId sMainButtonSelectTouchState;
static hdStateId sMainButtonSelectCursorState;
static hdStateId sMainButtonUnselectState;
static hdStateId sMainButtonDecideState;
static hdStateId sMainNullState;
static hdPartsStateId_c sMainTouchSlideDecideState;
static hdPartsStateId_c sMainTouchSlideDecideCmdState;
static hdPartsStateId_c sMainIconChangeState;
static hdPartsStateId_c sMainArrowLoopState;
static hdPartsStateId_c sMainNumState;
static hdPartsStateId_c sMainSwitchState;
static f32 sMainOffsetX;
static f32 sMainOffsetY;
/* 0263359C */
static void __sinit_hd_screen_menu_item_cpp() {
    // This TU inserts four limit floats between the pi pair and the header objects.
    for (u32 i = 0; i < 4; ++i) sMainHeaderZero.mWords[i] = 0;
    __register_global_object(gabi::ea(&sMainHeaderDescriptors.mDescriptors[0]));
    sMainPiBounds.x = sMainPiConstants.x;
    sMainPiBounds.y = sMainPiConstants.y;
    hdMenuHeader_constructPrimary(&sMainPrimaryHeaderObject);
    __register_global_object(gabi::ea(&sMainHeaderDescriptors.mDescriptors[1]));
    hdMenuHeader_constructSecondary(&sMainSecondaryHeaderObject);
    __register_global_object(gabi::ea(&sMainHeaderDescriptors.mDescriptors[2]));
    sMainHeaderLimits.mFarX = sMainFarLimit;
    sMainHeaderLimits.mFarY = sMainFarLimit;
    sMainHeaderLimits.mNearX = sMainNearLimit;
    sMainHeaderLimits.mNearY = sMainNearLimit;
    hdParts_setSafeString(&sMainPaneNames.mRoot, &sItemSafeStringVtable, "N_ItemIcon_00");
    hdParts_setSafeString(&sMenuGroups.mNames[0], &sItemSafeStringVtable, "G_Btn_00");
    hdParts_setSafeString(&sMenuGroups.mNames[1], &sItemSafeStringVtable, "G_IconChange_00");
    hdParts_setSafeString(&sMenuGroups.mNames[2], &sItemSafeStringVtable, "G_Float_00");
    hdParts_setSafeString(&sMenuGroups.mNames[3], &sItemSafeStringVtable, "G_Num_00");
    hdParts_setSafeString(&sMenuGroups.mNames[4], &sItemSafeStringVtable, "G_ArrowFire_00");
    hdParts_setSafeString(&sMenuGroups.mNames[5], &sItemSafeStringVtable, "G_ArrowIce_00");
    hdParts_setSafeString(&sMenuGroups.mNames[6], &sItemSafeStringVtable, "G_ArrowLight_00");
    hdParts_setSafeString(&sMenuGroups.mNames[7], &sItemSafeStringVtable, "P_ArrowFire_00");
    hdParts_setSafeString(&sMenuGroups.mNames[8], &sItemSafeStringVtable, "P_ArrowIce_00");
    hdParts_setSafeString(&sMenuGroups.mNames[9], &sItemSafeStringVtable, "P_ArrowLight_00");
    hdParts_setSafeString(&sMenuGroups.mNames[10], &sItemSafeStringVtable, "T_Num_00");
    hdParts_setSafeString(&sMenuGroups.mNames[11], &sItemSafeStringVtable, "T_Time_00");
    hdParts_setSafeString(&sMenuAnimations.mNames[0], &sItemSafeStringVtable, "TouchSlideDecide");
    hdParts_setSafeString(&sMenuAnimations.mNames[1], &sItemSafeStringVtable, "TouchSlideDecideCmd");
    hdParts_setSafeString(&sMenuAnimations.mNames[2], &sItemSafeStringVtable, "IconChange");
    hdParts_setSafeString(&sMenuAnimations.mNames[3], &sItemSafeStringVtable, "ArrowLoop");
    hdParts_setSafeString(&sMenuAnimations.mNames[4], &sItemSafeStringVtable, "ArrowLoop");
    hdParts_setSafeString(&sMenuAnimations.mNames[5], &sItemSafeStringVtable, "ArrowLoop");
    hdParts_setSafeString(&sMenuAnimations.mNames[6], &sItemSafeStringVtable, "Float");
    hdParts_setSafeString(&sMenuAnimations.mNames[7], &sItemSafeStringVtable, "FloatCancel");
    hdParts_setSafeString(&sMenuAnimations.mNames[8], &sItemSafeStringVtable, "Num");
    hdParts_setSafeString(&sMenuAnimations.mNames[9], &sItemSafeStringVtable, "Switch");
    hdParts_setSafeString(&sMainLayoutName, &sItemSafeStringVtable, "BtnItemIcon_00");
    u32 number;
    if (sMainStateCounterReady) number = sMainStateCounter;
    else { number = 0; sMainStateCounterReady = 1; }
    sMenuOffState.setup(++number, "::StateID_Off", &sMainStateVtable, 0x26, &sMainButtonOffState);
    sMainOnTouchState.setup(++number, "::StateID_OnTouch", &sMainStateVtable, 0x29, &sMainButtonOnTouchState);
    sCursorState.setup(++number, "::StateID_OnCursor", &sMainStateVtable, 0x2c, &sMainButtonOnCursorState);
    sMainSelectTouchState.setup(++number, "::StateID_SelectTouch", &sMainStateVtable, 0x2f, &sMainButtonSelectTouchState);
    sMainSelectCursorState.setup(++number, "::StateID_SelectCursor", &sMainStateVtable, 0x32, &sMainButtonSelectCursorState);
    sMainUnselectState.setup(++number, "::StateID_UnSelect", &sMainStateVtable, 0x35, &sMainButtonUnselectState);
    sMainDecideState.setup(++number, "::StateID_Decide", &sMainStateVtable, 0x38, &sMainButtonDecideState);
    sKeyState.setup(++number, "::StateID_ChangeItemByKey", &sMainStateVtable, 0x4b, &sMainNullState);
    sDragState.setup(++number, "::StateID_ChangeItemByDrag", &sMainStateVtable, 0x4e, &sMainNullState);
    sMainTouchSlideDecideState.setup(++number, "::StateID_TouchSlideDecide", &sMainStateVtable, 0x51, &sMainNullState);
    sMainTouchSlideDecideCmdState.setup(++number, "::StateID_TouchSlideDecideCmd", &sMainStateVtable, 0x54, &sMainNullState);
    sMainIconChangeState.setup(++number, "::StateID_IconChange", &sMainStateVtable, 0x57, &sMainNullState);
    sMainArrowLoopState.setup(++number, "::StateID_ArrowLoop", &sMainStateVtable, 0x5a, &sMainNullState);
    sMainFloatState.setup(++number, "::StateID_Float", &sMainStateVtable, 0x5d, &sMainNullState);
    sMainFloatCancelState.setup(++number, "::StateID_FloatCancel", &sMainStateVtable, 0x60, &sMainNullState);
    sMainNumState.setup(++number, "::StateID_Num", &sMainStateVtable, 0x63, &sMainNullState);
    sMainSwitchState.setup(++number, "::StateID_Switch", &sMainStateVtable, 0x66, &sMainNullState);
    sMainStateCounter = number;
    sTouchIconOffset.x = sMainOffsetX;
    sTouchIconOffset.y = sMainOffsetY;
}

/* WWHD glue
data sMainButtonOnTouchState 1049D860
data sMainButtonOnCursorState 1049D890
data sMainButtonSelectTouchState 1049D8C0
data sMainButtonSelectCursorState 1049D8F0
data sMainButtonUnselectState 1049D920
data sMainButtonDecideState 1049D950
data sMainHeaderZero 1048FAEC
data sMainHeaderDescriptors 101F54E8
data sMainPiBounds 1048FAD0
data sMainPiConstants 100E7FDC
data sMainPrimaryHeaderObject 1048FAE8
data sMainSecondaryHeaderObject 1048FAE9
ext 028ED6F8 void hdMenuHeader_constructPrimary(void*)
ext 028EAB2C void hdMenuHeader_constructSecondary(void*)
str 100E7FF0 in 0263359C
str 100E7FF4 in 0263359C
str 100E7FFC in 0263359C
str 100E8004 in 0263359C
str 100E8014 in 0263359C
str 100E8024 in 0263359C
str 100E8034 in 0263359C
str 100E8044 in 0263359C
str 100E8058 in 0263359C
str 100E8064 in 0263359C
str 100E8078 in 0263359C
str 100E8088 in 0263359C
str 100E8094 in 0263359C
str 100E80A4 in 0263359C
str 100E80B4 in 0263359C
str 100E80C0 in 0263359C
str 100E80D0 in 0263359C
str 100E80E4 in 0263359C
str 100E80FC in 0263359C
str 100E8118 in 0263359C
str 100E8138 in 0263359C
str 100E8150 in 0263359C
str 100E8160 in 0263359C
str 100E816C in 0263359C
str 100E8178 in 0263359C
str 100E8184 in 0263359C
str 100E8194 in 0263359C
str 100E81A8 in 0263359C
str 100E81B4 in 0263359C
str 100E81C4 in 0263359C
str 100E81D0 in 0263359C
str 100E81E0 in 0263359C
str 100E81F4 in 0263359C
str 100E820C in 0263359C
str 100E8220 in 0263359C
str 100E8234 in 0263359C
str 100E8250 in 0263359C
str 100E826C in 0263359C
str 100E8284 in 0263359C
data sMainHeaderLimits 1048FAD8
data sMainFarLimit 100E7FE4
data sMainNearLimit 100E7FE8
data sMainStateCounterReady 101FD8E4
data sMainStateCounter 101FDD50
data sMainStateVtable 100E7A70
data sMainButtonOffState 1049D830
data sMainNullState 101FF32C
data sMainTouchSlideDecideState 1048FD5C
data sMainTouchSlideDecideCmdState 1048FD8C
data sMainIconChangeState 1048FDBC
data sMainArrowLoopState 1048FDEC
data sMainNumState 1048FE7C
data sMainSwitchState 1048FEAC
data sMainOffsetX 100E7F24
data sMainOffsetY 100E7FEC
data sMainOnTouchState 1048FBDC
data sMainSelectTouchState 1048FC3C
data sMainSelectCursorState 1048FC6C
data sMainUnselectState 1048FC9C
ext 026F69A4 void hdItemButton_markTouchRelease()
ext 026F621C u32 hdItemButton_canDispatch(u32)
ext 02615140 u32 cking::ui::input::EventMgr::getMode()
data sItemSystem 101F8344
ext 02694500 u32 hdItem_findReceiverChild(hdParts_c*, u32)
ext 0267021C Pair32 hdItem_itemPosition(hdParts_c*, u32)
ext 026151A0 u32 cking::ui::input::EventMgr::getHover()
data sMainBlankState 1049DD60
data sMainButtonInState 1049D9E0
data sMainButtonOutState 1049DA10
ext 026150C0 void hdUiReceiverList_pushBack(u32, u32)
data sMainPaneNames 1048FAC8
data sMainFloatCancelState 1048FE4C
ext 02030B6C void hdItem_playDragSe(const char*, f32, f32)
str 100E7FA8 "SE_MENU_ICON_DRAG"
data sMainFloatState 1048FE1C
ext 020013C0 hdUiEvent* hdItem_copyEvent(hdUiEvent*, hdUiEvent*)
ext 026F67A4 u32 hdItemButton_inputEnabled()
ext 02615154 void cking::ui::input::EventMgr::setModeDefault(u32)
data sItemSafeStringVtable 100E79B8
ext 025F6D70 void hdItemMessage_setLabel(HdMsgText*, hdUiSafeString*, hdUiSafeString*)
ext 02704504 void hdTextPane_c::prepareMessage(PaneWork_l*, u32)
str 100E7F40 "MenuItem_00"
str 100E7F4C "L_BtnItemIcon_00-T_Time_00"
data sCountTextVtable 100E79E8
data sCountFormat 100E7F78
ext 02759D3C void hdItem_formatCount(HdMsgString*, be<u16>*, u32)
ext 027046D4 void hdTextPane_c::setStringPrepared(PaneString_l*, PaneWork_l*)
data sMainDecideState 1048FCCC
ext 0261AB28 void hdItem_movePoint(hdItemPoint_c*, hdItemPoint_c*, f32)
data sMainHeapManager 101F8B4C
data sMainLayoutName 1048FEDC
ext 02005708 void hdItemAnim_resetSlot(hdAnimSet*, u32, u32)
data sMenuInput 101F5088
ext 02756D88 u32 hdItem_atanIndex(f32)
data sTouchIconOffset 1048FEE4
data sMenuAnimations 1048FB5C
data sMenuGroups 1048FAFC
data sMenuGroupRemap 100E7A00
data sMenuUiSystem 1018C404

data sMenuVtable 100E82BC
data sMenuHolderVtable 100E829C
data sMenuSecondaryVtable 100E82AC
data sMenuOffState 1048FBAC
data sPartsDisplayState 1049D9B0
data sDragState 1048FD2C
data sKeyState 1048FCFC
data sCursorState 1048FC0C
data sMenuIcon 101F5578
data sItemMappings 100E7AB8
data sCountModes 100E7E48
data sMenuSave 101F84DC
data sMenuEquipmentFlags 1047B074
data sMenuEventManager 101F4FF0
str 100E7F80 "SE_MENU_DECIDE" in 026302AC
str 100E7F90 "SE_MENU_ITEM_SET" in 026313A8
str 100E7FBC "SE_MENU_ICON_DRAG_RELEASE" in 02631980
ext 02030B38 void hdItem_playSe(const char*)
ext 02615398 u32 cking::ui::input::EventMgr::getId()
ext 02004E04 void hdItemAnimation_bind(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*)
ext 02762170 void hdItemFader_construct(hdItemFader_l*, f32)
ext 02762258 void hdItemFader_set(hdItemFader_l*, u32, f32)
ext 027621EC void hdItemFader_update(hdItemFader_l*)
ext 02704AFC void hdItemPane_setAlpha(hdPartsPaneSRT_c*, u32)




ext 028F3F90 f32 hdItem_splitFraction(be<f32>*, f32)
ext 025B61C8 u32 hdItemSave_getTimer(dSv_player_item_record_c*)
ext 02720144 hdSavePicture_l* hdSaveHolders_c::getScratchPicture()
ext 02005334 void hdItemAnim_clearSlot(hdAnimSet*, u32)
ext 020051C4 void hdItemAnim_attach(hdAnimSet*, u32, u32)

ext 026F66B0 void hdButtonPart_c::clearFocus()
ext 026F6918 void hdButtonPart_c::handleTouchRelease()
ext 026F1FD8 void hdBasicButton_c::registerReceiver()
ext 026F5B74 u32 hdButtonPart_c::getReceiverId()
ext 026F5CD4 void hdButtonPart_c::setTextEnabled(u32)
ext 026F6614 BOOL hdButtonPart_c::requestFocus(u32)
ext 026F66F4 void hdButtonPart_c::synchronizeFocus()
ext 026F5CCC void hdButtonPart_c::setPaneEnabled(u32)
ext 026F2458 BOOL hdBasicButton_c::isEnabled()
ext 026F1F3C void hdBasicButton_c::unload()
ext 026F1F70 void hdBasicButton_c::update()
ext 026F17BC void hdBasicButton_c::decideEnd()
ext 026F0B8C void hdBasicButton_c::destroy(hdBasicButton_c*, u32)
*/
