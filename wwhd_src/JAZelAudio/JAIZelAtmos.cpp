/* JAIZelAtmos (WWHD): Zelda's ambient sound positions (sea, shore, river, waterfall, window rain),
 * methods of JAIZelBasic. The GameCube decompilation
 * (zeldaret/tww src/JAZelAudio/JAIZelAtmos.cpp) has only "Nonmatching" stubs here apart from the
 * three init functions, so the bodies are written from the WWHD code and verified against cking.rpx.
 *
 * Translation unit 0201C0E8..0201D413 (17 functions), from the image: JAIZelAnime ends with its
 * header __sinit (0201C054); JAIZelAtmos runs calcPosPanLR .. rainPlay (HD order: the calcPos*
 * helpers first), its header __sinit (0201D368, rodata 10003820 after the unit's constants
 * 100037D0..1000381C), then GHS's trailing per-TU copies: a deleting destructor (0201D3FC) and an
 * empty function (0201D410, called on a SafeString in seaShoreSE). JAIZelBasic follows (0201D414).
 *
 * Layout (HD, JAIZelBasic): +0 audio camera array (camera 0: +0 position pointer, +8 view matrix),
 * +0x30 (GC 0x20) mute flag, +0x64 (GC 0x54) a position, +0xA8, +0x268/+0x271/+0x274/+0x276/+0x277
 * state flags, +0x288 (GC 0x218), +0x28B, +0x294 (GC 0x224) scene number, +0x29D;
 * sea env positions [64] at +0x15F0, their camera-space copies at +0x18F0, count at +0x1BF0
 * (GC 0x1B80); shore position +0x1BF4; river positions [64] at +0x1C00, count +0x1E40 (GC 0x1DD0),
 * flag +0x1E44; waterfall positions [16] at +0x1E48, count +0x1F08; window positions [3] at
 * +0x1F0C, count +0x1F30 (GC 0x1EC0); sound handles +0x1F34/+0x1F38/+0x1F3C (sea), +0x1F40 (shore). */
#include "bindings.h"

