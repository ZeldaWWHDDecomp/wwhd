/** HD message resource manager: MSBP projects, per-language MSBT sets,
 * numeric message IDs and layout Copy-tag resolution.
 * HD: this subsystem has no GameCube counterpart. Preserve bounded string scans,
 * virtual assure calls, allocation fallbacks and guest stack extents.
 */
#include "d/hd_msg_res_mgr.h"
using namespace gabi;
using MsgWord = be<u32>;
using MsgByte = be<u8>;
using MsgFloat = be<f32>;
static MsgWord l_instance;
static MsgWord l_disposerHolder;
static MsgWord l_resourceManager;
static MsgWord l_config;
static MsgLanguageNames l_languageNames;
static MsgLanguageMap l_languageMap;
static MsgWord l_emptyText;
static MsgWords4 l_headerZeros;
static MsgFloat l_piLowConstant;
static MsgFloat l_piHighConstant;
static MsgFloat l_piLow;
static MsgFloat l_piHigh;
static MsgByte l_headerAngle;
static MsgByte l_headerCircle;
static MsgWord l_rttiRootGuard;
static MsgWord l_rttiBaseGuard;
static MsgWord l_rttiLayoutGuard;
static MsgWord l_rttiTargetGuard;
static MsgWord l_rttiRoot;
static MsgWord l_rttiBase;
static MsgWord l_rttiLayout;
static MsgWord l_rttiTarget;
static MsgSafeString l_unitSetName;
static MsgSafeString l_copyTagName;
static MsgSafeString l_copyTagLowerName;
static MsgSafeString l_layoutSeparator;
static MsgSafeString l_pathSeparator;
static MsgSafeString l_extensionSeparator;
static MsgSafeString l_emptyString;
static MsgSafeString l_projectFilename;
static MsgSafeString l_projectArchive;
static MsgSafeString l_region;
static MsgSafeString l_country;
static MsgWord l_pathSeparatorGuard;
static MsgWord l_extensionSeparatorGuard;
static MsgWord l_emptyStringGuard;
static MsgWord l_unitSetNameGuard;
static MsgWord l_copyTagNameGuard;
static MsgWord l_copyTagLowerNameGuard;
static MsgWord l_layoutSeparatorGuard;
static MsgCommonSetNames l_commonSetNames;

static const u32 kSafeStringVt = msg_res_tu::safeString;
static const u32 kBufferedVt = msg_res_tu::bufferedString;
static const u32 kFixed256Vt = msg_res_tu::fixed256;

/* strlen capped at 0x40000 as sead does */
static s32 MsgResMgr_cstrLength(u32 p) {
    s32 n = 0;
    if (!(u8)(*gabi::at<be<u8>>(p)))
        return 0;
    for (;;) {
        n++;
        if (n > MSG_MAX_SCAN)
            return 0;
        p++;
        if (!(u8)(*gabi::at<be<u8>>(p)))
            return n;
    }
}

/* virtual assureTermination of a string object (vtable at +4) */
static void MsgResMgr_vAssure(u32 s) {
    call_ptr<void>((u32)(gabi::at<MsgStringVtable>((u32)(gabi::at<MsgSafeString>(s)->__vtbl))
                             ->mpAssureTermination),
                   s);
}

/* buffered dst = string src (both assure calls virtual; dst buffer given) */
static void MsgResMgr_copyStr(u32 dst, u32 buf, u32 src) {
    MsgResMgr_vAssure(src);
    s32 n = MsgResMgr_cstrLength((u32)(gabi::at<MsgSafeString>(src)->mpText));
    const s32 size = s32((u32)(gabi::at<MsgBufferedString>(dst)->mCapacity));
    const u32 fn = (u32)(gabi::at<MsgStringVtable>((u32)(gabi::at<MsgSafeString>(src)->__vtbl))
                             ->mpAssureTermination);
    if (n >= size)
        n = size - 1;
    call_ptr<void>(fn, src);
    MsgCopyBytes(buf, (u32)(gabi::at<MsgSafeString>(src)->mpText), u32(n), 0u);
    gabi::at<be<u8>>(buf)[n] = (u8)(0);
}

/* buffered dst = temporary SafeString tmp (first assure devirtualised to the empty one) */
static void MsgResMgr_copyTmp(u32 dst, u32 buf, u32 tmp) {
    MsgSafeStringAssureEmpty(tmp);
    s32 n = MsgResMgr_cstrLength((u32)(gabi::at<MsgSafeString>(tmp)->mpText));
    const s32 size = s32((u32)(gabi::at<MsgBufferedString>(dst)->mCapacity));
    const u32 fn = (u32)(gabi::at<MsgStringVtable>((u32)(gabi::at<MsgSafeString>(tmp)->__vtbl))
                             ->mpAssureTermination);
    if (n >= size)
        n = size - 1;
    call_ptr<void>(fn, tmp);
    MsgCopyBytes(buf, (u32)(gabi::at<MsgSafeString>(tmp)->mpText), u32(n), 0u);
    gabi::at<be<u8>>(buf)[n] = (u8)(0);
}

/* FixedSafeString<256> dst += literal (temporary SafeString at tmp) */
static void MsgResMgr_appendLit(u32 dst, u32 tmp, u32 lit) {
    gabi::at<MsgSafeString>(tmp)->__vtbl = (u32)(kSafeStringVt);
    gabi::at<MsgSafeString>(tmp)->mpText = (u32)(lit);
    const u32 base = (u32)(gabi::at<MsgBufferedString>(dst)->mpText);
    MsgBufferedStringAssure(dst);
    const s32 cur = MsgResMgr_cstrLength((u32)(gabi::at<MsgBufferedString>(dst)->mpText));
    const s32 c0 = cur < 0 ? 0 : cur;
    const u32 fn = (u32)(gabi::at<MsgStringVtable>((u32)(gabi::at<MsgSafeString>(tmp)->__vtbl))
                             ->mpAssureTermination);
    call_ptr<void>(fn, tmp);
    s32 n = MsgResMgr_cstrLength((u32)(gabi::at<MsgSafeString>(tmp)->mpText));
    const s32 avail = s32((u32)(gabi::at<MsgBufferedString>(dst)->mCapacity)) - c0;
    if (n >= avail)
        n = avail - 1;
    if (n <= 0)
        return;
    MsgResMgr_vAssure(tmp);
    MsgCopyBytes(base + u32(c0), (u32)(gabi::at<MsgSafeString>(tmp)->mpText), u32(n), 0u);
    if (c0 + n > cur)
        gabi::at<be<u8>>(base)[u32(c0 + n)] = (u8)(0);
}

