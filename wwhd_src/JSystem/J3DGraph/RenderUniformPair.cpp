// J3D HD renderer: double-buffered uniform pairs (two agl uniform buffers, swapped every frame).
// Names are our own; written from the WWHD binary.
#include "JSystem/J3DGraph/RenderUniformPair.h"

static j3d::RenderCallbackQueue_l* sRenderCallbackQueue;
static j3d::RenderUniformPairVtable_l sRenderUniformPairTable;
static sead::SafeTextVtable sRenderCallbackTextTable;
static u8 sEmptyRenderCallbackName;
static u32 sRenderCallbackDispatchTable;
static sead::DelegateVtable sRenderUniformDelegateTable;

/* 027FB40C */
j3d::RenderUniformPair* j3d::RenderUniformPair::construct(RenderUniformPair* storage) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage == NULL)
        storage = static_cast<RenderUniformPair*>(allocateRenderUniformObject(sizeof(RenderUniformPair)));
    if (storage == NULL) return NULL;
    constructRenderDisposer(&storage->mDisposer);
    storage->mDisposer.mVtable = reinterpret_cast<sead::DisposerVtable*>(&sRenderUniformPairTable);
    constructRenderUniformArray(storage->mBuffers, 2, sizeof(agl::RenderUniformBuffer_l), initializeRenderUniformBuffer);
    storage->mActiveIndex = 0;
    RenderCallbackNode_l* callback = &storage->mCallback;
    storage->mReady = 0;
    storage->mPreviousIndex = 1;
    if (memberWrapsToNull(storage, offsetof(RenderUniformPair, mCallback))) {
        callback = static_cast<RenderCallbackNode_l*>(allocateRenderUniformObject(sizeof(RenderCallbackNode_l)));
        if (callback == NULL) return storage;
    }
    // The callback's name text sits at its start, so it is never NULL here (GHS keeps a dead test).
    sead::SafeText* text = &callback->mText;
    text->mVtable = &sRenderCallbackTextTable;
    text->mText = reinterpret_cast<be<u8>*>(&sEmptyRenderCallbackName);
    callback->mEmptyCharacter = 0;
    callback->mDispatchMethods = &sRenderCallbackDispatchTable;
    gabi::FrameLocal<sead::MemberSlot> method(8);
    method->mObjectAdjustment = 0;
    // This free binding supplies only the guest address of uploadPrevious.
    method->mTarget = gabi::guest_fn(reinterpret_cast<const void*>(uploadRenderUniformPrevious));
    sead::MethodDelegate* delegate = &callback->mDelegate;
    method->mVirtualIndex = -1;
    if (!memberWrapsToNull(callback, offsetof(RenderCallbackNode_l, mDelegate))) {
        delegate->mObject = reinterpret_cast<u8*>(storage);
        delegate->mMethod.mObjectAdjustment = method->mObjectAdjustment;
        delegate->mMethod.mVirtualIndex = method->mVirtualIndex;
        u32 target = method->mTarget;
        delegate->mVtable = &sRenderUniformDelegateTable;
        delegate->mMethod.mTarget = target;
    }
    return storage;
}

/* 027FB528 */
void j3d::RenderUniformPair::destroy(RenderUniformPair* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage == NULL) return;
    storage->mDisposer.mVtable = reinterpret_cast<sead::DisposerVtable*>(&sRenderUniformPairTable);
    for (u32 index = 0; index < 2; ++index) resetRenderUniform(&storage->mBuffers[index]);
    destroyRenderUniformArray(storage->mBuffers, 2, sizeof(agl::RenderUniformBuffer_l), destroyRenderUniformBuffer, 0, 0);
    destroyRenderDisposer(&storage->mDisposer, 0);
    if (flags & 1) releaseRenderUniformObject(storage);
}

