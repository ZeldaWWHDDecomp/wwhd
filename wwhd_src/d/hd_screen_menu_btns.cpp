/* HD menu button screen and its mapping value constructors. */
#include "d/hd_menu_button_mapping.h"
#include "d/hd_screen_menu_btns.h"

typedef hdMenuButtonEntry* (*MenuEntryConstructor)(hdMenuButtonEntry*);
typedef void (*MenuEntryDestructor)(hdMenuButtonEntry*, u32);
extern void* menuAllocate(u32);
extern void menuDelete(void*);
extern void menuConstructArray(void*, u32, u32, MenuEntryConstructor);
extern void menuDestroyArray(void*, u32, u32, MenuEntryDestructor, u32, u32);

/* 0268CDF0 */
static hdMenuButtonEntry* constructMenuButtonEntry(hdMenuButtonEntry* entry) {
    if (entry == NULL) {
        entry = static_cast<hdMenuButtonEntry*>(menuAllocate(sizeof(hdMenuButtonEntry)));
        if (entry == NULL) {
            return NULL;
        }
    }
    entry->mButton = -1;
    entry->mSlot = hdMenuButtonMapping::NO_SLOT;
    entry->mFlags = 0;
    entry->mEnabled = 0;
    return entry;
}

/* 0268CE40 */
static void destroyMenuButtonEntry(hdMenuButtonEntry* entry, u32 flags) {
    if (entry == NULL) {
        return;
    }
    entry->mButton = -1;
    entry->mSlot = hdMenuButtonMapping::NO_SLOT;
    entry->mEnabled = 0;
    entry->mFlags = 0;
    if ((flags & 1) != 0) {
        menuDelete(entry);
    }
}

/* 0268CE70 */
static hdMenuButtonMapping* constructMenuButtonMapping(hdMenuButtonMapping* mapping) {
    if (mapping == NULL) {
        mapping = static_cast<hdMenuButtonMapping*>(menuAllocate(sizeof(hdMenuButtonMapping)));
        if (mapping == NULL) {
            return NULL;
        }
    }
    mapping->mSlot = hdMenuButtonMapping::NO_SLOT;
    mapping->mEntryCount = 0;
    hdMenuButtonEntry* entries = mapping->mEntries;
    if (gabi::ea(entries) == 0) {
        entries = static_cast<hdMenuButtonEntry*>(menuAllocate(sizeof(mapping->mEntries)));
        if (entries == NULL) {
            return mapping;
        }
    }
    menuConstructArray(entries, hdMenuButtonMapping::ENTRY_COUNT, sizeof(hdMenuButtonEntry), constructMenuButtonEntry);
    return mapping;
}

/* 0268CEEC */
static void destroyMenuButtonMapping(hdMenuButtonMapping* mapping, u32 flags) {
    if (mapping == NULL) {
        return;
    }
    mapping->mSlot = hdMenuButtonMapping::NO_SLOT;
    mapping->mEntryCount = 0;
    for (u32 i = 0; i < hdMenuButtonMapping::ENTRY_COUNT; ++i) {
        hdMenuButtonEntry* entry = &mapping->mEntries[i];
        entry->mFlags = 0;
        entry->mEnabled = 0;
        entry->mSlot = hdMenuButtonMapping::NO_SLOT;
        entry->mButton = -1;
    }
    menuDestroyArray(mapping->mEntries, hdMenuButtonMapping::ENTRY_COUNT, sizeof(hdMenuButtonEntry), destroyMenuButtonEntry, 0, 0);
    if ((flags & 1) != 0) {
        menuDelete(mapping);
    }
}

