/* HD DungeonMapTreasure_00: layout lifecycle, visibility and map-coordinate conversion of the treasure marker. */
#include "d/hd_screen_dungeon_map_treasure.h"
static u32 sScreenDungeonMapTreasureRegistration;
static u32 sScreenDungeonMapTreasureSecondRegistration;
static u32 sScreenDungeonMapTreasureThirdRegistration;
static hdPartsVtbl_c sTreasureVtable;
static u8 sTreasureHolderVtable;
static u8 sTreasureSecondaryVtable;
static hdUiStringTable sTreasureStringVtable;
static hdUiSafeString sTreasureArchiveName;
static hdUiSafeString sTreasureLayoutName;
struct hdDungeonTreasureHeaderZero { be<u32> mWords[4]; };
static hdDungeonTreasureHeaderZero sTreasureHeaderZero;
static f32 sTreasureMinimumAngle;
static f32 sTreasureMaximumAngle;
static f32 sTreasureFarDistance;
static f32 sTreasureFarDistanceLimit;
static f32 sTreasureNearDistance;
static f32 sTreasureNearDistanceLimit;
static u8 sTreasureHeaderObject;
static u8 sTreasureHeaderOtherObject;

/* 026662BC */
u32 getDungeonTreasureStateNumber(hdPartsStateId_c* state) { return state->mNumber; }

/* 026663BC */
void hdDungeonMapTreasure_c::destroy(hdDungeonMapTreasure_c* part, s32 flags) {
    if (part == NULL) return;
    part->mStateHolder.mHolderVtable = &sTreasureHolderVtable;
    part->mSecondaryVtable = &sTreasureSecondaryVtable;
    treasureTransitionDestroy(part, 0);
    if (flags & 1) treasureFree(part);
}

/* 02666428 */
hdDungeonMapTreasure_c* hdDungeonMapTreasure_c::construct(hdDungeonMapTreasure_c* part) {
    if (part == NULL) {
        part = treasureAllocate(sizeof(hdDungeonMapTreasure_c));
        if (part == NULL) return NULL;
    }
    treasureTransitionConstruct(part);
    part->mStateHolder.mHolderVtable = &sTreasureHolderVtable;
    part->mVtable = &sTreasureVtable;
    part->mSecondaryVtable = &sTreasureSecondaryVtable;
    part->mTreasureIndex = 0;
    return part;
}

/* 0266649C */
u32 hdDungeonMapTreasure_c::create(u32 archiveArg, u32 layoutArg, void* description) {
    attachArchives(&sTreasureArchiveName, 1);
    if (!vLoad(&sTreasureLayoutName, gabi::ea(mpResAccessor.get()), archiveArg, layoutArg, description)) return FALSE;
    hdUiSafeString root;
    root.mText.v = gabi::guest_addr("");
    root.mVtable = &sTreasureStringVtable;
    treasureTransitionBindPane(this, &root);
    return TRUE;
}

/* 02666578 */
void hdDungeonMapTreasure_c::release() {
    vDestroyLayout();
    detachArchives();
}

/* 026665B8 */
void hdDungeonMapTreasure_c::stateExecute() { hdParts_executeStateMachine(stateMachine()); }

/* 026665C0 */
void hdDungeonMapTreasure_c::drawLayoutVirtual(void* info) { vDrawLayout(info); }

/* 026665D0 */
void hdDungeonMapTreasure_c::update() {
    gabi::call_ptr(vtbl()->mUpdatePosition.get(), this);
    treasureTransitionUpdateBlend(this);
    treasureTransitionApply(this);
    hdParts_c::displayExecute();
}

/* 02666620 */
void hdDungeonMapTreasure_c::convertPosition(HdPadPoint* input, HdPadPoint* output) {
    hdSeaChartTransitionMap_l* map = mMap;
    if (map == NULL || output == NULL || map->mScale == NULL) return;
    f32 scale = *map->mScale;
    if (hdDungeonMapTreasure_coordinator()->isDungeonMode()) scale = 1.0f;
    output->x = gabi::fmuls_ppc(f32(input->x), scale);
    output->y = -gabi::fmuls_ppc(f32(input->y), scale);
}

