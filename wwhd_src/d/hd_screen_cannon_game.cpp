/* HD cannon-game target reveal. */
#include "d/hd_minigame_hud.h"

/* 0264679C */
void hdScreenCannonGame_c::reveal(u32 target) {
    u32 n = mRevealedTargets;
    if (n >= target) return;
    // HD: an out-of-range child index falls back to the first child.
    static_cast<OneTimePart_l*>(mChildren.safeSlot(n + Minigame_CannonTargetPane)->get())->mTriggered = 1;
    mRevealedTargets = u32(mRevealedTargets) + 1;
}

/* WWHD glue
*/