struct MenuScreenInput_l { u8 _header[0x30]; be<u32> mFocus; };
struct MenuScreenSave_l { u8 _header[0x63]; be<u8> mStage; u8 _stageTail[0x70]; u8 mCollect; };
WWHD_OFFSET(MenuScreenSave_l, mCollect, 0xD4);
struct MenuScreenUiState_l { u8 _header[0x14]; be<u8> mUseAlternateFocus; };
struct MenuScreenSystem_l { u8 _header[0x224]; be<u32> mMapMode; };
struct MenuScreenFocus_l { gptr<void> mpData; be<u32> mKey; u8 _tail[0x20]; };
struct MenuScreenHeapVtable_l { u8 _header[0x34]; gfn mAllocate; u8 _allocateTail[4]; gfn mFree; };
struct MenuScreenHeap_l { u8 _header[0xC]; gptr<MenuScreenHeapVtable_l> mVtable; };
struct MenuScreenHeapManager_l { u8 _header[0x10]; gptr<MenuScreenHeap_l> mpHeap; };
struct MenuScreenDelegate_l {
    u8 _header[0x7C]; gfn mLoad;
    u8 _loadTail[0xC]; gfn mUpdateLayout;
    u8 _updateTail[0x14]; gfn mCreateChildren;
    u8 _createTail[0xC]; gfn mExecuteChildren;
    u8 _executeTail[0x14]; gfn mUpdateButton;
};
WWHD_OFFSET(MenuScreenDelegate_l, mUpdateLayout, 0x8C);
WWHD_OFFSET(MenuScreenDelegate_l, mCreateChildren, 0xA4);
WWHD_OFFSET(MenuScreenDelegate_l, mExecuteChildren, 0xB4);
WWHD_OFFSET(MenuScreenDelegate_l, mUpdateButton, 0xCC);
struct MenuScreenStringArray_l { hdUiSafeString mValues[4]; };
static MenuScreenInput_l* sMenuScreenInput;
static MenuScreenSave_l* sMenuScreenSave;
static MenuScreenUiState_l* sMenuScreenUiState;
static MenuScreenSystem_l* sMenuScreenSystem;
static MenuScreenHeapManager_l* sMenuScreenHeapManager;
static void* sMenuScreenHeapSystem;
static hdPartsStateId_c sMenuDisplayState;
static hdPartsStateId_c sMenuBlankState;
static hdPartsStateId_c sMenuOpenState;
static hdPartsStateId_c sMenuCloseState;
static u8 sMenuScreenVtable;
static u8 sMenuScreenOwnerVtable;
static u8 sMenuScreenSecondaryVtable;
static MenuScreenStringArray_l sMenuPaneNames;
extern u32 menuCurrentFocus(MenuScreenInput_l*);
extern u32 menuAlternateFocus(MenuScreenInput_l*);
extern void menuSelectFocus(MenuScreenInput_l*, u32);
extern void menuConstructFocus(MenuScreenFocus_l*, be<u32>*);
extern u32 menuFocusIsButton(void*);
extern u32 menuFocusIsAlternate(MenuScreenFocus_l*);
extern u32 menuFocusCanSelect(MenuScreenFocus_l*);
extern void menuPrepareFocus(hdMenuButton_c*);
extern u32 menuSaveHasCollect(void*, u32, u32);
extern void menuPlayAnimation(hdAnimSet*, u32, u32, f32);
extern u32 menuAnimationFinished(hdAnimSet*, u32);
extern void menuSetAnimationFrame(hdAnimSet*, u32, u32);
extern void menuResetAnimation(hdAnimSet*, u32, u32);
extern void menuExecuteStateMachine(hdStateMachine*);
extern void menuChangeState(hdStateMachine*, hdPartsStateId_c*);
extern void menuConstructScreenBase(hdMenuButtons*);
extern void menuDestroyScreenBase(hdMenuButtons*, u32);
extern void menuFinalizeScreenBase(hdMenuButtons*);
extern void menuCalculateScreenBase(hdMenuButtons*);
extern MenuScreenHeap_l* menuDefaultHeap(void*);
extern MenuScreenHeap_l* menuFindHeap(void*, void*);
extern u32 menuFindPanes(hdPartsLayout_c*, hdMenuButtonPaneArray*, MenuScreenStringArray_l*);

static MenuScreenDelegate_l* menuScreenDelegate(hdParts_c* self) {
    return reinterpret_cast<MenuScreenDelegate_l*>(self->mVtable.get());
}
static void updateMenuScreenLayout(hdMenuButtons* self) {
    gabi::call_ptr(menuScreenDelegate(self)->mUpdateLayout.get(), self);
}
static void updateMenuChildButton(hdMenuButton_c* child) {
    gabi::call_ptr(menuScreenDelegate(&child->mButton)->mUpdateButton.get(), child);
}

/* 0268CFC0 */
void hdMenuButtons::destroy(u32 flags) {
    if (this == NULL) return;
    mStateHolder.mHolderVtable = &sMenuScreenOwnerVtable;
    mSecondaryVtable = &sMenuScreenSecondaryVtable;
    menuDestroyScreenBase(this, 0);
    if ((flags & 1) != 0) menuDelete(this);
}

/* 0268D02C */
u32 hdMenuButtons::hasFocus() {
    if (menuCurrentFocus(sMenuScreenInput) == 0) return 1;
    for (u32 i = 0; i < 3; ++i) {
        hdMenuButton_c* child = button(i);
        u32 focus = menuCurrentFocus(sMenuScreenInput);
        if (focus == child->mButton.getReceiverId()) return 1;
    }
    return 0;
}

