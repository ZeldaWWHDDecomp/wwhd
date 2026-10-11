/* dMenu_PlaceName_c screen and the retained placename event helper.
 * GameCube event naming; HD uses a native layout, two text boxes and state delegates.
 * All fifteen entry points retain their verified register returns and call/load ordering. */
#include "d/d_menu_window.h"
using namespace MenuHD;

static DestructorRecord l_destructorList;
static DestructorRecord l_globalDestructor;
static DestructorRecord l_headerDestructor;
static DestructorRecord l_archiveDestructor;
static f32 l_initialX;
static f32 l_initialY;
static f32 mInitialX;
static f32 mInitialY;
static u8 l_headerByte;
static u8 l_archiveByte;
static SafeString l_moduleName;
static StringPair l_animationNames;
static StringPair l_animationRoots;
static StringPair l_textBoxNames;
static LocationNames l_locationNames;
static SafeString l_layoutName;
static StateID StateID_Wait;
static StateID StateID_InAll;
static StateID StateID_OutAll;
static AnimationIndices l_animationIndices;
static f32 l_animationZero;
static f32 l_animationEnd;
static u32 l_stateIdGuard;
static u32 l_nextStateId;
static u32 mDemoFrame;
static u32 mDemoMode;
static u32 mSave;
static u32 mAllocationManager;

/* 026C9CE4 */
u32 dMs_placenameMove() {
    auto* play = gabi::at<Play>(menuHD_getPlay());
    if (!play->mEventRunning.get())
        return gabi::ea(play);

    play = gabi::at<Play>(menuHD_getPlay());
    u32 result = menuHD_startCheck(&play->mEventManager, "awake");
    if (s32(result) != 0) {
        const u32 frame = mDemoFrame.get();
        if (s32(frame) == 200) {
            play = gabi::at<Play>(menuHD_getPlay());
            play->mStageNameIndex = 0;
            play->mStageNameStatus = StageNameOpen;
            auto* save = gabi::at<Save>(mSave.get());
            return menuHD_onEventBit(&save->mEvent, 0x3510);
        }
        if (s32(frame) == 350) {
            play = gabi::at<Play>(menuHD_getPlay());
            play->mStageNameStatus = StageNameClose;
            return gabi::ea(play);
        }
        return result;
    }

    play = gabi::at<Play>(menuHD_getPlay());
    result = menuHD_startCheck(&play->mEventManager, "majyuu_shinnyuu");
    if (s32(result) == 0)
        return result;
    const u32 frame = mDemoFrame.get();
    if (s32(frame) == 2900) {
        play = gabi::at<Play>(menuHD_getPlay());
        play->mStageNameIndex = 1;
        play->mStageNameStatus = StageNameOpen;
        return gabi::ea(play);
    }
    if (s32(frame) == 3050) {
        play = gabi::at<Play>(menuHD_getPlay());
        play->mStageNameStatus = StageNameClose;
        return gabi::ea(play);
    }
    // HD: the last event-query register is observable even when no stage-name flag changes.
    return result;
}

// HD: native dMenu_PlaceName_c screen and its StateProcessor lifecycle.
/* 026C97E4 */
u32 dMenu_PlaceName_c__dt(dMenu_PlaceName_c* object, u32 flags) {
    if (!object)
        return gabi::ea(object);
    object->mStateInterfaceVtable = kStateInterfaceVtable;
    object->mDrawInterfaceVtable = kDrawInterfaceVtable;
    u32 result = menuHD_baseDestruct(object, 0);
    if (flags & 1)
        result = menuHD_operatorDelete(object);
    return result;
}

/* 026C9850 */
u32 dMenu_PlaceName_c::executeWait() {
    auto* play = gabi::at<Play>(menuHD_getPlay());
    if (s32(play->mStageNameStatus.get()) == StageNameOpen)
        menuHD_changeState(&mStateProcessor, &StateID_InAll);
    const u32 target = gabi::at<PlaceVtable>(mVtable.get())->mInitialize.get();
    return gabi::call_ptr<u32>(target, this);
}

