/* hd_olv_02036E58: the HD Miiverse bottle-spot manager translation unit (02036E58..02038497). No GameCube
 * source: HD-only code.
 *
 * A sead singleton (0x30D4: vtable +0, IDisposer +4, 50 spot lists of 0xF4 at +0x14 (up to 20 positions
 * of 12 bytes, count +0xF0), spawned bottle positions (12 bytes each) at +0x2FBC, spawn count +0x30AC,
 * flags +0x30B0..+0x30B3, distance parameters +0x30B4..+0x30CC, debug counter +0x30D0). It loads the
 * per-room spot lists ("MiiverseBottleSpotRoom%02d.dat" in the Misc archive), spawns the Miiverse
 * bottle actors (profile 0x3FF7F14 / 0x3BFF7F14) either on random listed spots of the room or at a
 * random position around Link on the sea (11 tries inside the current 100000-unit sea sector, ground
 * and four line checks; the water height from the wave or the water check), and draws the spawn
 * positions in a debug view. */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_olv_02036E58 {

static constexpr u32 SS_VT = 0x10005298; /* this TU's SafeString vtable */
static constexpr u32 INST = 0x1018F450, DISP = 0x1018F454;
static constexpr u32 COMMENTMGR = 0x1018F4AC;
static constexpr u32 PSVECAdd = 0x028E8D88, PSVECSquareMag = 0x028E8DD0, cXyz_mi = 0x0201ADE0;
static inline u32 dComIfGp() { return gabi::call<u32>(0x025200D4); }
static inline f32 i2f(s32 v) { return (f32)(f64)v; }
static inline f32 u2f(u32 v) { return (f32)(f64)v; }

/* 02036E58: constructor (0x30D4; the IDisposer at +4 is built by createInstance) */
static u32 SpotMgr_ct(u32 self) {
    WWHD_FUNC(0x02036E58, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x30D4);
        if (t == 0) return 0;
    }
    st(t + 0, 0x10005418);
    gabi::call(0x028EFFD0, t + 0x14, 0x32u, 0xF4u, 0x0203857C);
    f32 z = ldf(0x10005450);
    stb(t + 0x30B0, 0);
    stf(t + 0x30B4, z);
    stb(t + 0x30D0, 0);
    stf(t + 0x30B8, z);
    stb(t + 0x30AC, 0);
    stf(t + 0x30BC, z);
    stb(t + 0x30B2, 0);
    stb(t + 0x30B1, 0);
    stb(t + 0x30B3, 0);
    return t;
}
VERIFY(0x02036E58, SpotMgr_ct);

/* 02036EEC: sead singleton createInstance(heap) (instance 1018F450, disposer +4 at 1018F454) */
static u32 SpotMgr_createInstance(u32 heap) {
    WWHD_FUNC(0x02036EEC, u32, heap);
    u32 cur = ld(INST);
    if (cur != 0) return cur;
    u32 d = gabi::call<u32>(0x0273B0D4, 0x30D4u, heap, 4u);
    u32 disp = d + 4;
    if (disp != 0) {
        gabi::call(0x02752B0C, disp, heap, 3u);
        st(disp + 0xC, 0x100054D8);
    }
    st(DISP, disp);
    u32 r = d;
    if (d != 0) r = gabi::call<u32>(0x02036E58, d);
    st(INST, r);
    return r;
}
VERIFY(0x02036EEC, SpotMgr_createInstance);

/* 02036F8C: distance parameters */
static void SpotMgr_setParams(u32 self) {
    WWHD_FUNC(0x02036F8C, void, self);
    f32 a = ldf(0x1000545C), b = ldf(0x10005460), c = ldf(0x10005464), d = ldf(0x10005454), e = ldf(0x10005458);
    stf(self + 0x30C0, a);
    stf(self + 0x30C4, b);
    stf(self + 0x30CC, c);
    stf(self + 0x30B8, d);
    stf(self + 0x30BC, d);
    stf(self + 0x30C8, b);
    stf(self + 0x30B4, e);
}
VERIFY(0x02036F8C, SpotMgr_setParams);