/* 0268D108 */
u32 hdMenuButtons::canAcceptInput() { return mEnabled != 0 && hasFocus() != 0 ? 1 : 0; }

/* 0268D360 */
u32 hdMenuButtons::hasMenuMap() { return menuSaveHasCollect(&sMenuScreenSave->mCollect, 3, 0) != 0 ? 1 : 0; }

/* 0268D39C */
u32 hdMenuButtons::hasDungeonMap() { return sMenuScreenSave->mStage == 0x21 ? 1 : 0; }

/* 0268D3B8 */
u32 hdMenuButtons::availableMaps() {
    u32 available = hasMenuMap() != 0 ? 1 : 0;
    if (hasDungeonMap() != 0) available |= 2;
    return available;
}

/* 0268D414 */
void hdMenuButtons::revealAvailableMaps() {
    u32 available = availableMaps();
    if (u32(mAvailability) == available) return;
    for (u32 i = 0; i < 2; ++i) {
        u32 bit = 1u << i;
        if ((available & bit) != 0 && (u32(mAvailability) & bit) == 0) {
            hdMenuPane_l* pane = mPanes.get(i + 2);
            pane->mFlags = (u8(pane->mFlags) & 0xFE) + 1;
        }
    }
    mAvailability = available;
}

/* 0268D4B4 */
void hdMenuButtons::updateButtons() {
    if (canAcceptInput() == 0) return;
    updateMenuChildButton(button(0));
    u32 available = mAvailability;
    if ((available & MENU_MAP_AVAILABLE) != 0) {
        updateMenuChildButton(button(1));
        available = mAvailability;
    }
    if ((available & DUNGEON_MAP_AVAILABLE) != 0) updateMenuChildButton(button(2));
}

/* 0268D570 */
void hdMenuButtons::displayExecute() {
    updateFocus();
    revealAvailableMaps();
    updateButtons();
    gabi::call_ptr(menuScreenDelegate(this)->mExecuteChildren.get(), this);
    updateMenuScreenLayout(this);
}

/* 0268D5D4 */
void hdMenuButtons::refreshAvailability() {
    u32 available = availableMaps();
    for (u32 i = 0; i < 2; ++i) {
        hdMenuPane_l* pane = mPanes.get(i + 2);
        u8 flags = u8(pane->mFlags) & 0xFE;
        pane->mFlags = (available & (1u << i)) != 0 ? flags + 1 : flags;
    }
    mAvailability = available;
}

/* 0268D68C */
void hdMenuButtons::inAllEnter() { refreshAvailability(); menuPlayAnimation(mpLayout->animSet(), 0, 0, 1.0f); }
/* 0268D6D4 */
void hdMenuButtons::inAllExecute() {
    if (menuAnimationFinished(mpLayout->animSet(), 0) != 0) menuChangeState(stateMachine(), &sMenuDisplayState);
    updateMenuScreenLayout(this);
}
/* 0268D738 */
void hdMenuButtons::outAllEnter() { refreshAvailability(); menuPlayAnimation(mpLayout->animSet(), 1, 1, 1.0f); }
/* 0268D780 */
void hdMenuButtons::outAllExecute() {
    if (menuAnimationFinished(mpLayout->animSet(), 0) != 0) menuChangeState(stateMachine(), &sMenuDisplayState);
    updateMenuScreenLayout(this);
}
/* 0268D7E4 */
void hdMenuButtons::menuOpenEnter() { menuPlayAnimation(mpLayout->animSet(), 2, 1, 1.0f); }
/* 0268D800 */
void hdMenuButtons::menuOpenExecute() {
    if (menuAnimationFinished(mpLayout->animSet(), 2) != 0) menuChangeState(stateMachine(), &sMenuBlankState);
    updateMenuScreenLayout(this);
}
/* 0268D864 */
void hdMenuButtons::menuCloseEnter() { menuPlayAnimation(mpLayout->animSet(), 3, 0, 1.0f); }
/* 0268D880 */
void hdMenuButtons::menuCloseExecute() {
    if (menuAnimationFinished(mpLayout->animSet(), 3) != 0) menuChangeState(stateMachine(), &sMenuBlankState);
    updateMenuScreenLayout(this);
}

