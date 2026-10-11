/* Game-over object and HD interfaces used here. */
#pragma once
#include "bindings.h"
struct dGameover_c {
    u8 _00[0x10C];
    /* HD: fields move eight bytes earlier than the GameCube object. */
    be<s16> field_0x114;
    be<s16> field_0x116;
    be<u8> mState;
    be<u8> mAnimeStart;
    u32 _execute();
    u32 _create();
    u32 deleteCheck();
};
WWHD_OFFSET(dGameover_c, mState, 0x110);
WWHD_OFFSET(dGameover_c, mAnimeStart, 0x111);
enum GameOverState : u8 {
    GAMEOVER_WAIT = 0,
    GAMEOVER_OPEN = 3,
    GAMEOVER_SELECT = 4,
    GAMEOVER_CLOSE = 5,
    GAMEOVER_DONE = 6,
};
struct GameOverStateVtable { u8 _00[0x14]; gfn getID; };
struct GameOverStateID { u8 _00[8]; gptr<GameOverStateVtable> vtable; };
struct GameOverStateMgr { u8 storage[1]; };
struct GameOverWindow {
    u8 _00[0x18];
    GameOverStateMgr mStateMgr;
    u8 _19[0xD1 - 0x19];
    be<u8> mFinished;
    u8 _D2[3];
    be<u8> mChoice;
};
struct GameOverGraphics {
    u8 _00[0x1BC];
    gptr<GameOverWindow> mpWindow;
    u8 _1C0[0x1E8 - 0x1C0];
    gptr<void> mpBlackPane;
    u8 _1EC[0x248 - 0x1EC];
    be<u8> mOpenGameOver;
};
struct GameOverGameInfo {
    u8 _00[0x5BB2];
    be<u8> mMesgStatus;
    u8 _5BB3[0x5BE3 - 0x5BB3];
    be<u8> mGameoverStatus;
    u8 _5BE4[8];
    be<u8> mHeapLockFlag;
    u8 _5BED[0x62F1 - 0x5BED];
    be<u8> mBackAlpha;
};
struct GameOverSaveData {
    u8 _00[0x22];
    be<u16> mLife;
    u8 _24[0x17A - 0x24];
    be<u16> mDeathCount;
};
inline GameOverGameInfo* dGameover_getGameInfo() {
    return reinterpret_cast<GameOverGameInfo*>(dComIfGp_get());
}
inline u32 dGameover_stateID(u32 callback, void* state) { return gabi::call_ptr<u32>(callback, state); }
GameOverStateID* GameOverStateMgr_getCurrent(GameOverStateMgr*);
void GameOverGraphics_start(GameOverGraphics*);
s32 GameOverGraphics_isReady(GameOverGraphics*);
void GameOverBlackPane_show(void*);
s32 dMenu_flag();
void dMenu_flagSet(u32);
struct GameOverHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    be<f32> firstPair[2];
    be<f32> secondPair[2];
    u8 firstObject;
    u8 secondObject;
    u8 padding[2];
    be<u32> zero[4];
};
struct GameOverConstants { be<f32> negativePi; be<f32> positivePi; be<f32> first; be<f32> second; };
void GameOverFirstHeaderObject_ct(u8*);
void GameOverSecondHeaderObject_ct(u8*);
