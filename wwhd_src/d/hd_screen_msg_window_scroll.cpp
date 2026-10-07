/* Scoped HD MsgWindowScroll updates. WWHD binary reconstruction; no rate changes.
 * Array indices use the original fallback-to-slot-zero behaviour. */
#include "gabi.h"
using namespace gabi;
namespace hd_screen_msg_window_scroll {
void setLines(u32 self, u32 text) {
    WWHD_FUNC(0x026B9BD8, void, self, text);
    u32 index = load<u32>(self + 0x118);
    store<u32>(self + 0x11C, 1);
    store<u32>(self + 0xC8 + (index < 20 ? index * 4 : 0), text);
    call<void>(0x026B9AF0, self, 0u);
    index = load<u32>(self + 0x78);
    store<u8>(load<u32>(self + 0x6C + (index < 3 ? index * 4 : 0)) + 0x45, 255);
    if (load<u32>(load<u32>(self + 0x138)) != 7) return;
    index = load<u32>(self + 0x118) + 1;
    const u32 count = load<u32>(self + 0x11C) + 1;
    const u32 message = load<u32>(self + 0x138);
    store<u32>(self + 0x11C, count);
    store<u32>(self + 0xC8 + (index < 20 ? index * 4 : 0), load<u32>(message + 0x64C));
    call<void>(0x026B9AF0, self, 1u);
    index = load<u32>(self + 0x7C);
    store<u8>(load<u32>(self + 0x6C + (index < 3 ? index * 4 : 0)) + 0x45, 100);
    if (load<u32>(load<u32>(self + 0x138)) != 7) return;
    index = load<u32>(self + 0x118) + 2;
    const u32 count3 = load<u32>(self + 0x11C) + 1;
    const u32 message3 = load<u32>(self + 0x138);
    store<u32>(self + 0x11C, count3);
    store<u32>(self + 0xC8 + (index < 20 ? index * 4 : 0), load<u32>(message3 + 0x64C));
    call<void>(0x026B9AF0, self, 2u);
    index = load<u32>(self + 0x80);
    store<u8>(load<u32>(self + 0x6C + (index < 3 ? index * 4 : 0)) + 0x45, 100);
}
VERIFY(0x026B9BD8, setLines);
bool animationEnded(u32 self, u32 animation) {
    WWHD_FUNC(0x026BA0B4, bool, self, animation);
    return call<bool>(0x02005840, load<u32>(load<u32>(self + 0x44) + 0xD4), animation);
}
VERIFY(0x026BA0B4, animationEnded);
void openingMove(u32 self) {
    WWHD_FUNC(0x026BA308, void, self);
    if (call<bool>(0x026BA0B4, self, 0u)) {
        const u32 state = load<u32>(self + 0xCC) == 0xFFFFFFFFu ? 0x10498CD8u : 0x10498BE8u;
        call<void>(0x020063C0, self + 0x18, state);
    }
    const f32 value = call<f32>(0x026BA1A4, self, 5u, load<u32>(self + 0xA0));
    const u8 alpha = u8(ftoi(value));
    const u32 play = call<u32>(0x025200D4);
    store<u8>(play + 0x62F1, alpha);
    store<u32>(self + 0xA0, load<u32>(self + 0xA0) + 1);
}
VERIFY(0x026BA308, openingMove);
void scrollUp(u32 self) {
    WWHD_FUNC(0x026BAA30, void, self);
    f32 position = fadds_ppc(load<f32>(self + 0xC4), load<f32>(0x100FF2C0));
    if (position > load<f32>(self + 0xB4)) {
        for (u32 i = 0; i < 3; ++i) {
            const u32 pane = load<u32>(self + 0x6C + 4 * i);
            store<u8>(pane + 0x44, load<u8>(pane + 0x44) & 0xFE);
        }
        const u32 line = load<u32>(self + 0x118) + 1;
        const f32 target = load<f32>(self + 0xC0);
        const f32 spacing = load<f32>(self + 0xBC);
        store<u32>(self + 0x118, line);
        position = fsubs_ppc(position, spacing);
        store<f32>(self + 0xC0, fsubs_ppc(target, spacing));
        call<void>(0x026B9BD8, self, load<u32>(self + 0xC8 + (line < 20 ? line * 4 : 0)));
    } else {
        const u32 line = load<u32>(self + 0x118);
        call<void>(0x026B9BD8, self, load<u32>(self + 0xC8 + (line < 20 ? line * 4 : 0)));
    }
    const f32 target = load<f32>(self + 0xC0);
    if (position > target) {
        position = target;
        call<void>(0x020063C0, self + 0x18, 0x10498CA8u);
    }
    store<f32>(self + 0xC4, position);
    call<void>(0x026B9E1C, self);
}
VERIFY(0x026BAA30, scrollUp);
void scrollDown(u32 self) {
    WWHD_FUNC(0x026BAC00, void, self);
    f32 position = fsubs_ppc(load<f32>(self + 0xC4), load<f32>(0x100FF2C0));
    if (position < load<f32>(self + 0xB8)) {
        for (u32 i = 0; i < 3; ++i) {
            const u32 pane = load<u32>(self + 0x6C + 4 * i);
            store<u8>(pane + 0x44, load<u8>(pane + 0x44) & 0xFE);
        }
        u32 line = load<u32>(self + 0x118);
        if (s32(line) > 0) {
            const f32 spacing = load<f32>(self + 0xBC);
            const f32 target = load<f32>(self + 0xC0);
            --line;
            store<u32>(self + 0x118, line);
            position = fadds_ppc(position, spacing);
            store<f32>(self + 0xC0, fadds_ppc(target, spacing));
        }
        call<void>(0x026B9BD8, self, load<u32>(self + 0xC8 + (line < 20 ? line * 4 : 0)));
    } else {
        const u32 line = load<u32>(self + 0x118);
        call<void>(0x026B9BD8, self, load<u32>(self + 0xC8 + (line < 20 ? line * 4 : 0)));
    }
    const f32 target = load<f32>(self + 0xC0);
    if (position < target) {
        position = target;
        call<void>(0x020063C0, self + 0x18, 0x10498CA8u);
    }
    store<f32>(self + 0xC4, position);
    call<void>(0x026B9E1C, self);
}
VERIFY(0x026BAC00, scrollDown);
void closingMove(u32 self) {
    WWHD_FUNC(0x026BB1D8, void, self);
    if (call<bool>(0x026BA0B4, self, 1u)) {
        call<void>(0x025F74D0, load<u32>(0x101F4B5C), 18u);
        call<void>(0x020063C0, self + 0x18, 0x10498D68u);
    }
    const f32 value = call<f32>(0x026BB088, self, 5u, load<u32>(self + 0xA0));
    const u8 alpha = u8(ftoi(value));
    const u32 play = call<u32>(0x025200D4);
    store<u8>(play + 0x62F1, alpha);
    const u32 timer = load<u32>(self + 0xA0);
    if (timer) store<u32>(self + 0xA0, timer - 1);
}
VERIFY(0x026BB1D8, closingMove);
}
