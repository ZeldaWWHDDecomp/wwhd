/* HD Title_00 scene controller. Derived from the game binary; */
#include "d/hd_scene_title.h"
extern void* titleAllocate(u32);
extern hdTitleScene_c* titleAllocateInstance(u32, void*, u32);
extern void titleDelete(void*);
extern void titleDisposerConstruct(HdStateDisposer*, void*, u32);
extern void titleDisposerDestroy(HdStateDisposer*, u32);
extern void titleTaskConstruct(hdTitleTask_l*);
extern void titleTaskDestroy(hdTitleTask_l*, u32);
extern void titleTaskRegister(void*, u32, hdTitleTask_l*);
extern void* titleGetScreenDim();
extern void titleSetScreenDimEnabled(void*, u32);
extern void titleInputMode(void*, u32);
extern void titleSoundReset();
extern void titleSoundMode(u32);
extern void titleSetViewport(u32, u32, u32, u32, u32, u32, u32);
extern void* titleScreenPosition(hdTitleLogo_c*);
extern f32 titleScreenScaleX(hdTitleLogo_c*);
extern f32 titleScreenScaleY(hdTitleLogo_c*);
extern void* titleScreenCameraPosition(hdTitleLogo_c*);
extern f32 titleScreenCameraNear(hdTitleLogo_c*);
extern f32 titleScreenCameraFar(hdTitleLogo_c*);
extern void* titleScreenLightPosition(hdTitleLogo_c*);
extern f32 titleScreenLightScaleX(hdTitleLogo_c*);
extern f32 titleScreenLightScaleY(hdTitleLogo_c*);
extern u32 titleScreenColor(hdTitleLogo_c*);
extern f32 titleScreenDepth(hdTitleLogo_c*);
extern u32 titleScreenMode(hdTitleLogo_c*);
extern u32 titleScreenFlags(hdTitleLogo_c*);
static hdTitleSingleton_l l_titleTitle;
static hdTitleDisposerSingleton_l l_titleTitleDisposer;
static hdTitleLayerSingleton_l l_titleRenderLayers;
static hdTitleGameSingleton_l l_titleGameState;
static hdSceneLayoutManagerSingleton_l l_titleLayoutMgr;
static hdTitleManagerSingleton_l l_titleInputMgr;
static u32 l_titleRenderCallback;
static u32 l_titleTaskVTable;
static u32 l_titleDelegateVTable;
static u32 l_titleEmptyDelegateVTable;
static u32 l_titleBaseDelegateVTable;
static u32 l_titleSafeStringVTable;
static u32 l_titleDisposerVTable;
static u8 l_titleDefaultTarget;
static u8 l_titleDefaultCamera;
static u8 l_titleEmptyString;
static u8 l_titleTaskName;
static HdStateStatics l_titleStatics;
static HdStateRange l_titleRange;
static f32 l_titleLower;
static f32 l_titleUpper;
static HdStateRegistration l_titleRegistration1;
static HdStateRegistration l_titleRegistration2;
static HdStateRegistration l_titleRegistration3;
extern void titleRegisterGlobal(void*);
extern void titleInitSigned(be<u8>*);
extern void titleInitUnsigned(be<u8>*);

