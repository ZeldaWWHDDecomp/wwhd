/* HD battleship HUD. */
#include "d/hd_minigame_hud.h"
static u32 battleshipTextVtable;

/* 02623B44 */
void hdScreenBattleship_c::updateDisplay() {
    // HD: keep the original frame offsets and the untouched middle buffer word.
    gabi::NativeFrame<0x20> frame;
    MinigameText* text = gabi::at<MinigameText>(frame + 8);
    MinigameTextBuffer3* buffer = gabi::at<MinigameTextBuffer3>(frame + 0x14);
    buffer->characters[2] = 0;
    buffer->characters[0] = 0;
    text->mBuffer = buffer->characters;
    text->__vtbl = gabi::ea(&battleshipTextVtable);
    text->mCapacity = 3;
    minigameFormatCount(text, "battleship count format", mRevealedHits);
    minigameSetText(mTextPane, text, 0);
}

/* 02624134 */
void hdScreenBattleship_c::reveal(u32 first, u32 second) {
    u32 n = mRevealedMisses;
    if (n < second) {
        u32 index = n + Minigame_BattleshipMissPane;
        gptr<MinigamePane>* slots = mPanes;
        // HD: an out-of-range pane index falls back to the first slot.
        if (index < mPaneCount) slots += index;
        slots->get()->mVisible = 1;
        mRevealedMisses = u32(mRevealedMisses) + 1;
    }
    n = mRevealedHits;
    if (n >= first) return;
    gptr<MinigamePane>* slots = mPanes;
    if (n < mPaneCount) slots += n;
    slots->get()->mVisible = 1;
    mRevealedHits = first;
    updateDisplay();
}

/* WWHD glue
data battleshipTextVtable 100E49F4
str 100E4AAC "battleship count format" renamed
ext 02759D3C void minigameFormatCount(MinigameText*, const char*, u32)
ext 027046D4 void minigameSetText(void*, MinigameText*, u32)
*/
