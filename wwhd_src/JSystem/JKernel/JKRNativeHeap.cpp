// HD JKernel heap adapters; private behavior reconstruction from local instructions.
#include "bindings.h"
#include "JSystem/JKernel/JKRNativeAdapters.h"

static sead::RegistryHeap* sRootHeap;
static u32 sRegionTypeInitialized;
static sead::RuntimeType sRegionType;
static sead::SafeTextVtable sHeapNameTable;
static NativeHeapNameByte sHeapName;
static u32 sRegionTypeTable;

/* 027EBCB0 */
static sead::RegistryHeap* createNativeRegionHeap(u32 size,sead::RegistryHeap* parent,u32 unused) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (size == 0xFFFFFFFF) size = 0;
    gabi::FrameLocal<sead::SafeText> name(8);
    name->mVtable = &sHeapNameTable;
    name->mText = &sHeapName;
    return sead::FreeRegionHeap::create(size,name.get(),parent,1,0);
}

/* 027EC0AC */
static void* allocateNativeMemory(u32 size,s32 alignment) {
    return gabi::tail_ptr<void*>(gabi::guest_fn((const void*)allocateNativeAligned),size,alignment);
}

/* 027EC124 */
static sead::RegistryHeap* findNativeAllocationHeap(void* memory) {
    sead::RegistryHeap* root = sRootHeap;
    return gabi::tail_ptr<sead::RegistryHeap*>(gabi::guest_fn((const void*)findRegistryHeap),root,memory);
}

/* 027EC134 */
static u32 nativeAllocationSize(sead::RegistryHeap* heap,void* memory) {
    if (heap == NULL) heap = findRegistryHeap(sRootHeap,memory);
    if (sRegionTypeInitialized == 0) {
        sRegionTypeInitialized = 1;
        sRegionType.mTable = &sRegionTypeTable;
    }
    if (heap != NULL && gabi::call_ptr<u32>(heap->dispatch()->mMatchesType.mFunction,heap,&sRegionType)) {
        return static_cast<sead::FreeRegionHeap*>(heap)->allocationSize(memory);
    }
    return 0xFFFFFFFF;
}

/* WWHD glue
 data sRootHeap 101F8B4C
 data sRegionTypeInitialized 101FD764
 data sRegionType 101FDF98
 data sHeapNameTable 1016DC20
 data sHeapName 1016DC38
 data sRegionTypeTable 1016DC78
 ext 02753004 sead::RegistryHeap* sead::FreeRegionHeap::create(u32,const sead::SafeText*,sead::RegistryHeap*,s32,u32)
 ext 02754814 u32 sead::FreeRegionHeap::allocationSize(void*)
 ext 02755FEC sead::RegistryHeap* findRegistryHeap(void*,void*) callback
 ext 0273AE48 void* allocateNativeAligned(u32,s32) callback
*/
