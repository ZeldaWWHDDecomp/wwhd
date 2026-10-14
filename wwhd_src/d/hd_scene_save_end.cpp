/* HD post-credits save scene; private binary-derived reconstruction. */
#include "d/hd_scene_save_end.h"
extern void* saveEndAllocate(u32);
extern hdSaveEndScene_c* saveEndAllocateInstance(u32, void*, u32);
extern void saveEndDelete(void*);
extern void saveEndDisposerConstruct(HdStateDisposer*, void*, u32);
extern void saveEndDisposerDestroy(HdStateDisposer*, u32);
extern hdSaveStateId_l* saveEndCurrentState(hdStateMachine*);
extern void saveEndChangeState(hdStateMachine*, hdSaveStateId_l*);
extern void saveEndRegisterGlobal(void*);
extern void saveEndInitSigned(be<u8>*);
extern void saveEndInitUnsigned(be<u8>*);
static hdSaveSceneSingleton_l l_saveEndSaveEnd;
static hdSaveDisposerSingleton_l l_saveEndSaveEndDisposer;
static hdSaveControllerSingleton_l l_saveEndController;
static hdSaveSlotSingleton_l l_saveEndSaveSlot;
static hdSaveStateId_l l_saveEndMenuSaveEnd;
static hdSaveStateId_l l_saveEndSaveInitState;
static hdSaveStateId_l l_saveEndSaveEndState;
static u32 l_saveEndSceneVTable;
static u32 l_saveEndDisposerVTable;
static u32 l_saveEndStateIdVTable;
static u32 l_saveEndEnterSaveEnd;
static u32 l_saveEndCalcSaveEnd;
static u32 l_saveEndExitSaveInit;
static u32 l_saveEndQuerySaveInit;
static u32 l_saveEndEnterSaveInit;
static u32 l_saveEndCalcSaveInit;
static u32 l_saveEndExitSaveEnd;
static u32 l_saveEndQuerySaveEnd;
static HdStateStatics l_saveEndStatics;
static HdStateRange l_saveEndRange;
static f32 l_saveEndLower;
static f32 l_saveEndUpper;
static HdStateRegistration l_saveEndRegistration1;
static HdStateRegistration l_saveEndRegistration2;
static HdStateRegistration l_saveEndRegistration3;
static u32 l_saveEndStateIdInitialized;
static u32 l_saveEndLastStateId;
/* 0271B204 */
static void saveEndRequestSave() { l_saveEndController.mpInstance->mSaveRequested = 1; }
/* 0271B218 */
void hdSaveEndScene_c::calcSaveInit() {
 hdSequenceWindow_c* dialog = menu();
 hdSaveStateId_l* current = saveEndCurrentState(&dialog->mStateHolder.mMachine);
 gfn* endVTable = l_saveEndMenuSaveEnd.mVTable;
 u32 currentId = gabi::call_ptr<u32>(current->mVTable[5].get(), current);
 u32 endId = gabi::call_ptr<u32>(endVTable[5].get(), &l_saveEndMenuSaveEnd);
 if(currentId == endId || dialog->mAdvanceRequested) { resetEventMode(); saveEndChangeState(&mState, &l_saveEndSaveEndState); }
}
/* 0271B2C8 */
void hdSaveEndScene_c::enterSaveInit() {
 screen()->hideHeader();
 s32 slot = l_saveEndSaveSlot.mpInstance->mSlot;
 menu()->open(slot == -2?13:14, 0);
 hdSaveStateObject_l* factory = static_cast<hdSaveStateObject_l*>(static_cast<void*>(mState.mpOwner.get()));
 hdSaveStateObject_l* state = gabi::call_ptr<hdSaveStateObject_l*>(factory->mVTable[5].get(), factory, &l_saveEndSaveInitState);
 mState.mpCurrentState = reinterpret_cast<hdStateObject*>(state);
 gabi::call_ptr<void>(state->mVTable[7].get(), state);
}
/* 0271B350 */
static void saveInitRequestSave() { l_saveEndController.mpInstance->mSaveRequested = 1; }
/* 0271B364 */
static hdSaveEndScene_c* saveEndConstruct(hdSaveEndScene_c* scene) {
 if(!scene) { scene = static_cast<hdSaveEndScene_c*>(saveEndAllocate(sizeof(hdSaveEndScene_c))); if(!scene) return NULL; }
 saveDataSceneConstruct(scene); scene->mpTaskVtable = reinterpret_cast<gfn*>(&l_saveEndSceneVTable); return scene;
}
/* 0271B3B8 */
static void saveEndCreateInstance(void* heap) {
 if(l_saveEndSaveEnd.mpInstance) return;
 hdSaveEndScene_c* scene = saveEndAllocateInstance(sizeof(hdSaveEndScene_c), heap, 4);
 // Map embedded addresses after 32-bit guest wrapping; a wrapped member can be NULL.
 HdStateDisposer* disposer = gabi::at<HdStateDisposer>(gabi::ea(scene)+offsetof(hdSaveEndScene_c, mDisposer));
 if(disposer) { saveEndDisposerConstruct(disposer, heap, 3); disposer->mVTable = reinterpret_cast<gfn*>(&l_saveEndDisposerVTable); }
 l_saveEndSaveEndDisposer.mpInstance = disposer;
 l_saveEndSaveEnd.mpInstance = scene?saveEndConstruct(scene):NULL;
}
/* 0271B458 */
static void saveEndDestroyInstance() {
 HdStateDisposer* disposer = l_saveEndSaveEndDisposer.mpInstance;
 if(!disposer) return;
 l_saveEndSaveEndDisposer.mpInstance = NULL;
 gabi::call_ptr<void>(disposer->mVTable[3].get(), disposer, 2u);
 hdSaveEndScene_c* scene = l_saveEndSaveEnd.mpInstance;
 if(scene) gabi::call_ptr<void>(scene->mpTaskVtable[3].get(), scene, 3u);
 l_saveEndSaveEnd.mpInstance = NULL;
}
/* 0271B4D8 */
static void saveEndDestroy(hdSaveEndScene_c* scene, u32 flags) {
 if(!scene) return;
 saveDataSceneDestroy(scene, 0);
 if(flags&1) saveEndDelete(scene);
}
/* 0271B52C */
u8 hdSaveEndScene_c::isFinished() {
 if(isSaveSelection()) return 1;
 hdSaveStateId_l* current = saveEndCurrentState(&mState);
 gfn* endVTable = l_saveEndSaveInitState.mVTable;
 u32 currentId = gabi::call_ptr<u32>(current->mVTable[5].get(), current);
 u32 endId = gabi::call_ptr<u32>(endVTable[5].get(), &l_saveEndSaveInitState);
 return currentId == endId;
}
/* 0271B5C4 */
static void saveEndDestroyDisposer(HdStateDisposer* disposer, u32 flags) {
 if(!disposer) return;
 disposer->mVTable = reinterpret_cast<gfn*>(&l_saveEndDisposerVTable);
 if(disposer == l_saveEndSaveEndDisposer.mpInstance) {
  hdSaveEndScene_c* scene = l_saveEndSaveEnd.mpInstance;
  l_saveEndSaveEndDisposer.mpInstance = NULL;
  gabi::call_ptr<void>(scene->mpTaskVtable[3].get(), scene, 2u);
  l_saveEndSaveEnd.mpInstance = NULL;
 }
 saveEndDisposerDestroy(disposer, 0); if(flags&1) saveEndDelete(disposer);
}
static void saveEndInitState(hdSaveStateId_l* state, u32 id, const char* name, void* enter, void* calc, void* exit, void* query) {
 state->mId = id; state->mName.v = gabi::guest_addr(name); state->mVTable = reinterpret_cast<gfn*>(&l_saveEndStateIdVTable);
 state->mEnterAdjustment = 0; state->mEnterVirtual = 0xFFFF; state->mEnter = enter;
 state->mCalcAdjustment = 0; state->mCalcVirtual = 0xFFFF; state->mCalc = calc;
 state->mExitAdjustment = 0; state->mExitVirtual = 0xFFFF; state->mExit = exit;
 state->mReserved = 0; state->mQueryVirtual = 0xFFFF; state->mQuery = query;
}
/* 0271B66C */
static void saveEndStaticInit() {
 l_saveEndStatics.mWords[2] = 0; l_saveEndStatics.mWords[0] = 0; l_saveEndStatics.mWords[3] = 0; l_saveEndStatics.mWords[1] = 0;
 saveEndRegisterGlobal(&l_saveEndRegistration1);
 f32 lower = l_saveEndLower; f32 upper = l_saveEndUpper;
 l_saveEndRange.mLower = lower; l_saveEndRange.mUpper = upper;
 saveEndInitSigned(&l_saveEndRange.mSigned); saveEndRegisterGlobal(&l_saveEndRegistration2);
 saveEndInitUnsigned(&l_saveEndRange.mUnsigned); saveEndRegisterGlobal(&l_saveEndRegistration3);
 u32 id = 0;
 if(l_saveEndStateIdInitialized) id = l_saveEndLastStateId; else l_saveEndStateIdInitialized = 1;
 l_saveEndLastStateId = id+2;
 saveEndInitState(&l_saveEndSaveInitState, id+1, "::StateID_SaveInit", &l_saveEndEnterSaveEnd, &l_saveEndCalcSaveEnd, &l_saveEndExitSaveInit, &l_saveEndQuerySaveInit);
 saveEndInitState(&l_saveEndSaveEndState, id+2, "::StateID_SaveEnd", &l_saveEndEnterSaveInit, &l_saveEndCalcSaveInit, &l_saveEndExitSaveEnd, &l_saveEndQuerySaveEnd);
}
/* WWHD glue
data l_saveEndSaveEnd 101F83A4
data l_saveEndSaveEndDisposer 101F83A8
data l_saveEndController 101F8378
data l_saveEndSaveSlot 101F852C
data l_saveEndMenuSaveEnd 1049DD60
data l_saveEndSaveInitState 1049F554
data l_saveEndSaveEndState 1049F580
data l_saveEndSceneVTable 101402F0
data l_saveEndDisposerVTable 10140320
data l_saveEndStateIdVTable 10140278
data l_saveEndEnterSaveEnd 0271B204
data l_saveEndCalcSaveEnd 0271B218
data l_saveEndExitSaveInit 0271B910
data l_saveEndQuerySaveInit 0271B914
data l_saveEndEnterSaveInit 0271B91C
data l_saveEndCalcSaveInit 0271B920
data l_saveEndExitSaveEnd 0271B924
data l_saveEndQuerySaveEnd 0271B928
data l_saveEndStatics 1049F544
data l_saveEndRange 1049F538
data l_saveEndLower 101402C0
data l_saveEndUpper 101402C4
data l_saveEndRegistration1 101F8380
data l_saveEndRegistration2 101F838C
data l_saveEndRegistration3 101F8398
data l_saveEndStateIdInitialized 101FD8E4
data l_saveEndLastStateId 101FDD50
str 101402DC "::StateID_SaveInit"
str 101402C8 "::StateID_SaveEnd"
ext 0273AD10 void* saveEndAllocate(u32)
ext 0273B0D4 hdSaveEndScene_c* saveEndAllocateInstance(u32,void*,u32)
ext 0273AF40 void saveEndDelete(void*)
ext 0270B780 hdSaveDataScene_c* saveDataSceneConstruct(hdSaveDataScene_c*)
ext 0270BA48 void saveDataSceneDestroy(hdSaveDataScene_c*,u32)
ext 02752B0C void saveEndDisposerConstruct(HdStateDisposer*,void*,u32)
ext 02752BEC void saveEndDisposerDestroy(HdStateDisposer*,u32)
ext 02006478 hdSaveStateId_l* saveEndCurrentState(hdStateMachine*)
ext 020063C0 void saveEndChangeState(hdStateMachine*,hdSaveStateId_l*)
ext 0270AAEC void hdSaveDataScene_c::resetEventMode()
ext 026E5580 void hdTitleStartBackground_c::hideHeader()
ext 026DBA08 void hdSequenceWindow_c::open(s32,u32)
ext 0270BCF4 u8 hdSaveDataScene_c::isSaveSelection()
ext 028F026C void saveEndRegisterGlobal(void*)
ext 028ED6F8 void saveEndInitSigned(be<u8>*)
ext 028EAB2C void saveEndInitUnsigned(be<u8>*)
*/
