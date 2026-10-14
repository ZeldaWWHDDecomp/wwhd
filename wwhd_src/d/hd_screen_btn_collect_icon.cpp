/* HD collection-menu item icons. */
#include "d/hd_screen_btn_collect_icon.h"
static hdPartsStateId_c sCollectActiveState;
static hdPartsStateId_c sCollectOffState;
static hdCollectIconTable_c sCollectIcons;
static hdPartsVtbl_c sCollectVtable;
static u8 sCollectHolderVtable;
static u8 sCollectSecondaryVtable;
static hdCollectHeapManagerPointer_c sCollectHeapManager;
extern void collectSetAnimationFrame(hdAnimSet*, u32, u32, f32);
extern void collectPlayAnimation(hdAnimSet*, u32, u32, f32);
extern void collectDestroyMessageButton(hdParts_c*, u32);
extern void collectMessageFinalize(hdParts_c*);
extern u32 collectIsOwned(dSv_player_collect_c*, u32, u32);
extern hdScreenHeap* collectFindHeap(void*, void*);
extern void collectReleaseBase(hdParts_c*);

/* 02629EF0 */
s32 collectIconStateNumber(hdStateId* state) { return state->mId; }

/* 02629FF0 */
void collectIconDestroy(hdBtnCollectIcon_c* button, u32 flags) {
    if (!button) return;
    button->mStateHolder.mHolderVtable = &sCollectHolderVtable;
    button->mSecondaryVtable = &sCollectSecondaryVtable;
    collectDestroyMessageButton(button, 0);
    if (flags & 1) operator_delete(button);
}

/* 0262A05C */
void hdBtnCollectIcon_c::iconChangeEnter() { collectPlayAnimation(animSet(), COLLECT_ANIMATION_ICON_CHANGE, 0, 1.0f); }

/* 0262A078 */
void collectIconCalc(hdBtnCollectIcon_c* button) {
    hdCollectButtonVtable_c* table = reinterpret_cast<hdCollectButtonVtable_c*>(button->mVtable.get());
    gabi::call_ptr<void>(table->mCalcLayout.get(), button);
}

/* 0262A088 */
hdBtnCollectIcon_c* collectIconConstruct(hdBtnCollectIcon_c* button) {
    if (!button) {
        button = static_cast<hdBtnCollectIcon_c*>(operator_new(sizeof(hdBtnCollectIcon_c)));
        if (!button) return NULL;
    }
    hdPartsNoDecideButton_construct(button);
    button->mVtable = &sCollectVtable;
    button->mStateHolder.mHolderVtable = &sCollectHolderVtable;
    button->mSecondaryVtable = &sCollectSecondaryVtable;
    hdCollectPaneArray_c* panes = hdPartsView<hdCollectPaneArray_c>(gabi::ea(button) + offsetof(hdBtnCollectIcon_c, mIconPanes));
    if (!gabi::ea(panes)) panes = static_cast<hdCollectPaneArray_c*>(operator_new(sizeof(hdCollectPaneArray_c)));
    if (panes) { panes->mpPanes = NULL; panes->mCount = 0; }
    button->mIcon = 0;
    button->mCollectionKind = COLLECT_KIND_SWORD;
    button->mIconVisible = 1;
    return button;
}

/* 0262A350 */
void hdBtnCollectIcon_c::release() {
    s32 count = mIconPanes.mCount;
    bool freeStorage = count > 0;
    if (count > 0) {
        for (s32 i = 0; i < count; ++i) mIconPanes.mpPanes.get()[i] = NULL;
    } else freeStorage = mIconPanes.mpPanes.get() != NULL;
    if (freeStorage) {
        hdScreenHeap* heap = collectFindHeap(sCollectHeapManager.mpManager, mIconPanes.mpPanes.get());
        heap->free(mIconPanes.mpPanes.get());
        mIconPanes.mCount = 0;
        mIconPanes.mpPanes = NULL;
    }
    collectReleaseBase(this);
}

/* 0262A3F8 */
void hdBtnCollectIcon_c::setActive(u32 active) {
    changeState(active ? &sCollectActiveState : &sCollectOffState);
}

/* 0262A418 */
void hdBtnCollectIcon_c::setIcon(u32 icon) {
    mIcon = icon;
    collectSetAnimationFrame(animSet(), COLLECT_ANIMATION_ICON_CHANGE, 0, f32(icon));
    collectButtonView(this)->setPaneEnabled(1);
}

/* 0262A484 */
void hdBtnCollectIcon_c::setSwordIcon(u32 icon) {
    mCollectionKind = COLLECT_KIND_SWORD;
    setIcon(icon);
}

