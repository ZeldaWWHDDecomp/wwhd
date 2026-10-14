/* HD-only touch-slide overlay. State names follow the HD StateID strings;
 * remaining names are descriptive. */
#include "d/hd_screen_touch_slide_decide.h"
static hdPartsVtbl_c sTouchVtable;
static u32 sTouchHolderVtable;
static u32 sTouchSecondaryVtable;
static hdUiSafeString sTouchLayoutName;
static void* sTouchHeapManager;



void hdItemPane_setAlpha(hdPartsPaneSRT_c*, u32);
static hdTouchPaneNames_c sTouchPaneNames;
static hdUiStringTable sTouchSafeStringTable;
static hdScreenType sTouchPaneType;
static hdScreenType sTouchPictureType;
static hdScreenType sTouchPictureBase;
static hdScreenType sTouchTextType;
static hdScreenType sTouchTextBase;
static hdScreenType sTouchTextPaneBase;
static u32 sTouchPictureTypeInitialized;
static u32 sTouchPictureBaseInitialized;
static u32 sTouchTextTypeInitialized;
static u32 sTouchTextBaseInitialized;
static u32 sTouchTextPaneBaseInitialized;
static u32 sTouchPaneTypeInitialized;
hdItemTypedPane_l* hdItem_findPane(void*, hdUiSafeString*, hdUiSafeString*);
static hdUiStringTable sTouchWideStringVtable;
static hdUiStringTable sTouchWideBufferVtable;
void hdTouchWide_assure(hdTouchWideString_l*);
void hdTouchWide_copy(void*, const void*, u32, u32);

static hdTouchAnimationNames_c sTouchAnimations;
static hdTouchGroupNames_c sTouchGroups;
static hdTouchGroupRemap_c sTouchGroupRemap;
static hdScreenUiSystem* sTouchUiSystem;
void hdItemAnimation_bind(hdAnimSet* set, u32 entry, hdUiSafeString* animation, hdUiSafeString* group);

