/* hd_olv_02038E24: the HD Miiverse comment manager translation unit (cking::olv OliveCommentMgr,
 * 02038E24..0203B61F). No GameCube source: HD-only code.
 *
 * A sead singleton (0x4FDC: vtable +0, IDisposer +4, 20 bottle entries of 0x3EC at +0x14 (A: the 10
 * shown, B at +0x274C: the 10 downloaded), 20 texture slots of 0x10 at +0x4E84 {memo buffer +0, image
 * buffer +4, screenshot buffer +8, state +0xC}, rotation index +0x4FC4, count +0x4FCC, its heap +0x4FD0,
 * flags +0x4FC8/+0x4FD4..+0x4FD8). A bottle entry holds three wide fixed strings (name +0 (0xB),
 * topic +0x24 (0x33), body +0x98 (0xD3)), counts and flags (+0x24C..+0x270), the screenshot data
 * (+0x274), the memo texture (GX2Texture +0x328, size +0x320) and the post id (+0x3C4); +0x3E4 is the
 * texture slot. Functions: the delegate invoke companion, the entry constructor/copy/topic setter, the
 * manager constructor/createInstance/init, counters and predicates over the entries, texture-slot
 * bookkeeping, the rotation of downloaded bottles into the shown ones, the topic tag of the current
 * stage, the message-system publishing, the disposer destructor and the TU's __sinit (stage-name table).
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_olv_02038E24 {

static constexpr u32 OSBlockMove_ = 0xC0009988;
static constexpr u32 OSBlockSet_ = 0xC0009990;
static constexpr u32 act_GetSlotNo = 0xC0004660;
static constexpr u32 ENT = 0x3EC;
static constexpr u32 A_OFF = 0x14, B_OFF = 0x274C, TEX_OFF = 0x4E84;
static constexpr u32 SS_VT = 0x100055F0; /* this TU's SafeString vtable */

static inline u32 texslot(u32 cm, s32 b) { return cm + TEX_OFF + ((u32)b < 0x14 ? (u32)b * 0x10 : 0); }
/* sead's capped wide strlen (0 when longer than 0x40000) */
static inline s32 wlen(u32 p) {
    s32 n = 0;
    if (lhz(p) == 0) return 0;
    for (;;) {
        n += 1;
        p += 2;
        if (n > 0x40000) return 0;
        if (lhz(p) == 0) return n;
    }
}
static inline void vcall14(u32 o, u32 vtoff) { gabi::call_ptr<u32>(ld(ld(o + vtoff) + 0x14), o); }

/* 02038E24: sead delegate invoke (GHS member pointer; r6..r8 are scratch, r4/r5 pass through) */
static u32 Delegate_invoke(u32 self, u32 a4, u32 a5) {
    WWHD_FUNC(0x02038E24, u32, self, a4, a5);
    u32 obj = ld(self + 4);
    if (obj == 0) return self;
    s32 idx = (s16)lhz(self + 0xA);
    if (idx == 0) return self;
    u32 t = obj + (u32)(s32)(s16)lhz(self + 8);
    u32 fn;
    if (idx < 0) {
        fn = ld(self + 0xC);
    } else {
        u32 vt = ld(t + (u32)(s32)(s16)lhz(self + 0xE));
        fn = ld(vt + (u32)idx * 8 + 4);
    }
    return gabi::call_ptr<u32>(fn, t, a4, a5);
}
VERIFY(0x02038E24, Delegate_invoke);

/* 02038E78: this-adjusting thunk (+0x10) to the deleting destructor 0203B620 */
static void Thunk_02038E78(u32 p, u32 flags) {
    WWHD_FUNC(0x02038E78, void, p, flags);
    gabi::call(0x0203B620, p - 0x10, flags);
}
VERIFY(0x02038E78, Thunk_02038E78);

