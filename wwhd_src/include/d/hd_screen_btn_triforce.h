/** HD collection Triforce fragment and completion button. */
#pragma once
#include "d/hd_screen_btn_pearl.h"
#include "d/hd_parts_button.h"
struct hdTriforceButton_c : NoDecideButton_l {
    hdScreenPaneArray mCompletePanes;
    hdScreenPaneArray mFragmentPanes;
    be<u32> mFragmentFlags;
    static void destroy(hdTriforceButton_c*,s32);
    static hdTriforceButton_c* construct(hdTriforceButton_c*);
    void markComplete();
    void initializeCompleteAnimation();
    void calculateLayout();
    void completeDemoEnter();
    void completeDemoExecute();
    u32 load(void*,u32,u32);
    void unload();
    void updateFragment(u32);
    void updateFragments();
    void updateComplete();
    u32 hasFragments() const;
    u32 isComplete() const;
    u32 itemId() const;
    u32 eventId() const;
    void requestCompleteDemo();
    u32 loadLayout(void*,void*,u32,u32,void*);
    void updateEnabled();
};
WWHD_OFFSET(hdTriforceButton_c,mCompletePanes,0x78);
WWHD_OFFSET(hdTriforceButton_c,mFragmentPanes,0x80);
WWHD_OFFSET(hdTriforceButton_c,mFragmentFlags,0x88);
WWHD_SIZE(hdTriforceButton_c,0x8C);
