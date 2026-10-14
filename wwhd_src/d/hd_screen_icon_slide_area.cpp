/* HD equipped-item slide-area controller. */
#include "d/hd_screen_icon_slide_area.h"
#include "hd/hd_ui_02001084.h"
static hdButtonEventManager_l* sControllerEventManager;

static hdAnimSetUiSystem* sControllerUiSystem;
static hdIconSlideGroupIndices_c sControllerGroupIndices;
static hdIconSlideGroupNames_c sControllerGroupNames;
static hdIconSlideAnimationNames_c sControllerAnimationNames;

static hdIconSlideItemPaneNames_c sControllerHitPaneNames;
static u32 sControllerHitTypeGuard;
static u32 sControllerHitParentGuard;
static u32 sControllerRootTypeGuard;
static hdScreenType sControllerHitType;
static hdScreenType sControllerHitParent;
static hdScreenType sControllerRootType;
static hdIconSlideItemPaneNames_c sControllerItemPaneNames;
static hdIconSlideEquipped_l* sControllerEquipped;
static void* sControllerHeapManager;
static hdIconSlideItemPaneNames_c sControllerChildPaneNames;
static hdUiSafeString sControllerItemClassName;
static hdIconSlideStartup_c sControllerStartup;
static hdIconSlideDestructorRecords_c sControllerDestructorRecords;
static hdUiStringTable sControllerLiteralVtable;
static hdPartsStateIdVtbl_c sControllerStateVtable;
static hdStateId sControllerNullState;
static u32 sControllerStateCounterGuard;
static u32 sControllerStateCounter;
static hdPartsStateId_c sControllerFloatState;
static hdPartsStateId_c sControllerCancelState;
static hdPartsStateId_c sControllerSwitchState;
static hdIconSlideItemPaneNames_c sControllerItemClassDescriptors;

static hdUiSafeString sControllerLayoutName;
static hdPartsVtbl_c sControllerPartsVtable;
static u32 sControllerHolderVtable;
static u32 sControllerSecondaryVtable;

/* 0266F4F4 */
void hdIconSlideArea_c::enterFloat() {
    hdAnimSet_play(animSet(), ICON_FLOAT_Y, ICON_DROP_Y, 1.0f);
    hdAnimSet_play(animSet(), ICON_FLOAT_X, ICON_DROP_X, 1.0f);
    hdAnimSet_play(animSet(), ICON_FLOAT_R, ICON_DROP_R, 1.0f);
}

/* 0266F584 */
void hdIconSlideArea_c::executeFloat() {
    vCalcLayout();
}

/* 0266F594 */
void hdIconSlideArea_c::enterFloatCancel() {
    hdAnimSet_play(animSet(), ICON_CANCEL_Y, ICON_DROP_Y, 1.0f);
    hdAnimSet_play(animSet(), ICON_CANCEL_X, ICON_DROP_X, 1.0f);
    hdAnimSet_play(animSet(), ICON_CANCEL_R, ICON_DROP_R, 1.0f);
}

/* 0266F624 */
void hdIconSlideArea_c::executeFloatCancel() {
    vCalcLayout();
}

/* 0266F634 */
void hdIconSlideArea_c::enterSwitch() {
    hdAnimSet_play(animSet(), ICON_SWITCH_X, ICON_GLOW_X, 1.0f);
    hdAnimSet_play(animSet(), ICON_SWITCH_Y, ICON_GLOW_Y, 1.0f);
    hdAnimSet_play(animSet(), ICON_SWITCH_R, ICON_GLOW_R, 1.0f);
}

/* 0266F6C4 */
void hdIconSlideArea_c::executeSwitch() {
    vCalcLayout();
}

/* 0266F488 */
void hdIconSlideArea_c::destroy(hdIconSlideArea_c* area, s32 flags) {
    if (area) {
        area->mStateHolder.mHolderVtable = &sControllerHolderVtable;
        area->mSecondaryVtable = &sControllerSecondaryVtable;
        iconSlideDestroyBase(area, 0);
        if (flags & 1) iconSlideDelete(area);
    }
}

