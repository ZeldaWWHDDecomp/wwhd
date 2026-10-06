/* WWHD m_Do_audio full HD cluster15CC..2113: timer, frame/tact/scene/wave,
 * audio compatibility entry points, global-object initializer. Most header wrappers
 * mapped to legacy JAIZelBasic names are actually singleton shims, not methods.
 * Reconstruction retains every callee argument read, including passthrough ABI slots.
 */
#include "bindings.h"
namespace m_Do_audio_cpp {
// Returns the timer's address (lbzu leaves r3 = 0x101F4707) when it is <= 1, else cLib_calcTimer<u8>'s result (tail branch).
static u32 calcLoadTimer() {
    WWHD_FUNC(0x025E15CC, u32, (u32)0);
    if (gabi::load<u8>(0x101F4707) <= 1) return 0x101F4707u;
    return gabi::call<u32>(0x0207A9A0, 0x101F4707u);
}
VERIFY(0x025E15CC, calcLoadTimer);
static void Execute() {
    WWHD_FUNC(0x025E15E0, void, (u32)0);
    if (gabi::load<u32>(0x101FFC78) != 0) {
        gabi::call<void>(0x025E15CC);
        u32 play = gabi::load<u32>(0x101F84DC);
        gabi::call<void>(0x020277A4, gabi::load<u32>(0x101FFC78), play + 0x644);
    }
}
VERIFY(0x025E15E0, Execute);
static s32 getTactDirection(s32 stick, s32 previous) {
    WWHD_FUNC(0x025E162C, s32, stick, previous);
    f64 magnitude;
    s32 angle;
    if (stick != 0) {
        magnitude = gabi::call<f64>(0x020079B4, 0);
        angle = gabi::call<s32>(0x02007A1C, 0);
    } else {
        magnitude = gabi::call<f64>(0x02007B44, 0);
        angle = gabi::call<s32>(0x02007BA0, 0);
    }
    if (magnitude < gabi::load<f32>(0x100584FC)) return 0;
    s32 absolute = angle < 0 ? (s32)(0u - (u32)angle) : angle;
    if (previous == 0) {
        if (absolute > 0x6000) return 1;
        if (angle >= 0x2000) return 2;
        if (angle <= -0x2000) return 4;
        return 3;
    }
    if (absolute > 0x7000) return 1;
    if ((u32)angle - 0x3000u < 0x2001u) return 2;
    if ((u32)angle + 0x5000u < 0x2001u) return 4;
    if (absolute < 0x1000) return 3;
    return previous;
}
VERIFY(0x025E162C, getTactDirection);
static void setSceneName(u32 name, s32 room, s32 layer) {
    WWHD_FUNC(0x025E17CC, void, name, room, layer);
    if (gabi::load<u8>(0x101F4707) == 0) {
        gabi::call<void>(0x02027814, gabi::load<u32>(0x101FFC78), name, room, layer);
        gabi::store<u8>(0x101F4707, 36);
    }
}
VERIFY(0x025E17CC, setSceneName);
static s32 load1stDynamicWave() {
    WWHD_FUNC(0x025E1828, s32, (u32)0);
    u8 timer = gabi::load<u8>(0x101F4707);
    if (timer == 0) return 1;
    if (timer <= 1) {
        gabi::call<void>(0x02026E04, gabi::load<u32>(0x101FFC78));
        gabi::store<u8>(0x101F4707, 0);
        return 1;
    }
    return 0;
}
VERIFY(0x025E1828, load1stDynamicWave);
static u32 seStart(u32 sound) {
    WWHD_FUNC(0x025E1988, u32, sound);
    if (sound == 0x806) return gabi::call<u32>(0x020311A0);
    f32 one = gabi::load<f32>(0x10058500);
    f32 negative = gabi::load<f32>(0x10058504);
    return gabi::call<u32>(0x0201EBA0, gabi::load<u32>(0x101FFC78), sound, 0, 0, 0, 0, one, one, negative, negative);
}
VERIFY(0x025E1988, seStart);
static void resetProcess() {
    WWHD_FUNC(0x025E1D40, void, (u32)0);
    if (gabi::load<u8>(0x101F4705) != 0) {
        gabi::call<void>(0x020263C0, gabi::load<u32>(0x101FFC78));
        gabi::call<void>(0x02030C10);
        gabi::store<u8>(0x101F4705, 1);
    }
}
VERIFY(0x025E1D40, resetProcess);
static void resetRecover() {
    WWHD_FUNC(0x025E1D8C, void, (u32)0);
    if (gabi::load<u8>(0x101F4705) != 0) {
        gabi::call<void>(0x0202736C, gabi::load<u32>(0x101FFC78));
        gabi::call<void>(0x02030C10);
        gabi::store<u8>(0x101F4705, 0);
    }
}
VERIFY(0x025E1D8C, resetRecover);
static void __sinit_audio() {
    WWHD_FUNC(0x025E202C, void, (u32)0);
    sinit_header_statics(0x1048ABBC, 0x101F46E0);
    gabi::call<void>(0x02029E5C, 0x1048CDD0u);
    gabi::call<void>(0x02025B44, 0x1048ABD8u);
    gabi::store<u32>(0x1048AC04, 0x100584B4);
}
VERIFY(0x025E202C, __sinit_audio);

static u32 audio_025E18AC() {
    WWHD_FUNC(0x025E18AC, u32, (u32)0);
    return gabi::call<u32>(0x02027AE4);
}
VERIFY(0x025E18AC, audio_025E18AC);

static u32 audio_025E18B8(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E18B8, u32, a0, a1, a2, a3, a4, a5, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0202796C, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a1, a2, a3, a4, a5, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E18B8, audio_025E18B8);

static u32 audio_025E18C4(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E18C4, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x020271E4, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E18C4, audio_025E18C4);

static u32 audio_025E18D0() {
    WWHD_FUNC(0x025E18D0, u32, (u32)0);
    return gabi::call<u32>(0x028024E0);
}
VERIFY(0x025E18D0, audio_025E18D0);

static u32 audio_025E18D4() {
    WWHD_FUNC(0x025E18D4, u32, (u32)0);
    return gabi::call<u32>(0x0202573C, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)));
}
VERIFY(0x025E18D4, audio_025E18D4);

