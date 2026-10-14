/* HD: the "Character" parts in the staff roll bubbles (layout "Bubble_00"): a character picture chosen by the
 * frame of its "Character" animation, with a "Loop" animation started at a random frame
 * (hd_screen_character.cpp). No GameCube counterpart. Layout, pane, animation and state names are HD
 * strings; class, member and method names are ours.
 *
 * The class derives from the "NoControl" parts class of FW-parts_msg_base (NoControlPart_l, 0x50). */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_no_control.h"

/* 02004E04: bind animation `entry` to its pane group (hd_ui_02004A3C) */
void hdCharacter_bindGroup(hdAnimSet* set, u32 entry, hdUiSafeString* name, hdUiSafeString* group);
/* 0200579C: hd_ui_02004A3C's setAnimationSpeed; the value set here is a frame, so the controller field it
 * writes (+0xC) is probably the current frame */
void hdCharacter_setAnimFrame(hdAnimSet* set, u32 entry, u32 slot, f32 frame);
/* 028713B8: an animation's frame count (nw::lyt animator) */
u32 hdCharacter_getFrameCount(hdAnimSetController* controller);
/* 0275CEE4: the next value of a random number generator */
u32 hdCharacter_random(void* random);

/* animation entries of "Bubble_00" */
enum hdCharacterAnim_e {
    CHARACTER_ANIM_CHARACTER = 0, /* its frame selects the character */
    CHARACTER_ANIM_LOOP = 1,
};

struct hdScreenCharacter_c : NoControlPart_l {
    static void destroy(hdScreenCharacter_c* i_this, s32 flags);
    void loopEnter();
    void loopExecute();
    static hdScreenCharacter_c* construct(hdScreenCharacter_c* i_this);
    u32 init(u32 arg1, u32 arg2, void* desc);
    u32 load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
    void setCharacter(s32 frame);
};
WWHD_SIZE(hdScreenCharacter_c, 0x50);