/* 0268D8E4 */
hdMenuButtons* hdMenuButtons::construct(hdMenuButtons* self) {
    if (self == NULL) {
        self = static_cast<hdMenuButtons*>(menuAllocate(sizeof(hdMenuButtons)));
        if (self == NULL) return NULL;
    }
    menuConstructScreenBase(self);
    self->mVtable = reinterpret_cast<hdPartsVtbl_c*>(&sMenuScreenVtable);
    self->mStateHolder.mHolderVtable = &sMenuScreenOwnerVtable;
    self->mSecondaryVtable = &sMenuScreenSecondaryVtable;
    hdMenuButtonPaneArray* panes = &self->mPanes;
    if (gabi::ea(panes) == 0) panes = static_cast<hdMenuButtonPaneArray*>(menuAllocate(sizeof(hdMenuButtonPaneArray)));
    if (panes != NULL) { panes->mpData = NULL; panes->mCount = 0; }
    be<u32>* available = &self->mAvailability;
    if (gabi::ea(available) == 0) available = static_cast<be<u32>*>(menuAllocate(sizeof(be<u32>)));
    if (available != NULL) *available = 0;
    self->mEnabled = 0;
    self->mSelected = NO_SELECTION;
    return self;
}

/* 0268D9A4 */
u32 hdMenuButtons::createPanes() {
    MenuScreenHeap_l* heap = sMenuScreenHeapManager->mpHeap;
    if (heap == NULL) heap = menuDefaultHeap(sMenuScreenHeapSystem);
    void* allocation = gabi::call_ptr<void*>(heap->mVtable->mAllocate.get(), heap, 0x10, 4);
    if (allocation != NULL) { mPanes.mpData = static_cast<gptr<hdMenuPane_l>*>(allocation); mPanes.mCount = 4; }
    s32 count = mPanes.mCount;
    for (s32 i = 0; i < count; ++i) mPanes.mpData.get()[i] = NULL;
    return menuFindPanes(mpLayout.get(), &mPanes, &sMenuPaneNames);
}

/* 0268DA54 */
void hdMenuButtons::initializeButtons() {
    button(0)->mOrder = 0;
    button(1)->mOrder = 1;
    button(2)->mOrder = 2;
    refreshAvailability();
}

/* 0268DAB0 */
u32 hdMenuButtons::buttonKey(u32 index) {
    if (index == 1 && (u32(mAvailability) & MENU_MAP_AVAILABLE) == 0) return 0;
    if (index == 2 && (u32(mAvailability) & DUNGEON_MAP_AVAILABLE) == 0) return 0;
    return gabi::ea(button(index)->mButton.mpReceiver.get());
}

/* 0268DD80 */
void hdMenuButtons::releasePanes() {
    s32 count = mPanes.mCount;
    if (count > 0) {
        for (s32 i = 0; i < count; ++i) mPanes.mpData.get()[i] = NULL;
    } else if (mPanes.mpData.get() == NULL) return;
    MenuScreenHeap_l* heap = menuFindHeap(sMenuScreenHeapSystem, mPanes.mpData.get());
    gabi::call_ptr(heap->mVtable->mFree.get(), heap, mPanes.mpData.get());
    mPanes.mCount = 0;
    mPanes.mpData = NULL;
}

/* 0268DE20 */
void hdMenuButtons::finalize() { releasePanes(); menuFinalizeScreenBase(this); menuCalculateScreenBase(this); }
/* 0268DE5C */
void hdMenuButtons::executeState() { mSelected = NO_SELECTION; menuExecuteStateMachine(stateMachine()); }
/* 0268DE6C */
void hdMenuButtons::show() {
    menuResetAnimation(mpLayout->animSet(), 0, 0);
    updateMenuScreenLayout(this);
    menuChangeState(stateMachine(), &sMenuDisplayState);
}
/* 0268DECC */
void hdMenuButtons::openMenu() { menuChangeState(stateMachine(), &sMenuOpenState); }
/* 0268DEDC */
void hdMenuButtons::closeMenu() { menuChangeState(stateMachine(), &sMenuCloseState); }

/* 0268E270 */
void hdMenuButtons::executeChildren() {
    button(0)->mButton.vExecuteState();
    u32 available = mAvailability;
    if ((available & MENU_MAP_AVAILABLE) != 0) { button(1)->mButton.vExecuteState(); available = mAvailability; }
    if ((available & DUNGEON_MAP_AVAILABLE) != 0) button(2)->mButton.vExecuteState();
}