static u32 audio_025E18E0(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E18E0, u32, a0, a1, a2, a3, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x020234A4, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a1, a2, a3, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E18E0, audio_025E18E0);

static u32 audio_025E18EC(u32 a0, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E18EC, u32, a0, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0202204C, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, 0x00000000u, 0x00000000u, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E18EC, audio_025E18EC);

static u32 audio_025E1904(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1904, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x02021F28, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, 0x00000000u, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1904, audio_025E1904);

static u32 audio_025E1918(u32 a0, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1918, u32, a0, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0201DE00, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1918, audio_025E1918);

static u32 audio_025E1928() {
    WWHD_FUNC(0x025E1928, u32, (u32)0);
    return gabi::call<u32>(0x0201D8BC, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)));
}
VERIFY(0x025E1928, audio_025E1928);

static u32 audio_025E1934(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1934, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x02027168, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1934, audio_025E1934);

static u32 audio_025E1944(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1944, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x02027F04, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1944, audio_025E1944);

static u32 audio_025E1950(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1950, u32, a0, a1, a2, a3, a4, a5, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x02027FD4, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a2, a3, a4, a5, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1950, audio_025E1950);

static u32 audio_025E1960(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1960, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0202844C, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1960, audio_025E1960);

static u32 audio_025E1970(f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1970, u32, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x020284B0, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1970, audio_025E1970);

static u32 audio_025E197C(f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E197C, u32, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x020284E4, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E197C, audio_025E197C);

static u32 audio_025E19CC(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E19CC, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0201EBA0, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a1, 0x00000000u, 0x00000000u, 0x00000000u, a6, a7, gabi::load<f32>((0x10060000u + 0xFFFF8500u)), gabi::load<f32>((0x10060000u + 0xFFFF8500u)), gabi::load<f32>((0x10060000u + 0xFFFF8504u)), gabi::load<f32>((0x10060000u + 0xFFFF8504u)), f5, f6, f7, f8);
}
VERIFY(0x025E19CC, audio_025E19CC);

static u32 audio_025E1A04(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1A04, u32, a0, a1, a2, a3, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0201EBA0, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a1, a2, 0x00000000u, 0x00000000u, a6, 0x10060000u, gabi::load<f32>((0x10060000u + 0xFFFF8500u)), gabi::load<f32>((0x10060000u + 0xFFFF8500u)), gabi::load<f32>((0x10060000u + 0xFFFF8504u)), gabi::load<f32>((0x10060000u + 0xFFFF8504u)), f5, f6, f7, f8);
}
VERIFY(0x025E1A04, audio_025E1A04);

