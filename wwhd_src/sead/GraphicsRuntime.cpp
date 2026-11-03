// Graphics runtime reconstructed from the WWHD binary.
// SDK boundary facts: wut gx2/surface.h, state.h and enum.h declarations only; no implementation copied.
#include "sead/GraphicsRuntime.h"
#include "sead/RuntimeType.h"
namespace sead {}
using namespace sead;

// The canonical records expose opaque dispatch and surface prefixes without duplicating classes.
static MethodTreeMgrBase* methodTreeManager(CafeGameFramework* framework) { return reinterpret_cast<MethodTreeMgrBase*>(framework->mMethodTreeMgr.get()); }
static gptr<DisplayFrame>& tvFrameBuffer(CafeGameFramework* framework) { return *reinterpret_cast<gptr<DisplayFrame>*>(&framework->mCommand); }
static gptr<DisplayFrame>& drcFrameBuffer(CafeGameFramework* framework) { return *reinterpret_cast<gptr<DisplayFrame>*>(&framework->mMessage); }
static VirtualFrameBuffer* logicalFrame(DisplayFrame* frame) { return gabi::at<VirtualFrameBuffer>(gabi::ea(frame->mSurfaceFields)); }
static GraphicsDevice* device(DeviceStateCache_l* state) { return gabi::at<GraphicsDevice>(gabi::ea(state->mBase)); }
static VtableSlot* frameworkTable(CafeGameFramework* framework) { return reinterpret_cast<VtableSlot*>(framework->mVtable.get()); }
static VtableSlot* frameTable(DisplayFrame* frame) { return static_cast<VtableSlot*>(logicalFrame(frame)->mVtable.get()); }
static VtableSlot* scanTable(ScanTarget* scan) { return static_cast<VtableSlot*>(scan->mTable.get()); }
enum CafeMethodSlot : u32 {
    cGetClearTarget=0x30,cUpdate=0x68,cBeforeTick=0xB0,cAfterTick=0xC0,cThreadBody=0xC8,
    cBeforeFrame=0xD0,cFrameUpdate=0xD8,cAfterFrame=0xE0,cPrepareRender=0xE8,cDraw=0xF0,
    cBindFrameBuffer=0x20,cClearFrameBuffer=0x28,cActivateFrameBuffer=0x38,cAllocateScan=0x18
};
static u32 method(const VtableSlot* table,u32 slot) { return table[slot/sizeof(VtableSlot)].mFunction; }

