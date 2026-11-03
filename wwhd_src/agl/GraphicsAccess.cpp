/* Per-core and platform graphics lock selection in the WWHD library. */
// References: WWHD's own instructions and strings; GX2 SDK record layouts and import arguments are public wut/Cemu facts. No other reference.
#include "agl/GraphicsAccess.h"
#include "agl/BitmapTransfer.h"
#include "sead/PoolHeap.h"
static void setGraphicsResourceHeap(void* root, sead::RegistryHeap* heap);
static void initializeShaderResources(void* resources, sead::RegistryHeap* heap, sead::RegistryHeap* previous);
static void registerGraphicsResource(void* debug, const sead::SafeText* name, void* resource);
static void* sGraphicsResourceDebug_l;
static void* sShaderResources_l;
static void* sDynamicTextureResources_l;
static void* sDebugTextureResources_l;
static void* sPrimitiveTextureResources_l;
static void* sPrimitiveShapeResources_l;
static void* sNativeResources_l;
static sead::SafeTextVtable sResourceTextMethods;
static u32 currentCoreId();
static void acquireGraphicsDevice(sead::GraphicsDevice* device);
static void releaseGraphicsDevice(sead::GraphicsDevice* device);
static void lockGraphicsCore(agl::GraphicsAccessManager* manager);
static void unlockGraphicsCore(agl::GraphicsAccessManager* manager);
static void* allocateObject(u32 bytes);
static void* allocateInHeap(u32 bytes, sead::RegistryHeap* heap, s32 alignment);
static void releaseObject(void* object);
static void registerInHeap(sead::RegisteredObject* object, sead::RegistryHeap* heap, u32 selection);
static void constructMutexArray(void* storage, u32 count, u32 stride, agl::AccessMutexConstructor constructor);
static void destroyMutexArray(void* storage, u32 count, u32 stride, agl::AccessMutexDestructor destructor,
                              u32 flags, u32 reserved);
static void destroyCriticalSection(sead::CriticalSection* mutex, u32 flags);
static u8 sResourceFlag;
static u32 sResourceState;
static agl::GraphicsAccessMethods sGraphicsAccessMethods;
static sead::DisposerVtable sGraphicsAccessDisposerMethods;
static agl::GraphicsAccessManager* sGraphicsAccessManager;
static sead::RegisteredObject* sGraphicsAccessDisposer;
static sead::DisposerVtable sGraphicsByteLayoutMethods;

/* 027B6FF8 */
void agl::GraphicsByteLayout::initialize() {
    mValues[2] = 0x2C;
    mValues[9] = 0x30;
    mValues[4] = 0xC8;
    mValues[8] = 0x40;
    mValues[7] = 0xB0;
    mValues[6] = 0x4C;
    mValues[1] = 0x40;
    mValues[5] = 0xC0;
    mValues[3] = 0x20;
    mValues[0] = 0x30;
}
/* 027B7044 */
agl::GraphicsByteLayout* agl::GraphicsByteLayout::construct() {
    gabi::AutoFrame frame{gabi::current_function()};
    GraphicsByteLayout* result = this;
    if (gabi::ea(result) == 0) {
        result = (GraphicsByteLayout*)allocateObject(sizeof(GraphicsByteLayout));
        if (!result)
            return result;
    }
    result->mVtable = &sGraphicsByteLayoutMethods;
    result->initialize();
    return result;
}
/* 027B7088 */
void agl::GraphicsByteLayout::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}
/* 027B709C */
void agl::graphics_access_text::TextA::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}
/* 027B70B0 */
void agl::graphics_access_text::TextA::assureTerminationImpl() {}
/* 027B7434 */
void agl::graphics_access_text::TextB::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}
/* 027B7448 */
void agl::graphics_access_text::TextB::assureTerminationImpl() {}

