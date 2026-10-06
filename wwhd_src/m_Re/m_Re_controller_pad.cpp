/* m_Re_controller_pad: HD controller-pad rumble patterns (file name from the assert strings; the
 * Twilight Princess Wii file of that name, ported to HD), WWHD. HD-only code (no GameCube source): written from the WWHD code.
 * Range 025F29F0..025F2D77 (static initialiser 025F2CDC, two empty companions 025F2D70/025F2D74),
 * plus the two initialiser-only TUs that follow (025F2D78, 025F2E0C).
 *
 * Pattern player (0xC), 4 of them at 101F499C (one per pad):
 *   +0 bit pattern (MSB first; 0 = idle), +4 u16 length in bits, +6 u16 position, +8 u8 loop.
 * A pattern resource starts with its u16 length, the bits follow.
 * 101F5088 -> +0x1D0: nonzero when the GamePad is the rumble target (the whole pattern is then
 * handed to the native motor call 028FB848 at the start of each run; only player 0 is used).
 * Native motor calls: WPADControlMotor (pad, on), VPADStopMotor (pad), VPADControlMotor (pad, pattern, length).
 */
#include "gabi.h"
using namespace gabi;

namespace m_Re_controller_pad {

static const u32 kPlayers = 0x101F499C;

/* 025F29F0: player start (pattern resource, loop) */
void playerStart(u32 p, u32 res, u32 loop) {
    WWHD_FUNC(0x025F29F0, void, p, res, loop);
    const u16 len = load<u16>(res);
    store<u8>(p + 8, u8(loop));
    store<u16>(p + 6, 0);
    store<u32>(p, res + 2);
    store<u16>(p + 4, len);
}
VERIFY(0x025F29F0, playerStart);

/* 025F2A10: player stop at the end of the current run (no loop, position = length) */
void playerStop(u32 p) {
    WWHD_FUNC(0x025F2A10, void, p);
    if (!load<u32>(p)) return;
    const u16 len = load<u16>(p + 4);
    store<u8>(p + 8, 0);
    store<u16>(p + 6, len);
}
VERIFY(0x025F2A10, playerStop);

/* 025F2A30: start a rumble pattern on pad n */
void startPattern(u32 n, u32 res, u32 loop) {
    WWHD_FUNC(0x025F2A30, void, n, res, loop);
    if (n >= 4) call<void>(0x0273AA24, 0x100590ECu, 0x34u, 0x10059104u);
    playerStart(kPlayers + n * 0xC, res, loop);
}
VERIFY(0x025F2A30, startPattern);

/* 025F2AAC: stop the rumble pattern of pad n */
void stopPattern(u32 n) {
    WWHD_FUNC(0x025F2AAC, void, n);
    if (n >= 4) call<void>(0x0273AA24, 0x10059118u, 0x47u, 0x10059130u);
    playerStop(kPlayers + n * 0xC);
}
VERIFY(0x025F2AAC, stopPattern);

/* 025F2B08: per-frame update of the four players */
void update() {
    WWHD_FUNC(0x025F2B08, void);
    u32 p = kPlayers;
    if (!load<u32>(load<u32>(0x101F5088) + 0x1D0)) {
        for (u32 i = 0; i < 4; i++, p += 0xC) {
            u32 pat = load<u32>(p);
            if (!pat) return;
            u32 pos = load<u16>(p + 6);
            if (pos >= load<u16>(p + 4)) {
                if (!load<u8>(p + 8)) {
                    store<u32>(p, 0);
                    call<void>(0xC0008398, i, 0u);
                    continue;
                }
                pat = load<u32>(p);
                pos = 0;
                store<u16>(p + 6, 0);
            }
            const u32 bit = load<u8>(pat + (s32(pos) >> 3)) & u32(0x80 >> (pos & 7));
            call<void>(0xC0008398, 0u, u32(bit != 0));
            store<u16>(p + 6, u16(load<u16>(p + 6) + 1));
        }
        return;
    }
    const u32 pat = load<u32>(p);
    if (!pat) return;
    const u32 len = load<u16>(p + 4);
    if (load<u16>(p + 6) >= len) {
        if (!load<u8>(p + 8)) {
            store<u32>(p, 0);
            call<void>(0xC0007720, 0u);
            return;
        }
        store<u16>(p + 6, 0);
        call<void>(0xC0007510, 0u, pat, len & 0xFF);
    } else if (!load<u16>(p + 6)) {
        call<void>(0xC0007510, 0u, pat, len & 0xFF);
    }
    store<u16>(p + 6, u16(load<u16>(p + 6) + 1));
}
VERIFY(0x025F2B08, update);

/* 025F2C88: stop every motor */
void stopAll() {
    WWHD_FUNC(0x025F2C88, void);
    for (u32 i = 0; i < 4; i++) call<void>(0xC0008398, i, 0u);
    call<void>(0xC0007720, 0u);
}
VERIFY(0x025F2C88, stopAll);

/* header-level static objects of every HD TU (as in hd_input_misc) */
static void stdInit(u32 b, u32 r1, u32 lo, u32 hi, u32 f, u32 r2, u32 r3) {
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, r1);
    const f32 a = load<f32>(lo), c = load<f32>(hi);
    store<f32>(f, a);
    store<f32>(f + 4, c);
    call<void>(0x028ED6F8, f + 8);
    call<void>(0x028F026C, r2);
    call<void>(0x028EAB2C, f + 9);
    call<void>(0x028F026C, r3);
}
void staticInit() {
    WWHD_FUNC(0x025F2CDC, void);
    stdInit(0x1048D568, 0x101F4978, 0x1005914C, 0x10059150, 0x1048D55C, 0x101F4984, 0x101F4990);
}
VERIFY(0x025F2CDC, staticInit);

/* 025F2D70 / 025F2D74: empty companions (inline empty destructors/ callbacks) */
void empty1() { WWHD_FUNC(0x025F2D70, void); }
VERIFY(0x025F2D70, empty1);
void empty2() { WWHD_FUNC(0x025F2D74, void); }
VERIFY(0x025F2D74, empty2);

/* 025F2D78 / 025F2E0C: two initialiser-only TUs */
void staticInitTu2() {
    WWHD_FUNC(0x025F2D78, void);
    stdInit(0x1048D584, 0x101F49CC, 0x10080144, 0x10080148, 0x1048D578, 0x101F49D8, 0x101F49E4);
}
VERIFY(0x025F2D78, staticInitTu2);
void staticInitTu3() {
    WWHD_FUNC(0x025F2E0C, void);
    stdInit(0x1048D5A0, 0x101F49F0, 0x100C4594, 0x100C4598, 0x1048D594, 0x101F49FC, 0x101F4A08);
}
VERIFY(0x025F2E0C, staticInitTu3);

} // namespace m_Re_controller_pad
