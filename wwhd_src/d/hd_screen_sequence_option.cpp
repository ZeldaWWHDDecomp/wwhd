/** HD SequenceOption_00 options screen. */
#include "d/hd_sequence_option.h"

static u32 sOptionStateOwnerVtable;
static u32 sOptionSecondaryVtable;
extern void optionBaseDestroy(hdParts_c*, u32);
extern void optionDelete(void*);

/* 026D57A8 */
u32 hdSequenceOption_c::rejectInput() { return 0; }
/* 026D57BC */
u32 hdSequenceOption_c::rejectTouch() { return 0; }
/* 026D57C4 */
u32 hdSequenceOption_c::rejectAction() { return 0; }
/* 026D57CC */
u32 optionStateKey(hdPartsStateId_c* state) { return state->mNumber; }

/* 026D58CC */
void hdSequenceOption_c::destroy(hdSequenceOption_c* object, u32 flags) {
    if (object == NULL)
        return;
    object->mStateHolder.mHolderVtable = &sOptionStateOwnerVtable;
    object->mSecondaryVtable = &sOptionSecondaryVtable;
    optionBaseDestroy(object, 0);
    if (flags & 1)
        optionDelete(object);
}

/* 026D5938 */
u32 hdSequenceOption_c::settingValue(u32 index) {
    u32 mode = mMode;
    u32 value = mSettingValues[index < SETTING_COUNT ? index : 0];
    if (mode == 1 && index == 1 && mSettingValues[0] == 2)
        return 1;
    return value;
}

/* 026D75CC */
void hdSequenceOption_c::requestClose() { mCloseRequested = 1; }

static hdOptionSettingTables_l sOptionSettings;
static hdOptionButtonIds_l sOptionNormalButtonIds;
static hdOptionButtonIds_l sOptionAlternateButtonIds;
extern void optionDetachArchives(hdParts_c*);

/* 026D5980 */
u32 hdSequenceOption_c::buttonIndex(u32 selection) {
    u32 pageSize = mEntriesPerPage;
    // HD: PowerPC unsigned division returns zero when the divisor is zero.
    u32 quotient = pageSize ? selection / pageSize : 0;
    u32 index = selection - quotient * pageSize;
    hdOptionButtonIds_l* ids = mMode == MODE_CONTROLLER ? &sOptionAlternateButtonIds : &sOptionNormalButtonIds;
    return hdPartsView<be<u32>>(gabi::ea(ids) + index * sizeof(be<u32>))->get();
}

/* 026D5E38 */
u32 hdSequenceOption_c::settingValueCount(u32 selection) {
    hdOptionSettingRecord_l* records =
        hdPartsView<gptr<hdOptionSettingRecord_l>>(gabi::ea(&sOptionSettings) + u32(mMode) * 4)->get();
    return hdPartsView<hdOptionSettingRecord_l>(gabi::ea(records) + selection * sizeof(*records))->mValueCount;
}

/* 026D705C */
void hdSequenceOption_c::bindButton(u32 index, u32 button, u32 action) {
    hdParts_c* child = mChildren.safeSlot(index)->get();
    reinterpret_cast<hdButtonPart_c*>(child)->setReceiverButtons(button, 0, 0, action, 0, 0, 0, 0);
}

/* 026D8020 */
void hdSequenceOption_c::remove() {
    vDeleteChildren();
    vDestroyLayout();
    optionDetachArchives(this);
}

extern u32 optionPaneContains(hdPartsPane_c*, hdOptionPoint_l*);

/* 026D67E0 */
u32 hdSequenceOption_c::isPointOutside(hdOptionPoint_l* point) {
    gabi::NativeFrame<0x18> frame;
    hdOptionPoint_l* copy = hdPartsView<hdOptionPoint_l>(frame.a - 8);
    // HD: the paired lfs/stfs copy preserves both input payloads and loads before either store.
    u32 x = point->mWords[0];
    u32 y = point->mWords[1];
    copy->mWords[0] = x;
    copy->mWords[1] = y;
    return optionPaneContains(mPanes[3].get(), copy) ^ 1;
}

static hdOptionLabels_l sOptionLabels;
static hdUiSafeString sOptionHelpLabel;
static hdUiStringTable sOptionStringVtable;

/* 026D5E58 */
void hdSequenceOption_c::setSettingLabel(u32 selection) {
    hdOptionArchiveName_l* layout = reinterpret_cast<hdOptionArchiveName_l*>(mpLayout->lyt());
    u32 mode = mMode;
    be<u8>* name = layout->mpName.get();
    hdOptionSettingRecord_l* records =
        hdPartsView<gptr<hdOptionSettingRecord_l>>(gabi::ea(&sOptionSettings) + mode * 4)->get();
    hdOptionSettingRecord_l* setting = hdPartsView<hdOptionSettingRecord_l>(
        gabi::ea(records) + selection * sizeof(*records));
    gabi::NativeFrame<0x18> frame;
    hdUiSafeString* archive = hdPartsView<hdUiSafeString>(frame.a - 8);
    archive->mVtable = &sOptionStringVtable;
    archive->mText = name;
    u32 label = setting->mDescriptionIndex;
    hdTextPane_resetMessage(mpSettingLabel.get(), archive,
        hdPartsView<hdUiSafeString>(gabi::ea(&sOptionLabels) + label * sizeof(hdUiSafeString)));
}

/* 026D81D0 */
void hdSequenceOption_c::setHelpLabel() {
    hdOptionArchiveName_l* layout = reinterpret_cast<hdOptionArchiveName_l*>(mpLayout->lyt());
    hdTextPane_c* pane = mpHelpLabel;
    be<u8>* name = layout->mpName.get();
    gabi::NativeFrame<0x18> frame;
    hdUiSafeString* archive = hdPartsView<hdUiSafeString>(frame.a - 8);
    archive->mVtable = &sOptionStringVtable;
    archive->mText = name;
    hdTextPane_resetMessage(pane, archive, &sOptionHelpLabel);
}

static hdPartsStateId_c sOptionButtonWaitState;
static hdPartsStateId_c sOptionConnectWaitState;

/* 026D624C */
void hdSequenceOption_c::inAllExecute() {
    if (hdAnimSet_isEnd(mpLayout->animSet(), 0))
        hdParts_changeState(stateMachine(), &sOptionButtonWaitState);
    vExecuteChildren();
}

/* 026D75D8 */
void hdSequenceOption_c::connectedWaitExecute() {
    if (!mCloseRequested)
        hdParts_changeState(stateMachine(), &sOptionConnectWaitState);
    vExecuteChildren();
}

/* 026D6614 */
Pair32 hdSequenceOption_c::settingAction(u32 selection) {
    hdOptionSettingRecord_l* records =
        hdPartsView<gptr<hdOptionSettingRecord_l>>(gabi::ea(&sOptionSettings) + u32(mMode) * 4)->get();
    hdOptionSettingRecord_l* setting = hdPartsView<hdOptionSettingRecord_l>(
        gabi::ea(records) + selection * sizeof(*records));
    u32 high = *reinterpret_cast<be<u32>*>(&setting->mAction);
    u32 low = setting->mAction.mFunction;
    return {high, low};
}

using hdOptionEventManagerPointer = gptr<cking::ui::input::EventMgr>;
static hdOptionEventManagerPointer sOptionEventManager;
static hdUiEvent* optionCurrentEvent(cking::ui::input::EventMgr* manager) {
    auto* queue = hdPartsView<hdOptionEventQueue_l>(gabi::ea(manager) + offsetof(cking::ui::input::EventMgr, mEventQueue));
    return &queue->mCurrent;
}
static u8 sOptionInputPrompt;
static f32 sOptionAnimationRate;

/* 026D6304 */
void hdSequenceOption_c::outAllEnter() {
    sOptionEventManager->setModeDefault(0);
    sOptionInputPrompt = 0;
    f32 rate = sOptionAnimationRate;
    hdAnimSet_play(mpLayout->animSet(), 11, 0, rate);
}

/* 026D7494 */
void hdSequenceOption_c::changeConnectEnter() {
    sOptionEventManager->setModeDefault(0);
    sOptionInputPrompt = 0;
    mConnectionDelay = 16;
}

static u32 sOptionVtable;
static f32 sOptionZero;
extern void* optionAllocate(u32 size);
extern void optionBaseConstruct(hdParts_c*);

/* 026D7828 */
hdSequenceOption_c* hdSequenceOption_c::construct(hdSequenceOption_c* object) {
    if (object == NULL) {
        object = static_cast<hdSequenceOption_c*>(optionAllocate(sizeof(hdSequenceOption_c)));
        if (object == NULL)
            return NULL;
    }
    optionBaseConstruct(object);
    object->mVtable = reinterpret_cast<hdPartsVtbl_c*>(&sOptionVtable);
    object->mSecondaryVtable = &sOptionSecondaryVtable;
    object->mStateHolder.mHolderVtable = &sOptionStateOwnerVtable;
    // GHS retains each embedded-array address; allocation returns are deliberately unused.
    const u32 offsets[] = {
        offsetof(hdSequenceOption_c, mPanes), offsetof(hdSequenceOption_c, mTextPanes),
        offsetof(hdSequenceOption_c, mItemTextPanes), offsetof(hdSequenceOption_c, mItemWidths),
        offsetof(hdSequenceOption_c, mItemHeights), offsetof(hdSequenceOption_c, mItemHalfWidths),
        offsetof(hdSequenceOption_c, mSavedSelections), offsetof(hdSequenceOption_c, mSavedPageStarts),
        offsetof(hdSequenceOption_c, mSettingValues)
    };
    const u32 sizes[] = {
        sizeof(object->mPanes), sizeof(object->mTextPanes), sizeof(object->mItemTextPanes),
        sizeof(object->mItemWidths), sizeof(object->mItemHeights), sizeof(object->mItemHalfWidths),
        sizeof(object->mSavedSelections), sizeof(object->mSavedPageStarts), sizeof(object->mSettingValues)
    };
    for (u32 index = 0; index < sizeof(offsets) / sizeof(offsets[0]); ++index) {
        if (gabi::ea(object) + offsets[index] == 0)
            optionAllocate(sizes[index]);
    }
    object->mCategory = 0;
    object->mPreviousSelection = -1;
    object->mItemCount = 0;
    object->mMode = MODE_UNOPENED;
    object->mPreviousCategory = 1;
    object->mDirectionalInputEnabled = 0;
    object->mScrollForward = 0;
    object->mConnectionMessage = 0;
    object->mpSettingLabel = NULL;
    object->mScrollAnimation = 0;
    object->mpHelpLabel = NULL;
    object->mPageStart = 0;
    object->mEntriesPerPage = 0;
    object->mConnectionDelay = 0;
    object->mCloseRequested = 0;
    object->mSelection = 0;
    object->mConnectionCounter = 0;
    object->mOutsideTouchArmed = 0;
    object->mTouchStarted = 0;
    for (u32 index = 0; index < PANE_COUNT; ++index)
        object->mPanes[index] = NULL;
    for (u32 index = 0; index < TEXT_PANE_COUNT; ++index)
        object->mTextPanes[index] = NULL;
    f32 zero = sOptionZero;
    for (u32 index = 0; index < ITEM_PANE_COUNT; ++index) {
        object->mItemTextPanes[index] = NULL;
        object->mItemWidths[index] = zero;
        object->mItemHeights[index] = zero;
        object->mItemHalfWidths[index] = zero;
    }
    for (u32 index = 0; index < MODE_COUNT; ++index) {
        object->mSavedSelections[index] = 0;
        object->mSavedPageStarts[index] = 0;
    }
    for (u32 index = 0; index < SETTING_COUNT; ++index)
        object->mSettingValues[index] = 0;
    return object;
}

