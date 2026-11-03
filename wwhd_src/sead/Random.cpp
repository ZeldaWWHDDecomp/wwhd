/* WWHD sead random generator: seeding (one seed word spread over the four state words, or the tick
 * counter's low word) and the xorshift step.
 * Reconstructed from this game's instructions. */
#include "sead/Random.h"

using namespace sead;

static constexpr u32 kSeedMultiplier = 1812433253;   // 0x6C078965

/* 0275CE48 */
void sead::Random::init(u32 seed) {
    u32 x = (seed ^ (seed >> 30)) * kSeedMultiplier + 1;
    u32 y = (x ^ (x >> 30)) * kSeedMultiplier + 2;
    u32 z = (y ^ (y >> 30)) * kSeedMultiplier + 3;
    mState2 = z;
    mState1 = y;
    mState0 = x;
    mState3 = (z ^ (z >> 30)) * kSeedMultiplier + 4;
}

/* 0275CEA4 */
void sead::Random::initFromTick() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<TickTime> now(8);
    TickTime* tick = TickTime::construct(now.get());
    init(tick->mLow);
}

/* 0275CEE4 */
u32 sead::Random::getU32() {
    // One xorshift step, in the order of the original's loads and stores: the state words move down
    // by one and the new last word mixes the old last word with the old first word.
    u32 last = mState3;
    u32 first = mState0;
    u32 mixedFirst = first ^ (first << 11);
    u32 third = mState2;
    u32 second = mState1;
    mState2 = last;
    mState0 = second;
    mState1 = third;
    u32 next = (last ^ (last >> 19)) ^ (mixedFirst ^ (mixedFirst >> 8));
    mState3 = next;
    return next;
}

/* WWHD glue
 ext 02760DE8 sead::TickTime* sead::TickTime::construct(sead::TickTime*)
*/
