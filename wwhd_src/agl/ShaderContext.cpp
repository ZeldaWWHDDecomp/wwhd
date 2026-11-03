// Geometry ring ownership, shader mode dispatch, and context replay selection.
#include "agl/ShaderContext.h"
using namespace agl;
static ShaderContextMethods sShaderContextMethods;
static sead::DeviceStateCache_l* sRenderContextState;
void* allocateShaderContext(u32);
void initializeShaderModeConfiguration(ShaderModeConfiguration*);
sead::CriticalSection* initializeShaderContextMutex(sead::CriticalSection*);
void releaseShaderRing(void*);
void destroyShaderContextMutex(sead::CriticalSection*,u32);
void destroyShaderModeConfiguration(ShaderModeConfiguration*,u32);
void releaseShaderContext(void*);
void setShaderModeParameters(u32,u32,u32,u32,u32,u32,u32);
void invalidateShaderData(u32,void*,u32);
void bindShaderInputRing(void*,u32);
void bindShaderOutputRing(void*,u32);
void callShaderContextDisplayList(void*,u32);
void setShaderContextState(void*);
void configureShaderContextMode(ShaderContext*,u32,const ShaderModeConfiguration*);

/* 0276ACA0 */
ShaderContext* ShaderContext::construct(ShaderContext* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<ShaderContext*>(allocateShaderContext(sizeof(ShaderContext)));
    if (object == NULL) return NULL;
    object->mModeConfiguration = &object->mOwnedModeConfiguration;
    object->mMethods = &sShaderContextMethods;
    initializeShaderModeConfiguration(&object->mOwnedModeConfiguration);
    object->mInputItemSize = 0;
    object->mInputRingBytes = 0;
    object->mInputRing = NULL;
    object->mOutputItemSize = 0;
    object->mOutputRing = NULL;
    object->mOutputRingBytes = 0;
    initializeShaderContextMutex(&object->mMutex);
    object->mUseDisplayList = 1;
    object->mUseContext = 1;
    DisplayListRecording::create(&object->mDisplayList);
    return object;
}
/* 0276ADD0 */
void ShaderContext::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    bool hasInputRing = mInputRing != NULL;
    mMethods = &sShaderContextMethods;
    if (hasInputRing) {
        releaseShaderRing(mInputRing);
        mInputRing = NULL;
    }
    if (mOutputRing != NULL) {
        releaseShaderRing(mOutputRing);
        mOutputRing = NULL;
    }
    mDisplayList.destroy(2);
    destroyShaderContextMutex(&mMutex,2);
    destroyShaderModeConfiguration(&mOwnedModeConfiguration,2);
    if (flags & 1) releaseShaderContext(this);
}
/* 0276AFF0 */
void ShaderContext::setMode(u32 mode,const ShaderModeConfiguration* configuration) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mode == 0 || mode == 1) {
        u32 vertexGprs = configuration->mNormalVertexGprs;
        u32 pixelGprs = configuration->mNormalPixelGprs;
        u32 pixelStack = configuration->mNormalPixelStack;
        u32 vertexStack = configuration->mNormalVertexStack;
        setShaderModeParameters(mode,vertexGprs,vertexStack,0,0,pixelGprs,pixelStack);
        if (mode == 1) invalidateShaderData(8,NULL,0xFFFFFFFF);
    } else if (mode == 2) {
        u32 vertexStack = configuration->mGeometryVertexStack;
        u32 geometryStack = configuration->mGeometryStack;
        u32 geometryGprs = configuration->mGeometryGprs;
        u32 pixelStack = configuration->mGeometryPixelStack;
        u32 pixelGprs = configuration->mGeometryPixelGprs;
        u32 vertexGprs = configuration->mGeometryVertexGprs;
        setShaderModeParameters(mode,vertexGprs,vertexStack,geometryGprs,geometryStack,pixelGprs,pixelStack);
        invalidateShaderData(8,NULL,0xFFFFFFFF);
    }
}
/* 0276B0CC */
void ShaderContext::setConfiguredMode(u32 mode) {
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)configureShaderContextMode),this,mode,static_cast<ShaderModeConfiguration*>(mModeConfiguration));
}
/* 0276B0D4 */
void ShaderContext::bindGeometryRings() {
    gabi::AutoFrame frame{gabi::current_function()};
    bindShaderInputRing(mInputRing,mInputRingBytes);
    bindShaderOutputRing(mOutputRing,mOutputRingBytes);
}
/* 0276B114 */
void ShaderContext::applyContext() {
    if (mUseContext == 0) {
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)setShaderContextState),static_cast<void*>(NULL));
        return;
    }
    bool useDisplayList = mUseDisplayList != 0;
    void* context = sRenderContextState->mContext;
    if (useDisplayList && mDisplayList.mUsed != 0) {
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)callShaderContextDisplayList),static_cast<void*>(mDisplayList.mData),u32(mDisplayList.mUsed));
        return;
    }
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)setShaderContextState),context);
}


u32 shaderInputRingSize(u32);
u32 shaderOutputRingSize(u32);
void* allocateShaderRing(u32,sead::RegistryHeap*,s32);
void flushShaderRing(void*,u32);
void shaderContextDisplayList(void*,gptr<void>*,be<u32>*);

