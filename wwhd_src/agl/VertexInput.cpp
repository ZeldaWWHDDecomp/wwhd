// AGL vertex attribute binding, reconstructed from the local WWHD binary.
#include "agl/VertexInput.h"
namespace agl {}
using namespace agl;
// Read-only first-record views; indexing does not assert the extent of the complete tables.
static const VertexFormatInfo sVertexFormatFirst;
static const VertexPackingInfo sVertexPackingFirst;
static sead::DisposerVtable sVertexInputMethods;
void initializeVertexAttributeArray(void*,u32,u32,u32);
sead::RegistryHeap* currentVertexInputHeap(sead::RegistryManagerState*);
sead::RegistryHeap* containingVertexInputHeap(sead::RegistryManagerState*,void*);
void releaseVertexInputArray(void*);
void* allocateVertexInputArray(u32,sead::RegistryHeap*,s32);
u32 graphicsFetchProgramSize(u32,u32,u32);

static const VertexFormatInfo* formatInfo(u32 index) {
    return gabi::at<const VertexFormatInfo>(gabi::ea(&sVertexFormatFirst)+index*sizeof(VertexFormatInfo));
}
/* 027BFF40 */
u32 agl::vertexFormatBytes(u32 format) { return formatInfo(format)->mByteCount; }
/* 027BFF54 */
u32 agl::vertexFormatChannels(u32 format) { return formatInfo(format)->mChannelCount; }
/* 027BFF68 */
u32 agl::vertexFormatChannelBits(u32 format,u32 channel) {
    return *gabi::at<const be<u8>>(gabi::ea(formatInfo(format))+channel);
}
/* 027BFF80 */
u32 agl::vertexFormatChannelOrder(u32 format,u32 channel) {
    return *gabi::at<const be<u8>>(gabi::ea(formatInfo(format))+offsetof(VertexFormatInfo,mChannelOrder)+channel);
}
/* 027BFF9C */
u32 agl::vertexFormatPacking(u32 format,u32 component) {
    return *gabi::at<const be<u32>>(gabi::ea(&sVertexPackingFirst)+format*sizeof(VertexPackingInfo)+component*sizeof(be<u32>));
}
/* 027BF8DC */
void agl::VertexInputSet::clear() {
    for(s32 location=0;location<16;++location) mBindings[location].mSource=nullptr;
    for(s32 index=0;index<mBufferCount;++index) mBuffers[index]=nullptr;
    mPrepared=0;
}
/* 027BFA24 */
s32 agl::VertexInputSet::removeBinding(VertexAttributeBinding* binding) {
    u32 source=gabi::ea(static_cast<VertexBufferPrefix_l*>(binding->mSource));
    binding->mSource=nullptr;
    binding->mAttribute=1;
    for(s32 location=0;location<16;++location) {
        if(gabi::ea(static_cast<VertexBufferPrefix_l*>(mBindings[location].mSource))==source) return -1;
    }
    u32 index=binding->mBufferSlot;
    gptr<VertexBufferPrefix_l>* slot=mBuffers;
    if(index<static_cast<u32>(mBufferCount)) slot+=index;
    *slot=nullptr;
    return -1;
}
/* 027BFA9C */
s32 agl::VertexInputSet::assignBinding(VertexAttributeBinding* binding,VertexBufferPrefix_l* source,s32 attribute) {
    gabi::AutoFrame frame{gabi::current_function()};
    binding->mSource=source;
    binding->mAttribute=attribute;
    u32 count=mBufferCount;
    s32 vacant=-1;
    gptr<VertexBufferPrefix_l>* buffers=mBuffers;
    for(s32 index=static_cast<s32>(count)-1;index>=0;--index) {
        gptr<VertexBufferPrefix_l>* slot=buffers;
        if(static_cast<u32>(index)<count) slot+=index;
        VertexBufferPrefix_l* current=*slot;
        if(current==static_cast<VertexBufferPrefix_l*>(binding->mSource)) return index;
        if(!current) vacant=index;
    }
    gptr<VertexBufferPrefix_l>* slot=buffers;
    if(static_cast<u32>(vacant)<count) slot+=vacant;
    *slot=source;
    return vacant;
}
/* 027BFB60 */
void agl::VertexInputSet::setBinding(s32 location,VertexBufferPrefix_l* source,s32 attribute) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(location==-1) return;
    VertexAttributeBinding* binding=&mBindings[static_cast<u32>(location)<16 ? location : 0];
    if(binding->mSource) {
        s32 slot=removeBinding(binding);
        binding->mBufferSlot=slot;
    }
    if(source) {
        binding=&mBindings[static_cast<u32>(location)<16 ? location : 0];
        s32 slot=assignBinding(binding,source,attribute);
        binding->mBufferSlot=slot;
    }
}
/* 027BF734 */
agl::VertexInputSet* agl::VertexInputSet::create(VertexInputSet* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!storage) storage=static_cast<VertexInputSet*>(allocateRegistryStorage(sizeof(VertexInputSet)));
    if(storage) {
        storage->mVtable=&sVertexInputMethods;
        initializeVertexAttributeArray(storage->mBindings,16,sizeof(VertexAttributeBinding),gabi::guest_fn(reinterpret_cast<const void*>(&VertexAttributeBinding::create)));
        be<s32>* count=&storage->mBufferCount;
        if(!gabi::ea(count)) count=static_cast<be<s32>*>(allocateRegistryStorage(8));
        if(count) { count[1]=0; count[0]=0; }
        storage->mInitialized=0;
        storage->mPrepared=0;
        storage->mFetchProgram=nullptr;
    }
    return storage;
}
/* 027BF7E8 */
void agl::VertexInputSet::release() {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!mInitialized) return;
    if(mBuffers) {
        sead::RegistryHeap* heap=containingVertexInputHeap(sRegistryManager,mBuffers);
        u32 release=heap->dispatch()->mRelease.mFunction;
        gabi::call_ptr<void>(release,heap,static_cast<gptr<VertexBufferPrefix_l>*>(mBuffers));
        mBufferCount=0;
        mBuffers=nullptr;
    }
    if(mFetchProgram) { releaseVertexInputArray(mFetchProgram); mFetchProgram=nullptr; }
    mInitialized=0;
    mPrepared=0;
}
/* 027BF880 */
void agl::VertexInputSet::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(gabi::ea(this)) {
        mVtable=&sVertexInputMethods;
        release();
        if(flags&1) releaseRegistryStorage(this);
    }
}
/* 027BF960 */
void agl::VertexInputSet::allocate(s32 count,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(count>0) {
        sead::RegistryHeap* allocator=heap ? heap : currentVertexInputHeap(sRegistryManager);
        void* buffers=gabi::call_ptr<void*>(allocator->dispatch()->mAllocate.mFunction,allocator,static_cast<u32>(count)*4,4);
        if(buffers) { mBuffers=static_cast<gptr<VertexBufferPrefix_l>*>(buffers); mBufferCount=count; }
    }
    u32 bytes=graphicsFetchProgramSize(16,0,0);
    mFetchProgram=allocateVertexInputArray(bytes,heap,256);
    clear();
    mInitialized=1;
}

