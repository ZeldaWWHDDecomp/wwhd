/* HD dungeon-map state handlers. The merged view supplies the object layout. */
#pragma once
#include "d/hd_gamepad_map.h"
#include "d/hd_parts.h"
#include "d/hd_cursor_mgr.h"
#include "f_op/f_op_actor.h"
#include "d/hd_parts_button.h"
#include "d/hd_parts_text_pane.h"
#include "d/hd_screen_map_display_coordinator.h"
#include "d/hd_screen_dungeon_map.h"


void hdDungeonPrepareOpen(HdPadDungeon*);
void hdDungeonPrepareWait(HdPadDungeon*);
void hdMapBeginZoomOut(HdPadMap*);
void hdMapBeginZoomIn(HdPadMap*);
void hdMapBeginZoomToPlayer(HdPadMap*);




void hdDungeonBounds(HdPadDungeon*, HdPadRect*);

void hdDungeonRefreshFloorSelection(HdPadDungeon*);

u32 hdDungeonMapCanZoom(HdPadMap*);

void hdDungeonButtonSetReceiver(hdButtonPart_c*, u32, hdButtonReceiver_l*);
void hdDungeonReceiverSetButton(hdButtonReceiver_l*, u32, hdButtonReceiver_l*);

u32 hdDungeonFloorLinksEnabled(hdDungeonMap_c*, u32);
u32 hdDungeonFloorChildIndex(hdDungeonMap_c*, u32);
u32 hdDungeonSecondaryFloorVisible(HdPadUi*);
void hdDungeonRefreshRootNavigation(hdDungeonMap_c*);
struct hdDungeonFloorOrder_l { be<u32> mChildren[5]; };

void hdDungeonMarkerBeginIn(hdParts_c*);
void hdDungeonMarkerBeginOut(hdParts_c*);
void hdDungeonMarkerResetDisplay(hdParts_c*);

void hdDungeonUpdateParts(HdPadDungeon*);
void hdDungeonRefreshFloorVisibility(HdPadDungeon*);

void hdDungeonStopAnimation(hdAnimSet*, u32, u32);

/* Canonical pane search table retains the original 0x60-byte view. */
WWHD_SIZE(hdPartsPaneSRTVtbl_c, 0x60);
struct hdDungeonFrameNames_l { hdScreenString mNames[4]; };
struct hdDungeonFloorNames_l { hdScreenString mNames[8]; };
struct hdDungeonFloorLabelNames_l { hdScreenString mNames[5]; };

void hdDungeonReleaseFirstMap(hdDungeonMap_c*, HdPadMap*);
void hdDungeonReleaseSecondMap(hdDungeonMap_c*, HdPadMap*);
void hdDungeonReleaseThirdMap(hdDungeonMap_c*, HdPadMap*);
void hdDungeonReleaseFourthMap(hdDungeonMap_c*, HdPadMap*);
void hdDungeonReleaseFifthMap(hdDungeonMap_c*, HdPadMap*);
void hdDungeonReleaseSixthMap(hdDungeonMap_c*, HdPadMap*);
void hdDungeonReleasePlayerMarker(hdDungeonMap_c*, hdParts_c*);
void hdDungeonReleaseLinkedMarker(hdDungeonMap_c*, hdParts_c*);
void hdDungeonReleaseBossMarker(hdDungeonMap_c*, hdParts_c*);
void hdDungeonReleaseSpecialMarker(hdDungeonMap_c*, hdParts_c*);
void hdDungeonReleasePartnerMarker(hdDungeonMap_c*, hdParts_c*);
void hdDungeonReleaseItemMarker(hdDungeonMap_c*, hdParts_c*);
void hdDungeonReleaseDoorMarker(hdDungeonMap_c*, hdParts_c*);
void hdDungeonReleaseTreasureMarker(hdDungeonMap_c*, hdParts_c*);
void hdDungeonReleaseEnemyMarker(hdDungeonMap_c*, hdParts_c*);
void hdDungeonReleaseSpecialItemMarker(hdDungeonMap_c*, hdParts_c*);