/* 02038E80: bottle entry constructor (0x3EC) */
static inline void wfixed_ct(u32 s, u32 vt, u32 cap) {
    sth(s + 0xC + cap * 2 - 2, 0);
    st(s + 0, s + 0xC);
    sth(s + 0xC, 0);
    st(s + 4, vt);
    st(s + 8, cap);
}
static u32 Entry_ct(u32 self) {
    WWHD_FUNC(0x02038E80, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(ENT);
        if (t == 0) return 0;
    }
    st(t + 0x3E8, 0x10005CC8);
    wfixed_ct(t + 0, 0x100056C0, 0xB);
    wfixed_ct(t + 0x24, 0x100056F0, 0x33);
    wfixed_ct(t + 0x98, 0x10005720, 0xD3);
    stb(t + 0x24C, 0);
    st(t + 0x34C, 0);
    stb(t + 0x3E4, 0xFF);
    stb(t + 0x25B, 0);
    sth(t + 0x320, 0);
    st(t + 0x260, 0);
    st(t + 0x2A4, 0);
    stb(t + 0x25A, 0);
    st(t + 0x31C, 0);
    stb(t + 0x264, 0);
    stb(t + 0x3E5, 0);
    stb(t + 0x259, 0);
    sth(t + 0x27A, 0);
    st(t + 0x268, 0);
    sth(t + 0x322, 0);
    stb(t + 0x258, 0);
    sth(t + 0x278, 0);
    st(t + 0x26C, 0);
    stb(t + 0x24D, 0);
    st(t + 0x254, 0);
    st(t + 0x274, 0);
    st(t + 0x270, 0);
    st(t + 0x250, 0);
    stb(t + 0x25C, 0);
    return t;
}
VERIFY(0x02038E80, Entry_ct);

static void Dt_02038FF0(u32 p, u32 flags) {
    WWHD_FUNC(0x02038FF0, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x02038FF0, Dt_02038FF0);

/* copy the wide string object SRC into the fixed one at DST (sead WFixedSafeString::copy) */
static inline void wstr_copy(u32 dst, u32 src) {
    u32 vt = ld(src + 4);
    u32 buf = ld(dst);
    gabi::call_ptr<u32>(ld(vt + 0x14), src);
    s32 n = wlen(ld(src));
    s32 cap = (s32)ld(dst + 8);
    u32 vt2 = ld(src + 4);
    if (!(n < cap)) n = cap - 1;
    gabi::call_ptr<u32>(ld(vt2 + 0x14), src);
    gabi::call(OSBlockMove_, buf, ld(src), (u32)n * 2, 0u);
    sth(buf + (u32)n * 2, 0);
}

/* 02039004: copy a bottle entry */
static void Entry_copy(u32 dst, u32 src) {
    WWHD_FUNC(0x02039004, void, dst, src);
    wstr_copy(dst, src);
    vcall14(src + 0x24, 4);
    gabi::call(0x025F8CDC, ld(0x101F4B5C), dst + 0x24, ld(src + 0x24));
    wstr_copy(dst + 0x98, src + 0x98);
    stb(dst + 0x24C, lbz(src + 0x24C));
    stb(dst + 0x24D, lbz(src + 0x24D));
    st(dst + 0x250, ld(src + 0x250));
    st(dst + 0x254, ld(src + 0x254));
    stb(dst + 0x258, lbz(src + 0x258));
    stb(dst + 0x259, lbz(src + 0x259));
    stb(dst + 0x25A, lbz(src + 0x25A));
    stb(dst + 0x25B, lbz(src + 0x25B));
    stb(dst + 0x25C, lbz(src + 0x25C));
    st(dst + 0x260, ld(src + 0x260));
    stb(dst + 0x264, lbz(src + 0x264));
    st(dst + 0x268, ld(src + 0x268));
    st(dst + 0x26C, ld(src + 0x26C));
    stb(dst + 0x3E4, lbz(src + 0x3E4));
    stb(dst + 0x3E5, lbz(src + 0x3E5));
    st(dst + 0x270, ld(src + 0x270));
    gabi::call(OSBlockMove_, dst + 0x274, src + 0x274, 0xA8u, 0u);
    gabi::call(OSBlockMove_, dst + 0x31C, src + 0x31C, 0xA8u, 0u);
    gabi::call(OSBlockMove_, dst + 0x3C4, src + 0x3C4, 0x20u, 0u);
}
VERIFY(0x02039004, Entry_copy);

/* 02039240: set the topic (converted by the message system) */
static void Entry_setTopic(u32 e, u32 ws) {
    WWHD_FUNC(0x02039240, void, e, ws);
    vcall14(ws, 4);
    gabi::call(0x025F8CDC, ld(0x101F4B5C), e + 0x24, ld(ws));
    if ((s32)ld(e + 0x2C) <= 0x32) {
        vcall14(e + 0x24, 4);
        /* (sead's capped strlen of the result follows; its value is unused) */
    } else {
        sth(ld(e + 0x24) + 0x64, 0);
    }
}
VERIFY(0x02039240, Entry_setTopic);

/* 02039300: OliveCommentMgr constructor (0x4FDC; the IDisposer at +4 is built by createInstance) */
static u32 CommentMgr_ct(u32 self) {
    WWHD_FUNC(0x02039300, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x4FDC);
        if (t == 0) return 0;
    }
    st(t + 0, 0x10005738);
    u32 a = t + A_OFF;
    if (a == 0) a = op_new(0x2738);
    if (a != 0) gabi::call(0x028EFFD0, a, 0xAu, ENT, 0x02038E80);
    u32 b = t + B_OFF;
    if (b == 0) b = op_new(0x2738);
    if (b != 0) gabi::call(0x028EFFD0, b, 0xAu, ENT, 0x02038E80);
    if (t + TEX_OFF == 0) op_new(0x140);
    st(t + 0x4FD8, 0);
    stb(t + 0x4FC8, 1);
    stb(t + 0x4FD5, 0);
    st(t + 0x4FC4, 0);
    st(t + 0x4FD0, 0);
    stb(t + 0x4FD7, 0);
    stb(t + 0x4FD6, 0);
    st(t + 0x4FCC, 0);
    stb(t + 0x4FD4, 0);
    return t;
}
VERIFY(0x02039300, CommentMgr_ct);