/* sead string equality as inlined: a (vtable at +4) against b */
static bool MsgResMgr_strEqual(u32 a, u32 b) {
    MsgResMgr_vAssure(a);
    MsgResMgr_vAssure(a);
    const u32 ta = (u32)(gabi::at<MsgSafeString>(a)->mpText);
    MsgResMgr_vAssure(b);
    if (ta == (u32)(gabi::at<MsgSafeString>(b)->mpText))
        return true;
    u32 p = (u32)(gabi::at<MsgSafeString>(a)->mpText);
    const u32 q = (u32)(gabi::at<MsgSafeString>(b)->mpText);
    for (u32 k = 0; k < MSG_MAX_SCAN; k++) {
        const u32 c = (u8)(gabi::at<be<u8>>(p)[k]);
        const u32 d = (u8)(gabi::at<be<u8>>(q)[k]);
        if (!c)
            return d == 0;
        if (!d || d != c)
            return false;
    }
    return true;
}

// HD: constructor
/* 025F391C */
u32 MsgResMgr_ctor(u32 p) {
    if (!p) {
        p = MsgAllocate(0x438u);
        if (!p)
            return 0;
    }
    gabi::at<MsgResMgr>(p)->mProject.mpData = (u32)(0);
    gabi::at<MsgResMgr>(p)->mProject.__vtbl = (u32)(msg_res_tu::project);
    for (u32 off = 0x14; off <= 0x24; off += 8) {
        u32 q = p + off;
        if (!q)
            q = MsgAllocate(8u);
        if (q) {
            gabi::at<MsgPair>(q)->mSize = (u32)(0);
            gabi::at<MsgPair>(q)->mValue = (u32)(0);
        }
    }
    gabi::at<MsgResMgr>(p)->mProject.mNameCount = (u32)(0);
    gabi::at<MsgResMgr>(p)->mSetCount = (u32)(0);
    if (!(p + 0x38))
        MsgAllocate(0x400u);
    for (u32 i = 0; i < MSG_MAX_SETS; i++)
        gabi::at<MsgResMgr>(p)->mpSets[i] = (u32)(0);
    return p;
}

// HD: creates the singleton with its disposer
/* 025F3A14 */
void MsgResMgr_createInstance(u32 heap) {
    if ((u32)(l_instance))
        return;
    const u32 p = MsgAllocateManager(0x438u, heap, 4u);
    if (p) {
        MsgDisposerConstruct(p, heap, 3u);
        gabi::at<MsgResMgr>(p)->__vtbl = (u32)(msg_res_tu::disposer);
    }
    l_disposerHolder = (u32)(p);
    l_instance = (u32)(p ? MsgResMgr_ctor(p) : 0u);
}

// HD: language folder name (UsEnglish, UsFrench, ...) for a language id
/* 025F3AA8 */
u32 MsgResMgr_langName(u32 self, u32 lang) {
    if (lang < 1 || lang > 5)
        return (u32)(l_languageNames.mNames[0]);
    return (u32)(l_languageNames.mNames[(u8)(l_languageMap.mIndices[lang])]);
}

/* 025F3AE8: the MSBT data of a message set (loads
 * "<region>/<country>/Message/<lang>/<name>_msbt.szs" when the archive is not loaded yet) */
/* 025F3AE8 */
u32 MsgResMgr_openMsbt(u32 self, u32 heap, u32 set, u32 lang) {
    Local<u8[0x230]> F;
    const u32 tmp = F.a, info = F.a + 8, B = F.a + 0x10, Bbuf = F.a + 0x1C;
    const u32 A = F.a + 0x11C, Abuf = F.a + 0x128, path = F.a + 0x228;
    (void)path;
    /* A = set name + ".msbt" (file name in the archive) */
    gabi::at<MsgSafeString>(A)->__vtbl = (u32)(kBufferedVt);
    gabi::at<MsgSafeString>(A)->mpText = (u32)(Abuf);
    gabi::at<MsgBufferedString>(A)->mCapacity = (u32)(0x100);
    gabi::at<be<u8>>(Abuf)[0xFF] = (u8)(0);
    MsgResMgr_copyStr(A, Abuf, set + 0xC);
    gabi::at<MsgSafeString>(A)->__vtbl = (u32)(kFixed256Vt);
    MsgResMgr_appendLit(A, tmp, msg_res_tu::msbtExtension);
    /* B = set name + "_msbt" (archive name) */
    gabi::at<MsgBufferedString>(B)->mCapacity = (u32)(0x100);
    gabi::at<be<u8>>(Bbuf)[0xFF] = (u8)(0);
    gabi::at<MsgSafeString>(B)->mpText = (u32)(Bbuf);
    gabi::at<MsgSafeString>(B)->__vtbl = (u32)(kBufferedVt);
    MsgResMgr_copyStr(B, Bbuf, set + 0xC);
    gabi::at<MsgSafeString>(B)->__vtbl = (u32)(kFixed256Vt);
    MsgResMgr_appendLit(B, tmp, msg_res_tu::archiveSuffix);
    u32 res = MsgFindResource((u32)(l_resourceManager), B);
    if (!res) {
        const u32 mgr = (u32)(l_resourceManager);
        MsgResMgr_vAssure(gabi::ea(&l_region));
        const u32 region = (u32)(l_region.mpText);
        MsgResMgr_vAssure(gabi::ea(&l_country));
        const u32 country = (u32)(l_country.mpText);
        MsgResMgr_vAssure(B);
        const u32 name = (u32)(gabi::at<MsgSafeString>(B)->mpText);
        const u32 ln = MsgResMgr_langName(self, lang);
        Local<u8[0x110]> P;
        const u32 p = MsgFormatPath(P.a, msg_res_tu::setPathFormat, region, country, ln, name);
        if (!MsgLoadResource(mgr, B, p, heap, 1u, 0x40000u, 0x2000u))
            return 0;
        res = MsgFindResource((u32)(l_resourceManager), B);
        if (!res)
            return 0;
    }
    gabi::at<MsgPair>(info)->mSize = (u32)(0);
    gabi::at<MsgPair>(info)->mValue = (u32)(0);
    return call_ptr<u32>(
        (u32)(gabi::at<MsgResourceVtable>((u32)(gabi::at<MsgResource>(res)->__vtbl))->mpGetFile),
        res, A, info);
}

