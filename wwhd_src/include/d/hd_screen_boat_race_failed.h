/* HD failure-screen owner and its character child. Binary-derived layout. */
#pragma once
#include "d/hd_screen_boat_race_start.h"
#include "d/hd_minigame_hud.h"
struct hdBoatRaceFailedCharView;
struct hdBoatRaceChildArray {
    be<s32> mCount;
    gptr<gptr<hdBoatRaceFailedCharView>> mData;
    gptr<hdBoatRaceFailedCharView>* slot(u32 index) {
        return hdPartsView<gptr<hdBoatRaceFailedCharView>>(gabi::ea(mData.get()) + (index < u32(mCount) ? index * 4 : 0));
    }
};
struct hdBoatRaceFailedCharView : hdScreenBoatRaceFailedChar_c {
    /* 0x5C */ be<u32> mDisplayWait;
    /* 0x60 */ be<u32> mInFrames;
    /* 0x64 */ be<u32> mUnusedFrames;
    /* 0x68 */ be<u32> mExitWait;
    /* 0x6C */ be<u32> mOutFrames;
    /* 0x70 */ u8 mPadding;
    /* 0x71 */ be<u8> mAdvance;
    /* 0x72 */ be<u8> mFinished;
    u8 mEndPadding;
    hdParts_c* parts() { return reinterpret_cast<hdParts_c*>(this); }
    hdPartsLayout_c* ownLayout() { return reinterpret_cast<hdPartsLayout_c*>(mTextPane.get()); }
    void setOwnLayout(hdPartsLayout_c* layout) { mTextPane = layout; }
    hdStateMachine* stateMachine() { return parts()->stateMachine(); }
    void changeState(hdPartsStateId_c* state) { parts()->changeState(state); }
    void startState(hdPartsStateId_c* state) { parts()->startState(state); }
    void vFinalize() { parts()->vFinalize(); }
    void vDelete(u32 flags) { parts()->vDelete(flags); }
    void vExecuteState() { parts()->vExecuteState(); }
    static hdBoatRaceFailedCharView* construct(hdBoatRaceFailedCharView*);
    static void destroy(hdBoatRaceFailedCharView*, u32 flags);
    void enterWait();
    void enterExitWait();
    void executeExitWait();
    void enterOut();
    void executeOut();
    void enterFinished();
    void enterIdle();
    void executeIdle();
    void configureIn();
    void configureOut();
    u32 setup(void* archive, void* first, void* second, hdPartsDesc_c* descriptor);
    void release();
    void execute();
    void reset();
};
class hdScreenBoatRaceFailed_c : public hdParts_c {
public:
    /* 0x50 */ gptr<hdPartsLayout_c> mpOwnLayout;
    /* 0x54 */ hdBoatRaceChildArray mCharacters;
    /* 0x5C */ be<s32> mCharacterCount;
    /* 0x60 */ be<u8> mAdvance;
    /* 0x61 */ be<u8> mFinished;
    /* 0x62 */ be<u8> mVisible;
    u8 mPadding;
    static hdScreenBoatRaceFailed_c* construct(hdScreenBoatRaceFailed_c*);
    static void destroy(hdScreenBoatRaceFailed_c*, u32 flags);
    void enterWait();
    void executeWait();
    void enterDisplay();
    void executeDisplay();
    void leaveDisplay();
    u32 createChildren();
    u32 setup(u32 first, u32 second);
    void release();
    void executeChildren();
    void execute();
    void draw(void* info);
    void reset();
    u32 createChild(hdPartsDesc_c* descriptor);
};
WWHD_SIZE(hdScreenBoatRaceFailedChar_c, 0x5C);
WWHD_OFFSET(hdBoatRaceFailedCharView, mScene, 0x54);
WWHD_OFFSET(hdBoatRaceFailedCharView, mWaitTimer, 0x58);
WWHD_OFFSET(hdBoatRaceFailedCharView, mAdvance, 0x71);
WWHD_OFFSET(hdBoatRaceFailedCharView, mFinished, 0x72);
WWHD_SIZE(hdBoatRaceFailedCharView, 0x74);
WWHD_SIZE(hdBoatRaceChildArray, 8);
WWHD_OFFSET(hdScreenBoatRaceFailed_c, mpOwnLayout, 0x50);
WWHD_OFFSET(hdScreenBoatRaceFailed_c, mCharacters, 0x54);
WWHD_OFFSET(hdScreenBoatRaceFailed_c, mAdvance, 0x60);
WWHD_SIZE(hdScreenBoatRaceFailed_c, 0x64);
