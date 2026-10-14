/* HD MagicBar_00 HUD animation and special-player placement. */
#pragma once
#include "d/hd_parts_heart.h"
#include "d/hd_parts_lyt.h"
#include "d/hd_parts_progress_bar.h"
struct hdHudMagicBar_c : ProgressBarPart_l {
    gptr<hdPartsPaneSRT_c> mPanes[3]; // 58
    be<f32> mOrigin[3]; // 64
    be<u32> mOriginalTranslation[3]; // 70
    be<f32> mPlacementFrame; // 7C
    be<f32> mPlayerFrame; // 80
    be<f32> mPaneSpacing; // 84
    be<u8> mWarning; // 88
    be<u8> mSpecialPlacement; // 89
    u8 mPadding[2];
    static void destroy(hdHudMagicBar_c*,u32);
    static hdHudMagicBar_c* construct(hdHudMagicBar_c*);
    void setFrame(u32,f32); void play(u32);
    void blankEnter(); void blankMove(); void inEnter(); void updatePlacement();
    void inMove(); void displayMove(); void outEnter(); void outMove();
    BOOL init(void*,u32,hdPartsDesc_c*);
    void execute(); void reset();
    BOOL loadLayout(void*,u32,u32,u32,void*);
};
WWHD_SIZE(hdHudMagicBar_c,0x8C);
WWHD_OFFSET(hdHudMagicBar_c,mPanes,0x58);
struct hdMagicStateNumber_c { be<u32> mNumber; u32 getNumber(); };