/* 026666D0 */
void hdDungeonMapTreasure_c::setVisible(u32 visible) {
    u8 flags = mTransition.mReservedFlag;
    mTransition.mReservedFlag = visible != 0 ? flags | TREASURE_VISIBLE : flags & ~TREASURE_VISIBLE;
}

/* 026666F4 */
u32 hdDungeonMapTreasure_c::isVisible() { return u8(mTransition.mReservedFlag) & TREASURE_VISIBLE; }

/* 02666700 */
static void initializeDungeonMapTreasureStatics() {
    sTreasureHeaderZero.mWords[3] = 0;
    sTreasureHeaderZero.mWords[2] = 0;
    sTreasureHeaderZero.mWords[1] = 0;
    sTreasureHeaderZero.mWords[0] = 0;
    __register_global_object(gabi::ea(&sScreenDungeonMapTreasureRegistration));
    sTreasureMinimumAngle = -3.1415927410125732f;
    sTreasureMaximumAngle = 3.1415927410125732f;
    constructTreasureHeaderObject(&sTreasureHeaderObject);
    __register_global_object(gabi::ea(&sScreenDungeonMapTreasureSecondRegistration));
    constructTreasureHeaderOtherObject(&sTreasureHeaderOtherObject);
    __register_global_object(gabi::ea(&sScreenDungeonMapTreasureThirdRegistration));
    sTreasureNearDistance = 10000.0f;
    hdParts_setSafeString(&sTreasureArchiveName, &sTreasureStringVtable, "DungeonMapTreasure_00");
    sTreasureNearDistanceLimit = 10000.0f;
    sTreasureFarDistanceLimit = 50000.0f;
    sTreasureFarDistance = 50000.0f;
    hdParts_setSafeString(&sTreasureLayoutName, &sTreasureStringVtable, "DungeonMapTreasure_00");
}

/* WWHD glue
 data sScreenDungeonMapTreasureRegistration 101F5DDC nocheck
 data sScreenDungeonMapTreasureSecondRegistration 101F5DE8 nocheck
 data sScreenDungeonMapTreasureThirdRegistration 101F5DF4 nocheck
data sTreasureVtable 100F3958 nocheck
data sTreasureHolderVtable 100F3A98 nocheck
data sTreasureSecondaryVtable 100F3AA8 nocheck
data sTreasureStringVtable 100F390C nocheck
data sTreasureArchiveName 10493C28 nocheck
data sTreasureLayoutName 10493C5C nocheck
data sTreasureHeaderZero 10493C4C nocheck
data sTreasureMinimumAngle 10493C30 nocheck
data sTreasureMaximumAngle 10493C34 nocheck
data sTreasureFarDistance 10493C38 nocheck
data sTreasureFarDistanceLimit 10493C3C nocheck
data sTreasureNearDistance 10493C40 nocheck
data sTreasureNearDistanceLimit 10493C44 nocheck
data sTreasureHeaderObject 10493C48 nocheck
data sTreasureHeaderOtherObject 10493C49 nocheck
str 100F392C "" in 0266649C
str 100F3940 "DungeonMapTreasure_00" in 02666700
ext 02684E94 void treasureTransitionDestroy(hdSeaChartStateHandlers_c*, s32)
ext 0273AF40 void treasureFree(hdDungeonMapTreasure_c*)
ext 0273AD10 hdDungeonMapTreasure_c* treasureAllocate(u32)
ext 02685134 hdSeaChartStateHandlers_c* treasureTransitionConstruct(hdSeaChartStateHandlers_c*)
ext 0268562C void treasureTransitionBindPane(hdSeaChartStateHandlers_c*, hdUiSafeString*)
ext 0268528C void treasureTransitionUpdateBlend(hdSeaChartStateHandlers_c*)
ext 026852B4 void treasureTransitionApply(hdSeaChartStateHandlers_c*)
ext 028ED6F8 void constructTreasureHeaderObject(void*)
ext 028EAB2C void constructTreasureHeaderOtherObject(void*)
ext 0268714C u32 hdMapDisplayCoordinator_c::isDungeonMode()
*/
