/* Material copy construction, including bounded names and capacity-based parameter copies. */
#include "nw/lyt/MaterialGpuCopy.h"
using namespace nw::lyt;
static u32 materialCopyStorageSize(u32 flags) {
    return (flags>>30)*28+((flags>>28)&3)*20+((flags>>26)&3)*8+((flags>>23)&7)*4+
        ((flags>>22)&1)*8+((flags>>20)&3)*4+((flags>>19)&1)*12+((flags>>16)&7)*20+((flags>>15)&1)*8;
}
/* 0288247C */
MaterialGpu* MaterialGpu::copyFrom(MaterialGpu* copy,const MaterialGpu* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!copy) copy=static_cast<MaterialGpu*>(materialCopyAllocate(sizeof(MaterialGpu)));
    if (!copy) return copy;
    copy->methods=materialGpuMethodTable();
    materialCopyConstructArray(copy,2,4,createPixelUniformLocation);
    copy->storage=nullptr;
    copy->setProgramIndex(source->programIndex());
    copy->ownershipFlags=static_cast<u8>(source->ownershipFlags);
    u32 count=0;
    while (count<28) {
        u32 character=source->name[count];
        if (!character) break;
        copy->name[count]=character;
        ++count;
    }
    layoutAtOffset<be<u8>>(copy->name,count)->set(0);
    for (u32 i=0;i<2;++i) copy->colors[i].value=static_cast<u32>(source->colors[i].value);
    copy->clearCounts(&copy->layoutFlags);
    copy->clearCounts(&copy->featureFlags);
    u32 capacity=source->layoutFlags;
    u32 projection=(static_cast<u32>(source->layoutFlags)>>15)&1;
    copy->reserve(capacity>>30,(capacity>>28)&3,(capacity>>26)&3,(capacity>>23)&7,
        (capacity>>22)&1,(capacity>>20)&3,(capacity>>19)&1,(capacity>>16)&7,projection);
    if (copy->storage) {
        u32 features=copy->featureFlags;
        features=(features&~0xC0000000u)|(static_cast<u32>(source->featureFlags)&0xC0000000u);
        copy->featureFlags=features;
        features=(features&~0x30000000u)|(static_cast<u32>(source->featureFlags)&0x30000000u);
        copy->featureFlags=features;
        features=(features&~0x0C000000u)|(static_cast<u32>(source->featureFlags)&0x0C000000u);
        capacity=copy->layoutFlags;
        copy->featureFlags=features;
        features=(features&~0x03800000u)|(static_cast<u32>(source->featureFlags)&0x03800000u);
        copy->featureFlags=features;
        features=(features&~0x00070000u)|(static_cast<u32>(source->featureFlags)&0x00070000u);
        copy->featureFlags=features;
        features=(features&~0x00008000u)|(static_cast<u32>(source->featureFlags)&0x00008000u);
        copy->featureFlags=features;
        void* destination=copy->storage;
        const void* input=source->storage;
        materialCopyMemory(destination,input,materialCopyStorageSize(capacity));
    }
    return copy;
}
/* WWHD glue
ext 0273AD10 void* materialCopyAllocate(u32)
ext 028EFFD0 void materialCopyConstructArray(void*,u32,u32,nw::lyt::MaterialCopyConstructor)
ext C000A848 void materialCopyMemory(void*,const void*,u32)
*/