namespace JAIZelAtmos_cpp {
#include "jaizel_local.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline f32 JAIGlobalParameter_getParamMaxVolumeDistance_l() { return gabi::call<f32>(0x02804724); }
static inline f32 JAIGlobalParameter_getParamMinDistanceVolume_l() { return gabi::call<f32>(0x02804730); }
static inline f32 JAIGlobalParameter_getParamSeDolbyCenterValue_l() { return gabi::call<f32>(0x02804754); }
static inline f32 JAIGlobalParameter_getParamSeDolbyFrontDistanceMax_l() { return gabi::call<f32>(0x02804760); }
static inline f32 JAIGlobalParameter_getParamSeDolbyBehindDistanceMax_l() { return gabi::call<f32>(0x0280476C); }
static inline void JAIBasic_startSoundVec_l(u32 b, u32 id, u32 handle, u32 pos, u32 a, u32 c, u32 d) { gabi::call(0x02802848, b, id, handle, pos, a, c, d); }
static inline void JAISound_setPan_l(u32 s, f32 v, u32 t, u32 k) { gabi::call(0x0280BA28, s, t, k, v); }
static inline void JAISound_setDolby_l(u32 s, f32 v, u32 t, u32 k) { gabi::call(0x0280C248, s, t, k, v); }
/* 020233C8 (named checkPlayingStreamBgmFlag): HD stream check through the HD sound manager, with an id */
static inline s32 JAIZelBasic_checkStream020233C8_l(u32 b, u32 id) { return gabi::call<s32>(0x020233C8, b, id); }

static inline f32 calcPosPanLR(u32 self, u32 pos);
static inline f32 calcPosPanSR(u32 self, u32 pos, f32 scale);
static inline f32 calcPosVolume(u32 self, u32 pos, f32 scale);
static inline s32 registSeaEnvPos(u32 self, u32 pos);
static inline s32 registRiverPos(u32 self, u32 pos);

struct SafeString_l { be<u32> mStringTop; be<u32> __vtbl; };

/* 0201C0E8 JAIZelBasic::calcPosPanLR(Vec*): 0 (left) .. 1 (right) */
static inline f32 calcPosPanLR(u32 self, u32 pos) {
    WWHD_FUNC(0x0201C0E8, f32, self, pos);
    f32 x = ldf(pos);
    if (x == 0.0f) return 0.5f;
    f32 z = ldf(pos + 8);
    if (z == 0.0f) {
        if (x > 0.0f) return 1.0f;
        if (x < 0.0f) return 0.0f;
    }
    f32 r = x / sqrtf_l(gabi::fmadds(x, x, z * z));
    if (r > 1.0f) return (1.0f + 1.0f) * 0.5f;
    if (r < -1.0f) r = -1.0f;
    return (r + 1.0f) * 0.5f;
}
VERIFY(0x0201C0E8, calcPosPanLR);

/* 0201C228 JAIZelBasic::calcPosPanSR(Vec*, f32): surround (dolby) position */
static inline f32 calcPosPanSR(u32 self, u32 pos, f32 scale) {
    WWHD_FUNC(0x0201C228, f32, self, pos, scale);
    f32 center = JAIGlobalParameter_getParamSeDolbyCenterValue_l() / 127.0f;
    f32 rest = 1.0f - center;
    f32 front = JAIGlobalParameter_getParamSeDolbyFrontDistanceMax_l() * scale;
    if (ldf(pos + 8) < front) return 0.0f;
    f32 z = ldf(pos + 8);
    if (z < 0.0f) {
        f32 a = JAIGlobalParameter_getParamSeDolbyFrontDistanceMax_l();
        f32 num = center * (a - ldf(pos + 8));
        return num / JAIGlobalParameter_getParamSeDolbyFrontDistanceMax_l();
    }
    if (!(z < JAIGlobalParameter_getParamSeDolbyBehindDistanceMax_l() * scale)) return 1.0f;
    f32 behind = JAIGlobalParameter_getParamSeDolbyBehindDistanceMax_l() * scale;
    return gabi::fmadds(rest, ldf(pos + 8) / behind, center);
}
VERIFY(0x0201C228, calcPosPanSR);

/* 0201C360 JAIZelBasic::calcPosVolume(Vec*, f32) */
static inline f32 calcPosVolume(u32 self, u32 pos, f32 scale) {
    WWHD_FUNC(0x0201C360, f32, self, pos, scale);
    if (pos == 0) return 0.0f;
    f32 y = ldf(pos + 4), x = ldf(pos), z = ldf(pos + 8);
    f32 dist = sqrtf_l(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)));
    if (dist < JAIGlobalParameter_getParamMaxVolumeDistance_l()) return 1.0f;
    f32 d = dist - JAIGlobalParameter_getParamMaxVolumeDistance_l();
    f32 range = JAIGlobalParameter_getParamDistanceMax_l();
    range = (range - JAIGlobalParameter_getParamMaxVolumeDistance_l()) * scale;
    if (!(d < range)) return 0.0f;
    f32 minVol = JAIGlobalParameter_getParamMinDistanceVolume_l();
    f32 minVol2 = JAIGlobalParameter_getParamMinDistanceVolume_l();
    return gabi::fmadds(1.0f - minVol2, 1.0f - d / range, minVol);
}
VERIFY(0x0201C360, calcPosVolume);

/* 0201C4B8 JAIZelBasic::initSeaEnvPos() */
void initSeaEnvPos(u32 self) {
    WWHD_FUNC(0x0201C4B8, void, self);
    st(self + 0x1BF0, 0);
}
VERIFY(0x0201C4B8, initSeaEnvPos);

/* 0201C4C4 JAIZelBasic::registSeaEnvPos(Vec*): new count, or -1 (none / full) */
static inline s32 registSeaEnvPos(u32 self, u32 pos) {
    WWHD_FUNC(0x0201C4C4, s32, self, pos);
    if (pos == 0) return -1;
    u32 dst = self + ld(self + 0x1BF0) * 12;
    st(dst + 0x15F0, ld(pos));
    st(dst + 0x15F4, ld(pos + 4));
    st(dst + 0x15F8, ld(pos + 8));
    s32 n = ld(self + 0x1BF0) + 1;
    if (n >= 0x40) {
        st(self + 0x1BF0, 0x3F);
        return -1;
    }
    st(self + 0x1BF0, n);
    return n;
}
VERIFY(0x0201C4C4, registSeaEnvPos);