struct MenuScreenIndexArray_l { be<u32> mValues[4]; };
struct MenuScreenDescriptor_l {
    u8 _header[8];
    hdUiSafeString mClass;
    u8 _classTail[4];
    hdUiSafeString mPane;
};
WWHD_OFFSET(MenuScreenDescriptor_l, mPane, 0x14);
static MenuScreenIndexArray_l sMenuFocusOrder;
static MenuScreenIndexArray_l sMenuAnimationGroups;
static MenuScreenStringArray_l sMenuChildNames;
static MenuScreenStringArray_l sMenuChildClasses;
static MenuScreenStringArray_l sMenuGroupNames;
static MenuScreenStringArray_l sMenuAnimationNames;
static MenuScreenStringArray_l sMenuArchives;
static hdUiSafeString sMenuLayoutName;
static hdUiSafeString sMenuButtonClass;
extern void menuAttachArchives(hdMenuButtons*, MenuScreenStringArray_l*, u32);
extern void* menuAllocateOnHeap(u32, MenuScreenHeap_l*, u32);
extern hdMenuButton_c* menuConstructButton(hdMenuButton_c*);
extern u32 menuCreateButton(hdMenuButton_c*, u32, u32, hdPartsDesc_c*);
extern u32 menuCreateOtherChild(hdMenuButtons*, u32, hdPartsDesc_c*);
extern void menuBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);

/* 0268D150 */
void hdMenuButtons::updateFocus() {
    MenuScreenInput_l* input = sMenuScreenInput;
    if (input == NULL) return;
    MenuScreenFocus_l focus;
    menuConstructFocus(&focus, &input->mFocus);
    if (menuFocusIsButton(focus.mpData.get()) != 0) {
        for (u32 i = 0; i < 3; ++i) {
            hdMenuButton_c* child = button(sMenuFocusOrder.mValues[i]);
            if (child != NULL) {
                u32 key = child->mButton.getReceiverId();
                if (u32(focus.mKey) == key) { mSelected = i; return; }
            }
        }
        return;
    }
    if (menuFocusIsAlternate(&focus) != 0) {
        if (sMenuScreenUiState->mUseAlternateFocus == 0) return;
        for (u32 i = 0; i < 3; ++i) {
            hdMenuButton_c* child = button(sMenuFocusOrder.mValues[i]);
            if (child != NULL) {
                u32 key = menuAlternateFocus(sMenuScreenInput);
                if (key == gabi::ea(child->mButton.mpReceiver.get())) { mSelected = i; return; }
            }
        }
        return;
    }
    if (menuFocusCanSelect(&focus) == 0 || canAcceptInput() == 0 || (u32(mAvailability) & 1) == 0) return;
    u32 mode = sMenuScreenSystem->mMapMode;
    u32 selected = mode == 1 || mode == 2 ? 0 : 1;
    hdMenuButton_c* child = button(sMenuFocusOrder.mValues[selected]);
    if (child == NULL) return;
    menuPrepareFocus(child);
    menuSelectFocus(sMenuScreenInput, gabi::ea(child->mButton.mpReceiver.get()));
    mSelected = selected;
}

/* 0268DB18 */
void hdMenuButtons::connectButtons() {
    hdMenuButton_c* first = button(0);
    u32 firstRight = buttonKey(1);
    first->mButton.setReceiverButtons(0, 0, 0, 0, 0, 0, 0, firstRight);
    hdMenuButton_c* second = button(1);
    u32 secondLeft = buttonKey(0);
    u32 secondRight = buttonKey(2);
    second->mButton.setReceiverButtons(0, 0, 0, 0, 0, 0, secondLeft, secondRight);
    hdMenuButton_c* third = button(2);
    u32 thirdLeft = buttonKey(1);
    third->mButton.setReceiverButtons(0, 0, 0, 0, 0, 0, thirdLeft, 0);
}

/* 0268DC34 */
u32 hdMenuButtons::create(void* first, void* second, void* third) {
    menuAttachArchives(this, &sMenuArchives, 2);
    u32 loaded = gabi::call_ptr<u32>(menuScreenDelegate(this)->mLoad.get(), this, &sMenuLayoutName, mpResAccessor.get(), first, second, third);
    if (loaded == 0 || gabi::call_ptr<u32>(menuScreenDelegate(this)->mCreateChildren.get(), this) == 0 || createPanes() == 0) return 0;
    initializeButtons();
    connectButtons();
    menuSetAnimationFrame(mpLayout->animSet(), 0, 0);
    updateMenuScreenLayout(this);
    hdStateOwner* owner = stateMachine()->mpOwner;
    hdStateObject* state = owner->createState(reinterpret_cast<hdStateId*>(&sMenuBlankState));
    stateMachine()->mpCurrentState = state;
    state->enter();
    return 1;
}

static bool menuStringsEqual(hdUiSafeString* source, hdUiSafeString* expected) {
    source->assureTermination();
    source->assureTermination();
    be<u8>* text = source->mText;
    expected->assureTermination();
    if (text == expected->mText.get()) return true;
    text = source->mText;
    be<u8>* expectedText = expected->mText;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 value = text[i];
        if (value != u8(expectedText[i])) return false;
        if (value == 0) return true;
    }
    return false;
}

