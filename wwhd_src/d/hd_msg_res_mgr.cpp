/* hd_msg_res_mgr: HD message resource manager (MSBT message sets per language, CKing.msbp project),
 * WWHD. HD-only code (no GameCube source): written from the
 * WWHD code. Range 025F391C..025F65F3 (static initialiser 025F62C0). Library companions (not
 * verified): 025F6404, 025F6434, 025F6448, 025F645C, 025F6470, 025F6484, 025F6498, 025F64AC,
 * 025F65E0 (sead string deleting dtors), 025F6418 (SafeString assureTermination, empty), 025F641C
 * (BufferedSafeString assureTermination), 025F64C0 (sead::FormatFixedSafeString<256> varargs ctor).
 *
 * MsgResMgr (0x438), singleton 101F4AE8 (disposer holder 101F4AEC): +0..+0x10 sead disposer (vtable
 * 100E09F0 at +0xC), +0x10 project (MSBP) object (+0x2C name count, the names via 0273A728),
 * +0x34 message set count, +0x38 MessageSet*[0x100]. MessageSet (0x118): +0 MSBT data (0273A1CC
 * label lookup, 0273A574 attribute/entry access), +4 entry count, +8 vtable 100E08C0,
 * +0xC FixedSafeString<256> name. Static SafeStrings: 1048D688 "CKing.msbp", 1048D690 "CKing_msbp",
 * 1048D698 "Cafe", 1048D6A0 "US", 1048D6C4 SafeString[4] "message".."message4", function-local
 * "/", ".", "" (1048D670/678/680), "unitString" (1048D650), "Copy"/"copy" (1048D658/660),
 * 1048D668. Message lookups fill a result object: +8 MessageSet, +0xC FixedSafeString set name
 * (size +0x14), +0x118 label index, +0x120 FixedSafeString label (size +0x128), +0x22C entry.
 */
#include "gabi.h"
using namespace gabi;