/* 027B70B4 */
agl::GraphicsAccessManager* agl::GraphicsAccessManager::construct() {
    gabi::AutoFrame frame{gabi::current_function()};
    auto result = this;
    if (gabi::ea(result) == 0) {
        result = (GraphicsAccessManager*)allocateObject(sizeof(GraphicsAccessManager));
        if (!result)
            return result;
    }
    result->mMethods = &sGraphicsAccessMethods;
    auto references = &result->mReferenceCount;
    if (gabi::ea(references) == 0)
        references = (be<u32>*)allocateObject(4);
    if (references)
        *references = 0;
    auto locks = result->mCoreLocks;
    result->mMode = 0;
    if (gabi::ea(locks) == 0)
        locks = (sead::CriticalSection*)allocateObject(0xB4);
    if (locks)
        constructMutexArray(locks, 3, 0x3C, sead::CriticalSection::construct);
    sead::CriticalSection::construct(&result->mSharedLock);
    result->mState = 0;
    result->mReserved = 0;
    return result;
}
/* 027B7168 */
agl::GraphicsAccessManager* agl::GraphicsAccessManager::create(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    GraphicsAccessManager* result = sGraphicsAccessManager;
    if (!result) {
        result = (GraphicsAccessManager*)allocateInHeap(sizeof(GraphicsAccessManager), heap, 4);
        auto disposer = &result->mDisposer;
        if (gabi::ea(disposer) != 0) {
            registerInHeap(disposer, heap, 3);
            disposer->mVtable = &sGraphicsAccessDisposerMethods;
        }
        sGraphicsAccessDisposer = disposer;
        if (result)
            result = result->construct();
        sGraphicsAccessManager = result;
    }
    return result;
}
/* 027B7208 */
void agl::GraphicsAccessManager::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0)
        return;
    mSharedLock.destroy(2);
    destroyMutexArray(mCoreLocks, 3, 0x3C, destroyCriticalSection, 0, 0);
    if (flags & 1)
        releaseObject(this);
}