/* 02670570 */
void hdIconSlideArea_c::getAnimationSet(gptr<hdAnimSet>* out) {
    *out = animSet();
}
/* inlined into hdIconSlideArea_c::construct */
static void initializeIconSlideArray(hdIconSlideArrayStorage_c* storage) {
    if (!storage) {
        storage = static_cast<hdIconSlideArrayStorage_c*>(iconSlideAllocate(sizeof(hdIconSlideArrayStorage_c)));
        if (!storage) return;
    }
    storage->mData = 0;
    storage->mCount = 0;
}

/* inlined into hdIconSlideArea_c::construct */
static void initializeIconSlideMask(be<u32>* storage) {
    if (!storage) {
        storage = static_cast<be<u32>*>(iconSlideAllocate(sizeof(be<u32>)));
        if (!storage) return;
    }
    *storage = 0;
}

/* 0266F6D4 */
hdIconSlideArea_c* hdIconSlideArea_c::construct(hdIconSlideArea_c* area) {
    if (!area) {
        area = static_cast<hdIconSlideArea_c*>(iconSlideAllocate(sizeof(hdIconSlideArea_c)));
        if (!area) return area;
    }
    iconSlideConstructBase(area);
    area->mVtable = &sControllerPartsVtable;
    area->mStateHolder.mHolderVtable = &sControllerHolderVtable;
    area->mSecondaryVtable = &sControllerSecondaryVtable;
    initializeIconSlideArray(gabi::at<hdIconSlideArrayStorage_c>(gabi::ea(area) + offsetof(hdIconSlideArea_c, mItemPaneCount)));
    initializeIconSlideArray(gabi::at<hdIconSlideArrayStorage_c>(gabi::ea(area) + offsetof(hdIconSlideArea_c, mHitPaneCount)));
    initializeIconSlideMask(gabi::at<be<u32>>(gabi::ea(area) + offsetof(hdIconSlideArea_c, mFloatingSlots)));
    initializeIconSlideMask(gabi::at<be<u32>>(gabi::ea(area) + offsetof(hdIconSlideArea_c, mGlowSlots)));
    return area;
}

/* 0266FD84 */
void hdIconSlideArea_c::execute() {
    iconSlideUpdateState(stateMachine());
    vExecuteChildren();
}

/* 026707D8 */
u32 hdIconSlideArea_c::loadLayout(void* name, u32 a, u32 b, u32 c, void* desc) {
    void* heap = sControllerUiSystem->mHeap;
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(iconSlideAllocateOnHeap(sizeof(hdPartsLayout_c), heap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, a, b, c, 9, 6, desc)) return 0;
    for (u32 i = 0; i < 9; ++i) {
        hdAnimSet* animations = animSet();
        u32 group = sControllerGroupIndices.mIndices[i];
        iconSlideBindAnimation(animations, i, &sControllerAnimationNames.mNames[i], &sControllerGroupNames.mNames[group]);
    }
    return 1;
}

/* 0266FF54 */
void hdIconSlideArea_c::startFloat(u32 slot, u32 playSound) {
    u32 bit = (slot & 63) < 32 ? 1u << (slot & 31) : 0;
    u32 old = mFloatingSlots;
    if (old & bit) return;
    mFloatingSlots = old | bit;
    iconSlideItemStartFloat(child(slot));
    iconSlideStartAnimation(animSet(), slot + ICON_SWITCH_X, slot);
    if (playSound) iconSlidePlaySound("SE_MENU_SUB_CURSOR");
}

/* 0266FFF8 */
void hdIconSlideArea_c::cancelFloat(u32 slot) {
    u32 bit = (slot & 63) < 32 ? 1u << (slot & 31) : 0;
    u32 old = mFloatingSlots;
    if (!(old & bit)) return;
    mFloatingSlots = old & ~bit;
    iconSlideItemCancelFloat(child(slot));
    hdAnimSet_operationCC(animSet(), slot + ICON_SWITCH_X, slot);
}

