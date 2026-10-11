/** HD looping boss-icon animation timers; private decomp source. */
#include "bindings.h"
#include "d/hd_screen_command_a.h"
static f32 sBossAnimationRate;
static f32 sBossFirstRandomRange;
static f32 sBossFirstBaseInterval;
static f32 sBossUnsignedThreshold;
static f32 sBossSecondRandomRange;
static f32 sBossSecondBaseInterval;

static u32 bossInterval(f32 value, f32 threshold) {
    if (value < threshold) {
        return u32(gabi::ftoi(value));
    }
    return u32(gabi::ftoi(gabi::fsubs_ppc(value, threshold))) + 0x80000000u;
}

/* 0267468C */
static void updateLoopBossIcon(hdHudLoopBossIcon* i_this) {
    f32 rate = sBossAnimationRate;
    f32 threshold = sBossUnsignedThreshold;
    u32 timer = u32(i_this->mTimers[0]) - 1;
    i_this->mTimers[0] = timer;
    if (timer == 0) {
        f32 random = cM_rndF(sBossFirstRandomRange);
        i_this->mTimers[0] = bossInterval(gabi::fadds_ppc(random, sBossFirstBaseInterval), threshold);
        hdHudStartAnimation(i_this->animation(), 0, 0, rate);
    }
    timer = u32(i_this->mTimers[1]) - 1;
    i_this->mTimers[1] = timer;
    if (timer == 0) {
        f32 random = cM_rndF(sBossSecondRandomRange);
        f32 interval = gabi::fadds_ppc(gabi::fadds_ppc(random, random), sBossSecondBaseInterval);
        i_this->mTimers[1] = bossInterval(interval, threshold);
        hdHudStartAnimation(i_this->animation(), 1, 1, rate);
    }
    i_this->update();
}

/* WWHD glue (tools/verify/autoglue.py): HD boss-icon tuning
 data sBossAnimationRate 100F7200
 data sBossFirstRandomRange 100F7208
 data sBossFirstBaseInterval 100F720C
 data sBossUnsignedThreshold 100F7210
 data sBossSecondRandomRange 100F7214
 data sBossSecondBaseInterval 100F7218
*/