/* 0201C524 JAIZelBasic::seaEnvSePlay(u32 type, s8 reverb): three sea layers (left, right, surround) */
void seaEnvSePlay(u32 self, u32 type, u32 reverb) {
    WWHD_FUNC(0x0201C524, void, self, type, reverb);
    if (ld8(self + 0x268) != 0) return;
    if (ld8(self + 0x274) == 1) return;
    if (ld8(self + 0x271) == 1) return;
    if (ld8(self + 0x276) == 1) return;
    if (ld8(self + 0x28B) == 0x14) {
        /* four points 5 units around the camera, at height 0 */
        u32 cpos = ld(ld(self));
        f32 x = ldf(cpos), z = ldf(cpos + 8);
        gabi::Local<Vec_l> v0, v1, v2, v3;
        u32 a = gabi::ea(v0.get()), b = gabi::ea(v1.get()), c = gabi::ea(v2.get()), d = gabi::ea(v3.get());
        stf(a, x - 5.0f); stf(a + 4, 0.0f); stf(a + 8, z - 5.0f);
        stf(b, x - 5.0f); stf(b + 4, 0.0f); stf(b + 8, z + 5.0f);
        stf(c, x + 5.0f); stf(c + 4, 0.0f); stf(c + 8, z - 5.0f);
        stf(d, x + 5.0f); stf(d + 4, 0.0f); stf(d + 8, z + 5.0f);
        registSeaEnvPos(self, a);
        registSeaEnvPos(self, b);
        registSeaEnvPos(self, c);
        registSeaEnvPos(self, d);
    }
    f32 left = 0.0f, right = 0.0f, surround = 0.0f;
    if (ld(self + 0x1BF0) == 0) return;
    u32 mtx = ld(ld(self) + 8);
    for (s32 i = 0; i < (s32)ld(self + 0x1BF0); i++) {
        u32 out = self + 0x18F0 + i * 12;
        PSMTXMultVec_l(mtx, self + 0x15F0 + i * 12, out);
        f32 vol = calcPosVolume(self, out, type < 8 ? 3.0f : 1.0f);
        f32 sr = calcPosPanSR(self, out, 1.0f);
        f32 lr = calcPosPanLR(self, out);
        f32 s = sr * vol;
        f32 l = (1.0f - lr) * vol;
        f32 r = lr * vol;
        if (l > left) left = l;
        if (r > right) right = r;
        if (s > surround) surround = s;
    }
    if (left > 1.0f) left = 1.0f;
    if (right > 1.0f) right = 1.0f;
    if (surround > 1.0f) surround = 1.0f;
    if (ld8(self + 0x30) != 0) {
        left = left * 0.33f;
        surround = surround * 0.33f;
        right = right * 0.33f;
    }
    if (JAIZelBasic_checkStreamPlaying_l(self, 0xC0000023)) {
        left = left * 0.7f;
        surround = surround * 0.7f;
        right = right * 0.7f;
    }
    u32 id0, id1, id2;
    if (type == 8) {
        id0 = 0x7006; id1 = 0x7007; id2 = 0x7008;
    } else {
        id0 = 0x7000; id1 = 0x7001; id2 = 0x7002;
    }
    if (left != 0.0f) {
        JAIBasic_startSoundVec_l(self, id0, self + 0x1F34, 0, 0, 0, 4);
        if (ld(self + 0x1F34) != 0) {
            JAISound_setPortData_l(ld(self + 0x1F34), 9, reverb & 0xFFFF);
            JAISound_setPan_l(ld(self + 0x1F34), 1.0f, 0, 0);
            JAISound_setDolby_l(ld(self + 0x1F34), 0.0f, 0, 0);
            JAISound_setVolume_l(ld(self + 0x1F34), left, 0, 0);
        }
    }
    if (right != 0.0f) {
        JAIBasic_startSoundVec_l(self, id1, self + 0x1F38, 0, 0, 0, 4);
        if (ld(self + 0x1F38) != 0) {
            JAISound_setPortData_l(ld(self + 0x1F38), 9, reverb & 0xFFFF);
            JAISound_setPan_l(ld(self + 0x1F38), 0.0f, 0, 0);
            JAISound_setDolby_l(ld(self + 0x1F38), 0.0f, 0, 0);
            JAISound_setVolume_l(ld(self + 0x1F38), right, 0, 0);
        }
    }
    if (surround != 0.0f) {
        JAIBasic_startSoundVec_l(self, id2, self + 0x1F3C, 0, 0, 0, 4);
        if (ld(self + 0x1F3C) != 0) {
            JAISound_setPortData_l(ld(self + 0x1F3C), 9, reverb & 0xFFFF);
            JAISound_setPan_l(ld(self + 0x1F3C), 0.5f, 0, 0);
            JAISound_setDolby_l(ld(self + 0x1F3C), 1.0f, 0, 0);
            JAISound_setVolume_l(ld(self + 0x1F3C), surround, 0, 0);
        }
    }
}
VERIFY(0x0201C524, seaEnvSePlay);