/* 020393E0: sead singleton createInstance(heap) (instance 1018F4AC, disposer 1018F4B0) */
static u32 CommentMgr_createInstance(u32 heap) {
    WWHD_FUNC(0x020393E0, u32, heap);
    u32 cur = ld(0x1018F4AC);
    if (cur != 0) return cur;
    u32 d = gabi::call<u32>(0x0273B0D4, 0x4FDCu, heap, 4u);
    u32 disp = d + 4;
    if (disp != 0) {
        gabi::call(0x02752B0C, disp, heap, 3u);
        st(disp + 0xC, 0x10005CD8);
    }
    st(0x1018F4B0, disp);
    u32 r = d;
    if (d != 0) r = gabi::call<u32>(0x02039300, d);
    st(0x1018F4AC, r);
    return r;
}
VERIFY(0x020393E0, CommentMgr_createInstance);

/* clear one bottle entry */
static inline void clear_entry(u32 e) {
    sth(ld(e + 0), 0);
    sth(ld(e + 0x24), 0);
    sth(ld(e + 0x98), 0);
    stb(e + 0x24C, 0);
    stb(e + 0x24D, 0);
    st(e + 0x250, 0);
    st(e + 0x254, 0);
    stb(e + 0x258, 0);
    stb(e + 0x259, 0);
    stb(e + 0x25A, 0);
    stb(e + 0x25B, 0);
    stb(e + 0x25C, 0);
    st(e + 0x260, 0);
    stb(e + 0x264, 0);
    st(e + 0x268, 0);
    st(e + 0x26C, 0);
    st(e + 0x270, 0);
    st(e + 0x2A4, 0);
    st(e + 0x34C, 0);
    stb(e + 0x3E4, 0xFF);
    gabi::call(OSBlockSet_, e + 0x3C4, 0u, 0x20u);
}
static inline void clear_all(u32 base) {
    for (u32 k = 0; k < 10; k++) clear_entry(base + k * ENT);
}

/* 02039480: init: clear all entries, the comment heap ("OliveCommentMgr"-type, 10 MB) with the
 * texture-slot buffers, and the account slot flag */
static void CommentMgr_init(u32 cm) {
    WWHD_FUNC(0x02039480, void, cm);
    clear_all(cm + A_OFF);
    clear_all(cm + B_OFF);
    gabi::Local<be<u32>[2]> nm;
    st(nm.a + 4, SS_VT);
    st(nm.a + 0, 0x100057B8);
    u32 parent = gabi::call<u32>(0x0203E8C4);
    u32 h = gabi::call<u32>(0x02753004, 0u, nm.a, parent, 1u, 0u);
    st(cm + 0x4FD0, h);
    u32 hp = h;
    for (u32 i = 0; i < 0x14; i++) {
        u32 t = cm + TEX_OFF + i * 0x10;
        st(t + 0, gabi::call<u32>(0x0273B0D4, 0x8000u, hp, 0x800u));
        st(t + 4, gabi::call<u32>(0x0273B0D4, 0x28000u, ld(cm + 0x4FD0), 0x2000u));
        st(t + 8, gabi::call<u32>(0x0273B0D4, 0x50000u, ld(cm + 0x4FD0), 0x40u));
        st(t + 0xC, 0);
        hp = ld(cm + 0x4FD0);
    }
    gabi::call_ptr<u32>(vfn(hp, 0xC, 0x2C), hp);
    stb(cm + 0x4FD7, 2);
    gabi::Local<be<u32>[1]> res;
    u32 slot = gabi::call<u32>(act_GetSlotNo);
    gabi::call(0x0286D938, res.a, slot);
    if (lbz(res.a) == 0) {
        slot = gabi::call<u32>(act_GetSlotNo);
        gabi::call(0x0286D9E0, cm + 0x4FD7, slot);
    }
    stb(cm + 0x4FD4, 0);
}
VERIFY(0x02039480, CommentMgr_init);

