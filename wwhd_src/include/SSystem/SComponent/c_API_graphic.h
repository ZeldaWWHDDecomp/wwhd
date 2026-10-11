/* Graphics API callbacks, WWHD layout (unchanged from GameCube). */
#pragma once
#include "bindings.h"

typedef void (*cAPIGph_Mthd)(void);
struct cAPIGph__Iface {
    /* 0x00 */ gptr<void> mpGInfo;
    /* 0x04 */ gfn mpCreate;
    /* 0x08 */ gfn mpBeforeOfDraw;
    /* 0x0C */ gfn mpAfterOfDraw;
    /* 0x10 */ gfn mpPainter;
    /* 0x14 */ gfn mpBlankingOn;
    /* 0x18 */ gfn mpBlankingOff;
};
WWHD_OFFSET(cAPIGph__Iface, mpBeforeOfDraw, 0x08);
WWHD_OFFSET(cAPIGph__Iface, mpAfterOfDraw, 0x0C);
WWHD_SIZE(cAPIGph__Iface, 0x1C);

void cAPIGph_BeforeOfDraw(void);
void cAPIGph_AfterOfDraw(void);