/* 026C98AC */
u32 dMenu_PlaceName_c::initializeInAll() {
    gabi::Local<StringFrame> frame;
    auto* play = gabi::at<Play>(menuHD_getPlay());
    u32 index = play->mStageNameIndex.get();
    if (index >= kLocationCount)
        index = 0;
    // Cache the two pane slots, but reload the layout and archive for each text call.
    const u32 paneSlots = gabi::ea(&mpTextBox);
    u32 slot = paneSlots;
    for (u32 i = 0; i < 2; ++i, slot += 4) {
        const u32 selectedSlot = i < 2 ? slot : paneSlots;
        auto* layout = gabi::at<Layout>(mpLayout.get());
        auto* data = gabi::at<LayoutData>(layout->mpData.get());
        const u32 archive = data->mpMessageArchive.get();
        frame->mArchiveName.mVtable = kSafeStringVtable;
        frame->mArchiveName.mpText = archive;
        const u32 pane = gabi::at<be<u32>>(selectedSlot)->get();
        menuHD_setText(gabi::at<u8>(pane), &frame->mArchiveName, &l_locationNames.mEntries[index]);
    }
    auto* layout = gabi::at<Layout>(mpLayout.get());
    const f64 animationFrame = l_animationZero.get();
    auto* animation = gabi::at<u8>(layout->mpAnimation.get());
    const u32 result = menuHD_setAnimationEnabled(animation, 0, 1, animationFrame);
    mVisible = 1;
    return result;
}

/* 026C9978 */
u32 dMenu_PlaceName_c::executeInAll() {
    auto* play = gabi::at<Play>(menuHD_getPlay());
    if (s32(play->mStageNameStatus.get()) == StageNameClose)
        menuHD_changeState(&mStateProcessor, &StateID_OutAll);
    const u32 target = gabi::at<PlaceVtable>(mVtable.get())->mInitialize.get();
    return gabi::call_ptr<u32>(target, this);
}

/* 026C99D4 */
u32 dMenu_PlaceName_c::initializeOutAll() {
    const f64 frame = l_animationZero.get();
    auto* layout = gabi::at<Layout>(mpLayout.get());
    return menuHD_setAnimationEnabled(gabi::at<u8>(layout->mpAnimation.get()), 1, 1, frame);
}

/* 026C99F0 */
u32 dMenu_PlaceName_c::executeOutAll() {
    auto* layout = gabi::at<Layout>(mpLayout.get());
    const u32 finished = menuHD_isAnimationFinished(gabi::at<u8>(layout->mpAnimation.get()), 1);
    if (s32(finished) != 0)
        menuHD_changeState(&mStateProcessor, &StateID_Wait);
    const u32 target = gabi::at<PlaceVtable>(mVtable.get())->mInitialize.get();
    return gabi::call_ptr<u32>(target, this);
}

/* 026C9A54 */
u32 dMenu_PlaceName_c::finalizeOutAll() {
    auto* play = gabi::at<Play>(menuHD_getPlay());
    play->mStageNameStatus = StageNameIdle;
    mVisible = 0;
    return gabi::ea(play);
}

/* 026C9A8C */
u32 dMenu_PlaceName_c__ct(dMenu_PlaceName_c* object) {
    if (!object)
        object = gabi::at<dMenu_PlaceName_c>(menuHD_operatorNew(sizeof(dMenu_PlaceName_c)));
    if (!object)
        return 0;
    menuHD_baseConstruct(object);
    object->mVtable = kPlaceVtable;
    object->mDrawInterfaceVtable = kDrawInterfaceVtable;
    object->mStateInterfaceVtable = kStateInterfaceVtable;
    // HD: preserve the GHS address-wrap allocation branch for the inline pane-pointer array.
    if (!gabi::ea(&object->mpTextBox))
        menuHD_operatorNew(8);
    object->mVisible = 0;
    return gabi::ea(object);
}

