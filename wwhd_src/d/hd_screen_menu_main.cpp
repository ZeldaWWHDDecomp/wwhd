/* HD main-menu tab, option-window and save-screen control. */
#include "d/hd_screen_menu_main.h"
#include "d/hd_input_eventmgr.h"
#include "d/hd_input_ctrl.h"
#include "d/hd_parts_text_pane.h"
#include "d/hd_screen_menu_item.h"
#include "d/hd_screen_collection.h"
#include "d/hd_btn_map_icon_number.h"
// Native guest pointers already carry the translated address; preserve NULL when viewing another type.
template <class T, class U> static T* menuMainView(U* object) { return reinterpret_cast<T*>(object); }
template <class T> static T* menuMainView(u32 address) { return hdPartsView<T>(address); }
static u8 sMenuMainBaseHolderVtable;
static u8 sMenuMainBaseSecondaryVtable;
static hdPartsStateId_c sMenuMainInAll;
static void destroyMenuMainParts(hdParts_c* self, s32 flags);
static void deleteMenuMainParts(void* self);

/* 0268E798 */
static u32 menuMainStateKey(hdPartsStateId_c* state) { return u32(s32(state->mNumber)); }
/* 0268E898 */
static void destroyMenuMainBase(hdParts_c* self, s32 flags) {
    if (self == NULL) return;
    self->mStateHolder.mHolderVtable = &sMenuMainBaseHolderVtable;
    self->mSecondaryVtable = &sMenuMainBaseSecondaryVtable;
    destroyMenuMainParts(self, 0);
    if (flags & 1) deleteMenuMainParts(self);
}
/* 0268E904 */
void hdMenuMain_c::requestOpen() { mOpenRequested = 1; }
/* 0268F920 */
void hdMenuMain_c::clearTabInput() { mInputBlocked = 0; }
/* 0268FFB8 */
void hdMenuMain_c::clearWindowInput() { mInputBlocked = 0; }
/* 026917BC */
void hdMenuMain_c::clearOptionInput() { mInputBlocked = 0; }
/* 02691A90 */
void hdMenuMain_c::clearSaveInput() { mInputBlocked = 0; }
/* 02692308 */
void hdMenuMain_c::calculateTabLayout() { vCalcLayout(); }
/* 02692390 */
void hdMenuMain_c::calculateOptionLayout() { vCalcLayout(); }
/* 026924DC */
void hdMenuMain_c::calculateSaveLayout() { vCalcLayout(); }
/* 026941AC */
void hdMenuMain_c::requestInAll() { changeState(&sMenuMainInAll); }
/* 0269498C */
u32 hdMenuMain_c::isActive() { return mActive; }
/* 02694994 */
void hdMenuMain_c::setContext(void* context, u32 first, u32 second) {
    mContextFirst = first;
    mContextSecond = second;
    mpContext = context;
}

static hdPartsStateId_c sMenuMainWindowClosing;
static void playMenuMainSound(const char* sound);
static void executeMenuMainWindowTransition(hdMenuMain_c* self);
static void finishMenuMainChildren(hdMenuMain_c* self);
static void finishMenuMainSubscreens(hdMenuMain_c* self);
static void deleteMenuMainChildren(hdParts_c* self);
static void detachMenuMainArchives(hdParts_c* self);
static void executeMenuMainChildren(hdMenuMain_c* self);
static void executeMenuMainParts(hdParts_c* self);
/* 02691FA4 */
void hdMenuMain_c::resetTransitionProgress() { mTransitionProgress = 0.0f; }
/* 02692210 */
void hdMenuMain_c::enterTabAnimation() { hdAnimSet_play(animSet(), 1, 0, 1.0f); }
/* 026923A0 */
void hdMenuMain_c::enterOptionAnimation() { hdAnimSet_play(animSet(), 6, 3, 1.0f); }
/* 02692420 */
void hdMenuMain_c::leaveOptionAnimation() { hdAnimSet_play(animSet(), 7, 3, 1.0f); }
/* 026924C0 */
void hdMenuMain_c::enterSaveAnimation() { hdAnimSet_play(animSet(), 8, 4, 1.0f); }
/* 02690CA0 */
void hdMenuMain_c::beginWindowTransition() {
    hdAnimSet_operationCC(animSet(), 8, 4);
    mTransitionFinished = 0;
}
/* 0268F92C */
void hdMenuMain_c::closeOptionWindow() {
    playMenuMainSound("SE_MENU_MINI_WINDOW_CLOSE");
    changeState(&sMenuMainWindowClosing);
}
/* 026910E4 */
void hdMenuMain_c::advanceWindowTransition() { executeMenuMainWindowTransition(this); }
/* 02693A84 */
void hdMenuMain_c::finishMenuResources() {
    finishMenuMainChildren(this);
    finishMenuMainSubscreens(this);
    deleteMenuMainChildren(this);
    detachMenuMainArchives(this);
}
/* 026972F0 */
void hdMenuMain_c::updateMenuResources() {
    executeMenuMainChildren(this);
    executeMenuMainParts(this);
}
/* 0268F210 */
u32 hdMenuMain_c::isMainTabItem(u32 item) {
    return item == 0 || (item >= 2 && item <= 0x10) || (item >= 0x14 && item <= 0x1B);
}
/* 02697324 */
u32 hdMenuMain_c::isBagTabItem(u32 item) {
    return (item >= 0x11 && item <= 0x13) || (item >= 0x1D && item <= 0x1E) || item == 0x28;
}

struct hdMenuMainButtonIndices_c { be<u32> mEntries[43]; };
static hdMenuMainButtonIndices_c sMenuMainButtonIndices;
static hdPartsStateId_c sMenuMainSelectionNormal;
static hdPartsStateId_c sMenuMainSelectionAlternate;
using hdMenuMainInputPointer = gptr<hdButtonEventManager_l>;
static hdMenuMainInputPointer sMenuMainInputOwner;
static u32 getMenuMainButtonKey(hdParts_c* child);
static u32 lookupMenuMainButtonIndex(hdMenuMain_c* self, u32 key);
static u32 lookupMenuMainChildIndex(hdMenuMain_c* self, u32 key);
static void setMenuMainChildEnabled(hdParts_c* child, u32 enabled);
static void updateMenuMainInput(void* input, u32 first, u32 second);
static void resetMenuMainChild(hdParts_c* child);
static void selectMenuMainInput(void* input, void* selection);
static void updateMenuMainCollectIcon(hdParts_c* child);
using hdMenuMainInputChild_l = hdButtonPart_c;
/* 0268E910 */
u32 hdMenuMain_c::findButtonIndex(u32 key) {
    for (u32 index = 0; index < 43; ++index) {
        const u32 childIndex = sMenuMainButtonIndices.mEntries[index];
        hdParts_c* child = *mChildren.safeSlot(childIndex);
        if (child != NULL && getMenuMainButtonKey(child) == key) return index;
    }
    return 43;
}
/* 0268E99C */
u32 hdMenuMain_c::findButtonChildIndex(u32 key) {
    const u32 index = lookupMenuMainButtonIndex(this, key);
    if (index == 43) return 46;
    // External index comes from the called search, preserving guest index arithmetic.
    return *gabi::at<be<u32>>(gabi::ea(&sMenuMainButtonIndices) + index * sizeof(be<u32>));
}
/* 0268E9D8 */
u32 hdMenuMain_c::isSelectableItem(u32 item) {
    return item == 0 || (item >= 2 && item <= 0x10) || (item >= 0x14 && item <= 0x1B) || (item >= 0x2A && item <= 0x2C);
}
/* 0268EC54 */
void hdMenuMain_c::setSelectionMode(u32 mode) {
    mSelectionMode = mode;
    if (mode == 0) changeState(&sMenuMainSelectionNormal);
    else if (mode == 1) changeState(&sMenuMainSelectionAlternate);
}
/* 0268EE78 */
void hdMenuMain_c::updateTabChild(u32 index, u32 enabled) {
    hdParts_c* child = *mTabChildren.safeSlot(index);
    setMenuMainChildEnabled(child, enabled);
}
/* 02690854 */
void hdMenuMain_c::updateSpecialButton(u32 key) {
    if (lookupMenuMainChildIndex(this, key) == 0x1D) updateMenuMainInput(sMenuMainInputOwner.get(), 0, 8);
}
/* 02691BB8 */
void hdMenuMain_c::activateOptionChild() {
    gptr<hdParts_c>* ownerSlot = mChildren.safeSlot(0x25);
    mActive = 1;
    mPending = 0;
    hdParts_c* owner = *ownerSlot;
    hdParts_c* child = *owner->mChildren.safeSlot(3);
    resetMenuMainChild(child);
}
/* 02692938 */
void hdMenuMain_c::updateInputOwner() {
    hdMenuMainInputChild_l* child = menuMainView<hdMenuMainInputChild_l>((*mChildren.safeSlot(0x14)).get());
    void* input = sMenuMainInputOwner.get();
    selectMenuMainInput(input, child->mpReceiver.get());
}
/* 02694500 */
void hdMenuMain_c::updateCollectIcon() { updateMenuMainCollectIcon(*mChildren.safeSlot(0x2D)); }

static hdPartsStateId_c sMenuMainWindowOpening;
static hdPartsStateId_c sMenuMainPrimaryReady;
static hdPartsStateId_c sMenuMainPendingReady;
static hdPartsStateId_c sMenuMainSecondaryReady;
static hdPartsStateId_c sMenuMainPrimaryHidden;
static hdPartsStateId_c sMenuMainSecondaryHidden;
static u32 isMenuMainCollectBlocked(hdParts_c* child);
static u32 isMenuMainSelectionBlocked(hdMenuMain_c* self);
static void refreshMenuMainWindow(hdMenuMain_c* self);
static void updateMenuMainButtonFocus(hdMenuMain_c* self);
static void updateMenuMainButtonSelection(hdMenuMain_c* self);
static void updateMenuMainWindowInput(hdMenuMain_c* self);
static void updateMenuMainTabInput(hdMenuMain_c* self);
static void updateMenuMainWindowChildren(hdMenuMain_c* self);
static void updateMenuMainWindowFlags(hdMenuMain_c* self);
struct hdMenuMainTabVtable_l {
    u8 _00[0x23C];
    gfn mEnterTab;
};
WWHD_OFFSET(hdMenuMainTabVtable_l, mEnterTab, 0x23C);
static void enterMenuMainTab(hdParts_c* child) {
    hdMenuMainTabVtable_l* table = gabi::at<hdMenuMainTabVtable_l>(child->mVtable.v.get());
    gabi::call_ptr(table->mEnterTab.get(), child);
}
/* 0268EC88 */
void hdMenuMain_c::openWindow(u32 mode) {
    mWindowMode = mode;
    playMenuMainSound("SE_MENU_DECIDE");
    playMenuMainSound("SE_MENU_MINI_WINDOW_OPEN");
    changeState(&sMenuMainWindowOpening);
}
/* 0268F2FC */
u32 hdMenuMain_c::isCollectInputBlocked() {
    if (isMenuMainCollectBlocked(*mChildren.safeSlot(0x2D))) return 1;
    return isMenuMainSelectionBlocked(this) != 0;
}
/* 0268FF50 */
void hdMenuMain_c::executeWindow() {
    updateMenuMainButtonFocus(this);
    updateMenuMainButtonSelection(this);
    updateMenuMainWindowInput(this);
    updateMenuMainTabInput(this);
    updateMenuMainWindowChildren(this);
    updateMenuMainWindowFlags(this);
    vCalcLayout();
}
/* 02690804 */
void hdMenuMain_c::openSaveAnimation() {
    mOpenRequested = 1;
    refreshMenuMainWindow(this);
    hdAnimSet_play(animSet(), 8, 4, 1.0f);
}
/* 02690CE4 */
void hdMenuMain_c::enterPrimaryTab() {
    enterMenuMainTab(*mChildren.safeSlot(0x28));
    hdAnimSet_play(animSet(), 2, 1, 1.0f);
}
/* 02690D50 */
void hdMenuMain_c::executePrimaryTab() {
    if (hdAnimSet_isEnd(animSet(), 2)) changeState(mPending ? &sMenuMainPendingReady : &sMenuMainPrimaryReady);
    vCalcLayout();
}
/* 02690DC8 */
void hdMenuMain_c::enterSecondaryTab() {
    enterMenuMainTab(*mChildren.safeSlot(0x11));
    hdAnimSet_play(animSet(), 3, 2, 1.0f);
}
/* 02690E34 */
void hdMenuMain_c::executeSecondaryTab() {
    if (hdAnimSet_isEnd(animSet(), 3)) changeState(&sMenuMainSecondaryReady);
    vCalcLayout();
}
/* 02690E98 */
void hdMenuMain_c::leavePrimaryTab() {
    selectMenuMainInput(sMenuMainInputOwner.get(), NULL);
    hdAnimSet_play(animSet(), 4, 1, 1.0f);
}
/* 02690EEC */
void hdMenuMain_c::executePrimaryTabExit() {
    if (hdAnimSet_isEnd(animSet(), 4)) changeState(&sMenuMainPrimaryHidden);
    vCalcLayout();
}
/* 02690F50 */
void hdMenuMain_c::leaveSecondaryTab() {
    selectMenuMainInput(sMenuMainInputOwner.get(), NULL);
    hdAnimSet_play(animSet(), 5, 2, 1.0f);
}
/* 02690FA4 */
void hdMenuMain_c::executeSecondaryTabExit() {
    if (hdAnimSet_isEnd(animSet(), 5)) changeState(&sMenuMainSecondaryHidden);
    vCalcLayout();
}

static hdPartsStateId_c sMenuMainBlank;
static hdPartsStateId_c sMenuMainOptionReady;
static hdPartsStateId_c sMenuMainWindowReady;
static hdPartsStateId_c sMenuMainSelectedChildClosing;
static void updateMenuMainItemSelection(hdMenuMain_c* self, u32 mode);
static void resetMenuMainOptionChild(hdParts_c* child);
static u32 isMenuMainCollectActionBlocked(hdParts_c* child);
static u32 isMenuMainActionBlocked(hdMenuMain_c* self);
/* 0269222C */
void hdMenuMain_c::executeTabAnimation() {
    if (hdAnimSet_isEnd(animSet(), 1)) changeState(&sMenuMainBlank);
    vCalcLayout();
}
/* 02692290 */
void hdMenuMain_c::enterBothTabAnimations() {
    hdAnimSet_play(animSet(), 2, 1, 1.0f);
    hdAnimSet_play(animSet(), 3, 2, 1.0f);
}
/* 02692318 */
void hdMenuMain_c::leaveBothTabAnimations() {
    hdAnimSet_play(animSet(), 4, 1, 1.0f);
    hdAnimSet_play(animSet(), 5, 2, 1.0f);
}
/* 026923BC */
void hdMenuMain_c::executeOptionAnimation() {
    if (hdAnimSet_isEnd(animSet(), 6)) changeState(&sMenuMainOptionReady);
    vCalcLayout();
}
/* 0269243C */
void hdMenuMain_c::executeOptionExitAnimation() {
    if (hdAnimSet_isEnd(animSet(), 7)) changeState(u32(mWindowMode) <= 2 ? &sMenuMainWindowReady : &sMenuMainSecondaryReady);
    vCalcLayout();
}
/* 026928D0 */
void hdMenuMain_c::executeSaveAnimation() {
    if (hdAnimSet_isEnd(animSet(), u32(mWindowMode) + 9)) changeState(&sMenuMainWindowReady);
    vCalcLayout();
}
/* 02692960 */
void hdMenuMain_c::leaveSaveAnimation() {
    selectMenuMainInput(sMenuMainInputOwner.get(), NULL);
    hdAnimSet* animations = animSet();
    const u32 animation = u32(mWindowMode) + 12;
    hdAnimSet_play(animations, animation, 5, 1.0f);
}
/* 026917C8 */
void hdMenuMain_c::selectCurrentChild() {
    hdMenuMainInputChild_l* child = menuMainView<hdMenuMainInputChild_l>((*mChildren.safeSlot(mSelectedChild)).get());
    if (child != NULL) {
        void* selection = child->mpReceiver.get();
        selectMenuMainInput(sMenuMainInputOwner.get(), selection);
        changeState(&sMenuMainSelectedChildClosing);
        playMenuMainSound("SE_MENU_MINI_WINDOW_CLOSE");
    }
}
/* 02691A9C */
void hdMenuMain_c::resetOptionChild() {
    updateMenuMainItemSelection(this, 0);
    hdParts_c* owner = *mChildren.safeSlot(0x25);
    resetMenuMainOptionChild(*owner->mChildren.safeSlot(3));
}
/* 02691BFC */
void hdMenuMain_c::beginWindowExit() {
    const u32 mode = mSelectionMode;
    hdPartsLayout_c* layout = mpLayout;
    mWindowOffset = -1280.0f;
    hdAnimSet* animations = layout->animSet();
    if (mode == 0) hdAnimSet_play(animations, 5, 2, 1.0f);
    else hdAnimSet_play(animations, 4, 1, 1.0f);
}
/* 02693E50 */
u32 hdMenuMain_c::isCollectActionBlocked() {
    if (isMenuMainCollectActionBlocked(*mChildren.safeSlot(0x2D))) return 1;
    return isMenuMainActionBlocked(this) != 0;
}

static void executeMenuMainWindowTransitionEntry(hdMenuMain_c* self);
static void startMenuMainWindowTransition(hdMenuMain_c* self);
static void executeMenuMainTransitionChildren(hdMenuMain_c* self);
static void updateMenuMainTransitionChildren(hdMenuMain_c* self);
static u32 hitTestMenuMainPane(hdMenuPane_l* pane, hdMenuPaneVector* point);
static void linkMenuMainTab(hdParts_c* child, void* link);
static void updateMenuMainTabLinks(hdMenuMain_c* self);
static u32 isMenuMainCollectWindowBlocked(hdParts_c* child);
static u32 isMenuMainWindowBlocked(hdMenuMain_c* self);
struct hdMenuMainTabLink_l : hdParts_c {
    u8 _50[0x23C - 0x50];
    u8 mLink;
};
WWHD_OFFSET(hdMenuMainTabLink_l, mLink, 0x23C);
/* 02691308 */
void hdMenuMain_c::beginWindowSlide() {
    executeMenuMainWindowTransitionEntry(this);
    startMenuMainWindowTransition(this);
    hdMenuPane_l* pane = *mPanes.slot(0);
    const float distance = gabi::fsubs_ppc(-1280.0f, pane->mTranslationX);
    mTransitionProgress = distance / 3.0f;
}
/* 026916B4 */
void hdMenuMain_c::beginWindowSlideBack() {
    executeMenuMainTransitionChildren(this);
    vCalcLayout();
    updateMenuMainTransitionChildren(this);
    hdMenuPane_l* pane = *mPanes.slot(0);
    const float step = float(pane->mTranslationX) / 3.0f;
    mTransitionProgress = -step;
}
/* 02691364 */
void hdMenuMain_c::addPaneTranslation(float delta) {
    hdMenuPane_l* pane = *mPanes.slot(0);
    const float y = pane->mTranslationY;
    const float x = pane->mTranslationX;
    const float z = pane->mTranslationZ;
    const float result = gabi::fadds_ppc(x, delta);
    pane = *mPanes.slot(0);
    pane->mTranslationX = result;
    const u8 flags = pane->mFlags;
    pane->mTranslationY = y;
    pane->mFlags = flags | 0x10;
    pane->mTranslationZ = z;
    pane = *mPanes.safeSlot(1);
    pane->mTranslationX = result;
    const u8 secondFlags = pane->mFlags;
    pane->mTranslationY = y;
    pane->mFlags = secondFlags | 0x10;
    pane->mTranslationZ = z;
}
/* 026913EC */
void hdMenuMain_c::setPaneTranslation(float x) {
    hdMenuPane_l* pane = *mPanes.slot(0);
    const float y = pane->mTranslationY;
    const float z = pane->mTranslationZ;
    pane = *mPanes.slot(0);
    pane->mTranslationX = x;
    const u8 flags = pane->mFlags;
    pane->mTranslationY = y;
    pane->mFlags = flags | 0x10;
    pane->mTranslationZ = z;
    pane = *mPanes.safeSlot(1);
    pane->mTranslationX = x;
    const u8 secondFlags = pane->mFlags;
    pane->mTranslationY = y;
    pane->mFlags = secondFlags | 0x10;
    pane->mTranslationZ = z;
}
/* 02691844 */
u32 hdMenuMain_c::hitTestCursorPane(hdMenuPaneVector* point) {
    const u32 count = mPanes.mCount;
    gptr<hdMenuPane_l>* panes = mPanes.slot(0);
    hdMenuPaneVector value;
    value.y = point->y;
    value.x = point->x;
    gptr<hdMenuPane_l>* slot = menuMainView<gptr<hdMenuPane_l>>(gabi::ea(panes) + (count > 5 ? 5 : 0) * sizeof(gptr<hdMenuPane_l>));
    hdMenuPane_l* pane = *slot;
    return hitTestMenuMainPane(pane, &value);
}
/* 02691A0C */
void hdMenuMain_c::linkWindowTab() {
    gptr<hdParts_c>* slot = mTabChildren.safeSlot(3);
    hdParts_c* child = *slot;
    hdMenuMainTabLink_l* linkOwner = menuMainView<hdMenuMainTabLink_l>((*slot).get());
    linkMenuMainTab(child, &linkOwner->mLink);
    updateMenuMainTabLinks(this);
    vCalcLayout();
}
/* 026946DC */
u32 hdMenuMain_c::isCollectWindowBlocked() {
    if (isMenuMainCollectWindowBlocked(*mChildren.safeSlot(0x2D))) return 1;
    return isMenuMainWindowBlocked(this) != 0;
}