// HD: creates one message set (name copied, MSBT opened and bound)
/* 025F3FB4 */
void MsgResMgr_loadSet(u32 self, u32 out, u32 heap, u32 name, u32 lang) {
    const u32 obj = MsgAllocateSet(0x118u, heap, 4u);
    if (obj) {
        u32 h = obj;
        if (!h)
            h = MsgAllocate(0xCu);
        if (h) {
            gabi::at<MsgSet>(h)->mpData = (u32)(0);
            gabi::at<MsgSet>(h)->mEntryCount = (u32)(0);
            gabi::at<MsgSet>(h)->__vtbl = (u32)(msg_res_tu::messageSet);
        }
        u32 s = obj + 0xC;
        bool ok = true;
        if (!s) {
            s = MsgAllocate(0x10Cu);
            if (!s)
                ok = false;
        }
        if (ok) {
            u32 b = s;
            if (!b)
                b = MsgAllocate(0xCu);
            if (b) {
                gabi::at<MsgSafeString>(b)->mpText = (u32)(s + 0xC);
                gabi::at<MsgSafeString>(b)->__vtbl = (u32)(msg_res_tu::buffered256Base);
                gabi::at<MsgBufferedString>(b)->mCapacity = (u32)(0x100);
                gabi::at<be<u8>>(s)[0x10B] = (u8)(0);
            }
            const u32 top = (u32)(gabi::at<MsgSafeString>(s)->mpText);
            gabi::at<MsgSafeString>(s)->__vtbl = (u32)(kBufferedVt);
            *gabi::at<be<u8>>(top) = (u8)(0);
            gabi::at<MsgSafeString>(s)->__vtbl = (u32)(kFixed256Vt);
        }
    }
    *gabi::at<be<u32>>(out) = (u32)(obj);
    Local<u32[2]> T;
    gabi::at<MsgSafeString>(T.a)->__vtbl = (u32)(kSafeStringVt);
    gabi::at<MsgSafeString>(T.a)->mpText = (u32)((u32)(gabi::at<MsgSafeString>(name)->mpText));
    const u32 buf = (u32)(gabi::at<MsgSet>(obj)->mName.mpText);
    MsgResMgr_copyTmp(obj + 0xC, buf, T.a);
    const u32 data = MsgResMgr_openMsbt(self, heap, obj, lang);
    if (data)
        MsgSetBind(obj, data, heap);
}

/* function-local static SafeString {lit} (guard, object, atexit record) */
static void MsgResMgr_localStr(u32 guard, u32 obj, u32 lit, u32 rec) {
    if ((u32)(*gabi::at<be<u32>>(guard)))
        return;
    *gabi::at<be<u32>>(guard) = (u32)(1);
    gabi::at<MsgSafeString>(obj)->__vtbl = (u32)(kSafeStringVt);
    gabi::at<MsgSafeString>(obj)->mpText = (u32)(lit);
    MsgRegisterGlobal(rec);
}

/* sead rfindIndex of pat in s (temporary SafeStrings at tmp), -1 when absent */
static s32 MsgResMgr_rfindIndex(u32 s, u32 pat, u32 tmp) {
    MsgResMgr_vAssure(s);
    const s32 l1 = MsgResMgr_cstrLength((u32)(gabi::at<MsgSafeString>(s)->mpText));
    MsgResMgr_vAssure(pat);
    const s32 l2 = MsgResMgr_cstrLength((u32)(gabi::at<MsgSafeString>(pat)->mpText));
    s32 i = l1 - l2;
    if (i < 0)
        return -1;
    for (s32 cnt = i + 1; cnt; cnt--, i--) {
        gabi::at<MsgSafeString>(tmp)->__vtbl = (u32)(kSafeStringVt);
        gabi::at<MsgSafeString>(tmp)->mpText =
            (u32)((u32)(gabi::at<MsgSafeString>(s)->mpText) + u32(i));
        MsgSafeStringAssureEmpty(tmp);
        MsgResMgr_vAssure(tmp);
        const u32 t = (u32)(gabi::at<MsgSafeString>(tmp)->mpText);
        MsgResMgr_vAssure(pat);
        const u32 q = (u32)(gabi::at<MsgSafeString>(pat)->mpText);
        if (t == q || l2 <= 0)
            return i;
        const u32 p = (u32)(gabi::at<MsgSafeString>(tmp)->mpText);
        bool eq = true;
        for (s32 k = 0; k < l2; k++) {
            const u32 c = (u8)(gabi::at<be<u8>>(p)[u32(k)]), d = (u8)(gabi::at<be<u8>>(q)[u32(k)]);
            if (!c) {
                eq = d == 0;
                break;
            }
            if (!d || d != c) {
                eq = false;
                break;
            }
        }
        if (eq)
            return i;
    }
    return -1;
}

// HD: creates every message set named by the project (file name without folder/extension)
/* 025F416C */
void MsgResMgr_loadSets(u32 self, u32 heap) {
    const u32 cnt = (u32)(gabi::at<MsgResMgr>(self)->mProject.mNameCount);
    gabi::at<MsgResMgr>(self)->mSetCount = (u32)(cnt);
    const u32 lang = (u32)(gabi::at<MsgConfig>((u32)(l_config))->mLanguage);
    if (s32(cnt) <= 0)
        return;
    Local<u8[0x220]> F;
    const u32 T = F.a, P = F.a + 8, Pbuf = F.a + 0x14, N = F.a + 0x114, Nbuf = F.a + 0x120;
    Local<u32[2]> T30;
    s32 left = s32(cnt);
    for (u32 i = 0;; i++) {
        gabi::at<be<u8>>(Nbuf)[0xFF] = (u8)(0);
        gabi::at<MsgSafeString>(N)->mpText = (u32)(Nbuf);
        *gabi::at<be<u8>>(Nbuf) = (u8)(0);
        gabi::at<MsgBufferedString>(N)->mCapacity = (u32)(0x100);
        gabi::at<MsgSafeString>(N)->__vtbl = (u32)(kFixed256Vt);
        u32 s = 0;
        if (i < (u32)(gabi::at<MsgResMgr>(self)->mProject.mNameCount))
            s = MsgGetProjectName((u32)(gabi::at<MsgResMgr>(self)->mProject.mpData), i);
        gabi::at<MsgSafeString>(T)->mpText = (u32)(s);
        gabi::at<MsgBufferedString>(P)->mCapacity = (u32)(0x100);
        gabi::at<MsgSafeString>(T)->__vtbl = (u32)(kSafeStringVt);
        gabi::at<be<u8>>(Pbuf)[0xFF] = (u8)(0);
        gabi::at<MsgSafeString>(P)->mpText = (u32)(Pbuf);
        gabi::at<MsgSafeString>(P)->__vtbl = (u32)(kBufferedVt);
        MsgResMgr_copyTmp(P, Pbuf, T);
        gabi::at<MsgSafeString>(P)->__vtbl = (u32)(kFixed256Vt);
        /* N = part after the last '/' */
        MsgResMgr_localStr(gabi::ea(&l_pathSeparatorGuard), gabi::ea(&l_pathSeparator),
                           msg_res_tu::pathSeparator, msg_res_tu::pathSeparatorDtor);
        const s32 slash = MsgResMgr_rfindIndex(P, gabi::ea(&l_pathSeparator), T30.a);
        if (slash >= 0) {
            MsgResMgr_vAssure(P);
            const u32 top = (u32)(gabi::at<MsgSafeString>(P)->mpText);
            const s32 plen = MsgResMgr_cstrLength(top);
            const s32 at = slash + 1;
            const u32 src = (at < 0 || at > plen) ? (u32)(l_emptyText) : top + u32(at);
            Local<u32[2]> T40;
            gabi::at<MsgSafeString>(T40.a)->__vtbl = (u32)(kSafeStringVt);
            gabi::at<MsgSafeString>(T40.a)->mpText = (u32)(src);
            MsgResMgr_copyTmp(N, (u32)(gabi::at<MsgSafeString>(N)->mpText), T40.a);
        } else {
            MsgResMgr_copyStr(N, (u32)(gabi::at<MsgSafeString>(N)->mpText), P);
        }
        /* cut at the last '.' */
        MsgResMgr_localStr(gabi::ea(&l_extensionSeparatorGuard), gabi::ea(&l_extensionSeparator),
                           msg_res_tu::extensionSeparator, msg_res_tu::extensionSeparatorDtor);
        s32 dot = MsgResMgr_rfindIndex(N, gabi::ea(&l_extensionSeparator), T30.a);
        MsgResMgr_localStr(gabi::ea(&l_emptyStringGuard), gabi::ea(&l_emptyString),
                           msg_res_tu::emptyString, msg_res_tu::emptyStringDtor);
        const u32 ntop = (u32)(gabi::at<MsgSafeString>(N)->mpText);
        MsgResMgr_vAssure(N);
        const s32 nlen = MsgResMgr_cstrLength((u32)(gabi::at<MsgSafeString>(N)->mpText));
        if (dot < 0) {
            dot = nlen + dot + 1;
            if (dot < 0)
                dot = 0;
        }
        const s32 avail = s32((u32)(gabi::at<MsgBufferedString>(N)->mCapacity)) - dot;
        s32 n = 1;
        if (avail <= 1)
            n = avail - 1;
        if (n > 0) {
            const u32 dst = ntop + u32(dot);
            MsgResMgr_vAssure(gabi::ea(&l_emptyString));
            MsgCopyBytes(dst, (u32)(l_emptyString.mpText), u32(n), 0u);
            if (dot + n > nlen)
                gabi::at<be<u8>>(ntop)[u32(dot + n)] = (u8)(0);
        }
        MsgResMgr_loadSet(self, i < MSG_MAX_SETS ? self + 0x38 + 4 * i : self + 0x38, heap, N,
                          lang);
        if (--left == 0)
            return;
    }
}

