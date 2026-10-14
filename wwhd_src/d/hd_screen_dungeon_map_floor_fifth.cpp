/* HD DungeonMapFloor_05: part lifetime and resource loading. */
#include "d/hd_dungeon_map_floor_variants.h"
static hdPartsVtbl_c sFifthFloorVtable;
static u8 sFifthFloorOwnerVtable;
static u8 sFifthFloorSecondaryVtable;
static hdUiSafeString sFifthFloorArchive;
static hdUiSafeString sFifthFloorLayout;
void prepareFifthFloor(hdDungeonMapFloor_l*);

/* 026649A0 */
void hdDungeonMapFloorFifth::destroy(u32 flags) {
 if (this == NULL) return;
 mStateHolder.mHolderVtable = &sFifthFloorOwnerVtable;
 mSecondaryVtable = &sFifthFloorSecondaryVtable;
 hdSeaChartMap_c::destroy(reinterpret_cast<hdSeaChartMap_c*>(this), 0);
 if (flags & 1) operator_delete(this);
}
/* 02664A0C */
hdDungeonMapFloorFifth* hdDungeonMapFloorFifth::construct(hdDungeonMapFloorFifth* self) {
 if (!self) {
  self = static_cast<hdDungeonMapFloorFifth*>(operator_new(sizeof(hdDungeonMapFloorFifth)));
  if (!self) return NULL;
 }
 hdSeaChartMap_c::construct(reinterpret_cast<hdSeaChartMap_c*>(self));
 self->mVtable = &sFifthFloorVtable;
 self->mStateHolder.mHolderVtable = &sFifthFloorOwnerVtable;
 self->mSecondaryVtable = &sFifthFloorSecondaryVtable;
 return self;
}
/* 02664B04 */
u32 hdDungeonMapFloorFifth::loadFloor(u32 heap, u32 resource, void* context) {
 this->attachArchives(&sFifthFloorArchive, 1);
 if (!vLoad(&sFifthFloorLayout, gabi::ea(mpResAccessor.get()), heap, resource, context)) return 0;
 prepareFifthFloor(this);
 return 1;
}
/* 02664BC4 */
void hdDungeonMapFloorFifth::finalizeFloor() {
 reinterpret_cast<hdSeaChartMap_c*>(this)->releaseLayoutAndCells();
 this->detachArchives();
}
/* 02664BF8 */
void hdDungeonMapFloorFifth::drawFloor(void* info) {
 vDrawLayout(info);
}
static hdDungeonFloorNameTable sFifthFloorCaptions;
static hdDungeonFloorRangeTable sFifthFloorStartTable;
static hdDungeonFloorRangeTable sFifthFloorEndTable;
/* 02664A78 */
void prepareFifthFloor(hdDungeonMapFloor_l* self) {
 self->mFloorCount = 4;
 self->mpFloorCaptions = &sFifthFloorCaptions.mNames[0];
 auto* chart = reinterpret_cast<hdSeaChartMap_c*>(self);
 chart->setResourceMappings(reinterpret_cast<SeaMapResourceMapping_l*>(&sFifthFloorStartTable.mValue),
                            reinterpret_cast<SeaMapMarkerMapping_l*>(&sFifthFloorEndTable.mValue));
 chart->configureAnimations(0, 18, 3, 0, 37, 0, 1, 1, 19, 19);
 chart->initializeCellPanes();
}
static hdScreenUiSystem* sFifthFloorUiSystem;
static hdDungeonFloorNameTable sFifthFloorAnimationNames;
static hdDungeonFloorNameTable sFifthFloorGroupNames;
void floorBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
void floorBindAnimations(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*, u32);
/* 02664C08 */
u32 hdDungeonMapFloorFifth::loadLayout(void* name, u32 accessor, u32 heap, u32 resource, void* context) {
 auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sFifthFloorUiSystem->mHeap, 4));
 if (layout) layout = hdPartsLayout_c::construct(layout);
 mpLayout = layout;
 if (!layout || !layout->vLoad(name, accessor, heap, resource, 38, 37, context)) return 0;
 floorBindAnimation(mpLayout->animSet(), 0, &sFifthFloorAnimationNames.mNames[0], &sFifthFloorGroupNames.mNames[0]);
 floorBindAnimations(mpLayout->animSet(), 1, &sFifthFloorAnimationNames.mNames[1], &sFifthFloorGroupNames.mNames[1], 18);
 floorBindAnimations(mpLayout->animSet(), 19, &sFifthFloorAnimationNames.mNames[19], &sFifthFloorGroupNames.mNames[19], 18);
 floorBindAnimation(mpLayout->animSet(), 37, &sFifthFloorAnimationNames.mNames[37], &sFifthFloorGroupNames.mNames[0]);
 vCalcLayout();
 return 1;
}
static hdDungeonFloorRegistration sFifthFloorStorageRegistration;
static f32 sFifthFloorMinimum;
static f32 sFifthFloorMaximum;
static hdDungeonFloorRegistration sFifthFloorFirstFlagRegistration;
static hdDungeonFloorRegistration sFifthFloorSecondFlagRegistration;
static f32 sFifthFloorOrigin;
static f32 sFifthFloorOffset;
static hdDungeonFloorStatics sFifthFloorStatics;
static u8 sFifthFloorStringVtable;
void floorRegisterStatic(hdDungeonFloorRegistration*);
void floorConstructFirstFlag(be<u8>*);
void floorConstructSecondFlag(be<u8>*);
/* 02664D50 */
static void initializeFifthFloorStatics() {
 sFifthFloorStatics.mStorage[3] = 0;
 sFifthFloorStatics.mStorage[2] = 0;
 sFifthFloorStatics.mStorage[1] = 0;
 sFifthFloorStatics.mStorage[0] = 0;
 floorRegisterStatic(&sFifthFloorStorageRegistration);
 const f32 minimum = sFifthFloorMinimum;
 const f32 maximum = sFifthFloorMaximum;
 sFifthFloorStatics.mMinimum = minimum;
 sFifthFloorStatics.mMaximum = maximum;
 floorConstructFirstFlag(&sFifthFloorStatics.mFirstFlag);
 floorRegisterStatic(&sFifthFloorFirstFlagRegistration);
 floorConstructSecondFlag(&sFifthFloorStatics.mSecondFlag);
 floorRegisterStatic(&sFifthFloorSecondFlagRegistration);
 sFifthFloorCaptions.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorCaptions.mNames[0].mText.v = gabi::guest_addr("N_All_00");
 const f32 origin = sFifthFloorOrigin;
 const f32 offset = sFifthFloorOffset;
 sFifthFloorGroupNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorCaptions.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[0].mText.v = gabi::guest_addr("G_All_00");
 sFifthFloorGroupNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorStatics.mOrigin.x = origin;
 sFifthFloorGroupNames.mNames[1].mText.v = gabi::guest_addr("G_Room_00");
 sFifthFloorGroupNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorCaptions.mNames[1].mText.v = gabi::guest_addr("N_B1_00");
 sFifthFloorCaptions.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorCaptions.mNames[2].mText.v = gabi::guest_addr("N_1F_00");
 sFifthFloorStatics.mOffset.x = offset;
 sFifthFloorStatics.mOffset.y = offset;
 sFifthFloorGroupNames.mNames[2].mText.v = gabi::guest_addr("G_Room_01");
 sFifthFloorGroupNames.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[3].mText.v = gabi::guest_addr("G_Room_02");
 sFifthFloorGroupNames.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[4].mText.v = gabi::guest_addr("G_Room_03");
 sFifthFloorStatics.mOrigin.y = origin;
 sFifthFloorGroupNames.mNames[5].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[5].mText.v = gabi::guest_addr("G_Room_04");
 sFifthFloorGroupNames.mNames[6].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[6].mText.v = gabi::guest_addr("G_Room_05");
 sFifthFloorCaptions.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[7].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[7].mText.v = gabi::guest_addr("G_Room_06");
 sFifthFloorGroupNames.mNames[8].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[8].mText.v = gabi::guest_addr("G_Room_07");
 sFifthFloorGroupNames.mNames[9].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[9].mText.v = gabi::guest_addr("G_Room_08");
 sFifthFloorGroupNames.mNames[10].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[10].mText.v = gabi::guest_addr("G_Room_09");
 sFifthFloorGroupNames.mNames[11].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[11].mText.v = gabi::guest_addr("G_Room_10");
 sFifthFloorGroupNames.mNames[12].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[12].mText.v = gabi::guest_addr("G_Room_11");
 sFifthFloorGroupNames.mNames[13].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorCaptions.mNames[3].mText.v = gabi::guest_addr("N_2F_00");
 sFifthFloorGroupNames.mNames[13].mText.v = gabi::guest_addr("G_Room_12");
 sFifthFloorGroupNames.mNames[14].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[14].mText.v = gabi::guest_addr("G_Room_13");
 sFifthFloorGroupNames.mNames[15].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[15].mText.v = gabi::guest_addr("G_Room_14");
 sFifthFloorGroupNames.mNames[16].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[16].mText.v = gabi::guest_addr("G_Room_15");
 sFifthFloorGroupNames.mNames[17].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[17].mText.v = gabi::guest_addr("G_Room_16");
 sFifthFloorGroupNames.mNames[18].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[18].mText.v = gabi::guest_addr("G_Room_17");
 sFifthFloorGroupNames.mNames[19].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[19].mText.v = gabi::guest_addr("G_Room_00");
 sFifthFloorGroupNames.mNames[20].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[20].mText.v = gabi::guest_addr("G_Room_01");
 sFifthFloorGroupNames.mNames[21].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[21].mText.v = gabi::guest_addr("G_Room_02");
 sFifthFloorGroupNames.mNames[22].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[22].mText.v = gabi::guest_addr("G_Room_03");
 sFifthFloorGroupNames.mNames[23].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[23].mText.v = gabi::guest_addr("G_Room_04");
 sFifthFloorGroupNames.mNames[24].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[24].mText.v = gabi::guest_addr("G_Room_05");
 sFifthFloorAnimationNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[0].mText.v = gabi::guest_addr("InAll");
 sFifthFloorGroupNames.mNames[25].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[1].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorGroupNames.mNames[25].mText.v = gabi::guest_addr("G_Room_06");
 sFifthFloorGroupNames.mNames[26].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[26].mText.v = gabi::guest_addr("G_Room_07");
 sFifthFloorGroupNames.mNames[27].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[27].mText.v = gabi::guest_addr("G_Room_08");
 sFifthFloorGroupNames.mNames[28].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[28].mText.v = gabi::guest_addr("G_Room_09");
 sFifthFloorGroupNames.mNames[29].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[29].mText.v = gabi::guest_addr("G_Room_10");
 sFifthFloorGroupNames.mNames[30].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[30].mText.v = gabi::guest_addr("G_Room_11");
 sFifthFloorGroupNames.mNames[31].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[31].mText.v = gabi::guest_addr("G_Room_12");
 sFifthFloorGroupNames.mNames[32].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[32].mText.v = gabi::guest_addr("G_Room_13");
 sFifthFloorGroupNames.mNames[33].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[33].mText.v = gabi::guest_addr("G_Room_14");
 sFifthFloorGroupNames.mNames[34].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[34].mText.v = gabi::guest_addr("G_Room_15");
 sFifthFloorGroupNames.mNames[35].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[35].mText.v = gabi::guest_addr("G_Room_16");
 sFifthFloorGroupNames.mNames[36].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorGroupNames.mNames[36].mText.v = gabi::guest_addr("G_Room_17");
 sFifthFloorAnimationNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[2].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[3].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[4].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[5].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[5].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[6].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[6].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[7].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[7].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[8].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[8].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[9].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[9].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[10].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[10].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[11].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[11].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[12].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[12].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[13].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[13].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[14].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[14].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[15].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[15].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[16].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[16].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[17].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[17].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[18].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[18].mText.v = gabi::guest_addr("InRoom");
 sFifthFloorAnimationNames.mNames[19].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[19].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[20].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[20].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[21].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[21].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[22].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[22].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[23].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[23].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[24].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[24].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[25].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[25].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[26].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[26].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[27].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[27].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorLayout.mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[28].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[28].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[29].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[29].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[30].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[30].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[31].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[31].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[32].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[32].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[33].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[33].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[34].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[34].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[35].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[35].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[36].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorArchive.mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[36].mText.v = gabi::guest_addr("ColorRoom");
 sFifthFloorAnimationNames.mNames[37].mVtable = reinterpret_cast<hdUiStringTable*>(&sFifthFloorStringVtable);
 sFifthFloorAnimationNames.mNames[37].mText.v = gabi::guest_addr("OutAll");
 sFifthFloorArchive.mText.v = gabi::guest_addr("DungeonMapFloor_05");
 sFifthFloorLayout.mText.v = gabi::guest_addr("DungeonMapFloor_05");
}
/* WWHD glue
data sFifthFloorStorageRegistration 101F5D04
data sFifthFloorMinimum 100F2B9C
data sFifthFloorMaximum 100F2BA0
data sFifthFloorFirstFlagRegistration 101F5D10
data sFifthFloorSecondFlagRegistration 101F5D1C
data sFifthFloorOrigin 100F2BA4
data sFifthFloorOffset 100F2BA8
data sFifthFloorStatics 10493810
data sFifthFloorStringVtable 100F2A8C
 str 100F2BAC "N_B1_00" in 02664D50
 str 100F2BB4 "N_1F_00" in 02664D50
 str 100F2BBC "N_2F_00" in 02664D50
 str 100F2BC4 "InAll" in 02664D50
 str 100F2BCC "InRoom" in 02664D50
 str 100F2BD4 "OutAll" in 02664D50
 str 100F2BDC "G_Room_00" in 02664D50
 str 100F2BE8 "G_Room_01" in 02664D50
 str 100F2BF4 "G_Room_02" in 02664D50
 str 100F2C00 "G_Room_03" in 02664D50
 str 100F2C0C "G_Room_04" in 02664D50
 str 100F2C18 "G_Room_05" in 02664D50
 str 100F2C24 "G_Room_06" in 02664D50
 str 100F2C30 "G_Room_07" in 02664D50
 str 100F2C3C "G_Room_08" in 02664D50
 str 100F2C48 "G_Room_09" in 02664D50
 str 100F2C54 "G_Room_10" in 02664D50
 str 100F2C60 "G_Room_11" in 02664D50
 str 100F2C6C "G_Room_12" in 02664D50
 str 100F2C78 "G_Room_13" in 02664D50
 str 100F2C84 "G_Room_14" in 02664D50
 str 100F2C90 "G_Room_15" in 02664D50
 str 100F2C9C "G_Room_16" in 02664D50
 str 100F2CA8 "G_Room_17" in 02664D50
 str 100F2CB4 "ColorRoom" in 02664D50
 str 100F2CC0 "N_All_00" in 02664D50
 str 100F2CCC "G_All_00" in 02664D50
 str 100F2CD8 "DungeonMapFloor_05" in 02664D50
 data sFifthFloorUiSystem 1018C404 nocheck
 data sFifthFloorAnimationNames 10493984
 data sFifthFloorGroupNames 1049385C
 data sFifthFloorCaptions 1049383C
 data sFifthFloorStartTable 100F2AA4
 data sFifthFloorEndTable 100F2ABC
 data sFifthFloorVtable 100F2CF0
 data sFifthFloorOwnerVtable 100F2F20
 data sFifthFloorSecondaryVtable 100F2F30
 data sFifthFloorArchive 10493808
 data sFifthFloorLayout 10493AB4
*/
