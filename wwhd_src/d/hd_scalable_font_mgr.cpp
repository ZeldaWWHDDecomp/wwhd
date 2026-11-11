/* HD scalable-font manager: creates the scalable font and its glyph texture cache from the OS shared font, and
 * runs the per-frame cache updates through its state machine. The font and the cache are the nw::font engine
 * (NW-FONT range 028B2204-028E81E4, replaced by the port's font module); this file only calls their entry points
 * by address. Reconstructed from the game's instructions. */
#include "d/hd_scalable_font_mgr.h"
using namespace gabi;
extern void* operator_new(u32);
extern hdScalableFontMgr_c* hdSFont_allocInstance(u32,void*,u32);
extern HdStateVTableObject* hdSFont_allocAllocator(u32,void*,u32);
extern void operator_delete(void*);
extern void hdSFont_constructStateMachine(HdStateMachine*,hdScalableFontMgr_c*,void*);
extern void hdSFont_changeState(HdStateMachine*,HdStateId*);
extern void hdSFont_executeState(HdStateMachine*);
extern void scalableFontConstruct(ScalableFontStorage*);
extern void glyphCacheConstruct(GlyphCacheStorage*);
extern void hdSFont_constructDisposer(HdStateDisposer*,void*,u32);
extern void hdSFont_destroyBaseDisposer(HdStateDisposer*,u32);
extern void glyphCacheUpdateC(GlyphCacheStorage*);
extern void glyphCacheFinalize(GlyphCacheStorage*,HdStateVTableObject*);
extern void glyphCacheDestroy(GlyphCacheStorage*,u32);
extern void scalableFontDestroy(ScalableFontStorage*,u32);
extern void OSGetSharedData(u32,u32,be<u32>*,be<u32>*);
extern void glyphCacheInitArgDefaults(GlyphCacheInitArg*);
extern void glyphCacheInitialize(GlyphCacheStorage*,GlyphCacheInitArg*);
extern void scalableFontInitArgDefaults(ScalableFontInitArg*);
extern void scalableFontInitialize(ScalableFontStorage*,ScalableFontInitArg*);
extern HdStateVTableObject* hdAppAllocatorConstruct(HdStateVTableObject*,void*);
extern void glyphCacheUpdateA(GlyphCacheStorage*, void*, u32, u32, u32, u32);
extern void glyphCacheUpdateB(GlyphCacheStorage*);
extern void hdSFont_runObject(void*);
extern void hdSFont_registerDestructor(void*);
extern void initializeHeaderByte(be<u8>*);
extern void initializeHeaderFlag(be<u8>*);
static hdSFontSingleton l_singleton;
static HdStateDisposerHolder l_disposerHolder;
static u32 l_baseVTable;
static u32 l_objectVTable;
static u32 l_initialTaskVTable;
static u32 l_taskVTable;
static u32 l_delegateVTable;
static u32 l_disposerVTable;
static u32 l_stateIdVTable;
static u32 l_factory;
static u32 l_updateHandler;
static HdStateId l_waitState;
static HdStateId l_workState;
static HdStateId l_finishState;
static HdStateStatics l_statics;
static HdStateRange l_range;
static f32 l_lower;
static f32 l_upper;
static HdStateRegistration l_registration1;
static HdStateRegistration l_registration2;
static HdStateRegistration l_registration3;
static u32 l_stateIdInitialized;
static u32 l_lastStateId;

/* 025F9644 */
void hdScalableFontMgr_c::clearFlag() {
    mFlag = 0;
}

/* 025F9650 */
u32 hdScalableFontMgr_c::isIdle() {
    return mProgress != HD_SFONT_WORKING ? 1u : 0u;
}

/* 025F9664 */
void hdScalableFontMgr_c::updateGlyphCache() {
    if (!isIdle()) return;
    mBusyCount = mBusyCount + 1;
    glyphCacheUpdateC(&mGlyphCache);
    mBusyCount = mBusyCount - 1;
    hdSFont_changeState(&mState,&l_finishState);
}

/* 025F96E0 */
void hdScalableFontMgr_c::setFlag() {
    mFlag = 1;
}

