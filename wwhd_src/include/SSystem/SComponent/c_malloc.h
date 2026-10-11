/* cMl: GameCube allocator facade.
 * HD: Heap remains the TU's guest pointer; the reference names its single mapped storage.
 * The heap's HD virtual layout is shared with the verified JKR heap adapters. */
#pragma once
#include "JSystem/JKRHeap_adapters.h"

using CmlHeapPtr = gptr<JKRHeap>;
struct cMl {
    static CmlHeapPtr& Heap;
    static void init(JKRHeap*);
    static void* memalignB(int alignment, u32 size);
    static void free(void*);
};

struct CmlHeaderStatics {
    be<f32> mLowerAngle;
    be<f32> mUpperAngle;
    u8 mFirstObject;
    u8 mSecondObject;
    u8 _0A[2];
    be<u32> mZeroWords[4];
};
struct CmlDestructorDescriptor {
    u8 mStorage[12];
};
struct CmlDestructorDescriptors {
    CmlDestructorDescriptor mEntries[3];
};
struct CmlAngleBounds {
    be<f32> mLower;
    be<f32> mUpper;
};
WWHD_OFFSET(CmlHeaderStatics, mFirstObject, 8);
WWHD_OFFSET(CmlHeaderStatics, mSecondObject, 9);
WWHD_OFFSET(CmlHeaderStatics, mZeroWords, 0xC);
WWHD_SIZE(CmlHeaderStatics, 0x1C);
WWHD_SIZE(CmlDestructorDescriptor, 0xC);
WWHD_SIZE(CmlDestructorDescriptors, 0x24);
WWHD_SIZE(CmlAngleBounds, 8);

void Cml_registerGlobalObject(CmlDestructorDescriptor*);
void Cml_constructFirstHeaderObject(u8*);
void Cml_constructSecondHeaderObject(u8*);