/* 02036FD4: load the spot lists of rooms 1..49 from the Misc archive */
static void SpotMgr_load(u32 self) {
    WWHD_FUNC(0x02036FD4, void, self);
    gabi::Local<be<u32>[2]> s54;
    gabi::Local<be<u32>[2]> s5c;
    st(s54.a + 0, 0x10005468);
    st(s5c.a + 4, SS_VT);
    st(s54.a + 4, SS_VT);
    st(s5c.a + 0, 0x10005470);
    u32 res = gabi::call<u32>(0x026124B0, ld(0x101F4F7C), s54.a, s5c.a, 0u);
    if (res == 0) return;
    u32 arc = gabi::call<u32>(0x027E2DC0, res);
    /* FixedSafeString<0x40> {buffer, vtable, capacity} + buffer */
    struct fixed_l { be<u32> h[3]; u8 b[0x40]; };
    gabi::Local<fixed_l> nm;
    const u32 buf = nm.a + 0xC;
    for (u32 i = 1; i < 0x32; i++) {
        stb(buf, 0);
        stb(buf + 0x3F, 0);
        st(nm.a + 8, 0x40);
        st(nm.a + 0, buf);
        st(nm.a + 4, 0x10005300);
        gabi::cpu->cr[6] = 0;
        gabi::call(0x02759C28, nm.a, 0x1000547C, i);
        gabi::call_ptr<u32>(ld(ld(nm.a + 4) + 0x14), nm.a);
        u32 off = ld(arc + 0x4C);
        u32 base = off != 0 ? arc + 0x4C + off : 0;
        u32 f = gabi::call<u32>(0x027DFA24, base, ld(nm.a));
        if (f == 0) continue;
        u32 o = ld(f);
        u32 d = o != 0 ? f + o : 0;
        if (d == 0) continue;
        s32 n = (s32)ld(d);
        if (n > 0x14) continue;
        u32 e = self + i * 0xF4;
        stb(e + 0x104, (u32)n);
        for (s32 k = 0; k < n; k++) {
            u32 s = d + 4 + (u32)k * 0xC, t = e + 0x14 + (u32)k * 0xC;
            stf(t + 0, ldf(s + 0));
            stf(t + 4, ldf(s + 4));
            stf(t + 8, ldf(s + 8));
        }
    }
}
VERIFY(0x02036FD4, SpotMgr_load);

/* 0203714C: spawn a bottle on listed position POS of spot list SPOT */
static void SpotMgr_spawnAt(u32 self, u32 pos, u32 spot) {
    WWHD_FUNC(0x0203714C, void, self, pos, spot);
    u32 ap = gabi::call<u32>(0x025D5600); /* fopAcM_CreateAppend */
    u32 v = self + 0x14 + spot * 0xF4 + pos * 0xC;
    st(ap + 0, 0x03FF7F14);
    st(ap + 4, ld(v + 0));
    st(ap + 8, ld(v + 4));
    u32 vz = ld(v + 8);
    sth(ap + 0x14, 0xFF);
    st(ap + 0xC, vz);
    stb(ap + 0x21, spot);
    stb(ap + 0x20, 0xFF);
    stb(ap + 0x1B, 0);
    u32 layer = gabi::call<u32>(0x025DED64);
    gabi::call(0x025E14A8, layer, 0xFFu, 0u, 0u, ap);
    gabi::call(0x02039D5C, ld(COMMENTMGR));
    u32 k = lbz(self + 0x30AC);
    u32 dst = self + k * 0xC + 0x2FBC;
    st(dst + 0, ld(v + 0));
    st(dst + 4, ld(v + 4));
    st(dst + 8, ld(v + 8));
    stb(self + 0x30AC, lbz(self + 0x30AC) + 1);
}
VERIFY(0x0203714C, SpotMgr_spawnAt);


/* 02037230: spawn bottles on random listed spots of room SPOT: a quarter of the room's list, at most
 * the number of ready Miiverse posts (the whole list with the debug flag +0x30B3) */