struct hdDungeonReceiverFlags_l {
    u8 mPadding[0x58];
    be<u8> mEnabled;
    u8 mPaddingToSecondary;
    be<u8> mSecondaryEnabled;
};
WWHD_OFFSET(hdDungeonReceiverFlags_l, mEnabled, 0x58);
WWHD_OFFSET(hdDungeonReceiverFlags_l, mSecondaryEnabled, 0x5A);
struct hdDungeonTextCapacities_l { be<u32> mCapacity[5]; };
void* hdDungeonAllocateReceiver(u32, hdScreenHeap*, u32);
hdButtonReceiver_l* hdDungeonConstructReceiver(void*, hdPartsPane_c*, u32);

void hdDungeonSelectUiMode(HdPadUi*);
void hdDungeonRefreshSelection(HdPadDungeon*);

f32 hdDungeonReadMapScale(HdPadDungeon*);
void hdDungeonReadMapPosition(HdPadDungeon*, HdPadPoint*);

struct hdDungeonMapPaneLink_l {
    u8 mPadding[0x4B0];
    gptr<HdPadPane> mpPane;
};
WWHD_OFFSET(hdDungeonMapPaneLink_l, mpPane, 0x4B0);
u32 hdDungeonHasMapDrag(HdPadDungeon*);
void hdDungeonUpdateDragTarget(HdPadDungeon*);
void hdDungeonApplyDragTarget(HdPadDungeon*);
void hdDungeonMapSetDelta(HdPadMap*,HdPadPoint*);
void hdDungeonMapSetAngle(HdPadMap*,f32);

hdPartsStateId_c* hdDungeonCurrentMapState(HdPadStateController*);


struct hdDungeonMapLinks_l {
    u8 mPadding[0x4B0];
    gptr<HdPadPane> mpPane;
    gptr<be<f32>> mpMapScale;
    gptr<void> mpFloorContext;
};
WWHD_OFFSET(hdDungeonMapLinks_l, mpPane, 0x4B0);
WWHD_OFFSET(hdDungeonMapLinks_l, mpMapScale, 0x4B4);
WWHD_OFFSET(hdDungeonMapLinks_l, mpFloorContext, 0x4B8);
void hdDungeonRefreshMapContext(HdPadDungeon*);

struct hdDungeonFloorLabelTexts_l { hdScreenString mNames[7]; };
struct hdDungeonFloorButton_l {
    u8 mPadding[0x78];
    be<u8> mFloor;
};
WWHD_OFFSET(hdDungeonFloorButton_l, mFloor, 0x78);
void hdDungeonApplyFloorLabel(HdPadDungeon*,hdPartsPane_c*,hdScreenString*);

struct hdDungeonMapArchiveNames_l { hdUiSafeString mNames[3]; };

struct hdDungeonPositionUi_l {
    u8 mPadding[0x50];
    be<u32> mMode;
    u8 mPaddingToFreeze[5];
    be<u8> mFreezePosition;
};
WWHD_OFFSET(hdDungeonPositionUi_l, mFreezePosition, 0x59);
struct hdDungeonPositionSave_l {
    u8 mPadding[0x1DB];
    be<u8> mResumeMode;
    u8 mPaddingToAngle[0xF72];
    be<s16> mAngle;
    HdPadPoint3 mPosition;
};
WWHD_OFFSET(hdDungeonPositionSave_l, mAngle, 0x114E);
WWHD_OFFSET(hdDungeonPositionSave_l, mPosition, 0x1150);
struct hdDungeonPositionPlay_l {
    u8 mPadding[0x5B3C];
    gptr<fopAc_ac_c> mpPartner;
};
WWHD_OFFSET(hdDungeonPositionPlay_l, mpPartner, 0x5B3C);
fopAc_ac_c* hdDungeonFindPlayer(HdPadDungeon*);
hdDungeonPositionPlay_l* hdDungeonPositionPlay();
fopAc_ac_c* hdDungeonFindItemActor(u32);

