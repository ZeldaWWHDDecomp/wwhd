/** HD command guide button selection and animation states; private binary-derived source. */
#include "d/hd_screen_command_extra.h"
#include "d/hd_screen_command_a.h"
#include "d/hd_save_info_holder.h"
#include "d/hd_msg_text.h"
#include "d/d_com_inf_game.h"

struct hdCommandPair_c {
    be<u32> mPrimary;
    be<u32> mSecondary;
};
typedef hdCommandPair_c sCommandPairs_t[16];
static sCommandPairs_t sCommandPairs;
typedef be<u32> sGuideAnimationSlots_t[16];
static sGuideAnimationSlots_t sGuideAnimationSlots;
typedef be<u32> sGuideButtonAnimations_t[6];
static sGuideButtonAnimations_t sGuideButtonAnimations;
static f32 sGuideAnimationRate;
static hdPartsStateId_c sGuideWaitState;
void hdCommandGuideStopAnimation(hdAnimSet*, u32, u32);

static hdPartsStateId_c sGuideOutState;
static hdPartsStateId_c sGuideHiddenState;

static hdCommandGuideVtbl_c sGuideVtable;
static u32 sGuideHolderVtable;
static u32 sGuideSecondaryVtable;
typedef hdUiSafeString hdGuideAnimationGroups_t[7];
typedef hdUiSafeString hdGuideAnimationNames_t[16];
static hdGuideAnimationGroups_t sGuideAnimationGroups;
static hdGuideAnimationNames_t sGuideAnimationNames;
void hdCommandBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);

static hdUiSafeString sGuideRootPaneName;
typedef hdUiSafeString hdGuideTextPaneNames_t[4];
typedef hdUiSafeString hdGuideMessagePaneNames_t[6];
typedef hdUiSafeString hdGuideArchiveNames_t[5];
static hdGuideTextPaneNames_t sGuideTextPaneNames;
static hdGuideMessagePaneNames_t sGuideMessagePaneNames;
static hdGuideMessagePaneNames_t sGuideMessageLabels;
static hdGuideArchiveNames_t sGuideArchiveNames;
static hdUiStringTable sGuideMessageStringVtable;
struct hdGuidePaneVtbl_c {
    u8 _padding00[0x5C];
    gfn mFindPane;
};
static hdCommandPane_c* findGuidePane(hdCommandPane_c* root, hdUiSafeString* name) {
    hdGuidePaneVtbl_c* table = hdPartsView<hdGuidePaneVtbl_c>(gabi::ea(root->mVtable.get()));
    name->assureTermination();
    return gabi::call_ptr<hdCommandPane_c*>(table->mFindPane.get(), root, name->mText.get(), 1);
}

/* 02649158 */
void hdCommandGuide_c::attachGuideArchives() {
    attachArchives(sGuideArchiveNames, 5);
}

/* 02649168 */
void hdCommandGuide_c::bindRootPane() {
    hdCommandPane_c* root = hdPartsView<hdCommandPane_c>(gabi::ea(mpLayout->lyt()->mpRootPane.get()));
    mpRootPane = findGuidePane(root, &sGuideRootPaneName);
}

/* 026491F0 */
void hdCommandGuide_c::bindTextPanes() {
    hdCommandPane_c* root = hdPartsView<hdCommandPane_c>(gabi::ea(mpLayout->lyt()->mpRootPane.get()));
    for (u32 i = 0; i < 4; ++i) {
        mpTextPanes[i] = (hdTextPane_c*)findGuidePane(root, &sGuideTextPaneNames[i]);
    }
    mpTextPanes[0]->setCharacterCapacity(18);
}

/* 026492AC */
void hdCommandGuide_c::initializePaneMessages() {
    hdCommandPane_c* root = hdPartsView<hdCommandPane_c>(gabi::ea(mpLayout->lyt()->mpRootPane.get()));
    for (u32 i = 0; i < 6; ++i) {
        hdTextPane_c* pane = (hdTextPane_c*)findGuidePane(root, &sGuideMessagePaneNames[i]);
        if (pane != NULL) {
            hdUiSafeString messageName;
            messageName.mVtable = &sGuideMessageStringVtable;
            messageName.mText.v = mpLayout->lyt()->mpMessageData.v.get();
            pane->resetMessage(&messageName, &sGuideMessageLabels[i]);
        }
    }
}

static hdUiSafeString sCommandGuideLayoutName;

/* 02649364 */
u32 hdCommandGuide_c::initializeGuide(u32 first, u32 second, hdPartsDesc_c* descriptor) {
    attachGuideArchives();
    if (vLoad(&sCommandGuideLayoutName, gabi::ea(mpResAccessor.get()), first, second, descriptor) == 0) return 0;
    if (gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this) == 0) return 0;
    bindRootPane();
    bindTextPanes();
    initializePaneMessages();
    ((hdItemDecide_c*)mChildren.safeSlot(1)->get())->mDragFlag = 1;
    ((hdItemDecide_c*)mChildren.safeSlot(2)->get())->mDragFlag = 1;
    ((hdItemDecide_c*)mChildren.safeSlot(3)->get())->mDragFlag = 1;
    ((hdBtnCollectIcon_c*)mChildren.safeSlot(0)->get())->mCollectionKind = 1;
    startState(&sGuideHiddenState);
    vCalcLayout();
    return 1;
}

/* 02649548 */
void hdCommandGuide_c::executeGuideState() {
    hdSaveInfoHolder_c* save = hdPartsView<hdSaveInfoHolder_c>(gabi::ea(dComIfGs_info()) - 0x20);
    hdSaveSlotFlags_c* flags = save->mHolders.getScratchFlags();
    u32 changed = flags->getChanged();
    hdCommandPane_c* root = mpRootPane.get();
    root->mFlags = (u8(root->mFlags) & 0xFE) | (changed == 0 ? 1 : 0);
    hdParts_executeStateMachine(stateMachine());
}

/* 02648860 */
static u32 getCommandGuideStateNumber(hdPartsStateId_c* state) {
    return state->mNumber;
}

/* 02648960 */
void hdCommandGuide_c::destroy(hdCommandGuide_c* self, u32 flags) {
    if (self == NULL) {
        return;
    }
    self->mStateHolder.mHolderVtable = &sGuideHolderVtable;
    self->mSecondaryVtable = &sGuideSecondaryVtable;
    hdParts_c::destroy(self, 0);
    if (flags & 1) {
        operator_delete(self);
    }
}

