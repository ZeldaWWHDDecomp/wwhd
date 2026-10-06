#include "gabi.h"
using namespace gabi;

s32 fpcCt_Abort_hd(void* process) {
    WWHD_FUNC(0x025DDCE4, s32, process);
    return call<s32>(0x025DD934, at<void>(load<u32>(ea(process)+0x14)));
}
VERIFY(0x025DDCE4, fpcCt_Abort_hd);

s32 fpcCt_Handler_hd() {
    WWHD_FUNC(0x025DDCEC, s32);
    return call<s32>(0x025DDAC4);
}
VERIFY(0x025DDCEC, fpcCt_Handler_hd);

void f_pc_creator_sinit_hd() {
    WWHD_FUNC(0x025DDCF0, void);
    store<u32>(0x1048A724, 0);
    store<u32>(0x1048A71C, 0);
    store<u32>(0x1048A728, 0);
    store<u32>(0x1048A720, 0);
    call<void>(0x028F026C, at<void>(0x101F39B0));
    f32 a = load<f32>(0x10057EAC);
    f32 b = load<f32>(0x10057EB0);
    store<f32>(0x1048A710, a);
    store<f32>(0x1048A714, b);
    call<void>(0x028ED6F8, at<void>(0x1048A718));
    call<void>(0x028F026C, at<void>(0x101F39BC));
    call<void>(0x028EAB2C, at<void>(0x1048A719));
    call<void>(0x028F026C, at<void>(0x101F39C8));
}
VERIFY(0x025DDCF0, f_pc_creator_sinit_hd);
