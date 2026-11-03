// WWHD primitive drawing dispatch, reconstructed from the local game instructions.
#include "sead/PrimitiveRenderer.h"

static sead::PrimitiveDrawController* sPrimitiveRenderer;
static void* sPrimitiveController;
static u32 sPrimitiveTable;
static u32 sControllerTable;
static sead::RenderMatrix34 sPrimitiveTransform;
void* allocatePrimitiveObject(u32 size);
void* allocatePrimitiveArray(u32 size, void* heap, s32 alignment);
void* allocatePrimitiveFromHeap(u32 size, void* heap, s32 alignment);
void initializePrimitiveBase(void* object);
void initializePrimitiveController(void* object, void* heap, u32 count);
void destroyPrimitiveBase(void* object, u32 flags);
void deletePrimitiveObject(void* object);
void* initializePrimitiveBackend(void* object, void* heap);
void copyPrimitiveTransform(const sead::RenderMatrix34* source, sead::RenderMatrix34* destination);

static sead::PrimitiveBackendTable* primitiveDispatchTable(sead::PrimitiveRenderer* backend) {
    return gabi::at<sead::PrimitiveBackendTable>((u32)backend->mTable);
}

/* 02750754 */
sead::PrimitiveDrawController* sead::PrimitiveDrawController::create(PrimitiveDrawController* object) {
    if (object == NULL) object = (PrimitiveDrawController*)allocatePrimitiveObject(sizeof(PrimitiveDrawController));
    if (object != NULL) {
        initializePrimitiveBase(object);
        object->mTable = (PrimitiveDrawControllerTable*)&sPrimitiveTable;
        object->mBackend = NULL;
        for (u32 row = 0; row < 3; ++row) {
            for (u32 column = 0; column < 4; ++column) {
                reinterpret_cast<be<u32>&>(object->mTransform.mRows[row][column]) =
                    (u32)reinterpret_cast<const be<u32>&>(sPrimitiveTransform.mRows[row][column]);
            }
        }
    }
    return object;
}

/* 027507E8 */
sead::PrimitiveDrawController* sead::PrimitiveDrawController::instance(void* heap) {
    PrimitiveDrawController* object = sPrimitiveRenderer;
    if (object == NULL) {
        object = (PrimitiveDrawController*)allocatePrimitiveArray(sizeof(PrimitiveDrawController), heap, 4);
        void* controller = &object->mEmbeddedController;
        if (gabi::ea(controller) != 0) {
            initializePrimitiveController(controller, heap, 3);
            reinterpret_cast<PrimitiveDrawController*>(controller)->mTable = (PrimitiveDrawControllerTable*)&sControllerTable;
        }
        sPrimitiveController = controller;
        if (object != NULL) object = create(object);
        sPrimitiveRenderer = object;
    }
    return object;
}

/* 02750884 */
void sead::PrimitiveDrawController::destroy(PrimitiveDrawController* object, u32 flags) {
    if (object == NULL) return;
    object->mTable = (PrimitiveDrawControllerTable*)&sPrimitiveTable;
    deletePrimitiveObject(object->mBackend);
    destroyPrimitiveBase(object, 0);
    if (flags & 1) deletePrimitiveObject(object);
}

/* 027508EC */
void sead::PrimitiveDrawController::createBackend(void* heap) {
    void* backend = allocatePrimitiveFromHeap(0x18C, heap, 4);
    if (backend != NULL) backend = initializePrimitiveBackend(backend, heap);
    mBackend = (PrimitiveRenderer*)backend;
}

/* 02750940 */
void sead::PrimitiveDrawController::initializeBackend(void* heap, const void* settings) {
    createBackend(heap);
    PrimitiveRenderer* backend = mBackend;
    gabi::call_ptr<void>(primitiveDispatchTable(backend)->mInitialize, backend, heap, settings);
}

/* 027509A0 */
void sead::PrimitiveDrawController::begin() {
    PrimitiveRenderer* backend = mBackend;
    gabi::tail_ptr<void>(primitiveDispatchTable(backend)->mBegin, backend);
}

/* 027509B4 */
void sead::PrimitiveDrawController::end() {
    PrimitiveRenderer* backend = mBackend;
    gabi::tail_ptr<void>(primitiveDispatchTable(backend)->mEnd, backend);
}

/* 027509C8 */
void sead::PrimitiveDrawController::setTransform(const RenderMatrix34* transform) {
    copyPrimitiveTransform(transform, &mTransform);
}

/* 027509D8 */
void sead::PrimitiveDrawController::prepare() {
    PrimitiveRenderer* backend = mBackend;
    gabi::tail_ptr<void>(primitiveDispatchTable(backend)->mPrepare, backend);
}

/* 027509EC */
void sead::PrimitiveDrawController::flush() {
    PrimitiveRenderer* backend = mBackend;
    gabi::tail_ptr<void>(primitiveDispatchTable(backend)->mFlush, backend);
}

/* 02750A00 */
void sead::PrimitiveDrawController::drawSolid(const void* vertices, const void* color) {
    PrimitiveRenderer* backend = mBackend;
    gabi::tail_ptr<void>(primitiveDispatchTable(backend)->mDrawSolid, backend, &mTransform, vertices, color);
}

/* 02750A24 */
void sead::PrimitiveDrawController::drawWire(const void* vertices, const void* color) {
    PrimitiveRenderer* backend = mBackend;
    gabi::tail_ptr<void>(primitiveDispatchTable(backend)->mDrawWire, backend, &mTransform, vertices, color);
}

/* 02750A48 */
static void destroyPrimitiveController(void* controller, u32 flags) {
    if (controller == NULL) return;
    reinterpret_cast<sead::PrimitiveDrawController*>(controller)->mTable = (sead::PrimitiveDrawControllerTable*)&sControllerTable;
    if (controller == sPrimitiveController) {
        sead::PrimitiveDrawController* object = sPrimitiveRenderer;
        sPrimitiveController = NULL;
        gabi::call_ptr<void>(object->mTable->mDestroy, object, 2);
        sPrimitiveRenderer = NULL;
    }
    destroyPrimitiveBase(controller, 0);
    if (flags & 1) deletePrimitiveObject(controller);
}

/* WWHD glue
 data sPrimitiveRenderer 101F8B28
 data sPrimitiveController 101F8B2C
 data sPrimitiveTable 1014553C
 data sControllerTable 1014552C
 data sPrimitiveTransform 104A041C
 ext 0273AD10 void* allocatePrimitiveObject(u32)
 ext 0273B0D4 void* allocatePrimitiveArray(u32,void*,s32)
 ext 0273B050 void* allocatePrimitiveFromHeap(u32,void*,s32)
 ext 02752A84 void initializePrimitiveBase(void*)
 ext 02752B0C void initializePrimitiveController(void*,void*,u32)
 ext 02752BEC void destroyPrimitiveBase(void*,u32)
 ext 0273AF40 void deletePrimitiveObject(void*)
 ext 0275181C void* initializePrimitiveBackend(void*,void*)
 ext 028E90D4 void copyPrimitiveTransform(const sead::RenderMatrix34*,sead::RenderMatrix34*)
*/