/* 027FB5D4 */
void j3d::RenderUniformPair::prepare(void* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    for (u32 index = 0; index < 2; ++index) {
        agl::RenderUniformBuffer_l* buffer = &mBuffers[index];
        resetRenderUniform(buffer);
        auto* table = reinterpret_cast<RenderUniformPairVtable_l*>(mDisposer.mVtable.get());
        gabi::call_ptr<void>(table->mConfigure.mFunction, this, buffer, heap);
    }
    auto* table = reinterpret_cast<RenderUniformPairVtable_l*>(mDisposer.mVtable.get());
    gabi::call_ptr<void>(table->mAfterSetup.mFunction, this);
    mActiveIndex = 0;
    mPreviousIndex = 1;
    mReady = 1;
}

/* 027FB678 */
void j3d::RenderUniformPair::advance() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 active = mActiveIndex;
    mPreviousIndex = active;
    mActiveIndex = active == 0;
    queueRenderUniformCallback(sRenderCallbackQueue, &mCallback);
}

/* 027FB6A0 */
void j3d::RenderUniformPair::configureFrameFields(agl::RenderUniformBuffer_l* buffer, void* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    defineRenderUniformFields(buffer, 9, heap);
    appendRenderUniformField(buffer, 5, 3);
    appendRenderUniformField(buffer, 5, 4);
    appendRenderUniformField(buffer, 4, 8);
    appendRenderUniformField(buffer, 5, 8);
    appendRenderUniformField(buffer, 5, 2);
    appendRenderUniformField(buffer, 4, 1);
    appendRenderUniformField(buffer, 2, 1);
    appendRenderUniformField(buffer, 2, 1);
    appendRenderUniformField(buffer, 5, 4);
    allocateRenderUniform(buffer, heap, 1);
}

/* 027FBA24 */
void j3d::RenderUniformPair::configureMatrixFields(agl::RenderUniformBuffer_l* buffer, void* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    defineRenderUniformFields(buffer, 2, heap);
    appendRenderUniformField(buffer, 5, 3);
    appendRenderUniformField(buffer, 1, 1);
    allocateRenderUniform(buffer, heap, 1);
}

/* 027FF6D0 */
void j3d::RenderUniformPair::uploadPrevious() {
    u32 active = mActiveIndex;
    auto* table = reinterpret_cast<RenderUniformPairVtable_l*>(mDisposer.mVtable.get());
    agl::RenderUniformBuffer_l* buffer = &mBuffers[active == 0 ? 1 : 0];
    gabi::tail_ptr<void>(table->mUpload.mFunction, this, buffer);
}

/* WWHD glue
 data sRenderCallbackTextTable 1016EE18
 data sEmptyRenderCallbackName 1016EDFC
 data sRenderCallbackDispatchTable 1016EE50
 data sRenderUniformDelegateTable 1016EE60
 data sRenderUniformPairTable 1016EF24
 data sRenderCallbackQueue 101F997C
 ext 027FF6D0 void uploadRenderUniformPrevious(j3d::RenderUniformPair*) callback
 ext 0273AD10 void* allocateRenderUniformObject(u32)
 ext 02752A84 void constructRenderDisposer(sead::Disposer*)
 ext 028EFFD0 void constructRenderUniformArray(agl::RenderUniformBuffer_l*,u32,u32,RenderUniformConstruct)
 ext 027BEB5C void initializeRenderUniformBuffer(agl::RenderUniformBuffer_l*) callback
 ext 027BEC90 void destroyRenderUniformBuffer(agl::RenderUniformBuffer_l*,u32) callback
 ext 028F0164 void destroyRenderUniformArray(agl::RenderUniformBuffer_l*,u32,u32,RenderUniformDestroy,u32,u32)
 ext 02752BEC void destroyRenderDisposer(sead::Disposer*,u32)
 ext 0273AF40 void releaseRenderUniformObject(void*)
 ext 027BEBEC void resetRenderUniform(agl::RenderUniformBuffer_l*)
 ext 027BECEC void defineRenderUniformFields(agl::RenderUniformBuffer_l*,u32,void*)
 ext 027BED84 void appendRenderUniformField(agl::RenderUniformBuffer_l*,u32,u32)
 ext 027BEEE4 void allocateRenderUniform(agl::RenderUniformBuffer_l*,void*,u32)
 ext 027F9AE0 void queueRenderUniformCallback(j3d::RenderCallbackQueue_l*,j3d::RenderCallbackNode_l*)
*/
