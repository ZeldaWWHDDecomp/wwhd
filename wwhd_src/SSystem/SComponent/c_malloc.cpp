/**
 * c_malloc.cpp: allocation through the selected game heap.
 * GameCube names and control flow; verified against the HD implementation.
 */
#include "SSystem/SComponent/c_malloc.h"

static CmlHeapPtr l_heap;
static CmlHeaderStatics l_headerStatics;
static CmlDestructorDescriptors l_destructors;
static CmlAngleBounds l_angleBounds;

// HD: a reference exposes the tww static member over the single guest-storage definition.
CmlHeapPtr& cMl::Heap = l_heap;

/* 02019424 */
void cMl::init(JKRHeap* heap) { Heap = heap; }

/* 02019430 */
void* cMl::memalignB(int alignment, u32 size) {
    if (size == 0) {
        return nullptr;
    }
    // HD: the allocator's virtual table and slots differ from the GameCube layout.
    auto* heap = reinterpret_cast<heap_adapters::Heap*>(Heap.get());
    return gabi::at<void>(heap->alloc(size, alignment));
}

/* 0201945C */
void cMl::free(void* memory) {
    if (memory != nullptr) {
        auto* heap = reinterpret_cast<heap_adapters::Heap*>(Heap.get());
        heap->free(gabi::ea(memory));
    }
}

// HD: compiler-generated and template code of this translation unit.
/* 0201947C */
static void __sinit_c_malloc_cpp() {
    l_headerStatics.mZeroWords[2] = 0;
    l_headerStatics.mZeroWords[0] = 0;
    l_headerStatics.mZeroWords[3] = 0;
    l_headerStatics.mZeroWords[1] = 0;
    Cml_registerGlobalObject(&l_destructors.mEntries[0]);
    f32 lower = l_angleBounds.mLower;
    f32 upper = l_angleBounds.mUpper;
    l_headerStatics.mLowerAngle = lower;
    l_headerStatics.mUpperAngle = upper;
    Cml_constructFirstHeaderObject(&l_headerStatics.mFirstObject);
    Cml_registerGlobalObject(&l_destructors.mEntries[1]);
    Cml_constructSecondHeaderObject(&l_headerStatics.mSecondObject);
    Cml_registerGlobalObject(&l_destructors.mEntries[2]);
}

// clang-format off
/* WWHD glue
data l_heap 1018CA3C
data l_headerStatics 101FF9B8
data l_destructors 1018CA18
data l_angleBounds 100035B0
ext 028F026C void Cml_registerGlobalObject(CmlDestructorDescriptor* descriptor)
ext 028ED6F8 void Cml_constructFirstHeaderObject(u8* object)
ext 028EAB2C void Cml_constructSecondHeaderObject(u8* object)
*/

// clang-format on
