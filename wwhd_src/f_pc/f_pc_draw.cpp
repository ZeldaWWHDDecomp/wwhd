#include "gabi.h"
using namespace gabi;

s32 fpcDw_Execute_hd(void* process) {
    WWHD_FUNC(0x025DE2CC, s32, process);
    if (call<s32>(0x025E0B20, process, 2) != 0) return 0;
    void* saved_layer = call<void*>(0x025DED64);
    u32 subtype = load<u32>(ea(process)+0xB8);
    u32 leaf_type = load<u32>(0x101F3BD8);
    call<s32>(0x025DD258, leaf_type, subtype);
    u32 methods = load<u32>(ea(process)+0xA8);
    void* layer = at<void>(load<u32>(ea(process)+0x2C));
    u32 draw = load<u32>(methods+0x10);
    call<void>(0x025DEAB4, layer);
    s32 result = call_ptr<s32>(draw, process);
    call<void>(0x025DEAB4, saved_layer);
    return result;
}
VERIFY(0x025DE2CC, fpcDw_Execute_hd);

s32 fpcDw_Handler_hd(u32 handler, u32 callback) {
    WWHD_FUNC(0x025DE37C, s32, handler, callback);
    call<void>(0x02007E18);
    s32 result = call_ptr<s32>(handler, callback);
    call<void>(0x02007E28);
    return result;
}
VERIFY(0x025DE37C, fpcDw_Handler_hd);

void f_pc_draw_sinit_hd() {
    WWHD_FUNC(0x025DE3CC, void);
    store<u32>(0x1048A794, 0);
    store<u32>(0x1048A78C, 0);
    store<u32>(0x1048A798, 0);
    store<u32>(0x1048A790, 0);
    call<void>(0x028F026C, at<void>(0x101F3A4C));
    f32 a = load<f32>(0x100582BC);
    f32 b = load<f32>(0x100582C0);
    store<f32>(0x1048A780, a);
    store<f32>(0x1048A784, b);
    call<void>(0x028ED6F8, at<void>(0x1048A788));
    call<void>(0x028F026C, at<void>(0x101F3A58));
    call<void>(0x028EAB2C, at<void>(0x1048A789));
    call<void>(0x028F026C, at<void>(0x101F3A64));
}
VERIFY(0x025DE3CC, f_pc_draw_sinit_hd);