static void SpotMgr_spawnListed(u32 self, u32 spot) {
    WWHD_FUNC(0x02037230, void, self, spot);
    if (spot == 0 || (s32)spot >= 0x32) return;
    if (gabi::call<u32>(0x02520C0C, 0x21u) == 0) return; /* dComIfGs_checkGetItem(Tingle Bottle) */
    s32 ready = (s32)gabi::call<u32>(0x02039A58, ld(COMMENTMGR));
    u32 dbg = lbz(self + 0x30B3);
    const u32 cntp = self + spot * 0xF4 + 0x104;
    u32 cnt;
    if (dbg == 0) {
        if (ready == 0) return;
        cnt = lbz(cntp);
        if (cnt == 0) return;
    } else {
        cnt = lbz(cntp);
    }
    s32 want = (s32)(cnt >> 2);
    if (want > ready) want = ready;
    if (dbg != 0) want = (s32)cnt;
    /* shuffle bag of the list indices */
    u32 arr = 0;
    s32 size = 0, filled = 0;
    u32 lim = cnt;
    if ((s32)cnt > 0) {
        u32 h = gabi::call<u32>(0x02756140, ld(0x101F8B4C));
        u32 p = gabi::call_ptr<u32>(vfn(h, 0xC, 0x34), h, cnt * 4, 4u);
        lim = lbz(cntp);
        if (p != 0) {
            size = (s32)cnt;
            arr = p;
        }
    }
    for (s32 i = 0; i < (s32)lim;) {
        if (filled < size) {
            s32 idx = filled;
            filled++;
            if (idx >= size) idx -= size;
            st(arr + (u32)idx * 4, (u32)i);
        }
        lim = lbz(cntp);
        i++;
    }
    for (s32 w = want; w > 0; w--) {
        u32 rnd = gabi::call<u32>(0x0275CEE4, ld(0x101F8B60));
        s32 j = (s32)(u32)(((u64)rnd * (u32)filled) >> 32);
        if ((u32)j < (u32)filled) {
            s32 idx = j;
            if (idx >= size) idx -= size;
            gabi::call(0x0203714C, self, ld(arr + (u32)idx * 4), spot);
        } else {
            gabi::call(0x0203714C, self, ld(arr), spot);
        }
        if (filled <= 0) continue;
        if (j < 0 || j >= filled) continue;
        filled--;
        if (j >= filled) continue;
        do {
            s32 a = j;
            if (a >= size) a -= size;
            s32 b = j + 1;
            if (b >= size) b -= size;
            j++;
            st(arr + (u32)a * 4, ld(arr + (u32)b * 4));
        } while (j < filled);
    }
    if (arr != 0) {
        u32 h = gabi::call<u32>(0x02755FEC, ld(0x101F8B4C), arr);
        gabi::call_ptr<u32>(vfn(h, 0xC, 0x3C), h, arr);
    }
}
VERIFY(0x02037230, SpotMgr_spawnListed);

/* 02037480: random sea position around Link (POS out: x +0, z +8): a random radius ahead/behind and a
 * random side offset along Link's facing, up to 11 tries until it lies inside Link's current sea
 * sector (100000 units, margin +0x30BC); +0x30B2 flags failure. Also measures the distances to the
 * room's listed spots (results unused). */