struct hdDungeonItemButtonOrder_l { be<u32> mChildren[3]; };
void hdDungeonPrepareItemButton(hdParts_c*,u32);
void hdDungeonActivateItemButton(hdParts_c*);
void hdDungeonDeactivateItemButton(hdParts_c*);

void hdDungeonMarkerSetMap(hdParts_c*,HdPadMap*);

void hdDungeonRefreshCaption(HdPadDungeon*);
hdDungeonMap_c* hdDungeonSelectMap(hdDungeonMap_c*);
void hdDungeonUpdateMapDimensions(HdPadDungeon*);
void hdDungeonInitializeZoom(HdPadDungeon*);
void hdDungeonUpdateSpecialMap(HdPadDungeon*);
void hdDungeonPopulateMapMarkers(HdPadDungeon*);
void hdDungeonRefreshMapItems(HdPadDungeon*);
hdDungeonMap_c* hdDungeonApplyZoomMode(hdDungeonMap_c*);
void hdDungeonUpdateAllMarkers(HdPadDungeon*);

struct hdDungeonAnimationNames_l { hdScreenString mNames[7]; };
struct hdDungeonAnimationGroups_l { hdScreenString mNames[6]; };
struct hdDungeonAnimationGroupOrder_l { be<u32> mIndices[7]; };
void hdDungeonBindAnimation(hdAnimSet*,u32,hdScreenString*,hdScreenString*);

struct hdDungeonDrawInfo_l {
    u8 mPadding[8];
    gptr<void> mpContext;
};
WWHD_OFFSET(hdDungeonDrawInfo_l, mpContext, 8);
void hdDungeonSaveClip(HdPadRect*,void*);
void hdDungeonApplyClip(HdPadRect*,void*);
u32 hdDungeonDrawPrimaryLast(HdPadDungeon*);

struct hdDungeonChildNames_l { hdScreenString mNames[8]; };
struct hdDungeonChildDescriptor_l {
    u8 mPadding[8];
    hdScreenString mResourceName;
    u8 mPaddingToPane[4];
    hdScreenString mPaneName;
    u8 mPaddingToClass[8];
    gptr<const char> mClassName;
};
WWHD_OFFSET(hdDungeonChildDescriptor_l, mResourceName, 8);
WWHD_OFFSET(hdDungeonChildDescriptor_l, mPaneName, 0x14);
WWHD_OFFSET(hdDungeonChildDescriptor_l, mClassName, 0x24);
hdParts_c* hdDungeonConstructItemButton(hdParts_c*);
hdParts_c* hdDungeonConstructFloorButton(hdParts_c*);
u32 hdDungeonInitializeItemButton(hdParts_c*,u32,u32,hdPartsDesc_c*);
u32 hdDungeonInitializeFloorButton(hdParts_c*,u32,u32,hdPartsDesc_c*);

struct hdDungeonMarkerRole_l {
    u8 mPadding[0xA0];
    be<u32> mRole;
};
WWHD_OFFSET(hdDungeonMarkerRole_l, mRole, 0xA0);
enum hdDungeonMarkerRole {
    DUNGEON_MARKER_LINKED = 0,
    DUNGEON_MARKER_SPECIAL = 1,
    DUNGEON_MARKER_BOSS = 2,
    DUNGEON_MARKER_PARTNER = 3,
    DUNGEON_MARKER_ITEM = 4,
};
void hdDungeonCreateFirstMap(HdPadDungeon*,gptr<HdPadMap>*,void*,void*);
void hdDungeonCreateSecondMap(HdPadDungeon*,gptr<HdPadMap>*,void*,void*);
void hdDungeonCreateThirdMap(HdPadDungeon*,gptr<HdPadMap>*,void*,void*);
void hdDungeonCreateFourthMap(HdPadDungeon*,gptr<HdPadMap>*,void*,void*);
void hdDungeonCreateFifthMap(HdPadDungeon*,gptr<HdPadMap>*,void*,void*);
void hdDungeonCreateSixthMap(HdPadDungeon*,gptr<HdPadMap>*,void*,void*);
void hdDungeonCreatePlayerMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*);
void hdDungeonCreateLinkedMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*);
void hdDungeonCreateBossMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*);
void hdDungeonCreateSpecialMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*);
void hdDungeonCreatePartnerMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*);
void hdDungeonCreateItemMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*);
void hdDungeonCreateDoorMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*);
void hdDungeonCreateTreasureMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*);
void hdDungeonCreateEnemyMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*);
void hdDungeonCreateSpecialItemMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*);

