/**
 * d_a_warphr_companions.cpp (WWHD)
 * Warphr - warp portal to Hyrule (Ghrwp, daWarphr_c)
 * Per-translation-unit HD actor and SafeString virtual helpers.
 *
 * Written from the WWHD code with the GameCube decompilation (zeldaret/tww src/d/actor/d_a_warphr.cpp) as
 * reference, verified against cking.rpx.
 */
#include "d/actor/d_a_warphr.h"
namespace {
static void actor_deleting_destructor(daWarphr_c *actor, s32 flags) {
  WWHD_FUNC(0x024DBADC, void, actor, flags);
  if (!actor)
    return;
  gabi::call(0x025D50BC, actor, 0);
  if (flags & 1)
    gabi::call(0x0273AF40, actor);
}
VERIFY(0x024DBADC, actor_deleting_destructor);

static void safestring_assure_termination(u8 *name) {
  WWHD_FUNC(0x024DBB30, void, name);
}
VERIFY(0x024DBB30, safestring_assure_termination);
} // namespace