static void SpotMgr_randomPos(u32 self, u32 pl, u32 spot, u32 out) {
    WWHD_FUNC(0x02037480, void, self, pl, spot, out);
    u32 sel = lbz(self + 0x30AC) != 0 ? 1 : 0;
    f32 px = ldf(pl + 0x314);
    f32 c350k = ldf(0x100054A0);
    f32 pz = ldf(pl + 0x31C);
    f32 c100k = ldf(0x1000549C);
    s32 ix = gabi::ftoi((px + c350k) / c100k);
    f32 dmax = ldf(self + 0x30B4);
    s32 iz = gabi::ftoi((pz + c350k) / c100k);
    f32 dmin = ldf(self + 0x30B8);
    f32 cm300k = ldf(0x100054A4);
    f32 rmin = ldf(self + 0x30C0 + sel * 4);
    f32 py = ldf(pl + 0x318);
    f32 cx = gabi::fmadds(i2f(ix), c100k, cm300k);
    f32 rmax = ldf(self + 0x30C8 + sel * 4);
    f32 half = ldf(0x10005464);
    f32 rrange = rmax - rmin;
    f32 zero = ldf(0x10005450);
    f32 drange = dmax - dmin;
    f32 p5 = ldf(0x100054B4);
    f32 inv32 = ldf(0x100054B0);
    f32 cz = gabi::fmadds(i2f(iz), c100k, cm300k);
    const u32 tbl = 0x104A44F8; /* sin/cos table */
    gabi::Local<be<u32>[3]> vec;
    gabi::Local<be<u32>[3]> pos;
    gabi::Local<be<u32>[3]> d2;
    gabi::Local<be<u32>[3]> dif;
    bool found = false;
    for (u32 t = 11; t != 0; t--) {
        f32 r1 = u2f(gabi::call<u32>(0x0275CEE4, ld(0x101F8B60))) * inv32;
        f32 rad = gabi::fmadds(r1, rrange, rmin);
        f32 r2 = u2f(gabi::call<u32>(0x0275CEE4, ld(0x101F8B60))) * inv32;
        f32 side = r2 * drange;
        f32 r3 = u2f(gabi::call<u32>(0x0275CEE4, ld(0x101F8B60))) * inv32;
        u32 ang = lhz(pl + 0x32A);
        if (r3 > p5) side = -side;
        stf(pos.a + 0, px);
        stf(pos.a + 4, py);
        u32 o1 = ang & ~7u;
        stf(pos.a + 8, pz);
        stf(vec.a + 0, rad * ldf(tbl + o1));
        stf(vec.a + 4, zero);
        stf(vec.a + 8, rad * ldf(tbl + o1 + 4));
        gabi::call(PSVECAdd, pos.a, vec.a, pos.a);
        u32 o2 = ((ang + 0x4000) & 0xFFFF) & ~7u;
        stf(vec.a + 0, side * ldf(tbl + o2));
        stf(vec.a + 4, zero);
        stf(vec.a + 8, side * ldf(tbl + o2 + 4));
        gabi::call(PSVECAdd, pos.a, vec.a, pos.a);
        f32 m = half - ldf(self + 0x30BC);
        f32 x = ldf(pos.a + 0);
        if (x < cx - m) continue;
        if (cx + m < x) continue;
        f32 z = ldf(pos.a + 8);
        if (z < cz - m) continue;
        if (!(cz + m < z)) { /* bge: also taken for NaN */
            found = true;
            break;
        }
    }
    if (!found) stb(self + 0x30B2, 1);
    u32 e = self + spot * 0xF4;
    u32 v = e + 0x14;
    for (u32 i = 0; i < lbz(e + 0x104); i++, v += 0xC) {
        gabi::call(cXyz_mi, v, dif.a, pos.a);
        stf(d2.a + 0, ldf(dif.a + 0));
        stf(d2.a + 4, zero);
        stf(d2.a + 8, ldf(dif.a + 8));
        gabi::call<f32>(PSVECSquareMag, d2.a);
    }
    f32 ox = ldf(pos.a + 0);
    f32 oz = ldf(pos.a + 8);
    stf(out + 0, ox);
    stf(out + 8, oz);
}
VERIFY(0x02037480, SpotMgr_randomPos);

/* 020378B4: is POS free open sea? no ground below, then the water height into *OUT (the wave height in
 * the sea area, else the water check), and none of four short lines from POS hits anything */
