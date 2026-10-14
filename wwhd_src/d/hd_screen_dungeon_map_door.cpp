/* HD dungeon-map door marker: layout loading and pane binding. */
#include "d/hd_dungeon_map_door.h"
static hdPartsVtbl_c sDoorMarkerVtable;
static u8 sDoorMarkerOwnerVtable;
static u8 sDoorMarkerSecondaryVtable;
static u8 sDoorMarkerStringVtable;
static hdUiSafeString sDoorMarkerArchive;
static hdUiSafeString sDoorMarkerLayout;
void doorConstructTransition(hdSeaChartStateHandlers_c*);
void doorDestroyTransition(hdSeaChartStateHandlers_c*, u32);

/* 0265FF18 */
void hdDungeonMapDoor::destroy(u32 flags) {
    if (this == NULL) return;
    mStateHolder.mHolderVtable = &sDoorMarkerOwnerVtable;
    mSecondaryVtable = &sDoorMarkerSecondaryVtable;
    doorDestroyTransition(this, 0);
    if (flags & 1) operator_delete(this);
}

/* 0265FF84 */
hdDungeonMapDoor* hdDungeonMapDoor::construct(hdDungeonMapDoor* self) {
    if (self == NULL) {
        self = static_cast<hdDungeonMapDoor*>(operator_new(sizeof(hdDungeonMapDoor)));
        if (self == NULL) return NULL;
    }
    doorConstructTransition(self);
    self->mVtable = &sDoorMarkerVtable;
    self->mStateHolder.mHolderVtable = &sDoorMarkerOwnerVtable;
    self->mSecondaryVtable = &sDoorMarkerSecondaryVtable;
    self->mPreviousDoorIndex = -1;
    self->mDoorIndex = -1;
    return self;
}

/* 0265FFFC */
u32 hdDungeonMapDoor::loadMarker(u32 heap, u32 resource, void* context) {
    attachArchives( &sDoorMarkerArchive, 1);
    if (!vLoad(&sDoorMarkerLayout, gabi::ea(mpResAccessor.get()), heap, resource, context))
        return 0;
    hdUiSafeString pane;
    pane.mVtable = reinterpret_cast<hdUiStringTable*>(&sDoorMarkerStringVtable);
    pane.mText.v = gabi::guest_addr("");
    bindPane( &pane);
    return 1;
}

static hdDungeonMarkerStatics sDoorMarkerStatics;
static hdDungeonMarkerRegistration sDoorStorageRegistration;
static hdDungeonMarkerRegistration sDoorFirstFlagRegistration;
static hdDungeonMarkerRegistration sDoorSecondFlagRegistration;
static f32 sDoorMinimum;
static f32 sDoorMaximum;
static f32 sDoorOrigin;
static f32 sDoorOffset;
void markerRegisterStatic(hdDungeonMarkerRegistration*);
void markerConstructFirstFlag(be<u8>*);
void markerConstructSecondFlag(be<u8>*);
/* 026601E0 */
static void initializeDoorMarkerStatics() {
    sDoorMarkerStatics.mStorage[3] = 0;
    sDoorMarkerStatics.mStorage[2] = 0;
    sDoorMarkerStatics.mStorage[1] = 0;
    sDoorMarkerStatics.mStorage[0] = 0;
    markerRegisterStatic(&sDoorStorageRegistration);
    const f32 minimum = sDoorMinimum;
    const f32 maximum = sDoorMaximum;
    sDoorMarkerStatics.mMinimum = minimum;
    sDoorMarkerStatics.mMaximum = maximum;
    markerConstructFirstFlag(&sDoorMarkerStatics.mFirstFlag);
    markerRegisterStatic(&sDoorFirstFlagRegistration);
    markerConstructSecondFlag(&sDoorMarkerStatics.mSecondFlag);
    markerRegisterStatic(&sDoorSecondFlagRegistration);
    const f32 offset = sDoorOffset;
    sDoorMarkerStatics.mOffset.x = offset;
    const f32 origin = sDoorOrigin;
    sDoorMarkerArchive.mVtable = reinterpret_cast<hdUiStringTable*>(&sDoorMarkerStringVtable);
    sDoorMarkerStatics.mOffset.y = offset;
    sDoorMarkerArchive.mText.v = gabi::guest_addr("DungeonMapDoor_00");
    sDoorMarkerStatics.mOrigin.y = origin;
    sDoorMarkerStatics.mOrigin.x = origin;
    sDoorMarkerLayout.mVtable = reinterpret_cast<hdUiStringTable*>(&sDoorMarkerStringVtable);
    sDoorMarkerLayout.mText.v = gabi::guest_addr("DungeonMapDoor_00");
}

static HdPadUiPointer sDoorDoorUi;
static f32 sDoorDoorUnitScale;
u32 doorUiUsesUnitScale(HdPadUi*);
/* 026600D8 */
void hdDungeonMapDoor::finalizeDoor() {
 vDestroyLayout();
 detachArchives();
}
/* 02660118 */
void hdDungeonMapDoor::executeDoor() { hdParts_executeStateMachine(stateMachine()); }
/* 02660120 */
void hdDungeonMapDoor::drawDoor(void* info) { vDrawLayout(info); }
/* 02660130 */
void hdDungeonMapDoor::scalePosition(HdPadPoint* input, HdPadPoint* output) {
 hdSeaChartTransitionMap_l* map = mMap;
 if (!map || !output || !map->mScale) return;
 f32 scale = *map->mScale;
 if (doorUiUsesUnitScale(sDoorDoorUi.mOwner->mUi)) scale = sDoorDoorUnitScale;
 output->x = gabi::fmuls_ppc(f32(input->x), scale);
 output->y = -gabi::fmuls_ppc(f32(input->y), scale);
}
/* WWHD glue
 data sDoorDoorUi 101F8344
 data sDoorDoorUnitScale 100F0C48
 ext 0268714C u32 doorUiUsesUnitScale(HdPadUi*)

 data sDoorMarkerStatics 1049272C
 data sDoorStorageRegistration 101F5C08
 data sDoorFirstFlagRegistration 101F5C14
 data sDoorSecondFlagRegistration 101F5C20
 data sDoorMinimum 100F0C50
 data sDoorMaximum 100F0C54
 data sDoorOrigin 100F0C58
 data sDoorOffset 100F0C5C
 str 100F0C60 in 026601E0
 data sDoorMarkerVtable 100F0C78
 data sDoorMarkerOwnerVtable 100F0DB8
 data sDoorMarkerSecondaryVtable 100F0DC8
 data sDoorMarkerStringVtable 100F0C2C
 data sDoorMarkerArchive 10492724
 data sDoorMarkerLayout 10492758
 str 100F0C4C in 0265FFFC
 ext 02684E94 void doorDestroyTransition(hdSeaChartStateHandlers_c*,u32)
 ext 02685134 void doorConstructTransition(hdSeaChartStateHandlers_c*)

*/
