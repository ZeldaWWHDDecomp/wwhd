/* HD dungeon-map child factory callbacks. */
#include "d/hd_dungeon_map_boss_parts.h"
#include "d/hd_screen_dungeon_map_up.h"
#include "d/hd_screen_dungeon_map_treasure.h"
#include "d/hd_screen_dungeon_map_statue.h"
#include "d/hd_dungeon_map_markers.h"
#include "d/hd_dungeon_map_floor_variants.h"
#include "d/hd_dungeon_map_door.h"
#include "d/hd_dungeon_map_down.h"
static hdScreenUiSystem* sDungeonUiSystem;
/* 0265ED4C */
void hdDungeonMapBoss::createMainFloor(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(1516, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapFloorMain::construct(static_cast<hdDungeonMapFloorMain*>(part));
    *output = part;
    static_cast<hdDungeonMapFloorMain*>(part)->loadFloor(resource, context, NULL);
}

/* 0265F544 */
void hdDungeonMapBoss::deleteMainFloor(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

/* 0265EDC4 */
void hdDungeonMapBoss::createFloorFirst(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(1512, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapFloorFirst::construct(static_cast<hdDungeonMapFloorFirst*>(part));
    *output = part;
    static_cast<hdDungeonMapFloorFirst*>(part)->loadFloor(resource, context, NULL);
}

/* 0265F59C */
void hdDungeonMapBoss::deleteFloorFirst(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

/* 0265EE3C */
void hdDungeonMapBoss::createFloorSecond(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(1512, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapFloorSecond::construct(static_cast<hdDungeonMapFloorSecond*>(part));
    *output = part;
    static_cast<hdDungeonMapFloorSecond*>(part)->loadFloor(resource, context, NULL);
}

/* 0265F5F4 */
void hdDungeonMapBoss::deleteFloorSecond(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

/* 0265EEB4 */
void hdDungeonMapBoss::createFloorThird(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(1512, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapFloorThird::construct(static_cast<hdDungeonMapFloorThird*>(part));
    *output = part;
    static_cast<hdDungeonMapFloorThird*>(part)->loadFloor(resource, context, NULL);
}

/* 0265F64C */
void hdDungeonMapBoss::deleteFloorThird(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

/* 0265EF2C */
void hdDungeonMapBoss::createFloorFourth(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(1512, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapFloorFourth::construct(static_cast<hdDungeonMapFloorFourth*>(part));
    *output = part;
    static_cast<hdDungeonMapFloorFourth*>(part)->loadFloor(resource, context, NULL);
}

/* 0265F6A4 */
void hdDungeonMapBoss::deleteFloorFourth(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

/* 0265EFA4 */
void hdDungeonMapBoss::createFloorFifth(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(1512, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapFloorFifth::construct(static_cast<hdDungeonMapFloorFifth*>(part));
    *output = part;
    static_cast<hdDungeonMapFloorFifth*>(part)->loadFloor(resource, context, NULL);
}

/* 0265F6FC */
void hdDungeonMapBoss::deleteFloorFifth(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

/* 0265F01C */
void hdDungeonMapBoss::createBoss(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(160, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapBoss::construct(static_cast<hdDungeonMapBoss*>(part));
    *output = part;
    auto* table = reinterpret_cast<hdDungeonChildVtable*>(part->mVtable.get());
    gabi::call_ptr(table->mLoadChild.get(), part, resource, context, 0);
}

/* 0265F754 */
void hdDungeonMapBoss::deleteBoss(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

/* 0265F0A0 */
void hdDungeonMapBoss::createLink(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(164, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapLink::construct(static_cast<hdDungeonMapLink*>(part));
    *output = part;
    auto* table = reinterpret_cast<hdDungeonChildVtable*>(part->mVtable.get());
    gabi::call_ptr(table->mLoadChild.get(), part, resource, context, 0);
}

/* 0265F7AC */
void hdDungeonMapBoss::deleteLink(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

/* 0265F124 */
void hdDungeonMapBoss::createMedley(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(164, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapMedley::construct(static_cast<hdDungeonMapMedley*>(part));
    *output = part;
    auto* table = reinterpret_cast<hdDungeonChildVtable*>(part->mVtable.get());
    gabi::call_ptr(table->mLoadChild.get(), part, resource, context, 0);
}

/* 0265F804 */
void hdDungeonMapBoss::deleteMedley(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

/* 0265F1A8 */
void hdDungeonMapBoss::createMakore(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(164, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapMakore::construct(static_cast<hdDungeonMapMakore*>(part));
    *output = part;
    auto* table = reinterpret_cast<hdDungeonChildVtable*>(part->mVtable.get());
    gabi::call_ptr(table->mLoadChild.get(), part, resource, context, 0);
}

/* 0265F85C */
void hdDungeonMapBoss::deleteMakore(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

/* 0265F22C */
void hdDungeonMapBoss::createShip(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(164, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapShip::construct(static_cast<hdDungeonMapShip*>(part));
    *output = part;
    auto* table = reinterpret_cast<hdDungeonChildVtable*>(part->mVtable.get());
    gabi::call_ptr(table->mLoadChild.get(), part, resource, context, 0);
}

/* 0265F8B4 */
void hdDungeonMapBoss::deleteShip(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

/* 0265F2B0 */
void hdDungeonMapBoss::createTreasure(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(168, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapStatue_c::construct(static_cast<hdDungeonMapStatue_c*>(part));
    *output = part;
    auto* table = reinterpret_cast<hdDungeonChildVtable*>(part->mVtable.get());
    gabi::call_ptr(table->mLoadChild.get(), part, resource, context, 0);
}

/* 0265F90C */
void hdDungeonMapBoss::deleteTreasure(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

/* 0265F334 */
void hdDungeonMapBoss::createDoor(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(164, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapDoor::construct(static_cast<hdDungeonMapDoor*>(part));
    *output = part;
    auto* table = reinterpret_cast<hdDungeonChildVtable*>(part->mVtable.get());
    gabi::call_ptr(table->mLoadChild.get(), part, resource, context, 0);
}

/* 0265F964 */
void hdDungeonMapBoss::deleteDoor(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

/* 0265F3B8 */
void hdDungeonMapBoss::createFloor(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(164, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapTreasure_c::construct(static_cast<hdDungeonMapTreasure_c*>(part));
    *output = part;
    auto* table = reinterpret_cast<hdDungeonChildVtable*>(part->mVtable.get());
    gabi::call_ptr(table->mLoadChild.get(), part, resource, context, 0);
}

/* 0265F9BC */
void hdDungeonMapBoss::deleteFloor(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

/* 0265F43C */
void hdDungeonMapBoss::createUp(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(176, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapUp_c::construct(static_cast<hdDungeonMapUp_c*>(part));
    *output = part;
    auto* table = reinterpret_cast<hdDungeonChildVtable*>(part->mVtable.get());
    gabi::call_ptr(table->mLoadChild.get(), part, resource, context, 0);
}

/* 0265FA14 */
void hdDungeonMapBoss::deleteUp(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

/* 0265F4C0 */
void hdDungeonMapBoss::createDown(gptr<hdParts_c>* output, u32 resource, u32 context) {
    hdParts_c* part = static_cast<hdParts_c*>(hdParts_new(176, sDungeonUiSystem->mHeap, 4));
    if (part) part = hdDungeonMapDown::construct(static_cast<hdDungeonMapDown*>(part));
    *output = part;
    auto* table = reinterpret_cast<hdDungeonChildVtable*>(part->mVtable.get());
    gabi::call_ptr(table->mLoadChild.get(), part, resource, context, 0);
}

/* 0265FA6C */
void hdDungeonMapBoss::deleteDown(hdParts_c* part) {
    if (part) {
        part->vFinalize();
        part->vDelete(3);
    }
}

static hdPartsVtbl_c sBossVtable;
static u8 sBossOwnerVtable;
static u8 sBossSecondaryVtable;
static u8 sBossStringVtable;
static hdUiSafeString sBossArchive;
static hdUiSafeString sBossLayout;
void bossConstructTransition(hdSeaChartStateHandlers_c*);
void bossDestroyTransition(hdSeaChartStateHandlers_c*, u32);
/* 0265EC4C */
static u32 dungeonBossStateKey(be<u32>* key) { return *key; }
/* 0265FAC4 */
void hdDungeonMapBoss::destroy(u32 flags) {
 if (!this) return;
 mStateHolder.mHolderVtable = &sBossOwnerVtable;
 mSecondaryVtable = &sBossSecondaryVtable;
 bossDestroyTransition(this, 0);
 if (flags & 1) operator_delete(this);
}
/* 0265FB30 */
hdDungeonMapBoss* hdDungeonMapBoss::construct(hdDungeonMapBoss* self) {
 if (!self) {
  self = static_cast<hdDungeonMapBoss*>(operator_new(sizeof(hdDungeonMapBoss)));
  if (!self) return NULL;
 }
 bossConstructTransition(self);
 self->mVtable = &sBossVtable;
 self->mStateHolder.mHolderVtable = &sBossOwnerVtable;
 self->mSecondaryVtable = &sBossSecondaryVtable;
 return self;
}
/* 0265FB9C */
u32 hdDungeonMapBoss::loadBoss(u32 heap, u32 resource, void* context) {
 attachArchives(&sBossArchive, 1);
 if (!vLoad(&sBossLayout, gabi::ea(mpResAccessor.get()), heap, resource, context)) return 0;
 hdUiSafeString pane;
 pane.mVtable = reinterpret_cast<hdUiStringTable*>(&sBossStringVtable);
 pane.mText.v = gabi::guest_addr("");
 bindPane(&pane);
 return 1;
}
/* 0265FC78 */
void hdDungeonMapBoss::finalizeBoss() {
 vDestroyLayout();
 detachArchives();
}
/* 0265FDB0 */
void hdDungeonMapBoss::drawBoss(void* info) { vDrawLayout(info); }

struct hdDungeonBossPositions { HdPadPoint mPositions[6]; };
static hdDungeonMarkerStatics sBossStatics;
static hdDungeonMarkerRegistration sBossStorageRegistration;
static hdDungeonMarkerRegistration sBossFirstFlagRegistration;
static hdDungeonMarkerRegistration sBossSecondFlagRegistration;
static f32 sBossMinimum;
static f32 sBossMaximum;
static f32 sBossOrigin;
static f32 sBossOffset;
static f32 sBossPositiveSpacing;
static f32 sBossNegativeSpacing;
void bossRegisterStatic(hdDungeonMarkerRegistration*);
void bossConstructFirstFlag(be<u8>*);
void bossConstructSecondFlag(be<u8>*);
/* 0265FDC0 */
static void initializeBossStatics() {
 sBossStatics.mStorage[3] = 0;
 sBossStatics.mStorage[2] = 0;
 sBossStatics.mStorage[1] = 0;
 sBossStatics.mStorage[0] = 0;
 bossRegisterStatic(&sBossStorageRegistration);
 f32 minimum = sBossMinimum, maximum = sBossMaximum;
 sBossStatics.mMinimum = minimum;
 sBossStatics.mMaximum = maximum;
 bossConstructFirstFlag(&sBossStatics.mFirstFlag);
 bossRegisterStatic(&sBossFirstFlagRegistration);
 bossConstructSecondFlag(&sBossStatics.mSecondFlag);
 bossRegisterStatic(&sBossSecondFlagRegistration);
 f32 zero = l_hdBossZero, offset = sBossOffset, origin = sBossOrigin;
 hdDungeonBossPositions* positions = reinterpret_cast<hdDungeonBossPositions*>(&l_bossPositions);
 positions->mPositions[0].x = zero;
 sBossStatics.mOrigin.y = origin;
 positions->mPositions[0].y = zero;
 f32 positive = sBossPositiveSpacing;
 positions->mPositions[1].x = zero;
 positions->mPositions[1].y = positive;
 positions->mPositions[2].x = positive;
 sBossStatics.mOffset.y = offset;
 positions->mPositions[2].y = zero;
 positions->mPositions[3].x = positive;
 f32 negative = sBossNegativeSpacing;
 sBossStatics.mOrigin.x = origin;
 positions->mPositions[3].y = zero;
 positions->mPositions[4].x = negative;
 positions->mPositions[4].y = zero;
 positions->mPositions[5].x = zero;
 positions->mPositions[5].y = negative;
 sBossLayout.mVtable = reinterpret_cast<hdUiStringTable*>(&sBossStringVtable);
 sBossLayout.mText.v = gabi::guest_addr("DungeonMapBoss_00");
 sBossArchive.mVtable = reinterpret_cast<hdUiStringTable*>(&sBossStringVtable);
 sBossArchive.mText.v = gabi::guest_addr("DungeonMapBoss_00");
 sBossStatics.mOffset.x = offset;
}
/* WWHD glue
 ext 026893C8 void hdPadRotatePoint(HdPadPoint*,HdPadPoint*,HdPadPoint*,f32)
 ext 02006364 void hdPadStateExecute(HdPadStateController*)
 data sBossStatics 104926C0
 data sBossStorageRegistration 101F5BE4
 data sBossFirstFlagRegistration 101F5BF0
 data sBossSecondFlagRegistration 101F5BFC
 data sBossMinimum 100F0A8C
 data sBossMaximum 100F0A90
 data sBossOrigin 100F0A94
 data sBossOffset 100F0A98
 data sBossPositiveSpacing 100F0A9C
 data sBossNegativeSpacing 100F0AA0
 str 100F0AA4 "DungeonMapBoss_00" in 0265FDC0
 ext 028F026C void bossRegisterStatic(hdDungeonMarkerRegistration*)
 ext 028ED6F8 void bossConstructFirstFlag(be<u8>*)
 ext 028EAB2C void bossConstructSecondFlag(be<u8>*)
 data sBossVtable 100F0AB8
 data sBossOwnerVtable 100F0BF8
 data sBossSecondaryVtable 100F0C08
 data sBossStringVtable 100F0A64
 data sBossArchive 104926B8
 data sBossLayout 1049271C
 str 100F0A84 in 0265FB9C
 ext 02685134 void bossConstructTransition(hdSeaChartStateHandlers_c*)
 ext 02684E94 void bossDestroyTransition(hdSeaChartStateHandlers_c*,u32)

data sDungeonUiSystem 1018C404 nocheck
ext 02665D64 hdDungeonMapStatue_c* hdDungeonMapStatue_c::construct(hdDungeonMapStatue_c*)
ext 02666428 hdDungeonMapTreasure_c* hdDungeonMapTreasure_c::construct(hdDungeonMapTreasure_c*)
ext 02666878 hdDungeonMapUp_c* hdDungeonMapUp_c::construct(hdDungeonMapUp_c*)
*/