static u8 sGraphicsOwnerDefinition;
/* 0274D690 */
void sead::activateFrameBuffer(DisplayFrame* frame) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 target=method(frameTable(frame),cActivateFrameBuffer);
    gabi::tail_ptr<void>(target,frame);
}
/* 0274BF78 */
void sead::CafeGameFramework::beginTick() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    gabi::call_ptr<void>(method(frameworkTable(this),cBeforeTick),this);
    auto time=seadGraphicsSystemTime();
    VtableSlot* methods=frameworkTable(this);
    frameStartClock()->mHigh=time.r3;
    frameStartClock()->mLow=time.r4;
    u32 next=method(methods,cAfterTick);
    // HD: frame-exact for cross-build save states (the main thread runs the game below this call):
    // the original's return address and r31.
    gabi::call_ptr_site(0x0274BFBC,{{31,gabi::ea(this)}},{},next,this);
}
/* 0274C554 */
void sead::CafeGameFramework::bindTv() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    DisplayFrame* target=graphicsTail()->mTvOverride;
    if(target) {
        u32 bind=method(frameTable(target),cBindFrameBuffer);
        void* buffer=graphicsStorage()->mTvBuffer;
        gabi::call_ptr<void>(bind,target,buffer);
        graphicsTail()->mTvOverride=nullptr;
    } else {
        target=tvFrameBuffer(this);
        if(target) {
            u32 bind=method(frameTable(target),cBindFrameBuffer);
            void* buffer=graphicsStorage()->mTvBuffer;
            gabi::call_ptr<void>(bind,target,buffer);
        }
    }
    seadGraphicsSetContext(reinterpret_cast<DeviceStateCache_l*>(sCurrentGraphicsDevice.get())->mContext);
}
/* 0274C5E8 */
void sead::CafeGameFramework::bindDrc() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    DisplayFrame* target=graphicsTail()->mDrcOverride;
    if(target) {
        u32 bind=method(frameTable(target),cBindFrameBuffer);
        void* buffer=graphicsStorage()->mDrcBuffer;
        gabi::call_ptr<void>(bind,target,buffer);
        graphicsTail()->mDrcOverride=nullptr;
    } else {
        target=drcFrameBuffer(this);
        if(target) {
            u32 bind=method(frameTable(target),cBindFrameBuffer);
            void* buffer=graphicsStorage()->mDrcBuffer;
            gabi::call_ptr<void>(bind,target,buffer);
        }
    }
    seadGraphicsSetContext(reinterpret_cast<DeviceStateCache_l*>(sCurrentGraphicsDevice.get())->mContext);
}
/* 0274C874 */
void sead::CafeGameFramework::waitForSwap() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!graphicsParameters()->mWaitVsync) return;
    gabi::FrameLocal<be<u32>> submitted(8),swapped(0xC);
    gabi::FrameLocal<u8[8]> lastFlip(0x10),lastVsync(0x18);
    do {
        seadGraphicsWaitVsync();
        seadGraphicsSwapStatus(reinterpret_cast<u32*>(submitted.get()),reinterpret_cast<u32*>(swapped.get()),lastFlip.get(),lastVsync.get());
    } while(static_cast<u32>(*swapped)<static_cast<u32>(*submitted));
}
/* 0274C9A8 */
void sead::CafeGameFramework::clearTarget() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    DisplayFrame* target=gabi::call_ptr<DisplayFrame*>(method(frameworkTable(this),cGetClearTarget),this);
    if(target) {
        u32 clear=method(frameTable(target),cClearFrameBuffer);
        gabi::call_ptr<void>(clear,target,7,&graphicsParameters()->mClearColor,0,1.0f);
    }
}
static MethodTreeMgrBase* checkedGraphicsOwner(MethodTreeMgrBase* owner,bool definitionFirst) {
    if(!sMethodTreeMgrTypeGuard) {
        if(definitionFirst) {
            sMethodTreeMgrType.mTable=&sGraphicsOwnerDefinition;
            sMethodTreeMgrTypeGuard=1;
        } else {
            sMethodTreeMgrTypeGuard=1;
            sMethodTreeMgrType.mTable=&sGraphicsOwnerDefinition;
        }
    }
    if(owner) {
        u32 target=owner->mVtable->mIsKindOf.mFunction;
        if(gabi::call_ptr<u32>(target,owner,&sMethodTreeMgrType)) return owner;
    }
    return nullptr;
}
static CafeMethodTreeScreenState* graphicsOwnerFlags(MethodTreeMgrBase* owner) {
    return gabi::at<CafeMethodTreeScreenState>(gabi::ea(owner)+kMethodTreeScreenStateOffset);
}
/* 0274BCD8 */
sead::DisplayFrame* sead::CafeGameFramework::targetForPass(u32 pass) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(pass<2 || pass>10) return nullptr;
    if(pass==2) {
        MethodTreeMgrBase* owner=checkedGraphicsOwner(methodTreeManager(this),true);
        if(graphicsOwnerFlags(owner)->mAuxiliaryPass) return drcFrameBuffer(this);
    } else if(pass<=4) {
        MethodTreeMgrBase* owner=checkedGraphicsOwner(methodTreeManager(this),true);
        if(graphicsOwnerFlags(owner)->mAlternatePass) return drcFrameBuffer(this);
    } else if(pass>=8) return drcFrameBuffer(this);
    return tvFrameBuffer(this);
}
/* 0274BE28 */
sead::VirtualFrameBuffer* sead::CafeGameFramework::viewportForPass(u32 pass) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(pass<2 || pass>10) return nullptr;
    if(pass==2) {
        MethodTreeMgrBase* owner=checkedGraphicsOwner(methodTreeManager(this),true);
        if(graphicsOwnerFlags(owner)->mAuxiliaryPass) return &mPhysicalFrameBuffer;
    } else if(pass<=4) {
        MethodTreeMgrBase* owner=checkedGraphicsOwner(methodTreeManager(this),true);
        if(graphicsOwnerFlags(owner)->mAlternatePass) return &mPhysicalFrameBuffer;
    } else if(pass>=8) return &mPhysicalFrameBuffer;
    return &mVirtualFrameBuffer;
}
/* 0274BFD0 */
sead::MethodTreeMgrBase* sead::CafeGameFramework::createMethodTreeManager(RegistryHeap* heap) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    auto* owner=static_cast<MethodTreeMgrBase*>(seadAllocateOwnerStorage(kDualScreenMethodTreeManagerBytes,heap,4));
    if(owner) owner=seadInitializeGraphicsOwner(owner);
    graphicsOwnerFlags(owner)->mAuxiliaryPass=0;
    return owner;
}
/* 0274C7E4 */
void sead::CafeGameFramework::updateManager() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    seadUpdateGraphicsTaskManager(taskMgr());
    MethodTreeMgrBase* owner=checkedGraphicsOwner(methodTreeManager(this),false);
    seadUpdateGraphicsOwner(owner);
}