/* 027B727C */
u32 agl::currentGraphicsSlot() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 core = currentCoreId();
    return sCoreReverse[core];
}
/* 027B72AC */
void agl::GraphicsAccessManager::lockCore() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 slot = currentGraphicsSlot();
    mCoreLocks[slot].lock();
}
/* 027B730C */
void agl::GraphicsAccessManager::unlockCore() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 slot = currentGraphicsSlot();
    mCoreLocks[slot].unlock();
}
/* 027B72E8 */
void agl::GraphicsAccessManager::acquire() {
    u32 mode = mMode;
    if (mode == 0) {
        auto device = sCurrentGraphicsDevice.get();
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)acquireGraphicsDevice), device);
    } else if (mode == 1) {
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)lockGraphicsCore), this);
    }
}
/* 027B7348 */
void agl::GraphicsAccessManager::release() {
    u32 mode = mMode;
    if (mode == 0) {
        auto device = sCurrentGraphicsDevice.get();
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseGraphicsDevice), device);
    } else if (mode == 1) {
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)unlockGraphicsCore), this);
    }
}
/* 027B736C */
void agl::GraphicsAccessManager::acquireLocal() {
    if (mMode == 1)
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)lockGraphicsCore), this);
}
/* 027B737C */
void agl::GraphicsAccessManager::releaseLocal() {
    if (mMode == 1)
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)unlockGraphicsCore), this);
}
/* 027B738C */
void agl::GraphicsAccessDisposer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0)
        return;
    mVtable = &sGraphicsAccessDisposerMethods;
    if (sGraphicsAccessDisposer == this) {
        GraphicsAccessManager* manager = sGraphicsAccessManager;
        sGraphicsAccessDisposer = nullptr;
        gabi::call_ptr<void>(manager->mMethods->mDestroy.mFunction, manager, 2);
        sGraphicsAccessManager = nullptr;
    }
    sead::RegisteredObject::destroy(0);
    if (flags & 1)
        releaseObject(this);
}
/* 027B59E4 */
void agl::setResourceFlag() { sResourceFlag = 1; }
/* 027B5B5C */
void agl::clearResourceState() { sResourceState = 0; }
/* 027B59F4 */
void agl::registerGraphicsResources(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto previousHeap = sBitmapHeapSource_l->mHeap.get();
    setGraphicsResourceHeap(sBitmapHeapSource_l, heap);
    initializeShaderResources(sShaderResources_l, heap, previousHeap);
    void* debug = sGraphicsResourceDebug_l;
    gabi::FrameLocal<sead::SafeText> text(8);
    text->mVtable = &sResourceTextMethods;
    text->mText.v = gabi::guest_str("detail::ShaderHolder");
    registerGraphicsResource(debug, text.get(), sShaderResources_l);
    text->mVtable = &sResourceTextMethods;
    text->mText.v = gabi::guest_str("utl::DynamicTextureAllocator");
    registerGraphicsResource(debug, text.get(), sDynamicTextureResources_l);
    text->mVtable = &sResourceTextMethods;
    text->mText.v = gabi::guest_str("utl::DebugTextureDrawer");
    registerGraphicsResource(debug, text.get(), sDebugTextureResources_l);
    text->mVtable = &sResourceTextMethods;
    text->mText.v = gabi::guest_str("utl::PrimitiveTexture");
    registerGraphicsResource(debug, text.get(), sPrimitiveTextureResources_l);
    text->mVtable = &sResourceTextMethods;
    text->mText.v = gabi::guest_str("utl::PrimitiveShape");
    registerGraphicsResource(debug, text.get(), sPrimitiveShapeResources_l);
    text->mVtable = &sResourceTextMethods;
    text->mText.v = gabi::guest_str("utl::DrawContextMgr");
    registerGraphicsResource(debug, text.get(), sGraphicsAccessManager);
    text->mVtable = &sResourceTextMethods;
    text->mText.v = gabi::guest_str("driver::GX2Resource");
    registerGraphicsResource(debug, text.get(), sNativeResources_l);
}
/* WWHD glue
str 101698B8
str 101698D0
str 1016984C
str 101698A0
str 10169864
str 10169878
str 1016988C
ext 02785714 void setGraphicsResourceHeap(void* root, sead::RegistryHeap* heap)
ext 02786520 void initializeShaderResources(void* resources, sead::RegistryHeap* heap, sead::RegistryHeap* previous)
ext 02786064 void registerGraphicsResource(void* debug, const sead::SafeText* name, void* resource)
data sGraphicsResourceDebug_l 101F8C1C
data sShaderResources_l 101F8D6C
data sDynamicTextureResources_l 101F8DD4
data sDebugTextureResources_l 101F8DC0
data sPrimitiveTextureResources_l 101F8DEC
data sPrimitiveShapeResources_l 101F8DE4
data sNativeResources_l 101F8BD8
data sResourceTextMethods 10169768
data sResourceFlag 101F8E01
data sResourceState 104A208C
ext C0009BB0 u32 currentCoreId()
ext 027601BC void sead::CriticalSection::lock()
ext 027601F0 void sead::CriticalSection::unlock()
ext 02760084 sead::CriticalSection* sead::CriticalSection::construct(sead::CriticalSection* object) callback
ext 02760168 void sead::CriticalSection::destroy(u32 flags)
ext 02760168 void destroyCriticalSection(sead::CriticalSection* mutex, u32 flags) callback
ext 02752BEC void sead::RegisteredObject::destroy(u32 flags)
ext 0273AD10 void* allocateObject(u32 bytes)
ext 0273B0D4 void* allocateInHeap(u32 bytes, sead::RegistryHeap* heap, s32 alignment)
ext 0273AF40 void releaseObject(void* object) callback
ext 02752B0C void registerInHeap(sead::RegisteredObject* object, sead::RegistryHeap* heap, u32 selection)
ext 028EFFD0 void constructMutexArray(void* storage, u32 count, u32 stride, agl::AccessMutexConstructor constructor)
ext 028F0164 void destroyMutexArray(void* storage, u32 count, u32 stride, agl::AccessMutexDestructor destructor, u32 flags, u32 reserved)
ext 0274FBF8 void acquireGraphicsDevice(sead::GraphicsDevice* device) callback
ext 0274FCCC void releaseGraphicsDevice(sead::GraphicsDevice* device) callback
ext 027B72AC void lockGraphicsCore(agl::GraphicsAccessManager* manager) callback
ext 027B730C void unlockGraphicsCore(agl::GraphicsAccessManager* manager) callback
data sGraphicsAccessMethods 101699C8
data sGraphicsAccessDisposerMethods 101699B8
data sGraphicsAccessManager 101F8E04
data sGraphicsAccessDisposer 101F8E08
data sGraphicsByteLayoutMethods 10169970
*/