/* 02634088 */
u32 hdItem_collectPanes(void* layout, hdItemPaneArray_c* panes, hdUiSafeString* names) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    u32 complete = 1;
    for (s32 i = 0; i < s32(panes->mCount); ++i) {
        gptr<hdPartsPaneSRT_c>* destination = panes->safeSlot(u32(i));
        gabi::FrameLocal<hdUiSafeString> prefix(8);
        prefix->mText.v = gabi::guest_str("");
        prefix->mVtable = &sTouchSafeStringTable;
        names->assureTermination();
        gabi::FrameLocal<hdUiSafeString> name(0x10);
        name->mVtable = &sTouchSafeStringTable;
        name->mText = names->mText;
        hdItemTypedPane_l* pane = hdItem_findPane(layout, prefix, name);
        if (!sTouchPaneTypeInitialized) {
            sTouchPaneType.mParent = NULL;
            sTouchPaneTypeInitialized = 1;
        }
        bool matching = false;
        if (pane) {
            hdScreenType* type = pane->type();
            while (type) {
                if (type == &sTouchPaneType) { matching = true; break; }
                type = type->mParent;
            }
        }
        *destination = matching ? (hdPartsPaneSRT_c*)pane : NULL;
        if (!panes->at(u32(i))) complete = 0;
        names = gabi::at<hdUiSafeString>(gabi::ea(names) + sizeof(hdUiSafeString));
    }
    return complete;
}
/* 0263420C */
u32 hdItem_collectPictures(void* layout, hdItemPaneArray_c* panes, hdUiSafeString* names) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    u32 complete = 1;
    for (s32 i = 0; i < s32(panes->mCount); ++i) {
        gptr<hdPartsPaneSRT_c>* destination = panes->safeSlot(u32(i));
        gabi::FrameLocal<hdUiSafeString> prefix(8);
        prefix->mText.v = gabi::guest_str("");
        prefix->mVtable = &sTouchSafeStringTable;
        names->assureTermination();
        gabi::FrameLocal<hdUiSafeString> name(0x10);
        name->mVtable = &sTouchSafeStringTable;
        name->mText = names->mText;
        hdItemTypedPane_l* pane = hdItem_findPane(layout, prefix, name);
        if (!sTouchPictureTypeInitialized) {
            sTouchPictureTypeInitialized = 1;
            if (!sTouchPictureBaseInitialized) {
                sTouchPictureBaseInitialized = 1;
                if (!sTouchPaneTypeInitialized) {
                    sTouchPaneType.mParent = NULL;
                    sTouchPaneTypeInitialized = 1;
                }
                sTouchPictureBase.mParent = &sTouchPaneType;
            }
            sTouchPictureType.mParent = &sTouchPictureBase;
        }
        bool matching = false;
        if (pane) {
            hdScreenType* type = pane->type();
            while (type) {
                if (type == &sTouchPictureType) { matching = true; break; }
                type = type->mParent;
            }
        }
        *destination = matching ? (hdPartsPaneSRT_c*)pane : NULL;
        if (!panes->at(u32(i))) complete = 0;
        names = gabi::at<hdUiSafeString>(gabi::ea(names) + sizeof(hdUiSafeString));
    }
    return complete;
}
/* 02634420 */
u32 hdItem_collectTexts(void* layout, hdItemPaneArray_c* panes, hdUiSafeString* names) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    u32 complete = 1;
    for (s32 i = 0; i < s32(panes->mCount); ++i) {
        gptr<hdPartsPaneSRT_c>* destination = panes->safeSlot(u32(i));
        gabi::FrameLocal<hdUiSafeString> prefix(0xC);
        prefix->mText.v = gabi::guest_str("");
        prefix->mVtable = &sTouchSafeStringTable;
        names->assureTermination();
        gabi::FrameLocal<hdUiSafeString> name(0x14);
        name->mVtable = &sTouchSafeStringTable;
        name->mText = names->mText;
        hdItemTypedPane_l* pane = hdItem_findPane(layout, prefix, name);
        if (!sTouchTextTypeInitialized) {
            sTouchTextTypeInitialized = 1;
            if (!sTouchTextBaseInitialized) {
                sTouchTextBaseInitialized = 1;
                if (!sTouchTextPaneBaseInitialized) {
                    sTouchTextPaneBaseInitialized = 1;
                    if (!sTouchPaneTypeInitialized) {
                        sTouchPaneTypeInitialized = 1;
                        sTouchPaneType.mParent = NULL;
                    }
                    sTouchTextPaneBase.mParent = &sTouchPaneType;
                }
                sTouchTextBase.mParent = &sTouchTextPaneBase;
            }
            sTouchTextType.mParent = &sTouchTextBase;
        }
        bool matching = false;
        if (pane) {
            hdScreenType* type = pane->type();
            while (type) {
                if (type == &sTouchTextType) { matching = true; break; }
                type = type->mParent;
            }
        }
        *destination = matching ? (hdPartsPaneSRT_c*)pane : NULL;
        if (!panes->at(u32(i))) complete = 0;
        names = gabi::at<hdUiSafeString>(gabi::ea(names) + sizeof(hdUiSafeString));
    }
    return complete;
}
/* 02633F88 */
static s32 hdTouch_stateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 0263465C */
void hdTouchSlideDecide_c::destroy(hdTouchSlideDecide_c* self, s32 flags) {
    if (!self) return;
    self->mButton.mStateHolder.mHolderVtable = &sTouchHolderVtable;
    self->mButton.mSecondaryVtable = &sTouchSecondaryVtable;
    hdBasicButton_c::destroy(self, 0);
    if (flags & 1) operator_delete(self);
}
/* 026346C8 */
void hdTouchSlideDecide_c::touchSlideDecideEnter() { hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_TOUCH_SLIDE_DECIDE, HD_TOUCH_SLOT_BUTTON, 1.0f); }
/* 026346E4 */
void hdTouchSlideDecide_c::touchSlideDecideExecute() { gabi::tail_ptr(mButton.mVtable->mCalcLayout.get(), this); }
/* 026346F4 */
void hdTouchSlideDecide_c::commandEnter() { hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_COMMAND, HD_TOUCH_SLOT_BUTTON, 1.0f); }
/* 02634710 */
void hdTouchSlideDecide_c::commandExecute() { gabi::tail_ptr(mButton.mVtable->mCalcLayout.get(), this); }
/* 02634720 */
void hdTouchSlideDecide_c::iconChangeEnter() { hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_ICON_CHANGE, HD_TOUCH_SLOT_ICON_CHANGE, 1.0f); }
/* 0263473C */
void hdTouchSlideDecide_c::iconChangeExecute() { gabi::tail_ptr(mButton.mVtable->mCalcLayout.get(), this); }
/* 0263474C */
void hdTouchSlideDecide_c::arrowLoopEnter() {
    hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_ARROW_FIRE, HD_TOUCH_SLOT_ARROW_FIRE, 1.0f);
    hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_ARROW_ICE, HD_TOUCH_SLOT_ARROW_ICE, 1.0f);
    hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_ARROW_LIGHT, HD_TOUCH_SLOT_ARROW_LIGHT, 1.0f);
}
/* 026347DC */
void hdTouchSlideDecide_c::arrowLoopExecute() { gabi::tail_ptr(mButton.mVtable->mCalcLayout.get(), this); }
/* 026347EC */
void hdTouchSlideDecide_c::floatEnter() { hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_FLOAT, HD_TOUCH_SLOT_FLOAT, 1.0f); }
/* 02634808 */
void hdTouchSlideDecide_c::floatExecute() { gabi::tail_ptr(mButton.mVtable->mCalcLayout.get(), this); }
/* 02634818 */
void hdTouchSlideDecide_c::floatCancelEnter() { hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_FLOAT_CANCEL, HD_TOUCH_SLOT_FLOAT, 1.0f); }
/* 02634834 */
void hdTouchSlideDecide_c::floatCancelExecute() { gabi::tail_ptr(mButton.mVtable->mCalcLayout.get(), this); }
/* 02634844 */
void hdTouchSlideDecide_c::numEnter() { hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_COUNT, HD_TOUCH_SLOT_COUNT, 1.0f); }
/* 02634860 */
void hdTouchSlideDecide_c::numExecute() { gabi::tail_ptr(mButton.mVtable->mCalcLayout.get(), this); }
/* 02634870 */
void hdTouchSlideDecide_c::switchEnter() { hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_SWITCH, HD_TOUCH_SLOT_BUTTON, 1.0f); }
/* 0263488C */
void hdTouchSlideDecide_c::switchExecute() { gabi::tail_ptr(mButton.mVtable->mCalcLayout.get(), this); }
/* 0263489C */
hdTouchSlideDecide_c* hdTouchSlideDecide_c::construct(hdTouchSlideDecide_c* self) {
    if (!self) self = (hdTouchSlideDecide_c*)operator_new(sizeof(hdTouchSlideDecide_c));
    if (self) {
        hdPartsBasicButton_construct(&self->mButton);
        self->mButton.mVtable = &sTouchVtable;
        self->mButton.mStateHolder.mHolderVtable = &sTouchHolderVtable;
        self->mButton.mSecondaryVtable = &sTouchSecondaryVtable;
        for (u32 i = 0; i < HD_ITEM_PANE_GROUP_COUNT; ++i) {
            hdItemPaneArray_c* group = gabi::at<hdItemPaneArray_c>(gabi::ea(self) + offsetof(hdTouchSlideDecide_c, mPaneGroups) + i * sizeof(hdItemPaneArray_c));
            if (!group) group = (hdItemPaneArray_c*)operator_new(sizeof(hdItemPaneArray_c));
            if (group) { group->mpData = NULL; group->mCount = 0; }
        }
        self->mSource.mpAnimation = NULL;
        self->mSource.mpPaneGroups[2] = NULL;
        self->mSource.mpVisible = NULL;
        self->mSource.mpPaneGroups[1] = NULL;
        self->mSource.mpPaneGroups[0] = NULL;
        self->mSource.mpRootPane = NULL;
    }
    return self;
}
/* 0263498C */
u32 hdTouchSlideDecide_c::preparePanes() {
    hdScreenHeap* heap = sTouchUiSystem->mHeap;
    const u32 counts[] = {1, 3, 2};
    for (u32 groupIndex = 0; groupIndex < HD_ITEM_PANE_GROUP_COUNT; ++groupIndex) {
        hdScreenHeap* allocationHeap = heap;
        if (!allocationHeap) allocationHeap = hdParts_getCurrentHeap(sTouchHeapManager);
        void* storage = allocationHeap->allocate(counts[groupIndex] * 4);
        hdItemPaneArray_c& group = mPaneGroups[groupIndex];
        if (storage) { group.mpData = (gptr<hdPartsPaneSRT_c>*)storage; group.mCount = counts[groupIndex]; }
        s32 count = group.mCount;
        for (s32 i = 0; i < count; ++i) {
            *gabi::at<gptr<hdPartsPaneSRT_c>>(gabi::ea(group.mpData.get()) + u32(i) * 4) = NULL;
        }
        u32 complete;
        if (groupIndex == 0) complete = hdItem_collectPanes(mButton.mpLayout.get(), &group, &sTouchPaneNames.mRoot);
        else if (groupIndex == 1) complete = hdItem_collectPictures(mButton.mpLayout.get(), &group, &sTouchPaneNames.mPictures[0]);
        else complete = hdItem_collectTexts(mButton.mpLayout.get(), &group, &sTouchPaneNames.mTexts[0]);
        if (!complete) return 0;
    }
    hdItemPane_setAlpha(mPaneGroups[2].at(1), 0x20);
    return 1;
}
/* 02634B98 */
u32 hdTouchSlideDecide_c::load(void* resource, u32 arg1, u32 arg2) {
    if (!hdPartsBasicButton_init(&mButton, &sTouchLayoutName, gabi::ea(resource), arg1, (hdPartsDesc_c*)gabi::at<void>(arg2))) return 0;
    return preparePanes() ? 1 : 0;
}
/* 02634C0C */
void hdTouchSlideDecide_c::releasePanes() {
    for (s32 groupIndex = 2; groupIndex >= 0; --groupIndex) {
        hdItemPaneArray_c& group = mPaneGroups[groupIndex];
        s32 count = group.mCount;
        for (s32 i = 0; i < count; ++i) {
            // HD: reload the array pointer each iteration; the count is captured by CTR.
            *gabi::at<gptr<hdPartsPaneSRT_c>>(gabi::ea(group.mpData.get()) + u32(i) * 4) = NULL;
        }
        if (s32(group.mCount) > 0 || group.mpData.get() != NULL) {
            hdScreenHeap* heap = hdParts_findContainHeap(sTouchHeapManager, group.mpData.get());
            heap->free(group.mpData.get());
            group.mCount = 0;
            group.mpData = NULL;
        }
    }
}
/* 02634D9C */
void hdTouchSlideDecide_c::finalize() { releasePanes(); hdBasicButton_c::unload(); }
/* 02634DD0 */
void hdTouchSlideDecide_c::syncAnimations() {
    hdAnimSet* source = mSource.mpAnimation;
    if (!source) return;
    for (u32 i = 0; i < HD_ITEM_ANIMATION_COUNT; ++i) {
        hdAnimSetEntry* sourceEntry = *source->mEntries.at(i);
        hdAnimSetEntry* targetEntry = *mButton.animSet()->mEntries.at(i);
        if (!sourceEntry || !targetEntry) continue;
        hdAnimSetController* original = sourceEntry->mController;
        hdAnimSetController* target = targetEntry->mController;
        if (original->mPlaying != target->mPlaying) {
            if (original->mPlaying) {
                target->setPlaying(1);
                targetEntry->mController->operationCC();
                target = targetEntry->mController;
            } else {
                target->setPlaying(0);
                target = targetEntry->mController;
            }
        }
        if (target->mPlaying) {
            original = sourceEntry->mController;
            if (original->mSpeed != target->mSpeed) targetEntry->mController->setFrame(original->mSpeed);
        }
    }
}
static void hdTouch_copyWide(hdTouchWideBuffer_c* target, hdTouchWideString_l* source) {
    be<u16>* destination = target->mpText;
    hdTouchWide_assure(source);
    be<u16>* text = source->mpText;
    s32 length = 0;
    while (text[length]) {
        if (++length > HD_ITEM_WIDE_SCAN_LIMIT) { length = 0; break; }
    }
    if (length >= target->mCapacity) length = s32(target->mCapacity) - 1;
    source->assureTermination();
    hdTouchWide_copy(destination, source->mpText.get(), u32(length) * 2, 0);
    *gabi::at<be<u16>>(gabi::ea(destination) + u32(length) * 2) = 0;
}
/* 02634F00 */
void hdTouchSlideDecide_c::syncPanes() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    hdItemPaneArray_c* groups[3] = {mSource.mpPaneGroups[0], mSource.mpPaneGroups[1], mSource.mpPaneGroups[2]};
    if (!groups[0] || !groups[1] || !groups[2]) return;
    if (mSource.mpRootPane) {
        hdPartsPaneSRT_c* root = (hdPartsPaneSRT_c*)mButton.mpLayout->lyt()->mpRootPane.get();
        u8 flags = root->mFlags;
        root->mTranslationXBits = mSource.mpRootPane->mTranslationXBits;
        root->mTranslationYBits = mSource.mpRootPane->mTranslationYBits;
        u32 z = mSource.mpRootPane->mTranslationZBits;
        root->mFlags = flags | HD_ITEM_PANE_TRANSLATION_DIRTY;
        root->mTranslationZBits = z;
    }
    for (u32 group = 0; group < 2; ++group) {
        hdItemPaneArray_c& target = mPaneGroups[group];
        for (s32 i = 0; i < s32(target.mCount); ++i) {
            if (!target.at(i) || !groups[group]->at(i)) continue;
            if ((target.at(i)->mFlags & HD_ITEM_PANE_VISIBLE) != (groups[group]->at(i)->mFlags & HD_ITEM_PANE_VISIBLE)) {
                hdPartsPaneSRT_c* pane = target.at(i);
                u8 source = groups[group]->at(i)->mFlags;
                pane->mFlags = (u8(pane->mFlags) & ~HD_ITEM_PANE_VISIBLE) | (source & HD_ITEM_PANE_VISIBLE);
            }
        }
    }
    hdItemPaneArray_c& texts = mPaneGroups[2];
    if (texts.at(1) && groups[2]->at(1) && (texts.at(1)->mFlags & HD_ITEM_PANE_VISIBLE) != (groups[2]->at(1)->mFlags & HD_ITEM_PANE_VISIBLE)) {
        hdPartsPaneSRT_c* pane = texts.at(1);
        u8 source = groups[2]->at(1)->mFlags;
        pane->mFlags = (u8(pane->mFlags) & ~HD_ITEM_PANE_VISIBLE) | (source & HD_ITEM_PANE_VISIBLE);
    }
    if (mSource.mpVisible) {
        hdPartsPaneSRT_c* root = (hdPartsPaneSRT_c*)mButton.mpLayout->lyt()->mpRootPane.get();
        root->mFlags = (u8(root->mFlags) & ~HD_ITEM_PANE_VISIBLE) | u8(*mSource.mpVisible);
    }
    gabi::FrameLocal<hdTouchWideBuffer_c> original(0x10);
    gabi::FrameLocal<hdTouchWideBuffer_c> current(0x5C);
    original->mBuffer[HD_ITEM_WIDE_BUFFER_CAPACITY - 1] = 0; original->mBuffer[0] = 0;
    original->mpText = original->mBuffer;
    original->mpVtable = &sTouchWideBufferVtable;
    original->mCapacity = HD_ITEM_WIDE_BUFFER_CAPACITY;
    current->mBuffer[HD_ITEM_WIDE_BUFFER_CAPACITY - 1] = 0; current->mBuffer[0] = 0;
    current->mpText = current->mBuffer;
    current->mpVtable = &sTouchWideBufferVtable;
    current->mCapacity = HD_ITEM_WIDE_BUFFER_CAPACITY;
    gabi::FrameLocal<hdTouchWideString_l> string(8);
    for (s32 i = 0; i < s32(texts.mCount); ++i) {
        if (!texts.at(i) || !groups[2]->at(i)) continue;
        string->mpVtable = &sTouchWideStringVtable;
        string->mpText = ((hdTextPane_c*)groups[2]->at(i))->mpText;
        hdTouch_copyWide(original, string);
        string->mpVtable = &sTouchWideStringVtable;
        string->mpText = ((hdTextPane_c*)texts.at(i))->mpText;
        hdTouch_copyWide(current, string);
        original->assureTermination(); original->assureTermination();
        be<u16>* left = original->mpText;
        current->assureTermination();
        be<u16>* right = current->mpText;
        bool equal = left == right;
        if (!equal) {
            left = original->mpText;
            for (u32 n = 0; n <= HD_ITEM_WIDE_SCAN_LIMIT; ++n) {
                u16 a = left[n], b = right[n];
                if (a != b) break;
                if (!a) { equal = true; break; }
            }
        }
        if (!equal) {
            hdTextPane_c* pane = (hdTextPane_c*)texts.at(i);
            gfn* table = pane->mpVTable;
            original->assureTermination();
            gabi::call_ptr<void>(table[HD_ITEM_TEXT_SETTER_SLOT].get(), pane, original->mpText.get(), 0);
        }
    }
}
/* 02635594 */
void hdTouchSlideDecide_c::execute() { syncAnimations(); syncPanes(); mButton.vExecuteChildren(); }
/* 026355DC */
void hdTouchSlideDecide_c::setSource(hdItemIconContext_c* source) {
    mSource.mpAnimation = source->mpAnimation;
    mSource.mpRootPane = source->mpRootPane;
    mSource.mpPaneGroups[0] = source->mpPaneGroups[0];
    mSource.mpPaneGroups[1] = source->mpPaneGroups[1];
    mSource.mpPaneGroups[2] = source->mpPaneGroups[2];
    mSource.mpVisible = source->mpVisible;
}
/* 02635610 */
u32 hdTouchSlideDecide_c::loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), sTouchUiSystem->mHeap, 4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mButton.mpLayout = layout;
    if (!layout || !layout->vLoad(name, arg1, arg2, arg3, HD_ITEM_ANIMATION_COUNT, 9, desc)) return 0;
    for (u32 i = 0; i < HD_ITEM_EXTRA_ANIMATION_COUNT; ++i) {
        u32 groupIndex = sTouchGroupRemap.mIndices[i];
        hdUiSafeString* group = &sTouchGroups.mNames[groupIndex];
        hdItemAnimation_bind(mButton.animSet(), i + HD_ITEM_ANIM_TOUCH_SLIDE_DECIDE, &sTouchAnimations.mNames[i], group);
    }
    return 1;
}

