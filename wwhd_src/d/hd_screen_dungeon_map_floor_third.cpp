/* HD DungeonMapFloor_03: part lifetime and resource loading. */
#include "d/hd_dungeon_map_floor_variants.h"
static hdPartsVtbl_c sThirdFloorVtable;
static u8 sThirdFloorOwnerVtable;
static u8 sThirdFloorSecondaryVtable;
static hdUiSafeString sThirdFloorArchive;
static hdUiSafeString sThirdFloorLayout;
void prepareThirdFloor(hdDungeonMapFloor_l*);

/* 026637F8 */
void hdDungeonMapFloorThird::destroy(u32 flags) {
 if (this == NULL) return;
 mStateHolder.mHolderVtable = &sThirdFloorOwnerVtable;
 mSecondaryVtable = &sThirdFloorSecondaryVtable;
 hdSeaChartMap_c::destroy(reinterpret_cast<hdSeaChartMap_c*>(this), 0);
 if (flags & 1) operator_delete(this);
}
/* 02663864 */
hdDungeonMapFloorThird* hdDungeonMapFloorThird::construct(hdDungeonMapFloorThird* self) {
 if (!self) {
  self = static_cast<hdDungeonMapFloorThird*>(operator_new(sizeof(hdDungeonMapFloorThird)));
  if (!self) return NULL;
 }
 hdSeaChartMap_c::construct(reinterpret_cast<hdSeaChartMap_c*>(self));
 self->mVtable = &sThirdFloorVtable;
 self->mStateHolder.mHolderVtable = &sThirdFloorOwnerVtable;
 self->mSecondaryVtable = &sThirdFloorSecondaryVtable;
 return self;
}
/* 0266395C */
u32 hdDungeonMapFloorThird::loadFloor(u32 heap, u32 resource, void* context) {
 this->attachArchives(&sThirdFloorArchive, 1);
 if (!vLoad(&sThirdFloorLayout, gabi::ea(mpResAccessor.get()), heap, resource, context)) return 0;
 prepareThirdFloor(this);
 return 1;
}
/* 02663A1C */
void hdDungeonMapFloorThird::finalizeFloor() {
 reinterpret_cast<hdSeaChartMap_c*>(this)->releaseLayoutAndCells();
 this->detachArchives();
}
/* 02663A50 */
void hdDungeonMapFloorThird::drawFloor(void* info) {
 vDrawLayout(info);
}
static hdDungeonFloorNameTable sThirdFloorCaptions;
static hdDungeonFloorRangeTable sThirdFloorStartTable;
static hdDungeonFloorRangeTable sThirdFloorEndTable;
/* 026638D0 */
void prepareThirdFloor(hdDungeonMapFloor_l* self) {
 self->mFloorCount = 5;
 self->mpFloorCaptions = &sThirdFloorCaptions.mNames[0];
 auto* chart = reinterpret_cast<hdSeaChartMap_c*>(self);
 chart->setResourceMappings(reinterpret_cast<SeaMapResourceMapping_l*>(&sThirdFloorStartTable.mValue),
                            reinterpret_cast<SeaMapMarkerMapping_l*>(&sThirdFloorEndTable.mValue));
 chart->configureAnimations(0, 22, 4, 0, 45, 44, 1, 0, 23, 22);
 chart->initializeCellPanes();
}
static hdScreenUiSystem* sThirdFloorUiSystem;
static hdDungeonFloorNameTable sThirdFloorAnimationNames;
static hdDungeonFloorNameTable sThirdFloorGroupNames;
void floorBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
void floorBindAnimations(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*, u32);
/* 02663A60 */
u32 hdDungeonMapFloorThird::loadLayout(void* name, u32 accessor, u32 heap, u32 resource, void* context) {
 auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sThirdFloorUiSystem->mHeap, 4));
 if (layout) layout = hdPartsLayout_c::construct(layout);
 mpLayout = layout;
 if (!layout || !layout->vLoad(name, accessor, heap, resource, 46, 45, context)) return 0;
 floorBindAnimation(mpLayout->animSet(), 0, &sThirdFloorAnimationNames.mNames[0], &sThirdFloorGroupNames.mNames[44]);
 floorBindAnimations(mpLayout->animSet(), 1, &sThirdFloorAnimationNames.mNames[1], &sThirdFloorGroupNames.mNames[0], 22);
 floorBindAnimations(mpLayout->animSet(), 23, &sThirdFloorAnimationNames.mNames[23], &sThirdFloorGroupNames.mNames[22], 22);
 floorBindAnimation(mpLayout->animSet(), 45, &sThirdFloorAnimationNames.mNames[45], &sThirdFloorGroupNames.mNames[44]);
 vCalcLayout();
 return 1;
}
static hdDungeonFloorRegistration sThirdFloorStorageRegistration;
static f32 sThirdFloorMinimum;
static f32 sThirdFloorMaximum;
static hdDungeonFloorRegistration sThirdFloorFirstFlagRegistration;
static hdDungeonFloorRegistration sThirdFloorSecondFlagRegistration;
static f32 sThirdFloorOffset;
static f32 sThirdFloorOrigin;
static hdDungeonFloorStatics sThirdFloorStatics;
static u8 sThirdFloorStringVtable;
void floorRegisterStatic(hdDungeonFloorRegistration*);
void floorConstructFirstFlag(be<u8>*);
void floorConstructSecondFlag(be<u8>*);
/* 02663BA8 */
static void initializeThirdFloorStatics() {
 sThirdFloorStatics.mStorage[3] = 0;
 sThirdFloorStatics.mStorage[2] = 0;
 sThirdFloorStatics.mStorage[1] = 0;
 sThirdFloorStatics.mStorage[0] = 0;
 floorRegisterStatic(&sThirdFloorStorageRegistration);
 const f32 minimum = sThirdFloorMinimum;
 const f32 maximum = sThirdFloorMaximum;
 sThirdFloorStatics.mMinimum = minimum;
 sThirdFloorStatics.mMaximum = maximum;
 floorConstructFirstFlag(&sThirdFloorStatics.mFirstFlag);
 floorRegisterStatic(&sThirdFloorFirstFlagRegistration);
 floorConstructSecondFlag(&sThirdFloorStatics.mSecondFlag);
 floorRegisterStatic(&sThirdFloorSecondFlagRegistration);
 const f32 offset = sThirdFloorOffset;
 const f32 origin = sThirdFloorOrigin;
 sThirdFloorStatics.mOrigin.y = origin;
 sThirdFloorStatics.mOrigin.x = origin;
 sThirdFloorCaptions.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorCaptions.mNames[0].mText.v = gabi::guest_addr("N_All_00");
 sThirdFloorCaptions.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorCaptions.mNames[1].mText.v = gabi::guest_addr("N_1F_00");
 sThirdFloorGroupNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorCaptions.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorStatics.mOffset.y = offset;
 sThirdFloorGroupNames.mNames[0].mText.v = gabi::guest_addr("G_Room_00");
 sThirdFloorGroupNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[1].mText.v = gabi::guest_addr("G_Room_01");
 sThirdFloorGroupNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[2].mText.v = gabi::guest_addr("G_Room_02");
 sThirdFloorGroupNames.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorStatics.mOffset.x = offset;
 sThirdFloorCaptions.mNames[2].mText.v = gabi::guest_addr("N_2F_00");
 sThirdFloorCaptions.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[3].mText.v = gabi::guest_addr("G_Room_03");
 sThirdFloorCaptions.mNames[3].mText.v = gabi::guest_addr("N_3F_00");
 sThirdFloorCaptions.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[4].mText.v = gabi::guest_addr("G_Room_04");
 sThirdFloorCaptions.mNames[4].mText.v = gabi::guest_addr("N_4F_00");
 sThirdFloorGroupNames.mNames[5].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[5].mText.v = gabi::guest_addr("G_Room_05");
 sThirdFloorGroupNames.mNames[6].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[6].mText.v = gabi::guest_addr("G_Room_06");
 sThirdFloorGroupNames.mNames[7].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[7].mText.v = gabi::guest_addr("G_Room_07");
 sThirdFloorGroupNames.mNames[8].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[8].mText.v = gabi::guest_addr("G_Room_08");
 sThirdFloorGroupNames.mNames[9].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[9].mText.v = gabi::guest_addr("G_Room_09");
 sThirdFloorGroupNames.mNames[10].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[10].mText.v = gabi::guest_addr("G_Room_10");
 sThirdFloorGroupNames.mNames[11].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[11].mText.v = gabi::guest_addr("G_Room_11");
 sThirdFloorGroupNames.mNames[12].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[12].mText.v = gabi::guest_addr("G_Room_12");
 sThirdFloorGroupNames.mNames[13].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[13].mText.v = gabi::guest_addr("G_Room_13");
 sThirdFloorGroupNames.mNames[14].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[14].mText.v = gabi::guest_addr("G_Room_14");
 sThirdFloorGroupNames.mNames[15].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[15].mText.v = gabi::guest_addr("G_Room_15");
 sThirdFloorGroupNames.mNames[16].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[16].mText.v = gabi::guest_addr("G_Room_16");
 sThirdFloorGroupNames.mNames[17].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[17].mText.v = gabi::guest_addr("G_Room_17");
 sThirdFloorGroupNames.mNames[18].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[18].mText.v = gabi::guest_addr("G_Room_18");
 sThirdFloorGroupNames.mNames[19].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[19].mText.v = gabi::guest_addr("G_Room_19");
 sThirdFloorGroupNames.mNames[20].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[20].mText.v = gabi::guest_addr("G_Room_20");
 sThirdFloorGroupNames.mNames[21].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[21].mText.v = gabi::guest_addr("G_Room_21");
 sThirdFloorGroupNames.mNames[22].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[22].mText.v = gabi::guest_addr("G_Room_00");
 sThirdFloorGroupNames.mNames[23].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[23].mText.v = gabi::guest_addr("G_Room_01");
 sThirdFloorGroupNames.mNames[24].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[24].mText.v = gabi::guest_addr("G_Room_02");
 sThirdFloorGroupNames.mNames[25].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[25].mText.v = gabi::guest_addr("G_Room_03");
 sThirdFloorGroupNames.mNames[26].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[26].mText.v = gabi::guest_addr("G_Room_04");
 sThirdFloorGroupNames.mNames[27].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[27].mText.v = gabi::guest_addr("G_Room_05");
 sThirdFloorGroupNames.mNames[28].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[28].mText.v = gabi::guest_addr("G_Room_06");
 sThirdFloorGroupNames.mNames[29].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[29].mText.v = gabi::guest_addr("G_Room_07");
 sThirdFloorGroupNames.mNames[30].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[30].mText.v = gabi::guest_addr("G_Room_08");
 sThirdFloorGroupNames.mNames[31].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[0].mText.v = gabi::guest_addr("InAll");
 sThirdFloorAnimationNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[31].mText.v = gabi::guest_addr("G_Room_09");
 sThirdFloorAnimationNames.mNames[1].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[2].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[3].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[32].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[32].mText.v = gabi::guest_addr("G_Room_10");
 sThirdFloorGroupNames.mNames[33].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[33].mText.v = gabi::guest_addr("G_Room_11");
 sThirdFloorGroupNames.mNames[34].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[34].mText.v = gabi::guest_addr("G_Room_12");
 sThirdFloorGroupNames.mNames[35].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[35].mText.v = gabi::guest_addr("G_Room_13");
 sThirdFloorGroupNames.mNames[36].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[36].mText.v = gabi::guest_addr("G_Room_14");
 sThirdFloorGroupNames.mNames[37].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[37].mText.v = gabi::guest_addr("G_Room_15");
 sThirdFloorGroupNames.mNames[38].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[38].mText.v = gabi::guest_addr("G_Room_16");
 sThirdFloorGroupNames.mNames[39].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[39].mText.v = gabi::guest_addr("G_Room_17");
 sThirdFloorGroupNames.mNames[40].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[40].mText.v = gabi::guest_addr("G_Room_18");
 sThirdFloorGroupNames.mNames[41].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[41].mText.v = gabi::guest_addr("G_Room_19");
 sThirdFloorGroupNames.mNames[42].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[42].mText.v = gabi::guest_addr("G_Room_20");
 sThirdFloorGroupNames.mNames[43].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[43].mText.v = gabi::guest_addr("G_Room_21");
 sThirdFloorGroupNames.mNames[44].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorGroupNames.mNames[44].mText.v = gabi::guest_addr("G_All_00");
 sThirdFloorAnimationNames.mNames[4].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[5].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[5].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[6].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[6].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[7].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[7].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[8].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[8].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[9].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[9].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[10].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[10].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[11].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[11].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[12].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[12].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[13].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[13].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[14].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[14].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[15].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[15].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[16].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[16].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[17].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[17].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[18].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[18].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[19].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[19].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[20].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[20].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[21].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[21].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[22].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[22].mText.v = gabi::guest_addr("InRoom");
 sThirdFloorAnimationNames.mNames[23].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[23].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[24].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[24].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[25].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[25].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[26].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[26].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[27].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[27].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[28].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[28].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[29].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[29].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[30].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[30].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[31].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[31].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[32].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[32].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[33].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[33].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[34].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[34].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[35].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[35].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[36].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[36].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[37].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[37].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[38].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[38].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[39].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[39].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[40].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[40].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[41].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[41].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorLayout.mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorLayout.mText.v = gabi::guest_addr("DungeonMapFloor_03");
 sThirdFloorAnimationNames.mNames[42].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[42].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[43].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[43].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[44].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorAnimationNames.mNames[44].mText.v = gabi::guest_addr("ColorRoom");
 sThirdFloorAnimationNames.mNames[45].mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorArchive.mVtable = reinterpret_cast<hdUiStringTable*>(&sThirdFloorStringVtable);
 sThirdFloorArchive.mText.v = gabi::guest_addr("DungeonMapFloor_03");
 sThirdFloorAnimationNames.mNames[45].mText.v = gabi::guest_addr("OutAll");
}
/* WWHD glue
data sThirdFloorStorageRegistration 101F5CBC
data sThirdFloorMinimum 100F2174
data sThirdFloorMaximum 100F2178
data sThirdFloorFirstFlagRegistration 101F5CC8
data sThirdFloorSecondFlagRegistration 101F5CD4
data sThirdFloorOffset 100F2180
data sThirdFloorOrigin 100F217C
data sThirdFloorStatics 104931A0
data sThirdFloorStringVtable 100F202C
 str 100F2184 "N_1F_00" in 02663BA8
 str 100F218C "N_2F_00" in 02663BA8
 str 100F2194 "N_3F_00" in 02663BA8
 str 100F219C "N_4F_00" in 02663BA8
 str 100F21A4 "InAll" in 02663BA8
 str 100F21AC "InRoom" in 02663BA8
 str 100F21B4 "OutAll" in 02663BA8
 str 100F21BC "G_Room_00" in 02663BA8
 str 100F21C8 "G_Room_01" in 02663BA8
 str 100F21D4 "G_Room_02" in 02663BA8
 str 100F21E0 "G_Room_03" in 02663BA8
 str 100F21EC "G_Room_04" in 02663BA8
 str 100F21F8 "G_Room_05" in 02663BA8
 str 100F2204 "G_Room_06" in 02663BA8
 str 100F2210 "G_Room_07" in 02663BA8
 str 100F221C "G_Room_08" in 02663BA8
 str 100F2228 "G_Room_09" in 02663BA8
 str 100F2234 "G_Room_10" in 02663BA8
 str 100F2240 "G_Room_11" in 02663BA8
 str 100F224C "G_Room_12" in 02663BA8
 str 100F2258 "G_Room_13" in 02663BA8
 str 100F2264 "G_Room_14" in 02663BA8
 str 100F2270 "G_Room_15" in 02663BA8
 str 100F227C "G_Room_16" in 02663BA8
 str 100F2288 "G_Room_17" in 02663BA8
 str 100F2294 "G_Room_18" in 02663BA8
 str 100F22A0 "G_Room_19" in 02663BA8
 str 100F22AC "G_Room_20" in 02663BA8
 str 100F22B8 "G_Room_21" in 02663BA8
 str 100F22C4 "ColorRoom" in 02663BA8
 str 100F22D0 "N_All_00" in 02663BA8
 str 100F22DC "G_All_00" in 02663BA8
 str 100F22E8 "DungeonMapFloor_03" in 02663BA8
 data sThirdFloorUiSystem 1018C404 nocheck
 data sThirdFloorAnimationNames 1049335C
 data sThirdFloorGroupNames 104931F4
 data sThirdFloorCaptions 104931CC
 data sThirdFloorStartTable 100F2044
 data sThirdFloorEndTable 100F2064
 data sThirdFloorVtable 100F2300
 data sThirdFloorOwnerVtable 100F2530
 data sThirdFloorSecondaryVtable 100F2540
 data sThirdFloorArchive 10493198
 data sThirdFloorLayout 104934CC
*/
