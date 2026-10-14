/* HD dungeon-map medley marker: layout loading and pane binding. */
#include "d/hd_dungeon_map_markers.h"
#include "d/hd_sea_chart_state_handlers.h"
static hdPartsVtbl_c sMedleyMarkerVtable;
static u8 sMedleyMarkerOwnerVtable;
static u8 sMedleyMarkerSecondaryVtable;
static u8 sMedleyMarkerStringVtable;
static hdUiSafeString sMedleyMarkerArchive;
static hdUiSafeString sMedleyMarkerLayout;

/* 0266574C */
void hdDungeonMapMedley::destroy(u32 flags) {
    if (this == NULL) return;
    mStateHolder.mHolderVtable = &sMedleyMarkerOwnerVtable;
    mSecondaryVtable = &sMedleyMarkerSecondaryVtable;
    hdMenuMapPanePart::destroy(0);
    if (flags & 1) operator_delete(this);
}

/* 026657B8 */
hdDungeonMapMedley* hdDungeonMapMedley::construct(hdDungeonMapMedley* self) {
    if (self == NULL) {
        self = static_cast<hdDungeonMapMedley*>(operator_new(sizeof(hdDungeonMapMedley)));
        if (self == NULL) return NULL;
    }
    hdMenuMapPanePart::construct(self);
    self->mVtable = &sMedleyMarkerVtable;
    self->mStateHolder.mHolderVtable = &sMedleyMarkerOwnerVtable;
    self->mSecondaryVtable = &sMedleyMarkerSecondaryVtable;
    return self;
}

/* 02665824 */
u32 hdDungeonMapMedley::loadMarker(u32 heap, u32 resource, void* context) {
    attachArchives( &sMedleyMarkerArchive, 1);
    if (!vLoad(&sMedleyMarkerLayout, gabi::ea(mpResAccessor.get()), heap, resource, context))
        return 0;
    hdUiSafeString pane;
    pane.mVtable = reinterpret_cast<hdUiStringTable*>(&sMedleyMarkerStringVtable);
    pane.mText.v = gabi::guest_addr("N_Rotate_00");
    reinterpret_cast<hdSeaChartStateHandlers_c*>(this)->bindRootAndPane( &pane);
    return 1;
}

static hdDungeonMarkerStatics sMedleyMarkerStatics;
static hdDungeonMarkerRegistration sMedleyStorageRegistration;
static hdDungeonMarkerRegistration sMedleyFirstFlagRegistration;
static hdDungeonMarkerRegistration sMedleySecondFlagRegistration;
static f32 sMedleyMinimum;
static f32 sMedleyMaximum;
static f32 sMedleyOrigin;
static f32 sMedleyOffset;
void markerRegisterStatic(hdDungeonMarkerRegistration*);
void markerConstructFirstFlag(be<u8>*);
void markerConstructSecondFlag(be<u8>*);
/* 02665900 */
static void initializeMedleyMarkerStatics() {
    sMedleyMarkerStatics.mStorage[3] = 0;
    sMedleyMarkerStatics.mStorage[2] = 0;
    sMedleyMarkerStatics.mStorage[1] = 0;
    sMedleyMarkerStatics.mStorage[0] = 0;
    markerRegisterStatic(&sMedleyStorageRegistration);
    const f32 minimum = sMedleyMinimum;
    const f32 maximum = sMedleyMaximum;
    sMedleyMarkerStatics.mMinimum = minimum;
    sMedleyMarkerStatics.mMaximum = maximum;
    markerConstructFirstFlag(&sMedleyMarkerStatics.mFirstFlag);
    markerRegisterStatic(&sMedleyFirstFlagRegistration);
    markerConstructSecondFlag(&sMedleyMarkerStatics.mSecondFlag);
    markerRegisterStatic(&sMedleySecondFlagRegistration);
    const f32 offset = sMedleyOffset;
    sMedleyMarkerStatics.mOffset.x = offset;
    const f32 origin = sMedleyOrigin;
    sMedleyMarkerArchive.mVtable = reinterpret_cast<hdUiStringTable*>(&sMedleyMarkerStringVtable);
    sMedleyMarkerStatics.mOffset.y = offset;
    sMedleyMarkerArchive.mText.v = gabi::guest_addr("DungeonMapMedley_00");
    sMedleyMarkerStatics.mOrigin.y = origin;
    sMedleyMarkerStatics.mOrigin.x = origin;
    sMedleyMarkerLayout.mVtable = reinterpret_cast<hdUiStringTable*>(&sMedleyMarkerStringVtable);
    sMedleyMarkerLayout.mText.v = gabi::guest_addr("DungeonMapMedley_00");
}

/* WWHD glue
 data sMedleyMarkerStatics 10493B3C
 data sMedleyStorageRegistration 101F5D70
 data sMedleyFirstFlagRegistration 101F5D7C
 data sMedleySecondFlagRegistration 101F5D88
 data sMedleyMinimum 100F3310
 data sMedleyMaximum 100F3314
 data sMedleyOrigin 100F3318
 data sMedleyOffset 100F331C
 str 100F3320 in 02665900
 data sMedleyMarkerVtable 100F3338
 data sMedleyMarkerOwnerVtable 100F3478
 data sMedleyMarkerSecondaryVtable 100F3488
 data sMedleyMarkerStringVtable 100F32E4
 data sMedleyMarkerArchive 10493B34
 data sMedleyMarkerLayout 10493B68
 str 100F3304 in 02665824

*/
