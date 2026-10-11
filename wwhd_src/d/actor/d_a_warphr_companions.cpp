/**
 * d_a_warphr_companions.cpp (WWHD)
 * Warphr - warp portal to Hyrule (Ghrwp, daWarphr_c)
 * Per-translation-unit HD actor and SafeString virtual helpers.
 *
 * Written from the WWHD code with the GameCube decompilation (zeldaret/tww src/d/actor/d_a_warphr.cpp) as
 * reference, verified against cking.rpx.
 */
#include "d/actor/d_a_warphr.h"
void warphrActorDestroy(daWarphr_c*, u32);
void warphrDelete(void*);
namespace {
/* 024DBADC */
static void WarphrActorDtor(daWarphr_c* actor, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!actor)
        return;
    warphrActorDestroy(actor, 0);
    if (flags & 1)
        warphrDelete(actor);
}

/* 024DBB30 */
static void WarphrSafeStringAssure(u8* name) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}
} // namespace

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
ext 025D50BC void warphrActorDestroy(daWarphr_c*, u32)
*/
// clang-format on
