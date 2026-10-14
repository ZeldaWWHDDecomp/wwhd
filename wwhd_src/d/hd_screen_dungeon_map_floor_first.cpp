/* HD DungeonMapFloor_01: part lifetime and resource loading. */
#include "d/hd_dungeon_map_floor_variants.h"
static hdPartsVtbl_c sFirstFloorVtable;
static u8 sFirstFloorOwnerVtable;
static u8 sFirstFloorSecondaryVtable;
static hdUiSafeString sFirstFloorArchive;
static hdUiSafeString sFirstFloorLayout;
void prepareFirstFloor(hdDungeonMapFloor_l*);

/* 02662590 */
void hdDungeonMapFloorFirst::destroy(u32 flags) {
 if (this == NULL) return;
 mStateHolder.mHolderVtable = &sFirstFloorOwnerVtable;
 mSecondaryVtable = &sFirstFloorSecondaryVtable;
 hdSeaChartMap_c::destroy(reinterpret_cast<hdSeaChartMap_c*>(this), 0);
 if (flags & 1) operator_delete(this);
}
/* 026625FC */
hdDungeonMapFloorFirst* hdDungeonMapFloorFirst::construct(hdDungeonMapFloorFirst* self) {
 if (!self) {
  self = static_cast<hdDungeonMapFloorFirst*>(operator_new(sizeof(hdDungeonMapFloorFirst)));
  if (!self) return NULL;
 }
 hdSeaChartMap_c::construct(reinterpret_cast<hdSeaChartMap_c*>(self));
 self->mVtable = &sFirstFloorVtable;
 self->mStateHolder.mHolderVtable = &sFirstFloorOwnerVtable;
 self->mSecondaryVtable = &sFirstFloorSecondaryVtable;
 return self;
}
/* 026626F4 */
u32 hdDungeonMapFloorFirst::loadFloor(u32 heap, u32 resource, void* context) {
 this->attachArchives(&sFirstFloorArchive, 1);
 if (!vLoad(&sFirstFloorLayout, gabi::ea(mpResAccessor.get()), heap, resource, context)) return 0;
 prepareFirstFloor(this);
 return 1;
}
/* 026627B4 */
void hdDungeonMapFloorFirst::finalizeFloor() {
 reinterpret_cast<hdSeaChartMap_c*>(this)->releaseLayoutAndCells();
 this->detachArchives();
}
/* 026627E8 */
void hdDungeonMapFloorFirst::drawFloor(void* info) {
 vDrawLayout(info);
}
static hdDungeonFloorNameTable sFirstFloorCaptions;
static hdDungeonFloorRangeTable sFirstFloorStartTable;
static hdDungeonFloorRangeTable sFirstFloorEndTable;
/* 02662668 */
void prepareFirstFloor(hdDungeonMapFloor_l* self) {
 self->mFloorCount = 5;
 self->mpFloorCaptions = &sFirstFloorCaptions.mNames[0];
 auto* chart = reinterpret_cast<hdSeaChartMap_c*>(self);
 chart->setResourceMappings(reinterpret_cast<SeaMapResourceMapping_l*>(&sFirstFloorStartTable.mValue),
                            reinterpret_cast<SeaMapMarkerMapping_l*>(&sFirstFloorEndTable.mValue));
 chart->configureAnimations(0, 21, 4, 0, 43, 42, 1, 0, 22, 21);
 chart->initializeCellPanes();
}
static hdScreenUiSystem* sFirstFloorUiSystem;
static hdDungeonFloorNameTable sFirstFloorAnimationNames;
static hdDungeonFloorNameTable sFirstFloorGroupNames;
void floorBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
void floorBindAnimations(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*, u32);
/* 026627F8 */
u32 hdDungeonMapFloorFirst::loadLayout(void* name, u32 accessor, u32 heap, u32 resource, void* context) {
 auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sFirstFloorUiSystem->mHeap, 4));
 if (layout) layout = hdPartsLayout_c::construct(layout);
 mpLayout = layout;
 if (!layout || !layout->vLoad(name, accessor, heap, resource, 44, 43, context)) return 0;
 floorBindAnimation(mpLayout->animSet(), 0, &sFirstFloorAnimationNames.mNames[0], &sFirstFloorGroupNames.mNames[42]);
 floorBindAnimations(mpLayout->animSet(), 1, &sFirstFloorAnimationNames.mNames[1], &sFirstFloorGroupNames.mNames[0], 21);
 floorBindAnimations(mpLayout->animSet(), 22, &sFirstFloorAnimationNames.mNames[22], &sFirstFloorGroupNames.mNames[21], 21);
 floorBindAnimation(mpLayout->animSet(), 43, &sFirstFloorAnimationNames.mNames[43], &sFirstFloorGroupNames.mNames[42]);
 vCalcLayout();
 return 1;
}
static hdDungeonFloorRegistration sFirstFloorStorageRegistration;
static f32 sFirstFloorMaximum;
static f32 sFirstFloorMinimum;
static hdDungeonFloorRegistration sFirstFloorFirstFlagRegistration;
static hdDungeonFloorRegistration sFirstFloorSecondFlagRegistration;
static f32 sFirstFloorOrigin;
static f32 sFirstFloorOffset;
static hdDungeonFloorStatics sFirstFloorStatics;
static u8 sFirstFloorStringVtable;
void floorRegisterStatic(hdDungeonFloorRegistration*);
void floorConstructFirstFlag(be<u8>*);
void floorConstructSecondFlag(be<u8>*);
/* 02662940 */
static void initializeFirstFloorStatics() {
 sFirstFloorStatics.mStorage[3] = 0;
 sFirstFloorStatics.mStorage[2] = 0;
 sFirstFloorStatics.mStorage[1] = 0;
 sFirstFloorStatics.mStorage[0] = 0;
 floorRegisterStatic(&sFirstFloorStorageRegistration);
 const f32 maximum = sFirstFloorMaximum;
 const f32 minimum = sFirstFloorMinimum;
 sFirstFloorStatics.mMaximum = maximum;
 sFirstFloorStatics.mMinimum = minimum;
 floorConstructFirstFlag(&sFirstFloorStatics.mFirstFlag);
 floorRegisterStatic(&sFirstFloorFirstFlagRegistration);
 floorConstructSecondFlag(&sFirstFloorStatics.mSecondFlag);
 floorRegisterStatic(&sFirstFloorSecondFlagRegistration);
 const f32 origin = sFirstFloorOrigin;
 const f32 offset = sFirstFloorOffset;
 sFirstFloorStatics.mOrigin.x = origin;
 sFirstFloorStatics.mOffset.y = offset;
 sFirstFloorStatics.mOrigin.y = origin;
 sFirstFloorGroupNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[0].mText.v = gabi::guest_addr("G_Room_00");
 sFirstFloorGroupNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[1].mText.v = gabi::guest_addr("G_Room_01");
 sFirstFloorGroupNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[2].mText.v = gabi::guest_addr("G_Room_02");
 sFirstFloorCaptions.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorCaptions.mNames[0].mText.v = gabi::guest_addr("N_All_00");
 sFirstFloorCaptions.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorCaptions.mNames[1].mText.v = gabi::guest_addr("N_1F_00");
 sFirstFloorCaptions.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorCaptions.mNames[2].mText.v = gabi::guest_addr("N_2F_00");
 sFirstFloorCaptions.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorCaptions.mNames[3].mText.v = gabi::guest_addr("N_3F_00");
 sFirstFloorCaptions.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorStatics.mOffset.x = offset;
 sFirstFloorGroupNames.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[3].mText.v = gabi::guest_addr("G_Room_03");
 sFirstFloorGroupNames.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[4].mText.v = gabi::guest_addr("G_Room_04");
 sFirstFloorGroupNames.mNames[5].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[5].mText.v = gabi::guest_addr("G_Room_05");
 sFirstFloorCaptions.mNames[4].mText.v = gabi::guest_addr("N_4F_00");
 sFirstFloorGroupNames.mNames[6].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[6].mText.v = gabi::guest_addr("G_Room_06");
 sFirstFloorGroupNames.mNames[7].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[7].mText.v = gabi::guest_addr("G_Room_07");
 sFirstFloorGroupNames.mNames[8].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[8].mText.v = gabi::guest_addr("G_Room_08");
 sFirstFloorGroupNames.mNames[9].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[9].mText.v = gabi::guest_addr("G_Room_09");
 sFirstFloorGroupNames.mNames[10].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[10].mText.v = gabi::guest_addr("G_Room_10");
 sFirstFloorGroupNames.mNames[11].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[11].mText.v = gabi::guest_addr("G_Room_11");
 sFirstFloorGroupNames.mNames[12].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[12].mText.v = gabi::guest_addr("G_Room_12");
 sFirstFloorGroupNames.mNames[13].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[13].mText.v = gabi::guest_addr("G_Room_13");
 sFirstFloorGroupNames.mNames[14].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[14].mText.v = gabi::guest_addr("G_Room_14");
 sFirstFloorGroupNames.mNames[15].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[15].mText.v = gabi::guest_addr("G_Room_15");
 sFirstFloorGroupNames.mNames[16].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[16].mText.v = gabi::guest_addr("G_Room_16");
 sFirstFloorGroupNames.mNames[17].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[17].mText.v = gabi::guest_addr("G_Room_17");
 sFirstFloorGroupNames.mNames[18].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[18].mText.v = gabi::guest_addr("G_Room_18");
 sFirstFloorGroupNames.mNames[19].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[19].mText.v = gabi::guest_addr("G_Room_19");
 sFirstFloorGroupNames.mNames[20].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[20].mText.v = gabi::guest_addr("G_Room_20");
 sFirstFloorGroupNames.mNames[21].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[21].mText.v = gabi::guest_addr("G_Room_00");
 sFirstFloorGroupNames.mNames[22].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[22].mText.v = gabi::guest_addr("G_Room_01");
 sFirstFloorGroupNames.mNames[23].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[23].mText.v = gabi::guest_addr("G_Room_02");
 sFirstFloorGroupNames.mNames[24].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[24].mText.v = gabi::guest_addr("G_Room_03");
 sFirstFloorGroupNames.mNames[25].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[25].mText.v = gabi::guest_addr("G_Room_04");
 sFirstFloorGroupNames.mNames[26].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[26].mText.v = gabi::guest_addr("G_Room_05");
 sFirstFloorGroupNames.mNames[27].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[27].mText.v = gabi::guest_addr("G_Room_06");
 sFirstFloorGroupNames.mNames[28].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[28].mText.v = gabi::guest_addr("G_Room_07");
 sFirstFloorGroupNames.mNames[29].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[29].mText.v = gabi::guest_addr("G_Room_08");
 sFirstFloorGroupNames.mNames[30].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[30].mText.v = gabi::guest_addr("G_Room_09");
 sFirstFloorGroupNames.mNames[31].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[31].mText.v = gabi::guest_addr("G_Room_10");
 sFirstFloorGroupNames.mNames[32].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[32].mText.v = gabi::guest_addr("G_Room_11");
 sFirstFloorGroupNames.mNames[33].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[33].mText.v = gabi::guest_addr("G_Room_12");
 sFirstFloorGroupNames.mNames[34].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[34].mText.v = gabi::guest_addr("G_Room_13");
 sFirstFloorGroupNames.mNames[35].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[35].mText.v = gabi::guest_addr("G_Room_14");
 sFirstFloorGroupNames.mNames[36].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[36].mText.v = gabi::guest_addr("G_Room_15");
 sFirstFloorGroupNames.mNames[37].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[0].mText.v = gabi::guest_addr("InAll");
 sFirstFloorGroupNames.mNames[37].mText.v = gabi::guest_addr("G_Room_16");
 sFirstFloorAnimationNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[1].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[2].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[38].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[38].mText.v = gabi::guest_addr("G_Room_17");
 sFirstFloorGroupNames.mNames[39].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[39].mText.v = gabi::guest_addr("G_Room_18");
 sFirstFloorGroupNames.mNames[40].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[40].mText.v = gabi::guest_addr("G_Room_19");
 sFirstFloorGroupNames.mNames[41].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[41].mText.v = gabi::guest_addr("G_Room_20");
 sFirstFloorAnimationNames.mNames[3].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorGroupNames.mNames[42].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorGroupNames.mNames[42].mText.v = gabi::guest_addr("G_All_00");
 sFirstFloorAnimationNames.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[4].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[5].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[5].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[6].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[6].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[7].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[7].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[8].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[8].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[9].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[9].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[10].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[10].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[11].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[11].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[12].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[12].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[13].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[13].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[14].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[14].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[15].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[15].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[16].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[16].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[17].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[17].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[18].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[18].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[19].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[19].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[20].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[20].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[21].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[21].mText.v = gabi::guest_addr("InRoom");
 sFirstFloorAnimationNames.mNames[22].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[22].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[23].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[23].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[24].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[24].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[25].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[25].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[26].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[26].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[27].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[27].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[28].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[28].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[29].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[29].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[30].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[30].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[31].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[31].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[32].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[32].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[33].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[33].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[34].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[34].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[35].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[35].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[36].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[36].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[37].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[37].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[38].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[38].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[39].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[39].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[40].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[40].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[41].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[41].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorAnimationNames.mNames[42].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[42].mText.v = gabi::guest_addr("ColorRoom");
 sFirstFloorLayout.mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[43].mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorArchive.mVtable = reinterpret_cast<hdUiStringTable*>(&sFirstFloorStringVtable);
 sFirstFloorAnimationNames.mNames[43].mText.v = gabi::guest_addr("OutAll");
 sFirstFloorArchive.mText.v = gabi::guest_addr("DungeonMapFloor_01");
 sFirstFloorLayout.mText.v = gabi::guest_addr("DungeonMapFloor_01");
}
/* WWHD glue
data sFirstFloorStorageRegistration 101F5C74
data sFirstFloorMaximum 100F1698
data sFirstFloorMinimum 100F1694
data sFirstFloorFirstFlagRegistration 101F5C80
data sFirstFloorSecondFlagRegistration 101F5C8C
data sFirstFloorOrigin 100F169C
data sFirstFloorOffset 100F16A0
data sFirstFloorStatics 10492AA0
data sFirstFloorStringVtable 100F155C
ext 028F026C void floorRegisterStatic(hdDungeonFloorRegistration*)
ext 028ED6F8 void floorConstructFirstFlag(be<u8>*)
ext 028EAB2C void floorConstructSecondFlag(be<u8>*)
 str 100F16A4 "N_1F_00" in 02662940
 str 100F16AC "N_2F_00" in 02662940
 str 100F16B4 "N_3F_00" in 02662940
 str 100F16BC "N_4F_00" in 02662940
 str 100F16C4 "InAll" in 02662940
 str 100F16CC "InRoom" in 02662940
 str 100F16D4 "OutAll" in 02662940
 str 100F16DC "G_Room_00" in 02662940
 str 100F16E8 "G_Room_01" in 02662940
 str 100F16F4 "G_Room_02" in 02662940
 str 100F1700 "G_Room_03" in 02662940
 str 100F170C "G_Room_04" in 02662940
 str 100F1718 "G_Room_05" in 02662940
 str 100F1724 "G_Room_06" in 02662940
 str 100F1730 "G_Room_07" in 02662940
 str 100F173C "G_Room_08" in 02662940
 str 100F1748 "G_Room_09" in 02662940
 str 100F1754 "G_Room_10" in 02662940
 str 100F1760 "G_Room_11" in 02662940
 str 100F176C "G_Room_12" in 02662940
 str 100F1778 "G_Room_13" in 02662940
 str 100F1784 "G_Room_14" in 02662940
 str 100F1790 "G_Room_15" in 02662940
 str 100F179C "G_Room_16" in 02662940
 str 100F17A8 "G_Room_17" in 02662940
 str 100F17B4 "G_Room_18" in 02662940
 str 100F17C0 "G_Room_19" in 02662940
 str 100F17CC "G_Room_20" in 02662940
 str 100F17D8 "ColorRoom" in 02662940
 str 100F17E4 "N_All_00" in 02662940
 str 100F17F0 "G_All_00" in 02662940
 str 100F17FC "DungeonMapFloor_01" in 02662940
 data sFirstFloorUiSystem 1018C404 nocheck
 data sFirstFloorAnimationNames 10492C4C
 data sFirstFloorGroupNames 10492AF4
 ext 02004E04 void floorBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
 ext 0200506C void floorBindAnimations(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*,u32)
 data sFirstFloorCaptions 10492ACC
 data sFirstFloorStartTable 100F1574
 data sFirstFloorEndTable 100F1594
 ext 0267E454 void hdSeaChartMap_c::setResourceMappings(SeaMapResourceMapping_l*,SeaMapMarkerMapping_l*)
 ext 0267E460 void hdSeaChartMap_c::configureAnimations(u32,u32,u32,u32,u32,u32,u32,u32,u32,u32)
 ext 0267E2B0 void hdSeaChartMap_c::initializeCellPanes()
 data sFirstFloorVtable 100F1810
 data sFirstFloorOwnerVtable 100F1A40
 data sFirstFloorSecondaryVtable 100F1A50
 data sFirstFloorArchive 10492A98
 data sFirstFloorLayout 10492DAC
 ext 0267CBE8 hdSeaChartMap_c* hdSeaChartMap_c::construct(hdSeaChartMap_c*)
 ext 0267B5AC void hdSeaChartMap_c::destroy(hdSeaChartMap_c*,u32)
 ext 0267CF04 void hdSeaChartMap_c::releaseLayoutAndCells()
*/
