/* HD MsgWindowMain_00 second TU per-step states, binary-derived.
 * Original 30 Hz behaviour; UI_msg_window_a only. */
#include "gabi.h"
using namespace gabi;
namespace hd_screen_msg_window_main {
static void change(u32 self, u32 state) { call<void>(0x020063C0, self + 0x18, state); }
static void refresh(u32 self) {
    u32 pane = load<u32>(self + 0x64);
    u32 message = load<u32>(self + 0xB0);
    call<void>(0x0270477C, pane, message);
}
static void sound() {
    u32 play = call<u32>(0x025200D4);
    call<void>(0x025E1988, load<u8>(play + 0x5BBA) == 39 ? 0x880u : 0x80Bu);
}

void advanceUpdate(u32 self) {
    WWHD_FUNC(0x026B7D8C, void, self);
    u32 flags = load<u32>(self + 0x54);
    u32 core = load<u32>(0x101F4B5C);
    if ((flags & 3) || load<u8>(core + 0x928)) {
        call<void>(0x025F74D0, core, 14u);
        sound();
        u32 n = load<u32>(self + 0x78) + 1;
        store<u32>(self + 0x78, n);
        store<u8>(call<u32>(0x025200D4) + 0x5BD2, u8(n));
        store<u8>(0x1047B09E, 0);
        change(self, 0x10498A40);
    } else {
        refresh(self);
        store<u8>(0x1047B09E, 4);
    }
}
VERIFY(0x026B7D8C, advanceUpdate);

void waitUpdate(u32 self) {
    WWHD_FUNC(0x026B7E70, void, self);
    u32 message = load<u32>(self + 0xB0);
    if (load<u32>(message + 0x6B4) == 0 || load<u8>(load<u32>(0x101F4B5C) + 0x928)) {
        store<u8>(message + 0x697, 0);
        change(self, 0x10498A40);
    } else {
        s32 timer = load<s32>(message + 0x6B4);
        store<u32>(message + 0x6B4, timer > 0 ? u32(timer) - 1 : 0);
        u32 msg = load<u32>(self + 0xB0);
        u32 pane = load<u32>(self + 0x64);
        call<void>(0x0270477C, pane, msg);
    }
}
VERIFY(0x026B7E70, waitUpdate);

void waitInputUpdate(u32 self) {
    WWHD_FUNC(0x026B7ED4, void, self);
    u32 message = load<u32>(self + 0xB0);
    if (load<u32>(message + 0x6B4) == 0 || load<u8>(load<u32>(0x101F4B5C) + 0x928)) {
        store<u8>(message + 0x698, 0);
        store<u8>(0x1047B09E, 0);
        u32 n = load<u32>(self + 0x78) + 1;
        store<u32>(self + 0x78, n);
        store<u8>(call<u32>(0x025200D4) + 0x5BD2, u8(n));
        change(self, 0x10498A40);
    } else if (load<u32>(self + 0x54) & 1) {
        store<u8>(message + 0x698, 0);
        sound();
        u32 n = load<u32>(self + 0x78) + 1;
        store<u32>(self + 0x78, n);
        store<u8>(call<u32>(0x025200D4) + 0x5BD2, u8(n));
        store<u8>(0x1047B09E, 0);
        change(self, 0x10498A40);
    } else {
        s32 timer = load<s32>(message + 0x6B4);
        store<u32>(message + 0x6B4, timer > 0 ? load<u32>(message + 0x6B4) - 1 : 0);
        u32 msg = load<u32>(self + 0xB0);
        u32 pane = load<u32>(self + 0x64);
        call<void>(0x0270477C, pane, msg);
        store<u8>(0x1047B09E, 4);
    }
}
VERIFY(0x026B7ED4, waitInputUpdate);

void nextStopUpdate(u32 self) {
    WWHD_FUNC(0x026B80F8, void, self);
    call<void>(0x026B7920, self, 2u);
    u32 next = load<u32>(self + 0xA4) + 1;
    u32 message = load<u32>(self + 0xB0);
    store<u32>(self + 0xA4, next);
    u32 color = load<u32>(message + 0x91D);
    u32 pane = load<u32>(self + 0x64);
    store<u32>(pane + 0xAC, color);
    store<u32>(pane + 0xB0, color);
    call<void>(0x026B7830, self);
}
VERIFY(0x026B80F8, nextStopUpdate);

void stopUpdate(u32 self) {
    WWHD_FUNC(0x026B81CC, void, self);
    if (load<u32>(self + 0x54) & 3) {
        u32 n = load<u32>(self + 0x78) + 1;
        store<u32>(self + 0x78, n);
        store<u8>(call<u32>(0x025200D4) + 0x5BD2, u8(n));
        store<u8>(0x1047B09E, 2);
        u32 next = load<u32>(self + 0xA4) + 1;
        u32 stop = self + 0x7C + (next < 10 ? next * 4 : 0);
        if (load<s32>(stop) >= 0) change(self, 0x104989B0);
        else {
            call<void>(0x025F74D0, load<u32>(0x101F4B5C), 14u);
            change(self, 0x10498A40);
        }
    } else {
        refresh(self);
        store<u8>(0x1047B09E, 1);
    }
}
VERIFY(0x026B81CC, stopUpdate);
}
