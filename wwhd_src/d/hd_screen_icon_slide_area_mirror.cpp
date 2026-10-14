/* HD equipped-item slide-area animation mirror. */
#include "d/hd_screen_icon_slide_area_mirror.h"

static hdAnimSetUiSystem* sMirrorUiSystem;
static hdIconSlideGroupIndices_c sMirrorGroupIndices;
static hdIconSlideGroupNames_c sMirrorGroupNames;
static hdIconSlideAnimationNames_c sMirrorAnimationNames;


static u32 sMirrorPaneTypeGuard;
static u32 sMirrorPaneParentGuard;
static u32 sMirrorRootTypeGuard;
static hdScreenType sMirrorPaneType;
static hdScreenType sMirrorPaneParent;
static hdScreenType sMirrorRootType;
static u32 sMirrorSafeStringVtable;

static hdIconSlideItemPaneNames_c sMirrorChildPaneNames;
static hdUiSafeString sMirrorItemClassName;
static hdIconSlideStartup_c sMirrorStartup;
static hdIconSlideDestructorRecords_c sMirrorDestructorRecords;
static hdUiStringTable sMirrorLiteralVtable;
static hdPartsStateIdVtbl_c sMirrorStateVtable;
static hdStateId sMirrorNullState;
static u32 sMirrorStateCounterGuard;
static u32 sMirrorStateCounter;
static hdPartsStateId_c sMirrorFloatState;
static hdPartsStateId_c sMirrorCancelState;
static hdPartsStateId_c sMirrorSwitchState;
static hdIconSlideItemPaneNames_c sMirrorItemClassDescriptors;

static hdUiSafeString sMirrorLayoutName;
static hdPartsVtbl_c sMirrorPartsVtable;
static u32 sMirrorHolderVtable;
static u32 sMirrorSecondaryVtable;

/* 0267101C */
void hdIconSlideAreaMirror_c::enterFloat() {
    hdAnimSet_play(animSet(), ICON_FLOAT_Y, ICON_DROP_Y, 1.0f);
    hdAnimSet_play(animSet(), ICON_FLOAT_X, ICON_DROP_X, 1.0f);
    hdAnimSet_play(animSet(), ICON_FLOAT_R, ICON_DROP_R, 1.0f);
}

/* 026710AC */
void hdIconSlideAreaMirror_c::executeFloat() {
    vCalcLayout();
}

/* 026710BC */
void hdIconSlideAreaMirror_c::enterFloatCancel() {
    hdAnimSet_play(animSet(), ICON_CANCEL_Y, ICON_DROP_Y, 1.0f);
    hdAnimSet_play(animSet(), ICON_CANCEL_X, ICON_DROP_X, 1.0f);
    hdAnimSet_play(animSet(), ICON_CANCEL_R, ICON_DROP_R, 1.0f);
}

/* 0267114C */
void hdIconSlideAreaMirror_c::executeFloatCancel() {
    vCalcLayout();
}

/* 0267115C */
void hdIconSlideAreaMirror_c::enterSwitch() {
    hdAnimSet_play(animSet(), ICON_SWITCH_X, ICON_GLOW_X, 1.0f);
    hdAnimSet_play(animSet(), ICON_SWITCH_Y, ICON_GLOW_Y, 1.0f);
    hdAnimSet_play(animSet(), ICON_SWITCH_R, ICON_GLOW_R, 1.0f);
}

/* 026711EC */
void hdIconSlideAreaMirror_c::executeSwitch() {
    vCalcLayout();
}

/* 02670FB0 */
void hdIconSlideAreaMirror_c::destroy(hdIconSlideAreaMirror_c* area, s32 flags) {
    if (area) {
        area->mStateHolder.mHolderVtable = &sMirrorHolderVtable;
        area->mSecondaryVtable = &sMirrorSecondaryVtable;
        iconSlideDestroyBase(area, 0);
        if (flags & 1) iconSlideDelete(area);
    }
}

/* 026711FC */
hdIconSlideAreaMirror_c* hdIconSlideAreaMirror_c::construct(hdIconSlideAreaMirror_c* area) {
    if (!area) {
        area = static_cast<hdIconSlideAreaMirror_c*>(iconSlideAllocate(sizeof(hdIconSlideAreaMirror_c)));
        if (!area) return area;
    }
    iconSlideConstructBase(area);
    area->mStateHolder.mHolderVtable = &sMirrorHolderVtable;
    area->mVtable = &sMirrorPartsVtable;
    area->mSecondaryVtable = &sMirrorSecondaryVtable;
    area->mpSourceAnimations = nullptr;
    return area;
}