namespace hd_msg_res_mgr {

static const u32 kInstance = 0x101F4AE8;
static const u32 kResMgr = 0x101F4F7C;
static const u32 kSafeStringVt = 0x100E0800;
static const u32 kBufferedVt = 0x100E0860;
static const u32 kFixed256Vt = 0x100E0878;
static const u32 kNoAssure = 0x025F6418;   /* SafeString::assureTerminationImpl_ (empty) */
static const u32 kFixedAssure = 0x025F641C; /* BufferedSafeString::assureTerminationImpl_ */

/* strlen capped at 0x40000 as sead does */
static s32 cstrLength(u32 p) {
    s32 n = 0;
    if (!load<u8>(p)) return 0;
    for (;;) {
        n++;
        if (n > 0x40000) return 0;
        p++;
        if (!load<u8>(p)) return n;
    }
}

/* virtual assureTermination of a string object (vtable at +4) */
static void vAssure(u32 s) { call_ptr<void>(load<u32>(load<u32>(s + 4) + 0x14), s); }

/* buffered dst = string src (both assure calls virtual; dst buffer given) */
static void copyStr(u32 dst, u32 buf, u32 src) {
    vAssure(src);
    s32 n = cstrLength(load<u32>(src));
    const s32 size = s32(load<u32>(dst + 8));
    const u32 fn = load<u32>(load<u32>(src + 4) + 0x14);
    if (n >= size) n = size - 1;
    call_ptr<void>(fn, src);
    call<void>(0xC0009988, buf, load<u32>(src), u32(n), 0u);
    store<u8>(buf + n, 0);
}

/* buffered dst = temporary SafeString tmp (first assure devirtualised to the empty one) */
static void copyTmp(u32 dst, u32 buf, u32 tmp) {
    call<void>(kNoAssure, tmp);
    s32 n = cstrLength(load<u32>(tmp));
    const s32 size = s32(load<u32>(dst + 8));
    const u32 fn = load<u32>(load<u32>(tmp + 4) + 0x14);
    if (n >= size) n = size - 1;
    call_ptr<void>(fn, tmp);
    call<void>(0xC0009988, buf, load<u32>(tmp), u32(n), 0u);
    store<u8>(buf + n, 0);
}

/* FixedSafeString<256> dst += literal (temporary SafeString at tmp) */
static void appendLit(u32 dst, u32 tmp, u32 lit) {
    store<u32>(tmp + 4, kSafeStringVt);
    store<u32>(tmp, lit);
    const u32 base = load<u32>(dst);
    call<void>(kFixedAssure, dst);
    const s32 cur = cstrLength(load<u32>(dst));
    const s32 c0 = cur < 0 ? 0 : cur;
    const u32 fn = load<u32>(load<u32>(tmp + 4) + 0x14);
    call_ptr<void>(fn, tmp);
    s32 n = cstrLength(load<u32>(tmp));
    const s32 avail = s32(load<u32>(dst + 8)) - c0;
    if (n >= avail) n = avail - 1;
    if (n <= 0) return;
    vAssure(tmp);
    call<void>(0xC0009988, base + u32(c0), load<u32>(tmp), u32(n), 0u);
    if (c0 + n > cur) store<u8>(base + u32(c0 + n), 0);
}

/* sead string equality as inlined: a (vtable at +4) against b */
static bool strEqual(u32 a, u32 b) {
    vAssure(a);
    vAssure(a);
    const u32 ta = load<u32>(a);
    vAssure(b);
    if (ta == load<u32>(b)) return true;
    u32 p = load<u32>(a);
    const u32 q = load<u32>(b);
    for (u32 k = 0; k < 0x40000; k++) {
        const u32 c = load<u8>(p + k);
        const u32 d = load<u8>(q + k);
        if (!c) return d == 0;
        if (!d || d != c) return false;
    }
    return true;
}

/* 025F391C: constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x025F391C, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x438u);
        if (!p) return 0;
    }
    store<u32>(p + 0x10, 0);
    store<u32>(p + 0x30, 0x10147400);
    for (u32 off = 0x14; off <= 0x24; off += 8) {
        u32 q = p + off;
        if (!q) q = call<u32>(0x0273AD10, 8u);
        if (q) {
            store<u32>(q + 4, 0);
            store<u32>(q, 0);
        }
    }
    store<u32>(p + 0x2C, 0);
    store<u32>(p + 0x34, 0);
    if (!(p + 0x38)) call<u32>(0x0273AD10, 0x400u);
    for (u32 i = 0; i < 0x100; i++) store<u32>(p + 0x38 + 4 * i, 0);
    return p;
}
VERIFY(0x025F391C, ctor);

/* 025F3A14: creates the singleton with its disposer */
void createInstance(u32 heap) {
    WWHD_FUNC(0x025F3A14, void, heap);
    if (load<u32>(kInstance)) return;
    const u32 p = call<u32>(0x0273B0D4, 0x438u, heap, 4u);
    if (p) {
        call<void>(0x02752B0C, p, heap, 3u);
        store<u32>(p + 0xC, 0x100E09F0);
    }
    store<u32>(0x101F4AEC, p);
    store<u32>(kInstance, p ? ctor(p) : 0u);
}
VERIFY(0x025F3A14, createInstance);

/* 025F3AA8: language folder name (UsEnglish, UsFrench, ...) for a language id */
u32 langName(u32 self, u32 lang) {
    WWHD_FUNC(0x025F3AA8, u32, self, lang);
    if (lang < 1 || lang > 5) return load<u32>(0x101F4A58);
    return load<u32>(0x101F4A58 + 4 * u32(load<u8>(0x100E08E7 + lang)));
}
VERIFY(0x025F3AA8, langName);

/* 025F3AE8: the MSBT data of a message set (loads "<region>/<country>/Message/<lang>/<name>_msbt.szs"
 * when the archive is not loaded yet) */
u32 openMsbt(u32 self, u32 heap, u32 set, u32 lang) {
    WWHD_FUNC(0x025F3AE8, u32, self, heap, set, lang);
    Local<u8[0x230]> F;
    const u32 tmp = F.a, info = F.a + 8, B = F.a + 0x10, Bbuf = F.a + 0x1C;
    const u32 A = F.a + 0x11C, Abuf = F.a + 0x128, path = F.a + 0x228;
    (void)path;
    /* A = set name + ".msbt" (file name in the archive) */
    store<u32>(A + 4, kBufferedVt);
    store<u32>(A, Abuf);
    store<u32>(A + 8, 0x100);
    store<u8>(Abuf + 0xFF, 0);
    copyStr(A, Abuf, set + 0xC);
    store<u32>(A + 4, kFixed256Vt);
    appendLit(A, tmp, 0x100E092C);
    /* B = set name + "_msbt" (archive name) */
    store<u32>(B + 8, 0x100);
    store<u8>(Bbuf + 0xFF, 0);
    store<u32>(B, Bbuf);
    store<u32>(B + 4, kBufferedVt);
    copyStr(B, Bbuf, set + 0xC);
    store<u32>(B + 4, kFixed256Vt);
    appendLit(B, tmp, 0x100E0934);
    u32 res = call<u32>(0x026123FC, load<u32>(kResMgr), B);
    if (!res) {
        const u32 mgr = load<u32>(kResMgr);
        vAssure(0x1048D698);
        const u32 region = load<u32>(0x1048D698);
        vAssure(0x1048D6A0);
        const u32 country = load<u32>(0x1048D6A0);
        vAssure(B);
        const u32 name = load<u32>(B);
        const u32 ln = langName(self, lang);
        Local<u8[0x110]> P;
        const u32 p = call<u32>(0x025F64C0, P.a, 0x100E093Cu, region, country, ln, name);
        if (!call<u32>(0x02612E64, mgr, B, p, heap, 1u, 0x40000u, 0x2000u)) return 0;
        res = call<u32>(0x026123FC, load<u32>(kResMgr), B);
        if (!res) return 0;
    }
    store<u32>(info + 4, 0);
    store<u32>(info, 0);
    return call_ptr<u32>(load<u32>(load<u32>(res + 0x10) + 0x3C), res, A, info);
}
VERIFY(0x025F3AE8, openMsbt);

/* 025F3FB4: creates one message set (name copied, MSBT opened and bound) */
void loadSet(u32 self, u32 out, u32 heap, u32 name, u32 lang) {
    WWHD_FUNC(0x025F3FB4, void, self, out, heap, name, lang);
    const u32 obj = call<u32>(0x0273B050, 0x118u, heap, 4u);
    if (obj) {
        u32 h = obj;
        if (!h) h = call<u32>(0x0273AD10, 0xCu);
        if (h) {
            store<u32>(h, 0);
            store<u32>(h + 4, 0);
            store<u32>(h + 8, 0x100E08C0);
        }
        u32 s = obj + 0xC;
        bool ok = true;
        if (!s) {
            s = call<u32>(0x0273AD10, 0x10Cu);
            if (!s) ok = false;
        }
        if (ok) {
            u32 b = s;
            if (!b) b = call<u32>(0x0273AD10, 0xCu);
            if (b) {
                store<u32>(b, s + 0xC);
                store<u32>(b + 4, 0x100E0818);
                store<u32>(b + 8, 0x100);
                store<u8>(s + 0x10B, 0);
            }
            const u32 top = load<u32>(s);
            store<u32>(s + 4, kBufferedVt);
            store<u8>(top, 0);
            store<u32>(s + 4, kFixed256Vt);
        }
    }
    store<u32>(out, obj);
    Local<u32[2]> T;
    store<u32>(T.a + 4, kSafeStringVt);
    store<u32>(T.a, load<u32>(name));
    const u32 buf = load<u32>(obj + 0xC);
    copyTmp(obj + 0xC, buf, T.a);
    const u32 data = openMsbt(self, heap, obj, lang);
    if (data) call<void>(0x027579BC, obj, data, heap);
}
VERIFY(0x025F3FB4, loadSet);

/* function-local static SafeString {lit} (guard, object, atexit record) */
static void localStr(u32 guard, u32 obj, u32 lit, u32 rec) {
    if (load<u32>(guard)) return;
    store<u32>(guard, 1);
    store<u32>(obj + 4, kSafeStringVt);
    store<u32>(obj, lit);
    call<void>(0x028F026C, rec);
}

/* sead rfindIndex of pat in s (temporary SafeStrings at tmp), -1 when absent */
static s32 rfindIndex(u32 s, u32 pat, u32 tmp) {
    vAssure(s);
    const s32 l1 = cstrLength(load<u32>(s));
    vAssure(pat);
    const s32 l2 = cstrLength(load<u32>(pat));
    s32 i = l1 - l2;
    if (i < 0) return -1;
    for (s32 cnt = i + 1; cnt; cnt--, i--) {
        store<u32>(tmp + 4, kSafeStringVt);
        store<u32>(tmp, load<u32>(s) + u32(i));
        call<void>(kNoAssure, tmp);
        vAssure(tmp);
        const u32 t = load<u32>(tmp);
        vAssure(pat);
        const u32 q = load<u32>(pat);
        if (t == q || l2 <= 0) return i;
        const u32 p = load<u32>(tmp);
        bool eq = true;
        for (s32 k = 0; k < l2; k++) {
            const u32 c = load<u8>(p + u32(k)), d = load<u8>(q + u32(k));
            if (!c) {
                eq = d == 0;
                break;
            }
            if (!d || d != c) {
                eq = false;
                break;
            }
        }
        if (eq) return i;
    }
    return -1;
}

/* 025F416C: creates every message set named by the project (file name without folder/extension) */
void loadSets(u32 self, u32 heap) {
    WWHD_FUNC(0x025F416C, void, self, heap);
    const u32 cnt = load<u32>(self + 0x2C);
    store<u32>(self + 0x34, cnt);
    const u32 lang = load<u32>(load<u32>(0x101F4BAC) + 0x14);
    if (s32(cnt) <= 0) return;
    Local<u8[0x220]> F;
    const u32 T = F.a, P = F.a + 8, Pbuf = F.a + 0x14, N = F.a + 0x114, Nbuf = F.a + 0x120;
    Local<u32[2]> T30;
    s32 left = s32(cnt);
    for (u32 i = 0;; i++) {
        store<u8>(Nbuf + 0xFF, 0);
        store<u32>(N, Nbuf);
        store<u8>(Nbuf, 0);
        store<u32>(N + 8, 0x100);
        store<u32>(N + 4, kFixed256Vt);
        u32 s = 0;
        if (i < load<u32>(self + 0x2C)) s = call<u32>(0x0273A728, load<u32>(self + 0x10), i);
        store<u32>(T, s);
        store<u32>(P + 8, 0x100);
        store<u32>(T + 4, kSafeStringVt);
        store<u8>(Pbuf + 0xFF, 0);
        store<u32>(P, Pbuf);
        store<u32>(P + 4, kBufferedVt);
        copyTmp(P, Pbuf, T);
        store<u32>(P + 4, kFixed256Vt);
        /* N = part after the last '/' */
        localStr(0x1048D6E4, 0x1048D670, 0x100E0954, 0x101F4A70);
        const s32 slash = rfindIndex(P, 0x1048D670, T30.a);
        if (slash >= 0) {
            vAssure(P);
            const u32 top = load<u32>(P);
            const s32 plen = cstrLength(top);
            const s32 at = slash + 1;
            const u32 src = (at < 0 || at > plen) ? load<u32>(0x104A0CD8) : top + u32(at);
            Local<u32[2]> T40;
            store<u32>(T40.a + 4, kSafeStringVt);
            store<u32>(T40.a, src);
            copyTmp(N, load<u32>(N), T40.a);
        } else {
            copyStr(N, load<u32>(N), P);
        }
        /* cut at the last '.' */
        localStr(0x1048D6E8, 0x1048D678, 0x100E0956, 0x101F4A7C);
        s32 dot = rfindIndex(N, 0x1048D678, T30.a);
        localStr(0x1048D6EC, 0x1048D680, 0x100E0958, 0x101F4A88);
        const u32 ntop = load<u32>(N);
        vAssure(N);
        const s32 nlen = cstrLength(load<u32>(N));
        if (dot < 0) {
            dot = nlen + dot + 1;
            if (dot < 0) dot = 0;
        }
        const s32 avail = s32(load<u32>(N + 8)) - dot;
        s32 n = 1;
        if (avail <= 1) n = avail - 1;
        if (n > 0) {
            const u32 dst = ntop + u32(dot);
            vAssure(0x1048D680);
            call<void>(0xC0009988, dst, load<u32>(0x1048D680), u32(n), 0u);
            if (dot + n > nlen) store<u8>(ntop + u32(dot + n), 0);
        }
        loadSet(self, i < 0x100 ? self + 0x38 + 4 * i : self + 0x38, heap, N, lang);
        if (--left == 0) return;
    }
}
VERIFY(0x025F416C, loadSets);

/* 025F4A60: loads the project (CKing.msbp from CKing_msbp.szs) and every message set */
void loadProject(u32 self, u32 heap) {
    WWHD_FUNC(0x025F4A60, void, self, heap);
    const u32 cfg = load<u32>(0x101F4BAC);
    const u32 arc = 0x1048D690;
    const u32 mgr0 = load<u32>(kResMgr);
    const u32 lang = load<u32>(cfg + 0x14);
    u32 res = call<u32>(0x026123FC, mgr0, arc);
    if (!res) {
        const u32 mgr = load<u32>(kResMgr);
        vAssure(0x1048D698);
        const u32 region = load<u32>(0x1048D698);
        vAssure(0x1048D6A0);
        const u32 country = load<u32>(0x1048D6A0);
        vAssure(arc);
        const u32 name = load<u32>(arc);
        const u32 ln = langName(self, lang);
        Local<u8[0x110]> P;
        const u32 p = call<u32>(0x025F64C0, P.a, 0x100E095Cu, region, country, ln, name);
        if (call<u32>(0x02612E64, mgr, arc, p, heap, 1u, 0x40000u, 0x2000u))
            res = call<u32>(0x026123FC, load<u32>(kResMgr), arc);
    }
    Local<u32[2]> info;
    store<u32>(info.a + 4, 0);
    store<u32>(info.a, 0);
    const u32 data = call_ptr<u32>(load<u32>(load<u32>(res + 0x10) + 0x3C), res, 0x1048D688u, info.a);
    call<void>(0x0275766C, self + 0x10, data, heap);
    loadSets(self, heap);
}
VERIFY(0x025F4A60, loadProject);

/* 025F4BC0: the project object */
u32 project(u32 self) {
    WWHD_FUNC(0x025F4BC0, u32, self);
    return self + 0x10;
}
VERIFY(0x025F4BC0, project);

/* 025F4BC8: the message set of that name (0 when none) */
u32 findSet(u32 self, u32 name, u32 arr, s32 count) {
    WWHD_FUNC(0x025F4BC8, u32, self, name, arr, count);
    if (count <= 0) return 0;
    u32 i = 0, off = 0, r28 = 0;
    for (;;) {
        u32 p;
        if (i < 0x100) {
            p = arr + off;
            r28 = off;
        } else {
            p = arr;
        }
        const u32 e = load<u32>(p);
        if (strEqual(e + 0xC, name)) {
            if (i < 0x100) arr += r28;
            return load<u32>(arr);
        }
        r28 += 4;
        i++;
        off += 4;
        if (--count == 0) return 0;
    }
}
VERIFY(0x025F4BC8, findSet);

/* 025F4CFC: the "unitString" message set */
u32 unitSet(u32 self) {
    WWHD_FUNC(0x025F4CFC, u32, self);
    const u32 s = 0x1048D650;
    if (!load<u32>(0x1048D6F0)) {
        store<u32>(0x1048D6F0, 1);
        store<u32>(s, 0x100E0974);
        store<u32>(s + 4, kSafeStringVt);
        call<void>(0x028F026C, 0x101F4A94u);
    }
    return findSet(self, s, self + 0x38, s32(load<u32>(self + 0x34)));
}
VERIFY(0x025F4CFC, unitSet);

/* 025F4D84: index of the message set of that name (-1 when none) */
s32 findSetIndex(u32 self, u32 name, u32 arr, s32 count) {
    WWHD_FUNC(0x025F4D84, s32, self, name, arr, count);
    if (count <= 0) return -1;
    u32 i = 0, q = arr;
    for (;;) {
        const u32 p = i < 0x100 ? q : arr;
        const u32 e = load<u32>(p);
        if (strEqual(e + 0xC, name)) return s32(i);
        i++;
        q += 4;
        if (--count == 0) return -1;
    }
}
VERIFY(0x025F4D84, findSetIndex);

/* 025F4EA4: the message set holding a label, searched in the named set */
u32 setWithLabel(u32 self, u32 name, u32 label, u32 arr, s32 count) {
    WWHD_FUNC(0x025F4EA4, u32, self, name, label, arr, count);
    const s32 idx = findSetIndex(self, name, arr, count);
    if (idx < 0) return 0;
    u32 p = arr;
    const u32 vt = load<u32>(label + 4);
    if (u32(idx) < 0x100) p = arr + (u32(idx) << 2);
    const u32 fn = load<u32>(vt + 0x14);
    const u32 e = load<u32>(p);
    call_ptr<void>(fn, label);
    const u32 li = call<u32>(0x0273A1CC, load<u32>(e), load<u32>(label));
    if (li >= 0xFFFFFFFEu) return 0;
    if (!call<u32>(0x02757A80, e, li)) return 0;
    return e;
}
VERIFY(0x025F4EA4, setWithLabel);

/* 025F4F48: the message set holding a label, searched in "message".."message4" */
u32 setWithLabelAny(u32 self, u32 label, u32 arr, s32 count) {
    WWHD_FUNC(0x025F4F48, u32, self, label, arr, count);
    for (u32 k = 0; k < 4; k++) {
        const u32 r = setWithLabel(self, 0x1048D6C4 + 8 * k, label, arr, count);
        if (r) return r;
    }
    return 0;
}
VERIFY(0x025F4F48, setWithLabelAny);

/* 025F4FC0: same over the manager's own sets */
u32 setWithLabelOwn(u32 self, u32 label) {
    WWHD_FUNC(0x025F4FC0, u32, self, label);
    return setWithLabelAny(self, label, self + 0x38, s32(load<u32>(self + 0x34)));
}
VERIFY(0x025F4FC0, setWithLabelOwn);

/* 025F4FCC: does the label start with a digit (numeric message id)? */
u32 isNumericLabel(u32 self, u32 s) {
    WWHD_FUNC(0x025F4FCC, u32, self, s);
    vAssure(s);
    const u32 top = load<u32>(s);
    u32 p = top;
    s32 n = 0;
    if (load<u8>(p)) {
        for (;;) {
            n++;
            if (n > 0x40000) break;
            p++;
            if (!load<u8>(p)) break;
        }
    }
    (void)n;
    return u32(load<u8>(top)) - 0x30 < 10 ? 1u : 0u;
}
VERIFY(0x025F4FCC, isNumericLabel);

/* 025F506C: the message set of a label (numeric labels: any "message*" set; else the named set) */
u32 setForLabel(u32 self, u32 name, u32 label) {
    WWHD_FUNC(0x025F506C, u32, self, name, label);
    if (isNumericLabel(self, label))
        return setWithLabelAny(self, label, self + 0x38, s32(load<u32>(self + 0x34)));
    return setWithLabel(self, name, label, self + 0x38, s32(load<u32>(self + 0x34)));
}
VERIFY(0x025F506C, setForLabel);

/* 025F50E8: looks up a message: fills the result object (set, names, label index, entry) */
void getMessage(u32 self, u32 out, u32 name, u32 label) {
    WWHD_FUNC(0x025F50E8, void, self, out, name, label);
    const u32 e = setForLabel(self, name, label);
    if (!e) {
        store<u32>(out + 8, 0);
        return;
    }
    vAssure(label);
    const u32 li = call<u32>(0x0273A1CC, load<u32>(e), load<u32>(label));
    if (li >= 0xFFFFFFFEu) {
        store<u32>(out + 8, 0);
        return;
    }
    u32 entry = 0;
    if (li < load<u32>(e + 4)) entry = call<u32>(0x0273A574, load<u32>(e), li);
    /* set name */
    if (!strEqual(out + 0xC, name)) {
        const u32 dst = load<u32>(out + 0xC);
        vAssure(name);
        s32 n = cstrLength(load<u32>(name));
        const s32 size = s32(load<u32>(out + 0x14));
        const u32 fn = load<u32>(load<u32>(name + 4) + 0x14);
        if (n >= size) n = size - 1;
        call_ptr<void>(fn, name);
        call<void>(0xC0009988, dst, load<u32>(name), u32(n), 0u);
        store<u8>(dst + n, 0);
    }
    store<u32>(out + 0x118, li);
    /* label */
    if (!strEqual(out + 0x120, label)) {
        const u32 dst = load<u32>(out + 0x120);
        vAssure(label);
        s32 n = cstrLength(load<u32>(label));
        const s32 size = s32(load<u32>(out + 0x128));
        const u32 fn = load<u32>(load<u32>(label + 4) + 0x14);
        if (n >= size) n = size - 1;
        call_ptr<void>(fn, label);
        call<void>(0xC0009988, dst, load<u32>(label), u32(n), 0u);
        store<u8>(dst + n, 0);
    }
    store<u32>(out + 0x22C, entry);
    store<u32>(out + 8, e);
}
VERIFY(0x025F50E8, getMessage);

/* sead RTTI: the guarded chain of static type infos ending at 101FDD3C (the type the tag target is
 * checked against), initialised on first use */
static void rttiChain() {
    if (load<u32>(0x101FDD40)) return;
    const u32 g1 = load<u32>(0x101FD8F0);
    store<u32>(0x101FDD40, 1);
    if (!g1) {
        const u32 g2 = load<u32>(0x101FD89C);
        store<u32>(0x101FD8F0, 1);
        if (!g2) {
            const u32 g3 = load<u32>(0x101FD888);
            store<u32>(0x101FD89C, 1);
            if (!g3) {
                store<u32>(0x101FD8FC, 0);
                store<u32>(0x101FD888, 1);
            }
            store<u32>(0x101FD8F8, 0x101FD8FC);
        }
        store<u32>(0x101FD8F4, 0x101FD8F8);
    }
    store<u32>(0x101FDD3C, 0x101FD8F4);
}

/* is the object (RTTI getter at vtable(+8)+0x14) of the type 101FDD3C or derived from it? */
static bool isTagTarget(u32 o) {
    const u32 t0 = call_ptr<u32>(load<u32>(load<u32>(o + 8) + 0x14), o);
    if (!t0) return false;
    for (u32 t = t0; t; t = load<u32>(t))
        if (t == 0x101FDD3C) return true;
    return false;
}

/* label = the target's label string (+0x128) */
static void copyTargetLabel(u32 o, u32 S256) {
    const u32 dst = load<u32>(S256);
    vAssure(o + 0x128);
    s32 n = cstrLength(load<u32>(o + 0x128));
    const s32 size = s32(load<u32>(S256 + 8));
    const u32 fn = load<u32>(load<u32>(o + 0x12C) + 0x14);
    if (n >= size) n = size - 1;
    call_ptr<void>(fn, o + 0x128);
    call<void>(0xC0009988, dst, load<u32>(o + 0x128), u32(n), 0u);
    store<u8>(dst + n, 0);
}

/* finds a child of a layout object by name (vtable(+8)+0x5C) */
static u32 findChild(u32 obj, u32 str) {
    const u32 vt = load<u32>(obj + 8);
    vAssure(str);
    return call_ptr<u32>(load<u32>(vt + 0x5C), obj, load<u32>(str), 1u);
}

/* 025F5444: resolves a message for a layout text box: "Copy"/"copy" tags redirect the label to the
 * label of another text box ("<pane>" or "<layout>:<pane>" style target names) */
void resolveMessage(u32 self, u32 out, u32 box, u32 name, u32 text) {
    WWHD_FUNC(0x025F5444, void, self, out, box, name, text);
    Local<u8[0x50]> L64;   /* FixedSafeString<64> set name */
    const u32 S64 = L64.a, S64buf = L64.a + 0xC;
    Local<u8[0x110]> L256; /* FixedSafeString<256> label */
    const u32 S256 = L256.a, S256buf = L256.a + 0xC;
    Local<u32[2]> T;
    store<u32>(S64, S64buf);
    store<u32>(S64 + 4, 0x100E0830);
    store<u32>(T.a + 4, kSafeStringVt);
    store<u8>(S64buf + 0x3F, 0);
    store<u32>(S64 + 8, 0x40);
    store<u32>(T.a, load<u32>(box + 0x1C));
    copyTmp(S64, S64buf, T.a);
    store<u32>(S64 + 4, 0x100E0848);
    vAssure(name);
    Local<u32[2]> T18;
    store<u32>(T18.a + 4, kSafeStringVt);
    store<u32>(T18.a, load<u32>(name));
    store<u32>(S256 + 4, kBufferedVt);
    store<u32>(S256, S256buf);
    store<u32>(S256 + 8, 0x100);
    store<u8>(S256buf + 0xFF, 0);
    copyTmp(S256, S256buf, T18.a);
    store<u32>(S256 + 4, kFixed256Vt);
    if (isNumericLabel(self, S256) || !load<u32>(load<u32>(box + 0xC) + 0xC)) {
        getMessage(self, out, S64, S256);
        return;
    }
    const u32 parent = load<u32>(box + 0x34);
    {
        const u32 dst = load<u32>(S64);
        Local<u32[2]> T20;
        store<u32>(T20.a + 4, kSafeStringVt);
        store<u32>(T20.a, load<u32>(parent + 0x1C));
        copyTmp(S64, dst, T20.a);
    }
    /* the "Copy" / "copy" tag */
    u32 tag;
    {
        localStr(0x1048D6F4, 0x1048D658, 0x100E0988, 0x101F4AA0);
        vAssure(0x1048D658);
        const u32 key = load<u32>(0x1048D658);
        const u32 a = call<u32>(0x028764B8, text);
        const u32 b = call<u32>(0x028764A0, text);
        tag = call<u32>(0x02705F7C, key, a, b, 0u);
    }
    if (!tag) {
        localStr(0x1048D6F8, 0x1048D660, 0x100E0990, 0x101F4AAC);
        vAssure(0x1048D660);
        const u32 key = load<u32>(0x1048D660);
        const u32 a = call<u32>(0x028764B8, text);
        const u32 b = call<u32>(0x028764A0, text);
        tag = call<u32>(0x02705F7C, key, a, b, 0u);
        if (!tag) {
            getMessage(self, out, S64, S256);
            return;
        }
    }
    const u32 val = tag + load<u32>(tag + 4);
    if (!val) {
        getMessage(self, out, S64, S256);
        return;
    }
    /* S128 = the tag value (target name) */
    Local<u8[0x90]> L128;
    const u32 S128 = L128.a, S128buf = L128.a + 0xC;
    {
        Local<u32[2]> T28;
        store<u32>(T28.a + 4, kSafeStringVt);
        store<u32>(S128, S128buf);
        store<u32>(T28.a, val);
        store<u32>(S128 + 8, 0x80);
        store<u8>(S128buf + 0x7F, 0);
        store<u32>(S128 + 4, 0x100E0890);
        copyTmp(S128, S128buf, T28.a);
    }
    store<u32>(S128 + 4, 0x100E08A8);
    /* forward search of the separator 1048D668 */
    localStr(0x1048D6FC, 0x1048D668, 0x100E0984, 0x101F4AB8);
    vAssure(S128);
    const s32 l1 = cstrLength(load<u32>(S128));
    vAssure(0x1048D668);
    const s32 l2 = cstrLength(load<u32>(0x1048D668));
    const s32 last = l1 - l2;
    s32 idx = -1;
    Local<u32[2]> T8;
    for (s32 i = 0; i <= last; i++) {
        store<u32>(T8.a + 4, kSafeStringVt);
        store<u32>(T8.a, load<u32>(S128) + u32(i));
        call<void>(kNoAssure, T8.a);
        vAssure(T8.a);
        const u32 t = load<u32>(T8.a);
        vAssure(0x1048D668);
        const u32 q = load<u32>(0x1048D668);
        bool eq = true;
        if (t != q && l2 > 0) {
            const u32 p = load<u32>(T8.a);
            for (s32 k = 0; k < l2; k++) {
                const u32 c = load<u8>(p + u32(k)), d = load<u8>(q + u32(k));
                if (!c) {
                    eq = d == 0;
                    break;
                }
                if (!d || d != c) {
                    eq = false;
                    break;
                }
            }
        }
        if (eq) {
            idx = i;
            break;
        }
    }
    if (idx < 0) {
        /* a pane of the box's own layout */
        const u32 lay = load<u32>(box + 0xC);
        const u32 vt = load<u32>(lay + 8);
        vAssure(S128);
        const u32 o = call_ptr<u32>(load<u32>(vt + 0x5C), lay, load<u32>(S128), 1u);
        rttiChain();
        if (o && isTagTarget(o)) copyTargetLabel(o, S256);
        getMessage(self, out, S64, S256);
        return;
    }
    /* "<layout><sep><pane>" */
    Local<u8[0x90]> LA;
    const u32 A = LA.a, Abuf = LA.a + 0xC;
    store<u8>(Abuf, 0);
    store<u32>(A + 4, 0x100E08A8);
    s32 n = idx < 0x80 ? idx : 0x7F;
    store<u8>(Abuf + 0x7F, 0);
    store<u32>(A, Abuf);
    store<u32>(A + 8, 0x80);
    vAssure(S128);
    call<void>(0xC0009988, Abuf, load<u32>(S128), u32(n), 0u);
    store<u8>(Abuf + n, 0);
    const u32 lay = findChild(load<u32>(parent + 0xC), A);
    if (!lay) {
        getMessage(self, out, S64, S256);
        return;
    }
    Local<u8[0x90]> LB;
    const u32 B = LB.a, Bbuf = LB.a + 0xC;
    store<u8>(Bbuf, 0);
    store<u32>(B + 4, 0x100E08A8);
    store<u32>(B + 8, 0x80);
    store<u32>(B, Bbuf);
    store<u8>(Bbuf + 0x7F, 0);
    vAssure(S128);
    {
        Local<u32[2]> T30;
        store<u32>(T30.a + 4, kSafeStringVt);
        store<u32>(T30.a, load<u32>(S128) + u32(idx) + 1);
        copyTmp(B, load<u32>(B), T30.a);
    }
    const u32 o = findChild(lay, B);
    rttiChain();
    if (o && isTagTarget(o)) copyTargetLabel(o, S256);
    getMessage(self, out, S64, S256);
}
VERIFY(0x025F5444, resolveMessage);

/* 025F5FEC: parses a 5-digit message number */
void parseNumber5(u32 self, u32 out, u32 s) {
    WWHD_FUNC(0x025F5FEC, void, self, out, s);
    if (!out || !s) return;
    vAssure(s);
    u32 p = load<u32>(s);
    s32 n = 0;
    if (load<u8>(p)) {
        for (;;) {
            n++;
            if (n > 0x40000) return;
            p++;
            if (!load<u8>(p)) break;
        }
    }
    if (n != 5) return;
    vAssure(s);
    u32 v = 0, m = 10000;
    const u32 t = load<u32>(s);
    for (u32 k = 0; k < 5; k++) {
        const u32 c = load<u8>(t + k);
        if (c - 0x30 >= 10) return;
        v += m * (c - 0x30);
        m = m / 10;
    }
    store<u32>(out, v);
}
VERIFY(0x025F5FEC, parseNumber5);

/* 025F60EC: finds the message whose attribute id (+0xC, s16) matches and returns its number */
u32 findById(u32 self, u32 labelOut, u32 numOut, u32 id) {
    WWHD_FUNC(0x025F60EC, u32, self, labelOut, numOut, id);
    if (!labelOut || !numOut) return 0;
    const u32 arr = self + 0x38;
    for (u32 k = 0; k < 4; k++) {
        const s32 idx = findSetIndex(self, 0x1048D6C4 + 8 * k, arr, s32(load<u32>(self + 0x34)));
        if (idx < 0) continue;
        const u32 e = u32(idx) < 0x100 ? load<u32>(arr + (u32(idx) << 2)) : load<u32>(arr);
        u32 cnt = load<u32>(e + 4);
        if (s32(cnt) <= 0) continue;
        s32 left = s32(cnt);
        for (u32 j = 0;;) {
            u32 item = 0;
            if (j < cnt) item = call<u32>(0x0273A574, load<u32>(e), j);
            if (u32(s32(load<s16>(item + 0xC))) == id) {
                if (!call<u32>(0x02757A9C, e, labelOut, j)) return 0;
                parseNumber5(self, numOut, labelOut);
                return 1;
            }
            j++;
            if (--left == 0) break;
            cnt = load<u32>(e + 4);
        }
    }
    return 0;
}
VERIFY(0x025F60EC, findById);

/* 025F6220: destructor */
void dtor(u32 p, u32 flags) {
    WWHD_FUNC(0x025F6220, void, p, flags);
    if (!p) return;
    store<u32>(p + 0xC, 0x100E09F0);
    if (p == load<u32>(0x101F4AEC)) {
        const u32 inst = load<u32>(kInstance);
        store<u32>(0x101F4AEC, 0);
        call<void>(0x02757644, inst + 0x10, 2u);
        store<u32>(kInstance, 0);
    }
    call<void>(0x02752BEC, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x025F6220, dtor);

/* 025F62C0: static initialiser (header statics + the TU's SafeString constants) */
void staticInit() {
    WWHD_FUNC(0x025F62C0, void);
    const u32 b = 0x1048D6B4;
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, 0x101F4AC4u);
    const f32 lo = load<f32>(0x100E0998), hi = load<f32>(0x100E099C);
    store<f32>(0x1048D6A8, lo);
    store<f32>(0x1048D6AC, hi);
    call<void>(0x028ED6F8, 0x1048D6B0u);
    call<void>(0x028F026C, 0x101F4AD0u);
    call<void>(0x028EAB2C, 0x1048D6B1u);
    call<void>(0x028F026C, 0x101F4ADCu);
    const u32 strs[][2] = {{0x1048D688, 0x100E09B4}, {0x1048D690, 0x100E09C0},
                           {0x1048D6C4, 0x100E09A0}, {0x1048D6CC, 0x100E09CC}, {0x1048D6D4, 0x100E09D8},
                           {0x1048D6DC, 0x100E09E4}, {0x1048D6A0, 0x100E09B0}, {0x1048D698, 0x100E09A8}};
    for (auto& s : strs) {
        store<u32>(s[0] + 4, kSafeStringVt);
        store<u32>(s[0], s[1]);
    }
}
VERIFY(0x025F62C0, staticInit);

} // namespace hd_msg_res_mgr