/* 0201D410: an empty per-TU copy (called on the left SafeString before the comparison) */
void atmos_empty_0201D410(u32 a) {
    WWHD_FUNC(0x0201D410, void, a);
}
VERIFY(0x0201D410, atmos_empty_0201D410);

/* sead::SafeString == (HD inline): the empty helper and cstr() (vtable slot 0x14) on the left
 * operand, cstr() on the right one, pointer compare, then a bounded strcmp */
static inline bool safestring_eq(u32 lit, u32 other, u32 vtbl) {
    gabi::Local<SafeString_l> sa;
    gabi::Local<SafeString_l> sb;
    sa->mStringTop = lit;
    sa->__vtbl = vtbl;
    sb->mStringTop = other;
    sb->__vtbl = vtbl;
    u32 a = gabi::ea(sa.get()), b = gabi::ea(sb.get());
    atmos_empty_0201D410(a);
    gabi::call_ptr(ld(ld(a + 4) + 0x14), a);
    u32 vb = ld(ld(b + 4) + 0x14);
    u32 pa = ld(a);
    gabi::call_ptr(vb, b);
    if (pa == ld(b)) return true;
    pa = ld(a);
    u32 pb = ld(b);
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = ld8(pa + i);
        if (ca != ld8(pb + i)) return false;
        if (ca == 0) return true;
    }
    return false;
}

/* 0201CA10 JAIZelBasic::seaShoreSE(u32 type, Vec* pos, u32 keep, s8 reverb) */
void seaShoreSE(u32 self, u32 type, u32 pos, u32 keep, u32 reverb) {
    WWHD_FUNC(0x0201CA10, void, self, type, pos, keep, reverb);
    if (ld8(self + 0x271) == 1) return;
    if (ld8(self + 0x276) == 1) return;
    u32 id;
    if (type == 1) id = 0x7004;
    else if (type == 2) id = 0x7005;
    else id = 0x7003;
    if (ld8(self + 0x29D) == 0) return;
    st(self + 0x1BF4, ld(pos));
    st(self + 0x1BF8, ld(pos + 4));
    st(self + 0x1BFC, ld(pos + 8));
    if (keep == 0 && ld(self + 0x1F40) != 0) JAISound_stop_l(ld(self + 0x1F40), 1);
    JAIBasic_startSoundVec_l(self, id, self + 0x1F40, self + 0x1BF4, 0, 0, 4);
    if (ld(self + 0x1F40) == 0) return;
    JAISound_setPortData_l(ld(self + 0x1F40), 9, reverb & 0xFFFF);
    if (ld8(self + 0x30) != 0) {
        JAISound_setVolume_l(ld(self + 0x1F40), 0.33f, 0, 0);
        return;
    }
    /* the current stage name (1047E6B8) against the literal at 10003814 */
    if (safestring_eq(0x10003814, 0x1047E6B8, 0x100037F8) && ld(0x101D6010) == 1) {
        JAISound_setVolume_l(ld(self + 0x1F40), 0.5f, 0, 0);
        return;
    }
    if (ldf(self + 0xA8) == 0.0f) JAISound_setVolume_l(ld(self + 0x1F40), 0.667f, 0, 0);
}
VERIFY(0x0201CA10, seaShoreSE);