/* 0268DEEC */
u32 hdMenuButtons::createChild(hdPartsDesc_c* descriptor) {
    MenuScreenDescriptor_l* resource = reinterpret_cast<MenuScreenDescriptor_l*>(descriptor);
    MenuScreenHeap_l* heap = sMenuScreenHeapManager->mpHeap;
    for (u32 i = 0; i < 3; ++i) {
        if (!menuStringsEqual(&resource->mPane, &sMenuChildNames.mValues[i])) continue;
        if (!menuStringsEqual(&resource->mClass, &sMenuButtonClass)) return menuCreateOtherChild(this, i, descriptor);
        gptr<hdParts_c>* slot = mChildren.safeSlot(i);
        hdMenuButton_c* child = static_cast<hdMenuButton_c*>(menuAllocateOnHeap(0x9C, heap, 4));
        if (child != NULL) child = menuConstructButton(child);
        *slot = child != NULL ? &child->mButton : NULL;
        if (mChildren.safeSlot(i)->get() == NULL) return 0;
        child = button(i);
        return menuCreateButton(child, 0, 0, descriptor);
    }
    return 1;
}

/* 0268E144 */
u32 hdMenuButtons::loadLayout(void* name, void* accessor, u32 first, u32 second, void* descriptor) {
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(menuAllocateOnHeap(0x110, sMenuScreenHeapManager->mpHeap.get(), 4));
    if (layout != NULL) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (layout == NULL || layout->vLoad(name, gabi::ea(accessor), first, second, 4, 2, descriptor) == 0) return 0;
    for (u32 i = 0; i < 4; ++i) {
        u32 group = sMenuAnimationGroups.mValues[i];
        menuBindAnimation(mpLayout->animSet(), i, &sMenuAnimationNames.mValues[i], &sMenuGroupNames.mValues[group]);
    }
    updateMenuScreenLayout(this);
    return 1;
}

struct MenuScreenStaticValues_l {
    be<f32> mNegativePi;
    be<f32> mPositivePi;
    be<f32> mLargeBounds[2];
    be<f32> mSmallBounds[2];
    be<u8> mRandom;
    be<u8> mUtility;
    u8 _padding[2];
    be<u32> mStorage[4];
};
struct MenuScreenRegistration_l { u8 mStorage[12]; };
static MenuScreenStaticValues_l sMenuScreenValues;
static MenuScreenRegistration_l sScreenStorageRegistration;
static MenuScreenRegistration_l sScreenRandomRegistration;
static MenuScreenRegistration_l sScreenUtilityRegistration;
static hdPartsStateId_c sMenuInAllState;
static hdPartsStateId_c sMenuOutAllState;
static hdStateId sMenuDisplayParentState;
static hdStateId sMenuNullState;
static hdPartsStateIdVtbl_c sMenuStateVtable;
static hdUiStringTable sMenuStringVtable;
struct MenuScreenCounter_l { be<u32> mValue; };
static MenuScreenCounter_l sMenuStateCounter;
static MenuScreenCounter_l sMenuStateCounterInitialized;
extern void menuRegisterStatic(MenuScreenRegistration_l*);
extern void menuConstructRandom(be<u8>*);
extern void menuConstructUtility(be<u8>*);

static void initializeMenuString(hdUiSafeString* string, const char* text) {
    string->mText.v = gabi::guest_addr(text);
    string->mVtable = &sMenuStringVtable;
}