using hdOptionSavePointer = gptr<hdOptionSaveRoot_l>;
static hdOptionSavePointer sOptionSave;
using hdOptionSystemPointer = gptr<void>;
static hdOptionSystemPointer sOptionSystem;
extern void optionApplyControllerSettings(void*);
static hdSaveHolders_c* optionSaveHolders() {
    return hdPartsView<hdSaveHolders_c>(gabi::ea(sOptionSave.get()) + offsetof(hdOptionSaveRoot_l, mHolders));
}

/* 026D6DD4 */
void hdSequenceOption_c::applySettings() {
    if (mMode == MODE_SETTINGS) {
        hdSaveSlotFlags_c* options = optionSaveHolders()->getScratchFlags();
        u32 value = mSettingValues[0];
        if (value < 2)
            options->mProgress[0] = value;
        value = mSettingValues[1];
        if (value < 2)
            options->mProgress[1] = value;
        value = mSettingValues[2];
        if (value < 2)
            options->mProgress[2] = value;
        options->setCreated(mSettingValues[4]);
        options->setChanged(mSettingValues[5]);
    } else if (mMode == MODE_CONTROLLER) {
        hdSavePair_c* options = optionSaveHolders()->getScratchPair();
        u32 savedPrimary = options->mPrimary;
        bool changed = mSettingValues[0] != savedPrimary;
        if (!changed) {
            options = optionSaveHolders()->getScratchPair();
            u32 secondary = mSettingValues[1];
            changed = secondary != options->mSecondary;
        }
        if (changed)
            optionApplyControllerSettings(sOptionSystem.get());
        options = optionSaveHolders()->getScratchPair();
        u32 value = mSettingValues[0];
        if (value < 3)
            options->mPrimary = value;
        options = optionSaveHolders()->getScratchPair();
        value = mSettingValues[1];
        if (value < 2)
            options->mSecondary = value;
    }
}

/* 026D6F18 */
void hdSequenceOption_c::buttonWaitExit() { applySettings(); }

static void optionExecuteButton(hdParts_c* button) {
    hdButtonPartVtable_l* vtable = reinterpret_cast<hdButtonPartVtable_l*>(button->mVtable.get());
    gabi::call_ptr<void>(vtable->mOnFocusChanged.get(), button);
}

/* 026D6968 */
void hdSequenceOption_c::executeButtons() {
    u32 category = mCategory;
    u32 count = mEntriesPerPage;
    if (category == 0) {
        optionExecuteButton(mChildren.safeSlot(7)->get());
        optionExecuteButton(mChildren.safeSlot(8)->get());
        --count;
    }
    for (u32 index = 0; index != count; ++index) {
        u32 selection = mPageStart + index;
        u32 childIndex = buttonIndex(selection);
        optionExecuteButton(mChildren.safeSlot(childIndex)->get());
    }
}

/* 026D7004 */
void hdSequenceOption_c::scrollExecute() {
    if (hdAnimSet_isEnd(mpLayout->animSet(), mScrollAnimation))
        hdParts_changeState(stateMachine(), &sOptionButtonWaitState);
    executeButtons();
}

static f32 sOptionItemWidthPadding;
static f32 sOptionItemHeightPadding;
static f32 sOptionHalf;

/* 026D5B20 */
void hdSequenceOption_c::measureItem(u32 index) {
    u32 slot = index < ITEM_PANE_COUNT ? index : 0;
    f32 width = mItemTextPanes[slot]->getTextWidth();
    f32 widthPadding = sOptionItemWidthPadding;
    mItemWidths[slot] = gabi::fadds_ppc(width, widthPadding);
    f32 height = mItemTextPanes[slot]->getTextHeight(-1);
    f32 heightPadding = sOptionItemHeightPadding;
    mItemHeights[slot] = gabi::fadds_ppc(height, heightPadding);
    f32 half = sOptionHalf;
    mItemHalfWidths[slot] = gabi::fmuls_ppc(width, half);
}

/* 026D5C3C */
void hdSequenceOption_c::setItemLabel(u32 selection) {
    u32 mode = mMode;
    hdOptionSettingRecord_l* records =
        hdPartsView<gptr<hdOptionSettingRecord_l>>(gabi::ea(&sOptionSettings) + mode * 4)->get();
    u32 childIndex = buttonIndex(selection);
    hdOptionArchiveName_l* layout = reinterpret_cast<hdOptionArchiveName_l*>(mpLayout->lyt());
    u32 slot = childIndex < ITEM_PANE_COUNT ? childIndex : 0;
    be<u8>* name = layout->mpName.get();
    gabi::NativeFrame<0x18> frame;
    hdUiSafeString* archive = hdPartsView<hdUiSafeString>(frame.a - 8);
    archive->mText = name;
    archive->mVtable = &sOptionStringVtable;
    u32 label = hdPartsView<hdOptionSettingRecord_l>(
        gabi::ea(records) + selection * sizeof(*records))->mNameIndex;
    hdTextPane_resetMessage(mItemTextPanes[slot].get(), archive,
        hdPartsView<hdUiSafeString>(gabi::ea(&sOptionLabels) + label * sizeof(hdUiSafeString)));
    measureItem(childIndex);
}

static hdUiSafeString sOptionButtonLabel;
extern u32 optionConstructMessage(HdMsgText*);
extern void optionSetMessageLabel(HdMsgText*, hdUiSafeString*, hdUiSafeString*);
static void optionSetButtonEnabled(hdParts_c* button, u32 enabled) {
    hdOptionButtonVtable_l* vtable = reinterpret_cast<hdOptionButtonVtable_l*>(button->mVtable.get());
    gabi::call_ptr<void>(vtable->mSetEnabled.get(), button, enabled);
}

/* 026D59B8 */
void hdSequenceOption_c::setButtonLabel(u32 selection) {
    hdOptionSettingRecord_l* records =
        hdPartsView<gptr<hdOptionSettingRecord_l>>(gabi::ea(&sOptionSettings) + u32(mMode) * 4)->get();
    be<u32>* labels = hdPartsView<hdOptionSettingRecord_l>(
        gabi::ea(records) + selection * sizeof(*records))->mpValueLabelIds.get();
    u32 value = settingValue(selection);
    u32 label = hdPartsView<be<u32>>(gabi::ea(labels) + value * 4)->get();
    gabi::NativeFrame<0x940> frame;
    HdMsgText* message = hdPartsView<HdMsgText>(frame.a);
    optionConstructMessage(message);
    hdOptionArchiveName_l* layout = reinterpret_cast<hdOptionArchiveName_l*>(mpLayout->lyt());
    be<u8>* name = layout->mpName.get();
    hdUiSafeString* archive = hdPartsView<hdUiSafeString>(frame.a - 8);
    archive->mVtable = &sOptionStringVtable;
    archive->mText = name;
    optionSetMessageLabel(message, archive,
        hdPartsView<hdUiSafeString>(gabi::ea(&sOptionLabels) + label * sizeof(hdUiSafeString)));
    u32 childIndex = buttonIndex(selection);
    hdButtonPart_setTextStyle(reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(childIndex)->get()),
        &sOptionButtonLabel, reinterpret_cast<PaneWork_l*>(message));
}

/* 026D5A90 */
u32 hdSequenceOption_c::updateAlternateButton() {
    hdParts_c* button = mChildren.safeSlot(6)->get();
    if (button != NULL) {
        optionSetButtonEnabled(button, mSettingValues[0] == 2 ? 0 : 1);
        setButtonLabel(1);
    }
    return 1;
}

static hdPartsStateId_c sOptionBlankState;
static u8 sOptionTouchMode;
using hdOptionInputOptionsPointer = gptr<cking::ui::input::Options>;
static hdOptionInputOptionsPointer sOptionInputOptions;
extern dOwPlayView_c* optionGetPlayButtons();
extern void optionEnableDirectionalInput(u32 enabled);

static bool optionIsBlank(hdParts_c* object) {
    hdPartsStateId_c* state = hdParts_getCurrentStateId(object->stateMachine());
    hdPartsStateIdVtbl_c* blankVtable = sOptionBlankState.mVtable;
    u32 key = state->getKey();
    u32 blankKey = gabi::call_ptr<u32>(blankVtable->mGetKey.get(), &sOptionBlankState);
    return key == blankKey;
}

/* 026D6360 */
void hdSequenceOption_c::outAllExit() {
    dOwPlayView_c* play = optionGetPlayButtons();
    u8 actionR = play->mSavedActionButtonR;
    u8 actionA = play->mSavedActionButtonA;
    play->mActionButtonR = actionR;
    u8 actionB = play->mSavedActionButtonB;
    play->mActionButtonA = actionA;
    play->mActionButtonB = actionB;
    sOptionTouchMode = 0;
    u32 mode = mMode;
    mSavedSelections[mode < MODE_COUNT ? mode : 0] = mSelection;
    mode = mMode;
    mSavedPageStarts[mode < MODE_COUNT ? mode : 0] = mPageStart;
    hdParts_changeState(stateMachine(), &sOptionBlankState);
}