/* 02648F3C */
hdCommandGuide_c* hdCommandGuide_c::construct(hdCommandGuide_c* self) {
    if (self == NULL) {
        self = (hdCommandGuide_c*)operator_new(sizeof(hdCommandGuide_c));
        if (self == NULL) {
            return NULL;
        }
    }
    hdParts_c::construct(self);
    self->mVtable = &sGuideVtable;
    self->mSecondaryVtable = &sGuideSecondaryVtable;
    self->mStateHolder.mHolderVtable = &sGuideHolderVtable;
    // HD: retain the inline arrays' allocation side effects at wrapped-null addresses.
    if (gabi::ea(&self->mpRootPane) == 0) operator_new(4);
    if (gabi::ea(self->mpTextPanes) == 0) operator_new(16);
    if (gabi::ea(self->mButtonFrames) == 0) operator_new(24);
    if (gabi::ea(self->mButtonCounters) == 0) operator_new(24);
    if (gabi::ea(self->mPreviousButtonFrames) == 0) operator_new(24);
    if (gabi::ea(self->mPendingButtonFrames) == 0) operator_new(24);
    if (gabi::ea(self->mButtonTransitions) == 0) operator_new(6);
    if (gabi::ea(self->mButtonCommands) == 0) operator_new(6);
    self->mInAnimation = 0;
    self->mOutAnimation = 0;
    self->mWaitTimer = 0;
    self->mCachedCommand = 0;
    self->mButtonGroup = 0;
    self->mCommandChanged = 0;
    self->mCachedButtons = 0;
    for (u32 i = 0; i < 4; ++i) self->mpTextPanes[i] = NULL;
    for (u32 i = 0; i < 6; ++i) {
        self->mButtonFrames[i] = 5;
        self->mButtonCounters[i] = 0;
        self->mPreviousButtonFrames[i] = 0xFFFFFFFFu;
        self->mPendingButtonFrames[i] = 0;
        self->mButtonTransitions[i] = 0;
        self->mButtonCommands[i] = 0;
    }
    return self;
}

/* 0264972C */
u32 hdCommandGuide_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* descriptor) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), sCommandUiSystem->mHeap, 4);
    if (layout != NULL) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (layout == NULL) return 0;
    // HD: the original does not branch on the layout loader's return value here.
    layout->vLoad(name, first, second, third, 16, 7, descriptor);
    for (u32 i = 0; i < 16; ++i) {
        u32 group = sGuideAnimationSlots[i];
        hdUiSafeString* groupName = gabi::at<hdUiSafeString>(gabi::ea(sGuideAnimationGroups) + group * 8);
        hdCommandBindAnimation(animSet(), i, &sGuideAnimationNames[i], groupName);
    }
    vCalcLayout();
    return 1;
}

/* 02648BAC */
void hdCommandGuide_c::resetButtons() {
    for (u32 i = 0; i < 6; ++i) {
        mButtonFrames[i] = getButtonAnimation(i);
        mPreviousButtonFrames[i] = 0xFFFFFFFFu;
        setButtonFrame(i, mButtonFrames[i]);
        mButtonCommands[i] = 0xFF;
    }
}

/* 02648CC8 */
void hdCommandGuide_c::enterIn() {
    mInAnimation = gabi::call_ptr<u32>(guideVtable()->mGetInAnimation.get(), this);
    mOutAnimation = gabi::call_ptr<u32>(guideVtable()->mGetOutAnimation.get(), this);
    resetButtons();
    refreshMessages();
    playAnimation(mInAnimation);
}

/* 02648DBC */
void hdCommandGuide_c::executeDisplay() {
    u32 buttons = getButtons();
    if (u32(mCachedButtons) != buttons) {
        changeState(&sGuideOutState);
    } else {
        refreshMessages();
    }
    vExecuteChildren();
    vCalcLayout();
}

/* 02648E78 */
void hdCommandGuide_c::enterOut() {
    playAnimation(mOutAnimation);
    hdParts_c* child = mChildren.safeSlot(4)->get();
    ((hdCommandPromptA_c*)child)->disable();
}

/* 02648EC4 */
void hdCommandGuide_c::executeOut() {
    if (hdAnimSet_isEnd(animSet(), mOutAnimation) != 0) {
        changeState(&sGuideHiddenState);
    }
    vExecuteChildren();
    vCalcLayout();
}

/* 026494F4 */
void hdCommandGuide_c::finalizeGuide() {
    vDeleteChildren();
    vDestroyLayout();
    detachArchives();
}

/* 026495C8 */
void hdCommandGuide_c::drawGuide(void* info) {
    vDrawLayout(info);
    vDrawChildren(info);
}

/* 02649624 */
void hdCommandGuide_c::hide() {
    changeState(&sGuideHiddenState);
    hdParts_c* child = mChildren.safeSlot(4)->get();
    ((hdCommandPromptA_c*)child)->hide();
    vExecuteChildren();
    vCalcLayout();
}

/* 026489CC */
void hdCommandGuide_c::stopAnimation(u32 animation) {
    u32 slot = *gabi::at<be<u32>>(gabi::ea(sGuideAnimationSlots) + animation * 4);
    hdCommandGuideStopAnimation(animSet(), animation, slot);
}

/* 026489E8 */
void hdCommandGuide_c::enterWait() {
    mWaitTimer = 0;
    stopAnimation(1);
}

/* 02648B40 */
void hdCommandGuide_c::setAnimationFrame(u32 animation, u32 frame) {
    u32 slot = *gabi::at<be<u32>>(gabi::ea(sGuideAnimationSlots) + animation * 4);
    hdAnimSet_setFrame(animSet(), animation, slot, f32(frame));
}

/* 02648B98 */
void hdCommandGuide_c::setButtonFrame(u32 button, u32 frame) {
    u32 animation = *gabi::at<be<u32>>(gabi::ea(sGuideButtonAnimations) + button * 4);
    setAnimationFrame(animation, frame);
}

/* 02648CA4 */
void hdCommandGuide_c::playAnimation(u32 animation) {
    u32 slot = *gabi::at<be<u32>>(gabi::ea(sGuideAnimationSlots) + animation * 4);
    hdAnimSet_play(animSet(), animation, slot, sGuideAnimationRate);
}

/* 02648D44 */
void hdCommandGuide_c::executeIn() {
    if (hdAnimSet_isEnd(animSet(), mInAnimation) != 0) {
        changeState(&sGuideWaitState);
    }
    vExecuteChildren();
    vCalcLayout();
}


/* 02648B20 */
u32 hdCommandGuide_c::getButtonAnimation(u32 button) {
    // Espresso's variable shift uses six low bits and yields zero for shifts above31.
    u32 shift = button & 63;
    u32 mask = shift < 32 ? (1u << shift) : 0;
    return (u32(mCachedButtons) & mask) != 0 ? 0 : 5;
}