/* 0268E318 */
static void initializeMenuButtonScreenStatics() {
    for (s32 i = 3; i >= 0; --i) sMenuScreenValues.mStorage[i] = 0;
    menuRegisterStatic(&sScreenStorageRegistration);
    sMenuScreenValues.mNegativePi = -3.1415927410125732f;
    sMenuScreenValues.mPositivePi = 3.1415927410125732f;
    menuConstructRandom(&sMenuScreenValues.mRandom);
    menuRegisterStatic(&sScreenRandomRegistration);
    menuConstructUtility(&sMenuScreenValues.mUtility);
    menuRegisterStatic(&sScreenUtilityRegistration);
    sMenuGroupNames.mValues[0].mVtable = &sMenuStringVtable;
    sMenuScreenValues.mLargeBounds[1] = 50000.0f;
    sMenuScreenValues.mSmallBounds[1] = 10000.0f;
    sMenuChildNames.mValues[0].mVtable = &sMenuStringVtable;
    initializeMenuString(&sMenuAnimationNames.mValues[0], "InAll");
    initializeMenuString(&sMenuAnimationNames.mValues[1], "MenuOpen");
    sMenuAnimationNames.mValues[2].mVtable = &sMenuStringVtable;
    sMenuGroupNames.mValues[0].mText.v = gabi::guest_addr("G_All_00");
    sMenuScreenValues.mLargeBounds[0] = 50000.0f;
    sMenuAnimationNames.mValues[2].mText.v = gabi::guest_addr("MenuClose");
    initializeMenuString(&sMenuAnimationNames.mValues[3], "OutAll");
    initializeMenuString(&sMenuGroupNames.mValues[1], "G_MenuOpen_00");
    sMenuChildNames.mValues[0].mText.v = gabi::guest_addr("L_BtnMenu_00");
    initializeMenuString(&sMenuChildNames.mValues[1], "L_BtnMenu_01");
    sMenuChildNames.mValues[2].mVtable = &sMenuStringVtable;
    initializeMenuString(&sMenuArchives.mValues[0], "MenuBtns_00");
    initializeMenuString(&sMenuArchives.mValues[1], "BtnMenu_00");
    sMenuScreenValues.mSmallBounds[0] = 10000.0f;
    initializeMenuString(&sMenuChildNames.mValues[2], "L_BtnMenu_02");
    sMenuChildClasses.mValues[0].mText.v = gabi::guest_addr("BtnMenu_00");
    sMenuChildClasses.mValues[0].mVtable = &sMenuStringVtable;
    initializeMenuString(&sMenuChildClasses.mValues[1], "BtnMenu_00");
    initializeMenuString(&sMenuChildClasses.mValues[2], "BtnMenu_00");
    initializeMenuString(&sMenuPaneNames.mValues[0], "N_All_00");
    initializeMenuString(&sMenuPaneNames.mValues[1], "N_MenuBtn_00");
    initializeMenuString(&sMenuPaneNames.mValues[2], "N_MenuBtn_01");
    initializeMenuString(&sMenuPaneNames.mValues[3], "N_MenuBtn_02");
    initializeMenuString(&sMenuLayoutName, "MenuBtns_00");
    u32 stateNumber;
    if (sMenuStateCounterInitialized.mValue != 0) stateNumber = sMenuStateCounter.mValue;
    else { stateNumber = 0; sMenuStateCounterInitialized.mValue = 1; }
    sMenuDisplayState.setup(++stateNumber, "::StateID_Display", &sMenuStateVtable, 7, &sMenuDisplayParentState);
    sMenuInAllState.setup(++stateNumber, "::StateID_InAll", &sMenuStateVtable, 25, &sMenuNullState);
    sMenuOpenState.setup(++stateNumber, "::StateID_MenuOpen", &sMenuStateVtable, 28, &sMenuNullState);
    sMenuCloseState.setup(++stateNumber, "::StateID_MenuClose", &sMenuStateVtable, 31, &sMenuNullState);
    ++stateNumber;
    sMenuStateCounter.mValue = stateNumber;
    sMenuOutAllState.setup(stateNumber, "::StateID_OutAll", &sMenuStateVtable, 34, &sMenuNullState);
}