/* 026D6400 */
void hdSequenceOption_c::outAllExecute() {
    if (hdAnimSet_isEnd(mpLayout->animSet(), 11)) {
        if (mDirectionalInputEnabled) {
            optionEnableDirectionalInput(0);
            hdPartsView<cking::ui::input::Options>(gabi::ea(sOptionInputOptions.get()))->mDirectionalInputEnabled = 0;
        }
        outAllExit();
    }
    vExecuteChildren();
}

/* 026D8074 */
void hdSequenceOption_c::execute() {
    if (!optionIsBlank(this)) {
        hdParts_executeStateMachine(stateMachine());
        hdTextPane_c* pane = mpSettingLabel;
        if (pane != NULL)
            pane->refresh(&pane->mWork);
        vCalcLayout();
    }
}

/* 026D811C */
void hdSequenceOption_c::draw(void* info) {
    if (!optionIsBlank(this)) {
        vDrawLayout(info);
        vDrawChildren(info);
    }
}

/* 026D821C */
void hdSequenceOption_c::refreshLabels() {
    u32 count = mEntriesPerPage;
    u32 start = mPageStart;
    for (u32 index = 0; index < count; ++index) {
        u32 selection = start + index;
        if (selection < mItemCount) {
            setItemLabel(selection);
            setButtonLabel(selection);
            count = mEntriesPerPage;
            start = mPageStart;
        }
    }
    setSettingLabel(start);
    setHelpLabel();
}

static hdOptionPaneNames_l sOptionPaneNames;
static hdOptionTextPaneNames_l sOptionTextPaneNames;
static hdOptionItemPaneNames_l sOptionItemPaneNames;
static hdUiSafeString sOptionSettingPaneName;
static hdUiSafeString sOptionHelpPaneName;
static hdPartsPane_c* optionFindPane(hdPartsPane_c* root, hdUiSafeString* name) {
    hdPartsPaneSRTVtbl_c* vtable = reinterpret_cast<hdPartsPaneSRTVtbl_c*>(root->mVtable.get());
    name->assureTermination();
    return gabi::call_ptr<hdPartsPane_c*>(vtable->mFindPane.get(), root, name->mText.get(), 1);
}

/* 026D7B30 */
void hdSequenceOption_c::findPanes() {
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    for (u32 index = 0; index < PANE_COUNT; ++index)
        mPanes[index] = optionFindPane(root, &sOptionPaneNames.mNames[index]);
}

/* 026D7C0C */
void hdSequenceOption_c::findTextPanes() {
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    for (u32 index = 0; index < TEXT_PANE_COUNT; ++index)
        mTextPanes[index] = reinterpret_cast<hdTextPane_c*>(optionFindPane(root, &sOptionTextPaneNames.mNames[index]));
}

/* 026D7CE8 */
void hdSequenceOption_c::findItemTextPanes() {
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    for (u32 index = 0; index < ITEM_PANE_COUNT; ++index)
        mItemTextPanes[index] = reinterpret_cast<hdTextPane_c*>(optionFindPane(root, &sOptionItemPaneNames.mNames[index]));
    hdTextPane_c* pane = reinterpret_cast<hdTextPane_c*>(optionFindPane(root, &sOptionSettingPaneName));
    mpSettingLabel = pane;
    pane->setCharacterCapacity(0x85);
    mpHelpLabel = reinterpret_cast<hdTextPane_c*>(optionFindPane(root, &sOptionHelpPaneName));
}

/* 026D62B0 */
void hdSequenceOption_c::buttonWaitEnter() {
    u32 childIndex = buttonIndex(mSelection);
    hdButtonPart_c* button = reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(childIndex)->get());
    void* receiver = button->mpReceiver;
    sOptionEventManager->setModeDefault(gabi::ea(receiver));
}

/* 026D6650 */
void hdSequenceOption_c::changeSetting(u32 increment) {
    u32 selection = mSelection;
    u32 count = settingValueCount(selection);
    if (count <= 1)
        return;
    u32 slot = selection < SETTING_COUNT ? selection : 0;
    if (increment != 0) {
        mSettingValues[slot] = u32(mSettingValues[slot]) + 1;
        selection = mSelection;
        slot = selection < SETTING_COUNT ? selection : 0;
        if (u32(mSettingValues[slot]) >= count) {
            mSettingValues[slot] = 0;
            selection = mSelection;
        }
    } else if (mSettingValues[slot] == 0) {
        mSettingValues[slot] = count - 1;
        selection = mSelection;
    } else {
        mSettingValues[slot] = u32(mSettingValues[slot]) - 1;
        selection = mSelection;
    }
    const auto bits = settingAction(selection);
    gabi::NativeFrame<0x18> frame;
    hdOptionMemberAction_l* words = hdPartsView<hdOptionMemberAction_l>(frame.a - 8);
    words->mWords[0] = bits.r3;
    words->mWords[1] = bits.r4;
    auto* action = reinterpret_cast<gabi::ptmf<void (hdSequenceOption_c::*)()>*>(words);
    if (action->i != 0) {
        (this->* *action)();
        selection = mSelection;
    }
    setButtonLabel(selection);
}

using hdOptionInputManagerPointer = gptr<hdInputManager>;
static hdOptionInputManagerPointer sOptionInputManager;

/* 026D5CE4 */
void hdSequenceOption_c::loadSettings() {
    if (mMode == MODE_SETTINGS) {
        hdSaveSlotFlags_c* options = optionSaveHolders()->getScratchFlags();
        mSettingValues[0] = options->mProgress[0];
        mSettingValues[1] = options->mProgress[1];
        mSettingValues[2] = options->mProgress[2];
        mSettingValues[3] = hdPartsView<hdInputManager>(gabi::ea(sOptionInputManager.get()))->mMode == MODE_SETTINGS ? 1 : 0;
        mSettingValues[4] = options->getCreated();
        mSettingValues[5] = options->getChanged();
    } else if (mMode == MODE_CONTROLLER) {
        mSettingValues[0] = optionSaveHolders()->getScratchPair()->mPrimary;
        mSettingValues[1] = optionSaveHolders()->getScratchPair()->mSecondary;
        updateAlternateButton();
    }
    for (u32 index = 0; index < u32(mEntriesPerPage); ++index) {
        u32 selection = mPageStart + index;
        if (selection < u32(mItemCount)) {
            setItemLabel(selection);
            setButtonLabel(selection);
        }
    }
}

static hdPartsStateId_c sOptionChangeConnectState;
static hdOptionAnimationGroups_l sOptionAnimationGroups;
static f32 sOptionReverseAnimationRate;
extern void optionPlaySound(const char*);
static char sOptionOpenSound;

/* 026D617C */
void hdSequenceOption_c::inAllEnter() {
    f32 rate = sOptionAnimationRate;
    hdAnimSet_play(mpLayout->animSet(), 0, 0, rate);
    hdAnimSet_play(mpLayout->animSet(), 1, 1, rate);
    hdAnimSet_play(mpLayout->animSet(), 2, 2, rate);
    loadSettings();
    updateCursor();
    optionGetPlayButtons()->mActionButtonA = 0x4B;
    optionGetPlayButtons()->mActionButtonB = 0x4C;
    sOptionInputPrompt = 3;
    optionPlaySound(reinterpret_cast<const char*>(&sOptionOpenSound));
}

/* 026D6F1C */
void hdSequenceOption_c::scrollEnter() {
    u32 start = mPageStart;
    u32 pageSize = mEntriesPerPage;
    f32 rate;
    u32 animation;
    if (mScrollForward) {
        rate = sOptionAnimationRate;
        u32 quotient = pageSize ? start / pageSize : 0;
        animation = start - quotient * pageSize + 6;
        mScrollAnimation = animation;
        mSelection = start + 4;
    } else {
        rate = sOptionReverseAnimationRate;
        u32 previous = start - 1;
        u32 quotient = pageSize ? previous / pageSize : 0;
        animation = previous - quotient * pageSize + 6;
        mSelection = previous;
        mScrollAnimation = animation;
    }
    u32 group = *hdPartsView<be<u32>>(gabi::ea(&sOptionAnimationGroups) + animation * 4);
    hdAnimSet_play(mpLayout->animSet(), animation, group, rate);
    setItemLabel(mSelection);
    setButtonLabel(mSelection);
}

/* 026D87D0 */
u32 hdSequenceOption_c::beginConnection() {
    u32 result;
    if (mSettingValues[3] == 1) {
        mPreviousCategory = 0;
        result = sOptionInputManager->hasDrc();
        mConnectionMessage = result ? 3 : 2;
    } else {
        mPreviousCategory = 1;
        result = sOptionInputManager->hasPad();
        mConnectionMessage = result;
    }
    hdParts_changeState(stateMachine(), &sOptionChangeConnectState);
    return result;
}

extern u32 optionButtonHasSelection(hdParts_c*);

// The canonical transform fields retain raw words; this controller uses lfs/stfs.
static be<f32>& optionPaneTranslationX(hdPartsPaneSRT_c* pane) {
    return *reinterpret_cast<be<f32>*>(&pane->mTranslationXBits);
}
static be<f32>& optionPaneTranslationY(hdPartsPaneSRT_c* pane) {
    return *reinterpret_cast<be<f32>*>(&pane->mTranslationYBits);
}
static be<f32>& optionPaneTranslationZ(hdPartsPaneSRT_c* pane) {
    return *reinterpret_cast<be<f32>*>(&pane->mTranslationZBits);
}

