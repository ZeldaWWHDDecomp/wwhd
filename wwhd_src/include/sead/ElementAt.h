/* Array elements addressed the way the game's code addresses them. */
#pragma once
#include "wwhd.h"

namespace sead {
/* array[index] of the guest array at guest address `array`. The console forms the address
 * array + index * sizeof(T) in 32 bits (slwi/mulli + add, e.g. a terminator store at
 * text + 2 * capacity - 2), so it wraps around the guest address space. Indexing the host view
 * instead (T* array; array[index]) does not wrap: for a NULL array and a large index (a u32
 * capacity - 1 of 0xFFFFFFFF, a count or slot index read from the object) it reaches a host
 * address gigabytes above the host's NULL, which the verify harness's guest-memory guard reads
 * as a host object whenever it lands in the host stack, a malloc region or the executable
 * (load-address dependent). */
template <class T> inline T& elementAt(u32 array, u32 index) { return *gabi::at<T>(array + index * u32(sizeof(T))); }
template <class T> inline T& elementAt(const gptr<T>& array, u32 index) { return elementAt<T>(u32(array.v), index); }
template <class T> inline T& elementAt(T* array, u32 index) { return elementAt<T>(gabi::ea(array), index); }
/* &array[index] and the element after `element`, as pointers formed the same way (NULL for guest address 0). For pointer
 * walks the console runs as `end = first + count * size; ... p += size; cmplw p,end`: the end wraps in 32 bits, so a
 * count whose byte size wraps 2^32 ends the walk early (count 2^30 of 4-byte elements: end == first, no step), and a
 * walk from a NULL array steps through guest addresses 0, size, 2 * size, ... up to that end. */
template <class T> inline T* elementPtr(T* array, u32 index) { return gabi::at<T>(gabi::ea(array) + index * u32(sizeof(T))); }
template <class T> inline T* nextElement(T* element) { return gabi::at<T>(gabi::ea(element) + u32(sizeof(T))); }
}
