/* hd_font_mgr: HD font resource manager (CKing*.bffnt fonts from Common/Font/EuJpUs/<name>.szs), WWHD.
 * HD-only code (no GameCube source): written from the WWHD
 * code. Range 025F2EA0..025F391B (static initialiser 025F37D8). Library companions (not verified):
 * 025F386C, 025F389C, 025F38B0, 025F38C4 (sead SafeString deleting dtors), 025F3880 (SafeString
 * assureTermination, empty), 025F3884 (BufferedSafeString assureTermination).
 *
 * FontMgr (0x84), singleton 101F4A50 (disposer holder 101F4A54): +0..+0x10 sead disposer
 * (vtable 100E07F0 at +0xC), +0x10 Font*[7], +0x30 Vector2f[7] (glyph width/height per font),
 * +0x68 f32[7] (scale, 1.0). Font index: 0 CKingMain, 1 CKingMsg, 2 CKingRuby, 3 CKingZelda,
 * 4 CKingPic, 5 CafeStd (system font, taken from the object at 101F4BD8 + 0x3C), 6 CKingMainL.
 * Two function-local static SafeString[7] tables: archive names ("<font>_bffnt", 1048D5F4, guard
 * 1048D648) and file names ("<font>.bffnt", 1048D5BC, guard 1048D64C).
 */
#include "gabi.h"
using namespace gabi;