// HD: loads the project (CKing.msbp from CKing_msbp.szs) and every message set
/* 025F4A60 */
void MsgResMgr_loadProject(u32 self, u32 heap) {
    const u32 cfg = (u32)(l_config);
    const u32 arc = gabi::ea(&l_projectArchive);
    const u32 mgr0 = (u32)(l_resourceManager);
    const u32 lang = (u32)(gabi::at<MsgConfig>(cfg)->mLanguage);
    u32 res = MsgFindResource(mgr0, arc);
    if (!res) {
        const u32 mgr = (u32)(l_resourceManager);
        MsgResMgr_vAssure(gabi::ea(&l_region));
        const u32 region = (u32)(l_region.mpText);
        MsgResMgr_vAssure(gabi::ea(&l_country));
        const u32 country = (u32)(l_country.mpText);
        MsgResMgr_vAssure(arc);
        const u32 name = (u32)(gabi::at<MsgSafeString>(arc)->mpText);
        const u32 ln = MsgResMgr_langName(self, lang);
        Local<u8[0x110]> P;
        const u32 p = MsgFormatPath(P.a, msg_res_tu::projectPathFormat, region, country, ln, name);
        if (MsgLoadResource(mgr, arc, p, heap, 1u, 0x40000u, 0x2000u))
            res = MsgFindResource((u32)(l_resourceManager), arc);
    }
    Local<u32[2]> info;
    gabi::at<MsgPair>(info.a)->mSize = (u32)(0);
    gabi::at<MsgPair>(info.a)->mValue = (u32)(0);
    const u32 data = call_ptr<u32>(
        (u32)(gabi::at<MsgResourceVtable>((u32)(gabi::at<MsgResource>(res)->__vtbl))->mpGetFile),
        res, gabi::ea(&l_projectFilename), info.a);
    MsgProjectBind(self + 0x10, data, heap);
    MsgResMgr_loadSets(self, heap);
}

// HD: the project object
/* 025F4BC0 */
u32 MsgResMgr_project(u32 self) { return self + 0x10; }

// HD: the message set of that name (0 when none)
/* 025F4BC8 */
u32 MsgResMgr_findSet(u32 self, u32 name, u32 arr, s32 count) {
    if (count <= 0)
        return 0;
    u32 i = 0, off = 0, r28 = 0;
    for (;;) {
        u32 p;
        if (i < MSG_MAX_SETS) {
            p = arr + off;
            r28 = off;
        } else {
            p = arr;
        }
        const u32 e = (u32)(*gabi::at<be<u32>>(p));
        if (MsgResMgr_strEqual(e + 0xC, name)) {
            if (i < MSG_MAX_SETS)
                arr += r28;
            return (u32)(*gabi::at<be<u32>>(arr));
        }
        r28 += 4;
        i++;
        off += 4;
        if (--count == 0)
            return 0;
    }
}

// HD: the "unitString" message set
/* 025F4CFC */
u32 MsgResMgr_unitSet(u32 self) {
    const u32 s = gabi::ea(&l_unitSetName);
    if (!(u32)(l_unitSetNameGuard)) {
        l_unitSetNameGuard = (u32)(1);
        gabi::at<MsgSafeString>(s)->mpText = (u32)(msg_res_tu::unitSetName);
        gabi::at<MsgSafeString>(s)->__vtbl = (u32)(kSafeStringVt);
        MsgRegisterGlobal(msg_res_tu::unitSetNameDtor);
    }
    return MsgResMgr_findSet(self, s, self + 0x38,
                             s32((u32)(gabi::at<MsgResMgr>(self)->mSetCount)));
}

// HD: index of the message set of that name (-1 when none)
/* 025F4D84 */
s32 MsgResMgr_findSetIndex(u32 self, u32 name, u32 arr, s32 count) {
    if (count <= 0)
        return -1;
    u32 i = 0, q = arr;
    for (;;) {
        const u32 p = i < MSG_MAX_SETS ? q : arr;
        const u32 e = (u32)(*gabi::at<be<u32>>(p));
        if (MsgResMgr_strEqual(e + 0xC, name))
            return s32(i);
        i++;
        q += 4;
        if (--count == 0)
            return -1;
    }
}

// HD: the message set holding a label, searched in the named set
/* 025F4EA4 */
u32 MsgResMgr_setWithLabel(u32 self, u32 name, u32 label, u32 arr, s32 count) {
    const s32 idx = MsgResMgr_findSetIndex(self, name, arr, count);
    if (idx < 0)
        return 0;
    u32 p = arr;
    const u32 vt = (u32)(gabi::at<MsgSafeString>(label)->__vtbl);
    if (u32(idx) < MSG_MAX_SETS)
        p = arr + (u32(idx) << 2);
    const u32 fn = (u32)(gabi::at<MsgStringVtable>(vt)->mpAssureTermination);
    const u32 e = (u32)(*gabi::at<be<u32>>(p));
    call_ptr<void>(fn, label);
    const u32 li = MsgFindLabel((u32)(gabi::at<MsgSet>(e)->mpData),
                                (u32)(gabi::at<MsgSafeString>(label)->mpText));
    if (li >= MSG_LABEL_NOT_FOUND)
        return 0;
    if (!MsgSetGetEntry(e, li))
        return 0;
    return e;
}

