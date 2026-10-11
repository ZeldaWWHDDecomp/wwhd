/* HD minigame HUD helpers. */
#pragma once
#include "bindings.h"

struct MinigamePane {
    u8 padding[0x50];
    be<u8> mVisible;
};
struct MinigameState { u8 storage[1]; };
struct MinigameStateMachine { u8 storage[1]; };
struct MinigameScreenBase {
    u8 padding[4];
    gptr<gfn> mVtable;
    u8 padding2[0x10];
    MinigameStateMachine mStateMachine;
    u8 padding3[0x2F];
    be<u32> mPaneCount;
    gptr<gptr<MinigamePane>> mPanes;
    gptr<void> mTextPane;
};
struct MinigameText {
    gptr<be<u16>> mBuffer;
    be<u32> __vtbl;
    be<u32> mCapacity;
};
struct MinigameTextBuffer3 { be<u16> characters[3]; };
struct MinigameTextBuffer5 { be<u16> characters[5]; };
struct MinigameAnimationNames { be<u32> animation, group; };
struct MinigameAnimationScene { u8 padding[0xD10]; be<u8> mActive; };
class hdScreenBattleship_c : public MinigameScreenBase {
public:
    u8 padding[4];
    be<u32> mRevealedMisses;
    be<u32> mRevealedHits;
    void updateDisplay();
    void reveal(u32 first, u32 second);
};
class hdScreenBoatRaceFailedChar_c : public MinigameScreenBase {
public:
    gptr<MinigameAnimationScene> mScene;
    be<u32> mWaitTimer;
    void wait();
};
class hdScreenBoatRaceResult_c : public MinigameScreenBase {
public:
    u8 padding[4];
    be<u32> mResult;
    be<u32> mWaitTimer;
    be<u32> mRollFrame;
    void wait();
    void roll();
};
class hdScreenCannonGame_c : public MinigameScreenBase {
public:
    u8 padding[8];
    be<u32> mRevealedTargets;
    void reveal(u32 target);
};
class hdScreenRupySwordCounter_c : public MinigameScreenBase {
public:
    be<u16> mShownCount;
    void updateDisplay();
    void roll();
};
struct MinigameGameCounter { u8 padding[0x5CEC]; be<u16> mTargetCount; };
WWHD_OFFSET(MinigameScreenBase, mStateMachine, 0x18);
WWHD_SIZE(MinigameScreenBase, 0x54);
WWHD_SIZE(MinigameText, 12);
WWHD_OFFSET(hdScreenBoatRaceResult_c, mRollFrame, 0x60);
WWHD_OFFSET(hdScreenCannonGame_c, mRevealedTargets, 0x5C);
WWHD_OFFSET(hdScreenRupySwordCounter_c, mShownCount, 0x54);
enum MinigameHudConstants : u32 {
    Minigame_BattleshipMissPane = 30,
    Minigame_CannonTargetPane = 10,
    Minigame_SwordCountLimit = 999,
    Minigame_ScreenRollSlot = 35,
    Minigame_ResultSecondPrize = 2,
    Minigame_ResultStartSound = 0x849,
    Minigame_ResultPrizeSound = 0x860,
    Minigame_SwordCurrencyGlyph = 0xD7,
};
void minigameFormatCount(MinigameText*, const char*, u32);
void minigameFormatCurrency(MinigameText*, const char*, u32, u32);
void minigameSetText(void*, MinigameText*, u32);
void minigamePlayAnimation(MinigameAnimationScene*, MinigameAnimationNames*);
void minigameChangeState(MinigameStateMachine*, MinigameState*);
void minigamePlaySound(u32);