namespace hd_font_mgr {

static const u32 kInstance = 0x101F4A50;
static const u32 kSafeStringVt = 0x100E0654;

/* 025F2EA0 / 025F2EC4: destroy the two static name tables at exit */
void destroyFileNames() {
    WWHD_FUNC(0x025F2EA0, void);
    call<void>(0x028F0164, 0x1048D5BCu, 7u, 8u, 0x025F386Cu, 0u, 0u);
}
VERIFY(0x025F2EA0, destroyFileNames);
void destroyArcNames() {
    WWHD_FUNC(0x025F2EC4, void);
    call<void>(0x028F0164, 0x1048D5F4u, 7u, 8u, 0x025F386Cu, 0u, 0u);
}
VERIFY(0x025F2EC4, destroyArcNames);

/* 025F2EE8: constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x025F2EE8, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x84u);
        if (!p) return 0;
    }
    if (!(p + 0x10)) call<u32>(0x0273AD10, 0x1Cu);
    u32 a = p + 0x30;
    if (!a) a = call<u32>(0x0273AD10, 0x38u);
    if (a) call<void>(0x028EFFD0, a, 7u, 8u, 0x025F38D8u);
    if (!(p + 0x68)) call<u32>(0x0273AD10, 0x1Cu);
    if (!load<u32>(0x1048D644)) {
        const f32 z = load<f32>(0x100E06BC);
        store<u32>(0x1048D644, 1);
        store<f32>(0x1048D640, z);
        store<f32>(0x1048D63C, z);
    }
    for (u32 i = 0; i < 7; i++) {
        store<u32>(p + 0x30 + 8 * i, load<u32>(0x1048D63C));
        store<u32>(p + 0x34 + 8 * i, load<u32>(0x1048D640));
    }
    const f32 one = load<f32>(0x100E06B8);
    for (u32 i = 0; i < 7; i++) store<f32>(p + 0x68 + 4 * i, one);
    return p;
}
VERIFY(0x025F2EE8, ctor);

/* 025F2FE0: creates the singleton with its disposer */
void createInstance(u32 heap) {
    WWHD_FUNC(0x025F2FE0, void, heap);
    if (load<u32>(kInstance)) return;
    const u32 p = call<u32>(0x0273B0D4, 0x84u, heap, 4u);
    if (p) {
        call<void>(0x02752B0C, p, heap, 3u);
        store<u32>(p + 0xC, 0x100E07F0);
    }
    store<u32>(0x101F4A54, p);
    store<u32>(kInstance, p ? ctor(p) : 0u);
}
VERIFY(0x025F2FE0, createInstance);

/* the function-local static SafeString[7] tables */
static u32 nameTable(u32 guard, u32 tab, const u32 (&names)[7]) {
    store<u32>(guard, 1);
    store<u32>(tab + 4, kSafeStringVt);
    store<u32>(tab, names[0]);
    for (u32 i = 1; i < 7; i++) {
        u32 s = tab + 8 * i;
        if (!s) s = call<u32>(0x0273AD10, 8u);
        if (s) {
            store<u32>(s + 4, kSafeStringVt);
            store<u32>(s, names[i]);
        }
    }
    return 0;
}

/* 025F3074: archive name of font i ("CKingMain_bffnt", ...) */
u32 arcName(u32 self, u32 i) {
    WWHD_FUNC(0x025F3074, u32, self, i);
    const u32 tab = 0x1048D5F4;
    if (!load<u32>(0x1048D648)) {
        static const u32 names[7] = {0x100E06C0, 0x100E06F0, 0x100E06D0, 0x100E0700, 0x100E0714, 0x100E06E0, 0x100E0724};
        nameTable(0x1048D648, tab, names);
        call<void>(0x028F026C, 0x101F4A14u);
    }
    return tab + (i << 3);
}
VERIFY(0x025F3074, arcName);

/* 025F320C: file name of font i ("CKingMain.bffnt", ...) */
u32 fileName(u32 self, u32 i) {
    WWHD_FUNC(0x025F320C, u32, self, i);
    const u32 tab = 0x1048D5BC;
    if (!load<u32>(0x1048D64C)) {
        static const u32 names[7] = {0x100E0738, 0x100E0768, 0x100E0748, 0x100E0778, 0x100E078C, 0x100E0758, 0x100E079C};
        nameTable(0x1048D64C, tab, names);
        call<void>(0x028F026C, 0x101F4A20u);
    }
    return tab + (i << 3);
}
VERIFY(0x025F320C, fileName);

static f32 s32ToF32(u32 v) { return f32(f64(s32(v))); }

/* 025F33A4: loads the seven fonts (CafeStd comes from the system object) */
void loadFonts(u32 self, u32 heap) {
    WWHD_FUNC(0x025F33A4, void, self, heap);
    Local<u8[0x110]> F;
    const u32 str = F.a, buf = F.a + 0xC;
    const u32 info = F.a + 0x108;
    for (u32 i = 0; i < 7; i = (i + 1) & 0xFF) {
        if (i == 5) {
            store<u32>(self + 0x10 + 4 * i, load<u32>(0x101F4BD8) + 0x3C);
            continue;
        }
        store<u8>(buf, 0);
        store<u32>(str, buf);
        store<u32>(str + 8, 0x100);
        store<u8>(buf + 0xFF, 0);
        store<u32>(str + 4, 0x100E069C);
        u32 s = arcName(self, i);
        call_ptr<void>(load<u32>(load<u32>(s + 4) + 0x14), s);
        call<void>(0x02759C28, str, 0x100E07B8u, load<u32>(s));
        u32 res = call<u32>(0x026123FC, load<u32>(0x101F4F7C), arcName(self, i));
        if (!res) {
            const u32 mgr = load<u32>(0x101F4F7C);
            s = arcName(self, i);
            if (call<u32>(0x02612E64, mgr, s, str, heap, 1u, 0x40000u, 0x2000u))
                res = call<u32>(0x026123FC, load<u32>(0x101F4F7C), arcName(self, i));
        }
        store<u32>(info, 0);
        store<u32>(info + 4, 0);
        const u32 fn = fileName(self, i);
        const u32 data = call_ptr<u32>(load<u32>(load<u32>(res + 0x10) + 0x3C), res, fn, info);
        u32 font = call<u32>(0x0273B050, 0xC4u, heap, 4u);
        if (font) font = call<u32>(0x0286ED48, font);
        call<void>(0x0274FBF8, load<u32>(0x101F8B18));
        call<void>(0x0286EFEC, font, data);
        call<void>(0x0274FCCC, load<u32>(0x101F8B18));
        call_ptr<void>(load<u32>(load<u32>(font + 4) + 0x74), font, u32(load<u16>(0x100E07D4 + 2 * i)));
        const u32 getH = load<u32>(load<u32>(font + 4) + 0x1C);
        const u32 dst = i < 7 ? self + 0x30 + 8 * i : self + 0x30;
        const f32 w = s32ToF32(call_ptr<u32>(getH, font));
        const f32 h = s32ToF32(call_ptr<u32>(load<u32>(load<u32>(font + 4) + 0x24), font));
        store<f32>(dst, w);
        store<f32>(dst + 4, h);
        store<u32>(self + 0x10 + 4 * i, font);
    }
    const u32 pic = load<u32>(self + 0x20);
    call_ptr<void>(load<u32>(load<u32>(pic + 4) + 0xEC), pic, 0u);
}
VERIFY(0x025F33A4, loadFonts);

/* 025F36B8: font i (0 when out of range) */
u32 getFont(u32 self, s32 i) {
    WWHD_FUNC(0x025F36B8, u32, self, i);
    if (i >= 7) return 0;
    return load<u32>(self + 0x10 + 4 * u32(i));
}
VERIFY(0x025F36B8, getFont);

/* 025F36D8: registers the seven fonts under their file names */
void registerFonts(u32 self, u32 mgr) {
    WWHD_FUNC(0x025F36D8, void, self, mgr);
    for (u32 i = 0; i < 7; i++) {
        const u32 s = fileName(self, i);
        call_ptr<void>(load<u32>(load<u32>(s + 4) + 0x14), s);
        const u32 top = load<u32>(s);
        call<void>(0x028736AC, mgr, top, getFont(self, s32(i)));
    }
}
VERIFY(0x025F36D8, registerFonts);

/* 025F3758: destructor */
void dtor(u32 p, u32 flags) {
    WWHD_FUNC(0x025F3758, void, p, flags);
    if (!p) return;
    store<u32>(p + 0xC, 0x100E07F0);
    if (p == load<u32>(0x101F4A54)) {
        store<u32>(0x101F4A54, 0);
        store<u32>(kInstance, 0);
    }
    call<void>(0x02752BEC, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x025F3758, dtor);

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
    WWHD_FUNC(0x025F37D8, void);
    stdInit(0x1048D62C, 0x101F4A2C, 0x100E07E4, 0x100E07E8, 0x1048D5B0, 0x101F4A38, 0x101F4A44);
}
VERIFY(0x025F37D8, staticInit);

/* 025F38D8: Vector2f element constructor of the size table (zero) */
u32 vec2Ctor(u32 p) {
    WWHD_FUNC(0x025F38D8, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 8u);
        if (!p) return 0;
    }
    const f32 z = load<f32>(0x100E06BC);
    store<f32>(p + 4, z);
    store<f32>(p, z);
    return p;
}
VERIFY(0x025F38D8, vec2Ctor);

} // namespace hd_font_mgr