static u32 audio_025E1A40(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1A40, u32, a0, a1, a2, a3, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0201EBA0, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a1, a2, a3, 0x00000000u, a6, 0x10060000u, gabi::load<f32>((0x10060000u + 0xFFFF8500u)), gabi::load<f32>((0x10060000u + 0xFFFF8500u)), gabi::load<f32>((0x10060000u + 0xFFFF8504u)), gabi::load<f32>((0x10060000u + 0xFFFF8504u)), f5, f6, f7, f8);
}
VERIFY(0x025E1A40, audio_025E1A40);

static u32 audio_025E1A7C(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1A7C, u32, a0, a1, a2, a3, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x020299D8, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a1, a2, a3, 0x00000000u, a6, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1A7C, audio_025E1A7C);

static u32 audio_025E1AA4(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1AA4, u32, a0, a1, a2, a3, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x020299D8, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a1, a2, a3, a4, a6, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1AA4, audio_025E1AA4);

static u32 audio_025E1ACC(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1ACC, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0201E538, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, 0x00000000u, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1ACC, audio_025E1ACC);

static u32 audio_025E1AE0(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1AE0, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0201E538, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a1, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1AE0, audio_025E1AE0);

static u32 audio_025E1AF8(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1AF8, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x02029044, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, 0xFFFFFFFFu, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1AF8, audio_025E1AF8);

static u32 audio_025E1B0C(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1B0C, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x02029044, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a1, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1B0C, audio_025E1B0C);

static u32 audio_025E1B24(u32 a0) {
    WWHD_FUNC(0x025E1B24, u32, a0);
    return gabi::call<u32>(0x0201D62C, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0);
}
VERIFY(0x025E1B24, audio_025E1B24);

static u32 audio_025E1B34(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1B34, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x02029070, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1B34, audio_025E1B34);

static u32 audio_025E1B44(u32 a0, u32 a1, u32 a2) {
    WWHD_FUNC(0x025E1B44, u32, a0, a1, a2);
    return gabi::call<u32>(0x02027558, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a1, a2);
}
VERIFY(0x025E1B44, audio_025E1B44);

static u32 audio_025E1B60(u32 a0) {
    WWHD_FUNC(0x025E1B60, u32, a0);
    return gabi::call<u32>(0x02027668, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0);
}
VERIFY(0x025E1B60, audio_025E1B60);

static u32 audio_025E1B70() {
    WWHD_FUNC(0x025E1B70, u32, (u32)0);
    return gabi::call<u32>(0x0201C4B8, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)));
}
VERIFY(0x025E1B70, audio_025E1B70);

static u32 audio_025E1B7C(u32 a0) {
    WWHD_FUNC(0x025E1B7C, u32, a0);
    return gabi::call<u32>(0x0201C4C4, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0);
}
VERIFY(0x025E1B7C, audio_025E1B7C);

static u32 audio_025E1B8C(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1B8C, u32, a0, a1, a2, a3, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0201C524, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, 0x00000000u, a3, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1B8C, audio_025E1B8C);

static u32 audio_025E1BA0(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1BA0, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0201CA10, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a1, a0, 0x00000000u, 0x00000000u, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1BA0, audio_025E1BA0);

static u32 audio_025E1BB8(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1BB8, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0201CA10, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a1, a0, 0x00000001u, 0x00000000u, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1BB8, audio_025E1BB8);

static u32 audio_025E1BD0(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1BD0, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0201CC50, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1BD0, audio_025E1BD0);

static u32 audio_025E1BE0(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1BE0, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0201CDA0, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a1, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1BE0, audio_025E1BE0);

static u32 audio_025E1BF8() {
    WWHD_FUNC(0x025E1BF8, u32, (u32)0);
    return gabi::call<u32>(0x0201CC44, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)));
}
VERIFY(0x025E1BF8, audio_025E1BF8);

static u32 audio_025E1C04(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1C04, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0201CF54, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a1, a2, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1C04, audio_025E1C04);

static u32 audio_025E1C20() {
    WWHD_FUNC(0x025E1C20, u32, (u32)0);
    return gabi::call<u32>(0x0201D140, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)));
}
VERIFY(0x025E1C20, audio_025E1C20);

static u32 audio_025E1C2C(u32 a0) {
    WWHD_FUNC(0x025E1C2C, u32, a0);
    return gabi::call<u32>(0x0201D14C, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0);
}
VERIFY(0x025E1C2C, audio_025E1C2C);

