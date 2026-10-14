/* HD collection-menu equipment mask. */
#include "d/hd_screen_btn_collect_icon_mask.h"
static hdPartsStateId_c sCollectMaskOffStateLocal;
static hdPartsStateId_c sCollectMaskOffState;
static hdPartsVtbl_c sCollectMaskVtable;
static u8 sCollectMaskHolderVtable;
static u8 sCollectMaskSecondaryVtable;
static hdCollectHeapManagerPointer_c sCollectMaskHeapManager;
static hdCollectEventManagerPointer_c sCollectMaskInput;
static hdPartsStateId_c sCollectMaskSelectTouchState;
static hdPartsStateId_c sCollectMaskOnCursorState;
static hdPartsStateId_c sCollectMaskSelectCursorState;
static hdPartsStateId_c sCollectMaskOnTouchState;
static hdPartsStateId_c sCollectMaskUnSelectState;
extern u32 collectMaskIsOwned(dSv_player_collect_c*, u32, u32);
extern void collectMaskEquip(dSv_player_collect_c*, u32, u32);
extern void collectMaskUnequip(dSv_player_collect_c*, u32, u32);
extern void collectMaskFinalize(hdParts_c*);
extern void collectMaskDestroyBase(hdParts_c*, u32);
extern void collectMaskPlay(hdAnimSet*, u32, u32, f32);
extern u32 collectMaskAnimationEnd(hdAnimSet*, u32);
extern void collectMaskSound(const char*);
extern hdScreenHeap* collectMaskFindHeap(void*, void*);
extern void collectMaskReleaseBase(hdParts_c*);
extern void collectMaskResetAnimation(hdAnimSet*, u32, u32);
extern void collectMaskCopyEvent(hdUiEvent*, hdUiEvent*);
extern u32 collectMaskInputEnabled(hdParts_c*);
extern u32 collectMaskCanDispatch(u32);
extern void collectMaskMarkTouchRelease(hdParts_c*);
extern hdUiEvent* collectMaskMakeEvent(hdUiEvent*, u32, u32, const void*);
extern void collectMaskQueueEvent(hdWidget_queue_c*, hdUiEvent*);

/* 0262AC68 */
s32 collectMaskStateNumber(hdStateId* state) { return state->mId; }

/* 0262AD68 */
void collectMaskDestroy(hdBtnCollectIconMask_c* button, u32 flags) {
    if (!button) return;
    button->mStateHolder.mHolderVtable = &sCollectMaskHolderVtable;
    button->mSecondaryVtable = &sCollectMaskSecondaryVtable;
    collectMaskDestroyBase(button, 0);
    if (flags & 1) operator_delete(button);
}

/* 0262ADD4 */
void hdBtnCollectIconMask_c::decideMove() {
    gabi::NativeFrame<0x38> frame;
    if (!sCollectMaskInput.mpManager.get()) return;
    hdUiEvent* event = hdPartsView<hdUiEvent>(frame + 8);
    collectMaskCopyEvent(event, collectCurrentEvent(sCollectMaskInput.mpManager));
    u32 receiverId = collectButtonView(this)->getReceiverId();
    if (u32(event->mId) == receiverId) {
        if (!acceptsDecide(event)) return;
        if (collectMaskInputEnabled(this) && collectButtonView(this)->mTouchEnabled) collectButtonView(this)->clearFocus();
        onDecide();
        changeState(&sCollectMaskSelectTouchState);
    } else if (collectMaskInputEnabled(this) && collectButtonView(this)->mTouchEnabled) {
        if (sCollectMaskInput.mpManager->getMode() == gabi::ea(collectButtonView(this)->mpReceiver.get())) {
            collectButtonView(this)->handleTouchRelease();
            changeState(&sCollectMaskSelectCursorState);
        }
    }
}

/* 0262AEE4 */
u32 hdBtnCollectIconMask_c::isEquipped() {
    return collectMaskIsOwned(&collectSavePlayer().mCollect, 4, 1) != 0;
}

/* 0262AF20 */
void hdBtnCollectIconMask_c::setEquipped(u32 equipped) {
    mEquipped = equipped;
    dSv_player_collect_c* collection = &collectSavePlayer().mCollect;
    if (equipped) collectMaskEquip(collection, 4, 1);
    else collectMaskUnequip(collection, 4, 1);
}