/* 026C9B10 */
u32 dMenu_PlaceName_c::create(u32 archive, u32 heap, u32 parent) {
    const u32 savedArchive = archive, savedHeap = heap, savedParent = parent;
    menuHD_setModuleName(this, &l_moduleName, 1);
    u32 target = gabi::at<PlaceVtable>(mVtable.get())->mCreateLayout.get();
    const u32 created = gabi::call_ptr<u32>(target, this, &l_layoutName, mpHeap.get(), savedArchive,
                                            savedHeap, savedParent);
    if (s32(created) == 0)
        return 0;

    auto* layout = gabi::at<Layout>(mpLayout.get());
    auto* data = gabi::at<LayoutData>(layout->mpData.get());
    const u32 root = data->mpRootPane.get();
    const u32 paneSlots = gabi::ea(&mpTextBox);
    u32 slot = paneSlots;
    for (u32 i = 0; i < 2; ++i, slot += 4) {
        const u32 selectedSlot = i < 2 ? slot : paneSlots;
        SafeString* name = &l_textBoxNames.mEntries[i];
        target = gabi::at<SafeStringVtable>(name->mVtable.get())->mAssureTermination.get();
        const u32 rootVtable = gabi::at<Pane>(root)->mPaneVtable.get();
        gabi::call_ptr<u32>(target, name);
        target = gabi::at<PaneVtable>(rootVtable)->mFindPane.get();
        const u32 text = name->mpText.get();
        const u32 pane = gabi::call_ptr<u32>(target, gabi::at<u8>(root), text, 1);
        *gabi::at<be<u32>>(selectedSlot) = pane;
    }
    layout = gabi::at<Layout>(mpLayout.get());
    const f64 endFrame = l_animationEnd.get();
    menuHD_setAnimationFrame(gabi::at<u8>(layout->mpAnimation.get()), 0, 1, endFrame);
    target = gabi::at<PlaceVtable>(mVtable.get())->mInitialize.get();
    gabi::call_ptr<u32>(target, this);

    const u32 processor = mStateProcessor.mpProcessor.get();
    target = gabi::at<StateVtable>(gabi::at<be<u32>>(processor)->get())->mFindState.get();
    const u32 state = gabi::call_ptr<u32>(target, gabi::at<u8>(processor), &StateID_Wait);
    mpState = state;
    target = gabi::at<StateVtable>(gabi::at<be<u32>>(state)->get())->mInitialize.get();
    gabi::call_ptr<u32>(target, gabi::at<u8>(state));
    return 1;
}

/* 026C9C90 */
u32 dMenu_PlaceName_c::calc() {
    u32 target = gabi::at<PlaceVtable>(mVtable.get())->mCalc.get();
    gabi::call_ptr<u32>(target, this);
    target = gabi::at<PlaceVtable>(mVtable.get())->mClose.get();
    gabi::call_ptr<u32>(target, this);
    return menuHD_baseCalc(this);
}

/* 026C9E0C */
u32 dMenu_PlaceName_c::update() {
    if (s32(mDemoMode.get()) == 1)
        menuHD_placenameMoveEntry();
    return menuHD_updateState(&mStateProcessor);
}

/* 026C9E54 */
u32 dMenu_PlaceName_c::draw() {
    if (!mVisible.get())
        return gabi::ea(this);
    const u32 target = gabi::at<PlaceVtable>(mVtable.get())->mDraw.get();
    return gabi::call_ptr<u32>(target, this);
}

/* 026C9E70 */
u32 dMenu_PlaceName_c::createLayout(u32 archive, u32 heap, u32 parent, u32 root, u32 flags) {
    auto* manager = gabi::at<AllocationManager>(mAllocationManager.get());
    u32 allocation = menuHD_heapNew(sizeof(Layout), gabi::at<u8>(manager->mpHeap.get()), 4);
    if (s32(allocation) != 0)
        allocation = menuHD_layoutConstruct(gabi::at<u8>(allocation));
    mpLayout = allocation;
    if (s32(allocation) == 0)
        return 0;
    const u32 target =
        gabi::at<LayoutVtable>(gabi::at<Layout>(allocation)->mVtable.get())->mCreate.get();
    gabi::call_ptr<u32>(target, gabi::at<u8>(allocation), archive, heap, parent, root, 2, 2, flags);
    for (u32 i = 0; i < 2; ++i) {
        auto* layout = gabi::at<Layout>(mpLayout.get());
        const u32 animationIndex = rotl32(l_animationIndices.mValues[i].get(), 3) & 0xFFFFFFF8u;
        menuHD_bindAnimation(gabi::at<u8>(layout->mpAnimation.get()), i,
                             &l_animationNames.mEntries[i],
                             gabi::at<u8>(gabi::ea(&l_animationRoots) + animationIndex));
    }
    const u32 initialize = gabi::at<PlaceVtable>(mVtable.get())->mInitialize.get();
    gabi::call_ptr<u32>(initialize, this);
    return 1;
}