/* 0276AE84 */
u32 ShaderContext::initializeRings(sead::RegistryHeap* heap,void* memory,u32 inputItemSize,u32 outputItemSize,u32 useDisplayList) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 start = gabi::ea(memory);
    u32 cursor = start;
    mInputItemSize = inputItemSize;
    mUseDisplayList = u8(useDisplayList);
    mOutputItemSize = outputItemSize;
    mInputRingBytes = shaderInputRingSize(inputItemSize);
    u32 outputBytes = shaderOutputRingSize(mOutputItemSize);
    u32 inputBytes = mInputRingBytes;
    mOutputRingBytes = outputBytes;
    if (start != 0) {
        if (inputBytes != 0) {
            outputBytes = mOutputRingBytes;
            cursor = (cursor + 255) & 0xFFFFFF00;
            inputBytes = mInputRingBytes;
            mInputRing.v = cursor;  // the ring starts at the aligned caller-provided address
            cursor += inputBytes;
        }
        if (outputBytes != 0) {
            outputBytes = mOutputRingBytes;
            cursor = (cursor + 255) & 0xFFFFFF00;
            mOutputRing.v = cursor;
            cursor += outputBytes;
        }
    } else {
        if (inputBytes != 0) {
            mInputRing = allocateShaderRing(inputBytes,heap,256);
            outputBytes = mOutputRingBytes;
        }
        if (outputBytes != 0) mOutputRing = allocateShaderRing(outputBytes,heap,256);
    }
    if (mInputRing != NULL) flushShaderRing(mInputRing,mInputRingBytes);
    if (mOutputRing != NULL) flushShaderRing(mOutputRing,mOutputRingBytes);
    void* context = sRenderContextState->mContext;
    if (context != NULL) {
        gabi::FrameLocal<gptr<void>> displayAddress(8);
        gabi::FrameLocal<be<u32>> displayBytes(12);
        shaderContextDisplayList(context,displayAddress.get(),displayBytes.get());
        u32 bytes = *displayBytes.get();
        void* address = *displayAddress.get();
        mDisplayList.setStorage(address,bytes);
        mDisplayList.mUsed = bytes;
    }
    return cursor - start;
}


static ShaderContext* sShaderContextSingleton;
static ShaderContextDisposer* sShaderContextInstanceDisposer;
static sead::DisposerVtable sShaderDisposerMethods;
void constructShaderDisposer(sead::Disposer*,sead::RegistryHeap*,u32);
void destroyShaderDisposerBase(sead::Disposer*,u32);
ShaderContext* constructShaderContextObject(ShaderContext*);

/* 0276AD30 */
void ShaderContext::createSingleton(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sShaderContextSingleton != NULL) return;
    ShaderContext* object = static_cast<ShaderContext*>(allocateShaderRing(sizeof(ShaderContext),heap,4));
    ShaderContextDisposer* disposer = &object->mDisposer;
    if (disposer != NULL) {
        constructShaderDisposer(disposer,heap,3);
        disposer->mVtable = &sShaderDisposerMethods;
    }
    sShaderContextInstanceDisposer = disposer;
    if (object != NULL) object = constructShaderContextObject(object);
    sShaderContextSingleton = object;
}
/* 0276B15C */
void ShaderContextDisposer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mVtable = &sShaderDisposerMethods;
    if (this == sShaderContextInstanceDisposer) {
        ShaderContext* context = sShaderContextSingleton;
        sShaderContextInstanceDisposer = NULL;
        gabi::call_ptr<void>(context->mMethods->mDestroy.mFunction,context,u32(2));
        sShaderContextSingleton = NULL;
    }
    destroyShaderDisposerBase(this,0);
    if (flags & 1) releaseShaderContext(this);
}

/* WWHD glue
 data sShaderContextSingleton 101F8BD8
 data sShaderContextInstanceDisposer 101F8BDC
 data sShaderDisposerMethods 1015FB1C
 ext 02752B0C void constructShaderDisposer(sead::Disposer*,sead::RegistryHeap*,u32)
 ext 02752BEC void destroyShaderDisposerBase(sead::Disposer*,u32)
 ext 0276ACA0 ShaderContext* constructShaderContextObject(ShaderContext*)
 ext C00060C0 u32 shaderInputRingSize(u32)
 ext C00060C8 u32 shaderOutputRingSize(u32)
 ext 0273B0D4 void* allocateShaderRing(u32,sead::RegistryHeap*,s32)
 ext C0008898 void flushShaderRing(void*,u32)
 ext C00061F0 void shaderContextDisplayList(void*,gptr<void>*,be<u32>*)
 ext 027C05A0 void agl::DisplayListRecording::setStorage(void* data,u32 capacity)
 data sShaderContextMethods 1015FB2C
 data sRenderContextState 101F8B18
 ext 0273AD10 void* allocateShaderContext(u32)
 ext 027B7044 void initializeShaderModeConfiguration(ShaderModeConfiguration*)
 ext 02760084 sead::CriticalSection* initializeShaderContextMutex(sead::CriticalSection*)
 ext 027C05F4 agl::DisplayListRecording* agl::DisplayListRecording::create(DisplayListRecording* storage)
 ext 0273AFC8 void releaseShaderRing(void*)
 ext 027C06C4 void agl::DisplayListRecording::destroy(u32 flags)
 ext 02760168 void destroyShaderContextMutex(sead::CriticalSection*,u32)
 ext 027B7088 void destroyShaderModeConfiguration(ShaderModeConfiguration*,u32)
 ext 0273AF40 void releaseShaderContext(void*)
 ext C0006988 void setShaderModeParameters(u32,u32,u32,u32,u32,u32,u32)
 ext C00064A8 void invalidateShaderData(u32,void*,u32)
 ext C0006890 void bindShaderInputRing(void*,u32)
 ext C0006898 void bindShaderOutputRing(void*,u32)
 ext C00060E0 void callShaderContextDisplayList(void*,u32) callback
 ext C00067E0 void setShaderContextState(void*) callback
 ext 0276AFF0 void configureShaderContextMode(ShaderContext*,u32,const ShaderModeConfiguration*) callback
*/
