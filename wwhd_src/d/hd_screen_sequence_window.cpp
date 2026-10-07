/* Scoped HD SequenceWindow/BtnSequence updates, reconstructed from the WWHD binary.
 * Original 30 Hz behaviour. */
#include "gabi.h"
using namespace gabi;
namespace hd_screen_sequence_window {
void waitMove(u32 self) {
    WWHD_FUNC(0x026DA8AC, void, self);
    const u32 timer = load<u32>(self + 0xB4);
    if (timer) store<u32>(self + 0xB4, timer - 1);
    else call<void>(0x020063C0, self + 0x18, load<u32>(self + 0x94));
}
VERIFY(0x026DA8AC, waitMove);
void waitFlagMove(u32 self) {
    WWHD_FUNC(0x026DAA34, void, self);
    const u32 timer = load<u32>(self + 0xB4);
    if (timer) {
        const u32 next = timer - 1;
        store<u32>(self + 0xB4, next);
        if (!next) store<u8>(self + 0xD7, 1);
    } else if (!load<u8>(self + 0xD7)) {
        call<void>(0x020063C0, self + 0x18, 0x1049B510u);
    }
}
VERIFY(0x026DAA34, waitFlagMove);
void saveMove(u32 self) {
    WWHD_FUNC(0x026DAB18, void, self);
    u32 timer = load<u32>(self + 0xB4);
    const u32 phase = load<u32>(self + 0xB8);
    if (timer) { --timer; store<u32>(self + 0xB4, timer); }
    if (phase > 1 || (phase == 1 && timer)) return;
    const u32 manager = load<u32>(0x101F852C);
    const u32 status = load<u32>(manager + 0x1C);
    if (status == 2 || status == 3) return;
    if (phase == 0) {
        call<void>(0x02721D48, manager);
        store<u32>(self + 0xB8, 1);
    } else {
        call<void>(0x02030B38, 0x10103FC0u);
        call<void>(0x020063C0, self + 0x18, 0x1049B660u);
    }
}
VERIFY(0x026DAB18, saveMove);
void buttonAdvance(u32 self) {
    WWHD_FUNC(0x026DAC68, void, self);
    store<u32>(self + 0x8C, load<u32>(self + 0x8C) + 0x28);
    call<void>(0x026D9974, self, 1u);
    call<void>(0x026D9C48, self);
}
VERIFY(0x026DAC68, buttonAdvance);
void loadMove(u32 self) {
    WWHD_FUNC(0x026DAD14, void, self);
    u32 timer = load<u32>(self + 0xB4);
    const u32 phase = load<u32>(self + 0xB8);
    if (timer) { --timer; store<u32>(self + 0xB4, timer); }
    if (phase > 1 || (phase == 1 && timer)) return;
    const u32 manager = load<u32>(0x101F852C);
    const u32 status = load<u32>(manager + 0x1C);
    if (status == 2 || status == 3) return;
    if (phase == 0) {
        call<void>(0x0272276C, manager);
        store<u32>(self + 0xB8, 1);
    } else {
        call<void>(0x02030B38, 0x10103FD0u);
        call<void>(0x020063C0, self + 0x18, 0x1049B660u);
    }
}
VERIFY(0x026DAD14, loadMove);
}