void* hdDungeonNew(u32);
void hdDungeonConstructRect(HdPadRect*);
void hdDungeonConstructFloorContext(void*);

struct hdDungeonInitializerWords_l { be<u32> mWords[4]; };
struct hdDungeonGlobalRegistration_l { be<u32> mWords[3]; };
struct hdDungeonInitializerConstants_l {
    be<f32> mAngleMin, mAngleMax, mHalfExtent, mPlayerLimit;
    HdPadPoint mMapCoordinates[17];
    HdPadPoint mExtraCoordinates[4];
};
struct hdDungeonInitializerLimits_l {
    be<f32> mAngleMin, mAngleMax, mHalfWidth, mHalfHeight, mPlayerLimitX, mPlayerLimitY;
};
struct hdDungeonInitializedCoordinate_l { HdPadPoint mPosition; u8 mPadding[8]; };
struct hdDungeonInitializedCoordinates_l { hdDungeonInitializedCoordinate_l mEntries[21]; };
struct hdDungeonCaptionNames_l { hdScreenString mNames[6]; };
struct hdDungeonResourceNames_l { hdScreenString mNames[8]; };
struct hdDungeonStateInitializers_l { hdPartsStateId_c mStates[14]; };
void hdDungeonRegisterGlobal(void*);
void hdDungeonInitializeAngle(void*);
void hdDungeonInitializeLimits(void*);

WWHD_OFFSET(hdDungeonInitializerConstants_l, mAngleMin, 0x00);
WWHD_OFFSET(hdDungeonInitializerConstants_l, mAngleMax, 0x04);
WWHD_OFFSET(hdDungeonInitializerConstants_l, mHalfExtent, 0x08);
WWHD_OFFSET(hdDungeonInitializerConstants_l, mPlayerLimit, 0x0C);
WWHD_OFFSET(hdDungeonInitializerConstants_l, mMapCoordinates, 0x10);
WWHD_OFFSET(hdDungeonInitializerConstants_l, mExtraCoordinates, 0x98);
WWHD_SIZE(hdDungeonInitializerConstants_l, 0xB8);
WWHD_OFFSET(hdDungeonInitializerLimits_l, mHalfWidth, 0x08);
WWHD_OFFSET(hdDungeonInitializerLimits_l, mHalfHeight, 0x0C);
WWHD_OFFSET(hdDungeonInitializerLimits_l, mPlayerLimitX, 0x10);
WWHD_OFFSET(hdDungeonInitializerLimits_l, mPlayerLimitY, 0x14);
WWHD_SIZE(hdDungeonInitializerLimits_l, 0x18);
WWHD_OFFSET(hdDungeonInitializedCoordinate_l, mPosition, 0x00);
WWHD_SIZE(hdDungeonInitializedCoordinate_l, 0x10);
WWHD_OFFSET(hdDungeonInitializedCoordinates_l, mEntries, 0x00);
WWHD_SIZE(hdDungeonInitializedCoordinates_l, 0x150);
WWHD_SIZE(hdDungeonStateInitializers_l, 0x2A0);

void hdDungeonPaneDragOrigin(HdPadPoint3*, HdPadPane*);