// HD: the message set holding a label, searched in "message".."message4"
/* 025F4F48 */
u32 MsgResMgr_setWithLabelAny(u32 self, u32 label, u32 arr, s32 count) {
    for (u32 k = 0; k < 4; k++) {
        const u32 r =
            MsgResMgr_setWithLabel(self, gabi::ea(&l_commonSetNames.mNames[k]), label, arr, count);
        if (r)
            return r;
    }
    return 0;
}

// HD: same over the manager's own sets
/* 025F4FC0 */
u32 MsgResMgr_setWithLabelOwn(u32 self, u32 label) {
    return MsgResMgr_setWithLabelAny(self, label, self + 0x38,
                                     s32((u32)(gabi::at<MsgResMgr>(self)->mSetCount)));
}

// HD: does the label start with a digit (numeric message id)?
/* 025F4FCC */
u32 MsgResMgr_isNumericLabel(u32 self, u32 s) {
    MsgResMgr_vAssure(s);
    const u32 top = (u32)(gabi::at<MsgSafeString>(s)->mpText);
    u32 p = top;
    s32 n = 0;
    if ((u8)(*gabi::at<be<u8>>(p))) {
        for (;;) {
            n++;
            if (n > MSG_MAX_SCAN)
                break;
            p++;
            if (!(u8)(*gabi::at<be<u8>>(p)))
                break;
        }
    }
    (void)n;
    return u32((u8)(*gabi::at<be<u8>>(top))) - 0x30 < 10 ? 1u : 0u;
}

/* 025F506C: the message set of a label (numeric labels: any "message*" set; else the named set) */
/* 025F506C */
u32 MsgResMgr_setForLabel(u32 self, u32 name, u32 label) {
    if (MsgResMgr_isNumericLabel(self, label))
        return MsgResMgr_setWithLabelAny(self, label, self + 0x38,
                                         s32((u32)(gabi::at<MsgResMgr>(self)->mSetCount)));
    return MsgResMgr_setWithLabel(self, name, label, self + 0x38,
                                  s32((u32)(gabi::at<MsgResMgr>(self)->mSetCount)));
}

// HD: looks up a message: fills the result object (set, names, label index, entry)
/* 025F50E8 */
void MsgResMgr_getMessage(u32 self, u32 out, u32 name, u32 label) {
    const u32 e = MsgResMgr_setForLabel(self, name, label);
    if (!e) {
        gabi::at<MsgResult>(out)->mpSet = (u32)(0);
        return;
    }
    MsgResMgr_vAssure(label);
    const u32 li = MsgFindLabel((u32)(gabi::at<MsgSet>(e)->mpData),
                                (u32)(gabi::at<MsgSafeString>(label)->mpText));
    if (li >= MSG_LABEL_NOT_FOUND) {
        gabi::at<MsgResult>(out)->mpSet = (u32)(0);
        return;
    }
    u32 entry = 0;
    if (li < (u32)(gabi::at<MsgSet>(e)->mEntryCount))
        entry = MsgGetAttribute((u32)(gabi::at<MsgSet>(e)->mpData), li);
    /* set name */
    if (!MsgResMgr_strEqual(out + 0xC, name)) {
        const u32 dst = (u32)(gabi::at<MsgResult>(out)->mSetName.mpText);
        MsgResMgr_vAssure(name);
        s32 n = MsgResMgr_cstrLength((u32)(gabi::at<MsgSafeString>(name)->mpText));
        const s32 size = s32((u32)(gabi::at<MsgResult>(out)->mSetName.mCapacity));
        const u32 fn = (u32)(gabi::at<MsgStringVtable>((u32)(gabi::at<MsgSafeString>(name)->__vtbl))
                                 ->mpAssureTermination);
        if (n >= size)
            n = size - 1;
        call_ptr<void>(fn, name);
        MsgCopyBytes(dst, (u32)(gabi::at<MsgSafeString>(name)->mpText), u32(n), 0u);
        gabi::at<be<u8>>(dst)[n] = (u8)(0);
    }
    gabi::at<MsgResult>(out)->mLabelIndex = (u32)(li);
    /* label */
    if (!MsgResMgr_strEqual(out + 0x120, label)) {
        const u32 dst = (u32)(gabi::at<MsgResult>(out)->mLabel.mpText);
        MsgResMgr_vAssure(label);
        s32 n = MsgResMgr_cstrLength((u32)(gabi::at<MsgSafeString>(label)->mpText));
        const s32 size = s32((u32)(gabi::at<MsgResult>(out)->mLabel.mCapacity));
        const u32 fn =
            (u32)(gabi::at<MsgStringVtable>((u32)(gabi::at<MsgSafeString>(label)->__vtbl))
                      ->mpAssureTermination);
        if (n >= size)
            n = size - 1;
        call_ptr<void>(fn, label);
        MsgCopyBytes(dst, (u32)(gabi::at<MsgSafeString>(label)->mpText), u32(n), 0u);
        gabi::at<be<u8>>(dst)[n] = (u8)(0);
    }
    gabi::at<MsgResult>(out)->mpEntry = (u32)(entry);
    gabi::at<MsgResult>(out)->mpSet = (u32)(e);
}

/* sead RTTI: the guarded chain of static type infos ending at 101FDD3C (the type the tag target is
 * checked against), initialised on first use */
static void MsgResMgr_rttiChain() {
    if ((u32)(l_rttiTargetGuard))
        return;
    const u32 g1 = (u32)(l_rttiLayoutGuard);
    l_rttiTargetGuard = (u32)(1);
    if (!g1) {
        const u32 g2 = (u32)(l_rttiBaseGuard);
        l_rttiLayoutGuard = (u32)(1);
        if (!g2) {
            const u32 g3 = (u32)(l_rttiRootGuard);
            l_rttiBaseGuard = (u32)(1);
            if (!g3) {
                l_rttiRoot = (u32)(0);
                l_rttiRootGuard = (u32)(1);
            }
            l_rttiBase = (u32)(gabi::ea(&l_rttiRoot));
        }
        l_rttiLayout = (u32)(gabi::ea(&l_rttiBase));
    }
    l_rttiTarget = (u32)(gabi::ea(&l_rttiLayout));
}

/* is the object (RTTI getter at vtable(+8)+0x14) of the type 101FDD3C or derived from it? */
static bool MsgResMgr_isTagTarget(u32 o) {
    const u32 t0 = call_ptr<u32>(
        (u32)(gabi::at<MsgLayoutVtable>((u32)gabi::at<MsgLayout>(o)->__vtbl)->mpGetRtti), o);
    if (!t0)
        return false;
    for (u32 t = t0; t; t = (u32)(*gabi::at<be<u32>>(t)))
        if (t == gabi::ea(&l_rttiTarget))
            return true;
    return false;
}