static u32 audio_025E1C3C(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1C3C, u32, a0, a1, a2, a3, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0202914C, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a1, a2, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1C3C, audio_025E1C3C);

static u32 audio_025E1C58(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1C58, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x02027D4C, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1C58, audio_025E1C58);

static u32 audio_025E1C64(f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1C64, u32, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0201DB50, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), 0x0000001Eu, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1C64, audio_025E1C64);

static u32 audio_025E1C74() {
    WWHD_FUNC(0x025E1C74, u32, (u32)0);
    return gabi::call<u32>(0x02029C98, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)));
}
VERIFY(0x025E1C74, audio_025E1C74);

static u32 audio_025E1C84() {
    WWHD_FUNC(0x025E1C84, u32, (u32)0);
    return gabi::call<u32>(0x02027D40, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)));
}
VERIFY(0x025E1C84, audio_025E1C84);

static u32 audio_025E1C90(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1C90, u32, a0, a1, a2, a3, a4, a5, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x020293CC, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a1, a2, a3, a5, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1C90, audio_025E1C90);

static u32 audio_025E1CB4(u32 a0) {
    WWHD_FUNC(0x025E1CB4, u32, a0);
    return gabi::call<u32>(0x020292B4, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0);
}
VERIFY(0x025E1CB4, audio_025E1CB4);

static u32 audio_025E1CC4(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7) {
    WWHD_FUNC(0x025E1CC4, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7);
    return gabi::call<u32>(0x02029638, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7);
}
VERIFY(0x025E1CC4, audio_025E1CC4);

static u32 audio_025E1CD4(u32 a0) {
    WWHD_FUNC(0x025E1CD4, u32, a0);
    return gabi::call<u32>(0x020299D0, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0);
}
VERIFY(0x025E1CD4, audio_025E1CD4);

static u32 audio_025E1CE4(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7) {
    WWHD_FUNC(0x025E1CE4, u32, a0, a1, a2, a3, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7);
    return gabi::call<u32>(0x02027B0C, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a1, a3, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7);
}
VERIFY(0x025E1CE4, audio_025E1CE4);

static u32 audio_025E1CFC() {
    WWHD_FUNC(0x025E1CFC, u32, (u32)0);
    return gabi::call<u32>(0x02028D6C, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)));
}
VERIFY(0x025E1CFC, audio_025E1CFC);

static u32 audio_025E1D08(u32 a0) {
    WWHD_FUNC(0x025E1D08, u32, a0);
    return gabi::call<u32>(0x02028FE0, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0);
}
VERIFY(0x025E1D08, audio_025E1D08);

static u32 audio_025E1D18() {
    WWHD_FUNC(0x025E1D18, u32, (u32)0);
    return gabi::call<u32>(0x020283D8, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)));
}
VERIFY(0x025E1D18, audio_025E1D18);

static u32 audio_025E1D24(f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1D24, u32, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x020283E4, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1D24, audio_025E1D24);

static u32 audio_025E1D30(u32 a0) {
    WWHD_FUNC(0x025E1D30, u32, a0);
    return gabi::call<u32>(0x02028E54, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0);
}
VERIFY(0x025E1D30, audio_025E1D30);

static u32 audio_025E1DD8(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1DD8, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0202879C, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1DD8, audio_025E1DD8);

static u32 audio_025E1DE4(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1DE4, u32, a0, a1, a2, a3, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x02027670, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a2, a3, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1DE4, audio_025E1DE4);

static u32 audio_025E1DF4(u32 a0) {
    WWHD_FUNC(0x025E1DF4, u32, a0);
    return gabi::call<u32>(0x02027778, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0);
}
VERIFY(0x025E1DF4, audio_025E1DF4);

static u32 audio_025E1E04(u32 a0) {
    WWHD_FUNC(0x025E1E04, u32, a0);
    return gabi::call<u32>(0x02027CEC, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0);
}
VERIFY(0x025E1E04, audio_025E1E04);

static u32 audio_025E1E14(u32 a0, u32 a1) {
    WWHD_FUNC(0x025E1E14, u32, a0, a1);
    return gabi::call<u32>(0x02027C50, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a1);
}
VERIFY(0x025E1E14, audio_025E1E14);

static u32 audio_025E1E2C(u32 a0, u32 a1) {
    WWHD_FUNC(0x025E1E2C, u32, a0, a1);
    return gabi::call<u32>(0x02027C9C, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a1);
}
VERIFY(0x025E1E2C, audio_025E1E2C);