/* 0267007C */
void hdIconSlideArea_c::startGlow(u32 slot) {
    u32 bit = (slot & 63) < 32 ? 1u << (slot & 31) : 0;
    u32 old = mGlowSlots;
    if (old & bit) return;
    mGlowSlots = old | bit;
    for (u32 i = 0; i < hdIconSlideArea_c::ITEM_SLOT_COUNT; ++i) {
        hdAnimSetEntry* entry = *animSet()->mEntries.at(i);
        entry->mController->mSpeed = 0.0f;
    }
    hdAnimSet_play(animSet(), slot, slot + ICON_DROP_X, 1.0f);
}

/* 026700F8 */
void hdIconSlideArea_c::cancelGlow(u32 slot) {
    u32 bit = (slot & 63) < 32 ? 1u << (slot & 31) : 0;
    u32 old = mGlowSlots;
    if (!(old & bit)) return;
    mGlowSlots = old & ~bit;
    hdAnimSet_play(animSet(), slot + ICON_DROP_X, slot + ICON_DROP_X, 1.0f);
}

/* 02670130 */
u32 hdIconSlideArea_c::findItemByButton(u32 button) {
    for (u32 i = 0; i < hdIconSlideArea_c::ITEM_SLOT_COUNT; ++i) {
        hdParts_c* item = child(i);
        if (item && iconSlideReceiverId(item) == button) {
            return reinterpret_cast<hdMenuItem_c*>(item)->mAnimationSlot;
        }
    }
    return 0xFF;
}

/* 026701C0 */
u32 hdIconSlideArea_c::findHitPane(u32 receiverId) {
    for (u32 i = 0; i < hdIconSlideArea_c::ITEM_SLOT_COUNT; ++i) {
        u32 index = i < u32(mHitPaneCount) ? i : 0;
        hdWidget_touch_c* hit = mpHitPanes[index];
        if (hit->mSerial == receiverId) return i;
    }
    return ITEM_SLOT_COUNT;
}

/* 02670338 */
u32 hdIconSlideArea_c::hasActiveItem() {
    for (u32 i = 0; i < hdIconSlideArea_c::ITEM_SLOT_COUNT; ++i) {
        hdParts_c* item = child(i);
        if (item && iconSlideItemIsActive(item)) return 1;
    }
    return 0;
}

/* 026703D8 */
u32 hdIconSlideArea_c::hasSelectedItem() {
    for (u32 i = 0; i < hdIconSlideArea_c::ITEM_SLOT_COUNT; ++i) {
        hdParts_c* item = child(i);
        if (item && iconSlideItemIsSelected(item)) return 1;
    }
    return 0;
}

/* 0266FAF8 */
u32 hdIconSlideArea_c::load(u32 first, u32 second, hdPartsDesc_c* desc) {
    if (!hdPartsNoControl_init(this, &sControllerLayoutName, first, second, desc)) return 0;
    if (!bindItemPanes()) return 0;
    if (!createHitPanes()) return 0;
    return 1;
}

/* 0266FD48 */
void hdIconSlideArea_c::release() {
    deleteHitPanes();
    releaseItemPanes();
    iconSlideReleaseBase(this);
}

/* 026702B4 */
u32 hdIconSlideArea_c::hasItemEvent() {
    if (sControllerEventManager) {
        hdUiEvent event;
        iconSlideGetEvent(&event, &sControllerEventManager->mCurrentEvent);
        u32 kind = event.mKind;
        if (kind - 0x43u <= 5 && findItemByButton(event.mId) != 0xFF) return 1;
    }
    return 0;
}

/* 0267021C */
Pair32 hdIconSlideArea_c::itemPosition(u32 slot) {
    u32 index = slot < u32(mItemPaneCount) ? slot : 0;
    hdPartsPane_c* pane = mpItemPanes[index];
    if (!pane) return {0, 0};
    hdIconSlidePanePosition_c output;
    iconSlidePanePosition(&output, pane);
    f32 x = output.x;
    f32 y = output.y;
    hdWidget_vec2_c position;
    position.x = x;
    position.y = y;
    auto* bits = reinterpret_cast<hdIconSlidePositionBits_c*>(&position);
    return {bits->x, bits->y};
}

