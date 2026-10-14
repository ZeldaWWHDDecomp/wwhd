/* HD DungeonMapDown_00: indicator visibility and caption. */
#include "d/hd_dungeon_map_down.h"
#include "d/d_save_readable.h"
static hdPartsVtbl_c sDownVtable;
static u8 sDownOwnerVtable;
static u8 sDownSecondaryVtable;
static u8 sDownStringVtable;
static hdUiSafeString sDownArchive;
static hdUiSafeString sDownLayout;
static hdUiSafeString sDownCaptionPaneName;
static hdUiSafeString sDownFallbackCaption;
static hdDungeonDownStringTable sDownStageNames;
static hdDungeonDownStringTable sDownCaptionNames;
static hdDungeonDownFloors sDownFloorRecords;
static hdDungeonDownEmptyCaption sDownEmptyCaption;
static u32 sDownTextStyle;
static HdPadUiPointer sDownUi;
void downConstructTransition(hdSeaChartStateHandlers_c*);
void downDestroyTransition(hdSeaChartStateHandlers_c*, u32);
void downSetTextStyle(hdDungeonDownCaption*, u32);
void downSetPosition(hdSeaChartStateHandlers_c*, hdDungeonDownAttachPane*);
void downApplyTransition(hdSeaChartStateHandlers_c*);
u32 downUiUsesSpecialCaptions(HdPadUi*);
void downResetMessage(hdDungeonDownCaption*, hdUiSafeString*, hdUiSafeString*);
void downStringConstruct(hdUiSafeString*);
/* 026602EC */
void hdDungeonMapDown::destroy(u32 flags) {
 if (!this) return;
 mStateHolder.mHolderVtable = &sDownOwnerVtable;
 mSecondaryVtable = &sDownSecondaryVtable;
 downDestroyTransition(this, 0);
 if (flags&1) operator_delete(this);
}
/* 02660358 */
hdDungeonMapDown* hdDungeonMapDown::construct(hdDungeonMapDown* self) {
 if (!self) {
  self = static_cast<hdDungeonMapDown*>(operator_new(sizeof(hdDungeonMapDown)));
  if (!self) return NULL;
 }
 downConstructTransition(self);
 self->mVtable = &sDownVtable;
 self->mSecondaryVtable = &sDownSecondaryVtable;
 self->mStateHolder.mHolderVtable = &sDownOwnerVtable;
 // GHS retains the embedded pointer constructor's wrapping NULL allocation.
 if (u32(gabi::ea(self)+offsetof(hdDungeonMapDown, mpCaption)) == 0) operator_new(4);
 self->mpVisibilityPane = NULL;
 self->mpAttachPane = NULL;
 self->mUseStageProgress = 0;
 self->mpCaption = NULL;
 return self;
}
/* 026603E8 */
void hdDungeonMapDown::findCaption() {
 hdPartsLyt_c* layout = mpLayout->lyt();
 sDownCaptionPaneName.assureTermination();
 auto* root = reinterpret_cast<hdDungeonDownRoot*>(layout->mpRootPane.get());
 mpCaption = gabi::call_ptr<hdDungeonDownCaption*>(root->mVtable->mFindPane.get(), root, sDownCaptionPaneName.mText.get(), 1);
}
/* 02660464 */
void hdDungeonMapDown::resetCaption() {
 if (mpCaption) {
  downSetTextStyle(mpCaption, sDownTextStyle);
  hdDungeonDownCaption* pane = mpCaption;
  gabi::call_ptr(pane->mVtable->mSetText.get(), pane, &sDownEmptyCaption, 0);
 }
}
/* 026604C8 */
u32 hdDungeonMapDown::loadIndicator(u32 heap, u32 resource, void* context) {
 attachArchives(&sDownArchive, 1);
 if (!vLoad(&sDownLayout, gabi::ea(mpResAccessor.get()), heap, resource, context)) return 0;
 findCaption();
 resetCaption();
 hdUiSafeString pane;
 pane.mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
 pane.mText.v = gabi::guest_addr("");
 bindPane(&pane);
 vCalcLayout();
 return 1;
}
/* 026605C8 */
void hdDungeonMapDown::finalizeIndicator() { vDestroyLayout(); detachArchives(); }
/* 02660608 */
void hdDungeonMapDown::executeIndicator() { hdParts_executeStateMachine(stateMachine()); }
/* 02660610 */
void hdDungeonMapDown::drawIndicator(void* info) {
 hdPartsStateId_c* state = hdParts_getCurrentStateId(stateMachine());
 auto* expectedTable = sTransitionBlankState.mVtable.get();
 u32 current = state->getKey();
 u32 blank = gabi::call_ptr<u32>(expectedTable->mGetKey.get(), &sTransitionBlankState);
 if (current != blank) vDrawLayout(info);
}
/* 026606B4 */
u32 hdDungeonMapDown::stageProgressVisible() {
 u32 dungeon = sDownUi.mOwner->mUi->mBossIndex;
 switch (dungeon) {
 case 1:return d_save_cpp::saveBase()->mInfo.mSavedata.mEvent.isEventBit(0x2A10)>0;
 case 2:return d_save_cpp::saveBase()->mInfo.isSwitch(0x1C, 5)>0;
 case 3:return d_save_cpp::saveBase()->mInfo.mSavedata.mEvent.isEventBit(0x1B01)>0;
 case 4:return d_save_cpp::saveBase()->mInfo.isSwitch(0x10, 3)>0;
 default:return 0;
 }
}
/* 02660790 */
void hdDungeonMapDown::displayIndicator() {
 if (mpAttachPane && mpVisibilityPane) {
  updateBlend();
  downSetPosition(this, mpAttachPane);
  if (mUseStageProgress) mTransition.mHidden = stageProgressVisible() != 0;
  else mTransition.mHidden = mpVisibilityPane->mVisible != 0;
 }
 downApplyTransition(this);
 hdParts_c::calcLayout();
}
/* 02660834 */
void hdDungeonMapDown::setCaptionLabel(hdDungeonDownCaption* pane, hdUiSafeString* label) {
 if (pane) {
  hdUiSafeString message;
  message.mText.v = gabi::ea(mpLayout->lyt()->mpMessageData.get());
  message.mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
  downResetMessage(pane, &message, label);
 }
}
/* 02660880 */
void hdDungeonMapDown::updateCaption() {
 hdDungeonDownFloors floors;
 for (u32 i = 0; i<5; ++i) {
  floors.mRecords[i].mFloor = sDownFloorRecords.mRecords[i].mFloor;
  floors.mRecords[i].mpName = sDownFloorRecords.mRecords[i].mpName;
 }
 if (downUiUsesSpecialCaptions(sDownUi.mOwner->mUi)) {
  u32 floor = mTransition.mPaneIndex;
  if (floor == 0x85) setCaptionLabel(mpCaption, &sDownCaptionNames.mNames[3]);
  else if (floor == 0x81) setCaptionLabel(mpCaption, &sDownCaptionNames.mNames[2]);
  else if (floor == 0x80) setCaptionLabel(mpCaption, &sDownFallbackCaption);
 } else {
  u32 floor = mTransition.mPaneIndex;
  for (u32 i = 0; i<5; ++i) {
   if (floor == floors.mRecords[i].mFloor) {
    setCaptionLabel(mpCaption, floors.mRecords[i].mpName);
    break;
   }
  }
 }
}
/* 02660984 */
void hdDungeonMapDown::detectStageProgress() {
 hdDungeonDownAttachPane* pane = mpAttachPane;
 if (!pane) return;
 u32 dungeon = sDownUi.mOwner->mUi->mBossIndex;
 if (dungeon<1 || dungeon>4) return;
 hdUiSafeString stage;
 stage.mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
 stage.mText = &pane->mStageName[0];
 downStringConstruct(&stage);
 stage.assureTermination();
 hdUiSafeString* expected = &sDownStageNames.mNames[dungeon-1];
 be<u8>* snapshot = stage.mText;
 expected->assureTermination();
 bool equal = snapshot == expected->mText.get();
 if (!equal) {
  be<u8>* left = stage.mText;
  be<u8>* right = expected->mText;
  for (u32 remaining = 0x40001; remaining; --remaining) {
   u8 a = *left++;
   u8 b = *right++;
   if (a != b) break;
   if (!a) {equal = true; break; }
  }
 }
 if (equal) mUseStageProgress = 1;
}
static hdDungeonMarkerRegistration sDownFloorStorageRegistration;
static f32 sDownFloorMinimum;
static f32 sDownFloorMaximum;
static hdDungeonMarkerRegistration sDownFloorFirstFlagRegistration;
static hdDungeonMarkerRegistration sDownFloorSecondFlagRegistration;
static f32 sDownFloorOrigin;
static f32 sDownFloorOffset;
static hdDungeonMarkerStatics sDownFloorStatics;
void floorRegisterStatic(hdDungeonMarkerRegistration*);
void floorConstructFirstFlag(be<u8>*);
void floorConstructSecondFlag(be<u8>*);
/* 02660CC8 */
static void initializeDownFloorStatics() {
 sDownFloorStatics.mStorage[3] = 0;
 sDownFloorStatics.mStorage[2] = 0;
 sDownFloorStatics.mStorage[1] = 0;
 sDownFloorStatics.mStorage[0] = 0;
 floorRegisterStatic(&sDownFloorStorageRegistration);
 const f32 minimum = sDownFloorMinimum;
 const f32 maximum = sDownFloorMaximum;
 sDownFloorStatics.mMinimum = minimum;
 sDownFloorStatics.mMaximum = maximum;
 floorConstructFirstFlag(&sDownFloorStatics.mFirstFlag);
 floorRegisterStatic(&sDownFloorFirstFlagRegistration);
 floorConstructSecondFlag(&sDownFloorStatics.mSecondFlag);
 floorRegisterStatic(&sDownFloorSecondFlagRegistration);
 const f32 origin = sDownFloorOrigin;
 const f32 offset = sDownFloorOffset;
 sDownFloorStatics.mOffset.x = offset;
 sDownFloorStatics.mOrigin.x = origin;
 sDownStageNames.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
 sDownFloorStatics.mOrigin.y = origin;
 sDownFloorStatics.mOffset.y = offset;
 sDownStageNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
 sDownCaptionNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
 sDownCaptionNames.mNames[0].mText.v = gabi::guest_addr("T_FloorD_00_B2");
 sDownCaptionNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
 sDownStageNames.mNames[2].mText.v = gabi::guest_addr("N_3FDown_00");
 sDownCaptionPaneName.mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
 sDownCaptionPaneName.mText.v = gabi::guest_addr("T_Floor_00");
 sDownCaptionNames.mNames[1].mText.v = gabi::guest_addr("T_FloorD_00_B1");
 sDownCaptionNames.mNames[2].mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
 sDownStageNames.mNames[3].mText.v = gabi::guest_addr("N_1FDown_00");
 sDownCaptionNames.mNames[2].mText.v = gabi::guest_addr("T_FloorD_00_1F");
 sDownCaptionNames.mNames[3].mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
 sDownCaptionNames.mNames[3].mText.v = gabi::guest_addr("T_FloorD_00_2F");
 sDownCaptionNames.mNames[4].mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
 sDownStageNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
 sDownStageNames.mNames[0].mText.v = gabi::guest_addr("N_2FDown_01");
 sDownStageNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
 sDownStageNames.mNames[1].mText.v = gabi::guest_addr("N_1FDown_00");
 sDownCaptionNames.mNames[4].mText.v = gabi::guest_addr("T_FloorD_00_3F");
 sDownFallbackCaption.mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
 sDownFallbackCaption.mText.v = gabi::guest_addr("T_FloorD_00_outdoor");
 sDownArchive.mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
 sDownArchive.mText.v = gabi::guest_addr("DungeonMapDown_00");
 sDownLayout.mVtable = reinterpret_cast<hdUiStringTable*>(&sDownStringVtable);
 sDownLayout.mText.v = gabi::guest_addr("DungeonMapDown_00");
}
/* WWHD glue
data sDownFloorStorageRegistration 101F5C2C
data sDownFloorMinimum 100F0E3C
data sDownFloorMaximum 100F0E40
data sDownFloorFirstFlagRegistration 101F5C38
data sDownFloorSecondFlagRegistration 101F5C44
data sDownFloorOrigin 100F0E44
data sDownFloorOffset 100F0E48
data sDownFloorStatics 10492798
ext 028F026C void floorRegisterStatic(hdDungeonMarkerRegistration*)
 str 100F0E4C "N_2FDown_01" in 02660CC8
 str 100F0E58 "N_1FDown_00" in 02660CC8
 str 100F0E64 "N_3FDown_00" in 02660CC8
 str 100F0E70 "T_FloorD_00_outdoor" in 02660CC8
 str 100F0E84 "DungeonMapDown_00" in 02660CC8
 str 100F0E98 "T_Floor_00" in 02660CC8
 str 100F0EA4 "T_FloorD_00_B2" in 02660CC8
 str 100F0EB4 "T_FloorD_00_B1" in 02660CC8
 str 100F0EC4 "T_FloorD_00_1F" in 02660CC8
 str 100F0ED4 "T_FloorD_00_2F" in 02660CC8
 str 100F0EE4 "T_FloorD_00_3F" in 02660CC8
 data sDownVtable 100F0EF8
 data sDownOwnerVtable 100F1038
 data sDownSecondaryVtable 100F1048
 data sDownStringVtable 100F0DF0
 data sDownArchive 10492790
 data sDownLayout 104927EC
 data sDownCaptionPaneName 10492780
 data sDownFallbackCaption 10492788
 data sDownStageNames 10492760
 data sDownCaptionNames 104927C4
 data sDownFloorRecords 100F0E14
 data sDownEmptyCaption 100F0E0C
 data sDownTextStyle 100F0DEC
 data sDownUi 101F8344
 str 100F0E10 "" in 026604C8
 ext 02685134 void downConstructTransition(hdSeaChartStateHandlers_c*)
 ext 02684E94 void downDestroyTransition(hdSeaChartStateHandlers_c*,u32)

 ext 02704AFC void downSetTextStyle(hdDungeonDownCaption*,u32)

 ext 026853E8 void downSetPosition(hdSeaChartStateHandlers_c*,hdDungeonDownAttachPane*)
 ext 026852B4 void downApplyTransition(hdSeaChartStateHandlers_c*)

 ext 0268714C u32 downUiUsesSpecialCaptions(HdPadUi*)
 ext 027045B0 void downResetMessage(hdDungeonDownCaption*,hdUiSafeString*,hdUiSafeString*)
 ext 02660EB0 void downStringConstruct(hdUiSafeString*)
*/