/* 025F96EC */
static hdScalableFontMgr_c* hdSFont_constructManager(hdScalableFontMgr_c* i_this) {
    if (!i_this) {
        i_this = static_cast<hdScalableFontMgr_c*>(operator_new(sizeof(hdScalableFontMgr_c)));
        if (!i_this) return NULL;
    }
    hdScalableFontMgr_c* base = i_this;
    if (!base) base = static_cast<hdScalableFontMgr_c*>(operator_new(0x28));
    if (base) {
        base->mSelf = i_this;
        base->mReserved = 0;
        base->mObjectVTable = reinterpret_cast<gfn*>(&l_objectVTable);
        base->mVTable = reinterpret_cast<gfn*>(&l_baseVTable);
        base->mTaskVTable = reinterpret_cast<gfn*>(&l_initialTaskVTable);
        hdSFont_constructStateMachine(&base->mState,base,&l_factory);
    }
    i_this->mHeap = NULL;
    i_this->mTaskVTable = reinterpret_cast<gfn*>(&l_taskVTable);
    scalableFontConstruct(&i_this->mFont);
    glyphCacheConstruct(&i_this->mGlyphCache);
    i_this->mAllocator = NULL;
    i_this->mFontData = 0;
    // HD: preserve the embedded delegate's wrapping guest-address allocation check.
    HdStateDelegate* delegate = gabi::at<HdStateDelegate>(gabi::ea(i_this)+offsetof(hdScalableFontMgr_c,mUpdateDelegate));
    if (!delegate) delegate = static_cast<HdStateDelegate*>(operator_new(sizeof(HdStateDelegate)));
    if (delegate) {
        delegate->mObject = i_this;
        delegate->mThisAdjustment = 0;
        delegate->mVTable = reinterpret_cast<gfn*>(&l_delegateVTable);
        delegate->mFunction = &l_updateHandler;
        delegate->mVirtualOffset = 0xFFFF;
    }
    i_this->mProgress = HD_SFONT_IDLE;
    i_this->mFlag = 0;
    i_this->mObject = NULL;
    be<u32>* counter = gabi::at<be<u32>>(gabi::ea(i_this)+offsetof(hdScalableFontMgr_c,mBusyCount));
    if (!counter) {
        counter = static_cast<be<u32>*>(operator_new(sizeof(be<u32>)));
        if (!counter) return i_this;
    }
    *counter = 0;
    return i_this;
}

/* 025F9818 */
static void hdSFont_createInstance(void* heap) {
    if (l_singleton.mInstance) return;
    hdScalableFontMgr_c* task = hdSFont_allocInstance(sizeof(hdScalableFontMgr_c),heap,4);
    // HD: even a failed allocation passes the wrapped embedded disposer address onward.
    HdStateDisposer* disposer = gabi::at<HdStateDisposer>(gabi::ea(task)+offsetof(hdScalableFontMgr_c,mDisposer));
    if (disposer) {
        hdSFont_constructDisposer(disposer,heap,3);
        disposer->mVTable = reinterpret_cast<gfn*>(&l_disposerVTable);
    }
    l_disposerHolder.mDisposer = disposer;
    l_singleton.mInstance = task ? hdSFont_constructManager(task) : NULL;
}

/* 025F98B8 */
static void hdSFont_destroyManager(hdScalableFontMgr_c* i_this,u32 flags) {
    if (!i_this) return;
    HdStateVTableObject* worker = i_this->mAllocator;
    i_this->mTaskVTable = reinterpret_cast<gfn*>(&l_taskVTable);
    glyphCacheFinalize(&i_this->mGlyphCache,worker);
    worker = i_this->mAllocator;
    if (worker) {
        gabi::call_ptr<void>(worker->mVTable[5].get(),worker,3u);
        i_this->mAllocator = NULL;
    }
    HdStateObject* object = i_this->mObject;
    if (object) {
        gabi::call_ptr<void>(object->mVTable[3].get(),object,3u);
        i_this->mObject = NULL;
    }
    glyphCacheDestroy(&i_this->mGlyphCache,2);
    scalableFontDestroy(&i_this->mFont,2);
    if (i_this) {
        HdStateVTableObject* factory = i_this->mState.mFactory;
        gabi::call_ptr<void>(factory->mVTable[7].get(),factory,&i_this->mState.mNext);
        factory = i_this->mState.mFactory;
        gabi::call_ptr<void>(factory->mVTable[7].get(),factory,&i_this->mState.mCurrent);
    }
    if (flags&1) operator_delete(i_this);
}

