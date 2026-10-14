/* HD DungeonMapUp_00: captions and visibility of an upward-floor marker. */
#include "d/hd_screen_dungeon_map_up.h"
static u32 sScreenDungeonMapUpRegistration;
static u32 sScreenDungeonMapUpSecondRegistration;
static u32 sScreenDungeonMapUpThirdRegistration;
static hdPartsVtbl_c sUpVtable;
static u8 sUpHolderVtable;
static u8 sUpSecondaryVtable;
static hdUiStringTable sUpStringVtable;
static hdUiSafeString sUpArchiveName;
static hdUiSafeString sUpLayoutName;
static hdUiSafeString sUpFloorTextName;
static hdUiSafeString sUpOutdoorFloorTextName;
static hdUiSafeString sUpFirstFloorEntrance;
static hdUiSafeString sUpFirstFloor;
static hdUiSafeString sUpSecondFloor;
static hdUiSafeString sUpBasement;
struct hdDungeonUpFloorLabels { hdUiSafeString mLabels[hdDungeonMapUp_c::FLOOR_COUNT]; };
static hdDungeonUpFloorLabels sUpFloorLabels;
struct hdDungeonUpFloorEntry { gptr<hdUiSafeString> mpLabel; be<u32> mFloor; };
struct hdDungeonUpFloorTable { hdDungeonUpFloorEntry mEntries[hdDungeonMapUp_c::FLOOR_COUNT]; };
static hdDungeonUpFloorTable sUpFloorTable;
static u32 sUpFloorTextCapacity;
struct hdDungeonUpHeaderZero { be<u32> mWords[4]; };
static hdDungeonUpHeaderZero sUpHeaderZero;
static f32 sUpMinimumAngle;
static f32 sUpMaximumAngle;
static f32 sUpFarDistance;
static f32 sUpFarDistanceLimit;
static f32 sUpNearDistance;
static f32 sUpNearDistanceLimit;
static u8 sUpHeaderObject;
static u8 sUpHeaderOtherObject;

/* 0266680C */
void hdDungeonMapUp_c::destroy(hdDungeonMapUp_c* part, s32 flags) {
    if (part == NULL) return;
    part->mStateHolder.mHolderVtable = &sUpHolderVtable;
    part->mSecondaryVtable = &sUpSecondaryVtable;
    upTransitionDestroy(part, 0);
    if (flags & 1) upFree(part);
}

/* 02666878 */
hdDungeonMapUp_c* hdDungeonMapUp_c::construct(hdDungeonMapUp_c* part) {
    if (part == NULL) {
        part = upAllocate(sizeof(hdDungeonMapUp_c));
        if (part == NULL) return NULL;
    }
    upTransitionConstruct(part);
    part->mVtable = &sUpVtable;
    part->mSecondaryVtable = &sUpSecondaryVtable;
    part->mStateHolder.mHolderVtable = &sUpHolderVtable;
    /* The embedded four-byte pane holder still calls operator new when its guest address wraps. */
    if (u32(gabi::ea(part) + offsetof(hdDungeonMapUp_c, mpFloorText)) == 0) upAllocate(4);
    part->mpVisibilityPane = NULL;
    part->mpSourcePane = NULL;
    part->mDungeonEntrance = 0;
    part->mpFloorText = NULL;
    return part;
}

/* 02666908 */
void hdDungeonMapUp_c::findFloorText() {
    hdPartsLyt_c* layout = mpLayout->lyt();
    sUpFloorTextName.assureTermination();
    hdPartsPaneSRT_c* root = static_cast<hdPartsPaneSRT_c*>(layout->mpRootPane.get());
    mpFloorText = gabi::call_ptr<hdTextPane_c*>(root->srtVtbl()->mFindPane.get(), root, sUpFloorTextName.mText.get(), 1);
}

