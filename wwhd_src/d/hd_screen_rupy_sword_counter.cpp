/* HD sword rupee counter. */
#include "d/hd_minigame_hud.h"
static u32 swordTextVtable;
static u8 swordCounterActive;
static MinigameState swordIdleState;

/* 026CB2F4 */
void hdScreenRupySwordCounter_c::updateDisplay() {
    // HD: preserve the original frame offsets and partial buffer initialization.
    gabi::NativeFrame<0x28> frame;
    MinigameText* text = gabi::at<MinigameText>(frame + 8);
    MinigameTextBuffer5* buffer = gabi::at<MinigameTextBuffer5>(frame + 0x14);
    buffer->characters[4] = 0;
    buffer->characters[0] = 0;
    text->mBuffer = buffer->characters;
    text->__vtbl = gabi::ea(&swordTextVtable);
    text->mCapacity = 5;
    minigameFormatCurrency(text, "sword currency format", Minigame_SwordCurrencyGlyph, u32(mShownCount));
    minigameSetText(mTextPane, text, 0);
}

/* 026CB4A0 */
void hdScreenRupySwordCounter_c::roll() {
    MinigameGameCounter* game = reinterpret_cast<MinigameGameCounter*>(dComIfGp_get());
    u32 shown = mShownCount;
    u32 target = game->mTargetCount;
    if (shown < target) {
        // HD: narrow before clamping, preserving the original 16-bit wrap.
        u32 next = u16(shown + 1);
        if (next > Minigame_SwordCountLimit) next = Minigame_SwordCountLimit;
        mShownCount = next;
        updateDisplay();
    } else if (shown > target) {
        mShownCount = u16(shown - 1);
        updateDisplay();
    }
    if (!swordCounterActive) minigameChangeState(&mStateMachine, &swordIdleState);
}

/* WWHD glue
data swordTextVtable 10101D6C
data swordCounterActive 1047B09A
data swordIdleState 1049A3A8
str 10101E28 "sword currency format" renamed
ext 02759D3C void minigameFormatCurrency(MinigameText*, const char*, u32, u32)
*/