static u32 isMenuMainMainTabItem(hdMenuMain_c* self, u32 item);
static u32 isMenuMainItemSelected(hdParts_c* child);
static u32 isMenuMainItemActivated(hdParts_c* child);
static void refreshMenuMainOptionChild(hdParts_c* child);
static void addMenuMainPaneTranslation(hdMenuMain_c* self, float delta);
static void setMenuMainPaneTranslation(hdMenuMain_c* self, float x);
static hdPartsStateId_c sMenuMainWindowSlid;
static hdPartsStateId_c sMenuMainChildOpening;
static void updateMenuMainSelectedItem(hdMenuMain_c* self, u32 key);
static u32 getMenuMainChildMessage(hdParts_c* child);
static void updateMenuMainTabChild(hdMenuMain_c* self, u32 index, u32 enabled);
struct hdMenuMainRefreshVtable_l {
    hdPartsVtbl_c mBase;
    u8 _C8[4];
    gfn mRefresh;
};
WWHD_OFFSET(hdMenuMainRefreshVtable_l, mRefresh, 0xCC);
/* 0268F248 */
u32 hdMenuMain_c::anyMainItemSelected() {
    for (u32 index = 0; index < 28; ++index) {
        if (!isMenuMainMainTabItem(this, index)) continue;
        hdParts_c* child = *mChildren.safeSlot(index);
        if (child != NULL && isMenuMainItemSelected(child)) return 1;
    }
    return 0;
}
/* 0268F36C */
u32 hdMenuMain_c::anyMainItemActivated() {
    for (u32 index = 0; index < 28; ++index) {
        if (!isMenuMainMainTabItem(this, index)) continue;
        hdParts_c* child = *mChildren.safeSlot(index);
        if (child != NULL && isMenuMainItemActivated(child)) return 1;
    }
    return 0;
}
/* 02690974 */
void hdMenuMain_c::refreshMappedChildren() {
    if (!mInputEnabled) return;
    for (u32 index = 31; index < 43; ++index) {
        hdParts_c* child = *mChildren.safeSlot(sMenuMainButtonIndices.mEntries[index]);
        if (child != NULL) {
            hdMenuMainRefreshVtable_l* table = gabi::at<hdMenuMainRefreshVtable_l>(child->mVtable.v.get());
            gabi::call_ptr(table->mRefresh.get(), child);
        }
    }
    refreshMenuMainOptionChild(*mChildren.safeSlot(0x25));
}
/* 02690A3C */
void hdMenuMain_c::executeTabChildren() {
    for (u32 index = 29; index < 42; ++index) {
        hdParts_c* child = *mChildren.safeSlot(index);
        if (child != NULL) child->vExecuteState();
    }
}
/* 02691474 */
void hdMenuMain_c::executeWindowSlide() {
    addMenuMainPaneTranslation(this, mTransitionProgress);
    hdMenuPane_l* pane = *mPanes.slot(0);
    if (!(float(pane->mTranslationX) > -1280.0f)) {
        setMenuMainPaneTranslation(this, -1280.0f);
        hdMenuMainInputChild_l* child = menuMainView<hdMenuMainInputChild_l>((*mChildren.safeSlot(0x28)).get());
        void* input = sMenuMainInputOwner.get();
        selectMenuMainInput(input, child->mpReceiver.get());
        changeState(&sMenuMainWindowSlid);
    }
    vCalcLayout();
}
/* 0268F060 */
void hdMenuMain_c::openSelectedChild(u32 index) {
    hdParts_c* child = *mChildren.safeSlot(index);
    if (child == NULL) return;
    mSelectedChild = index;
    const u32 key = getMenuMainButtonKey(child);
    updateMenuMainSelectedItem(this, key);
    const u32 message = getMenuMainChildMessage(child);
    updateMenuMainTabChild(this, 3, message);
    selectMenuMainInput(sMenuMainInputOwner.get(), NULL);
    changeState(&sMenuMainChildOpening);
    playMenuMainSound("SE_MENU_DECIDE_OPEN_S");
}

struct hdMenuMainPlay_l {
    u8 _00[0x5C28];
    be<u8> mMenuRequestedItem;
    be<u8> mMenuRequestState;
};
WWHD_OFFSET(hdMenuMainPlay_l, mMenuRequestedItem, 0x5C28);
WWHD_OFFSET(hdMenuMainPlay_l, mMenuRequestState, 0x5C29);
static Pair32 classifyMenuMainSelection(hdMenuMain_c* self, u32 index);
static hdMenuMainPlay_l* getMenuMainPlay();
static u32 getMenuMainItemValue(hdParts_c* child);
static void resetMenuMainTabChild(hdMenuMain_c* self, u32 mode);
static void copyMenuMainEvent(hdButtonEvent_l* destination, hdButtonEvent_l* source);
static hdButtonReceiver_l* isMenuMainReceiverEvent(hdButtonEventManager_l* input);
/* 0268EA20 */
void hdMenuMain_c::updatePlayMenuRequest(u32 key) {
    const u32 index = lookupMenuMainChildIndex(this, key);
    // The classification leaf returns Boolean r3 and preserves its index argument in r4.
    auto classification = classifyMenuMainSelection(this, index);
    if (classification.r3) {
        hdMenuItem_c* child = menuMainView<hdMenuItem_c>((*mChildren.safeSlot(classification.r4)).get());
        if (child != NULL) {
            const u8 enabled = child->mVisible;
            hdMenuMainPlay_l* play = getMenuMainPlay();
            play->mMenuRequestedItem = enabled ? 0x21 : 0;
            getMenuMainPlay()->mMenuRequestState = 7;
        }
    } else {
        getMenuMainPlay()->mMenuRequestedItem = 0x17;
        getMenuMainPlay()->mMenuRequestState = 7;
    }
}
/* 0268EFBC */
void hdMenuMain_c::updateSelectedItem(u32 key) {
    const u32 index = lookupMenuMainChildIndex(this, key);
    auto classification = classifyMenuMainSelection(this, index);
    if (!classification.r3) return;
    hdMenuItem_c* child = menuMainView<hdMenuItem_c>((*mChildren.safeSlot(classification.r4)).get());
    if (child == NULL) return;
    if (child->mVisible) {
        const u32 value = getMenuMainItemValue(menuMainView<hdParts_c>(child));
        updateMenuMainTabChild(this, 0, value);
    } else resetMenuMainTabChild(this, 0);
}
/* 02693D34 */
u32 hdMenuMain_c::isReceiverCancelEvent() {
    hdButtonEvent_l event;
    copyMenuMainEvent(&event, &sMenuMainInputOwner->mCurrentEvent);
    if (!isMenuMainReceiverEvent(sMenuMainInputOwner.get())) return 0;
    const u32 type = event.mType;
    return type == 0x47 || type == 0x45;
}
/* 02694624 */
u32 hdMenuMain_c::isSelectedWindowEvent() {
    hdButtonEventManager_l* input = sMenuMainInputOwner;
    if (input == NULL) return 0;
    hdButtonEvent_l event;
    copyMenuMainEvent(&event, &input->mCurrentEvent);
    if (u32(event.mType) - 0x43 > 5) return 0;
    const u32 index = lookupMenuMainChildIndex(this, event.mReceiverId);
    auto classification = classifyMenuMainSelection(this, index);
    if (!classification.r3) return 0;
    hdMenuItem_c* child = menuMainView<hdMenuItem_c>((*mChildren.safeSlot(classification.r4)).get());
    return child->mVisible != 0;
}

static hdPartsStateId_c sMenuMainWindowReturned;
static void setMenuMainSelectionMode(hdMenuMain_c* self, u32 mode);
/* 0269171C */
void hdMenuMain_c::executeWindowSlideBack() {
    addMenuMainPaneTranslation(this, mTransitionProgress);
    hdMenuPane_l* pane = *mPanes.slot(0);
    if (!(float(pane->mTranslationX) < 0.0f)) {
        setMenuMainPaneTranslation(this, 0.0f);
        hdMenuMainInputChild_l* child = menuMainView<hdMenuMainInputChild_l>((*mChildren.safeSlot(0x11)).get());
        void* input = sMenuMainInputOwner.get();
        selectMenuMainInput(input, child->mpReceiver.get());
        changeState(&sMenuMainWindowReturned);
    }
    vCalcLayout();
}
/* 02692150 */
void hdMenuMain_c::executeInitialTabAnimation() {
    if (hdAnimSet_isEnd(animSet(), 0)) {
        const u32 mode = mSelectionMode;
        if (mode == 0) {
            setMenuMainPaneTranslation(this, 0.0f);
            changeState(&sMenuMainWindowReturned);
        } else if (mode == 1) {
            setMenuMainPaneTranslation(this, -1280.0f);
            changeState(&sMenuMainWindowSlid);
        }
    }
    vCalcLayout();
}
/* 026929B8 */
void hdMenuMain_c::executeSaveExitAnimation() {
    if (hdAnimSet_isEnd(animSet(), u32(mWindowMode) + 12)) {
        if (mPending != 0) setMenuMainSelectionMode(this, 1);
        else changeState(&sMenuMainSecondaryReady);
    }
    vCalcLayout();
}

static u32 isMenuMainOptionChildFinished(hdParts_c* child);
static u32 isMenuMainItemActionBlocked(hdParts_c* child);
/* 02691B00 */
void hdMenuMain_c::executeOptionChild() {
    hdParts_c* owner = *mChildren.safeSlot(0x25);
    if (isMenuMainOptionChildFinished(*owner->mChildren.safeSlot(3)))
        changeState(&sMenuMainPrimaryReady);
    hdParts_c* child = *owner->mChildren.safeSlot(3);
    child->vExecuteState();
    vCalcLayout();
}
/* 02693D9C */
u32 hdMenuMain_c::anyMainItemActionBlocked() {
    for (u32 index = 0; index < 28; ++index) {
        if (!isMenuMainMainTabItem(this, index)) continue;
        hdParts_c* child = *mChildren.safeSlot(index);
        if (child != NULL && isMenuMainItemActionBlocked(child)) return 1;
    }
    return 0;
}

struct hdMenuMainTransitionItem_c {
    be<u32> mKey;
    be<u32> mChildIndex;
};
WWHD_OFFSET(hdMenuMainTransitionItem_c, mKey, 0);
WWHD_OFFSET(hdMenuMainTransitionItem_c, mChildIndex, 4);
static_assert(sizeof(hdMenuMainTransitionItem_c) == 8, "menu transition item stride");
struct hdMenuMainTransitionItems_c { hdMenuMainTransitionItem_c mItems[19]; };
static hdMenuMainTransitionItems_c sMenuMainTransitionItems;
static u32 getMenuMainTransitionItemValue(u32 key);
static void setMenuMainTransitionItem(hdParts_c* child, u32 key, u32 value);
static void refreshMenuMainTransitionItem(hdParts_c* child);
/* 02691514 */
void hdMenuMain_c::executeTransitionChildren() {
    for (u32 index = 0; index < 19; ++index) {
        const u32 key = sMenuMainTransitionItems.mItems[index].mKey;
        const u32 value = getMenuMainTransitionItemValue(key);
        const u32 childIndex = sMenuMainTransitionItems.mItems[index].mChildIndex;
        setMenuMainTransitionItem(*mChildren.safeSlot(childIndex), key, value);
        refreshMenuMainTransitionItem(*mChildren.safeSlot(childIndex));
    }
}

using hdMenuMainInputModePointer = gptr<cking::ui::input::Options>;
static hdMenuMainInputModePointer sMenuMainInputMode;
static u32 isMenuMainInputFocusChanged(hdButtonEventManager_l* input);
static hdButtonReceiver_l* getMenuMainInputReceiver(hdButtonEventManager_l* input);
static u32 isMenuMainFocusEvent(u32 type);
static void updateMenuMainPlayRequest(hdMenuMain_c* self, u32 key);
/* 0268EAE0 */
void hdMenuMain_c::updateButtonFocus() {
    if (!mInputEnabled) return;
    const u8 receiverMode = sMenuMainInputMode->mDirectionalInputEnabled;
    hdButtonEventManager_l* input = sMenuMainInputOwner.get();
    if (receiverMode != 0) {
        if (!isMenuMainInputFocusChanged(input)) return;
        hdButtonReceiver_l* receiver = getMenuMainInputReceiver(sMenuMainInputOwner.get());
        if (receiver != NULL) updateMenuMainPlayRequest(this, receiver->mId);
    } else {
        hdButtonEvent_l event;
        copyMenuMainEvent(&event, &input->mCurrentEvent);
        if (isMenuMainFocusEvent(event.mType)) updateMenuMainPlayRequest(this, event.mReceiverId);
    }
}

struct hdMenuMainEventPair_c {
    hdButtonEvent_l mIncoming;
    hdButtonEvent_l mSelection;
};
WWHD_OFFSET(hdMenuMainEventPair_c, mSelection, 0x28);
static_assert(sizeof(hdMenuMainEventPair_c) == 0x50, "menu event pair extent");
struct hdMenuMainSelectionEvents_c { be<u32> mTypes[43]; };
static hdMenuMainSelectionEvents_c sMenuMainSelectionEvents;
static u32 isMenuMainSelectionEvent(u32 type);
static u32 isMenuMainBagItem(hdMenuMain_c* self, u32 item);
static void constructMenuMainEvent(hdButtonEvent_l* event, u32 type, u32 receiver, u32 button);
static void queueMenuMainEvent(void* queue, hdButtonEvent_l* event);
static void destroyMenuMainEvent(hdButtonEvent_l* event);
/* 0268EB8C */
void hdMenuMain_c::updateButtonSelection() {
    hdButtonEventManager_l* input = sMenuMainInputOwner.get();
    if (input == NULL) return;
    hdMenuMainEventPair_c events;
    copyMenuMainEvent(&events.mIncoming, &input->mCurrentEvent);
    if (!isMenuMainSelectionEvent(events.mIncoming.mType)) return;
    const u32 index = lookupMenuMainButtonIndex(this, events.mIncoming.mReceiverId);
    if (index == 43) return;
    // The called lookup supplies the guest table index, including its wrapping arithmetic.
    const u32 childIndex = *gabi::at<be<u32>>(gabi::ea(&sMenuMainButtonIndices) + index * sizeof(be<u32>));
    if (!isMenuMainBagItem(this, childIndex)) return;
    const u32 type = *gabi::at<be<u32>>(gabi::ea(&sMenuMainSelectionEvents) + index * sizeof(be<u32>));
    constructMenuMainEvent(&events.mSelection, type, events.mIncoming.mReceiverId, 0);
    input = sMenuMainInputOwner.get();
    queueMenuMainEvent(&input->mQueue[0], &events.mSelection);
    destroyMenuMainEvent(&events.mSelection);
}

static hdScreenStringTable sMenuMainResourceNameVtable;
static void setMenuMainChildResourceName(hdParts_c* child, hdScreenString* name);
/* 0268FFC4 */
void hdMenuMain_c::setTabResourceName(u32 index) {
    const u32 count = u32(mTabChildren.mNum);
    hdPartsLayout_c* layout = mpLayout.get();
    hdPartsLyt_c* resource = layout->lyt();
    gptr<hdParts_c>* children = mTabChildren.mpData.get();
    const u32 text = hdButtonResourceName(resource);
    gptr<hdParts_c>* slot = children;
    if (index < count) slot = menuMainView<gptr<hdParts_c>>(gabi::ea(children) + index * sizeof(gptr<hdParts_c>));
    hdScreenString name;
    name.mText.v = text;
    name.mVtable = &sMenuMainResourceNameVtable;
    setMenuMainChildResourceName(*slot, &name);
}

static u32 getMenuMainCollectItemValue(hdParts_c* child, u32 key);
static void startMenuMainOptionTransition(hdParts_c* child);
static void startMenuMainItemTransition(hdParts_c* child);
static void startMenuMainBagTransition(hdParts_c* child);
static void startMenuMainTabTransition(hdParts_c* child);
static void startMenuMainSaveTransition(hdParts_c* child);
/* 0268F420 */
u32 hdMenuMain_c::getSelectedItemValue(u32 key) {
    const u32 index = lookupMenuMainChildIndex(this, key);
    auto classification = classifyMenuMainSelection(this, index);
    gptr<hdParts_c>* children = mChildren.mpData.get();
    const u32 count = u32(mChildren.mNum);
    if (classification.r3 != 0) {
        const u32 selected = classification.r4 < count ? classification.r4 : 0;
        gptr<hdParts_c>* slot = menuMainView<gptr<hdParts_c>>(gabi::ea(children) + selected * sizeof(gptr<hdParts_c>));
        hdMenuItem_c* child = menuMainView<hdMenuItem_c>((*slot).get());
        if (child->mVisible != 0) {
            child = menuMainView<hdMenuItem_c>((*slot).get());
            return child->mAnimationSlot;
        }
    }
    const u32 collectIndex = count > 0x2D ? 0x2D : 0;
    gptr<hdParts_c>* collectSlot = menuMainView<gptr<hdParts_c>>(gabi::ea(children) + collectIndex * sizeof(gptr<hdParts_c>));
    hdParts_c* collect = *collectSlot;
    return getMenuMainCollectItemValue(collect, key);
}
/* 02691008 */
void hdMenuMain_c::startWindowChildTransitions() {
    startMenuMainOptionTransition(*mChildren.safeSlot(0x25));
    startMenuMainItemTransition(*mChildren.safeSlot(0x26));
    startMenuMainBagTransition(*mChildren.safeSlot(0x27));
    for (u32 index = 0x1F; index < 0x25; ++index)
        startMenuMainTabTransition(*mChildren.safeSlot(index));
    startMenuMainSaveTransition(*mChildren.safeSlot(0x29));
}

static void updateMenuMainAlternateRequest(hdMenuMain_c* self, u32 key);
/* 02690724 */
void hdMenuMain_c::refreshWindowSelection() {
    if (sMenuMainInputMode->mDirectionalInputEnabled != 0) {
        hdButtonReceiver_l* receiver = getMenuMainInputReceiver(sMenuMainInputOwner.get());
        if (receiver == NULL) return;
        updateMenuMainItemSelection(this, receiver->mId);
        const u32 mode = mSelectionMode;
        if (mode == 0) updateMenuMainPlayRequest(this, receiver->mId);
        else if (mode == 1) updateMenuMainAlternateRequest(this, receiver->mId);
    } else {
        updateMenuMainItemSelection(this, 0);
    }
}
/* 02691EA8 */
void hdMenuMain_c::executeSelectedWindowSlide() {
    addMenuMainPaneTranslation(this, mTransitionProgress);
    hdMenuPane_l* pane = *mPanes.slot(0);
    const u32 mode = mSelectionMode;
    const float x = pane->mTranslationX;
    if (mode == 0) {
        if (!(0.0f > x)) {
            setMenuMainPaneTranslation(this, 0.0f);
            hdMenuMainInputChild_l* child = menuMainView<hdMenuMainInputChild_l>((*mChildren.safeSlot(0x11)).get());
            void* input = sMenuMainInputOwner.get();
            selectMenuMainInput(input, child->mpReceiver.get());
            changeState(&sMenuMainWindowReturned);
        }
    } else if (!(x > -1280.0f)) {
        setMenuMainPaneTranslation(this, -1280.0f);
        hdMenuMainInputChild_l* child = menuMainView<hdMenuMainInputChild_l>((*mChildren.safeSlot(0x28)).get());
        void* input = sMenuMainInputOwner.get();
        selectMenuMainInput(input, child->mpReceiver.get());
        changeState(&sMenuMainWindowSlid);
    }
    vCalcLayout();
}

struct hdMenuMainSaveChild_l : hdParts_c {
    u8 _50[0x30];
    be<u8> mMenuItemEnabled;
};
WWHD_OFFSET(hdMenuMainSaveChild_l, mMenuItemEnabled, 0x80);
static u32 getMenuMainOptionItemKind(hdParts_c* child, u32 key);
static void updateMenuMainSpecialButton(hdMenuMain_c* self, u32 key);
/* 026905EC */
void hdMenuMain_c::updateAlternateRequest(u32 key) {
    const u32 index = lookupMenuMainChildIndex(this, key);
    if ((index >= 0x1F && index <= 0x24) || (index >= 0x26 && index <= 0x27)) {
        getMenuMainPlay()->mMenuRequestedItem = 0;
        getMenuMainPlay()->mMenuRequestState = 7;
    } else if (index == 0x29) {
        hdMenuMainSaveChild_l* child = menuMainView<hdMenuMainSaveChild_l>((*mChildren.safeSlot(0x29)).get());
        if (child == NULL) return;
        const u8 enabled = child->mMenuItemEnabled;
        hdMenuMainPlay_l* play = getMenuMainPlay();
        play->mMenuRequestedItem = enabled != 0 ? 0x43 : 0;
        getMenuMainPlay()->mMenuRequestState = 7;
    } else {
        const u32 kind = getMenuMainOptionItemKind(*mChildren.safeSlot(0x25), key);
        hdMenuMainPlay_l* play = getMenuMainPlay();
        play->mMenuRequestedItem = kind <= 3 ? 0 : 0x17;
        getMenuMainPlay()->mMenuRequestState = 7;
    }
}
/* 02690890 */
void hdMenuMain_c::updateAlternateButtonFocus() {
    if (!mInputEnabled) return;
    const u8 receiverMode = sMenuMainInputMode->mDirectionalInputEnabled;
    hdButtonEventManager_l* input = sMenuMainInputOwner.get();
    if (receiverMode != 0) {
        if (!isMenuMainInputFocusChanged(input)) return;
        hdButtonReceiver_l* receiver = getMenuMainInputReceiver(sMenuMainInputOwner.get());
        if (receiver == NULL) return;
        updateMenuMainItemSelection(this, receiver->mId);
        updateMenuMainAlternateRequest(this, receiver->mId);
        updateMenuMainSpecialButton(this, receiver->mId);
    } else {
        hdButtonEvent_l event;
        copyMenuMainEvent(&event, &input->mCurrentEvent);
        if (isMenuMainFocusEvent(event.mType)) {
            updateMenuMainItemSelection(this, event.mReceiverId);
            updateMenuMainAlternateRequest(this, event.mReceiverId);
        }
    }
}

/* 026915B4 */
void hdMenuMain_c::enterTransitionChildren() {
    for (u32 index = 0; index < 19; ++index) {
        const u32 childIndex = sMenuMainTransitionItems.mItems[index].mChildIndex;
        hdMenuItem_c* child = menuMainView<hdMenuItem_c>((*mChildren.safeSlot(childIndex)).get());
        if (child != NULL && child->mVisible != 0) enterMenuMainTab(menuMainView<hdParts_c>(child));
    }
    enterMenuMainTab(*mChildren.safeSlot(0x12));
    enterMenuMainTab(*mChildren.safeSlot(0x13));
}

static void initializeMenuMainItem(hdParts_c* child, u32 key, u32 value);
static void resetMenuMainItemSelection(hdParts_c* child);
static void resetMenuMainItemAnimation(hdParts_c* child);
static void resetMenuMainItemFocus(hdParts_c* child);
static void resetMenuMainItemAction(hdParts_c* child);
/* 02693AC8 */
void hdMenuMain_c::initializeTransitionChildren() {
    for (u32 index = 0; index < 19; ++index) {
        const u32 key = sMenuMainTransitionItems.mItems[index].mKey;
        const u32 value = getMenuMainTransitionItemValue(key);
        const u32 childIndex = sMenuMainTransitionItems.mItems[index].mChildIndex;
        initializeMenuMainItem(*mChildren.safeSlot(childIndex), key, value);
        resetMenuMainItemSelection(*mChildren.safeSlot(childIndex));
        resetMenuMainItemAnimation(*mChildren.safeSlot(childIndex));
        resetMenuMainItemFocus(*mChildren.safeSlot(childIndex));
        resetMenuMainItemAction(*mChildren.safeSlot(childIndex));
    }
}