/* 02670478 */
u32 hdIconSlideArea_c::containsPoint(hdWidget_vec2_c* point) {
    // Non-adjacent FPR loads/stores quiet signaling NaNs before passing the point.
    f32 x = point->x;
    f32 y = point->y;
    hdWidget_vec2_c position;
    position.x = x;
    position.y = y;
    for (u32 i = 0; i < hdIconSlideArea_c::ITEM_SLOT_COUNT; ++i) {
        hdMenuItem_c* item = reinterpret_cast<hdMenuItem_c*>(child(i));
        if (iconSlidePaneContains(iconSlideItemTouch(item)->mpPane, &position)) return 1;
    }
    for (u32 i = 0; i < hdIconSlideArea_c::ITEM_SLOT_COUNT; ++i) {
        u32 index = i < u32(mHitPaneCount) ? i : 0;
        hdWidget_touch_c* hit = mpHitPanes[index];
        if (iconSlidePaneContains(hit->mpPane, &position)) return 1;
    }
    return 0;
}

/* 0266FDC8 */
void hdIconSlideArea_c::refreshItems() {
    for (u32 i = 0; i < hdIconSlideArea_c::ITEM_SLOT_COUNT; ++i) {
        u32 itemId = sControllerEquipped->mSlots[i];
        u32 texture = iconSlideItemTexture(itemId);
        hdParts_c* item = child(i);
        if (item) {
            iconSlideSetItem(item, itemId, texture);
            iconSlideUpdateItem(item);
            iconSlideRefreshItemState(item);
            iconSlideRefreshItemCount(item);
        }
    }
}

/* 0266FE68 */
void hdIconSlideArea_c::registerHitPanes() {
    if (!iconSlideCurrentReceiverId(sControllerEventManager)) {
        for (u32 i = 0; i < hdIconSlideArea_c::ITEM_SLOT_COUNT; ++i) {
            hdParts_c* item = child(i);
            if (item) {
                auto* vtable = reinterpret_cast<hdButtonPartVtable_l*>(item->mVtable.get());
                gabi::call_ptr(vtable->mOnFocusChanged.get(), item);
            }
        }
    }
    for (u32 i = 0; i < hdIconSlideArea_c::ITEM_SLOT_COUNT; ++i) {
        hdButtonEventManager_l* manager = sControllerEventManager;
        if (manager) {
            u32 index = i < u32(mHitPaneCount) ? i : 0;
            hdWidget_touch_c* hit = mpHitPanes[index];
            if (hit) iconSlideRegisterHit(manager, mpHitPanes[index]);
        }
    }
}

/* 0266FB7C */
void hdIconSlideArea_c::deleteHitPanes() {
    s32 count = mHitPaneCount;
    gptr<hdWidget_touch_c>* data = mpHitPanes;
    for (s32 i = 0; i < count; ++i) {
        u32 index = u32(i) < u32(count) ? u32(i) : 0;
        hdWidget_touch_c* hit = data[index];
        if (hit) {
            hit = data[index];
            if (hit) {
                auto* vtable = gabi::at<hdIconSlideTouchVtable_l>(hit->mVtable);
                gabi::call_ptr(vtable->mDestroy.get(), hit, 3);
                count = mHitPaneCount;
                data = mpHitPanes;
            }
            index = u32(i) < u32(count) ? u32(i) : 0;
            data[index] = nullptr;
            count = mHitPaneCount;
            data = mpHitPanes;
        }
    }
    if (data) {
        hdAnimSetHeap* heap = iconSlideFindHeap(sControllerHeapManager, data);
        gabi::call_ptr(heap->mVtable->mFree.get(), heap, mpHitPanes.get());
        mHitPaneCount = 0;
        mpHitPanes = nullptr;
    }
}

/* 0266FCA8 */
void hdIconSlideArea_c::releaseItemPanes() {
    s32 count = mItemPaneCount;
    if (count > 0) {
        for (s32 i = 0; i < count; ++i) mpItemPanes[i] = nullptr;
    } else if (!mpItemPanes) {
        return;
    }
    hdAnimSetHeap* heap = iconSlideFindHeap(sControllerHeapManager, mpItemPanes.get());
    gabi::call_ptr(heap->mVtable->mFree.get(), heap, mpItemPanes.get());
    mItemPaneCount = 0;
    mpItemPanes = nullptr;
}

