/* Process creation entry points. WWHD uses the shared base layout. */
#pragma once
#include "bindings.h"
#include "f_pc/f_pc_base.h"
struct create_request;
BOOL fpcCtRq_Cancel(create_request*);
BOOL fpcCtRq_Handler();
struct CreatorHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 mPadding[2];
    be<u32> zero[4];
};
struct CreatorPiPair { be<f32> negativePi; be<f32> positivePi; };
WWHD_OFFSET(CreatorHeaderStatics, zero, 0xC);
WWHD_SIZE(CreatorHeaderStatics, 0x1C);
WWHD_SIZE(CreatorPiPair, 8);
void CreatorFirstHeaderObject_ct(u8*);
void CreatorSecondHeaderObject_ct(u8*);