/* label = the target's label string (+0x128) */
static void MsgResMgr_copyTargetLabel(u32 o, u32 S256) {
    const u32 dst = (u32)(gabi::at<MsgSafeString>(S256)->mpText);
    MsgResMgr_vAssure(o + 0x128);
    s32 n = MsgResMgr_cstrLength((u32)(gabi::at<MsgTagTarget>(o)->mLabel.mpText));
    const s32 size = s32((u32)(gabi::at<MsgBufferedString>(S256)->mCapacity));
    const u32 fn = (u32)(gabi::at<MsgStringVtable>((u32)(gabi::at<MsgTagTarget>(o)->mLabel.__vtbl))
                             ->mpAssureTermination);
    if (n >= size)
        n = size - 1;
    call_ptr<void>(fn, o + 0x128);
    MsgCopyBytes(dst, (u32)(gabi::at<MsgTagTarget>(o)->mLabel.mpText), u32(n), 0u);
    gabi::at<be<u8>>(dst)[n] = (u8)(0);
}

/* finds a child of a layout object by name (vtable(+8)+0x5C) */
static u32 MsgResMgr_findChild(u32 obj, u32 str) {
    const u32 vt = (u32)(gabi::at<MsgLayout>(obj)->__vtbl);
    MsgResMgr_vAssure(str);
    return call_ptr<u32>((u32)(gabi::at<MsgLayoutVtable>(vt)->mpFindChild), obj,
                         (u32)(gabi::at<MsgSafeString>(str)->mpText), 1u);
}

/* 025F5444: resolves a message for a layout text box: "Copy"/"copy" tags redirect the label to the
 * label of another text box ("<pane>" or "<layout>:<pane>" style target names) */
