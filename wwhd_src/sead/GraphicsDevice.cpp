// WWHD graphics-device locking and viewport dispatch, from local instructions.
#include "sead/GraphicsDevice.h"
#include "sead/Viewport.h"

static u32 sGraphicsTable;
struct GraphicsThreadRegistry_l { u8 mBase[0x24]; be<u32> mSlot; };
static GraphicsThreadRegistry_l* sGraphicsThreads;
void* allocateGraphicsObject(u32 size);
void initializeGraphicsBase(void* object);
void destroyGraphicsBase(void* object, u32 flags);
void initializeGraphicsMutex(void* mutex);
void destroyGraphicsMutex(void* mutex, u32 flags);
void acquireGraphicsMutex(void* mutex);
void releaseGraphicsMutex(void* mutex);
u32 getGraphicsThreadSlot(u32 slot);
void deleteGraphicsObject(void* object);

/* 0274FB24 */
sead::GraphicsDevice* sead::GraphicsDevice::create(GraphicsDevice* object) {
    if (object == NULL) object = (GraphicsDevice*)allocateGraphicsObject(sizeof(GraphicsDevice));
    if (object != NULL) {
        initializeGraphicsBase(object);
        object->mOwnerThread = 0;
        object->mLockCount = 0;
        object->mTable = (GraphicsDeviceTable*)&sGraphicsTable;
        initializeGraphicsMutex(&object->mMutex);
        object->mLockCallback = 0;
    }
    return object;
}

/* 0274FB98 */
void sead::GraphicsDevice::destroy(GraphicsDevice* object, u32 flags) {
    if (object == NULL) return;
    destroyGraphicsMutex(&object->mMutex, 2);
    destroyGraphicsBase(object, 0);
    if (flags & 1) deleteGraphicsObject(object);
}

/* 0274FBF8 */
void sead::GraphicsDevice::acquire() {
    u32 callback = mLockCallback;
    if (callback != 0) {
        gabi::call_ptr<void>(callback, 1);
        return;
    }
    u32 thread = getGraphicsThreadSlot(sGraphicsThreads->mSlot);
    if (thread == (u32)mOwnerThread) {
        s32 count = mLockCount;
        if (count > 0) {
            mLockCount = (u32)count + 1;
            return;
        }
    }
    acquireGraphicsMutex(&mMutex);
    gabi::call_ptr<void>(mTable->mAcquire, this);
    mOwnerThread = thread;
    mLockCount = 1;
}

/* 0274FCCC */
void sead::GraphicsDevice::release() {
    u32 callback = mLockCallback;
    if (callback != 0) {
        gabi::call_ptr<void>(callback, 0);
        return;
    }
    getGraphicsThreadSlot(sGraphicsThreads->mSlot);
    u32 count = (u32)mLockCount - 1;
    mLockCount = count;
    if (count == 0) {
        GraphicsDeviceTable* table = mTable;
        mOwnerThread = 0;
        gabi::call_ptr<void>(table->mRelease, this);
        releaseGraphicsMutex(&mMutex);
    }
}

/* 0274FD64 */
void sead::GraphicsDevice::reset() {
    gabi::AutoFrame frame{gabi::current_function()};
    acquire();
    gabi::call_ptr<void>(mTable->mReset, this);
    gabi::FrameLocal<CompactRenderState> state(8);
    CompactRenderState::create(state);
    state->apply();
    release();
}

/* 0274F964 */
void sead::Viewport::apply(const RenderSurface* surface) const {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<RenderVector2> position(8);
    gabi::FrameLocal<RenderVector2> extent(16);
    mapPosition(position, surface);
    mapExtent(extent, surface);
    f32 height = gabi::fsubs_ppc((f32)surface->mTop, (f32)surface->mBottom);
    f32 top = gabi::fsubs_ppc(height, (f32)extent->y);
    f32 y = gabi::fsubs_ppc(top, (f32)position->y);
    position->y = y;
    GraphicsDevice* device = sCurrentGraphicsDevice;
    gabi::call_ptr<void>(device->mTable->mSetViewport, device,
                         (f32)position->x, y, (f32)extent->x, (f32)extent->y);
    device = sCurrentGraphicsDevice;
    gabi::call_ptr<void>(device->mTable->mSetScissor, device,
                         (f32)position->x, (f32)position->y, (f32)extent->x, (f32)extent->y);
}

/* 0274FA00 */
void sead::Viewport::applyViewport(const RenderSurface* surface) const {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<RenderVector2> position(8);
    gabi::FrameLocal<RenderVector2> extent(16);
    mapPosition(position, surface);
    mapExtent(extent, surface);
    f32 height = gabi::fsubs_ppc((f32)surface->mTop, (f32)surface->mBottom);
    f32 top = gabi::fsubs_ppc(height, (f32)extent->y);
    f32 y = gabi::fsubs_ppc(top, (f32)position->y);
    position->y = y;
    GraphicsDevice* device = sCurrentGraphicsDevice;
    gabi::call_ptr<void>(device->mTable->mSetViewport, device,
                         (f32)position->x, y, (f32)extent->x, (f32)extent->y);
}

/* 0274FA70 */
void sead::Viewport::applyScissor(const RenderSurface* surface) const {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<RenderVector2> position(8);
    gabi::FrameLocal<RenderVector2> extent(16);
    mapPosition(position, surface);
    mapExtent(extent, surface);
    f32 height = gabi::fsubs_ppc((f32)surface->mTop, (f32)surface->mBottom);
    f32 top = gabi::fsubs_ppc(height, (f32)extent->y);
    f32 y = gabi::fsubs_ppc(top, (f32)position->y);
    position->y = y;
    GraphicsDevice* device = sCurrentGraphicsDevice;
    gabi::call_ptr<void>(device->mTable->mSetScissor, device,
                         (f32)position->x, y, (f32)extent->x, (f32)extent->y);
}

/* 027517C8 */
static void destroyCafeGraphicsDevice(sead::GraphicsDevice* object, u32 flags) {
    if (object == NULL) return;
    sead::GraphicsDevice::destroy(object, 0);
    if (flags & 1) deleteGraphicsObject(object);
}

/* WWHD glue
 data sGraphicsTable 10145380
 data sGraphicsThreads 101F8B94
 ext 0273AD10 void* allocateGraphicsObject(u32)
 ext 02752A84 void initializeGraphicsBase(void*)
 ext 02752BEC void destroyGraphicsBase(void*,u32)
 ext 02760084 void initializeGraphicsMutex(void*)
 ext 02760168 void destroyGraphicsMutex(void*,u32)
 ext 027601BC void acquireGraphicsMutex(void*)
 ext 027601F0 void releaseGraphicsMutex(void*)
 ext C0009CC8 u32 getGraphicsThreadSlot(u32)
 ext 0273AF40 void deleteGraphicsObject(void*)
*/