static void Empty_02039834() {
    WWHD_FUNC(0x02039834, void);
}
VERIFY(0x02039834, Empty_02039834);

/* 02039838: shown entries that are valid and not yet read */
static u32 CommentMgr_countUnread(u32 cm) {
    WWHD_FUNC(0x02039838, u32, cm);
    u32 n = 0;
    for (u32 k = 0; k < 10; k++) {
        u32 e = cm + A_OFF + k * ENT;
        if (lbz(e + 0x25B) == 0) continue;
        if (lbz(e + 0x25A) != 0) continue;
        n++;
    }
    return n;
}
VERIFY(0x02039838, CommentMgr_countUnread);

/* 020398B8: notification count (0 while a menu/event of kind 2 is up) */
static u32 CommentMgr_notifyCount(u32 cm) {
    WWHD_FUNC(0x020398B8, u32, cm);
    u32 g = ld(0x101F5088);
    u32 p = ld(0x101F8344);
    bool busy = gabi::call<u32>(0x02618498, g) != 0 || gabi::call<u32>(0x02617AE4, g) != 0;
    if (busy && ld(p + 0x224) == 2) return 0;
    u32 n = gabi::call<u32>(0x02039838, cm);
    if (lbz(cm + 0x4FD5) != 0 && lbz(cm + 0x4FD6) == 0) n++;
    return n;
}
VERIFY(0x020398B8, CommentMgr_notifyCount);

static u32 CommentMgr_hasNotify(u32 cm) {
    WWHD_FUNC(0x02039970, u32, cm);
    u32 r = gabi::call<u32>(0x020398B8, cm);
    return ((0u - r) & ~r) >> 31;
}
VERIFY(0x02039970, CommentMgr_hasNotify);

static u32 CommentMgr_unreadCount(u32 cm) {
    WWHD_FUNC(0x0203999C, u32, cm);
    u32 n = gabi::call<u32>(0x02039838, cm);
    if (lbz(cm + 0x4FD5) != 0 && lbz(cm + 0x4FD6) == 0) n++;
    return n;
}
VERIFY(0x0203999C, CommentMgr_unreadCount);

static u32 CommentMgr_hasUnread(u32 cm) {
    WWHD_FUNC(0x020399DC, u32, cm);
    u32 r = gabi::call<u32>(0x0203999C, cm);
    return ((0u - r) & ~r) >> 31;
}
VERIFY(0x020399DC, CommentMgr_hasUnread);

/* 02039A08: valid shown entries */
static u32 CommentMgr_countShown(u32 cm) {
    WWHD_FUNC(0x02039A08, u32, cm);
    u32 n = 0;
    for (u32 k = 0; k < 10; k++)
        if (lbz(cm + A_OFF + k * ENT + 0x25B) != 0) n++;
    return n;
}
VERIFY(0x02039A08, CommentMgr_countShown);

/* downloaded entry K with a texture slot in STATE, valid and with a post id */
static inline bool dl_ready(u32 cm, u32 k, u32 state) {
    u32 e = cm + B_OFF + k * ENT;
    s32 b = (s8)lbz(e + 0x3E4);
    if (b < 0) return false;
    if (ld(texslot(cm, b) + 0xC) != state) return false;
    if (lbz(e + 0x25B) == 0) return false;
    return lbz(e + 0x3C4) != 0;
}
static inline u32 save_mode() { return lbz(gabi::call<u32>(0x027200F4, ld(0x101F84DC) + 0x12C0)); }

/* 02039A58 / 02039C34: downloaded entries ready in texture state 1 / 2 (none in mode 2) */
static u32 CommentMgr_countReady1(u32 cm) {
    WWHD_FUNC(0x02039A58, u32, cm);
    if (save_mode() == 2) return 0;
    u32 n = 0;
    for (u32 k = 0; k < 10; k++)
        if (dl_ready(cm, k, 1)) n++;
    return n;
}
VERIFY(0x02039A58, CommentMgr_countReady1);