/* 0274C3B8 */
void sead::CafeGameFramework::drawTv() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    MethodTreeMgrBase* owner=checkedGraphicsOwner(methodTreeManager(this),false);
    seadGraphicsSampleTop(&graphicsTail()->mGpuQueries->mTimes[0]);
    DisplayFrame* target=tvFrameBuffer(this);
    if(target) sead::activateFrameBuffer(target);
    gabi::call_ptr<void>(method(frameworkTable(this),cDraw),this,6);
    seadDrawGraphicsOwnerTv(owner);
    seadGraphicsSampleBottom(&graphicsTail()->mGpuQueries->mTimes[1]);
}
/* 0274C484 */
void sead::CafeGameFramework::drawDrc() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    MethodTreeMgrBase* owner=checkedGraphicsOwner(methodTreeManager(this),false);
    seadGraphicsSampleTop(&graphicsTail()->mGpuQueries->mTimes[2]);
    DisplayFrame* target=drcFrameBuffer(this);
    if(target) sead::activateFrameBuffer(target);
    gabi::call_ptr<void>(method(frameworkTable(this),cDraw),this,9);
    seadDrawGraphicsOwnerDrc(owner);
    seadGraphicsSampleBottom(&graphicsTail()->mGpuQueries->mTimes[3]);
}
/* 0274C264 */
void sead::CafeGameFramework::processFrame() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    seadEnterGraphicsFrame(this);
    seadPrepareGraphicsTaskManager(taskMgr());
    gabi::call_ptr<void>(method(frameworkTable(this),cBeforeFrame),this);
    // HD: frame-exact for cross-build save states (the main thread runs the game below this call):
    // the original's return address and r30; the framework is taken back from r30 (after a state load
    // it is the snapshot's).
    gabi::call_ptr_site(0x0274C2B0,{{30,gabi::ea(this)}},{},method(frameworkTable(this),cFrameUpdate),this);
    CafeGameFramework* self=gabi::at<CafeGameFramework>(gabi::cpu->r[30]);
    gabi::call_ptr<void>(method(frameworkTable(self),cUpdate),self);
    u32 flags=self->mPendingFrame;
    bool render=(flags&3)!=3;
    u32 callback=self->mReserved4C;
    if(callback) gabi::call_ptr<void>(callback,1);
    if(render) {
        gabi::call_ptr<void>(method(frameworkTable(self),cPrepareRender),self);
        self->measureGpuTime();
    }
    callback=self->mReserved4C;
    if(callback) gabi::call_ptr<void>(callback,0);
    seadLeaveGraphicsFrame(self);
    auto* start=frameStartClock();
    auto elapsed=seadElapsedGraphicsTime(start);
    gabi::FrameLocal<GraphicsClock> temporary(8);
    temporary->mHigh=elapsed.r3;
    temporary->mLow=elapsed.r4;
    self->mFrameIndex=elapsed.r3;
    self->mFrameCount=elapsed.r4;
    auto time=seadGraphicsSystemTime();
    u32 state=self->mDisplayRunning;
    frameStartClock()->mLow=time.r4;
    frameStartClock()->mHigh=time.r3;
    flags=self->mPendingFrame;
    if(state==1) self->mDisplayRunning=2;
    VtableSlot* methods=frameworkTable(self);
    if(flags&1) self->mPendingFrame=flags^2;
    gabi::call_ptr<void>(method(methods,cAfterFrame),self);
}
/* 0274C67C */
void sead::CafeGameFramework::renderFrame() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 flags=mPendingFrame;
    if((flags&1) && !(flags&2)) return;
    u32 callback=mReserved4C;
    if(callback) gabi::call_ptr<void>(callback,1);
    if(mSurface) drawDrc(); else drawTv();
    if(!graphicsTail()->mDeferBinding) {
        if(mSurface) bindDrc(); else bindTv();
    }
    if(mSurface) drawTv(); else drawDrc();
    if(!graphicsTail()->mSwapCallback) {
        if(graphicsTail()->mDeferBinding) {
            if(mSurface) bindDrc(); else bindTv();
        }
        if(mSurface) bindTv(); else bindDrc();
    }
    seadGraphicsFlush();
    callback=mReserved4C;
    if(callback) gabi::call_ptr<void>(callback,0);
}
/* 0274C8C4 */
void sead::CafeGameFramework::swapBuffers() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if((static_cast<u32>(mPendingFrame)&3)==3) return;
    u32 callback=graphicsTail()->mSwapCallback;
    if(callback) {
        gabi::call_ptr<void>(callback,0);
        seadGraphicsDrawDone();
        if(graphicsTail()->mDeferBinding) {
            if(mSurface) bindDrc(); else bindTv();
        }
        if(mSurface) bindTv(); else bindDrc();
        seadGraphicsFlush();
        gabi::call_ptr<void>(graphicsTail()->mSwapCallback,1);
    }
    if(mDisplayRunning==2) seadGraphicsSwapBuffers();
    seadGraphicsDrawDone();
}
/* 0274C00C */
void sead::CafeGameFramework::threadLoop() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    // HD: frame-exact for cross-build save states: the original's return address and r31.
    CafeGameFramework* self=this;
    for(;;) {
        gabi::call_ptr_site(0x0274C034,{{31,gabi::ea(self)}},{},method(frameworkTable(self),cThreadBody),self);
        self=gabi::at<CafeGameFramework>(gabi::cpu->r[31]);
    }
}