/* 026713F8 */
void hdIconSlideAreaMirror_c::setSourceAnimations(gptr<hdAnimSet>* source) {
    mpSourceAnimations.v = source->v.get();
}

/* 02671288 */
void hdIconSlideAreaMirror_c::syncAnimations() {
    hdAnimSet* source = mpSourceAnimations;
    if (!source) return;
    for (u32 i = 0; i < 9; ++i) {
        hdAnimSetEntry* sourceEntry = *source->mEntries.at(i);
        hdAnimSet* destination = animSet();
        hdAnimSetEntry* destinationEntry = *destination->mEntries.at(i);
        if (!sourceEntry || !destinationEntry) continue;
        hdAnimSetController* sourceController = sourceEntry->mController;
        hdAnimSetController* destinationController = destinationEntry->mController;
        if (sourceController->mPlaying != destinationController->mPlaying) {
            if (sourceController->mPlaying != 0) {
                destinationController->setPlaying(1);
                destinationEntry->mController->operationCC();
            } else {
                destinationController->setPlaying(0);
            }
            destinationController = destinationEntry->mController;
        }
        if (destinationController->mPlaying != 0) {
            sourceController = sourceEntry->mController;
            // The canonical mSpeed field at +0xC holds the frame in this controller.
            f32 destinationFrame = destinationController->mSpeed;
            f32 sourceFrame = sourceController->mSpeed;
            if (sourceFrame != destinationFrame) {
                destinationEntry->mController->setFrame(sourceController->mSpeed);
            }
        }
    }
}

/* 026713B8 */
void hdIconSlideAreaMirror_c::execute() {
    syncAnimations();
    vExecuteChildren();
}

/* 0267165C */
u32 hdIconSlideAreaMirror_c::loadLayout(void* name, u32 a, u32 b, u32 c, void* desc) {
    void* heap = sMirrorUiSystem->mHeap;
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(iconSlideAllocateOnHeap(sizeof(hdPartsLayout_c), heap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, a, b, c, 9, 6, desc)) return 0;
    for (u32 i = 0; i < 9; ++i) {
        hdAnimSet* animations = animSet();
        u32 group = sMirrorGroupIndices.mIndices[i];
        iconSlideBindAnimation(animations, i, &sMirrorAnimationNames.mNames[i], &sMirrorGroupNames.mNames[group]);
    }
    return 1;
}

/* 02671270 */
u32 hdIconSlideAreaMirror_c::load(u32 first, u32 second, hdPartsDesc_c* desc) {
    return hdPartsNoControl_init(this, &sMirrorLayoutName, first, second, desc);
}

/* 02670C9C */
static hdScreenType* iconSlideParentType(hdScreenType* type) {
    return type->mParent;
}

/* 02670D9C */
u32 iconSlideBindPanes(hdPartsLayout_c* layout, hdIconSlidePaneArray_c* panes, hdUiSafeString* names) {
    u32 success = 1;
    s32 count = panes->mCount;
    gptr<hdPartsPane_c>* data = panes->mpData;
    for (s32 i = 0; i < count; ++i) {
        u32 index = u32(i) < u32(count) ? u32(i) : 0;
        gptr<hdPartsPane_c>* slot = hdPartsView<gptr<hdPartsPane_c>>(gabi::ea(data) + index * 4);
        hdUiSafeString prefix;
        prefix.mText.v = gabi::guest_addr("");
        prefix.mVtable = reinterpret_cast<hdUiStringTable*>(&sMirrorSafeStringVtable);
        hdUiSafeString name;
        names[i].assureTermination();
        name.mText.v = names[i].mText.v.get();
        name.mVtable = reinterpret_cast<hdUiStringTable*>(&sMirrorSafeStringVtable);
        hdPartsPane_c* pane = iconSlideFindPane(layout, &prefix, &name);
        if (!sMirrorPaneTypeGuard) {
            sMirrorPaneTypeGuard = 1;
            if (!sMirrorPaneParentGuard) {
                sMirrorPaneParentGuard = 1;
                if (!sMirrorRootTypeGuard) {
                    sMirrorRootType.mParent = nullptr;
                    sMirrorRootTypeGuard = 1;
                }
                sMirrorPaneParent.mParent = &sMirrorRootType;
            }
            sMirrorPaneType.mParent = &sMirrorPaneParent;
        }
        hdPartsPane_c* accepted = nullptr;
        if (pane) {
            hdScreenType* type = pane->getRuntimeTypeInfo();
            while (type) {
                if (type == &sMirrorPaneType) { accepted = pane; break; }
                type = type->mParent;
            }
        }
        *slot = accepted;
        count = panes->mCount;
        data = panes->mpData;
        index = u32(i) < u32(count) ? u32(i) : 0;
        if (!data[index]) success = 0;
    }
    return success;
}