/* 0266F7C4 */
u32 hdIconSlideArea_c::bindItemPanes() {
    hdAnimSetHeap* heap = sControllerUiSystem->mHeap;
    if (!heap) heap = iconSlideCurrentHeap(sControllerHeapManager);
    void* storage = heap->allocate(hdIconSlideArea_c::ITEM_SLOT_COUNT * sizeof(gptr<hdPartsPane_c>));
    if (storage) {
        mpItemPanes = static_cast<gptr<hdPartsPane_c>*>(storage);
        mItemPaneCount = ITEM_SLOT_COUNT;
    }
    s32 count = mItemPaneCount;
    for (s32 i = 0; i < count; ++i) mpItemPanes[i] = nullptr;
    return iconSlideBindPanes(mpLayout, reinterpret_cast<hdIconSlidePaneArray_c*>(&mItemPaneCount), sControllerItemPaneNames.mNames);
}

/* inlined into hdIconSlideArea_c::createChild */
static bool controllerIconSlideNamesMatch(hdUiSafeString* actual, hdUiSafeString* expected) {
    actual->assureTermination();
    actual->assureTermination();
    u32 savedText = actual->mText.v.get();
    expected->assureTermination();
    if (savedText == expected->mText.v.get()) return true;
    u32 actualText = actual->mText.v.get();
    u32 expectedText = expected->mText.v.get();
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 a = *hdPartsView<be<u8>>(actualText + i);
        u8 b = *hdPartsView<be<u8>>(expectedText + i);
        if (a != b) return false;
        if (!a) return true;
    }
    return false;
}

/* 02670580 */
u32 hdIconSlideArea_c::createChild(hdPartsDesc_c* desc) {
    void* heap = sControllerUiSystem->mHeap;
    auto* description = reinterpret_cast<hdIconSlideChildDesc_l*>(desc);
    for (u32 i = 0; i < hdIconSlideArea_c::ITEM_SLOT_COUNT; ++i) {
        if (!controllerIconSlideNamesMatch(&description->mPaneName, &sControllerChildPaneNames.mNames[i])) continue;
        if (!controllerIconSlideNamesMatch(&description->mClassName, &sControllerItemClassName)) {
            return iconSlideCreateChildBase(this, i, desc);
        }
        gptr<hdParts_c>* slot = childSlot(i);
        void* storage = iconSlideAllocateOnHeap(0x118, heap, 4);
        hdParts_c* item = static_cast<hdParts_c*>(storage);
        if (storage) item = iconSlideControllerItemConstruct(item);
        *slot = item;
        if (!child(i)) return 0;
        return iconSlideControllerItemInitialize(child(i), 0, 0, desc);
    }
    return 1;
}