/* WWHD glue (tools/verify/autoglue.py): allocation and compiler array helpers
 data sMenuScreenInput 101F4FF0
 data sMenuScreenSave 101F84DC
 data sMenuScreenUiState 101F8378
 data sMenuScreenSystem 101F8344
 data sMenuScreenHeapManager 1018C404
 data sMenuScreenHeapSystem 101F8B4C
 data sMenuDisplayState 104966E4
 data sMenuBlankState 1049DD60
 data sMenuOpenState 10496744
 data sMenuCloseState 10496774
 data sMenuScreenVtable 100F9FF8
 data sMenuScreenOwnerVtable 100FA120
 data sMenuScreenSecondaryVtable 100FA130
 data sMenuPaneNames 104966C4
 ext 02615398 u32 menuCurrentFocus(MenuScreenInput_l*)
 ext 02615140 u32 menuAlternateFocus(MenuScreenInput_l*)
 ext 02615154 void menuSelectFocus(MenuScreenInput_l*,u32)
 ext 020013C0 void menuConstructFocus(MenuScreenFocus_l*,be<u32>*)
 ext 026F61DC u32 menuFocusIsButton(void*)
 ext 026F6288 u32 menuFocusIsAlternate(MenuScreenFocus_l*)
 ext 0263C69C u32 menuFocusCanSelect(MenuScreenFocus_l*)
 ext 0263C654 void menuPrepareFocus(hdMenuButton_c*)
 ext 025B7A2C u32 menuSaveHasCollect(void*,u32,u32)
 ext 020053E4 void menuPlayAnimation(hdAnimSet*,u32,u32,f32)
 ext 02005840 u32 menuAnimationFinished(hdAnimSet*,u32)
 ext 02005674 void menuSetAnimationFrame(hdAnimSet*,u32,u32)
 ext 02005708 void menuResetAnimation(hdAnimSet*,u32,u32)
 ext 02006364 void menuExecuteStateMachine(hdStateMachine*)
 ext 020063C0 void menuChangeState(hdStateMachine*,hdPartsStateId_c*)
 ext 026F89E8 void menuConstructScreenBase(hdMenuButtons*)
 ext 026F88F0 void menuDestroyScreenBase(hdMenuButtons*,u32)
 ext 026F8B00 void menuFinalizeScreenBase(hdMenuButtons*)
 ext 026F91F8 void menuCalculateScreenBase(hdMenuButtons*)
 ext 02756140 MenuScreenHeap_l* menuDefaultHeap(void*)
 ext 02755FEC MenuScreenHeap_l* menuFindHeap(void*,void*)
 ext 02634088 u32 menuFindPanes(hdPartsLayout_c*,hdMenuButtonPaneArray*,MenuScreenStringArray_l*)
 data sMenuFocusOrder 100F9E1C
 data sMenuAnimationGroups 100F9E0C
 data sMenuChildNames 10496684
 data sMenuChildClasses 1049669C
 data sMenuGroupNames 10496654
 data sMenuAnimationNames 10496664
 data sMenuArchives 104966B4
 data sMenuLayoutName 104967D4
 data sMenuButtonClass 104906AC
 ext 026F90D8 void menuAttachArchives(hdMenuButtons*,MenuScreenStringArray_l*,u32)
 ext 0273B050 void* menuAllocateOnHeap(u32,MenuScreenHeap_l*,u32)
 ext 0263BF68 hdMenuButton_c* menuConstructButton(hdMenuButton_c*)
 ext 0263C25C u32 menuCreateButton(hdMenuButton_c*,u32,u32,hdPartsDesc_c*)
 ext 026F92C0 u32 menuCreateOtherChild(hdMenuButtons*,u32,hdPartsDesc_c*)
 ext 02004E04 void menuBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
 data sMenuScreenValues 10496628
 data sScreenStorageRegistration 101F6374
 data sScreenRandomRegistration 101F6380
 data sScreenUtilityRegistration 101F638C
 data sMenuInAllState 10496714
 data sMenuOutAllState 104967A4
 data sMenuDisplayParentState 1049DD90
 data sMenuNullState 101FF32C
 data sMenuStateVtable 100F9E70
 data sMenuStringVtable 100F9DF4
 data sMenuStateCounter 101FDD50
 data sMenuStateCounterInitialized 101FD8E4
 str 100F9F0C "G_MenuOpen_00"
 str 100F9ECC "InAll"
 str 100F9ED4 "OutAll"
 str 100F9F1C "MenuClose"
 str 100F9F3C "G_All_00"
 str 100F9F48 "MenuOpen"
 str 100F9F54 "L_BtnMenu_00"
 str 100F9F64 "L_BtnMenu_01"
 str 100F9F74 "L_BtnMenu_02"
 str 100F9F84 "BtnMenu_00"
 str 100F9F90 "N_All_00"
 str 100F9F9C "N_MenuBtn_00"
 str 100F9FAC "N_MenuBtn_01"
 str 100F9FBC "N_MenuBtn_02"
 str 100F9EEC "MenuBtns_00"
 str 100F9F28 "::StateID_Display"
 str 100F9EDC "::StateID_InAll"
 str 100F9FCC "::StateID_MenuOpen"
 str 100F9EF8 "::StateID_MenuClose"
 str 100F9FE0 "::StateID_OutAll"
 ext 028F026C void menuRegisterStatic(MenuScreenRegistration_l*)
 ext 028ED6F8 void menuConstructRandom(be<u8>*)
 ext 028EAB2C void menuConstructUtility(be<u8>*)
 ext 0273AD10 void* menuAllocate(u32)
 ext 0273AF40 void menuDelete(void*)
 ext 028EFFD0 void menuConstructArray(void*, u32, u32, MenuEntryConstructor)
 ext 028F0164 void menuDestroyArray(void*, u32, u32, MenuEntryDestructor, u32, u32)
 ext 026F5B74 u32 hdButtonPart_c::getReceiverId()
 ext 026F5C08 void hdButtonPart_c::setReceiverButtons(u32,u32,u32,u32,u32,u32,u32,u32)
*/
