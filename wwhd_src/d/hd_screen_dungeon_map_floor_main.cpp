/* HD DungeonMapFloor_00: the main floor map part. */
#include "d/hd_dungeon_map_floor_variants.h"
#include "d/hd_parts_valid.h"
#include "d/hd_pane_util.h"
#include "d/hd_screen_map_display_coordinator.h"
void seaMapUpdateTile(SeaMapTile_l*);
static hdPartsVtbl_c sMainFloorVtable;
static u8 sMainFloorOwnerVtable;
static u8 sMainFloorSecondaryVtable;
static hdDungeonFloorArchiveNames sMainFloorArchives;
static hdUiSafeString sMainFloorLayout;
void prepareMainFloor(hdDungeonMapFloorMain*);
/* 02660EB4 */
void hdDungeonMapFloorMain::destroy(u32 flags) {
 if (!this) return;
 mStateHolder.mHolderVtable = &sMainFloorOwnerVtable;
 mSecondaryVtable = &sMainFloorSecondaryVtable;
 hdSeaChartMap_c::destroy(reinterpret_cast<hdSeaChartMap_c*>(this), 0);
 if (flags & 1) operator_delete(this);
}
/* 02660F20 */
hdDungeonMapFloorMain* hdDungeonMapFloorMain::construct(hdDungeonMapFloorMain* self) {
 if (!self) {
  self = static_cast<hdDungeonMapFloorMain*>(operator_new(sizeof(hdDungeonMapFloorMain)));
  if (!self) return NULL;
 }
 hdSeaChartMap_c::construct(reinterpret_cast<hdSeaChartMap_c*>(self));
 self->mStateHolder.mHolderVtable = &sMainFloorOwnerVtable;
 self->mVtable = &sMainFloorVtable;
 self->mSecondaryVtable = &sMainFloorSecondaryVtable;
 self->mDestinationActive = 0;
 return self;
}
/* 02661090 */
u32 hdDungeonMapFloorMain::loadFloor(u32 heap, u32 resource, void* context) {
 this->attachArchives(&sMainFloorArchives.mNames[0], 2);
 if (!vLoad(&sMainFloorLayout, gabi::ea(mpResAccessor.get()), heap, resource, context)) return 0;
 if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
 prepareMainFloor(this);
 return 1;
}
/* 0266116C */
void hdDungeonMapFloorMain::finalizeFloor() {
 vDeleteChildren();
 reinterpret_cast<hdSeaChartMap_c*>(this)->releaseLayoutAndCells();
 this->detachArchives();
}
/* 026611B4 */
void hdDungeonMapFloorMain::drawFloor(void* info) { vDrawLayout(info); }
static hdDungeonFloorNameTable sMainFloorCaptions;
static hdDungeonFloorRangeTable sMainFloorStartTable;
static hdDungeonFloorRangeTable sMainFloorEndTable;
static HdPadUiPointer sMainFloorUi;
static f32 sMainFloorUnitBlend;
void mainFloorShowPart(hdParts_c*);
void mainFloorHidePart(hdParts_c*);
f32 mainFloorGetBlend(hdParts_c*);
/* 02660F94 */
void prepareMainFloor(hdDungeonMapFloorMain* self) {
 self->mFloorCount = 7;
 self->mpFloorCaptions = &sMainFloorCaptions.mNames[0];
 auto* chart = reinterpret_cast<hdSeaChartMap_c*>(self);
 chart->setResourceMappings(reinterpret_cast<SeaMapResourceMapping_l*>(&sMainFloorStartTable.mValue),
                            reinterpret_cast<SeaMapMarkerMapping_l*>(&sMainFloorEndTable.mValue));
 chart->configureAnimations(0, 15, 3, 0, 31, 30, 1, 0, 16, 15);
 chart->initializeCellPanes();
 if (self->mpFloorChildren && self->mChildCount >= 7) {
  hdParts_c* child = u32(self->mChildCount) > 6 ? self->mpFloorChildren.get()[6].get() : NULL;
  hdDungeonFloorPoint3Words upper;
  hdPaneUtil_c::getGlobalTranslation(reinterpret_cast<cXyz*>(&upper), reinterpret_cast<PaneUtilPane_l*>(child));
  self->mUpperPosition.x = upper.x;
  self->mUpperPosition.y = upper.y;
  child = u32(self->mChildCount) > 5 ? self->mpFloorChildren.get()[5].get() : NULL;
  hdDungeonFloorPoint3Words lower;
  hdPaneUtil_c::getGlobalTranslation(reinterpret_cast<cXyz*>(&lower), reinterpret_cast<PaneUtilPane_l*>(child));
  self->mLowerPosition.x = lower.x;
  self->mLowerPosition.y = lower.y;
 }
}
/* 02661414 */
void hdDungeonMapFloorMain::showDestination() {
 if (mpFloorChildren && mChildCount >= 7) {
  hdParts_c* child = u32(mChildCount) > 4 ? mpFloorChildren.get()[4].get() : NULL;
  mainFloorShowPart(child);
 }
}
/* 02661440 */
void hdDungeonMapFloorMain::hideDestination() {
 if (mpFloorChildren && mChildCount >= 7) {
  hdParts_c* child = u32(mChildCount) > 4 ? mpFloorChildren.get()[4].get() : NULL;
  mainFloorHidePart(child);
 }
}
/* 0266146C */
void hdDungeonMapFloorMain::resetDestination() {
 hdDungeonFloorFlags_l* flags = mpFloorFlags;
 mDestinationActive = 0;
 if (flags->mEntered) showDestination();
 else hideDestination();
}
/* 0266148C */
u32 hdDungeonMapFloorMain::destinationFinished() {
 if (!mpFloorChildren || mChildCount < 7) return 0;
 hdParts_c* child = u32(mChildCount) > 4 ? mpFloorChildren.get()[4].get() : NULL;
 f32 blend = mainFloorGetBlend(child);
 return !(sMainFloorUnitBlend > blend);
}
/* 026614FC */
void hdDungeonMapFloorMain::updateDestination() {
 if (mpFloorFlags->mDestinationRequested) {
  mDestinationActive = 1;
  if (destinationFinished()) mDestinationActive = 0;
 } else if (mDestinationActive) {
  if (destinationFinished()) mDestinationActive = 0;
 }
}
/* 02661574 */
void hdDungeonMapFloorMain::updateButtons() {
 for (u32 index = 0; index<3; ++index) {
  hdParts_c* part = child(index);
  if (part) {
   u32 enabled = reinterpret_cast<hdMapDisplayCoordinator_c*>(sMainFloorUi.mOwner->mUi.get())->isSpecialChartUnlocked(index) ? 0 : 1;
   reinterpret_cast<hdPartsValid_c*>(part)->setValid(enabled);
  }
 }
}
/* 02661604 */
void hdDungeonMapFloorMain::clearUnavailableButtonFocus() {
 for (u32 index = 0; index<3; ++index) {
  hdParts_c* part = child(index);
  if (part && reinterpret_cast<hdMapDisplayCoordinator_c*>(sMainFloorUi.mOwner->mUi.get())->isSpecialChartUnlocked(index) && reinterpret_cast<hdPartsValid_c*>(part)->isValid())
   reinterpret_cast<hdPartsValid_c*>(part)->changeValid(0);
 }
}
/* 0266169C */
hdDungeonFloorCell_l* hdDungeonMapFloorMain::findFloorCell(u32 room) {
 hdDungeonFloorCell_l* cell = reinterpret_cast<hdDungeonFloorCell_l*>(
     reinterpret_cast<hdSeaChartMap_c*>(this)->findTile(room));
 if (cell || !mpFloorFlags) return cell;
 u32 index = mpFloorFlags->mCurrentFloorIndex;
 cell = &mFloorCells[index<10 ? index : 0];
 if (u32(cell->mIndex) != room) return NULL;
 return &mFloorCells[index<10 ? index : 0];
}
/* 02661858 */
void hdDungeonMapFloorMain::executeFloor() {
 if (mpFloorFlags) {
  if (mpFloorFlags->mNeedsUpdate) reinterpret_cast<hdSeaChartMap_c*>(this)->refreshMarkerAnimations();
  updateDestination();
  clearUnavailableButtonFocus();
 }
}
/* 026618B0 */
void hdDungeonMapFloorMain::resetFloor() {
 if (mpFloorFlags) {
  u32 index = mpFloorFlags->mCurrentFloorIndex;
  hdDungeonFloorCell_l* cell = &mFloorCells[index<10 ? index : 0];
  if (cell) seaMapUpdateTile(reinterpret_cast<SeaMapTile_l*>(cell));
 }
 reinterpret_cast<hdSeaChartMap_c*>(this)->updateTileTransforms();
}
static hdScreenUiSystem* sMainFloorUiSystem;
static hdDungeonFloorNameTable sMainFloorAnimationNames;
static hdDungeonFloorNameTable sMainFloorGroupNames;
static hdDungeonFloorNameTable sMainFloorChildNames;
void mainFloorBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
void mainFloorBindAnimations(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*, u32);
/* 026611C4 */
u32 hdDungeonMapFloorMain::createFloorChild(hdDungeonFloorDesc_l* desc) {
 for (u32 index = 0; index<3; ++index) {
  hdUiSafeString* name = &desc->mName;
  name->assureTermination();
  name->assureTermination();
  hdUiSafeString* expected = &sMainFloorChildNames.mNames[index];
  be<u8>* snapshot = name->mText;
  expected->assureTermination();
  bool equal = snapshot == expected->mText.get();
  if (!equal) {
   be<u8>* left = name->mText;
   be<u8>* right = expected->mText;
   for (u32 remaining = 0x40001; remaining; --remaining) {
    u8 a = *left++;
    u8 b = *right++;
    if (a != b) break;
    if (a == 0) { equal = true; break; }
   }
  }
  if (equal) return createChild(index, reinterpret_cast<hdPartsDesc_c*>(desc));
 }
 return 1;
}
/* 026612CC */
u32 hdDungeonMapFloorMain::loadLayout(void* name, u32 accessor, u32 heap, u32 resource, void* context) {
 auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sMainFloorUiSystem->mHeap, 4));
 if (layout) layout = hdPartsLayout_c::construct(layout);
 mpLayout = layout;
 if (!layout || !layout->vLoad(name, accessor, heap, resource, 32, 31, context)) return 0;
 mainFloorBindAnimation(mpLayout->animSet(), 0, &sMainFloorAnimationNames.mNames[0], &sMainFloorGroupNames.mNames[30]);
 mainFloorBindAnimations(mpLayout->animSet(), 1, &sMainFloorAnimationNames.mNames[1], &sMainFloorGroupNames.mNames[0], 15);
 mainFloorBindAnimations(mpLayout->animSet(), 16, &sMainFloorAnimationNames.mNames[16], &sMainFloorGroupNames.mNames[15], 15);
 mainFloorBindAnimation(mpLayout->animSet(), 31, &sMainFloorAnimationNames.mNames[31], &sMainFloorGroupNames.mNames[30]);
 vCalcLayout();
 return 1;
}
static hdDungeonFloorRegistration sFloorMainFloorStorageRegistration;
static f32 sFloorMainFloorMinimum;
static f32 sFloorMainFloorMaximum;
static hdDungeonFloorRegistration sFloorMainFloorFirstFlagRegistration;
static hdDungeonFloorRegistration sFloorMainFloorSecondFlagRegistration;
static f32 sFloorMainFloorOffset;
static f32 sFloorMainFloorOrigin;
static hdDungeonFloorNameTable sMainFloorChildClasses;
static hdDungeonFloorStatics sFloorMainFloorStatics;
static u8 sFloorMainFloorStringVtable;
void floorRegisterStatic(hdDungeonFloorRegistration*);
void floorConstructFirstFlag(be<u8>*);
void floorConstructSecondFlag(be<u8>*);
/* 026620F4 */
static void initializeMainFloorStatics() {
 sFloorMainFloorStatics.mStorage[3] = 0;
 sFloorMainFloorStatics.mStorage[2] = 0;
 sFloorMainFloorStatics.mStorage[1] = 0;
 sFloorMainFloorStatics.mStorage[0] = 0;
 floorRegisterStatic(&sFloorMainFloorStorageRegistration);
 const f32 minimum = sFloorMainFloorMinimum;
 const f32 maximum = sFloorMainFloorMaximum;
 sFloorMainFloorStatics.mMinimum = minimum;
 sFloorMainFloorStatics.mMaximum = maximum;
 floorConstructFirstFlag(&sFloorMainFloorStatics.mFirstFlag);
 floorRegisterStatic(&sFloorMainFloorFirstFlagRegistration);
 floorConstructSecondFlag(&sFloorMainFloorStatics.mSecondFlag);
 floorRegisterStatic(&sFloorMainFloorSecondFlagRegistration);
 const f32 offset = sFloorMainFloorOffset;
 const f32 origin = sFloorMainFloorOrigin;
 sFloorMainFloorStatics.mOrigin.x = origin;
 sFloorMainFloorStatics.mOrigin.y = origin;
 sFloorMainFloorStatics.mOffset.y = offset;
 sMainFloorCaptions.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorCaptions.mNames[0].mText.v = gabi::guest_addr("N_All_00");
 sMainFloorCaptions.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorCaptions.mNames[1].mText.v = gabi::guest_addr("N_1F_00");
 sMainFloorCaptions.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorCaptions.mNames[2].mText.v = gabi::guest_addr("N_2F_00");
 sMainFloorCaptions.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorCaptions.mNames[3].mText.v = gabi::guest_addr("N_Outdoor_00");
 sMainFloorCaptions.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorCaptions.mNames[4].mText.v = gabi::guest_addr("P_Destination_00");
 sMainFloorCaptions.mNames[5].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorCaptions.mNames[5].mText.v = gabi::guest_addr("N_OutdoorCenter_00");
 sMainFloorCaptions.mNames[6].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sFloorMainFloorStatics.mOffset.x = offset;
 sMainFloorGroupNames.mNames[0].mText.v = gabi::guest_addr("G_Room_00");
 sMainFloorGroupNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[1].mText.v = gabi::guest_addr("G_Room_01");
 sMainFloorGroupNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[2].mText.v = gabi::guest_addr("G_Room_02");
 sMainFloorGroupNames.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[3].mText.v = gabi::guest_addr("G_Room_03");
 sMainFloorGroupNames.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[4].mText.v = gabi::guest_addr("G_Room_04");
 sMainFloorGroupNames.mNames[5].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[5].mText.v = gabi::guest_addr("G_Room_05");
 sMainFloorGroupNames.mNames[6].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[6].mText.v = gabi::guest_addr("G_Room_06");
 sMainFloorCaptions.mNames[6].mText.v = gabi::guest_addr("N_FloorCenter_00");
 sMainFloorGroupNames.mNames[7].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[7].mText.v = gabi::guest_addr("G_Room_07");
 sMainFloorGroupNames.mNames[8].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[8].mText.v = gabi::guest_addr("G_Room_08");
 sMainFloorGroupNames.mNames[9].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[9].mText.v = gabi::guest_addr("G_Room_09");
 sMainFloorGroupNames.mNames[10].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[10].mText.v = gabi::guest_addr("G_Room_10");
 sMainFloorGroupNames.mNames[11].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[11].mText.v = gabi::guest_addr("G_Room_11");
 sMainFloorGroupNames.mNames[12].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[12].mText.v = gabi::guest_addr("G_Room_12");
 sMainFloorGroupNames.mNames[13].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[13].mText.v = gabi::guest_addr("G_Room_13");
 sMainFloorGroupNames.mNames[14].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[14].mText.v = gabi::guest_addr("G_Room_14");
 sMainFloorGroupNames.mNames[15].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[15].mText.v = gabi::guest_addr("G_Room_00");
 sMainFloorGroupNames.mNames[16].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[16].mText.v = gabi::guest_addr("G_Room_01");
 sMainFloorGroupNames.mNames[17].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[17].mText.v = gabi::guest_addr("G_Room_02");
 sMainFloorGroupNames.mNames[18].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[18].mText.v = gabi::guest_addr("G_Room_03");
 sMainFloorGroupNames.mNames[19].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[19].mText.v = gabi::guest_addr("G_Room_04");
 sMainFloorGroupNames.mNames[20].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[20].mText.v = gabi::guest_addr("G_Room_05");
 sMainFloorGroupNames.mNames[21].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[21].mText.v = gabi::guest_addr("G_Room_06");
 sMainFloorGroupNames.mNames[22].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[22].mText.v = gabi::guest_addr("G_Room_07");
 sMainFloorGroupNames.mNames[23].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[23].mText.v = gabi::guest_addr("G_Room_08");
 sMainFloorGroupNames.mNames[24].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[24].mText.v = gabi::guest_addr("G_Room_09");
 sMainFloorGroupNames.mNames[25].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[25].mText.v = gabi::guest_addr("G_Room_10");
 sMainFloorGroupNames.mNames[26].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[26].mText.v = gabi::guest_addr("G_Room_11");
 sMainFloorGroupNames.mNames[27].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[27].mText.v = gabi::guest_addr("G_Room_12");
 sMainFloorGroupNames.mNames[28].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[28].mText.v = gabi::guest_addr("G_Room_13");
 sMainFloorGroupNames.mNames[29].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[29].mText.v = gabi::guest_addr("G_Room_14");
 sMainFloorGroupNames.mNames[30].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorGroupNames.mNames[30].mText.v = gabi::guest_addr("G_All_00");
 sMainFloorAnimationNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[0].mText.v = gabi::guest_addr("InAll");
 sMainFloorAnimationNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[1].mText.v = gabi::guest_addr("InRoom");
 sMainFloorAnimationNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[2].mText.v = gabi::guest_addr("InRoom");
 sMainFloorAnimationNames.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[3].mText.v = gabi::guest_addr("InRoom");
 sMainFloorAnimationNames.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[4].mText.v = gabi::guest_addr("InRoom");
 sMainFloorAnimationNames.mNames[5].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[5].mText.v = gabi::guest_addr("InRoom");
 sMainFloorAnimationNames.mNames[6].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[6].mText.v = gabi::guest_addr("InRoom");
 sMainFloorAnimationNames.mNames[7].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[7].mText.v = gabi::guest_addr("InRoom");
 sMainFloorAnimationNames.mNames[8].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[8].mText.v = gabi::guest_addr("InRoom");
 sMainFloorAnimationNames.mNames[9].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[9].mText.v = gabi::guest_addr("InRoom");
 sMainFloorAnimationNames.mNames[10].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[10].mText.v = gabi::guest_addr("InRoom");
 sMainFloorAnimationNames.mNames[11].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[11].mText.v = gabi::guest_addr("InRoom");
 sMainFloorAnimationNames.mNames[12].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[12].mText.v = gabi::guest_addr("InRoom");
 sMainFloorAnimationNames.mNames[13].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[13].mText.v = gabi::guest_addr("InRoom");
 sMainFloorAnimationNames.mNames[14].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[14].mText.v = gabi::guest_addr("InRoom");
 sMainFloorAnimationNames.mNames[15].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[15].mText.v = gabi::guest_addr("InRoom");
 sMainFloorChildNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorChildNames.mNames[0].mText.v = gabi::guest_addr("L_Searchlight_00");
 sMainFloorChildNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorChildNames.mNames[1].mText.v = gabi::guest_addr("L_Searchlight_01");
 sMainFloorAnimationNames.mNames[16].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[16].mText.v = gabi::guest_addr("ColorRoom");
 sMainFloorAnimationNames.mNames[17].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[17].mText.v = gabi::guest_addr("ColorRoom");
 sMainFloorAnimationNames.mNames[18].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[18].mText.v = gabi::guest_addr("ColorRoom");
 sMainFloorAnimationNames.mNames[19].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[19].mText.v = gabi::guest_addr("ColorRoom");
 sMainFloorAnimationNames.mNames[20].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[20].mText.v = gabi::guest_addr("ColorRoom");
 sMainFloorAnimationNames.mNames[21].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[21].mText.v = gabi::guest_addr("ColorRoom");
 sMainFloorAnimationNames.mNames[22].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[22].mText.v = gabi::guest_addr("ColorRoom");
 sMainFloorAnimationNames.mNames[23].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[23].mText.v = gabi::guest_addr("ColorRoom");
 sMainFloorAnimationNames.mNames[24].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[24].mText.v = gabi::guest_addr("ColorRoom");
 sMainFloorAnimationNames.mNames[25].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[25].mText.v = gabi::guest_addr("ColorRoom");
 sMainFloorAnimationNames.mNames[26].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[26].mText.v = gabi::guest_addr("ColorRoom");
 sMainFloorAnimationNames.mNames[27].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[27].mText.v = gabi::guest_addr("ColorRoom");
 sMainFloorAnimationNames.mNames[28].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[28].mText.v = gabi::guest_addr("ColorRoom");
 sMainFloorAnimationNames.mNames[29].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[29].mText.v = gabi::guest_addr("ColorRoom");
 sMainFloorAnimationNames.mNames[30].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorChildNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorAnimationNames.mNames[30].mText.v = gabi::guest_addr("ColorRoom");
 sMainFloorAnimationNames.mNames[31].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorChildClasses.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorChildClasses.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorChildClasses.mNames[0].mText.v = gabi::guest_addr("Searchlight_00");
 sMainFloorChildClasses.mNames[2].mText.v = gabi::guest_addr("Searchlight_00");
 sMainFloorChildClasses.mNames[1].mText.v = gabi::guest_addr("Searchlight_00");
 sMainFloorLayout.mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorArchives.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorArchives.mNames[0].mText.v = gabi::guest_addr("DungeonMapFloor_00");
 sMainFloorAnimationNames.mNames[31].mText.v = gabi::guest_addr("OutAll");
 sMainFloorChildNames.mNames[2].mText.v = gabi::guest_addr("L_Searchlight_02");
 sMainFloorArchives.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
 sMainFloorArchives.mNames[1].mText.v = gabi::guest_addr("Searchlight_00");
 sMainFloorLayout.mText.v = gabi::guest_addr("DungeonMapFloor_00");
 sMainFloorChildClasses.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sFloorMainFloorStringVtable);
}
static hdDungeonFloorAnimationRemap sMainFloorAnimationRemap;
void mainFloorResetAnimationRange(hdAnimSet*, u32, u32);
/* 02661734 */
s32 hdDungeonMapFloorMain::currentRoom() {
 enum { ROOM_MAIN = 10, ROOM_UPPER = 0x85 };
 s32 room = reinterpret_cast<hdSeaChartMap_c*>(this)->findLargestTile();
 if (room == ROOM_MAIN) {
  auto* table = reinterpret_cast<hdDungeonFloorVtable*>(mVtable.get());
  hdDungeonFloorCell_l* cell = gabi::call_ptr<hdDungeonFloorCell_l*>(table->mFindCell.get(), this, ROOM_UPPER);
  if (cell && mOriginX < cell->mScale) room = ROOM_UPPER;
 }
 return room;
}
/* 026617AC */
void hdDungeonMapFloorMain::resetAnimations() {
 if (!mpFloorFlags) return;
 reinterpret_cast<hdSeaChartMap_c*>(this)->updateMarkerAnimations();
 for (u32 index = 0; index<5; ++index) {
  u32 offset = sMainFloorAnimationRemap.mIds[index];
  u32 leaveEnd = u32(mLeaveEnd) + offset;
  u32 enterEnd = u32(mEnterEnd) + offset;
  u32 leaveStart = u32(mLeaveStart) + offset;
  u32 enterStart = u32(mEnterStart) + offset;
  mainFloorResetAnimationRange(mpLayout->animSet(), leaveStart, leaveEnd);
  mainFloorResetAnimationRange(mpLayout->animSet(), enterStart, enterEnd);
 }
 resetDestination();
 updateButtons();
}