/* 0264AA50 */
u32 hdCommandGuide_c::getPrimaryCommand() {
    return sCommandPairs[u32(mCachedCommand) & 15].mPrimary;
}

/* 0264AA68 */
u32 hdCommandGuide_c::getSecondaryCommand() {
    return sCommandPairs[u32(mCachedCommand) & 15].mSecondary;
}

typedef hdUiSafeString hdGuideChildGroupNames_t[5];
static hdGuideChildGroupNames_t sGuideChildGroupNames;
static u8 sGuideCommandMode;
static u32 sGuideCommandStyles;

/* 026496A0 */
void hdCommandGuide_c::hideMessageGroup() {
    hdCommandPane_c* root = hdPartsView<hdCommandPane_c>(gabi::ea(mpLayout->lyt()->mpRootPane.get()));
    hdCommandPane_c* pane = findGuidePane(root, &sGuideChildGroupNames[4]);
    if (pane != NULL) pane->mFlags = u8(pane->mFlags) & 0xFE;
}

/* 0264A730 */
u32 hdCommandGuide_c::getCommandMode() {
    return sGuideCommandMode;
}

/* 0264A73C */
u32 hdCommandGuide_c::getButtonCommandStyle(u32 button, u32 command) {
    if (mButtonCounters[button < 6 ? button : 0] != 0) return 2;
    return *gabi::at<be<u32>>(gabi::ea(&sGuideCommandStyles) + command * 4);
}

struct hdCommandChildDescriptor_l {
    u8 _padding00[8];
    hdUiSafeString mName;
    u8 _padding10[4];
    hdUiSafeString mGroup;
};
WWHD_OFFSET(hdCommandChildDescriptor_l, mName, 8);
WWHD_OFFSET(hdCommandChildDescriptor_l, mGroup, 0x14);
struct hdCommandChildFactory_l {
    gptr<be<u8>> mName;
    gfn mCreate;
};
static hdUiSafeString sGuideItemChildName;
static hdUiSafeString sGuideCollectChildName;

static bool commandNamesEqual(hdUiSafeString* name, hdUiSafeString* expected) {
    name->assureTermination();
    name->assureTermination();
    u32 text = gabi::ea(name->mText.get());
    expected->assureTermination();
    if (text == gabi::ea(expected->mText.get())) return true;
    u32 left = gabi::ea(name->mText.get());
    u32 right = gabi::ea(expected->mText.get());
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 c = *gabi::at<be<u8>>(left + i);
        if (c != *gabi::at<be<u8>>(right + i)) return false;
        if (c == 0) return true;
    }
    return false;
}

/* 02649850 */
u32 hdCommandGuide_c::createCommandChild(hdPartsDesc_c* descriptor) {
    typedef hdCommandChildFactory_l hdCommandChildFactories_t[3];
    gabi::Local<hdCommandChildFactories_t> factoryStorage;
    hdCommandChildFactories_t& factories = *factoryStorage.get();
    factories[0].mName = NULL;
    factories[2].mName = NULL;
    factories[2].mCreate.v = gabi::guest_fn((const void*)&createCommandAPrompt);
    factories[1].mName = NULL;
    factories[1].mCreate.v = gabi::guest_fn((const void*)&createCommandCollectButton);
    factories[0].mCreate.v = gabi::guest_fn((const void*)&createCommandItemButton);
    sGuideItemChildName.assureTermination();
    factories[0].mName = sGuideItemChildName.mText;
    sGuideCollectChildName.assureTermination();
    factories[1].mName = sGuideCollectChildName.mText;
    sCommandALayoutName.assureTermination();
    factories[2].mName = sCommandALayoutName.mText;
    hdCommandChildDescriptor_l* names = hdPartsView<hdCommandChildDescriptor_l>(gabi::ea(descriptor));
    for (u32 group = 0; group < 5; ++group) {
        if (!commandNamesEqual(&names->mGroup, &sGuideChildGroupNames[group])) continue;
        for (u32 factory = 0; factory < 3; ++factory) {
            hdUiSafeString name;
            name.mText = factories[factory].mName;
            name.mVtable = &sGuideMessageStringVtable;
            if (!commandNamesEqual(&names->mName, &name)) continue;
            gptr<hdParts_c>* slot = mChildren.safeSlot(group);
            hdParts_c* child = gabi::call_ptr<hdParts_c*>(factories[factory].mCreate.get(), 0, 0, descriptor);
            *slot = child;
            return mChildren.safeSlot(group)->get() != NULL ? 1 : 0;
        }
        return hdParts_c::createChild(group, descriptor);
    }
    return 1;
}

typedef hdUiSafeString hdGuideCommandLabels_t[78];
static hdGuideCommandLabels_t sGuideCommandLabels;
PaneWork_l* constructGuideMessageWork(PaneWork_l*);
void resetGuideMessageWork(PaneWork_l*, hdUiSafeString*, hdUiSafeString*);

/* 02649B40 */
void hdCommandGuide_c::setButtonMessage(u32 button) {
    // The unchecked button index can reach beyond this table, so its native
    // stack address and the neighboring message objects are observable.
    gabi::NativeFrame<0x968> frame;
    be<s32>* paneIndices = gabi::at<be<s32>>(frame + 0x18);
    paneIndices[0] = 0;
    paneIndices[1] = 1;
    paneIndices[4] = -1;
    paneIndices[2] = 2;
    paneIndices[5] = 3;
    paneIndices[3] = -1;
    s32 paneIndex = *gabi::at<be<s32>>(gabi::ea(paneIndices) + button * 4);
    if (paneIndex < 0) return;
    HdMsgText* messageWork = gabi::at<HdMsgText>(frame + 0x30);
    PaneWork_l* work = (PaneWork_l*)messageWork;
    constructGuideMessageWork(work);
    u32 command = mPreviousButtonFrames[button < 6 ? button : 0];
    hdUiSafeString* label = gabi::at<hdUiSafeString>(gabi::ea(sGuideCommandLabels) + command * 8);
    u32 paneSlot = u32(paneIndex) < 4 ? u32(paneIndex) : 0;
    if (mpTextPanes[paneSlot] != NULL) {
        u32 messageText = mpLayout->lyt()->mpMessageData.v.get();
        hdUiSafeString& message = *gabi::at<hdUiSafeString>(frame + 8);
        message.mVtable = &sGuideMessageStringVtable;
        message.mText.v = messageText;
        mpTextPanes[paneSlot]->resetMessage(&message, label);
    }
    if (button == 0) {
        if (canShowCommandText(mPreviousButtonFrames[0]) != 0) {
            u32 messageText = mpLayout->lyt()->mpMessageData.v.get();
            hdUiSafeString& message = *gabi::at<hdUiSafeString>(frame + 0x10);
            message.mVtable = &sGuideMessageStringVtable;
            message.mText.v = messageText;
            resetGuideMessageWork(work, &message, label);
            ((hdCommandPromptA_c*)mChildren.safeSlot(4)->get())->setMessage(work);
        } else {
            ((hdCommandPromptA_c*)mChildren.safeSlot(4)->get())->disable();
        }
    }
}