/* 0262A490 */
u32 hdBtnCollectIcon_c::swordIcon() {
    for (s32 level = 3; level >= 0; --level)
        if (collectIsOwned(&collectSavePlayer().mCollect, 0, level)) return level;
    return COLLECT_ICON_NONE;
}

/* 0262A554 */
u32 hdBtnCollectIcon_c::swordVariantIcon() {
    for (s32 level = 3; level >= 0; --level)
        if (collectIsOwned(&collectSavePlayer().mCollect, 0, level)) return level + 11;
    return COLLECT_ICON_NONE;
}

/* 0262A618 */
u32 hdBtnCollectIcon_c::shieldIcon() {
    if (collectIsOwned(&collectSavePlayer().mCollect, 1, 1)) return 5;
    if (collectIsOwned(&collectSavePlayer().mCollect, 1, 0)) return 4;
    return COLLECT_ICON_NONE;
}

/* 0262A694 */
u32 hdBtnCollectIcon_c::piratesCharmIcon() {
    return collectIsOwned(&collectSavePlayer().mCollect, 3, 0) ? COLLECT_ICON_PIRATES_CHARM : COLLECT_ICON_NONE;
}

/* 0262A6E4 */
u32 hdBtnCollectIcon_c::sailIcon() {
    enum { SWIFT_SAIL_ITEM = 0x77 };
    u32 item = collectSavePlayer().mPlayerItem.mItems[1];
    if (item == SWIFT_SAIL_ITEM) return COLLECT_ICON_SWIFT_SAIL;
    if (item == item_data::dItemNo_SAIL_e) return COLLECT_ICON_SAIL;
    return COLLECT_ICON_NONE;
}

/* 0262A714 */
u32 hdBtnCollectIcon_c::windWakerIcon() {
    return collectSavePlayer().mPlayerItem.mItems[2] == item_data::dItemNo_WIND_WAKER_e ? COLLECT_ICON_WIND_WAKER : COLLECT_ICON_NONE;
}

/* 0262A734 */
u32 hdBtnCollectIcon_c::tingleIcon() {
    return collectIsOwned(&collectSavePlayer().mCollect, 2, 0) ? COLLECT_ICON_TINGLE : COLLECT_ICON_NONE;
}

/* 0262A784 */
void hdBtnCollectIcon_c::setIconVisible(u32 visible) {
    if (u32(mIconVisible) == visible) return;
    gptr<hdCollectPane_c>* panes = mIconPanes.mpPanes;
    mIconVisible = visible;
    hdCollectPane_c* pane = panes[0];
    hdPartsPaneSRT_c* flags = reinterpret_cast<hdPartsPaneSRT_c*>(pane);
    flags->mFlags = (u32(flags->mFlags) & ~1u) | visible;
    hdCollectButtonVtable_c* table = reinterpret_cast<hdCollectButtonVtable_c*>(mVtable.get());
    gabi::call_ptr<void>(table->mRefreshVisibility.get(), this);
}

/* 0262A7BC */
void hdBtnCollectIcon_c::updateIcon() {
    u32 icon = COLLECT_ICON_NONE;
    switch (mCollectionKind) {
    case COLLECT_KIND_SWORD: icon = swordIcon(); break;
    case COLLECT_KIND_SWORD_VARIANT: icon = swordVariantIcon(); break;
    case COLLECT_KIND_SHIELD: icon = shieldIcon(); break;
    case COLLECT_KIND_PIRATES_CHARM: icon = piratesCharmIcon(); break;
    case COLLECT_KIND_SAIL: icon = sailIcon(); break;
    case COLLECT_KIND_WIND_WAKER: icon = windWakerIcon(); break;
    case COLLECT_KIND_TINGLE: icon = tingleIcon(); break;
    }
    if (icon == COLLECT_ICON_NONE) { setIconVisible(0); return; }
    setIconVisible(1);
    if (u32(mIcon) != icon) setIcon(icon);
}

/* 0262A918 */
s32 hdBtnCollectIcon_c::itemNumber() {
    u32 icon = mIcon;
    return icon == COLLECT_ICON_NONE ? -1 : sCollectIcons.record(icon)->mItem;
}

/* 0262A938 */
s32 hdBtnCollectIcon_c::descriptionNumber() {
    u32 icon = mIcon;
    return icon == COLLECT_ICON_NONE ? -1 : sCollectIcons.record(icon)->mDescription;
}

