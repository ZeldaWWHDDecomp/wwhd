/* HD cannon-game target reveal. */
#include "d/hd_minigame_hud.h"

/* 0264679C */
void hdScreenCannonGame_c::reveal(u32 target) {
    u32 n = mRevealedTargets;
    if (n >= target) return;
    u32 index = n + Minigame_CannonTargetPane;
    gptr<MinigamePane>* slots = mPanes;
    // HD: an out-of-range pane index falls back to the first slot.
    if (index < mPaneCount) slots += index;
    slots->get()->mVisible = 1;
    mRevealedTargets = u32(mRevealedTargets) + 1;
}

/* WWHD glue
*/