/* 0274C038 */
void sead::CafeGameFramework::measureGpuTime() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    seadInvalidateGraphicsQueries(graphicsTail()->mGpuQueries,32);
    u32 order=mSurface;
    u32 first=order?2:0;
    u32 others[3]={order?3u:1u,order?0u:2u,order?1u:3u};
    GraphicsQueries* queries=graphicsTail()->mGpuQueries;
    u32 high=queries->mTimes[first].mHigh;
    u32 low=queries->mTimes[first].mLow;
    auto gpuStart=seadGraphicsGpuToCpu(high,low);
    u32 startLow=frameStartClock()->mLow;
    u32 startHigh=frameStartClock()->mHigh;
    u64 delta=((static_cast<u64>(gpuStart.r3)<<32)|gpuStart.r4)-
              ((static_cast<u64>(startHigh)<<32)|startLow);
    gabi::FrameLocal<GraphicsClock> scratch(8);
    gabi::FrameLocal<GraphicsClock> baseTime(0x10);
    gabi::FrameLocal<GraphicsClock> startTime(0x18);
    gabi::FrameLocal<GraphicsClock> startOffset(0x20);
    startTime->mLow=startLow;
    startTime->mHigh=startHigh;
    scratch->mLow=static_cast<u32>(delta);
    scratch->mHigh=static_cast<u32>(delta>>32);
    startOffset->mLow=scratch->mLow;
    startOffset->mHigh=scratch->mHigh;
    auto base=seadAddGraphicsClock(startTime.get(),startOffset.get());
    for(u32 index=0;index<3;++index) {
        queries=graphicsTail()->mGpuQueries;
        high=queries->mTimes[others[index]].mHigh;
        low=queries->mTimes[others[index]].mLow;
        if(index==0) {
            baseTime->mLow=base.r4;
            baseTime->mHigh=base.r3;
        }
        auto endpoint=seadGraphicsGpuToCpu(high,low);
        queries=graphicsTail()->mGpuQueries;
        high=queries->mTimes[first].mHigh;
        low=queries->mTimes[first].mLow;
        auto origin=seadGraphicsGpuToCpu(high,low);
        delta=((static_cast<u64>(endpoint.r3)<<32)|endpoint.r4)-
              ((static_cast<u64>(origin.r3)<<32)|origin.r4);
        scratch->mLow=static_cast<u32>(delta);
        scratch->mHigh=static_cast<u32>(delta>>32);
        gabi::FrameLocal<GraphicsClock> savedBase(0x28+index*0x10);
        gabi::FrameLocal<GraphicsClock> offset(0x30+index*0x10);
        savedBase->mHigh=baseTime->mHigh;
        savedBase->mLow=baseTime->mLow;
        offset->mHigh=scratch->mHigh;
        offset->mLow=scratch->mLow;
        seadAddGraphicsClock(savedBase.get(),offset.get());
    }
}

struct GraphicsInitAttributes { be<u32> mWords[9]; };
WWHD_SIZE(GraphicsInitAttributes,0x24);
static GraphicsInitAttributes sDefaultGraphicsAttributes;
static u8 sGraphicsDriverVtable;
static u8 sVirtualFrameBufferVtable;
static u8 sSurfaceTargetVtable;