/* 02039B80: index after the last valid downloaded entry (max 10) */
static s32 CommentMgr_downloadedEnd(u32 cm) {
    WWHD_FUNC(0x02039B80, s32, cm);
    s32 last = -1;
    for (u32 k = 0; k < 10; k++) {
        u32 e = cm + B_OFF + k * ENT;
        if ((s8)lbz(e + 0x3E4) < 0) continue;
        if (lbz(e + 0x25B) == 0) continue;
        if (lbz(e + 0x3C4) == 0) continue;
        last = (s32)k;
    }
    s32 r = last + 1;
    if (r > 10) r = 10;
    return r;
}
VERIFY(0x02039B80, CommentMgr_downloadedEnd);

static u32 CommentMgr_countReady2(u32 cm) {
    WWHD_FUNC(0x02039C34, u32, cm);
    if (save_mode() == 2) return 0;
    u32 n = 0;
    for (u32 k = 0; k < 10; k++)
        if (dl_ready(cm, k, 2)) n++;
    return n;
}
VERIFY(0x02039C34, CommentMgr_countReady2);

/* 02039D5C: move the first ready (state 1) downloaded entry's texture to state 2 */
static u32 CommentMgr_promoteOne(u32 cm) {
    WWHD_FUNC(0x02039D5C, u32, cm);
    for (u32 k = 0; k < 10; k++) {
        if (!dl_ready(cm, k, 1)) continue;
        s32 b = (s8)lbz(cm + B_OFF + k * ENT + 0x3E4);
        st(texslot(cm, b) + 0xC, 2);
        return 1;
    }
    return 0;
}
VERIFY(0x02039D5C, CommentMgr_promoteOne);

static inline bool uses_slot(u32 e, u32 i) {
    if (i != (u32)(s32)(s8)lbz(e + 0x3E4)) return false;
    if (lbz(e + 0x25B) == 0) return false;
    return lbz(e + 0x3C4) != 0;
}

/* 02039E4C: free (state 4) the texture slots no valid entry uses; state 2 -> 1 */
static void CommentMgr_syncTex(u32 cm) {
    WWHD_FUNC(0x02039E4C, void, cm);
    for (u32 i = 0; i < 0x14; i++) {
        bool found = false;
        for (u32 k = 0; k < 10 && !found; k++)
            if (uses_slot(cm + B_OFF + k * ENT, i)) found = true;
        if (found) continue;
        for (u32 k = 0; k < 10; k++)
            if (uses_slot(cm + A_OFF + k * ENT, i)) {
                found = true;
                break;
            }
        if (found) continue;
        u32 t = cm + TEX_OFF + i * 0x10;
        if (ld(t + 0xC) != 0) st(t + 0xC, 4);
    }
    for (u32 i = 0; i < 0x14; i++) {
        u32 t = cm + TEX_OFF + i * 0x10;
        if (ld(t + 0xC) == 2) st(t + 0xC, 1);
    }
    stb(cm + 0x4FD4, 0);
}
VERIFY(0x02039E4C, CommentMgr_syncTex);

/* 0203A070: drop the downloaded entries (texture states 4/2/1 -> 0) */
static void CommentMgr_resetDownloaded(u32 cm) {
    WWHD_FUNC(0x0203A070, void, cm);
    for (u32 i = 0; i < 0x14; i++) {
        u32 t = cm + TEX_OFF + i * 0x10;
        u32 s = ld(t + 0xC);
        if (s == 4 || s == 2 || s == 1) st(t + 0xC, 0);
    }
    clear_all(cm + B_OFF);
    st(cm + 0x4FCC, 0);
    stb(cm + 0x4FD4, 1);
}
VERIFY(0x0203A070, CommentMgr_resetDownloaded);

/* 0203A24C: full reset (shown entries, rotation state, state-3 slots, then the downloaded ones) */
static void CommentMgr_reset(u32 cm) {
    WWHD_FUNC(0x0203A24C, void, cm);
    st(cm + 0x4FD8, 0);
    stb(cm + 0x4FD6, 0);
    stb(cm + 0x4FC8, 1);
    st(cm + 0x4FC4, 0);
    stb(cm + 0x4FD5, 0);
    clear_all(cm + A_OFF);
    for (u32 i = 0; i < 0x14; i++) {
        u32 t = cm + TEX_OFF + i * 0x10;
        if (ld(t + 0xC) == 3) st(t + 0xC, 0);
    }
    gabi::call(0x0203A070, cm);
}
VERIFY(0x0203A24C, CommentMgr_reset);