static u32 SpotMgr_bgCheck(u32 self, u32 pos, u32 out) {
    WWHD_FUNC(0x020378B4, u32, self, pos, out);
    gabi::Local<be<u32>[0x38]> frame; /* the original's frame 0x00..0xDF (offsets below as in it) */
    const u32 F = frame.a;
    gabi::call(0x02008E0C, F + 0x20); /* cBgS_GndChk::cBgS_GndChk */
    f32 h = ldf(0x100054C0);
    f32 x = ldf(pos + 0), z = ldf(pos + 8);
    /* dBgS_GndChk parts (vtables, pass checks) */
    stf(F + 0x44, x);
    stb(F + 0x67, 0);
    stb(F + 0x65, 0);
    st(F + 0x20, F + 0x60);
    st(F + 0x6C, 0x10005358);
    st(F + 0x40, 0x10005348);
    stf(F + 0x4C, z);
    stb(F + 0x66, 0);
    stb(F + 0x6A, 0);
    st(F + 0x30, 0x10005338);
    stb(F + 0x64, 0);
    st(F + 0x70, 1);
    stb(F + 0x68, 0);
    st(F + 0x24, F + 0x6C);
    stf(F + 0x48, h);
    st(F + 0x60, 0x10005368);
    stb(F + 0x69, 0);
    f32 g = gabi::call<f32>(0x02008974, dComIfGp() + 0x12A0, F + 0x20); /* cBgS::GroundCross */
    f32 zero = ldf(0x10005450);
    if (g > zero) {
        st(F + 0x40, 0x10005348);
        st(F + 0x60, 0x10005368);
        st(F + 0x6C, 0x10005328);
        gabi::call(0x02008DAC, F + 0x20, 0u); /* ~cBgS_Chk */
        return 0;
    }
    if (gabi::call<u32>(0x0246B6A4, ldf(pos + 0), ldf(pos + 8)) != 0) { /* daSea_ChkArea */
        f32 wz = ldf(pos + 8);
        f32 wx = ldf(pos + 0);
        stf(out, gabi::call<f32>(0x0246BA0C, wx, wz)); /* daSea_calcWave */
    } else {
        const u32 W = 0x102009FC; /* function-local static dBgS_WtrChk */
        if (ld(0x10200A4C) == 0) {
            st(0x10200A4C, 1);
            gabi::call(0x024F22DC, W);
            gabi::call(0x028F026C, 0x1018F420);
        }
        f32 vz = ldf(pos + 8);
        f32 lo = ldf(0x100054C4);
        f32 vx = ldf(pos + 0);
        stf(W + 0x3C, lo);
        f32 hi = ldf(0x100054C8);
        stf(W + 0x40, vz);
        stf(W + 0x44, hi);
        stf(W + 0x38, vx);
        if (gabi::call<u32>(0x024EF7C0, dComIfGp() + 0x12A0, W) != 0) /* dBgS::SplGrpChk */
            stf(out, ldf(W + 0x48));
        else
            stf(out, zero);
    }
    gabi::call(0x02008FEC, F + 0x74); /* cBgS_LinChk::ct */
    st(F + 0x1C, ld(pos + 8));
    st(F + 0x18, ld(pos + 4));
    st(F + 0x14, ld(pos + 0));
    stb(F + 0xD0, 0);
    stb(F + 0xD1, 0);
    stb(F + 0xD2, 0);
    stb(F + 0xD3, 0);
    stb(F + 0xD4, 0);
    stb(F + 0xD5, 0);
    stb(F + 0xD6, 0);
    st(F + 0x74, F + 0xCC);
    st(F + 0x84, 0x100053D8);
    st(F + 0x94, 0x100053E8);
    st(F + 0xDC, 1);
    st(F + 0x78, F + 0xD8);
    st(F + 0xCC, 0x10005408);
    st(F + 0xD8, 0x100053F8);
    u32 ok = 1;
    for (u32 k = 0; k < 4; k++) {
        f32 dz = ldf(0x10005440 + k * 4);
        f32 dx = ldf(0x10005430 + k * 4);
        stf(F + 0xC, zero);
        stf(F + 0x10, dz);
        stf(F + 0x8, dx);
        gabi::call(PSVECAdd, F + 8, pos, F + 8);
        gabi::call(0x024F1AFC, F + 0x74, F + 8, F + 0x14, 0u);                  /* dBgS_LinChk::Set */
        if (gabi::call<u32>(0x02008860, dComIfGp() + 0x12A0, F + 0x74) != 0) { /* cBgS::LineCross */
            ok = 0;
            break;
        }
    }
    st(F + 0xCC, 0x10005408);
    st(F + 0xD8, 0x10005328);
    st(F + 0x94, 0x10005318);
    gabi::call(0x02008B4C, F + 0x74, 0u);
    st(F + 0x60, 0x10005368);
    st(F + 0x6C, 0x10005328);
    st(F + 0x40, 0x10005348);
    gabi::call(0x02008DAC, F + 0x20, 0u);
    return ok;
}
VERIFY(0x020378B4, SpotMgr_bgCheck);