/* inlined into CafeGameFramework::initializeGraphics */
static void initializeFrameBufferArea(FrameBufferArea* bounds,f32 width,f32 height) {
    if(0.0f<width) {
        bounds->mMaxX=width;
        bounds->mMinX=0.0f;
    } else {
        bounds->mMaxX=0.0f;
        bounds->mMinX=width;
    }
    if(0.0f<height) {
        bounds->mMaxY=height;
        bounds->mMinY=0.0f;
    } else {
        bounds->mMinY=height;
        bounds->mMaxY=0.0f;
    }
}
/* 0274B40C */
void sead::CafeGameFramework::initializeGraphics(RegistryHeap* heap,u32 argc,u32 argvAddress,const FrameBufferSize* tvSize,const FrameBufferSize* drcSize,RegistryHeap* scanHeap) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 allocate=heap->dispatch()->mAllocate.mFunction;
    void* commandMemory=gabi::call_ptr<void*>(allocate,heap,static_cast<u32>(graphicsParameters()->mCommandBytes),0x40);
    gabi::FrameLocal<GraphicsInitAttributes> attributes(0x50);
    graphicsStorage()->mCommandMemory=commandMemory;
    for(u32 word=0;word<9;++word) attributes->mWords[word]=sDefaultGraphicsAttributes.mWords[word];
    u32 commandBytes=graphicsParameters()->mCommandBytes;
    attributes->mWords[1]=gabi::ea(commandMemory);
    attributes->mWords[3]=commandBytes;
    attributes->mWords[7]=argvAddress;
    attributes->mWords[5]=argc;
    seadGraphicsInitialize(attributes.get());
    auto* driver=static_cast<DeviceStateCache_l*>(seadAllocateOwnerStorage(sizeof(DeviceStateCache_l),heap,4));
    if(driver) {
        seadClearDriverMemory(driver,sizeof(DeviceStateCache_l));
        seadInitializeDriverBase(driver);
        device(driver)->mTable=reinterpret_cast<GraphicsDeviceTable*>(&sGraphicsDriverVtable);
        auto* driverFlags=gabi::at<be<u32>>(gabi::ea(driver)+sizeof(GraphicsDevice));
        if(!driverFlags) driverFlags=static_cast<be<u32>*>(seadAllocateViewStorage(4));
        if(driverFlags) *driverFlags=0;
        auto* blend=gabi::at<BlendState>(gabi::ea(driver)+offsetof(DeviceStateCache_l,mDriverState));
        if(!blend) blend=static_cast<BlendState*>(seadAllocateViewStorage(8*sizeof(BlendState)));
        if(blend) seadConstructBlendArray(blend,8,sizeof(BlendState),gabi::guest_fn(reinterpret_cast<const void*>(&createBlendChannels)));
        auto* dirty=gabi::at<be<u32>>(gabi::ea(driver)+offsetof(DeviceStateCache_l,mBlendMask));
        if(!dirty) dirty=static_cast<be<u32>*>(seadAllocateViewStorage(4));
        if(dirty) *dirty=0;
    }
    sCurrentGraphicsDevice=reinterpret_cast<GraphicsDevice*>(driver);
    seadResetGraphicsDriver(driver);
    allocate=heap->dispatch()->mAllocate.mFunction;
    void* context=gabi::call_ptr<void*>(allocate,heap,0xA100,0x100);
    seadGraphicsInitializeContext(context,0);
    reinterpret_cast<DeviceStateCache_l*>(sCurrentGraphicsDevice.get())->mContext=context;
    gabi::FrameLocal<GraphicsClock> conversion(8);
    gabi::FrameLocal<FrameBufferSize> targetCopy(0x40);
    gabi::FrameLocal<FrameBufferSize> targetDimensions(0x28);
    gabi::FrameLocal<be<u32>> hiZSize(0x3C),hiZAlignment(0x38);
    gabi::FrameLocal<be<u32>> drcHiZSize(0x48),drcHiZAlignment(0x4C);
    if(graphicsParameters()->mAllocateTargets) {
        u32 width=graphicsParameters()->mDisplayWidth;
        u32 height=graphicsParameters()->mDisplayHeight;
        graphicsStorage()->mTvColor.mSurface.mUsage=3;
        graphicsStorage()->mTvColor.mSurface.mWidth=width;
        graphicsStorage()->mTvColor.mSurface.mHeight=height;
        graphicsStorage()->mTvColor.mSurface.mDepth=1;
        graphicsStorage()->mTvColor.mSurface.mMipCount=1;
        graphicsStorage()->mTvColor.mSurface.mFormat=0x1A;
        graphicsStorage()->mTvColor.mSurface.mSamples=0;
        graphicsStorage()->mTvColor.mSurface.mTileMode=0;
        graphicsStorage()->mTvColor.mSurface.mSwizzle=0;
        graphicsStorage()->mTvColor.mViewMip=0;
        graphicsStorage()->mTvColor.mFirstSlice=0;
        graphicsStorage()->mTvColor.mSurface.mDimension=1;
        graphicsStorage()->mTvColor.mSliceCount=1;
        seadGraphicsSurfaceLayout(&graphicsStorage()->mTvColor.mSurface);
        seadGraphicsColorRegisters(&graphicsStorage()->mTvColor);
        u32 bytes=graphicsStorage()->mTvColor.mSurface.mImageBytes;
        u32 alignment=graphicsStorage()->mTvColor.mSurface.mAlignment;
        void* image=allocateGraphicsFrame(bytes,alignment);
        width=graphicsParameters()->mDisplayWidth;
        height=graphicsParameters()->mDisplayHeight;
        graphicsStorage()->mTvDepth.mSurface.mWidth=width;
        graphicsStorage()->mTvDepth.mSurface.mHeight=height;
        graphicsStorage()->mTvDepth.mSurface.mDepth=1;
        graphicsStorage()->mTvColor.mSurface.mImage=image;
        graphicsStorage()->mTvDepth.mSurface.mMipCount=1;
        graphicsStorage()->mTvDepth.mSurface.mFormat=0x80E;
        graphicsStorage()->mTvDepth.mSurface.mSamples=0;
        graphicsStorage()->mTvDepth.mSurface.mUsage=5;
        graphicsStorage()->mTvDepth.mSurface.mTileMode=0;
        graphicsStorage()->mTvDepth.mSurface.mSwizzle=0;
        graphicsStorage()->mTvDepth.mViewMip=0;
        graphicsStorage()->mTvDepth.mFirstSlice=0;
        graphicsStorage()->mTvDepth.mSliceCount=1;
        graphicsStorage()->mTvDepth.mHiZ=nullptr;
        graphicsStorage()->mTvDepth.mHiZBytes=0;
        graphicsStorage()->mTvDepth.mDepthClear=1.0f;
        graphicsStorage()->mTvDepth.mSurface.mDimension=1;
        graphicsStorage()->mTvDepth.mStencilClear=0;
        seadGraphicsSurfaceLayout(&graphicsStorage()->mTvDepth.mSurface);
        seadGraphicsDepthRegisters(&graphicsStorage()->mTvDepth);
        alignment=graphicsStorage()->mTvDepth.mSurface.mAlignment;
        bytes=graphicsStorage()->mTvDepth.mSurface.mImageBytes;
        graphicsStorage()->mTvDepth.mSurface.mImage=allocateGraphicsFrame(bytes,alignment);
        seadGraphicsHiZInfo(&graphicsStorage()->mTvDepth,reinterpret_cast<u32*>(hiZSize.get()),reinterpret_cast<u32*>(hiZAlignment.get()));
        alignment=*hiZAlignment;
        bytes=*hiZSize;
        void* hiZ=allocateGraphicsFrame(bytes,alignment);
        graphicsStorage()->mTvDepth.mHiZ=hiZ;
        seadGraphicsHiZEnable(&graphicsStorage()->mTvDepth,gabi::ea(hiZ)!=0);
        graphicsStorage()->mDrcColor.mSurface.mDimension=1;
        graphicsStorage()->mDrcColor.mSurface.mUsage=3;
        graphicsStorage()->mDrcColor.mSurface.mWidth=854;
        graphicsStorage()->mDrcColor.mSurface.mHeight=480;
        graphicsStorage()->mDrcColor.mSurface.mDepth=1;
        graphicsStorage()->mDrcColor.mSurface.mMipCount=1;
        graphicsStorage()->mDrcColor.mSurface.mFormat=0x1A;
        graphicsStorage()->mDrcColor.mSurface.mSamples=0;
        graphicsStorage()->mDrcColor.mSurface.mTileMode=0;
        graphicsStorage()->mDrcColor.mSurface.mSwizzle=0;
        graphicsStorage()->mDrcColor.mViewMip=0;
        graphicsStorage()->mDrcColor.mFirstSlice=0;
        graphicsStorage()->mDrcColor.mSliceCount=1;
        seadGraphicsSurfaceLayout(&graphicsStorage()->mDrcColor.mSurface);
        seadGraphicsColorRegisters(&graphicsStorage()->mDrcColor);
        graphicsStorage()->mDrcDepth.mSurface.mWidth=854;
        graphicsStorage()->mDrcDepth.mSurface.mHeight=480;
        graphicsStorage()->mDrcDepth.mSurface.mDepth=1;
        graphicsStorage()->mDrcDepth.mSurface.mMipCount=1;
        graphicsStorage()->mDrcDepth.mSurface.mFormat=0x80E;
        graphicsStorage()->mDrcDepth.mSurface.mSamples=0;
        graphicsStorage()->mDrcDepth.mSurface.mUsage=5;
        graphicsStorage()->mDrcDepth.mSurface.mTileMode=0;
        graphicsStorage()->mDrcDepth.mSurface.mSwizzle=0;
        graphicsStorage()->mDrcDepth.mViewMip=0;
        graphicsStorage()->mDrcDepth.mFirstSlice=0;
        graphicsStorage()->mDrcDepth.mSliceCount=1;
        graphicsStorage()->mDrcDepth.mHiZ=nullptr;
        graphicsStorage()->mDrcDepth.mHiZBytes=0;
        void* sharedColor=graphicsStorage()->mTvColor.mSurface.mImage;
        graphicsStorage()->mDrcDepth.mStencilClear=0;
        graphicsStorage()->mDrcColor.mSurface.mImage=sharedColor;
        graphicsStorage()->mDrcDepth.mDepthClear=1.0f;
        graphicsStorage()->mDrcDepth.mSurface.mDimension=1;
        seadGraphicsSurfaceLayout(&graphicsStorage()->mDrcDepth.mSurface);
        seadGraphicsDepthRegisters(&graphicsStorage()->mDrcDepth);
        void* sharedDepth=graphicsStorage()->mTvDepth.mSurface.mImage;
        graphicsStorage()->mDrcDepth.mSurface.mImage=sharedDepth;
        seadGraphicsHiZInfo(&graphicsStorage()->mDrcDepth,reinterpret_cast<u32*>(drcHiZSize.get()),reinterpret_cast<u32*>(drcHiZAlignment.get()));
        void* sharedHiZ=graphicsStorage()->mTvDepth.mHiZ;
        graphicsStorage()->mDrcDepth.mHiZ=sharedHiZ;
        seadGraphicsHiZEnable(&graphicsStorage()->mDrcDepth,gabi::ea(sharedHiZ)!=0);
        auto* tvTarget=static_cast<DisplayFrame*>(seadAllocateOwnerStorage(sizeof(DisplayFrame),heap,4));
        if(tvTarget) {
            bool defaultSize=tvSize->mWidth==0.0f && tvSize->mHeight==0.0f;
            width=graphicsParameters()->mDisplayWidth;
            height=graphicsParameters()->mDisplayHeight;
            conversion->mHigh=0x43300000;
            conversion->mLow=width;
            f32 boundsWidth=static_cast<f32>(width);
            conversion->mLow=height;
            f32 boundsHeight=static_cast<f32>(height);
            if(defaultSize) {
                targetDimensions->mWidth=boundsWidth;
                targetDimensions->mHeight=boundsHeight;
                targetCopy->mWidth=static_cast<f32>(targetDimensions->mWidth);
                targetCopy->mHeight=static_cast<f32>(targetDimensions->mHeight);
            } else {
                targetCopy->mWidth=static_cast<f32>(tvSize->mWidth);
                targetCopy->mHeight=static_cast<f32>(tvSize->mHeight);
            }
            logicalFrame(tvTarget)->mVtable=&sVirtualFrameBufferVtable;
            logicalFrame(tvTarget)->mSize.mWidth=static_cast<f32>(targetCopy->mWidth);
            logicalFrame(tvTarget)->mSize.mHeight=static_cast<f32>(targetCopy->mHeight);
            auto* bounds=gabi::at<FrameBufferArea>(gabi::ea(tvTarget)+offsetof(VirtualFrameBuffer,mArea));
            if(!bounds) bounds=static_cast<FrameBufferArea*>(seadAllocateViewStorage(16));
            if(bounds) initializeFrameBufferArea(bounds,boundsWidth,boundsHeight);
            logicalFrame(tvTarget)->mVtable=&sSurfaceTargetVtable;
            tvTarget->mColor=reinterpret_cast<ColorBuffer_l*>(&graphicsStorage()->mTvColor);
            tvTarget->mDepth=&graphicsStorage()->mTvDepth;
        }
        tvFrameBuffer(this)=tvTarget;
        auto* drcTarget=static_cast<DisplayFrame*>(seadAllocateOwnerStorage(sizeof(DisplayFrame),heap,4));
        if(drcTarget) {
            logicalFrame(drcTarget)->mVtable=&sVirtualFrameBufferVtable;
            logicalFrame(drcTarget)->mSize.mWidth=static_cast<f32>(drcSize->mWidth);
            logicalFrame(drcTarget)->mSize.mHeight=static_cast<f32>(drcSize->mHeight);
            auto* bounds=gabi::at<FrameBufferArea>(gabi::ea(drcTarget)+offsetof(VirtualFrameBuffer,mArea));
            if(!bounds) bounds=static_cast<FrameBufferArea*>(seadAllocateViewStorage(16));
            if(bounds) {
                bounds->mMaxX=854.0f;
                bounds->mMinY=0.0f;
                bounds->mMinX=0.0f;
                bounds->mMaxY=480.0f;
            }
            logicalFrame(drcTarget)->mVtable=&sSurfaceTargetVtable;
            drcTarget->mColor=reinterpret_cast<ColorBuffer_l*>(&graphicsStorage()->mDrcColor);
            drcTarget->mDepth=&graphicsStorage()->mDrcDepth;
        }
        DisplayFrame* target=tvFrameBuffer(this);
        drcFrameBuffer(this)=drcTarget;
        sead::activateFrameBuffer(target);
    }
    u32 highDefinition=static_cast<u32>(graphicsParameters()->mDisplayHeight)!=720;
    auto* tvScan=static_cast<ScanTarget*>(seadAllocateOwnerStorage(sizeof(ScanTarget),heap,4));
    if(tvScan) tvScan=seadConstructScanBuffer(tvScan,0,highDefinition);
    u32 width=graphicsParameters()->mDisplayWidth;
    u32 height=graphicsParameters()->mDisplayHeight;
    conversion->mHigh=0x43300000;
    conversion->mLow=width;
    f32 scanWidth=static_cast<f32>(width);
    conversion->mLow=height;
    graphicsStorage()->mTvBuffer=tvScan;
    VtableSlot* scanMethods=scanTable(tvScan);
    f32 scanHeight=static_cast<f32>(height);
    tvScan->mHeight=scanHeight;
    tvScan->mWidth=scanWidth;
    u32 initialize=method(scanMethods,cAllocateScan);
    gabi::call_ptr<void>(initialize,tvScan,scanHeap?scanHeap:heap);
    auto* drcScan=static_cast<ScanTarget*>(seadAllocateOwnerStorage(sizeof(ScanTarget),heap,4));
    if(drcScan) drcScan=seadConstructScanBuffer(drcScan,1,highDefinition);
    graphicsStorage()->mDrcBuffer=drcScan;
    scanMethods=scanTable(drcScan);
    drcScan->mHeight=480.0f;
    drcScan->mWidth=854.0f;
    initialize=method(scanMethods,cAllocateScan);
    gabi::call_ptr<void>(initialize,drcScan,scanHeap?scanHeap:heap);
    allocate=heap->dispatch()->mAllocate.mFunction;
    graphicsTail()->mGpuQueries=static_cast<GraphicsQueries*>(gabi::call_ptr<void*>(allocate,heap,0x40,0x40));
    seadGraphicsSetSwapInterval(graphicsParameters()->mWaitVsync);
    bool defaultViewport=tvSize->mWidth==0.0f && tvSize->mHeight==0.0f;
    gabi::FrameLocal<FrameBufferSize> viewportDimensions(0x20),viewportCopy(0x30);
    width=graphicsParameters()->mDisplayWidth;
    height=graphicsParameters()->mDisplayHeight;
    conversion->mHigh=0x43300000;
    conversion->mLow=width;
    f32 boundsWidth=static_cast<f32>(width);
    conversion->mLow=height;
    f32 boundsHeight=static_cast<f32>(height);
    if(defaultViewport) {
        viewportDimensions->mWidth=boundsWidth;
        viewportDimensions->mHeight=boundsHeight;
        viewportCopy->mWidth=static_cast<f32>(viewportDimensions->mWidth);
        viewportCopy->mHeight=static_cast<f32>(viewportDimensions->mHeight);
    } else {
        viewportCopy->mWidth=static_cast<f32>(tvSize->mWidth);
        viewportCopy->mHeight=static_cast<f32>(tvSize->mHeight);
    }
    mVirtualFrameBuffer.mSize.mWidth=static_cast<f32>(viewportCopy->mWidth);
    mVirtualFrameBuffer.mSize.mHeight=static_cast<f32>(viewportCopy->mHeight);
    gabi::FrameLocal<FrameBufferArea> boundsCopy(0x10);
    f32 left;
    if(0.0f<boundsWidth) {
        boundsCopy->mMaxX=boundsWidth;
        boundsCopy->mMinX=0.0f;
        left=0.0f;
    } else {
        boundsCopy->mMaxX=0.0f;
        boundsCopy->mMinX=boundsWidth;
        left=boundsWidth;
    }
    f32 right=boundsCopy->mMaxX;
    mVirtualFrameBuffer.mArea.mMinX=left;
    mVirtualFrameBuffer.mArea.mMaxX=right;
    if(0.0f<boundsHeight) {
        mVirtualFrameBuffer.mArea.mMinY=0.0f;
        mVirtualFrameBuffer.mArea.mMaxY=boundsHeight;
        f32 drcWidth=drcSize->mWidth;
        boundsCopy->mMinY=0.0f;
        mPhysicalFrameBuffer.mSize.mWidth=drcWidth;
        f32 drcHeight=drcSize->mHeight;
        boundsCopy->mMaxY=boundsHeight;
        mPhysicalFrameBuffer.mSize.mHeight=drcHeight;
    } else {
        mVirtualFrameBuffer.mArea.mMinY=boundsHeight;
        mVirtualFrameBuffer.mArea.mMaxY=0.0f;
        f32 drcWidth=drcSize->mWidth;
        boundsCopy->mMinY=boundsHeight;
        mPhysicalFrameBuffer.mSize.mWidth=drcWidth;
        f32 drcHeight=drcSize->mHeight;
        boundsCopy->mMaxY=0.0f;
        mPhysicalFrameBuffer.mSize.mHeight=drcHeight;
    }
    mPhysicalFrameBuffer.mArea.mMinX=0.0f;
    mPhysicalFrameBuffer.mArea.mMaxX=854.0f;
    mPhysicalFrameBuffer.mArea.mMinY=0.0f;
    mPhysicalFrameBuffer.mArea.mMaxY=480.0f;
}

