// J3D HD renderer: small helpers (text destructors, delegate dispatch, registry statics, completion).
// Names are our own; written from the WWHD binary.
#include "JSystem/J3DGraph/RenderValueStorage.h"

void releaseRenderTextStorage(void*);

static j3d::RenderStaticRegistry sRenderRegistry;
static u32 sRenderRegistration;
static j3d::RenderStaticRegistry sMaterialRegistry;
static u32 sMaterialRegistration;
static j3d::RenderStaticRegistry sResourceRegistry;
static u32 sResourceRegistration;
static j3d::RenderStaticRegistry sGeometryRegistry;
static u32 sGeometryRegistration;
static Mtx34 sTextureBiasMatrix;
static Mtx34 sTextureDepthBiasMatrix;

static inline void setMatrixRow(Mtx34& matrix, u32 row, f32 x, f32 y, f32 z, f32 w) {
    matrix.m[row][0] = x;
    matrix.m[row][1] = y;
    matrix.m[row][2] = z;
    matrix.m[row][3] = w;
}

// Fixed texture-coordinate transforms initialized after this unit's registry.
/* 027FB2D4 */
static void initializeMaterialRenderStatics() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    sMaterialRegistry.mWords[2] = 0;
    sMaterialRegistry.mWords[0] = 0;
    sMaterialRegistry.mWords[3] = 0;
    sMaterialRegistry.mWords[1] = 0;
    registerRenderStatics(&sMaterialRegistration);
    // Clip space [-1, 1] to texture space [0, 1] (y flipped); the second matrix applies the offset through z
    // (projective texture coordinates).
    setMatrixRow(sTextureBiasMatrix, 0, 0.5f, 0.0f, 0.0f, 0.5f);
    setMatrixRow(sTextureBiasMatrix, 1, 0.0f, -0.5f, 0.0f, 0.5f);
    setMatrixRow(sTextureBiasMatrix, 2, 0.0f, 0.0f, 1.0f, 0.0f);
    setMatrixRow(sTextureDepthBiasMatrix, 0, 0.5f, 0.0f, 0.5f, 0.0f);
    setMatrixRow(sTextureDepthBiasMatrix, 1, 0.0f, -0.5f, 0.5f, 0.0f);
    setMatrixRow(sTextureDepthBiasMatrix, 2, 0.0f, 0.0f, 1.0f, 0.0f);
}

/* 027FB3B4 */
void j3d::RenderTextEntries::assureText() {}

/* 027FB3A0 */
void j3d::RenderTextEntries::destroyBase(sead::SafeText* text, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (text != NULL && (flags & 1)) releaseRenderTextStorage(text);
}

/* 027FB3D0 */
void j3d::RenderTextEntries::destroyBufferedA(sead::BufferedText* text, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (text != NULL && (flags & 1)) releaseRenderTextStorage(text);
}

/* 027FB3E4 */
void j3d::RenderTextEntries::destroyBufferedB(sead::BufferedText* text, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (text != NULL && (flags & 1)) releaseRenderTextStorage(text);
}

/* 027FB3F8 */
void j3d::RenderTextEntries::destroyBufferedC(sead::BufferedText* text, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (text != NULL && (flags & 1)) releaseRenderTextStorage(text);
}

/* 027FF638 */
void j3d::RenderTextEntries::destroyCallback(sead::SafeText* text, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (text != NULL && (flags & 1)) releaseRenderTextStorage(text);
}

/* 0280037C */
void j3d::RenderCompletionState_l::complete(u32 result, RenderCompletionState_l* state) {
    if (result != 0) return;
    u32 epoch = state->mEpochFlags;
    u32 flags = state->mFlags;
    state->mChanged = 1;
    state->mEpochFlags = epoch & 0xFFFFFF00u;
    state->mReady = 1;
    state->mFlags = flags & 0xFFFFFFFCu;
    state->mComplete = 1;
    state->mFailed = 0;
    state->mEnabled = 1;
    state->mPending = 1;
}

/* 028003C0 */
static void initializeGeometryRegistry() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    sGeometryRegistry.mWords[2] = 0;
    sGeometryRegistry.mWords[0] = 0;
    sGeometryRegistry.mWords[3] = 0;
    sGeometryRegistry.mWords[1] = 0;
    registerRenderStatics(&sGeometryRegistration);
}

/* 027FB3B8 */
void j3d::RenderTextEntries::ensureTerminated(sead::BufferedText* text) {
    be<u8>* bytes = text->mText;
    u32 capacity = text->mCapacity;
    bytes[capacity - 1] = 0;
}

/* 027FE640 */
void* j3d::RenderPointerEntries::get(const gptr<void>* entries, u32 index) {
    return entries[index];
}



/* 027FF678 */
void j3d::RenderTextEntries::assureCallbackText() {}



// Compiler-generated initialization of this translation unit's registry.
/* 027FF610 */
static void initializeRenderRegistry() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    sRenderRegistry.mWords[2] = 0;
    sRenderRegistry.mWords[0] = 0;
    sRenderRegistry.mWords[3] = 0;
    sRenderRegistry.mWords[1] = 0;
    registerRenderStatics(&sRenderRegistration);
}

/* 027FFCBC */
j3d::RenderStaticRegistry* j3d::RenderStaticRegistry::get() {
    return &sRenderRegistry;
}



/* 02800938 */
static void initializeResourceRegistry() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    sResourceRegistry.mWords[2] = 0;
    sResourceRegistry.mWords[0] = 0;
    sResourceRegistry.mWords[3] = 0;
    sResourceRegistry.mWords[1] = 0;
    registerRenderStatics(&sResourceRegistration);
}




/* 027FF6F8 */
void j3d::RenderDelegateEntries::dispatch(RenderCallbackNode_l* node) {
    sead::MethodDelegate* delegate = &node->mDelegate;
    gabi::tail_ptr<void>(delegate->mVtable->mInvoke.mFunction, delegate);
}

/* 027FF708 */
void j3d::RenderDelegateEntries::invoke(sead::MethodDelegate* delegate) {
    u8* receiver;
    u32 target;
    if (delegate->resolve(receiver, target)) gabi::tail_ptr<void>(target, receiver);
}

/* WWHD glue
 data sGeometryRegistry 104B4E84
 data sGeometryRegistration 101F9A3C
 ext 0273AF40 void releaseRenderTextStorage(void*)
 data sResourceRegistry 104B4E94
 data sResourceRegistration 101F9A48
 data sMaterialRegistry 104B49F4
 data sMaterialRegistration 101F99F4
 data sTextureBiasMatrix 104B4A34
 data sTextureDepthBiasMatrix 104B4A04
 data sRenderRegistry 104B4A64
 data sRenderRegistration 101F9A00
 ext 028F026C void registerRenderStatics(void*)
*/
