/* WWHD process deletion scheduler.
 * HD TU 025DDFA4..025DE2CB, including the header-static initializer.
 * ToQueue is inlined into ToDeleteQ; the HD delete method has no profile unload call. */
#include "bindings.h"
namespace f_pc_deletor_cpp {
static s32 fpcDt_IsComplete() {
    WWHD_FUNC(0x025DDFA4, s32, (u32)0);
    return gabi::call<s32>(0x025DDE18);
}
VERIFY(0x025DDFA4, fpcDt_IsComplete);
static s32 fpcDt_deleteMethod(u32 proc) {
    WWHD_FUNC(0x025DDFA8, s32, proc);
    u32 layer = gabi::load<u32>(proc + 0x60);
    gabi::call<void>(0x025DEAB4, layer);
    gabi::call<void>(0x025DF694, proc + 0x34);
    if (gabi::call<s32>(0x025DD3B0, proc) == 1) {
        gabi::call<void>(0x025DEA54, layer);
        return 1;
    }
    return 0;
}
VERIFY(0x025DDFA8, fpcDt_deleteMethod);
static s32 fpcDt_Handler() {
    WWHD_FUNC(0x025DE024, s32, (u32)0);
    return gabi::call<s32>(0x020100A0, 0x101F3A1Cu, 0x025DDE44u, 0x025DDFA8u);
}
VERIFY(0x025DE024, fpcDt_Handler);
static s32 fpcDt_ToDeleteQ(u32 proc) {
    WWHD_FUNC(0x025DE040, s32, proc);
    if (gabi::load<u8>(proc + 0xA) == 1) return 0;
    if (gabi::call<s32>(0x0201A71C, proc + 0x4C) != 0) return 1;
    if (gabi::call<s32>(0x025DD258, gabi::load<u32>(0x101F3D60), gabi::load<u32>(proc + 0xB8)) != 0) {
        if (gabi::call<s32>(0x025E012C, proc) == 0) return 0;
        u32 layer = proc + 0xC0;
        if (gabi::call<s32>(0x025DEBE0, layer) == 0)
            gabi::call<void>(0x0273AA24, 0x10058288u, 196, 0x10058284u);
        if (gabi::call<s32>(0x025DEDA8, layer, 0x025DE040u, 0) == 0) return 0;
    }
    if (gabi::load<u8>(proc + 0xA) == 1) return 0;
    if (gabi::call<s32>(0x025DD354, proc) != 1) return 0;
    if (gabi::call<s32>(0x025E0CBC, proc + 0x68) == 1)
        gabi::call<void>(0x025E0CC0, proc + 0x68);
    gabi::store<u32>(proc + 0x60, gabi::load<u32>(proc + 0x2C));
    gabi::call<void>(0x025DDE2C, proc + 0x4C);
    gabi::call<void>(0x025DEA44, gabi::load<u32>(proc + 0x2C));
    if (gabi::call<s32>(0x025DE564, gabi::load<u32>(proc + 4)) == 1) {
        if (gabi::call<s32>(0x025DE6C4, proc) == 0) return 0;
    } else {
        if (gabi::call<s32>(0x025DDCE4, proc) == 0) return 0;
    }
    gabi::store<u8>(proc + 0xC, 3);
    return 1;
}
VERIFY(0x025DE040, fpcDt_ToDeleteQ);
static s32 fpcDt_Delete(u32 proc) {
    WWHD_FUNC(0x025DE1B8, s32, proc);
    if (proc == 0) return 1;
    if (gabi::call<s32>(0x025DD9E4, gabi::load<u32>(proc + 0x14)) == 1) return 0;
    if (gabi::load<u8>(proc + 0xC) == 3) return 0;
    return gabi::call<s32>(0x025DE040, proc);
}
VERIFY(0x025DE1B8, fpcDt_Delete);
static void __sinit_f_pc_deletor() {
    WWHD_FUNC(0x025DE238, void, (u32)0);
    sinit_header_statics(0x1048A764, 0x101F3A28);
}
VERIFY(0x025DE238, __sinit_f_pc_deletor);
}