/* 0271ECB8 */
static u32 titleCanEnter() { return 0; }
/* 0271ECC8 */
static u32 titleCanLeave() { return 0; }
/* 0271ECD4 */
static u32 titleCanPause() { return 0; }
/* 0271ECE0 */
static u32 titleCanResume() { return 0; }
/* 0271ECEC */
static u32 titleHasPendingEvent() { return 0; }
/* 0271ECF4 */
static u32 titleStateId(HdStateId* state) { return state->mId; }
/* 0271EDC8 */
static void* titleStateSetOwner(hdTitleState_l* state, void* owner) {
 state->mpNext = owner;
 return &state->mOwner;
}
/* 0271EDD4 */
static void titleStateClear(void*, gptr<void>* state) { *state = NULL; }
/* 0271EDE0 */
static hdTitleStateObject_l* titleStateCurrent(hdTitleState_l* state) { return state->mpCurrent; }
/* 0271EDE8 */
static void titleStateEnter(hdTitleState_l* state) {
 hdTitleStateObject_l* current = state->mpCurrent;
 gabi::tail_ptr<void>(current->mVTable[0x24/4].get(), current, state->mOwner.get());
}
/* 0271EE04 */
static void titleStateExecute(hdTitleState_l* state) {
 hdTitleStateObject_l* current = state->mpCurrent;
 gabi::tail_ptr<void>(current->mVTable[0x2C/4].get(), current, state->mOwner.get());
}
/* 0271EE20 */
static void titleStateExit(hdTitleState_l* state) {
 hdTitleStateObject_l* current = state->mpCurrent;
 gabi::call_ptr<void>(current->mVTable[0x34/4].get(), current, state->mOwner.get());
 state->mpCurrent = NULL;
}
/* 0271EE68 */
void hdTitleScene_c::unloadLayout() {
 hdScreenString name;
 name.mText.v = gabi::guest_addr("Title_00");
 name.mVtable = reinterpret_cast<hdScreenStringTable*>(&l_titleSafeStringVTable);
 l_titleLayoutMgr.mpInstance->unload(&name);
}
/* 0271EEAC */
void hdTitleScene_c::createScreen() {
 hdRenderLayerRegistry_l* layers = l_titleRenderLayers.mpInstance;
 u32 index = layers->mLayerCount>3 ? 3 : 0;
 hdLayerBase_c* params = static_cast<hdLayerBase_c*>(layers->mpLayers[index].get());
 hdTitleLogo_c* screen = static_cast<hdTitleLogo_c*>(titleAllocate(0xA0));
 if(screen) screen = hdTitleLogo_c::construct(screen);
 mTask.mpScreen = screen;
 u32 flags = params->mFlags;
 void* target;
 if(flags & 0x1000) target = params->mpInternalProjection;
 else { target = params->mpProjection; if(!target) target = &l_titleDefaultTarget; }
 void* camera;
 if((flags&0x40) && (flags&0x80)) camera = &params->mViewMatrix;
 else { camera = params->mpViewMatrix; if(!camera) camera = &l_titleDefaultCamera; }
 screen->initialize(gabi::ea(target), gabi::ea(camera), NULL);
}
/* 0271EF88 */
void hdTitleScene_c::enter() {
 titleSetScreenDimEnabled(titleGetScreenDim(), 0);
 l_titleGameState.mpInstance->mSceneMode = 0;
 // Map embedded addresses after 32-bit guest wrapping; a wrapped member can be NULL.
 HdStateDelegate* delegate = gabi::at<HdStateDelegate>(gabi::ea(this)+offsetof(hdTitleScene_c, mTask)+offsetof(hdTitleTask_l, mDelegate));
 if(delegate) {
  delegate->mObject = this; delegate->mThisAdjustment = 0;
  delegate->mVTable = reinterpret_cast<gfn*>(&l_titleDelegateVTable);
  delegate->mFunction = &l_titleRenderCallback; delegate->mVirtualOffset = 0xFFFF;
 }
 mTask.mEnabled = 1;
 mTask.mName.mStringTop = gabi::ea(&l_titleTaskName);
 hdRenderLayerRegistry_l* layers = l_titleRenderLayers.mpInstance;
 void* layer = layers->mpLayers[layers->mLayerCount>3?3:0];
 titleTaskRegister(layer, 3, &mTask);
 createScreen();
 titleInputMode(l_titleInputMgr.mpInstance, 3);
 titleSoundReset(); titleSoundMode(2);
}
/* 0271F064 */
void hdTitleScene_c::calc() {
 hdTitleLogo_c* screen = mTask.mpScreen;
 gabi::tail_ptr<void>(screen->mVtable->mExecuteState.get(), screen);
}
/* 0271F078 */
void hdTitleScene_c::execute() { calc(); }
/* 0271F07C */
void hdTitleScene_c::draw() {
 hdTitleLogo_c* screen = mTask.mpScreen;
 gabi::tail_ptr<void>(screen->mVtable->mFinalize.get(), screen);
}
/* 0271F090 */
void hdTitleScene_c::leave() {
 l_titleGameState.mpInstance->mSceneMode = 3; draw(); unloadLayout(); titleSetScreenDimEnabled(titleGetScreenDim(), 1);
}
/* 0271F0E0 */
void hdTitleScene_c::requestStart() { mTask.mpScreen->mAdvanceRequested = 1; }
/* 0271F0F0 */
void hdTitleScene_c::requestFinish() { mTask.mpScreen->mSkipRequested = 1; }
/* 0271F100 */
void hdTitleScene_c::render(void* context) {
 titleSetViewport(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
 hdRenderLayerRegistry_l* layers = l_titleRenderLayers.mpInstance;
 hdTitleLogo_c* screen = mTask.mpScreen;
 u32 count = layers->mLayerCount;
 u32 index = static_cast<hdTitleRenderContext_l*>(context)->mLayerIndex;
 hdPartsVtbl_c* vtable = screen->mVtable;
 gptr<hdLayerBase_c>* items = layers->mpLayers;
 if(index<count) items = items+index;
 void* layer = items->get();
 gabi::call_ptr<void>(vtable->mDraw.get(), screen, &static_cast<hdLayerBase_c*>(layer)->mViewport);
}
/* 0271F194 */
void* hdTitleScene_c::getPosition() { return titleScreenPosition(mTask.mpScreen); }
/* 0271F19C */
f32 hdTitleScene_c::getScaleX() { return titleScreenScaleX(mTask.mpScreen); }
/* 0271F1A4 */
f32 hdTitleScene_c::getScaleY() { return titleScreenScaleY(mTask.mpScreen); }
/* 0271F1AC */
void* hdTitleScene_c::getCameraPosition() { return titleScreenCameraPosition(mTask.mpScreen); }
/* 0271F1B4 */
f32 hdTitleScene_c::getCameraNear() { return titleScreenCameraNear(mTask.mpScreen); }
/* 0271F1BC */
f32 hdTitleScene_c::getCameraFar() { return titleScreenCameraFar(mTask.mpScreen); }
/* 0271F1C4 */
void* hdTitleScene_c::getLightPosition() { return titleScreenLightPosition(mTask.mpScreen); }
/* 0271F1CC */
f32 hdTitleScene_c::getLightScaleX() { return titleScreenLightScaleX(mTask.mpScreen); }
/* 0271F1D4 */
f32 hdTitleScene_c::getLightScaleY() { return titleScreenLightScaleY(mTask.mpScreen); }
/* 0271F1DC */
u32 hdTitleScene_c::getColor() { return titleScreenColor(mTask.mpScreen); }
/* 0271F1E4 */
f32 hdTitleScene_c::getDepth() { return titleScreenDepth(mTask.mpScreen); }
/* 0271F1EC */
u32 hdTitleScene_c::getMode() { return titleScreenMode(mTask.mpScreen); }
/* 0271F1F4 */
u32 hdTitleScene_c::getFlags() { return titleScreenFlags(mTask.mpScreen); }
/* 0271F1FC */
static hdTitleScene_c* titleConstruct(hdTitleScene_c* scene) {
 if(!scene) { scene = static_cast<hdTitleScene_c*>(titleAllocate(sizeof(hdTitleScene_c))); if(!scene) return NULL; }
 titleTaskConstruct(&scene->mTask);
 SafeString* name = gabi::at<SafeString>(gabi::ea(scene)+offsetof(hdTitleScene_c, mTask)+offsetof(hdTitleTask_l, mName));
 if(!name) name = static_cast<SafeString*>(titleAllocate(8));
 if(name) { name->__vtbl = gabi::ea(&l_titleSafeStringVTable); name->mStringTop = gabi::ea(&l_titleEmptyString); }
 scene->mTask.mEnabled = 0; scene->mTask.mReserved = 0;
 scene->mTask.mVTable = reinterpret_cast<gfn*>(&l_titleTaskVTable);
 HdStateDelegate* delegate = gabi::at<HdStateDelegate>(gabi::ea(scene)+offsetof(hdTitleScene_c, mTask)+offsetof(hdTitleTask_l, mDelegate));
 if(delegate) {
  HdStateDelegate* base = delegate;
  if(!base) base = static_cast<HdStateDelegate*>(titleAllocate(sizeof(HdStateDelegate)));
  if(base) { base->mFunction = NULL; base->mThisAdjustment = 0; base->mVTable = reinterpret_cast<gfn*>(&l_titleBaseDelegateVTable); base->mVirtualOffset = 0; base->mObject = NULL; }
  delegate->mVTable = reinterpret_cast<gfn*>(&l_titleEmptyDelegateVTable);
 }
 scene->mTask.mpScreen = NULL;
 return scene;
}
/* 0271F2F0 */
static void titleCreateInstance(void* heap) {
 if(l_titleTitle.mpInstance) return;
 hdTitleScene_c* scene = titleAllocateInstance(sizeof(hdTitleScene_c), heap, 4);
 if(scene) { titleDisposerConstruct(&scene->mDisposer, heap, 3); scene->mDisposer.mVTable = reinterpret_cast<gfn*>(&l_titleDisposerVTable); }
 l_titleTitleDisposer.mpInstance = scene?&scene->mDisposer:NULL;
 l_titleTitle.mpInstance = scene?titleConstruct(scene):NULL;
}
/* 0271F384 */
static void titleDestroy(hdTitleScene_c* scene, u32 flags) {
 if(!scene) return;
 titleTaskDestroy(&scene->mTask, 2);
 if(flags&1) titleDelete(scene);
}
/* 0271F3D8 */
static void titleDestroyInstance() {
 HdStateDisposer* disposer = l_titleTitleDisposer.mpInstance;
 if(!disposer) return;
 l_titleTitleDisposer.mpInstance = NULL;
 gabi::call_ptr<void>(disposer->mVTable[3].get(), disposer, 2u);
 titleDestroy(l_titleTitle.mpInstance, 3);
 l_titleTitle.mpInstance = NULL;
}
/* 0271F444 */
static void titleDestroyDisposer(HdStateDisposer* disposer, u32 flags) {
 if(!disposer) return;
 disposer->mVTable = reinterpret_cast<gfn*>(&l_titleDisposerVTable);
 if(disposer == l_titleTitleDisposer.mpInstance) {
  hdTitleScene_c* scene = l_titleTitle.mpInstance; l_titleTitleDisposer.mpInstance = NULL; titleDestroy(scene, 2); l_titleTitle.mpInstance = NULL;
 }
 titleDisposerDestroy(disposer, 0);
 if(flags&1) titleDelete(disposer);
}
/* 0271F4E0 */
static void titleStaticInit() {
 l_titleStatics.mWords[2] = 0; l_titleStatics.mWords[0] = 0; l_titleStatics.mWords[3] = 0; l_titleStatics.mWords[1] = 0;
 titleRegisterGlobal(&l_titleRegistration1);
 f32 lower = l_titleLower; f32 upper = l_titleUpper;
 l_titleRange.mLower = lower; l_titleRange.mUpper = upper;
 titleInitSigned(&l_titleRange.mSigned); titleRegisterGlobal(&l_titleRegistration2);
 titleInitUnsigned(&l_titleRange.mUnsigned); titleRegisterGlobal(&l_titleRegistration3);
}

/* WWHD glue
str 101406EC "Title_00"
data l_titleTitle 101F83FC
data l_titleTitleDisposer 101F8400
data l_titleRenderLayers 101F95D0
data l_titleGameState 101F86E8
data l_titleLayoutMgr 101F7274
data l_titleInputMgr 101F5088
data l_titleRenderCallback 0271F100
data l_titleTaskVTable 10169E3C
data l_titleDelegateVTable 101406D8
data l_titleEmptyDelegateVTable 101406C0
data l_titleBaseDelegateVTable 10140690
data l_titleSafeStringVTable 101406A8
data l_titleDisposerVTable 10140708
data l_titleTaskName 101406F8
data l_titleEmptyString 101406FB
data l_titleDefaultTarget 104A20FC
data l_titleDefaultCamera 104A2098
data l_titleStatics 1049F6CC
data l_titleRange 1049F6C0
data l_titleLower 101406FC
data l_titleUpper 10140700
data l_titleRegistration1 101F83D8
data l_titleRegistration2 101F83E4
data l_titleRegistration3 101F83F0
ext 0273AD10 void* titleAllocate(u32)
ext 0273B0D4 hdTitleScene_c* titleAllocateInstance(u32,void*,u32)
ext 0273AF40 void titleDelete(void*)
ext 02752B0C void titleDisposerConstruct(HdStateDisposer*,void*,u32)
ext 02752BEC void titleDisposerDestroy(HdStateDisposer*,u32)
ext 02752A84 void titleTaskConstruct(hdTitleTask_l*)
ext 027C0804 void titleTaskDestroy(hdTitleTask_l*,u32)
ext 027C23F8 void titleTaskRegister(void*,u32,hdTitleTask_l*)
ext 026EAF38 hdTitleLogo_c* hdTitleLogo_c::construct(hdTitleLogo_c*)
ext 026EB750 u32 hdTitleLogo_c::initialize(u32,u32,hdPartsDesc_c*)
ext 02032C6C void* titleGetScreenDim()
ext 02032AD8 void titleSetScreenDimEnabled(void*,u32)
ext 02618094 void titleInputMode(void*,u32)
ext 020310F4 void titleSoundReset()
ext 020309AC void titleSoundMode(u32)
ext C0006988 void titleSetViewport(u32,u32,u32,u32,u32,u32,u32)
ext 026EBD58 void* titleScreenPosition(hdTitleLogo_c*)
ext 026EBD64 f32 titleScreenScaleX(hdTitleLogo_c*)
ext 026EBD70 f32 titleScreenScaleY(hdTitleLogo_c*)
ext 026EBD7C void* titleScreenCameraPosition(hdTitleLogo_c*)
ext 026EBD88 f32 titleScreenCameraNear(hdTitleLogo_c*)
ext 026EBD94 f32 titleScreenCameraFar(hdTitleLogo_c*)
ext 026EBDA0 void* titleScreenLightPosition(hdTitleLogo_c*)
ext 026EBDAC f32 titleScreenLightScaleX(hdTitleLogo_c*)
ext 026EBDB8 f32 titleScreenLightScaleY(hdTitleLogo_c*)
ext 026EBDC4 u32 titleScreenColor(hdTitleLogo_c*)
ext 026EBDD0 f32 titleScreenDepth(hdTitleLogo_c*)
ext 026EBDDC u32 titleScreenMode(hdTitleLogo_c*)
ext 026EBDE8 u32 titleScreenFlags(hdTitleLogo_c*)
ext 028F026C void titleRegisterGlobal(void*)
ext 028ED6F8 void titleInitSigned(be<u8>*)
ext 028EAB2C void titleInitUnsigned(be<u8>*)
*/