/* 02037D64: spawn one bottle at a random free sea position near Link (10 failed checks give up) */
static u32 SpotMgr_trySpawnSea(u32 self, u32 spot) {
    WWHD_FUNC(0x02037D64, u32, self, spot);
    if (spot == 0 || (s32)spot >= 0x32) return 0;
    if (gabi::call<u32>(0x02520C0C, 0x21u) == 0) return 0;
    if ((s32)gabi::call<u32>(0x02039A58, ld(COMMENTMGR)) <= 0) return 0;
    u32 pl = ld(dComIfGp() + 0x5B2C);
    if (pl == 0) return 0;
    f32 zero = ldf(0x10005450);
    u32 tries = 0;
    stb(self + 0x30B2, 0);
    gabi::Local<be<u32>[1]> hgt;
    gabi::Local<be<u32>[3]> pos;
    stf(hgt.a, zero);
    for (;;) {
        gabi::call(0x02037480, self, pl, spot, pos.a);
        u32 r = gabi::call<u32>(0x020378B4, self, pos.a, hgt.a);
        if (r == 0) {
            tries = (tries + 1) & 0xFF;
            if (tries >= 10) {
                stb(self + 0x30B2, 1);
                return 0;
            }
        }
        if (lbz(self + 0x30B2) != 0) return 0;
        if (r != 0) break;
    }
    f32 x = ldf(pos.a + 0), z = ldf(pos.a + 8), y = ldf(hgt.a);
    u32 ap = gabi::call<u32>(0x025D5600); /* fopAcM_CreateAppend */
    stf(ap + 4, x);
    stb(ap + 0x1B, 0);
    st(ap + 0, 0x3BFF7F14);
    stf(ap + 0xC, z);
    stb(ap + 0x20, 0xFF);
    sth(ap + 0x14, 0xFF);
    stf(ap + 8, y);
    stb(ap + 0x21, spot);
    u32 layer = gabi::call<u32>(0x025DED64);
    gabi::call(0x025E14A8, layer, 0xFFu, 0u, 0u, ap);
    gabi::call(0x02039D5C, ld(COMMENTMGR));
    u32 e = self + lbz(self + 0x30AC) * 0xC + 0x2FBC;
    stf(e + 0, x);
    stf(e + 4, y);
    stf(e + 8, z);
    u32 n = lbz(self + 0x30AC), d = lbz(self + 0x30D0);
    stb(self + 0x30AC, n + 1);
    stb(self + 0x30D0, d + 1);
    return 1;
}
VERIFY(0x02037D64, SpotMgr_trySpawnSea);

/* 02037F80: is POS inside Link's current sea sector (minus the margin)? */
static u32 SpotMgr_inSector(u32 self, u32 pos) {
    WWHD_FUNC(0x02037F80, u32, self, pos);
    u32 pl = ld(dComIfGp() + 0x5B2C);
    if (pl == 0) return 0;
    f32 pz = ldf(pl + 0x31C), px = ldf(pl + 0x314);
    f32 c350k = ldf(0x100054A0), c100k = ldf(0x1000549C);
    s32 ix = gabi::ftoi((px + c350k) / c100k);
    s32 iz = gabi::ftoi((pz + c350k) / c100k);
    f32 half = ldf(0x10005464);
    f32 margin = ldf(self + 0x30BC);
    f32 cm300k = ldf(0x100054A4);
    f32 m = half - margin;
    f32 cx = gabi::fmadds(i2f(ix), c100k, cm300k);
    f32 x = ldf(pos + 0);
    f32 cz = gabi::fmadds(i2f(iz), c100k, cm300k);
    if (x < cx - m) return 0;
    if (cx + m < x) return 0;
    f32 z = ldf(pos + 8);
    if (z < cz - m) return 0;
    return !(cz + m < z) ? 1 : 0; /* bge: also taken for NaN */
}
VERIFY(0x02037F80, SpotMgr_inSector);