/* 02666984 */
void hdDungeonMapUp_c::resetFloorText() {
    if (mpFloorText == NULL) return;
    mpFloorText->setCharacterCapacity(sUpFloorTextCapacity);
    hdTextPane_c* text = mpFloorText;
    hdDungeonUpTextPaneVtbl_l* vtable = reinterpret_cast<hdDungeonUpTextPaneVtbl_l*>(text->mpVTable.get());
    gabi::call_ptr(vtable->mSetString.get(), text, "", 0);
}

/* 026669E8 */
u32 hdDungeonMapUp_c::create(u32 archiveArg, u32 layoutArg, void* description) {
    attachArchives(&sUpArchiveName, 1);
    if (!vLoad(&sUpLayoutName, gabi::ea(mpResAccessor.get()), archiveArg, layoutArg, description)) return FALSE;
    findFloorText();
    resetFloorText();
    hdUiSafeString root;
    root.mVtable = &sUpStringVtable;
    root.mText.v = gabi::guest_addr("");
    upTransitionBindPane(this, &root);
    vCalcLayout();
    return TRUE;
}

/* 02666AE8 */
void hdDungeonMapUp_c::release() { vDestroyLayout(); detachArchives(); }

/* 02666B28 */
void hdDungeonMapUp_c::stateExecute() { hdParts_executeStateMachine(stateMachine()); }

/* 02666B30 */
void hdDungeonMapUp_c::draw(void* info) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(stateMachine());
    u32 currentKeyFunction = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* blankVtable = sTransitionBlankState.mVtable;
    u32 currentKey = gabi::call_ptr<u32>(currentKeyFunction, current);
    u32 blankKey = gabi::call_ptr<u32>(blankVtable->mGetKey.get(), &sTransitionBlankState);
    if (currentKey != blankKey) vDrawLayout(info);
}

/* 02666BD4 */
u32 hdDungeonMapUp_c::isEntranceUnlocked() {
    switch (u32(hdDungeonMapUp_coordinator()->mStageKind)) {
    case 1: return dComIfGs_info()->mSavedata.mEvent.isEventBit(0x2A10) > 0;
    case 2: return dComIfGs_isSwitch(0x1C, 5) > 0;
    case 3: return dComIfGs_info()->mSavedata.mEvent.isEventBit(0x1B01) > 0;
    case 4: return dComIfGs_isSwitch(0x10, 3) > 0;
    default: return FALSE;
    }
}

/* 02666CB0 */
void hdDungeonMapUp_c::update() {
    if (mpSourcePane != NULL && mpVisibilityPane != NULL) {
        upTransitionUpdateBlend(this);
        upTransitionSetPosition(this, mpSourcePane);
        if (mDungeonEntrance != 0) mTransition.mHidden = isEntranceUnlocked() != 0;
        else mTransition.mHidden = mpVisibilityPane->mAlpha != 0;
    }
    upTransitionApply(this);
    hdParts_c::displayExecute();
}

/* 02666D54 */
void hdDungeonMapUp_c::setFloorText(hdTextPane_c* text, hdUiSafeString* label) {
    if (text == NULL) return;
    hdPartsLyt_c* layout = mpLayout->lyt();
    hdUiSafeString messages;
    messages.mVtable = &sUpStringVtable;
    messages.mText.v = gabi::ea(layout->mpMessageData.get());
    text->resetMessage(&messages, label);
}

/* 02666DA0 */
void hdDungeonMapUp_c::refreshFloorText() {
    hdDungeonUpFloorTable table;
    table = sUpFloorTable;
    if (hdDungeonMapUp_coordinator()->isDungeonMode()) {
        switch (u8(mTransition.mPaneIndex)) {
        case 0x81: setFloorText(mpFloorText, &sUpOutdoorFloorTextName); break;
        case 0x80: setFloorText(mpFloorText, &sUpFloorLabels.mLabels[2]); break;
        case 0x85: setFloorText(mpFloorText, &sUpFloorLabels.mLabels[1]); break;
        }
    } else {
        u32 floor = mTransition.mPaneIndex;
        for (u32 i = 0; i < FLOOR_COUNT; ++i) {
            if (floor == u32(table.mEntries[i].mFloor)) {
                setFloorText(mpFloorText, table.mEntries[i].mpLabel);
                break;
            }
        }
    }
}

