// HD event-window state updates, reconstructed from the scoped Wii U functions.
#include "gabi.h"
using namespace gabi;
namespace hd_screen_msg_window_event {
static u32 slot(u32 base, u32 count, u32 index) { return base + (index < count ? index * 4 : 0); }
static void state(u32 self, u32 id) { call<void>(0x020063C0, self + 0x18, id); }
static u32 core() { return load<u32>(0x101F4B5C); }
static u32 play() { return call<u32>(0x025200D4); }
static void status(u32 object, u32 value) { call<void>(0x025F74D0, object, value); }
static void sound(u32 id) { call<void>(0x025E1988, id); }
static void flush() { call<void>(0x025E1C64); }
static void advance(u32 self) {
    u32 frame = load<u32>(self + 0x84) + 1;
    store<u32>(self + 0x84, frame);
    store<u8>(play() + 0x5BD2, frame);
}
u32 lengthIndex(u32 self, s32 index) {
    WWHD_FUNC(0x026B1A50, u32, self, index);
    if (index < 0) index = 0;
    else if (index >= 8) index = 7;
    return load<u32>(0x100FD9BC + (u32)index * 4);
}
VERIFY(0x026B1A50, lengthIndex);
u32 paneIndex(u32 self, s32 row, s32 column) {
    WWHD_FUNC(0x026B1A8C, u32, self, row, column);
    if (row < 0) row = 0;
    else if (row >= 3) row = 2;
    if (column < 0) column = 0;
    else if (column >= 6) column = 5;
    return load<u32>(0x100FD9DC + ((u32)row * 6 + (u32)column) * 4);
}
VERIFY(0x026B1A8C, paneIndex);
void resetPane(u32 self, u32 index) {
    WWHD_FUNC(0x026B1C60, void, self, index);
    call<void>(0x02621388, load<u32>(slot(load<u32>(self + 0x4C), load<u32>(self + 0x48), index)));
}
VERIFY(0x026B1C60, resetPane);
void enterWait(u32 self) {
    WWHD_FUNC(0x026B21B8, void, self);
    u32 message = load<u32>(self + 0x118);
    u8 ready = load<u8>(message + 0x90D);
    u32 controller = core();
    if (ready) {
        store<u8>(message + 0x697, 1);
        message = load<u32>(self + 0x118);
        store<u32>(message + 0x6B4, 30);
        status(controller, 10);
        state(self, 0x1049E22C);
    }
}
VERIFY(0x026B21B8, enterWait);
void initializeBeats(u32 self) {
    WWHD_FUNC(0x026B2224, void, self);
    store<u32>(self + 0x1AC, 0);
    call<void>(0x025E1F58, load<u32>(self + 0x1B0));
    for (u32 i = 0; i < 6; ++i) store<u32>(self + 0x194 + i * 4, 0);
    s32 count = load<s32>(self + 0x1B4);
    store<u32>(self + 0x1C0, 0);
    if (count > 6) store<u32>(self + 0x1B4, 6);
    for (u32 i = 0; (s32)i < load<s32>(self + 0x1B4); ++i) {
        u32 next = slot(self + 0x1C0, 7, i + 1);
        Local<be<u32>> note;
        float length = call<float>(0x025E1F68, load<u32>(self + 0x1B0), i, note.get());
        store<u32>(next, ftoi(length));
        next = slot(self + 0x1C0, 7, i + 1);
        store<u32>(next, load<u32>(next) + load<u32>(slot(self + 0x1C0, 7, i)));
    }
    for (u32 i = 0; (s32)i < load<s32>(self + 0x1B8); ++i) {
        u32 index = call<u32>(0x026B1A8C, self, load<u32>(self + 0x1BC), i);
        call<void>(0x026B1C60, self, index);
    }
    store<u32>(self + 0x190, load<u32>(slot(self + 0x1C0, 7, load<u32>(self + 0x1AC) + 1)) - 1);
}
VERIFY(0x026B2224, initializeBeats);
void showPane(u32 self, u32 index) {
    WWHD_FUNC(0x026B23A4, void, self, index);
    call<void>(0x02621394, load<u32>(slot(load<u32>(self + 0x4C), load<u32>(self + 0x48), index)));
}
VERIFY(0x026B23A4, showPane);
void updateBeatDisplay(u32 self) {
    WWHD_FUNC(0x026B23C4, void, self);
    u32 count = load<u32>(self + 0x1B4);
    s32 frame = load<s32>(self + 0x190);
    s32 end = load<s32>(slot(self + 0x1C0, 7, count));
    if (frame < end) {
        u32 nextFrame = (u32)frame + 1;
        store<u32>(self + 0x190, nextFrame);
        u32 index = load<u32>(self + 0x1AC);
        count = load<u32>(self + 0x1B4);
        if ((s32)index < (s32)count && (s32)nextFrame >= load<s32>(slot(self + 0x1C0, 7, index + 1))) {
            store<u32>(slot(self + 0x194, 6, index), 1);
            index = load<u32>(self + 0x1AC);
            if ((s32)index < load<s32>(self + 0x1B8)) {
                u32 pane = call<u32>(0x026B1A8C, self, load<u32>(self + 0x1BC), index);
                call<void>(0x026B23A4, self, pane);
                index = load<u32>(self + 0x1AC);
            }
            count = load<u32>(self + 0x1B4);
            store<u32>(self + 0x1AC, index + 1);
        }
    }
    for (u32 i = 0; (s32)i < (s32)count; ++i) {
        u32 address = slot(self + 0x194, 6, i);
        s32 age = load<s32>(address);
        if (age > 0 && age <= 35) {
            store<u32>(address, (u32)age + 1);
            count = load<u32>(self + 0x1B4);
        }
    }
    if (load<s32>(slot(self + 0x194, 6, count - 1)) > 35) state(self, 0x1049845C);
}
VERIFY(0x026B23C4, updateBeatDisplay);
void updateShortWait(u32 self) {
    WWHD_FUNC(0x026B25B4, void, self);
    u32 count = load<u32>(self + 0x190) + 1;
    store<u32>(self + 0x190, count);
    if ((s32)count >= 5) state(self, 0x1049848C);
}
VERIFY(0x026B25B4, updateShortWait);
u32 inputReady(u32 self) {
    WWHD_FUNC(0x026B2698, u32, self);
    if (call<u32>(0x027161BC)) { store<u32>(self + 0x1EC, 2); return 0; }
    u32 delay = load<u32>(self + 0x1EC);
    if (delay) { --delay; store<u32>(self + 0x1EC, delay); }
    if ((s32)delay > 0) return 0;
    return load<u8>(0x1047B097) == 0;
}
VERIFY(0x026B2698, inputReady);
void hidePane(u32 self, u32 index) {
    WWHD_FUNC(0x026B2754, void, self, index);
    call<void>(0x026213A4, load<u32>(slot(load<u32>(self + 0x4C), load<u32>(self + 0x48), index)));
}
VERIFY(0x026B2754, hidePane);
void playEventSound(u32 self, u32 alternate) {
    WWHD_FUNC(0x026B2774, void, self, alternate);
    flush();
    u32 game = play();
    sound(load<u8>(game + 0x5BBA) == 0x27 || alternate ? 0x880 : 0x80D);
}
VERIFY(0x026B2774, playEventSound);
void updateSequence(u32 self) {
    WWHD_FUNC(0x026B27CC, void, self);
    if (call<u32>(0x026B2698, self)) {
        status(core(), 14);
        store<u8>(play() + 0x5BD3, 1);
        sound(0x80D);
        flush();
        advance(self);
        return;
    }
    u32 song = load<u8>(play() + 0x5BDA);
    u32 expected = call<u32>(0x026B1A50, self, song);
    u32 actual = call<u32>(0x025E1EFC);
    if (load<s32>(self + 0x1AC) < (s32)expected) {
        if (!(expected == actual && load<u32>(self + 0x1B4) == actual)) {
            for (u32 i = 0; (s32)i < load<s32>(self + 0x1E4); ++i) {
                u32 pane = call<u32>(0x026B1A8C, self, load<u32>(self + 0x1BC), i);
                call<void>(0x026B2754, self, pane);
            }
            store<u32>(self + 0x1B4, actual);
            store<u32>(self + 0x1DC, 0);
            store<u32>(self + 0x1AC, 0);
            store<u32>(self + 0x1E4, 0);
            store<u32>(self + 0x1E0, actual == 3 ? 0 : actual == 4 ? 1 : 2);
        }
        u32 actor = load<u32>(play() + 0x5B2C);
        if (load<u32>(actor + 0x3C0) & 0x01000000) {
            u32 player = load<u32>(play() + 0x5B34);
            u32 note = (u32)(s32)load<s16>(player + 0x691C);
            Local<be<u32>> wanted;
            *wanted.get() = 0xFFFFFFFF;
            if (song != 255) call<void>(0x025E1F68, song, load<u32>(self + 0x1AC), wanted.get());
            if (note == (u32)*wanted.get() && expected == actual) {
                u32 completed = load<u32>(self + 0x1DC);
                u32 progress = load<u32>(self + 0x1AC);
                if (completed == progress) {
                    store<u32>(slot(self + 0x194, 6, completed), 1);
                    if (load<u32>(self + 0x1E0) == load<u32>(self + 0x1BC)) {
                        u32 pane = call<u32>(0x026B1A8C, self, load<u32>(self + 0x1BC), load<u32>(self + 0x1DC));
                        call<void>(0x026B23A4, self, pane);
                        store<u32>(self + 0x1E4, load<u32>(self + 0x1E4) + 1);
                    }
                    store<u32>(self + 0x1DC, load<u32>(self + 0x1DC) + 1);
                }
            }
            u32 progress = load<u32>(self + 0x1AC) + 1;
            store<u32>(self + 0x1AC, progress);
            if ((s32)progress >= (s32)actual) {
                u32 player = load<u32>(play() + 0x5B34);
                if (!call<u32>(0x0244311C, player)) {
                    if (load<u32>(self + 0x1E0) == load<u32>(self + 0x1BC)) {
                        for (u32 i = 0; (s32)i < load<s32>(self + 0x1E4); ++i) {
                            u32 pane = call<u32>(0x026B1A8C, self, load<u32>(self + 0x1BC), i);
                            call<void>(0x026B2754, self, pane);
                        }
                    }
                    store<u32>(self + 0x1E4, 0);
                    store<u32>(self + 0x1AC, 0);
                    store<u32>(self + 0x1DC, 0);
                    for (u32 i = 0; i < 6; ++i) store<u32>(self + 0x194 + i * 4, 0);
                }
            }
        } else if (song != 1 && song != 5 && song != 6 && song != 7) {
            u32 message = load<u32>(self + 0x118);
            u8 ready = load<u8>(message + 0x90D);
            u32 controller = core();
            if (ready) {
                status(controller, 14);
                store<u32>(self + 0x190, 0);
                call<void>(0x026B2774, self, 0u);
                flush();
                advance(self);
                state(self, 0x1049854C);
                return;
            }
        }
    }
    s32 completed = load<s32>(self + 0x1DC);
    if (completed > 0) {
        u32 index = (u32)completed - 1;
        u32 address = slot(self + 0x194, 6, index);
        s32 age = load<s32>(address);
        if (age <= 5) store<u32>(address, (u32)age + 1);
        else if (index == expected - 1) { status(core(), 14); state(self, 0x104984BC); }
    }
}
VERIFY(0x026B27CC, updateSequence);
void updateTyping(u32 self) {
    WWHD_FUNC(0x026B2EB4, void, self);
    u32 controller = core();
    s32 delta = call<s32>(0x026FCB00, self);
    if (!delta) return;
    s32 count = load<s32>(self + 0x90);
    if (count < 335) {
        u32 index = load<u32>(self + 0xE8) + 1;
        count = (s32)((u32)count + (u32)delta);
        store<u32>(self + 0x90, count);
        u32 limit = slot(self + 0xC0, 10, index);
        if (count > load<s32>(limit)) store<u32>(self + 0x90, load<u32>(limit));
    }
    call<void>(0x026B2CBC, self);
    u32 message = load<u32>(self + 0x118);
    if (load<u8>(message + 0x658)) return;
    switch (load<u32>(message)) {
    case 7: state(self, 0x1049DF2C); break;
    case 10:
        if (load<u8>(message + 0x697)) state(self, 0x1049E22C);
        else if (load<u8>(message + 0x698)) state(self, 0x1049E28C);
        break;
    case 6:
        store<u32>(message, 10); status(controller, 10); state(self, 0x1049E1CC); break;
    case 14:
        if (load<u32>(self + 0x11C) == 2) {
            store<u32>(message, 10); status(controller, 10); state(self, 0x1049E1CC);
        } else { status(controller, load<u32>(message)); state(self, 0x1049E2BC); }
        break;
    }
}
VERIFY(0x026B2EB4, updateTyping);
void updateCloseWait(u32 self) {
    WWHD_FUNC(0x026B30FC, void, self);
    if (call<u32>(0x026FF668, self, 4u)) {
        if (load<u32>(self + 0x11C) == 2) {
            u32 object = load<u32>(0x101D5FE8);
            store<u32>(object + 0x38, load<u32>(object + 0x38) - 1);
        }
        state(self, 0x1049857C);
    }
}
VERIFY(0x026B30FC, updateCloseWait);
void advanceStopPoint(u32 self) {
    WWHD_FUNC(0x026B3238, void, self);
    store<u32>(self + 0xE8, load<u32>(self + 0xE8) + 1);
    u32 message = load<u32>(self + 0x118);
    store<u32>(self + 0x90, 0);
    u32 color = load<u32>(message + 0x91D);
    u32 pane = load<u32>(self + 0x68);
    store<u32>(pane + 0xAC, color);
    store<u32>(pane + 0xB0, color);
    call<void>(0x026B2CBC, self);
    call<void>(0x026FF5EC, self, 2u, 1u);
}
VERIFY(0x026B3238, advanceStopPoint);
void updateInput(u32 self) {
    WWHD_FUNC(0x026B3324, void, self);
    bool advanceInput = (load<u32>(self + 0x54) & 3) || load<u32>(load<u32>(self + 0x118) + 0x6B8) || load<u8>(self + 0x1E8);
    if (!advanceInput && load<u32>(self + 0x5C) == 0x5AC) advanceInput = load<u8>(play() + 0x5BD3) != 0;
    if (advanceInput && !load<u8>(play() + 0x5C20)) {
        store<u32>(load<u32>(self + 0x118) + 0x6B8, 0);
        state(self, 0x1049854C);
        store<u8>(0x1047B09E, 0);
        advance(self);
        return;
    }
    store<u8>(0x1047B09E, 4);
}
VERIFY(0x026B3324, updateInput);
} // namespace hd_screen_msg_window_event