/* 025F99BC */
void hdScalableFontMgr_c::initScalableFont() {
    // HD: retain original frame allocation and the partially initialized parameter/message bytes.
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::Local<hdSFontInitFrame> storage;
    hdSFontInitFrame* frame = storage.get();
    OSGetSharedData(2,0,&mFontData,&frame->mFontDataSize);
    frame->mZeroWord = 0;
    glyphCacheInitArgDefaults(&frame->mCacheArg);
    HdStateVTableObject* worker = mAllocator;
    frame->mCacheArg.mSheetHeight = 0x800;
    frame->mCacheArg.mFontData = &mFontData;
    frame->mCacheArg.mZeroWord = &frame->mZeroWord;
    frame->mCacheArg.mSheetWidth = 0x400;
    frame->mCacheArg.mFontCount = 1;
    frame->mCacheArg.mAllocator = worker;
    frame->mCacheArg.mFontDataSize = &frame->mFontDataSize;
    frame->mCacheArg.mReserved2C = 0x1400;
    glyphCacheInitialize(&mGlyphCache,&frame->mCacheArg);
    scalableFontInitArgDefaults(&frame->mFontArg);
    frame->mFontArg.mModeSelect = 1;
    frame->mFontArg.mFace = 0;
    frame->mFontArg.mGlyphCache = &mGlyphCache;
    frame->mFontArg.mFontSize = 0x19;
    scalableFontInitialize(&mFont,&frame->mFontArg);
}

/* 025F9A90 */
void hdScalableFontMgr_c::init(void* heap) {
    mHeap = heap;
    HdStateVTableObject* worker = hdSFont_allocAllocator(8,heap,4);
    if (worker) worker = hdAppAllocatorConstruct(worker,mHeap);
    mAllocator = worker;
    initScalableFont();
    HdStateVTableObject* factory = mState.mFactory;
    HdStateVTableObject* state = gabi::call_ptr<HdStateVTableObject*>(factory->mVTable[5].get(),factory,&l_waitState);
    mState.mCurrent = state;
    gabi::call_ptr<void>(state->mVTable[7].get(),state);
}

/* 025F9B14 */
void hdScalableFontMgr_c::execute() {
    hdSFont_executeState(&mState);
}

/* 025F9B1C */
u32 hdScalableFontMgr_c::requestCacheUpdateA(void* text, u32 a, u32 b, u32 c, u32 d) {
    if (!isIdle()) return 0;
    glyphCacheUpdateA(&mGlyphCache, text, a, b, c, d);
    return 1;
}

/* 025F9B68 */
u32 hdScalableFontMgr_c::requestCacheUpdateB() {
    if (mBusyCount) return 0;
    if (!isIdle()) return 0;
    glyphCacheUpdateB(&mGlyphCache);
    return 1;
}

/* 025F9BC4 */
void hdScalableFontMgr_c::onMessage(void* object,u32 message) {
    if (message != HD_SFONT_MSG_UPDATE) return;
    mProgress = HD_SFONT_WORKING;
    glyphCacheUpdateB(&mGlyphCache);
    hdSFont_runObject(object);
    mProgress = HD_SFONT_DONE;
}

/* 025F9C28 */
static void hdSFont_destroyDisposer(HdStateDisposer* disposer,u32 flags) {
    if (!disposer) return;
    disposer->mVTable = reinterpret_cast<gfn*>(&l_disposerVTable);
    if (disposer == l_disposerHolder.mDisposer) {
        hdScalableFontMgr_c* instance = l_singleton.mInstance;
        l_disposerHolder.mDisposer = NULL;
        gabi::call_ptr<void>(instance->mTaskVTable[3].get(),instance,2u);
        l_singleton.mInstance = NULL;
    }
    hdSFont_destroyBaseDisposer(disposer,0);
    if (flags&1) operator_delete(disposer);
}

static void setStateId(HdStateId* state,u32 id,const char* name,u16 enter,u16 execute,u16 exit) {
    state->mEnterAdjustment = 0;
    state->mId = id;
    state->mName = name;
    state->mEnterOffset = 0x24;
    state->mEnterIndex = enter;
    state->mExecuteAdjustment = 0;
    state->mExecuteIndex = execute;
    state->mExecuteOffset = 0x24;
    state->mExitOffset = 0x24;
    state->mReserved28 = 0;
    state->mExitAdjustment = 0;
    state->mExitIndex = exit;
    state->mVTable = reinterpret_cast<gfn*>(&l_stateIdVTable);
    state->mReserved24 = 0;
    state->mFactory = &l_factory;
    state->mReserved26 = 0;
}