/* 02666EA4 */
void hdDungeonMapUp_c::findDungeonEntrance() {
    hdDungeonUpNamedPane_l* source = mpSourcePane;
    if (source == NULL) return;
    hdUiSafeString* expected;
    switch (u32(hdDungeonMapUp_coordinator()->mStageKind)) {
    case 1: expected = &sUpFirstFloorEntrance; break;
    case 2: expected = &sUpFirstFloor; break;
    case 3: expected = &sUpSecondFloor; break;
    case 4: expected = &sUpBasement; break;
    default: return;
    }
    hdUiSafeString name;
    name.mVtable = &sUpStringVtable;
    name.mText.v = gabi::ea(source->mName);
    assureDungeonUpName(&name);
    name.assureTermination();
    /* the pane name is read before the expected name is terminated; the byte loop reads both again */
    be<u8>* sourceName = name.mText;
    expected->assureTermination();
    if (gabi::ea(sourceName) == gabi::ea(expected->mText.get())) {
        mDungeonEntrance = 1;
        return;
    }
    be<u8>* wanted = expected->mText;
    be<u8>* actualName = name.mText;
    enum { MAX_NAME_BYTES = 0x40001 };
    for (u32 i = 0; i < MAX_NAME_BYTES; ++i) {
        u8 actual = *hdPartsView<be<u8>>(gabi::ea(actualName) + i);
        u8 target = *hdPartsView<be<u8>>(gabi::ea(wanted) + i);
        if (actual != target) return;
        if (actual == 0) { mDungeonEntrance = 1; return; }
    }
}

/* 026671E8 */
static void initializeDungeonMapUpStatics() {
    sUpHeaderZero.mWords[3] = 0;
    sUpHeaderZero.mWords[2] = 0;
    sUpHeaderZero.mWords[1] = 0;
    sUpHeaderZero.mWords[0] = 0;
    __register_global_object(gabi::ea(&sScreenDungeonMapUpRegistration));
    sUpMinimumAngle = -3.1415927410125732f;
    sUpMaximumAngle = 3.1415927410125732f;
    constructUpHeaderObject(&sUpHeaderObject);
    __register_global_object(gabi::ea(&sScreenDungeonMapUpSecondRegistration));
    constructUpHeaderOtherObject(&sUpHeaderOtherObject);
    __register_global_object(gabi::ea(&sScreenDungeonMapUpThirdRegistration));
    hdParts_setSafeString(&sUpFirstFloorEntrance, &sUpStringVtable, "N_1FUp_01");
    sUpFarDistance = 50000.0f;
    sUpNearDistance = 10000.0f;
    sUpNearDistanceLimit = 10000.0f;
    hdParts_setSafeString(&sUpFloorLabels.mLabels[0], &sUpStringVtable, "T_FloorU_00_B1");
    hdParts_setSafeString(&sUpFloorLabels.mLabels[1], &sUpStringVtable, "T_FloorU_00_1F");
    sUpFarDistanceLimit = 50000.0f;
    hdParts_setSafeString(&sUpFloorLabels.mLabels[2], &sUpStringVtable, "T_FloorU_00_2F");
    hdParts_setSafeString(&sUpFloorLabels.mLabels[3], &sUpStringVtable, "T_FloorU_00_3F");
    hdParts_setSafeString(&sUpFloorLabels.mLabels[4], &sUpStringVtable, "T_FloorU_00_4F");
    hdParts_setSafeString(&sUpFirstFloor, &sUpStringVtable, "N_1FUp_00");
    hdParts_setSafeString(&sUpSecondFloor, &sUpStringVtable, "N_2FUp_00");
    hdParts_setSafeString(&sUpBasement, &sUpStringVtable, "N_B1Up_00");
    hdParts_setSafeString(&sUpFloorTextName, &sUpStringVtable, "T_Floor_00");
    hdParts_setSafeString(&sUpOutdoorFloorTextName, &sUpStringVtable, "T_FloorU_00_outdoor");
    hdParts_setSafeString(&sUpArchiveName, &sUpStringVtable, "DungeonMapUp_00");
    hdParts_setSafeString(&sUpLayoutName, &sUpStringVtable, "DungeonMapUp_00");
}

