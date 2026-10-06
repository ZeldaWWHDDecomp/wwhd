/* JAIZelBasic (WWHD), part 4: bgmStart (0202204C..020233C7).
 * A "Nonmatching" stub in the GameCube decompilation,
 * so the body is written from the WWHD code and verified against cking.rpx. See JAIZelBasic.cpp. */
#include "bindings.h"

namespace JAIZelBasic_4_cpp {
#include "jaizel_local.h"
#include "jaizel_basic_local.h"

static inline void bgmMute_l(u32 b, u32 h, u32 bgm, u32 set, u32 fade) { gabi::call(0x0201DD18, b, h, bgm, set, fade); }
static inline void bgmStop_l(u32 b, u32 fade, u32 keepSub) { gabi::call(0x02021F28, b, fade, keepSub); }
static inline void JAISound_setTempoProportion_l(u32 s, f32 v, u32 fade) { gabi::call(0x0280C320, s, fade, v); }

/* 0202204C JAIZelBasic::bgmStart(u32 id, u32 fade, s32 keepSub). HD: the bgm ids 0x80000105..0x80000152
 * are variants (track mute sets 1..3, tempo) of the base bgms; starting a variant of the bgm already
 * playing only switches the track mutes (or the tempo). */
void bgmStart(u32 self, u32 id, u32 fade, u32 keepSub) {
    WWHD_FUNC(0x0202204C, void, self, id, fade, keepSub);
    st8(self + 0xCE, 0);
    if (ld8(self + 0x73) != 0) return;
    if (ld8(self + 0x76) != 0) {
        if (id != 0x8000001E) return;
    } else {
        if (id == 0x80000800) return;
        if (id == 0) return;
        if (id == 0xFFFFFFFF) return;
        if (id == 0xC0000000) return;
    }
    const u32 orig = id;
    if (ld(self + 0x88) == 0x8000000F && id == 0x80000007) return;
    /* map the variant to its base bgm; c5: 1 = keep the sub bgm, 0 = clear, 2 = unchanged */
    int c5 = 0;
    switch (id) {
    case 0x8000000A: st8(self + 0xDA, 1); c5 = 2; break;
    case 0x80000007: st(self + 0x298, id); c5 = 2; break;
    case 0x80000003:
    case 0x80000010: c5 = 1; break;
    case 0x80000005:
    case 0x80000105: id = 0x80000005; c5 = 1; break;
    case 0x80000014:
    case 0x80000110:
    case 0x80000111:
    case 0x80000112: id = 0x80000014; c5 = 1; break;
    case 0x80000015:
    case 0x80000115: id = 0x80000015; c5 = 1; break;
    case 0x80000023:
    case 0x80000120:
    case 0x80000121:
    case 0x80000122:
    case 0x80000123: id = 0x80000023; c5 = 1; break;
    case 0x80000029:
    case 0x80000124: id = 0x80000029; c5 = 1; break;
    case 0x80000028:
    case 0x80000125:
    case 0x80000126: id = 0x80000028; c5 = 2; break;
    case 0x80000042:
    case 0x80000130:
    case 0x80000131:
    case 0x80000132:
    case 0x80000133: id = 0x80000042; c5 = 2; break;
    case 0x80000048:
    case 0x80000140: id = 0x80000048; c5 = 1; break;
    case 0x80000049:
    case 0x80000150: id = 0x80000049; c5 = 1; break;
    case 0x8000004A:
    case 0x80000151: id = 0x8000004A; c5 = 1; break;
    case 0x8000004C:
    case 0x80000152: id = 0x8000004C; c5 = 1; break;
    case 0xC0000006: st8(self + 0x74, 1); c5 = 2; break;
    }
    if (c5 != 2) st8(self + 0xC5, c5);
    const f32 tempo = ldf(0x10003964); /* 1.3143 */
    u32 main = ld(self + 0x78);
    if (main != 0 && id == ld(self + 0x88)) {
        const u32 h = self + 0x78;
        switch (id) {
        case 0x80000005:
            if (orig == 0x80000105) bgmMute_l(self, h, id, 1, 5);
            else if (orig == 0x80000005) bgmMute_l(self, h, id, 0, 5);
            return;
        case 0x80000014:
            if (orig > 0x80000112) return;
            if (orig == 0x80000112) bgmMute_l(self, h, id, 3, 5);
            else if (orig == 0x80000111) bgmMute_l(self, h, id, 2, 5);
            else if (orig == 0x80000110) bgmMute_l(self, h, id, 1, 5);
            else if (orig == 0x80000014) bgmMute_l(self, h, id, 0, 5);
            return;
        case 0x80000015:
            if (orig == 0x80000115) bgmMute_l(self, h, id, 1, 5);
            else if (orig == 0x80000015) bgmMute_l(self, h, id, 0, 5);
            return;
        case 0x80000023:
            if (orig >= 0x80000121) {
                if (orig > 0x80000123) return;
                if (orig == 0x80000123) bgmMute_l(self, h, 0x80000022, 3, 5);
                else if (orig == 0x80000122) bgmMute_l(self, h, id, 3, 5);
                else bgmMute_l(self, h, id, 2, 5);
            } else if (orig == 0x80000120) {
                bgmMute_l(self, h, id, 1, 5);
            } else if (orig == 0x80000023) {
                bgmMute_l(self, h, id, 0, 5);
            }
            return;
        case 0x80000042:
            if (orig >= 0x80000131) {
                if (orig > 0x80000133) return;
                if (orig == 0x80000133) bgmMute_l(self, h, 0x80000043, 3, 0x3C);
                else if (orig == 0x80000132) bgmMute_l(self, h, 0x80000042, 3, 0x3C);
                else bgmMute_l(self, h, 0x80000042, 2, 0x3C);
            } else if (orig == 0x80000130) {
                bgmMute_l(self, h, 0x80000042, 1, 0x3C);
            } else if (orig == 0x80000042) {
                bgmMute_l(self, h, 0x80000042, 0, 0x3C);
            }
            return;
        case 0x80000029:
        case 0x80000048:
        case 0x80000049:
        case 0x8000004A:
        case 0x8000004C: {
            u32 var = id == 0x80000029 ? 0x80000124 : id == 0x80000048 ? 0x80000140 : id == 0x80000049 ? 0x80000150
                    : id == 0x8000004A ? 0x80000151 : 0x80000152;
            if (orig == var) bgmMute_l(self, h, id, 1, 5);
            else if (orig == id) bgmMute_l(self, h, id, 0, 5);
            return;
        }
        case 0x80000028:
            if (orig == 0x80000126) {
                bgmMute_l(self, h, 0x80000028, 2, 0x3C);
                JAISound_setTempoProportion_l(ld(self + 0x78), 1.0f, 0x3C);
            } else if (orig == 0x80000125) {
                bgmMute_l(self, h, 0x80000028, 1, 0x3C);
                JAISound_setTempoProportion_l(ld(self + 0x78), tempo, 0x3C);
            } else if (orig == 0x80000028) {
                bgmMute_l(self, h, 0x80000028, 0, 0x3C);
                JAISound_setTempoProportion_l(ld(self + 0x78), 1.0f, 0x3C);
            }
            return;
        }
    }
    /* start the new bgm */
    bgmStop_l(self, 0, keepSub);
    JAIBasic_startSoundVec_l(self, id, self + 0x78, 0, fade, 0, 4);
    if ((s32)ld(self + 0x294) == 0x12 && ld8(self + 0x2A8) == 0x29) {
        /* one room: the bgm fades in near the point (195000, 198000) */
        u32 cp = ld(ld(self));
        if (cp != 0) {
            f32 dz = ldf(cp + 8) - 198000.0f;
            f32 dx = ldf(cp) - 195000.0f;
            f32 d = sqrtf_l(gabi::fmadds(dx, dx, dz * dz));
            f32 v = 1.0f;
            if (d > 8000.0f) {
            } else if (d > 5000.0f) {
                v = (d - 5000.0f) / 3000.0f;
            } else {
                v = 0.0f;
            }
            if (ld(self + 0x88) == 0x8000000A) v = 1.0f;
            stf(self + 0xA0, v);
        }
    }
    if (id == 0x8000000A) {
        stf(self + 0xA0, 1.0f);
        stf(self + 0x9C, 1.0f);
    } else {
        f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
        v = mul(v, ldf(self + 0x98));
        v = mul(v, ldf(self + 0x9C));
        v = mul(v, ldf(self + 0xA0));
        v = mul(v, ldf(self + 0xA4));
        v = mul(v, ldf(self + 0xA8));
        v = mul(v, ldf(self + 0xAC));
        v = mul(v, ldf(self + 0xBC));
        if (v != 1.0f && id != 0x80000051 && id != 0x80000052) {
            u32 m = ld(self + 0x78);
            if (m != 0) JAISound_setVolume_l(m, v, 0, 0);
        }
    }
    /* the variant's track mutes / tempo */
    const u32 h = self + 0x78;
    switch (orig) {
    case 0x80000005:
    case 0x80000014:
    case 0x80000015:
    case 0x80000023:
    case 0x80000042:
    case 0x80000048:
    case 0x80000049:
    case 0x8000004A:
    case 0x8000004C:
        bgmMute_l(self, h, id, 0, 0);
        break;
    case 0x80000028:
    case 0x80000029:
        bgmMute_l(self, h, id, 0, 0);
        break;
    case 0x8000002E:
    case 0x8000003C:
        if (ld8(self + 0x1FAD) == 0 && ldf(self + 0x1FB0) < 0.2f) bgmMute_l(self, h, orig, 1, 0);
        else bgmMute_l(self, h, orig, 0, 0);
        break;
    case 0x80000105:
    case 0x80000110:
    case 0x80000115:
    case 0x80000120:
    case 0x80000124:
    case 0x80000130:
    case 0x80000140:
    case 0x80000150:
    case 0x80000151:
    case 0x80000152:
        bgmMute_l(self, h, id, 1, 0);
        break;
    case 0x80000111:
    case 0x80000121:
    case 0x80000126:
    case 0x80000131:
        bgmMute_l(self, h, id, 2, 0);
        break;
    case 0x80000112:
    case 0x80000122:
    case 0x80000132:
        bgmMute_l(self, h, id, 3, 0);
        break;
    case 0x80000123:
        bgmMute_l(self, h, id - 1, 3, 0);
        break;
    case 0x80000133:
        bgmMute_l(self, h, id + 1, 3, 0);
        break;
    case 0x80000125: {
        bgmMute_l(self, h, id, 1, 0);
        u32 m = ld(self + 0x78);
        if (m != 0) JAISound_setTempoProportion_l(m, tempo, 0);
        break;
    }
    }
    st(self + 0x88, id);
    if (id == 0x80000038 || id == 0x80000001 || id == 0x8000000E) st8(self + 0x1FAC, 1);
}
VERIFY(0x0202204C, bgmStart);

} // namespace JAIZelBasic_4_cpp
