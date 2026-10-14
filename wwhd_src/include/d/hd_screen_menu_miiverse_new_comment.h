/** HD new-comment menu, including its sequence/control helpers. Private binary-derived source. */
#pragma once
#include "d/hd_screen_menu_miiverse_comment.h"
#include "d/hd_parts_basic_button.h"
#include "d/hd_screen_menu_miiverse.h"
// The two input-event temporaries are adjacent at SP+8 and SP+0x30 in both sequences.
struct hdMiiverseSequenceInputEvents_l {
    hdButtonEvent_l mCurrent, mPosted;
};
WWHD_OFFSET(hdMiiverseSequenceInputEvents_l, mPosted, 0x28);
WWHD_SIZE(hdMiiverseSequenceInputEvents_l, 0x50);
// The menu's factory request embeds a name string within the canonical parts descriptor resource.
struct hdMiiversePartRequest_l {
    u8 mResourcePrefix[0x14];
    hdUiSafeString mPartName;
    u8 mResourceSuffix[8];
    gptr<const char> mClassName;
};
WWHD_OFFSET(hdMiiversePartRequest_l, mPartName, 0x14);
WWHD_OFFSET(hdMiiversePartRequest_l, mClassName, 0x24);
WWHD_SIZE(hdMiiversePartRequest_l, 0x28);
struct hdMenuMiiverseNewComment_c : hdParts_c {
    be<u8> mEnabled;
    be<u8> mPressed;
    be<u8> mVisible;
    u8 mPresentationPadding;
    be<f32> mPositionY;
    gptr<hdPartsPaneSRT_c> mpSequencePane;
    static hdMenuMiiverseNewComment_c* construct(hdMenuMiiverseNewComment_c*);
    static void destroy(hdMenuMiiverseNewComment_c*, u32);
    void finalizeParts();
    void findSequencePane();
    u32 initializeParts(u32, u32, void*);
    u32 loadLayout(void*, u32, u32, u32, void*);
    void translateLayoutY(f32);
    void setSequencePresentation(u32, f32);
    void enterWait();
    void clearPressed();
    u32 processInput();
    void executeWait();
    void enterInOut();
    void executeInOut();
    void enterAppear();
    void executeAppear();
    void updateState();
    void clearContentNavigation();
    u32 createSequencePart(hdMiiversePartRequest_l*);
    void beginInOut();
    void beginAppear();
    void showInOut();
    void finishInOut();
    void finishAppear();
    f32 sequenceHeight() const;
    f32 sequenceGlobalY() const;
    void connectUpperControl(hdButtonPart_c*);
    void connectLowerControl(hdButtonPart_c*);
    void connectContentAbove(hdButtonPart_c*);
    void connectContentBelow(hdButtonPart_c*);
};
WWHD_OFFSET(hdMenuMiiverseNewComment_c, mEnabled, 0x50);
WWHD_OFFSET(hdMenuMiiverseNewComment_c, mPressed, 0x51);
WWHD_OFFSET(hdMenuMiiverseNewComment_c, mVisible, 0x52);
WWHD_OFFSET(hdMenuMiiverseNewComment_c, mPositionY, 0x54);
WWHD_OFFSET(hdMenuMiiverseNewComment_c, mpSequencePane, 0x58);
WWHD_SIZE(hdMenuMiiverseNewComment_c, 0x5C);
