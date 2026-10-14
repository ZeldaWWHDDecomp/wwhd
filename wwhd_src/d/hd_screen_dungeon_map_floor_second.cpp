/* HD DungeonMapFloor_02: part lifetime and resource loading. */
#include "d/hd_dungeon_map_floor_variants.h"
static hdPartsVtbl_c sSecondFloorVtable;
static u8 sSecondFloorOwnerVtable;
static u8 sSecondFloorSecondaryVtable;
static hdUiSafeString sSecondFloorArchive;
static hdUiSafeString sSecondFloorLayout;
void prepareSecondFloor(hdDungeonMapFloor_l*);

/* 02662E44 */
void hdDungeonMapFloorSecond::destroy(u32 flags) {
 if (this == NULL) return;
 mStateHolder.mHolderVtable = &sSecondFloorOwnerVtable;
 mSecondaryVtable = &sSecondFloorSecondaryVtable;
 hdSeaChartMap_c::destroy(reinterpret_cast<hdSeaChartMap_c*>(this), 0);
 if (flags & 1) operator_delete(this);
}
/* 02662EB0 */
hdDungeonMapFloorSecond* hdDungeonMapFloorSecond::construct(hdDungeonMapFloorSecond* self) {
 if (!self) {
  self = static_cast<hdDungeonMapFloorSecond*>(operator_new(sizeof(hdDungeonMapFloorSecond)));
  if (!self) return NULL;
 }
 hdSeaChartMap_c::construct(reinterpret_cast<hdSeaChartMap_c*>(self));
 self->mVtable = &sSecondFloorVtable;
 self->mStateHolder.mHolderVtable = &sSecondFloorOwnerVtable;
 self->mSecondaryVtable = &sSecondFloorSecondaryVtable;
 return self;
}
/* 02662FA8 */
u32 hdDungeonMapFloorSecond::loadFloor(u32 heap, u32 resource, void* context) {
 this->attachArchives(&sSecondFloorArchive, 1);
 if (!vLoad(&sSecondFloorLayout, gabi::ea(mpResAccessor.get()), heap, resource, context)) return 0;
 prepareSecondFloor(this);
 return 1;
}
/* 02663068 */
void hdDungeonMapFloorSecond::finalizeFloor() {
 reinterpret_cast<hdSeaChartMap_c*>(this)->releaseLayoutAndCells();
 this->detachArchives();
}
/* 0266309C */
void hdDungeonMapFloorSecond::drawFloor(void* info) {
 vDrawLayout(info);
}
static hdDungeonFloorNameTable sSecondFloorCaptions;
static hdDungeonFloorRangeTable sSecondFloorStartTable;
static hdDungeonFloorRangeTable sSecondFloorEndTable;
/* 02662F1C */
void prepareSecondFloor(hdDungeonMapFloor_l* self) {
 self->mFloorCount = 6;
 self->mpFloorCaptions = &sSecondFloorCaptions.mNames[0];
 auto* chart = reinterpret_cast<hdSeaChartMap_c*>(self);
 chart->setResourceMappings(reinterpret_cast<SeaMapResourceMapping_l*>(&sSecondFloorStartTable.mValue),
                            reinterpret_cast<SeaMapMarkerMapping_l*>(&sSecondFloorEndTable.mValue));
 chart->configureAnimations(0, 27, 5, 0, 55, 54, 1, 0, 28, 27);
 chart->initializeCellPanes();
}
static hdScreenUiSystem* sSecondFloorUiSystem;
static hdDungeonFloorNameTable sSecondFloorAnimationNames;
static hdDungeonFloorNameTable sSecondFloorGroupNames;
void floorBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
void floorBindAnimations(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*, u32);
/* 026630AC */
u32 hdDungeonMapFloorSecond::loadLayout(void* name, u32 accessor, u32 heap, u32 resource, void* context) {
 auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sSecondFloorUiSystem->mHeap, 4));
 if (layout) layout = hdPartsLayout_c::construct(layout);
 mpLayout = layout;
 if (!layout || !layout->vLoad(name, accessor, heap, resource, 56, 55, context)) return 0;
 floorBindAnimation(mpLayout->animSet(), 0, &sSecondFloorAnimationNames.mNames[0], &sSecondFloorGroupNames.mNames[54]);
 floorBindAnimations(mpLayout->animSet(), 1, &sSecondFloorAnimationNames.mNames[1], &sSecondFloorGroupNames.mNames[0], 27);
 floorBindAnimations(mpLayout->animSet(), 28, &sSecondFloorAnimationNames.mNames[28], &sSecondFloorGroupNames.mNames[27], 27);
 floorBindAnimation(mpLayout->animSet(), 55, &sSecondFloorAnimationNames.mNames[55], &sSecondFloorGroupNames.mNames[54]);
 vCalcLayout();
 return 1;
}
static hdDungeonFloorRegistration sSecondFloorStorageRegistration;
static f32 sSecondFloorMinimum;
static f32 sSecondFloorMaximum;
static hdDungeonFloorRegistration sSecondFloorFirstFlagRegistration;
static hdDungeonFloorRegistration sSecondFloorSecondFlagRegistration;
static f32 sSecondFloorOrigin;
static f32 sSecondFloorOffset;
static hdDungeonFloorStatics sSecondFloorStatics;
static u8 sSecondFloorStringVtable;
void floorRegisterStatic(hdDungeonFloorRegistration*);
void floorConstructFirstFlag(be<u8>*);
void floorConstructSecondFlag(be<u8>*);
/* 026631F4 */
static void initializeSecondFloorStatics() {
 sSecondFloorStatics.mStorage[3] = 0;
 sSecondFloorStatics.mStorage[2] = 0;
 sSecondFloorStatics.mStorage[1] = 0;
 sSecondFloorStatics.mStorage[0] = 0;
 floorRegisterStatic(&sSecondFloorStorageRegistration);
 const f32 minimum = sSecondFloorMinimum;
 const f32 maximum = sSecondFloorMaximum;
 sSecondFloorStatics.mMinimum = minimum;
 sSecondFloorStatics.mMaximum = maximum;
 floorConstructFirstFlag(&sSecondFloorStatics.mFirstFlag);
 floorRegisterStatic(&sSecondFloorFirstFlagRegistration);
 floorConstructSecondFlag(&sSecondFloorStatics.mSecondFlag);
 floorRegisterStatic(&sSecondFloorSecondFlagRegistration);
 const f32 origin = sSecondFloorOrigin;
 sSecondFloorStatics.mOrigin.y = origin;
 const f32 offset = sSecondFloorOffset;
 sSecondFloorStatics.mOffset.y = offset;
 sSecondFloorStatics.mOrigin.x = origin;
 sSecondFloorCaptions.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorCaptions.mNames[0].mText.v = gabi::guest_addr("N_All_00");
 sSecondFloorCaptions.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorCaptions.mNames[1].mText.v = gabi::guest_addr("N_B1_00");
 sSecondFloorGroupNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorCaptions.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorCaptions.mNames[2].mText.v = gabi::guest_addr("N_1F_00");
 sSecondFloorCaptions.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorCaptions.mNames[3].mText.v = gabi::guest_addr("N_2F_00");
 sSecondFloorGroupNames.mNames[0].mText.v = gabi::guest_addr("G_Room_00");
 sSecondFloorCaptions.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[1].mText.v = gabi::guest_addr("G_Room_01");
 sSecondFloorGroupNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[2].mText.v = gabi::guest_addr("G_Room_02");
 sSecondFloorCaptions.mNames[4].mText.v = gabi::guest_addr("N_3F_00");
 sSecondFloorGroupNames.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[3].mText.v = gabi::guest_addr("G_Room_03");
 sSecondFloorCaptions.mNames[5].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[4].mText.v = gabi::guest_addr("G_Room_04");
 sSecondFloorGroupNames.mNames[5].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[5].mText.v = gabi::guest_addr("G_Room_05");
 sSecondFloorGroupNames.mNames[6].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[6].mText.v = gabi::guest_addr("G_Room_06");
 sSecondFloorStatics.mOffset.x = offset;
 sSecondFloorGroupNames.mNames[7].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[7].mText.v = gabi::guest_addr("G_Room_07");
 sSecondFloorCaptions.mNames[5].mText.v = gabi::guest_addr("N_4F_00");
 sSecondFloorGroupNames.mNames[8].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[8].mText.v = gabi::guest_addr("G_Room_08");
 sSecondFloorGroupNames.mNames[9].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[9].mText.v = gabi::guest_addr("G_Room_09");
 sSecondFloorGroupNames.mNames[10].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[10].mText.v = gabi::guest_addr("G_Room_10");
 sSecondFloorGroupNames.mNames[11].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[11].mText.v = gabi::guest_addr("G_Room_11");
 sSecondFloorGroupNames.mNames[12].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[12].mText.v = gabi::guest_addr("G_Room_12");
 sSecondFloorGroupNames.mNames[13].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[13].mText.v = gabi::guest_addr("G_Room_13");
 sSecondFloorGroupNames.mNames[14].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[14].mText.v = gabi::guest_addr("G_Room_14");
 sSecondFloorGroupNames.mNames[15].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[15].mText.v = gabi::guest_addr("G_Room_15");
 sSecondFloorGroupNames.mNames[16].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[16].mText.v = gabi::guest_addr("G_Room_16");
 sSecondFloorGroupNames.mNames[17].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[17].mText.v = gabi::guest_addr("G_Room_17");
 sSecondFloorGroupNames.mNames[18].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[18].mText.v = gabi::guest_addr("G_Room_18");
 sSecondFloorGroupNames.mNames[19].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[19].mText.v = gabi::guest_addr("G_Room_19");
 sSecondFloorGroupNames.mNames[20].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[20].mText.v = gabi::guest_addr("G_Room_20");
 sSecondFloorGroupNames.mNames[21].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[21].mText.v = gabi::guest_addr("G_Room_21");
 sSecondFloorGroupNames.mNames[22].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[22].mText.v = gabi::guest_addr("G_Room_22");
 sSecondFloorGroupNames.mNames[23].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[23].mText.v = gabi::guest_addr("G_Room_23");
 sSecondFloorGroupNames.mNames[24].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[24].mText.v = gabi::guest_addr("G_Room_24");
 sSecondFloorGroupNames.mNames[25].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[25].mText.v = gabi::guest_addr("G_Room_25");
 sSecondFloorGroupNames.mNames[26].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[26].mText.v = gabi::guest_addr("G_Room_26");
 sSecondFloorGroupNames.mNames[27].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[27].mText.v = gabi::guest_addr("G_Room_00");
 sSecondFloorGroupNames.mNames[28].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[28].mText.v = gabi::guest_addr("G_Room_01");
 sSecondFloorGroupNames.mNames[29].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[29].mText.v = gabi::guest_addr("G_Room_02");
 sSecondFloorGroupNames.mNames[30].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[30].mText.v = gabi::guest_addr("G_Room_03");
 sSecondFloorGroupNames.mNames[31].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[31].mText.v = gabi::guest_addr("G_Room_04");
 sSecondFloorGroupNames.mNames[32].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[32].mText.v = gabi::guest_addr("G_Room_05");
 sSecondFloorGroupNames.mNames[33].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[33].mText.v = gabi::guest_addr("G_Room_06");
 sSecondFloorGroupNames.mNames[34].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[34].mText.v = gabi::guest_addr("G_Room_07");
 sSecondFloorGroupNames.mNames[35].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[35].mText.v = gabi::guest_addr("G_Room_08");
 sSecondFloorGroupNames.mNames[36].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[36].mText.v = gabi::guest_addr("G_Room_09");
 sSecondFloorGroupNames.mNames[37].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[37].mText.v = gabi::guest_addr("G_Room_10");
 sSecondFloorGroupNames.mNames[38].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[38].mText.v = gabi::guest_addr("G_Room_11");
 sSecondFloorAnimationNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[0].mText.v = gabi::guest_addr("InAll");
 sSecondFloorAnimationNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[39].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[1].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[2].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[39].mText.v = gabi::guest_addr("G_Room_12");
 sSecondFloorGroupNames.mNames[40].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[40].mText.v = gabi::guest_addr("G_Room_13");
 sSecondFloorGroupNames.mNames[41].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[41].mText.v = gabi::guest_addr("G_Room_14");
 sSecondFloorGroupNames.mNames[42].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[42].mText.v = gabi::guest_addr("G_Room_15");
 sSecondFloorGroupNames.mNames[43].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[43].mText.v = gabi::guest_addr("G_Room_16");
 sSecondFloorGroupNames.mNames[44].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[44].mText.v = gabi::guest_addr("G_Room_17");
 sSecondFloorGroupNames.mNames[45].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[45].mText.v = gabi::guest_addr("G_Room_18");
 sSecondFloorGroupNames.mNames[46].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[46].mText.v = gabi::guest_addr("G_Room_19");
 sSecondFloorGroupNames.mNames[47].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[47].mText.v = gabi::guest_addr("G_Room_20");
 sSecondFloorGroupNames.mNames[48].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[48].mText.v = gabi::guest_addr("G_Room_21");
 sSecondFloorGroupNames.mNames[49].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[49].mText.v = gabi::guest_addr("G_Room_22");
 sSecondFloorGroupNames.mNames[50].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[50].mText.v = gabi::guest_addr("G_Room_23");
 sSecondFloorGroupNames.mNames[51].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[51].mText.v = gabi::guest_addr("G_Room_24");
 sSecondFloorGroupNames.mNames[52].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[52].mText.v = gabi::guest_addr("G_Room_25");
 sSecondFloorGroupNames.mNames[53].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[53].mText.v = gabi::guest_addr("G_Room_26");
 sSecondFloorAnimationNames.mNames[3].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorGroupNames.mNames[54].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorGroupNames.mNames[54].mText.v = gabi::guest_addr("G_All_00");
 sSecondFloorAnimationNames.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[4].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[5].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[5].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[6].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[6].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[7].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[7].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[8].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[8].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[9].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[9].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[10].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[10].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[11].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[11].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[12].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[12].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[13].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[13].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[14].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[14].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[15].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[15].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[16].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[16].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[17].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[17].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[18].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[18].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[19].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[19].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[20].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[20].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[21].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[21].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[22].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[22].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[23].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[23].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[24].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[24].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[25].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[25].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[26].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[26].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[27].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[27].mText.v = gabi::guest_addr("InRoom");
 sSecondFloorAnimationNames.mNames[28].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[28].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[29].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[29].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[30].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[30].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[31].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[31].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[32].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[32].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[33].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[33].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[34].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[34].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[35].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[35].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[36].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[36].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[37].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[37].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[38].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[38].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[39].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[39].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[40].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[40].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[41].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[41].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[42].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[42].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[43].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[43].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[44].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[44].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[45].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[45].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[46].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[46].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[47].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[47].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[48].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[48].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[49].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[49].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[50].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[50].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[51].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[51].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[52].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[52].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[53].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[53].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorAnimationNames.mNames[54].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorArchive.mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[54].mText.v = gabi::guest_addr("ColorRoom");
 sSecondFloorLayout.mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[55].mVtable = reinterpret_cast<hdUiStringTable*>(&sSecondFloorStringVtable);
 sSecondFloorAnimationNames.mNames[55].mText.v = gabi::guest_addr("OutAll");
 sSecondFloorArchive.mText.v = gabi::guest_addr("DungeonMapFloor_02");
 sSecondFloorLayout.mText.v = gabi::guest_addr("DungeonMapFloor_02");
}
/* WWHD glue
data sSecondFloorStorageRegistration 101F5C98
data sSecondFloorMinimum 100F1BFC
data sSecondFloorMaximum 100F1C00
data sSecondFloorFirstFlagRegistration 101F5CA4
data sSecondFloorSecondFlagRegistration 101F5CB0
data sSecondFloorOrigin 100F1C04
data sSecondFloorOffset 100F1C08
data sSecondFloorStatics 10492DBC
data sSecondFloorStringVtable 100F1A74
 str 100F1C0C "N_B1_00" in 026631F4
 str 100F1C14 "N_1F_00" in 026631F4
 str 100F1C1C "N_2F_00" in 026631F4
 str 100F1C24 "N_3F_00" in 026631F4
 str 100F1C2C "N_4F_00" in 026631F4
 str 100F1C34 "InAll" in 026631F4
 str 100F1C3C "InRoom" in 026631F4
 str 100F1C44 "OutAll" in 026631F4
 str 100F1C4C "G_Room_00" in 026631F4
 str 100F1C58 "G_Room_01" in 026631F4
 str 100F1C64 "G_Room_02" in 026631F4
 str 100F1C70 "G_Room_03" in 026631F4
 str 100F1C7C "G_Room_04" in 026631F4
 str 100F1C88 "G_Room_05" in 026631F4
 str 100F1C94 "G_Room_06" in 026631F4
 str 100F1CA0 "G_Room_07" in 026631F4
 str 100F1CAC "G_Room_08" in 026631F4
 str 100F1CB8 "G_Room_09" in 026631F4
 str 100F1CC4 "G_Room_10" in 026631F4
 str 100F1CD0 "G_Room_11" in 026631F4
 str 100F1CDC "G_Room_12" in 026631F4
 str 100F1CE8 "G_Room_13" in 026631F4
 str 100F1CF4 "G_Room_14" in 026631F4
 str 100F1D00 "G_Room_15" in 026631F4
 str 100F1D0C "G_Room_16" in 026631F4
 str 100F1D18 "G_Room_17" in 026631F4
 str 100F1D24 "G_Room_18" in 026631F4
 str 100F1D30 "G_Room_19" in 026631F4
 str 100F1D3C "G_Room_20" in 026631F4
 str 100F1D48 "G_Room_21" in 026631F4
 str 100F1D54 "G_Room_22" in 026631F4
 str 100F1D60 "G_Room_23" in 026631F4
 str 100F1D6C "G_Room_24" in 026631F4
 str 100F1D78 "G_Room_25" in 026631F4
 str 100F1D84 "G_Room_26" in 026631F4
 str 100F1D90 "ColorRoom" in 026631F4
 str 100F1D9C "N_All_00" in 026631F4
 str 100F1DA8 "G_All_00" in 026631F4
 str 100F1DB4 "DungeonMapFloor_02" in 026631F4
 data sSecondFloorUiSystem 1018C404 nocheck
 data sSecondFloorAnimationNames 10492FD0
 data sSecondFloorGroupNames 10492E18
 data sSecondFloorCaptions 10492DE8
 data sSecondFloorStartTable 100F1A8C
 data sSecondFloorEndTable 100F1AB4
 data sSecondFloorVtable 100F1DC8
 data sSecondFloorOwnerVtable 100F1FF8
 data sSecondFloorSecondaryVtable 100F2008
 data sSecondFloorArchive 10492DB4
 data sSecondFloorLayout 10493190
*/
