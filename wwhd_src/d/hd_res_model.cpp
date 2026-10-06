/* hd_res_model: HD model resource (bfres model file + one sharcfb shader archive per model), WWHD.
 * HD-only code (no GameCube source): written from the
 * WWHD code. Range 02602AD4..0260344B; static initialiser 026031F0. The trailing companions are
 * sead library code and not verified here: 026032F0 (sead::FormatFixedSafeString<256> constructor,
 * varargs), 02603284/026032B4/026032C8/026032DC/02603410/02603424/02603438 (deleting destructors of
 * sead string types), 02603298/0260329C (SafeString assureTermination).
 *
 * ResModel (0x24): base 02752A84 (sead disposer-like, dtor 02752BEC), +0xC vtable 100E1960,
 * +0x10 vtable 100E1970, +0x14 nn::g3d ResFile, +0x18 entry count, +0x1C entries (16 bytes each:
 * +0 shader archive object (0x54, ctor 027B744C), +4 archive data, +8 u32, +0xC u8 ready), +0x20 u32.
 */
#include "gabi.h"
using namespace gabi;

namespace hd_res_model {

static const u32 kSafeStringVt = 0x100E1858;
static const u32 kResLoader = 0x101F4F7C;

/* 02602AD4: constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x02602AD4, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x24u);
        if (!p) return 0;
    }
    call<void>(0x02752A84, p);
    store<u32>(p + 0xC, 0x100E1960);
    store<u32>(p + 0x14, 0);
    store<u32>(p + 0x10, 0x100E1970);
    u32 q = p + 0x18;
    if (!q) q = call<u32>(0x0273AD10, 8u);
    if (q) {
        store<u32>(q + 4, 0);
        store<u32>(q, 0);
    }
    store<u32>(p + 0x20, 0);
    return p;
}
VERIFY(0x02602AD4, ctor);

/* 02602B68: destructor */
void dtor(u32 p, u32 flags) {
    WWHD_FUNC(0x02602B68, void, p, flags);
    if (!p) return;
    store<u32>(p + 0x10, 0x100E1970);
    call<void>(0x02752BEC, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x02602B68, dtor);

/* entry idx of the list, clamped to the first one */
static u32 entry(u32 self, u32 idx) {
    const u32 n = load<u32>(self + 0x18);
    const u32 a = load<u32>(self + 0x1C);
    return idx < n ? a + 16 * idx : a;
}

/* relative pointer at p (0 stays 0) */
static u32 rel(u32 p) {
    const u32 o = load<u32>(p);
    return o ? p + o : 0;
}

/* shared body of the two loaders: the bfres file, then for every model "<model name>.sharcfb" */
static void loadFiles(u32 self, u32 archive, u32 name, u32 bindArg, u32 heap, u32 fmtFile, u32 fmtShader, bool lock, u32 matName) {
    Local<u8[0x248]> F;
    const u32 f = F.a;
    call_ptr<void>(load<u32>(load<u32>(name + 4) + 0x14), name);
    const u32 path = call<u32>(0x026032F0, f + 0x138, fmtFile, load<u32>(name));
    const u32 data = call<u32>(0x026124F4, load<u32>(kResLoader), archive, path, 0u);
    const u32 file = call<u32>(0x027E2DC0, data);
    store<u32>(self + 0x14, file);
    call<void>(0x027E2DC4, file);
    if (bindArg) call<void>(0x027E2EA4, load<u32>(self + 0x14), bindArg, 1u);
    s32 n = s32(load<u16>(load<u32>(self + 0x14) + 0x50));
    if (!n) return;
    u32 h = heap;
    if (!h) h = call<u32>(0x02756140, load<u32>(0x101F8B4C));
    const u32 list = call_ptr<u32>(load<u32>(load<u32>(h + 0xC) + 0x34), h, u32(n) << 4, 4u);
    if (list) {
        store<u32>(self + 0x1C, list);
        store<u32>(self + 0x18, u32(n));
    }
    if (n <= 0) n = 1;
    for (u32 i = 0; n; i++, n--) {
        u32 shader = call<u32>(0x0273B050, 0x54u, heap, 4u);
        if (shader) shader = call<u32>(0x027B744C, shader);
        const u32 models = rel(load<u32>(self + 0x14) + 0x20);
        const u32 model = rel(models + 8 + 16 * i + 0x1C);
        store<u32>(f + 0xC, kSafeStringVt);
        store<u32>(f + 8, rel(model + 4));
        call<void>(0x02603298, f + 8);
        const u32 spath = call<u32>(0x026032F0, f + 0x28, fmtShader, load<u32>(f + 8));
        const u32 sdata = call<u32>(0x026124F4, load<u32>(kResLoader), archive, spath, 0u);
        const u32 cnt = load<u32>(self + 0x18), arr = load<u32>(self + 0x1C);
        u32 e = i < cnt ? arr + 16 * i : arr;
        if (sdata) {
            store<u32>(e, shader);
            store<u32>(entry(self, i) + 4, sdata);
            store<u32>(entry(self, i) + 8, 0);
            store<u32>(f + 0x1C, 0);
            store<u32>(f + 0x24, 0);
            store<u32>(f + 0x20, sdata);
            store<u32>(f + 0x18, sdata);
            call<void>(0x027B8904, shader, f + 0x20, f + 0x24, 0u, heap);
            if (lock) call<void>(0x0274FBF8, load<u32>(0x101F8B18));
            call<void>(0x027B9070, shader);
            if (lock) call<void>(0x0274FCCC, load<u32>(0x101F8B18));
            store<u8>(entry(self, i) + 0xC, 1);
            const u32 resFile = load<u32>(self + 0x14);
            store<u32>(f + 0x14, kSafeStringVt);
            store<u32>(f + 0x10, matName);
            call<void>(0x027735DC, resFile, shader, f + 0x10);
        } else {
            store<u32>(e, 0);
            store<u32>(entry(self, i) + 4, 0);
            store<u32>(entry(self, i) + 8, 0);
        }
    }
}

/* 02602BC8: loads "<name>.bfres" and the models' shader archives (shader setup under the GX2 lock) */
void loadModel(u32 self, u32 archive, u32 name, u32 bindArg, u32 heap) {
    WWHD_FUNC(0x02602BC8, void, self, archive, name, bindArg, heap);
    loadFiles(self, archive, name, bindArg, heap, 0x100E1924, 0x100E1930, true, 0x100E1920);
}
VERIFY(0x02602BC8, loadModel);

/* 02602ECC: loadModel without a bind argument */
void loadModelNoBind(u32 self, u32 archive, u32 name, u32 heap) {
    WWHD_FUNC(0x02602ECC, void, self, archive, name, heap);
    loadModel(self, archive, name, 0u, heap);
}
VERIFY(0x02602ECC, loadModelNoBind);

/* 02602ED8: second variant (no lock around the shader setup, own strings) */
void loadModel2(u32 self, u32 archive, u32 name, u32 bindArg, u32 heap) {
    WWHD_FUNC(0x02602ED8, void, self, archive, name, bindArg, heap);
    loadFiles(self, archive, name, bindArg, heap, 0x100E1940, 0x100E194C, false, 0x100E193C);
}
VERIFY(0x02602ED8, loadModel2);

/* 026031C4: loadModel2 without a bind argument */
void loadModel2NoBind(u32 self, u32 archive, u32 name, u32 heap) {
    WWHD_FUNC(0x026031C4, void, self, archive, name, heap);
    loadModel2(self, archive, name, 0u, heap);
}
VERIFY(0x026031C4, loadModel2NoBind);

/* 026031D0: shader archive object of entry idx */
u32 getShader(u32 self, u32 idx) {
    WWHD_FUNC(0x026031D0, u32, self, idx);
    return load<u32>(entry(self, idx));
}
VERIFY(0x026031D0, getShader);

/* 026031F0: static initialiser */
void staticInit() {
    WWHD_FUNC(0x026031F0, void);
    const u32 b = 0x1048DD04;
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, 0x101F4EE0u);
    const f32 lo = load<f32>(0x100E1958), hi = load<f32>(0x100E195C);
    store<f32>(0x1048DCF8, lo);
    store<f32>(0x1048DCFC, hi);
    call<void>(0x028ED6F8, 0x1048DD00u);
    call<void>(0x028F026C, 0x101F4EECu);
    call<void>(0x028EAB2C, 0x1048DD01u);
    call<void>(0x028F026C, 0x101F4EF8u);
}
VERIFY(0x026031F0, staticInit);

} // namespace hd_res_model
