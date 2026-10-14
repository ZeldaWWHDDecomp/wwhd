/* HD DungeonMapFloor_04: part lifetime and resource loading. */
#include "d/hd_dungeon_map_floor_variants.h"
static hdPartsVtbl_c sFourthFloorVtable;
static u8 sFourthFloorOwnerVtable;
static u8 sFourthFloorSecondaryVtable;
static hdUiSafeString sFourthFloorArchive;
static hdUiSafeString sFourthFloorLayout;
void prepareFourthFloor(hdDungeonMapFloor_l*);

/* 026640D4 */
void hdDungeonMapFloorFourth::destroy(u32 flags) {
 if (this == NULL) return;
 mStateHolder.mHolderVtable = &sFourthFloorOwnerVtable;
 mSecondaryVtable = &sFourthFloorSecondaryVtable;
 hdSeaChartMap_c::destroy(reinterpret_cast<hdSeaChartMap_c*>(this), 0);
 if (flags & 1) operator_delete(this);
}
/* 02664140 */
hdDungeonMapFloorFourth* hdDungeonMapFloorFourth::construct(hdDungeonMapFloorFourth* self) {
 if (!self) {
  self = static_cast<hdDungeonMapFloorFourth*>(operator_new(sizeof(hdDungeonMapFloorFourth)));
  if (!self) return NULL;
 }
 hdSeaChartMap_c::construct(reinterpret_cast<hdSeaChartMap_c*>(self));
 self->mVtable = &sFourthFloorVtable;
 self->mStateHolder.mHolderVtable = &sFourthFloorOwnerVtable;
 self->mSecondaryVtable = &sFourthFloorSecondaryVtable;
 return self;
}
/* 02664238 */
u32 hdDungeonMapFloorFourth::loadFloor(u32 heap, u32 resource, void* context) {
 this->attachArchives(&sFourthFloorArchive, 1);
 if (!vLoad(&sFourthFloorLayout, gabi::ea(mpResAccessor.get()), heap, resource, context)) return 0;
 prepareFourthFloor(this);
 return 1;
}
/* 026642F8 */
void hdDungeonMapFloorFourth::finalizeFloor() {
 reinterpret_cast<hdSeaChartMap_c*>(this)->releaseLayoutAndCells();
 this->detachArchives();
}
/* 0266432C */
void hdDungeonMapFloorFourth::drawFloor(void* info) {
 vDrawLayout(info);
}
static hdDungeonFloorNameTable sFourthFloorCaptions;
static hdDungeonFloorRangeTable sFourthFloorStartTable;
static hdDungeonFloorRangeTable sFourthFloorEndTable;
/* 026641AC */
void prepareFourthFloor(hdDungeonMapFloor_l* self) {
 self->mFloorCount = 4;
 self->mpFloorCaptions = &sFourthFloorCaptions.mNames[0];
 auto* chart = reinterpret_cast<hdSeaChartMap_c*>(self);
 chart->setResourceMappings(reinterpret_cast<SeaMapResourceMapping_l*>(&sFourthFloorStartTable.mValue),
                            reinterpret_cast<SeaMapMarkerMapping_l*>(&sFourthFloorEndTable.mValue));
 chart->configureAnimations(0, 22, 3, 0, 45, 44, 1, 0, 23, 22);
 chart->initializeCellPanes();
}
static hdScreenUiSystem* sFourthFloorUiSystem;
static hdDungeonFloorNameTable sFourthFloorAnimationNames;
static hdDungeonFloorNameTable sFourthFloorGroupNames;
void floorBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
void floorBindAnimations(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*, u32);
/* 0266433C */
u32 hdDungeonMapFloorFourth::loadLayout(void* name, u32 accessor, u32 heap, u32 resource, void* context) {
 auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sFourthFloorUiSystem->mHeap, 4));
 if (layout) layout = hdPartsLayout_c::construct(layout);
 mpLayout = layout;
 if (!layout || !layout->vLoad(name, accessor, heap, resource, 46, 45, context)) return 0;
 floorBindAnimation(mpLayout->animSet(), 0, &sFourthFloorAnimationNames.mNames[0], &sFourthFloorGroupNames.mNames[44]);
 floorBindAnimations(mpLayout->animSet(), 1, &sFourthFloorAnimationNames.mNames[1], &sFourthFloorGroupNames.mNames[0], 22);
 floorBindAnimations(mpLayout->animSet(), 23, &sFourthFloorAnimationNames.mNames[23], &sFourthFloorGroupNames.mNames[22], 22);
 floorBindAnimation(mpLayout->animSet(), 45, &sFourthFloorAnimationNames.mNames[45], &sFourthFloorGroupNames.mNames[44]);
 vCalcLayout();
 return 1;
}
static hdDungeonFloorRegistration sFourthFloorStorageRegistration;
static f32 sFourthFloorMinimum;
static f32 sFourthFloorMaximum;
static hdDungeonFloorRegistration sFourthFloorFirstFlagRegistration;
static hdDungeonFloorRegistration sFourthFloorSecondFlagRegistration;
static f32 sFourthFloorOrigin;
static f32 sFourthFloorOffset;
static hdDungeonFloorStatics sFourthFloorStatics;
static u8 sFourthFloorStringVtable;
void floorRegisterStatic(hdDungeonFloorRegistration*);
void floorConstructFirstFlag(be<u8>*);
void floorConstructSecondFlag(be<u8>*);
/* 02664484 */
static void initializeFourthFloorStatics() {
 sFourthFloorStatics.mStorage[3] = 0;
 sFourthFloorStatics.mStorage[2] = 0;
 sFourthFloorStatics.mStorage[1] = 0;
 sFourthFloorStatics.mStorage[0] = 0;
 floorRegisterStatic(&sFourthFloorStorageRegistration);
 const f32 minimum = sFourthFloorMinimum;
 const f32 maximum = sFourthFloorMaximum;
 sFourthFloorStatics.mMinimum = minimum;
 sFourthFloorStatics.mMaximum = maximum;
 floorConstructFirstFlag(&sFourthFloorStatics.mFirstFlag);
 floorRegisterStatic(&sFourthFloorFirstFlagRegistration);
 floorConstructSecondFlag(&sFourthFloorStatics.mSecondFlag);
 floorRegisterStatic(&sFourthFloorSecondFlagRegistration);
 sFourthFloorCaptions.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorCaptions.mNames[0].mText.v = gabi::guest_addr("N_All_00");
 const f32 origin = sFourthFloorOrigin;
 const f32 offset = sFourthFloorOffset;
 sFourthFloorStatics.mOrigin.x = origin;
 sFourthFloorCaptions.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorCaptions.mNames[1].mText.v = gabi::guest_addr("N_B2_00");
 sFourthFloorStatics.mOffset.x = offset;
 sFourthFloorCaptions.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorCaptions.mNames[2].mText.v = gabi::guest_addr("N_B1_00");
 sFourthFloorGroupNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[0].mText.v = gabi::guest_addr("G_Room_00");
 sFourthFloorGroupNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[1].mText.v = gabi::guest_addr("G_Room_01");
 sFourthFloorCaptions.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorStatics.mOffset.y = offset;
 sFourthFloorGroupNames.mNames[2].mText.v = gabi::guest_addr("G_Room_02");
 sFourthFloorGroupNames.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[3].mText.v = gabi::guest_addr("G_Room_03");
 sFourthFloorStatics.mOrigin.y = origin;
 sFourthFloorGroupNames.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[4].mText.v = gabi::guest_addr("G_Room_04");
 sFourthFloorGroupNames.mNames[5].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[5].mText.v = gabi::guest_addr("G_Room_05");
 sFourthFloorGroupNames.mNames[6].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[6].mText.v = gabi::guest_addr("G_Room_06");
 sFourthFloorGroupNames.mNames[7].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[7].mText.v = gabi::guest_addr("G_Room_07");
 sFourthFloorGroupNames.mNames[8].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[8].mText.v = gabi::guest_addr("G_Room_08");
 sFourthFloorGroupNames.mNames[9].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[9].mText.v = gabi::guest_addr("G_Room_09");
 sFourthFloorGroupNames.mNames[10].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[10].mText.v = gabi::guest_addr("G_Room_10");
 sFourthFloorGroupNames.mNames[11].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[11].mText.v = gabi::guest_addr("G_Room_11");
 sFourthFloorGroupNames.mNames[12].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[12].mText.v = gabi::guest_addr("G_Room_12");
 sFourthFloorCaptions.mNames[3].mText.v = gabi::guest_addr("N_1F_00");
 sFourthFloorGroupNames.mNames[13].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[13].mText.v = gabi::guest_addr("G_Room_13");
 sFourthFloorGroupNames.mNames[14].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[14].mText.v = gabi::guest_addr("G_Room_14");
 sFourthFloorGroupNames.mNames[15].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[15].mText.v = gabi::guest_addr("G_Room_15");
 sFourthFloorGroupNames.mNames[16].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[16].mText.v = gabi::guest_addr("G_Room_16");
 sFourthFloorGroupNames.mNames[17].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[17].mText.v = gabi::guest_addr("G_Room_17");
 sFourthFloorGroupNames.mNames[18].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[18].mText.v = gabi::guest_addr("G_Room_18");
 sFourthFloorGroupNames.mNames[19].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[19].mText.v = gabi::guest_addr("G_Room_19");
 sFourthFloorGroupNames.mNames[20].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[20].mText.v = gabi::guest_addr("G_Room_20");
 sFourthFloorGroupNames.mNames[21].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[21].mText.v = gabi::guest_addr("G_Room_21");
 sFourthFloorGroupNames.mNames[22].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[22].mText.v = gabi::guest_addr("G_Room_00");
 sFourthFloorGroupNames.mNames[23].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[23].mText.v = gabi::guest_addr("G_Room_01");
 sFourthFloorGroupNames.mNames[24].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[24].mText.v = gabi::guest_addr("G_Room_02");
 sFourthFloorGroupNames.mNames[25].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[25].mText.v = gabi::guest_addr("G_Room_03");
 sFourthFloorGroupNames.mNames[26].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[26].mText.v = gabi::guest_addr("G_Room_04");
 sFourthFloorGroupNames.mNames[27].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[27].mText.v = gabi::guest_addr("G_Room_05");
 sFourthFloorGroupNames.mNames[28].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[28].mText.v = gabi::guest_addr("G_Room_06");
 sFourthFloorGroupNames.mNames[29].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[29].mText.v = gabi::guest_addr("G_Room_07");
 sFourthFloorGroupNames.mNames[30].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[0].mText.v = gabi::guest_addr("InAll");
 sFourthFloorGroupNames.mNames[30].mText.v = gabi::guest_addr("G_Room_08");
 sFourthFloorGroupNames.mNames[31].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[31].mText.v = gabi::guest_addr("G_Room_09");
 sFourthFloorGroupNames.mNames[32].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[32].mText.v = gabi::guest_addr("G_Room_10");
 sFourthFloorGroupNames.mNames[33].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[33].mText.v = gabi::guest_addr("G_Room_11");
 sFourthFloorGroupNames.mNames[34].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[34].mText.v = gabi::guest_addr("G_Room_12");
 sFourthFloorGroupNames.mNames[35].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[35].mText.v = gabi::guest_addr("G_Room_13");
 sFourthFloorGroupNames.mNames[36].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[36].mText.v = gabi::guest_addr("G_Room_14");
 sFourthFloorGroupNames.mNames[37].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[37].mText.v = gabi::guest_addr("G_Room_15");
 sFourthFloorGroupNames.mNames[38].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[38].mText.v = gabi::guest_addr("G_Room_16");
 sFourthFloorGroupNames.mNames[39].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[39].mText.v = gabi::guest_addr("G_Room_17");
 sFourthFloorGroupNames.mNames[40].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[40].mText.v = gabi::guest_addr("G_Room_18");
 sFourthFloorGroupNames.mNames[41].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[41].mText.v = gabi::guest_addr("G_Room_19");
 sFourthFloorGroupNames.mNames[42].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[42].mText.v = gabi::guest_addr("G_Room_20");
 sFourthFloorGroupNames.mNames[43].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[43].mText.v = gabi::guest_addr("G_Room_21");
 sFourthFloorAnimationNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[44].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorGroupNames.mNames[44].mText.v = gabi::guest_addr("G_All_00");
 sFourthFloorAnimationNames.mNames[1].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[2].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[3].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[4].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[5].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[5].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[6].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[6].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[7].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[7].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[8].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[8].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[9].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[9].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[10].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[10].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[11].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[11].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[12].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[12].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[13].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[13].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[14].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[14].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[15].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[15].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[16].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[16].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[17].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[17].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[18].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[18].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[19].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[19].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[20].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[20].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[21].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[21].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[22].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[22].mText.v = gabi::guest_addr("InRoom");
 sFourthFloorAnimationNames.mNames[23].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[23].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[24].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[24].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[25].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[25].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[26].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[26].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[27].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[27].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[28].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[28].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[29].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[29].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[30].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[30].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[31].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[31].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[32].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[32].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[33].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[33].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorLayout.mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[34].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[34].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[35].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[35].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[36].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[36].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[37].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[37].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[38].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[38].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[39].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[39].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[40].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[40].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[41].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[41].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[42].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[42].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[43].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[43].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorAnimationNames.mNames[44].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorAnimationNames.mNames[44].mText.v = gabi::guest_addr("ColorRoom");
 sFourthFloorLayout.mText.v = gabi::guest_addr("DungeonMapFloor_04");
 sFourthFloorAnimationNames.mNames[45].mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorArchive.mVtable = reinterpret_cast<hdUiStringTable*>(&sFourthFloorStringVtable);
 sFourthFloorArchive.mText.v = gabi::guest_addr("DungeonMapFloor_04");
 sFourthFloorAnimationNames.mNames[45].mText.v = gabi::guest_addr("OutAll");
}
/* WWHD glue
data sFourthFloorStorageRegistration 101F5CE0
data sFourthFloorMinimum 100F26A4
data sFourthFloorMaximum 100F26A8
data sFourthFloorFirstFlagRegistration 101F5CEC
data sFourthFloorSecondFlagRegistration 101F5CF8
data sFourthFloorOrigin 100F26AC
data sFourthFloorOffset 100F26B0
data sFourthFloorStatics 104934DC
data sFourthFloorStringVtable 100F2564
 str 100F26B4 "N_B2_00" in 02664484
 str 100F26BC "N_B1_00" in 02664484
 str 100F26C4 "N_1F_00" in 02664484
 str 100F26CC "InAll" in 02664484
 str 100F26D4 "InRoom" in 02664484
 str 100F26DC "OutAll" in 02664484
 str 100F26E4 "G_Room_00" in 02664484
 str 100F26F0 "G_Room_01" in 02664484
 str 100F26FC "G_Room_02" in 02664484
 str 100F2708 "G_Room_03" in 02664484
 str 100F2714 "G_Room_04" in 02664484
 str 100F2720 "G_Room_05" in 02664484
 str 100F272C "G_Room_06" in 02664484
 str 100F2738 "G_Room_07" in 02664484
 str 100F2744 "G_Room_08" in 02664484
 str 100F2750 "G_Room_09" in 02664484
 str 100F275C "G_Room_10" in 02664484
 str 100F2768 "G_Room_11" in 02664484
 str 100F2774 "G_Room_12" in 02664484
 str 100F2780 "G_Room_13" in 02664484
 str 100F278C "G_Room_14" in 02664484
 str 100F2798 "G_Room_15" in 02664484
 str 100F27A4 "G_Room_16" in 02664484
 str 100F27B0 "G_Room_17" in 02664484
 str 100F27BC "G_Room_18" in 02664484
 str 100F27C8 "G_Room_19" in 02664484
 str 100F27D4 "G_Room_20" in 02664484
 str 100F27E0 "G_Room_21" in 02664484
 str 100F27EC "ColorRoom" in 02664484
 str 100F27F8 "N_All_00" in 02664484
 str 100F2804 "G_All_00" in 02664484
 str 100F2810 "DungeonMapFloor_04" in 02664484
 data sFourthFloorUiSystem 1018C404 nocheck
 data sFourthFloorAnimationNames 10493690
 data sFourthFloorGroupNames 10493528
 data sFourthFloorCaptions 10493508
 data sFourthFloorStartTable 100F257C
 data sFourthFloorEndTable 100F2594
 data sFourthFloorVtable 100F2828
 data sFourthFloorOwnerVtable 100F2A58
 data sFourthFloorSecondaryVtable 100F2A68
 data sFourthFloorArchive 104934D4
 data sFourthFloorLayout 10493800
*/