struct hdMenuMainWindowIndices_c { be<u32> mIndices[3]; };
static hdMenuMainWindowIndices_c sMenuMainWindowIndices;
/* 02692A50 */
void hdMenuMain_c::finishSaveWindow() {
    if (u32(mWindowMode) == 3) return;
    hdMenuPane_l* pane = *mWindowPanes.safeSlot(1);
    pane->mFlags = u8(pane->mFlags) & 0xFE;
    u32 mode = mWindowMode;
    u32 paneIndex = *gabi::at<be<u32>>(gabi::ea(&sMenuMainWindowIndices) + mode * sizeof(be<u32>));
    pane = *mWindowPanes.safeSlot(paneIndex);
    pane->mFlags = (u8(pane->mFlags) & 0xFE) + 1;
    mode = mWindowMode;
    paneIndex = *gabi::at<be<u32>>(gabi::ea(&sMenuMainWindowIndices) + mode * sizeof(be<u32>));
    hdMenuMainInputChild_l* child = menuMainView<hdMenuMainInputChild_l>((*mChildren.safeSlot(paneIndex)).get());
    void* input = sMenuMainInputOwner.get();
    selectMenuMainInput(input, child->mpReceiver.get());
    executeMenuMainTransitionChildren(this);
    vCalcLayout();
    mWindowMode = 3;
}

static u32 isMenuMainOptionWindowActive(hdMenuMain_c* self);
static void finishMenuMainAnimation(hdAnimSet* set, u32 entry, u32 slot);
static hdPartsStateId_c sMenuMainOutAll;
/* 026942E0 */
void hdMenuMain_c::requestOutAll() {
    if (isMenuMainOptionWindowActive(this)) finishMenuMainAnimation(animSet(), 7, 3);
    if (u32(mWindowMode) != 3) {
        hdMenuPane_l* pane = *mWindowPanes.safeSlot(1);
        pane->mFlags = u8(pane->mFlags) & 0xFE;
        const u32 mode = mWindowMode;
        const u32 paneIndex = *gabi::at<be<u32>>(gabi::ea(&sMenuMainWindowIndices) + mode * sizeof(be<u32>));
        pane = *mWindowPanes.safeSlot(paneIndex);
        pane->mFlags = (u8(pane->mFlags) & 0xFE) + 1;
        finishMenuMainAnimation(animSet(), u32(mWindowMode) + 12, 5);
        mWindowMode = 3;
    }
    changeState(&sMenuMainOutAll);
}

static bool menuMainHasState(hdMenuMain_c* self, hdPartsStateId_c* expected) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(self->stateMachine());
    const u32 currentGetter = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* expectedTable = expected->mVtable.get();
    const u32 key = gabi::call_ptr<u32>(currentGetter, current);
    return key == gabi::call_ptr<u32>(expectedTable->mGetKey.get(), expected);
}
/* 026941BC */
u32 hdMenuMain_c::isOptionWindowActive() {
    if (menuMainHasState(this, &sMenuMainChildOpening)) return 1;
    if (menuMainHasState(this, &sMenuMainOptionReady)) return 1;
    return menuMainHasState(this, &sMenuMainSelectedChildClosing);
}
static void initializeMenuMainCollect(hdParts_c* child);
static void initializeMenuMainTransitionItems(hdMenuMain_c* self);
static void initializeMenuMainWindowItems(hdMenuMain_c* self);
static void initializeMenuMainSpecialItems(hdMenuMain_c* self);
static void startMenuMainWindowChildTransitions(hdMenuMain_c* self);
/* 026940A8 */
void hdMenuMain_c::finishInAll() {
    if (!menuMainHasState(this, &sMenuMainBlank)) {
        const u32 mode = mSelectionMode;
        if (mode == 0) {
            initializeMenuMainCollect(*mChildren.safeSlot(0x2D));
            initializeMenuMainTransitionItems(this);
            initializeMenuMainWindowItems(this);
            initializeMenuMainSpecialItems(this);
        } else if (mode == 1) {
            startMenuMainWindowChildTransitions(this);
        }
    }
    mOpenRequested = 0;
    hdParts_executeStateMachine(stateMachine());
}

// Window-mode indices are read from guest state; the original external-table accesses wrap at 32 bits.
struct hdMenuMainWindowCounts_c {
    be<u32> mCounts[3];
    u32 count(u32 mode) { return *gabi::at<be<u32>>(gabi::ea(this) + mode * sizeof(be<u32>)); }
};
struct hdMenuMainWindowItems_c {
    gptr<hdMenuMainTransitionItem_c> mItems[3];
    hdMenuMainTransitionItem_c* items(u32 mode) {
        return *gabi::at<gptr<hdMenuMainTransitionItem_c>>(gabi::ea(this) + mode * sizeof(gptr<hdMenuMainTransitionItem_c>));
    }
};
static_assert(sizeof(hdMenuMainWindowCounts_c) == 12, "menu window count table");
static_assert(sizeof(hdMenuMainWindowItems_c) == 12, "menu window item table");
static hdMenuMainWindowCounts_c sMenuMainWindowCounts;
static hdMenuMainWindowItems_c sMenuMainWindowItems;
/* 026924EC */
void hdMenuMain_c::executeSaveChildren() {
    const u32 mode = mWindowMode;
    if (mode == 3) return;
    u32 count = sMenuMainWindowCounts.count(mode);
    u32 cursor = gabi::ea(sMenuMainWindowItems.items(mode));
    for (u32 index = 0; index < count; ++index) {
        hdMenuMainTransitionItem_c* item = menuMainView<hdMenuMainTransitionItem_c>(cursor);
        const u32 value = getMenuMainTransitionItemValue(item->mKey);
        hdParts_c* child = *mChildren.safeSlot(item->mChildIndex);
        const u32 key = item->mKey;
        setMenuMainTransitionItem(child, key, value);
        refreshMenuMainTransitionItem(*mChildren.safeSlot(item->mChildIndex));
        count = sMenuMainWindowCounts.count(mWindowMode);
        cursor += sizeof(hdMenuMainTransitionItem_c);
    }
    refreshMenuMainTransitionItem(*mChildren.safeSlot(1));
}
/* 026925EC */
void hdMenuMain_c::enterSaveChildren() {
    const u32 mode = mWindowMode;
    u32 count = sMenuMainWindowCounts.count(mode);
    u32 cursor = gabi::ea(sMenuMainWindowItems.items(mode));
    for (u32 index = 0; index < count; ++index) {
        hdMenuMainTransitionItem_c* item = menuMainView<hdMenuMainTransitionItem_c>(cursor);
        hdMenuItem_c* child = menuMainView<hdMenuItem_c>((*mChildren.safeSlot(item->mChildIndex)).get());
        if (child != NULL && child->mVisible != 0) {
            enterMenuMainTab(menuMainView<hdParts_c>(child));
            count = sMenuMainWindowCounts.count(mWindowMode);
        }
        cursor += sizeof(hdMenuMainTransitionItem_c);
    }
    enterMenuMainTab(*mChildren.safeSlot(0x1C));
}
/* 02693BC8 */
void hdMenuMain_c::initializeSaveChildren() {
    const u32 mode = mWindowMode;
    if (mode == 3) return;
    u32 count = sMenuMainWindowCounts.count(mode);
    u32 cursor = gabi::ea(sMenuMainWindowItems.items(mode));
    for (u32 index = 0; index < count; ++index) {
        hdMenuMainTransitionItem_c* item = menuMainView<hdMenuMainTransitionItem_c>(cursor);
        const u32 value = getMenuMainTransitionItemValue(item->mKey);
        hdParts_c* child = *mChildren.safeSlot(item->mChildIndex);
        const u32 key = item->mKey;
        initializeMenuMainItem(child, key, value);
        resetMenuMainItemSelection(*mChildren.safeSlot(item->mChildIndex));
        resetMenuMainItemAnimation(*mChildren.safeSlot(item->mChildIndex));
        resetMenuMainItemFocus(*mChildren.safeSlot(item->mChildIndex));
        resetMenuMainItemAction(*mChildren.safeSlot(item->mChildIndex));
        count = sMenuMainWindowCounts.count(mWindowMode);
        cursor += sizeof(hdMenuMainTransitionItem_c);
    }
    resetMenuMainItemAnimation(*mChildren.safeSlot(1));
}

using hdMenuMainHeapManagerPointer = gptr<void>;
static hdMenuMainHeapManagerPointer sMenuMainHeapManager;
/* 026937C8 */
void hdMenuMain_c::destroyInputReceivers() {
    for (s32 index = 0; index < s32(mReceivers.mCount); ++index) {
        hdButtonReceiver_l* receiver = *mReceivers.safeSlot(u32(index));
        if (receiver != NULL) {
            receiver = *mReceivers.safeSlot(u32(index));
            if (receiver != NULL)
                gabi::call_ptr(receiver->mVtable->mDestroy.get(), receiver, 3);
            *mReceivers.safeSlot(u32(index)) = NULL;
        }
    }
    void* memory = mReceivers.mpData.get();
    if (memory != NULL) {
        hdScreenHeap* heap = hdParts_findContainHeap(sMenuMainHeapManager.get(), memory);
        heap->free(mReceivers.mpData.get());
        mReceivers.mCount = 0;
        mReceivers.mpData = NULL;
    }
}

/* 026938F4 */
void hdMenuMain_c::destroyPaneArrays() {
    const s32 windowCount = s32(u32(mWindowPanes.mCount));
    for (s32 index = 0; index < windowCount; ++index) *mWindowPanes.slot(u32(index)) = NULL;
    void* memory = mWindowPanes.mpPanes.get();
    if (windowCount > 0 || memory != NULL) {
        hdScreenHeap* heap = hdParts_findContainHeap(sMenuMainHeapManager.get(), memory);
        heap->free(mWindowPanes.mpPanes.get());
        mWindowPanes.mCount = 0;
        mWindowPanes.mpPanes = NULL;
    }
    const s32 tabCount = mTabChildren.mNum;
    for (s32 index = 0; index < tabCount; ++index) *mTabChildren.slot(u32(index)) = NULL;
    memory = mTabChildren.mpData.get();
    if (tabCount > 0 || memory != NULL) {
        hdScreenHeap* heap = hdParts_findContainHeap(sMenuMainHeapManager.get(), memory);
        heap->free(mTabChildren.mpData.get());
        mTabChildren.mNum = 0;
        mTabChildren.mpData = NULL;
    }
    const s32 paneCount = s32(u32(mPanes.mCount));
    for (s32 index = 0; index < paneCount; ++index) *mPanes.slot(u32(index)) = NULL;
    memory = mPanes.mpPanes.get();
    if (paneCount > 0 || memory != NULL) {
        hdScreenHeap* heap = hdParts_findContainHeap(sMenuMainHeapManager.get(), memory);
        heap->free(mPanes.mpPanes.get());
        mPanes.mCount = 0;
        mPanes.mpPanes = NULL;
    }
}

struct hdMenuMainCollectChild_l : hdParts_c {
    u8 _50[8];
    hdMenuMainReceiverArray_c mReceivers;
};
WWHD_OFFSET(hdMenuMainCollectChild_l, mReceivers, 0x58);
static u32 isMenuMainCancelEvent(hdMenuMain_c* self);
static u32 getMenuMainSelectedItemValue(hdMenuMain_c* self, u32 key);
static void enableMenuMainCollectReceiver(hdParts_c* child, u32 index, u32 enabled);
static void resetMenuMainCollectReceiver(hdParts_c* child, u32 index);
static void activateMenuMainCollectReceiver(hdParts_c* child, u32 index);
static void deactivateMenuMainCollectReceiver(hdParts_c* child, u32 index);
static u32 isMenuMainCollectActionUnavailable(hdMenuMain_c* self);
static u32 isMenuMainCollectInputBlocked(hdMenuMain_c* self);
static u32 isMenuMainAnyItemActivated(hdMenuMain_c* self);
/* 02693EC0 */
void hdMenuMain_c::initializeCollectReceivers() {
    hdButtonEventManager_l* input = sMenuMainInputOwner.get();
    if (input == NULL) return;
    hdMenuMainCollectChild_l* collect = menuMainView<hdMenuMainCollectChild_l>((*mChildren.safeSlot(0x2D)).get());
    hdButtonEvent_l event;
    copyMenuMainEvent(&event, &input->mCurrentEvent);
    hdButtonReceiver_l* currentReceiver = isMenuMainReceiverEvent(sMenuMainInputOwner.get());
    if (isMenuMainCancelEvent(this)) {
        const u8 item = u8(getMenuMainSelectedItemValue(this, event.mReceiverId));
        if (item == 4 || item == 0xB || item == 0x12 || item == 0xFF) return;
        for (u32 index = 0; index < 3; ++index) {
            hdButtonReceiver_l* receiver = *collect->mReceivers.safeSlot(index);
            if (u32(currentReceiver->mId) == u32(receiver->mId)) {
                enableMenuMainCollectReceiver(collect, index, 1);
                deactivateMenuMainCollectReceiver(collect, index);
            } else {
                resetMenuMainCollectReceiver(collect, index);
                if (isMenuMainCollectActionUnavailable(this)) activateMenuMainCollectReceiver(collect, index);
            }
        }
    } else {
        if (isMenuMainCollectInputBlocked(this)) return;
        if (isMenuMainAnyItemActivated(this)) return;
        for (u32 index = 0; index < 3; ++index) resetMenuMainCollectReceiver(collect, index);
        if (isMenuMainCollectActionUnavailable(this)) {
            for (u32 index = 0; index < 3; ++index) activateMenuMainCollectReceiver(collect, index);
        } else {
            for (u32 index = 0; index < 3; ++index) deactivateMenuMainCollectReceiver(collect, index);
        }
    }
}

/* 026943C8 */
void hdMenuMain_c::setTabImmediately(u32 mode) {
    hdPartsLayout_c* layout = mpLayout.get();
    mSelectionMode = mode;
    finishMenuMainAnimation(layout->animSet(), 0, 0);
    const u32 selection = mSelectionMode;
    if (selection == 0) {
        setMenuMainPaneTranslation(this, 0.0f);
        enterMenuMainTab(*mChildren.safeSlot(0x11));
        finishMenuMainAnimation(animSet(), 3, 2);
        changeState(&sMenuMainSecondaryReady);
    } else if (selection == 1) {
        setMenuMainPaneTranslation(this, -1280.0f);
        enterMenuMainTab(*mChildren.safeSlot(0x28));
        finishMenuMainAnimation(animSet(), 2, 1);
        changeState(&sMenuMainPrimaryReady);
    }
    vCalcLayout();
}
struct hdMenuMainCollectItemInfo_c { be<u32> mWords[2]; };
WWHD_SIZE(hdMenuMainCollectItemInfo_c, 8);
static Pair32 getMenuMainCollectItemInfo(hdParts_c* child, u32 index);
static void setMenuMainItemInfo(hdParts_c* child, hdMenuMainCollectItemInfo_c* info);
/* 0269451C */
void hdMenuMain_c::selectCollectItem(u32 key, u32 collectIndex) {
    const u32 childIndex = lookupMenuMainChildIndex(this, key);
    if (!isMenuMainMainTabItem(this, childIndex)) return;
    hdParts_c* collect = *mChildren.safeSlot(0x2D);
    auto result = getMenuMainCollectItemInfo(collect, collectIndex);
    gptr<hdParts_c>* slot = mChildren.safeSlot(childIndex);
    hdMenuMainCollectItemInfo_c info;
    info.mWords[1] = result.r4;
    info.mWords[0] = result.r3;
    setMenuMainItemInfo(*slot, &info);
    for (u32 index = 0; index < 3; ++index) {
        if (collectIndex == index) enableMenuMainCollectReceiver(collect, index, 0);
        else resetMenuMainCollectReceiver(collect, index);
        deactivateMenuMainCollectReceiver(collect, index);
    }
}

static u32 getMenuMainInputActionKey(hdButtonEventManager_l* input);
static void refreshMenuMainCollectChildren(hdParts_c* child);
/* 0268FCE4 */
void hdMenuMain_c::refreshWindowChildren() {
    if (!mInputEnabled) return;
    if (isMenuMainCollectInputBlocked(this)) return;
    if (isMenuMainAnyItemActivated(this)) return;
    bool skipItems = false;
    if (getMenuMainInputActionKey(sMenuMainInputOwner.get()) != 0) {
        const u32 key = getMenuMainInputActionKey(sMenuMainInputOwner.get());
        skipItems = getMenuMainSelectedItemValue(this, key) == 0xFF;
    }
    if (!skipItems) {
        for (u32 index = 22; index < 31; ++index) {
            const u32 childIndex = sMenuMainButtonIndices.mEntries[index];
            hdParts_c* child = *mChildren.safeSlot(childIndex);
            if (child != NULL) {
                hdMenuMainRefreshVtable_l* table = menuMainView<hdMenuMainRefreshVtable_l>(child->mVtable.v.get());
                gabi::call_ptr(table->mRefresh.get(), child);
            }
        }
    }
    refreshMenuMainCollectChildren(*mChildren.safeSlot(0x2D));
}
/* 0268FE10 */
void hdMenuMain_c::executeWindowChildren() {
    for (u32 index = 0x14; index < 0x1D; ++index) {
        hdParts_c* child = *mChildren.safeSlot(index);
        if (child != NULL) child->vExecuteState();
    }
    hdParts_c* child = *mChildren.safeSlot(1);
    if (child != NULL) child->vExecuteState();
    child = *mChildren.safeSlot(0x2D);
    if (child != NULL) child->vExecuteState();
}

/* 0268F4F8 */
void hdMenuMain_c::refreshMainChildren() {
    if (!mInputEnabled) return;
    if (isMenuMainCollectInputBlocked(this)) return;
    if (isMenuMainAnyItemActivated(this)) return;
    bool skipItems = false;
    if (getMenuMainInputActionKey(sMenuMainInputOwner.get()) != 0) {
        const u32 key = getMenuMainInputActionKey(sMenuMainInputOwner.get());
        skipItems = getMenuMainSelectedItemValue(this, key) == 0xFF;
    }
    if (!skipItems) {
        for (u32 index = 0; index < 22; ++index) {
            const u32 childIndex = sMenuMainButtonIndices.mEntries[index];
            hdParts_c* child = *mChildren.safeSlot(childIndex);
            if (child != NULL) {
                hdMenuMainRefreshVtable_l* table = menuMainView<hdMenuMainRefreshVtable_l>(child->mVtable.v.get());
                gabi::call_ptr(table->mRefresh.get(), child);
            }
        }
    }
    refreshMenuMainCollectChildren(*mChildren.safeSlot(0x2D));
}
static u32 isMenuMainConfirmEvent(hdButtonEvent_l* event);
static u32 canMenuMainItemOpen(hdParts_c* child);
static void openMenuMainSelectedChild(hdMenuMain_c* self, u32 index);
/* 0268F110 */
void hdMenuMain_c::updateTabInput() {
    if (sMenuMainInputMode->mDirectionalInputEnabled == 0) return;
    hdButtonEvent_l event;
    copyMenuMainEvent(&event, &sMenuMainInputOwner->mCurrentEvent);
    if (!isMenuMainConfirmEvent(&event)) return;
    hdButtonReceiver_l* receiver = getMenuMainInputReceiver(sMenuMainInputOwner.get());
    if (receiver == NULL) return;
    const u32 index = lookupMenuMainChildIndex(this, receiver->mId);
    auto classification = classifyMenuMainSelection(this, index);
    if (classification.r3 == 0) return;
    gptr<hdParts_c>* slot = mChildren.safeSlot(index);
    hdMenuItem_c* child = menuMainView<hdMenuItem_c>((*slot).get());
    if (child->mVisible == 0) return;
    if (canMenuMainItemOpen(*slot)) openMenuMainSelectedChild(this, index);
}

/* 0268F624 */
void hdMenuMain_c::executeMainChildren() {
    for (u32 index = 0; index < 0x14; ++index) {
        hdParts_c* child = *mChildren.safeSlot(index);
        if (child != NULL) child->vExecuteState();
    }
    for (u32 index = 0; index < 3; ++index) {
        const u32 childIndex = sMenuMainWindowIndices.mIndices[index];
        hdParts_c* child = *mChildren.safeSlot(childIndex);
        if (child != NULL) child->vExecuteState();
    }
    hdParts_c* child = *mChildren.safeSlot(0x2D);
    if (child != NULL) child->vExecuteState();
}
static u32 isMenuMainTabEvent(u32 type);
static void updateMenuMainMainInput(hdMenuMain_c* self);
static void updateMenuMainTabConfirmation(hdMenuMain_c* self);
static void refreshMenuMainMainChildren(hdMenuMain_c* self);
static void executeMenuMainMainChildren(hdMenuMain_c* self);
/* 0268F790 */
void hdMenuMain_c::executeMainTab() {
    updateMenuMainButtonFocus(this);
    updateMenuMainButtonSelection(this);
    hdButtonEvent_l event;
    copyMenuMainEvent(&event, &sMenuMainInputOwner->mCurrentEvent);
    if (isMenuMainTabEvent(event.mType)) {
        const u32 key = getMenuMainButtonKey(*mChildren.safeSlot(0x11));
        if (u32(event.mReceiverId) == key) setMenuMainSelectionMode(this, 1);
    } else if (isMenuMainConfirmEvent(&event) || u32(event.mType) == 0xC) {
        if (sMenuMainInputMode->mDirectionalInputEnabled != 0 &&
            getMenuMainInputActionKey(sMenuMainInputOwner.get()) == 0) {
            hdButtonReceiver_l* receiver = getMenuMainInputReceiver(sMenuMainInputOwner.get());
            if (receiver != NULL) {
                const u32 key = getMenuMainButtonKey(*mChildren.safeSlot(0x11));
                if (u32(receiver->mId) == key) {
                    setMenuMainSelectionMode(this, 1);
                    playMenuMainSound("SE_MENU_TAB_SWITCH");
                }
            }
        }
    }
    updateMenuMainMainInput(this);
    updateMenuMainTabConfirmation(this);
    refreshMenuMainMainChildren(this);
    executeMenuMainMainChildren(this);
    vCalcLayout();
}