/* 0201CC44 JAIZelBasic::initRiverPos() */
void initRiverPos(u32 self) {
    WWHD_FUNC(0x0201CC44, void, self);
    st(self + 0x1E40, 0);
}
VERIFY(0x0201CC44, initRiverPos);

/* 0201CC50 JAIZelBasic::registRiverPos(Vec*): only positions within 3x the sound distance (camera space)
 * are kept; returns the new count, the old count when too far, or -1 (none / full: count reset to 47) */
static inline s32 registRiverPos(u32 self, u32 pos) {
    WWHD_FUNC(0x0201CC50, s32, self, pos);
    if (pos == 0) return -1;
    u32 mtx = ld(ld(self) + 8);
    gabi::Local<Vec_l> v;
    u32 sp = gabi::ea(v.get());
    f32 y = ldf(pos + 4);
    stf(sp + 4, y);
    f32 z = ldf(pos + 8), x = ldf(pos);
    stf(sp + 8, z);
    stf(sp, x);
    if (mtx != 0) {
        PSMTXMultVec_l(mtx, sp, sp);
        y = ldf(sp + 4); x = ldf(sp); z = ldf(sp + 8);
    }
    f32 dist = sqrtf_l(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)));
    f32 max = JAIGlobalParameter_getParamDistanceMax_l();
    s32 n = ld(self + 0x1E40);
    if (dist > max * 3.0f) return n;
    u32 dst = self + n * 12;
    st(dst + 0x1C00, ld(pos));
    st(dst + 0x1C04, ld(pos + 4));
    st(dst + 0x1C08, ld(pos + 8));
    n = ld(self + 0x1E40) + 1;
    if (n >= 0x40) {
        st(self + 0x1E40, 0x2F);
        return -1;
    }
    st(self + 0x1E40, n);
    return n;
}
VERIFY(0x0201CC50, registRiverPos);

/* 0201CDA0 JAIZelBasic::riverSePlay(u8 type, s8 reverb) */
void riverSePlay(u32 self, u32 type, u32 reverb) {
    WWHD_FUNC(0x0201CDA0, void, self, type, reverb);
    if (ld(self + 0x1E40) == 0) return;
    if (ld8(self + 0x268) != 0) return;
    if (ld8(self + 0x271) == 1) return;
    if (ld8(self + 0x276) == 1) return;
    if ((s32)ld(self + 0x288) == 0x18) registRiverPos(self, self + 0x64);
    u32 id;
    switch (type) {
    case 0: id = 0x3033; break;
    case 1: id = 0x3034; break;
    case 2:
        if (ld8(self + 0x1E44) == 0) return;
        id = 0x7035;
        break;
    case 4:
    case 6: id = 0x303A; break;
    default: return;
    }
    for (s32 i = 0; i < (s32)ld(self + 0x1E40); i++)
        JAIZelBasic_seStart_l(self, id, self + 0x1C00 + i * 12, 0, reverb, 1.0f, 1.0f, -1.0f, -1.0f, 0);
}
VERIFY(0x0201CDA0, riverSePlay);

