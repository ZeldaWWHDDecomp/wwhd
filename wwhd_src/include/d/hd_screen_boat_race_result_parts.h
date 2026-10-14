/* HD result-screen companion view; preserves the merged timing class unchanged. */
#pragma once
#include "d/hd_screen_boat_race_start.h"
#include "d/hd_parts_text_pane.h"
#include "d/hd_minigame_hud.h"
class hdBoatRaceResultView : public hdScreenBoatRaceResult_c {
public:
    /* 0x64 */ be<u8> mVisible;
    u8 mPadding[3];
    hdParts_c* parts() { return reinterpret_cast<hdParts_c*>(this); }
    hdTextPane_c* timePane() { return reinterpret_cast<hdTextPane_c*>(mTextPane.get()); }
    void setTimePane(hdTextPane_c* pane) { mTextPane = pane; }
    gptr<hdTextPane_c>* rupeePaneSlot() { return hdPartsView<gptr<hdTextPane_c>>(gabi::ea(this) + sizeof(MinigameScreenBase)); }
    hdTextPane_c* rupeePane() { return *rupeePaneSlot(); }
    void setRupeePane(hdTextPane_c* pane) { *rupeePaneSlot() = pane; }
    hdStateMachine* stateMachine() { return parts()->stateMachine(); }
    hdAnimSet* animSet() { return parts()->animSet(); }
    void changeState(hdPartsStateId_c* state) { parts()->changeState(state); }
    void startState(hdPartsStateId_c* state) { parts()->startState(state); }
    void vCalcLayout() { parts()->vCalcLayout(); }
    void vDestroyLayout() { parts()->vDestroyLayout(); }
    u32 vLoad(void* name, u32 archive, u32 first, u32 second, void* descriptor) { return parts()->vLoad(name, archive, first, second, descriptor); }
    static hdBoatRaceResultView* construct(hdBoatRaceResultView*);
    static void destroy(hdBoatRaceResultView*, u32 flags);
    void enterWait();
    void updateText();
    void enterInAll();
    u32 setup(u32 first, u32 second, void* descriptor);
    void release();
    void execute();
    void updateLayout();
    void show(u32 result);
    u32 load(void* name, u32 archive, u32 first, u32 second, void* descriptor);
};
WWHD_SIZE(hdScreenBoatRaceResult_c, 0x64);
WWHD_OFFSET(hdBoatRaceResultView, mTextPane, 0x50);
WWHD_OFFSET(hdBoatRaceResultView, mResult, 0x58);
WWHD_OFFSET(hdBoatRaceResultView, mVisible, 0x64);
WWHD_SIZE(hdBoatRaceResultView, 0x68);
