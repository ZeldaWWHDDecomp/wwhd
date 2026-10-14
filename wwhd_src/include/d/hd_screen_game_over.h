/* HD GameOver aggregate screen; private game-derived reconstruction. */
#pragma once
#include "d/hd_screen_game_over_char.h"
struct hdGameOverNames { hdUiSafeString mNames[2]; };
struct hdGameOverRuntimeWords { be<u32> mWords[4]; };
struct hdGameOverRegistration { be<u32> mWords[3]; };
struct hdGameOverContext { be<u8> mByte; };
WWHD_SIZE(hdGameOverNames, 0x10);
WWHD_SIZE(hdGameOverRuntimeWords, 0x10);
WWHD_SIZE(hdGameOverRegistration, 0xC);
WWHD_SIZE(hdGameOverContext, 1);
struct hdGameOverCharacters {
    be<s32> mCount;
    gptr<gptr<hdGameOverChar_c>> mpItems;
    gptr<hdGameOverChar_c>* slot(u32 index) {
        return gabi::at<gptr<hdGameOverChar_c>>(gabi::ea(mpItems.get()) + index * sizeof(gptr<hdGameOverChar_c>));
    }
};
WWHD_SIZE(hdGameOverCharacters, 8);
WWHD_OFFSET(hdGameOverCharacters, mCount, 0);
WWHD_OFFSET(hdGameOverCharacters, mpItems, 4);
struct hdGameOver_c : hdParts_c {
    gptr<hdPartsLayout_c> mpGameLayout;
    hdGameOverCharacters mCharacters;
    be<u32> mCharacterCount;
    be<u8> mRequested, mFinished;
    u8 mPadding62[2];
    static hdGameOver_c* construct(hdGameOver_c*);
    static void destroy(hdGameOver_c*, u32);
    void waitEnter();
    void waitMove();
    void outEnter();
    void outMove();
    void completeEnter();
    u32 createCharacters();
    u32 initializeGameOver(u32 resource, u32 option);
    void releaseGameOver();
    void executeCharacters();
    void executeGameOver();
    void drawGameOver(void* info);
    void createCharacter(void* context);
};
WWHD_SIZE(hdGameOver_c, 0x64);
WWHD_OFFSET(hdGameOver_c, mpGameLayout, 0x50);
WWHD_OFFSET(hdGameOver_c, mCharacters, 0x54);
WWHD_OFFSET(hdGameOver_c, mCharacterCount, 0x5C);
WWHD_OFFSET(hdGameOver_c, mRequested, 0x60);
WWHD_OFFSET(hdGameOver_c, mFinished, 0x61);