/* WWHD glue
 data sScreenDungeonMapUpRegistration 101F5E00 nocheck
 data sScreenDungeonMapUpSecondRegistration 101F5E0C nocheck
 data sScreenDungeonMapUpThirdRegistration 101F5E18 nocheck
data sUpVtable 100F3BE0 nocheck
data sUpHolderVtable 100F3D20 nocheck
data sUpSecondaryVtable 100F3D30 nocheck
data sUpStringVtable 100F3AD0 nocheck
data sUpArchiveName 10493C94 nocheck
data sUpLayoutName 10493CF0 nocheck
data sUpFloorTextName 10493C84 nocheck
data sUpOutdoorFloorTextName 10493C8C nocheck
data sUpFirstFloorEntrance 10493C64 nocheck
data sUpFirstFloor 10493C6C nocheck
data sUpSecondFloor 10493C74 nocheck
data sUpBasement 10493C7C nocheck
data sUpFloorLabels 10493CC8 nocheck
data sUpFloorTable 100F3AF4 nocheck
data sUpFloorTextCapacity 100F3ACC nocheck
data sUpHeaderZero 10493CB8 nocheck
data sUpMinimumAngle 10493C9C nocheck
data sUpMaximumAngle 10493CA0 nocheck
data sUpFarDistance 10493CA4 nocheck
data sUpFarDistanceLimit 10493CA8 nocheck
data sUpNearDistance 10493CAC nocheck
data sUpNearDistanceLimit 10493CB0 nocheck
data sUpHeaderObject 10493CB4 nocheck
data sUpHeaderOtherObject 10493CB5 nocheck
str 100F3AEC "" in 02666984
str 100F3AF0 "" in 026669E8
str 100F3B50 "N_1FUp_01" in 026671E8
str 100F3B5C "N_1FUp_00" in 026671E8
str 100F3B68 "N_2FUp_00" in 026671E8
str 100F3B74 "N_B1Up_00" in 026671E8
str 100F3B80 "T_Floor_00" in 026671E8
str 100F3B3C "T_FloorU_00_outdoor" in 026671E8
str 100F3B8C "T_FloorU_00_B1" in 026671E8
str 100F3B9C "T_FloorU_00_1F" in 026671E8
str 100F3BAC "T_FloorU_00_2F" in 026671E8
str 100F3BBC "T_FloorU_00_3F" in 026671E8
str 100F3BCC "T_FloorU_00_4F" in 026671E8
str 100F3B2C "DungeonMapUp_00" in 026671E8
ext 02684E94 void upTransitionDestroy(hdSeaChartStateHandlers_c*, s32)
ext 0273AF40 void upFree(hdDungeonMapUp_c*)
ext 0273AD10 hdDungeonMapUp_c* upAllocate(u32)
ext 02685134 hdSeaChartStateHandlers_c* upTransitionConstruct(hdSeaChartStateHandlers_c*)
ext 0268562C void upTransitionBindPane(hdSeaChartStateHandlers_c*, hdUiSafeString*)
ext 0268528C void upTransitionUpdateBlend(hdSeaChartStateHandlers_c*)
ext 026853E8 void upTransitionSetPosition(hdSeaChartStateHandlers_c*, hdSeaChartPane_l*)
ext 026852B4 void upTransitionApply(hdSeaChartStateHandlers_c*)
ext 02704AFC void hdTextPane_c::setCharacterCapacity(u32)
ext 027045B0 void hdTextPane_c::resetMessage(hdUiSafeString*, hdUiSafeString*)
ext 026673D8 void assureDungeonUpName(hdUiSafeString*)
ext 028ED6F8 void constructUpHeaderObject(void*)
ext 028EAB2C void constructUpHeaderOtherObject(void*)
*/