/* 0203A3F0 / 0203A438: account-slot predicates (debug overrides 1018F4B4/B8/BC) */
static u32 CommentMgr_slotOk(u32 cm) {
    WWHD_FUNC(0x0203A3F0, u32, cm);
    if (ld(0x1018F4B4) != 0) return ld(0x1018F4B8) != 0;
    u32 b = lbz(cm + 0x4FD7);
    return (b == 0 || b == 1) ? 1 : 0;
}
VERIFY(0x0203A3F0, CommentMgr_slotOk);

static u32 CommentMgr_slotFirst(u32 cm) {
    WWHD_FUNC(0x0203A438, u32, cm);
    if (ld(0x1018F4B4) != 0) return ld(0x1018F4BC) != 0;
    return lbz(cm + 0x4FD7) == 0;
}
VERIFY(0x0203A438, CommentMgr_slotFirst);

/* 0203A46C: save mode 0 or 1 */
static u32 CommentMgr_modeOk() {
    WWHD_FUNC(0x0203A46C, u32);
    u32 v = save_mode();
    return (v == 0 || v == 1) ? 1 : 0;
}
VERIFY(0x0203A46C, CommentMgr_modeOk);

/* shift the downloaded entries down by one (B[i-1] = B[i]) */
static inline void shift_downloaded(u32 cm) {
    u32 b = cm + B_OFF;
    for (u32 i = 1; i < 10; i++) gabi::call(0x02039004, b + (i - 1) * ENT, b + i * ENT);
}

/* 0203A4B8: rotate the next downloaded bottle into shown slot +0x4FC4 */
static void CommentMgr_rotate(u32 cm) {
    WWHD_FUNC(0x0203A4B8, void, cm);
    if (gabi::call<u32>(0x0203A46C) == 0) return;
    if (gabi::call<u32>(0x0203A3F0, cm) == 0) return;
    if (lbz(cm + 0x4FC8) == 0) {
        u32 i = ld(cm + 0x4FC4) + 1;
        st(cm + 0x4FC4, i);
        if ((s32)i >= 10) st(cm + 0x4FC4, 0);
    } else {
        u32 i = ld(cm + 0x4FC4);
        stb(cm + 0x4FC8, 0);
        if ((s32)i >= 10) st(cm + 0x4FC4, 0);
    }
    s32 n = (s32)gabi::call<u32>(0x02039C34, cm);
    st(cm + 0x4FCC, (u32)n);
    if (n <= 0) return;
    u32 a = cm + A_OFF;
    u32 idx = ld(cm + 0x4FC4);
    u32 e = idx < 10 ? a + idx * ENT : a;
    s32 b = (s8)lbz(e + 0x3E4);
    if (b >= 0) {
        st(texslot(cm, b) + 0xC, 0);
        idx = ld(cm + 0x4FC4);
    }
    clear_entry(idx < 10 ? a + idx * ENT : a);
    u32 b0 = cm + B_OFF;
    u32 last = cm + A_OFF + 19 * ENT;
    while (lbz(b0 + 0x25B) == 0 || lbz(b0 + 0x3C4) == 0) {
        shift_downloaded(cm);
        clear_entry(last);
    }
    idx = ld(cm + 0x4FC4);
    gabi::call(0x02039004, idx < 10 ? a + idx * ENT : a, b0);
    idx = ld(cm + 0x4FC4);
    e = idx < 10 ? a + idx * ENT : a;
    b = (s8)lbz(e + 0x3E4);
    if (b >= 0) st(texslot(cm, b) + 0xC, 3);
    shift_downloaded(cm);
    st(cm + 0x4FCC, ld(cm + 0x4FCC) - 1);
    clear_entry(last);
}
VERIFY(0x0203A4B8, CommentMgr_rotate);

/* 0203A910 / 0203A958: display interval parameters */
static f32 CommentMgr_interval(u32 cm) {
    WWHD_FUNC(0x0203A910, f32, cm);
    if ((s32)gabi::call<u32>(0x020398B8, cm) > 0) return ldf(0x1018F4C4);
    return ldf(0x1018F4C0);
}
VERIFY(0x0203A910, CommentMgr_interval);

static f32 CommentMgr_rate(u32 cm) {
    WWHD_FUNC(0x0203A958, f32, cm);
    f32 f = gabi::call<f32>(0x0203A910, cm);
    return fdivs_ppc(ldf(0x100057D0), f);
}
VERIFY(0x0203A958, CommentMgr_rate);