u32 commandCollectionHasItem(dSv_player_collect_c*, u32, u32);

/* 02649CD8 */
u32 hdCommandGuide_c::applyButtonCommand(u32 button, u32 command) {
    u32 style = *gabi::at<be<u32>>(gabi::ea(&sGuideCommandStyles) + command * 4);
    if (style == 1) setButtonMessage(button);
    else if (style == 2) {
        if (button != 1) return 0;
        if (command == 0x35) {
            u32 icon = 11;
            if (commandCollectionHasItem(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 0, 3) != 0) icon = 14;
            else if (commandCollectionHasItem(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 0, 2) != 0) icon = 13;
            else if (commandCollectionHasItem(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 0, 1) != 0) icon = 12;
            ((hdBtnCollectIcon_c*)mChildren.slot(0)->get())->setSwordIcon(icon);
        } else if (command - 0x37 <= 5) {
            ((hdBtnCollectIcon_c*)mChildren.slot(0)->get())->setSwordIcon(0);
        } else return 0;
    }
    return style;
}

typedef be<u32> hdGuideExcludedCommands_t[12];
static hdGuideExcludedCommands_t sGuideExcludedCommands;
static u32 sGuideContextFlags;
static u8 sGuideContextChoice;

/* 02649B10 */
u32 hdCommandGuide_c::canShowCommandText(u32 command) {
    for (u32 i = 0; i < 12; ++i) {
        if (command == sGuideExcludedCommands[i]) return 0;
    }
    return 1;
}

typedef be<u32> hdGuideItemChildIndices_t[6];
static hdGuideItemChildIndices_t sGuideItemChildIndices;
typedef be<u8> hdGuideItemButtonMasks_t[3];
static hdGuideItemButtonMasks_t sGuideItemButtonMasks;
static u32 commandSelectedItemSlot(u32 button);

/* 02649E08 */
void hdCommandGuide_c::updateCommandButton(u32 button) {
    u32 safeButton = button < 6 ? button : 0;
    if (mButtonFrames[safeButton] == 5) return;
    u32 mask = button == 0 ? 1 : button == 1 ? 2 : button == 5 ? 0x10 : 0;
    mask &= gabi::call_ptr<u32>(guideVtable()->mGetAvailableButtons.get(), this);
    bool changed = false;
    if (mask != 0 && mButtonTransitions[safeButton] != 0) {
        changed = true;
        mButtonTransitions[safeButton] = 0;
    }
    if (u32(mButtonCommands[safeButton]) != mask) {
        changed = true;
        mButtonCommands[safeButton] = mask;
    }
    u32 command = gabi::call_ptr<u32>(guideVtable()->mGetButtonCommand.get(), this, button);
    if (u32(mPreviousButtonFrames[safeButton]) != command) changed = true;
    if (!changed) return;
    mPreviousButtonFrames[safeButton] = command;
    if (command == 0x1A || (command >= 0x2D && command <= 0x2E) || command == 0x31) {
        setAnimationFrame(14, 0);
        playAnimation(15);
        ((hdCommandPromptA_c*)mChildren.safeSlot(4)->get())->setMessage(NULL);
        return;
    }
    if (command == 0xFFFFFFFFu) {
        mButtonFrames[safeButton] = 0;
        if (button == 0) ((hdCommandPromptA_c*)mChildren.safeSlot(4)->get())->disable();
    } else mButtonFrames[safeButton] = applyButtonCommand(button, command);
    if (mButtonGroup == 0 && mask == 0) {
        mButtonFrames[safeButton] = command == 0xFFFFFFFFu ? 4 : 3;
        mButtonTransitions[safeButton] = 1;
    }
    setButtonFrame(button, mButtonFrames[safeButton]);
}

/* 0264A190 */
u32 hdCommandGuide_c::applyItemCommand(u32 button, u32 command) {
    u32 style = gabi::call_ptr<u32>(guideVtable()->mGetCommandStyle.get(), this, button, command);
    if (style == 1) setButtonMessage(button);
    else if (style == 2) {
        u32 childIndex = *gabi::at<be<u32>>(gabi::ea(sGuideItemChildIndices) + button * 4);
        u32 safeButton = button < 6 ? button : 0;
        if (mButtonCounters[safeButton] == 0 && command == 0x47) return 7;
        mPendingButtonFrames[safeButton] = commandSelectedItemSlot(button);
        if ((sGuideContextFlags & 0x01000000) && button == 4) {
            mPendingButtonFrames[4] = 12;
            style = 6;
        }
        ((hdItemDecide_c*)mChildren.safeSlot(childIndex)->get())->mFlag = 0;
        hdItemDecide_c* child = (hdItemDecide_c*)mChildren.safeSlot(childIndex)->get();
        child->setSelection(mPendingButtonFrames[safeButton], command);
    }
    return style;
}

/* 0264A318 */
void hdCommandGuide_c::updateItemButton(u32 button) {
    u32 safeButton = button < 6 ? button : 0;
    if (mButtonFrames[safeButton] == 5) return;
    u32 mask = button >= 2 && button <= 4 ? u32(sGuideItemButtonMasks[button - 2]) : 0;
    mask &= gabi::call_ptr<u32>(guideVtable()->mGetAvailableButtons.get(), this);
    bool changed = false;
    if (mask != 0 && mButtonTransitions[safeButton] != 0) {
        changed = true;
        mButtonTransitions[safeButton] = 0;
    }
    if (u32(mButtonCommands[safeButton]) != mask) {
        changed = true;
        mButtonCommands[safeButton] = mask;
    }
    u32 command = gabi::call_ptr<u32>(guideVtable()->mGetButtonCommand.get(), this, button);
    if (u32(mPreviousButtonFrames[safeButton]) != command) changed = true;
    if (command == 0x82) {
        u32 oldSlot = mPendingButtonFrames[safeButton];
        if (oldSlot != commandSelectedItemSlot(button)) changed = true;
    }
    if (!changed) return;
    mPreviousButtonFrames[safeButton] = command;
    if (command == 0xFFFFFFFFu) mButtonFrames[safeButton] = 0;
    else mButtonFrames[safeButton] = applyItemCommand(button, command);
    if (mButtonGroup == 0 && mask == 0) {
        mButtonFrames[safeButton] = command == 0xFFFFFFFFu ? 4 : 3;
        mButtonTransitions[safeButton] = 1;
    }
    setButtonFrame(button, mButtonFrames[safeButton]);
}

