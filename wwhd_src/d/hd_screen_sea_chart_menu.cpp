/** HD sea-chart menu parts and child factories; equivalence gates pending. */
#include "d/hd_screen_sea_chart_menu.h"
#include "d/hd_gamepad_map.h"
#include "d/hd_screen_cursor.h"
#include "d/hd_parts_button.h"
#include "gc_api/d/d_com_inf_game.h"
using namespace gabi;
using SeaMenuUiPointer_l = gptr<hdScreenUiSystem>;
static SeaMenuUiPointer_l sSeaMenuUi;
extern hdParts_c* seaChartConstructRupy(hdParts_c*);
extern void seaChartInitializeRupy(hdParts_c*, u32, u32, u32);
extern hdParts_c* seaChartConstructHeart(hdParts_c*);
extern void seaChartInitializeHeart(hdParts_c*, u32, u32, u32);
extern hdParts_c* seaChartConstructBarDisplay(hdParts_c*);
extern void seaChartInitializeBarDisplay(hdParts_c*, u32, u32, u32);
extern hdParts_c* seaChartConstructCompassClock(hdParts_c*);
extern void seaChartInitializeCompassClock(hdParts_c*, u32, u32, u32);
extern hdParts_c* seaChartConstructDungeonKey(hdParts_c*);
extern void seaChartInitializeDungeonKey(hdParts_c*, u32, u32, u32);
extern hdParts_c* seaChartConstructSwimTime(hdParts_c*);
extern void seaChartInitializeSwimTime(hdParts_c*, u32, u32, u32);
extern hdParts_c* seaChartConstructShortcut(hdParts_c*);
extern void seaChartInitializeShortcut(hdParts_c*, u32, u32, u32);
extern hdParts_c* seaChartConstructBossHp(hdParts_c*);
extern void seaChartInitializeBossHp(hdParts_c*, u32, u32, u32);
extern hdParts_c* seaChartConstructEnemyHp(hdParts_c*);
extern void seaChartInitializeEnemyHp(hdParts_c*, u32, u32, u32);
extern hdParts_c* seaChartConstructTime(hdParts_c*);
extern void seaChartInitializeTime(hdParts_c*, u32, u32, u32);
extern hdParts_c* seaChartConstructArrow(hdParts_c*);
extern void seaChartInitializeArrow(hdParts_c*, u32, u32, u32);
extern hdParts_c* seaChartConstructBattery(hdParts_c*);
extern void seaChartInitializeBattery(hdParts_c*, u32, u32, u32);
extern hdParts_c* seaChartConstructRupySwordCounter(hdParts_c*);
extern void seaChartInitializeRupySwordCounter(hdParts_c*, u32, u32, u32);
/* 026772A0 */
hdParts_c* hdSeaChartFactory_c::createRupy(u32 first, u32 second, u32 third) {
    constexpr u32 partSize = 0x64;
    auto* part = static_cast<hdParts_c*>(hdParts_new(partSize, sSeaMenuUi->mHeap.get(), 4));
    if (!part) return nullptr;
    part = seaChartConstructRupy(part);
    if (!part) return nullptr;
    seaChartInitializeRupy(part, first, second, third);
    return part;
}
/* 02677350 */
hdParts_c* hdSeaChartFactory_c::createHeart(u32 first, u32 second, u32 third) {
    constexpr u32 partSize = 0x104;
    auto* part = static_cast<hdParts_c*>(hdParts_new(partSize, sSeaMenuUi->mHeap.get(), 4));
    if (!part) return nullptr;
    part = seaChartConstructHeart(part);
    if (!part) return nullptr;
    seaChartInitializeHeart(part, first, second, third);
    return part;
}
/* 02677400 */
hdParts_c* hdSeaChartFactory_c::createBarDisplay(u32 first, u32 second, u32 third) {
    constexpr u32 partSize = 0x8C;
    auto* part = static_cast<hdParts_c*>(hdParts_new(partSize, sSeaMenuUi->mHeap.get(), 4));
    if (!part) return nullptr;
    part = seaChartConstructBarDisplay(part);
    if (!part) return nullptr;
    seaChartInitializeBarDisplay(part, first, second, third);
    return part;
}
/* 026774B0 */
hdParts_c* hdSeaChartFactory_c::createCompassClock(u32 first, u32 second, u32 third) {
    constexpr u32 partSize = 0x64;
    auto* part = static_cast<hdParts_c*>(hdParts_new(partSize, sSeaMenuUi->mHeap.get(), 4));
    if (!part) return nullptr;
    part = seaChartConstructCompassClock(part);
    if (!part) return nullptr;
    seaChartInitializeCompassClock(part, first, second, third);
    return part;
}
/* 02677560 */
hdParts_c* hdSeaChartFactory_c::createDungeonKey(u32 first, u32 second, u32 third) {
    constexpr u32 partSize = 0x6C;
    auto* part = static_cast<hdParts_c*>(hdParts_new(partSize, sSeaMenuUi->mHeap.get(), 4));
    if (!part) return nullptr;
    part = seaChartConstructDungeonKey(part);
    if (!part) return nullptr;
    seaChartInitializeDungeonKey(part, first, second, third);
    return part;
}
/* 02677610 */
hdParts_c* hdSeaChartFactory_c::createSwimTime(u32 first, u32 second, u32 third) {
    constexpr u32 partSize = 0xA8;
    auto* part = static_cast<hdParts_c*>(hdParts_new(partSize, sSeaMenuUi->mHeap.get(), 4));
    if (!part) return nullptr;
    part = seaChartConstructSwimTime(part);
    if (!part) return nullptr;
    seaChartInitializeSwimTime(part, first, second, third);
    return part;
}
/* 026776C0 */
hdParts_c* hdSeaChartFactory_c::createShortcut(u32 first, u32 second, u32 third) {
    constexpr u32 partSize = 0x6C;
    auto* part = static_cast<hdParts_c*>(hdParts_new(partSize, sSeaMenuUi->mHeap.get(), 4));
    if (!part) return nullptr;
    part = seaChartConstructShortcut(part);
    if (!part) return nullptr;
    seaChartInitializeShortcut(part, first, second, third);
    return part;
}
/* 02677770 */
hdParts_c* hdSeaChartFactory_c::createBossHp(u32 first, u32 second, u32 third) {
    constexpr u32 partSize = 0x50;
    auto* part = static_cast<hdParts_c*>(hdParts_new(partSize, sSeaMenuUi->mHeap.get(), 4));
    if (!part) return nullptr;
    part = seaChartConstructBossHp(part);
    if (!part) return nullptr;
    seaChartInitializeBossHp(part, first, second, third);
    return part;
}
/* 02677820 */
hdParts_c* hdSeaChartFactory_c::createEnemyHp(u32 first, u32 second, u32 third) {
    constexpr u32 partSize = 0x54;
    auto* part = static_cast<hdParts_c*>(hdParts_new(partSize, sSeaMenuUi->mHeap.get(), 4));
    if (!part) return nullptr;
    part = seaChartConstructEnemyHp(part);
    if (!part) return nullptr;
    seaChartInitializeEnemyHp(part, first, second, third);
    return part;
}
/* 026778D0 */
hdParts_c* hdSeaChartFactory_c::createTime(u32 first, u32 second, u32 third) {
    constexpr u32 partSize = 0x60;
    auto* part = static_cast<hdParts_c*>(hdParts_new(partSize, sSeaMenuUi->mHeap.get(), 4));
    if (!part) return nullptr;
    part = seaChartConstructTime(part);
    if (!part) return nullptr;
    seaChartInitializeTime(part, first, second, third);
    return part;
}
/* 02677980 */
hdParts_c* hdSeaChartFactory_c::createArrow(u32 first, u32 second, u32 third) {
    constexpr u32 partSize = 0x70;
    auto* part = static_cast<hdParts_c*>(hdParts_new(partSize, sSeaMenuUi->mHeap.get(), 4));
    if (!part) return nullptr;
    part = seaChartConstructArrow(part);
    if (!part) return nullptr;
    seaChartInitializeArrow(part, first, second, third);
    return part;
}
/* 02677A30 */
hdParts_c* hdSeaChartFactory_c::createBattery(u32 first, u32 second, u32 third) {
    constexpr u32 partSize = 0x54;
    auto* part = static_cast<hdParts_c*>(hdParts_new(partSize, sSeaMenuUi->mHeap.get(), 4));
    if (!part) return nullptr;
    part = seaChartConstructBattery(part);
    if (!part) return nullptr;
    seaChartInitializeBattery(part, first, second, third);
    return part;
}
/* 02677AE0 */
hdParts_c* hdSeaChartFactory_c::createRupySwordCounter(u32 first, u32 second, u32 third) {
    constexpr u32 partSize = 0x58;
    auto* part = static_cast<hdParts_c*>(hdParts_new(partSize, sSeaMenuUi->mHeap.get(), 4));
    if (!part) return nullptr;
    part = seaChartConstructRupySwordCounter(part);
    if (!part) return nullptr;
    seaChartInitializeRupySwordCounter(part, first, second, third);
    return part;
}
static hdPartsVtbl_c sSeaMenuVtable;
static u8 sSeaMenuHolderVtable;
static u8 sSeaMenuSecondaryVtable;
extern void* seaMenuAllocate(u32);
extern void seaMenuDelete(void*);
/* 02677B90 */
void hdSeaChartMenu_c::destroy(u32 flags) {
    if (this) {
        mStateHolder.mHolderVtable = &sSeaMenuHolderVtable;
        mSecondaryVtable = &sSeaMenuSecondaryVtable;
        NoControlPart_l::destroy(0);
        if (flags & 1) seaMenuDelete(this);
    }
}
/* 02677BFC */
hdSeaChartMenu_c* hdSeaChartMenu_c::construct(hdSeaChartMenu_c* part) {
    if (!part) part = static_cast<hdSeaChartMenu_c*>(seaMenuAllocate(sizeof(hdSeaChartMenu_c)));
    if (part) {
        NoControlPart_l::construct(part);
        part->mVtable = &sSeaMenuVtable;
        part->mSecondaryVtable = &sSeaMenuSecondaryVtable;
        part->mStateHolder.mHolderVtable = &sSeaMenuHolderVtable;
        if (!ea(part->mpMenuPanes)) seaMenuAllocate(sizeof(part->mpMenuPanes));
        if (!ea(&part->mpMainPane)) seaMenuAllocate(sizeof(part->mpMainPane));
        if (!ea(part->mpInfoPanes)) seaMenuAllocate(sizeof(part->mpInfoPanes));
        if (!ea(&part->mpTextPane)) seaMenuAllocate(sizeof(part->mpTextPane));
        if (!ea(part->mpMapMarkerPanes)) seaMenuAllocate(sizeof(part->mpMapMarkerPanes));
        if (!ea(&part->mpSelectedPane)) seaMenuAllocate(sizeof(part->mpSelectedPane));
        part->mpActiveButton = nullptr;
        part->mMapInputActive = 0;
        part->mOpening = 0;
        part->mSelectionMode = 0;
        part->mClosing = 0;
        part->mPreviousSelection = 0;
        part->mpMap = nullptr;
        for (u32 i = 0; i < 8; ++i) part->mpMenuPanes[i] = nullptr;
        part->mpMainPane = nullptr;
        for (u32 i = 0; i < 3; ++i) part->mpInfoPanes[i] = nullptr;
        part->mpTextPane = nullptr;
        for (u32 i = 0; i < 49; ++i) part->mpMapMarkerPanes[i] = nullptr;
        part->mpSelectedPane = nullptr;
    }
    return part;
}
extern hdParts_c* seaMenuConstructMap(hdParts_c*);
extern void seaMenuSetMapMode(hdParts_c*, u32);
extern void seaMenuOpenMap(hdParts_c*);
extern void seaMenuCloseMap(hdParts_c*);
extern void seaMenuStopAnimation(hdAnimSet*, u32, u32);
struct SeaMenuResourceIds_l { be<u32> mValues[9]; };
static SeaMenuResourceIds_l sSeaMenuResourceIds;
/* 0267810C */
void hdSeaChartMenu_c::allocateMap() {
    constexpr u32 mapSize = 0x158;
    auto* map = static_cast<hdParts_c*>(hdParts_new(mapSize, sSeaMenuUi->mHeap.get(), 4));
    if (map) map = seaMenuConstructMap(map);
    mpMap = map;
}
/* 02679B88 */
void hdSeaChartMenu_c::setSelectionMode(u32 mode) {
    hdParts_c* map = mpMap.get();
    mSelectionMode = mode;
    if (map) seaMenuSetMapMode(map, mode);
}
/* 02679D68 */
void hdSeaChartMenu_c::openMap() {
    hdParts_c* map = mpMap.get();
    if (map) seaMenuOpenMap(map);
}
/* 02679D78 */
void hdSeaChartMenu_c::closeMap() {
    hdParts_c* map = mpMap.get();
    if (map) seaMenuCloseMap(map);
}
/* 02679BC8 */
void hdSeaChartMenu_c::stopIconAnimations() {
    constexpr u32 firstAnimation = 0x62;
    constexpr u32 firstGroup = 0x63;
    constexpr u32 iconCount = 8;
    for (u32 i = 0; i < iconCount; ++i)
        seaMenuStopAnimation(mpLayout->animSet(), firstAnimation + i, firstGroup + i);
}
/* 0267A23C */
u32 hdSeaChartMenu_c::getMapResource(u32 index) {
    return sSeaMenuResourceIds.mValues[index];
}
/* 0267A250 */
s32 hdSeaChartMenu_c::findMapResource(u32 resource) {
    for (u32 i = 0; i < 9; ++i)
        if (getMapResource(i) == resource) return s32(i);
    return -1;
}
struct SeaMenuMapDeleteVtable_l { u8 mReserved[0xC]; gfn mDelete; };
struct SeaMenuMapDeleteView_l { u8 mReserved[0x154]; gptr<SeaMenuMapDeleteVtable_l> mpVtable; };
WWHD_OFFSET(SeaMenuMapDeleteView_l, mpVtable, 0x154);
extern void seaMenuReleaseMap(hdParts_c*);
extern void seaMenuReleaseBaseChildren(hdSeaChartMenu_c*);
extern void seaMenuUpdateSelectionPosition(hdSeaChartMenu_c*);
extern void seaMenuRefreshSelection(hdSeaChartMenu_c*);
extern void seaMenuResetIcons(hdSeaChartMenu_c*);
/* 02678374 */
void hdSeaChartMenu_c::deleteSelectedPane() {
    if (mpSelectedPane) {
        auto* pane = mpSelectedPane.get();
        if (pane) call_ptr(pane->mpVtable->mDelete.get(), pane, 3);
        mpSelectedPane = nullptr;
    }
}
/* 026783D0 */
void hdSeaChartMenu_c::deleteMap() {
    auto* map = mpMap.get();
    if (!map) return;
    seaMenuReleaseMap(map);
    map = mpMap.get();
    if (map) {
        auto* view = reinterpret_cast<SeaMenuMapDeleteView_l*>(map);
        call_ptr(view->mpVtable->mDelete.get(), map, 3);
    }
    mpMap = nullptr;
}
/* 02678430 */
void hdSeaChartMenu_c::releaseChildren() {
    seaMenuReleaseBaseChildren(this);
    deleteSelectedPane();
    deleteMap();
}
/* 02678B00 */
void hdSeaChartMenu_c::hideMenuPanes() {
    constexpr u32 paneCount = 8;
    constexpr u8 visibleFlag = 1;
    for (u32 i = 0; i < paneCount; ++i) {
        if (mpMenuPanes[i]) {
            auto* pane = mpMenuPanes[i].get();
            pane->mFlags = u8(pane->mFlags) & u8(~visibleFlag);
        }
    }
}
/* 02679BA0 */
void hdSeaChartMenu_c::updateSelectionPosition() {
    seaMenuUpdateSelectionPosition(this);
}
/* 02679BA4 */
void hdSeaChartMenu_c::hideSelection() {
    hideMenuPanes();
    seaMenuRefreshSelection(this);
}
/* 02679C38 */
void hdSeaChartMenu_c::stopAndResetIcons() {
    stopIconAnimations();
    seaMenuResetIcons(this);
}
struct SeaMenuPaneNames_l { hdUiSafeString mNames[8]; };
struct SeaMenuInfoNames_l { hdUiSafeString mNames[3]; };
static SeaMenuPaneNames_l sSeaMenuPaneNames;
static SeaMenuInfoNames_l sSeaMenuInfoNames;
static hdUiSafeString sSeaMenuMainName;
extern hdSeaMenuSelected_l* seaMenuConstructSelected(hdSeaMenuSelected_l*, hdPartsPaneSRT_c*, u32);
/* 02677D3C */
void hdSeaChartMenu_c::findMenuPanes() {
    for (u32 i = 0; i < 8; ++i) {
        auto* name = &sSeaMenuPaneNames.mNames[i];
        auto* accessor = mpLayout->mAccessor.get();
        name->assureTermination();
        mpMenuPanes[i] = static_cast<hdPartsPaneSRT_c*>(accessor->mObject->findPane(
            reinterpret_cast<const char*>(name->mText.get())));
    }
}
/* 02677DF0 */
void hdSeaChartMenu_c::findMainPane() {
    auto* accessor = mpLayout->mAccessor.get();
    sSeaMenuMainName.assureTermination();
    mpMainPane = static_cast<hdPartsPaneSRT_c*>(accessor->mObject->findPane(
        reinterpret_cast<const char*>(sSeaMenuMainName.mText.get())));
}
/* 02677E6C */
void hdSeaChartMenu_c::findInfoPanes() {
    for (u32 i = 0; i < 3; ++i) {
        auto* name = &sSeaMenuInfoNames.mNames[i];
        auto* accessor = mpLayout->mAccessor.get();
        name->assureTermination();
        mpInfoPanes[i] = static_cast<hdPartsPaneSRT_c*>(accessor->mObject->findPane(
            reinterpret_cast<const char*>(name->mText.get())));
    }
}
/* 02678090 */
void hdSeaChartMenu_c::allocateSelectedPane() {
    auto* heap = sSeaMenuUi->mHeap.get();
    if (!mpMainPane) return;
    constexpr u32 selectedPaneSize = 0x60;
    auto* selected = static_cast<hdSeaMenuSelected_l*>(hdParts_new(selectedPaneSize, heap, 4));
    if (selected) selected = seaMenuConstructSelected(selected, mpMainPane.get(), 0);
    mpSelectedPane = selected;
}
// MAP-sea_chart#2 owns the map display; this typed view is pending its merged header.
struct SeaMenuMapBindings_l {
    u8 mReserved[0xC4];
    gptr<hdSeaMenuSelected_l> mpSelectedPane;
    gptr<hdPartsPaneSRT_c> mpMainPane;
    u8 mReservedCC[0x18];
    gptr<hdAnimSet> mpAnimations;
    be<u32> mFirstAnimation;
    be<u32> mAnimationCount;
    be<u32> mFirstIconAnimation;
    be<u32> mLastIconAnimation;
    be<u32> mFirstGroup;
    be<u32> mGroupCount;
    be<u32> mFirstIconGroup;
    be<u32> mLastIconGroup;
    u8 mReserved108[4];
    gptr<hdSeaChartMenu_c> mpMenu;
};
WWHD_OFFSET(SeaMenuMapBindings_l, mpSelectedPane, 0xC4);
WWHD_OFFSET(SeaMenuMapBindings_l, mpMainPane, 0xC8);
WWHD_OFFSET(SeaMenuMapBindings_l, mpAnimations, 0xE4);
WWHD_OFFSET(SeaMenuMapBindings_l, mFirstAnimation, 0xE8);
WWHD_OFFSET(SeaMenuMapBindings_l, mAnimationCount, 0xEC);
WWHD_OFFSET(SeaMenuMapBindings_l, mFirstIconAnimation, 0xF0);
WWHD_OFFSET(SeaMenuMapBindings_l, mLastIconAnimation, 0xF4);
WWHD_OFFSET(SeaMenuMapBindings_l, mFirstGroup, 0xF8);
WWHD_OFFSET(SeaMenuMapBindings_l, mGroupCount, 0xFC);
WWHD_OFFSET(SeaMenuMapBindings_l, mFirstIconGroup, 0x100);
WWHD_OFFSET(SeaMenuMapBindings_l, mLastIconGroup, 0x104);
WWHD_OFFSET(SeaMenuMapBindings_l, mpMenu, 0x10C);
extern void seaMenuCreateMap(hdParts_c*);
extern void seaMenuFinishMap(hdParts_c*);
/* 0267815C */
void hdSeaChartMenu_c::initializeMap() {
    auto* map = mpMap.get();
    if (!map) return;
    seaMenuCreateMap(map);
    auto bindings = [this]() { return reinterpret_cast<SeaMenuMapBindings_l*>(mpMap.get()); };
    bindings()->mpSelectedPane = mpSelectedPane.get();
    bindings()->mpMainPane = mpMainPane.get();
    bindings()->mpAnimations = mpLayout->animSet();
    bindings()->mFirstAnimation = 0;
    bindings()->mAnimationCount = 49;
    bindings()->mFirstIconAnimation = 0x6B;
    bindings()->mLastIconAnimation = 0x9C;
    bindings()->mFirstGroup = 1;
    bindings()->mGroupCount = 50;
    bindings()->mFirstIconGroup = 0x6C;
    bindings()->mLastIconGroup = 0x9D;
    bindings()->mpMenu = this;
    seaMenuFinishMap(mpMap.get());
}
static hdUiSafeString sSeaMenuTextName;
static hdUiSafeString sSeaMenuTextType;
static hdUiSafeString sSeaMenuLayoutName;
using SeaMenuAnimationRate_l = be<f32>;
static SeaMenuAnimationRate_l sSeaMenuAnimationRate;
extern hdPartsPaneSRT_c* seaMenuFindTextPane(hdPartsLayout_c*, hdUiSafeString*, hdUiSafeString*);
extern void seaMenuUpdateMap(hdParts_c*);
extern void seaMenuUpdateMarkerPosition(hdSeaChartMenu_c*);
/* 02677F20 */
void hdSeaChartMenu_c::findTextPane() {
    mpTextPane = seaMenuFindTextPane(mpLayout.get(), &sSeaMenuTextName, &sSeaMenuTextType);
}
/* 0267822C */
void hdSeaChartMenu_c::initializeChildren() {
    findMenuPanes();
    findMainPane();
    findInfoPanes();
    findTextPane();
    findMapMarkerPanes();
    allocateSelectedPane();
    allocateMap();
    initializeMap();
    constexpr u32 markerCount = 49;
    for (u32 i = 0; i < markerCount; ++i)
        seaMenuStopAnimation(mpLayout->animSet(), 49 + i, 50 + i);
    hdAnimSet_play(mpLayout->animSet(), 0x6A, 0x6B, sSeaMenuAnimationRate);
    vCalcLayout();
}
/* 02678308 */
u32 hdSeaChartMenu_c::create(u32 first, u32 second, u32 third) {
    if (!NoControlPart_l::create(&sSeaMenuLayoutName, gabi::at<u8>(first), second, third)) return 0;
    initializeChildren();
    return 1;
}
/* 02679C6C */
void hdSeaChartMenu_c::updateOpenMap() {
    auto* map = mpMap.get();
    mClosing = 0;
    if (!map) return;
    seaMenuUpdateMap(map);
    seaMenuUpdateMarkerPosition(this);
    updateSelectionPosition();
    hideSelection();
    stopAndResetIcons();
}
static hdUiSafeString sSeaMenuMarkerPrefix;
static hdScreenStringTable sSeaMenuMarkerStringVtable;
extern void seaMenuFormatMarker(hdScreenFixedString*, const char*, be<u8>*, u32);
/* 02677F64 */
void hdSeaChartMenu_c::findMapMarkerPanes() {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x78> frame;
    auto* name = at<hdScreenFixedString>(frame + 8);
    constexpr u32 capacity = 64;
    name->mText = name->mBuffer;
    name->mBuffer[0] = 0;
    name->mBuffer[capacity - 1] = 0;
    name->mCapacity = capacity;
    name->mVtable = &sSeaMenuMarkerStringVtable;
    for (u32 i = 0; i < 49; ++i) {
        sSeaMenuMarkerPrefix.assureTermination();
        seaMenuFormatMarker(name, "%s%02d", sSeaMenuMarkerPrefix.mText.get(), i);
        auto* accessor = mpLayout->mAccessor.get();
        name->assureTermination();
        mpMapMarkerPanes[i] = static_cast<hdPartsPaneSRT_c*>(accessor->mObject->findPane(
            reinterpret_cast<const char*>(name->mText.get())));
    }
}
// FW-parts_buttons owns this derived button layout; adopt its header after merge.
struct SeaMenuButton_l : hdButtonPart_c { u8 mReserved74[0xD0]; be<u32> mMode; };
WWHD_OFFSET(SeaMenuButton_l, mMode, 0x144);
static hdPartsStateId_c sSeaMenuBlankState;
using SeaMenuEventMgrPointer_l = gptr<cking::ui::input::EventMgr>;
static SeaMenuEventMgrPointer_l sSeaMenuEventMgr;
extern hdPartsStateId_c* seaMenuCurrentState(hdStateMachine*);
extern u32 seaMenuReceiverId(cking::ui::input::EventMgr*);
extern void seaMenuAppendReceiver(cking::ui::input::ReceiverList*, cking::ui::input::ReceiverNode*);
/* 02679200 */
void hdSeaChartMenu_c::updateButton() {
    auto* button = mpActiveButton.get();
    if (!button) return;
    auto* current = seaMenuCurrentState(button->stateMachine());
    auto* expectedVtable = sSeaMenuBlankState.mVtable.get();
    u32 actualKey = current->getKey();
    u32 expectedKey = call_ptr<u32>(expectedVtable->mGetKey.get(), &sSeaMenuBlankState);
    if (actualKey == expectedKey) return;
    button = mpActiveButton.get();
    auto* view = reinterpret_cast<SeaMenuButton_l*>(button);
    if (u32(view->mMode) == 0 || u32(view->mMode) == 2) return;
    constexpr u32 executeButtonSlot = 0xCC / 4;
    auto* vtable = reinterpret_cast<gfn*>(button->mVtable.get());
    call_ptr(vtable[executeButtonSlot].get(), button);
}
/* 026792B8 */
void hdSeaChartMenu_c::registerSelectedPane() {
    auto* manager = sSeaMenuEventMgr.get();
    if (!manager || !mpSelectedPane) return;
    if (seaMenuReceiverId(manager)) {
        auto* selected = mpSelectedPane.get();
        u32 id = seaMenuReceiverId(sSeaMenuEventMgr.get());
        if (id != u32(selected->mId)) return;
    }
    seaMenuAppendReceiver(sSeaMenuEventMgr.get(), mpSelectedPane.get());
}
/* 02679340 */
void hdSeaChartMenu_c::updateMapInput() {
    if (hdPadUiBusy(l_hdMapUi.mOwner->mUi.get()) || !mMapInputActive) return;
    updateButton();
    auto* button = reinterpret_cast<SeaMenuButton_l*>(mpActiveButton.get());
    if (button && u32(button->mMode) == 3) return;
    registerSelectedPane();
}
struct SeaMenuMapTile_l { u8 mReserved[0x97]; be<u8> mAvailable; u8 mReserved98; be<u8> mBusy; };
struct SeaMenuMapStatus_l {
    u8 mReserved[0x118]; gptr<SeaMenuMapTile_l> mpSelectedTile;
    u8 mReserved11C[0x21]; be<u8> mForceOpening;
    u8 mReserved13E[0x13]; be<u8> mOpening;
};
WWHD_OFFSET(SeaMenuMapTile_l, mAvailable, 0x97);
WWHD_OFFSET(SeaMenuMapTile_l, mBusy, 0x99);
WWHD_OFFSET(SeaMenuMapStatus_l, mpSelectedTile, 0x118);
WWHD_OFFSET(SeaMenuMapStatus_l, mForceOpening, 0x13D);
WWHD_OFFSET(SeaMenuMapStatus_l, mOpening, 0x151);
struct SeaMenuCoordinates_l { HdPadCoordinate mValues[9]; };
static SeaMenuCoordinates_l sSeaMenuCoordinates;
extern SeaMenuMapTile_l* seaMenuMapTile(hdParts_c*, HdPadCoordinate*);
extern void seaMenuSetMapSelection(void*, u32);
// This owner prefix is already shared by the merged map code.
struct SeaMenuMapSelectionOwner_l { u8 mReserved[0x1BC]; gptr<void> mpSelection; };
WWHD_OFFSET(SeaMenuMapSelectionOwner_l, mpSelection, 0x1BC);
/* 02678770 */
void hdSeaChartMenu_c::beginOpening() {
    auto* map = reinterpret_cast<SeaMenuMapStatus_l*>(mpMap.get());
    auto* selected = map->mpSelectedTile.get();
    for (u32 i = 0; i < 9; ++i) {
        auto* tile = seaMenuMapTile(mpMap.get(), &sSeaMenuCoordinates.mValues[i]);
        if (tile != selected) continue;
        auto* owner = reinterpret_cast<SeaMenuMapSelectionOwner_l*>(l_hdMapUi.mOwner.get());
        auto* selection = owner->mpSelection.get();
        if (!selection) continue;
        u32 value = selected->mAvailable ? i + 1 : 0;
        seaMenuSetMapSelection(selection, value);
        mPreviousSelection = i;
        auto* currentMap = reinterpret_cast<SeaMenuMapStatus_l*>(mpMap.get());
        mOpening = 1;
        currentMap->mOpening = 1;
        return;
    }
}
/* 02678830 */
void hdSeaChartMenu_c::updateOpeningFlag() {
    auto* map = reinterpret_cast<SeaMenuMapStatus_l*>(mpMap.get());
    mOpening = 0;
    if (!map) return;
    auto* tile = map->mpSelectedTile.get();
    if (!tile) return;
    if (!tile->mBusy && !map->mForceOpening) return;
    if (mClosing) return;
    beginOpening();
}
union SeaMenuCoordinateResult_l { HdPadCoordinate mCoordinate; be<u32> mWord; };
/* 0267A204 */
u32 hdSeaChartMenu_c::getMapCoordinate(u32 index) {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x18> frame;
    auto* packed = at<SeaMenuCoordinateResult_l>(frame + 0xC);
    u8 x = sSeaMenuCoordinates.mValues[index].x;
    u8 y = sSeaMenuCoordinates.mValues[index].y;
    auto* temporary = at<HdPadCoordinate>(frame + 8);
    temporary->x = x;
    temporary->y = y;
    x = temporary->x;
    y = temporary->y;
    packed->mCoordinate.x = x;
    packed->mCoordinate.y = y;
    return packed->mWord;
}
struct SeaMenuQueuedEvent_l { be<u32> mKind; be<u32> mReceiver; u8 mPayload[0x20]; };
struct SeaMenuLatestEvent_l { u8 mReserved[0x30]; SeaMenuQueuedEvent_l mLatest; };
WWHD_SIZE(SeaMenuQueuedEvent_l, 0x28);
WWHD_OFFSET(SeaMenuLatestEvent_l, mLatest, 0x30);
extern void seaMenuReadEvent(SeaMenuQueuedEvent_l*, SeaMenuQueuedEvent_l*);
extern void seaMenuConfirmSelection(hdParts_c*);
extern void seaMenuConfirmAlternate(hdParts_c*);
extern void seaMenuCloseTile(SeaMenuMapTile_l*);
extern void seaMenuPlaySound(const char*);
/* 02679ED4 */
void hdSeaChartMenu_c::activateSelection() {
    auto* button = mpActiveButton.get();
    if (!button) return;
    auto* map = reinterpret_cast<SeaMenuMapStatus_l*>(mpMap.get());
    if (!map) return;
    auto* tile = map->mpSelectedTile.get();
    if (!tile) return;
    auto* current = seaMenuCurrentState(button->stateMachine());
    auto* expectedVtable = sSeaMenuBlankState.mVtable.get();
    u32 actualKey = current->getKey();
    u32 expectedKey = call_ptr<u32>(expectedVtable->mGetKey.get(), &sSeaMenuBlankState);
    if (actualKey == expectedKey) return;
    if (tile->mAvailable) {
        button = mpActiveButton.get();
        u32 mode = reinterpret_cast<SeaMenuButton_l*>(button)->mMode;
        if (mode == 4) {
            seaMenuConfirmSelection(button);
            seaMenuPlaySound("SE_MENU_DECIDE");
            return;
        }
        if (mode == 1) {
            seaMenuConfirmAlternate(button);
            seaMenuPlaySound("SE_MENU_DECIDE");
            return;
        }
        if (mode != 3) return;
    }
    seaMenuCloseTile(tile);
    seaMenuPlaySound("SE_MENU_CHART_CLOSE");
}
/* 02679FE4 */
void hdSeaChartMenu_c::pollSelectionActivation() {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x38> frame;
    auto* event = at<SeaMenuQueuedEvent_l>(frame + 8);
    auto* manager = reinterpret_cast<SeaMenuLatestEvent_l*>(sSeaMenuEventMgr.get());
    seaMenuReadEvent(event, &manager->mLatest);
    constexpr u32 activateEvent = 0x60;
    if (u32(event->mKind) == activateEvent) activateSelection();
}
struct SeaMenuPanePosition_l { u8 mReserved[0x1C]; be<f32> mX, mY, mZ; };
struct SeaMenuTilePosition_l { u8 mReserved[0x14]; HdPadPoint mPosition; };
WWHD_OFFSET(SeaMenuPanePosition_l, mX, 0x1C);
WWHD_OFFSET(SeaMenuTilePosition_l, mPosition, 0x14);
using SeaMenuFloat_l = be<f32>;
static SeaMenuFloat_l sSeaMenuZero;
/* 02678A54 */
void hdSeaChartMenu_c::updateSelectionPanePosition() {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x18> frame;
    auto* pane = mpInfoPanes[1].get();
    if (!pane) return;
    auto* ui = l_hdMapUi.mOwner->mUi.get();
    u8 x = ui->mCoordinate.x, y = ui->mCoordinate.y;
    auto* coordinate = at<HdPadCoordinate>(frame + 8);
    coordinate->x = x;
    coordinate->y = y;
    auto* tile = seaMenuMapTile(mpMap.get(), coordinate);
    if (!tile) return;
    auto* position = reinterpret_cast<SeaMenuTilePosition_l*>(tile);
    f32 tileY = position->mPosition.y;
    u8 flags = pane->mFlags;
    f32 zero = sSeaMenuZero;
    f32 tileX = position->mPosition.x;
    auto* output = reinterpret_cast<SeaMenuPanePosition_l*>(pane);
    output->mZ = zero;
    output->mX = tileX;
    output->mY = tileY;
    pane->mFlags = flags | 0x10;
    u32 busy = hdPadUiBusy(l_hdMapUi.mOwner->mUi.get());
    pane->mFlags = (u32(pane->mFlags) & 0xFE) | (busy ^ 1);
}
struct SeaMenuRoute_l { u8 mReserved[0x34]; HdPadPoint mStart, mTarget; };
struct SeaMenuRouteMap_l {
    u8 mReserved[0x120]; gptr<SeaMenuRoute_l> mpRoute;
    u8 mReserved124[0x1C]; be<u32> mMode;
};
struct SeaMenuCompass_l { u8 mReserved[0x18]; be<u32> mMode; };
struct SeaMenuRouteSettings_l { u8 mReserved[0x14]; be<u8> mEnabled; };
using SeaMenuCompassPointer_l = gptr<SeaMenuCompass_l>;
using SeaMenuRouteSettingsPointer_l = gptr<SeaMenuRouteSettings_l>;
static SeaMenuCompassPointer_l sSeaMenuCompass;
static SeaMenuRouteSettingsPointer_l sSeaMenuRouteSettings;
WWHD_OFFSET(SeaMenuRouteMap_l, mpRoute, 0x120);
WWHD_OFFSET(SeaMenuRouteMap_l, mMode, 0x140);
WWHD_OFFSET(SeaMenuCompass_l, mMode, 0x18);
WWHD_OFFSET(SeaMenuRoute_l, mStart, 0x34);
extern void seaMenuSetCompassStart(SeaMenuCompass_l*, HdPadPoint*);
extern void seaMenuSetCompassTarget(SeaMenuCompass_l*, HdPadPoint*);
/* 0267846C */
void hdSeaChartMenu_c::updateCompassRoute() {
    auto* map = reinterpret_cast<SeaMenuRouteMap_l*>(mpMap.get());
    if (!map) return;
    if (u32(map->mMode) == 1) sSeaMenuCompass->mMode = 1;
    else if (u32(map->mMode) == 0) sSeaMenuCompass->mMode = 0;
    if (!sSeaMenuRouteSettings->mEnabled) return;
    map = reinterpret_cast<SeaMenuRouteMap_l*>(mpMap.get());
    auto* route = map->mpRoute.get();
    if (!route) return;
    seaMenuSetCompassStart(sSeaMenuCompass.get(), &route->mStart);
    seaMenuSetCompassTarget(sSeaMenuCompass.get(), &route->mTarget);
}
using SeaMenuEventWord_l = be<u32>;
static SeaMenuEventWord_l sSeaMenuReceiverIndex;
static SeaMenuEventWord_l sSeaMenuForwardKind;
extern u32 seaMenuButtonId(hdParts_c*);
extern u32 seaMenuIsButtonEvent(u32);
extern void seaMenuUpdateTouchMap(hdParts_c*);
extern void seaMenuCreateEvent(SeaMenuQueuedEvent_l*, u32, u32, u32);
extern void seaMenuQueueEvent(u8*, SeaMenuQueuedEvent_l*);
extern void seaMenuDestroyEvent(SeaMenuQueuedEvent_l*);
/* 02679D88 */
void hdSeaChartMenu_c::forwardButtonEvent(u32 receiver) {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x38> frame;
    u32 index = sSeaMenuReceiverIndex;
    auto* button = child(index);
    if (!button || receiver != seaMenuButtonId(button)) return;
    u32 id = seaMenuButtonId(button);
    u32 kind = sSeaMenuForwardKind;
    auto* event = at<SeaMenuQueuedEvent_l>(frame + 8);
    seaMenuCreateEvent(event, kind, id, 0);
    seaMenuQueueEvent(sSeaMenuEventMgr->mEventQueue, event);
    seaMenuDestroyEvent(event);
}
/* 02679E30 */
void hdSeaChartMenu_c::pollButtonEvent() {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x38> frame;
    auto* manager = sSeaMenuEventMgr.get();
    if (!manager) return;
    auto* map = mpMap.get();
    if (map && mMapInputActive) {
        seaMenuUpdateTouchMap(map);
        manager = sSeaMenuEventMgr.get();
    }
    auto* event = at<SeaMenuQueuedEvent_l>(frame + 8);
    seaMenuReadEvent(event, &reinterpret_cast<SeaMenuLatestEvent_l*>(manager)->mLatest);
    u32 kind = event->mKind;
    u32 receiver = event->mReceiver;
    if (!seaMenuIsButtonEvent(kind)) return;
    auto* button = mpActiveButton.get();
    if (button && receiver == seaMenuButtonId(button)) forwardButtonEvent(receiver);
}
struct SeaMenuButtonBinding_l : hdButtonPart_c {
    u8 mReserved74[4]; be<u32> mEnabled; u8 mReserved7C[0x88];
    gptr<hdPartsPaneSRT_c> mpMainPane; u8 mReserved108[0x5C]; gptr<hdParts_c> mpMap;
};
WWHD_OFFSET(SeaMenuButtonBinding_l, mEnabled, 0x78);
WWHD_OFFSET(SeaMenuButtonBinding_l, mpMainPane, 0x104);
WWHD_OFFSET(SeaMenuButtonBinding_l, mpMap, 0x164);
extern void seaMenuInitializeAlternate(hdParts_c*);
/* 02679CCC */
void hdSeaChartMenu_c::bindActiveButton() {
    auto* button = mChildren.mpData[0].get();
    mpActiveButton = button;
    if (!button) return;
    u32 mode = mSelectionMode;
    if (mode == 0) {
        reinterpret_cast<SeaMenuButtonBinding_l*>(button)->mEnabled = 1;
        auto* binding = reinterpret_cast<SeaMenuButtonBinding_l*>(mpActiveButton.get());
        binding->mpMainPane = mpMainPane.get();
        binding = reinterpret_cast<SeaMenuButtonBinding_l*>(mpActiveButton.get());
        binding->mpMap = mpMap.get();
    } else if (mode == 1) {
        seaMenuInitializeAlternate(button);
        reinterpret_cast<SeaMenuButtonBinding_l*>(mpActiveButton.get())->mpMap = mpMap.get();
    }
}
struct SeaMenuMapCloseFlag_l { u8 mReserved[0x13E]; be<u8> mCloseRequested; };
WWHD_OFFSET(SeaMenuMapCloseFlag_l, mCloseRequested, 0x13E);
extern void seaMenuConfirmRoute(SeaMenuRoute_l*);
struct SeaMenuUiMapOwner_l { u8 mReserved[0x28]; gptr<hdParts_c> mpOwner; };
WWHD_OFFSET(SeaMenuUiMapOwner_l, mpOwner, 0x28);
/* 0267A034 */
void hdSeaChartMenu_c::processMapAction() {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x48> frame;
    auto* button = mpActiveButton.get();
    if (!button) return;
    auto* map = reinterpret_cast<SeaMenuMapStatus_l*>(mpMap.get());
    if (!map) return;
    if (map->mForceOpening) {
        auto* tile = map->mpSelectedTile.get();
        if (tile) {
            if (mSelectionMode) return;
            auto* current = seaMenuCurrentState(button->stateMachine());
            auto* expectedVtable = sSeaMenuBlankState.mVtable.get();
            u32 actualKey = current->getKey();
            u32 expectedKey = call_ptr<u32>(expectedVtable->mGetKey.get(), &sSeaMenuBlankState);
            if (actualKey == expectedKey) return;
            if (tile->mAvailable) {
                button = mpActiveButton.get();
                u32 mode = reinterpret_cast<SeaMenuButton_l*>(button)->mMode;
                if (mode == 4) {
                    seaMenuConfirmSelection(button);
                    seaMenuPlaySound("SE_MENU_DECIDE");
                    return;
                }
                if (mode == 1) {
                    seaMenuConfirmAlternate(button);
                    seaMenuPlaySound("SE_MENU_DECIDE");
                    return;
                }
                if (mode != 3) return;
            }
            seaMenuCloseTile(tile);
            seaMenuPlaySound("SE_MENU_CHART_CLOSE");
        } else {
            auto* route = reinterpret_cast<SeaMenuRouteMap_l*>(map)->mpRoute.get();
            if (!route) return;
            seaMenuConfirmRoute(route);
            seaMenuPlaySound("SE_MENU_DECIDE");
        }
    } else if (reinterpret_cast<SeaMenuMapCloseFlag_l*>(map)->mCloseRequested) {
        auto* owner = reinterpret_cast<SeaMenuUiMapOwner_l*>(l_hdMapUi.mOwner->mUi.get())->mpOwner.get();
        auto* receiver = owner->child(3);
        u32 id = seaMenuButtonId(receiver);
        auto* event = at<SeaMenuQueuedEvent_l>(frame + 8);
        seaMenuCreateEvent(event, 0x6D, id, 0);
        seaMenuQueueEvent(sSeaMenuEventMgr->mEventQueue, event);
    }
}
struct SeaMenuSelectionBlock_l { u8 mReserved[0xD1]; be<u8> mAllowsMapInput; };
WWHD_OFFSET(SeaMenuSelectionBlock_l, mAllowsMapInput, 0xD1);
extern void seaMenuUpdateOpening(hdSeaChartMenu_c*);
/* 026793B4 */
void hdSeaChartMenu_c::executeMapMenu() {
    auto* owner = reinterpret_cast<SeaMenuMapSelectionOwner_l*>(l_hdMapUi.mOwner.get());
    auto* selection = static_cast<hdParts_c*>(owner->mpSelection.get());
    if (selection) {
        auto* current = seaMenuCurrentState(selection->stateMachine());
        auto* expectedVtable = sSeaMenuBlankState.mVtable.get();
        u32 actualKey = current->getKey();
        u32 expectedKey = call_ptr<u32>(expectedVtable->mGetKey.get(), &sSeaMenuBlankState);
        if (actualKey != expectedKey && !reinterpret_cast<SeaMenuSelectionBlock_l*>(selection)->mAllowsMapInput) return;
    }
    updateCompassRoute();
    u32 mode = mSelectionMode;
    if (mode == 0) seaMenuUpdateOpening(this);
    else if (mode == 1) updateOpeningFlag();
    seaMenuUpdateMarkerPosition(this);
    seaMenuUpdateSelectionPosition(this);
    seaMenuRefreshSelection(this);
    seaMenuResetIcons(this);
    vExecuteChildren();
    vCalcLayout();
    updateMapInput();
}
struct SeaMenuAnimationNames_l { hdUiSafeString mNames[0xA7]; };
struct SeaMenuAnimationGroups_l { hdUiSafeString mNames[0xA8]; };
struct SeaMenuAnimationRemap_l { be<u32> mIndices[0xA7]; };
static SeaMenuAnimationNames_l sSeaMenuAnimationNames;
static SeaMenuAnimationGroups_l sSeaMenuAnimationGroups;
static SeaMenuAnimationRemap_l sSeaMenuAnimationRemap;
extern void seaMenuBindAnimationRange(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*, u32);
/* 026799F0 */
u32 hdSeaChartMenu_c::loadMenuLayout(void* name, u32 first, u32 second, u32 third, void* archive) {
    auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sSeaMenuUi->mHeap.get(), 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, first, second, third, 0xA7, 0xA8, archive)) return 0;
    seaMenuBindAnimationRange(animSet(), 0, &sSeaMenuAnimationNames.mNames[0], &sSeaMenuAnimationGroups.mNames[1], 49);
    seaMenuBindAnimationRange(animSet(), 49, &sSeaMenuAnimationNames.mNames[49], &sSeaMenuAnimationGroups.mNames[50], 49);
    seaMenuBindAnimationRange(animSet(), 0x62, &sSeaMenuAnimationNames.mNames[0x62], &sSeaMenuAnimationGroups.mNames[0x63], 8);
    hdAnimSet_bindGroup(animSet(), 0x6A, &sSeaMenuAnimationNames.mNames[0x6A], &sSeaMenuAnimationGroups.mNames[0x6B]);
    seaMenuBindAnimationRange(animSet(), 0x6B, &sSeaMenuAnimationNames.mNames[0x6B], &sSeaMenuAnimationGroups.mNames[0x6C], 49);
    for (u32 i = 0; i < 11; ++i) {
        u32 index = i + 0x9C;
        u32 group = sSeaMenuAnimationRemap.mIndices[index];
        hdAnimSet_bindGroup(animSet(), index, &sSeaMenuAnimationNames.mNames[index], &sSeaMenuAnimationGroups.mNames[group]);
    }
    return 1;
}
struct SeaMenuInitializerWords_l { be<u32> mWords[4]; };
struct SeaMenuInitializerLimits_l { be<f32> mMinimumAngle, mMaximumAngle, mWidth, mHeight, mLimitX, mLimitY; };
struct SeaMenuRegistration_l { u8 mReserved[12]; };
struct SeaMenuAuxiliaryNames_l { hdUiSafeString mNames[3]; };
static SeaMenuInitializerWords_l sSeaMenuInitializerWords;
static SeaMenuInitializerLimits_l sSeaMenuInitializerLimits;
static SeaMenuRegistration_l sSeaMenuFirstRegistration;
static SeaMenuRegistration_l sSeaMenuSecondRegistration;
static SeaMenuRegistration_l sSeaMenuThirdRegistration;
static SeaMenuAuxiliaryNames_l sSeaMenuAuxiliaryNames;
using SeaMenuByte_l = be<u8>;
static SeaMenuByte_l sSeaMenuFirstObject;
static SeaMenuByte_l sSeaMenuSecondObject;
static SeaMenuFloat_l sSeaMenuMinimumAngle;
static SeaMenuFloat_l sSeaMenuMaximumAngle;
static SeaMenuFloat_l sSeaMenuExtent;
static SeaMenuFloat_l sSeaMenuLimit;
static hdUiStringTable sSeaMenuStringVtable;
extern void seaMenuRegisterGlobal(SeaMenuRegistration_l*);
extern void seaMenuConstructFirstObject(be<u8>*);
extern void seaMenuConstructSecondObject(be<u8>*);
static void seaMenuInitializeName(hdUiSafeString* name, const char* text) {
    name->mVtable = &sSeaMenuStringVtable;
    name->mText.v = guest_addr(text);
}
/* 0267A29C */
static void __sinit_hd_screen_sea_chart_menu_cpp() {
    sSeaMenuInitializerWords.mWords[3] = 0;
    sSeaMenuInitializerWords.mWords[2] = 0;
    sSeaMenuInitializerWords.mWords[1] = 0;
    sSeaMenuInitializerWords.mWords[0] = 0;
    seaMenuRegisterGlobal(&sSeaMenuFirstRegistration);
    f32 minimum = sSeaMenuMinimumAngle, maximum = sSeaMenuMaximumAngle;
    sSeaMenuInitializerLimits.mMinimumAngle = minimum;
    sSeaMenuInitializerLimits.mMaximumAngle = maximum;
    seaMenuConstructFirstObject(&sSeaMenuFirstObject);
    seaMenuRegisterGlobal(&sSeaMenuSecondRegistration);
    seaMenuConstructSecondObject(&sSeaMenuSecondObject);
    seaMenuRegisterGlobal(&sSeaMenuThirdRegistration);
    f32 extent = sSeaMenuExtent;
    sSeaMenuInitializerLimits.mHeight = extent;
    f32 limit = sSeaMenuLimit;
    sSeaMenuInitializerLimits.mLimitX = limit;
    sSeaMenuInitializerLimits.mLimitY = limit;
    sSeaMenuInitializerLimits.mWidth = extent;
    seaMenuInitializeName(&sSeaMenuMainName, "B_Area_00");
    seaMenuInitializeName(&sSeaMenuTextName, "L_LinkPos_00");
    seaMenuInitializeName(&sSeaMenuTextType, "N_Rotate_00");
    seaMenuInitializeName(&sSeaMenuMarkerPrefix, "P_TreasureGet_");
    seaMenuInitializeName(&sSeaMenuPaneNames.mNames[0], "N_MapMark_03");
    seaMenuInitializeName(&sSeaMenuPaneNames.mNames[1], "N_MapMark_12");
    seaMenuInitializeName(&sSeaMenuPaneNames.mNames[2], "N_MapMark_17");
    seaMenuInitializeName(&sSeaMenuPaneNames.mNames[3], "N_MapMark_22");
    seaMenuInitializeName(&sSeaMenuPaneNames.mNames[4], "N_MapMark_26");
    seaMenuInitializeName(&sSeaMenuPaneNames.mNames[5], "N_MapMark_31");
    seaMenuInitializeName(&sSeaMenuPaneNames.mNames[6], "N_MapMark_40");
    seaMenuInitializeName(&sSeaMenuPaneNames.mNames[7], "N_MapMark_44");
    seaMenuInitializeName(&sSeaMenuInfoNames.mNames[0], "L_BtnMapBalloon_00");
    seaMenuInitializeName(&sSeaMenuInfoNames.mNames[1], "L_MapAreaNow_00");
    seaMenuInitializeName(&sSeaMenuInfoNames.mNames[2], "L_LinkPos_00");
    seaMenuInitializeName(&sSeaMenuAuxiliaryNames.mNames[0], "BtnMapBalloon_00");
    seaMenuInitializeName(&sSeaMenuAuxiliaryNames.mNames[1], "MapAreaNow_00");
    seaMenuInitializeName(&sSeaMenuAuxiliaryNames.mNames[2], "LinkPos_00");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[0], "G_All_00");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[1], "G_MapTouch_00");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[2], "G_MapTouch_01");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[3], "G_MapTouch_02");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[4], "G_MapTouch_03");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[5], "G_MapTouch_04");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[6], "G_MapTouch_05");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[7], "G_MapTouch_06");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[8], "G_MapTouch_07");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[9], "G_MapTouch_08");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[10], "G_MapTouch_09");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[11], "G_MapTouch_10");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[12], "G_MapTouch_11");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[13], "G_MapTouch_12");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[14], "G_MapTouch_13");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[15], "G_MapTouch_14");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[16], "G_MapTouch_15");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[17], "G_MapTouch_16");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[18], "G_MapTouch_17");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[19], "G_MapTouch_18");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[20], "G_MapTouch_19");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[21], "G_MapTouch_20");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[22], "G_MapTouch_21");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[23], "G_MapTouch_22");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[24], "G_MapTouch_23");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[25], "G_MapTouch_24");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[26], "G_MapTouch_25");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[27], "G_MapTouch_26");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[28], "G_MapTouch_27");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[29], "G_MapTouch_28");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[30], "G_MapTouch_29");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[31], "G_MapTouch_30");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[32], "G_MapTouch_31");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[33], "G_MapTouch_32");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[34], "G_MapTouch_33");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[35], "G_MapTouch_34");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[36], "G_MapTouch_35");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[37], "G_MapTouch_36");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[38], "G_MapTouch_37");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[39], "G_MapTouch_38");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[40], "G_MapTouch_39");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[41], "G_MapTouch_40");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[42], "G_MapTouch_41");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[43], "G_MapTouch_42");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[44], "G_MapTouch_43");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[45], "G_MapTouch_44");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[46], "G_MapTouch_45");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[47], "G_MapTouch_46");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[48], "G_MapTouch_47");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[49], "G_MapTouch_48");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[50], "G_TreasureGet_00");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[51], "G_TreasureGet_01");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[52], "G_TreasureGet_02");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[53], "G_TreasureGet_03");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[54], "G_TreasureGet_04");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[55], "G_TreasureGet_05");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[56], "G_TreasureGet_06");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[57], "G_TreasureGet_07");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[58], "G_TreasureGet_08");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[59], "G_TreasureGet_09");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[60], "G_TreasureGet_10");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[61], "G_TreasureGet_11");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[62], "G_TreasureGet_12");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[63], "G_TreasureGet_13");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[64], "G_TreasureGet_14");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[65], "G_TreasureGet_15");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[66], "G_TreasureGet_16");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[67], "G_TreasureGet_17");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[68], "G_TreasureGet_18");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[69], "G_TreasureGet_19");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[70], "G_TreasureGet_20");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[71], "G_TreasureGet_21");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[72], "G_TreasureGet_22");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[73], "G_TreasureGet_23");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[74], "G_TreasureGet_24");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[75], "G_TreasureGet_25");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[76], "G_TreasureGet_26");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[77], "G_TreasureGet_27");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[78], "G_TreasureGet_28");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[79], "G_TreasureGet_29");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[80], "G_TreasureGet_30");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[81], "G_TreasureGet_31");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[82], "G_TreasureGet_32");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[83], "G_TreasureGet_33");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[84], "G_TreasureGet_34");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[85], "G_TreasureGet_35");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[86], "G_TreasureGet_36");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[87], "G_TreasureGet_37");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[88], "G_TreasureGet_38");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[89], "G_TreasureGet_39");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[90], "G_TreasureGet_40");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[91], "G_TreasureGet_41");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[92], "G_TreasureGet_42");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[93], "G_TreasureGet_43");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[94], "G_TreasureGet_44");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[95], "G_TreasureGet_45");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[96], "G_TreasureGet_46");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[97], "G_TreasureGet_47");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[98], "G_TreasureGet_48");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[99], "G_Korog_00");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[100], "G_Korog_01");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[101], "G_Korog_02");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[102], "G_Korog_03");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[103], "G_Korog_04");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[104], "G_Korog_05");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[105], "G_Korog_06");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[106], "G_Korog_07");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[107], "G_MapMark_00");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[108], "G_OpenDemo_00");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[109], "G_OpenDemo_01");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[110], "G_OpenDemo_02");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[111], "G_OpenDemo_03");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[112], "G_OpenDemo_04");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[113], "G_OpenDemo_05");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[114], "G_OpenDemo_06");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[115], "G_OpenDemo_07");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[116], "G_OpenDemo_08");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[117], "G_OpenDemo_09");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[118], "G_OpenDemo_10");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[119], "G_OpenDemo_11");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[120], "G_OpenDemo_12");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[121], "G_OpenDemo_13");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[122], "G_OpenDemo_14");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[123], "G_OpenDemo_15");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[124], "G_OpenDemo_16");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[125], "G_OpenDemo_17");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[126], "G_OpenDemo_18");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[127], "G_OpenDemo_19");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[128], "G_OpenDemo_20");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[129], "G_OpenDemo_21");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[130], "G_OpenDemo_22");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[131], "G_OpenDemo_23");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[132], "G_OpenDemo_24");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[133], "G_OpenDemo_25");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[134], "G_OpenDemo_26");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[135], "G_OpenDemo_27");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[136], "G_OpenDemo_28");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[137], "G_OpenDemo_29");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[138], "G_OpenDemo_30");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[139], "G_OpenDemo_31");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[140], "G_OpenDemo_32");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[141], "G_OpenDemo_33");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[142], "G_OpenDemo_34");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[143], "G_OpenDemo_35");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[144], "G_OpenDemo_36");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[145], "G_OpenDemo_37");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[146], "G_OpenDemo_38");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[147], "G_OpenDemo_39");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[148], "G_OpenDemo_40");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[149], "G_OpenDemo_41");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[150], "G_OpenDemo_42");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[151], "G_OpenDemo_43");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[152], "G_OpenDemo_44");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[153], "G_OpenDemo_45");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[154], "G_OpenDemo_46");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[155], "G_OpenDemo_47");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[156], "G_OpenDemo_48");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[157], "G_Island_00");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[158], "G_Island_03");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[159], "G_Island_10");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[160], "G_Island_12");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[161], "G_Island_19");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[162], "G_Island_22");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[163], "G_Island_25");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[164], "G_Island_39");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[165], "G_Island_40");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[166], "G_Island_43");
    seaMenuInitializeName(&sSeaMenuAnimationGroups.mNames[167], "G_Island_44");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[0], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[1], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[2], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[3], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[4], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[5], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[6], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[7], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[8], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[9], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[10], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[11], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[12], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[13], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[14], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[15], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[16], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[17], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[18], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[19], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[20], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[21], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[22], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[23], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[24], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[25], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[26], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[27], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[28], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[29], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[30], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[31], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[32], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[33], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[34], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[35], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[36], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[37], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[38], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[39], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[40], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[41], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[42], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[43], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[44], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[45], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[46], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[47], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[48], "MapTouch");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[49], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[50], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[51], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[52], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[53], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[54], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[55], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[56], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[57], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[58], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[59], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[60], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[61], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[62], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[63], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[64], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[65], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[66], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[67], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[68], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[69], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[70], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[71], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[72], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[73], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[74], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[75], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[76], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[77], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[78], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[79], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[80], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[81], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[82], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[83], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[84], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[85], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[86], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[87], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[88], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[89], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[90], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[91], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[92], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[93], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[94], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[95], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[96], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[97], "TreasureGet");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[98], "Korog");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[99], "Korog");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[100], "Korog");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[101], "Korog");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[102], "Korog");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[103], "Korog");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[104], "Korog");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[105], "Korog");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[106], "MapMarkLoop");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[107], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[108], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[109], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[110], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[111], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[112], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[113], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[114], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[115], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[116], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[117], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[118], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[119], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[120], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[121], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[122], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[123], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[124], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[125], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[126], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[127], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[128], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[129], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[130], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[131], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[132], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[133], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[134], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[135], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[136], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[137], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[138], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[139], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[140], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[141], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[142], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[143], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[144], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[145], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[146], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[147], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[148], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[149], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[150], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[151], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[152], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[153], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[154], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[155], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[156], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[157], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[158], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[159], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[160], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[161], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[162], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[163], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[164], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[165], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuAnimationNames.mNames[166], "OpenDemo");
    seaMenuInitializeName(&sSeaMenuLayoutName, "Map_00");
    sSeaMenuCoordinates.mValues[0].x = -2;
    sSeaMenuCoordinates.mValues[0].y = -2;
    sSeaMenuCoordinates.mValues[1].x = 0;
    sSeaMenuCoordinates.mValues[1].y = -2;
    sSeaMenuCoordinates.mValues[2].x = 2;
    sSeaMenuCoordinates.mValues[2].y = -2;
    sSeaMenuCoordinates.mValues[3].x = -2;
    sSeaMenuCoordinates.mValues[3].y = 0;
    sSeaMenuCoordinates.mValues[4].x = 1;
    sSeaMenuCoordinates.mValues[4].y = 0;
    sSeaMenuCoordinates.mValues[5].x = 2;
    sSeaMenuCoordinates.mValues[5].y = 2;
    sSeaMenuCoordinates.mValues[6].x = -2;
    sSeaMenuCoordinates.mValues[6].y = 3;
    sSeaMenuCoordinates.mValues[7].x = -1;
    sSeaMenuCoordinates.mValues[7].y = -1;
    sSeaMenuCoordinates.mValues[8].x = 0;
    sSeaMenuCoordinates.mValues[8].y = 2;
}
struct SeaMenuMapChangeFlags_l {
    u8 mReserved[0x138]; be<u8> mPositionChanged; u8 mReserved139[2];
    be<u8> mSelectionChanged, mModeChanged;
};
struct SeaMenuTileName_l { u8 mReserved[0x24]; hdUiSafeString mName; };
WWHD_OFFSET(SeaMenuMapChangeFlags_l, mPositionChanged, 0x138);
WWHD_OFFSET(SeaMenuMapChangeFlags_l, mSelectionChanged, 0x13B);
WWHD_OFFSET(SeaMenuMapChangeFlags_l, mModeChanged, 0x13C);
WWHD_OFFSET(SeaMenuTileName_l, mName, 0x24);
static hdPartsStateId_c sSeaMenuButtonConfirmState;
static hdPartsStateId_c sSeaMenuButtonTouchState;
extern void seaMenuSetButtonBusy(hdParts_c*);
extern void seaMenuSetButtonName(hdParts_c*, hdUiSafeString*);
extern void seaMenuPrepareButton(hdParts_c*);
extern void seaMenuOpenButton(hdParts_c*);
extern void seaMenuExecuteMapButton(hdParts_c*);
static bool seaMenuMatchesButtonState(hdParts_c* button, hdPartsStateId_c* expected) {
    auto* current = seaMenuCurrentState(button->stateMachine());
    auto* expectedVtable = expected->mVtable.get();
    u32 actualKey = current->getKey();
    u32 expectedKey = call_ptr<u32>(expectedVtable->mGetKey.get(), expected);
    return actualKey == expectedKey;
}
/* 0267852C */
void hdSeaChartMenu_c::updateOpeningButton() {
    if (!mpActiveButton || !mpMap) return;
    if (hdPadUiBusy(l_hdMapUi.mOwner->mUi.get())) {
        seaMenuSetButtonBusy(mpActiveButton.get());
        return;
    }
    auto* map = mpMap.get();
    auto* flags = reinterpret_cast<SeaMenuMapChangeFlags_l*>(map);
    if (!flags->mPositionChanged && !flags->mModeChanged && !flags->mSelectionChanged) return;
    auto* tile = reinterpret_cast<SeaMenuMapStatus_l*>(map)->mpSelectedTile.get();
    if (!tile) {
        if (!seaMenuMatchesButtonState(mpActiveButton.get(), &sSeaMenuBlankState))
            seaMenuInitializeAlternate(mpActiveButton.get());
        return;
    }
    seaMenuSetButtonName(mpActiveButton.get(), &reinterpret_cast<SeaMenuTileName_l*>(tile)->mName);
    seaMenuPrepareButton(mpActiveButton.get());
    bool blank = seaMenuMatchesButtonState(mpActiveButton.get(), &sSeaMenuBlankState);
    auto* button = mpActiveButton.get();
    if (blank) { seaMenuOpenButton(button); return; }
    if (seaMenuMatchesButtonState(button, &sSeaMenuButtonConfirmState) ||
        seaMenuMatchesButtonState(mpActiveButton.get(), &sSeaMenuButtonTouchState))
        seaMenuExecuteMapButton(mpActiveButton.get());
}
struct SeaMenuPaneTransform_l {
    u8 mReserved[0x1C]; HdPadPoint3 mPosition, mRotation;
    be<f32> mScaleX, mScaleY; HdPadPoint mSize; be<u8> mFlags;
};
struct SeaMenuUiMotion_l {
    u8 mReserved[0x30]; HdPadPoint mPosition; be<f32> mAngle;
    u8 mReserved3C[0xC]; be<f32> mAlternateAngle; u8 mReserved4C[2]; be<u8> mVisible;
};
struct SeaMenuAngleMode_l { u8 mReserved[0xA0]; be<u8> mUsePrimaryAngle; };
struct SeaMenuAngleOwner_l { u8 mReserved[0x1C4]; gptr<SeaMenuAngleMode_l> mpMode; };
WWHD_OFFSET(SeaMenuPaneTransform_l, mSize, 0x3C);
WWHD_OFFSET(SeaMenuPaneTransform_l, mFlags, 0x44);
WWHD_OFFSET(SeaMenuUiMotion_l, mAlternateAngle, 0x48);
WWHD_OFFSET(SeaMenuUiMotion_l, mVisible, 0x4E);
static SeaMenuFloat_l sSeaMenuGridSize;
static SeaMenuFloat_l sSeaMenuCoordinateScale;
extern u32 seaMenuUiHasPointer(HdPadUi*);
/* 02678878 */
void hdSeaChartMenu_c::updateMarkerPanePosition() {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x28> frame;
    auto* marker = reinterpret_cast<SeaMenuPaneTransform_l*>(mpInfoPanes[2].get());
    auto* text = reinterpret_cast<SeaMenuPaneTransform_l*>(mpTextPane.get());
    auto* main = reinterpret_cast<SeaMenuPaneTransform_l*>(mpMainPane.get());
    if (!marker || !main || !text) return;
    f32 width = main->mSize.x;
    f32 grid = sSeaMenuGridSize;
    f32 cellWidth = f32(width / grid);
    auto* motion = reinterpret_cast<SeaMenuUiMotion_l*>(l_hdMapUi.mOwner->mUi.get());
    f32 height = main->mSize.y;
    auto* position = at<HdPadPoint>(frame + 8);
    reinterpret_cast<be<u32>*>(position)[0] = u32(reinterpret_cast<be<u32>*>(&motion->mPosition)[0]);
    reinterpret_cast<be<u32>*>(position)[1] = u32(reinterpret_cast<be<u32>*>(&motion->mPosition)[1]);
    f32 inputY = position->y, inputX = position->x;
    f32 cellHeight = f32(height / grid);
    f32 zero = sSeaMenuZero;
    f32 xProduct = fmuls_ppc(inputX, cellWidth);
    f32 scale = sSeaMenuCoordinateScale;
    f32 yProduct = fmuls_ppc(inputY, cellHeight);
    f32 xOffset = f32(xProduct / scale);
    f32 mainY = main->mPosition.y, mainX = main->mPosition.x;
    marker->mPosition.z = zero;
    u8 flags = marker->mFlags;
    f32 x = fadds_ppc(mainX, xOffset);
    marker->mFlags = flags | 0x10;
    f32 yOffset = f32(yProduct / scale);
    f32 y = fsubs_ppc(mainY, yOffset);
    marker->mPosition.x = x;
    marker->mPosition.y = y;
    auto* rotation = at<HdPadPoint3>(frame + 0x10);
    reinterpret_cast<be<u32>*>(rotation)[0] = u32(reinterpret_cast<be<u32>*>(&text->mRotation)[0]);
    reinterpret_cast<be<u32>*>(rotation)[1] = u32(reinterpret_cast<be<u32>*>(&text->mRotation)[1]);
    reinterpret_cast<be<u32>*>(rotation)[2] = u32(reinterpret_cast<be<u32>*>(&text->mRotation)[2]);
    bool alternate = false;
    if (dComIfGp_checkPlayerStatus0(0, 0x10000)) {
        auto* owner = reinterpret_cast<SeaMenuAngleOwner_l*>(l_hdMapUi.mOwner.get());
        if (!owner->mpMode->mUsePrimaryAngle)
            alternate = !seaMenuUiHasPointer(l_hdMapUi.mOwner->mUi.get());
    }
    motion = reinterpret_cast<SeaMenuUiMotion_l*>(l_hdMapUi.mOwner->mUi.get());
    if (alternate) {
        flags = text->mFlags;
        f32 savedX = rotation->x;
        f32 angle = motion->mAlternateAngle;
        text->mRotation.x = savedX;
        f32 savedY = rotation->y;
        text->mRotation.z = angle;
        text->mRotation.y = savedY;
    } else {
        f32 angle = motion->mAngle;
        f32 savedX = rotation->x;
        flags = text->mFlags;
        text->mRotation.x = savedX;
        f32 savedY = rotation->y;
        text->mRotation.z = angle;
        text->mRotation.y = savedY;
    }
    text->mFlags = flags | 0x10;
    auto* ui = l_hdMapUi.mOwner->mUi.get();
    motion = reinterpret_cast<SeaMenuUiMotion_l*>(ui);
    bool visible = motion->mVisible;
    if (visible) visible = !hdPadUiBusy(ui);
    marker->mFlags = visible ? u8(marker->mFlags) | 1 : u8(marker->mFlags) & 0xFE;
}
static SeaMenuFloat_l sSeaMenuCollectedFrame;
/* 02678F70 */
void hdSeaChartMenu_c::updateChartIconFrames() {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x80> frame;
    if (hdPadUiBusy(l_hdMapUi.mOwner->mUi.get())) { hideMenuPanes(); return; }
    auto* order = at<be<u32>>(frame + 8);
    order[0] = 2;
    order[2] = 0;
    order[5] = 1;
    order[4] = 5;
    order[6] = 6;
    order[3] = 7;
    order[1] = 4;
    order[7] = 3;
    if (dComIfGs_isEventBit(0x102)) {
        for (u32 i = 0; i < 8; ++i) hdAnimSet_operationCC(animSet(), 0x62 + i, 0x63 + i);
        return;
    }
    if (!dComIfGs_isEventBit(0x3940)) return;
    f32 collected = sSeaMenuCollectedFrame;
    f32 available = sSeaMenuAnimationRate;
    f32 missing = sSeaMenuZero;
    for (u32 i = 0; i < 8; ++i) {
        u32 index = order[i];
        u32 group = index + 0x63, animation = index + 0x62;
        if (!dComIfGs_checkBottle(0x59)) {
            hdAnimSet_setFrame(animSet(), animation, group, missing);
            continue;
        }
        bool complete = true;
        if (!dComIfGs_isEventBit(0x102)) {
            u32 bits = dComIfGs_getEventRegHD(0x9EFF);
            complete = ((s32(bits) >> (i & 7)) & 1) != 0;
        }
        hdAnimSet_setFrame(animSet(), animation, group, complete ? collected : available);
    }
}
static void seaMenuSetPaneVisible(hdPartsPaneSRT_c* pane, bool visible) {
    if (pane) pane->mFlags = (u8(pane->mFlags) & 0xFE) + u32(visible);
}
/* 02678B74 */
void hdSeaChartMenu_c::updateProgressIcons() {
    if (hdPadUiBusy(l_hdMapUi.mOwner->mUi.get())) { hideMenuPanes(); return; }
    if (!dComIfGs_isEventBit(0x908)) return;
    if (dComIfGs_isEventBit(0xF80)) seaMenuSetPaneVisible(mpMenuPanes[1].get(), true);
    if (dComIfGs_isSymbol(1)) {
        seaMenuSetPaneVisible(mpMenuPanes[1].get(), false);
        seaMenuSetPaneVisible(mpMenuPanes[6].get(), true);
    }
    if (dComIfGs_isSymbol(2)) {
        seaMenuSetPaneVisible(mpMenuPanes[6].get(), false);
        seaMenuSetPaneVisible(mpMenuPanes[3].get(), true);
    }
    if (dComIfGs_isEventBit(0x1E80)) seaMenuSetPaneVisible(mpMenuPanes[3].get(), false);
    if (dComIfGs_isEventBit(0x3920)) {
        bool hidden = dComIfGs_isEventBit(0x1480);
        seaMenuSetPaneVisible(mpMenuPanes[2].get(), !hidden);
        hidden = dComIfGs_isEventBit(0x1440);
        seaMenuSetPaneVisible(mpMenuPanes[4].get(), !hidden);
        hidden = dComIfGs_isEventBit(0x1410);
        seaMenuSetPaneVisible(mpMenuPanes[5].get(), !hidden);
    }
    if (!dComIfGs_isEventBit(0x1820)) return;
    seaMenuSetPaneVisible(mpMenuPanes[0].get(), true);
    seaMenuSetPaneVisible(mpMenuPanes[7].get(), true);
    if (dComIfGs_isStageBossEnemy(6)) seaMenuSetPaneVisible(mpMenuPanes[7].get(), false);
    if (dComIfGs_isStageBossEnemy(7)) seaMenuSetPaneVisible(mpMenuPanes[0].get(), false);
}