/* inlined into hdIconSlideAreaMirror_c::createChild */
static bool mirrorIconSlideNamesMatch(hdUiSafeString* actual, hdUiSafeString* expected) {
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

/* 02671404 */
u32 hdIconSlideAreaMirror_c::createChild(hdPartsDesc_c* desc) {
    void* heap = sMirrorUiSystem->mHeap;
    auto* description = reinterpret_cast<hdIconSlideChildDesc_l*>(desc);
    for (u32 i = 0; i < hdIconSlideArea_c::ITEM_SLOT_COUNT; ++i) {
        if (!mirrorIconSlideNamesMatch(&description->mPaneName, &sMirrorChildPaneNames.mNames[i])) continue;
        if (!mirrorIconSlideNamesMatch(&description->mClassName, &sMirrorItemClassName)) {
            return iconSlideCreateChildBase(this, i, desc);
        }
        gptr<hdParts_c>* slot = childSlot(i);
        void* storage = iconSlideAllocateOnHeap(0xBC, heap, 4);
        hdParts_c* item = static_cast<hdParts_c*>(storage);
        if (storage) item = iconSlideMirrorItemConstruct(item);
        *slot = item;
        if (!child(i)) return 0;
        return iconSlideMirrorItemInitialize(child(i), 0, 0, desc);
    }
    return 1;
}

/* 02671774 */
static void initializeIconSlideAreaMirror() {
    for (s32 i = 3; i >= 0; --i) sMirrorStartup.mZeros[i] = 0;
    iconSlideRegisterGlobal(sMirrorDestructorRecords.mRecords[0]);
    sMirrorStartup.mMinAngle = -3.1415927410125732f;
    sMirrorStartup.mMaxAngle = 3.1415927410125732f;
    iconSlideInitializeFirstFlag(&sMirrorStartup.mFlags[0]);
    iconSlideRegisterGlobal(sMirrorDestructorRecords.mRecords[1]);
    iconSlideInitializeSecondFlag(&sMirrorStartup.mFlags[1]);
    iconSlideRegisterGlobal(sMirrorDestructorRecords.mRecords[2]);
    hdParts_setSafeString(&sMirrorGroupNames.mNames[0], &sMirrorLiteralVtable, "G_XGlow_00");
    hdParts_setSafeString(&sMirrorGroupNames.mNames[1], &sMirrorLiteralVtable, "G_YGlow_00");
    hdParts_setSafeString(&sMirrorGroupNames.mNames[2], &sMirrorLiteralVtable, "G_RGlow_00");
    hdParts_setSafeString(&sMirrorGroupNames.mNames[3], &sMirrorLiteralVtable, "G_XDropSlideFloat_00");
    hdParts_setSafeString(&sMirrorGroupNames.mNames[4], &sMirrorLiteralVtable, "G_YDropSlideFloat_00");
    hdParts_setSafeString(&sMirrorGroupNames.mNames[5], &sMirrorLiteralVtable, "G_RDropSlideFloat_00");
    hdParts_setSafeString(&sMirrorChildPaneNames.mNames[0], &sMirrorLiteralVtable, "L_XItemIcon_00");
    hdParts_setSafeString(&sMirrorChildPaneNames.mNames[1], &sMirrorLiteralVtable, "L_YItemIcon_00");
    hdParts_setSafeString(&sMirrorChildPaneNames.mNames[2], &sMirrorLiteralVtable, "L_RItemIcon_00");
    hdParts_setSafeString(&sMirrorItemClassDescriptors.mNames[0], &sMirrorLiteralVtable, "BtnItemIcon_00");
    hdParts_setSafeString(&sMirrorItemClassDescriptors.mNames[1], &sMirrorLiteralVtable, "BtnItemIcon_00");
    hdParts_setSafeString(&sMirrorItemClassDescriptors.mNames[2], &sMirrorLiteralVtable, "BtnItemIcon_00");
    hdParts_setSafeString(&sMirrorAnimationNames.mNames[0], &sMirrorLiteralVtable, "Float");
    hdParts_setSafeString(&sMirrorAnimationNames.mNames[1], &sMirrorLiteralVtable, "Float");
    hdParts_setSafeString(&sMirrorAnimationNames.mNames[2], &sMirrorLiteralVtable, "Float");
    hdParts_setSafeString(&sMirrorAnimationNames.mNames[3], &sMirrorLiteralVtable, "FloatCancel");
    hdParts_setSafeString(&sMirrorAnimationNames.mNames[4], &sMirrorLiteralVtable, "FloatCancel");
    hdParts_setSafeString(&sMirrorAnimationNames.mNames[5], &sMirrorLiteralVtable, "FloatCancel");
    hdParts_setSafeString(&sMirrorAnimationNames.mNames[6], &sMirrorLiteralVtable, "Switch");
    hdParts_setSafeString(&sMirrorAnimationNames.mNames[7], &sMirrorLiteralVtable, "Switch");
    hdParts_setSafeString(&sMirrorAnimationNames.mNames[8], &sMirrorLiteralVtable, "Switch");
    hdParts_setSafeString(&sMirrorLayoutName, &sMirrorLiteralVtable, "IconSlideArea_00");
    u32 counter;
    if (sMirrorStateCounterGuard) counter = sMirrorStateCounter;
    else { sMirrorStateCounterGuard = 1; counter = 0; }
    sMirrorFloatState.setup(++counter, "::StateID_Float", &sMirrorStateVtable, 0x19, &sMirrorNullState);
    sMirrorCancelState.setup(++counter, "::StateID_FloatCancel", &sMirrorStateVtable, 0x1C, &sMirrorNullState);
    sMirrorStateCounter = ++counter;
    sMirrorSwitchState.setup(counter, "::StateID_Switch", &sMirrorStateVtable, 0x1F, &sMirrorNullState);
}

/* WWHD glue
 data sMirrorLayoutName 10494CEC
 data sMirrorUiSystem 1018C404
 data sMirrorGroupIndices 100F6134
 data sMirrorGroupNames 10494BB4
 data sMirrorAnimationNames 10494C14
 data sMirrorPartsVtable 100F6310
 data sMirrorHolderVtable 100F6420
 data sMirrorSecondaryVtable 100F6430
 data sMirrorPaneTypeGuard 101FD914
 data sMirrorPaneParentGuard 101FD890
 data sMirrorRootTypeGuard 101FD888
 data sMirrorPaneType 101FD918
 data sMirrorPaneParent 101FD91C
 data sMirrorRootType 101FD8FC
 data sMirrorSafeStringVtable 100F5DBC
 ext 02704FC0 hdPartsPane_c* iconSlideFindPane(hdPartsLayout_c*, hdUiSafeString*, hdUiSafeString*)
 str 100F6110
 data sMirrorChildPaneNames 10494BE4
 data sMirrorItemClassName 10490140
 ext 0263489C hdParts_c* iconSlideMirrorItemConstruct(hdParts_c*)
 ext 02634B98 u32 iconSlideMirrorItemInitialize(hdParts_c*, u32, u32, hdPartsDesc_c*)
 data sMirrorStartup 10494B98
 data sMirrorDestructorRecords 101F6064
 data sMirrorLiteralVtable 100F611C
 data sMirrorStateVtable 100F61A0
 data sMirrorNullState 101FF32C
 data sMirrorStateCounterGuard 101FD8E4
 data sMirrorStateCounter 101FDD50
 data sMirrorFloatState 10494C5C
 data sMirrorCancelState 10494C8C
 data sMirrorSwitchState 10494CBC
 data sMirrorItemClassDescriptors 10494BFC
 str 100F6238 "G_XGlow_00" in 02671774
 str 100F6244 "G_YGlow_00" in 02671774
 str 100F6250 "G_RGlow_00" in 02671774
 str 100F625C "G_XDropSlideFloat_00" in 02671774
 str 100F6274 "G_YDropSlideFloat_00" in 02671774
 str 100F628C "G_RDropSlideFloat_00" in 02671774
 str 100F62A4 "L_XItemIcon_00" in 02671774
 str 100F62B4 "L_YItemIcon_00" in 02671774
 str 100F62C4 "L_RItemIcon_00" in 02671774
 str 100F62D4 "BtnItemIcon_00" in 02671774
 str 100F61F4 "Float" in 02671774
 str 100F6214 "FloatCancel" in 02671774
 str 100F61FC "Switch" in 02671774
 str 100F62E4 "IconSlideArea_00" in 02671774
 str 100F6204 "::StateID_Float" in 02671774
 str 100F6220 "::StateID_FloatCancel" in 02671774
 str 100F62F8 "::StateID_Switch" in 02671774
*/
