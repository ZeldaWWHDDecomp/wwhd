/* HD event-message companions. Game-only layouts extend the canonical parts base;
 * merged event-window declarations remain in hd_screen_msg_window_event.h. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_screen_msg_window_event.h"

struct hdMsgEventFrame_c : hdParts_c {
    static hdMsgEventFrame_c* construct(hdMsgEventFrame_c* self);
    static void destroy(hdMsgEventFrame_c* self, s32 flags);
    void playAnimation(u32 animation);
    void playDefaultAnimation();
    void setAnimationFrame(u32 animation, u32 frame);
    u32 loadFrame(u32 resource, u32 kind, void* description);
    void destroyFrameLayout();
    void executeFrame();
    void enterLoop();
    u32 createFrameLayout(void* name, u32 kind, u32 option, u32 flags, void* description);
};
WWHD_SIZE(hdMsgEventFrame_c, 0x50);


struct hdMsgEventFrameUiPointer_l { gptr<hdScreenUiSystem> mpSystem; };
WWHD_SIZE(hdMsgEventFrameUiPointer_l, 4);
struct hdMsgEventFrameHeader_l { be<u32> mWords[4]; };
WWHD_SIZE(hdMsgEventFrameHeader_l, 0x10);
struct hdMsgEventFrameRegistration_l { u8 mOpaque[0xC]; };
WWHD_SIZE(hdMsgEventFrameRegistration_l, 0xC);
void hdMsgEventFrame_register(hdMsgEventFrameRegistration_l* registration);
void hdMsgEventFrame_constructAngle(be<u8>* object);
void hdMsgEventFrame_constructPhase(be<u8>* object);

struct hdMsgEventFrameAngle_l { be<f32> mValue; };
WWHD_SIZE(hdMsgEventFrameAngle_l, 4);
struct hdMsgEventFrameAngleLimits_l { hdMsgEventFrameAngle_l mMinimum, mMaximum; };
WWHD_SIZE(hdMsgEventFrameAngleLimits_l, 8);
