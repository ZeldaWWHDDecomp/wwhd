/* Process drawing. WWHD retains full guest callback registers. */
#pragma once
#include "f_pc/f_pc_node.h"
using fpcDw_HandlerFunc = u32;
using fpcDw_HandlerFuncFunc = u32;
s32 fpcPause_IsEnable(base_process_class*, u32);
void cAPIGph_BeforeOfDraw();
void cAPIGph_AfterOfDraw();
struct DrawHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 mPadding[2];
    be<u32> zero[4];
};
struct DrawPiPair { be<f32> negativePi; be<f32> positivePi; };
WWHD_OFFSET(DrawHeaderStatics, zero, 0xC);
WWHD_SIZE(DrawHeaderStatics, 0x1C);
WWHD_SIZE(DrawPiPair, 8);
void DrawFirstHeaderObject_ct(u8*);
void DrawSecondHeaderObject_ct(u8*);
