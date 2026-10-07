/* HD MsgOnly_00 per-step states. Binary-derived; no GameCube counterpart.
 * Original 30 Hz behaviour. Only UI_msg_window_a entries. */
#include "gabi.h"
using namespace gabi;
namespace hd_screen_msg_only {
static u32 virtualTarget(u32 self, u32 slot) { return load<u32>(load<u32>(self + 4) + slot); }
static void change(u32 self, u32 state) { call<void>(0x020063C0, self + 0x18, state); }
static void refreshWait(u32 self) {
    u32 message = load<u32>(self + 0x118);
    u32 pane = load<u32>(self + 0x12C);
    call<void>(0x0270477C, pane, message);
}

// Open: inline typing accumulator, fast-forward, and message status dispatch.
void openUpdate(u32 self) {
    WWHD_FUNC(0x026AEFF4, void, self);
    u32 flags = load<u32>(self + 0x50);
    u32 core = load<u32>(0x101F4B5C);
    f32 zero = load<f32>(0x100FD0A4);
    u32 message = load<u32>(self + 0x118);
    u32 count = 0;
    bool showAll = false;
    if (flags & 2) {
        u32 text = load<u32>(message + 0x22C);
        if (text && load<u8>(load<u32>(message + 0x22C) + 2) == 0) {
            store<u32>(self + 0x90, 0x14F);
            store<u32>(load<u32>(self + 0x118) + 0x6AC, 0);
            store<f32>(self + 0x94, zero);
            count = 1;
            showAll = true;
        }
    }
    if (!showAll) {
        if (load<s32>(message + 0x6AC) != 0) {
            s32 pause = load<s32>(message + 0x6AC);
            store<u32>(message + 0x6AC, pause > 0 ? u32(pause) - 1 : 0);
            store<f32>(self + 0x94, zero);
            return;
        }
        f32 speed = load<f32>(core + 0x980);
        if (flags & 1) {
            u32 text = load<u32>(message + 0x22C);
            if (text && load<u8>(load<u32>(message + 0x22C) + 2) == 0)
                speed = fmuls_ppc(speed, load<f32>(0x100FD0A8));
        }
        f32 fraction = fadds_ppc(load<f32>(self + 0x94), speed);
        f32 one = load<f32>(0x100FD020);
        if (fraction >= one) {
            do {
                fraction = fsubs_ppc(fraction, one);
                ++count;
            } while (fraction >= one);
        }
        store<f32>(self + 0x94, fraction);
        if (!count) return;
    }
    s32 shown = load<s32>(self + 0x90);
    if (shown < 0x14F) {
        u32 next = load<u32>(self + 0xE8) + 1;
        u32 stop = self + 0xC0 + (next < 10 ? next * 4 : 0);
        shown = s32(u32(shown) + count);
        store<u32>(self + 0x90, u32(shown));
        if (shown > load<s32>(stop)) store<u32>(self + 0x90, load<u32>(stop));
    }
    call_ptr<void>(virtualTarget(self, 0x434), self);
    message = load<u32>(self + 0x118);
    if (load<u8>(message + 0x658)) return;
    u32 status = load<u32>(message);
    if (status == 7) {
        call<void>(0x025F74D0, core, load<u32>(message));
        change(self, 0x1049DF2C);
    } else if (status == 10) {
        call<void>(0x025F74D0, core, load<u32>(message));
        message = load<u32>(self + 0x118);
        if (load<u8>(message + 0x697)) change(self, 0x10497FE0);
        else if (load<u8>(message + 0x698)) change(self, 0x1049E25C);
    } else if (status == 14) {
        call<void>(0x025F74D0, core, load<u32>(message));
        change(self, 0x1049E1CC);
    }
}
VERIFY(0x026AEFF4, openUpdate);

void waitUpdate(u32 self) {
    WWHD_FUNC(0x026AF2E4, void, self);
    u32 message = load<u32>(self + 0x118);
    if (load<u32>(message + 0x6B4) == 0) {
        store<u8>(message + 0x697, 0);
        change(self, 0x104980A0);
    } else {
        s32 timer = load<s32>(message + 0x6B4);
        store<u32>(message + 0x6B4, timer > 0 ? load<u32>(message + 0x6B4) - 1 : 0);
        refreshWait(self);
    }
}
VERIFY(0x026AF2E4, waitUpdate);

void waitInputUpdate(u32 self) {
    WWHD_FUNC(0x026AF338, void, self);
    u32 message = load<u32>(self + 0x118);
    if (load<u32>(message + 0x6B4) == 0) {
        store<u8>(message + 0x698, 0);
        store<u8>(0x1047B09E, 0);
        u32 n = load<u32>(self + 0x84) + 1;
        store<u32>(self + 0x84, n);
        store<u8>(call<u32>(0x025200D4) + 0x5BD2, u8(n));
        change(self, 0x104980A0);
        return;
    }
    if (load<u32>(self + 0x54) & 3) {
        u32 play = call<u32>(0x025200D4);
        u8 blocked = load<u8>(play + 0x5C20);
        message = load<u32>(self + 0x118);
        if (!blocked) {
            store<u8>(message + 0x698, 0);
            store<u8>(0x1047B09E, 0);
            change(self, 0x10498160);
            return;
        }
    }
    s32 timer = load<s32>(message + 0x6B4);
    store<u32>(message + 0x6B4, timer > 0 ? u32(timer) - 1 : 0);
    refreshWait(self);
    store<u8>(0x1047B09E, 4);
}
VERIFY(0x026AF338, waitInputUpdate);

void stopUpdate(u32 self) {
    WWHD_FUNC(0x026AF4E0, void, self);
    if (load<u32>(self + 0x54) & 3) {
        u32 n = load<u32>(self + 0x84) + 1;
        store<u32>(self + 0x84, n);
        store<u8>(call<u32>(0x025200D4) + 0x5BD2, u8(n));
        store<u8>(0x1047B09E, 2);
        change(self, 0x10498100);
    } else {
        u32 pane = load<u32>(self + 0x12C);
        u32 message = load<u32>(self + 0x118);
        call<void>(0x0270477C, pane, message);
        store<u8>(0x1047B09E, 1);
    }
}
VERIFY(0x026AF4E0, stopUpdate);

void closeUpdate(u32 self) {
    WWHD_FUNC(0x026AF5DC, void, self);
    if (!call<u32>(0x026FF668, self, 5u)) return;
    call<void>(0x025F74D0, load<u32>(0x101F4B5C), 0u);
    u32 manager = load<u32>(0x101D5FE8);
    store<u32>(manager + 0x38, load<u32>(manager + 0x38) - 1);
    call_ptr<void>(virtualTarget(self, 0x444), self);
    call_ptr<void>(virtualTarget(self, 0x4A4), self);
    store<u8>(call<u32>(0x025200D4) + 0x5BC6, 0xFF);
    store<u32>(call<u32>(0x025200D4) + 0x5C30, 0xFFFFFFFF);
    store<u8>(call<u32>(0x025200D4) + 0x5BC5, 0);
    store<u32>(call<u32>(0x025200D4) + 0x5C34, 0);
    change(self, 0x10497F80);
}
VERIFY(0x026AF5DC, closeUpdate);

void nextStopUpdate(u32 self) {
    WWHD_FUNC(0x026AF6D8, void, self);
    if (!call_ptr<u32>(virtualTarget(self, 0x4D4), self)) return;
    u32 next = load<u32>(self + 0xE8) + 1;
    u32 message = load<u32>(self + 0x118);
    store<u32>(self + 0xE8, next);
    store<u32>(self + 0x90, 0);
    u32 color = load<u32>(message + 0x91D);
    u32 pane = load<u32>(self + 0x68);
    store<u32>(pane + 0xAC, color);
    store<u32>(pane + 0xB0, color);
    store<u8>(self + 0xED, 0);
    call_ptr<void>(virtualTarget(self, 0x434), self);
    change(self, 0x10497FB0);
}
VERIFY(0x026AF6D8, nextStopUpdate);

void advanceUpdate(u32 self) {
    WWHD_FUNC(0x026AF93C, void, self);
    call_ptr<void>(virtualTarget(self, 0x4B4), self);
    u32 play = call<u32>(0x025200D4);
    call<void>(0x025E1988, load<u8>(play + 0x5BBA) == 39 ? 0x880u : 0x80Bu);
    u32 n = load<u32>(self + 0x84) + 1;
    store<u32>(self + 0x84, n);
    store<u8>(call<u32>(0x025200D4) + 0x5BD2, u8(n));
}
VERIFY(0x026AF93C, advanceUpdate);
}
