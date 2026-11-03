// Cafe player scalar utilities, with full-width integer results retained.
#pragma once
#include "wwhd.h"
namespace JASystem { namespace Player {
s32 extend8to16(u32 value);
f32 pitchToCent(f32 pitch, f32 range);
s32 getRandomS32();
} }
void __sinit_JASPlayer_impl_cpp();
