// Render job execution (job table slot +0xC): records one command around the job's frame step. The core's command
// ring is looked up once; frame, command pool and owner are reloaded around each call.
#include "agl/DisplayFrameState.h"
#include "sead/PoolHeap.h"
using namespace agl::render;
u32 renderJobCoreId();
static sead::SafeTextVtable sExecutedJobTextMethods_l;
static void setCommandName(sead::SafeText* name,const char* text) { name->mText.v = gabi::guest_str(text); }
/* 027D74AC */
void agl::render::RenderJob::execute() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 core = renderJobCoreId();
    u32 kind = mKind;
    u32 ring = sCoreReverse[core];
    if (kind == kRenderJobDraw) {
        gabi::FrameLocal<sead::SafeText> name(8);
        name->mText = sFileDeviceEmptyText.mText;
        agl::RenderLayerPrefix* owner = mOwner;
        DisplayFrameState* state = mFrame;
        name->mVtable = &sExecutedJobTextMethods_l;
        u32 request = (u32)owner->mManagerIndex + 0x40u;
        RenderCommandPool* pool = state->mCommands;
        u32 index = pool->beginCommand(name.get(),request,ring);
        owner = mOwner;
        state = mFrame;
        state->prepareRenderPass(owner,1);
        state = mFrame;
        pool = state->mCommands;
        agl::gx2util::TimedCounter* command = pool->endCommand(index,ring);
        owner = mOwner;
        state = mFrame;
        state->appendDeferred(command,owner);
    } else if (kind == kRenderJobBegin) {
        gabi::FrameLocal<sead::SafeText> name(0x10);
        name->mVtable = &sExecutedJobTextMethods_l;
        DisplayFrameState* state = mFrame;
        setCommandName(name.get(),"begin");
        RenderCommandPool* pool = state->mCommands;
        u32 index = pool->beginCommand(name.get(),0xFFFFFFFEu,ring);
        state = mFrame;
        state->prepareAttachedSurface();
        state = mFrame;
        pool = state->mCommands;
        agl::gx2util::TimedCounter* command = pool->endCommand(index,ring);
        state = mFrame;
        state->appendDeferred(command,NULL);
    } else if (kind == kRenderJobEnd) {
        gabi::FrameLocal<sead::SafeText> name(0x18);
        name->mVtable = &sExecutedJobTextMethods_l;
        DisplayFrameState* state = mFrame;
        setCommandName(name.get(),"end");
        RenderCommandPool* pool = state->mCommands;
        u32 index = pool->beginCommand(name.get(),0x400u,ring);
        state = mFrame;
        state->invokePassHook();
        state = mFrame;
        pool = state->mCommands;
        agl::gx2util::TimedCounter* command = pool->endCommand(index,ring);
        state = mFrame;
        state->appendDeferred(command,NULL);
    } else if (kind == kRenderJobGpuCalc) {
        gabi::FrameLocal<sead::SafeText> name(0x20);
        name->mText = sFileDeviceEmptyText.mText;
        DisplayFrameState* state = mFrame;
        agl::RenderLayerPrefix* owner = mOwner;
        name->mVtable = &sExecutedJobTextMethods_l;
        RenderCommandPool* pool = state->mCommands;
        u32 request = owner->mManagerIndex;
        u32 index = pool->beginCommand(name.get(),request,ring);
        owner = mOwner;
        state = mFrame;
        state->finishRenderPass(owner);
        state = mFrame;
        pool = state->mCommands;
        agl::gx2util::TimedCounter* command = pool->endCommand(index,ring);
        state = mFrame;
        state->appendDeferred(command,NULL);
    }
}
/* WWHD glue
str 1016C340 "begin"
str 1016C33C "end"
data sExecutedJobTextMethods_l 1016C180
ext C0009BB0 u32 renderJobCoreId()
*/