static u32 hitTestMenuMainCollect(hdParts_c* child, hdMenuPaneVector* point);
/* 0268F970 */
u32 hdMenuMain_c::hitTestWindowChildren(hdMenuPaneVector* point) {
    const u32 count = mPanes.mCount;
    gptr<hdMenuPane_l>* panes = mPanes.slot(0);
    hdMenuPaneVector value;
    value.y = point->y;
    value.x = point->x;
    gptr<hdMenuPane_l>* slot = menuMainView<gptr<hdMenuPane_l>>(gabi::ea(panes) + (count > 3 ? 3 : 0) * sizeof(gptr<hdMenuPane_l>));
    hdMenuPane_l* pane = *slot;
    if (hitTestMenuMainPane(pane, &value) != 0 &&
        hitTestMenuMainPane(*mPanes.safeSlot(2), &value) != 0 &&
        hitTestMenuMainPane(*mPanes.safeSlot(4), &value) != 0) return 1;
    for (u32 index = 22; index < 31; ++index) {
        const u32 childIndex = sMenuMainButtonIndices.mEntries[index];
        hdButtonPart_c* child = menuMainView<hdButtonPart_c>((*mChildren.safeSlot(childIndex)).get());
        cking::ui::input::Receiver* receiver = menuMainView<cking::ui::input::Receiver>(child->mpReceiver.v.get());
        if (hitTestMenuMainPane(menuMainView<hdMenuPane_l>(receiver->mpLayout.get()), &value)) return 1;
    }
    return hitTestMenuMainCollect(*mChildren.safeSlot(0x2D), point) != 0;
}

static hdPartsVtbl_c sMenuMainVtable;
static void* allocateMenuMainObject(u32 size);
static void constructMenuMainParts(hdParts_c* self);
/* 02692B44 */
hdMenuMain_c* hdMenuMain_c::construct(hdMenuMain_c* self) {
    if (self == NULL) self = menuMainView<hdMenuMain_c>(allocateMenuMainObject(sizeof(hdMenuMain_c)));
    if (self == NULL) return NULL;
    constructMenuMainParts(self);
    self->mSecondaryVtable = &sMenuMainBaseSecondaryVtable;
    self->mWindowMode = 3;
    self->mSelectionMode = 0;
    self->mWindowOffset = 0.0f;
    self->mVtable = &sMenuMainVtable;
    self->mStateHolder.mHolderVtable = &sMenuMainBaseHolderVtable;
    self->mSelectedChild = 0;
    hdMenuMainPaneArray_c* panes = menuMainView<hdMenuMainPaneArray_c>(u32(gabi::ea(self) + offsetof(hdMenuMain_c, mPanes)));
    self->mTransitionProgress = 0.0f;
    if (gabi::ea(panes) == 0) panes = menuMainView<hdMenuMainPaneArray_c>(allocateMenuMainObject(sizeof(hdMenuMainPaneArray_c)));
    if (panes != NULL) {
        panes->mpPanes = NULL;
        panes->mCount = 0;
    }
    hdPartsChildArray_c* tabs = menuMainView<hdPartsChildArray_c>(u32(gabi::ea(self) + offsetof(hdMenuMain_c, mTabChildren)));
    if (gabi::ea(tabs) == 0) tabs = menuMainView<hdPartsChildArray_c>(allocateMenuMainObject(sizeof(hdPartsChildArray_c)));
    if (tabs != NULL) {
        tabs->mpData = NULL;
        tabs->mNum = 0;
    }
    hdMenuMainPaneArray_c* windows = menuMainView<hdMenuMainPaneArray_c>(u32(gabi::ea(self) + offsetof(hdMenuMain_c, mWindowPanes)));
    if (gabi::ea(windows) == 0) windows = menuMainView<hdMenuMainPaneArray_c>(allocateMenuMainObject(sizeof(hdMenuMainPaneArray_c)));
    if (windows != NULL) {
        windows->mpPanes = NULL;
        windows->mCount = 0;
    }
    hdMenuMainReceiverArray_c* receivers = menuMainView<hdMenuMainReceiverArray_c>(u32(gabi::ea(self) + offsetof(hdMenuMain_c, mReceivers)));
    if (gabi::ea(receivers) == 0) receivers = menuMainView<hdMenuMainReceiverArray_c>(allocateMenuMainObject(sizeof(hdMenuMainReceiverArray_c)));
    if (receivers != NULL) {
        receivers->mpData = NULL;
        receivers->mCount = 0;
    }
    self->mInputBlocked = 0;
    self->mInputEnabled = 0;
    self->mTransitionFinished = 0;
    self->mpContext = NULL;
    self->mPending = 0;
    self->mActive = 0;
    self->mOpenRequested = 0;
    self->mContextSecond = 0;
    self->mContextFirst = 0;
    return self;
}

static void closeMenuMainOptionWindow(hdMenuMain_c* self);
/* 0269474C */
void hdMenuMain_c::requestAlternateSelection() {
    mPending = 1;
    mActive = 0;
    if (menuMainHasState(this, &sMenuMainBlank)) {
        mSelectionMode = 1;
        return;
    }
    if (menuMainHasState(this, &sMenuMainPrimaryReady)) {
        changeState(&sMenuMainPendingReady);
        return;
    }
    if (menuMainHasState(this, &sMenuMainSecondaryReady)) {
        setMenuMainSelectionMode(this, 1);
        return;
    }
    if (menuMainHasState(this, &sMenuMainWindowReady)) {
        closeMenuMainOptionWindow(this);
        return;
    }
    if (menuMainHasState(this, &sMenuMainPendingReady)) changeState(&sMenuMainPendingReady);
}

struct hdMenuMainArchiveNames_c { hdUiSafeString mNames[13]; };
static hdMenuMainArchiveNames_c sMenuMainArchiveNames;
static hdUiSafeString sMenuMainLayoutName;
static void attachMenuMainArchives(hdParts_c* self, hdUiSafeString* names, u32 count);
static u32 createMenuMainPaneArrays(hdMenuMain_c* self);
static void createMenuMainInputReceivers(hdMenuMain_c* self);
static void setupMenuMainItemNames(hdMenuMain_c* self);
static void setupMenuMainWindowNames(hdMenuMain_c* self);
static void setupMenuMainButtonSounds(hdMenuMain_c* self);
/* 02693650 */
u32 hdMenuMain_c::loadMenu(u32 first, u32 second, void* descriptor) {
    attachMenuMainArchives(this, sMenuMainArchiveNames.mNames, 13);
    if (!vLoad(&sMenuMainLayoutName, mpResAccessor.v.get(), first, second, descriptor)) return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    if (!createMenuMainPaneArrays(this)) return 0;
    createMenuMainInputReceivers(this);
    setupMenuMainItemNames(this);
    setupMenuMainWindowNames(this);
    hdAnimSet_operationCC(animSet(), 2, 1);
    hdAnimSet_operationCC(animSet(), 3, 2);
    hdAnimSet_operationCC(animSet(), 0, 0);
    executeMenuMainParts(this);
    setupMenuMainButtonSounds(this);
    startState(&sMenuMainBlank);
    return 1;
}

struct hdMenuMainWindowLabels_c { hdUiSafeString mNames[9]; };
static hdMenuMainWindowLabels_c sMenuMainWindowLabels;
static void bindMenuMainChildLabel(hdParts_c* child, hdScreenString* paneName, hdUiSafeString* text);
/* 02693328 */
void hdMenuMain_c::setupWindowNames() {
    for (u32 index = 0; index < 3; ++index) {
        const u32 childIndex = sMenuMainWindowIndices.mIndices[index];
        hdButtonPart_c* child = menuMainView<hdButtonPart_c>((*mChildren.safeSlot(childIndex)).get());
        child->mpReceiver->mEnabled = 0;
    }
    hdButtonPart_c* button = menuMainView<hdButtonPart_c>((*mChildren.safeSlot(1)).get());
    button->mpReceiver->mEnabled = 0;
    hdMenuPane_l* pane = *mWindowPanes.safeSlot(1);
    pane->mFlags = u8(pane->mFlags) & 0xFE;
    hdParts_c* child = *mChildren.safeSlot(0x12);
    hdScreenString name;
    name.mVtable = &sMenuMainResourceNameVtable;
    name.mText.v = gabi::guest_addr("T_Btn_00");
    bindMenuMainChildLabel(child, &name, &sMenuMainWindowLabels.mNames[0]);
    child = *mChildren.safeSlot(0x1D);
    name.mVtable = &sMenuMainResourceNameVtable;
    name.mText.v = gabi::guest_addr("T_Btn_00");
    bindMenuMainChildLabel(child, &name, &sMenuMainWindowLabels.mNames[0]);
    child = *mChildren.safeSlot(0x13);
    name.mVtable = &sMenuMainResourceNameVtable;
    name.mText.v = gabi::guest_addr("T_Btn_00");
    bindMenuMainChildLabel(child, &name, &sMenuMainWindowLabels.mNames[1]);
    child = *mChildren.safeSlot(0x1E);
    name.mVtable = &sMenuMainResourceNameVtable;
    name.mText.v = gabi::guest_addr("T_Btn_00");
    bindMenuMainChildLabel(child, &name, &sMenuMainWindowLabels.mNames[1]);
}

struct hdMenuMainSoundNames_c { hdScreenString mNames[4]; };
WWHD_SIZE(hdMenuMainSoundNames_c, 0x20);
static hdScreenString sMenuMainEmptySound;
static void setMenuMainChildSound(hdParts_c* child, hdScreenString* sound, u32 slot);
/* 026934B8 */
void hdMenuMain_c::setupButtonSounds() {
    hdMenuMainSoundNames_c sounds;
    hdParts_c* child = *mChildren.safeSlot(0x12);
    if (child != NULL) {
        sounds.mNames[0].mVtable = &sMenuMainResourceNameVtable;
        sounds.mNames[0].mText.v = gabi::guest_addr("SE_MENU_OPTION_IN");
        setMenuMainChildSound(child, &sounds.mNames[0], 0);
    }
    child = *mChildren.safeSlot(0x13);
    if (child != NULL) {
        sounds.mNames[1].mVtable = &sMenuMainResourceNameVtable;
        sounds.mNames[1].mText.v = gabi::guest_addr("SE_MENU_OPTION_IN");
        setMenuMainChildSound(child, &sounds.mNames[1], 0);
    }
    child = *mChildren.safeSlot(0x11);
    if (child != NULL) {
        sounds.mNames[2].mVtable = &sMenuMainResourceNameVtable;
        sounds.mNames[2].mText.v = gabi::guest_addr("SE_MENU_TAB_SWITCH");
        setMenuMainChildSound(child, &sounds.mNames[2], 0);
    }
    child = *mChildren.safeSlot(0x28);
    if (child != NULL) {
        sounds.mNames[3].mVtable = &sMenuMainResourceNameVtable;
        sounds.mNames[3].mText.v = gabi::guest_addr("SE_MENU_TAB_SWITCH");
        setMenuMainChildSound(child, &sounds.mNames[3], 0);
    }
    child = *mChildren.safeSlot(0x1C);
    if (child != NULL) setMenuMainChildSound(child, &sMenuMainEmptySound, 0);
}

static void enterMenuMainTransitionChildren(hdMenuMain_c* self);
/* 02691FB4 */
void hdMenuMain_c::enterInitialTabAnimation() {
    const u32 initialMode = mSelectionMode;
    if (initialMode == 0) executeMenuMainTransitionChildren(this);
    else if (initialMode == 1) executeMenuMainWindowTransitionEntry(this);
    vCalcLayout();
    const u32 mode = mSelectionMode;
    if (mode == 0) enterMenuMainTransitionChildren(this);
    else if (mode == 1) startMenuMainWindowTransition(this);
    hdAnimSet_operationCC(animSet(), 2, 1);
    hdAnimSet_operationCC(animSet(), 3, 2);
    hdAnimSet_play(animSet(), 0, 0, 1.0f);
}

static void updateMenuMainAlternateButtonFocus(hdMenuMain_c* self);
static void refreshMenuMainTabChildren(hdMenuMain_c* self);
static void executeMenuMainTabChildren(hdMenuMain_c* self);
/* 02690AE4 */
void hdMenuMain_c::executeAlternateTab() {
    updateMenuMainAlternateButtonFocus(this);
    if (mTransitionFinished != 0) {
        gptr<hdParts_c>* slot = mTabChildren.safeSlot(2);
        hdMenuMainTabLink_l* linked = menuMainView<hdMenuMainTabLink_l>((*slot).get());
        hdParts_c* child = *slot;
        linkMenuMainTab(child, &linked->mLink);
    }
    updateMenuMainButtonSelection(this);
    hdButtonEvent_l event;
    copyMenuMainEvent(&event, &sMenuMainInputOwner->mCurrentEvent);
    if (isMenuMainTabEvent(event.mType)) {
        const u32 key = getMenuMainButtonKey(*mChildren.safeSlot(0x28));
        if (u32(event.mReceiverId) == key) setMenuMainSelectionMode(this, 0);
    } else if (isMenuMainConfirmEvent(&event) || u32(event.mType) == 0xB) {
        if (sMenuMainInputMode->mDirectionalInputEnabled != 0 &&
            getMenuMainInputActionKey(sMenuMainInputOwner.get()) == 0) {
            hdButtonReceiver_l* receiver = getMenuMainInputReceiver(sMenuMainInputOwner.get());
            if (receiver != NULL) {
                const u32 key = getMenuMainButtonKey(*mChildren.safeSlot(0x28));
                if (u32(receiver->mId) == key) {
                    setMenuMainSelectionMode(this, 0);
                    playMenuMainSound("SE_MENU_TAB_SWITCH");
                }
            }
        }
    }
    refreshMenuMainTabChildren(this);
    executeMenuMainTabChildren(this);
    vCalcLayout();
}

static u32 isMenuMainWindowRequestEvent(hdButtonEvent_l* event);
static void openMenuMainWindow(hdMenuMain_c* self, u32 mode);

/* 0268ECDC */
void hdMenuMain_c::updateWindowRequest() {
    hdButtonEvent_l event;
    copyMenuMainEvent(&event, &sMenuMainInputOwner->mCurrentEvent);
    if (isMenuMainFocusEvent(event.mType)) {
        for (u32 mode = 0; mode < 3; ++mode) {
            const u32 childIndex = sMenuMainWindowIndices.mIndices[mode];
            const u32 key = getMenuMainButtonKey(*mChildren.safeSlot(childIndex));
            if (u32(event.mReceiverId) == key) {
                hdMenuItem_c* child = menuMainView<hdMenuItem_c>((*mChildren.safeSlot(childIndex)).get());
                if (child->mVisible != 0) openMenuMainWindow(this, mode);
            }
        }
        return;
    }
    if (!isMenuMainWindowRequestEvent(&event)) return;
    if (sMenuMainInputMode->mDirectionalInputEnabled == 0) return;
    hdButtonReceiver_l* receiver = getMenuMainInputReceiver(sMenuMainInputOwner.get());
    if (receiver == NULL) return;
    for (u32 mode = 0; mode < 3; ++mode) {
        const u32 childIndex = sMenuMainWindowIndices.mIndices[mode];
        const u32 key = getMenuMainButtonKey(*mChildren.safeSlot(childIndex));
        if (u32(receiver->mId) == key) {
            hdMenuItem_c* child = menuMainView<hdMenuItem_c>((*mChildren.safeSlot(childIndex)).get());
            if (child->mVisible != 0) openMenuMainWindow(this, mode);
        }
    }
}

struct hdMenuMainAnimationGroups_c { be<u32> mIndices[15]; };
struct hdMenuMainAnimationNames_c { hdAnimSetString mNames[15]; };
struct hdMenuMainAnimationGroupNames_c {
    hdAnimSetString mNames[8];
    hdAnimSetString* at(u32 index) {
        return gabi::at<hdAnimSetString>(gabi::ea(this) + index * sizeof(hdAnimSetString));
    }
};
using hdMenuMainUiSystemPtr = gptr<hdScreenUiSystem>;
static hdMenuMainUiSystemPtr sMenuMainUiSystem;
static hdMenuMainAnimationGroups_c sMenuMainAnimationGroups;
static hdMenuMainAnimationNames_c sMenuMainAnimationNames;
static hdMenuMainAnimationGroupNames_c sMenuMainAnimationGroupNames;
static void* allocateMenuMainLayout(u32 size, hdScreenHeap* heap, u32 alignment);
static void bindMenuMainAnimation(hdAnimSet* animationSet, u32 index, hdAnimSetString* name, hdAnimSetString* group);

/* 02697000 */
u32 hdMenuMain_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* descriptor) {
    hdPartsLayout_c* layout = menuMainView<hdPartsLayout_c>(allocateMenuMainLayout(sizeof(hdPartsLayout_c), sMenuMainUiSystem->mHeap.get(), 4));
    if (layout != NULL) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, first, second, third, 15, 8, descriptor)) return 0;
    for (u32 index = 0; index < 15; ++index) {
        hdAnimSet* animationSet = mpLayout->animSet();
        const u32 groupIndex = sMenuMainAnimationGroups.mIndices[index];
        bindMenuMainAnimation(animationSet, index, &sMenuMainAnimationNames.mNames[index], sMenuMainAnimationGroupNames.at(groupIndex));
    }
    executeMenuMainParts(this);
    return 1;
}

static void resetMenuMainButton(hdParts_c* child);
static hdParts_c* menuMainCachedChild(gptr<hdParts_c>* data, u32 count, u32 index) {
    return *menuMainView<gptr<hdParts_c>>(gabi::ea(data) + (index < count ? index : 0) * sizeof(gptr<hdParts_c>));
}

/* 02697120 */
void hdMenuMain_c::resetButtons() {
    gptr<hdParts_c>* data = mChildren.mpData.get();
    u32 count = mChildren.mNum;
    for (u32 index = 0; index < 0x2E; ++index) {
        auto classification = classifyMenuMainSelection(this, index);
        if (classification.r3 != 0 || index == 1) {
            hdParts_c* child = menuMainCachedChild(data, count, index);
            if (child != NULL) {
                resetMenuMainButton(child);
                count = mChildren.mNum;
                data = mChildren.mpData.get();
            }
        }
    }
    for (u32 index = 0x1F; index < 0x25; ++index) {
        hdParts_c* child = menuMainCachedChild(data, count, index);
        if (child != NULL) {
            resetMenuMainButton(child);
            count = mChildren.mNum;
            data = mChildren.mpData.get();
        }
    }
    hdParts_c* child = menuMainCachedChild(data, count, 0x26);
    if (child != NULL) {
        resetMenuMainButton(child);
        count = mChildren.mNum;
        data = mChildren.mpData.get();
    }
    child = menuMainCachedChild(data, count, 0x27);
    if (child != NULL) {
        resetMenuMainButton(child);
        count = mChildren.mNum;
        data = mChildren.mpData.get();
    }
    for (u32 index = 0; index < 4; ++index) {
        hdParts_c* parent = menuMainCachedChild(data, count, 0x25);
        child = *parent->mChildren.safeSlot(index);
        if (child != NULL) {
            resetMenuMainButton(child);
            count = mChildren.mNum;
            data = mChildren.mpData.get();
        }
    }
    child = menuMainCachedChild(data, count, 0x29);
    if (child != NULL) resetMenuMainButton(child);
}

using hdMenuMainInputManagerPtr = gptr<hdInputManager>;
static hdMenuMainInputManagerPtr sMenuMainInputManager;
static hdMenuPaneVector sMenuMainInvalidPointer;
static u32 isMenuMainAlternateConfirmEvent(hdButtonEvent_l* event);
static u32 hitTestMenuMainCursorPane(hdMenuMain_c* self, hdMenuPaneVector* point);
static void selectMenuMainCurrentChild(hdMenuMain_c* self);

/* 02691890 */
void hdMenuMain_c::updateCursorSelection() {
    hdButtonEvent_l event;
    copyMenuMainEvent(&event, &sMenuMainInputOwner->mCurrentEvent);
    if (isMenuMainConfirmEvent(&event) || isMenuMainAlternateConfirmEvent(&event)) {
        selectMenuMainCurrentChild(this);
        return;
    }
    hdPointerController* pointer = sMenuMainInputManager->pointer();
    hdMenuPaneVector* point = &sMenuMainInvalidPointer;
    if ((u32(pointer->mHold) & INPUT_BUTTON_TOUCH) != 0) {
        if ((u32(pointer->mPointerFlags) & INPUT_POINTER_VALID) != 0)
            point = menuMainView<hdMenuPaneVector>(&pointer->mCalibratedX);
        if (hitTestMenuMainCursorPane(this, point) == 0) mInputBlocked = 1;
        return;
    }
    if ((u32(pointer->mRepeat) & INPUT_BUTTON_TOUCH) != 0) {
        if (mInputBlocked == 0) return;
        if ((u32(pointer->mPointerFlags) & INPUT_POINTER_VALID) != 0)
            point = menuMainView<hdMenuPaneVector>(&pointer->mCalibratedX);
        if (hitTestMenuMainCursorPane(this, point) != 0) mInputBlocked = 0;
        return;
    }
    if ((u32(pointer->mTrigger) & INPUT_BUTTON_TOUCH) == 0 || mInputBlocked == 0 || mInputEnabled == 0) return;
    if (getMenuMainInputActionKey(sMenuMainInputOwner.get()) != 0) return;
    if (hitTestMenuMainCursorPane(this, menuMainView<hdMenuPaneVector>(&pointer->mCalibratedX)) == 0)
        selectMenuMainCurrentChild(this);
}

// This transition copies translation words with lwz/stw, preserving their IEEE bits.
static void executeMenuMainSaveChildren(hdMenuMain_c* self);
static void enterMenuMainSaveChildren(hdMenuMain_c* self);