/* compare the SafeString literal LIT with the current stage name (dComIfGp + 0x5134) */
static inline bool stage_is(u32 litLocal, u32 stgLocal, u32 lit) {
    st(litLocal + 4, SS_VT);
    st(litLocal + 0, lit);
    u32 g = gabi::call<u32>(0x025200D4);
    u32 vt = ld(litLocal + 4);
    st(stgLocal + 4, SS_VT);
    st(stgLocal + 0, g + 0x5134);
    gabi::call_ptr<u32>(ld(vt + 0x14), litLocal);
    gabi::call_ptr<u32>(ld(ld(litLocal + 4) + 0x14), litLocal);
    u32 vt2 = ld(stgLocal + 4);
    u32 s1 = ld(litLocal);
    gabi::call_ptr<u32>(ld(vt2 + 0x14), stgLocal);
    u32 s2 = ld(stgLocal);
    if (s1 == s2) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u32 x = lbz(s1 + i), y = lbz(s2 + i);
        if (x != y) return false;
        if (x == 0) return true;
    }
    return false;
}

/* 0203A984: the Miiverse topic tag for the current place: stage category of the current room/map
 * (save collect flag for one case, a per-category table, or the stage-name lookup), then the
 * message text of the stage-name table entry copied into WS */
static void CommentMgr_topicTag(u32 cm, u32 ws) {
    WWHD_FUNC(0x0203A984, void, cm, ws);
    s32 id = 0;
    u32 g = gabi::call<u32>(0x025200D4);
    u32 o = g + 0x5150;
    bool viaStage = false;
    if (gabi::call_ptr<u32>(ld(ld(o) + 0x15C), o) == 0) {
        id = 0;
    } else {
        g = gabi::call<u32>(0x025200D4);
        o = g + 0x5150;
        u32 info = gabi::call_ptr<u32>(ld(ld(o) + 0x15C), o);
        u32 k = (lbz(info + 9) >> 1) & 0x7F;
        if (k < 3) {
            if (k <= 1) {
                viaStage = true;
            } else {
                id = gabi::call<u32>(0x025B7A2C, ld(0x101F84DC) + 0xD4, 3u, 0u) != 0 ? 0x32 : 0x3A;
            }
        } else if (k <= 9) {
            id = (s8)lbz(0x100057E1 + k);
        } else if (k <= 0xD) {
            viaStage = true;
        } else if (k <= 0xF) {
            id = 0x31;
        } else {
            id = 0;
        }
    }
    if (viaStage) {
        id = (s8)lbz(ld(ld(0x101F8344) + 0x218) + 0x3E);
        if (id == 0x2B || id == 0) {
            gabi::Local<be<u32>[2]> l8;
            gabi::Local<be<u32>[2]> l10;
            gabi::Local<be<u32>[2]> l20;
            gabi::Local<be<u32>[2]> l28;
            if (stage_is(l8.a, l20.a, 0x100057D4) || stage_is(l10.a, l28.a, 0x100057DC)) id = 0x3A;
        }
    }
    /* message lookup of the stage-name table entry (index clamped to 0x3A; negative -> entry 0) */
    struct msg_l { be<u32> w[0x24A]; }; /* 0x30..0x958 of the original frame */
    gabi::Local<msg_l> m;
    const u32 l30 = m.a, l3c = m.a + 0xC, l150 = m.a + 0x120;
    u32 name = 0x10200AA8;
    if (id >= 0) {
        if (id > 0x3A) id = 0x3A;
        id = (s8)id;
        name += (u32)id * 8;
    }
    gabi::call(0x025F65F4, l30);
    gabi::call(0x025F6D70, l30, 0x10200A74, name);
    gabi::call(0x025F50E8, ld(0x101F4AE8), l30, l3c, l150);
    sth(ld(ws), 0);
    u32 r11 = ld(l30 + 8);
    u32 text = 0;
    u32 mi = ld(l3c + 0x10C);
    if (mi < ld(r11 + 4)) text = gabi::call<u32>(0x0273A2E8, ld(r11 + 0), mi);
    u32 dst = ld(ws);
    gabi::Local<be<u32>[2]> src;
    st(src.a + 4, 0x10005650);
    st(src.a + 0, text);
    gabi::call(0x0203B6B0, src.a);
    s32 n = wlen(ld(src.a));
    s32 cap = (s32)ld(ws + 8);
    u32 vt = ld(src.a + 4);
    if (!(n < cap)) n = cap - 1;
    gabi::call_ptr<u32>(ld(vt + 0x14), src.a);
    gabi::call(OSBlockMove_, dst, ld(src.a), (u32)n * 2, 0u);
    sth(dst + (u32)n * 2, 0);
}
VERIFY(0x0203A984, CommentMgr_topicTag);

