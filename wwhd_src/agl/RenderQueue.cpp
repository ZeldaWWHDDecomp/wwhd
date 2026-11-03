// AGL queue selection and registration, reconstructed from assigned WWHD instructions.
#include "agl/RenderQueue.h"
#include "sead/DisplayTargets.h"
namespace agl {}
using namespace agl;
static sead::DisposerVtable sQueueConfigurationMethods;
void prepareRenderView(RenderViewContext_l*);
void executeRenderView(RenderViewContext_l*,sead::PointerBuffer*);
void removeRenderViewLayer(RenderViewContext_l*,RenderLayerPrefix*);
u32 registerRenderMethodResource(RenderMethodResource_l*,void*);
u32 countRenderMethodResource(RenderMethodResource_l*,void*);
static RenderViewContext_l* indexedView(const RenderQueueManager* manager,u32 index) {
    return gabi::at<RenderViewContext_l>(gabi::ea(manager)+offsetof(RenderQueueManager,mViews)+index*sizeof(RenderViewContext_l));
}
static RenderMethodResource_l* indexedResource(RenderMethodResource_l* base,u32 index) {
    return gabi::at<RenderMethodResource_l>(gabi::ea(base)+index*sizeof(RenderMethodResource_l));
}
/* 027C087C */
u32 agl::compareRenderOrder(const RenderOrderPrefix* left,const RenderOrderPrefix* right) {
    u32 a=left->mSortKey;u32 b=right->mSortKey;return a-b;
}
/* 027C1834 */
u32 agl::RenderQueueManager::isReady() const { return mState==1; }
/* 027C1A24 */
void agl::RenderQueueManager::prepareView(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(isReady()) prepareRenderView(indexedView(this,index));
}
/* 027C1A60 */
void agl::RenderQueueManager::executeView(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 interval=mFrameInterval;u32 frameIndex=mFrameIndex;
    u32 quotient=interval ? frameIndex/interval : 0;
    if(frameIndex-quotient*interval) return;
    if(!isReady()) return;
    u32 flags=mFlags;
    if(flags&(1u<<5)) return;
    sead::PointerBuffer* pending=(flags&(1u<<2)) ? &mPendingQueues[2] : nullptr;
    executeRenderView(indexedView(this,index),pending);
}
/* 027C1C64 */
void agl::RenderQueueManager::setViewOutput(u32 kind,u32 output) {
    switch(kind) {
    case 0:mViews[0].mPrimaryOutput=output;break;
    case 1:mViews[1].mPrimaryOutput=output;break;
    case 2:mViews[0].mSecondaryOutput=output;break;
    case 3:mViews[1].mSecondaryOutput=output;break;
    case 4:mViews[0].mSharedOutput=output;mViews[1].mSharedOutput=output;break;
    default:break;
    }
}
/* 027C1CBC */
s32 agl::RenderQueueManager::findLayer(RenderLayerPrefix* layer) const {
    if(!layer) return -1;
    u32 count=mLayerCount;gptr<RenderLayerPrefix>* current=mLayers;
    u32 begin=gabi::ea(current);u32 end=begin+count*4;
    while(gabi::ea(current)!=end) {
        if(static_cast<RenderLayerPrefix*>(*current)==layer) return (gabi::ea(current)-begin)>>2;
        current=gabi::at<gptr<RenderLayerPrefix>>(gabi::ea(current)+4);
    }
    return -1;
}
/* 027C1D0C */
u32 agl::RenderQueueManager::removeLayer(RenderLayerPrefix* layer) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 index=findLayer(layer);if(index==-1) return 0;
    u32 count=mLayerCount;gptr<RenderLayerPrefix>* slot=mLayers;
    if(static_cast<u32>(index)<count) slot=gabi::at<gptr<RenderLayerPrefix>>(gabi::ea(slot)+static_cast<u32>(index)*4);
    *slot=nullptr;
    removeRenderViewLayer(&mViews[0],layer);
    removeRenderViewLayer(&mViews[1],layer);
    mFlags=static_cast<u16>(mFlags)|1;return 1;
}
/* 027C1DD8 */
void agl::RenderQueueManager::retireOwner(void* token) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count=mLayerCount;gptr<RenderLayerPrefix>* current=mLayers;
    u32 end=gabi::ea(current)+count*4;
    while(gabi::ea(current)!=end) {
        RenderLayerPrefix* layer=*current;
        if(layer) layer->countResourceReferences(token);
        current=gabi::at<gptr<RenderLayerPrefix>>(gabi::ea(current)+4);
    }
}
/* 027C1F70 */
void agl::RenderQueueManager::setMode(u32 mode) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(mMode==mode) return;
    mMode=mode;mPendingFrames=60;
    if(mode==2) return;
    u32 enabled=mode==1;u32 count=mLayerCount;
    gptr<RenderLayerPrefix>* current=mLayers;u32 end=gabi::ea(current)+count*4;
    while(gabi::ea(current)!=end) {
        RenderLayerPrefix* layer=*current;
        if(layer) layer->setCameraMode(enabled);
        current=gabi::at<gptr<RenderLayerPrefix>>(gabi::ea(current)+4);
    }
}
/* 027C2060 */
void agl::RenderQueueManager::attachLayer(RenderLayerPrefix* layer,u32 index,u32 reserved,u32 view,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    layer->mManager=this;u32 flags=layer->mFlags;
    layer->mManagerIndex=index;layer->mFlags=flags|0x20;
    layer->setView(view);layer->prepareResources(heap);
    RenderViewContext_l* context=indexedView(this,view);
    layer->mViewport.mLeft=context->mBounds.mLeft.value;
    layer->mViewport.mBottom=context->mBounds.mTop.value;
    layer->mViewport.mRight=context->mBounds.mRight.value;
    layer->mViewport.mTop=context->mBounds.mBottom.value;
    layer->mViewport.mOrientation=context->mBounds.mDeviceMode;
    u32 count=mLayerCount;gptr<RenderLayerPrefix>* slot=mLayers;
    if(index<count) slot=gabi::at<gptr<RenderLayerPrefix>>(gabi::ea(slot)+index*4);
    *slot=layer;mFlags=static_cast<u16>(mFlags)|1;
}
/* 027C21C4 */
agl::RenderQueueConfiguration* agl::RenderQueueConfiguration::create(RenderQueueConfiguration* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!storage) storage=static_cast<RenderQueueConfiguration*>(allocateRegistryStorage(sizeof(RenderQueueConfiguration)));
    if(storage) {
        storage->mVtable=&sQueueConfigurationMethods;
        storage->mLayerCapacity=32;storage->mCommandBytes=0x400000;storage->mMode=2;
        be<u32>* flags=&storage->mFlags;
        if(!gabi::ea(flags)) flags=static_cast<be<u32>*>(allocateRegistryStorage(4));
        if(flags) *flags=0;
        storage->mCallback=nullptr;
        for(auto& size:storage->mOutputSizes) { size.mWidth=1280;size.mHeight=720; }
        for(auto& size:storage->mTargetSizes) { size.mWidth=1280;size.mHeight=720; }
    }
    return storage;
}
/* 027C22D0 */
void agl::RenderQueueConfiguration::setDimensions(u32 index,const RenderDimension* output,const RenderDimension* target) {
    u32 width=output->mWidth;
    RenderDimension* destination=&mOutputSizes[index<2 ? index : 0];
    destination->mWidth=width;destination->mHeight=output->mHeight;
    width=target->mWidth;destination=&mTargetSizes[index<2 ? index : 0];
    destination->mWidth=width;destination->mHeight=target->mHeight;
}
/* 027C23D4 */
void agl::RenderLayerPrefix::setView(u32 view) {
    u32 previous=mView;if(previous==view) return;
    RenderQueueManager* manager=mManager;mView=view;
    manager->mFlags=static_cast<u16>(manager->mFlags)|1;
}
/* 027C23F8 */
void* agl::RenderLayerPrefix::registerResourceMethod(u32 index,void* token) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count=mResourceCount;RenderMethodResource_l* resource=mResources;
    if(index<count) resource=indexedResource(resource,index);
    return registerRenderMethodResource(resource,token) ? token : nullptr;
}
/* 027C2450 */
void* agl::RenderLayerPrefix::registerMethodAcrossResources(void* token) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count=mResourceCount;RenderMethodResource_l* current=mResources;
    u32 begin=gabi::ea(current);u32 end=begin+count*sizeof(RenderMethodResource_l);u32 found=0;
    while(gabi::ea(current)!=end) {
        u32 offset=gabi::ea(current)-begin;
        u32 index=static_cast<u32>((static_cast<u64>(offset)*0x7D8C42B3u)>>32)>>9;
        found|=registerResourceMethod(index,token)!=nullptr;
        current=indexedResource(current,1);
    }
    return found ? token : nullptr;
}
/* 027C24E4 */
u32 agl::RenderLayerPrefix::countResourceReferences(void* token) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count=mResourceCount;RenderMethodResource_l* current=mResources;
    u32 end=gabi::ea(current)+count*sizeof(RenderMethodResource_l);u32 matches=0;
    while(gabi::ea(current)!=end) { matches+=countRenderMethodResource(current,token);current=indexedResource(current,1); }
    return matches;
}
/* 027C2560 */
u32 agl::RenderLayerPrefix::countResourceReference(u32 index,void* token) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count=mResourceCount;RenderMethodResource_l* resource=mResources;
    if(index<count) resource=indexedResource(resource,index);
    return countRenderMethodResource(resource,token);
}
/* 027C2580 */
void* agl::RenderLayerPrefix::viewStateHandle() const {
    s32 view=mViewOverride;RenderQueueManager* manager=mManager;
    if(view==-1) view=mView;
    return gabi::at<void>(gabi::ea(manager)+offsetof(RenderQueueManager,mViews)+static_cast<u32>(view)*sizeof(RenderViewContext_l)+offsetof(RenderViewContext_l,mAccessorOffset));
}
static RenderQueueManager* sAglRenderQueueManager;
static sead::Disposer* sAglRegisteredRenderManager;
static sead::DisposerVtable sRenderOwnerDisposerMethods;
static sead::DisposerVtable sRenderManagerDisposerMethods;
static sead::SafeTextVtable sRenderQueueNameMethods;
typedef gptr<be<u8>> RenderQueueNamePointer;
static const RenderQueueNamePointer sRenderQueueNameFirst;
struct RenderManagerLifetimeMethods { sead::VtableSlot mHeader,mType,mDestroy; };
void finishRenderDisposer(sead::Disposer*,u32);
void constructRenderDisposer(sead::Disposer*,sead::RegistryHeap*,u32);
void* allocateRenderManagerStorage(u32,sead::RegistryHeap*,s32);
s32 compareQueuedRenderEntries(void*,void*);
/* 027C0804 */
void agl::RenderOwnerDisposer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!gabi::ea(this)) return;
    mVtable=&sRenderOwnerDisposerMethods;
    RenderQueueManager* manager=sAglRenderQueueManager;
    if(manager) manager->retireOwner(this);
    finishRenderDisposer(this,0);
    if(flags&1) releaseRegistryStorage(this);
}
/* 027C211C */
void agl::RenderManagerDisposer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!gabi::ea(this)) return;
    mVtable=&sRenderManagerDisposerMethods;
    if(static_cast<sead::Disposer*>(sAglRegisteredRenderManager)==this) {
        RenderQueueManager* manager=sAglRenderQueueManager;
        sAglRegisteredRenderManager=nullptr;
        auto* methods=gabi::at<RenderManagerLifetimeMethods>(gabi::ea(static_cast<void*>(manager->mVtable)));
        u32 target=methods->mDestroy.mFunction;
        gabi::call_ptr<void>(target,manager,2);
        sAglRenderQueueManager=nullptr;
    }
    finishRenderDisposer(this,0);
    if(flags&1) releaseRegistryStorage(this);
}
/* 027C0A60 */
agl::RenderQueueManager* agl::RenderQueueManager::instance(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    RenderQueueManager* manager=sAglRenderQueueManager;
    if(!manager) {
        manager=static_cast<RenderQueueManager*>(allocateRenderManagerStorage(sizeof(RenderQueueManager),heap,4));
        sead::Disposer* disposer=gabi::at<sead::Disposer>(gabi::ea(manager)+offsetof(RenderQueueManager,mDisposer));
        if(disposer) { constructRenderDisposer(disposer,heap,3);disposer->mVtable=&sRenderManagerDisposerMethods; }
        sAglRegisteredRenderManager=disposer;
        if(manager) manager=create(manager);
        sAglRenderQueueManager=manager;
    }
    return manager;
}
/* 027C2008 */
sead::SafeText* agl::RenderQueueManager::createName(sead::SafeText* storage,u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u8>* text=*gabi::at<const RenderQueueNamePointer>(gabi::ea(&sRenderQueueNameFirst)+index*4);
    if(!storage) storage=static_cast<sead::SafeText*>(allocateRegistryStorage(sizeof(sead::SafeText)));
    if(storage) { storage->mText=text;storage->mVtable=&sRenderQueueNameMethods; }
    return storage;
}
/* 027C1E48 */
sead::JobQueue* agl::RenderQueueManager::selectQueue(u32 group) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 index=group<3 ? group : 0;
    sead::PointerBuffer* pending=&mPendingQueues[index];
    sead::JobQueue* queue=&mQueues[index];
    u32 count=gabi::call_ptr<u32>(queue->mTable->mTotal,queue);
    if(count!=static_cast<u32>(pending->mCount)) {
        pending->heapSort(gabi::guest_fn(reinterpret_cast<const void*>(&compareQueuedRenderEntries)));
        u32 total=pending->mCount;gptr<void>* current=static_cast<gptr<void>*>(static_cast<void*>(pending->mStorage));
        u32 end=gabi::ea(current)+total*4;
        while(gabi::ea(current)!=end) {
            queue->append(gabi::ea(static_cast<void*>(*current)));
            current=gabi::at<gptr<void>>(gabi::ea(current)+4);
        }
    }
    return queue;
}
static RenderManagerLifetimeMethods sRenderQueueManagerMethods;
void initializeRenderQueueArray(void*,u32,u32,u32);
void* createRenderDependencyGraph(RenderDependencyGraphStorage_l*);
RenderViewContext_l* constructRenderView(RenderViewContext_l*);
/* 027C088C */
agl::RenderQueueManager* agl::RenderQueueManager::create(RenderQueueManager* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!storage) storage=static_cast<RenderQueueManager*>(allocateRegistryStorage(sizeof(RenderQueueManager)));
    if(storage) {
        storage->mContext=nullptr;storage->mVtable=&sRenderQueueManagerMethods;
        void* views=storage->mViews;
        if(!gabi::ea(views)) views=allocateRegistryStorage(sizeof(storage->mViews));
        if(views) initializeRenderQueueArray(views,2,sizeof(RenderViewContext_l),gabi::guest_fn(reinterpret_cast<const void*>(&constructRenderView)));
        if(!gabi::ea(&storage->mOutputs[0])) allocateRegistryStorage(8);
        be<u32>* layers=&storage->mLayerCount;
        if(!gabi::ea(layers)) layers=static_cast<be<u32>*>(allocateRegistryStorage(8));
        if(layers) { layers[1]=0;layers[0]=0; }
        createRenderDependencyGraph(&storage->mDependencyGraph);
        storage->mColorFormat=0x1D;storage->mDepthFormat=0x3A;
        be<u16>* flags=&storage->mFlags;
        if(!gabi::ea(flags)) flags=static_cast<be<u16>*>(allocateRegistryStorage(2));
        if(flags) *flags=0x1878;
        storage->mState=0;
        if(!gabi::ea(storage->mBounds)) allocateRegistryStorage(sizeof(storage->mBounds));
        sead::CriticalSection::construct(&storage->mQueueLock);
        void* queues=storage->mQueues;
        if(!gabi::ea(queues)) queues=allocateRegistryStorage(sizeof(storage->mQueues));
        if(queues) initializeRenderQueueArray(queues,3,sizeof(sead::JobQueue),gabi::guest_fn(reinterpret_cast<const void*>(&sead::JobQueue::construct)));
        void* pending=storage->mPendingQueues;
        if(!gabi::ea(pending)) pending=allocateRegistryStorage(sizeof(storage->mPendingQueues));
        if(pending) initializeRenderQueueArray(pending,3,sizeof(sead::PointerBuffer),gabi::guest_fn(reinterpret_cast<const void*>(&RenderAddressRange::create)));
        storage->mDrawArgument=0;storage->mDrawCallback=0;storage->mMode=0;storage->mFrameState=0;storage->mExposure=1.0f;
        be<f32>* jitter=storage->mJitter;
        if(!gabi::ea(jitter)) jitter=static_cast<be<f32>*>(allocateRegistryStorage(8));
        if(jitter) { jitter[1]=0.0f;jitter[0]=0.0f; }
        storage->mFrameIndex=0;storage->mFrameInterval=1;storage->mPendingFrames=0;
        storage->mRetention=0.9f;storage->mFrameToken=0;
        storage->mInitialized=1;storage->mEnabled=1;storage->mSingleThread=0;
    }
    return storage;
}
sead::RegistryHeap* containingVertexInputHeap(sead::RegistryManagerState*,void*);
void destroyRenderDependencyGraph(RenderDependencyGraphStorage_l*,u32);
void destroyRenderQueueArray(void*,u32,u32,u32,u32,u32);
void destroyRenderView(RenderViewContext_l*,u32);
void destroyRenderRange(RenderAddressRange*,u32);
void collectRenderViewJobs(RenderViewContext_l*,sead::PointerBuffer*);
u32 renderJobCallback(void*);
void resetRenderView(RenderViewContext_l*);
void finishRenderView(RenderViewContext_l*);
void invalidateRenderView(RenderViewContext_l*);
void drawRenderView(RenderViewContext_l*,u32);
void activateTransferTarget(sead::DisplayFrame*);
/* 027C0B00 */
void agl::RenderQueueManager::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(gabi::ea(this)) {
        mVtable=&sRenderQueueManagerMethods;
        u32 cursor=gabi::ea(static_cast<gptr<RenderLayerPrefix>*>(mLayers));
        u32 end=cursor+static_cast<u32>(mLayerCount)*4;
        while(cursor!=end) {
            RenderLayerPrefix* layer=*gabi::at<gptr<RenderLayerPrefix>>(cursor);
            if(layer) {
                u32 target=layer->mVtable->mDestroy.mFunction;
                gabi::call_ptr<void>(target,layer,3);
                *gabi::at<gptr<RenderLayerPrefix>>(cursor)=nullptr;
            }
            cursor+=4;
        }
        for(u32 i=0;i<3;++i) { mQueues[i].clear();mPendingQueues[i].release(); }
        if(mLayers) {
            sead::RegistryHeap* heap=containingVertexInputHeap(sRegistryManager,mLayers);
            u32 target=heap->dispatch()->mRelease.mFunction;
            gabi::call_ptr<void>(target,heap,static_cast<void*>(mLayers));
            mLayerCount=0;mLayers=nullptr;
        }
        for(u32 i=0;i<2;++i) {
            void* output=mOutputs[i];
            if(output) {
                auto* frame=static_cast<RenderOutputPrefix_l*>(output);
                u32 target=frame->mVtable->mDestroy.mFunction;
                gabi::call_ptr<void>(target,output,3);
            }
        }
        destroyRenderQueueArray(mQueues,3,sizeof(sead::JobQueue),gabi::guest_fn(reinterpret_cast<const void*>(&destroyRenderRange)),0,0);
        mQueueLock.destroy(2);destroyRenderDependencyGraph(&mDependencyGraph,2);
        destroyRenderQueueArray(mViews,2,sizeof(RenderViewContext_l),gabi::guest_fn(reinterpret_cast<const void*>(&destroyRenderView)),0,0);
        if(flags&1) releaseRegistryStorage(this);
    }
}
/* 027C1848 */
void agl::RenderQueueManager::collectJobs(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 interval=mFrameInterval,frameIndex=mFrameIndex;
    u32 quotient=interval ? frameIndex/interval:0;
    if(frameIndex-quotient*interval || !isReady()) return;
    if(!(static_cast<u32>(mFlags)&4)) { collectRenderViewJobs(indexedView(this,index),nullptr);return; }
    struct JobCollectionStorage_l { sead::PointerBuffer mBuffer;be<u32> mEntries[256]; };
    gabi::FrameLocal<JobCollectionStorage_l> collection(8);
    sead::PointerBuffer* jobs=&collection->mBuffer;
    jobs->mCount=0;jobs->mCapacity=0;jobs->mStorage=nullptr;
    jobs->setStorage(0x100,collection->mEntries);jobs->mCount=0;
    collectRenderViewJobs(indexedView(this,index),jobs);
    bool split=(static_cast<u32>(mFlags)&(1u<<13))!=0;
    u32 cursor=gabi::ea(static_cast<void*>(jobs->mStorage));
    u32 end=cursor+static_cast<u32>(jobs->mCount)*4;
    while(cursor!=end) {
        u32 group=0;
        if(split) {
            void* job=*gabi::at<gptr<void>>(cursor);
            u32 callback=renderJobCallback(job);
            group=static_cast<s32>(static_cast<u32>(mDrawCallback))<static_cast<s32>(callback) ? 0:1;
        }
        sead::PointerBuffer* pending=&mPendingQueues[group];
        s32 count=pending->mCount;
        u32 job=*gabi::at<be<u32>>(cursor);  // loaded before the capacity check, as the original does (027C19A4)
        if(count<static_cast<s32>(pending->mCapacity)) {
            auto* slots=static_cast<be<u32>*>(static_cast<void*>(pending->mStorage));
            slots[count]=job;
            pending->mCount=static_cast<u32>(pending->mCount)+1;
        }
        cursor+=4;
    }
}
/* 027C1AD8 */
u32 agl::RenderQueueManager::drawView(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 interval=mFrameInterval,frameIndex=mFrameIndex;
    u32 quotient=interval ? frameIndex/interval:0;
    if(frameIndex-quotient*interval) return 0;
    u32 flags=mFlags;
    if(flags&0x100) {
        void* output=mOutputs[0];
        if(output) {
            auto* outputState=static_cast<RenderOutputPrefix_l*>(output);
            u32 clearFlags=outputState->mDepthState ? 3:1;
            if(static_cast<u32>(mViews[0].mRenderFlags)&0x80) {
                activateTransferTarget(static_cast<sead::DisplayFrame*>(output));
                gabi::FrameLocal<sead::Viewport> viewport(0x18);
                sead::Viewport* created=sead::Viewport::createSurface(viewport,static_cast<const sead::RenderVector2*>(output));
                created->apply(static_cast<const sead::RenderSurface*>(output));
                gabi::FrameLocal<sead::RenderQuaternion> color(8);
                color->x=0.0f;color->z=0.0f;color->y=0.0f;color->w=0.0f;
                u32 target=outputState->mVtable->mClear.mFunction;
                gabi::call_ptr<void>(target,output,clearFlags,static_cast<sead::RenderQuaternion*>(color),0,1.0f);
                flags=mFlags;
            }
        }
        flags&=~0x100u;mFlags=flags;
    }
    if(!isReady()) { resetRenderView(indexedView(this,index));finishRenderView(indexedView(this,index));return 1; }
    if(flags&0x80) { mFlags=flags&~0x80u;invalidateRenderView(indexedView(this,index));flags=mFlags; }
    drawRenderView(indexedView(this,index),(flags&0x200) ? 0:1);
    finishRenderView(indexedView(this,index));return 1;
}
sead::RegistryHeap* currentVertexInputHeap(sead::RegistryManagerState*);
void initializeRenderDependencyGraph(RenderDependencyGraphStorage_l*,u32,u32,u32,sead::RegistryHeap*);
void initializeRenderView(RenderViewContext_l*,u32,const RenderDimension*,const RenderDimension*,u32,sead::RegistryHeap*);
void* allocateRenderOutputStorage(u32,sead::RegistryHeap*,s32);
void* createRenderOutput(void*);
void notifyRenderManager(RenderQueueManager*,sead::SafeText*);
typedef be<u32> RenderCoreCount;
static RenderCoreCount sRenderCoreCount;
typedef be<u8> RenderManagerNameByte;
static const RenderManagerNameByte sRenderManagerNameFirst;
/* 027C0CEC */
void agl::RenderQueueManager::initialize(const RenderQueueConfiguration* configuration,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 capacity=configuration->mLayerCapacity;
    if(capacity>0) {
        sead::RegistryHeap* allocator=heap ? heap:currentVertexInputHeap(sRegistryManager);
        u32 target=allocator->dispatch()->mAllocate.mFunction;
        void* layers=gabi::call_ptr<void*>(target,allocator,static_cast<u32>(capacity)*4,4);
        if(layers) { mLayers=static_cast<gptr<RenderLayerPrefix>*>(layers);mLayerCount=capacity; }
    }
    u32 commandBytes=configuration->mCommandBytes;
    if(commandBytes) initializeRenderDependencyGraph(&mDependencyGraph,commandBytes,static_cast<u32>(mLayerCount)*2+8,configuration->mMode,heap);
    mContext=configuration->mCallback;
    for(u32 i=0;i<2;++i) {
        RenderViewContext_l* view=&mViews[i];
        initializeRenderView(view,i,&configuration->mOutputSizes[i],&configuration->mTargetSizes[i],mLayerCount,heap);
        if(!(static_cast<u32>(configuration->mFlags)&1)) {
            void* output=allocateRenderOutputStorage(0x40,heap,4);
            if(output) output=createRenderOutput(output);
            mOutputs[i]=output;view->mFrame=mOutputs[i];
        } else { mOutputs[i]=nullptr;mFlags=static_cast<u32>(mFlags)&~0x10u; }
        if(configuration->mCommandBytes) view->mGraph=&mDependencyGraph;
    }
    u32 cursor=gabi::ea(static_cast<gptr<RenderLayerPrefix>*>(mLayers));
    u32 end=cursor+static_cast<u32>(mLayerCount)*4;
    while(cursor!=end) { *gabi::at<be<u32>>(cursor)=0;cursor+=4; }
    for(u32 i=0;i<5;++i) reinterpret_cast<be<u32>*>(mBounds)[i]=0;
    for(u32 i=0;i<3;++i) {
        mPendingQueues[i].allocate(static_cast<u32>(configuration->mLayerCapacity)+8,heap,4);
        mQueues[i].initialize(static_cast<u32>(mLayerCount)+(i<2 ? 4:0),heap);
        u32 core=0;
        while(core<static_cast<u32>(sRenderCoreCount)) {
            // Both source branches use separate ABI stack words for the core record.
            if(i<2) {
                gabi::FrameLocal<sead::CoreIdentity> identity(8);identity->mIndex=core;mQueues[i].setBatchSize(identity,1);
            } else {
                gabi::FrameLocal<sead::CoreIdentity> identity(0xC);identity->mIndex=core;mQueues[i].setBatchSize(identity,1);
            }
            ++core;
        }
    }
    gabi::FrameLocal<sead::SafeText> name(0x10);
    name->mVtable=&sRenderQueueNameMethods;name->mText=const_cast<RenderManagerNameByte*>(&sRenderManagerNameFirst);
    notifyRenderManager(this,name);
}
void updateRenderViewState(RenderViewContext_l*);
void resetRenderViewStorage(RenderViewContext_l*);
void configureRenderViewOutput(RenderViewContext_l*,void*,RenderViewContext_l*,u32,u32,u32,s32,u32,u32);
void prepareRenderViewBounds(RenderViewContext_l*);
void appendRenderViewLayer(RenderViewContext_l*,RenderLayerPrefix*);
void updateRenderDependencyGraph(RenderDependencyGraphStorage_l*,u32);
/* 027C106C */
void agl::RenderQueueManager::update(u32 execute) {
    gabi::AutoFrame frame{gabi::current_function()};
    // Three empty original invalidation walks have no reads or calls on valid finite arrays.
    if(static_cast<u32>(mFlags)&1) mFlags=static_cast<u32>(mFlags)&~1u;
    for(u32 i=0;i<2;++i) {
        auto* view=&mViews[i];updateRenderViewState(view);
        u32 flags=view->mRenderFlags;
        if(flags&0x100) { view->mRenderFlags=flags&~0x100u;mFlags=static_cast<u32>(mFlags)|0x10; }
    }
    struct ViewOffsets_l { sead::RenderVector2 mEntries[2]; };
    struct ViewScales_l { be<f32> mEntries[2]; };
    gabi::FrameLocal<ViewOffsets_l> offsets(0x68);
    initializeRenderQueueArray(offsets,2,sizeof(RenderStorageHandle),gabi::guest_fn(reinterpret_cast<const void*>(&RenderStorageHandle::create)));
    offsets->mEntries[0].x=0.0f;offsets->mEntries[1].y=0.0f;offsets->mEntries[0].y=0.0f;offsets->mEntries[1].x=0.0f;
    gabi::FrameLocal<ViewScales_l> scales(0x50);
    scales->mEntries[1]=1.0f;scales->mEntries[0]=1.0f;
    if(static_cast<u32>(mFrameToken)==4) {
        f32 weight0=mViews[0].mWeight,weight1=mViews[1].mWeight;
        f32 denominator=gabi::fmuls_ppc(gabi::fadds_ppc(weight0,weight1),mRetention);
        f32 scale1=gabi::fdivs_ppc(weight1,denominator),scale0=gabi::fdivs_ppc(weight0,denominator);
        scales->mEntries[1]=scale1;scales->mEntries[0]=scale0;
        offsets->mEntries[0].x=0.0f;
        offsets->mEntries[0].y=gabi::fmuls_ppc(mViews[0].mAccessorOffset.y,gabi::fsubs_ppc(1.0f,scale0));
        mFlags=static_cast<u32>(mFlags)|0x100;
        offsets->mEntries[1].y=0.0f;
        offsets->mEntries[1].x=gabi::fmuls_ppc(mViews[0].mAccessorOffset.x,gabi::fsubs_ppc(1.0f,scale1));
    }
    u32 mode=mMode,argument=mode==1 ? static_cast<u32>(mFrameState):0;
    u32 cursor=gabi::ea(static_cast<gptr<RenderLayerPrefix>*>(mLayers));
    u32 end=cursor+static_cast<u32>(mLayerCount)*4;
    while(cursor!=end) {
        RenderLayerPrefix* layer=*gabi::at<gptr<RenderLayerPrefix>>(cursor);
        if(layer) {
            u32 flags=layer->mFlags;layer->mSelectedViews=0;
            if(flags&2) {
                u32 view=layer->mView;
                f32 scale=*gabi::at<be<f32>>(gabi::ea(static_cast<ViewScales_l*>(scales))+view*4);
                gabi::FrameLocal<sead::RenderVector2> pair(0x38);pair->y=scale;pair->x=scale;
                view=layer->mView;
                auto* offset=gabi::at<sead::RenderVector2>(gabi::ea(static_cast<ViewOffsets_l*>(offsets))+view*sizeof(sead::RenderVector2));
                layer->update(argument,pair,offset);
            }
        }
        cursor+=4;
    }
    if(!static_cast<u32>(mMode) && mPendingFrames) mPendingFrames=0;
    if(static_cast<u32>(mFlags)&0x10) for(u32 i=0;i<2;++i) resetRenderViewStorage(&mViews[i]);
    for(u32 index=0;index<2;++index) {
        auto* view=&mViews[index];view->mFrame=mOutputs[index];
        u32 flags=view->mRenderFlags;
        if(static_cast<u32>(mFrameToken)==4) flags|=0x20;else flags&=~0x20u;
        view->mRenderFlags=flags;
        u32 managerFlags=mFlags;
        if(managerFlags&0x800) flags|=0x80;else flags&=~0x80u;
        view->mRenderFlags=flags;managerFlags=mFlags;
        if(managerFlags&0x10) {
            RenderViewContext_l* parent=index && (managerFlags&8) ? &mViews[0]:nullptr;
            s32 initialize=(managerFlags&0x40) ? static_cast<s32>(static_cast<u32>(mInitialized)):-1;
            configureRenderViewOutput(view,mContext,parent,mColorFormat,mSingleThread,mDepthFormat,initialize,(managerFlags>>12)&1,(managerFlags>>14)&1);
        }
        prepareRenderViewBounds(view);
        view->mScaledBounds.mLeft.value=view->mBounds.mLeft.value;
        view->mScaledBounds.mTop.value=view->mBounds.mTop.value;
        view->mScaledBounds.mBottom.value=view->mBounds.mBottom.value;
        view->mScaledBounds.mRight.value=view->mBounds.mRight.value;
        view->mScaledBounds.mDeviceMode=view->mBounds.mDeviceMode;
        f32 denominator=gabi::fsubs_ppc(mViews[0].mBounds.mBottom.value,mViews[0].mBounds.mTop.value);
        f32 sy=gabi::fdivs_ppc(gabi::fsubs_ppc(view->mBounds.mBottom.value,view->mBounds.mTop.value),denominator);
        f32 sx=scales->mEntries[index],ox=offsets->mEntries[index].x;
        f32 left=gabi::fmuls_ppc(view->mScaledBounds.mLeft.value,sx);
        f32 top=gabi::fmuls_ppc(view->mScaledBounds.mTop.value,sy);
        f32 right=gabi::fmuls_ppc(view->mScaledBounds.mRight.value,sx);
        f32 bottom=gabi::fmuls_ppc(view->mScaledBounds.mBottom.value,sy);
        view->mScaledBounds.mLeft.value=gabi::fadds_ppc(left,ox);
        view->mScaledBounds.mTop.value=gabi::fadds_ppc(top,0.0f);
        view->mScaledBounds.mRight.value=gabi::fadds_ppc(right,ox);
        view->mScaledBounds.mBottom.value=gabi::fadds_ppc(bottom,0.0f);
        u32 token=mFrameToken,selection=view->mSelection;
        if(token==1) selection=!selection;else if(token==2) selection=0;else if(token==3) selection=1;
        cursor=gabi::ea(static_cast<gptr<RenderLayerPrefix>*>(mLayers));end=cursor+static_cast<u32>(mLayerCount)*4;
        u32 selectionBit=1u<<index;
        while(cursor!=end) {
            RenderLayerPrefix* layer=*gabi::at<gptr<RenderLayerPrefix>>(cursor);
            if(!(static_cast<u32>(view->mRenderFlags)&1) && layer) {
                u32 layerFlags=layer->mFlags;
                if((layerFlags&3)==3 && !(layerFlags&0x10)) {
                    u32 target=layer->methods()->mSelect.mFunction;
                    if(!gabi::call_ptr<u32>(target,layer)) {
                        s32 layerSelection=layer->mViewOverride;
                        if(layerSelection==-1) layerSelection=layer->mView;
                        if(static_cast<u32>(layerSelection)==selection) {
                            u32 only=mDrawArgument;
                            if(!only || only==gabi::ea(layer)) {
                                appendRenderViewLayer(view,layer);
                                layer->mSelectedViews=static_cast<u8>(layer->mSelectedViews)|selectionBit;
                            }
                        }
                    }
                }
            }
            cursor+=4;
        }
        flags=view->mRenderFlags;
        if(!(static_cast<u32>(mFlags)&2)) flags|=8;else flags&=~8u;
        view->mRenderFlags=flags;
    }
    if(static_cast<u32>(mFrameToken)==4) mViews[1].mFrame=mOutputs[0];
    u32 flags=mFlags;
    updateRenderDependencyGraph(&mDependencyGraph,execute&((flags&0x200) ? 0:1)&((flags&0x400) ? 0:1));
    mFlags=static_cast<u32>(mFlags)&0xFDEF;mFrameIndex=static_cast<u32>(mFrameIndex)+1;
    for(u32 index=0;index<3;++index) {
        mQueues[index].clear();
        for(u32 core=0;core<static_cast<u32>(sRenderCoreCount);++core) {
            gabi::FrameLocal<sead::CoreIdentity> identity(0xC);identity->mIndex=core;
            mQueues[index].setBatchSize(identity,mEnabled);
        }
        mPendingQueues[index].mCount=0;
    }
}
/* WWHD glue
ext 027D660C void updateRenderViewState(agl::RenderViewContext_l*)
ext 027D5C48 void resetRenderViewStorage(agl::RenderViewContext_l*)
ext 027D6658 void configureRenderViewOutput(agl::RenderViewContext_l*,void*,agl::RenderViewContext_l*,u32,u32,u32,s32,u32,u32)
ext 027D646C void prepareRenderViewBounds(agl::RenderViewContext_l*)
ext 027D64C8 void appendRenderViewLayer(agl::RenderViewContext_l*,agl::RenderLayerPrefix*)
ext 027D54A4 void updateRenderDependencyGraph(agl::RenderDependencyGraphStorage_l*,u32)
ext 027D527C void initializeRenderDependencyGraph(agl::RenderDependencyGraphStorage_l*,u32,u32,u32,sead::RegistryHeap*)
ext 027D60D4 void initializeRenderView(agl::RenderViewContext_l*,u32,const agl::RenderDimension*,const agl::RenderDimension*,u32,sead::RegistryHeap*)
ext 0273B050 void* allocateRenderOutputStorage(u32,sead::RegistryHeap*,s32)
ext 027B95B4 void* createRenderOutput(void*)
ext 02785CB4 void notifyRenderManager(agl::RenderQueueManager*,sead::SafeText*)
ext 0273B584 void sead::PointerBuffer::allocate(s32,void*,s32)
ext 027587B4 void sead::JobQueue::initialize(s32,sead::RegistryHeap*)
ext 027584E0 void sead::JobQueue::setBatchSize(const sead::CoreIdentity*,u32)
data sRenderCoreCount 101F8B5C
data sRenderManagerNameFirst 10169EA0
ext 02758B00 void sead::JobQueue::clear()
ext 0273B5DC void sead::PointerBuffer::release()
ext 02760168 void sead::CriticalSection::destroy(u32)
ext 027D514C void destroyRenderDependencyGraph(agl::RenderDependencyGraphStorage_l*,u32)
ext 028F0164 void destroyRenderQueueArray(void*,u32,u32,u32,u32,u32)
ext 027D5CF4 void destroyRenderView(agl::RenderViewContext_l*,u32) callback
ext 0273B560 void sead::PointerBuffer::setStorage(s32,void*)
ext 027C237C void destroyRenderRange(agl::RenderAddressRange*,u32) callback
ext 027D72B8 void collectRenderViewJobs(agl::RenderViewContext_l*,sead::PointerBuffer*)
ext 027D76B8 u32 renderJobCallback(void*)
ext 027D6C3C void resetRenderView(agl::RenderViewContext_l*)
ext 027D740C void finishRenderView(agl::RenderViewContext_l*)
ext 027D7050 void invalidateRenderView(agl::RenderViewContext_l*)
ext 027D6EEC void drawRenderView(agl::RenderViewContext_l*,u32)
data sQueueConfigurationMethods 10169E84
data sAglRenderQueueManager 101F95D0
data sAglRegisteredRenderManager 101F95D4
data sRenderOwnerDisposerMethods 10169E3C
data sRenderManagerDisposerMethods 10169EB4
data sRenderQueueNameMethods 10169E4C
data sRenderQueueNameFirst 101F95C8
data sRenderQueueManagerMethods 10169EC4
ext 028EFFD0 void initializeRenderQueueArray(void*,u32,u32,u32)
ext 027D5064 void* createRenderDependencyGraph(agl::RenderDependencyGraphStorage_l*)
ext 027D5948 agl::RenderViewContext_l* constructRenderView(agl::RenderViewContext_l*) callback
ext 02758724 sead::JobQueue* sead::JobQueue::construct(sead::JobQueue*) callback
ext 02760084 sead::CriticalSection* sead::CriticalSection::construct(sead::CriticalSection*)
ext 02752BEC void finishRenderDisposer(sead::Disposer*,u32)
ext 02752B0C void constructRenderDisposer(sead::Disposer*,sead::RegistryHeap*,u32)
ext 0273B0D4 void* allocateRenderManagerStorage(u32,sead::RegistryHeap*,s32)
ext 0273B7B4 void sead::PointerBuffer::heapSort(u32)
ext 02758AAC u32 sead::JobQueue::append(u32)
ext 027D76D4 s32 compareQueuedRenderEntries(void*,void*) callback
ext 027D6E80 void prepareRenderView(agl::RenderViewContext_l*)
ext 027D7384 void executeRenderView(agl::RenderViewContext_l*,sead::PointerBuffer*)
ext 027D650C void removeRenderViewLayer(agl::RenderViewContext_l*,agl::RenderLayerPrefix*)
ext 027D57B8 u32 registerRenderMethodResource(agl::RenderMethodResource_l*,void*)
ext 027D5818 u32 countRenderMethodResource(agl::RenderMethodResource_l*,void*)
*/