/* 0266F874 */
u32 hdIconSlideArea_c::createHitPanes() {
    hdAnimSetHeap* savedHeap = sControllerUiSystem->mHeap;
    hdAnimSetHeap* heap = savedHeap;
    if (!heap) heap = iconSlideCurrentHeap(sControllerHeapManager);
    void* storage = heap->allocate(hdIconSlideArea_c::ITEM_SLOT_COUNT * sizeof(gptr<hdWidget_touch_c>));
    if (storage) { mpHitPanes = static_cast<gptr<hdWidget_touch_c>*>(storage); mHitPaneCount = ITEM_SLOT_COUNT; }
    s32 count = mHitPaneCount;
    for (s32 i = 0; i < count; ++i) mpHitPanes[i] = nullptr;
    count = mHitPaneCount;
    for (s32 i = 0; i < count; ++i) {
        hdUiSafeString* name = &sControllerHitPaneNames.mNames[i];
        hdPartsLyt_c* layout = mpLayout->lyt();
        name->assureTermination();
        hdPartsPane_c* root = layout->mpRootPane;
        auto* table = reinterpret_cast<hdButtonPaneVtable_l*>(root->mVtable.get());
        hdPartsPane_c* pane = gabi::call_ptr<hdPartsPane_c*>(table->mFindPane.get(), root, name->mText.get(), 1);
        if (!sControllerHitTypeGuard) {
            sControllerHitTypeGuard = 1;
            if (!sControllerHitParentGuard) {
                sControllerHitParentGuard = 1;
                if (!sControllerRootTypeGuard) {
                    sControllerRootType.mParent = nullptr;
                    sControllerRootTypeGuard = 1;
                }
                sControllerHitParent.mParent = &sControllerRootType;
            }
            sControllerHitType.mParent = &sControllerHitParent;
        }
        hdPartsPane_c* accepted = nullptr;
        if (pane) {
            hdScreenType* type = pane->getRuntimeTypeInfo();
            while (type) {
                if (type == &sControllerHitType) { accepted = pane; break; }
                type = type->mParent;
            }
        }
        u32 index = u32(i) < u32(mHitPaneCount) ? u32(i) : 0;
        gptr<hdWidget_touch_c>* slot = hdPartsView<gptr<hdWidget_touch_c>>(gabi::ea(mpHitPanes.get()) + index * 4);
        void* allocation = iconSlideAllocateOnHeap(sizeof(hdIconSlideHitWidget_l), savedHeap, 4);
        hdWidget_touch_c* hit = static_cast<hdWidget_touch_c*>(allocation);
        if (hit) hit = iconSlideConstructHit(hit, accepted, 0);
        *slot = hit;
        index = u32(i) < u32(mHitPaneCount) ? u32(i) : 0;
        if (!mpHitPanes[index]) return 0;
        static_cast<hdIconSlideHitWidget_l*>(mpHitPanes[index].get())->mRegistered = 1;
        count = mHitPaneCount;
    }
    return 1;
}

/* 026708F0 */
static void initializeIconSlideArea() {
    for (s32 i = 3; i >= 0; --i) sControllerStartup.mZeros[i] = 0;
    iconSlideRegisterGlobal(sControllerDestructorRecords.mRecords[0]);
    sControllerStartup.mMinAngle = -3.1415927410125732f;
    sControllerStartup.mMaxAngle = 3.1415927410125732f;
    iconSlideInitializeFirstFlag(&sControllerStartup.mFlags[0]);
    iconSlideRegisterGlobal(sControllerDestructorRecords.mRecords[1]);
    iconSlideInitializeSecondFlag(&sControllerStartup.mFlags[1]);
    iconSlideRegisterGlobal(sControllerDestructorRecords.mRecords[2]);
    hdParts_setSafeString(&sControllerGroupNames.mNames[0], &sControllerLiteralVtable, "G_XGlow_00");
    hdParts_setSafeString(&sControllerGroupNames.mNames[1], &sControllerLiteralVtable, "G_YGlow_00");
    hdParts_setSafeString(&sControllerGroupNames.mNames[2], &sControllerLiteralVtable, "G_RGlow_00");
    hdParts_setSafeString(&sControllerGroupNames.mNames[3], &sControllerLiteralVtable, "G_XDropSlideFloat_00");
    hdParts_setSafeString(&sControllerGroupNames.mNames[4], &sControllerLiteralVtable, "G_YDropSlideFloat_00");
    hdParts_setSafeString(&sControllerGroupNames.mNames[5], &sControllerLiteralVtable, "G_RDropSlideFloat_00");
    hdParts_setSafeString(&sControllerChildPaneNames.mNames[0], &sControllerLiteralVtable, "L_XItemIcon_00");
    hdParts_setSafeString(&sControllerChildPaneNames.mNames[1], &sControllerLiteralVtable, "L_YItemIcon_00");
    hdParts_setSafeString(&sControllerChildPaneNames.mNames[2], &sControllerLiteralVtable, "L_RItemIcon_00");
    hdParts_setSafeString(&sControllerItemClassDescriptors.mNames[0], &sControllerLiteralVtable, "BtnItemIcon_00");
    hdParts_setSafeString(&sControllerItemClassDescriptors.mNames[1], &sControllerLiteralVtable, "BtnItemIcon_00");
    hdParts_setSafeString(&sControllerItemClassDescriptors.mNames[2], &sControllerLiteralVtable, "BtnItemIcon_00");
    hdParts_setSafeString(&sControllerHitPaneNames.mNames[0], &sControllerLiteralVtable, "B_X_00");
    hdParts_setSafeString(&sControllerHitPaneNames.mNames[1], &sControllerLiteralVtable, "B_Y_00");
    hdParts_setSafeString(&sControllerHitPaneNames.mNames[2], &sControllerLiteralVtable, "B_R_00");
    hdParts_setSafeString(&sControllerAnimationNames.mNames[0], &sControllerLiteralVtable, "Float");
    hdParts_setSafeString(&sControllerAnimationNames.mNames[1], &sControllerLiteralVtable, "Float");
    hdParts_setSafeString(&sControllerAnimationNames.mNames[2], &sControllerLiteralVtable, "Float");
    hdParts_setSafeString(&sControllerAnimationNames.mNames[3], &sControllerLiteralVtable, "FloatCancel");
    hdParts_setSafeString(&sControllerAnimationNames.mNames[4], &sControllerLiteralVtable, "FloatCancel");
    hdParts_setSafeString(&sControllerAnimationNames.mNames[5], &sControllerLiteralVtable, "FloatCancel");
    hdParts_setSafeString(&sControllerAnimationNames.mNames[6], &sControllerLiteralVtable, "Switch");
    hdParts_setSafeString(&sControllerAnimationNames.mNames[7], &sControllerLiteralVtable, "Switch");
    hdParts_setSafeString(&sControllerAnimationNames.mNames[8], &sControllerLiteralVtable, "Switch");
    hdParts_setSafeString(&sControllerLayoutName, &sControllerLiteralVtable, "IconSlideArea_00");
    u32 counter;
    if (sControllerStateCounterGuard) counter = sControllerStateCounter;
    else { sControllerStateCounterGuard = 1; counter = 0; }
    sControllerFloatState.setup(++counter, "::StateID_Float", &sControllerStateVtable, 0x19, &sControllerNullState);
    sControllerCancelState.setup(++counter, "::StateID_FloatCancel", &sControllerStateVtable, 0x1C, &sControllerNullState);
    sControllerStateCounter = ++counter;
    sControllerSwitchState.setup(counter, "::StateID_Switch", &sControllerStateVtable, 0x1F, &sControllerNullState);
}