static hdUiStringTable sTouchStaticStringVtable;
static u32 sTouchStateCounterReady;
static u32 sTouchStateCounter;
static hdPartsStateIdVtbl_c sTouchStateVtable;
static hdPartsStateId_c sTouchTouchSlideDecideState;
static hdStateId sTouchNullState;
static hdPartsStateId_c sTouchTouchSlideDecideCmdState;
static hdPartsStateId_c sTouchIconChangeState;
static hdPartsStateId_c sTouchArrowLoopState;
static hdPartsStateId_c sTouchFloatState;
static hdPartsStateId_c sTouchFloatCancelState;
static hdPartsStateId_c sTouchNumState;
static hdPartsStateId_c sTouchSwitchState;
static hdItemHeaderStatics_c sTouchHeaderStatics;
static hdItemHeaderDescriptors_c sTouchHeaderDescriptors;
static hdItemHeaderZero_c sTouchHeaderZero;
static hdItemPoint_c sTouchPiConstants;
/* 02635728 */
static void __sinit_hd_screen_touch_slide_decide_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sTouchHeaderStatics), gabi::ea(&sTouchHeaderDescriptors), gabi::ea(&sTouchHeaderZero),
                                gabi::ea(&sTouchPiConstants));
    hdParts_setSafeString(&sTouchPaneNames.mRoot, &sTouchStaticStringVtable, "N_ItemIcon_00");
    hdParts_setSafeString(&sTouchGroups.mNames[0], &sTouchStaticStringVtable, "G_Btn_00");
    hdParts_setSafeString(&sTouchGroups.mNames[1], &sTouchStaticStringVtable, "G_IconChange_00");
    hdParts_setSafeString(&sTouchGroups.mNames[2], &sTouchStaticStringVtable, "G_Float_00");
    hdParts_setSafeString(&sTouchGroups.mNames[3], &sTouchStaticStringVtable, "G_Num_00");
    hdParts_setSafeString(&sTouchGroups.mNames[4], &sTouchStaticStringVtable, "G_ArrowFire_00");
    hdParts_setSafeString(&sTouchGroups.mNames[5], &sTouchStaticStringVtable, "G_ArrowIce_00");
    hdParts_setSafeString(&sTouchGroups.mNames[6], &sTouchStaticStringVtable, "G_ArrowLight_00");
    hdParts_setSafeString(&sTouchGroups.mNames[7], &sTouchStaticStringVtable, "P_ArrowFire_00");
    hdParts_setSafeString(&sTouchGroups.mNames[8], &sTouchStaticStringVtable, "P_ArrowIce_00");
    hdParts_setSafeString(&sTouchGroups.mNames[9], &sTouchStaticStringVtable, "P_ArrowLight_00");
    hdParts_setSafeString(&sTouchGroups.mNames[10], &sTouchStaticStringVtable, "T_Num_00");
    hdParts_setSafeString(&sTouchGroups.mNames[11], &sTouchStaticStringVtable, "T_Time_00");
    hdParts_setSafeString(&sTouchAnimations.mNames[0], &sTouchStaticStringVtable, "TouchSlideDecide");
    hdParts_setSafeString(&sTouchAnimations.mNames[1], &sTouchStaticStringVtable, "TouchSlideDecideCmd");
    hdParts_setSafeString(&sTouchAnimations.mNames[2], &sTouchStaticStringVtable, "IconChange");
    hdParts_setSafeString(&sTouchAnimations.mNames[3], &sTouchStaticStringVtable, "ArrowLoop");
    hdParts_setSafeString(&sTouchAnimations.mNames[4], &sTouchStaticStringVtable, "ArrowLoop");
    hdParts_setSafeString(&sTouchAnimations.mNames[5], &sTouchStaticStringVtable, "ArrowLoop");
    hdParts_setSafeString(&sTouchAnimations.mNames[6], &sTouchStaticStringVtable, "Float");
    hdParts_setSafeString(&sTouchAnimations.mNames[7], &sTouchStaticStringVtable, "FloatCancel");
    hdParts_setSafeString(&sTouchAnimations.mNames[8], &sTouchStaticStringVtable, "Num");
    hdParts_setSafeString(&sTouchAnimations.mNames[9], &sTouchStaticStringVtable, "Switch");
    hdParts_setSafeString(&sTouchLayoutName, &sTouchStaticStringVtable, "BtnItemIcon_00");
    u32 number;
    if (sTouchStateCounterReady) number = sTouchStateCounter;
    else { number = 0; sTouchStateCounterReady = 1; }
    sTouchTouchSlideDecideState.setup(++number, "::StateID_TouchSlideDecide", &sTouchStateVtable, 0x4b, &sTouchNullState);
    sTouchTouchSlideDecideCmdState.setup(++number, "::StateID_TouchSlideDecideCmd", &sTouchStateVtable, 0x4e, &sTouchNullState);
    sTouchIconChangeState.setup(++number, "::StateID_IconChange", &sTouchStateVtable, 0x51, &sTouchNullState);
    sTouchArrowLoopState.setup(++number, "::StateID_ArrowLoop", &sTouchStateVtable, 0x54, &sTouchNullState);
    sTouchFloatState.setup(++number, "::StateID_Float", &sTouchStateVtable, 0x57, &sTouchNullState);
    sTouchFloatCancelState.setup(++number, "::StateID_FloatCancel", &sTouchStateVtable, 0x5a, &sTouchNullState);
    sTouchNumState.setup(++number, "::StateID_Num", &sTouchStateVtable, 0x5d, &sTouchNullState);
    sTouchSwitchState.setup(++number, "::StateID_Switch", &sTouchStateVtable, 0x60, &sTouchNullState);
    sTouchStateCounter = number;
}