/* 0262AF48 */
void hdBtnCollectIconMask_c::toggleEquipment() {
    if (isEquipped()) {
        mAnimation = COLLECT_MASK_ANIMATION_EQUIP_OFF;
        setEquipped(0);
        collectMaskSound("SE_MENU_ITEM_EQUIP_OFF");
    } else {
        mAnimation = COLLECT_MASK_ANIMATION_EQUIP_ON;
        setEquipped(1);
        collectMaskSound("SE_MENU_ITEM_EQUIP_ON");
    }
}

/* 0262AFBC */
void hdBtnCollectIconMask_c::offMove() {
    gabi::NativeFrame<0x40> frame;
    if (!sCollectMaskInput.mpManager.get()) return;
    hdUiEvent* event = hdPartsView<hdUiEvent>(frame + 8);
    collectMaskCopyEvent(event, collectCurrentEvent(sCollectMaskInput.mpManager));
    u32 receiverId = collectButtonView(this)->getReceiverId();
    if (u32(event->mId) == receiverId) {
        if (!acceptsSelect(event)) return;
        toggleEquipment();
        changeState(&sCollectMaskUnSelectState);
        return;
    }
    s32 receiver = event->mId;
    if (receiver == -1) {
        if (!collectMaskInputEnabled(this) || !collectButtonView(this)->mTouchEnabled) return;
        if (sCollectMaskInput.mpManager->getMode() != gabi::ea(collectButtonView(this)->mpReceiver.get())) return;
        if (acceptsPoint(event)) {
            toggleEquipment();
            changeState(&sCollectMaskUnSelectState);
            return;
        }
        if (collectMaskCanDispatch(event->mKind) && acceptsDispatch(event)) {
            collectMaskMarkTouchRelease(this);
            changeState(&sCollectMaskUnSelectState);
        }
    } else if (!receiver) {
        if (collectMaskInputEnabled(this) && collectButtonView(this)->mTouchEnabled) changeState(&sCollectMaskOnCursorState);
        else changeState(&sCollectMaskUnSelectState);
    }
}

/* 0262B14C */
void hdBtnCollectIconMask_c::selectMove() {
    gabi::NativeFrame<0x38> frame;
    if (!sCollectMaskInput.mpManager.get()) return;
    hdUiEvent* event = hdPartsView<hdUiEvent>(frame + 8);
    collectMaskCopyEvent(event, collectCurrentEvent(sCollectMaskInput.mpManager));
    u32 receiverId = collectButtonView(this)->getReceiverId();
    if (u32(event->mId) == receiverId) {
        if (!acceptsSelect(event)) return;
        toggleEquipment();
        changeState(&sCollectMaskUnSelectState);
        return;
    }
    if (s32(event->mId) == -1) {
        if (!collectMaskInputEnabled(this) || !collectButtonView(this)->mTouchEnabled) return;
        if (sCollectMaskInput.mpManager->getMode() != gabi::ea(collectButtonView(this)->mpReceiver.get())) return;
        if (acceptsPoint(event)) {
            toggleEquipment();
            changeState(&sCollectMaskUnSelectState);
            return;
        }
        if (collectMaskCanDispatch(event->mKind) && acceptsDispatch(event)) {
            collectMaskMarkTouchRelease(this);
            changeState(&sCollectMaskUnSelectState);
        }
    } else {
        if (collectMaskInputEnabled(this) && collectButtonView(this)->mTouchEnabled &&
            sCollectMaskInput.mpManager->getMode() == gabi::ea(collectButtonView(this)->mpReceiver.get())) return;
        changeState(&sCollectMaskUnSelectState);
    }
}

/* 0262B2CC */
void hdBtnCollectIconMask_c::touchEnter() {
    collectMaskPlay(animSet(), 0, 0, 1.0f);
    mAnimationState = 0;
}