static u32 audio_025E1E44(u32 a0) {
    WWHD_FUNC(0x025E1E44, u32, a0);
    return gabi::call<u32>(0x02027CF4, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0);
}
VERIFY(0x025E1E44, audio_025E1E44);

static u32 audio_025E1E54(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1E54, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0202348C, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1E54, audio_025E1E54);

static u32 audio_025E1E60(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1E60, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0202149C, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1E60, audio_025E1E60);

static u32 audio_025E1E6C(u32 a0) {
    WWHD_FUNC(0x025E1E6C, u32, a0);
    return gabi::call<u32>(0x02027FF0, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0);
}
VERIFY(0x025E1E6C, audio_025E1E6C);

static u32 audio_025E1E7C(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1E7C, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x02028518, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1E7C, audio_025E1E7C);

static u32 audio_025E1E88(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1E88, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0202862C, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1E88, audio_025E1E88);

static u32 audio_025E1E94(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1E94, u32, a0, a1, a2, a3, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x02029F10, (0x10490000u + 0xFFFFCDD0u), a1, a2, a3, a4, a5, a6, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1E94, audio_025E1E94);

static u32 audio_025E1EA0() {
    WWHD_FUNC(0x025E1EA0, u32, (u32)0);
    return gabi::call<u32>(0x0202A038);
}
VERIFY(0x025E1EA0, audio_025E1EA0);

static u32 audio_025E1EB4() {
    WWHD_FUNC(0x025E1EB4, u32, (u32)0);
    return gabi::call<u32>(0x0202ABA0, (0x10490000u + 0xFFFFCDD0u));
}
VERIFY(0x025E1EB4, audio_025E1EB4);

static u32 audio_025E1EC0() {
    WWHD_FUNC(0x025E1EC0, u32, (u32)0);
    return gabi::call<u32>(0x0202A034, (0x10490000u + 0xFFFFCDD0u));
}
VERIFY(0x025E1EC0, audio_025E1EC0);

static u32 audio_025E1ED4() {
    WWHD_FUNC(0x025E1ED4, u32, (u32)0);
    return gabi::call<u32>(0x0202AB9C, (0x10490000u + 0xFFFFCDD0u));
}
VERIFY(0x025E1ED4, audio_025E1ED4);

static u32 audio_025E1EE0(u32 a0) {
    WWHD_FUNC(0x025E1EE0, u32, a0);
    return gabi::call<u32>(0x0202A040, (0x10490000u + 0xFFFFCDD0u), a0);
}
VERIFY(0x025E1EE0, audio_025E1EE0);

static u32 audio_025E1EF0(f64 f1) {
    WWHD_FUNC(0x025E1EF0, u32, f1);
    return gabi::call<u32>(0x0202A14C, (0x10490000u + 0xFFFFCDD0u), f1);
}
VERIFY(0x025E1EF0, audio_025E1EF0);

static u32 audio_025E1EFC() {
    WWHD_FUNC(0x025E1EFC, u32, (u32)0);
    return gabi::load<u8>((0x10490000u + 0xFFFFCDD7u));
}
VERIFY(0x025E1EFC, audio_025E1EFC);

/* mDoAud_tact_getBeatFrames: returns the f32 at 0x1048CDE8 in f1 (lfs f1); the player's tact code reads it as the beat
 * length (game test 2026-10-05: returning r3 broke the Wind's Requiem conducting). */
static f32 audio_025E1F08() {
    WWHD_FUNC(0x025E1F08, f32);
    return gabi::load<f32>(0x1048CDE8);
}
VERIFY(0x025E1F08, audio_025E1F08);

static u32 audio_025E1F14(f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1F14, u32, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0202A77C, (0x10490000u + 0xFFFFCDD0u), f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1F14, audio_025E1F14);

static u32 audio_025E1F20(u32 a0, u32 a1, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1F20, u32, a0, a1, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0202A1B8, (0x10490000u + 0xFFFFCDD0u), a0, a1, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1F20, audio_025E1F20);

static u32 audio_025E1F34(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1F34, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0202A3B4, (0x10490000u + 0xFFFFCDD0u), a0, a1, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1F34, audio_025E1F34);

static u32 audio_025E1F48(u32 a0) {
    WWHD_FUNC(0x025E1F48, u32, a0);
    return gabi::call<u32>(0x0202A87C, (0x10490000u + 0xFFFFCDD0u), a0);
}
VERIFY(0x025E1F48, audio_025E1F48);