/* WWHD glue
data sTouchHeaderStatics 1048FEF4
data sTouchHeaderDescriptors 101F550C
data sTouchHeaderZero 1048FF00
data sTouchPiConstants 100E8744
str 100E874C in 02635728
str 100E8750 in 02635728
str 100E8758 in 02635728
str 100E8760 in 02635728
str 100E8770 in 02635728
str 100E8780 in 02635728
str 100E8790 in 02635728
str 100E87A0 in 02635728
str 100E87B4 in 02635728
str 100E87C0 in 02635728
str 100E87D4 in 02635728
str 100E87E4 in 02635728
str 100E87F0 in 02635728
str 100E8800 in 02635728
str 100E8810 in 02635728
str 100E881C in 02635728
str 100E883C in 02635728
str 100E8854 in 02635728
str 100E8864 in 02635728
str 100E8870 in 02635728
str 100E887C in 02635728
str 100E8888 in 02635728
str 100E8898 in 02635728
str 100E88AC in 02635728
str 100E88B8 in 02635728
str 100E88C8 in 02635728
str 100E88D4 in 02635728
str 100E88E4 in 02635728
str 100E8900 in 02635728
str 100E8918 in 02635728
data sTouchStaticStringVtable 100E8624
data sTouchStateCounterReady 101FD8E4
data sTouchStateCounter 101FDD50
data sTouchStateVtable 100E86F4
data sTouchTouchSlideDecideState 1048FFC0
data sTouchNullState 101FF32C
data sTouchTouchSlideDecideCmdState 1048FFF0
data sTouchIconChangeState 10490020
data sTouchArrowLoopState 10490050
data sTouchFloatState 10490080
data sTouchFloatCancelState 104900B0
data sTouchNumState 104900E0
data sTouchSwitchState 10490110
data sTouchPictureType 101FD90C
data sTouchPictureBase 101FD910
data sTouchTextType 101FDD3C
data sTouchTextBase 101FD8F4
data sTouchTextPaneBase 101FD8F8
data sTouchPictureTypeInitialized 101FD908
data sTouchPictureBaseInitialized 101FD898
data sTouchTextTypeInitialized 101FDD40
data sTouchTextBaseInitialized 101FD8F0
data sTouchTextPaneBaseInitialized 101FD89C
data sTouchAnimations 1048FF70
data sTouchGroups 1048FF10
data sTouchGroupRemap 100E8684
data sTouchUiSystem 1018C404

data sTouchVtable 100E8930
data sTouchHolderVtable 100E8C48
data sTouchSecondaryVtable 100E8C58
data sTouchLayoutName 10490140
data sTouchHeapManager 101F8B4C
data sTouchSafeStringTable 100E79B8
data sTouchPaneType 101FD8FC
data sTouchPaneTypeInitialized 101FD888
ext 02704FC0 hdItemTypedPane_l* hdItem_findPane(void*, hdUiSafeString*, hdUiSafeString*)
str 100E8298 in 02634088
str 100E8299 in 0263420C
str 100E829A in 02634420
data sTouchPaneNames 1048FEEC
data sTouchWideStringVtable 100E863C
data sTouchWideBufferVtable 100E866C
ext 02635CC4 void hdTouchWide_assure(hdTouchWideString_l*)
ext C0009988 void hdTouchWide_copy(void*, const void*, u32, u32)
*/