/* 025F9CD0 */
static void hdSFont_staticInit() {
    l_statics.mWords[2] = 0;
    l_statics.mWords[0] = 0;
    l_statics.mWords[3] = 0;
    l_statics.mWords[1] = 0;
    hdSFont_registerDestructor(&l_registration1);
    const f32 lower = l_lower;
    const f32 upper = l_upper;
    l_range.mLower = lower;
    l_range.mUpper = upper;
    initializeHeaderByte(&l_range.mSigned);
    hdSFont_registerDestructor(&l_registration2);
    initializeHeaderFlag(&l_range.mUnsigned);
    hdSFont_registerDestructor(&l_registration3);
    u32 id = 0;
    if (l_stateIdInitialized) id = l_lastStateId;
    else l_stateIdInitialized = 1;
    setStateId(&l_waitState,id+1,gabi::at<const char>(gabi::guest_addr("::StateID_Wait")),2,3,4);
    setStateId(&l_workState,id+2,gabi::at<const char>(gabi::guest_addr("::StateID_InProgress")),5,6,7);
    l_lastStateId = id+3;
    setStateId(&l_finishState,id+3,gabi::at<const char>(gabi::guest_addr("::StateID_Finish")),8,9,10);
}

/* WWHD glue (tools/verify/autoglue.py)
data l_singleton 101F4BD8
data l_disposerHolder 101F4BDC
data l_baseVTable 100E0E00
data l_objectVTable 100E0DC0
data l_initialTaskVTable 100E0DA0
data l_taskVTable 100E0F08
data l_delegateVTable 100E0DB0
data l_disposerVTable 100E0EF8
data l_stateIdVTable 100E0E68
data l_factory 101FF32C
data l_updateHandler 025F9BC4
data l_waitState 1048D7D4
data l_workState 1048D804
data l_finishState 1048D834
data l_statics 1048D7C4
data l_range 1048D7B8
data l_lower 100E0EB0
data l_upper 100E0EB4
data l_registration1 101F4BB4
data l_registration2 101F4BC0
data l_registration3 101F4BCC
data l_stateIdInitialized 101FD8E4
data l_lastStateId 101FDD50
str 100E0EB8 "::StateID_Wait"
str 100E0EC8 "::StateID_InProgress"
str 100E0EE0 "::StateID_Finish"
ext 0273AD10 void* operator_new(u32)
ext 0273B0D4 hdScalableFontMgr_c* hdSFont_allocInstance(u32,void*,u32)
ext 0273B050 HdStateVTableObject* hdSFont_allocAllocator(u32,void*,u32)
ext 0273AF40 void operator_delete(void*)
ext 020062E8 void hdSFont_constructStateMachine(HdStateMachine*,hdScalableFontMgr_c*,void*)
ext 020063C0 void hdSFont_changeState(HdStateMachine*,HdStateId*)
ext 02006364 void hdSFont_executeState(HdStateMachine*)
ext 028B2204 void scalableFontConstruct(ScalableFontStorage*)
ext 028B28F8 void glyphCacheConstruct(GlyphCacheStorage*)
ext 02752B0C void hdSFont_constructDisposer(HdStateDisposer*,void*,u32)
ext 02752BEC void hdSFont_destroyBaseDisposer(HdStateDisposer*,u32)
ext 028B3E8C void glyphCacheUpdateC(GlyphCacheStorage*)
ext 028B3124 void glyphCacheFinalize(GlyphCacheStorage*,HdStateVTableObject*)
ext 028B2A40 void glyphCacheDestroy(GlyphCacheStorage*,u32)
ext 028B229C void scalableFontDestroy(ScalableFontStorage*,u32)
ext C0009C58 void OSGetSharedData(u32,u32,be<u32>*,be<u32>*)
ext 028B4230 void glyphCacheInitArgDefaults(GlyphCacheInitArg*)
ext 028B2B64 void glyphCacheInitialize(GlyphCacheStorage*,GlyphCacheInitArg*)
ext 028B289C void scalableFontInitArgDefaults(ScalableFontInitArg*)
ext 028B239C void scalableFontInitialize(ScalableFontStorage*,ScalableFontInitArg*)
ext 020039C0 HdStateVTableObject* hdAppAllocatorConstruct(HdStateVTableObject*,void*)
ext 028B3540 void glyphCacheUpdateA(GlyphCacheStorage*, void*, u32, u32, u32, u32)
ext 028B3804 void glyphCacheUpdateB(GlyphCacheStorage*)
ext 0275FFC8 void hdSFont_runObject(void*)
ext 028F026C void hdSFont_registerDestructor(void*)
ext 028ED6F8 void initializeHeaderByte(be<u8>*)
ext 028EAB2C void initializeHeaderFlag(be<u8>*)
*/
