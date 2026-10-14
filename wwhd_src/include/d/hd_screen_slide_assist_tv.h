/** HD television slide-assistance panel. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_screen_touch_slide_decide.h"
#include "d/hd_screen_item_screen_tu3.h"
#include "d/hd_screen_menu_main.h"
struct hdSlideAssistTV_c : hdParts_c {
    be<u32> mDelay;
    void configureChildren();
    u32 createChild(hdPartsDesc_c* descriptor);
    u32 loadScreen(void* name, u32 archive, u32 resource, u32 flags, void* descriptor);
    void waitExecute();
    void displayExecute();
    void delayEnter();
    void delayExecute();
    void inAllEnter();
    void inAllExecute();
    void outAllEnter();
    void outAllExecute();
    u32 setup(u32 archive, u32 resource, u32 flags);
    void release();
    void stateExecute();
    void draw(void* context);
};
WWHD_SIZE(hdSlideAssistTV_c, 0x54);
WWHD_OFFSET(hdSlideAssistTV_c, mDelay, 0x50);
struct hdSlideMenuContext_l { u8 _00[0x17C]; gptr<hdParts_c> mpItemScreen; };
WWHD_OFFSET(hdSlideMenuContext_l, mpItemScreen, 0x17C);
void slideDestroyBase(hdSlideAssistTV_c* panel, u32 flags);
hdSlideAssistTV_c* slideConstructBase(hdSlideAssistTV_c* panel);
void slideAttachArchives(hdSlideAssistTV_c* panel, hdUiSafeString* names, u32 count);
void slideDetachArchives(hdSlideAssistTV_c* panel);
void slideExecuteState(hdStateMachine* machine);
void slideConfigureChildren(hdSlideAssistTV_c* panel);
void slideDrawBase(hdSlideAssistTV_c* panel, void* context);

struct hdSlideAnimationBinding_l { be<u32> mAnimation; };
void slideGetAnimation(hdParts_c* part, hdSlideAnimationBinding_l* binding);
void slideInstallAnimation(hdParts_c* part, hdSlideAnimationBinding_l* binding);
void* slideAllocate(u32 size, void* heap, u32 alignment);
hdParts_c* slideConstructIcon(hdParts_c* part);
u32 slideSetupIcon(hdParts_c* part, u32 archive, u32 resource, hdPartsDesc_c* descriptor);
u32 slideCreateBase(hdSlideAssistTV_c* panel, u32 index, hdPartsDesc_c* descriptor);
void slideBindAnimation(hdAnimSet* animation, u32 track, hdUiSafeString* name, hdUiSafeString* pane);