/* 0201CF54 JAIZelBasic::waterfallSePlay(u8 type, Vec* pos, s8 reverb) */
void waterfallSePlay(u32 self, u32 type, u32 pos, u32 reverb) {
    WWHD_FUNC(0x0201CF54, void, self, type, pos, reverb);
    u32 id;
    if (type == 0) id = 0x1067;
    else if (type == 2 || type == 4) id = 0x107C;
    else id = (s32)ld(self + 0x294) == 0x39 ? 0x10A7 : 0x1066;
    u32 mtx = ld(ld(self) + 8);
    gabi::Local<Vec_l> v;
    u32 sp = gabi::ea(v.get());
    f32 y = ldf(pos + 4);
    stf(sp + 4, y);
    f32 z = ldf(pos + 8), x = ldf(pos);
    stf(sp + 8, z);
    stf(sp, x);
    if (mtx != 0) {
        PSMTXMultVec_l(mtx, sp, sp);
        y = ldf(sp + 4); x = ldf(sp); z = ldf(sp + 8);
    }
    f32 dist = sqrtf_l(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)));
    if (dist > JAIGlobalParameter_getParamDistanceMax_l() * 4.0f) return;
    u32 dst = self + 0x1E48 + ld(self + 0x1F08) * 12;
    st(dst, ld(pos));
    st(dst + 4, ld(pos + 4));
    st(dst + 8, ld(pos + 8));
    if (ld8(self + 0x277) == 0 && !JAIZelBasic_checkStream020233C8_l(self, 0xC000003C) && ld8(self + 0x277) == 0)
        JAIZelBasic_seStart_l(self, id, self + 0x1E48 + ld(self + 0x1F08) * 12, 0, reverb, 1.0f, 1.0f, -1.0f, -1.0f, 0);
    s32 n = ld(self + 0x1F08);
    if (n < 0xF) st(self + 0x1F08, n + 1);
}
VERIFY(0x0201CF54, waterfallSePlay);

/* 0201D140 JAIZelBasic::initWindowPos() */
void initWindowPos(u32 self) {
    WWHD_FUNC(0x0201D140, void, self);
    st(self + 0x1F30, 0);
}
VERIFY(0x0201D140, initWindowPos);

/* 0201D14C JAIZelBasic::registWindowPos(Vec*) */
s32 registWindowPos(u32 self, u32 pos) {
    WWHD_FUNC(0x0201D14C, s32, self, pos);
    if (pos == 0) return -1;
    u32 dst = self + ld(self + 0x1F30) * 12;
    st(dst + 0x1F0C, ld(pos));
    st(dst + 0x1F10, ld(pos + 4));
    st(dst + 0x1F14, ld(pos + 8));
    s32 n = ld(self + 0x1F30) + 1;
    if (n >= 3) {
        st(self + 0x1F30, 2);
        return -1;
    }
    st(self + 0x1F30, n);
    return n;
}
VERIFY(0x0201D14C, registWindowPos);

/* 0201D1AC JAIZelBasic::rainPlay(s32 type): without window positions a plain rain sound
 * (HD: scene 0x5C uses 0x10A8 for heavy rain), else one sound per window */
void rainPlay(u32 self, s32 type) {
    WWHD_FUNC(0x0201D1AC, void, self, type);
    if (ld(self + 0x1F30) == 0) {
        if (type == 1) {
            if ((s32)ld(self + 0x294) == 0x5C) JAIZelBasic_seStart_l(self, 0x10A8, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f, 0);
            else JAIZelBasic_seStart_l(self, 0x1085, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f, 0);
        } else {
            JAIZelBasic_seStart_l(self, 0x105B, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f, 0);
        }
        return;
    }
    for (s32 i = 0; i < (s32)ld(self + 0x1F30); i++)
        JAIZelBasic_seStart_l(self, type == 1 ? 0x1086 : 0x1084, self + 0x1F0C + i * 12, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f, 0);
}
VERIFY(0x0201D1AC, rainPlay);

/* 0201D368 __sinit_JAIZelAtmos_cpp (header statics) */
void __sinit_JAIZelAtmos_cpp() {
    WWHD_FUNC(0x0201D368, void);
    header_sinit(0x101FFC40, 0x1018D4CC, 0x10003820);
}
VERIFY(0x0201D368, __sinit_JAIZelAtmos_cpp);

/* 0201D3FC: a per-TU deleting destructor of a class without a destructor body */
void atmos_deleting_dtor_0201D3FC(u32 self, u32 flags) {
    WWHD_FUNC(0x0201D3FC, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    gabi::call(0x0273AF40, self); /* __dl */
}
VERIFY(0x0201D3FC, atmos_deleting_dtor_0201D3FC);

} // namespace JAIZelAtmos_cpp
