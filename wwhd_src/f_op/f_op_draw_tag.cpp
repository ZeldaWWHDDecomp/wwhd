#include "gabi.h"
using namespace gabi;

u32 fopDwTg_ToDrawQ_hd(void* tag, s32 priority) {
    WWHD_FUNC(0x025DA874, u32, tag, priority);
    return call<u32>(0x0201A770, at<void>(0x101F33DC), priority, tag);
}
VERIFY(0x025DA874, fopDwTg_ToDrawQ_hd);

void fopDwTg_DrawQTo_hd(void* tag) {
    WWHD_FUNC(0x025DA884, void, tag);
    call<void>(0x0201A724, tag);
}
VERIFY(0x025DA884, fopDwTg_DrawQTo_hd);

bool fopDwTg_Init_hd(void* tag, void* process) {
    WWHD_FUNC(0x025DA888, bool, tag, process);
    call<void>(0x0201A918, tag, process);
    return true;
}
VERIFY(0x025DA888, fopDwTg_Init_hd);

void fopDwTg_CreateQueue_hd() {
    WWHD_FUNC(0x025DA8AC, void);
    call<void>(0x0201AB04, at<void>(0x101F33DC), at<void>(0x104875AC), 1000);
}
VERIFY(0x025DA8AC, fopDwTg_CreateQueue_hd);

void f_op_draw_tag_sinit_hd() {
    WWHD_FUNC(0x025DA8C4, void);
    store<u32>(0x104875A4, 0);
    store<u32>(0x1048759C, 0);
    store<u32>(0x104875A8, 0);
    store<u32>(0x104875A0, 0);
    call<void>(0x028F026C, at<void>(0x101F33B8));
    f32 a = load<f32>(0x10057994);
    f32 b = load<f32>(0x10057998);
    store<f32>(0x10487590, a);
    store<f32>(0x10487594, b);
    call<void>(0x028ED6F8, at<void>(0x10487598));
    call<void>(0x028F026C, at<void>(0x101F33C4));
    call<void>(0x028EAB2C, at<void>(0x10487599));
    call<void>(0x028F026C, at<void>(0x101F33D0));
}
VERIFY(0x025DA8C4, f_op_draw_tag_sinit_hd);
