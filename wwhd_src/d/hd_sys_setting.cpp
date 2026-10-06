/* hd_sys_setting: HD system setting singleton (a console setting read through 0286D620, mirrored
 * into the save options), WWHD. HD-only code (no GameCube
 * source): written from the WWHD code. Range 025F9370..025F9643 (static initialiser 025F95B0).
 *
 * SysSetting (0x18), singleton 101F4BAC (disposer holder 101F4BB0): +0..+0x10 sead disposer (vtable
 * 100E0D88 at +0xC), +0x10 state (1 not read / unavailable, 2 read), +0x14 value (1, 2 or 5; other
 * values are reset to 1). The value is stored as 0/2/3 in byte +4 of the save options object
 * (027200D0 of the save info + 0x12C0). The message manager reads +0x14 as the language id.
 */
#include "gabi.h"
using namespace gabi;

namespace hd_sys_setting {

static const u32 kInstance = 0x101F4BAC;

/* 025F9370: constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x025F9370, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x18u);
        if (!p) return 0;
    }
    store<u32>(p + 0x10, 1);
    store<u32>(p + 0x14, 0);
    return p;
}
VERIFY(0x025F9370, ctor);

/* 025F93B4: creates the singleton with its disposer */
void createInstance(u32 heap) {
    WWHD_FUNC(0x025F93B4, void, heap);
    if (load<u32>(kInstance)) return;
    const u32 p = call<u32>(0x0273B0D4, 0x18u, heap, 4u);
    if (p) {
        call<void>(0x02752B0C, p, heap, 3u);
        store<u32>(p + 0xC, 0x100E0D88);
    }
    store<u32>(0x101F4BB0, p);
    store<u32>(kInstance, p ? ctor(p) : 0u);
}
VERIFY(0x025F93B4, createInstance);

/* 025F9448: reads the setting and mirrors it into the save options */
void update(u32 p) {
    WWHD_FUNC(0x025F9448, void, p);
    if (s32(call<u32>(0x0286D620, p + 0x14)) == -3) {
        store<u32>(p + 0x10, 1);
        store<u32>(p + 0x14, 0);
        return;
    }
    u32 v = load<u32>(p + 0x14);
    store<u32>(p + 0x10, 2);
    if (v < 1 || (v > 2 && v != 5)) {
        v = 1;
        store<u32>(p + 0x14, 1);
    }
    u8 m = 0;
    if (v == 2) m = 2;
    else if (v == 5) m = 3;
    const u32 opt = call<u32>(0x027200D0, load<u32>(0x101F84DC) + 0x12C0);
    store<u8>(opt + 4, m);
}
VERIFY(0x025F9448, update);

/* 025F9530: destructor */
void dtor(u32 p, u32 flags) {
    WWHD_FUNC(0x025F9530, void, p, flags);
    if (!p) return;
    store<u32>(p + 0xC, 0x100E0D88);
    if (p == load<u32>(0x101F4BB0)) {
        store<u32>(0x101F4BB0, 0);
        store<u32>(kInstance, 0);
    }
    call<void>(0x02752BEC, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x025F9530, dtor);

void staticInit() {
    WWHD_FUNC(0x025F95B0, void);
    const u32 b = 0x1048D7A8;
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, 0x101F4B88u);
    const f32 lo = load<f32>(0x100E0D80), hi = load<f32>(0x100E0D84);
    store<f32>(0x1048D79C, lo);
    store<f32>(0x1048D7A0, hi);
    call<void>(0x028ED6F8, 0x1048D7A4u);
    call<void>(0x028F026C, 0x101F4B94u);
    call<void>(0x028EAB2C, 0x1048D7A5u);
    call<void>(0x028F026C, 0x101F4BA0u);
}
VERIFY(0x025F95B0, staticInit);

} // namespace hd_sys_setting