/* 0262B318 */
void hdBtnCollectIconMask_c::touchMove() {
    gabi::NativeFrame<0x40> frame;
    if (!sCollectMaskInput.mpManager.get()) return;
    hdUiEvent* event = hdPartsView<hdUiEvent>(frame + 8);
    if (!mAnimationState) {
        collectMaskCopyEvent(event, collectCurrentEvent(sCollectMaskInput.mpManager));
        u32 receiverId = collectButtonView(this)->getReceiverId();
        if (u32(event->mId) == receiverId) {
            if (acceptsSelect(event)) {
                toggleEquipment(); changeState(&sCollectMaskUnSelectState); return;
            }
        } else if (s32(event->mId) == -1) {
            if (collectMaskInputEnabled(this) && collectButtonView(this)->mTouchEnabled &&
                sCollectMaskInput.mpManager->getMode() == gabi::ea(collectButtonView(this)->mpReceiver.get())) {
                if (acceptsPoint(event)) {
                    toggleEquipment(); changeState(&sCollectMaskUnSelectState); return;
                }
                if (collectMaskCanDispatch(event->mKind) && acceptsDispatch(event)) {
                    collectMaskMarkTouchRelease(this); mAnimationState = 2;
                }
            }
        } else if (!s32(event->mId)) {
            mAnimationState = collectMaskInputEnabled(this) && collectButtonView(this)->mTouchEnabled ? 1 : 2;
        }
    }
    if (!collectMaskAnimationEnd(animSet(), 0)) return;
    u32 phase = mAnimationState;
    if (phase == 2) { changeState(&sCollectMaskUnSelectState); return; }
    u32 receiver = collectButtonView(this)->getReceiverId();
    collectMaskMakeEvent(event, HD_BUTTON_EVENT_SPECIAL_e, receiver, NULL);
    hdWidget_queue_c* queue = hdPartsView<hdWidget_queue_c>(gabi::ea(sCollectMaskInput.mpManager.get()) + offsetof(cking::ui::input::EventMgr, mEventQueue));
    collectMaskQueueEvent(queue, event);
    changeState(phase == 1 ? &sCollectMaskOnCursorState : &sCollectMaskOnTouchState);
}

/* 0262B55C */
void hdBtnCollectIconMask_c::cursorEnter() {
    collectMaskPlay(animSet(), 0, 0, 1.0f);
    mAnimationState = 1;
}

/* 0262B5A8 */
void hdBtnCollectIconMask_c::cursorMove() {
    gabi::NativeFrame<0x40> frame;
    if (!sCollectMaskInput.mpManager.get()) return;
    hdUiEvent* event = hdPartsView<hdUiEvent>(frame + 8);
    if (u32(mAnimationState) == 1) {
        collectMaskCopyEvent(event, collectCurrentEvent(sCollectMaskInput.mpManager));
        u32 receiverId = collectButtonView(this)->getReceiverId();
        if (u32(event->mId) == receiverId) {
            if (acceptsSelect(event)) {
                toggleEquipment(); changeState(&sCollectMaskUnSelectState); return;
            }
        } else if (s32(event->mId) == -1) {
            if (collectMaskInputEnabled(this) && collectButtonView(this)->mTouchEnabled &&
                sCollectMaskInput.mpManager->getMode() == gabi::ea(collectButtonView(this)->mpReceiver.get())) {
                if (acceptsPoint(event)) {
                    toggleEquipment(); changeState(&sCollectMaskUnSelectState); return;
                }
                if (collectMaskCanDispatch(event->mKind) && acceptsDispatch(event)) {
                    collectMaskMarkTouchRelease(this); mAnimationState = 2;
                }
            }
        } else if (!(collectMaskInputEnabled(this) && collectButtonView(this)->mTouchEnabled &&
            sCollectMaskInput.mpManager->getMode() == gabi::ea(collectButtonView(this)->mpReceiver.get()))) mAnimationState = 2;
    }
    if (!collectMaskAnimationEnd(animSet(), 0)) return;
    if (u32(mAnimationState) == 2) { changeState(&sCollectMaskUnSelectState); return; }
    u32 receiver = collectButtonView(this)->getReceiverId();
    collectMaskMakeEvent(event, HD_BUTTON_EVENT_SPECIAL_e, receiver, NULL);
    hdWidget_queue_c* queue = hdPartsView<hdWidget_queue_c>(gabi::ea(sCollectMaskInput.mpManager.get()) + offsetof(cking::ui::input::EventMgr, mEventQueue));
    collectMaskQueueEvent(queue, event);
    changeState(&sCollectMaskOnCursorState);
}

/* 0262B79C */
void hdBtnCollectIconMask_c::equipmentAnimationEnter() {
    collectMaskPlay(animSet(), mAnimation, 0, 1.0f);
}