/* 026D5EC4 */
void hdSequenceOption_c::updateCursor() {
    gabi::NativeFrame<0x28> frame;
    u32 childIndex = buttonIndex(mSelection);
    u32 slot = childIndex < ITEM_PANE_COUNT ? childIndex : 0;
    auto* text = reinterpret_cast<hdPartsPaneSRT_c*>(mItemTextPanes[slot].get());
    f32 y = optionPaneTranslationY(text);
    auto* cursor = reinterpret_cast<hdPartsPaneSRT_c*>(mPanes[0].get());
    f32 z = optionPaneTranslationZ(cursor);
    f32 x = optionPaneTranslationX(cursor);
    cursor = reinterpret_cast<hdPartsPaneSRT_c*>(mPanes[0].get());
    optionPaneTranslationX(cursor) = x;
    optionPaneTranslationY(cursor) = y;
    u8 flags = cursor->mFlags;
    optionPaneTranslationZ(cursor) = z;
    cursor->mFlags = flags | OPTION_PANE_DIRTY;
    text = reinterpret_cast<hdPartsPaneSRT_c*>(mItemTextPanes[slot].get());
    f32 halfWidth = mItemHalfWidths[slot];
    f32 left = gabi::fsubs_ppc(optionPaneTranslationX(text), halfWidth);
    for (u32 index = 1; index <= 2; ++index) {
        auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mPanes[index].get());
        // GHS saves each position word to its guest stack temporary in order.
        auto* position = hdPartsView<hdOptionPosition_l>(frame.a - 8);
        position->mWords[0] = pane->mTranslationXBits;
        position->mWords[1] = pane->mTranslationYBits;
        position->mWords[2] = pane->mTranslationZBits;
        pane = reinterpret_cast<hdPartsPaneSRT_c*>(mPanes[index].get());
        optionPaneTranslationY(pane) = y;
        optionPaneTranslationX(pane) = left;
        // The GHS stack copy is reloaded through lfs before stfs.
        f32 restoredZ = *reinterpret_cast<be<f32>*>(&position->mWords[2]);
        flags = pane->mFlags;
        optionPaneTranslationZ(pane) = restoredZ;
        pane->mFlags = flags | OPTION_PANE_DIRTY;
        f32 width = mItemWidths[slot];
        f32 height = mItemHeights[slot];
        pane = reinterpret_cast<hdPartsPaneSRT_c*>(mPanes[index].get());
        flags = pane->mFlags;
        pane->mSizeY = height;
        pane->mSizeX = width;
        pane->mFlags = flags | OPTION_PANE_DIRTY;
        pane = reinterpret_cast<hdPartsPaneSRT_c*>(mPanes[index].get());
        pane->mFlags = (u8(pane->mFlags) & ~OPTION_PANE_VISIBLE) + 1;
    }
    hdParts_c* child = mChildren.safeSlot(childIndex)->get();
    u32 valueCount = settingValueCount(mSelection);
    bool visible = valueCount > 1 && optionButtonHasSelection(child) != 0;
    cursor = reinterpret_cast<hdPartsPaneSRT_c*>(mPanes[0].get());
    cursor->mFlags = (u8(cursor->mFlags) & ~OPTION_PANE_VISIBLE) + (visible ? 1 : 0);
    setSettingLabel(mSelection);
}

/* 026D7370 */
void hdSequenceOption_c::scrollExit() {
    if (mScrollForward) {
        mPageStart = u32(mPageStart) + 1;
        optionSetButtonEnabled(mChildren.safeSlot(7)->get(), 1);
        if (u32(mPageStart) >= u32(mItemCount) - 4)
            optionSetButtonEnabled(mChildren.safeSlot(8)->get(), 0);
    } else {
        mPageStart = u32(mPageStart) - 1;
        if (mPageStart == 0)
            optionSetButtonEnabled(mChildren.safeSlot(7)->get(), 0);
        optionSetButtonEnabled(mChildren.safeSlot(8)->get(), 1);
    }
    bindInputButtons();
}

static hdPartsStateId_c sOptionConnectedWaitState;

/* 026D74E0 */
void hdSequenceOption_c::changeConnectExecute() {
    if (mConnectionDelay != 0) {
        mConnectionDelay = u32(mConnectionDelay) - 1;
        if (mConnectionDelay == 0) {
            hdAnimSet_play(mpLayout->animSet(), 11, 0, sOptionAnimationRate);
            vExecuteChildren();
            return;
        }
    } else if (hdAnimSet_isEnd(mpLayout->animSet(), 11)) {
        u32 message = mConnectionMessage;
        if (message == 1 || message == 3) {
            hdParts_changeState(stateMachine(), &sOptionConnectedWaitState);
            vExecuteChildren();
            return;
        }
        hdParts_changeState(stateMachine(), &sOptionConnectWaitState);
    }
    vExecuteChildren();
}

extern void optionCopyEvent(hdUiEvent*, hdUiEvent*);
extern u32 optionIsCancelEvent(hdUiEvent*);
static hdOptionPoint_l sOptionDefaultTouch;

/* 026D681C */
u32 hdSequenceOption_c::pollCancel() {
    gabi::NativeFrame<0x50> frame;
    auto* event = hdPartsView<hdUiEvent>(frame.a + 0x10);
    auto* current = optionCurrentEvent(sOptionEventManager.get());
    optionCopyEvent(event, current);
    if (optionIsCancelEvent(event))
        return 1;
    auto* pointer = hdPartsView<cking::ui::input::Event>(
        gabi::ea(sOptionInputManager.get()) + offsetof(hdInputManager, mPointer));
    auto* source = reinterpret_cast<cking::ui::input::Vec2*>(&sOptionDefaultTouch);
    if (pointer->mHold & OPTION_INPUT_TOUCH) {
        if (pointer->mPointerFlags & 1)
            source = &pointer->mTouch;
        auto* point = hdPartsView<hdOptionPoint_l>(frame.a - 8);
        u32 x = source->words[0];
        u32 y = source->words[1];
        point->mWords[0] = x;
        point->mWords[1] = y;
        if (isPointOutside(point))
            mOutsideTouchArmed = 1;
        return 0;
    }
    if (pointer->mRepeat & OPTION_INPUT_TOUCH) {
        if (pointer->mPointerFlags & 1)
            source = &pointer->mTouch;
        auto* point = hdPartsView<hdOptionPoint_l>(frame.a);
        u32 x = source->words[0];
        u32 y = source->words[1];
        point->mWords[0] = x;
        point->mWords[1] = y;
        if (!isPointOutside(point))
            mOutsideTouchArmed = 0;
        return 0;
    }
    if (pointer->mRelease & OPTION_INPUT_TOUCH) {
        auto* point = hdPartsView<hdOptionPoint_l>(frame.a + 8);
        u32 x = pointer->mTouch.words[0];
        u32 y = pointer->mTouch.words[1];
        point->mWords[0] = x;
        point->mWords[1] = y;
        if (isPointOutside(point) && mOutsideTouchArmed)
            return 1;
    }
    return 0;
}

/* 026D6480 */
s32 hdSequenceOption_c::findFocusedSelection() {
    u32 receiver = sOptionEventManager->getMode();
    u32 childCount = mChildren.mNum;
    gptr<hdParts_c>* children = mChildren.mpData;
    mPreviousSelection = -1;
    auto* previous = reinterpret_cast<hdButtonPart_c*>(children[7 < childCount ? 7 : 0].get());
    if (gabi::ea(previous->mpReceiver.get()) == receiver) {
        mPreviousSelection = 7;
        return -1;
    }
    auto* next = reinterpret_cast<hdButtonPart_c*>(children[8 < childCount ? 8 : 0].get());
    if (gabi::ea(next->mpReceiver.get()) == receiver) {
        mPreviousSelection = 8;
        return -1;
    }
    u32 count = mEntriesPerPage;
    if (mCategory == 0)
        --count;
    if (count == 0)
        return -1;
    u32 start = mPageStart;
    for (u32 index = 0; index != count; ++index) {
        u32 selection = start + index;
        u32 childIndex = buttonIndex(selection);
        auto* child = reinterpret_cast<hdButtonPart_c*>(children[childIndex < childCount ? childIndex : 0].get());
        if (child && gabi::ea(child->mpReceiver.get()) == receiver)
            return selection;
    }
    return -1;
}

static hdOptionArchiveNames_l sOptionArchiveNames;
static hdUiSafeString sOptionLayoutName;

/* 026D7E4C */
u32 hdSequenceOption_c::setup(u32 a, u32 b, u32 c) {
    attachArchives(sOptionArchiveNames.mNames, 3);
    if (!vLoad(&sOptionLayoutName, gabi::ea(mpResAccessor.get()), a, b, hdPartsView<void>(c)))
        return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this))
        return 0;
    findPanes();
    findTextPanes();
    findItemTextPanes();
    for (u32 index = 0; index < BUTTON_COUNT; ++index)
        reinterpret_cast<hdBasicButton_c*>(mChildren.safeSlot(index)->get())->mRequestedState = 1;
    reinterpret_cast<hdOptionButtonState_l*>(mChildren.safeSlot(7)->get())->mScrollDirection = 0;
    reinterpret_cast<hdOptionButtonState_l*>(mChildren.safeSlot(7)->get())->mScrollButton = 1;
    reinterpret_cast<hdOptionButtonState_l*>(mChildren.safeSlot(8)->get())->mScrollDirection = 1;
    reinterpret_cast<hdOptionButtonState_l*>(mChildren.safeSlot(8)->get())->mScrollButton = 1;
    startState(&sOptionBlankState);
    return 1;
}

extern void optionButtonPlayAnimation(hdParts_c*, hdUiSafeString*, u32);
static char sOptionEmptyAnimation;
static hdUiSafeString sOptionArrowAnimation;

/* 026D82C8 */
void hdSequenceOption_c::clearButtonAnimations() {
    gabi::NativeFrame<0x18> frame;
    u32 count = mEntriesPerPage;
    u32 childCount = mChildren.mNum;
    gptr<hdParts_c>* children = mChildren.mpData;
    for (u32 index = 0; index < count; ++index) {
        u32 childIndex = buttonIndex(index);
        hdParts_c* button = children[childIndex < childCount ? childIndex : 0].get();
        if (button) {
            auto* name = hdPartsView<hdUiSafeString>(frame.a - 8);
            name->mVtable = &sOptionStringVtable;
            name->mText = reinterpret_cast<be<u8>*>(&sOptionEmptyAnimation);
            optionButtonPlayAnimation(button, name, 0);
            count = mEntriesPerPage;
            children = mChildren.mpData;
            childCount = mChildren.mNum;
        }
    }
    hdParts_c* previous = children[7 < childCount ? 7 : 0].get();
    if (previous) {
        optionButtonPlayAnimation(previous, &sOptionArrowAnimation, 0);
        childCount = mChildren.mNum;
        children = mChildren.mpData;
    }
    hdParts_c* next = children[8 < childCount ? 8 : 0].get();
    if (next)
        optionButtonPlayAnimation(next, &sOptionArrowAnimation, 0);
}

using hdOptionUiSystemPointer = gptr<hdScreenUiSystem>;
static hdOptionUiSystemPointer sOptionUiSystem;
static hdOptionAnimationNames_l sOptionAnimationNames;
static hdOptionAnimationGroupNames_l sOptionAnimationGroupNames;
extern void optionBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);