static hdCollectUiSystemPointer_c collectUiSystem;
static hdCollectCollectAnimationIndicesArray_c collectAnimationIndices;
static hdCollectCollectGroupsArray_c collectGroups;
static hdUiSafeString collectAnimationNames;
extern void collectBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
extern void* collectAllocateLayout(u32, hdScreenHeap*, u32);
/* 0262A95C */
u32 hdBtnCollectIcon_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* descriptor) {
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(collectAllocateLayout(sizeof(hdPartsLayout_c), collectUiSystem.mpSystem->mHeap.get(), 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout) return 0;
    layout->vLoad(name, first, second, third, 6, 1, descriptor);
    for (u32 index = 0; index < 1; ++index) {
        u32 animation = collectAnimationIndices[index];
        hdUiSafeString* animationName = hdPartsView<hdUiSafeString>(gabi::ea(&collectAnimationNames) + animation * sizeof(hdUiSafeString));
        collectBindAnimation(animSet(), COLLECT_ANIMATION_ICON_CHANGE + index, &collectGroups[index], animationName);
    }
    return 1;
}

static hdCollectStaticAngles_c collectAngles;
static hdCollectZeroVector_c collectZeroVector;
static hdCollectCollectRegistrationsArray_c collectRegistrations;
static hdCollectCollectAngleConstantsArray_c collectAngleConstants;
static hdUiSafeString collectLayoutName;
static hdUiSafeString collectPaneName;
static hdUiStringTable collectStringVtable;
static hdPartsStateId_c collectIconChangeState;
static hdPartsStateIdVtbl_c collectStateVtable;
static hdStateId collectNullState;
static hdCollectWord collectStateReady;
static hdCollectSignedWord collectStateCounter;
extern void collectRegister(hdCollectGlobalRegistration_c*);
extern void collectConstructFirst(u8*);
extern void collectConstructSecond(u8*);
/* 0262AA40 */
void collectIconStaticInit() {
    collectZeroVector.mWords[2] = 0;
    collectZeroVector.mWords[0] = 0;
    collectZeroVector.mWords[3] = 0;
    collectZeroVector.mWords[1] = 0;
    collectRegister(&collectRegistrations[0]);
    f32 low = collectAngleConstants[0], high = collectAngleConstants[1];
    collectAngles.mLow = low; collectAngles.mHigh = high;
    collectConstructFirst(&collectAngles.mFirst);
    collectRegister(&collectRegistrations[1]);
    collectConstructSecond(&collectAngles.mSecond);
    collectRegister(&collectRegistrations[2]);
    hdParts_setSafeString(&collectLayoutName, &collectStringVtable, "BtnCollectIcon_00");
    hdParts_setSafeString(&collectPaneName, &collectStringVtable, "P_Btn_00");
    hdParts_setSafeString(&collectAnimationNames, &collectStringVtable, "G_Btn_00");
    hdParts_setSafeString(&collectGroups[0], &collectStringVtable, "IconChange");
    s32 number;
    if (collectStateReady) number = s32(u32(collectStateCounter) + 1);
    else { number = 1; collectStateReady = 1; }
    collectStateCounter = number;
    collectIconChangeState.setup(number, "::StateID_IconChange", &collectStateVtable, 0x40, &collectNullState);
}

static hdScreenType collectPaneType;
static hdScreenType collectParentType;
static hdScreenType collectRootType;
static hdCollectWord collectPaneTypeReady;
static hdCollectWord collectParentTypeReady;
static hdCollectWord collectRootTypeReady;
extern u32 collectInitBase(hdParts_c*, hdUiSafeString*, u32, u32, hdPartsDesc_c*);
extern hdScreenHeap* collectCurrentHeap(void*);
/* 0262A130 */
u32 hdBtnCollectIcon_c::init(u32 first, u32 second, hdPartsDesc_c* descriptor) {
    if (!collectInitBase(this, &collectLayoutName, first, second, descriptor)) return 0;
    hdScreenHeap* heap = collectUiSystem.mpSystem->mHeap;
    if (!heap) heap = collectCurrentHeap(sCollectHeapManager.mpManager);
    void* memory = heap->allocate(sizeof(gptr<hdCollectPane_c>));
    if (memory) { mIconPanes.mpPanes = static_cast<gptr<hdCollectPane_c>*>(memory); mIconPanes.mCount = 1; }
    s32 count = mIconPanes.mCount;
    if (count > 0) {
        for (s32 index = 0; index < count; ++index)
            *hdPartsView<gptr<hdCollectPane_c>>(gabi::ea(mIconPanes.mpPanes.get()) + u32(index) * 4) = NULL;
        count = mIconPanes.mCount;
    }
    for (s32 index = 0; index < count; ++index) {
        u32 assure = collectPaneName.mVtable->mAssureTermination.get();
        hdPartsLyt_c* layout = mpLayout->lyt();
        u32 slotAddress = gabi::ea(mIconPanes.mpPanes.get());
        if (u32(index) < u32(count)) slotAddress += u32(index) * sizeof(gptr<hdCollectPane_c>);
        hdUiSafeString* name = hdPartsView<hdUiSafeString>(gabi::ea(&collectPaneName) + u32(index) * sizeof(hdUiSafeString));
        gabi::call_ptr<void>(assure, name);
        hdScreenResourceObject* root = reinterpret_cast<hdScreenResourceObject*>(layout->mpRootPane.get());
        hdPartsPane_c* pane = static_cast<hdPartsPane_c*>(root->findPane(reinterpret_cast<const char*>(collectPaneName.mText.get())));
        if (!collectPaneTypeReady) {
            u32 parentReady = collectParentTypeReady;
            collectPaneTypeReady = 1;
            if (!parentReady) {
                u32 rootReady = collectRootTypeReady;
                collectParentTypeReady = 1;
                if (!rootReady) { collectRootType.mParent = NULL; collectRootTypeReady = 1; }
                collectParentType.mParent = &collectRootType;
            }
            collectPaneType.mParent = &collectParentType;
        }
        if (pane) {
            hdScreenType* type = pane->getRuntimeTypeInfo();
            while (type && type != &collectPaneType) type = type->mParent;
            if (!type) pane = NULL;
        }
        *hdPartsView<gptr<hdCollectPane_c>>(slotAddress) = reinterpret_cast<hdCollectPane_c*>(pane);
        count = mIconPanes.mCount;
    }
    return 1;
}

/* WWHD glue (tools/verify/autoglue.py)
data collectPaneType 101FD90C
data collectParentType 101FD910
data collectRootType 101FD8FC
data collectPaneTypeReady 101FD908
data collectParentTypeReady 101FD898
data collectRootTypeReady 101FD888
ext 0270178C u32 collectInitBase(hdParts_c*,hdUiSafeString*,u32,u32,hdPartsDesc_c*)
ext 02756140 hdScreenHeap* collectCurrentHeap(void*)

data collectAngles 1048F48C
data collectZeroVector 1048F498
data collectRegistrations 101F5410
data collectAngleConstants 100E6390
data collectLayoutName 1048F4D8
data collectPaneName 1048F484
data collectStringVtable 100E6250
data collectIconChangeState 1048F4A8
data collectStateVtable 100E62B0
data collectNullState 101FF32C
data collectStateReady 101FD8E4
data collectStateCounter 101FDD50
ext 028F026C void collectRegister(hdCollectGlobalRegistration_c*)
ext 028ED6F8 void collectConstructFirst(u8*)
ext 028EAB2C void collectConstructSecond(u8*)
str 100E6398 "BtnCollectIcon_00" in 0262AA40
str 100E63C4 "P_Btn_00" in 0262AA40
str 100E63AC "G_Btn_00" in 0262AA40
str 100E63B8 "IconChange" in 0262AA40
str 100E63D0 "::StateID_IconChange" in 0262AA40

data collectUiSystem 1018C404
data collectAnimationIndices 100E624C
data collectGroups 1048F47C
data collectAnimationNames 1048F474
ext 02004E04 void collectBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 0273B050 void* collectAllocateLayout(u32,hdScreenHeap*,u32)
data sCollectActiveState 1049E53C
data sCollectOffState 1049E65C
data sCollectIcons 100E62F8
data sCollectVtable 100E63E8
data sCollectHolderVtable 100E6600
data sCollectSecondaryVtable 100E6610
data sCollectHeapManager 101F8B4C
ext 0200552C void collectSetAnimationFrame(hdAnimSet*,u32,u32,f32)
ext 020053E4 void collectPlayAnimation(hdAnimSet*,u32,u32,f32)
ext 02701718 hdParts_c* hdPartsNoDecideButton_construct(hdParts_c*)
ext 02700C9C void collectDestroyMessageButton(hdParts_c*,u32)
ext 026F5CCC void hdButtonPart_c::setPaneEnabled(u32)
ext 025B7A2C u32 collectIsOwned(dSv_player_collect_c*,u32,u32)
ext 02755FEC hdScreenHeap* collectFindHeap(void*,void*)
ext 02701984 void collectReleaseBase(hdParts_c*)
*/