/* 025F5444 */
void MsgResMgr_resolveMessage(u32 self, u32 out, u32 box, u32 name, u32 text) {
    Local<u8[0x50]> L64; /* FixedSafeString<64> set name */
    const u32 S64 = L64.a, S64buf = L64.a + 0xC;
    Local<u8[0x110]> L256; /* FixedSafeString<256> label */
    const u32 S256 = L256.a, S256buf = L256.a + 0xC;
    Local<u32[2]> T;
    gabi::at<MsgSafeString>(S64)->mpText = (u32)(S64buf);
    gabi::at<MsgSafeString>(S64)->__vtbl = (u32)(msg_res_tu::buffered64Base);
    gabi::at<MsgSafeString>(T.a)->__vtbl = (u32)(kSafeStringVt);
    gabi::at<be<u8>>(S64buf)[0x3F] = (u8)(0);
    gabi::at<MsgBufferedString>(S64)->mCapacity = (u32)(0x40);
    gabi::at<MsgSafeString>(T.a)->mpText = (u32)((u32)(gabi::at<MsgLayout>(box)->mpSetName));
    MsgResMgr_copyTmp(S64, S64buf, T.a);
    gabi::at<MsgSafeString>(S64)->__vtbl = (u32)(msg_res_tu::fixed64);
    MsgResMgr_vAssure(name);
    Local<u32[2]> T18;
    gabi::at<MsgSafeString>(T18.a)->__vtbl = (u32)(kSafeStringVt);
    gabi::at<MsgSafeString>(T18.a)->mpText = (u32)((u32)(gabi::at<MsgSafeString>(name)->mpText));
    gabi::at<MsgSafeString>(S256)->__vtbl = (u32)(kBufferedVt);
    gabi::at<MsgSafeString>(S256)->mpText = (u32)(S256buf);
    gabi::at<MsgBufferedString>(S256)->mCapacity = (u32)(0x100);
    gabi::at<be<u8>>(S256buf)[0xFF] = (u8)(0);
    MsgResMgr_copyTmp(S256, S256buf, T18.a);
    gabi::at<MsgSafeString>(S256)->__vtbl = (u32)(kFixed256Vt);
    if (MsgResMgr_isNumericLabel(self, S256) ||
        !(u32)gabi::at<MsgLayout>((u32)gabi::at<MsgLayout>(box)->mpLayout)->mpLayout) {
        MsgResMgr_getMessage(self, out, S64, S256);
        return;
    }
    const u32 parent = (u32)(gabi::at<MsgLayout>(box)->mpParent);
    {
        const u32 dst = (u32)(gabi::at<MsgSafeString>(S64)->mpText);
        Local<u32[2]> T20;
        gabi::at<MsgSafeString>(T20.a)->__vtbl = (u32)(kSafeStringVt);
        gabi::at<MsgSafeString>(T20.a)->mpText =
            (u32)((u32)(gabi::at<MsgLayout>(parent)->mpSetName));
        MsgResMgr_copyTmp(S64, dst, T20.a);
    }
    /* the "Copy" / "copy" tag */
    u32 tag;
    {
        MsgResMgr_localStr(gabi::ea(&l_copyTagNameGuard), gabi::ea(&l_copyTagName),
                           msg_res_tu::copyTag, msg_res_tu::copyTagDtor);
        MsgResMgr_vAssure(gabi::ea(&l_copyTagName));
        const u32 key = (u32)(l_copyTagName.mpText);
        const u32 a = MsgTextTagBegin(text);
        const u32 b = MsgTextTagEnd(text);
        tag = MsgFindTextTag(key, a, b, 0u);
    }
    if (!tag) {
        MsgResMgr_localStr(gabi::ea(&l_copyTagLowerNameGuard), gabi::ea(&l_copyTagLowerName),
                           msg_res_tu::copyTagLower, msg_res_tu::copyTagLowerDtor);
        MsgResMgr_vAssure(gabi::ea(&l_copyTagLowerName));
        const u32 key = (u32)(l_copyTagLowerName.mpText);
        const u32 a = MsgTextTagBegin(text);
        const u32 b = MsgTextTagEnd(text);
        tag = MsgFindTextTag(key, a, b, 0u);
        if (!tag) {
            MsgResMgr_getMessage(self, out, S64, S256);
            return;
        }
    }
    const u32 val = tag + (u32)(gabi::at<MsgTag>(tag)->mValueOffset);
    if (!val) {
        MsgResMgr_getMessage(self, out, S64, S256);
        return;
    }
    /* S128 = the tag value (target name) */
    Local<u8[0x90]> L128;
    const u32 S128 = L128.a, S128buf = L128.a + 0xC;
    {
        Local<u32[2]> T28;
        gabi::at<MsgSafeString>(T28.a)->__vtbl = (u32)(kSafeStringVt);
        gabi::at<MsgSafeString>(S128)->mpText = (u32)(S128buf);
        gabi::at<MsgSafeString>(T28.a)->mpText = (u32)(val);
        gabi::at<MsgBufferedString>(S128)->mCapacity = (u32)(0x80);
        gabi::at<be<u8>>(S128buf)[0x7F] = (u8)(0);
        gabi::at<MsgSafeString>(S128)->__vtbl = (u32)(msg_res_tu::buffered128Base);
        MsgResMgr_copyTmp(S128, S128buf, T28.a);
    }
    gabi::at<MsgSafeString>(S128)->__vtbl = (u32)(msg_res_tu::fixed128);
    /* forward search of the separator 1048D668 */
    MsgResMgr_localStr(gabi::ea(&l_layoutSeparatorGuard), gabi::ea(&l_layoutSeparator),
                       msg_res_tu::layoutSeparator, msg_res_tu::layoutSeparatorDtor);
    MsgResMgr_vAssure(S128);
    const s32 l1 = MsgResMgr_cstrLength((u32)(gabi::at<MsgSafeString>(S128)->mpText));
    MsgResMgr_vAssure(gabi::ea(&l_layoutSeparator));
    const s32 l2 = MsgResMgr_cstrLength((u32)(l_layoutSeparator.mpText));
    const s32 last = l1 - l2;
    s32 idx = -1;
    Local<u32[2]> T8;
    for (s32 i = 0; i <= last; i++) {
        gabi::at<MsgSafeString>(T8.a)->__vtbl = (u32)(kSafeStringVt);
        gabi::at<MsgSafeString>(T8.a)->mpText =
            (u32)((u32)(gabi::at<MsgSafeString>(S128)->mpText) + u32(i));
        MsgSafeStringAssureEmpty(T8.a);
        MsgResMgr_vAssure(T8.a);
        const u32 t = (u32)(gabi::at<MsgSafeString>(T8.a)->mpText);
        MsgResMgr_vAssure(gabi::ea(&l_layoutSeparator));
        const u32 q = (u32)(l_layoutSeparator.mpText);
        bool eq = true;
        if (t != q && l2 > 0) {
            const u32 p = (u32)(gabi::at<MsgSafeString>(T8.a)->mpText);
            for (s32 k = 0; k < l2; k++) {
                const u32 c = (u8)(gabi::at<be<u8>>(p)[u32(k)]),
                          d = (u8)(gabi::at<be<u8>>(q)[u32(k)]);
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
        const u32 lay = (u32)(gabi::at<MsgLayout>(box)->mpLayout);
        const u32 vt = (u32)(gabi::at<MsgLayout>(lay)->__vtbl);
        MsgResMgr_vAssure(S128);
        const u32 o = call_ptr<u32>((u32)(gabi::at<MsgLayoutVtable>(vt)->mpFindChild), lay,
                                    (u32)(gabi::at<MsgSafeString>(S128)->mpText), 1u);
        MsgResMgr_rttiChain();
        if (o && MsgResMgr_isTagTarget(o))
            MsgResMgr_copyTargetLabel(o, S256);
        MsgResMgr_getMessage(self, out, S64, S256);
        return;
    }
    /* "<layout><sep><pane>" */
    Local<u8[0x90]> LA;
    const u32 A = LA.a, Abuf = LA.a + 0xC;
    *gabi::at<be<u8>>(Abuf) = (u8)(0);
    gabi::at<MsgSafeString>(A)->__vtbl = (u32)(msg_res_tu::fixed128);
    s32 n = idx < 0x80 ? idx : 0x7F;
    gabi::at<be<u8>>(Abuf)[0x7F] = (u8)(0);
    gabi::at<MsgSafeString>(A)->mpText = (u32)(Abuf);
    gabi::at<MsgBufferedString>(A)->mCapacity = (u32)(0x80);
    MsgResMgr_vAssure(S128);
    MsgCopyBytes(Abuf, (u32)(gabi::at<MsgSafeString>(S128)->mpText), u32(n), 0u);
    gabi::at<be<u8>>(Abuf)[n] = (u8)(0);
    const u32 lay = MsgResMgr_findChild((u32)(gabi::at<MsgLayout>(parent)->mpLayout), A);
    if (!lay) {
        MsgResMgr_getMessage(self, out, S64, S256);
        return;
    }
    Local<u8[0x90]> LB;
    const u32 B = LB.a, Bbuf = LB.a + 0xC;
    *gabi::at<be<u8>>(Bbuf) = (u8)(0);
    gabi::at<MsgSafeString>(B)->__vtbl = (u32)(msg_res_tu::fixed128);
    gabi::at<MsgBufferedString>(B)->mCapacity = (u32)(0x80);
    gabi::at<MsgSafeString>(B)->mpText = (u32)(Bbuf);
    gabi::at<be<u8>>(Bbuf)[0x7F] = (u8)(0);
    MsgResMgr_vAssure(S128);
    {
        Local<u32[2]> T30;
        gabi::at<MsgSafeString>(T30.a)->__vtbl = (u32)(kSafeStringVt);
        gabi::at<MsgSafeString>(T30.a)->mpText =
            (u32)((u32)(gabi::at<MsgSafeString>(S128)->mpText) + u32(idx) + 1);
        MsgResMgr_copyTmp(B, (u32)(gabi::at<MsgSafeString>(B)->mpText), T30.a);
    }
    const u32 o = MsgResMgr_findChild(lay, B);
    MsgResMgr_rttiChain();
    if (o && MsgResMgr_isTagTarget(o))
        MsgResMgr_copyTargetLabel(o, S256);
    MsgResMgr_getMessage(self, out, S64, S256);
}

// HD: parses a 5-digit message number
/* 025F5FEC */
void MsgResMgr_parseNumber5(u32 self, u32 out, u32 s) {
    if (!out || !s)
        return;
    MsgResMgr_vAssure(s);
    u32 p = (u32)(gabi::at<MsgSafeString>(s)->mpText);
    s32 n = 0;
    if ((u8)(*gabi::at<be<u8>>(p))) {
        for (;;) {
            n++;
            if (n > MSG_MAX_SCAN)
                return;
            p++;
            if (!(u8)(*gabi::at<be<u8>>(p)))
                break;
        }
    }
    if (n != 5)
        return;
    MsgResMgr_vAssure(s);
    u32 v = 0, m = 10000;
    const u32 t = (u32)(gabi::at<MsgSafeString>(s)->mpText);
    for (u32 k = 0; k < 5; k++) {
        const u32 c = (u8)(gabi::at<be<u8>>(t)[k]);
        if (c - 0x30 >= 10)
            return;
        v += m * (c - 0x30);
        m = m / 10;
    }
    *gabi::at<be<u32>>(out) = (u32)(v);
}

// HD: finds the message whose attribute id (+0xC, s16) matches and returns its number
/* 025F60EC */
u32 MsgResMgr_findById(u32 self, u32 labelOut, u32 numOut, u32 id) {
    if (!labelOut || !numOut)
        return 0;
    const u32 arr = self + 0x38;
    for (u32 k = 0; k < 4; k++) {
        const s32 idx = MsgResMgr_findSetIndex(self, gabi::ea(&l_commonSetNames.mNames[k]), arr,
                                               s32((u32)(gabi::at<MsgResMgr>(self)->mSetCount)));
        if (idx < 0)
            continue;
        const u32 e = u32(idx) < MSG_MAX_SETS ? (u32)(gabi::at<be<u32>>(arr)[u32(idx)])
                                              : (u32)(*gabi::at<be<u32>>(arr));
        u32 cnt = (u32)(gabi::at<MsgSet>(e)->mEntryCount);
        if (s32(cnt) <= 0)
            continue;
        s32 left = s32(cnt);
        for (u32 j = 0;;) {
            u32 item = 0;
            if (j < cnt)
                item = MsgGetAttribute((u32)(gabi::at<MsgSet>(e)->mpData), j);
            if (u32(s32((s16)(gabi::at<MsgAttribute>(item)->mId))) == id) {
                if (!MsgSetGetLabel(e, labelOut, j))
                    return 0;
                MsgResMgr_parseNumber5(self, numOut, labelOut);
                return 1;
            }
            j++;
            if (--left == 0)
                break;
            cnt = (u32)(gabi::at<MsgSet>(e)->mEntryCount);
        }
    }
    return 0;
}

// HD: destructor
/* 025F6220 */
void MsgResMgr_dtor(u32 p, u32 flags) {
    if (!p)
        return;
    gabi::at<MsgResMgr>(p)->__vtbl = (u32)(msg_res_tu::disposer);
    if (p == (u32)(l_disposerHolder)) {
        const u32 inst = (u32)(l_instance);
        l_disposerHolder = (u32)(0);
        MsgProjectDestroy(inst + 0x10, 2u);
        l_instance = (u32)(0);
    }
    MsgDisposerDestroy(p, 0u);
    if (flags & 1)
        MsgDeallocate(p);
}

// HD: static initialiser (header statics + the TU's SafeString constants)
/* 025F62C0 */
void MsgResMgr_staticInit() {
    l_headerZeros.mWords[2] = (u32)(0);
    l_headerZeros.mWords[0] = (u32)(0);
    l_headerZeros.mWords[3] = (u32)(0);
    l_headerZeros.mWords[1] = (u32)(0);
    MsgRegisterGlobal(msg_res_tu::headerZeroDtor);
    const f32 lo = (f32)(l_piLowConstant), hi = (f32)(l_piHighConstant);
    l_piLow = (f32)(lo);
    l_piHigh = (f32)(hi);
    MsgAngleConstruct(gabi::ea(&l_headerAngle));
    MsgRegisterGlobal(msg_res_tu::headerAngleDtor);
    MsgCircleConstruct(gabi::ea(&l_headerCircle));
    MsgRegisterGlobal(msg_res_tu::headerCircleDtor);
    const u32 strs[][2] = {{gabi::ea(&l_projectFilename), msg_res_tu::projectFile},
                           {gabi::ea(&l_projectArchive), msg_res_tu::projectArchive},
                           {gabi::ea(&l_commonSetNames), msg_res_tu::message},
                           {gabi::ea(&l_commonSetNames.mNames[1]), msg_res_tu::message2},
                           {gabi::ea(&l_commonSetNames.mNames[2]), msg_res_tu::message3},
                           {gabi::ea(&l_commonSetNames.mNames[3]), msg_res_tu::message4},
                           {gabi::ea(&l_country), msg_res_tu::country},
                           {gabi::ea(&l_region), msg_res_tu::region}};
    for (auto &s : strs) {
        gabi::at<MsgSafeString>(s[0])->__vtbl = (u32)(kSafeStringVt);
        gabi::at<MsgSafeString>(s[0])->mpText = (u32)(s[1]);
    }
}

/* WWHD glue (tools/verify/autoglue.py): HD message resource manager
data l_instance 101F4AE8
data l_disposerHolder 101F4AEC
data l_resourceManager 101F4F7C
data l_config 101F4BAC
data l_languageNames 101F4A58
data l_languageMap 100E08E7
data l_emptyText 104A0CD8
data l_headerZeros 1048D6B4
data l_piLowConstant 100E0998
data l_piHighConstant 100E099C
data l_piLow 1048D6A8
data l_piHigh 1048D6AC
data l_headerAngle 1048D6B0
data l_headerCircle 1048D6B1
data l_rttiRootGuard 101FD888
data l_rttiBaseGuard 101FD89C
data l_rttiLayoutGuard 101FD8F0
data l_rttiTargetGuard 101FDD40
data l_rttiRoot 101FD8FC
data l_rttiBase 101FD8F8
data l_rttiLayout 101FD8F4
data l_rttiTarget 101FDD3C
data l_unitSetName 1048D650
data l_copyTagName 1048D658
data l_copyTagLowerName 1048D660
data l_layoutSeparator 1048D668
data l_pathSeparator 1048D670
data l_extensionSeparator 1048D678
data l_emptyString 1048D680
data l_projectFilename 1048D688
data l_projectArchive 1048D690
data l_region 1048D698
data l_country 1048D6A0
data l_pathSeparatorGuard 1048D6E4
data l_extensionSeparatorGuard 1048D6E8
data l_emptyStringGuard 1048D6EC
data l_unitSetNameGuard 1048D6F0
data l_copyTagNameGuard 1048D6F4
data l_copyTagLowerNameGuard 1048D6F8
data l_layoutSeparatorGuard 1048D6FC
data l_commonSetNames 1048D6C4
ext 025F64C0 u32 MsgFormatPath(u32,u32,u32,u32,u32,u32)
ext 026123FC u32 MsgFindResource(u32,u32)
ext 02612E64 u32 MsgLoadResource(u32,u32,u32,u32,u32,u32,u32)
ext 02705F7C u32 MsgFindTextTag(u32,u32,u32,u32)
ext 0273A1CC u32 MsgFindLabel(u32,u32)
ext 0273A574 u32 MsgGetAttribute(u32,u32)
ext 0273A728 u32 MsgGetProjectName(u32,u32)
ext 0273AD10 u32 MsgAllocate(u32)
ext 0273AF40 void MsgDeallocate(u32)
ext 0273B050 u32 MsgAllocateSet(u32,u32,u32)
ext 0273B0D4 u32 MsgAllocateManager(u32,u32,u32)
ext 02752B0C void MsgDisposerConstruct(u32,u32,u32)
ext 02752BEC void MsgDisposerDestroy(u32,u32)
ext 02757644 void MsgProjectDestroy(u32,u32)
ext 0275766C void MsgProjectBind(u32,u32,u32)
ext 027579BC void MsgSetBind(u32,u32,u32)
ext 02757A80 u32 MsgSetGetEntry(u32,u32)
ext 02757A9C u32 MsgSetGetLabel(u32,u32,u32)
ext 028764A0 u32 MsgTextTagEnd(u32)
ext 028764B8 u32 MsgTextTagBegin(u32)
ext 028EAB2C void MsgCircleConstruct(u32)
ext 028ED6F8 void MsgAngleConstruct(u32)
ext 028F026C void MsgRegisterGlobal(u32)
ext C0009988 void MsgCopyBytes(u32,u32,u32,u32)
ext 025F6418 void MsgSafeStringAssureEmpty(u32)
ext 025F641C void MsgBufferedStringAssure(u32)
*/
