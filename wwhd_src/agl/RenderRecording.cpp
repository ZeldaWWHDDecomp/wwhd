// AGL command recording and small storage records, reconstructed from the supplied WWHD binary.
#include "agl/RenderRecording.h"
namespace agl {}
using namespace agl;
static sead::DisposerVtable sRecordingMethods;
static sead::SafeTextVtable sRecordingNameMethods;
typedef gptr<be<u8>> RecordingNamePointer;
static RecordingNamePointer sDefaultRecordingName;
void beginGraphicsDisplayList(void*,u32,u32);
u32 endGraphicsDisplayList(void*);
void currentGraphicsDisplayList(gptr<void>*,be<u32>*);
void completeGraphicsWork();

/* 027C05A0 */
void agl::DisplayListRecording::setStorage(void* data,u32 capacity) {
    gabi::AutoFrame frame{gabi::current_function()};
    u8 flags=mFlags;
    u32 available=data ? capacity : 0;
    mCapacity=available;
    mData=data;
    mFlags=flags&~1u;
}
/* 027C05C8 */
void agl::DisplayListRecording::clear() {
    gabi::AutoFrame frame{gabi::current_function()};
    setStorage(nullptr,0);
    mUsed=0;
}
/* 027C05F4 */
agl::DisplayListRecording* agl::DisplayListRecording::create(DisplayListRecording* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!storage) storage=static_cast<DisplayListRecording*>(allocateRegistryStorage(sizeof(DisplayListRecording)));
    if(storage) {
        storage->mVtable=&sRecordingMethods;
        storage->mName.mVtable=&sRecordingNameMethods;
        storage->mName.mText=sDefaultRecordingName;
        be<u8>* flags=&storage->mFlags;
        if(!gabi::ea(flags)) flags=static_cast<be<u8>*>(allocateRegistryStorage(1));
        if(flags) *flags=0;
        storage->clear();
    }
    return storage;
}
/* 027C0680 */
void agl::DisplayListRecording::release() {
    gabi::AutoFrame frame{gabi::current_function()};
    if(mFlags&1) releaseRegistryStorage(mData);
    clear();
}
/* 027C06C4 */
void agl::DisplayListRecording::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(gabi::ea(this)) {
        mVtable=&sRecordingMethods;
        release();
        if(flags&1) releaseRegistryStorage(this);
    }
}
/* 027C0720 */
void agl::DisplayListRecording::copyStorage(DisplayListStorage* destination) const {
    gabi::AutoFrame frame{gabi::current_function()};
    destination->mData=mData;
    destination->mCapacity=mCapacity;
    destination->mUsed=mUsed;
}
/* 027C073C */
u32 agl::DisplayListRecording::begin(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 capacity=mCapacity;
    u32 base=gabi::ea(static_cast<void*>(mData));
    mUsed=0;
    beginGraphicsDisplayList(gabi::at<void>(base+capacity*index),capacity,0);
    return 1;
}
/* 027C077C */
u32 agl::DisplayListRecording::end(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<gptr<void>> current(8);
    gabi::FrameLocal<be<u32>> capacity(0xC);
    currentGraphicsDisplayList(current,capacity);
    u32 size=endGraphicsDisplayList(*current);
    u32 stride=mCapacity;
    u32 base=gabi::ea(static_cast<void*>(mData));
    u32 recording=gabi::ea(static_cast<void*>(*current));
    u32 used=size+recording-(base+index*stride);
    mUsed=used;
    return used;
}
/* 027C0588 */
void agl::CaptureEncodingPolicy::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(gabi::ea(this) && (flags&1)) releaseRegistryStorage(this);
}
/* 027C059C */
void agl::CaptureEncodingPolicy::finish() {
    gabi::AutoFrame frame{gabi::current_function()};}
/* 027C07E4 */
void agl::RecordingPolicy::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(gabi::ea(this) && (flags&1)) releaseRegistryStorage(this);
}
/* 027C07F8 */
void agl::RecordingPolicy::finish() {
    gabi::AutoFrame frame{gabi::current_function()};}
/* 027C07FC */
u32 agl::RecordingPolicy::alignment() {
    gabi::AutoFrame frame{gabi::current_function()}; return 32; }
/* 027C2338 */
void agl::RenderStorageHandle::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(gabi::ea(this) && (flags&1)) releaseRegistryStorage(this);
}
/* 027C234C */
agl::RenderStorageHandle* agl::RenderStorageHandle::create(RenderStorageHandle* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!storage) storage=static_cast<RenderStorageHandle*>(allocateRegistryStorage(sizeof(RenderStorageHandle)));
    return storage;
}
/* 027C2378 */
void agl::RenderAddressRange::finish() {
    gabi::AutoFrame frame{gabi::current_function()};}
/* 027C237C */
void agl::RenderAddressRange::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(gabi::ea(this) && (flags&1)) releaseRegistryStorage(this);
}
/* 027C2390 */
agl::RenderAddressRange* agl::RenderAddressRange::create(RenderAddressRange* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!storage) storage=static_cast<RenderAddressRange*>(allocateRegistryStorage(sizeof(RenderAddressRange)));
    if(storage) { storage->mData=nullptr; storage->mCapacity=0; storage->mCount=0; }
    return storage;
}
/* 027BFEF8 */
agl::VertexAttributeBinding* agl::VertexAttributeBinding::create(VertexAttributeBinding* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!storage) storage=static_cast<VertexAttributeBinding*>(allocateRegistryStorage(sizeof(VertexAttributeBinding)));
    if(storage) { storage->mSource=nullptr; storage->mBufferSlot=-1; storage->mAttribute=-1; }
    return storage;
}
/* 027BF730 */
void agl::waitForGraphicsCompletion() {
    gabi::AutoFrame frame{gabi::current_function()}; completeGraphicsWork(); }

/* WWHD glue
data sRecordingMethods 10169E2C
data sRecordingNameMethods 10169E14
data sDefaultRecordingName 104A0CD8
ext 0273AD10 void* allocateRegistryStorage(u32)
ext 0273AF40 void releaseRegistryStorage(void*)
ext C0006090 void beginGraphicsDisplayList(void*,u32,u32)
ext C0006190 u32 endGraphicsDisplayList(void*)
ext C0006218 void currentGraphicsDisplayList(gptr<void>*,be<u32>*)
ext C0006168 void completeGraphicsWork()
*/
