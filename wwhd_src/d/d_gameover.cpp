/** Game-over engine. */
#include "d/d_gameover.h"
static GameOverGraphics* l_graphics;
static GameOverSaveData* l_save;
static GameOverStateID l_expectedState;
static s16 l_gameoverDelay;
static const f32 l_alphaScale;
static GameOverHeaderStatics l_headerStatics;
static const GameOverConstants l_constants;

/* 02549990 */
static void dDlst_Gameover_CAPTURE_draw() {
    // HD: this draw entry is a no-op and does not read an object argument.
}
/* 02549994 */
static void dDlst_GameOverScrnDraw_draw() {
    // HD: drawing is handled by the graphics object; this entry is a no-op.
}
/* 02549998 */
static u32 d_GameOver_IsDelete() {
    return TRUE;
}
/* 025499A0 */
u32 dGameover_c::_execute() {
    u8 state = mState;
    if (state == GAMEOVER_OPEN) {
        GameOverGraphics* graphics = l_graphics;
        graphics->mOpenGameOver = TRUE;
        mState = GAMEOVER_SELECT;
        l_save->mLife = 12;
        return TRUE;
    }
    if (state == GAMEOVER_SELECT) {
        GameOverWindow* window = l_graphics->mpWindow;
        GameOverStateID* activeState = GameOverStateMgr_getCurrent(&window->mStateMgr);
        u32 currentCallback = activeState->vtable->getID.get();
        GameOverStateVtable* expectedVtable = l_expectedState.vtable;
        // HD: preserve the vtable capture and two indirect calls in their original order.
        u32 current = dGameover_stateID(currentCallback, activeState);
        u32 expected = dGameover_stateID(expectedVtable->getID.get(), &l_expectedState);
        if (current == expected || window->mFinished != 0) {
            u8 choice = window->mChoice;
            dGameover_getGameInfo()->mGameoverStatus = choice == 0 ? 3 : 2;
            dMenu_flagSet(0);
            mState = GAMEOVER_DONE;
        }
        return TRUE;
    }
    if (state == GAMEOVER_CLOSE || state == GAMEOVER_DONE || mAnimeStart == 0) {
        return TRUE;
    }
    s16 timer = field_0x114;
    GameOverGraphics* graphics = l_graphics;
    if (timer == 90) {
        GameOverGraphics_start(graphics);
        field_0x114 = (s16)u16(field_0x114 - 1);
    } else if (GameOverGraphics_isReady(graphics) != 0) {
        mState = GAMEOVER_OPEN;
    }
    return TRUE;
}
/* 02549B14 */
static u32 d_GameOver_Execute(dGameover_c* gameover) {
    return gameover->_execute();
}
/* 02549B18 */
static u32 d_GameOver_Draw() {
    return TRUE;
}
/* 02549B20 */
static u32 d_GameOver_Delete() {
    return TRUE;
}
/* 02549B28 */
u32 dGameover_c::_create() {
    // HD: no GameCube archive/heap construction; retain each game-info accessor call.
    if (dMenu_flag() != 0) {
        return cPhs_INIT_e;
    }
    if (dGameover_getGameInfo()->mHeapLockFlag != 0 &&
        dGameover_getGameInfo()->mHeapLockFlag != 4) {
        return cPhs_INIT_e;
    }
    if (dGameover_getGameInfo()->mMesgStatus != 0) {
        return cPhs_INIT_e;
    }
    GameOverSaveData* save = l_save;
    u16 deaths = save->mDeathCount;
    if (deaths < 9999) {
        save->mDeathCount = deaths + 1;
    }
    field_0x114 = 90;
    s16 delay = l_gameoverDelay;
    mState = GAMEOVER_WAIT;
    field_0x116 = delay;
    mAnimeStart = FALSE;
    return cPhs_COMPLEATE_e;
}
/* 02549BE8 */
static u32 d_GameOver_Create(dGameover_c* gameover) {
    return gameover->_create();
}
/* 02549BEC */
u32 dGameover_c::deleteCheck() {
    return mState == GAMEOVER_DONE;
}
/* 02549C00 */
static void d_GameOver_setBackAlpha(f64 alpha) {
    // HD: free-function entry with f64 input; call the black-pane method before conversion.
    GameOverBlackPane_show(l_graphics->mpBlackPane);
    u8 value = (u8)gabi::ftoi((f32)(alpha * (f64)l_alphaScale));
    dGameover_getGameInfo()->mBackAlpha = value;
}
/* 02549C74 */
static void __sinit_d_gameover_cpp() {
    // HD: preserve header construction and the two additional constant-pair stores.
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[1] = 0;
    l_headerStatics.zero[0] = 0;
    __register_global_object(0x101E1CF8);
    f32 a = l_constants.negativePi;
    f32 b = l_constants.positivePi;
    l_headerStatics.negativePi = a;
    l_headerStatics.positivePi = b;
    GameOverFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101E1D04);
    GameOverSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101E1D10);
    a = l_constants.first;
    b = l_constants.second;
    l_headerStatics.firstPair[0] = a;
    l_headerStatics.secondPair[0] = b;
    l_headerStatics.firstPair[1] = a;
    l_headerStatics.secondPair[1] = b;
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data l_graphics 101F8344
data l_save 101F84DC
data l_expectedState 1049DD60
data l_gameoverDelay 1047AF1E
data l_alphaScale 1004E1D8
data l_headerStatics 1047595C
data l_constants 1004E1E0
ext 02006478 GameOverStateID* GameOverStateMgr_getCurrent(GameOverStateMgr*)
ext 027170D4 void GameOverGraphics_start(GameOverGraphics*)
ext 027170F8 s32 GameOverGraphics_isReady(GameOverGraphics*)
ext 0266A064 void GameOverBlackPane_show(void*)
ext 025986BC s32 dMenu_flag()
ext 0259170C void dMenu_flagSet(u32)
ext 028ED6F8 void GameOverFirstHeaderObject_ct(u8*)
ext 028EAB2C void GameOverSecondHeaderObject_ct(u8*)
*/
