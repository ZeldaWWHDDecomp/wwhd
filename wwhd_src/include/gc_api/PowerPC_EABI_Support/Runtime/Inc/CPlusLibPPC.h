// GHS array lifetime helpers used by verified HD constructors.
#pragma once
#include "bindings.h"
/* 028EFFD0 [v d_a_bmd] */
inline void __construct_array(void* array, u32 count, u32 size, void* (*ctor)(void*)) {
    gabi::call(0x028EFFD0, array, count, size, gabi::guest_fn((const void*)ctor));
}
/* 028F0164 [v d_a_bmd]: HD retains two trailing integer arguments. */
inline void __destroy_arr(void* array, u32 count, u32 size, void (*dtor)(void*, s32), s32 flags, s32 extra) {
    gabi::call(0x028F0164, array, count, size, gabi::guest_fn((const void*)dtor), flags, extra);
}