/* 0262B7BC */
void hdBtnCollectIconMask_c::equipmentAnimationMove() {
    gabi::NativeFrame<0x38> frame;
    hdUiEvent* event = hdPartsView<hdUiEvent>(frame + 8);
    collectMaskCopyEvent(event, collectCurrentEvent(sCollectMaskInput.mpManager));
    u32 receiverId = collectButtonView(this)->getReceiverId();
    if (u32(event->mId) == receiverId) {
        if (acceptsDecide(event)) {
            if (collectMaskInputEnabled(this) && collectButtonView(this)->mTouchEnabled) collectButtonView(this)->clearFocus();
            onDecide(); changeState(&sCollectMaskSelectTouchState); return;
        }
    } else if (s32(event->mId) == -1) {
        if (collectMaskInputEnabled(this) && collectButtonView(this)->mTouchEnabled &&
            sCollectMaskInput.mpManager->getMode() == gabi::ea(collectButtonView(this)->mpReceiver.get()) && acceptsPoint(event)) {
            toggleEquipment(); changeState(&sCollectMaskUnSelectState); return;
        }
    }
    if (collectMaskAnimationEnd(animSet(), mAnimation)) {
        mAnimation = 1;
        changeState(&sCollectMaskOffStateLocal);
    }
}

/* 0262B928 */
void hdBtnCollectIconMask_c::equipOffEnter() {
    collectMaskPlay(animSet(), COLLECT_MASK_ANIMATION_EQUIP_OFF, 0, 1.0f);
    collectMaskSound("SE_MENU_ITEM_EQUIP_OFF");
}

/* 0262B96C */
void hdBtnCollectIconMask_c::equipOffMove() {
    if (collectMaskAnimationEnd(animSet(), COLLECT_MASK_ANIMATION_EQUIP_OFF)) changeState(&sCollectMaskOffStateLocal);
}

/* 0262B9BC */
void hdBtnCollectIconMask_c::equipOnEnter() {
    collectMaskPlay(animSet(), COLLECT_MASK_ANIMATION_EQUIP_ON, 0, 1.0f);
    collectMaskSound("SE_MENU_ITEM_EQUIP_ON");
}

/* 0262BA00 */
void hdBtnCollectIconMask_c::equipOnMove() {
    if (collectMaskAnimationEnd(animSet(), COLLECT_MASK_ANIMATION_EQUIP_ON)) changeState(&sCollectMaskOffStateLocal);
}

/* 0262BA50 */
hdBtnCollectIconMask_c* collectMaskConstruct(hdBtnCollectIconMask_c* button) {
    if (!button) {
        button = static_cast<hdBtnCollectIconMask_c*>(operator_new(sizeof(hdBtnCollectIconMask_c)));
        if (!button) return NULL;
    }
    hdPartsNoDecideButton_construct(button);
    button->mVtable = &sCollectMaskVtable;
    button->mStateHolder.mHolderVtable = &sCollectMaskHolderVtable;
    button->mSecondaryVtable = &sCollectMaskSecondaryVtable;
    hdCollectPaneArray_c* panes = hdPartsView<hdCollectPaneArray_c>(gabi::ea(button) + offsetof(hdBtnCollectIconMask_c, mIconPanes));
    if (!gabi::ea(panes)) panes = static_cast<hdCollectPaneArray_c*>(operator_new(sizeof(hdCollectPaneArray_c)));
    if (panes) { panes->mpPanes = NULL; panes->mCount = 0; }
    button->mIconVisible = 0;
    button->mEquipped = 0;
    button->mAnimation = 1;
    return button;
}

/* 0262BAF8 */
void hdBtnCollectIconMask_c::setIconVisible(u32 visible) {
    if (u32(mIconVisible) == visible) return;
    gptr<hdCollectPane_c>* panes = mIconPanes.mpPanes;
    mIconVisible = visible;
    hdCollectPane_c* pane = panes[0];
    hdPartsPaneSRT_c* flags = reinterpret_cast<hdPartsPaneSRT_c*>(pane);
    flags->mFlags = (u32(flags->mFlags) & ~1u) | visible;
    hdCollectButtonVtable_c* table = reinterpret_cast<hdCollectButtonVtable_c*>(mVtable.get());
    gabi::call_ptr<void>(table->mRefreshVisibility.get(), this);
}

