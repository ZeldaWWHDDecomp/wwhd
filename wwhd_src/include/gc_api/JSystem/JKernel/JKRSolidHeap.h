/* JKRSolidHeap's HD virtual accessors [v d_ky_thunder]. */
#pragma once
#include "bindings.h"
/* HD: vtable at +0xC, getHeapSize +0x6C, getStartAddr +0x5C. */
inline u32 JKRSolidHeap_getHeapSize(JKRSolidHeap* p) { u32 vt = gabi::load<u32>(gabi::ea(p) + 0xC); return gabi::call_ptr<u32>(gabi::load<u32>(vt + 0x6C), p); }
inline void* JKRSolidHeap_getStartAddr(JKRSolidHeap* p) { u32 vt = gabi::load<u32>(gabi::ea(p) + 0xC); return gabi::call_ptr<void*>(gabi::load<u32>(vt + 0x5C), p); }
/* HD: the unit stores its own debug name in the heap at +0x10 after creation. */
inline void JKRSolidHeap_setDebugName(JKRSolidHeap* p, const char* name) { gabi::store<u32>(gabi::ea(p) + 0x10, gabi::guest_addr(name)); }

/* HD: callers that use both slots cache the table before either virtual call. */
struct JKRSolidHeap_vtable {
    u8 _0[0x5C];
    be<u32> getStartAddr;
    u8 _60[0xC];
    be<u32> getHeapSize;
};
WWHD_OFFSET(JKRSolidHeap_vtable, getStartAddr, 0x5C);
WWHD_OFFSET(JKRSolidHeap_vtable, getHeapSize, 0x6C);
WWHD_SIZE(JKRSolidHeap_vtable, 0x70);
inline JKRSolidHeap_vtable* JKRSolidHeap_getVirtualTable(JKRSolidHeap* p) { return gabi::at<JKRSolidHeap_vtable>(gabi::load<u32>(gabi::ea(p) + 0xC)); }
inline u32 JKRSolidHeap_getHeapSize(JKRSolidHeap* p, JKRSolidHeap_vtable* vt) { return gabi::call_ptr<u32>(vt->getHeapSize, p); }
inline void* JKRSolidHeap_getStartAddr(JKRSolidHeap* p, JKRSolidHeap_vtable* vt) { return gabi::call_ptr<void*>(vt->getStartAddr, p); }
