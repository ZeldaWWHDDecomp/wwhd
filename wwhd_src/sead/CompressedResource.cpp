/* WWHD sead SZS-compressed resources: size and decompression of a resource that holds an SZS image, the
 * translation unit's file-handle destructor copy and its static initialiser.
 * Reconstructed from this game's instructions. */
#include "sead/CompressedResource.h"

using namespace sead;

static void releaseObject(void* object);
static void destroyDisposer(Disposer* disposer, u32 flags);

static constexpr u32 kDestroyRelease = 1;

static DisposerVtable sFileHandleVtable;
static InterfaceObject sLoaderInterface;
static u32 sLoaderInterfaceVtable;

/* 0275F800 */
u32 sead::compressed_resource::getDecompSize(DirectResource* resource) {
    return SZSDecompressor::getDecompSize(reinterpret_cast<SzsHeader*>(resource->mRawData.get()));
}

/* 0275F808 */
s32 sead::compressed_resource::decompressTo(DirectResource* resource, u8* destination, u32 destinationSize) {
    s32 result = -2;   // the destination is too small
    if (getDecompSize(resource) <= destinationSize) {
        result = SZSDecompressor::decomp(destination, destinationSize,
                                         reinterpret_cast<SzsHeader*>(resource->mRawData.get()), resource->mRawSize);
    }
    return result;
}

/* 0275F87C */
void sead::compressed_resource_text::FileHandle::destroy(u32 flags) {
    if (this == NULL) return;
    FileDevice* device = mSourceDevice;
    mDisposer.mVtable = &sFileHandleVtable;
    if (device != NULL) device->tryClose(this);
    destroyDisposer(&mDisposer, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275F8F0 */
static void __sinit_CompressedResource_cpp() {
    sLoaderInterface.mVtable = &sLoaderInterfaceVtable;
}

/* WWHD glue
 data sFileHandleVtable 1015E2E0 nocheck
 data sLoaderInterface 104A0CE8
 data sLoaderInterfaceVtable 1015E2F0 nocheck
 ext 0273AF40 void releaseObject(void*)
 ext 02752BEC void destroyDisposer(sead::Disposer*, u32)
*/
