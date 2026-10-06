/* d_attention part 3: drawing (dAttDraw_c::draw, runDrawProc, Draw), the constructors and the
 * destructor, Run and the action-button queries. WWHD. 
 * See d_attention.cpp for the unit's range and layouts. Calls to functions of the other parts go
 * by address.
 *
 * HD: dAttDraw_c::mpAnmMatClr is an mDoExt_bpkAnm (025E7480 creates it, 025E74FC init, 025E779C
 * entry) instead of a J3DMatColorAnm array. The arrow's animation name is compared with
 * sead::SafeString operator== ("yj_in", "yj_out", "yj_delete"): draw() poses the arrow at the
 * end frame of yj_in / the start frame of yj_out for the matrix calculation, and runDrawProc
 * does not restart yj_out while yj_delete is playing. */
#include "bindings.h"

namespace d_attention_3_cpp {

static constexpr u32 SAFESTRING_VTBL_ATT = 0x100430F8;

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
static inline void memzero_l(u32 p, u32 n) { gabi::call(0x028F521C, p, n); }
static inline u32 operator_new_l(u32 size) { return gabi::call<u32>(0x0273AD10, size); }
static inline void operator_delete_l(u32 p) { gabi::call(0x0273AF40, p); }
static inline void cSAngle_mi_l(u32 a, u32 res, s16 v) { gabi::call(0x02006908, a, res, v); }
static inline void cSGlobe_ct_l(u32 g, u32 v) { gabi::call(0x02007324, g, v); }
static inline void cXyz_mi_l(u32 a, u32 res, u32 b) { gabi::call(0x0201ADE0, a, res, b); }
static inline void PSMTXTrans_l(u32 m, f32 x, f32 y, f32 z) { gabi::call(0x028E93CC, m, x, y, z); }
static inline void PSMTXConcat_l(u32 a, u32 b, u32 ab) { gabi::call(0x028E9108, a, b, ab); }
static inline BOOL PSMTXInverse_l(u32 src, u32 dst) { return gabi::call<BOOL>(0x028E91EC, src, dst); }
static inline void mDoAud_seStart_l(u32 id) { gabi::call(0x025E1988, id); } /* HD: sound id only */
static inline BOOL mDoCPd_L_LOCK_BUTTON_l(u32 pad) { return gabi::call<BOOL>(0x02007CA4, pad); }
static inline u32 dComIfGs_getOptAttentionType_l(u32 p) { return gabi::call<u32>(0x027200D0, p); }
static inline BOOL dComIfGs_checkGetItemNum_l(u8 item) { return gabi::call<BOOL>(0x02520FE4, item); }
static inline s32 McaMorf_play_l(u32 morf, u32 pos, u32 a, u32 b) { return gabi::call<s32>(0x025E535C, morf, pos, a, b); }
static inline void McaMorf_calc_l(u32 morf) { gabi::call(0x025E55A0, morf); }
static inline void McaMorf_entryDL_l(u32 morf) { gabi::call(0x025E5590, morf); }
static inline void McaMorf_updateDL_l(u32 morf) { gabi::call(0x025E54D8, morf); }
/* mDoExt_McaMorf::mDoExt_McaMorf(this, modelData, cb1, cb2, anm, mode, speed, start, end; stack: 4 words) */
static inline u32 McaMorf_ct_l(u32 t, u32 md, u32 cb1, u32 cb2, u32 anm, s32 mode, f32 spd, s32 start, s32 end, s32 s0,
                               u32 s1, u32 s2, u32 s3) {
    return gabi::call<u32>(0x025E4F64, t, md, cb1, cb2, anm, mode, spd, start, end, s0, s1, s2, s3);
}
/* mDoExt_bpkAnm: (this == NULL) constructor, init(modelData, anm, anmPlay, attr, rate, start, end, modify; stack: 1), entry */
static inline u32 bpkAnm_ct_l(u32 t) { return gabi::call<u32>(0x025E7480, t); }
static inline s32 bpkAnm_init_l(u32 t, u32 md, u32 anm, s32 play, s32 attr, f32 rate, s32 start, s32 end, s32 modify, s32 s0) {
    return gabi::call<s32>(0x025E74FC, t, md, anm, play, attr, rate, start, end, modify, s0);
}
static inline void bpkAnm_entry_l(u32 t, u32 md, f32 frame) { gabi::call(0x025E779C, t, md, frame); }
static inline u32 createSolidHeapFromGameToCurrent_l(u32 size, u32 align) { return gabi::call<u32>(0x025E3630, size, align); }
static inline void restoreCurrentHeap_l() { gabi::call(0x025E37D8); }
static inline s32 adjustSolidHeap_l(u32 heap) { return gabi::call<s32>(0x025E3678, heap); }
static inline void DCStoreRangeNoSync_l(u32 p, u32 n) { gabi::call(0xC00088B8, p, n); }
static inline void destroySolidHeap_l(u32 heap) { gabi::call(0x025E3868, heap); }

/* functions of the other parts */
static inline void setAnm_l(u32 d, s32 bck, s32 bpk, s32 mode) { gabi::call(0x024EC10C, d, bck, bpk, mode); }
static inline u32 getActor_l(u32 e) { return gabi::call<u32>(0x024EBA14, e); }
static inline u32 LockonTarget_l(u32 t, s32 i) { return gabi::call<u32>(0x024EC8D0, t, i); }
static inline u32 LockonTargetPId_l(u32 t, s32 i) { return gabi::call<u32>(0x024ECE9C, t, i); }
static inline void initList_l(u32 t, u32 m) { gabi::call(0x024EC6A8, t, m); }
static inline s32 freeAttention_l(u32 t) { return gabi::call<s32>(0x024ECA8C, t); }
static inline void judgementButton_l(u32 t) { gabi::call(0x024ECCFC, t); }
static inline void judgementStatusSw_l(u32 t, u32 m) { gabi::call(0x024ECED4, t, m); }
static inline void judgementStatusHd_l(u32 t, u32 m) { gabi::call(0x024ED0A4, t, m); }
static inline void runSoundProc_l(u32 t) { gabi::call(0x024ED200, t); }
static inline void dAttHint_c_init_l(u32 t) { gabi::call(0x024EBAB4, t); }
static inline void dAttHint_c_proc_l(u32 t) { gabi::call(0x024EBACC, t); }
static inline void dAttCatch_c_init_l(u32 t) { gabi::call(0x024EBB24, t); }
static inline void dAttCatch_c_proc_l(u32 t) { gabi::call(0x024EBB44, t); }
static inline void dAttLook_c_init_l(u32 t) { gabi::call(0x024EBD4C, t); }
static inline void dAttLook_c_proc_l(u32 t) { gabi::call(0x024EBD64, t); }
static inline u32 dAttParam_c_ct0_l(u32 t) { return gabi::call<u32>(0x024EB904, t); }
static inline u32 dAttParam_c_ct_l(u32 t, s32 x) { return gabi::call<u32>(0x024EB984, t, x); }
static inline f32 distace_angle_adjust_l(f32 d, s32 a, f32 r) { return gabi::call<f32>(0x024EB12C, d, a, r); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline s16 lds16(u32 a) { return gabi::load<s16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
static inline u32 play() { return gabi::ea(dComIfGp_get()); }
static inline u32 searchByID(u32 id) {
    gabi::Local<be<u32>> key;
    *key = id;
    if (id == 0xFFFFFFFFu) return 0;
    return gabi::call<u32>(0x025D5218, 0x025E1234u, key.get());
}
static inline u32 getRes_l(u32 str, s32 idx) {
    gabi::Local<SafeString> key;
    key->mStringTop = str;
    key->__vtbl = SAFESTRING_VTBL_ATT;
    return gabi::call<u32>(0x026066C4, ld(0x101F4F28), key.get(), idx);
}

struct cXyz_l { be<f32> x, y, z; };
struct cSGlobe_l { be<f32> r; be<s16> v, u; };
struct Mtx_l { be<f32> m[12]; };

enum : u32 { DIST_TABLE = 0x101D3CC0, MTX_NOW = 0x1048D0CC, J3DSYS = 0x104B45C0 };
enum : u32 { AC_ATTN_DIST = 0x388, AC_ATTN_POS = 0x390, AC_ATTN_FLAGS = 0x39C, AC_EVT_COND = 0xFA, AC_SHAPE_Y = 0x32A };
enum : u32 { A_PLAYER = 0, A_TARGETID = 4, A_CALLBACK = 8, A_PADNO = 0xC, A_FLAGMASK = 0x10, A_LOCKSTATE = 0x18,
             A_LOCKSTATE2 = 0x19, A_BUTTON = 0x1A, A_1B = 0x1B, A_1C = 0x1C, A_FLAGS = 0x20, A_HEAP = 0x24, A_28 = 0x28,
             A_DRAWPOS = 0x2C, A_DRAW = 0x38, A_DRAWTARGET = 0x50, A_LLIST = 0x54, A_LCOUNT = 0xD4, A_LOFFSET = 0xD8,
             A_ALIST = 0xDC, A_ACOUNT = 0x11C, A_AOFFSET = 0x120, A_HINT = 0x124, A_CATCH = 0x130, A_LOOK0 = 0x148,
             A_LOOK1 = 0x158, A_ENEMYID = 0x168, A_ENEMYDIST = 0x16C, A_PARAM = 0x170 };

/* the name of the morf's current transform animation (resource name block, or NULL) */
static inline u32 morf_anm_name(u32 morf) {
    u32 p = ld(ld(morf + 0x94) + 8);
    s32 off = (s32)ld(p + 4);
    return off != 0 ? p + 4 + off : 0;
}

/* sead::SafeString operator== (inline): a = {name}, b = {lit}; `first` selects the HD sequence of
 * the first comparison (a direct assureTermination call on a) or a following one (a.cstr() twice) */
static inline bool safestring_eq(u32 a, u32 b, bool first) {
    if (first) {
        gabi::call(0x024EE634, a);
        gabi::call_ptr(ld(ld(a + 4) + 0x14), a);
    } else {
        gabi::call_ptr(ld(ld(a + 4) + 0x14), a);
        gabi::call_ptr(ld(ld(a + 4) + 0x14), a);
    }
    u32 p = ld(a);
    gabi::call_ptr(ld(ld(b + 4) + 0x14), b);
    u32 q = ld(b);
    if (p == q) return true;
    for (u32 n = 0x40001; n != 0; n--) {
        u8 c = ld8(p);
        u8 d = ld8(q);
        if (c != d) return false;
        if (c == 0) return true;
        p++;
        q++;
    }
    return false;
}

/* 024EC1C8: HD: bpk animation through mDoExt_bpkAnm; the arrow is calculated at a fixed frame
 * for yj_in / yj_out (the frame is then restored truncated to an integer) */
static void dAttDraw_c_draw(u32 i_this, u32 pos, u32 mtx) {
    WWHD_FUNC(0x024EC1C8, void, i_this, pos, mtx);
    f32 x = ldf(pos + 0), y = ldf(pos + 4), z = ldf(pos + 8);
    u32 model = ld(ld(i_this) + 0x90);
    PSMTXTrans_l(MTX_NOW, x, y, z);
    PSMTXConcat_l(MTX_NOW, mtx, MTX_NOW);
    {
        f32 m[12];
        for (int i = 0; i < 12; i++) m[i] = ldf(MTX_NOW + i * 4);
        for (int i = 0; i < 12; i++) stf(model + 0xC8 + i * 4, m[i]);
    }
    u32 clr = ld(i_this + 4);
    u32 md = ld(model + 0xAC);
    if (clr == 0) {
        getRes_l(0x10043144, 0x45);
        st(md + 0x40, 0); /* removeMatColorAnimator */
    } else {
        st(clr + 0, ld(ld(i_this) + 0x9C)); /* setFrame(anm->getFrame()): lfs/stfs pair, a bit copy in the recompiled code */
        bpkAnm_init_l(ld(i_this + 8), md, ld(i_this + 4), 1, 2, 1.0f, 0, -1, 1, 0);
        bpkAnm_entry_l(ld(i_this + 8), md, ldf(ld(i_this) + 0x9C));
    }
    u8 mono = ld8(0x101F4829);
    u32 p = play();
    if (mono != 0) { /* dComIfGd_setListP1 */
        st(J3DSYS + 0x74, ld(p + 0x5D58));
        st(J3DSYS + 0x78, ld(play() + 0x5D60));
    } else { /* dComIfGd_setListMaskOff */
        st(J3DSYS + 0x74, ld(p + 0x5D84));
        st(J3DSYS + 0x78, ld(play() + 0x5D88));
    }
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> b;
    gabi::Local<SafeString> c;
    a->mStringTop = morf_anm_name(ld(i_this));
    a->__vtbl = SAFESTRING_VTBL_ATT;
    b->mStringTop = 0x1004313C; /* "yj_in" */
    b->__vtbl = SAFESTRING_VTBL_ATT;
    s32 frameOff;
    if (safestring_eq(gabi::ea(a.get()), gabi::ea(b.get()), true)) {
        frameOff = 0xA2; /* end frame */
    } else {
        c->mStringTop = 0x1004314C; /* "yj_out" */
        c->__vtbl = SAFESTRING_VTBL_ATT;
        if (!safestring_eq(gabi::ea(a.get()), gabi::ea(c.get()), false)) {
            McaMorf_updateDL_l(ld(i_this));
            st(J3DSYS + 0x74, ld(play() + 0x5D78)); /* dComIfGd_setList */
            st(J3DSYS + 0x78, ld(play() + 0x5D7C));
            return;
        }
        frameOff = 0xA0; /* start frame */
    }
    u32 morf = ld(i_this);
    f32 f = (f32)(s16)gabi::ftoi((f32)lds16(morf + frameOff));
    f32 old = ldf(morf + 0x9C);
    stf(morf + 0x9C, f);
    McaMorf_calc_l(ld(i_this));
    f32 back = (f32)(s16)gabi::ftoi(old);
    stf(ld(i_this) + 0x9C, back);
    McaMorf_entryDL_l(ld(i_this));
    st(J3DSYS + 0x74, ld(play() + 0x5D78)); /* dComIfGd_setList */
    st(J3DSYS + 0x78, ld(play() + 0x5D7C));
}
VERIFY(0x024EC1C8, dAttDraw_c_draw);

static inline bool se_allowed() {
    if ((ld(play() + 0x5CD8) & 0x37A02371) == 0) return true;
    return (ld(play() + 0x5CDC) & 0x11) != 0;
}

/* 024ED2A0: HD: yj_out is not restarted while yj_delete is the current animation */
static void dAttention_c_runDrawProc(u32 i_this) {
    WWHD_FUNC(0x024ED2A0, void, i_this);
    u32 f = ld(i_this + A_FLAGS);
    if (f & 8) {
        setAnm_l(i_this + A_DRAW, 0x1B, 0x48, 0);
        if (se_allowed()) mDoAud_seStart_l(0x804);
    } else if (f & 0x10) {
        setAnm_l(i_this + A_DRAW, 0x17, 0x44, 0);
        if ((s8)ld8(i_this + A_28) >= 0) {
            u32 f2 = ld(i_this + A_FLAGS);
            st8(i_this + A_28, 1);
            st(i_this + A_FLAGS, f2 | 0x40000000);
        }
        if (se_allowed()) mDoAud_seStart_l(0x805);
    } else if (f & 1) {
        setAnm_l(i_this + A_DRAW, 0x18, 0x45, 0);
        st(i_this + A_FLAGS, ld(i_this + A_FLAGS) | 0x40000000);
    } else if (f & 2) {
        setAnm_l(i_this + A_DRAW, 0x18, 0x45, 0);
        setAnm_l(i_this + A_DRAW + 0xC, 0x1A, 0x47, 0);
        st(i_this + A_FLAGS, ld(i_this + A_FLAGS) | 0x40000000);
    } else if ((s32)ld(i_this + A_LCOUNT) <= 0 && (s8)ld8(i_this + A_28) == 0) {
        gabi::Local<SafeString> a;
        gabi::Local<SafeString> b;
        a->mStringTop = morf_anm_name(ld(i_this + A_DRAW));
        a->__vtbl = SAFESTRING_VTBL_ATT;
        b->mStringTop = 0x10043160; /* "yj_delete" */
        b->__vtbl = SAFESTRING_VTBL_ATT;
        if (!safestring_eq(gabi::ea(a.get()), gabi::ea(b.get()), true)) setAnm_l(i_this + A_DRAW, 0x1A, 0x47, 0);
        u32 f2 = ld(i_this + A_FLAGS);
        st8(i_this + A_28, 1);
        st(i_this + A_FLAGS, f2 | 0x40000000);
    }
    if (ld8(i_this + A_LOCKSTATE) == 1) {
        if (McaMorf_play_l(ld(i_this + A_DRAW), 0, 0, 0)) {
            setAnm_l(i_this + A_DRAW, 0x19, -1, 2);
            st(i_this + A_FLAGS, ld(i_this + A_FLAGS) & ~0x40000000u);
        }
    } else {
        if (McaMorf_play_l(ld(i_this + A_DRAW), 0, 0, 0)) {
            u32 f2 = ld(i_this + A_FLAGS);
            st8(i_this + A_28, 0xFF);
            st(i_this + A_FLAGS, f2 & ~0x40000000u);
        }
    }
    if (McaMorf_play_l(ld(i_this + A_DRAW + 0xC), 0, 0, 0)) st(i_this + A_DRAW + 0xC + 4, 0);
}
VERIFY(0x024ED2A0, dAttention_c_runDrawProc);

/* the inline member initialisation of both constructors (HD value-initialises the members) */
static inline void members_ct(u32 t) {
    st(t + A_PLAYER, 0);
    st8(t + A_28, 0);
    st8(t + A_LOCKSTATE, 0);
    st(t + A_CALLBACK, 0x1004321C); /* dAttDraw_CallBack_c vtable */
    st(t + A_HEAP, 0);
    st(t + A_TARGETID, 0);
    st(t + A_PADNO, 0);
    st8(t + A_LOCKSTATE2, 0);
    st(t + A_FLAGMASK, 0);
    st(t + 0x14, 0);
    st16(t + A_1C, 0);
    st(t + A_FLAGS, 0);
    st8(t + A_BUTTON, 0);
    st8(t + A_1B, 0);
    memzero_l(t + A_DRAW, 0x18);
    st(t + A_DRAWTARGET, 0);
    memzero_l(t + A_LLIST, 0x80);
    st(t + A_LOFFSET, 0);
    st(t + A_LCOUNT, 0);
    memzero_l(t + A_ALIST, 0x40);
    st(t + A_ACOUNT, 0);
    st(t + A_AOFFSET, 0);
    memzero_l(t + A_HINT, 0xC);
    memzero_l(t + A_CATCH, 0x18);
    memzero_l(t + A_LOOK0, 0x10);
    memzero_l(t + A_LOOK1, 0x10);
    st(t + A_ENEMYID, 0);
    stf(t + A_ENEMYDIST, 0.0f);
    dAttParam_c_ct0_l(t + A_PARAM);
}

/* 024ED630: HD dAttention_c() (value-initialising default constructor) */
static u32 dAttention_c_ct0(u32 i_this) {
    WWHD_FUNC(0x024ED630, u32, i_this);
    if (i_this == 0) {
        i_this = operator_new_l(0x198);
        if (i_this == 0) return 0;
    }
    members_ct(i_this);
    return i_this;
}
VERIFY(0x024ED630, dAttention_c_ct0);

/* 024ED73C: HD: the heap is created with size 0 (adjusted afterwards) and named; the bpk
 * animators are mDoExt_bpkAnm objects initialised with the animation that has the most
 * materials */
static u32 dAttention_c_ct(u32 i_this, u32 i_player, u32 i_padNo) {
    WWHD_FUNC(0x024ED73C, u32, i_this, i_player, i_padNo);
    if (i_this == 0) {
        i_this = operator_new_l(0x198);
        if (i_this == 0) return 0;
    }
    members_ct(i_this);
    st(i_this + A_PLAYER, i_player);
    st(i_this + A_PADNO, i_padNo);
    initList_l(i_this, 0xFFFFFFFF);
    st(i_this + A_FLAGMASK, 0);
    st8(i_this + A_1B, 0);
    st8(i_this + A_LOCKSTATE, 0);
    st(i_this + A_TARGETID, 0xFFFFFFFF);
    st8(i_this + A_BUTTON, 0);
    st16(i_this + A_1C, 0xFFFF);
    st8(i_this + A_LOCKSTATE2, 0);
    u32 heap = createSolidHeapFromGameToCurrent_l(0, 0);
    st(i_this + A_HEAP, heap);
    if (heap == 0) {
        JUT_ASSERT_l(0x10043174, 0x119, 0x10043184); /* heap != 0 */
        heap = ld(i_this + A_HEAP);
    }
    st(heap + 0x10, 0x100431A0); /* heap name */
    u32 modelData = getRes_l(0x1004316C, 0x3D);
    if (modelData == 0) JUT_ASSERT_l(0x10043174, 0x121, 0x100431B0);
    u32 anmColNum = 0;
    u32 maxCol = 0;
    for (u32 i = 0; i < 5; i++) {
        u32 anmCol = getRes_l(0x1004316C, ld16(0x101D5150 + i * 2));
        if (anmCol == 0) JUT_ASSERT_l(0x10043174, 0x132, 0x10043190);
        gabi::call_ptr(ld(ld(anmCol + 4) + 0x1C), anmCol, modelData); /* searchUpdateMaterialID */
        u32 tbl = ld(anmCol + 0xC);
        if ((s32)ld16(tbl + 0x14) > (s32)anmColNum) {
            anmColNum = ld16(tbl + 0x14);
            maxCol = anmCol;
        }
    }
    for (u32 i = 0; i < 2; i++) {
        u32 d = i_this + A_DRAW + i * 0xC;
        u32 bck = getRes_l(0x1004316C, 0x19);
        u32 morf = McaMorf_ct_l(0, modelData, i_this + A_CALLBACK, 0, bck, 2, 1.0f, 0, -1, 1, 0, 0x80000, 0x01000003);
        st(d, morf);
        if (morf == 0 || ld(morf + 0x90) == 0) JUT_ASSERT_l(0x10043174, 0x14B, 0x100431C4);
        st(d + 4, 0);
        u32 bpk = bpkAnm_ct_l(0);
        st(d + 8, bpk);
        bpkAnm_init_l(bpk, modelData, maxCol, 1, 2, 1.0f, 0, -1, 0, 0);
    }
    restoreCurrentHeap_l();
    if (adjustSolidHeap_l(ld(i_this + A_HEAP)) >= 0) {
        u32 h = ld(i_this + A_HEAP);
        u32 vt = ld(h + 0xC);
        u32 size = gabi::call_ptr<u32>(ld(vt + 0x6C), h);   /* getHeapSize */
        u32 start = gabi::call_ptr<u32>(ld(vt + 0x5C), h);  /* getStartAddr */
        DCStoreRangeNoSync_l(start, size);
    }
    st(i_this + A_FLAGS, 0);
    st8(i_this + A_28, 0xFF);
    dAttHint_c_init_l(i_this + A_HINT);
    dAttCatch_c_init_l(i_this + A_CATCH);
    dAttLook_c_init_l(i_this + A_LOOK0);
    dAttLook_c_init_l(i_this + A_LOOK1);
    if (i_this + A_PARAM != 0) dAttParam_c_ct_l(i_this + A_PARAM, 0);
    return i_this;
}
VERIFY(0x024ED73C, dAttention_c_ct);

/* 024EDB20 */
static void dAttention_c_dt(u32 i_this, s32 flags) {
    WWHD_FUNC(0x024EDB20, void, i_this, flags);
    if (i_this == 0) return;
    u32 heap = ld(i_this + A_HEAP);
    if (heap != 0) {
        destroySolidHeap_l(heap);
        st(i_this + A_HEAP, 0);
    }
    if (flags & 1) operator_delete_l(i_this);
}
VERIFY(0x024EDB20, dAttention_c_dt);

/* 024EDB80: HD: the flags kept at the start are 0xF0000000 (GameCube also kept 0x08000000) */
static bool dAttention_c_Run(u32 i_this, u32 interactMask) {
    WWHD_FUNC(0x024EDB80, bool, i_this, interactMask);
    u32 opt = dComIfGs_getOptAttentionType_l(ld(0x101F84DC) + 0x12C0);
    u32 f = ld(i_this + A_FLAGS);
    bool hold = ld8(opt) == 0;
    if (f & 0x80) {
        u32 pl = ld(play() + 0x5B2C);
        st(i_this + A_PADNO, 0);
        f = ld(i_this + A_FLAGS);
        st(i_this + A_PLAYER, pl);
    }
    st(i_this + A_FLAGS, f & 0xF0000000);
    if (ld8(play() + 0x5292) != 0) { /* dComIfGp_event_runCheck */
        u32 f2 = ld(i_this + A_FLAGS);
        st8(i_this + A_LOCKSTATE, 0);
        st8(i_this + A_BUTTON, 0);
        st(i_this + A_TARGETID, 0xFFFFFFFF);
        st8(i_this + A_1B, 0);
        st(i_this + A_FLAGS, f2 & 0xC0000000);
        freeAttention_l(i_this);
    } else {
        judgementButton_l(i_this);
        if (hold) judgementStatusHd_l(i_this, interactMask);
        else judgementStatusSw_l(i_this, interactMask);
        if (ld(i_this + A_FLAGS) & 0x10000000) {
            if (!mDoCPd_L_LOCK_BUTTON_l(ld(i_this + A_PADNO))) {
                u32 f2 = ld(i_this + A_FLAGS);
                if (f2 & 0x20000000) {
                    mDoAud_seStart_l(0x81D);
                    f2 = ld(i_this + A_FLAGS) & ~0x20000000u;
                }
                u8 s = ld8(i_this + A_LOCKSTATE);
                st(i_this + A_FLAGS, f2 & ~0x10000000u);
                st8(i_this + A_LOCKSTATE2, s);
                goto procs;
            }
        } else if (mDoCPd_L_LOCK_BUTTON_l(ld(i_this + A_PADNO))) {
            if (LockonTarget_l(i_this, 0) == 0) {
                st(i_this + A_FLAGS, ld(i_this + A_FLAGS) | 0x20000020);
                mDoAud_seStart_l(0x81C);
            }
            st(i_this + A_FLAGS, ld(i_this + A_FLAGS) | 0x10000000);
        }
    }
    st8(i_this + A_LOCKSTATE2, ld8(i_this + A_LOCKSTATE));
procs:
    runSoundProc_l(i_this);
    gabi::call(0x024ED2A0, i_this); /* runDrawProc */
    {
        u32 pad = ld(i_this + A_PADNO);
        u8 s = ld8(i_this + A_LOCKSTATE);
        u32 w = play() + pad * 0x34 + 0x5B00; /* camera attention status */
        if (s == 1) st(w, ld(w) | 1);
        else st(w, ld(w) & ~1u);
    }
    dAttHint_c_proc_l(i_this + A_HINT);
    dAttCatch_c_proc_l(i_this + A_CATCH);
    dAttLook_c_proc_l(i_this + A_LOOK0);
    dAttLook_c_proc_l(i_this + A_LOOK1);
    return true;
}
VERIFY(0x024EDB80, dAttention_c_Run);

static inline void copy_pos(u32 dst, u32 src) {
    st(dst + 0, ld(src + 0));
    st(dst + 4, ld(src + 4));
    st(dst + 8, ld(src + 8));
}

/* 024EDDF8 */
static void dAttention_c_Draw(u32 i_this) {
    WWHD_FUNC(0x024EDDF8, void, i_this);
    gabi::Local<Mtx_l> invCamera;
    PSMTXInverse_l(ld(play() + 0x5FA4) + 0x1E4, gabi::ea(invCamera.get()));
    u32 target = LockonTarget_l(i_this, 0);
    if (ld8(play() + 0x5292) != 0) return;   /* dComIfGp_event_runCheck */
    if (ld8(play() + 0x5BB3) != 0) return;   /* dComIfGp_getScopeMesgStatus */
    u32 inv = gabi::ea(invCamera.get());
    if (target != 0) {
        dAttDraw_c_draw(i_this + A_DRAW, target + AC_ATTN_POS, inv);
        s32 cnt = (s32)ld(i_this + A_LCOUNT);
        if (cnt >= 2 && ld(i_this + A_DRAW + 0xC + 4) != 0) {
            s32 off = (s32)ld(i_this + A_LOFFSET);
            if (off == 0) off = cnt;
            u32 e = i_this + A_LLIST + (off - 1) * 0x10;
            if (getActor_l(e) != 0) dAttDraw_c_draw(i_this + A_DRAW + 0xC, getActor_l(e) + AC_ATTN_POS, inv);
        }
        st(i_this + A_DRAWTARGET, LockonTargetPId_l(i_this, 0));
        st(i_this + A_DRAWPOS + 0, ld(target + AC_ATTN_POS + 0));
        st(i_this + A_DRAWPOS + 4, ld(target + AC_ATTN_POS + 4));
        u32 z = ld(target + AC_ATTN_POS + 8);
        st8(i_this + A_28, 0);
        st(i_this + A_DRAWPOS + 8, z);
    } else if ((s8)ld8(i_this + A_28) > 0) {
        target = searchByID(ld(i_this + A_DRAWTARGET));
        if (target != 0) {
            dAttDraw_c_draw(i_this + A_DRAW, target + AC_ATTN_POS, inv);
            copy_pos(i_this + A_DRAWPOS, target + AC_ATTN_POS);
        } else {
            dAttDraw_c_draw(i_this + A_DRAW, i_this + A_DRAWPOS, inv);
        }
    }
}
VERIFY(0x024EDDF8, dAttention_c_Draw);

/* 024EDFCC */
static bool dAttention_c_LockonTruth(u32 i_this) {
    WWHD_FUNC(0x024EDFCC, bool, i_this);
    u8 s = ld8(i_this + A_LOCKSTATE);
    if (s == 1) return true;
    if (s == 2 && LockonTarget_l(i_this, 0) != 0) return true;
    return false;
}
VERIFY(0x024EDFCC, dAttention_c_LockonTruth);

static inline u32 list_at(u32 i_this, u32 cntOff, u32 offOff, u32 listOff, s32 idx) {
    s32 cnt = (s32)ld(i_this + cntOff);
    if (cnt == 0) return 0;
    u32 a = ld(i_this + offOff) + (u32)idx;
    u32 r = a - ppc_divw(a, (u32)cnt) * (u32)cnt;
    return i_this + listOff + r * 0x10;
}

/* 024EE020 */
static u32 dAttention_c_GetActionList(u32 i_this, s32 idx) {
    WWHD_FUNC(0x024EE020, u32, i_this, idx);
    return list_at(i_this, A_ACOUNT, A_AOFFSET, A_ALIST, idx);
}
VERIFY(0x024EE020, dAttention_c_GetActionList);

/* 024EE058 */
static u32 dAttention_c_GetLockonList(u32 i_this, s32 idx) {
    WWHD_FUNC(0x024EE058, u32, i_this, idx);
    return list_at(i_this, A_LCOUNT, A_LOFFSET, A_LLIST, idx);
}
VERIFY(0x024EE058, dAttention_c_GetLockonList);

/* 024EE090: HD: the actors are null-checked */
static u32 dAttention_c_getActionBtnB(u32 i_this) {
    WWHD_FUNC(0x024EE090, u32, i_this);
    u32 list = dAttention_c_GetLockonList(i_this, 0);
    if (list != 0 && getActor_l(list) != 0 && ld(list + 8) == 1 && dAttention_c_LockonTruth(i_this)) {
        u32 a = getActor_l(list);
        if (a != 0 && !(ld(a + AC_ATTN_FLAGS) & 0x02000000)) return list;
    }
    if (ld(i_this + A_ACOUNT) == 0) return 0;
    for (s32 i = 0; i < (s32)ld(i_this + A_ACOUNT); i++) {
        u32 e = i_this + A_ALIST + i * 0x10;
        if (ld(e + 8) == 3) {
            u32 a = getActor_l(e);
            if (a == 0 || (ld(a + AC_ATTN_FLAGS) & 0x02000000)) continue;
        }
        return e;
    }
    return 0;
}
VERIFY(0x024EE090, dAttention_c_getActionBtnB);

/* eventInfo: condition (0xFA) CANTALKITEM 0x20, XY check callback (0x104) */
static inline bool xy_check(u32 a, s32 button) {
    if (!(ld16(a + AC_EVT_COND) & 0x20)) return false;
    u32 cb = ld(a + 0x104);
    if (cb == 0) return true;
    return gabi::call_ptr<s32>(cb, a, button) != 0;
}

/* 024EE190 */
static u32 dAttention_c_getActionBtnXYZ_local(u32 i_this, s32 button) {
    WWHD_FUNC(0x024EE190, u32, i_this, button);
    u32 list = dAttention_c_GetLockonList(i_this, 0);
    if (list != 0 && getActor_l(list) != 0 && ld(list + 8) == 1 && dAttention_c_LockonTruth(i_this)) {
        u32 a = getActor_l(list);
        if (xy_check(a, button)) return list;
        return 0;
    }
    if (ld(i_this + A_ACOUNT) == 0) return 0;
    for (s32 i = 0; i < (s32)ld(i_this + A_ACOUNT); i++) {
        u32 e = i_this + A_ALIST + i * 0x10;
        if (ld(e + 8) == 3) {
            u32 a = getActor_l(e);
            if (xy_check(a, button)) return e;
        }
    }
    return 0;
}
VERIFY(0x024EE190, dAttention_c_getActionBtnXYZ_local);

/* 024EE2F4 / 024EE350 / 024EE3AC / 024EE408: getActionBtnX / Y / Z and an HD fourth item button */
static inline u32 getActionBtn(u32 i_this, s32 k) {
    u8 item = ld8(play() + 0x5BBB + k);
    u32 ret = 0;
    if (dComIfGs_checkGetItemNum_l(item)) ret = dAttention_c_getActionBtnXYZ_local(i_this, k);
    return ret;
}
static u32 dAttention_c_getActionBtnX(u32 i_this) {
    WWHD_FUNC(0x024EE2F4, u32, i_this);
    return getActionBtn(i_this, 0);
}
VERIFY(0x024EE2F4, dAttention_c_getActionBtnX);
static u32 dAttention_c_getActionBtnY(u32 i_this) {
    WWHD_FUNC(0x024EE350, u32, i_this);
    return getActionBtn(i_this, 1);
}
VERIFY(0x024EE350, dAttention_c_getActionBtnY);
static u32 dAttention_c_getActionBtnZ(u32 i_this) {
    WWHD_FUNC(0x024EE3AC, u32, i_this);
    return getActionBtn(i_this, 2);
}
VERIFY(0x024EE3AC, dAttention_c_getActionBtnZ);
static u32 dAttention_c_getActionBtn4(u32 i_this) {
    WWHD_FUNC(0x024EE408, u32, i_this);
    return getActionBtn(i_this, 3);
}
VERIFY(0x024EE408, dAttention_c_getActionBtn4);

/* 024EE464: ActionTarget (matcher: LockonTarget) */
static u32 dAttention_c_ActionTarget(u32 i_this, s32 idx) {
    WWHD_FUNC(0x024EE464, u32, i_this, idx);
    s32 cnt = (s32)ld(i_this + A_ACOUNT);
    if (idx >= cnt) return 0;
    s32 listIdx = (s32)ld(i_this + A_AOFFSET) + idx;
    if (listIdx >= cnt) listIdx -= cnt;
    return getActor_l(i_this + A_ALIST + listIdx * 0x10);
}
VERIFY(0x024EE464, dAttention_c_ActionTarget);

/* 024EE49C */
static f32 dAttention_c_LockonReleaseDistanse(u32 i_this) {
    WWHD_FUNC(0x024EE49C, f32, i_this);
    if (!dAttention_c_LockonTruth(i_this)) return 0.0f;
    u32 actor = getActor_l(i_this + A_LLIST + ld(i_this + A_LOFFSET) * 0x10);
    if (actor == 0) return 0.0f;
    u32 e = i_this + A_LLIST + ld(i_this + A_LOFFSET) * 0x10;
    u32 p = ld(i_this + A_PLAYER);
    u8 idx = ld8(actor + ld(e + 8) + AC_ATTN_DIST);
    gabi::Local<be<s16>> angle;
    gabi::Local<cSGlobe_l> globe;
    gabi::Local<cXyz_l> d;
    cXyz_mi_l(actor + AC_ATTN_POS, gabi::ea(d.get()), p + AC_ATTN_POS);
    cSGlobe_ct_l(gabi::ea(globe.get()), gabi::ea(d.get()));
    cSAngle_mi_l(gabi::ea(globe.get()) + 6, gabi::ea(angle.get()), lds16(ld(i_this + A_PLAYER) + AC_SHAPE_Y));
    u32 dt = DIST_TABLE + idx * 0x1C;
    f32 r = distace_angle_adjust_l(ldf(dt + 8), (s16)*angle, 1.0f);
    return ldf(dt + 4) + r;
}
VERIFY(0x024EE49C, dAttention_c_LockonReleaseDistanse);

} // namespace d_attention_3_cpp