/* 026D8AB0 */
u32 hdSequenceOption_c::load(void* name, u32 resource, u32 a, u32 b, void* descriptor) {
    auto* layout = static_cast<hdPartsLayout_c*>(hdScreenAllocate(sizeof(hdPartsLayout_c), sOptionUiSystem->mHeap, 4));
    if (layout)
        layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, resource, a, b, ANIMATION_COUNT, ANIMATION_GROUP_COUNT, descriptor))
        return 0;
    for (u32 index = 0; index < ANIMATION_COUNT; ++index) {
        u32 group = sOptionAnimationGroups.mGroups[index];
        optionBindAnimation(mpLayout->animSet(), index, &sOptionAnimationNames.mNames[index],
            hdPartsView<hdUiSafeString>(gabi::ea(&sOptionAnimationGroupNames) + group * sizeof(hdUiSafeString)));
    }
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    return 1;
}

static hdUiSafeString sOptionSequenceButtonClass;
extern hdParts_c* optionConstructSequenceButton(hdParts_c*, u32, hdPartsDesc_c*);

static bool optionSameName(hdUiSafeString* left, hdUiSafeString* right) {
    left->assureTermination();
    left->assureTermination();
    be<u8>* text = left->mText;
    right->assureTermination();
    if (text == right->mText.get())
        return true;
    be<u8>* a = left->mText;
    be<u8>* b = right->mText;
    for (u32 index = 0; index < 0x40001; ++index) {
        u8 ca = a[index];
        u8 cb = b[index];
        if (ca != cb)
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}

/* 026D8878 */
u32 hdSequenceOption_c::createOptionChild(hdPartsDesc_c* descriptor) {
    sOptionSequenceButtonClass.assureTermination();
    be<u8>* className = sOptionSequenceButtonClass.mText;
    auto* details = reinterpret_cast<hdOptionChildDescriptor_l*>(descriptor);
    gabi::NativeFrame<0x48> frame;
    for (u32 index = 0; index < TEXT_PANE_COUNT; ++index) {
        if (!optionSameName(&details->mPaneName, &sOptionTextPaneNames.mNames[index]))
            continue;
        auto* expectedClass = hdPartsView<hdUiSafeString>(frame.a - 8);
        expectedClass->mText = className;
        expectedClass->mVtable = &sOptionStringVtable;
        if (!optionSameName(&details->mArchive, expectedClass))
            return hdParts_c::createChild(index, descriptor);
        gptr<hdParts_c>* slot = mChildren.safeSlot(index);
        *slot = optionConstructSequenceButton(NULL, 0, descriptor);
        return mChildren.safeSlot(index)->get() != NULL;
    }
    return 1;
}

static hdPartsStateId_c sOptionInAllState;
static f64 sOptionUnsignedBias;
extern u32 optionDirectionalInputIsEnabled();

/* 026D83F0 */
void hdSequenceOption_c::open(u32 mode) {
    if (!optionIsBlank(this))
        return;
    mMode = mode;
    if (mode == MODE_CONTROLLER) {
        mCategory = 2;
        mEntriesPerPage = 2;
        mItemCount = 2;
        f32 frame = f32(4503599627370496.0 + 2.0 - sOptionUnsignedBias);
        hdAnimSet_setFrame(mpLayout->animSet(), 5, 3, frame);
    } else {
        mCategory = 0;
        mEntriesPerPage = 5;
        u32 category = mCategory;
        f32 frame = f32(4503599627370496.0 + f64(category) - sOptionUnsignedBias);
        mItemCount = 6;
        hdAnimSet_setFrame(mpLayout->animSet(), 5, 3, frame);
    }
    hdAnimSet_operationCC(mpLayout->animSet(), 6, 6);
    u32 slot = mMode < MODE_COUNT ? u32(mMode) : 0;
    mSelection = mSavedSelections[slot];
    slot = mMode < MODE_COUNT ? u32(mMode) : 0;
    u32 start = mSavedPageStarts[slot];
    mPageStart = start;
    if (start == 0) {
        optionSetButtonEnabled(mChildren.safeSlot(7)->get(), 0);
        optionSetButtonEnabled(mChildren.safeSlot(8)->get(), 1);
    } else if (start >= u32(mItemCount) - 4) {
        optionSetButtonEnabled(mChildren.safeSlot(7)->get(), 1);
        optionSetButtonEnabled(mChildren.safeSlot(8)->get(), 0);
    } else {
        optionSetButtonEnabled(mChildren.safeSlot(7)->get(), 1);
        optionSetButtonEnabled(mChildren.safeSlot(8)->get(), 1);
    }
    if (mCategory == 0) {
        u32 pageSize = mEntriesPerPage;
        u32 page = mPageStart;
        u32 quotient = pageSize ? page / pageSize : 0;
        u32 animation = page - quotient * pageSize + 6;
        mScrollAnimation = animation;
        u32 group = *hdPartsView<be<u32>>(gabi::ea(&sOptionAnimationGroups) + animation * 4);
        hdAnimSet_setFrame(mpLayout->animSet(), animation, group, sOptionZero);
    }
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    refreshLabels();
    bindInputButtons();
    sOptionEventManager->setModeDefault(0);
    dOwPlayView_c* play = optionGetPlayButtons();
    u8 actionA = play->mActionButtonA;
    u8 actionB = play->mActionButtonB;
    play->mSavedActionButtonA = actionA;
    u8 actionR = play->mActionButtonR;
    play->mSavedActionButtonB = actionB;
    play->mSavedActionButtonR = actionR;
    sOptionInputPrompt = 0;
    sOptionTouchMode = 1;
    mTouchStarted = 0;
    mDirectionalInputEnabled = 0;
    if (!optionDirectionalInputIsEnabled()) {
        optionEnableDirectionalInput(1);
        hdPartsView<cking::ui::input::Options>(gabi::ea(sOptionInputOptions.get()))->mDirectionalInputEnabled = 1;
        mDirectionalInputEnabled = 1;
    }
    mCloseRequested = 0;
    mOutsideTouchArmed = 0;
    clearButtonAnimations();
    hdParts_changeState(stateMachine(), &sOptionInAllState);
}

using hdOptionPromptRootPointer = gptr<hdOptionPromptRoot_l>;
static hdOptionPromptRootPointer sOptionPromptRoot;
static hdOptionPromptIds_l sOptionPromptIds;
static hdOptionConnectionSounds_l sOptionConnectionSounds;
extern void optionOpenPrompt(hdOptionPromptStatus_l*, u32, u32);

/* 026D7630 */
void hdSequenceOption_c::connectWaitEnter() {
    u32 message = mConnectionMessage;
    auto* root = hdPartsView<hdOptionPromptRoot_l>(gabi::ea(sOptionPromptRoot.get()));
    u32 id = *hdPartsView<be<u32>>(gabi::ea(&sOptionPromptIds) + message * 4);
    optionOpenPrompt(root->mpPrompt, id, 1);
    message = mConnectionMessage;
    if (message <= 3)
        optionPlaySound(sOptionConnectionSounds.mNames[message]);
    mConnectionCounter = 0;
}

/* 026D76A4 */
void hdSequenceOption_c::connectWaitExecute() {
    u32 count = mConnectionCounter;
    auto* root = hdPartsView<hdOptionPromptRoot_l>(gabi::ea(sOptionPromptRoot.get()));
    hdOptionPromptStatus_l* prompt = root->mpPrompt;
    if (count < 1) {
        if (prompt->mWaitComplete) {
            mConnectionCounter = 1;
            mCloseRequested = 1;
        }
    } else if (count == 1) {
        if (!mCloseRequested) {
            prompt->mWaitComplete = 0;
            mConnectionCounter = 2;
        }
    }
    if (optionIsBlank(prompt) || prompt->mAdvanceRequested) {
        if (prompt->mInputAccepted) {
            outAllExit();
            vExecuteChildren();
            return;
        }
        hdParts_changeState(stateMachine(), &sOptionInAllState);
    }
    vExecuteChildren();
}

static u32 optionButtonReceiver(hdParts_c* button) {
    auto* input = reinterpret_cast<hdButtonPart_c*>(button);
    return gabi::ea(input->mpReceiver.get());
}

/* 026D70BC */
void hdSequenceOption_c::bindInputButtons() {
    u32 previous = 0;
    if (mCategory == 0) {
        u32 first = buttonIndex(mPageStart);
        bindButton(7, 0, optionButtonReceiver(mChildren.safeSlot(first)->get()));
        u32 childCount = mChildren.mNum;
        gptr<hdParts_c>* children = mChildren.mpData;
        previous = optionButtonReceiver(children[7 < childCount ? 7 : 0].get());
        u32 count = mEntriesPerPage;
        for (u32 index = 0; index < count - 1; ++index) {
            u32 start = mPageStart;
            u32 next;
            if (index < count - 2) {
                u32 nextIndex = buttonIndex(start + index + 1);
                next = optionButtonReceiver(children[nextIndex < childCount ? nextIndex : 0].get());
            } else {
                next = optionButtonReceiver(children[8 < childCount ? 8 : 0].get());
            }
            u32 childIndex = buttonIndex(start + index);
            bindButton(childIndex, previous, next);
            childCount = mChildren.mNum;
            children = mChildren.mpData;
            previous = optionButtonReceiver(children[childIndex < childCount ? childIndex : 0].get());
            count = mEntriesPerPage;
        }
        bindButton(8, previous, 0);
    } else {
        for (u32 index = 0; index < u32(mEntriesPerPage); ++index) {
            u32 next = 0;
            if (index < u32(mEntriesPerPage) - 1) {
                u32 nextIndex = buttonIndex(index + 1);
                next = optionButtonReceiver(mChildren.safeSlot(nextIndex)->get());
            }
            u32 childIndex = buttonIndex(index);
            bindButton(childIndex, previous, next);
            previous = optionButtonReceiver(mChildren.safeSlot(childIndex)->get());
        }
    }
    sOptionEventManager->setParams(0, 8);
}

static hdPartsStateId_c sOptionScrollState;
static hdPartsStateId_c sOptionOutAllState;
static char sOptionScrollSound;
static char sOptionChangeSound;
static char sOptionCloseSound;
extern u32 optionIsConfirmEvent(hdUiEvent*);

static u32 optionReceiverId(hdParts_c* button) {
    auto* receiver = hdPartsView<cking::ui::input::Receiver>(optionButtonReceiver(button));
    return receiver->mId;
}
static bool optionMatchesFocus(hdParts_c* button) {
    auto* focused = hdPartsView<cking::ui::input::Receiver>(sOptionEventManager->getMode());
    return focused->mId == optionReceiverId(button);
}
static u32 optionAcceptsKind(hdParts_c* button, u32 kind) {
    auto* table = reinterpret_cast<hdOptionButtonVtable_l*>(button->mVtable.get());
    return gabi::call_ptr<u32>(table->mAcceptsKind.get(), button, kind);
}
static u32 optionAcceptsEvent(hdParts_c* button, hdUiEvent* event) {
    auto* table = reinterpret_cast<hdOptionButtonVtable_l*>(button->mVtable.get());
    return gabi::call_ptr<u32>(table->mAcceptsEvent.get(), button, event);
}

/* 026D6A58 */
void hdSequenceOption_c::buttonWaitExecute() {
    gabi::NativeFrame<0x48> frame;
    auto* event = hdPartsView<hdUiEvent>(frame.a - 8);
    optionCopyEvent(event, optionCurrentEvent(sOptionEventManager.get()));
    u32 kind = event->mKind;
    s32 previous = mPreviousSelection;
    s32 focused = findFocusedSelection();
    if (focused < 0) {
        if (mPreviousSelection >= 0) {
            if (u32(mPreviousSelection) != u32(previous)) {
                mTouchStarted = 1;
                for (u32 index = 0; index < 3; ++index) {
                    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mPanes[index].get());
                    pane->mFlags = u8(pane->mFlags) & ~OPTION_PANE_VISIBLE;
                }
            }
            hdParts_c* button = mChildren.safeSlot(mPreviousSelection)->get();
            if (button && optionButtonHasSelection(button)) {
                bool accepted = optionAcceptsKind(button, kind) && u32(event->mId) == optionReceiverId(button);
                if (!accepted && optionAcceptsEvent(button, event))
                    accepted = optionMatchesFocus(button);
                if (accepted) {
                    mScrollForward = mPreviousSelection == 7 ? 0 : 1;
                    optionPlaySound(reinterpret_cast<const char*>(&sOptionScrollSound));
                    hdParts_changeState(stateMachine(), &sOptionScrollState);
                    executeButtons();
                    vExecuteChildren();
                    return;
                }
            }
        }
    } else {
        u32 selection = mSelection;
        if (selection != u32(focused) || mTouchStarted) {
            mSelection = focused;
            mTouchStarted = 0;
            updateCursor();
            selection = mSelection;
        }
        u32 childIndex = buttonIndex(selection);
        u32 values = settingValueCount(selection);
        hdParts_c* button = mChildren.safeSlot(childIndex)->get();
        if (button && optionButtonHasSelection(button)) {
            bool accepted = false;
            u32 forward = 1;
            if (kind == EVENT_DECIDE) {
                accepted = u32(event->mId) == optionReceiverId(button);
            } else if (optionIsConfirmEvent(event)) {
                accepted = optionMatchesFocus(button);
            } else if (values != 0 && (kind == EVENT_NEXT || kind == EVENT_PREVIOUS)) {
                if (optionMatchesFocus(button)) {
                    if (kind == EVENT_NEXT) {
                        hdAnimSet_play(mpLayout->animSet(), 4, 5, sOptionAnimationRate);
                    } else {
                        forward = 0;
                        hdAnimSet_play(mpLayout->animSet(), 3, 4, sOptionAnimationRate);
                    }
                    accepted = true;
                }
            }
            if (accepted) {
                optionPlaySound(reinterpret_cast<const char*>(&sOptionChangeSound));
                changeSetting(forward);
                executeButtons();
                vExecuteChildren();
                return;
            }
        }
    }
    if (pollCancel()) {
        optionPlaySound(reinterpret_cast<const char*>(&sOptionCloseSound));
        hdParts_changeState(stateMachine(), &sOptionOutAllState);
    }
    executeButtons();
    vExecuteChildren();
}

