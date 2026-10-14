/* HD dungeon-map link marker: layout loading and pane binding. */
#include "d/hd_dungeon_map_markers.h"
#include "d/hd_sea_chart_state_handlers.h"
static hdPartsVtbl_c sLinkMarkerVtable;
static u8 sLinkMarkerOwnerVtable;
static u8 sLinkMarkerSecondaryVtable;
static u8 sLinkMarkerStringVtable;
static hdUiSafeString sLinkMarkerArchive;
static hdUiSafeString sLinkMarkerLayout;

/* 026651CC */
void hdDungeonMapLink::destroy(u32 flags) {
    if (this == NULL) return;
    mStateHolder.mHolderVtable = &sLinkMarkerOwnerVtable;
    mSecondaryVtable = &sLinkMarkerSecondaryVtable;
    hdMenuMapPanePart::destroy(0);
    if (flags & 1) operator_delete(this);
}

/* 02665238 */
hdDungeonMapLink* hdDungeonMapLink::construct(hdDungeonMapLink* self) {
    if (self == NULL) {
        self = static_cast<hdDungeonMapLink*>(operator_new(sizeof(hdDungeonMapLink)));
        if (self == NULL) return NULL;
    }
    hdMenuMapPanePart::construct(self);
    self->mVtable = &sLinkMarkerVtable;
    self->mStateHolder.mHolderVtable = &sLinkMarkerOwnerVtable;
    self->mSecondaryVtable = &sLinkMarkerSecondaryVtable;
    return self;
}

/* 026652A4 */
u32 hdDungeonMapLink::loadMarker(u32 heap, u32 resource, void* context) {
    attachArchives( &sLinkMarkerArchive, 1);
    if (!vLoad(&sLinkMarkerLayout, gabi::ea(mpResAccessor.get()), heap, resource, context))
        return 0;
    hdUiSafeString pane;
    pane.mVtable = reinterpret_cast<hdUiStringTable*>(&sLinkMarkerStringVtable);
    pane.mText.v = gabi::guest_addr("N_Rotate_00");
    reinterpret_cast<hdSeaChartStateHandlers_c*>(this)->bindRootAndPane( &pane);
    return 1;
}

static hdDungeonMarkerStatics sLinkMarkerStatics;
static hdDungeonMarkerRegistration sLinkStorageRegistration;
static hdDungeonMarkerRegistration sLinkFirstFlagRegistration;
static hdDungeonMarkerRegistration sLinkSecondFlagRegistration;
static f32 sLinkMinimum;
static f32 sLinkMaximum;
static f32 sLinkOrigin;
static f32 sLinkOffset;
void markerRegisterStatic(hdDungeonMarkerRegistration*);
void markerConstructFirstFlag(be<u8>*);
void markerConstructSecondFlag(be<u8>*);
/* 02665380 */
static void initializeLinkMarkerStatics() {
    sLinkMarkerStatics.mStorage[3] = 0;
    sLinkMarkerStatics.mStorage[2] = 0;
    sLinkMarkerStatics.mStorage[1] = 0;
    sLinkMarkerStatics.mStorage[0] = 0;
    markerRegisterStatic(&sLinkStorageRegistration);
    const f32 minimum = sLinkMinimum;
    const f32 maximum = sLinkMaximum;
    sLinkMarkerStatics.mMinimum = minimum;
    sLinkMarkerStatics.mMaximum = maximum;
    markerConstructFirstFlag(&sLinkMarkerStatics.mFirstFlag);
    markerRegisterStatic(&sLinkFirstFlagRegistration);
    markerConstructSecondFlag(&sLinkMarkerStatics.mSecondFlag);
    markerRegisterStatic(&sLinkSecondFlagRegistration);
    const f32 offset = sLinkOffset;
    sLinkMarkerStatics.mOffset.x = offset;
    const f32 origin = sLinkOrigin;
    sLinkMarkerArchive.mVtable = reinterpret_cast<hdUiStringTable*>(&sLinkMarkerStringVtable);
    sLinkMarkerStatics.mOffset.y = offset;
    sLinkMarkerArchive.mText.v = gabi::guest_addr("DungeonMapLink_00");
    sLinkMarkerStatics.mOrigin.y = origin;
    sLinkMarkerStatics.mOrigin.x = origin;
    sLinkMarkerLayout.mVtable = reinterpret_cast<hdUiStringTable*>(&sLinkMarkerStringVtable);
    sLinkMarkerLayout.mText.v = gabi::guest_addr("DungeonMapLink_00");
}

/* WWHD glue
 data sLinkMarkerStatics 10493AC4
 data sLinkStorageRegistration 101F5D28
 data sLinkFirstFlagRegistration 101F5D34
 data sLinkSecondFlagRegistration 101F5D40
 data sLinkMinimum 100F2F80
 data sLinkMaximum 100F2F84
 data sLinkOrigin 100F2F88
 data sLinkOffset 100F2F8C
 str 100F2F90 in 02665380
 ext 028F026C void markerRegisterStatic(hdDungeonMarkerRegistration*)
 ext 028ED6F8 void markerConstructFirstFlag(be<u8>*)
 ext 028EAB2C void markerConstructSecondFlag(be<u8>*)
 data sLinkMarkerVtable 100F2FA8
 data sLinkMarkerOwnerVtable 100F30E8
 data sLinkMarkerSecondaryVtable 100F30F8
 data sLinkMarkerStringVtable 100F2F54
 data sLinkMarkerArchive 10493ABC
 data sLinkMarkerLayout 10493AF0
 str 100F2F74 in 026652A4
 ext 0268B728 void hdMenuMapPanePart::destroy(u32)
 ext 0268B7B0 hdMenuMapPanePart* hdMenuMapPanePart::construct(hdMenuMapPanePart*)

*/
