/* HD dungeon-map ship marker: layout loading and pane binding. */
#include "d/hd_dungeon_map_markers.h"
#include "d/hd_sea_chart_state_handlers.h"
static hdPartsVtbl_c sShipMarkerVtable;
static u8 sShipMarkerOwnerVtable;
static u8 sShipMarkerSecondaryVtable;
static u8 sShipMarkerStringVtable;
static hdUiSafeString sShipMarkerArchive;
static hdUiSafeString sShipMarkerLayout;

/* 02665A0C */
void hdDungeonMapShip::destroy(u32 flags) {
    if (this == NULL) return;
    mStateHolder.mHolderVtable = &sShipMarkerOwnerVtable;
    mSecondaryVtable = &sShipMarkerSecondaryVtable;
    hdMenuMapPanePart::destroy(0);
    if (flags & 1) operator_delete(this);
}

/* 02665A78 */
hdDungeonMapShip* hdDungeonMapShip::construct(hdDungeonMapShip* self) {
    if (self == NULL) {
        self = static_cast<hdDungeonMapShip*>(operator_new(sizeof(hdDungeonMapShip)));
        if (self == NULL) return NULL;
    }
    hdMenuMapPanePart::construct(self);
    self->mVtable = &sShipMarkerVtable;
    self->mStateHolder.mHolderVtable = &sShipMarkerOwnerVtable;
    self->mSecondaryVtable = &sShipMarkerSecondaryVtable;
    return self;
}

/* 02665AE4 */
u32 hdDungeonMapShip::loadMarker(u32 heap, u32 resource, void* context) {
    attachArchives( &sShipMarkerArchive, 1);
    if (!vLoad(&sShipMarkerLayout, gabi::ea(mpResAccessor.get()), heap, resource, context))
        return 0;
    hdUiSafeString pane;
    pane.mVtable = reinterpret_cast<hdUiStringTable*>(&sShipMarkerStringVtable);
    pane.mText.v = gabi::guest_addr("");
    reinterpret_cast<hdSeaChartStateHandlers_c*>(this)->bindPane( &pane);
    return 1;
}

static hdDungeonMarkerStatics sShipMarkerStatics;
static hdDungeonMarkerRegistration sShipStorageRegistration;
static hdDungeonMarkerRegistration sShipFirstFlagRegistration;
static hdDungeonMarkerRegistration sShipSecondFlagRegistration;
static f32 sShipMinimum;
static f32 sShipMaximum;
static f32 sShipOrigin;
static f32 sShipOffset;
void markerRegisterStatic(hdDungeonMarkerRegistration*);
void markerConstructFirstFlag(be<u8>*);
void markerConstructSecondFlag(be<u8>*);
/* 02665BC0 */
static void initializeShipMarkerStatics() {
    sShipMarkerStatics.mStorage[3] = 0;
    sShipMarkerStatics.mStorage[2] = 0;
    sShipMarkerStatics.mStorage[1] = 0;
    sShipMarkerStatics.mStorage[0] = 0;
    markerRegisterStatic(&sShipStorageRegistration);
    const f32 minimum = sShipMinimum;
    const f32 maximum = sShipMaximum;
    sShipMarkerStatics.mMinimum = minimum;
    sShipMarkerStatics.mMaximum = maximum;
    markerConstructFirstFlag(&sShipMarkerStatics.mFirstFlag);
    markerRegisterStatic(&sShipFirstFlagRegistration);
    markerConstructSecondFlag(&sShipMarkerStatics.mSecondFlag);
    markerRegisterStatic(&sShipSecondFlagRegistration);
    const f32 offset = sShipOffset;
    sShipMarkerStatics.mOffset.x = offset;
    const f32 origin = sShipOrigin;
    sShipMarkerArchive.mVtable = reinterpret_cast<hdUiStringTable*>(&sShipMarkerStringVtable);
    sShipMarkerStatics.mOffset.y = offset;
    sShipMarkerArchive.mText.v = gabi::guest_addr("DungeonMapShip_00");
    sShipMarkerStatics.mOrigin.y = origin;
    sShipMarkerStatics.mOrigin.x = origin;
    sShipMarkerLayout.mVtable = reinterpret_cast<hdUiStringTable*>(&sShipMarkerStringVtable);
    sShipMarkerLayout.mText.v = gabi::guest_addr("DungeonMapShip_00");
}

/* WWHD glue
 data sShipMarkerStatics 10493B78
 data sShipStorageRegistration 101F5D94
 data sShipFirstFlagRegistration 101F5DA0
 data sShipSecondFlagRegistration 101F5DAC
 data sShipMinimum 100F34D0
 data sShipMaximum 100F34D4
 data sShipOrigin 100F34D8
 data sShipOffset 100F34DC
 str 100F34E0 in 02665BC0
 data sShipMarkerVtable 100F34F8
 data sShipMarkerOwnerVtable 100F3638
 data sShipMarkerSecondaryVtable 100F3648
 data sShipMarkerStringVtable 100F34AC
 data sShipMarkerArchive 10493B70
 data sShipMarkerLayout 10493BA4
 str 100F34CC in 02665AE4

*/