/* 026926F8 */
void hdMenuMain_c::beginSaveTransition() {
    selectMenuMainInput(sMenuMainInputOwner.get(), NULL);
    hdMenuPane_l* pane = *mWindowPanes.safeSlot(1);
    pane->mFlags = (u8(pane->mFlags) & ~1u) + 1;
    u32 childIndex = *gabi::at<be<u32>>(gabi::ea(&sMenuMainWindowIndices) + u32(mWindowMode) * sizeof(be<u32>));
    pane = *mWindowPanes.safeSlot(childIndex);
    pane->mFlags = u8(pane->mFlags) & ~1u;
    hdParts_c* destinationChild = *mChildren.safeSlot(1);
    childIndex = *gabi::at<be<u32>>(gabi::ea(&sMenuMainWindowIndices) + u32(mWindowMode) * sizeof(be<u32>));
    hdMenuItem_c* sourceChild = menuMainView<hdMenuItem_c>((*mChildren.safeSlot(childIndex)).get());
    hdMenuItem_c* valueChild = menuMainView<hdMenuItem_c>((*mChildren.safeSlot(childIndex)).get());
    const u32 key = sourceChild->mAnimationSlot;
    const u32 value = valueChild->mSlot;
    setMenuMainTransitionItem(destinationChild, key, value);
    const u32 paneCount = mWindowPanes.mCount;
    gptr<hdMenuPane_l>* panes = mWindowPanes.mpPanes.get();
    childIndex = *gabi::at<be<u32>>(gabi::ea(&sMenuMainWindowIndices) + u32(mWindowMode) * sizeof(be<u32>));
    hdPartsPaneSRT_c* destination = menuMainView<hdPartsPaneSRT_c>((*menuMainView<gptr<hdMenuPane_l>>(gabi::ea(panes) + (paneCount > 1 ? 1 : 0) * sizeof(gptr<hdMenuPane_l>))).get());
    hdPartsPaneSRT_c* source = menuMainView<hdPartsPaneSRT_c>((*menuMainView<gptr<hdMenuPane_l>>(gabi::ea(panes) + (childIndex < paneCount ? childIndex : 0) * sizeof(gptr<hdMenuPane_l>))).get());
    const u32 x = source->mTranslationXBits;
    const u8 flags = destination->mFlags;
    destination->mTranslationXBits = x;
    destination->mTranslationYBits = source->mTranslationYBits;
    const u32 z = source->mTranslationZBits;
    destination->mFlags = flags | 0x10;
    destination->mTranslationZBits = z;
    executeMenuMainSaveChildren(this);
    vCalcLayout();
    enterMenuMainTransitionChildren(this);
    enterMenuMainSaveChildren(this);
    hdAnimSet* animations = animSet();
    const u32 animation = u32(mWindowMode) + 9;
    hdAnimSet_play(animations, animation, 5, 1.0f);
}

struct hdMenuMainTransitionChild_l : hdParts_c {
    u8 _50[0x84 - 0x50];
    be<u8> mNestedEntryEnabled;
    u8 _85[3];
    be<u8> mEntryEnabled;
};
WWHD_OFFSET(hdMenuMainTransitionChild_l, mNestedEntryEnabled, 0x84);
WWHD_OFFSET(hdMenuMainTransitionChild_l, mEntryEnabled, 0x88);
static u32 isMenuMainFourthOptionEnabled(hdParts_c* child);

/* 026910E8 */
void hdMenuMain_c::enterWindowTransition() {
    hdParts_c* child = *mChildren.safeSlot(0x1D);
    hdMenuMainTabVtable_l* table = menuMainView<hdMenuMainTabVtable_l>(child->mVtable.get());
    gabi::call_ptr(table->mEnterTab.get(), child);
    child = *mChildren.safeSlot(0x1E);
    table = menuMainView<hdMenuMainTabVtable_l>(child->mVtable.get());
    gabi::call_ptr(table->mEnterTab.get(), child);
    gptr<hdParts_c>* data = mChildren.mpData.get();
    u32 count = mChildren.mNum;
    for (u32 index = 0x1F; index < 0x25; ++index) {
        hdMenuMainTransitionChild_l* entry = menuMainView<hdMenuMainTransitionChild_l>(menuMainCachedChild(data, count, index));
        if (entry != NULL && entry->mEntryEnabled != 0) {
            resetMenuMainChild(entry);
            count = mChildren.mNum;
            data = mChildren.mpData.get();
        }
    }
    hdParts_c* owner = menuMainCachedChild(data, count, 0x25);
    count = owner->mChildren.mNum;
    data = owner->mChildren.mpData.get();
    for (u32 index = 0; index < 3; ++index) {
        hdMenuMainTransitionChild_l* entry = menuMainView<hdMenuMainTransitionChild_l>(menuMainCachedChild(data, count, index));
        if (entry->mNestedEntryEnabled != 0) {
            resetMenuMainChild(menuMainCachedChild(data, count, index));
            count = owner->mChildren.mNum;
            data = owner->mChildren.mpData.get();
        }
    }
    if (isMenuMainFourthOptionEnabled(menuMainCachedChild(data, count, 3)) != 0)
        resetMenuMainChild(*owner->mChildren.safeSlot(3));
    resetMenuMainChild(*mChildren.safeSlot(0x26));
    resetMenuMainChild(*mChildren.safeSlot(0x27));
    const u32 mainCount = mChildren.mNum;
    gptr<hdParts_c>* mainData = mChildren.mpData.get();
    hdMenuMainSaveChild_l* save = menuMainView<hdMenuMainSaveChild_l>(menuMainCachedChild(mainData, mainCount, 0x29));
    if (save->mMenuItemEnabled != 0)
        resetMenuMainChild(menuMainCachedChild(mainData, mainCount, 0x29));
}

struct hdMenuMainInputSample_c {
    be<u32> mWords[2];
    f32 horizontal() const { return __builtin_bit_cast(f32, u32(mWords[0])); }
};
WWHD_SIZE(hdMenuMainInputSample_c, 8);
static Pair32 getMenuMainDragPosition(hdButtonEventManager_l* input);
static Pair32 getMenuMainSlideDelta(hdButtonEventManager_l* input);
static hdPartsStateId_c sMenuMainSliding;

/* 02691C44 */
void hdMenuMain_c::updateTabSlide() {
    hdButtonEventManager_l* input = sMenuMainInputOwner.get();
    hdButtonEvent_l event;
    copyMenuMainEvent(&event, &input->mCurrentEvent);
    if (u32(event.mType) == 0x44 && u32(event.mReceiverId) == u32((*mReceivers.mpData.get())->mId)) {
        hdMenuMainInputSample_c position;
        auto result = getMenuMainDragPosition(input);
        position.mWords[0] = result.r3;
        const u32 mode = mSelectionMode;
        const float x = position.horizontal();
        position.mWords[1] = result.r4;
        if (mode == 0) {
            setMenuMainPaneTranslation(this, (x < 0.0f) ? x : 0.0f);
        } else {
            setMenuMainPaneTranslation(this, (0.0f < x) ? gabi::fadds_ppc(x, -1280.0f) : -1280.0f);
        }
        vCalcLayout();
        return;
    }
    hdMenuMainInputSample_c delta;
    auto result = getMenuMainSlideDelta(input);
    delta.mWords[0] = result.r3;
    delta.mWords[1] = result.r4;
    const u32 mode = mSelectionMode;
    const float paneX = (*mPanes.slot(0))->mTranslationX;
    if (mode == 0) {
        const float x = delta.horizontal();
        if ((x < -500.0f)) {
            const float distance = gabi::fsubs_ppc(-1280.0f, paneX);
            const float step = distance / 3.0f;
            mSelectionMode = 1;
            mTransitionProgress = step;
        } else if ((x < 0.0f)) {
            const float step = paneX / 3.0f;
            mTransitionProgress = -step;
        }
    } else if (mode == 1) {
        const float x = delta.horizontal();
        if ((500.0f < x)) {
            const float step = paneX / 3.0f;
            mSelectionMode = 0;
            mTransitionProgress = -step;
        } else if ((0.0f < x)) {
            const float distance = gabi::fsubs_ppc(-1280.0f, paneX);
            mTransitionProgress = distance / 3.0f;
        }
    }
    changeState(&sMenuMainSliding);
    vCalcLayout();
}

struct hdMenuMainCategoryButton_l : hdParts_c {
    u8 _50[0x84 - 0x50];
    be<u32> mCategory;
};
WWHD_OFFSET(hdMenuMainCategoryButton_l, mCategory, 0x84);
static void refreshMenuMainFourthOption(hdParts_c* child);
static void setupMenuMainItemLabel(hdParts_c* child);

/* 02693170 */
void hdMenuMain_c::setupItemNames() {
    initializeMenuMainCollect(*mChildren.safeSlot(0x2D));
    executeMenuMainTransitionChildren(this);
    for (u32 index = 0; index < 6; ++index) {
        hdMenuMainCategoryButton_l* child = menuMainView<hdMenuMainCategoryButton_l>((*mChildren.safeSlot(index + 0x1F)).get());
        child->mCategory = index == 0 ? 0 : index + 1;
    }
    executeMenuMainWindowTransitionEntry(this);
    hdParts_c* owner = *mChildren.safeSlot(0x25);
    refreshMenuMainFourthOption(*owner->mChildren.safeSlot(3));
    setupMenuMainItemLabel(*mChildren.safeSlot(3));
    setupMenuMainItemLabel(*mChildren.safeSlot(2));
    setupMenuMainItemLabel(*mChildren.safeSlot(7));
    setupMenuMainItemLabel(*mChildren.safeSlot(6));
}

struct hdMenuMainReceiverNames_c {
    hdScreenString mNames[3];
    hdScreenString* at(u32 index) { return menuMainView<hdScreenString>(gabi::ea(this) + index * sizeof(hdScreenString)); }
};
static hdMenuMainReceiverNames_c sMenuMainReceiverNames;
using hdMenuMainTypeGuard = be<u32>;
static hdMenuMainTypeGuard sMenuMainReceiverTypeInitialized;
static hdMenuMainTypeGuard sMenuMainPaneTypeInitialized;
static hdMenuMainTypeGuard sMenuMainObjectTypeInitialized;
static hdScreenType sMenuMainReceiverType;
static hdScreenType sMenuMainPaneType;
static hdScreenType sMenuMainObjectType;
static hdButtonReceiver_l* constructMenuMainReceiver(hdButtonReceiver_l* receiver, hdPartsPane_c* pane, u32 option);

/* 02692F10 */
u32 hdMenuMain_c::createReceivers() {
    hdScreenHeap* receiverHeap = sMenuMainUiSystem->mHeap.get();
    hdScreenHeap* arrayHeap = receiverHeap;
    if (arrayHeap == NULL) arrayHeap = hdParts_getCurrentHeap(sMenuMainHeapManager.get());
    void* storage = arrayHeap->allocate(3 * sizeof(gptr<hdButtonReceiver_l>));
    if (storage != NULL) {
        mReceivers.mpData = menuMainView<gptr<hdButtonReceiver_l>>(storage);
        mReceivers.mCount = 3;
    }
    const s32 clearCount = mReceivers.mCount;
    for (s32 index = 0; index < clearCount; ++index)
        *menuMainView<gptr<hdButtonReceiver_l>>(gabi::ea(mReceivers.mpData.get()) + u32(index) * sizeof(gptr<hdButtonReceiver_l>)) = NULL;
    for (s32 index = 0; index < s32(mReceivers.mCount); ++index) {
        hdScreenString* name = sMenuMainReceiverNames.at(u32(index));
        hdPartsLayout_c* layout = mpLayout.get();
        hdScreenAccessor* accessor = layout->mAccessor.get();
        // The original retains the layout in r4 at the string's virtual call (02692FF0).
        gabi::cpu->r[4] = gabi::ea(layout);
        name->assureTermination();
        hdPartsPane_c* pane = menuMainView<hdPartsPane_c>(accessor->mObject->findPane(name->mText.get()));
        if (sMenuMainReceiverTypeInitialized == 0) {
            sMenuMainReceiverTypeInitialized = 1;
            if (sMenuMainPaneTypeInitialized == 0) {
                sMenuMainPaneTypeInitialized = 1;
                if (sMenuMainObjectTypeInitialized == 0) {
                    sMenuMainObjectType.mParent = NULL;
                    sMenuMainObjectTypeInitialized = 1;
                }
                sMenuMainPaneType.mParent = &sMenuMainObjectType;
            }
            sMenuMainReceiverType.mParent = &sMenuMainPaneType;
        }
        bool matches = false;
        if (pane != NULL) {
            hdScreenType* type = pane->getRuntimeTypeInfo();
            while (type != NULL) {
                if (type == &sMenuMainReceiverType) { matches = true; break; }
                type = type->mParent.get();
            }
        }
        if (!matches) pane = NULL;
        gptr<hdButtonReceiver_l>* slot = mReceivers.safeSlot(u32(index));
        hdButtonReceiver_l* receiver = menuMainView<hdButtonReceiver_l>(allocateMenuMainLayout(sizeof(hdButtonReceiver_l), receiverHeap, 4));
        if (receiver != NULL) receiver = constructMenuMainReceiver(receiver, pane, 0);
        *slot = receiver;
        if (*mReceivers.safeSlot(u32(index)) == NULL) return 0;
    }
    return 1;
}

struct hdMenuMainWideString_c {
    gptr<be<u16>> mpText;
    gptr<hdScreenStringTable> mVtable;
};
struct hdMenuMainTabTextScratch_c {
    hdMenuMainWideString_c mSource;
    PaneString_l mFixed;
    be<u16> mBuffer[2];
};
WWHD_OFFSET(hdMenuMainTabTextScratch_c, mFixed, 8);
WWHD_OFFSET(hdMenuMainTabTextScratch_c, mBuffer, 0x14);
using hdMenuMainWideCharacter = be<u16>;
static hdMenuMainWideCharacter sMenuMainEmptyWideText;
static hdScreenStringTable sMenuMainWideStringVtable;
static gfn sMenuMainBufferedWideStringVtable;
static gfn sMenuMainFixedWideStringVtable;
static void normalizeMenuMainWideString(hdMenuMainWideString_c* text);
static void copyMenuMainWideText(void* destination, void* source, u32 size, u32 option);
static void setMenuMainTabText(hdTextPane_c* child, PaneString_l* text, PaneWork_l* work);

/* 0268EE9C */
void hdMenuMain_c::clearTabText(u32 index) {
    hdMenuMainTabTextScratch_c scratch;
    scratch.mSource.mVtable = &sMenuMainWideStringVtable;
    scratch.mFixed.mpVTable = &sMenuMainBufferedWideStringVtable;
    scratch.mFixed.mpBuffer = scratch.mBuffer;
    scratch.mBuffer[1] = 0;
    scratch.mFixed.mCapacity = 2;
    scratch.mSource.mpText = &sMenuMainEmptyWideText;
    normalizeMenuMainWideString(&scratch.mSource);
    be<u16>* text = scratch.mSource.mpText.get();
    s32 length = 0;
    if (*text != 0) {
        for (;;) {
            ++length;
            if (length > 0x40000) { length = 0; break; }
            text = menuMainView<be<u16>>(gabi::ea(text) + sizeof(be<u16>));
            if (*text == 0) break;
        }
    }
    const s32 capacity = scratch.mFixed.mCapacity;
    const u32 assure = scratch.mSource.mVtable->mAssureTermination.get();
    if (length >= capacity) length = capacity - 1;
    gabi::call_ptr(assure, &scratch.mSource);
    copyMenuMainWideText(scratch.mBuffer, scratch.mSource.mpText.get(), u32(length) * sizeof(be<u16>), 0);
    *menuMainView<be<u16>>(gabi::ea(scratch.mBuffer) + u32(length) * sizeof(be<u16>)) = 0;
    scratch.mFixed.mpVTable = &sMenuMainFixedWideStringVtable;
    hdTextPane_c* child = menuMainView<hdTextPane_c>((*mTabChildren.safeSlot(index)).get());
    setMenuMainTabText(child, &scratch.mFixed, NULL);
}

static u32 hitTestMenuMainWindowChildren(hdMenuMain_c* self, hdMenuPaneVector* point);
static void closeMenuMainOptionWindow(hdMenuMain_c* self);

/* 0268FABC */
void hdMenuMain_c::updateWindowCloseRequest() {
    hdButtonEvent_l event;
    copyMenuMainEvent(&event, &sMenuMainInputOwner->mCurrentEvent);
    if (isMenuMainTabEvent(event.mType)) {
        const u32 key = getMenuMainButtonKey(*mChildren.safeSlot(0x1C));
        if (u32(event.mReceiverId) == key) closeMenuMainOptionWindow(this);
        return;
    }
    if (isMenuMainConfirmEvent(&event)) {
        if (sMenuMainInputMode->mDirectionalInputEnabled == 0) return;
        hdButtonReceiver_l* receiver = getMenuMainInputReceiver(sMenuMainInputOwner.get());
        if (receiver == NULL) return;
        const u32 key = getMenuMainButtonKey(*mChildren.safeSlot(0x1C));
        if (u32(receiver->mId) == key) closeMenuMainOptionWindow(this);
        return;
    }
    if (isMenuMainAlternateConfirmEvent(&event)) {
        if (sMenuMainInputMode->mDirectionalInputEnabled != 0 && mInputEnabled != 0)
            closeMenuMainOptionWindow(this);
        return;
    }
    hdPointerController* pointer = sMenuMainInputManager->pointer();
    hdMenuPaneVector* point = &sMenuMainInvalidPointer;
    if ((u32(pointer->mHold) & INPUT_BUTTON_TOUCH) != 0) {
        if ((u32(pointer->mPointerFlags) & INPUT_POINTER_VALID) != 0)
            point = menuMainView<hdMenuPaneVector>(&pointer->mCalibratedX);
        if (hitTestMenuMainWindowChildren(this, point) == 0) mInputBlocked = 1;
        return;
    }
    if ((u32(pointer->mRepeat) & INPUT_BUTTON_TOUCH) != 0) {
        if (mInputBlocked == 0) return;
        if ((u32(pointer->mPointerFlags) & INPUT_POINTER_VALID) != 0)
            point = menuMainView<hdMenuPaneVector>(&pointer->mCalibratedX);
        if (hitTestMenuMainWindowChildren(this, point) != 0) mInputBlocked = 0;
        return;
    }
    if ((u32(pointer->mTrigger) & INPUT_BUTTON_TOUCH) == 0 || mInputBlocked == 0 || mInputEnabled == 0) return;
    if (getMenuMainInputActionKey(sMenuMainInputOwner.get()) != 0) return;
    if (hitTestMenuMainWindowChildren(this, menuMainView<hdMenuPaneVector>(&pointer->mCalibratedX)) == 0)
        closeMenuMainOptionWindow(this);
}

struct hdMenuMainPaneNames_c { hdScreenString mNames[6]; };
struct hdMenuMainTabNames_c { hdScreenString mNames[5]; };
struct hdMenuMainWindowPaneNames_c { hdScreenString mNames[0x2E]; };
struct hdMenuMainTabFonts_c { be<u32> mIndices[5]; };
static hdMenuMainPaneNames_c sMenuMainPaneNames;
static hdMenuMainTabNames_c sMenuMainTabNames;
static hdMenuMainWindowPaneNames_c sMenuMainWindowPaneNames;
static hdMenuMainTabFonts_c sMenuMainTabFonts;
static u32 bindMenuMainPanes(hdPartsLayout_c* layout, hdMenuMainPaneArray_c* panes, hdScreenString* names);
static u32 bindMenuMainTabs(hdPartsLayout_c* layout, hdPartsChildArray_c* tabs, hdScreenString* names);
static u32 bindMenuMainWindowPanes(hdPartsLayout_c* layout, hdMenuMainPaneArray_c* panes, hdScreenString* names);
static void setMenuMainTabFont(hdParts_c* tab, u32 font);
static void clearMenuMainTabText(hdMenuMain_c* self, u32 index);

/* 02692C80 */
u32 hdMenuMain_c::createPaneArrays() {
    hdScreenHeap* heap = sMenuMainUiSystem->mHeap.get();
    hdScreenHeap* arrayHeap = heap;
    if (arrayHeap == NULL) arrayHeap = hdParts_getCurrentHeap(sMenuMainHeapManager.get());
    void* storage = arrayHeap->allocate(6 * sizeof(gptr<hdMenuPane_l>));
    if (storage != NULL) {
        mPanes.mpPanes = menuMainView<gptr<hdMenuPane_l>>(storage);
        mPanes.mCount = 6;
    }
    const s32 clearPanes = mPanes.mCount;
    for (s32 index = 0; index < clearPanes; ++index) *mPanes.slot(u32(index)) = NULL;
    hdMenuMainPaneArray_c* panes = menuMainView<hdMenuMainPaneArray_c>(gabi::ea(this) + offsetof(hdMenuMain_c, mPanes));
    if (!bindMenuMainPanes(mpLayout.get(), panes, sMenuMainPaneNames.mNames)) return 0;
    for (u32 index = 2; index < 5; ++index) {
        hdMenuPane_l* pane = *mPanes.safeSlot(index);
        pane->mFlags = u8(pane->mFlags) & ~1u;
    }
    arrayHeap = heap;
    if (arrayHeap == NULL) arrayHeap = hdParts_getCurrentHeap(sMenuMainHeapManager.get());
    storage = arrayHeap->allocate(5 * sizeof(gptr<hdParts_c>));
    if (storage != NULL) {
        mTabChildren.mpData = menuMainView<gptr<hdParts_c>>(storage);
        mTabChildren.mNum = 5;
    }
    const s32 clearTabs = mTabChildren.mNum;
    for (s32 index = 0; index < clearTabs; ++index) *mTabChildren.slot(u32(index)) = NULL;
    hdPartsChildArray_c* tabs = menuMainView<hdPartsChildArray_c>(gabi::ea(this) + offsetof(hdMenuMain_c, mTabChildren));
    if (!bindMenuMainTabs(mpLayout.get(), tabs, sMenuMainTabNames.mNames)) return 0;
    for (u32 index = 0; index < 5; ++index) {
        hdParts_c* tab = *mTabChildren.safeSlot(index);
        const u32 font = sMenuMainTabFonts.mIndices[index];
        setMenuMainTabFont(tab, font);
        clearMenuMainTabText(this, index);
    }
    arrayHeap = heap;
    if (arrayHeap == NULL) arrayHeap = hdParts_getCurrentHeap(sMenuMainHeapManager.get());
    storage = arrayHeap->allocate(0x2E * sizeof(gptr<hdMenuPane_l>));
    if (storage != NULL) {
        mWindowPanes.mpPanes = menuMainView<gptr<hdMenuPane_l>>(storage);
        mWindowPanes.mCount = 0x2E;
    }
    const s32 clearWindowPanes = mWindowPanes.mCount;
    for (s32 index = 0; index < clearWindowPanes; ++index) *mWindowPanes.slot(u32(index)) = NULL;
    hdMenuMainPaneArray_c* windowPanes = menuMainView<hdMenuMainPaneArray_c>(gabi::ea(this) + offsetof(hdMenuMain_c, mWindowPanes));
    return bindMenuMainWindowPanes(mpLayout.get(), windowPanes, sMenuMainWindowPaneNames.mNames) != 0 ? 1 : 0;
}