void seaMapInitializeTilePane(SeaMapTile_l*);
HdPadPoint* seaMapTileSize(SeaMapTile_l*);
static f32 sMainFloorHalf;
static f32 sMainFloorSpacing;
static f32 sMainFloorZero;
struct hdDungeonFloorChildRange { be<u32> mChild, mRoom; };
struct hdDungeonFloorChildRanges { hdDungeonFloorChildRange mRanges[16]; };
/* 02661910 */
void hdDungeonMapFloorMain::prepareUpperCell() {
 reinterpret_cast<hdSeaChartMap_c*>(this)->bindTilePanes();
 hdDungeonFloorVtable* table = static_cast<hdDungeonFloorVtable*>(mVtable.get());
 u32 index = mpFloorFlags->mCurrentFloorIndex;
 void* room = gabi::call_ptr<void*>(table->mFindRoom.get(), this, 0x85);
 if (!room) return;
 hdDungeonFloorCell_l* cell = &mFloorCells[index<10?index:0];
 cell->mpPane = room;
 cell->mIndex = 0x85;
 seaMapInitializeTilePane(reinterpret_cast<SeaMapTile_l*>(cell));
 cell->mBaseScale = mOriginX;
 cell->mpOwner = reinterpret_cast<hdSeaChartMap_c*>(this);
 if (mpCellContext) cell->mpTextPane = reinterpret_cast<hdTextPane_c*>(mpCellContext.get());
 if (reinterpret_cast<hdSeaChartMap_c*>(this)->selectedCell()) {
  cell->mpParent = &mReferenceCell;
  HdPadPoint* size = seaMapTileSize(reinterpret_cast<SeaMapTile_l*>(&mReferenceCell));
  cell->mSize.x = size->x;
  cell->mSize.y = size->y;
 }
}
/* 02661ACC */
void hdDungeonMapFloorMain::arrangeCurrentCell() {
 reinterpret_cast<hdSeaChartMap_c*>(this)->arrangeTilePositions();
 u32 index = mpFloorFlags->mCurrentFloorIndex;
 u32 previous = index-1;
 hdDungeonFloorCell_l* before = &mFloorCells[previous<10?previous:0];
 hdDungeonFloorCell_l* current = &mFloorCells[index<10?index:0];
 f32 half = sMainFloorHalf;
 f32 previousHeight = seaMapTileSize(reinterpret_cast<SeaMapTile_l*>(before))->y;
 f32 height = seaMapTileSize(reinterpret_cast<SeaMapTile_l*>(current))->y;
 f32 spacing = gabi::fmuls_ppc(gabi::fmadds(previousHeight, half, gabi::fmuls_ppc(height, half)), sMainFloorSpacing);
 current->mPosition.x = before->mPosition.x;
 current->mPosition.y = f32(before->mPosition.y)+spacing;
}
/* 02661C4C */
hdParts_c* hdDungeonMapFloorMain::findFloorChild(u32 room) {
 hdParts_c* child = static_cast<hdParts_c*>(reinterpret_cast<hdSeaChartMap_c*>(this)->findResourceCell(room));
 if (child || !mpFloorFlags) return child;
 hdDungeonFloorChildRange* range = &reinterpret_cast<hdDungeonFloorChildRanges*>(&sMainFloorStartTable)->mRanges[u32(mpFloorFlags->mCurrentFloorIndex)];
 if (u32(range->mRoom) != room) return NULL;
 u32 index = range->mChild;
 return index<u32(mChildCount)?mpFloorChildren.get()[index].get():NULL;
}
/* 02661CD8 */
void hdDungeonMapFloorMain::arrangeFloorOffsets() {
 hdDungeonFloorVtable* table = static_cast<hdDungeonFloorVtable*>(mVtable.get());
 u32 first = mpFloorFlags->mFirstRoom;
 s32 count = mpFloorFlags->mCurrentFloorIndex;
 s32 room = gabi::call_ptr<s32>(table->mCurrentRoom.get(), this);
 if (room == 10) {
  hdDungeonFloorCell_l* upper = gabi::call_ptr<hdDungeonFloorCell_l*>(static_cast<hdDungeonFloorVtable*>(mVtable.get())->mFindCell.get(), this, 0x85);
  if (upper && f32(mOriginX)<f32(upper->mScale)) room = 0x85;
 }
 f32 zero = sMainFloorZero;
 f32 halfHeight = zero, center = zero;
 if (room != 10) {
  hdDungeonFloorCell_l* selected = gabi::call_ptr<hdDungeonFloorCell_l*>(static_cast<hdDungeonFloorVtable*>(mVtable.get())->mFindCell.get(), this, room);
  if (selected) {
   f32 height = seaMapTileSize(reinterpret_cast<SeaMapTile_l*>(selected))->y;
   halfHeight = (height-f32(selected->mSize.y))*sMainFloorHalf;
   center = selected->mCurrent.y;
  }
  if (room == 0x85) {
   if (count <= 0) return;
   center -= halfHeight;
   for(s32 i = 0; i<count; ++i) {
    hdDungeonFloorCell_l* cell = gabi::call_ptr<hdDungeonFloorCell_l*>(static_cast<hdDungeonFloorVtable*>(mVtable.get())->mFindCell.get(), this, u8(first+i));
    if(cell) {cell->mCurrent.y = center; cell->mCurrent.x = zero; }
   }
   return;
  }
  f32 lower = center-halfHeight, upper = center+halfHeight;
  for(s32 i = 0; i<count; ++i) {
   u32 number = u8(first+i);
   hdDungeonFloorCell_l* cell = gabi::call_ptr<hdDungeonFloorCell_l*>(static_cast<hdDungeonFloorVtable*>(mVtable.get())->mFindCell.get(), this, number);
   if(cell && number<u32(room)) {cell->mCurrent.y = lower; cell->mCurrent.x = zero; }
   else if(cell && number>u32(room)) {cell->mCurrent.x = zero; cell->mCurrent.y = upper; }
  }
  hdDungeonFloorCell_l* cell = gabi::call_ptr<hdDungeonFloorCell_l*>(static_cast<hdDungeonFloorVtable*>(mVtable.get())->mFindCell.get(), this, 0x85);
  if(cell) {cell->mCurrent.x = zero; cell->mCurrent.y = center+halfHeight; }
  return;
 }
 for(s32 i = 0; i<count; ++i) {
  hdDungeonFloorCell_l* cell = gabi::call_ptr<hdDungeonFloorCell_l*>(static_cast<hdDungeonFloorVtable*>(mVtable.get())->mFindCell.get(), this, u8(first++));
  if(cell) {cell->mCurrent.y = zero; cell->mCurrent.x = zero; }
 }
 hdDungeonFloorCell_l* cell = gabi::call_ptr<hdDungeonFloorCell_l*>(static_cast<hdDungeonFloorVtable*>(mVtable.get())->mFindCell.get(), this, 0x85);
 if(cell) {cell->mCurrent.y = zero; cell->mCurrent.x = zero; }
}
/* 02662028 */
void hdDungeonMapFloorMain::clearUpperTarget() {
 reinterpret_cast<hdSeaChartMap_c*>(this)->resetTileOffsets();
 hdDungeonFloorCell_l* cell = gabi::call_ptr<hdDungeonFloorCell_l*>(static_cast<hdDungeonFloorVtable*>(mVtable.get())->mFindCell.get(), this, 0x85);
 if(cell) {f32 zero = sMainFloorZero; cell->mOffset.y = zero; cell->mOffset.x = zero; }
}
/* 02662084 */
void hdDungeonMapFloorMain::resetUpperCell() {
 reinterpret_cast<hdSeaChartMap_c*>(this)->prepareScrollTiles();
 if(u8(mSelectedRoom) == 10) return;
 hdDungeonFloorCell_l* cell = gabi::call_ptr<hdDungeonFloorCell_l*>(static_cast<hdDungeonFloorVtable*>(mVtable.get())->mFindCell.get(), this, 0x85);
 if(cell) {cell->mScale = mOriginX; f32 zero = sMainFloorZero; cell->mCurrent.y = zero; cell->mCurrent.x = zero; }
}
/* WWHD glue
 ext 026F8CD0 void hdParts_c::calcLayout()
 ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString*,u32)
 ext 026F91F8 void hdParts_c::detachArchives()
 ext 026F92C0 u32 hdParts_c::createChild(u32,hdPartsDesc_c*)
 ext 027076B0 u8 hdPartsValid_c::isValid() const
 ext 0270762C void hdPartsValid_c::changeValid(u32)
 ext 02707640 void hdPartsValid_c::setValid(u32)
 ext 0267E6F8 void hdSeaChartMap_c::bindTilePanes()
 ext 0267E934 void hdSeaChartMap_c::arrangeTilePositions()
 ext 0267E498 void hdSeaChartMap_c::resetTileOffsets()
 ext 0267E56C void hdSeaChartMap_c::prepareScrollTiles()
 ext 0267BA9C void* hdSeaChartMap_c::selectedCell()
 ext 0267F824 void seaMapInitializeTilePane(SeaMapTile_l*)
 ext 0267F7F4 HdPadPoint* seaMapTileSize(SeaMapTile_l*)
 ext 0267EB68 void* hdSeaChartMap_c::findResourceCell(u32)
 data sMainFloorHalf 100F112C
 data sMainFloorSpacing 100F1130
 data sMainFloorZero 100F1134
 data sMainFloorAnimationRemap 100F109C
 ext 0267D698 u32 hdSeaChartMap_c::findLargestTile()
 ext 0267D7E4 void hdSeaChartMap_c::updateMarkerAnimations()
 ext 02005708 void mainFloorResetAnimationRange(hdAnimSet*,u32,u32)
data sFloorMainFloorStorageRegistration 101F5C50
data sFloorMainFloorMinimum 100F1138
data sFloorMainFloorMaximum 100F113C
data sFloorMainFloorFirstFlagRegistration 101F5C5C
data sFloorMainFloorSecondFlagRegistration 101F5C68
data sFloorMainFloorOffset 100F1144
data sFloorMainFloorOrigin 100F1140
data sFloorMainFloorStatics 104927F4
data sFloorMainFloorStringVtable 100F106C
data sMainFloorChildClasses 10492870
 str 100F1148 "N_1F_00" in 026620F4
 str 100F1150 "N_2F_00" in 026620F4
 str 100F1158 "InAll" in 026620F4
 str 100F1160 "InRoom" in 026620F4
 str 100F1168 "OutAll" in 026620F4
 str 100F1170 "G_Room_00" in 026620F4
 str 100F117C "G_Room_01" in 026620F4
 str 100F1188 "G_Room_02" in 026620F4
 str 100F1194 "G_Room_03" in 026620F4
 str 100F11A0 "G_Room_04" in 026620F4
 str 100F11AC "G_Room_05" in 026620F4
 str 100F11B8 "G_Room_06" in 026620F4
 str 100F11C4 "G_Room_07" in 026620F4
 str 100F11D0 "G_Room_08" in 026620F4
 str 100F11DC "G_Room_09" in 026620F4
 str 100F11E8 "G_Room_10" in 026620F4
 str 100F11F4 "G_Room_11" in 026620F4
 str 100F1200 "G_Room_12" in 026620F4
 str 100F120C "G_Room_13" in 026620F4
 str 100F1218 "G_Room_14" in 026620F4
 str 100F1224 "ColorRoom" in 026620F4
 str 100F1230 "N_All_00" in 026620F4
 str 100F123C "N_Outdoor_00" in 026620F4
 str 100F124C "P_Destination_00" in 026620F4
 str 100F1260 "N_OutdoorCenter_00" in 026620F4
 str 100F1274 "N_FloorCenter_00" in 026620F4
 str 100F1288 "G_All_00" in 026620F4
 str 100F1294 "L_Searchlight_00" in 026620F4
 str 100F12A8 "L_Searchlight_01" in 026620F4
 str 100F12BC "L_Searchlight_02" in 026620F4
 str 100F12D0 "Searchlight_00" in 026620F4
 str 100F12E0 "DungeonMapFloor_00" in 026620F4
 data sMainFloorUiSystem 1018C404 nocheck
 data sMainFloorAnimationNames 10492990
 data sMainFloorGroupNames 10492898
 data sMainFloorChildNames 10492858
 ext 02004E04 void mainFloorBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
 ext 0200506C void mainFloorBindAnimations(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*,u32)
 ext 0267D614 SeaMapTile_l* hdSeaChartMap_c::findTile(u32)
 ext 0267DA4C void hdSeaChartMap_c::refreshMarkerAnimations()
 ext 0267F6B8 void seaMapUpdateTile(SeaMapTile_l*)
 ext 0267D4D0 void hdSeaChartMap_c::updateTileTransforms()
 data sMainFloorCaptions 10492820
 data sMainFloorStartTable 100F1084
 data sMainFloorEndTable 100F10B0
 data sMainFloorUi 101F8344
 data sMainFloorUnitBlend 100F1128
 ext 02704DE4 void hdPaneUtil_c::getGlobalTranslation(cXyz*,PaneUtilPane_l*)
 ext 026898F0 void mainFloorShowPart(hdParts_c*)
 ext 02689914 void mainFloorHidePart(hdParts_c*)
 ext 026899D4 f32 mainFloorGetBlend(hdParts_c*)
 ext 0268A098 u32 hdMapDisplayCoordinator_c::isSpecialChartUnlocked(u32)
 data sMainFloorVtable 100F12F8
 data sMainFloorOwnerVtable 100F1528
 data sMainFloorSecondaryVtable 100F1538
 data sMainFloorArchives 10492888
 data sMainFloorLayout 10492A90
*/