/* 0203AD94: send an entry's three strings to the message system (101F4BD8) */
static inline void send_wstr(u32 s) {
    vcall14(s, 4);
    u32 vt = ld(s + 4);
    u32 p = ld(s);
    gabi::call_ptr<u32>(ld(vt + 0x14), s);
    s32 n = wlen(ld(s));
    gabi::call(0x025F9B1C, ld(0x101F4BD8), p, (u32)n, 0x19u, 0u, 0xFFFFFFFFu);
}
static void CommentMgr_sendEntry(u32 cm, u32 e) {
    WWHD_FUNC(0x0203AD94, void, cm, e);
    send_wstr(e);
    send_wstr(e + 0x24);
    send_wstr(e + 0x98);
}
VERIFY(0x0203AD94, CommentMgr_sendEntry);

/* 0203AF50: publish all entries to the message system and wait until it took them */
static void CommentMgr_publish(u32 cm) {
    WWHD_FUNC(0x0203AF50, void, cm);
    gabi::call(0x025F9B1C, ld(0x101F4BD8), 0x100057EC, 0x14u, 0x19u, 0u, 0xFFFFFFFFu);
    gabi::Local<be<u16>[2]> sep;
    sth(sep.a, 0x2026);
    gabi::call(0x025F9B1C, ld(0x101F4BD8), sep.a, 2u, 0x19u, 0u, 0xFFFFFFFFu);
    for (u32 k = 0; k < 10; k++) gabi::call(0x0203AD94, cm, cm + A_OFF + k * ENT);
    for (u32 k = 0; k < 10; k++) gabi::call(0x0203AD94, cm, cm + B_OFF + k * ENT);
    gabi::Local<be<u32>[2]> span;
    while (gabi::call<u32>(0x025F9B68, ld(0x101F4BD8)) == 0) {
        st(span.a + 4, 1);
        st(span.a + 0, 0);
        gabi::call(0x02760C3C, span.a);
    }
    gabi::call(0x020063C0, ld(0x101F4BD8) + 0x10, 0x1048D804);
}
VERIFY(0x0203AF50, CommentMgr_publish);

/* 0203B0B4: the singleton disposer's destructor */
static void CommentMgr_disposer_dt(u32 p, u32 flags) {
    WWHD_FUNC(0x0203B0B4, void, p, flags);
    if (p == 0) return;
    st(p + 0xC, 0x10005CD8);
    if (p == ld(0x1018F4B0)) {
        u32 inst = ld(0x1018F4AC);
        st(0x1018F4B0, 0);
        gabi::call_ptr<u32>(vfn(inst, 0, 0x14), inst, 2u);
        st(0x1018F4AC, 0);
    }
    gabi::call(0x02752BEC, p, 0u);
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203B0B4, CommentMgr_disposer_dt);

/* 0203B15C: __sinit: header statics, interval floats, the 59-entry stage-name table (SafeStrings
 * 10200AA8) and its message group name (10200A74) */
static void sinit_0203B15C() {
    WWHD_FUNC(0x0203B15C, void);
    const u32 obj = 0x10200A98, reg = 0x1018F488;
    st(obj + 0xC, 0); st(obj + 8, 0); st(obj + 4, 0); st(obj + 0, 0);
    gabi::call(0x028F026C, reg);
    f32 a = ldf(0x10005804), b = ldf(0x10005808);
    stf(0x10200A7C, a);
    stf(0x10200A80, b);
    gabi::call(0x028ED6F8, 0x10200A94);
    gabi::call(0x028F026C, reg + 0xC);
    gabi::call(0x028EAB2C, 0x10200A95);
    gabi::call(0x028F026C, reg + 0x18);
    f32 f50k = ldf(0x1000580C), f10k = ldf(0x10005810);
    stf(0x10200A88, f50k);
    stf(0x10200A8C, f10k);
    stf(0x10200A84, f50k);
    stf(0x10200A90, f10k);
    for (u32 i = 0; i < 59; i++) {
        st(0x10200AA8 + i * 8 + 4, SS_VT);
        st(0x10200AA8 + i * 8, 0x10005814 + i * 0x14);
    }
    st(0x10200A74 + 4, SS_VT);
    st(0x10200A74, 0x10005CB0);
}
VERIFY(0x0203B15C, sinit_0203B15C);

static void Dt_0203B5F8(u32 p, u32 flags) {
    WWHD_FUNC(0x0203B5F8, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203B5F8, Dt_0203B5F8);

static void Dt_0203B60C(u32 p, u32 flags) {
    WWHD_FUNC(0x0203B60C, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203B60C, Dt_0203B60C);

}  // namespace hd_olv_02038E24
