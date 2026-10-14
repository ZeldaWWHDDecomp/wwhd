/* HD staff-roll scene, screen groups and render passes. */
#include "d/hd_scene_staff_roll.h"
#include "JSystem/J3DGraph.h"
extern void staffUpdateAmbientBubbles(hdStaffAmbientBubbles_c*);
extern void staffUpdateCharacterBubbles(hdStaffCharacterBubbles_c*);
extern void staffRegisterTask(void*, u32, hdStaffRenderTask_l*);
extern void staffInputMode(void*, u32);
extern void staffSoundMode(u32);
extern void* staffCreateHeap(u32, u32);
extern void staffHeapComplete(void*);
extern J3DModelData* staffGetModelResource(void*, SafeString*, SafeString*, SafeString*);
extern J3DModel* staffCreateModel(J3DModelData*, u32, u32);
extern mDoExt_btkAnm* staffBtkConstruct(mDoExt_btkAnm*);
extern hdStaffMaterialTable_l* staffMaterialTable(J3DModelData*);
extern void staffSafeStringValidate(SafeString*);
extern void* staffGetResource(void*, SafeString*, SafeString*, u32);
extern hdStaffResource_l* staffResourceView(void*);
extern hdStaffFile_l* staffFindFile(void*, const char*);
extern void staffSortBubbles(hdStaffBubbleList_l*, void*);
extern hdStaffPlay_l* staffGetPlay();
extern void staffModelUpdateDL(J3DModel*, void*);
extern void* staffAllocate(u32);
extern void staffStateExecute(HdStateMachine*);
extern void staffStateChange(HdStateMachine*, hdSaveStateId_l*);
extern void staffDestroyHeap(void*);
extern void staffSetViewport(u32, u32, u32, u32, u32, u32, u32);
static void staffModelUpdate(hdStaffModel_l*, u32);
extern void staffMatrixTranslate(Mtx34*, f32, f32, f32);
extern void staffMatrixRotate(Mtx34*, s16, s16, s16);
extern void staffSoundStart(u32);
extern u32 staffSoundReady(u32);
extern void* staffGetScreenDim();
extern void staffSetScreenDimEnabled(void*, u32);
extern u32 staffChaseFade(be<f32>*, f32, f32);
extern u32 staffIsActor(hdStaffProcess_l*);
extern u32 staffIsDrawable(hdStaffProcess_l*);
extern void staffRemoveDrawTag(void*);
extern void* staffSearch(void*, u32);
extern void staffDelete(void*);
extern void staffTaskConstruct(hdStaffRenderTask_l*);
extern void staffConstructState(HdStateMachine*, hdStaffScene_c*, void*);
extern void staffReserveList(hdStaffBubbleList_l*, u32, gptr<hdParts_c>*);
extern void staffRegisterGlobal(void*);
extern void staffInitSigned(be<u8>*);
extern void staffInitUnsigned(be<u8>*);
extern void staffTaskDestroy(void*, u32);
extern void staffDisposerDestroy(HdStateDisposer*, u32);
static hdStaffScene_c* staffConstruct(hdStaffScene_c*);
extern hdStaffScene_c* staffAllocateInstance(u32, void*, u32);
extern void staffDisposerConstruct(HdStateDisposer*, void*, u32);
static u32 l_staffBaseVTable;
static u32 l_staffObjectVTable;
static u32 l_staffBaseTaskVTable;
static u32 l_staffTaskVTable;
static u32 l_staffSceneVTable;
static u32 l_staffFactory;
static u32 l_staffDelegateBaseVTable;
static u32 l_staffDelegateEmptyVTable;
static u8 l_staffEmptyName;
static HdStateStatics l_staffStatics;
static hdStaffUiRanges_l l_staffUiRanges;
static HdStateRegistration l_staffRegistration1;
static HdStateRegistration l_staffRegistration2;
static HdStateRegistration l_staffRegistration3;
static f32 l_staffLower;
static f32 l_staffUpper;
static u32 l_staffStateInitialized;
static u32 l_staffLastStateId;
static u32 l_staffStateVTable;
static u32 l_staffLoadBgmEnter;
static u32 l_staffLoadBgmCalc;
static u32 l_staffLoadBgmExit;
static u32 l_staffLoadBgmQuery;
static u32 l_staffStartWaitEnter;
static u32 l_staffStartWaitCalc;
static u32 l_staffStartWaitExit;
static u32 l_staffStartWaitQuery;
static u32 l_staffFadeOutInEnter;
static u32 l_staffFadeOutInCalc;
static u32 l_staffFadeOutInExit;
static u32 l_staffFadeOutInQuery;
static u32 l_staffCalcEnter;
static u32 l_staffCalcCalc;
static u32 l_staffCalcExit;
static u32 l_staffCalcQuery;
static u32 l_staffEndEnter;
static u32 l_staffEndCalc;
static u32 l_staffEndExit;
static u32 l_staffEndQuery;
static hdTitleGameSingleton_l l_staffGameState;
static hdTitleManagerSingleton_l l_staffInput;
static hdPartsStateId_c l_staffBackgroundStart;
static hdSaveStateId_l l_staffInitialState;
static u32 l_staffRenderDelegateVTable;
static u32 l_staffRenderCallback;
static u32 l_staffBackgroundCallback;
static u32 l_staffFadeCallback;
static u8 l_staffTaskName;
static u8 l_staffBackgroundTaskName;
static u8 l_staffFadeTaskName;
static hdTitleManagerSingleton_l l_staffResControl;
static Mtx34 l_staffMatrix;
static hdTitleManagerSingleton_l l_staffResourceMgr;
static u32 l_staffBubbleParamsVTable;
static hdStaffRollBubbleStateId_c l_staffBubbleWait;
static u32 l_staffBubbleSort;
static u8 l_staffDefaultTarget;
static u8 l_staffDefaultCamera;
static hdStaffDrawState_l l_staffDrawState;
static hdTitleLayerSingleton_l l_staffLayers;
static hdSceneLayoutManagerSingleton_l l_staffLayoutMgr;
static u32 l_staffSafeStringVTable;
static u32 l_staffModelVTable;
static u32 l_staffDisposerVTable;
static hdStaffSceneSingleton_l l_staffScene;
static hdStaffDisposerSingleton_l l_staffDisposer;
static hdSaveStateId_l l_staffStartWait;
static hdSaveStateId_l l_staffFadeOutIn;
static hdSaveStateId_l l_staffCalc;
static hdSaveStateId_l l_staffFinish;
static hdStaffSoundTiming_l l_staffTiming;
static u32 l_staffProcessCallback;
/* 0271B914 */
static u32 staffSaveInitQuery() { return 0; }
/* 0271B928 */
static u32 staffSaveEndQuery() { return 0; }
/* 0271B930 */
static u32 staffStateQuery() { return 0; }
/* 0271B938 */
static u32 staffStateId(hdSaveStateId_l* state) { return state->mId; }
/* 0271BA0C */
static s32 staffCompareBubble(hdStaffRollBubbleBase_c* a, hdStaffRollBubbleBase_c* b) {
 f32 first = a->mMotion.getScale();
 f32 second = b->mMotion.getScale();
 return first-second>0.0f?1:-1;
}
/* 0271BA7C */
void hdStaffAmbientBubbles_c::create() {
 hdRenderLayerRegistry_l* layers = l_staffLayers.mpInstance;
 hdLayerBase_c* params = static_cast<hdLayerBase_c*>(layers->mpLayers[layers->mLayerCount>3?3:0].get());
 for(u32 i = 0; i<BUBBLE_COUNT; ++i) {
  hdStaffRollBubbleS_c* screen = static_cast<hdStaffRollBubbleS_c*>(staffAllocate(0xAC));
  if(screen) screen = hdStaffRollBubbleS_c::construct(screen);
  mpBubbles[i] = screen;
  u32 flags = params->mFlags;
  void* target;
  if(flags&0x1000) target = params->mpInternalProjection;
  else { target = params->mpProjection; if(!target) target = &l_staffDefaultTarget; }
  void* camera;
  if((flags&0x40) && (flags&0x80)) camera = &params->mViewMatrix;
  else { camera = params->mpViewMatrix; if(!camera) camera = &l_staffDefaultCamera; }
  mpBubbles[i]->init(i*i+3, gabi::ea(target), gabi::ea(camera), NULL);
  s32 capacity = mDrawList.mCapacity;
  s32 count = mDrawList.mCount;
  hdParts_c* bubble = mpBubbles[i];
  if(count<capacity) { mDrawList.mpItems[count] = bubble; mDrawList.mCount = mDrawList.mCount+1; }
 }
 staffSortBubbles(&mDrawList, &l_staffBubbleSort);
}
/* 0271BC28 */
void hdStaffAmbientBubbles_c::start() {
 for(u32 i = 0; i<BUBBLE_COUNT; ++i) {
  hdPartsStateIdVtbl_c* waitVTable = static_cast<hdPartsStateIdVtbl_c*>(l_staffBubbleWait.mVtable.get());
  hdPartsStateId_c* current = hdParts_getCurrentStateId(mpBubbles[i]->stateMachine());
  u32 currentId = current->getKey();
  u32 waitId = gabi::call_ptr<u32>(waitVTable->mGetKey.get(), &l_staffBubbleWait);
  if(currentId == waitId) { hdParts_c* screen = mpBubbles[i]; gabi::call_ptr<void>(static_cast<hdStaffSceneBubbleVtable_l*>(screen->mVtable.get())->mRestart.get(), screen); }
 }
}
/* 0271BD0C */
void hdStaffAmbientBubbles_c::update() {
 for(u32 i = 0; i<BUBBLE_COUNT; ++i) {
  hdPartsStateIdVtbl_c* waitVTable = static_cast<hdPartsStateIdVtbl_c*>(l_staffBubbleWait.mVtable.get());
  hdParts_c* screen = mpBubbles[i]; gabi::call_ptr<void>(screen->mVtable->mExecuteState.get(), screen);
  hdPartsStateId_c* current = hdParts_getCurrentStateId(mpBubbles[i]->stateMachine());
  u32 currentId = current->getKey();
  u32 waitId = gabi::call_ptr<u32>(waitVTable->mGetKey.get(), &l_staffBubbleWait);
  if(currentId == waitId) { screen = mpBubbles[i]; gabi::call_ptr<void>(static_cast<hdStaffSceneBubbleVtable_l*>(screen->mVtable.get())->mRestart.get(), screen, 1u); }
 }
}
/* 0271BF60 */
void hdStaffCharacterBubbles_c::create() {
 hdRenderLayerRegistry_l* layers = l_staffLayers.mpInstance;
 hdLayerBase_c* params = static_cast<hdLayerBase_c*>(layers->mpLayers[layers->mLayerCount>3?3:0].get());
 for(u32 i = 0; i<BUBBLE_COUNT; ++i) {
  hdStaffRollBubble_c* screen = static_cast<hdStaffRollBubble_c*>(staffAllocate(0xC0));
  if(screen) screen = hdStaffRollBubble_c::construct(screen);
  mpBubbles[i] = screen;
  u32 flags = params->mFlags;
  void* target;
  if(flags&0x1000) target = params->mpInternalProjection;
  else { target = params->mpProjection; if(!target) target = &l_staffDefaultTarget; }
  void* camera;
  if((flags&0x40) && (flags&0x80)) camera = &params->mViewMatrix;
  else { camera = params->mpViewMatrix; if(!camera) camera = &l_staffDefaultCamera; }
  mpBubbles[i]->init(gabi::ea(target), gabi::ea(camera), NULL);
  s32 capacity = mDrawList.mCapacity;
  s32 count = mDrawList.mCount;
  hdParts_c* bubble = mpBubbles[i];
  if(count<capacity) { mDrawList.mpItems[count] = bubble; mDrawList.mCount = mDrawList.mCount+1; }
 }
 mCursor = 0; mFrame = 0;
}
static void staffResetBubbleParams(hdStaffRollBubbleMotion_c* params) {
 params->mVtable = &l_staffBubbleParamsVTable; params->mType = 0;
 params->mPos.set(0, 0, 0); params->mVelocity.set(0, 0, 0); params->mStartVelocity.set(0, 0, 0);
 params->mAccel = 0; params->mSway.set(0, 0, 0); params->mSwayAmplitude = 0;
 params->mSwayStep = 0; params->mSwayAngle = 0; params->mScale = 0; params->mFlags = 0;
}
/* 0271C0F8 */
void hdStaffCharacterBubbles_c::start() {
 hdStaffRollBubbleMotion_c params;
 for(u32 i = 0; i<BUBBLE_COUNT; ++i) {
  Record* record = &mRecords[mCursor<300?mCursor:0];
  if(record->mFrame>(s32)mFrame) continue;
  hdPartsStateId_c* current = hdParts_getCurrentStateId(mpBubbles[i]->stateMachine());
  hdPartsStateIdVtbl_c* waitVTable = static_cast<hdPartsStateIdVtbl_c*>(l_staffBubbleWait.mVtable.get());
  u32 currentId = current->getKey();
  u32 waitId = gabi::call_ptr<u32>(waitVTable->mGetKey.get(), &l_staffBubbleWait);
  if(currentId != waitId) continue;
  u32 cursor = mCursor;
  staffResetBubbleParams(&params);
  record = &mRecords[cursor<300?cursor:0];
  params.setKind(record->mId, record->mKind, record->mFrame, record->mX, record->mY);
  mpBubbles[i]->start(&params);
  u32 next = mCursor+1;
  s32 last = (s32)(mRecordCount-1);
  mCursor = next;
  if(last <= (s32)next) break;
 }
}
/* 0271C3C8 */
void hdStaffCharacterBubbles_c::update() {
 hdStaffRollBubbleMotion_c params;
 mFrame = mFrame+1;
 for(u32 i = 0; i<BUBBLE_COUNT; ++i) {
  // The dispatch keeps the current array-slot address in the second argument register.
  hdParts_c* screen = mpBubbles[i]; gabi::call_ptr<void>(screen->mVtable->mExecuteState.get(), screen, &mpBubbles[i]);
  if((s32)mRecordCount <= (s32)mCursor) continue;
  hdPartsStateId_c* current = hdParts_getCurrentStateId(mpBubbles[i]->stateMachine());
  hdPartsStateIdVtbl_c* waitVTable = static_cast<hdPartsStateIdVtbl_c*>(l_staffBubbleWait.mVtable.get());
  u32 currentId = current->getKey();
  u32 waitId = gabi::call_ptr<u32>(waitVTable->mGetKey.get(), &l_staffBubbleWait);
  if(currentId != waitId) continue;
  u32 cursor = mCursor;
  u32 frame = mFrame;
  Record* record = &mRecords[cursor<300?cursor:0];
  if(record->mFrame>(s32)frame) continue;
  staffResetBubbleParams(&params);
  params.setKind(record->mId, record->mKind, record->mFrame, record->mX, record->mY);
  mpBubbles[i]->start(&params);
  staffSortBubbles(&mDrawList, &l_staffBubbleSort);
  mCursor = mCursor+1;
 }
}
static void staffCopyRecord(hdStaffCharacterBubbles_c::Record* dst, hdStaffCharacterBubbles_c::Record* src) {
 dst->mId = src->mId; dst->mFrame = src->mFrame; dst->mXBits = src->mXBits; dst->mYBits = src->mYBits; dst->mKind = src->mKind;
}
/* 0271C7D8 */
void hdStaffCharacterBubbles_c::loadRecords() {
 SafeString group;
 SafeString archive;
 group.mStringTop = gabi::guest_addr("Misc"); group.__vtbl = gabi::ea(&l_staffSafeStringVTable);
 archive.mStringTop = gabi::guest_addr("Misc.bfres"); archive.__vtbl = gabi::ea(&l_staffSafeStringVTable);
 void* resource = staffGetResource(l_staffResourceMgr.mpInstance, &group, &archive, 0);
 if(!resource) return;
 hdStaffResource_l* view = staffResourceView(resource);
 s32 offset = view->mUserDataOffset;
 // Resource offsets are relative to their field or containing file in guest memory.
 void* userData = offset?gabi::at<void>(gabi::ea(&view->mUserDataOffset)+offset):NULL;
 hdStaffFile_l* file = staffFindFile(userData, "StaffRollBubble.dat");
 if(!file) return;
 s32 dataOffset = file->mDataOffset;
 Record* records = dataOffset?gabi::at<Record>(gabi::ea(file)+dataOffset):NULL;
 if(!records) return;
 u32 count = file->mSize/sizeof(Record);
 if(count>300) count = 300;
 mRecordCount = count;
 for(s32 i = 0; i<(s32)mRecordCount; ++i) staffCopyRecord(&mRecords[(u32)i<300?i:0], &records[i]);
 Record temp;
 for(s32 i = 0; i<(s32)mRecordCount; ++i) {
  for(s32 j = 1; j<(s32)mRecordCount; ++j) {
   if(j <= i) continue;
   Record* first = &mRecords[(u32)i<300?i:0];
   Record* next = &mRecords[(u32)j<300?j:0];
   if((u32)first->mFrame <= (u32)next->mFrame) continue;
   staffCopyRecord(&temp, first); staffCopyRecord(first, next); staffCopyRecord(next, &temp);
  }
 }
}
/* 0271BE50 */
void hdStaffAmbientBubbles_c::draw(void* context) {
 s32 count = mDrawList.mCount;
 for(s32 i = 0; i<count; ++i) {
  hdParts_c* screen = NULL;
  if((u32)i<(u32)count) screen = mDrawList.mpItems[i];
  gabi::call_ptr<void>(screen->mVtable->mDraw.get(), screen, context);
  count = mDrawList.mCount;
 }
}
/* 0271BEE4 */
void hdStaffAmbientBubbles_c::release() {
 for(u32 i = 0; i<BUBBLE_COUNT; ++i) {
  hdParts_c* screen = mpBubbles[i];
  gabi::call_ptr<void>(screen->mVtable->mFinalize.get(), screen);
 }
}
/* 0271C6C8 */
void hdStaffCharacterBubbles_c::draw(void* context) {
 s32 count = mDrawList.mCount;
 for(s32 i = 0; i<count; ++i) {
  hdParts_c* screen = NULL;
  if((u32)i<(u32)count) screen = mDrawList.mpItems[i];
  gabi::call_ptr<void>(screen->mVtable->mDraw.get(), screen, context);
  count = mDrawList.mCount;
 }
}
/* 0271C75C */
void hdStaffCharacterBubbles_c::release() {
 for(u32 i = 0; i<BUBBLE_COUNT; ++i) {
  hdParts_c* screen = mpBubbles[i];
  gabi::call_ptr<void>(screen->mVtable->mFinalize.get(), screen);
 }
}
/* 0271CE14 */
static void staffModelDestroy(hdStaffModel_l* model, u32 flags) {
 if(!model) return;
 model->mVTable = reinterpret_cast<gfn*>(&l_staffModelVTable);
 if(flags&1) staffDelete(model);
}
/* 0271D6D0 */
void hdStaffModel_l::release() {
 staffDestroyHeap(mpHeap); mpHeap = NULL; mpBtk = NULL; mpModel = NULL;
}
/* 0271D710 */
void hdStaffScene_c::unloadLayouts() {
 hdScreenString name;
 name.mText.v = gabi::guest_addr("StaffRoll_00"); name.mVtable = reinterpret_cast<hdScreenStringTable*>(&l_staffSafeStringVTable);
 l_staffLayoutMgr.mpInstance->unload(&name);
 name.mVtable = reinterpret_cast<hdScreenStringTable*>(&l_staffSafeStringVTable); name.mText.v = gabi::guest_addr("StaffRollBG_00");
 l_staffLayoutMgr.mpInstance->unload(&name);
 name.mVtable = reinterpret_cast<hdScreenStringTable*>(&l_staffSafeStringVTable); name.mText.v = gabi::guest_addr("StaffRollBubble_00");
 l_staffLayoutMgr.mpInstance->unload(&name);
 name.mVtable = reinterpret_cast<hdScreenStringTable*>(&l_staffSafeStringVTable); name.mText.v = gabi::guest_addr("StaffRollBubbleS_00");
 l_staffLayoutMgr.mpInstance->unload(&name);
 name.mVtable = reinterpret_cast<hdScreenStringTable*>(&l_staffSafeStringVTable); name.mText.v = gabi::guest_addr("Bubble_00");
 l_staffLayoutMgr.mpInstance->unload(&name);
}
/* 0271DC4C */
void hdStaffScene_c::execute() { staffStateExecute(&mState); }
/* 0271DC54 */
void hdStaffScene_c::releaseScreens() {
 hdParts_c* screen = mpStaff; gabi::call_ptr<void>(screen->mVtable->mFinalize.get(), screen);
 screen = mpBackground; gabi::call_ptr<void>(screen->mVtable->mFinalize.get(), screen);
 mAmbientBubbles.release(); mCharacterBubbles.release();
 screen = mpFade; gabi::call_ptr<void>(screen->mVtable->mFinalize.get(), screen);
}
/* 0271DCC8 */
void hdStaffScene_c::release() { releaseScreens(); unloadLayouts(); mModel.release(); }
/* 0271DD04 */
void hdStaffScene_c::drawStaff(void* context) {
 hdRenderLayerRegistry_l* layers = l_staffLayers.mpInstance;
 u32 index = static_cast<hdTitleRenderContext_l*>(context)->mLayerIndex;
 gptr<hdLayerBase_c>* items = layers->mpLayers;
 if(index<layers->mLayerCount) items = items+index;
 hdLayerBase_c* layer = static_cast<hdLayerBase_c*>(items->get());
 void* info = &layer->mViewport;
 mAmbientBubbles.draw(info); mCharacterBubbles.draw(info);
 hdParts_c* screen = mpStaff; gabi::call_ptr<void>(screen->mVtable->mDraw.get(), screen, info);
}
/* 0271DD90 */
void hdStaffScene_c::renderStaff(void* context) {
 staffSetViewport(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0); if(mDisplay) drawStaff(context);
}
/* 0271DDFC */
void hdStaffScene_c::renderBackground(void* context) {
 staffSetViewport(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
 if(!mDisplay) return;
 hdRenderLayerRegistry_l* layers = l_staffLayers.mpInstance;
 hdParts_c* screen = mpBackground;
 u32 count = layers->mLayerCount;
 u32 index = static_cast<hdTitleRenderContext_l*>(context)->mLayerIndex;
 hdPartsVtbl_c* vtable = screen->mVtable;
 gptr<hdLayerBase_c>* items = layers->mpLayers;
 if(index<count) items = items+index;
 hdLayerBase_c* layer = static_cast<hdLayerBase_c*>(items->get());
 gabi::call_ptr<void>(vtable->mDraw.get(), screen, &layer->mViewport);
}
/* 0271DE9C */
void hdStaffScene_c::renderFade(void* context) {
 staffSetViewport(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
 hdRenderLayerRegistry_l* layers = l_staffLayers.mpInstance;
 hdFadeWipeIris_c* screen = mpFade;
 u32 count = layers->mLayerCount;
 u32 index = static_cast<hdTitleRenderContext_l*>(context)->mLayerIndex;
 hdPartsVtbl_c* vtable = screen->mVtable;
 gptr<hdLayerBase_c>* items = layers->mpLayers;
 if(index<count) items = items+index;
 hdLayerBase_c* layer = static_cast<hdLayerBase_c*>(items->get());
 gabi::call_ptr<void>(vtable->mDraw.get(), screen, &layer->mViewport);
}
/* 0271DF30 */
void hdStaffScene_c::drawModel() { if(mDisplay) { staffModelUpdate(&mModel, mFrame); mModel.draw(); } }
/* 0271DF78 */
static void staffStartBgm() { staffSoundStart(3); }
/* 0271DF80 */
void hdStaffScene_c::loadBgmCalc() { if(staffSoundReady(3)) staffStateChange(&mState, &l_staffStartWait); }
/* 0271DFC8 */
void hdStaffScene_c::startWaitCalc() {
 if(l_staffTiming.mStatus == 1 && l_staffTiming.mFrame >= 0xAF0) staffStateChange(&mState, &l_staffFadeOutIn);
}
/* 0271DFF8 */
void hdStaffScene_c::fadeOutInEnter() { hdFadeWipeIris_c* screen = mpFade; mFadePhase = 0; mFadeAlpha = 1.0f; screen->setAmount(1.0f); }
/* 0271E018 */
static u32 staffHideProcess(hdStaffProcess_l* process) {
 if(staffIsActor(process)) { if(process) staffRemoveDrawTag(&process->mDrawTag); }
 else if(process && staffIsDrawable(process) && process) process->mNoDraw = 1;
 return 0;
}
/* 0271E090 */
static void staffHideProcesses() { staffSearch(&l_staffProcessCallback, 0); }
/* 0271E0A0 */
void hdStaffScene_c::fadeOutInCalc() {
 switch(mFadePhase) {
 case 0:
  if(staffChaseFade(&mFadeAlpha, 0.0f, 1.0f/30.0f)) {
   mFadePhase = mFadePhase+1; staffSetScreenDimEnabled(staffGetScreenDim(), 0); staffHideProcesses();
   hdParts_c* screen = mpBackground; mDisplay = 1; gabi::call_ptr<void>(screen->mVtable->mExecuteState.get(), screen);
  }
  break;
 case 1:
  if(staffChaseFade(&mFadeAlpha, 1.0f, 1.0f/30.0f)) mFadePhase = mFadePhase+1;
  break;
 case 2: staffStateChange(&mState, &l_staffCalc); break;
 }
 mpFade->setAmount(mFadeAlpha);
 hdFadeWipeIris_c* screen = mpFade; gabi::call_ptr<void>(screen->mVtable->mExecuteState.get(), screen);
}
/* 0271E1FC */
void hdStaffScene_c::calcEnter() { mFrame = 0; extern void staffSoundCalc(u32); staffSoundCalc(3); }
/* 0271E20C */
void hdStaffScene_c::calcScreens() {
 hdParts_c* screen = mpStaff; gabi::call_ptr<void>(screen->mVtable->mExecuteState.get(), screen);
 screen = mpBackground; gabi::call_ptr<void>(screen->mVtable->mExecuteState.get(), screen);
 staffUpdateAmbientBubbles(&mAmbientBubbles); staffUpdateCharacterBubbles(&mCharacterBubbles);
 screen = mpFade; gabi::call_ptr<void>(screen->mVtable->mExecuteState.get(), screen);
}
/* 0271E280 */
void hdStaffScene_c::calc() {
 calcScreens(); u32 frame = mFrame+1; mFrame = frame; staffModelUpdate(&mModel, frame);
 s32 end = mpBackground->getAllFrameCount();
 if(end <= (s32)mFrame) staffStateChange(&mState, &l_staffFinish);
}
/* 0271E2E4 */
void hdStaffScene_c::calcExit() { staffSetScreenDimEnabled(staffGetScreenDim(), 1); calcScreens(); }
/* 0271E320 */
static void staffFinishEnter(hdStaffScene_c* scene) { scene->calcScreens(); }
/* 0271E618 */
static void staffCreateInstance(void* heap) {
 if(l_staffScene.mpInstance) return;
 hdStaffScene_c* scene = staffAllocateInstance(sizeof(hdStaffScene_c), heap, 4);
 // Map embedded addresses after 32-bit guest wrapping; a wrapped member can be NULL.
 HdStateDisposer* disposer = gabi::at<HdStateDisposer>(gabi::ea(scene)+offsetof(hdStaffScene_c, mDisposer));
 if(disposer) { staffDisposerConstruct(disposer, heap, 3); disposer->mVTable = reinterpret_cast<gfn*>(&l_staffDisposerVTable); }
 l_staffDisposer.mpInstance = disposer;
 l_staffScene.mpInstance = scene?staffConstruct(scene):NULL;
}
/* 0271E6B8 */
static void staffDestroyInstance() {
 HdStateDisposer* disposer = l_staffDisposer.mpInstance;
 if(!disposer) return;
 l_staffDisposer.mpInstance = NULL;
 gabi::call_ptr<void>(disposer->mVTable[3].get(), disposer, 2u);
 hdStaffScene_c* scene = l_staffScene.mpInstance;
 if(scene) gabi::call_ptr<void>(scene->mVTable[3].get(), scene, 3u);
 l_staffScene.mpInstance = NULL;
}
/* 0271E7E8 */
static void staffDestroyDisposer(HdStateDisposer* disposer, u32 flags) {
 if(!disposer) return;
 disposer->mVTable = reinterpret_cast<gfn*>(&l_staffDisposerVTable);
 if(disposer == l_staffDisposer.mpInstance) {
  hdStaffScene_c* scene = l_staffScene.mpInstance; l_staffDisposer.mpInstance = NULL;
  gabi::call_ptr<void>(scene->mVTable[3].get(), scene, 2u); l_staffScene.mpInstance = NULL;
 }
 staffDisposerDestroy(disposer, 0); if(flags&1) staffDelete(disposer);
}
/* 0271CE34 */
static void staffModelLoad(hdStaffModel_l* model) {
 void* heap = staffCreateHeap(0x8000, 0x100);
 model->mpHeap = heap;
 void* control = l_staffResControl.mpInstance;
 SafeString room;
 SafeString stage;
 SafeString resource;
 room.mStringTop = gabi::guest_addr("Room0"); room.__vtbl = gabi::ea(&l_staffSafeStringVTable);
 stage.mStringTop = gabi::guest_addr("Hyrule"); stage.__vtbl = gabi::ea(&l_staffSafeStringVTable);
 resource.mStringTop = gabi::guest_addr("model2.bmd"); resource.__vtbl = gabi::ea(&l_staffSafeStringVTable);
 J3DModelData* data = staffGetModelResource(control, &room, &stage, &resource);
 model->mpModel = staffCreateModel(data, 0, 0x11000022);
 room.mStringTop = gabi::guest_addr("Room0"); room.__vtbl = gabi::ea(&l_staffSafeStringVTable);
 stage.mStringTop = gabi::guest_addr("Hyrule"); stage.__vtbl = gabi::ea(&l_staffSafeStringVTable);
 resource.mStringTop = gabi::guest_addr("model2.btk"); resource.__vtbl = gabi::ea(&l_staffSafeStringVTable);
 J3DAnmTextureSRTKey* key = reinterpret_cast<J3DAnmTextureSRTKey*>(staffGetModelResource(control, &room, &stage, &resource));
 if(key) {
  mDoExt_btkAnm* btk = staffBtkConstruct(NULL); model->mpBtk = btk;
  btk->init(data, key, true, 2, 1.0f, 0, -1, false, 0);
 }
 SafeString sky;
 SafeString materialName;
 for(s32 i = 0; i<staffMaterialTable(model->mpModel->mModelData)->mCount; ++i) {
  J3DModelData* modelData = model->mpModel->mModelData;
  hdStaffMaterial_l* material = reinterpret_cast<hdStaffMaterial_l*>(modelData->getMaterialNodePointer((u16)i));
  if(!material) continue;
  sky.mStringTop = gabi::guest_addr("sky_0_v_x"); sky.__vtbl = gabi::ea(&l_staffSafeStringVTable);
  hdStaffMaterialHeader_l* header = material->mpHeader;
  s32 offset = header->mNameOffset;
  materialName.mStringTop = offset?gabi::ea(&header->mNameOffset)+offset:0;
  materialName.__vtbl = gabi::ea(&l_staffSafeStringVTable);
  staffSafeStringValidate(&sky);
  gabi::call_ptr<void>(reinterpret_cast<hdStaffStringView_l*>(&sky)->mVTable->mAssureTermination.get(), &sky);
  be<u8>* skyTop = reinterpret_cast<hdStaffStringView_l*>(&sky)->mpText;
  gabi::call_ptr<void>(reinterpret_cast<hdStaffStringView_l*>(&materialName)->mVTable->mAssureTermination.get(), &materialName);
  be<u8>* materialTop = reinterpret_cast<hdStaffStringView_l*>(&materialName)->mpText;
  bool same = skyTop == materialTop;
  if(!same) {
   be<u8>* first = skyTop;
   be<u8>* next = materialTop;
   for(u32 n = 0; n<0x40001; ++n) {
    u8 a = first[n]; u8 b = next[n];
    if(a != b) break;
    if(a == 0) { same = true; break; }
   }
  }
  if(!same) material->mpShape->mVisible = 0;
 }
 model->mPosition.set(model->mFirstPosition);
 model->mScale.set(model->mFirstScale);
 const f32 angleScale = 182.04444885253906f;
 model->mRotation.x = (s16)gabi::ftoi(gabi::fmuls_ppc(model->mFirstRotation.x, angleScale));
 model->mRotation.y = (s16)gabi::ftoi(gabi::fmuls_ppc(model->mFirstRotation.y, angleScale));
 model->mRotation.z = (s16)gabi::ftoi(gabi::fmuls_ppc(model->mFirstRotation.z, angleScale));
 staffHeapComplete(model->mpHeap);
}
/* 0271D7D4 */
void hdStaffScene_c::createScreens() {
 hdRenderLayerRegistry_l* layers = l_staffLayers.mpInstance;
 hdLayerBase_c* params = static_cast<hdLayerBase_c*>(layers->mpLayers[layers->mLayerCount>3?3:0].get());
 hdScreenStaffRoll_c* screen = static_cast<hdScreenStaffRoll_c*>(staffAllocate(0xB8));
 if(screen) screen = hdScreenStaffRoll_c::construct(screen);
 mpStaff = screen;
 u32 flags = params->mFlags;
 void* target = (flags&0x1000)?params->mpInternalProjection.get():params->mpProjection.get();
 if(!(flags&0x1000) && !target) target = &l_staffDefaultTarget;
 void* camera;
 if((flags&0x40) && (flags&0x80)) camera = &params->mViewMatrix;
 else { camera = params->mpViewMatrix; if(!camera) camera = &l_staffDefaultCamera; }
 screen->init(gabi::ea(target), gabi::ea(camera), NULL);
 hdScreenStaffRollBG_c* background = static_cast<hdScreenStaffRollBG_c*>(staffAllocate(0x50));
 if(background) background = hdScreenStaffRollBG_c::construct(background);
 mpBackground = background;
 flags = params->mFlags;
 target = (flags&0x1000)?params->mpInternalProjection.get():params->mpProjection.get();
 if(!(flags&0x1000) && !target) target = &l_staffDefaultTarget;
 if((flags&0x40) && (flags&0x80)) camera = &params->mViewMatrix;
 else { camera = params->mpViewMatrix; if(!camera) camera = &l_staffDefaultCamera; }
 background->init(gabi::ea(target), gabi::ea(camera), NULL);
 hdFadeWipeIris_c* fade = static_cast<hdFadeWipeIris_c*>(staffAllocate(sizeof(hdFadeWipeIris_c)));
 if(fade) fade = hdFadeWipeIris_c::construct(fade);
 mpFade = fade;
 flags = params->mFlags;
 target = (flags&0x1000)?params->mpInternalProjection.get():params->mpProjection.get();
 if(!(flags&0x1000) && !target) target = &l_staffDefaultTarget;
 if((flags&0x40) && (flags&0x80)) camera = &params->mViewMatrix;
 else { camera = params->mpViewMatrix; if(!camera) camera = &l_staffDefaultCamera; }
 fade->setup(gabi::ea(target), gabi::ea(camera), NULL);
}
static void staffSetTaskDelegate(hdStaffRenderTask_l* task, hdStaffScene_c* owner, void* callback) {
 HdStateDelegate* delegate = gabi::at<HdStateDelegate>(gabi::ea(task)+offsetof(hdStaffRenderTask_l, mDelegate));
 if(delegate) {
  delegate->mVTable = reinterpret_cast<gfn*>(&l_staffRenderDelegateVTable); delegate->mObject = owner;
  delegate->mVirtualOffset = 0xFFFF; delegate->mFunction = callback; delegate->mThisAdjustment = 0;
 }
}
/* 0271D9FC */
void hdStaffScene_c::enter() {
 l_staffGameState.mpInstance->mSceneMode = 2;
 staffSetTaskDelegate(&mStaffTask, this, &l_staffRenderCallback);
 mStaffTask.mName.mStringTop = gabi::ea(&l_staffTaskName); mStaffTask.mEnabled = 1;
 hdRenderLayerRegistry_l* layers = l_staffLayers.mpInstance;
 void* layer = layers->mpLayers[layers->mLayerCount>3?3:0];
 staffRegisterTask(layer, 3, &mStaffTask);
 staffSetTaskDelegate(&mBackgroundTask, this, &l_staffBackgroundCallback);
 mBackgroundTask.mEnabled = 1; mBackgroundTask.mName.mStringTop = gabi::ea(&l_staffBackgroundTaskName);
 layers = l_staffLayers.mpInstance; layer = layers->mpLayers[layers->mLayerCount>3?3:0];
 staffRegisterTask(layer, 0, &mBackgroundTask);
 staffSetTaskDelegate(&mFadeTask, this, &l_staffFadeCallback);
 mFadeTask.mEnabled = 1; mFadeTask.mName.mStringTop = gabi::ea(&l_staffFadeTaskName);
 layers = l_staffLayers.mpInstance; layer = layers->mpLayers[layers->mLayerCount>5?5:0];
 staffRegisterTask(layer, 0, &mFadeTask);
 createScreens();
 hdFadeWipeIris_c* screen = mpFade; mFadeAlpha = 1.0f; screen->setAmount(1.0f);
 screen = mpFade; gabi::call_ptr<void>(screen->mVtable->mExecuteState.get(), screen);
 mCharacterBubbles.loadRecords(); mCharacterBubbles.create(); mCharacterBubbles.start(); mAmbientBubbles.create(); mAmbientBubbles.start();
 mpBackground->changeState(&l_staffBackgroundStart);
 staffModelLoad(&mModel);
 staffInputMode(l_staffInput.mpInstance, 3); staffSoundMode(2);
 HdStateVTableObject* factory = mState.mFactory;
 HdStateVTableObject* state = gabi::call_ptr<HdStateVTableObject*>(factory->mVTable[5].get(), factory, &l_staffInitialState);
 mState.mCurrent = state;
 gabi::call_ptr<void>(state->mVTable[7].get(), state);
}
// The original single-precision multiply-add propagates its addend's NaN payload
// when both the interpolation delta and base are NaNs; retain that operand order.
static f32 staffInterpolate(f32 next, f32 first, f32 amount) {
 f32 delta = gabi::fsubs_ppc(next, first);
 if(first != first) return gabi::ppc_qnan(first);
 return gabi::fmadds(delta, amount, first);
}
/* 0271D104 */
static void staffModelUpdate(hdStaffModel_l* model, u32 frame) {
 cXyz firstPos;
 cXyz nextPos;
 cXyz firstScale;
 cXyz nextScale;
 cXyz firstRotation;
 cXyz nextRotation;
 f32 t;
 s32 second = model->mSecondFrame;
 if((s32)frame>second) {
  s32 third = model->mThirdFrame;
  if(second >= third) { third = (s32)((u32)second+1); model->mThirdFrame = third; second = model->mSecondFrame; }
  t = (f32)(s32)(frame-(u32)second)/(f32)(s32)((u32)third-(u32)second);
  if(t<0.0f) t = 0.0f; else if(t>1.0f) t = 1.0f;
  firstPos = model->mSecondPosition; nextPos = model->mThirdPosition;
  firstScale = model->mSecondScale; nextScale = model->mThirdScale;
  firstRotation = model->mSecondRotation; nextRotation = model->mThirdRotation;
 } else {
  s32 first = model->mFirstFrame;
  if((s32)frame>first) {
   if(first >= second) { second = (s32)((u32)first+1); model->mSecondFrame = second; first = model->mFirstFrame; }
   t = (f32)(s32)(frame-(u32)first)/(f32)(s32)((u32)second-(u32)first);
   if(t<0.0f) t = 0.0f; else if(t>1.0f) t = 1.0f;
   firstPos = model->mFirstPosition; nextPos = model->mSecondPosition;
   firstScale = model->mFirstScale; nextScale = model->mSecondScale;
   firstRotation = model->mFirstRotation; nextRotation = model->mSecondRotation;
  } else {
   t = 0;
   firstPos = model->mFirstPosition; nextPos = model->mFirstPosition;
   firstScale = model->mFirstScale; nextScale = model->mFirstScale;
   firstRotation = model->mFirstRotation; nextRotation = model->mFirstRotation;
  }
 }
 model->mPosition.x = staffInterpolate(nextPos.x, firstPos.x, t);
 model->mPosition.y = staffInterpolate(nextPos.y, firstPos.y, t);
 model->mPosition.z = staffInterpolate(nextPos.z, firstPos.z, t);
 model->mScale.x = staffInterpolate(nextScale.x, firstScale.x, t);
 model->mScale.y = staffInterpolate(nextScale.y, firstScale.y, t);
 model->mScale.z = staffInterpolate(nextScale.z, firstScale.z, t);
 const f32 angleScale = 182.04444885253906f;
 model->mRotation.x = (s16)gabi::ftoi(gabi::fmuls_ppc(staffInterpolate(nextRotation.x, firstRotation.x, t), angleScale));
 model->mRotation.y = (s16)gabi::ftoi(gabi::fmuls_ppc(staffInterpolate(nextRotation.y, firstRotation.y, t), angleScale));
 model->mRotation.z = (s16)gabi::ftoi(gabi::fmuls_ppc(staffInterpolate(nextRotation.z, firstRotation.z, t), angleScale));
 J3DModel* j3d = model->mpModel;
 j3d->mBaseScale = model->mScale;
 staffMatrixTranslate(&l_staffMatrix, model->mPosition.x, model->mPosition.y, model->mPosition.z);
 staffMatrixRotate(&l_staffMatrix, (s16)model->mRotation.x, (s16)model->mRotation.y, (s16)model->mRotation.z);
 model->mpModel->setBaseTRMtx(&l_staffMatrix);
 model->mpBtk->play();
}
/* 0271D630 */
void hdStaffModel_l::draw() {
 J3DModel* model = mpModel;
 mDoExt_btkAnm* btk = mpBtk;
 J3DModelData* data = model->mModelData;
 f32 frame = btk->mFrameCtrl.mFrame;
 btk->entry(data, frame);
 l_staffDrawState.mList = staffGetPlay()->mStaffDrawList;
 l_staffDrawState.mBuffer = staffGetPlay()->mStaffDrawList;
 hdRenderLayerRegistry_l* layers = l_staffLayers.mpInstance;
 void* layer = layers->mpLayers[layers->mLayerCount>3?3:0];
 staffModelUpdateDL(mpModel, layer);
 l_staffDrawState.mList = staffGetPlay()->mDrawList;
 l_staffDrawState.mBuffer = staffGetPlay()->mDrawBuffer;
}
// In-place vector constructors allocate when their guest member address wraps to zero.
static void staffConstructVector(cXyz* vector, f32 x, f32 y, f32 z) {
 if(gabi::ea(vector) == 0) vector = static_cast<cXyz*>(staffAllocate(sizeof(cXyz)));
 if(vector) vector->set(x, y, z);
}
static void staffConstructRotation(hdStaffModel_l::Rotation* rotation) {
 if(gabi::ea(rotation) == 0) rotation = static_cast<hdStaffModel_l::Rotation*>(staffAllocate(sizeof(hdStaffModel_l::Rotation)));
 if(rotation) { rotation->x = 0; rotation->y = 0; rotation->z = 0; }
}
/* 0271CB64 */
static hdStaffModel_l* staffModelConstruct(hdStaffModel_l* model) {
 if(!model) { model = static_cast<hdStaffModel_l*>(staffAllocate(0xAC)); if(!model) return NULL; }
 model->mpHeap = NULL; model->mVTable = reinterpret_cast<gfn*>(&l_staffModelVTable); model->mpBtk = NULL; model->mpModel = NULL;
 staffConstructVector(&model->mPosition, 0, 0, 0); staffConstructVector(&model->mScale, 0.002f, 0.002f, 0.002f);
 staffConstructRotation(&model->mRotation);
 model->mFirstFrame = 7000; staffConstructVector(&model->mFirstPosition, 550, 350, 0); staffConstructVector(&model->mFirstScale, 0.001f, 0.001f, 0.001f); staffConstructVector(&model->mFirstRotation, -25, 0, 0);
 model->mSecondFrame = 9300; staffConstructVector(&model->mSecondPosition, 0, -650, 0); staffConstructVector(&model->mSecondScale, 0.005f, 0.005f, 0.005f); staffConstructVector(&model->mSecondRotation, -15, 0, 0);
 model->mThirdFrame = 9700; staffConstructVector(&model->mThirdPosition, 0, -1100, 0); staffConstructVector(&model->mThirdScale, 0.005f, 0.005f, 0.005f); staffConstructVector(&model->mThirdRotation, -2, 0, 0);
 return model;
}
/* 0271E738 */
static void staffDestroy(hdStaffScene_c* scene, u32 flags) {
 if(!scene) return;
 staffModelDestroy(&scene->mModel, 2);
 staffTaskDestroy(&scene->mFadeTask, 2); staffTaskDestroy(&scene->mBackgroundTask, 2); staffTaskDestroy(&scene->mStaffTask, 2);
 HdStateVTableObject* factory = scene->mState.mFactory;
 gabi::call_ptr<void>(factory->mVTable[7].get(), factory, &scene->mState.mNext);
 factory = scene->mState.mFactory;
 gabi::call_ptr<void>(factory->mVTable[7].get(), factory, &scene->mState.mCurrent);
 if(flags&1) staffDelete(scene);
}
static void staffConstructTask(hdStaffRenderTask_l* task) {
 staffTaskConstruct(task);
 SafeString* name = gabi::at<SafeString>(gabi::ea(task)+offsetof(hdStaffRenderTask_l, mName));
 if(!name) name = static_cast<SafeString*>(staffAllocate(8));
 if(name) { name->mStringTop = gabi::ea(&l_staffEmptyName); name->__vtbl = gabi::ea(&l_staffSafeStringVTable); }
 task->mEnabled = 0; task->mVTable = reinterpret_cast<gfn*>(&l_staffTaskVTable); task->mReserved = 0;
 HdStateDelegate* delegate = gabi::at<HdStateDelegate>(gabi::ea(task)+offsetof(hdStaffRenderTask_l, mDelegate));
 if(delegate) {
  HdStateDelegate* base = delegate;
  if(!base) base = static_cast<HdStateDelegate*>(staffAllocate(sizeof(HdStateDelegate)));
  if(base) { base->mFunction = NULL; base->mVTable = reinterpret_cast<gfn*>(&l_staffDelegateBaseVTable); base->mThisAdjustment = 0; base->mVirtualOffset = 0; base->mObject = NULL; }
  delegate->mVTable = reinterpret_cast<gfn*>(&l_staffDelegateEmptyVTable);
 }
}
/* 0271E324 */
static hdStaffScene_c* staffConstruct(hdStaffScene_c* scene) {
 if(!scene) { scene = static_cast<hdStaffScene_c*>(staffAllocate(sizeof(hdStaffScene_c))); if(!scene) return NULL; }
 hdStaffScene_c* base = scene;
 if(!base) base = static_cast<hdStaffScene_c*>(staffAllocate(0x28));
 if(base) {
  base->mpSelf = scene; base->mReserved = 0;
  base->mObjectVTable = reinterpret_cast<gfn*>(&l_staffObjectVTable);
  base->mBaseVTable = reinterpret_cast<gfn*>(&l_staffBaseVTable);
  base->mVTable = reinterpret_cast<gfn*>(&l_staffBaseTaskVTable);
  staffConstructState(&base->mState, base, &l_staffFactory);
 }
 scene->mVTable = reinterpret_cast<gfn*>(&l_staffSceneVTable);
 staffConstructTask(&scene->mStaffTask); staffConstructTask(&scene->mBackgroundTask); staffConstructTask(&scene->mFadeTask);
 scene->mpBackground = NULL; scene->mpFade = NULL; scene->mpStaff = NULL;
 if((u32)(gabi::ea(scene)+offsetof(hdStaffScene_c, mAmbientBubbles)) == 0) staffAllocate(0x190);
 hdStaffBubbleList_l* list = gabi::at<hdStaffBubbleList_l>(gabi::ea(scene)+offsetof(hdStaffScene_c, mAmbientBubbles)+offsetof(hdStaffAmbientBubbles_c, mDrawList));
 if(!list) list = static_cast<hdStaffBubbleList_l*>(staffAllocate(0x19C));
 if(list) { list->mCount = 0; list->mCapacity = 0; list->mpItems = NULL; staffReserveList(list, 100, reinterpret_cast<gptr<hdParts_c>*>(list+1)); }
 if((u32)(gabi::ea(scene)+offsetof(hdStaffScene_c, mCharacterBubbles)) == 0) staffAllocate(0x78);
 list = gabi::at<hdStaffBubbleList_l>(gabi::ea(scene)+offsetof(hdStaffScene_c, mCharacterBubbles)+offsetof(hdStaffCharacterBubbles_c, mDrawList));
 if(!list) list = static_cast<hdStaffBubbleList_l*>(staffAllocate(0x84));
 if(list) { list->mCount = 0; list->mCapacity = 0; list->mpItems = NULL; staffReserveList(list, 30, reinterpret_cast<gptr<hdParts_c>*>(list+1)); }
 scene->mCharacterBubbles.mCursor = 0; scene->mCharacterBubbles.mFrame = 0;
 if((u32)(gabi::ea(scene)+offsetof(hdStaffScene_c, mCharacterBubbles)+offsetof(hdStaffCharacterBubbles_c, mRecords)) == 0) staffAllocate(0x1770);
 scene->mDisplay = 0; scene->mCharacterBubbles.mRecordCount = 0; scene->mFrame = 0; scene->mFadePhase = 0; scene->mFadeAlpha = 1.0f;
 staffModelConstruct(&scene->mModel);
 return scene;
}
static void staffInitState(hdSaveStateId_l* state, u32 id, const char* name, void* enter, void* calc, void* exit, void* query) {
 state->mId = id; state->mName.v = gabi::guest_addr(name); state->mVTable = reinterpret_cast<gfn*>(&l_staffStateVTable);
 state->mEnterAdjustment = 0; state->mEnterVirtual = 0xFFFF; state->mEnter = enter;
 state->mCalcAdjustment = 0; state->mCalcVirtual = 0xFFFF; state->mCalc = calc;
 state->mExitAdjustment = 0; state->mExitVirtual = 0xFFFF; state->mExit = exit;
 state->mReserved = 0; state->mQueryVirtual = 0xFFFF; state->mQuery = query;
}
/* 0271E890 */
static void staffStaticInit() {
 l_staffStatics.mWords[3] = 0; l_staffStatics.mWords[2] = 0; l_staffStatics.mWords[1] = 0; l_staffStatics.mWords[0] = 0;
 staffRegisterGlobal(&l_staffRegistration1);
 f32 lower = l_staffLower; f32 upper = l_staffUpper;
 l_staffUiRanges.mLower = lower; l_staffUiRanges.mUpper = upper;
 staffInitSigned(&l_staffUiRanges.mSigned); staffRegisterGlobal(&l_staffRegistration2);
 staffInitUnsigned(&l_staffUiRanges.mUnsigned); staffRegisterGlobal(&l_staffRegistration3);
 l_staffUiRanges.mMinX = -940; l_staffUiRanges.mTop = 150;
 l_staffUiRanges.mWidth = 1280; l_staffUiRanges.mHeight = 720;
 u32 initialized = l_staffStateInitialized;
 l_staffUiRanges.mMinY = -790; l_staffUiRanges.mDepth = 510; l_staffUiRanges.mMaxY = 510;
 u32 id = 0;
 if(initialized) id = l_staffLastStateId; else l_staffStateInitialized = 1;
 staffInitState(&l_staffInitialState, id+1, "::StateID_LoadBgm", &l_staffLoadBgmEnter, &l_staffLoadBgmCalc, &l_staffLoadBgmExit, &l_staffLoadBgmQuery);
 staffInitState(&l_staffStartWait, id+2, "::StateID_StartWait", &l_staffStartWaitEnter, &l_staffStartWaitCalc, &l_staffStartWaitExit, &l_staffStartWaitQuery);
 staffInitState(&l_staffFadeOutIn, id+3, "::StateID_FadeOutIn", &l_staffFadeOutInEnter, &l_staffFadeOutInCalc, &l_staffFadeOutInExit, &l_staffFadeOutInQuery);
 staffInitState(&l_staffCalc, id+4, "::StateID_Calc", &l_staffCalcEnter, &l_staffCalcCalc, &l_staffCalcExit, &l_staffCalcQuery);
 staffInitState(&l_staffFinish, id+5, "::StateID_End", &l_staffEndEnter, &l_staffEndCalc, &l_staffEndExit, &l_staffEndQuery);
 l_staffLastStateId = id+5;
}
/* WWHD glue
data l_staffBaseVTable 10140428
data l_staffObjectVTable 101403E8
data l_staffBaseTaskVTable 101403B0
data l_staffTaskVTable 10169E3C
data l_staffSceneVTable 10140668
data l_staffFactory 101FF32C
data l_staffDelegateBaseVTable 10140348
data l_staffDelegateEmptyVTable 10140398
data l_staffEmptyName 101405E4
data l_staffStatics 1049F5D4
data l_staffUiRanges 1049F5AC
data l_staffRegistration1 101F83AC
data l_staffRegistration2 101F83B8
data l_staffRegistration3 101F83C4
data l_staffLower 101405E8
data l_staffUpper 101405EC
data l_staffStateInitialized 101FD8E4
data l_staffLastStateId 101FDD50
data l_staffStateVTable 101404A0
str 10140630 "::StateID_LoadBgm"
str 10140608 "::StateID_StartWait"
str 1014061C "::StateID_FadeOutIn"
str 10140654 "::StateID_Calc"
str 10140644 "::StateID_End"
ext 02752A84 void staffTaskConstruct(hdStaffRenderTask_l*)
ext 020062E8 void staffConstructState(HdStateMachine*,hdStaffScene_c*,void*)
ext 0273B560 void staffReserveList(hdStaffBubbleList_l*,u32,gptr<hdParts_c>*)
ext 028F026C void staffRegisterGlobal(void*)
ext 028ED6F8 void staffInitSigned(be<u8>*)
ext 028EAB2C void staffInitUnsigned(be<u8>*)
data l_staffLoadBgmEnter 0271DF78
data l_staffLoadBgmCalc 0271DF80
data l_staffLoadBgmExit 0271ECB4
data l_staffLoadBgmQuery 0271ECB8
data l_staffStartWaitEnter 0271ECC0
data l_staffStartWaitCalc 0271DFC8
data l_staffStartWaitExit 0271ECC4
data l_staffStartWaitQuery 0271ECC8
data l_staffFadeOutInEnter 0271DFF8
data l_staffFadeOutInCalc 0271E0A0
data l_staffFadeOutInExit 0271ECD0
data l_staffFadeOutInQuery 0271ECD4
data l_staffCalcEnter 0271E1FC
data l_staffCalcCalc 0271E280
data l_staffCalcExit 0271ECDC
data l_staffCalcQuery 0271ECE0
data l_staffEndEnter 0271E2E4
data l_staffEndCalc 0271E320
data l_staffEndExit 0271ECE8
data l_staffEndQuery 0271ECEC

data l_staffGameState 101F86E8
data l_staffInput 101F5088
data l_staffBackgroundStart 1049C248
data l_staffInitialState 1049F5E4
data l_staffRenderDelegateVTable 10140448
data l_staffRenderCallback 0271DD90
data l_staffBackgroundCallback 0271DDFC
data l_staffFadeCallback 0271DE9C
data l_staffTaskName 101405CC
data l_staffBackgroundTaskName 101405D0
data l_staffFadeTaskName 101405D8
data l_staffResControl 101F4F28
data l_staffMatrix 1048D0CC
data l_staffResourceMgr 101F4F7C
data l_staffBubbleParamsVTable 101403C0
data l_staffBubbleWait 1049C36C
data l_staffBubbleSort 0271BA0C
data l_staffDefaultTarget 104A20FC
data l_staffDefaultCamera 104A2098
data l_staffDrawState 104B4634
data l_staffLayers 101F95D0
data l_staffLayoutMgr 101F7274
data l_staffSafeStringVTable 10140360
data l_staffModelVTable 101403D8
data l_staffDisposerVTable 10140678
data l_staffScene 101F83D0
data l_staffDisposer 101F83D4
data l_staffStartWait 1049F610
data l_staffFadeOutIn 1049F63C
data l_staffCalc 1049F668
data l_staffFinish 1049F694
data l_staffTiming 101D600C
data l_staffProcessCallback 0271E018
str 10140598 "StaffRoll_00"
str 101405A8 "StaffRollBG_00"
str 101405B8 "StaffRollBubble_00"
str 10140578 "StaffRollBubbleS_00"
str 1014058C "Bubble_00"
str 101404E8 "Misc"
str 10140504 "Misc.bfres"
str 101404F0 "StaffRollBubble.dat"
ext 026124B0 void* staffGetResource(void*,SafeString*,SafeString*,u32)
ext 027E2DC0 hdStaffResource_l* staffResourceView(void*)
ext 027DFA24 hdStaffFile_l* staffFindFile(void*,const char*)
ext 0273B6AC void staffSortBubbles(hdStaffBubbleList_l*,void*)
ext 0273AD10 void* staffAllocate(u32)
ext 02006364 void staffStateExecute(HdStateMachine*)
ext 020063C0 void staffStateChange(HdStateMachine*,hdSaveStateId_l*)
ext 026FBA38 u32 hdLayoutResMgr_c::unload(hdScreenString*)
ext 025E3868 void staffDestroyHeap(void*)
ext C0006988 void staffSetViewport(u32,u32,u32,u32,u32,u32,u32)
str 1014053C "Room0"
str 10140544 "Hyrule"
str 10140558 "model2.bmd"
str 10140564 "model2.btk"
str 1014054C "sky_0_v_x"
ext 02669D08 hdFadeWipeIris_c* hdFadeWipeIris_c::construct(hdFadeWipeIris_c*)
ext 02669D84 u32 hdFadeWipeIris_c::setup(u32,u32,void*)
ext 027C23F8 void staffRegisterTask(void*,u32,hdStaffRenderTask_l*)
ext 02618094 void staffInputMode(void*,u32)
ext 020309AC void staffSoundMode(u32)
ext 025E3630 void* staffCreateHeap(u32,u32)
ext 025E3834 void staffHeapComplete(void*)
ext 02606BEC J3DModelData* staffGetModelResource(void*,SafeString*,SafeString*,SafeString*)
ext 025E38E0 J3DModel* staffCreateModel(J3DModelData*,u32,u32)
ext 025E7C6C mDoExt_btkAnm* staffBtkConstruct(mDoExt_btkAnm*)
ext 027F3F8C hdStaffMaterialTable_l* staffMaterialTable(J3DModelData*)
ext 0271EC08 void staffSafeStringValidate(SafeString*)
ext 028E93CC void staffMatrixTranslate(Mtx34*,f32,f32,f32)
ext 025F1B48 void staffMatrixRotate(Mtx34*,s16,s16,s16)
ext 02030E90 void staffSoundStart(u32)
ext 02030F1C u32 staffSoundReady(u32)
ext 02032C6C void* staffGetScreenDim()
ext 02032AD8 void staffSetScreenDimEnabled(void*,u32)
ext 0266A064 void hdFadeWipeIris_c::setAmount(f32)
ext 0268B070 u32 staffChaseFade(be<f32>*,f32,f32)
ext 025D4604 u32 staffIsActor(hdStaffProcess_l*)
ext 025DAC54 u32 staffIsDrawable(hdStaffProcess_l*)
ext 025DA884 void staffRemoveDrawTag(void*)
ext 025DE508 void* staffSearch(void*,u32)
ext 0273AF40 void staffDelete(void*)
ext 027C0804 void staffTaskDestroy(void*,u32)
ext 02752BEC void staffDisposerDestroy(HdStateDisposer*,u32)
ext 0273B0D4 hdStaffScene_c* staffAllocateInstance(u32,void*,u32)
ext 02752B0C void staffDisposerConstruct(HdStateDisposer*,void*,u32)
ext 02030FA8 void staffSoundCalc(u32)
ext 025200D4 hdStaffPlay_l* staffGetPlay()
ext 025E2DE0 void staffModelUpdateDL(J3DModel*,void*)
ext 0271BD0C void staffUpdateAmbientBubbles(hdStaffAmbientBubbles_c*)
ext 0271C3C8 void staffUpdateCharacterBubbles(hdStaffCharacterBubbles_c*)
ext 026E352C void hdStaffRollBubbleMotion_c::setKind(u32,u32,u32,f32,f32)
ext 026E306C void hdStaffRollBubble_c::start(hdStaffRollBubbleMotion_c*)
ext 026E46F4 hdStaffRollBubbleS_c* hdStaffRollBubbleS_c::construct(hdStaffRollBubbleS_c*)
ext 026E2B70 hdStaffRollBubble_c* hdStaffRollBubble_c::construct(hdStaffRollBubble_c*)
ext 026E4778 u32 hdStaffRollBubbleS_c::init(u32,u32,u32,void*)
ext 026E2C00 u32 hdStaffRollBubble_c::init(u32,u32,void*)
ext 026E071C hdScreenStaffRoll_c* hdScreenStaffRoll_c::construct(hdScreenStaffRoll_c*)
ext 026E2404 hdScreenStaffRollBG_c* hdScreenStaffRollBG_c::construct(hdScreenStaffRollBG_c*)
ext 026E07D0 u32 hdScreenStaffRoll_c::init(u32,u32,void*)
ext 026E2470 u32 hdScreenStaffRollBG_c::init(u32,u32,void*)
ext 026E26A0 s32 hdScreenStaffRollBG_c::getAllFrameCount()
ext 026E3980 f32 hdStaffRollBubbleMotion_c::getScale()
*/
