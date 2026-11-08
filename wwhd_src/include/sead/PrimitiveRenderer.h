// WWHD primitive-renderer layout derived from its constructors and virtual calls.
#pragma once
#include "sead/RenderMath.h"
#include "sead/HeapRegistry.h"
namespace sead {
struct PrimitiveBackendTable {
    u8 mTypeAndDestroy[20];
    be<u32> mInitialize;
    be<u32> mInitializeAdjustment;
    be<u32> mBegin;
    be<u32> mBeginAdjustment;
    be<u32> mEnd;
    be<u32> mEndAdjustment;
    be<u32> mPrepare;
    be<u32> mPrepareAdjustment;
    be<u32> mFlush;
    u8 mShapeEntries[0x6C - 0x38];
    be<u32> mDrawSolid;
    u8 mOutlineEntries[0x9C - 0x70];
    be<u32> mDrawWire;
};
struct PrimitiveDrawControllerTable {
    u8 mTypeAndLinks[12];
    be<u32> mDestroy;
};
struct PrimitiveDrawController {
    u8 mObjectBase[12];
    gptr<PrimitiveDrawControllerTable> mTable;
    u8 mEmbeddedController[16];
    gptr<PrimitiveRenderer> mBackend;
    RenderMatrix34 mTransform;
    static PrimitiveDrawController* create(PrimitiveDrawController* object);
    static PrimitiveDrawController* instance(void* heap);
    static void destroy(PrimitiveDrawController* object, u32 flags);
    void createBackend(void* heap);
    void initializeBackend(void* heap, const void* settings);
    void begin(const void* model);
    void end(void* projection);
    void setTransform(const RenderMatrix34* transform);
    void prepare();
    void flush();
    void drawSolid(const void* vertices, const void* color);
    void drawWire(const void* vertices, const void* color);
};
}
WWHD_OFFSET(sead::PrimitiveRenderer,mTable,0);
WWHD_OFFSET(sead::PrimitiveBackendTable,mInitialize,0x14);
WWHD_OFFSET(sead::PrimitiveBackendTable,mBegin,0x1C);
WWHD_OFFSET(sead::PrimitiveBackendTable,mDrawSolid,0x6C);
WWHD_OFFSET(sead::PrimitiveBackendTable,mDrawWire,0x9C);
WWHD_OFFSET(sead::PrimitiveDrawController,mTable,12);
WWHD_OFFSET(sead::PrimitiveDrawController,mBackend,32);
WWHD_OFFSET(sead::PrimitiveDrawController,mTransform,36);
WWHD_SIZE(sead::PrimitiveDrawController,84);