static hdUiSafeString sSeaMenuMapButtonClass;
static hdUiSafeString sSeaMenuLoopClass;
static hdUiSafeString sSeaMenuNoControlClass;
extern hdParts_c* seaMenuConstructMapButton(hdParts_c*);
extern u32 seaMenuInitializeMapButton(hdParts_c*, u32, u32, hdPartsDesc_c*);
extern u32 seaMenuCreateDefaultChild(hdParts_c*, u32, hdPartsDesc_c*);
// Match the game's inline comparison, retaining its callback-visible reloads.
static bool seaMenuMatchesClass(hdUiSafeString* actual, hdUiSafeString* expected, bool firstAssured = false) {
    if (!firstAssured) actual->assureTermination();
    actual->assureTermination();
    auto* snapshot = actual->mText.get();
    expected->assureTermination();
    if (snapshot == expected->mText.get()) return true;
    auto* left = actual->mText.get();
    auto* right = expected->mText.get();
    constexpr u32 comparisonLimit = 0x40001;
    for (u32 i = 0; i < comparisonLimit; ++i) {
        u8 character = left[i];
        if (character != u8(right[i])) return false;
        if (!character) return true;
    }
    return false;
}
/* 0267952C */
u32 hdSeaChartMenu_c::createMenuChild(hdPartsDesc_c* descriptor) {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x40> frame;
    auto* resource = reinterpret_cast<hdUiSafeString*>(descriptor->mResource);
    auto* type = reinterpret_cast<hdUiSafeString*>(descriptor->mResource + 0xC);
    auto* className = at<hdUiSafeString>(frame + 8);
    u32 classText = descriptor->mClassName.v;
    auto* heap = sSeaMenuUi->mHeap.get();
    className->mVtable = &sSeaMenuStringVtable;
    className->mText.v = classText;
    // The first virtual callback retains the string-table address in incoming r4.
    call_ptr(type->mVtable->mAssureTermination.get(), type, &sSeaMenuStringVtable);
    for (u32 index = 0; index < 3; ++index) {
        if (!seaMenuMatchesClass(type, &sSeaMenuInfoNames.mNames[index], index == 0)) continue;
        if (seaMenuMatchesClass(resource, &sSeaMenuMapButtonClass)) {
            auto* slot = childSlot(index);
            auto* part = static_cast<hdParts_c*>(hdParts_new(0x168, heap, 4));
            if (part) part = seaMenuConstructMapButton(part);
            *slot = part;
            if (!child(index)) return 0;
            return seaMenuInitializeMapButton(child(index), 0, 0, descriptor);
        }
        if (seaMenuMatchesClass(className, &sSeaMenuLoopClass)) {
            auto* slot = childSlot(index);
            auto* part = static_cast<hdParts_c*>(hdParts_new(0x54, heap, 4));
            if (part) part = hdPartsLoop_construct(part);
            *slot = part;
            if (!child(index)) return 0;
            return hdPartsLoop_init(child(index), resource, 0, 0, descriptor);
        }
        if (seaMenuMatchesClass(className, &sSeaMenuNoControlClass)) {
            auto* slot = childSlot(index);
            auto* part = static_cast<hdParts_c*>(hdParts_new(0x50, heap, 4));
            if (part) part = hdPartsNoControl_construct(part);
            *slot = part;
            if (!child(index)) return 0;
            return hdPartsNoControl_init(child(index), resource, 0, 0, descriptor);
        }
        return seaMenuCreateDefaultChild(this, index, descriptor);
    }
    return 1;
}
/* WWHD glue (tools/verify/autoglue.py)
data sSeaMenuUi 1018C404
ext 026CA9F4 hdParts_c* seaChartConstructRupy(hdParts_c*)
ext 026CAAD4 void seaChartInitializeRupy(hdParts_c*, u32, u32, u32)
ext 0266D9F0 hdParts_c* seaChartConstructHeart(hdParts_c*)
ext 0266DB04 void seaChartInitializeHeart(hdParts_c*, u32, u32, u32)
ext 0267516C hdParts_c* seaChartConstructBarDisplay(hdParts_c*)
ext 02675294 void seaChartInitializeBarDisplay(hdParts_c*, u32, u32, u32)
ext 0264CC50 hdParts_c* seaChartConstructCompassClock(hdParts_c*)
ext 0264CD98 void seaChartInitializeCompassClock(hdParts_c*, u32, u32, u32)
ext 02653FB8 hdParts_c* seaChartConstructDungeonKey(hdParts_c*)
ext 02654104 void seaChartInitializeDungeonKey(hdParts_c*, u32, u32, u32)
ext 026E6E04 hdParts_c* seaChartConstructSwimTime(hdParts_c*)
ext 026E6F48 void seaChartInitializeSwimTime(hdParts_c*, u32, u32, u32)
ext 026DDA04 hdParts_c* seaChartConstructShortcut(hdParts_c*)
ext 026DDA9C void seaChartInitializeShortcut(hdParts_c*, u32, u32, u32)
ext 026297A8 hdParts_c* seaChartConstructBossHp(hdParts_c*)
ext 02629814 void seaChartInitializeBossHp(hdParts_c*, u32, u32, u32)
ext 0266855C hdParts_c* seaChartConstructEnemyHp(hdParts_c*)
ext 026685D0 void seaChartInitializeEnemyHp(hdParts_c*, u32, u32, u32)
ext 026E9E0C hdParts_c* seaChartConstructTime(hdParts_c*)
ext 026E9E8C void seaChartInitializeTime(hdParts_c*, u32, u32, u32)
ext 0261B3C4 hdParts_c* seaChartConstructArrow(hdParts_c*)
ext 0261B4E4 void seaChartInitializeArrow(hdParts_c*, u32, u32, u32)
ext 026232AC hdParts_c* seaChartConstructBattery(hdParts_c*)
ext 02623320 void seaChartInitializeBattery(hdParts_c*, u32, u32, u32)
ext 026CB5B0 hdParts_c* seaChartConstructRupySwordCounter(hdParts_c*)
ext 026CB628 void seaChartInitializeRupySwordCounter(hdParts_c*, u32, u32, u32)
data sSeaMenuVtable 100F8B28
data sSeaMenuHolderVtable 100F8BF0
data sSeaMenuSecondaryVtable 100F8C00
ext 0273AD10 void* seaMenuAllocate(u32)
ext 0273AF40 void seaMenuDelete(void*)
ext 02700ADC NoControlPart_l* NoControlPart_l::construct(NoControlPart_l*)
ext 02700A70 void NoControlPart_l::destroy(u32)
data sSeaMenuResourceIds 100F7BAC
ext 0268081C hdParts_c* seaMenuConstructMap(hdParts_c*)
ext 02680D90 void seaMenuSetMapMode(hdParts_c*, u32)
ext 0268354C void seaMenuOpenMap(hdParts_c*)
ext 02683760 void seaMenuCloseMap(hdParts_c*)
ext 02005674 void seaMenuStopAnimation(hdAnimSet*, u32, u32)
ext 02680A68 void seaMenuReleaseMap(hdParts_c*)
ext 026F8B00 void seaMenuReleaseBaseChildren(hdSeaChartMenu_c*)
ext 02678A54 void seaMenuUpdateSelectionPosition(hdSeaChartMenu_c*)
ext 02678B74 void seaMenuRefreshSelection(hdSeaChartMenu_c*)
ext 02678F70 void seaMenuResetIcons(hdSeaChartMenu_c*)
data sSeaMenuPaneNames 10495410
data sSeaMenuInfoNames 10495450
data sSeaMenuMainName 104953C4
ext 02616ED4 hdSeaMenuSelected_l* seaMenuConstructSelected(hdSeaMenuSelected_l*, hdPartsPaneSRT_c*, u32)
ext 026809DC void seaMenuCreateMap(hdParts_c*)
ext 02680BAC void seaMenuFinishMap(hdParts_c*)
data sSeaMenuTextName 104953CC
data sSeaMenuTextType 104953D4
data sSeaMenuLayoutName 10495F0C
data sSeaMenuAnimationRate 100F7E70
ext 02704FC0 hdPartsPaneSRT_c* seaMenuFindTextPane(hdPartsLayout_c*, hdUiSafeString*, hdUiSafeString*)
ext 02700B48 u32 NoControlPart_l::create(void*,void*,u32,u32)
ext 02682AA8 void seaMenuUpdateMap(hdParts_c*)
ext 02678878 void seaMenuUpdateMarkerPosition(hdSeaChartMenu_c*)
data sSeaMenuMarkerPrefix 104953DC
data sSeaMenuMarkerStringVtable 100F7B94
ext 02759C28 void seaMenuFormatMarker(hdScreenFixedString*, const char*, be<u8>*, u32)
str 100F7E74 "%s%02d" in 02677F64
data sSeaMenuBlankState 1049DD60 nocheck
data sSeaMenuEventMgr 101F4FF0
ext 02006478 hdPartsStateId_c* seaMenuCurrentState(hdStateMachine*)
ext 026150C0 void seaMenuAppendReceiver(cking::ui::input::ReceiverList*, cking::ui::input::ReceiverNode*)
ext 02615398 u32 seaMenuReceiverId(cking::ui::input::EventMgr*)
ext 0268936C u32 hdPadUiBusy(HdPadUi*)
data sSeaMenuCoordinates 10495480
ext 02680DE0 SeaMenuMapTile_l* seaMenuMapTile(hdParts_c*, HdPadCoordinate*)
ext 026DBC44 void seaMenuSetMapSelection(void*, u32)
ext 020013C0 void seaMenuReadEvent(SeaMenuQueuedEvent_l*, SeaMenuQueuedEvent_l*)
ext 0263A438 void seaMenuConfirmSelection(hdParts_c*)
ext 0263A520 void seaMenuConfirmAlternate(hdParts_c*)
ext 02680364 void seaMenuCloseTile(SeaMenuMapTile_l*)
ext 02030B38 void seaMenuPlaySound(const char*)
str 100F7EA0 "SE_MENU_DECIDE" in 02679ED4
str 100F7E8C "SE_MENU_CHART_CLOSE" in 02679ED4
data sSeaMenuZero 100F7E84
data sSeaMenuCompass 101F5A44
data sSeaMenuRouteSettings 101F8378
ext 026538FC void seaMenuSetCompassStart(SeaMenuCompass_l*, HdPadPoint*)
ext 02653904 void seaMenuSetCompassTarget(SeaMenuCompass_l*, HdPadPoint*)
data sSeaMenuReceiverIndex 100F7B44
data sSeaMenuForwardKind 100F7B48
ext 026F5B74 u32 seaMenuButtonId(hdParts_c*)
ext 026F61DC u32 seaMenuIsButtonEvent(u32)
ext 026832C4 void seaMenuUpdateTouchMap(hdParts_c*)
ext 02001338 void seaMenuCreateEvent(SeaMenuQueuedEvent_l*, u32, u32, u32)
ext 02001FF4 void seaMenuQueueEvent(u8*, SeaMenuQueuedEvent_l*)
ext 02001490 void seaMenuDestroyEvent(SeaMenuQueuedEvent_l*)
ext 0263A39C void seaMenuInitializeAlternate(hdParts_c*)
ext 0268034C void seaMenuConfirmRoute(SeaMenuRoute_l*)
str 100F7EC4 "SE_MENU_DECIDE" in 0267A034
str 100F7EB0 "SE_MENU_CHART_CLOSE" in 0267A034
ext 0267852C void seaMenuUpdateOpening(hdSeaChartMenu_c*)
data sSeaMenuAnimationNames 104959D4
data sSeaMenuAnimationGroups 10495494
data sSeaMenuAnimationRemap 100F7BD0
ext 0200506C void seaMenuBindAnimationRange(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*, u32)
str 100F7FAC "B_Area_00" in 0267A29C
str 100F8668 "L_LinkPos_00" in 0267A29C
str 100F7F04 "N_Rotate_00" in 0267A29C
str 100F8678 "P_TreasureGet_" in 0267A29C
str 100F85E8 "N_MapMark_03" in 0267A29C
str 100F85F8 "N_MapMark_12" in 0267A29C
str 100F8608 "N_MapMark_17" in 0267A29C
str 100F8618 "N_MapMark_22" in 0267A29C
str 100F8628 "N_MapMark_26" in 0267A29C
str 100F8638 "N_MapMark_31" in 0267A29C
str 100F8648 "N_MapMark_40" in 0267A29C
str 100F8658 "N_MapMark_44" in 0267A29C
str 100F8AF0 "L_BtnMapBalloon_00" in 0267A29C
str 100F7EF4 "L_MapAreaNow_00" in 0267A29C
str 100F8B04 "BtnMapBalloon_00" in 0267A29C
str 100F85D8 "MapAreaNow_00" in 0267A29C
str 100F8B18 "LinkPos_00" in 0267A29C
str 100F8688 "G_All_00" in 0267A29C
str 100F7FB8 "G_MapTouch_00" in 0267A29C
str 100F7FC8 "G_MapTouch_01" in 0267A29C
str 100F7FD8 "G_MapTouch_02" in 0267A29C
str 100F7FE8 "G_MapTouch_03" in 0267A29C
str 100F7FF8 "G_MapTouch_04" in 0267A29C
str 100F8008 "G_MapTouch_05" in 0267A29C
str 100F8018 "G_MapTouch_06" in 0267A29C
str 100F8028 "G_MapTouch_07" in 0267A29C
str 100F8038 "G_MapTouch_08" in 0267A29C
str 100F8048 "G_MapTouch_09" in 0267A29C
str 100F8058 "G_MapTouch_10" in 0267A29C
str 100F8068 "G_MapTouch_11" in 0267A29C
str 100F8078 "G_MapTouch_12" in 0267A29C
str 100F8088 "G_MapTouch_13" in 0267A29C
str 100F8098 "G_MapTouch_14" in 0267A29C
str 100F80A8 "G_MapTouch_15" in 0267A29C
str 100F80B8 "G_MapTouch_16" in 0267A29C
str 100F80C8 "G_MapTouch_17" in 0267A29C
str 100F80D8 "G_MapTouch_18" in 0267A29C
str 100F80E8 "G_MapTouch_19" in 0267A29C
str 100F80F8 "G_MapTouch_20" in 0267A29C
str 100F8108 "G_MapTouch_21" in 0267A29C
str 100F8118 "G_MapTouch_22" in 0267A29C
str 100F8128 "G_MapTouch_23" in 0267A29C
str 100F8138 "G_MapTouch_24" in 0267A29C
str 100F8148 "G_MapTouch_25" in 0267A29C
str 100F8158 "G_MapTouch_26" in 0267A29C
str 100F8168 "G_MapTouch_27" in 0267A29C
str 100F8178 "G_MapTouch_28" in 0267A29C
str 100F8188 "G_MapTouch_29" in 0267A29C
str 100F8198 "G_MapTouch_30" in 0267A29C
str 100F81A8 "G_MapTouch_31" in 0267A29C
str 100F81B8 "G_MapTouch_32" in 0267A29C
str 100F81C8 "G_MapTouch_33" in 0267A29C
str 100F81D8 "G_MapTouch_34" in 0267A29C
str 100F81E8 "G_MapTouch_35" in 0267A29C
str 100F81F8 "G_MapTouch_36" in 0267A29C
str 100F8208 "G_MapTouch_37" in 0267A29C
str 100F8218 "G_MapTouch_38" in 0267A29C
str 100F8228 "G_MapTouch_39" in 0267A29C
str 100F8238 "G_MapTouch_40" in 0267A29C
str 100F8248 "G_MapTouch_41" in 0267A29C
str 100F8258 "G_MapTouch_42" in 0267A29C
str 100F8268 "G_MapTouch_43" in 0267A29C
str 100F8278 "G_MapTouch_44" in 0267A29C
str 100F8288 "G_MapTouch_45" in 0267A29C
str 100F8298 "G_MapTouch_46" in 0267A29C
str 100F82A8 "G_MapTouch_47" in 0267A29C
str 100F82B8 "G_MapTouch_48" in 0267A29C
str 100F8694 "G_TreasureGet_00" in 0267A29C
str 100F86A8 "G_TreasureGet_01" in 0267A29C
str 100F86BC "G_TreasureGet_02" in 0267A29C
str 100F86D0 "G_TreasureGet_03" in 0267A29C
str 100F86E4 "G_TreasureGet_04" in 0267A29C
str 100F86F8 "G_TreasureGet_05" in 0267A29C
str 100F870C "G_TreasureGet_06" in 0267A29C
str 100F8720 "G_TreasureGet_07" in 0267A29C
str 100F8734 "G_TreasureGet_08" in 0267A29C
str 100F8748 "G_TreasureGet_09" in 0267A29C
str 100F875C "G_TreasureGet_10" in 0267A29C
str 100F8770 "G_TreasureGet_11" in 0267A29C
str 100F8784 "G_TreasureGet_12" in 0267A29C
str 100F8798 "G_TreasureGet_13" in 0267A29C
str 100F87AC "G_TreasureGet_14" in 0267A29C
str 100F87C0 "G_TreasureGet_15" in 0267A29C
str 100F87D4 "G_TreasureGet_16" in 0267A29C
str 100F87E8 "G_TreasureGet_17" in 0267A29C
str 100F87FC "G_TreasureGet_18" in 0267A29C
str 100F8810 "G_TreasureGet_19" in 0267A29C
str 100F8824 "G_TreasureGet_20" in 0267A29C
str 100F8838 "G_TreasureGet_21" in 0267A29C
str 100F884C "G_TreasureGet_22" in 0267A29C
str 100F8860 "G_TreasureGet_23" in 0267A29C
str 100F8874 "G_TreasureGet_24" in 0267A29C
str 100F8888 "G_TreasureGet_25" in 0267A29C
str 100F889C "G_TreasureGet_26" in 0267A29C
str 100F88B0 "G_TreasureGet_27" in 0267A29C
str 100F88C4 "G_TreasureGet_28" in 0267A29C
str 100F88D8 "G_TreasureGet_29" in 0267A29C
str 100F88EC "G_TreasureGet_30" in 0267A29C
str 100F8900 "G_TreasureGet_31" in 0267A29C
str 100F8914 "G_TreasureGet_32" in 0267A29C
str 100F8928 "G_TreasureGet_33" in 0267A29C
str 100F893C "G_TreasureGet_34" in 0267A29C
str 100F8950 "G_TreasureGet_35" in 0267A29C
str 100F8964 "G_TreasureGet_36" in 0267A29C
str 100F8978 "G_TreasureGet_37" in 0267A29C
str 100F898C "G_TreasureGet_38" in 0267A29C
str 100F89A0 "G_TreasureGet_39" in 0267A29C
str 100F89B4 "G_TreasureGet_40" in 0267A29C
str 100F89C8 "G_TreasureGet_41" in 0267A29C
str 100F89DC "G_TreasureGet_42" in 0267A29C
str 100F89F0 "G_TreasureGet_43" in 0267A29C
str 100F8A04 "G_TreasureGet_44" in 0267A29C
str 100F8A18 "G_TreasureGet_45" in 0267A29C
str 100F8A2C "G_TreasureGet_46" in 0267A29C
str 100F8A40 "G_TreasureGet_47" in 0267A29C
str 100F8A54 "G_TreasureGet_48" in 0267A29C
str 100F8A68 "G_Korog_00" in 0267A29C
str 100F8A74 "G_Korog_01" in 0267A29C
str 100F8A80 "G_Korog_02" in 0267A29C
str 100F8A8C "G_Korog_03" in 0267A29C
str 100F8A98 "G_Korog_04" in 0267A29C
str 100F8AA4 "G_Korog_05" in 0267A29C
str 100F8AB0 "G_Korog_06" in 0267A29C
str 100F8ABC "G_Korog_07" in 0267A29C
str 100F8AC8 "G_MapMark_00" in 0267A29C
str 100F82C8 "G_OpenDemo_00" in 0267A29C
str 100F82D8 "G_OpenDemo_01" in 0267A29C
str 100F82E8 "G_OpenDemo_02" in 0267A29C
str 100F82F8 "G_OpenDemo_03" in 0267A29C
str 100F8308 "G_OpenDemo_04" in 0267A29C
str 100F8318 "G_OpenDemo_05" in 0267A29C
str 100F8328 "G_OpenDemo_06" in 0267A29C
str 100F8338 "G_OpenDemo_07" in 0267A29C
str 100F8348 "G_OpenDemo_08" in 0267A29C
str 100F8358 "G_OpenDemo_09" in 0267A29C
str 100F8368 "G_OpenDemo_10" in 0267A29C
str 100F8378 "G_OpenDemo_11" in 0267A29C
str 100F8388 "G_OpenDemo_12" in 0267A29C
str 100F8398 "G_OpenDemo_13" in 0267A29C
str 100F83A8 "G_OpenDemo_14" in 0267A29C
str 100F83B8 "G_OpenDemo_15" in 0267A29C
str 100F83C8 "G_OpenDemo_16" in 0267A29C
str 100F83D8 "G_OpenDemo_17" in 0267A29C
str 100F83E8 "G_OpenDemo_18" in 0267A29C
str 100F83F8 "G_OpenDemo_19" in 0267A29C
str 100F8408 "G_OpenDemo_20" in 0267A29C
str 100F8418 "G_OpenDemo_21" in 0267A29C
str 100F8428 "G_OpenDemo_22" in 0267A29C
str 100F8438 "G_OpenDemo_23" in 0267A29C
str 100F8448 "G_OpenDemo_24" in 0267A29C
str 100F8458 "G_OpenDemo_25" in 0267A29C
str 100F8468 "G_OpenDemo_26" in 0267A29C
str 100F8478 "G_OpenDemo_27" in 0267A29C
str 100F8488 "G_OpenDemo_28" in 0267A29C
str 100F8498 "G_OpenDemo_29" in 0267A29C
str 100F84A8 "G_OpenDemo_30" in 0267A29C
str 100F84B8 "G_OpenDemo_31" in 0267A29C
str 100F84C8 "G_OpenDemo_32" in 0267A29C
str 100F84D8 "G_OpenDemo_33" in 0267A29C
str 100F84E8 "G_OpenDemo_34" in 0267A29C
str 100F84F8 "G_OpenDemo_35" in 0267A29C
str 100F8508 "G_OpenDemo_36" in 0267A29C
str 100F8518 "G_OpenDemo_37" in 0267A29C
str 100F8528 "G_OpenDemo_38" in 0267A29C
str 100F8538 "G_OpenDemo_39" in 0267A29C
str 100F8548 "G_OpenDemo_40" in 0267A29C
str 100F8558 "G_OpenDemo_41" in 0267A29C
str 100F8568 "G_OpenDemo_42" in 0267A29C
str 100F8578 "G_OpenDemo_43" in 0267A29C
str 100F8588 "G_OpenDemo_44" in 0267A29C
str 100F8598 "G_OpenDemo_45" in 0267A29C
str 100F85A8 "G_OpenDemo_46" in 0267A29C
str 100F85B8 "G_OpenDemo_47" in 0267A29C
str 100F85C8 "G_OpenDemo_48" in 0267A29C
str 100F7F10 "G_Island_00" in 0267A29C
str 100F7F1C "G_Island_03" in 0267A29C
str 100F7F28 "G_Island_10" in 0267A29C
str 100F7F34 "G_Island_12" in 0267A29C
str 100F7F40 "G_Island_19" in 0267A29C
str 100F7F4C "G_Island_22" in 0267A29C
str 100F7F58 "G_Island_25" in 0267A29C
str 100F7F64 "G_Island_39" in 0267A29C
str 100F7F70 "G_Island_40" in 0267A29C
str 100F7F7C "G_Island_43" in 0267A29C
str 100F7F88 "G_Island_44" in 0267A29C
str 100F8AD8 "MapTouch" in 0267A29C
str 100F7F94 "TreasureGet" in 0267A29C
str 100F7EE4 "Korog" in 0267A29C
str 100F7FA0 "MapMarkLoop" in 0267A29C
str 100F8AE4 "OpenDemo" in 0267A29C
str 100F7EEC "Map_00" in 0267A29C
data sSeaMenuInitializerWords 10495400
data sSeaMenuInitializerLimits 104953E4
data sSeaMenuFirstRegistration 101F61DC
data sSeaMenuSecondRegistration 101F61E8
data sSeaMenuThirdRegistration 101F61F4
data sSeaMenuAuxiliaryNames 10495468
data sSeaMenuFirstObject 104953FC
data sSeaMenuSecondObject 104953FD
data sSeaMenuMinimumAngle 100F7ED4
data sSeaMenuMaximumAngle 100F7ED8
data sSeaMenuExtent 100F7EDC
data sSeaMenuLimit 100F7EE0
data sSeaMenuStringVtable 100F7B4C
ext 028F026C void seaMenuRegisterGlobal(SeaMenuRegistration_l*)
ext 028ED6F8 void seaMenuConstructFirstObject(be<u8>*)
ext 028EAB2C void seaMenuConstructSecondObject(be<u8>*)
data sSeaMenuButtonConfirmState 10490464
data sSeaMenuButtonTouchState 10490494
ext 0263A3AC void seaMenuSetButtonBusy(hdParts_c*)
ext 0263A010 void seaMenuSetButtonName(hdParts_c*, hdUiSafeString*)
ext 0263A348 void seaMenuPrepareButton(hdParts_c*)
ext 0263A38C void seaMenuOpenButton(hdParts_c*)
ext 02638144 void seaMenuExecuteMapButton(hdParts_c*)
data sSeaMenuGridSize 100F7E7C
data sSeaMenuCoordinateScale 100F7E80
ext 026890E8 u32 seaMenuUiHasPointer(HdPadUi*)
data sSeaMenuCollectedFrame 100F7E88
data sSeaMenuMapButtonClass 104904F4
data sSeaMenuLoopClass 1049DE30
data sSeaMenuNoControlClass 1049E518
ext 026376E0 hdParts_c* seaMenuConstructMapButton(hdParts_c*)
ext 02637FE0 u32 seaMenuInitializeMapButton(hdParts_c*, u32, u32, hdPartsDesc_c*)
ext 026F92C0 u32 seaMenuCreateDefaultChild(hdParts_c*, u32, hdPartsDesc_c*)
*/