/* 0262BDFC */
void hdBtnCollectIconMask_c::release() {
    s32 count = mIconPanes.mCount;
    bool freeStorage = count > 0;
    if (count > 0) {
        for (s32 i = 0; i < count; ++i) mIconPanes.mpPanes.get()[i] = NULL;
    } else freeStorage = mIconPanes.mpPanes.get() != NULL;
    if (freeStorage) {
        hdScreenHeap* heap = collectMaskFindHeap(sCollectMaskHeapManager.mpManager, mIconPanes.mpPanes.get());
        heap->free(mIconPanes.mpPanes.get());
        mIconPanes.mCount = 0;
        mIconPanes.mpPanes = NULL;
    }
    collectMaskReleaseBase(this);
}

/* 0262BEA4 */
void hdBtnCollectIconMask_c::finalize() { collectMaskFinalize(this); }

/* 0262BEA8 */
void hdBtnCollectIconMask_c::setActive(u32 active) {
    changeState(active ? &sCollectMaskOffStateLocal : &sCollectMaskOffState);
}

/* 0262BEC8 */
void hdBtnCollectIconMask_c::updateEquipment() {
    if (!collectMaskIsOwned(&collectSavePlayer().mCollect, 4, 0)) { setIconVisible(0); return; }
    setIconVisible(1);
    u32 currentEquipped = isEquipped();
    if (u32(mEquipped) == currentEquipped) return;
    u32 equipped = isEquipped();
    hdPartsLayout_c* layout = mpLayout;
    mEquipped = equipped;
    collectMaskResetAnimation(layout->animSet(), equipped ? COLLECT_MASK_ANIMATION_EQUIP_ON : COLLECT_MASK_ANIMATION_EQUIP_OFF, 0);
    collectButtonView(this)->setPaneEnabled(1);
}

