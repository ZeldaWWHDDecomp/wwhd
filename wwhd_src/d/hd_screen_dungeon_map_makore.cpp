/* HD dungeon-map makore marker: layout loading and pane binding. */
#include "d/hd_dungeon_map_markers.h"
#include "d/hd_sea_chart_state_handlers.h"
static hdPartsVtbl_c sMakoreMarkerVtable;
static u8 sMakoreMarkerOwnerVtable;
static u8 sMakoreMarkerSecondaryVtable;
static u8 sMakoreMarkerStringVtable;
static hdUiSafeString sMakoreMarkerArchive;
static hdUiSafeString sMakoreMarkerLayout;

/* 0266548C */
void hdDungeonMapMakore::destroy(u32 flags) {
    if (this == NULL) return;
    mStateHolder.mHolderVtable = &sMakoreMarkerOwnerVtable;
    mSecondaryVtable = &sMakoreMarkerSecondaryVtable;
    hdMenuMapPanePart::destroy(0);
    if (flags & 1) operator_delete(this);
}

/* 026654F8 */
hdDungeonMapMakore* hdDungeonMapMakore::construct(hdDungeonMapMakore* self) {
    if (self == NULL) {
        self = static_cast<hdDungeonMapMakore*>(operator_new(sizeof(hdDungeonMapMakore)));
        if (self == NULL) return NULL;
    }
    hdMenuMapPanePart::construct(self);
    self->mVtable = &sMakoreMarkerVtable;
    self->mStateHolder.mHolderVtable = &sMakoreMarkerOwnerVtable;
    self->mSecondaryVtable = &sMakoreMarkerSecondaryVtable;
    return self;
}

/* 02665564 */
u32 hdDungeonMapMakore::loadMarker(u32 heap, u32 resource, void* context) {
    attachArchives( &sMakoreMarkerArchive, 1);
    if (!vLoad(&sMakoreMarkerLayout, gabi::ea(mpResAccessor.get()), heap, resource, context))
        return 0;
    hdUiSafeString pane;
    pane.mVtable = reinterpret_cast<hdUiStringTable*>(&sMakoreMarkerStringVtable);
    pane.mText.v = gabi::guest_addr("N_Rotate_00");
    reinterpret_cast<hdSeaChartStateHandlers_c*>(this)->bindRootAndPane( &pane);
    return 1;
}

static hdDungeonMarkerStatics sMakoreMarkerStatics;
static hdDungeonMarkerRegistration sMakoreStorageRegistration;
static hdDungeonMarkerRegistration sMakoreFirstFlagRegistration;
static hdDungeonMarkerRegistration sMakoreSecondFlagRegistration;
static f32 sMakoreMinimum;
static f32 sMakoreMaximum;
static f32 sMakoreOrigin;
static f32 sMakoreOffset;
void markerRegisterStatic(hdDungeonMarkerRegistration*);
void markerConstructFirstFlag(be<u8>*);
void markerConstructSecondFlag(be<u8>*);
/* 02665640 */
static void initializeMakoreMarkerStatics() {
    sMakoreMarkerStatics.mStorage[3] = 0;
    sMakoreMarkerStatics.mStorage[2] = 0;
    sMakoreMarkerStatics.mStorage[1] = 0;
    sMakoreMarkerStatics.mStorage[0] = 0;
    markerRegisterStatic(&sMakoreStorageRegistration);
    const f32 minimum = sMakoreMinimum;
    const f32 maximum = sMakoreMaximum;
    sMakoreMarkerStatics.mMinimum = minimum;
    sMakoreMarkerStatics.mMaximum = maximum;
    markerConstructFirstFlag(&sMakoreMarkerStatics.mFirstFlag);
    markerRegisterStatic(&sMakoreFirstFlagRegistration);
    markerConstructSecondFlag(&sMakoreMarkerStatics.mSecondFlag);
    markerRegisterStatic(&sMakoreSecondFlagRegistration);
    const f32 offset = sMakoreOffset;
    sMakoreMarkerStatics.mOffset.x = offset;
    const f32 origin = sMakoreOrigin;
    sMakoreMarkerArchive.mVtable = reinterpret_cast<hdUiStringTable*>(&sMakoreMarkerStringVtable);
    sMakoreMarkerStatics.mOffset.y = offset;
    sMakoreMarkerArchive.mText.v = gabi::guest_addr("DungeonMapMakore_00");
    sMakoreMarkerStatics.mOrigin.y = origin;
    sMakoreMarkerStatics.mOrigin.x = origin;
    sMakoreMarkerLayout.mVtable = reinterpret_cast<hdUiStringTable*>(&sMakoreMarkerStringVtable);
    sMakoreMarkerLayout.mText.v = gabi::guest_addr("DungeonMapMakore_00");
}

/* WWHD glue
 data sMakoreMarkerStatics 10493B00
 data sMakoreStorageRegistration 101F5D4C
 data sMakoreFirstFlagRegistration 101F5D58
 data sMakoreSecondFlagRegistration 101F5D64
 data sMakoreMinimum 100F3148
 data sMakoreMaximum 100F314C
 data sMakoreOrigin 100F3150
 data sMakoreOffset 100F3154
 str 100F3158 in 02665640
 data sMakoreMarkerVtable 100F3170
 data sMakoreMarkerOwnerVtable 100F32B0
 data sMakoreMarkerSecondaryVtable 100F32C0
 data sMakoreMarkerStringVtable 100F311C
 data sMakoreMarkerArchive 10493AF8
 data sMakoreMarkerLayout 10493B2C
 str 100F313C in 02665564

*/
