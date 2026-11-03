// Render view constructor: the embedded textures, surfaces and render buffer are built by their own agl constructors;
// every embedded member gets the GHS placement guard (allocate when its address is 0) the original keeps.
#include "agl/DisplayFrameState.h"
#include "agl/RootNode.h"
using namespace agl::render;
// agl::TextureData::create (027BE6B8); bound here under its own name, the light collections bind the method name.
agl::TextureData* constructViewTexture(agl::TextureData*);
static sead::SafeTextVtable sDisplayNameMethods;
static u8 sDisplayStateMethods;
static u8 sDisplayFrameBufferMethods;
static DisplayFloatConstant sDisplayZero;
static DisplayFloatConstant sDisplayOne;
static DisplayFloatConstant sDisplayGray;
template<class T> static T* placedMember(T* member) {
    if (!gabi::ea(member)) member = static_cast<T*>(allocateRegistryStorage(sizeof(T)));
    return member;
}
static void resetPointerArray(RenderPointerArray* array) {
    if (array) {
        be<s32>* count = placedMember(&array->mCount);
        if (count) *count = 0;
        array->mStorage = NULL;
        array->mCapacity = 0;
    }
}
/* 027D5948 */
agl::render::DisplayFrameState* agl::render::createDisplayFrameState(DisplayFrameState* state) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!state) state = static_cast<DisplayFrameState*>(allocateRegistryStorage(sizeof(DisplayFrameState)));
    if (state) {
        state->mMethods = gabi::ea(&sDisplayStateMethods);
        state->mCategory = 2;
        state->mRenderTarget = NULL;
        state->mAttachment = NULL;
        state->mSecondaryAttachment = NULL;
        constructViewTexture(&state->mTextureA);
        constructViewTexture(&state->mTextureB);
        state->mSurfaceA.construct();
        state->mSurfaceAuxiliary.construct();
        state->mSurfaceB.construct();
        state->mEmbeddedBuffer.construct();
        state->mExtraAttachment = NULL;
        constructViewTexture(&state->mTextureC);
        state->mSurfaceC.construct();
        sead::RenderVector2* size = placedMember(&state->mSize);
        f32 zero = sDisplayZero;
        if (size) { size->y = zero; size->x = zero; }
        state->mCommands = NULL;
        f32 one = sDisplayOne;
        state->mFrameBuffer.mVtable = &sDisplayFrameBufferMethods;
        sead::FrameBufferSize* extent = placedMember(&state->mFrameBuffer.mSize);
        if (extent) { extent->mHeight = one; extent->mWidth = one; }
        sead::FrameBufferArea* area = placedMember(&state->mFrameBuffer.mArea);
        if (area) { area->mMaxX = one; area->mMinY = zero; area->mMinX = zero; area->mMaxY = one; }
        sead::Viewport::createEmpty(&state->mViewport);
        sead::Viewport::createEmpty(&state->mScaledViewport);
        be<u32>* flags = placedMember(&state->mFlags);
        if (flags) *flags = 0x88;
        RenderPointerArray* pending = placedMember(&state->mPending);
        if (pending) { pending->mCapacity = 0; pending->mCount = 0; pending->mStorage = NULL; }
        RenderJobTable* jobs = placedMember(&state->mPrimaryJobs);
        if (jobs) { jobs->mJobs = NULL; jobs->mCount = 0; }
        jobs = placedMember(&state->mSecondaryJobs);
        if (jobs) { jobs->mJobs = NULL; jobs->mCount = 0; }
        state->mClearStencil = 0;
        state->mClearColor[2] = one;
        f32 gray = sDisplayGray;
        state->mCurrentJob = -1;
        state->mClearColor[0] = gray;
        state->mClearDepth = one;
        state->mClearMask = 3;
        state->mClearColor[3] = zero;
        state->mSurfaceChanged = 0;
        state->mClearColor[1] = gray;
        resetPointerArray(placedMember(&state->mDeferred));
        RenderCommandTable* commands = placedMember(&state->mSubmittedCommands);
        if (commands) { commands->mCommands = NULL; commands->mCount = 0; }
        resetPointerArray(placedMember(&state->mSubmitted));
        state->mCommandHook = NULL;
        state->mBeginHook = NULL;
        state->mPassHook = NULL;
        gabi::FrameLocal<sead::SafeText> name(8);
        name->mVtable = &sDisplayNameMethods;
        name->mText.v = gabi::guest_str("Icon = SCREEN");
        agl::detail::RootNode::setNodeMeta(state,name.get());
    }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
    return state;
}
/* WWHD glue
data sDisplayStateMethods 1016C348
data sDisplayFrameBufferMethods 1016C210
data sDisplayNameMethods 1016C180
str 1016C29C "Icon = SCREEN"
data sDisplayZero 1016C290
data sDisplayOne 1016C294
data sDisplayGray 1016C298
ext 027B6CB8 agl::ColorSurface* agl::ColorSurface::construct()
ext 027B6E64 agl::DepthSurface* agl::DepthSurface::construct()
ext 027B95B4 agl::RenderBuffer* agl::RenderBuffer::construct()
ext 0274F358 sead::Viewport* sead::Viewport::createEmpty(sead::Viewport*)
ext 027BE6B8 agl::TextureData* constructViewTexture(agl::TextureData*)
*/