/* WWHD glue
data sDefaultGraphicsAttributes 10144F78
data sGraphicsDriverVtable 101456CC
data sVirtualFrameBufferVtable 10144F30
data sSurfaceTargetVtable 10145660
ext C00063A8 void seadGraphicsInitialize(void*)
ext 028F521C void seadClearDriverMemory(void*,u32)
ext 0274FB24 void seadInitializeDriverBase(void*)
ext 028EFFD0 void seadConstructBlendArray(sead::BlendState*,u32,u32,u32)
ext 0274FD64 void seadResetGraphicsDriver(void*)
ext C0006A60 void seadGraphicsInitializeContext(void*,u32)
ext C00060D0 void seadGraphicsSurfaceLayout(sead::GraphicsSurface*)
ext C00063D0 void seadGraphicsColorRegisters(sead::GraphicsColorBuffer*)
ext C00063F0 void seadGraphicsDepthRegisters(sead::GraphicsDepthBuffer*)
ext C00060B0 void seadGraphicsHiZInfo(sead::GraphicsDepthBuffer*,u32*,u32*)
ext C00063E8 void seadGraphicsHiZEnable(sead::GraphicsDepthBuffer*,u32)
ext 027510FC sead::ScanTarget* seadConstructScanBuffer(sead::ScanTarget*,u32,u32)
ext C00069C0 void seadGraphicsSetSwapInterval(u32)

ext C00088A8 void seadInvalidateGraphicsQueries(sead::GraphicsQueries*,u32)
ext C00061B8 Pair32 seadGraphicsGpuToCpu(u32,u32)
ext 02760EB8 Pair32 seadAddGraphicsClock(const sead::GraphicsClock*,const sead::GraphicsClock*)

ext C0006760 void seadGraphicsSampleTop(sead::GraphicsClock*)
ext C0006748 void seadGraphicsSampleBottom(sead::GraphicsClock*)
ext 02746798 void seadDrawGraphicsOwnerTv(sead::MethodTreeMgrBase*)
ext 027467A0 void seadDrawGraphicsOwnerDrc(sead::MethodTreeMgrBase*)
ext 0274B054 void seadEnterGraphicsFrame(sead::CafeGameFramework*)
ext 0274B06C void seadLeaveGraphicsFrame(sead::CafeGameFramework*)
ext 0274A5EC void seadPrepareGraphicsTaskManager(sead::TaskMgr*)
ext 02760E58 Pair32 seadElapsedGraphicsTime(const sead::GraphicsClock*)
ext C00061A8 void seadGraphicsFlush()
ext C0006168 void seadGraphicsDrawDone()
ext C0006A90 void seadGraphicsSwapBuffers()

data sGraphicsOwnerDefinition 10144F50
ext 0273B050 void* seadAllocateOwnerStorage(u32,void*,s32)
ext 02745894 sead::MethodTreeMgrBase* seadInitializeGraphicsOwner(sead::MethodTreeMgrBase*)
ext 0274A414 void seadUpdateGraphicsTaskManager(sead::TaskMgr*)
ext 02746790 void seadUpdateGraphicsOwner(sead::MethodTreeMgrBase*)

ext C0009C98 Pair32 seadGraphicsSystemTime()
ext C00067E0 void seadGraphicsSetContext(void*)
ext C0006AE8 void seadGraphicsWaitVsync()
ext C0006348 void seadGraphicsSwapStatus(u32*,u32*,void*,void*)

*/