static u32 getMenuMainCategoryPrimaryHint(hdParts_c* child);
static u32 getMenuMainCategorySecondaryHint(hdParts_c* child);
static u32 getMenuMainOptionPrimaryHint(hdParts_c* child);
static u32 getMenuMainOptionSecondaryHint(hdParts_c* child);
static u32 getMenuMainFourthPrimaryHint(hdParts_c* child);
static u32 getMenuMainFourthSecondaryHint(hdParts_c* child);
static u32 findMenuMainOptionIndex(hdParts_c* owner, u32 key);
static void setMenuMainHintAnimation(hdAnimSet* set, u32 entry, u32 slot);
// The original caller supplies r5; 0268FFC4 obtains its own resource name and ignores r5.
static void setMenuMainTabResourceHint(hdMenuMain_c* self, u32 index, hdUiSafeString* unusedHint);

/* 02690020 */
void hdMenuMain_c::updateSelectionHints(u32 key) {
    mTransitionFinished = 0;
    const u32 childIndex = lookupMenuMainChildIndex(this, key);
    if (childIndex - 0x1F <= 5) {
        hdMenuMainTransitionChild_l* child = menuMainView<hdMenuMainTransitionChild_l>((*mChildren.safeSlot(childIndex)).get());
        if (child == NULL) return;
        if (child->mEntryEnabled != 0) {
            u32 hint = getMenuMainCategoryPrimaryHint(child);
            updateMenuMainTabChild(this, 1, hint);
            hint = getMenuMainCategorySecondaryHint(child);
            updateMenuMainTabChild(this, 2, hint);
            setMenuMainHintAnimation(animSet(), 8, 4);
            mTransitionFinished = 1;
            return;
        }
    } else if (childIndex == 0x26 || childIndex == 0x27) {
        updateMenuMainTabChild(this, 1, childIndex == 0x26 ? 0x194 : 0x1F7);
        updateMenuMainTabChild(this, 2, childIndex == 0x26 ? 0x25C : 0x2BF);
        setMenuMainHintAnimation(animSet(), 8, 4);
        mTransitionFinished = 1;
        return;
    } else if (childIndex == 0x29 || childIndex == 0x1D || childIndex == 0x1E) {
        bool enabled = true;
        if (childIndex == 0x29) {
            hdMenuMainSaveChild_l* child = menuMainView<hdMenuMainSaveChild_l>((*mChildren.safeSlot(0x29)).get());
            if (child == NULL) return;
            enabled = child->mMenuItemEnabled != 0;
        }
        if (enabled) {
            const u32 first = childIndex == 0x29 ? 0x19E : childIndex == 0x1D ? 0x1F8 : 0x19D;
            updateMenuMainTabChild(this, 1, first);
            u32 label = childIndex == 0x29 ? 8 : childIndex == 0x1D ? 6 : 7;
            if (sMenuMainInputMode->mDirectionalInputEnabled != 0) label -= 3;
            hdUiSafeString* text = menuMainView<hdUiSafeString>(gabi::ea(&sMenuMainWindowLabels) + label * sizeof(hdUiSafeString));
            setMenuMainTabResourceHint(this, 4, text);
            const u32 second = childIndex == 0x29 ? 0x266 : childIndex == 0x1D ? 0x2C0 : 0x2C1;
            updateMenuMainTabChild(this, 2, second);
            hdAnimSet_play(animSet(), 8, 4, 1.0f);
            mTransitionFinished = 1;
            return;
        }
    } else if (childIndex != 0x28) {
        hdParts_c* owner = *mChildren.safeSlot(0x25);
        const u32 option = findMenuMainOptionIndex(owner, key);
        if (option <= 2) {
            hdMenuMainTransitionChild_l* child = menuMainView<hdMenuMainTransitionChild_l>((*owner->mChildren.safeSlot(option)).get());
            if (child == NULL) return;
            if (child->mNestedEntryEnabled != 0) {
                u32 hint = getMenuMainOptionPrimaryHint(child);
                updateMenuMainTabChild(this, 1, hint);
                hint = getMenuMainOptionSecondaryHint(child);
                updateMenuMainTabChild(this, 2, hint);
                setMenuMainHintAnimation(animSet(), 8, 4);
                mTransitionFinished = 1;
                return;
            }
        } else if (option == 3) {
            hdParts_c* child = *owner->mChildren.safeSlot(3);
            if (child == NULL) return;
            if (isMenuMainFourthOptionEnabled(child) != 0) {
                u32 hint = getMenuMainFourthPrimaryHint(child);
                updateMenuMainTabChild(this, 1, hint);
                hint = getMenuMainFourthSecondaryHint(child);
                updateMenuMainTabChild(this, 2, hint);
                setMenuMainHintAnimation(animSet(), 8, 4);
                mTransitionFinished = 1;
                return;
            }
        }
    }
    clearMenuMainTabText(this, 1);
    clearMenuMainTabText(this, 4);
    clearMenuMainTabText(this, 2);
}

struct hdMenuMainClassNames_c { hdScreenString mNames[0x2E]; };
struct hdMenuMainStaticRecord_l {
    gptr<hdMenuMainStaticRecord_l> mNext;
    gptr<void> mpObject;
    gfn mDestroy;
};
WWHD_OFFSET(hdMenuMainStaticRecord_l, mpObject, 4);
WWHD_OFFSET(hdMenuMainStaticRecord_l, mDestroy, 8);
WWHD_SIZE(hdMenuMainStaticRecord_l, 12);
struct hdMenuMainRandomState_c { be<u32> mWords[4]; };
using hdMenuMainStaticFloat = be<f32>;
using hdMenuMainStaticByte = be<u8>;
static hdMenuMainClassNames_c sMenuMainClassNames;
static hdMenuMainStaticRecord_l sMenuMainRandomRegistration;
static hdMenuMainStaticRecord_l sMenuMainSignedRegistration;
static hdMenuMainStaticRecord_l sMenuMainUnsignedRegistration;
static hdMenuMainRandomState_c sMenuMainRandomState;
static hdMenuMainStaticFloat sMenuMainMinAngle;
static hdMenuMainStaticFloat sMenuMainMaxAngle;
static hdMenuMainStaticByte sMenuMainSignedFlag;
static hdMenuMainStaticByte sMenuMainUnsignedFlag;
static hdMenuMainTypeGuard sMenuMainStateCounterInitialized;
static hdMenuMainTypeGuard sMenuMainStateCounter;
static hdPartsStateIdVtbl_c sMenuMainStateVtable;
static hdStateId sMenuMainNullState;
static void registerMenuMainStatic(hdMenuMainStaticRecord_l* record);
static void constructMenuMainSignedFlag(be<u8>* flag);
static void constructMenuMainUnsignedFlag(be<u8>* flag);
// These canonical SafeString views share the same two-word ABI and actual vtable.
template <typename T> static void initializeMenuMainText(T* destination, const char* text) {
    destination->mText.v = gabi::guest_addr(text);
    destination->mVtable.v = gabi::ea(&sMenuMainResourceNameVtable);
}
static hdPartsStateId_c sMenuMainDrag;
static hdPartsStateId_c sMenuMainInArrow;
static hdPartsStateId_c sMenuMainOutArrow;
static hdPartsStateId_c sMenuMainTextFade;

/* 0269735C */
static void initializeMenuMainStatics() {
    sMenuMainRandomState.mWords[3] = 0;
    sMenuMainRandomState.mWords[2] = 0;
    sMenuMainRandomState.mWords[1] = 0;
    sMenuMainRandomState.mWords[0] = 0;
    registerMenuMainStatic(&sMenuMainRandomRegistration);
    sMenuMainMinAngle = -3.1415927410125732f;
    sMenuMainMaxAngle = 3.1415927410125732f;
    constructMenuMainSignedFlag(&sMenuMainSignedFlag);
    registerMenuMainStatic(&sMenuMainSignedRegistration);
    constructMenuMainUnsignedFlag(&sMenuMainUnsignedFlag);
    registerMenuMainStatic(&sMenuMainUnsignedRegistration);
    // The remaining original stores have no intervening calls or escaping reads.
    // Group the disjoint static string and state fields into their actual objects.
    initializeMenuMainText(&sMenuMainAnimationGroupNames.mNames[0], "G_All_00");
    initializeMenuMainText(&sMenuMainAnimationGroupNames.mNames[1], "G_ArrowL_00");
    initializeMenuMainText(&sMenuMainAnimationGroupNames.mNames[2], "G_ArrowR_00");
    initializeMenuMainText(&sMenuMainAnimationGroupNames.mNames[3], "G_ItemCaption_00");
    initializeMenuMainText(&sMenuMainAnimationGroupNames.mNames[4], "G_TextFade_00");
    initializeMenuMainText(&sMenuMainAnimationGroupNames.mNames[5], "G_BagItemGame_00");
    initializeMenuMainText(&sMenuMainAnimationGroupNames.mNames[6], "G_BagItemFeed_00");
    initializeMenuMainText(&sMenuMainAnimationGroupNames.mNames[7], "G_BagItemDelivery_00");
    initializeMenuMainText(&sMenuMainReceiverNames.mNames[0], "B_Slide_00");
    initializeMenuMainText(&sMenuMainReceiverNames.mNames[1], "B_SlideR_00");
    initializeMenuMainText(&sMenuMainReceiverNames.mNames[2], "B_SlideL_00");
    initializeMenuMainText(&sMenuMainPaneNames.mNames[0], "N_All_00");
    initializeMenuMainText(&sMenuMainPaneNames.mNames[1], "N_All_01");
    initializeMenuMainText(&sMenuMainPaneNames.mNames[2], "N_BagBaseArea_00");
    initializeMenuMainText(&sMenuMainPaneNames.mNames[3], "N_BagBaseArea_01");
    initializeMenuMainText(&sMenuMainPaneNames.mNames[4], "N_BagBaseArea_02");
    initializeMenuMainText(&sMenuMainPaneNames.mNames[5], "N_BaseArea_00");
    initializeMenuMainText(&sMenuMainTabNames.mNames[0], "T_TitleL_00");
    initializeMenuMainText(&sMenuMainTabNames.mNames[1], "T_TitleR_00");
    initializeMenuMainText(&sMenuMainTabNames.mNames[2], "T_TextR_00");
    initializeMenuMainText(&sMenuMainTabNames.mNames[3], "T_TextL_00");
    initializeMenuMainText(&sMenuMainTabNames.mNames[4], "T_TitleR_01");
    initializeMenuMainText(&sMenuMainAnimationNames.mNames[0], "InAll");
    initializeMenuMainText(&sMenuMainAnimationNames.mNames[1], "OutAll");
    initializeMenuMainText(&sMenuMainAnimationNames.mNames[2], "InArrow");
    initializeMenuMainText(&sMenuMainAnimationNames.mNames[3], "InArrow");
    initializeMenuMainText(&sMenuMainAnimationNames.mNames[4], "OutArrow");
    initializeMenuMainText(&sMenuMainAnimationNames.mNames[5], "OutArrow");
    initializeMenuMainText(&sMenuMainAnimationNames.mNames[6], "ItemCaptionIn");
    initializeMenuMainText(&sMenuMainAnimationNames.mNames[7], "ItemCaptionOut");
    initializeMenuMainText(&sMenuMainAnimationNames.mNames[8], "TextFade");
    initializeMenuMainText(&sMenuMainAnimationNames.mNames[9], "BagItemIn");
    initializeMenuMainText(&sMenuMainAnimationNames.mNames[10], "BagItemIn");
    initializeMenuMainText(&sMenuMainAnimationNames.mNames[11], "BagItemIn");
    initializeMenuMainText(&sMenuMainAnimationNames.mNames[12], "BagItemOut");
    initializeMenuMainText(&sMenuMainAnimationNames.mNames[13], "BagItemOut");
    initializeMenuMainText(&sMenuMainAnimationNames.mNames[14], "BagItemOut");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[0], "L_BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[1], "L_BtnItemIconFront_00");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[2], "L_BtnItemIcon_06");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[3], "L_BtnItemIcon_05");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[4], "L_BtnItemIcon_07");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[5], "L_BtnItemIcon_14");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[6], "L_BtnItemIcon_13");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[7], "L_BtnItemIcon_12");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[8], "L_BtnItemIcon_02");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[9], "L_BtnItemIcon_09");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[10], "L_BtnItemIcon_16");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[11], "L_BtnItemIcon_03");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[12], "L_BtnItemIcon_10");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[13], "L_BtnItemIcon_17");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[14], "L_BtnItemIcon_04");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[15], "L_BtnItemIcon_11");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[16], "L_BtnItemIcon_18");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[17], "L_MenuArrowR_00");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[18], "L_MenuSave_00");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[19], "L_MenuOption_00");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[20], "L_BtnItemIcon_19");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[21], "L_BtnItemIcon_20");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[22], "L_BtnItemIcon_21");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[23], "L_BtnItemIcon_22");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[24], "L_BtnItemIcon_23");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[25], "L_BtnItemIcon_24");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[26], "L_BtnItemIcon_25");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[27], "L_BtnItemIcon_26");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[28], "L_BtnBack_00");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[29], "L_MenuSave_01");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[30], "L_MenuOption_01");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[31], "L_BtnCollectIcon_00");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[32], "L_BtnCollectIcon_01");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[33], "L_BtnCollectIcon_02");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[34], "L_BtnCollectIcon_03");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[35], "L_BtnCollectIcon_04");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[36], "L_BtnCollectIcon_05");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[37], "L_Collection_00");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[38], "L_BtnMenuHeart_00");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[39], "L_BtnMapIcon_00");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[40], "L_MenuArrowL_00");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[41], "L_BtnCollectIconMask_00");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[42], "L_BtnItemIcon_01");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[43], "L_BtnItemIcon_08");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[44], "L_BtnItemIcon_15");
    initializeMenuMainText(&sMenuMainWindowPaneNames.mNames[45], "L_IconSlideArea_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[0], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[1], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[2], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[3], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[4], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[5], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[6], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[7], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[8], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[9], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[10], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[11], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[12], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[13], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[14], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[15], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[16], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[17], "BtnMenuArrow_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[18], "BtnMenuSaveOption_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[19], "BtnMenuSaveOption_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[20], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[21], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[22], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[23], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[24], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[25], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[26], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[27], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[28], "BtnBack_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[29], "BtnMenuSaveOption_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[30], "BtnMenuSaveOption_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[31], "BtnCollectIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[32], "BtnCollectIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[33], "BtnCollectIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[34], "BtnCollectIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[35], "BtnCollectIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[36], "BtnCollectIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[37], "Collection_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[38], "BtnMenuHeart_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[39], "BtnMapIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[40], "BtnMenuArrow_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[41], "BtnCollectIconMask_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[42], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[43], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[44], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainClassNames.mNames[45], "IconSlideArea_00");
    initializeMenuMainText(&sMenuMainArchiveNames.mNames[0], "MenuItem_00");
    initializeMenuMainText(&sMenuMainArchiveNames.mNames[1], "BtnItemIcon_00");
    initializeMenuMainText(&sMenuMainArchiveNames.mNames[2], "BtnMenuArrow_00");
    initializeMenuMainText(&sMenuMainArchiveNames.mNames[3], "BtnMenuSaveOption_00");
    initializeMenuMainText(&sMenuMainArchiveNames.mNames[4], "BtnBack_00");
    initializeMenuMainText(&sMenuMainArchiveNames.mNames[5], "BtnCollectIcon_00");
    initializeMenuMainText(&sMenuMainArchiveNames.mNames[6], "Collection_00");
    initializeMenuMainText(&sMenuMainArchiveNames.mNames[7], "BtnMenuHeart_00");
    initializeMenuMainText(&sMenuMainArchiveNames.mNames[8], "BtnMapIcon_00");
    initializeMenuMainText(&sMenuMainArchiveNames.mNames[9], "BtnCollectIconMask_00");
    initializeMenuMainText(&sMenuMainArchiveNames.mNames[10], "IconSlideArea_00");
    initializeMenuMainText(&sMenuMainArchiveNames.mNames[11], "BtnPearl_00");
    initializeMenuMainText(&sMenuMainArchiveNames.mNames[12], "BtnTriforce_00");
    initializeMenuMainText(&sMenuMainWindowLabels.mNames[0], "L_MenuSave_00-T_Btn_00");
    initializeMenuMainText(&sMenuMainWindowLabels.mNames[1], "L_MenuOption_00-T_Btn_00");
    initializeMenuMainText(&sMenuMainWindowLabels.mNames[2], "L_BtnBack_00-T_Btn_00");
    initializeMenuMainText(&sMenuMainWindowLabels.mNames[3], "T_TitleR_01_01");
    initializeMenuMainText(&sMenuMainWindowLabels.mNames[4], "T_TitleR_01_00");
    initializeMenuMainText(&sMenuMainWindowLabels.mNames[5], "T_TitleR_01_02");
    initializeMenuMainText(&sMenuMainWindowLabels.mNames[6], "T_TitleR_01_04");
    initializeMenuMainText(&sMenuMainWindowLabels.mNames[7], "T_TitleR_01_03");
    initializeMenuMainText(&sMenuMainWindowLabels.mNames[8], "T_TitleR_01_05");
    initializeMenuMainText(&sMenuMainLayoutName, "MenuItem_00");
    u32 nextState;
    if (sMenuMainStateCounterInitialized != 0) nextState = sMenuMainStateCounter;
    else {
        nextState = 0;
        sMenuMainStateCounterInitialized = 1;
    }
    ++nextState;
    sMenuMainSecondaryReady.setup(s32(nextState), "::StateID_EquipItem", &sMenuMainStateVtable, 25, &sMenuMainNullState);
    ++nextState;
    sMenuMainPrimaryReady.setup(s32(nextState), "::StateID_CollectItem", &sMenuMainStateVtable, 31, &sMenuMainNullState);
    ++nextState;
    sMenuMainWindowSlid.setup(s32(nextState), "::StateID_InArrowL", &sMenuMainStateVtable, 34, &sMenuMainNullState);
    ++nextState;
    sMenuMainWindowReturned.setup(s32(nextState), "::StateID_InArrowR", &sMenuMainStateVtable, 37, &sMenuMainNullState);
    ++nextState;
    sMenuMainSelectionNormal.setup(s32(nextState), "::StateID_OutArrowL", &sMenuMainStateVtable, 40, &sMenuMainNullState);
    ++nextState;
    sMenuMainSelectionAlternate.setup(s32(nextState), "::StateID_OutArrowR", &sMenuMainStateVtable, 43, &sMenuMainNullState);
    ++nextState;
    sMenuMainSecondaryHidden.setup(s32(nextState), "::StateID_EquipToCollect", &sMenuMainStateVtable, 46, &sMenuMainNullState);
    ++nextState;
    sMenuMainPrimaryHidden.setup(s32(nextState), "::StateID_CollectToEquip", &sMenuMainStateVtable, 49, &sMenuMainNullState);
    ++nextState;
    sMenuMainOptionReady.setup(s32(nextState), "::StateID_ItemCaption", &sMenuMainStateVtable, 52, &sMenuMainNullState);
    ++nextState;
    sMenuMainPendingReady.setup(s32(nextState), "::StateID_TriforceCompleteDemo", &sMenuMainStateVtable, 55, &sMenuMainNullState);
    ++nextState;
    sMenuMainDrag.setup(s32(nextState), "::StateID_Drag", &sMenuMainStateVtable, 58, &sMenuMainNullState);
    ++nextState;
    sMenuMainSliding.setup(s32(nextState), "::StateID_Slide", &sMenuMainStateVtable, 61, &sMenuMainNullState);
    ++nextState;
    sMenuMainInAll.setup(s32(nextState), "::StateID_InAll", &sMenuMainStateVtable, 64, &sMenuMainNullState);
    ++nextState;
    sMenuMainOutAll.setup(s32(nextState), "::StateID_OutAll", &sMenuMainStateVtable, 67, &sMenuMainNullState);
    ++nextState;
    sMenuMainWindowReady.setup(s32(nextState), "::StateID_BagItem", &sMenuMainStateVtable, 28, &sMenuMainNullState);
    ++nextState;
    sMenuMainInArrow.setup(s32(nextState), "::StateID_InArrow", &sMenuMainStateVtable, 70, &sMenuMainNullState);
    ++nextState;
    sMenuMainOutArrow.setup(s32(nextState), "::StateID_OutArrow", &sMenuMainStateVtable, 73, &sMenuMainNullState);
    ++nextState;
    sMenuMainChildOpening.setup(s32(nextState), "::StateID_ItemCaptionIn", &sMenuMainStateVtable, 76, &sMenuMainNullState);
    ++nextState;
    sMenuMainSelectedChildClosing.setup(s32(nextState), "::StateID_ItemCaptionOut", &sMenuMainStateVtable, 79, &sMenuMainNullState);
    ++nextState;
    sMenuMainTextFade.setup(s32(nextState), "::StateID_TextFade", &sMenuMainStateVtable, 82, &sMenuMainNullState);
    ++nextState;
    sMenuMainWindowOpening.setup(s32(nextState), "::StateID_BagItemIn", &sMenuMainStateVtable, 85, &sMenuMainNullState);
    ++nextState;
    sMenuMainWindowClosing.setup(s32(nextState), "::StateID_BagItemOut", &sMenuMainStateVtable, 88, &sMenuMainNullState);
    sMenuMainStateCounter = nextState;
}

