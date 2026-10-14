/* HD minigame HUD helpers. */
#pragma once
#include "bindings.h"
#include "d/hd_parts.h"
#include "d/hd_parts_no_control.h"
#include "d/hd_parts_one_time.h"
#include "d/hd_screen_cursor.h"

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
/* the battleship (squid hunt) minigame screen "BattleshipGame_00" (hd_screen_battleship_game.cpp,
 * hd_screen_battleship.cpp); children 0..29 are cannonball icons, 30..32 the squids (OneTimePart_l) */
struct hdScreenBattleship_c : hdParts_c {
    /* 0x50 */ gptr<void> mpTextPanes[2]; /* "T_NumCannonball_00", "T_NumBestScore_00" */
    /* 0x58 */ be<u32> mRevealedMisses;
    /* 0x5C */ be<u32> mRevealedHits;
    /* 0x60 */ be<u32> mBestScore;
    /* 0x64 */ be<u8> mVisible;
    /* 0x65 */ u8 _65[3];

    static void destroy(hdScreenBattleship_c* i_this, s32 flags);
    void updateBestScore();
    void reset();
    void inAllEnter();
    void inAllExecute();
    void outAllExecute();
    void outAllExit();
    static hdScreenBattleship_c* construct(hdScreenBattleship_c* i_this);
    void findTextPanes();
    u32 init(u32 arg1, u32 arg2, void* desc);
    void finalize();
    void executeState();
    void draw(void* info);
    u32 createChild(hdPartsDesc_c* desc);
    void updateDisplay();
    void reveal(u32 first, u32 second);

    u32 vCreateChildren() { return gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this); }
};
WWHD_OFFSET(hdScreenBattleship_c, mpTextPanes, 0x50);
WWHD_OFFSET(hdScreenBattleship_c, mRevealedMisses, 0x58);
WWHD_OFFSET(hdScreenBattleship_c, mRevealedHits, 0x5C);
WWHD_OFFSET(hdScreenBattleship_c, mBestScore, 0x60);
WWHD_OFFSET(hdScreenBattleship_c, mVisible, 0x64);
WWHD_SIZE(hdScreenBattleship_c, 0x68);
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
/* the cannon minigame screen "CannonGame_00" (hd_screen_cannon_game_2.cpp, hd_screen_cannon_game.cpp);
 * children 0..9 are cannonball icons, 10..14 battleship icons (OneTimePart_l) */
struct hdScreenCannonGame_c : hdParts_c {
    /* 0x50 */ gptr<hdCursorPane_c> mpAnglePane;  /* "N_BatteryAngle_00" */
    /* 0x54 */ gptr<void> mpDegreeText;           /* "T_NumDegree_00" */
    /* 0x58 */ be<f32> mAngle;                     /* degrees shown and turned */
    /* 0x5C */ be<u32> mRevealedTargets;
    /* 0x60 */ be<u32> mShotCount;
    /* 0x64 */ be<u8> mVisible;
    /* 0x65 */ u8 _65[3];

    static void destroy(hdScreenCannonGame_c* i_this, s32 flags);
    void updateDegreeText();
    void turnCannon();
    void reset();
    void inAllEnter();
    void inAllExecute();
    void outAllEnter();
    void outAllExecute();
    void outAllExit();
    static hdScreenCannonGame_c* construct(hdScreenCannonGame_c* i_this);
    u32 init(u32 arg1, u32 arg2, void* desc);
    void finalize();
    void executeState();
    void draw(void* info);
    void setShotCount(u32 count);
    void setAngle(f32 angle);
    u32 createChild(hdPartsDesc_c* desc);
    u32 load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
    void reveal(u32 target);

    u32 vCreateChildren() { return gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this); }
};
WWHD_OFFSET(hdScreenCannonGame_c, mAngle, 0x58);
WWHD_OFFSET(hdScreenCannonGame_c, mRevealedTargets, 0x5C);
WWHD_OFFSET(hdScreenCannonGame_c, mVisible, 0x64);
WWHD_SIZE(hdScreenCannonGame_c, 0x68);
/* the sword minigame's rupee counter "RupySwordCounter_00" (hd_screen_rupy_sword_counter_2.cpp,
 * hd_screen_rupy_sword_counter.cpp) */
struct hdScreenRupySwordCounter_c : NoControlPart_l {
    /* 0x50 */ gptr<void> mpNumText;  /* "T_Num_00" */
    /* 0x54 */ be<u16> mShownCount;
    /* 0x56 */ u8 _56[2];

    static void destroy(hdScreenRupySwordCounter_c* i_this, s32 flags);
    void waitEnter();
    void waitExecute();
    void inEnter();
    void inExecute();
    void outEnter();
    void outExecute();
    static hdScreenRupySwordCounter_c* construct(hdScreenRupySwordCounter_c* i_this);
    u32 init(u32 arg1, u32 arg2, void* desc);
    void hide();
    u32 createChild(hdPartsDesc_c* desc);
    u32 load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
    void updateDisplay();
    void roll();

    hdParts_c* firstChild() { return *mChildren.slot(0); }
};
WWHD_OFFSET(hdScreenRupySwordCounter_c, mShownCount, 0x54);
WWHD_SIZE(hdScreenRupySwordCounter_c, 0x58);
struct MinigameGameCounter { u8 padding[0x5CEC]; be<u16> mTargetCount; };
WWHD_OFFSET(MinigameScreenBase, mStateMachine, 0x18);
WWHD_SIZE(MinigameScreenBase, 0x54);
WWHD_SIZE(MinigameText, 12);
WWHD_OFFSET(hdScreenBoatRaceResult_c, mRollFrame, 0x60);
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
/* the parts classes' state machine (hdPartsStateHolder_c::mMachine) */
inline void minigameChangeState(hdStateMachine* machine, MinigameState* state) {
    minigameChangeState(reinterpret_cast<MinigameStateMachine*>(machine), state);
}
void minigamePlaySound(u32);