static u32 audio_025E1F58(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1F58, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0202A8F0, (0x10490000u + 0xFFFFCDD0u), a0, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1F58, audio_025E1F58);

/* mDoAud_getMelodyPattern(n, step, out): tail branch to JAIZelInst::getMelodyPattern(0x1048CDD0, n, step, out), which reads
 * r3..r6 and returns an f32 in f1 (the metronome reads f1). */
static f32 audio_025E1F68(s32 n, s32 step, u32 out) {
    WWHD_FUNC(0x025E1F68, f32, n, step, out);
    return gabi::call<f32>(0x0202AA5C, 0x1048CDD0u, n, step, out);
}
VERIFY(0x025E1F68, audio_025E1F68);

static u32 audio_025E1F80(u32 a0) {
    WWHD_FUNC(0x025E1F80, u32, a0);
    gabi::store<u8>((gabi::load<u32>((0x10200000u + 0xFFFFFC78u)) + 0x0000003Cu), a0);
    return a0;
}
VERIFY(0x025E1F80, audio_025E1F80);

static u32 audio_025E1F90(u32 a0) {
    WWHD_FUNC(0x025E1F90, u32, a0);
    gabi::store<u8>((gabi::load<u32>((0x10200000u + 0xFFFFFC78u)) + 0x0000003Du), a0);
    return a0;
}
VERIFY(0x025E1F90, audio_025E1F90);

static u32 audio_025E1FA0(u32 a0) {
    WWHD_FUNC(0x025E1FA0, u32, a0);
    gabi::store<u8>((gabi::load<u32>((0x10200000u + 0xFFFFFC78u)) + 0x0000003Eu), a0);
    return a0;
}
VERIFY(0x025E1FA0, audio_025E1FA0);

static u32 audio_025E1FB0(u32 a0, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1FB0, u32, a0, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x02028038, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1FB0, audio_025E1FB0);

static u32 audio_025E1FC0(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1FC0, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x020282F8, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1FC0, audio_025E1FC0);

static u32 audio_025E1FCC(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1FCC, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x020214B4, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1FCC, audio_025E1FCC);

static u32 audio_025E1FD8(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1FD8, u32, a0, a1, a2, a3, a4, a5, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0201E4AC, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a1, a2, a3, a4, a5, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1FD8, audio_025E1FD8);

static u32 audio_025E1FE4(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1FE4, u32, a0, a1, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x02028FE8, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, a2, a3, a4, a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1FE4, audio_025E1FE4);

static u32 audio_025E1FF4(f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E1FF4, u32, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x02028FF8, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E1FF4, audio_025E1FF4);

static u32 audio_025E2000() {
    WWHD_FUNC(0x025E2000, u32, (u32)0);
    return gabi::call<u32>(0x02029038, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)));
}
VERIFY(0x025E2000, audio_025E2000);

static u32 audio_025E200C(u32 a0, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    WWHD_FUNC(0x025E200C, u32, a0, f1, f2, f3, f4, f5, f6, f7, f8);
    return gabi::call<u32>(0x0201D1AC, gabi::load<u32>((0x10200000u + 0xFFFFFC78u)), a0, f1, f2, f3, f4, f5, f6, f7, f8);
}
VERIFY(0x025E200C, audio_025E200C);

static u32 audio_025E201C(u32 a0) {
    WWHD_FUNC(0x025E201C, u32, a0);
    gabi::store<u8>((gabi::load<u32>((0x10200000u + 0xFFFFFC78u)) + 0x000000CFu), a0);
    return a0;
}
VERIFY(0x025E201C, audio_025E201C);

/* Trailing emitted header stubs and the deleting destructor (vtable100584B4+0C). */
static u32 audio_zero_20F0() {
    WWHD_FUNC(0x025E20F0, u32, (u32)0);
    return 0;
}
VERIFY(0x025E20F0, audio_zero_20F0);
static u32 audio_zero_20F8() {
    WWHD_FUNC(0x025E20F8, u32, (u32)0);
    return 0;
}
VERIFY(0x025E20F8, audio_zero_20F8);
static void audio_destructor(u32 object, u32 flags) {
    WWHD_FUNC(0x025E2100, void, object, flags);
    if (object == 0) return;
    if ((flags & 1) == 0) return;
    gabi::call<void>(0x0273AF40, object);
}
VERIFY(0x025E2100, audio_destructor);
}
