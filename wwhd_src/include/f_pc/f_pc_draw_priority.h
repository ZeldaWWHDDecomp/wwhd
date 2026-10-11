/* Draw priority. HD tests the full setter register and stores its low halfword. */
#pragma once
#include "bindings.h"
struct draw_priority_class { be<s16> mPriority; };
WWHD_OFFSET(draw_priority_class, mPriority, 0);
WWHD_SIZE(draw_priority_class, 2);
struct DrawPriorityHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 mPadding[2];
    be<u32> zero[4];
};
struct DrawPriorityPiPair { be<f32> negativePi; be<f32> positivePi; };
WWHD_OFFSET(DrawPriorityHeaderStatics, zero, 0xC);
WWHD_SIZE(DrawPriorityHeaderStatics, 0x1C);
WWHD_SIZE(DrawPriorityPiPair, 8);
void DrawPriorityFirstHeaderObject_ct(u8*);
void DrawPrioritySecondHeaderObject_ct(u8*);