struct hdCommandForcedInfo_l {
    u8 _padding00[0x5BB5];
    be<u8> mRCommand;
    be<u8> mACommand;
    be<u8> mDoCommand;
    be<u8> mForcedRCommand;
    be<u8> mForcedACommand;
    be<u8> mForcedDoCommand;
    u8 _padding5BBB[0x73];
    be<u8> mExtraCommand;
    be<u8> mForcedExtraCommand;
};
WWHD_OFFSET(hdCommandForcedInfo_l, mForcedDoCommand, 0x5BBA);
WWHD_OFFSET(hdCommandForcedInfo_l, mForcedExtraCommand, 0x5C2F);
static hdCommandForcedInfo_l* commandForcedInfo() {
    return (hdCommandForcedInfo_l*)dComIfGp_get();
}
u32 commandGuideIsPaused();

/* 0264A5F0 */
void hdCommandGuide_c::updateGuideMessages() {
    if (commandGuideIsPaused() != 0) {
        if (commandForcedInfo()->mForcedDoCommand != 0) {
            u8 command = commandForcedInfo()->mForcedDoCommand;
            commandForcedInfo()->mDoCommand = command;
            commandForcedInfo()->mForcedDoCommand = 0;
        }
        if (commandForcedInfo()->mForcedACommand != 0) {
            u8 command = commandForcedInfo()->mForcedACommand;
            commandForcedInfo()->mACommand = command;
            commandForcedInfo()->mForcedACommand = 0;
        }
        if (commandForcedInfo()->mForcedRCommand != 0) {
            u8 command = commandForcedInfo()->mForcedRCommand;
            commandForcedInfo()->mRCommand = command;
            commandForcedInfo()->mForcedRCommand = 0;
        }
        if (commandForcedInfo()->mForcedExtraCommand != 0x4D) {
            u8 command = commandForcedInfo()->mForcedExtraCommand;
            commandForcedInfo()->mExtraCommand = command;
            commandForcedInfo()->mForcedExtraCommand = 0x4D;
        }
    }
    if (mCommandChanged == 0) {
        updateCommandButton(0);
        updateCommandButton(1);
        updateItemButton(2);
        updateItemButton(3);
        updateItemButton(4);
        updateCommandButton(5);
    }
}

static u32 commandSelectedItemSlot(u32 button) {
    dSv_player_status_a_c* status = &dComIfGs_info()->mSavedata.mPlayer.mPlayerStatusA;
    return *gabi::at<be<u8>>(gabi::ea(status->mSelectItem) + button - 2);
}

/* 0264A774 */
u32 hdCommandGuide_c::getButtonCommand(u32 button) {
    u32 safeButton = button < 6 ? button : 0;
    mButtonCounters[safeButton] = 0;
    if (button - 2 <= 2) {
        if ((sGuideContextFlags & 0x01000000) && button == 4) {
            mButtonCounters[4] = 1;
            return 0x27;
        }
        if (button == 2 && commandForcedInfo()->mExtraCommand != 0x4D) {
            return commandForcedInfo()->mExtraCommand;
        }
        dSv_player_c* player = &dComIfGs_info()->mSavedata.mPlayer;
        u32 slot = *gabi::at<be<u8>>(gabi::ea(player->mPlayerStatusA.mSelectItem) + button - 2);
        u32 item = 0xFFFFFFFFu;
        if (slot < 0x15) item = player->mPlayerItem.mItems[slot];
        else if (slot >= 0x18 && slot < 0x20) item = player->mBagItem.mBeast[slot - 0x18];
        else if (slot >= 0x24 && slot < 0x2C) item = player->mBagItem.mBait[slot - 0x24];
        else if (slot - 0x30 < 8) item = player->mBagItem.mReserve[slot - 0x30];
        if (item == 0xFF) item = 0xFFFFFFFFu;
        mButtonCounters[safeButton] = 1;
        return item;
    }
    u32 command;
    if (button == 0) command = commandForcedInfo()->mDoCommand;
    else if (button == 1) command = commandForcedInfo()->mACommand;
    else if (button == 5) command = commandForcedInfo()->mRCommand;
    else return 0xFFFFFFFFu;
    return command - 1 < 0x4C ? command : 0xFFFFFFFFu;
}

u32 readCommandGuideFlag(hdCommandGuide_c*);

/* 0264A980 */
u32 hdCommandGuide_c::getAvailableButtons() {
    u32 command = readCommandGuideFlag(this);
    u32 excluded = sGuideContextChoice;
    u32 buttons = 0;
    if ((command & 1) && !(excluded & 1)) buttons = 1;
    if ((command & 2) && !(excluded & 2)) buttons |= 2;
    if ((command & 8) && !(excluded & 8)) buttons |= 0x20;
    if ((command & 4) && !(excluded & 0x14) && !(sGuideContextFlags & 0x01000000)) buttons |= 4;
    if ((command & 4) && !(excluded & 0x24) && !(sGuideContextFlags & 0x01020000)) buttons |= 8;
    if ((command & 4) && !(excluded & 0x44) && !(sGuideContextFlags & 0x20000)) buttons |= 0x10;
    return buttons;
}

/* 0264AA80 */
u32 hdCommandGuide_c::getContextPrimaryCommand() {
    u32 flags = sGuideContextFlags;
    if (flags & 0x80) return 2;
    if (flags & 0x20000) return sGuideContextChoice != 0 ? 2 : 6;
    if (flags & 0x01000000) return 0;
    return getPrimaryCommand();
}

/* 0264AAD0 */
u32 hdCommandGuide_c::getContextSecondaryCommand() {
    u32 flags = sGuideContextFlags;
    if (flags & 0x80) return 3;
    if (flags & 0x20000) return sGuideContextChoice != 0 ? 3 : 7;
    if (flags & 0x01000000) return 1;
    return getSecondaryCommand();
}

struct hdCommandGuideStatics_l {
    be<f32> mPiPair[2];
    be<u8> mAngle;
    be<u8> mShortVector;
    u8 _paddingA[2];
    be<u32> mZero[4];
};
struct hdCommandGuideRegistration_l { be<u32> mWords[3]; };
typedef hdCommandGuideRegistration_l hdGuideRegistrations_t[3];
static hdCommandGuideStatics_l sGuideStatics;
static hdGuideRegistrations_t sGuideRegistrations;
static f32 sGuidePiLower;
static f32 sGuidePiUpper;
static hdPartsStateIdVtbl_c sGuideStateVtable;

static void initializeGuideName(hdUiSafeString* name, const char* text) {
    name->mVtable = &sGuideMessageStringVtable;
    name->mText.v = gabi::guest_addr(text);
}