struct hdMenuMainChildResource_l {
    u8 _00[8];
    hdScreenString mClassName;
    u8 _10[4];
    hdScreenString mPaneName;
};
WWHD_OFFSET(hdMenuMainChildResource_l, mClassName, 8);
WWHD_OFFSET(hdMenuMainChildResource_l, mPaneName, 0x14);
static bool menuMainResourceMatches(hdScreenString* input, hdScreenString* expected) {
    input->assureTermination();
    input->assureTermination();
    const char* cachedText = input->mText.get();
    expected->assureTermination();
    if (cachedText == expected->mText.get()) return true;
    const u32 inputText = gabi::ea(input->mText.get());
    const u32 expectedText = gabi::ea(expected->mText.get());
    for (u32 offset = 0; offset < 0x40001; ++offset) {
        const u8 first = *menuMainView<be<u8>>(inputText + offset);
        const u8 second = *menuMainView<be<u8>>(expectedText + offset);
        if (first != second) return false;
        if (first == 0) return true;
    }
    return false;
}
static hdScreenString sMenuMainItemIconClass;
static hdMenuItem_c* constructMenuMainItemIcon(hdMenuItem_c* child);
static u32 loadMenuMainItemIcon(hdMenuItem_c* child, u32 first, u32 second, hdPartsDesc_c* resource);
static hdScreenString sMenuMainCollectIconClass;
static hdParts_c* constructMenuMainCollectIcon(hdParts_c* child);
static u32 loadMenuMainCollectIcon(hdParts_c* child, u32 first, u32 second, hdPartsDesc_c* resource);
static hdScreenString sMenuMainCollectIconMaskClass;
static hdParts_c* constructMenuMainCollectIconMask(hdParts_c* child);
static u32 loadMenuMainCollectIconMask(hdParts_c* child, u32 first, u32 second, hdPartsDesc_c* resource);
static hdScreenString sMenuMainMapIconClass;
static hdBtnMapIconNumber_c* constructMenuMainMapIcon(hdBtnMapIconNumber_c* child);
static u32 loadMenuMainMapIcon(hdBtnMapIconNumber_c* child, u32 first, u32 second, hdPartsDesc_c* resource);
static hdScreenString sMenuMainMenuHeartClass;
static hdParts_c* constructMenuMainMenuHeart(hdParts_c* child);
static u32 loadMenuMainMenuHeart(hdParts_c* child, u32 first, u32 second, hdPartsDesc_c* resource);
static hdScreenString sMenuMainCollectionClass;
static hdCollection_c* constructMenuMainCollection(hdCollection_c* child);
static u32 loadMenuMainCollection(hdCollection_c* child, u32 first, u32 second, hdPartsDesc_c* resource);
static hdScreenString sMenuMainIconSlideAreaClass;
static hdParts_c* constructMenuMainIconSlideArea(hdParts_c* child);
static u32 loadMenuMainIconSlideArea(hdParts_c* child, u32 first, u32 second, hdPartsDesc_c* resource);
static u32 createMenuMainBaseChild(hdParts_c* self, u32 index, hdPartsDesc_c* resource);

/* 026966A8 */
u32 hdMenuMain_c::createMenuChild(hdPartsDesc_c* resource) {
    hdMenuMainChildResource_l* names = menuMainView<hdMenuMainChildResource_l>(resource);
    hdScreenString* paneName = menuMainView<hdScreenString>(gabi::ea(names) + offsetof(hdMenuMainChildResource_l, mPaneName));
    hdScreenString* className = menuMainView<hdScreenString>(gabi::ea(names) + offsetof(hdMenuMainChildResource_l, mClassName));
    hdScreenHeap* heap = sMenuMainUiSystem->mHeap.get();
    for (u32 index = 0; index < 0x2E; ++index) {
        if (!menuMainResourceMatches(paneName, &sMenuMainWindowPaneNames.mNames[index])) continue;
        u32 kind;
        if (menuMainResourceMatches(className, &sMenuMainItemIconClass)) kind = 0;
        else if (menuMainResourceMatches(className, &sMenuMainCollectIconClass)) kind = 1;
        else if (menuMainResourceMatches(className, &sMenuMainCollectIconMaskClass)) kind = 2;
        else if (menuMainResourceMatches(className, &sMenuMainMapIconClass)) kind = 3;
        else if (menuMainResourceMatches(className, &sMenuMainMenuHeartClass)) kind = 4;
        else if (menuMainResourceMatches(className, &sMenuMainCollectionClass)) kind = 5;
        else if (menuMainResourceMatches(className, &sMenuMainIconSlideAreaClass)) kind = 6;
        else return createMenuMainBaseChild(this, index, resource);
        gptr<hdParts_c>* slot = mChildren.safeSlot(index);
        hdParts_c* child;
        switch (kind) {
        case 0:
            child = menuMainView<hdParts_c>(allocateMenuMainLayout(sizeof(hdMenuItem_c), heap, 4));
            if (child != NULL) child = menuMainView<hdParts_c>(constructMenuMainItemIcon(menuMainView<hdMenuItem_c>(child)));
            break;
        case 1:
            child = menuMainView<hdParts_c>(allocateMenuMainLayout(0x8C, heap, 4));
            if (child != NULL) child = constructMenuMainCollectIcon(child);
            break;
        case 2:
            child = menuMainView<hdParts_c>(allocateMenuMainLayout(0x88, heap, 4));
            if (child != NULL) child = constructMenuMainCollectIconMask(child);
            break;
        case 3:
            child = menuMainView<hdParts_c>(allocateMenuMainLayout(sizeof(hdBtnMapIconNumber_c), heap, 4));
            if (child != NULL) child = menuMainView<hdParts_c>(constructMenuMainMapIcon(menuMainView<hdBtnMapIconNumber_c>(child)));
            break;
        case 4:
            child = menuMainView<hdParts_c>(allocateMenuMainLayout(0x7C, heap, 4));
            if (child != NULL) child = constructMenuMainMenuHeart(child);
            break;
        case 5:
            child = menuMainView<hdParts_c>(allocateMenuMainLayout(sizeof(hdCollection_c), heap, 4));
            if (child != NULL) child = menuMainView<hdParts_c>(constructMenuMainCollection(menuMainView<hdCollection_c>(child)));
            break;
        case 6:
            child = menuMainView<hdParts_c>(allocateMenuMainLayout(0x68, heap, 4));
            if (child != NULL) child = constructMenuMainIconSlideArea(child);
            break;
        default: child = NULL; break;
        }
        *slot = child;
        if (*mChildren.safeSlot(index) == NULL) return 0;
        child = *mChildren.safeSlot(index);
        switch (kind) {
        case 0: return loadMenuMainItemIcon(menuMainView<hdMenuItem_c>(child), 0, 0, resource);
        case 1: return loadMenuMainCollectIcon(child, 0, 0, resource);
        case 2: return loadMenuMainCollectIconMask(child, 0, 0, resource);
        case 3: return loadMenuMainMapIcon(menuMainView<hdBtnMapIconNumber_c>(child), 0, 0, resource);
        case 4: return loadMenuMainMenuHeart(child, 0, 0, resource);
        case 5: return loadMenuMainCollection(menuMainView<hdCollection_c>(child), 0, 0, resource);
        case 6: return loadMenuMainIconSlideArea(child, 0, 0, resource);
        default: return 0;
        }
    }
    return 1;
}

static void bindMenuMainReceiver(u32 context, u32 direction, u32 receiver);
static void setMenuMainReceiverLinks(hdParts_c* child, u32 link0, u32 link1, u32 link2, u32 link3, u32 link4, u32 link5, u32 link6, u32 link7);
static u32 setMenuMainReceiverLink(hdParts_c* child, u32 direction, u32 receiver);
static u32 menuMainReceiver(hdParts_c* child) {
    return menuMainView<hdButtonPart_c>(child)->mpReceiver.v;
}

/* 02695D18 */
void hdMenuMain_c::updateReceiverLinks() {
    if (menuMainHasState(this, &sMenuMainInAll)) return;
    if (menuMainHasState(this, &sMenuMainWindowSlid)) return;
    u32 count = mChildren.mNum;
    gptr<hdParts_c>* data = mChildren.mpData.get();
    hdParts_c* options = menuMainCachedChild(data, count, 0x25);
    if (mpContext)
        bindMenuMainReceiver(gabi::ea(mpContext.get()), 3, menuMainReceiver(menuMainCachedChild(data, count, 0x1D)));
    if (mContextFirst)
        bindMenuMainReceiver(mContextFirst, 3, menuMainReceiver(*options->mChildren.safeSlot(3)));
    if (mContextSecond)
        bindMenuMainReceiver(mContextSecond, 3, menuMainReceiver(*options->mChildren.safeSlot(2)));
    {
        count = mChildren.mNum;
        data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x28);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x1D));
        u32 link2 = menuMainReceiver(menuMainCachedChild(data, count, 0x1E));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x1F));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x1E));
        setMenuMainReceiverLinks(button, link0, 0, link2, link3, 0, 0, 0, link7);
    }
    {
        count = mChildren.mNum;
        data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x1D);
        u32 link0 = gabi::ea(mpContext.get());
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x1E));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x28));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, 0);
    }
    {
        count = mChildren.mNum;
        data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x1E);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x1D));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x20));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x28));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, 0);
    }
    {
        count = mChildren.mNum;
        data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x1F);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x28));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x23));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x28));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x20));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        count = mChildren.mNum;
        data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x20);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x1E));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x24));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x1F));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x21));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        count = mChildren.mNum;
        data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x21);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x1E));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x29));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x20));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x22));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        count = mChildren.mNum;
        data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x22);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x1E));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x29));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x21));
        u32 link7 = menuMainReceiver(*options->mChildren.safeSlot(3));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        count = mChildren.mNum;
        data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x23);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x1F));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x28));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x24));
        setMenuMainReceiverLinks(button, link0, 0, 0, 0, 0, 0, link6, link7);
    }
    {
        count = mChildren.mNum;
        data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x24);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x20));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x23));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x29));
        setMenuMainReceiverLinks(button, link0, 0, 0, 0, 0, 0, link6, link7);
    }
    {
        count = mChildren.mNum;
        data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x29);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x21));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x24));
        u32 link7 = menuMainReceiver(*options->mChildren.safeSlot(3));
        setMenuMainReceiverLinks(button, link0, 0, 0, 0, 0, 0, link6, link7);
    }
    {
        count = mChildren.mNum;
        data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x26);
        u32 link0 = mContextSecond;
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x27));
        u32 link6 = menuMainReceiver(*options->mChildren.safeSlot(2));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, 0);
    }
    {
        count = mChildren.mNum;
        data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x27);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x26));
        u32 link6 = menuMainReceiver(*options->mChildren.safeSlot(1));
        setMenuMainReceiverLinks(button, link0, 0, 0, 0, 0, 0, link6, 0);
    }
    setMenuMainReceiverLink(*options->mChildren.safeSlot(3), 0, mContextFirst);
    {
        hdParts_c* button = *options->mChildren.safeSlot(3);
        hdParts_c* neighbor = *mChildren.safeSlot(0x22);
        setMenuMainReceiverLink(button, 6, menuMainReceiver(neighbor));
    }
    {
        hdParts_c* button = *options->mChildren.safeSlot(1);
        hdParts_c* neighbor = *mChildren.safeSlot(0x27);
        setMenuMainReceiverLink(button, 7, menuMainReceiver(neighbor));
    }
    setMenuMainReceiverLink(*options->mChildren.safeSlot(2), 0, mContextSecond);
    {
        hdParts_c* button = *options->mChildren.safeSlot(2);
        hdParts_c* neighbor = *mChildren.safeSlot(0x26);
        setMenuMainReceiverLink(button, 7, menuMainReceiver(neighbor));
    }
}

/* 026949A4 */
void hdMenuMain_c::updateMainReceiverLinks() {
    if (menuMainHasState(this, &sMenuMainInAll)) return;
    if (menuMainHasState(this, &sMenuMainWindowReturned)) return;
    if (mpContext && mWindowMode == 3)
        bindMenuMainReceiver(gabi::ea(mpContext.get()), 3, menuMainReceiver(*mChildren.safeSlot(0x12)));
    if (mContextFirst && mWindowMode == 3)
        bindMenuMainReceiver(mContextFirst, 3, menuMainReceiver(*mChildren.safeSlot(0)));
    u32 secondContext = mContextSecond;
    if (secondContext) {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        u32 index = mWindowMode == 3 ? 0x2A : 0x14;
        bindMenuMainReceiver(secondContext, 3, menuMainReceiver(menuMainCachedChild(data, count, index)));
        secondContext = mContextSecond;
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x11);
        u32 link0 = secondContext;
        u32 link4 = menuMainReceiver(menuMainCachedChild(data, count, 0xF));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0xE));
        setMenuMainReceiverLinks(button, link0, 0, 0, 0, link4, 0, link6, 0);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x12);
        u32 link0 = gabi::ea(mpContext.get());
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x13));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x0));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, 0, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x13);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x12));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x3));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x0));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, 0, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x0);
        u32 link0 = mContextFirst;
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x4));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x13));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x2A));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x2A);
        u32 link0 = mContextSecond;
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x2B));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x0));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x8));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x8);
        u32 link0 = mContextSecond;
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x9));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x2A));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0xB));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0xB);
        u32 link0 = mContextSecond;
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0xC));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x8));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0xE));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0xE);
        u32 link0 = mContextSecond;
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0xF));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0xB));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x11));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x3);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x13));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x7));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x2));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, 0, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x2);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x13));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x6));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x3));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x4));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x4);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x0));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x5));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x2));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x2B));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x2B);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x2A));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x2C));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x4));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x9));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x9);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x8));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0xA));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x2B));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0xC));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0xC);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0xB));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0xD));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x9));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0xF));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0xF);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0xE));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x10));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0xC));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x11));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x7);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x3));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x6));
        setMenuMainReceiverLinks(button, link0, 0, 0, 0, 0, 0, 0, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x6);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x2));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x7));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x5));
        setMenuMainReceiverLinks(button, link0, 0, 0, 0, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x5);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x4));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x6));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x2C));
        setMenuMainReceiverLinks(button, link0, 0, 0, 0, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x2C);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x2B));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x5));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0xA));
        setMenuMainReceiverLinks(button, link0, 0, 0, 0, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0xA);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x9));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x2C));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0xD));
        setMenuMainReceiverLinks(button, link0, 0, 0, 0, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0xD);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0xC));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0xA));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x10));
        setMenuMainReceiverLinks(button, link0, 0, 0, 0, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x10);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0xF));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0xD));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x11));
        setMenuMainReceiverLinks(button, link0, 0, 0, 0, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x14);
        u32 link0 = mContextSecond;
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x15));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x17));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, 0, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x15);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x14));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x16));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x18));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, 0, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x16);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x15));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x19));
        setMenuMainReceiverLinks(button, link0, 0, 0, 0, 0, 0, 0, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x17);
        u32 link0 = mContextSecond;
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x18));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x14));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x1A));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x18);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x17));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x19));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x15));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x1B));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x19);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x18));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x16));
        u32 link7 = menuMainReceiver(menuMainCachedChild(data, count, 0x1C));
        setMenuMainReceiverLinks(button, link0, 0, 0, 0, 0, 0, link6, link7);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x1A);
        u32 link0 = mContextSecond;
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x1B));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x17));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, 0);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x1B);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x1A));
        u32 link3 = menuMainReceiver(menuMainCachedChild(data, count, 0x1C));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x18));
        setMenuMainReceiverLinks(button, link0, 0, 0, link3, 0, 0, link6, 0);
    }
    {
        u32 count = mChildren.mNum;
        gptr<hdParts_c>* data = mChildren.mpData.get();
        hdParts_c* button = menuMainCachedChild(data, count, 0x1C);
        u32 link0 = menuMainReceiver(menuMainCachedChild(data, count, 0x1B));
        u32 link6 = menuMainReceiver(menuMainCachedChild(data, count, 0x19));
        setMenuMainReceiverLinks(button, link0, 0, 0, 0, 0, 0, link6, 0);
    }
}