// HD: compiler-generated initialization of SafeStrings and native state delegates.
/* 026C9F94 */
u32 __sinit_d_menu_window_cpp() {
    u32 result = 0;
    l_destructorList.mFlags = 0;
    l_destructorList.mpDestructor = 0;
    l_destructorList.mpObject = 0;
    l_destructorList.mNext = 0;
    result = menuHD_registerGlobal(&l_globalDestructor.mNext);
    const f64 initialX = l_initialX.get();
    const f64 initialY = l_initialY.get();
    mInitialX = initialX;
    mInitialY = initialY;
    result = menuHD_constructHeaderByte(&l_headerByte);
    result = menuHD_registerGlobal(&l_headerDestructor.mNext);
    result = menuHD_constructArchiveByte(&l_archiveByte);
    result = menuHD_registerGlobal(&l_archiveDestructor.mNext);
    l_animationNames.mEntries[0].mVtable = kSafeStringVtable;
    l_animationNames.mEntries[0].mpText = gabi::guest_addr("InAll");
    l_animationNames.mEntries[1].mVtable = kSafeStringVtable;
    l_animationRoots.mEntries[0].mVtable = kSafeStringVtable;
    l_animationRoots.mEntries[0].mpText = gabi::guest_addr("G_Text_00");
    l_animationRoots.mEntries[1].mVtable = kSafeStringVtable;
    l_animationRoots.mEntries[1].mpText = gabi::guest_addr("G_All_00");
    l_animationNames.mEntries[1].mpText = gabi::guest_addr("OutAll");
    l_locationNames.mEntries[0].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[0].mpText = gabi::guest_addr("T_PlaceName_00_01");
    l_locationNames.mEntries[1].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[1].mpText = gabi::guest_addr("T_PlaceName_00_02");
    l_locationNames.mEntries[2].mVtable = kSafeStringVtable;
    l_textBoxNames.mEntries[0].mVtable = kSafeStringVtable;
    l_textBoxNames.mEntries[0].mpText = gabi::guest_addr("T_PlaceName_00");
    l_locationNames.mEntries[2].mpText = gabi::guest_addr("T_PlaceName_00_03");
    l_locationNames.mEntries[3].mVtable = kSafeStringVtable;
    l_textBoxNames.mEntries[1].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[3].mpText = gabi::guest_addr("T_PlaceName_00_04");
    l_textBoxNames.mEntries[1].mpText = gabi::guest_addr("T_PlaceName_01");
    l_locationNames.mEntries[4].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[4].mpText = gabi::guest_addr("T_PlaceName_00_05");
    l_locationNames.mEntries[5].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[5].mpText = gabi::guest_addr("T_PlaceName_00_06");
    l_locationNames.mEntries[6].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[6].mpText = gabi::guest_addr("T_PlaceName_00_07");
    l_moduleName.mVtable = kSafeStringVtable;
    l_locationNames.mEntries[7].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[7].mpText = gabi::guest_addr("T_PlaceName_00_08");
    l_locationNames.mEntries[8].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[8].mpText = gabi::guest_addr("T_PlaceName_00_09");
    l_locationNames.mEntries[9].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[9].mpText = gabi::guest_addr("T_PlaceName_00_10");
    l_locationNames.mEntries[10].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[10].mpText = gabi::guest_addr("T_PlaceName_00_11");
    l_locationNames.mEntries[11].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[11].mpText = gabi::guest_addr("T_PlaceName_00_12");
    l_locationNames.mEntries[12].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[12].mpText = gabi::guest_addr("T_PlaceName_00_01");
    l_locationNames.mEntries[13].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[13].mpText = gabi::guest_addr("T_PlaceName_00_14");
    l_locationNames.mEntries[14].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[14].mpText = gabi::guest_addr("T_PlaceName_00_15");
    l_locationNames.mEntries[15].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[15].mpText = gabi::guest_addr("T_PlaceName_00_16");
    l_locationNames.mEntries[16].mVtable = kSafeStringVtable;
    l_locationNames.mEntries[16].mpText = gabi::guest_addr("T_PlaceName_00_17");
    l_locationNames.mEntries[17].mVtable = kSafeStringVtable;
    const u32 guardValue = l_stateIdGuard.get();
    l_locationNames.mEntries[17].mpText = gabi::guest_addr("T_PlaceName_00_18");
    l_locationNames.mEntries[18].mVtable = kSafeStringVtable;
    l_layoutName.mVtable = kSafeStringVtable;
    l_moduleName.mpText = gabi::guest_addr("PlaceName_00");
    l_layoutName.mpText = gabi::guest_addr("PlaceName_00");
    l_locationNames.mEntries[18].mpText = gabi::guest_addr("T_PlaceName_00_19");
    u32 stateId;
    if (guardValue != 0) {
        stateId = l_nextStateId.get();
    } else {
        l_stateIdGuard = 1;
        stateId = 0;
    }
    StateID_Wait.mInitialize.mThisAdjust = 0;
    StateID_Wait.mInitialize.mVirtualSlot = 25;
    StateID_Wait.mExecute.mThisAdjust = 0;
    StateID_Wait.mExecute.mVirtualSlot = 26;
    StateID_Wait.mId = (stateId + 1);
    StateID_Wait.mpName = gabi::guest_addr("::StateID_Wait");
    StateID_Wait.mFinalize.mThisAdjust = 0;
    StateID_Wait.mInitialize.mFlags = kStateCallbackFlags;
    StateID_Wait.mExecute.mFlags = kStateCallbackFlags;
    StateID_Wait.mFinalize.mFlags = kStateCallbackFlags;
    StateID_Wait.mFinalize.mVirtualSlot = 27;
    StateID_Wait.mReserved.mThisAdjust = 0;
    StateID_InAll.mId = ((stateId + 1) + 1);
    StateID_InAll.mpName = gabi::guest_addr("::StateID_InAll");
    StateID_InAll.mInitialize.mFlags = kStateCallbackFlags;
    StateID_InAll.mInitialize.mThisAdjust = 0;
    StateID_InAll.mInitialize.mVirtualSlot = 28;
    StateID_InAll.mExecute.mThisAdjust = 0;
    StateID_InAll.mExecute.mVirtualSlot = 29;
    StateID_InAll.mExecute.mFlags = kStateCallbackFlags;
    StateID_InAll.mFinalize.mFlags = kStateCallbackFlags;
    StateID_InAll.mReserved.mFlags = 0;
    StateID_InAll.mVtable = kStateVtable;
    StateID_Wait.mReserved.mFlags = 0;
    StateID_InAll.mFinalize.mThisAdjust = 0;
    StateID_Wait.mReserved.mVirtualSlot = 0;
    StateID_Wait.mVtable = kStateVtable;
    StateID_Wait.mpNullState = kNullState;
    l_nextStateId = (((stateId + 1) + 1) + 1);
    StateID_InAll.mFinalize.mVirtualSlot = 30;
    StateID_OutAll.mInitialize.mThisAdjust = 0;
    StateID_InAll.mReserved.mThisAdjust = 0;
    StateID_InAll.mReserved.mVirtualSlot = 0;
    StateID_OutAll.mId = (((stateId + 1) + 1) + 1);
    StateID_OutAll.mInitialize.mVirtualSlot = 31;
    StateID_OutAll.mpName = gabi::guest_addr("::StateID_OutAll");
    StateID_OutAll.mInitialize.mFlags = kStateCallbackFlags;
    StateID_OutAll.mExecute.mFlags = kStateCallbackFlags;
    StateID_OutAll.mFinalize.mFlags = kStateCallbackFlags;
    StateID_OutAll.mReserved.mFlags = 0;
    StateID_OutAll.mVtable = kStateVtable;
    StateID_InAll.mpNullState = kNullState;
    StateID_OutAll.mExecute.mThisAdjust = 0;
    StateID_OutAll.mExecute.mVirtualSlot = 32;
    StateID_OutAll.mFinalize.mThisAdjust = 0;
    StateID_OutAll.mFinalize.mVirtualSlot = 33;
    StateID_OutAll.mReserved.mThisAdjust = 0;
    StateID_OutAll.mReserved.mVirtualSlot = 0;
    StateID_OutAll.mpNullState = kNullState;
    return result;
}