/* 0264AB20 */
static void initializeCommandGuideStatics() {
    sGuideStatics.mZero[3] = 0;
    sGuideStatics.mZero[2] = 0;
    sGuideStatics.mZero[1] = 0;
    sGuideStatics.mZero[0] = 0;
    __register_global_object(gabi::ea(&sGuideRegistrations[0]));
    f32 lower = sGuidePiLower;
    f32 upper = sGuidePiUpper;
    sGuideStatics.mPiPair[0] = lower;
    sGuideStatics.mPiPair[1] = upper;
    hdCommandInitializeAngle(&sGuideStatics.mAngle);
    __register_global_object(gabi::ea(&sGuideRegistrations[1]));
    hdCommandInitializeShortVector(&sGuideStatics.mShortVector);
    __register_global_object(gabi::ea(&sGuideRegistrations[2]));

    initializeGuideName(&sGuideAnimationGroups[0], "G_All_00");
    initializeGuideName(&sGuideAnimationGroups[1], "G_A_00");
    initializeGuideName(&sGuideAnimationGroups[2], "G_B_00");
    initializeGuideName(&sGuideAnimationGroups[3], "G_X_00");
    initializeGuideName(&sGuideAnimationGroups[4], "G_Y_00");
    initializeGuideName(&sGuideAnimationGroups[5], "G_R_00");
    initializeGuideName(&sGuideAnimationGroups[6], "G_ZR_00");

    initializeGuideName(&sGuideAnimationNames[0], "In6");
    initializeGuideName(&sGuideAnimationNames[1], "Out6");
    initializeGuideName(&sGuideAnimationNames[2], "In5");
    initializeGuideName(&sGuideAnimationNames[3], "Out5");
    initializeGuideName(&sGuideAnimationNames[4], "In2");
    initializeGuideName(&sGuideAnimationNames[5], "Out2");
    initializeGuideName(&sGuideAnimationNames[6], "InPictograph");
    initializeGuideName(&sGuideAnimationNames[7], "OutPictograph");
    initializeGuideName(&sGuideAnimationNames[8], "State");
    initializeGuideName(&sGuideAnimationNames[9], "State");
    initializeGuideName(&sGuideAnimationNames[10], "State");
    initializeGuideName(&sGuideAnimationNames[11], "State");
    initializeGuideName(&sGuideAnimationNames[12], "State");
    initializeGuideName(&sGuideAnimationNames[13], "State");
    initializeGuideName(&sGuideAnimationNames[14], "ASpecial");
    initializeGuideName(&sGuideAnimationNames[15], "ASpecialLoop");

    initializeGuideName(&sGuideChildGroupNames[0], "L_BCollectIcon_00");
    initializeGuideName(&sGuideChildGroupNames[1], "L_XItemIcon_00");
    initializeGuideName(&sGuideChildGroupNames[2], "L_YItemIcon_00");
    initializeGuideName(&sGuideChildGroupNames[3], "L_RItemIcon_00");
    initializeGuideName(&sGuideChildGroupNames[4], "L_CommandA_00");

    initializeGuideName(&sGuideMessagePaneNames[0], "T_A_00");
    initializeGuideName(&sGuideMessagePaneNames[1], "T_B_00");
    initializeGuideName(&sGuideMessagePaneNames[2], "T_X_00");
    initializeGuideName(&sGuideMessagePaneNames[3], "T_Y_00");
    initializeGuideName(&sGuideMessagePaneNames[4], "T_R_00");
    initializeGuideName(&sGuideMessagePaneNames[5], "T_ZR_00");

    initializeGuideName(&sGuideTextPaneNames[0], "T_CommandA_00");
    initializeGuideName(&sGuideTextPaneNames[1], "T_CommandB_00");
    initializeGuideName(&sGuideTextPaneNames[2], "T_CommandX_00");
    initializeGuideName(&sGuideTextPaneNames[3], "T_CommandZR_00");

    initializeGuideName(&sGuideArchiveNames[0], "CommandGuide_00");
    initializeGuideName(&sGuideArchiveNames[1], "BtnItemIcon_00");
    initializeGuideName(&sGuideArchiveNames[2], "BtnCollectIcon_00");
    initializeGuideName(&sGuideArchiveNames[3], "CommandA_00");
    initializeGuideName(&sGuideArchiveNames[4], "BtnFishMan_00");

    initializeGuideName(&sGuideMessageLabels[0], "T_A_00");
    initializeGuideName(&sGuideMessageLabels[1], "T_B_00");
    initializeGuideName(&sGuideMessageLabels[2], "T_X_00");
    initializeGuideName(&sGuideMessageLabels[3], "T_Y_00");
    initializeGuideName(&sGuideMessageLabels[4], "T_R_00");
    initializeGuideName(&sGuideMessageLabels[5], "T_ZR_00");

    initializeGuideName(&sGuideCommandLabels[0], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[1], "T_ACT_Command_34");
    initializeGuideName(&sGuideCommandLabels[2], "T_ACT_Command_20");
    initializeGuideName(&sGuideCommandLabels[3], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[4], "T_ACT_Command_04");
    initializeGuideName(&sGuideCommandLabels[5], "T_ACT_Command_05");
    initializeGuideName(&sGuideCommandLabels[6], "T_ACT_Command_10");
    initializeGuideName(&sGuideCommandLabels[7], "T_ACT_Command_06");
    initializeGuideName(&sGuideCommandLabels[8], "T_ACT_Command_13");
    initializeGuideName(&sGuideCommandLabels[9], "T_ACT_Command_11");
    initializeGuideName(&sGuideCommandLabels[10], "T_ACT_Command_07");
    initializeGuideName(&sGuideCommandLabels[11], "T_ACT_Command_08");
    initializeGuideName(&sGuideCommandLabels[12], "T_ACT_Command_12");
    initializeGuideName(&sGuideCommandLabels[13], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[14], "T_ACT_Command_15");
    initializeGuideName(&sGuideCommandLabels[15], "T_ACT_Command_14");
    initializeGuideName(&sGuideCommandLabels[16], "T_ACT_Command_16");
    initializeGuideName(&sGuideCommandLabels[17], "T_ACT_Command_17");
    initializeGuideName(&sGuideCommandLabels[18], "T_ACT_Command_18");
    initializeGuideName(&sGuideCommandLabels[19], "T_ACT_Command_21");
    initializeGuideName(&sGuideCommandLabels[20], "T_ACT_Command_22");
    initializeGuideName(&sGuideCommandLabels[21], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[22], "T_ACT_Command_02");
    initializeGuideName(&sGuideCommandLabels[23], "T_ACT_Command_19");
    initializeGuideName(&sGuideCommandLabels[24], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[25], "T_ACT_Command_25");
    initializeGuideName(&sGuideCommandLabels[26], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[27], "T_ACT_Command_04");
    initializeGuideName(&sGuideCommandLabels[28], "T_ACT_Command_05");
    initializeGuideName(&sGuideCommandLabels[29], "T_ACT_Command_48");
    initializeGuideName(&sGuideCommandLabels[30], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[31], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[32], "T_ACT_Command_29");
    initializeGuideName(&sGuideCommandLabels[33], "T_ACT_Command_30");
    initializeGuideName(&sGuideCommandLabels[34], "T_ACT_Command_24");
    initializeGuideName(&sGuideCommandLabels[35], "T_ACT_Command_31");
    initializeGuideName(&sGuideCommandLabels[36], "T_ACT_Command_33");
    initializeGuideName(&sGuideCommandLabels[37], "T_ACT_Command_42");
    initializeGuideName(&sGuideCommandLabels[38], "T_ACT_Command_01");
    initializeGuideName(&sGuideCommandLabels[39], "T_ACT_Command_37");
    initializeGuideName(&sGuideCommandLabels[40], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[41], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[42], "T_ACT_Command_38");
    initializeGuideName(&sGuideCommandLabels[43], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[44], "T_ACT_Command_40");
    initializeGuideName(&sGuideCommandLabels[45], "T_ACT_Command_15");
    initializeGuideName(&sGuideCommandLabels[46], "T_ACT_Command_04");
    initializeGuideName(&sGuideCommandLabels[47], "T_ACT_Command_41");
    initializeGuideName(&sGuideCommandLabels[48], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[49], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[50], "T_ACT_Command_06");
    initializeGuideName(&sGuideCommandLabels[51], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[52], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[53], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[54], "T_ACT_Command_27");
    initializeGuideName(&sGuideCommandLabels[55], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[56], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[57], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[58], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[59], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[60], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[61], "T_ACT_Command_43");
    initializeGuideName(&sGuideCommandLabels[62], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[63], "T_ACT_Command_43");
    initializeGuideName(&sGuideCommandLabels[64], "T_ACT_Command_44");
    initializeGuideName(&sGuideCommandLabels[65], "T_ACT_Command_45");
    initializeGuideName(&sGuideCommandLabels[66], "T_ACT_Command_46");
    initializeGuideName(&sGuideCommandLabels[67], "T_ACT_Command_47");
    initializeGuideName(&sGuideCommandLabels[68], "T_ACT_Command_49");
    initializeGuideName(&sGuideCommandLabels[69], "T_ACT_Command_50");
    initializeGuideName(&sGuideCommandLabels[70], "T_ACT_Command_03");
    initializeGuideName(&sGuideCommandLabels[71], "T_ACT_Command_00");
    initializeGuideName(&sGuideCommandLabels[72], "T_ACT_Command_23");
    initializeGuideName(&sGuideCommandLabels[73], "T_ACT_Command_28");
    initializeGuideName(&sGuideCommandLabels[74], "T_ACT_Command_26");
    initializeGuideName(&sGuideCommandLabels[75], "T_ACT_Command_35");
    initializeGuideName(&sGuideCommandLabels[76], "T_ACT_Command_36");
    initializeGuideName(&sGuideCommandLabels[77], "T_ACT_Command_00");

    initializeGuideName(&sGuideRootPaneName, "N_All_00");
    initializeGuideName(&sCommandGuideLayoutName, "CommandGuide_00");
    u32 number;
    if (sCommandStateCounterReady != 0) number = sCommandStateCounter;
    else {
        sCommandStateCounterReady = 1;
        number = 0;
    }
    sGuideHiddenState.setup(s32(++number), "::StateID_NoButton", &sGuideStateVtable, 0x19, &sCommandNullState);
    hdPartsStateId_c* inState = &sGuideActiveState;
    inState->setup(s32(++number), "::StateID_In", &sGuideStateVtable, 0x1C, &sCommandNullState);
    sGuideWaitState.setup(s32(++number), "::StateID_Wait", &sGuideStateVtable, 0x1F, &sCommandNullState);
    sGuideOutState.setup(s32(++number), "::StateID_Out", &sGuideStateVtable, 0x22, &sCommandNullState);
    sCommandStateCounter = number;
}