/* WWHD glue
ext 02001BE4 void bindMenuMainReceiver(u32 context, u32 direction, u32 receiver)
ext 026F5C08 void setMenuMainReceiverLinks(hdParts_c* child, u32 link0, u32 link1, u32 link2, u32 link3, u32 link4, u32 link5, u32 link6, u32 link7)
ext 026F5C00 u32 setMenuMainReceiverLink(hdParts_c* child, u32 direction, u32 receiver)
data sMenuMainItemIconClass 1048FEDC
ext 02631D0C hdMenuItem_c* constructMenuMainItemIcon(hdMenuItem_c* child)
ext 0263211C u32 loadMenuMainItemIcon(hdMenuItem_c* child, u32 first, u32 second, hdPartsDesc_c* resource)
data sMenuMainCollectIconClass 1048F4D8
ext 0262A088 hdParts_c* constructMenuMainCollectIcon(hdParts_c* child)
ext 0262A130 u32 loadMenuMainCollectIcon(hdParts_c* child, u32 first, u32 second, hdPartsDesc_c* resource)
data sMenuMainCollectIconMaskClass 1048F69C
ext 0262BA50 hdParts_c* constructMenuMainCollectIconMask(hdParts_c* child)
ext 0262BB30 u32 loadMenuMainCollectIconMask(hdParts_c* child, u32 first, u32 second, hdPartsDesc_c* resource)
data sMenuMainMapIconClass 10490528
ext 0263AEB0 hdBtnMapIconNumber_c* constructMenuMainMapIcon(hdBtnMapIconNumber_c* child)
ext 0263AF48 u32 loadMenuMainMapIcon(hdBtnMapIconNumber_c* child, u32 first, u32 second, hdPartsDesc_c* resource)
data sMenuMainMenuHeartClass 104907D4
ext 0263DDB0 hdParts_c* constructMenuMainMenuHeart(hdParts_c* child)
ext 0263DE24 u32 loadMenuMainMenuHeart(hdParts_c* child, u32 first, u32 second, hdPartsDesc_c* resource)
data sMenuMainCollectionClass 104910F0
ext 02647250 hdCollection_c* constructMenuMainCollection(hdCollection_c* child)
ext 026474A8 u32 loadMenuMainCollection(hdCollection_c* child, u32 first, u32 second, hdPartsDesc_c* resource)
data sMenuMainIconSlideAreaClass 10494B90
ext 0266F6D4 hdParts_c* constructMenuMainIconSlideArea(hdParts_c* child)
ext 0266FAF8 u32 loadMenuMainIconSlideArea(hdParts_c* child, u32 first, u32 second, hdPartsDesc_c* resource)
ext 026F92C0 u32 createMenuMainBaseChild(hdParts_c* self, u32 index, hdPartsDesc_c* resource)
data sMenuMainClassNames 10496A90
data sMenuMainRandomRegistration 101F6398
data sMenuMainSignedRegistration 101F63A4
data sMenuMainUnsignedRegistration 101F63B0
data sMenuMainRandomState 104967E8
data sMenuMainMinAngle 104967DC
data sMenuMainMaxAngle 104967E0
data sMenuMainSignedFlag 104967E4
data sMenuMainUnsignedFlag 104967E5
data sMenuMainStateCounterInitialized 101FD8E4
data sMenuMainStateCounter 101FDD50
data sMenuMainStateVtable 100FA320
data sMenuMainNullState 101FF32C
ext 028F026C void registerMenuMainStatic(hdMenuMainStaticRecord_l* record)
ext 028ED6F8 void constructMenuMainSignedFlag(be<u8>* flag)
ext 028EAB2C void constructMenuMainUnsignedFlag(be<u8>* flag)
data sMenuMainDrag 10496EC0
data sMenuMainInArrow 10496F80
data sMenuMainOutArrow 10496FB0
data sMenuMainTextFade 10497040
str 100FA654 "InArrow" in 0269735C
str 100FA65C "InAll" in 0269735C
str 100FA664 "OutAll" in 0269735C
str 100FA66C "L_MenuArrowR_00" in 0269735C
str 100FA67C "L_MenuOption_00" in 0269735C
str 100FA68C "L_MenuOption_01" in 0269735C
str 100FA69C "L_Collection_00" in 0269735C
str 100FA6AC "L_BtnMapIcon_00" in 0269735C
str 100FA6BC "L_MenuArrowL_00" in 0269735C
str 100FA6CC "L_BtnCollectIconMask_00" in 0269735C
str 100FA6E4 "BtnMenuArrow_00" in 0269735C
str 100FA6F4 "BtnMenuHeart_00" in 0269735C
str 100FA704 "::StateID_Slide" in 0269735C
str 100FA714 "::StateID_InAll" in 0269735C
str 100FA724 "::StateID_ItemCaptionIn" in 0269735C
str 100FA73C "G_ArrowL_00" in 0269735C
str 100FA748 "G_ArrowR_00" in 0269735C
str 100FA754 "L_BtnCollectIcon_00" in 0269735C
str 100FA768 "L_BtnCollectIcon_01" in 0269735C
str 100FA77C "L_BtnCollectIcon_02" in 0269735C
str 100FA790 "L_BtnCollectIcon_03" in 0269735C
str 100FA7A4 "L_BtnCollectIcon_04" in 0269735C
str 100FA7B8 "L_BtnCollectIcon_05" in 0269735C
str 100FA7CC "MenuItem_00" in 0269735C
str 100FA7D8 "BtnPearl_00" in 0269735C
str 100FA7E4 "B_SlideR_00" in 0269735C
str 100FA7F0 "B_SlideL_00" in 0269735C
str 100FA7FC "T_TitleL_00" in 0269735C
str 100FA808 "T_TitleR_00" in 0269735C
str 100FA814 "T_TitleR_01" in 0269735C
str 100FA820 "::StateID_EquipItem" in 0269735C
str 100FA834 "::StateID_OutArrowL" in 0269735C
str 100FA848 "::StateID_OutArrowR" in 0269735C
str 100FA85C "::StateID_BagItemIn" in 0269735C
str 100FA870 "G_TextFade_00" in 0269735C
str 100FA880 "ItemCaptionIn" in 0269735C
str 100FA890 "BagItemIn" in 0269735C
str 100FA89C "L_BtnItemIconFront_00" in 0269735C
str 100FA8B4 "L_MenuSave_00" in 0269735C
str 100FA8C4 "L_MenuSave_01" in 0269735C
str 100FA8D4 "L_BtnMenuHeart_00" in 0269735C
str 100FA8E8 "BtnCollectIcon_00" in 0269735C
str 100FA8FC "Collection_00" in 0269735C
str 100FA90C "BtnMapIcon_00" in 0269735C
str 100FA91C "BtnCollectIconMask_00" in 0269735C
str 100FA934 "N_BaseArea_00" in 0269735C
str 100FA944 "L_BtnBack_00-T_Btn_00" in 0269735C
str 100FA95C "::StateID_CollectItem" in 0269735C
str 100FA974 "::StateID_ItemCaption" in 0269735C
str 100FA98C "::StateID_BagItem" in 0269735C
str 100FA9A0 "::StateID_InArrow" in 0269735C
str 100FA9B4 "G_All_00" in 0269735C
str 100FA9C0 "G_ItemCaption_00" in 0269735C
str 100FA9D4 "G_BagItemGame_00" in 0269735C
str 100FA9E8 "G_BagItemFeed_00" in 0269735C
str 100FA9FC "G_BagItemDelivery_00" in 0269735C
str 100FAA14 "OutArrow" in 0269735C
str 100FAA20 "ItemCaptionOut" in 0269735C
str 100FAA30 "TextFade" in 0269735C
str 100FAA3C "BagItemOut" in 0269735C
str 100FAA48 "L_BtnItemIcon_00" in 0269735C
str 100FAA5C "L_BtnItemIcon_06" in 0269735C
str 100FAA70 "L_BtnItemIcon_05" in 0269735C
str 100FAA84 "L_BtnItemIcon_07" in 0269735C
str 100FAA98 "L_BtnItemIcon_14" in 0269735C
str 100FAAAC "L_BtnItemIcon_13" in 0269735C
str 100FAAC0 "L_BtnItemIcon_12" in 0269735C
str 100FAAD4 "L_BtnItemIcon_02" in 0269735C
str 100FAAE8 "L_BtnItemIcon_09" in 0269735C
str 100FAAFC "L_BtnItemIcon_16" in 0269735C
str 100FAB10 "L_BtnItemIcon_03" in 0269735C
str 100FAB24 "L_BtnItemIcon_10" in 0269735C
str 100FAB38 "L_BtnItemIcon_17" in 0269735C
str 100FAB4C "L_BtnItemIcon_04" in 0269735C
str 100FAB60 "L_BtnItemIcon_11" in 0269735C
str 100FAB74 "L_BtnItemIcon_18" in 0269735C
str 100FAB88 "L_BtnItemIcon_19" in 0269735C
str 100FAB9C "L_BtnItemIcon_20" in 0269735C
str 100FABB0 "L_BtnItemIcon_21" in 0269735C
str 100FABC4 "L_BtnItemIcon_22" in 0269735C
str 100FABD8 "L_BtnItemIcon_23" in 0269735C
str 100FABEC "L_BtnItemIcon_24" in 0269735C
str 100FAC00 "L_BtnItemIcon_25" in 0269735C
str 100FAC14 "L_BtnItemIcon_26" in 0269735C
str 100FAC28 "L_BtnBack_00" in 0269735C
str 100FAC38 "L_BtnItemIcon_01" in 0269735C
str 100FAC4C "L_BtnItemIcon_08" in 0269735C
str 100FAC60 "L_BtnItemIcon_15" in 0269735C
str 100FAC74 "L_IconSlideArea_00" in 0269735C
str 100FAC88 "BtnItemIcon_00" in 0269735C
str 100FAC98 "BtnMenuSaveOption_00" in 0269735C
str 100FACB0 "BtnBack_00" in 0269735C
str 100FACBC "IconSlideArea_00" in 0269735C
str 100FACD0 "BtnTriforce_00" in 0269735C
str 100FACE0 "B_Slide_00" in 0269735C
str 100FACEC "N_All_00" in 0269735C
str 100FACF8 "N_All_01" in 0269735C
str 100FAD04 "N_BagBaseArea_00" in 0269735C
str 100FAD18 "N_BagBaseArea_01" in 0269735C
str 100FAD2C "N_BagBaseArea_02" in 0269735C
str 100FAD40 "T_TextR_00" in 0269735C
str 100FAD4C "T_TextL_00" in 0269735C
str 100FAD58 "L_MenuSave_00-T_Btn_00" in 0269735C
str 100FAD70 "L_MenuOption_00-T_Btn_00" in 0269735C
str 100FAD8C "T_TitleR_01_01" in 0269735C
str 100FAD9C "T_TitleR_01_00" in 0269735C
str 100FADAC "T_TitleR_01_02" in 0269735C
str 100FADBC "T_TitleR_01_04" in 0269735C
str 100FADCC "T_TitleR_01_03" in 0269735C
str 100FADDC "T_TitleR_01_05" in 0269735C
str 100FADEC "::StateID_InArrowL" in 0269735C
str 100FAE00 "::StateID_InArrowR" in 0269735C
str 100FAE14 "::StateID_EquipToCollect" in 0269735C
str 100FAE30 "::StateID_CollectToEquip" in 0269735C
str 100FAE4C "::StateID_TriforceCompleteDemo" in 0269735C
str 100FAE6C "::StateID_Drag" in 0269735C
str 100FAE7C "::StateID_OutAll" in 0269735C
str 100FAE90 "::StateID_OutArrow" in 0269735C
str 100FAEA4 "::StateID_ItemCaptionOut" in 0269735C
str 100FAEC0 "::StateID_TextFade" in 0269735C
str 100FAED4 "::StateID_BagItemOut" in 0269735C
ext 0262A918 u32 getMenuMainCategoryPrimaryHint(hdParts_c* child)
ext 0262A938 u32 getMenuMainCategorySecondaryHint(hdParts_c* child)
ext 02641E1C u32 getMenuMainOptionPrimaryHint(hdParts_c* child)
ext 02641E3C u32 getMenuMainOptionSecondaryHint(hdParts_c* child)
ext 02645064 u32 getMenuMainFourthPrimaryHint(hdParts_c* child)
ext 02645094 u32 getMenuMainFourthSecondaryHint(hdParts_c* child)
ext 0264761C u32 findMenuMainOptionIndex(hdParts_c* owner, u32 key)
ext 02005674 void setMenuMainHintAnimation(hdAnimSet* set, u32 entry, u32 slot)
ext 0268FFC4 void setMenuMainTabResourceHint(hdMenuMain_c* self, u32 index, hdUiSafeString* unusedHint)
data sMenuMainPaneNames 10496850
data sMenuMainTabNames 10496880
data sMenuMainWindowPaneNames 10496920
data sMenuMainTabFonts 100FA2AC
ext 02634088 u32 bindMenuMainPanes(hdPartsLayout_c* layout, hdMenuMainPaneArray_c* panes, hdScreenString* names)
ext 02634420 u32 bindMenuMainTabs(hdPartsLayout_c* layout, hdPartsChildArray_c* tabs, hdScreenString* names)
ext 02670D9C u32 bindMenuMainWindowPanes(hdPartsLayout_c* layout, hdMenuMainPaneArray_c* panes, hdScreenString* names)
ext 02704AFC void setMenuMainTabFont(hdParts_c* tab, u32 font)
ext 0268EE9C void clearMenuMainTabText(hdMenuMain_c* self, u32 index)
ext 0268F970 u32 hitTestMenuMainWindowChildren(hdMenuMain_c* self, hdMenuPaneVector* point)
ext 0268F92C void closeMenuMainOptionWindow(hdMenuMain_c* self)
data sMenuMainEmptyWideText 100FA588
data sMenuMainWideStringVtable 100FA168
data sMenuMainBufferedWideStringVtable 100FA180
data sMenuMainFixedWideStringVtable 100FA198
ext 02698564 void normalizeMenuMainWideString(hdMenuMainWideString_c* text)
ext C0009988 void copyMenuMainWideText(void* destination, void* source, u32 size, u32 option)
ext 027046D4 void setMenuMainTabText(hdTextPane_c* child, PaneString_l* text, PaneWork_l* work)
data sMenuMainReceiverNames 10496838
data sMenuMainReceiverTypeInitialized 101FD938
data sMenuMainPaneTypeInitialized 101FD88C
data sMenuMainObjectTypeInitialized 101FD888
data sMenuMainReceiverType 101FD93C
data sMenuMainPaneType 101FD940
data sMenuMainObjectType 101FD8FC
ext 02616ED4 hdButtonReceiver_l* constructMenuMainReceiver(hdButtonReceiver_l* receiver, hdPartsPane_c* pane, u32 option)
ext 02644F0C void refreshMenuMainFourthOption(hdParts_c* child)
ext 02632D74 void setupMenuMainItemLabel(hdParts_c* child)
ext 026151A8 Pair32 getMenuMainDragPosition(hdButtonEventManager_l* input)
ext 02615260 Pair32 getMenuMainSlideDelta(hdButtonEventManager_l* input)
data sMenuMainSliding 10496EF0
ext 02645048 u32 isMenuMainFourthOptionEnabled(hdParts_c* child)
ext 026924EC void executeMenuMainSaveChildren(hdMenuMain_c* self)
ext 026925EC void enterMenuMainSaveChildren(hdMenuMain_c* self)
data sMenuMainInputManager 101F5088
data sMenuMainInvalidPointer 1049FFA0
ext 026F6340 u32 isMenuMainAlternateConfirmEvent(hdButtonEvent_l* event)
ext 02691844 u32 hitTestMenuMainCursorPane(hdMenuMain_c* self, hdMenuPaneVector* point)
ext 026917C8 void selectMenuMainCurrentChild(hdMenuMain_c* self)
ext 026F5CDC void resetMenuMainButton(hdParts_c* child)
data sMenuMainUiSystem 1018C404
data sMenuMainAnimationGroups 100FA270
data sMenuMainAnimationNames 104968A8
data sMenuMainAnimationGroupNames 104967F8
ext 0273B050 void* allocateMenuMainLayout(u32 size, hdScreenHeap* heap, u32 alignment)
ext 02004E04 void bindMenuMainAnimation(hdAnimSet* animationSet, u32 index, hdAnimSetString* name, hdAnimSetString* group)
ext 02632E64 u32 isMenuMainWindowRequestEvent(hdButtonEvent_l* event)
ext 0268EC88 void openMenuMainWindow(hdMenuMain_c* self, u32 mode)
ext 02690890 void updateMenuMainAlternateButtonFocus(hdMenuMain_c* self)
ext 02690974 void refreshMenuMainTabChildren(hdMenuMain_c* self)
ext 02690A3C void executeMenuMainTabChildren(hdMenuMain_c* self)
str 100FA5D4 "SE_MENU_TAB_SWITCH" in 02690AE4
ext 026915B4 void enterMenuMainTransitionChildren(hdMenuMain_c* self)
data sMenuMainEmptySound 104A0CD8
ext 026F2540 void setMenuMainChildSound(hdParts_c* child, hdScreenString* sound, u32 slot)
str 100FA624 "SE_MENU_OPTION_IN" in 026934B8
str 100FA638 "SE_MENU_TAB_SWITCH" in 026934B8
data sMenuMainWindowLabels 10496C68
ext 026F5E80 void bindMenuMainChildLabel(hdParts_c* child, hdScreenString* paneName, hdUiSafeString* text)
str 100FA618 "T_Btn_00" in 02693328
data sMenuMainArchiveNames 10496C00
data sMenuMainLayoutName 104970D0
ext 026F90D8 void attachMenuMainArchives(hdParts_c* self, hdUiSafeString* names, u32 count)
ext 02692C80 u32 createMenuMainPaneArrays(hdMenuMain_c* self)
ext 02692F10 void createMenuMainInputReceivers(hdMenuMain_c* self)
ext 02693170 void setupMenuMainItemNames(hdMenuMain_c* self)
ext 02693328 void setupMenuMainWindowNames(hdMenuMain_c* self)
ext 026934B8 void setupMenuMainButtonSounds(hdMenuMain_c* self)
data sMenuMainVtable 100FAEF0
ext 0273AD10 void* allocateMenuMainObject(u32 size)
ext 026F89E8 void constructMenuMainParts(hdParts_c* self)
ext 02670478 u32 hitTestMenuMainCollect(hdParts_c* child, hdMenuPaneVector* point)
ext 026F61DC u32 isMenuMainTabEvent(u32 type)
ext 0268ECDC void updateMenuMainMainInput(hdMenuMain_c* self)
ext 0268F110 void updateMenuMainTabConfirmation(hdMenuMain_c* self)
ext 0268F4F8 void refreshMenuMainMainChildren(hdMenuMain_c* self)
ext 0268F624 void executeMenuMainMainChildren(hdMenuMain_c* self)
str 100FA5A4 "SE_MENU_TAB_SWITCH" in 0268F790
ext 026F6288 u32 isMenuMainConfirmEvent(hdButtonEvent_l* event)
ext 02632B4C u32 canMenuMainItemOpen(hdParts_c* child)
ext 0268F060 void openMenuMainSelectedChild(hdMenuMain_c* self, u32 index)
ext 02615398 u32 getMenuMainInputActionKey(hdButtonEventManager_l* input)
ext 0266FE68 void refreshMenuMainCollectChildren(hdParts_c* child)
ext 0267021C Pair32 getMenuMainCollectItemInfo(hdParts_c* child, u32 index)
ext 02632BBC void setMenuMainItemInfo(hdParts_c* child, hdMenuMainCollectItemInfo_c* info)
ext 02693D34 u32 isMenuMainCancelEvent(hdMenuMain_c* self)
ext 0268F420 u32 getMenuMainSelectedItemValue(hdMenuMain_c* self, u32 key)
ext 0266FF54 void enableMenuMainCollectReceiver(hdParts_c* child, u32 index, u32 enabled)
ext 0266FFF8 void resetMenuMainCollectReceiver(hdParts_c* child, u32 index)
ext 0267007C void activateMenuMainCollectReceiver(hdParts_c* child, u32 index)
ext 026700F8 void deactivateMenuMainCollectReceiver(hdParts_c* child, u32 index)
ext 02693E50 u32 isMenuMainCollectActionUnavailable(hdMenuMain_c* self)
ext 0268F2FC u32 isMenuMainCollectInputBlocked(hdMenuMain_c* self)
ext 0268F36C u32 isMenuMainAnyItemActivated(hdMenuMain_c* self)
data sMenuMainHeapManager 101F8B4C
data sMenuMainWindowCounts 100FA2C0
data sMenuMainWindowItems 101F63BC
ext 0266FDC8 void initializeMenuMainCollect(hdParts_c* child)
ext 02693AC8 void initializeMenuMainTransitionItems(hdMenuMain_c* self)
ext 02693BC8 void initializeMenuMainWindowItems(hdMenuMain_c* self)
ext 02693EC0 void initializeMenuMainSpecialItems(hdMenuMain_c* self)
ext 02691008 void startMenuMainWindowChildTransitions(hdMenuMain_c* self)
data sMenuMainOutAll 10496F50
ext 026941BC u32 isMenuMainOptionWindowActive(hdMenuMain_c* self)
ext 02005708 void finishMenuMainAnimation(hdAnimSet* set, u32 entry, u32 slot)
data sMenuMainWindowIndices 100FA2CC
ext 02632724 void initializeMenuMainItem(hdParts_c* child, u32 key, u32 value)
ext 02632730 void resetMenuMainItemSelection(hdParts_c* child)
ext 0263286C void resetMenuMainItemAnimation(hdParts_c* child)
ext 026329E0 void resetMenuMainItemFocus(hdParts_c* child)
ext 02632A38 void resetMenuMainItemAction(hdParts_c* child)
ext 0264761C u32 getMenuMainOptionItemKind(hdParts_c* child, u32 key)
ext 02690854 void updateMenuMainSpecialButton(hdMenuMain_c* self, u32 key)
ext 026905EC void updateMenuMainAlternateRequest(hdMenuMain_c* self, u32 key)
ext 02670130 u32 getMenuMainCollectItemValue(hdParts_c* child, u32 key)
ext 026476AC void startMenuMainOptionTransition(hdParts_c* child)
ext 0263DE3C void startMenuMainItemTransition(hdParts_c* child)
ext 0263B230 void startMenuMainBagTransition(hdParts_c* child)
ext 0262A7BC void startMenuMainTabTransition(hdParts_c* child)
ext 0262BEC8 void startMenuMainSaveTransition(hdParts_c* child)
data sMenuMainResourceNameVtable 100FA150
ext 027045B0 void setMenuMainChildResourceName(hdParts_c* child, hdScreenString* name)
data sMenuMainSelectionEvents 100FA4AC
ext 026F620C u32 isMenuMainSelectionEvent(u32 type)
ext 02697324 u32 isMenuMainBagItem(hdMenuMain_c* self, u32 item)
ext 02001338 void constructMenuMainEvent(hdButtonEvent_l* event, u32 type, u32 receiver, u32 button)
ext 02001FF4 void queueMenuMainEvent(void* queue, hdButtonEvent_l* event)
ext 02001490 void destroyMenuMainEvent(hdButtonEvent_l* event)
data sMenuMainInputMode 101F8378
ext 02615188 u32 isMenuMainInputFocusChanged(hdButtonEventManager_l* input)
ext 02615140 hdButtonReceiver_l* getMenuMainInputReceiver(hdButtonEventManager_l* input)
ext 026F61CC u32 isMenuMainFocusEvent(u32 type)
ext 0268EA20 void updateMenuMainPlayRequest(hdMenuMain_c* self, u32 key)
data sMenuMainTransitionItems 100FA368
ext 02632D94 u32 getMenuMainTransitionItemValue(u32 key)
ext 026302A0 void setMenuMainTransitionItem(hdParts_c* child, u32 key, u32 value)
ext 026327E8 void refreshMenuMainTransitionItem(hdParts_c* child)
ext 02645058 u32 isMenuMainOptionChildFinished(hdParts_c* child)
ext 02632A40 u32 isMenuMainItemActionBlocked(hdParts_c* child)
data sMenuMainWindowReturned 10496D70
ext 0268EC54 void setMenuMainSelectionMode(hdMenuMain_c* self, u32 mode)
data sMenuMainBaseHolderVtable 100FB1C8
data sMenuMainBaseSecondaryVtable 100FB1D8
data sMenuMainInAll 10496F20
ext 026F88F0 void destroyMenuMainParts(hdParts_c* self, s32 flags)
ext 0273AF40 void deleteMenuMainParts(void* self)
data sMenuMainWindowClosing 104970A0
str 100FA5B8 "SE_MENU_MINI_WINDOW_CLOSE" in 0268F92C
ext 02030B38 void playMenuMainSound(const char* sound)
ext 02691008 void executeMenuMainWindowTransition(hdMenuMain_c* self)
ext 026937C8 void finishMenuMainChildren(hdMenuMain_c* self)
ext 026938F4 void finishMenuMainSubscreens(hdMenuMain_c* self)
ext 026F8B00 void deleteMenuMainChildren(hdParts_c* self)
ext 026F91F8 void detachMenuMainArchives(hdParts_c* self)
ext 02697120 void executeMenuMainChildren(hdMenuMain_c* self)
ext 026F8CD0 void executeMenuMainParts(hdParts_c* self)
data sMenuMainButtonIndices 100FA400
data sMenuMainSelectionNormal 10496DA0
data sMenuMainSelectionAlternate 10496DD0
data sMenuMainInputOwner 101F4FF0
ext 026F5B74 u32 getMenuMainButtonKey(hdParts_c* child)
ext 0268E910 u32 lookupMenuMainButtonIndex(hdMenuMain_c* self, u32 key)
ext 0268E99C u32 lookupMenuMainChildIndex(hdMenuMain_c* self, u32 key)
ext 027045F0 void setMenuMainChildEnabled(hdParts_c* child, u32 enabled)
ext 02615148 void updateMenuMainInput(void* input, u32 first, u32 second)
ext 02701B38 void resetMenuMainChild(hdParts_c* child)
ext 02615154 void selectMenuMainInput(void* input, void* selection)
ext 026701C0 void updateMenuMainCollectIcon(hdParts_c* child)
data sMenuMainWindowOpening 10497070
data sMenuMainPrimaryReady 10496D10
data sMenuMainPendingReady 10496E90
data sMenuMainSecondaryReady 10496CB0
data sMenuMainPrimaryHidden 10496E30
data sMenuMainSecondaryHidden 10496E00
str 100FA55C "SE_MENU_DECIDE" in 0268EC88
str 100FA56C "SE_MENU_MINI_WINDOW_OPEN" in 0268EC88
ext 026703D8 u32 isMenuMainCollectBlocked(hdParts_c* child)
ext 0268F248 u32 isMenuMainSelectionBlocked(hdMenuMain_c* self)
ext 02690724 void refreshMenuMainWindow(hdMenuMain_c* self)
ext 0268EAE0 void updateMenuMainButtonFocus(hdMenuMain_c* self)
ext 0268EB8C void updateMenuMainButtonSelection(hdMenuMain_c* self)
ext 0268FABC void updateMenuMainWindowInput(hdMenuMain_c* self)
ext 0268F110 void updateMenuMainTabInput(hdMenuMain_c* self)
ext 0268FCE4 void updateMenuMainWindowChildren(hdMenuMain_c* self)
ext 0268FE10 void updateMenuMainWindowFlags(hdMenuMain_c* self)
data sMenuMainBlank 1049DD60
data sMenuMainOptionReady 10496E60
data sMenuMainWindowReady 10496CE0
data sMenuMainSelectedChildClosing 10497010
str 100FA5F4 "SE_MENU_MINI_WINDOW_CLOSE" in 026917C8
ext 02690020 void updateMenuMainItemSelection(hdMenuMain_c* self, u32 mode)
ext 026450C4 void resetMenuMainOptionChild(hdParts_c* child)
ext 02670338 u32 isMenuMainCollectActionBlocked(hdParts_c* child)
ext 02693D9C u32 isMenuMainActionBlocked(hdMenuMain_c* self)
ext 026910E4 void executeMenuMainWindowTransitionEntry(hdMenuMain_c* self)
ext 026910E8 void startMenuMainWindowTransition(hdMenuMain_c* self)
ext 02691514 void executeMenuMainTransitionChildren(hdMenuMain_c* self)
ext 026915B4 void updateMenuMainTransitionChildren(hdMenuMain_c* self)
ext 0287F05C u32 hitTestMenuMainPane(hdMenuPane_l* pane, hdMenuPaneVector* point)
ext 0270477C void linkMenuMainTab(hdParts_c* child, void* link)
ext 02691890 void updateMenuMainTabLinks(hdMenuMain_c* self)
ext 026702B4 u32 isMenuMainCollectWindowBlocked(hdParts_c* child)
ext 02694624 u32 isMenuMainWindowBlocked(hdMenuMain_c* self)
data sMenuMainWindowSlid 10496D40
data sMenuMainChildOpening 10496FE0
str 100FA58C "SE_MENU_DECIDE_OPEN_S" in 0268F060
ext 0268F210 u32 isMenuMainMainTabItem(hdMenuMain_c* self, u32 item)
ext 02632A6C u32 isMenuMainItemSelected(hdParts_c* child)
ext 02632ADC u32 isMenuMainItemActivated(hdParts_c* child)
ext 0264759C void refreshMenuMainOptionChild(hdParts_c* child)
ext 02691364 void addMenuMainPaneTranslation(hdMenuMain_c* self, float delta)
ext 026913EC void setMenuMainPaneTranslation(hdMenuMain_c* self, float x)
ext 0268EFBC void updateMenuMainSelectedItem(hdMenuMain_c* self, u32 key)
ext 02632D40 u32 getMenuMainChildMessage(hdParts_c* child)
ext 0268EE78 void updateMenuMainTabChild(hdMenuMain_c* self, u32 index, u32 enabled)
ext 0268E9D8 Pair32 classifyMenuMainSelection(hdMenuMain_c* self, u32 index)
ext 025200D4 hdMenuMainPlay_l* getMenuMainPlay()
ext 02632CFC u32 getMenuMainItemValue(hdParts_c* child)
ext 0268EE9C void resetMenuMainTabChild(hdMenuMain_c* self, u32 mode)
ext 020013C0 void copyMenuMainEvent(hdButtonEvent_l* destination, hdButtonEvent_l* source)
ext 026151A0 hdButtonReceiver_l* isMenuMainReceiverEvent(hdButtonEventManager_l* input)
*/