static char sOptionHelpLabelText;
static hdOptionChildArchiveNames_l sOptionChildArchiveNames;
static hdOptionZeroVector_l sOptionZeroVector;
static hdOptionRegistration_l sOptionVectorRegistration;
static hdOptionRegistration_l sOptionFirstFlagRegistration;
static hdOptionRegistration_l sOptionSecondFlagRegistration;
static hdOptionLibraryFlag_l sOptionFirstLibraryFlag;
static hdOptionLibraryFlag_l sOptionSecondLibraryFlag;
static f32 sOptionMinimumAngle;
static f32 sOptionMaximumAngle;
static f32 sOptionLargeLimitX;
static f32 sOptionLargeLimitY;
static f32 sOptionSmallLimitX;
static f32 sOptionSmallLimitY;
static f32 sOptionNegativePi;
static f32 sOptionPositivePi;
static f32 sOptionLargeLimit;
static f32 sOptionSmallLimit;
static u32 sOptionStateCounterInitialized;
static hdPartsStateCounter_c sOptionStateCounter;
static hdPartsStateIdVtbl_c sOptionStateVtable;
static hdStateId sOptionNoStateId;
extern void optionRegisterGlobal(hdOptionRegistration_l*);
extern void optionConstructFirstFlag(hdOptionLibraryFlag_l*);
extern void optionConstructSecondFlag(hdOptionLibraryFlag_l*);

