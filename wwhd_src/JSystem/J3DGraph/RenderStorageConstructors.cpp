// J3D HD renderer: compiler-generated element constructors used by __construct_array.
// Names are our own; written from the WWHD binary.
#include "JSystem/J3DGraph/RenderValueStorage.h"

void* allocateRenderStorage(u32 size);

/* 027FF64C */
j3d::RenderFloat3* j3d::RenderFloat3::construct(RenderFloat3* storage) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage == NULL) storage = static_cast<RenderFloat3*>(allocateRenderStorage(sizeof(RenderFloat3)));
    return storage;
}

/* 027FF67C */
j3d::RenderFloat4* j3d::RenderFloat4::construct(RenderFloat4* storage) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage == NULL) storage = static_cast<RenderFloat4*>(allocateRenderStorage(sizeof(RenderFloat4)));
    if (storage != NULL) {
        storage->mY = 0.0f;
        storage->mX = 0.0f;
        storage->mZ = 0.0f;
        storage->mW = 1.0f;
    }
    return storage;
}

/* 027FFC7C */
j3d::RenderStorageSpan* j3d::RenderStorageSpan::construct(RenderStorageSpan* storage) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage == NULL) storage = static_cast<RenderStorageSpan*>(allocateRenderStorage(sizeof(RenderStorageSpan)));
    if (storage != NULL) {
        storage->mStorage = NULL;
        storage->mCount = 0;
    }
    return storage;
}

/* 02800960 */
j3d::RenderResourceSpan* j3d::RenderResourceSpan::construct(RenderResourceSpan* storage) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage == NULL) storage = static_cast<RenderResourceSpan*>(allocateRenderStorage(sizeof(RenderResourceSpan)));
    if (storage != NULL) {
        storage->mStorage = NULL;
        storage->mCount = 0;
    }
    return storage;
}

/* WWHD glue
 ext 0273AD10 void* allocateRenderStorage(u32)
*/