/* WWHD glue
 data sControllerLayoutName 10494B90
 data sControllerUiSystem 1018C404
 data sControllerGroupIndices 100F5DD4
 data sControllerGroupNames 10494A40
 data sControllerAnimationNames 10494AB8
 data sControllerPartsVtable 100F5FE0
 data sControllerHolderVtable 100F60F0
 data sControllerSecondaryVtable 100F6100
 ext 02700ADC hdParts_c* iconSlideConstructBase(hdParts_c*)
 ext 02700A70 void iconSlideDestroyBase(hdParts_c*, s32)
 ext 0273AD10 void* iconSlideAllocate(u32)
 ext 0273AF40 void iconSlideDelete(void*)
 ext 02006364 void iconSlideUpdateState(hdStateMachine*)
 ext 0273B050 void* iconSlideAllocateOnHeap(u32, void*, u32)
 ext 02004E04 void iconSlideBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*)
 ext 026360EC void iconSlideItemStartFloat(hdParts_c*)
 ext 02636134 void iconSlideItemCancelFloat(hdParts_c*)
 ext 02005708 void iconSlideStartAnimation(hdAnimSet*, u32, u32)
 ext 02030B38 void iconSlidePlaySound(const char*)
 ext 026F5B74 u32 iconSlideReceiverId(hdParts_c*)
 ext 02632A40 u32 iconSlideItemIsActive(hdParts_c*)
 ext 02632A6C u32 iconSlideItemIsSelected(hdParts_c*)
 str 100F5E8C
 data sControllerEventManager 101F4FF0
 ext 026F8B00 void iconSlideReleaseBase(hdParts_c*)
 ext 020013C0 void iconSlideGetEvent(hdUiEvent*, void*)
 ext 02704D2C void iconSlidePanePosition(hdIconSlidePanePosition_c*, hdPartsPane_c*)
 ext 0287F05C u32 iconSlidePaneContains(void*, hdWidget_vec2_c*)
 data sControllerEquipped 101F84DC
 ext 02632D94 u32 iconSlideItemTexture(u32)
 ext 02632724 void iconSlideSetItem(hdParts_c*, u32, u32)
 ext 02632730 void iconSlideUpdateItem(hdParts_c*)
 ext 026329E0 void iconSlideRefreshItemState(hdParts_c*)
 ext 02632A38 void iconSlideRefreshItemCount(hdParts_c*)
 ext 02615398 u32 iconSlideCurrentReceiverId(hdButtonEventManager_l*)
 ext 026150C0 void iconSlideRegisterHit(hdButtonEventManager_l*, hdWidget_touch_c*)
 data sControllerHeapManager 101F8B4C
 ext 02755FEC hdAnimSetHeap* iconSlideFindHeap(void*, void*)
 data sControllerItemPaneNames 10494A70
 ext 02756140 hdAnimSetHeap* iconSlideCurrentHeap(void*)
 data sControllerChildPaneNames 10494A70
 data sControllerItemClassName 1048FEDC
 ext 02635FD4 hdParts_c* iconSlideControllerItemConstruct(hdParts_c*)
 ext 02636040 u32 iconSlideControllerItemInitialize(hdParts_c*, u32, u32, hdPartsDesc_c*)
 ext 026F92C0 u32 iconSlideCreateChildBase(hdParts_c*, u32, hdPartsDesc_c*)
 data sControllerHitPaneNames 10494AA0
 data sControllerHitTypeGuard 101FD938
 data sControllerHitParentGuard 101FD88C
 data sControllerRootTypeGuard 101FD888
 data sControllerHitType 101FD93C
 data sControllerHitParent 101FD940
 data sControllerRootType 101FD8FC
 ext 02616ED4 hdWidget_touch_c* iconSlideConstructHit(hdWidget_touch_c*, hdPartsPane_c*, u32)
 data sControllerStartup 10494A24
 data sControllerDestructorRecords 101F6040
 data sControllerLiteralVtable 100F5DBC
 data sControllerStateVtable 100F5E40
 data sControllerNullState 101FF32C
 data sControllerStateCounterGuard 101FD8E4
 data sControllerStateCounter 101FDD50
 data sControllerFloatState 10494B00
 data sControllerCancelState 10494B30
 data sControllerSwitchState 10494B60
 data sControllerItemClassDescriptors 10494A88
 str 100F5F08 "G_XGlow_00" in 026708F0
 str 100F5F14 "G_YGlow_00" in 026708F0
 str 100F5F20 "G_RGlow_00" in 026708F0
 str 100F5F2C "G_XDropSlideFloat_00" in 026708F0
 str 100F5F44 "G_YDropSlideFloat_00" in 026708F0
 str 100F5F5C "G_RDropSlideFloat_00" in 026708F0
 str 100F5F74 "L_XItemIcon_00" in 026708F0
 str 100F5F84 "L_YItemIcon_00" in 026708F0
 str 100F5F94 "L_RItemIcon_00" in 026708F0
 str 100F5FA4 "BtnItemIcon_00" in 026708F0
 str 100F5EAC "Float" in 026708F0
 str 100F5EE4 "FloatCancel" in 026708F0
 str 100F5EB4 "Switch" in 026708F0
 str 100F5FB4 "IconSlideArea_00" in 026708F0
 str 100F5ED4 "::StateID_Float" in 026708F0
 str 100F5EF0 "::StateID_FloatCancel" in 026708F0
 str 100F5FC8 "::StateID_Switch" in 026708F0
 str 100F5EBC "B_X_00"
 str 100F5EC4 "B_Y_00"
 str 100F5ECC "B_R_00"
 ext 028F026C void iconSlideRegisterGlobal(void*)
 ext 028ED6F8 void iconSlideInitializeFirstFlag(void*)
 ext 028EAB2C void iconSlideInitializeSecondFlag(void*)
*/