/* 026D8BDC */
void optionInitializeStatics() {
    sOptionZeroVector.mWords[3] = 0;
    sOptionZeroVector.mWords[2] = 0;
    sOptionZeroVector.mWords[1] = 0;
    sOptionZeroVector.mWords[0] = 0;
    optionRegisterGlobal(&sOptionVectorRegistration);
    sOptionMinimumAngle = sOptionNegativePi;
    sOptionMaximumAngle = sOptionPositivePi;
    optionConstructFirstFlag(&sOptionFirstLibraryFlag);
    optionRegisterGlobal(&sOptionFirstFlagRegistration);
    optionConstructSecondFlag(&sOptionSecondLibraryFlag);
    optionRegisterGlobal(&sOptionSecondFlagRegistration);
    f32 largeLimit = sOptionLargeLimit;
    f32 smallLimit = sOptionSmallLimit;
    sOptionLargeLimitX = largeLimit;
    sOptionLargeLimitY = largeLimit;
    sOptionSmallLimitY = smallLimit;
    sOptionSmallLimitX = smallLimit;

    // AnimationGroupNames.
    hdParts_setSafeString(&sOptionAnimationGroupNames.mNames[0], &sOptionStringVtable, "G_All_00");
    hdParts_setSafeString(&sOptionAnimationGroupNames.mNames[1], &sOptionStringVtable, "G_ContentCursor_00");
    hdParts_setSafeString(&sOptionAnimationGroupNames.mNames[2], &sOptionStringVtable, "G_Arrow_00");
    hdParts_setSafeString(&sOptionAnimationGroupNames.mNames[3], &sOptionStringVtable, "G_ContentPattern_00");
    hdParts_setSafeString(&sOptionAnimationGroupNames.mNames[4], &sOptionStringVtable, "G_ArrowL_00");
    hdParts_setSafeString(&sOptionAnimationGroupNames.mNames[5], &sOptionStringVtable, "G_ArrowR_00");
    hdParts_setSafeString(&sOptionAnimationGroupNames.mNames[6], &sOptionStringVtable, "G_ContentScroll_00");

    // AnimationNames.
    hdParts_setSafeString(&sOptionAnimationNames.mNames[0], &sOptionStringVtable, "InAll");
    hdParts_setSafeString(&sOptionAnimationNames.mNames[1], &sOptionStringVtable, "TitleCursorLoop");
    hdParts_setSafeString(&sOptionAnimationNames.mNames[2], &sOptionStringVtable, "ArrowLoop");
    hdParts_setSafeString(&sOptionAnimationNames.mNames[3], &sOptionStringVtable, "ArrowTransfer");
    hdParts_setSafeString(&sOptionAnimationNames.mNames[4], &sOptionStringVtable, "ArrowTransfer");
    hdParts_setSafeString(&sOptionAnimationNames.mNames[5], &sOptionStringVtable, "ContentPattern");
    hdParts_setSafeString(&sOptionAnimationNames.mNames[6], &sOptionStringVtable, "ContentScroll_00");
    hdParts_setSafeString(&sOptionAnimationNames.mNames[7], &sOptionStringVtable, "ContentScroll_01");
    hdParts_setSafeString(&sOptionAnimationNames.mNames[8], &sOptionStringVtable, "ContentScroll_02");
    hdParts_setSafeString(&sOptionAnimationNames.mNames[9], &sOptionStringVtable, "ContentScroll_03");
    hdParts_setSafeString(&sOptionAnimationNames.mNames[10], &sOptionStringVtable, "ContentScroll_04");
    hdParts_setSafeString(&sOptionAnimationNames.mNames[11], &sOptionStringVtable, "OutAll");

    // PaneNames.
    hdParts_setSafeString(&sOptionPaneNames.mNames[0], &sOptionStringVtable, "N_Arrow_00");
    hdParts_setSafeString(&sOptionPaneNames.mNames[1], &sOptionStringVtable, "W_ContentCursor_00");
    hdParts_setSafeString(&sOptionPaneNames.mNames[2], &sOptionStringVtable, "W_ContentCursor_01");
    hdParts_setSafeString(&sOptionPaneNames.mNames[3], &sOptionStringVtable, "N_OptionArea_00");

    // ItemPaneNames.
    hdParts_setSafeString(&sOptionItemPaneNames.mNames[0], &sOptionStringVtable, "T_Content_00");
    hdParts_setSafeString(&sOptionItemPaneNames.mNames[1], &sOptionStringVtable, "T_Content_01");
    hdParts_setSafeString(&sOptionItemPaneNames.mNames[2], &sOptionStringVtable, "T_Content_02");
    hdParts_setSafeString(&sOptionItemPaneNames.mNames[3], &sOptionStringVtable, "T_Content_03");
    hdParts_setSafeString(&sOptionItemPaneNames.mNames[4], &sOptionStringVtable, "T_Content_04");
    hdParts_setSafeString(&sOptionItemPaneNames.mNames[5], &sOptionStringVtable, "T_Content_05");
    hdParts_setSafeString(&sOptionItemPaneNames.mNames[6], &sOptionStringVtable, "T_Content_06");

    // TextPaneNames.
    hdParts_setSafeString(&sOptionTextPaneNames.mNames[0], &sOptionStringVtable, "L_BtnOption_00");
    hdParts_setSafeString(&sOptionTextPaneNames.mNames[1], &sOptionStringVtable, "L_BtnOption_01");
    hdParts_setSafeString(&sOptionTextPaneNames.mNames[2], &sOptionStringVtable, "L_BtnOption_02");
    hdParts_setSafeString(&sOptionTextPaneNames.mNames[3], &sOptionStringVtable, "L_BtnOption_03");
    hdParts_setSafeString(&sOptionTextPaneNames.mNames[4], &sOptionStringVtable, "L_BtnOption_04");
    hdParts_setSafeString(&sOptionTextPaneNames.mNames[5], &sOptionStringVtable, "L_BtnOption_05");
    hdParts_setSafeString(&sOptionTextPaneNames.mNames[6], &sOptionStringVtable, "L_BtnOption_06");
    hdParts_setSafeString(&sOptionTextPaneNames.mNames[7], &sOptionStringVtable, "L_BtnArrowT_00");
    hdParts_setSafeString(&sOptionTextPaneNames.mNames[8], &sOptionStringVtable, "L_BtnArrowB_00");

    // ChildArchiveNames.
    hdParts_setSafeString(&sOptionChildArchiveNames.mNames[0], &sOptionStringVtable, "BtnOption_00");
    hdParts_setSafeString(&sOptionChildArchiveNames.mNames[1], &sOptionStringVtable, "BtnOption_00");
    hdParts_setSafeString(&sOptionChildArchiveNames.mNames[2], &sOptionStringVtable, "BtnOption_00");
    hdParts_setSafeString(&sOptionChildArchiveNames.mNames[3], &sOptionStringVtable, "BtnOption_00");
    hdParts_setSafeString(&sOptionChildArchiveNames.mNames[4], &sOptionStringVtable, "BtnOption_00");
    hdParts_setSafeString(&sOptionChildArchiveNames.mNames[5], &sOptionStringVtable, "BtnOption_00");
    hdParts_setSafeString(&sOptionChildArchiveNames.mNames[6], &sOptionStringVtable, "BtnOption_00");
    hdParts_setSafeString(&sOptionChildArchiveNames.mNames[7], &sOptionStringVtable, "BtnMenuArrow_00");
    hdParts_setSafeString(&sOptionChildArchiveNames.mNames[8], &sOptionStringVtable, "BtnMenuArrow_00");

    // ArchiveNames.
    hdParts_setSafeString(&sOptionArchiveNames.mNames[0], &sOptionStringVtable, "SequenceOption_00");
    hdParts_setSafeString(&sOptionArchiveNames.mNames[1], &sOptionStringVtable, "BtnOption_00");
    hdParts_setSafeString(&sOptionArchiveNames.mNames[2], &sOptionStringVtable, "BtnMenuArrow_00");

    // Labels.
    hdParts_setSafeString(&sOptionLabels.mNames[0], &sOptionStringVtable, "T_Content_01");
    hdParts_setSafeString(&sOptionLabels.mNames[1], &sOptionStringVtable, "T_Msg_00_01");
    hdParts_setSafeString(&sOptionLabels.mNames[2], &sOptionStringVtable, "L_BtnOption_00-T_Btn_01");
    hdParts_setSafeString(&sOptionLabels.mNames[3], &sOptionStringVtable, "L_BtnOption_00-T_Btn_02");
    hdParts_setSafeString(&sOptionLabels.mNames[4], &sOptionStringVtable, "T_Content_02");
    hdParts_setSafeString(&sOptionLabels.mNames[5], &sOptionStringVtable, "T_Msg_00_02");
    hdParts_setSafeString(&sOptionLabels.mNames[6], &sOptionStringVtable, "L_BtnOption_01-T_Btn_01");
    hdParts_setSafeString(&sOptionLabels.mNames[7], &sOptionStringVtable, "L_BtnOption_01-T_Btn_02");
    hdParts_setSafeString(&sOptionLabels.mNames[8], &sOptionStringVtable, "T_Content_03");
    hdParts_setSafeString(&sOptionLabels.mNames[9], &sOptionStringVtable, "T_Msg_00_03");
    hdParts_setSafeString(&sOptionLabels.mNames[10], &sOptionStringVtable, "L_BtnOption_02-T_Btn_01");
    hdParts_setSafeString(&sOptionLabels.mNames[11], &sOptionStringVtable, "L_BtnOption_02-T_Btn_02");
    hdParts_setSafeString(&sOptionLabels.mNames[12], &sOptionStringVtable, "T_Content_04");
    hdParts_setSafeString(&sOptionLabels.mNames[13], &sOptionStringVtable, "T_Msg_00_04");
    hdParts_setSafeString(&sOptionLabels.mNames[14], &sOptionStringVtable, "L_BtnOption_03-T_Btn_02");
    hdParts_setSafeString(&sOptionLabels.mNames[15], &sOptionStringVtable, "L_BtnOption_03-T_Btn_01");
    hdParts_setSafeString(&sOptionLabels.mNames[16], &sOptionStringVtable, "T_Content_05");
    hdParts_setSafeString(&sOptionLabels.mNames[17], &sOptionStringVtable, "T_Msg_00_05");
    hdParts_setSafeString(&sOptionLabels.mNames[18], &sOptionStringVtable, "L_BtnOption_04-T_Btn_01");
    hdParts_setSafeString(&sOptionLabels.mNames[19], &sOptionStringVtable, "L_BtnOption_04-T_Btn_02");
    hdParts_setSafeString(&sOptionLabels.mNames[20], &sOptionStringVtable, "T_Content_08");
    hdParts_setSafeString(&sOptionLabels.mNames[21], &sOptionStringVtable, "T_Msg_00_08");
    hdParts_setSafeString(&sOptionLabels.mNames[22], &sOptionStringVtable, "L_BtnOption_08-T_Btn_02");
    hdParts_setSafeString(&sOptionLabels.mNames[23], &sOptionStringVtable, "L_BtnOption_08-T_Btn_01");
    hdParts_setSafeString(&sOptionLabels.mNames[24], &sOptionStringVtable, "T_Content_09");
    hdParts_setSafeString(&sOptionLabels.mNames[25], &sOptionStringVtable, "T_Msg_00_09");
    hdParts_setSafeString(&sOptionLabels.mNames[26], &sOptionStringVtable, "L_BtnOption_09-T_Btn_02");
    hdParts_setSafeString(&sOptionLabels.mNames[27], &sOptionStringVtable, "L_BtnOption_09-T_Btn_01");
    hdParts_setSafeString(&sOptionLabels.mNames[28], &sOptionStringVtable, "T_Content_06");
    hdParts_setSafeString(&sOptionLabels.mNames[29], &sOptionStringVtable, "T_Msg_00_06");
    hdParts_setSafeString(&sOptionLabels.mNames[30], &sOptionStringVtable, "L_BtnOption_06-T_Btn_01");
    hdParts_setSafeString(&sOptionLabels.mNames[31], &sOptionStringVtable, "L_BtnOption_06-T_Btn_02");
    hdParts_setSafeString(&sOptionLabels.mNames[32], &sOptionStringVtable, "L_BtnOption_06-T_Btn_03");
    hdParts_setSafeString(&sOptionLabels.mNames[33], &sOptionStringVtable, "T_Content_07");
    hdParts_setSafeString(&sOptionLabels.mNames[34], &sOptionStringVtable, "T_Msg_00_07");
    hdParts_setSafeString(&sOptionLabels.mNames[35], &sOptionStringVtable, "L_BtnOption_07-T_Btn_01");
    hdParts_setSafeString(&sOptionLabels.mNames[36], &sOptionStringVtable, "L_BtnOption_07-T_Btn_02");
    hdParts_setSafeString(&sOptionLabels.mNames[37], &sOptionStringVtable, reinterpret_cast<const char*>(&sOptionHelpLabelText));

    hdParts_setSafeString(&sOptionSettingPaneName, &sOptionStringVtable, "T_Msg_00");
    hdParts_setSafeString(&sOptionHelpPaneName, &sOptionStringVtable, "T_Header_00");
    hdParts_setSafeString(&sOptionButtonLabel, &sOptionStringVtable, "T_Btn_00");
    hdParts_setSafeString(&sOptionLayoutName, &sOptionStringVtable, "SequenceOption_00");

    u32 number;
    if (sOptionStateCounterInitialized) {
        number = sOptionStateCounter.mValue;
    } else {
        sOptionStateCounterInitialized = 1;
        number = 0;
    }
    sOptionInAllState.setup(++number, "::StateID_InAll", &sOptionStateVtable, 0x19, &sOptionNoStateId);
    sOptionOutAllState.setup(++number, "::StateID_OutAll", &sOptionStateVtable, 0x1C, &sOptionNoStateId);
    sOptionButtonWaitState.setup(++number, "::StateID_ButtonWait", &sOptionStateVtable, 0x1F, &sOptionNoStateId);
    sOptionScrollState.setup(++number, "::StateID_Scroll", &sOptionStateVtable, 0x22, &sOptionNoStateId);
    sOptionChangeConnectState.setup(++number, "::StateID_ChangeConnect", &sOptionStateVtable, 0x25, &sOptionNoStateId);
    sOptionConnectedWaitState.setup(++number, "::StateID_ConnectedWait", &sOptionStateVtable, 0x28, &sOptionNoStateId);
    sOptionConnectWaitState.setup(++number, "::StateID_ConnectWait", &sOptionStateVtable, 0x2B, &sOptionNoStateId);
    sOptionStateCounter.mValue = number;
}

