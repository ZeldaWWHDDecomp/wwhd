// HD context creation and the three per-core mode slots.
#include "JSystem/J3DGraph/J3DRenderContext.h"
#include "sead/PoolHeap.h"
using namespace j3d;
void* allocateRenderContext(u32);
void constructRenderContextModes(void*,u32,u32,u32);
void initializeRenderContextProjection(void*);
void initializeRenderContextView(void*);
u32 renderContextCoreId();
void registerRenderContextExit(ExitRecord*);
static RenderContext sRenderContext;
static RuntimeRecord sRenderContextRuntime;
static ExitRecord sRenderContextRuntimeExit;
static ExitRecord sRenderContextExit;
/* 027F28CC */
RenderContext* RenderContext::create(RenderContext* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(context==NULL) context=static_cast<RenderContext*>(allocateRenderContext(sizeof(RenderContext)));
    if(context==NULL) return context;
    context->mState=0;
    context->mFlags=0;
    if(gabi::ea(context->mRenderState)==0) allocateRenderContext(0x30);
    context->mTarget=NULL;
    gptr<PacketDrawBuffer_l>* buffers=context->mDrawBuffers;
    context->mSource=NULL;
    if(gabi::ea(buffers)==0) allocateRenderContext(8);
    context->mEnabled=1;
    constructRenderContextModes(context->mCoreModes,3,sizeof(ContextMode),gabi::guest_fn((const void*)&ContextMode::create));
    context->mProjectionDirty=0;
    initializeRenderContextProjection(context->mProjection);
    context->mProjectionOwner=NULL;
    if(gabi::ea(context->mViewMatrix)==0) allocateRenderContext(0x40);
    initializeRenderContextView(context->mViewStatePrefix);
    buffers[0]=NULL;
    buffers[1]=NULL;
    return context;
}
/* 027F29B8 */
void RenderContext::enableGlobal() {
    sRenderContext.mRenderMode=0x17;
    sRenderContext.mViewEnabled=1;
}
/* 027F29D4 */
ContextMode* RenderContext::currentCoreMode() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 core=renderContextCoreId();
    u32 slot=sCoreReverse[core];
    if(slot==0) return &mCoreModes[0];
    if(slot==1) return &mCoreModes[1];
    if(slot==2) return &mCoreModes[2];
    return NULL;
}
/* 027F2A4C */
void j3d::initializeRenderContextRuntime() {
    gabi::AutoFrame frame{gabi::current_function()};
    sRenderContextRuntime.mWords[2]=0;
    sRenderContextRuntime.mWords[0]=0;
    sRenderContextRuntime.mWords[3]=0;
    sRenderContextRuntime.mWords[1]=0;
    registerRenderContextExit(&sRenderContextRuntimeExit);
    RenderContext::create(&sRenderContext);
    registerRenderContextExit(&sRenderContextExit);
}
/* WWHD glue
 ext 0273AD10 void* allocateRenderContext(u32)
 ext 028EFFD0 void constructRenderContextModes(void*,u32,u32,u32)
 ext 0274E1C8 void initializeRenderContextProjection(void*)
 ext 02750250 void initializeRenderContextView(void*)
 ext C0009BB0 u32 renderContextCoreId()
 ext 028F026C void registerRenderContextExit(j3d::ExitRecord*)
 data sRenderContext 104B45C0
 data sRenderContextRuntime 104B45B0
 data sRenderContextRuntimeExit 101F98E4
 data sRenderContextExit 101F98F0
*/