static hdCollectUiSystemPointer_c collectMaskUiSystem;
static hdCollectCollectMaskAnimationIndicesArray_c collectMaskAnimationIndices;
static hdCollectCollectMaskGroupsArray_c collectMaskGroups;
static hdUiSafeString collectMaskAnimationNames;
extern void collectMaskBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
extern void* collectMaskAllocateLayout(u32, hdScreenHeap*, u32);
/* 0262BF84 */
u32 hdBtnCollectIconMask_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* descriptor) {
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(collectMaskAllocateLayout(sizeof(hdPartsLayout_c), collectMaskUiSystem.mpSystem->mHeap.get(), 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout) return 0;
    if (!layout->vLoad(name, first, second, third, 7, 1, descriptor)) return 0;
    for (u32 index = 0; index < 2; ++index) {
        u32 animation = collectMaskAnimationIndices[index];
        hdUiSafeString* animationName = hdPartsView<hdUiSafeString>(gabi::ea(&collectMaskAnimationNames) + animation * sizeof(hdUiSafeString));
        collectMaskBindAnimation(animSet(), COLLECT_MASK_ANIMATION_EQUIP_OFF + index, &collectMaskGroups[index], animationName);
    }
    return 1;
}

static hdCollectStaticAngles_c collectMaskAngles;
static hdCollectZeroVector_c collectMaskZeroVector;
static hdCollectCollectMaskRegistrationsArray_c collectMaskRegistrations;
static hdCollectCollectMaskAngleConstantsArray_c collectMaskAngleConstants;
static hdUiSafeString collectMaskLayoutName;
static hdUiSafeString collectMaskPaneName;
static hdUiStringTable collectMaskStringVtable;
static hdPartsStateIdVtbl_c collectMaskStateVtable;
static hdStateId collectMaskNullState;
static hdPartsStateId_c collectMaskEquipOffState;
static hdPartsStateId_c collectMaskEquipOnState;
static hdCollectCollectMaskParentsArray_c collectMaskParents;
static hdCollectWord collectMaskStateReady;
static hdCollectSignedWord collectMaskStateCounter;
extern void collectMaskRegister(hdCollectGlobalRegistration_c*);
extern void collectMaskConstructFirst(u8*);
extern void collectMaskConstructSecond(u8*);
/* 0262C09C */
void collectMaskStaticInit() {
    collectMaskZeroVector.mWords[2] = 0;
    collectMaskZeroVector.mWords[0] = 0;
    collectMaskZeroVector.mWords[3] = 0;
    collectMaskZeroVector.mWords[1] = 0;
    collectMaskRegister(&collectMaskRegistrations[0]);
    f32 low = collectMaskAngleConstants[0], high = collectMaskAngleConstants[1];
    collectMaskAngles.mLow = low; collectMaskAngles.mHigh = high;
    collectMaskConstructFirst(&collectMaskAngles.mFirst);
    collectMaskRegister(&collectMaskRegistrations[1]);
    collectMaskConstructSecond(&collectMaskAngles.mSecond);
    collectMaskRegister(&collectMaskRegistrations[2]);
    hdParts_setSafeString(&collectMaskPaneName, &collectMaskStringVtable, "P_Btn_00");
    hdParts_setSafeString(&collectMaskGroups[0], &collectMaskStringVtable, "Off");
    hdParts_setSafeString(&collectMaskGroups[1], &collectMaskStringVtable, "On");
    hdParts_setSafeString(&collectMaskLayoutName, &collectMaskStringVtable, "BtnCollectIconMask_00");
    hdParts_setSafeString(&collectMaskAnimationNames, &collectMaskStringVtable, "G_Btn_00");
    s32 number = 0;
    if (collectMaskStateReady) number = collectMaskStateCounter;
    else collectMaskStateReady = 1;
    sCollectMaskOffStateLocal.setup(number = s32(u32(number) + 1), "::StateID_Off", &collectMaskStateVtable, 0x23, reinterpret_cast<hdStateId*>(&collectMaskParents[0]));
    sCollectMaskOnTouchState.setup(number = s32(u32(number) + 1), "::StateID_OnTouch", &collectMaskStateVtable, 0x26, reinterpret_cast<hdStateId*>(&collectMaskParents[1]));
    sCollectMaskOnCursorState.setup(number = s32(u32(number) + 1), "::StateID_OnCursor", &collectMaskStateVtable, 0x29, reinterpret_cast<hdStateId*>(&collectMaskParents[2]));
    sCollectMaskSelectTouchState.setup(number = s32(u32(number) + 1), "::StateID_SelectTouch", &collectMaskStateVtable, 0x2C, reinterpret_cast<hdStateId*>(&collectMaskParents[3]));
    sCollectMaskSelectCursorState.setup(number = s32(u32(number) + 1), "::StateID_SelectCursor", &collectMaskStateVtable, 0x2F, reinterpret_cast<hdStateId*>(&collectMaskParents[4]));
    sCollectMaskUnSelectState.setup(number = s32(u32(number) + 1), "::StateID_UnSelect", &collectMaskStateVtable, 0x32, reinterpret_cast<hdStateId*>(&collectMaskParents[5]));
    collectMaskEquipOffState.setup(number = s32(u32(number) + 1), "::StateID_EquipOff", &collectMaskStateVtable, 0x40, &collectMaskNullState);
    collectMaskEquipOnState.setup(number = s32(u32(number) + 1), "::StateID_EquipOn", &collectMaskStateVtable, 0x43, &collectMaskNullState);
    collectMaskStateCounter = number;
}

static hdScreenType collectMaskPaneType;
static hdScreenType collectMaskParentType;
static hdScreenType collectMaskRootType;
static hdCollectWord collectMaskPaneTypeReady;
static hdCollectWord collectMaskParentTypeReady;
static hdCollectWord collectMaskRootTypeReady;
extern u32 collectMaskInitBase(hdParts_c*, hdUiSafeString*, u32, u32, hdPartsDesc_c*);
extern hdScreenHeap* collectMaskCurrentHeap(void*);
/* 0262BB30 */
u32 hdBtnCollectIconMask_c::init(u32 first, u32 second, hdPartsDesc_c* descriptor) {
    if (!collectMaskInitBase(this, &collectMaskLayoutName, first, second, descriptor)) return 0;
    hdScreenHeap* heap = collectMaskUiSystem.mpSystem->mHeap;
    if (!heap) heap = collectMaskCurrentHeap(sCollectMaskHeapManager.mpManager);
    void* memory = heap->allocate(sizeof(gptr<hdCollectPane_c>));
    if (memory) { mIconPanes.mpPanes = static_cast<gptr<hdCollectPane_c>*>(memory); mIconPanes.mCount = 1; }
    s32 count = mIconPanes.mCount;
    if (count > 0) {
        for (s32 index = 0; index < count; ++index)
            *hdPartsView<gptr<hdCollectPane_c>>(gabi::ea(mIconPanes.mpPanes.get()) + u32(index) * 4) = NULL;
        count = mIconPanes.mCount;
    }
    for (s32 index = 0; index < count; ++index) {
        u32 assure = collectMaskPaneName.mVtable->mAssureTermination.get();
        hdPartsLyt_c* layout = mpLayout->lyt();
        u32 slotAddress = gabi::ea(mIconPanes.mpPanes.get());
        if (u32(index) < u32(count)) slotAddress += u32(index) * sizeof(gptr<hdCollectPane_c>);
        hdUiSafeString* name = hdPartsView<hdUiSafeString>(gabi::ea(&collectMaskPaneName) + u32(index) * sizeof(hdUiSafeString));
        gabi::call_ptr<void>(assure, name);
        hdScreenResourceObject* root = reinterpret_cast<hdScreenResourceObject*>(layout->mpRootPane.get());
        hdPartsPane_c* pane = static_cast<hdPartsPane_c*>(root->findPane(reinterpret_cast<const char*>(collectMaskPaneName.mText.get())));
        if (!collectMaskPaneTypeReady) {
            u32 parentReady = collectMaskParentTypeReady;
            collectMaskPaneTypeReady = 1;
            if (!parentReady) {
                u32 rootReady = collectMaskRootTypeReady;
                collectMaskParentTypeReady = 1;
                if (!rootReady) { collectMaskRootType.mParent = NULL; collectMaskRootTypeReady = 1; }
                collectMaskParentType.mParent = &collectMaskRootType;
            }
            collectMaskPaneType.mParent = &collectMaskParentType;
        }
        if (pane) {
            hdScreenType* type = pane->getRuntimeTypeInfo();
            while (type && type != &collectMaskPaneType) type = type->mParent;
            if (!type) pane = NULL;
        }
        *hdPartsView<gptr<hdCollectPane_c>>(slotAddress) = reinterpret_cast<hdCollectPane_c*>(pane);
        count = mIconPanes.mCount;
    }
    if (!collectMaskIsOwned(&collectSavePlayer().mCollect, 4, 0)) {
        mIconVisible = 1; setIconVisible(0);
        hdPartsLayout_c* layout = mpLayout;
        mEquipped = 0;
        collectMaskResetAnimation(layout->animSet(), COLLECT_MASK_ANIMATION_EQUIP_OFF, 0);
    } else {
        mIconVisible = 0; setIconVisible(1);
        u32 equipped = isEquipped();
        hdPartsLayout_c* layout = mpLayout;
        mEquipped = equipped;
        collectMaskResetAnimation(layout->animSet(), equipped ? COLLECT_MASK_ANIMATION_EQUIP_ON : COLLECT_MASK_ANIMATION_EQUIP_OFF, 0);
    }
    collectButtonView(this)->setPaneEnabled(1);
    return 1;
}

/* WWHD glue (tools/verify/autoglue.py)
data collectMaskPaneType 101FD90C
data collectMaskParentType 101FD910
data collectMaskRootType 101FD8FC
data collectMaskPaneTypeReady 101FD908
data collectMaskParentTypeReady 101FD898
data collectMaskRootTypeReady 101FD888
ext 0270178C u32 collectMaskInitBase(hdParts_c*,hdUiSafeString*,u32,u32,hdPartsDesc_c*)
ext 02756140 hdScreenHeap* collectMaskCurrentHeap(void*)

data collectMaskAngles 1048F4F0
data collectMaskZeroVector 1048F4FC
data collectMaskRegistrations 101F5434
data collectMaskAngleConstants 100E673C
data collectMaskLayoutName 1048F69C
data collectMaskPaneName 1048F4E8
data collectMaskStringVtable 100E662C
data collectMaskStateVtable 100E668C
data collectMaskNullState 101FF32C
data collectMaskParents 1049E53C
data collectMaskEquipOffState 1048F63C
data collectMaskEquipOnState 1048F66C
data collectMaskStateReady 101FD8E4
data collectMaskStateCounter 101FDD50
ext 028F026C void collectMaskRegister(hdCollectGlobalRegistration_c*)
ext 028ED6F8 void collectMaskConstructFirst(u8*)
ext 028EAB2C void collectMaskConstructSecond(u8*)
str 100E67C0 "P_Btn_00" in 0262C09C
str 100E6744 "Off" in 0262C09C
str 100E6748 "On" in 0262C09C
str 100E674C "BtnCollectIconMask_00" in 0262C09C
str 100E67B4 "G_Btn_00" in 0262C09C
str 100E6764 "::StateID_Off" in 0262C09C
str 100E6774 "::StateID_OnTouch" in 0262C09C
str 100E67CC "::StateID_OnCursor" in 0262C09C
str 100E6788 "::StateID_SelectTouch" in 0262C09C
str 100E67E0 "::StateID_SelectCursor" in 0262C09C
str 100E67F8 "::StateID_UnSelect" in 0262C09C
str 100E680C "::StateID_EquipOff" in 0262C09C
str 100E67A0 "::StateID_EquipOn" in 0262C09C

data collectMaskUiSystem 1018C404
data collectMaskAnimationIndices 100E6620
data collectMaskGroups 1048F50C
data collectMaskAnimationNames 1048F4E0
ext 02004E04 void collectMaskBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 0273B050 void* collectMaskAllocateLayout(u32,hdScreenHeap*,u32)
data sCollectMaskOffStateLocal 1048F51C
data sCollectMaskOffState 1049E65C
data sCollectMaskVtable 100E6820
data sCollectMaskHolderVtable 100E6A50
data sCollectMaskSecondaryVtable 100E6A60
data sCollectMaskHeapManager 101F8B4C
data sCollectMaskInput 101F4FF0
data sCollectMaskSelectTouchState 1048F5AC
data sCollectMaskOnCursorState 1048F57C
data sCollectMaskSelectCursorState 1048F5DC
data sCollectMaskOnTouchState 1048F54C
data sCollectMaskUnSelectState 1048F60C
ext 025B7A2C u32 collectMaskIsOwned(dSv_player_collect_c*,u32,u32)
ext 025B7944 void collectMaskEquip(dSv_player_collect_c*,u32,u32)
ext 025B79B8 void collectMaskUnequip(dSv_player_collect_c*,u32,u32)
ext 027019B8 void collectMaskFinalize(hdParts_c*)
ext 02701718 hdParts_c* hdPartsNoDecideButton_construct(hdParts_c*)
ext 02700C9C void collectMaskDestroyBase(hdParts_c*,u32)
ext 020053E4 void collectMaskPlay(hdAnimSet*,u32,u32,f32)
ext 02005840 u32 collectMaskAnimationEnd(hdAnimSet*,u32)
ext 02030B38 void collectMaskSound(const char*)
ext 02755FEC hdScreenHeap* collectMaskFindHeap(void*,void*)
ext 02701984 void collectMaskReleaseBase(hdParts_c*)
ext 02005708 void collectMaskResetAnimation(hdAnimSet*,u32,u32)
ext 026F5B74 u32 hdButtonPart_c::getReceiverId()
ext 026F66B0 void hdButtonPart_c::clearFocus()
ext 026F6918 void hdButtonPart_c::handleTouchRelease()
ext 026F67A4 u32 collectMaskInputEnabled(hdParts_c*)
ext 026F621C u32 collectMaskCanDispatch(u32)
ext 026F69A4 void collectMaskMarkTouchRelease(hdParts_c*)
ext 02615140 u32 cking::ui::input::EventMgr::getMode()
ext 020013C0 void collectMaskCopyEvent(hdUiEvent*,hdUiEvent*)
ext 02001338 hdUiEvent* collectMaskMakeEvent(hdUiEvent*,u32,u32,const void*)
ext 02001FF4 void collectMaskQueueEvent(hdWidget_queue_c*,hdUiEvent*)
str 100E66F4 "SE_MENU_ITEM_EQUIP_OFF" in 0262AF48
str 100E66DC "SE_MENU_ITEM_EQUIP_ON" in 0262AF48
str 100E670C "SE_MENU_ITEM_EQUIP_OFF" in 0262B928
str 100E6724 "SE_MENU_ITEM_EQUIP_ON" in 0262B9BC
*/