struct FetchAttribute {
    be<u32> mLocation,mBuffer,mOffset,mFormat,mType,mStep,mMask,mByteSwapped;
};
struct FetchAttributeArray { FetchAttribute mEntries[16]; };
WWHD_SIZE(FetchAttribute,32);
void initializeGraphicsFetchShader(void*,void*,u32,const FetchAttribute*,u32,u32);
void flushGraphicsFetchProgram(void*,u32);
void bindGraphicsFetchShader(const void*);
void bindGraphicsVertexBuffer(u32,u32,u32,const void*);
/* 027BFC74 */
void agl::VertexInputSet::prepare() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<FetchAttributeArray> attributes(8);
    u32 count=0;
    for(u32 location=0;location<16;++location) {
        const VertexAttributeBinding* binding=&mBindings[location];
        VertexBufferPrefix_l* source=binding->mSource;
        u32 attribute=binding->mAttribute;
        if(!source) continue;
        const VertexBufferElement* element=&source->mElements[attribute<16 ? attribute : 0];
        u32 mode=element->mFetchMode;
        FetchAttribute* output=&attributes->mEntries[count];
        output->mBuffer=binding->mBufferSlot;
        u32 offset=element->mOffset;
        output->mLocation=location;
        output->mOffset=offset;
        output->mFormat=element->mFormat;
        u32 mask=element->mComponentMask;
        output->mStep=mode;output->mMask=mask;output->mType=mode;
        u32 swap=element->mByteSwapped;
        output->mByteSwapped=swap;
        ++count;
    }
    initializeGraphicsFetchShader(mFetchShader,mFetchProgram,count,attributes->mEntries,0,0);
    u32 bytes=graphicsFetchProgramSize(count,0,0);
    flushGraphicsFetchProgram(mFetchProgram,bytes);
    mPrepared=1;
}
/* 027BFE5C */
void agl::VertexInputSet::bind() {
    gabi::AutoFrame frame{gabi::current_function()};
    bindGraphicsFetchShader(mFetchShader);
    for(s32 index=0;index<mBufferCount;++index) {
        gptr<VertexBufferPrefix_l>* slot=mBuffers;
        if(static_cast<u32>(index)<static_cast<u32>(mBufferCount)) slot=gabi::at<gptr<VertexBufferPrefix_l>>(gabi::ea(slot)+static_cast<u32>(index)*4);
        VertexBufferPrefix_l* source=*slot;
        if(source) {
            const void* data=source->mData;
            u32 bytes=source->mBytes;
            u32 stride=source->mStride;
            bindGraphicsVertexBuffer(index,bytes,stride,data);
        }
    }
}

/* WWHD glue
data sVertexFormatFirst 101F8E0C
data sVertexPackingFirst 101F9208
data sVertexInputMethods 10169DBC
ext 0273AFC8 void releaseVertexInputArray(void*)
ext 0273B0D4 void* allocateVertexInputArray(u32,sead::RegistryHeap*,s32)
ext 02756140 sead::RegistryHeap* currentVertexInputHeap(sead::RegistryManagerState*)
ext 02755FEC sead::RegistryHeap* containingVertexInputHeap(sead::RegistryManagerState*,void*)
ext 028EFFD0 void initializeVertexAttributeArray(void*,u32,u32,u32)
ext C00060B8 u32 graphicsFetchProgramSize(u32,u32,u32)
ext C0006400 void initializeGraphicsFetchShader(void*,void*,u32,const FetchAttribute*,u32,u32)
ext C00088A0 void flushGraphicsFetchProgram(void*,u32)
ext C0006860 void bindGraphicsFetchShader(const void*)
ext C0006798 void bindGraphicsVertexBuffer(u32,u32,u32,const void*)
*/