/* WWHD glue
data sOptionHelpLabelText 10103474
str 10103534 "G_All_00"
str 10103540 "G_ContentCursor_00"
str 10103554 "G_Arrow_00"
str 10103448 "G_ContentPattern_00"
str 1010345C "G_ArrowL_00"
str 10103468 "G_ArrowR_00"
str 10103200 "InAll"
str 10103210 "TitleCursorLoop"
str 101034EC "ArrowLoop"
str 101034F8 "ArrowTransfer"
str 10103574 "ContentPattern"
str 10103560 "G_ContentScroll_00"
str 10103584 "ContentScroll_00"
str 10103598 "ContentScroll_01"
str 101035AC "ContentScroll_02"
str 101035C0 "ContentScroll_03"
str 101035E8 "N_Arrow_00"
str 101035D4 "ContentScroll_04"
str 10103208 "OutAll"
str 101035F4 "W_ContentCursor_00"
str 10103608 "W_ContentCursor_01"
str 1010361C "T_Content_00"
str 1010362C "T_Content_01"
str 1010363C "T_Content_02"
str 1010364C "T_Content_03"
str 1010365C "T_Content_04"
str 1010368C "L_BtnOption_00"
str 1010369C "L_BtnOption_01"
str 101036AC "L_BtnOption_02"
str 101036BC "L_BtnOption_03"
str 101036CC "L_BtnOption_04"
str 101036DC "L_BtnOption_05"
str 1010371C "BtnOption_00"
str 1010366C "T_Content_05"
str 1010367C "T_Content_06"
str 10103220 "N_OptionArea_00"
str 101036EC "L_BtnOption_06"
str 101036FC "L_BtnArrowT_00"
str 1010370C "L_BtnArrowB_00"
str 10103230 "BtnMenuArrow_00"
str 10103480 "T_Msg_00_01"
str 10103240 "L_BtnOption_00-T_Btn_01"
str 10103258 "L_BtnOption_00-T_Btn_02"
str 1010372C "T_Msg_00"
str 10103508 "SequenceOption_00"
str 1010348C "T_Msg_00_02"
str 10103270 "L_BtnOption_01-T_Btn_01"
str 10103288 "L_BtnOption_01-T_Btn_02"
str 10103498 "T_Msg_00_03"
str 101032A0 "L_BtnOption_02-T_Btn_01"
str 10103474 "T_Header_00"
str 101032B8 "L_BtnOption_02-T_Btn_02"
str 101034A4 "T_Msg_00_04"
str 101032D0 "L_BtnOption_03-T_Btn_02"
str 101032E8 "L_BtnOption_03-T_Btn_01"
str 101034B0 "T_Msg_00_05"
str 10103300 "L_BtnOption_04-T_Btn_01"
str 10103318 "L_BtnOption_04-T_Btn_02"
str 10103738 "T_Content_08"
str 101034BC "T_Msg_00_08"
str 10103330 "L_BtnOption_08-T_Btn_02"
str 10103348 "L_BtnOption_08-T_Btn_01"
str 10103748 "T_Content_09"
str 101034C8 "T_Msg_00_09"
str 10103360 "L_BtnOption_09-T_Btn_02"
str 10103378 "L_BtnOption_09-T_Btn_01"
str 101034D4 "T_Msg_00_06"
str 10103390 "L_BtnOption_06-T_Btn_01"
str 101033A8 "L_BtnOption_06-T_Btn_02"
str 101033C0 "L_BtnOption_06-T_Btn_03"
str 10103758 "T_Content_07"
str 101034E0 "T_Msg_00_07"
str 10103768 "T_Btn_00"
str 101033D8 "L_BtnOption_07-T_Btn_01"
str 101033F0 "L_BtnOption_07-T_Btn_02"
str 10103508 "SequenceOption_00"
str 10103408 "::StateID_InAll"
str 10103774 "::StateID_OutAll"
str 10103788 "::StateID_ButtonWait"
str 101037A0 "::StateID_Scroll"
str 10103418 "::StateID_ChangeConnect"
str 10103430 "::StateID_ConnectedWait"
str 1010351C "::StateID_ConnectWait"
ext 028F026C void optionRegisterGlobal(hdOptionRegistration_l*)
ext 028ED6F8 void optionConstructFirstFlag(hdOptionLibraryFlag_l*)
ext 028EAB2C void optionConstructSecondFlag(hdOptionLibraryFlag_l*)
data sOptionChildArchiveNames 1049AE24
data sOptionZeroVector 1049AB94
data sOptionVectorRegistration 101F6A7C
data sOptionFirstFlagRegistration 101F6A88
data sOptionSecondFlagRegistration 101F6A94
data sOptionFirstLibraryFlag 1049AB90
data sOptionSecondLibraryFlag 1049AB91
data sOptionMinimumAngle 1049AB78
data sOptionMaximumAngle 1049AB7C
data sOptionLargeLimitX 1049AB80
data sOptionLargeLimitY 1049AB84
data sOptionSmallLimitX 1049AB88
data sOptionSmallLimitY 1049AB8C
data sOptionNegativePi 101031F0
data sOptionPositivePi 101031F4
data sOptionLargeLimit 101031F8
data sOptionSmallLimit 101031FC
data sOptionStateCounterInitialized 101FD8E4
data sOptionStateCounter 101FDD50
data sOptionStateVtable 10103034
data sOptionNoStateId 101FF32C
ext 026F6288 u32 optionIsConfirmEvent(hdUiEvent*)
data sOptionScrollState 1049AEFC
data sOptionOutAllState 1049AE9C
data sOptionScrollSound 10103154
data sOptionChangeSound 1010313C
data sOptionCloseSound 10103170
ext 026DBA08 void optionOpenPrompt(hdOptionPromptStatus_l*,u32,u32)
data sOptionPromptRoot 101F8344
data sOptionPromptIds 10103188
data sOptionConnectionSounds 10103198
ext 025986BC u32 optionDirectionalInputIsEnabled()
data sOptionInAllState 1049AE6C
data sOptionUnsignedBias 101031E8
ext 026D969C hdParts_c* optionConstructSequenceButton(hdParts_c*,u32,hdPartsDesc_c*)
ext 026F92C0 u32 hdParts_c::createChild(u32,hdPartsDesc_c*)
data sOptionSequenceButtonClass 10490768
ext 0273B050 void* hdScreenAllocate(u32,hdScreenHeap*,u32)
ext 02004E04 void optionBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
data sOptionUiSystem 1018C404
data sOptionAnimationNames 1049AD7C
data sOptionAnimationGroupNames 1049ABA4
ext 026F2540 void optionButtonPlayAnimation(hdParts_c*,hdUiSafeString*,u32)
data sOptionEmptyAnimation 101031CC
data sOptionArrowAnimation 104A0CD8
ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString*,u32)
ext 027045B0 void hdTextPane_c::resetMessage(hdUiSafeString*,hdUiSafeString*)
data sOptionArchiveNames 1049AC34
data sOptionLayoutName 1049AFBC
ext 020013C0 void optionCopyEvent(hdUiEvent*,hdUiEvent*)
ext 026F6340 u32 optionIsCancelEvent(hdUiEvent*)
ext 02615140 u32 cking::ui::input::EventMgr::getMode()
data sOptionDefaultTouch 1049FFA0
ext 02615148 void cking::ui::input::EventMgr::setParams(u32,u32)
data sOptionConnectedWaitState 1049AF5C
ext 026F2458 u32 optionButtonHasSelection(hdParts_c*)
ext 02617FCC u32 hdInputManager::hasDrc()
ext 02617F14 u32 hdInputManager::hasPad()
ext 02030B38 void optionPlaySound(const char*)
data sOptionOpenSound 10103120
data sOptionChangeConnectState 1049AF2C
data sOptionAnimationGroups 10102F9C
data sOptionReverseAnimationRate 10103184
ext 02615154 void cking::ui::input::EventMgr::setModeDefault(u32)
ext 0287F05C u32 optionPaneContains(hdPartsPane_c*,hdOptionPoint_l*)
data sOptionInputManager 101F5088
ext 0271FC44 u32 hdSaveSlotFlags_c::getCreated()
ext 0271FC8C u32 hdSaveSlotFlags_c::getChanged()
data sOptionPaneNames 1049ABDC
data sOptionTextPaneNames 1049ADDC
data sOptionItemPaneNames 1049ABFC
data sOptionSettingPaneName 1049AB68
data sOptionHelpPaneName 1049AB70
ext 02704AFC void hdTextPane_c::setCharacterCapacity(u32)
data sOptionBlankState 1049DD60
data sOptionTouchMode 1047B096
data sOptionInputOptions 101F8378
ext 025200D4 dOwPlayView_c* optionGetPlayButtons()
ext 0259170C void optionEnableDirectionalInput(u32)
ext 0270477C void hdTextPane_c::refresh(PaneWork_l*)
data sOptionButtonLabel 1049AB60
ext 025F65F4 u32 optionConstructMessage(HdMsgText*)
ext 025F6D70 void optionSetMessageLabel(HdMsgText*,hdUiSafeString*,hdUiSafeString*)
ext 026F6048 void hdButtonPart_c::setTextStyle(hdUiSafeString*,u32)
data sOptionItemWidthPadding 10103114
data sOptionItemHeightPadding 10103118
data sOptionHalf 1010311C
ext 02704A08 f32 hdTextPane_c::getTextWidth()
ext 02704AF0 f32 hdTextPane_c::getTextHeight(s32)
data sOptionSave 101F84DC
data sOptionSystem 1018F4AC
ext 0203A070 void optionApplyControllerSettings(void*)
ext 027200D0 hdSaveSlotFlags_c* hdSaveHolders_c::getScratchFlags()
ext 027200F4 hdSavePair_c* hdSaveHolders_c::getScratchPair()
ext 0271FC4C void hdSaveSlotFlags_c::setCreated(u32)
ext 0271FC94 void hdSaveSlotFlags_c::setChanged(u32)
data sOptionVtable 101037B8
data sOptionZero 101031C8
ext 0273AD10 void* optionAllocate(u32)
ext 026F89E8 void optionBaseConstruct(hdParts_c*)
data sOptionEventManager 101F4FF0
data sOptionInputPrompt 1047B07C
data sOptionAnimationRate 10103110
data sOptionButtonWaitState 1049AECC
data sOptionConnectWaitState 1049AF8C
data sOptionLabels 1049AC4C
data sOptionHelpLabel 1049AD74
data sOptionStringVtable 10102F54
data sOptionSettings 101F6AA0
data sOptionNormalButtonIds 10102FCC
data sOptionAlternateButtonIds 10102EF8
ext 026F5C08 void hdButtonPart_c::setReceiverButtons(u32,u32,u32,u32,u32,u32,u32,u32)
ext 026F91F8 void optionDetachArchives(hdParts_c*)
data sOptionStateOwnerVtable 10103928
data sOptionSecondaryVtable 10103938
ext 026F88F0 void optionBaseDestroy(hdParts_c*,u32)
ext 0273AF40 void optionDelete(void*)
*/