/* WWHD glue
data l_moduleName 1049A01C
data mInitialX 1049A024
data mInitialY 1049A028
data l_headerByte 1049A02C
data l_archiveByte 1049A02D
data l_destructorList 1049A030
data l_animationRoots 1049A040
data l_animationNames 1049A050
data l_textBoxNames 1049A060
data l_locationNames 1049A070
data StateID_Wait 1049A108
data StateID_InAll 1049A138
data StateID_OutAll 1049A168
data l_layoutName 1049A198
data l_globalDestructor 101F6950
data l_headerDestructor 101F695C
data l_archiveDestructor 101F6968
data l_initialX 10101708
data l_initialY 1010170C
data l_animationZero 101016E8
data l_animationEnd 101016EC
data l_animationIndices 10101630
data l_stateIdGuard 101FD8E4
data l_nextStateId 101FDD50
data mDemoFrame 101D600C
data mDemoMode 101D6010
data mSave 101F84DC
data mAllocationManager 1018C404
str 101016F0 "awake"
str 101016F8 "majyuu_shinnyuu"
str 10101710 "InAll"
str 10101718 "OutAll"
str 10101720 "::StateID_InAll"
str 10101730 "G_Text_00"
str 1010173C "T_PlaceName_00_01"
str 10101750 "T_PlaceName_00_02"
str 10101764 "T_PlaceName_00_03"
str 10101778 "T_PlaceName_00_04"
str 1010178C "T_PlaceName_00_05"
str 101017A0 "T_PlaceName_00_06"
str 101017B4 "T_PlaceName_00_07"
str 101017C8 "T_PlaceName_00_08"
str 101017DC "T_PlaceName_00_09"
str 101017F0 "T_PlaceName_00_10"
str 10101804 "T_PlaceName_00_11"
str 10101818 "T_PlaceName_00_12"
str 1010182C "T_PlaceName_00_14"
str 10101840 "T_PlaceName_00_15"
str 10101854 "T_PlaceName_00_16"
str 10101868 "T_PlaceName_00_17"
str 1010187C "T_PlaceName_00_18"
str 10101890 "T_PlaceName_00_19"
str 101018A4 "G_All_00"
str 101018B0 "T_PlaceName_00"
str 101018C0 "T_PlaceName_01"
str 101018D0 "PlaceName_00"
str 101018E0 "::StateID_Wait"
str 101018F0 "::StateID_OutAll"
ext 025200D4 u32 menuHD_getPlay()
ext 026F88F0 u32 menuHD_baseDestruct(void*, u32)
ext 0273AF40 u32 menuHD_operatorDelete(void*)
ext 020063C0 u32 menuHD_changeState(void*, void*)
ext 027045B0 u32 menuHD_setText(void*, void*, void*)
ext 020053E4 u32 menuHD_setAnimationEnabled(void*, u32, u32, f64)
ext 02005840 u32 menuHD_isAnimationFinished(void*, u32)
ext 0273AD10 u32 menuHD_operatorNew(u32)
ext 026F89E8 u32 menuHD_baseConstruct(void*)
ext 026F90D8 u32 menuHD_setModuleName(void*, void*, u32)
ext 0200552C u32 menuHD_setAnimationFrame(void*, u32, u32, f64)
ext 026F91F8 u32 menuHD_baseCalc(void*)
ext 025445B8 u32 menuHD_startCheck(void*, const char*)
ext 025B8B68 u32 menuHD_onEventBit(void*, u32)
ext 026C9CE4 u32 menuHD_placenameMoveEntry()
ext 02006364 u32 menuHD_updateState(void*)
ext 0273B050 u32 menuHD_heapNew(u32, void*, u32)
ext 026FADB0 u32 menuHD_layoutConstruct(void*)
ext 02004E04 u32 menuHD_bindAnimation(void*, u32, void*, void*)
ext 028F026C u32 menuHD_registerGlobal(void*)
ext 028ED6F8 u32 menuHD_constructHeaderByte(void*)
ext 028EAB2C u32 menuHD_constructArchiveByte(void*)
*/