/* 020380B8: copy a 3x4 matrix (through float registers) */
static void SpotMgr_copyMtx(u32 dst, u32 src) {
    WWHD_FUNC(0x020380B8, void, dst, src);
    f32 v[12];
    for (u32 i = 0; i < 12; i++) v[i] = ldf(src + i * 4);
    for (u32 i = 0; i < 12; i++) stf(dst + i * 4, v[i]);
}
VERIFY(0x020380B8, SpotMgr_copyMtx);

/* 02038158: debug view of the spawn positions (only on one stage, with the debug flag +0x30B0) */
static void SpotMgr_debugDraw(u32 self) {
    WWHD_FUNC(0x02038158, void, self);
    if (lbz(self + 0x30B0) == 0) return;
    gabi::Local<be<u32>[2]> l20;
    gabi::Local<be<u32>[2]> l28;
    st(l20.a + 0, 0x100054CC);
    st(l20.a + 4, SS_VT);
    u32 g = dComIfGp();
    u32 vt = ld(l20.a + 4);
    st(l28.a + 4, SS_VT);
    st(l28.a + 0, g + 0x5134);
    gabi::call_ptr<u32>(ld(vt + 0x14), l20.a);
    gabi::call_ptr<u32>(ld(ld(l20.a + 4) + 0x14), l20.a);
    u32 vt2 = ld(l28.a + 4);
    u32 s1 = ld(l20.a);
    gabi::call_ptr<u32>(ld(vt2 + 0x14), l28.a);
    u32 s2 = ld(l28.a);
    if (s1 != s2) {
        u32 i = 0;
        for (; i < 0x40001; i++) {
            u32 a = lbz(s1 + i), b = lbz(s2 + i);
            if (a != b) return;
            if (a == 0) break;
        }
        if (i == 0x40001) return;
    }
    u32 pl = ld(dComIfGp() + 0x5B2C);
    if (pl == 0) return;
    const u32 D = 0x104B45C0;
    gabi::Local<be<u32>[12]> mtx;
    gabi::Local<be<u32>[16]> prj;
    gabi::Local<be<u32>[3]> dif;
    gabi::Local<be<u32>[3]> d2;
    gabi::call(0x020380B8, mtx.a, D + 0x38);
    for (u32 i = 0; i < 16; i++) st(prj.a + i * 4, ld(D + 0x14C + i * 4));
    u32 r = gabi::call<u32>(0x027F29D4, D);
    u32 prev = gabi::call<u32>(0x027A9D24, mtx.a, prj.a, ld(r));
    for (u32 i = 0; i < lbz(self + 0x30AC); i++) {
        f32 zero = ldf(0x10005450);
        gabi::call(cXyz_mi, pl + 0x314, dif.a, self + 0x2FBC + i * 0xC);
        stf(d2.a + 0, ldf(dif.a + 0));
        stf(d2.a + 4, zero);
        stf(d2.a + 8, ldf(dif.a + 8));
        f32 m = gabi::call<f32>(PSVECSquareMag, d2.a);
        gabi::call<f32>(0x028F4384, m);
    }
    r = gabi::call<u32>(0x027F29D4, D);
    st(r + 0, prev);
    st(r + 4, 0);
}
VERIFY(0x02038158, SpotMgr_debugDraw);

/* 0203835C: the singleton disposer's destructor */
static void SpotMgr_disposer_dt(u32 p, u32 flags) {
    WWHD_FUNC(0x0203835C, void, p, flags);
    if (p == 0) return;
    st(p + 0xC, 0x100054D8);
    if (p == ld(DISP)) {
        u32 inst = ld(INST);
        st(DISP, 0);
        gabi::call_ptr<u32>(vfn(inst, 0, 0x14), inst, 2u);
        st(INST, 0);
    }
    gabi::call(0x02752BEC, p, 0u);
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203835C, SpotMgr_disposer_dt);

static void sinit_02038404() {
    WWHD_FUNC(0x02038404, void);
    header_sinit(0x102009EC, 0x1018F42C, 0x100054D0);
}
VERIFY(0x02038404, sinit_02038404);

}  // namespace hd_olv_02036E58