/* WWHD glue
 str 100EDDAC "G_ZR_00" in 0264AB20
 str 100EDDB4 "T_ZR_00" in 0264AB20
 str 100EDDBC "In6" in 0264AB20
 str 100EDDC0 "In5" in 0264AB20
 str 100EDDC4 "In2" in 0264AB20
 str 100EDDC8 "State" in 0264AB20
 str 100EDDD0 "G_A_00" in 0264AB20
 str 100EDDD8 "G_B_00" in 0264AB20
 str 100EDDE0 "G_X_00" in 0264AB20
 str 100EDDE8 "G_Y_00" in 0264AB20
 str 100EDDF0 "G_R_00" in 0264AB20
 str 100EDDF8 "Out6" in 0264AB20
 str 100EDE00 "Out5" in 0264AB20
 str 100EDE08 "Out2" in 0264AB20
 str 100EDE10 "T_A_00" in 0264AB20
 str 100EDE18 "T_B_00" in 0264AB20
 str 100EDE20 "T_X_00" in 0264AB20
 str 100EDE28 "T_Y_00" in 0264AB20
 str 100EDE30 "T_R_00" in 0264AB20
 str 100EDE38 "CommandGuide_00" in 0264AB20
 str 100EDE48 "CommandA_00" in 0264AB20
 str 100EDE54 "OutPictograph" in 0264AB20
 str 100EDE64 "L_BCollectIcon_00" in 0264AB20
 str 100EDE78 "L_CommandA_00" in 0264AB20
 str 100EDE88 "T_CommandA_00" in 0264AB20
 str 100EDE98 "T_CommandB_00" in 0264AB20
 str 100EDEA8 "T_CommandX_00" in 0264AB20
 str 100EDEB8 "BtnCollectIcon_00" in 0264AB20
 str 100EDECC "BtnFishMan_00" in 0264AB20
 str 100EDEDC "::StateID_Out" in 0264AB20
 str 100EDEEC "G_All_00" in 0264AB20
 str 100EDEF8 "InPictograph" in 0264AB20
 str 100EDF08 "ASpecial" in 0264AB20
 str 100EDF14 "ASpecialLoop" in 0264AB20
 str 100EDF24 "N_All_00" in 0264AB20
 str 100EDF30 "L_XItemIcon_00" in 0264AB20
 str 100EDF40 "L_YItemIcon_00" in 0264AB20
 str 100EDF50 "L_RItemIcon_00" in 0264AB20
 str 100EDF60 "T_CommandZR_00" in 0264AB20
 str 100EDF70 "BtnItemIcon_00" in 0264AB20
 str 100EDF80 "T_ACT_Command_00" in 0264AB20
 str 100EDF94 "T_ACT_Command_34" in 0264AB20
 str 100EDFA8 "T_ACT_Command_20" in 0264AB20
 str 100EDFBC "T_ACT_Command_04" in 0264AB20
 str 100EDFD0 "T_ACT_Command_05" in 0264AB20
 str 100EDFE4 "T_ACT_Command_10" in 0264AB20
 str 100EDFF8 "T_ACT_Command_06" in 0264AB20
 str 100EE00C "T_ACT_Command_13" in 0264AB20
 str 100EE020 "T_ACT_Command_11" in 0264AB20
 str 100EE034 "T_ACT_Command_07" in 0264AB20
 str 100EE048 "T_ACT_Command_08" in 0264AB20
 str 100EE05C "T_ACT_Command_12" in 0264AB20
 str 100EE070 "T_ACT_Command_15" in 0264AB20
 str 100EE084 "T_ACT_Command_14" in 0264AB20
 str 100EE098 "T_ACT_Command_16" in 0264AB20
 str 100EE0AC "T_ACT_Command_17" in 0264AB20
 str 100EE0C0 "T_ACT_Command_18" in 0264AB20
 str 100EE0D4 "T_ACT_Command_21" in 0264AB20
 str 100EE0E8 "T_ACT_Command_22" in 0264AB20
 str 100EE0FC "T_ACT_Command_02" in 0264AB20
 str 100EE110 "T_ACT_Command_19" in 0264AB20
 str 100EE124 "T_ACT_Command_25" in 0264AB20
 str 100EE138 "T_ACT_Command_48" in 0264AB20
 str 100EE14C "T_ACT_Command_29" in 0264AB20
 str 100EE160 "T_ACT_Command_30" in 0264AB20
 str 100EE174 "T_ACT_Command_24" in 0264AB20
 str 100EE188 "T_ACT_Command_31" in 0264AB20
 str 100EE19C "T_ACT_Command_33" in 0264AB20
 str 100EE1B0 "T_ACT_Command_42" in 0264AB20
 str 100EE1C4 "T_ACT_Command_01" in 0264AB20
 str 100EE1D8 "T_ACT_Command_37" in 0264AB20
 str 100EE1EC "T_ACT_Command_38" in 0264AB20
 str 100EE200 "T_ACT_Command_40" in 0264AB20
 str 100EE214 "T_ACT_Command_41" in 0264AB20
 str 100EE228 "T_ACT_Command_27" in 0264AB20
 str 100EE23C "T_ACT_Command_43" in 0264AB20
 str 100EE250 "T_ACT_Command_44" in 0264AB20
 str 100EE264 "T_ACT_Command_45" in 0264AB20
 str 100EE278 "T_ACT_Command_46" in 0264AB20
 str 100EE28C "T_ACT_Command_47" in 0264AB20
 str 100EE2A0 "T_ACT_Command_49" in 0264AB20
 str 100EE2B4 "T_ACT_Command_50" in 0264AB20
 str 100EE2C8 "T_ACT_Command_03" in 0264AB20
 str 100EE2DC "T_ACT_Command_23" in 0264AB20
 str 100EE2F0 "T_ACT_Command_28" in 0264AB20
 str 100EE304 "T_ACT_Command_26" in 0264AB20
 str 100EE318 "T_ACT_Command_35" in 0264AB20
 str 100EE32C "T_ACT_Command_36" in 0264AB20
 str 100EE340 "::StateID_NoButton" in 0264AB20
 str 100EE354 "::StateID_In" in 0264AB20
 str 100EE364 "::StateID_Wait" in 0264AB20
 ext 026489F8 u32 readCommandGuideFlag(hdCommandGuide_c*)
 data sGuideStatics 1049122C
 data sGuideRegistrations 101F5888
 data sGuidePiLower 100EDDA4
 data sGuidePiUpper 100EDDA8
 data sGuideStateVtable 100EDB44
 data sGuideItemChildIndices 100EDD88
 data sGuideItemButtonMasks 100EDDA0
 ext 026302A0 void hdMenuItem_c::setSelection(u32, u32)
 ext 025AF2A4 u32 commandGuideIsPaused()
 ext 025B7A2C u32 commandCollectionHasItem(dSv_player_collect_c*, u32, u32)
 ext 0262A484 void hdBtnCollectIcon_c::setSwordIcon(u32)
 data sCommandGuideLayoutName 10491700
 ext 027200D0 hdSaveSlotFlags_c* hdSaveHolders_c::getScratchFlags()
 ext 0271FC8C u32 hdSaveSlotFlags_c::getChanged()
 data sGuideItemChildName 1048FEDC
 data sGuideCollectChildName 1048F4D8
 ext 026F92C0 u32 hdParts_c::createChild(u32, hdPartsDesc_c*)
 data sGuideCommandLabels 10491350
 ext 025F65F4 PaneWork_l* constructGuideMessageWork(PaneWork_l*)
 ext 025F6D70 void resetGuideMessageWork(PaneWork_l*, hdUiSafeString*, hdUiSafeString*)
 data sGuideChildGroupNames 104912B0
 data sGuideCommandMode 1047B07D
 data sGuideCommandStyles 100EDC0C
 data sGuideExcludedCommands 100EDD58
 data sGuideContextFlags 1047B074
 data sGuideContextChoice 1047B07B
 data sGuideRootPaneName 10491224
 data sGuideTextPaneNames 10491308
 data sGuideMessagePaneNames 104912D8
 data sGuideMessageLabels 10491248
 data sGuideArchiveNames 10491328
 data sGuideMessageStringVtable 100EDA8C
 ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString*, u32)
 ext 026F88F0 void hdParts_c::destroy(hdParts_c*, s32)
 ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c*)
 ext 026F91F8 void hdParts_c::detachArchives()
 data sGuideVtable 100EE378
 data sGuideHolderVtable 100EE4D8
 data sGuideSecondaryVtable 100EE4E8
 data sGuideAnimationGroups 10491278
 data sGuideAnimationNames 104915C0
 data sCommandPairs 100EDB8C
 data sGuideAnimationSlots 100EDAA4
 data sGuideButtonAnimations 100EDAE4
 data sGuideAnimationRate 100EDD48
 data sGuideWaitState 104916A0
 data sGuideOutState 104916D0
 data sGuideHiddenState 10491640
 ext 02005708 void hdCommandGuideStopAnimation(hdAnimSet*, u32, u32)
*/
