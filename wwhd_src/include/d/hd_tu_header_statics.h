#pragma once
/* Compiler-generated header statics of an HD translation unit, as initialised by
 * sinit_header_statics_rodata(P, D, Z, PI) (bindings.h): the copied pi pair and the two one-byte
 * header objects (P), the three __register_global_object descriptors (D, D+0xC, D+0x18, in .data),
 * the zeroed 16-byte object (Z) and the pi pair in .rodata (PI). */
#include "wwhd.h"
struct hdTuHeaderStatics_c { be<f32> mPiBounds[2]; u8 mHeaderObjects[2]; u8 _0A[2]; };
WWHD_SIZE(hdTuHeaderStatics_c, 0xC);
struct hdTuGlobalDescriptor_c { be<u32> mWords[3]; };
struct hdTuHeaderDescriptors_c { hdTuGlobalDescriptor_c mDescriptors[3]; };
WWHD_SIZE(hdTuHeaderDescriptors_c, 0x24);
struct hdTuHeaderZero_c { be<u32> mWords[4]; };
WWHD_SIZE(hdTuHeaderZero_c, 0x10);
struct hdTuPiPair_c { be<f32> mLower, mUpper; };
WWHD_SIZE(hdTuPiPair_c, 8);
