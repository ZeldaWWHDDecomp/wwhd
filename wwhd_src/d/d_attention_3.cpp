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
#include "d/d_attention.h"

// HD: endian-aware image accesses retained for word copies and dynamic virtual slots.
namespace AttentionMemory3 {
static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline s16 lds16(u32 a) { return gabi::load<s16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
} // namespace AttentionMemory3

void att3_runDrawProc(u32);
void att3_assureTermination(u32);
u32 att3_getObjectRes(u32, u32, s32);
u32 att3_searchActor(u32, u32);

static constexpr u32 SAFESTRING_VTBL_ATT = 0x100430F8;

/* ---- local bindings (SHARED-CANDIDATE) ---- */
void att3_JUT_ASSERT(u32 file, s32 line, u32 msg);
void att3_memzero(u32 p, u32 n);
u32 att3_operator_new(u32 size);
void att3_operator_delete(u32 p);
void att3_cSAngle_mi(u32 a, u32 res, s16 v);
void att3_cSGlobe_ct(u32 g, u32 v);
void att3_cXyz_mi(u32 a, u32 res, u32 b);
void att3_PSMTXTrans(u32 m, f32 x, f32 y, f32 z);
void att3_PSMTXConcat(u32 a, u32 b, u32 ab);
BOOL att3_PSMTXInverse(u32 src, u32 dst);
void att3_mDoAud_seStart(u32 id); /* HD: sound id only */
BOOL att3_mDoCPd_L_LOCK_BUTTON(u32 pad);
u32 att3_dComIfGs_getOptAttentionType(u32 p);
BOOL att3_dComIfGs_checkGetItemNum(u8 item);
s32 att3_McaMorf_play(u32 morf, u32 pos, u32 a, u32 b);
void att3_McaMorf_calc(u32 morf);
void att3_McaMorf_entryDL(u32 morf);
void att3_McaMorf_updateDL(u32 morf);
/* mDoExt_McaMorf::mDoExt_McaMorf(this, modelData, cb1, cb2, anm, mode, speed, start, end; stack: 4 words) */
u32 att3_McaMorf_ct(u32 t, u32 md, u32 cb1, u32 cb2, u32 anm, s32 mode, f32 spd, s32 start, s32 end,
                    s32 s0, u32 s1, u32 s2, u32 s3);
/* mDoExt_bpkAnm: (this == NULL) constructor, init(modelData, anm, anmPlay, attr, rate, start, end, modify; stack: 1), entry */
u32 att3_bpkAnm_ct(u32 t);
s32 att3_bpkAnm_init(u32 t, u32 md, u32 anm, s32 play, s32 attr, f32 rate, s32 start, s32 end,
                     s32 modify, s32 s0);
void att3_bpkAnm_entry(u32 t, u32 md, f32 frame);
u32 att3_createSolidHeapFromGameToCurrent(u32 size, u32 align);
void att3_restoreCurrentHeap();
s32 att3_adjustSolidHeap(u32 heap);
void att3_DCStoreRangeNoSync(u32 p, u32 n);
void att3_destroySolidHeap(u32 heap);

/* functions of the other parts */
void att3_setAnm(u32 d, s32 bck, s32 bpk, s32 mode);
u32 att3_getActor(u32 e);
u32 att3_LockonTarget(u32 t, s32 i);
u32 att3_LockonTargetPId(u32 t, s32 i);
void att3_initList(u32 t, u32 m);
s32 att3_freeAttention(u32 t);
void att3_judgementButton(u32 t);
void att3_judgementStatusSw(u32 t, u32 m);
void att3_judgementStatusHd(u32 t, u32 m);
void att3_runSoundProc(u32 t);
void att3_dAttHint_c_init(u32 t);
void att3_dAttHint_c_proc(u32 t);
void att3_dAttCatch_c_init(u32 t);
void att3_dAttCatch_c_proc(u32 t);
void att3_dAttLook_c_init(u32 t);
void att3_dAttLook_c_proc(u32 t);
u32 att3_dAttParam_c_ct0(u32 t);
u32 att3_dAttParam_c_ct(u32 t, s32 x);
f32 att3_distace_angle_adjust(f32 d, s32 a, f32 r);

static inline u32 play() { return gabi::ea(dComIfGp_get()); }
static inline u32 searchByID(u32 id) {
    gabi::Local<be<u32>> key;
    *key = id;
    if (id == 0xFFFFFFFFu)
        return 0;
    return att3_searchActor(0x025E1234u, gabi::ea(key.get()));
}
static inline u32 getRes_l(u32 str, s32 idx) {
    gabi::Local<SafeString> key;
    key->mStringTop = str;
    key->__vtbl = SAFESTRING_VTBL_ATT;
    return att3_getObjectRes(AttentionMemory3::ld(0x101F4F28), gabi::ea(key.get()), idx);
}

struct Mtx_l {
    be<f32> m[12];
};

enum : u32 { DIST_TABLE = 0x101D3CC0, MTX_NOW = 0x1048D0CC, J3DSYS = 0x104B45C0 };
enum : u32 {
    AC_ATTN_DIST = 0x388,
    AC_ATTN_POS = 0x390,
    AC_ATTN_FLAGS = 0x39C,
    AC_EVT_COND = 0xFA,
    AC_SHAPE_Y = 0x32A
};
enum : u32 {
    A_PLAYER = 0,
    A_TARGETID = 4,
    A_CALLBACK = 8,
    A_PADNO = 0xC,
    A_FLAGMASK = 0x10,
    A_LOCKSTATE = 0x18,
    A_LOCKSTATE2 = 0x19,
    A_BUTTON = 0x1A,
    A_1B = 0x1B,
    A_1C = 0x1C,
    A_FLAGS = 0x20,
    A_HEAP = 0x24,
    A_28 = 0x28,
    A_DRAWPOS = 0x2C,
    A_DRAW = 0x38,
    A_DRAWTARGET = 0x50,
    A_LLIST = 0x54,
    A_LCOUNT = 0xD4,
    A_LOFFSET = 0xD8,
    A_ALIST = 0xDC,
    A_ACOUNT = 0x11C,
    A_AOFFSET = 0x120,
    A_HINT = 0x124,
    A_CATCH = 0x130,
    A_LOOK0 = 0x148,
    A_LOOK1 = 0x158,
    A_ENEMYID = 0x168,
    A_ENEMYDIST = 0x16C,
    A_PARAM = 0x170
};

/* the name of the morf's current transform animation (resource name block, or NULL) */
static inline u32 morf_anm_name(u32 morf) {
    u32 p = (u32)AttentionHD::view<AttentionHD::AnimationResource>(
                (u32)AttentionHD::view<AttentionHD::Morf>(morf)->mpAnm)
                ->mpNameBlock;
    s32 off = AttentionHD::view<AttentionHD::ResourceNameBlock>(p)->mNameOffset;
    return off != 0 ? p + 4 + off : 0;
}

/* sead::SafeString operator== (inline): a = {name}, b = {lit}; `first` selects the HD sequence of
 * the first comparison (a direct assureTermination call on a) or a following one (a.cstr() twice) */
static inline bool safestring_eq(u32 a, u32 b, bool first) {
    if (first) {
        att3_assureTermination(a);
        gabi::call_ptr(AttentionMemory3::ld(AttentionMemory3::ld(a + 4) + 0x14), a);
    } else {
        gabi::call_ptr(AttentionMemory3::ld(AttentionMemory3::ld(a + 4) + 0x14), a);
        gabi::call_ptr(AttentionMemory3::ld(AttentionMemory3::ld(a + 4) + 0x14), a);
    }
    u32 p = AttentionMemory3::ld(a);
    gabi::call_ptr(AttentionMemory3::ld(AttentionMemory3::ld(b + 4) + 0x14), b);
    u32 q = AttentionMemory3::ld(b);
    if (p == q)
        return true;
    for (u32 n = 0x40001; n != 0; n--) {
        u8 c = AttentionMemory3::ld8(p);
        u8 d = AttentionMemory3::ld8(q);
        if (c != d)
            return false;
        if (c == 0)
            return true;
        p++;
        q++;
    }
    return false;
}

// HD: bpk animation through mDoExt_bpkAnm; the arrow is calculated at a fixed frame
// for yj_in / yj_out (the frame is then restored truncated to an integer)
/* 024EC1C8 */
static void dAttDraw_c_draw(u32 i_this, u32 pos, u32 mtx) {
    auto* self = AttentionHD::view<AttentionHD::Draw>(i_this);
    f32 x = AttentionMemory3::ldf(pos + 0), y = AttentionMemory3::ldf(pos + 4),
        z = AttentionMemory3::ldf(pos + 8);
    u32 model = AttentionMemory3::ld((u32)self->anm + 0x90);
    att3_PSMTXTrans(MTX_NOW, x, y, z);
    att3_PSMTXConcat(MTX_NOW, mtx, MTX_NOW);
    {
        f32 m[12];
        for (int i = 0; i < 12; i++)
            m[i] = AttentionMemory3::ldf(MTX_NOW + i * 4);
        for (int i = 0; i < 12; i++)
            AttentionMemory3::stf(model + 0xC8 + i * 4, m[i]);
    }
    u32 clr = (u32)self->mpAnmClr;
    u32 md = (u32)AttentionHD::view<AttentionHD::Model>(model)->mpModelData;
    if (clr == 0) {
        getRes_l(0x10043144, 0x45);
        AttentionMemory3::st(md + 0x40, 0); /* removeMatColorAnimator */
    } else {
        AttentionMemory3::st(
            clr + 0,
            AttentionMemory3::ld(
                (u32)self->anm +
                0x9C)); /* setFrame(anm->getFrame()): lfs/stfs pair, a bit copy in the recompiled code */
        att3_bpkAnm_init((u32)self->mpAnmMatClr, md, (u32)self->mpAnmClr, 1, 2, 1.0f, 0, -1, 1, 0);
        att3_bpkAnm_entry((u32)self->mpAnmMatClr, md, AttentionMemory3::ldf((u32)self->anm + 0x9C));
    }
    u8 mono = AttentionMemory3::ld8(0x101F4829);
    u32 p = play();
    if (mono != 0) { /* dComIfGd_setListP1 */
        AttentionMemory3::st(J3DSYS + 0x74, AttentionMemory3::ld(p + 0x5D58));
        AttentionMemory3::st(J3DSYS + 0x78, AttentionMemory3::ld(play() + 0x5D60));
    } else { /* dComIfGd_setListMaskOff */
        AttentionMemory3::st(J3DSYS + 0x74, AttentionMemory3::ld(p + 0x5D84));
        AttentionMemory3::st(J3DSYS + 0x78, AttentionMemory3::ld(play() + 0x5D88));
    }
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> b;
    gabi::Local<SafeString> c;
    a->mStringTop = morf_anm_name((u32)self->anm);
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
            att3_McaMorf_updateDL((u32)self->anm);
            AttentionMemory3::st(J3DSYS + 0x74,
                                 AttentionMemory3::ld(play() + 0x5D78)); /* dComIfGd_setList */
            AttentionMemory3::st(J3DSYS + 0x78, AttentionMemory3::ld(play() + 0x5D7C));
            return;
        }
        frameOff = 0xA0; /* start frame */
    }
    u32 morf = (u32)self->anm;
    f32 f = (f32)(s16)gabi::ftoi((f32)AttentionMemory3::lds16(morf + frameOff));
    f32 old = (f32)AttentionHD::view<AttentionHD::Morf>(morf)->mFrame;
    AttentionHD::view<AttentionHD::Morf>(morf)->mFrame = f;
    att3_McaMorf_calc((u32)self->anm);
    f32 back = (f32)(s16)gabi::ftoi(old);
    AttentionMemory3::stf((u32)self->anm + 0x9C, back);
    att3_McaMorf_entryDL((u32)self->anm);
    AttentionMemory3::st(J3DSYS + 0x74,
                         AttentionMemory3::ld(play() + 0x5D78)); /* dComIfGd_setList */
    AttentionMemory3::st(J3DSYS + 0x78, AttentionMemory3::ld(play() + 0x5D7C));
}

static inline bool se_allowed() {
    if ((AttentionMemory3::ld(play() + 0x5CD8) & 0x37A02371) == 0)
        return true;
    return (AttentionMemory3::ld(play() + 0x5CDC) & 0x11) != 0;
}

// HD: yj_out is not restarted while yj_delete is the current animation
/* 024ED2A0 */
static void dAttention_c_runDrawProc(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    u32 f = (u32)self->mFlags;
    if (f & 8) {
        att3_setAnm(i_this + A_DRAW, 0x1B, 0x48, 0);
        if (se_allowed())
            att3_mDoAud_seStart(0x804);
    } else if (f & 0x10) {
        att3_setAnm(i_this + A_DRAW, 0x17, 0x44, 0);
        if ((s8)(u8)self->field_0x028 >= 0) {
            u32 f2 = (u32)self->mFlags;
            self->field_0x028 = 1;
            self->mFlags = f2 | 0x40000000;
        }
        if (se_allowed())
            att3_mDoAud_seStart(0x805);
    } else if (f & 1) {
        att3_setAnm(i_this + A_DRAW, 0x18, 0x45, 0);
        self->mFlags = (u32)self->mFlags | 0x40000000;
    } else if (f & 2) {
        att3_setAnm(i_this + A_DRAW, 0x18, 0x45, 0);
        att3_setAnm(i_this + A_DRAW + 0xC, 0x1A, 0x47, 0);
        self->mFlags = (u32)self->mFlags | 0x40000000;
    } else if ((s32)(u32)self->mLockonCount <= 0 && (s8)(u8)self->field_0x028 == 0) {
        gabi::Local<SafeString> a;
        gabi::Local<SafeString> b;
        a->mStringTop = morf_anm_name((u32)self->draw[0].anm);
        a->__vtbl = SAFESTRING_VTBL_ATT;
        b->mStringTop = 0x10043160; /* "yj_delete" */
        b->__vtbl = SAFESTRING_VTBL_ATT;
        if (!safestring_eq(gabi::ea(a.get()), gabi::ea(b.get()), true))
            att3_setAnm(i_this + A_DRAW, 0x1A, 0x47, 0);
        u32 f2 = (u32)self->mFlags;
        self->field_0x028 = 1;
        self->mFlags = f2 | 0x40000000;
    }
    if ((u8)self->mLockOnState == 1) {
        if (att3_McaMorf_play((u32)self->draw[0].anm, 0, 0, 0)) {
            att3_setAnm(i_this + A_DRAW, 0x19, -1, 2);
            self->mFlags = (u32)self->mFlags & ~0x40000000u;
        }
    } else {
        if (att3_McaMorf_play((u32)self->draw[0].anm, 0, 0, 0)) {
            u32 f2 = (u32)self->mFlags;
            self->field_0x028 = 0xFF;
            self->mFlags = f2 & ~0x40000000u;
        }
    }
    if (att3_McaMorf_play((u32)self->draw[1].anm, 0, 0, 0))
        self->draw[1].mpAnmClr = 0;
}

/* the inline member initialisation of both constructors (HD value-initialises the members) */
static inline void members_ct(u32 t) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(t);
    self->mpPlayer = 0;
    self->field_0x028 = 0;
    self->mLockOnState = 0;
    self->mCallBack = 0x1004321C; /* dAttDraw_CallBack_c vtable */
    self->heap = 0;
    self->mLockonTargetID = 0;
    self->mPadNo = 0;
    self->field_0x019 = 0;
    self->mFlagMask = 0;
    AttentionMemory3::st(t + 0x14, 0);
    self->field_0x01c = 0;
    self->mFlags = 0;
    self->field_0x01a = 0;
    self->field_0x01b = 0;
    att3_memzero(t + A_DRAW, 0x18);
    self->mDrawLockonTargetID = 0;
    att3_memzero(t + A_LLIST, 0x80);
    self->mLockOnOffset = 0;
    self->mLockonCount = 0;
    att3_memzero(t + A_ALIST, 0x40);
    self->mActionCount = 0;
    self->mActionOffset = 0;
    att3_memzero(t + A_HINT, 0xC);
    att3_memzero(t + A_CATCH, 0x18);
    att3_memzero(t + A_LOOK0, 0x10);
    att3_memzero(t + A_LOOK1, 0x10);
    self->mEnemyID = 0;
    self->mEnemyDistance = 0.0f;
    att3_dAttParam_c_ct0(t + A_PARAM);
}

// HD: HD dAttention_c() (value-initialising default constructor)
/* 024ED630 */
static u32 dAttention_c_ct0(u32 i_this) {
    if (i_this == 0) {
        i_this = att3_operator_new(0x198);
        if (i_this == 0)
            return 0;
    }
    members_ct(i_this);
    return i_this;
}

// HD: the heap is created with size 0 (adjusted afterwards) and named; the bpk
// animators are mDoExt_bpkAnm objects initialised with the animation that has the most
// materials
/* 024ED73C */
static u32 dAttention_c_ct(u32 i_this, u32 i_player, u32 i_padNo) {
    if (i_this == 0) {
        i_this = att3_operator_new(0x198);
        if (i_this == 0)
            return 0;
    }
    members_ct(i_this);
    AttentionHD::view<AttentionHD::Attention>(i_this)->mpPlayer = i_player;
    AttentionHD::view<AttentionHD::Attention>(i_this)->mPadNo = i_padNo;
    att3_initList(i_this, 0xFFFFFFFF);
    AttentionHD::view<AttentionHD::Attention>(i_this)->mFlagMask = 0;
    AttentionHD::view<AttentionHD::Attention>(i_this)->field_0x01b = 0;
    AttentionHD::view<AttentionHD::Attention>(i_this)->mLockOnState = 0;
    AttentionHD::view<AttentionHD::Attention>(i_this)->mLockonTargetID = 0xFFFFFFFF;
    AttentionHD::view<AttentionHD::Attention>(i_this)->field_0x01a = 0;
    AttentionHD::view<AttentionHD::Attention>(i_this)->field_0x01c = 0xFFFF;
    AttentionHD::view<AttentionHD::Attention>(i_this)->field_0x019 = 0;
    u32 heap = att3_createSolidHeapFromGameToCurrent(0, 0);
    AttentionHD::view<AttentionHD::Attention>(i_this)->heap = heap;
    if (heap == 0) {
        att3_JUT_ASSERT(0x10043174, 0x119, 0x10043184); /* heap != 0 */
        heap = (u32)AttentionHD::view<AttentionHD::Attention>(i_this)->heap;
    }
    AttentionHD::view<AttentionHD::Heap>(heap)->mName = 0x100431A0; /* heap name */
    u32 modelData = getRes_l(0x1004316C, 0x3D);
    if (modelData == 0)
        att3_JUT_ASSERT(0x10043174, 0x121, 0x100431B0);
    u32 anmColNum = 0;
    u32 maxCol = 0;
    for (u32 i = 0; i < 5; i++) {
        u32 anmCol = getRes_l(0x1004316C, AttentionMemory3::ld16(0x101D5150 + i * 2));
        if (anmCol == 0)
            att3_JUT_ASSERT(0x10043174, 0x132, 0x10043190);
        gabi::call_ptr(AttentionMemory3::ld(AttentionMemory3::ld(anmCol + 4) + 0x1C), anmCol,
                       modelData); /* searchUpdateMaterialID */
        u32 tbl =
            (u32)AttentionHD::view<AttentionHD::ColorAnimation>(anmCol)->mpUpdateMaterialTable;
        if ((s32)(u16)AttentionHD::view<AttentionHD::UpdateMaterialTable>(tbl)->mUpdateMaterialNum >
            (s32)anmColNum) {
            anmColNum =
                (u16)AttentionHD::view<AttentionHD::UpdateMaterialTable>(tbl)->mUpdateMaterialNum;
            maxCol = anmCol;
        }
    }
    for (u32 i = 0; i < 2; i++) {
        u32 d = i_this + A_DRAW + i * 0xC;
        u32 bck = getRes_l(0x1004316C, 0x19);
        u32 morf = att3_McaMorf_ct(0, modelData, i_this + A_CALLBACK, 0, bck, 2, 1.0f, 0, -1, 1, 0,
                                   0x80000, 0x01000003);
        AttentionHD::view<AttentionHD::Draw>(d)->anm = morf;
        if (morf == 0 || (u32)AttentionHD::view<AttentionHD::Morf>(morf)->mpModel == 0)
            att3_JUT_ASSERT(0x10043174, 0x14B, 0x100431C4);
        AttentionHD::view<AttentionHD::Draw>(d)->mpAnmClr = 0;
        u32 bpk = att3_bpkAnm_ct(0);
        AttentionHD::view<AttentionHD::Draw>(d)->mpAnmMatClr = bpk;
        att3_bpkAnm_init(bpk, modelData, maxCol, 1, 2, 1.0f, 0, -1, 0, 0);
    }
    att3_restoreCurrentHeap();
    if (att3_adjustSolidHeap((u32)AttentionHD::view<AttentionHD::Attention>(i_this)->heap) >= 0) {
        u32 h = (u32)AttentionHD::view<AttentionHD::Attention>(i_this)->heap;
        u32 vt = (u32)AttentionHD::view<AttentionHD::Heap>(h)->__vtbl;
        u32 size = gabi::call_ptr<u32>(AttentionMemory3::ld(vt + 0x6C), h);  /* getHeapSize */
        u32 start = gabi::call_ptr<u32>(AttentionMemory3::ld(vt + 0x5C), h); /* getStartAddr */
        att3_DCStoreRangeNoSync(start, size);
    }
    AttentionHD::view<AttentionHD::Attention>(i_this)->mFlags = 0;
    AttentionHD::view<AttentionHD::Attention>(i_this)->field_0x028 = 0xFF;
    att3_dAttHint_c_init(i_this + A_HINT);
    att3_dAttCatch_c_init(i_this + A_CATCH);
    att3_dAttLook_c_init(i_this + A_LOOK0);
    att3_dAttLook_c_init(i_this + A_LOOK1);
    if (i_this + A_PARAM != 0)
        att3_dAttParam_c_ct(i_this + A_PARAM, 0);
    return i_this;
}

/* 024EDB20 */
static void dAttention_c_dt(u32 i_this, s32 flags) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    if (i_this == 0)
        return;
    u32 heap = (u32)self->heap;
    if (heap != 0) {
        att3_destroySolidHeap(heap);
        self->heap = 0;
    }
    if (flags & 1)
        att3_operator_delete(i_this);
}

// HD: the flags kept at the start are 0xF0000000 (GameCube also kept 0x08000000)
/* 024EDB80 */
static bool dAttention_c_Run(u32 i_this, u32 interactMask) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    bool lockStateCopied = false;
    u32 opt = att3_dComIfGs_getOptAttentionType(AttentionMemory3::ld(0x101F84DC) + 0x12C0);
    u32 f = (u32)self->mFlags;
    bool hold = AttentionMemory3::ld8(opt) == 0;
    if (f & 0x80) {
        u32 pl = AttentionMemory3::ld(play() + 0x5B2C);
        self->mPadNo = 0;
        f = (u32)self->mFlags;
        self->mpPlayer = pl;
    }
    self->mFlags = f & 0xF0000000;
    if (AttentionMemory3::ld8(play() + 0x5292) != 0) { /* dComIfGp_event_runCheck */
        u32 f2 = (u32)self->mFlags;
        self->mLockOnState = 0;
        self->field_0x01a = 0;
        self->mLockonTargetID = 0xFFFFFFFF;
        self->field_0x01b = 0;
        self->mFlags = f2 & 0xC0000000;
        att3_freeAttention(i_this);
    } else {
        att3_judgementButton(i_this);
        if (hold)
            att3_judgementStatusHd(i_this, interactMask);
        else
            att3_judgementStatusSw(i_this, interactMask);
        if ((u32)self->mFlags & 0x10000000) {
            if (!att3_mDoCPd_L_LOCK_BUTTON((u32)self->mPadNo)) {
                u32 f2 = (u32)self->mFlags;
                if (f2 & 0x20000000) {
                    att3_mDoAud_seStart(0x81D);
                    f2 = (u32)self->mFlags & ~0x20000000u;
                }
                u8 s = (u8)self->mLockOnState;
                self->mFlags = f2 & ~0x10000000u;
                self->field_0x019 = s;
                lockStateCopied = true;
            }
        } else if (att3_mDoCPd_L_LOCK_BUTTON((u32)self->mPadNo)) {
            if (att3_LockonTarget(i_this, 0) == 0) {
                self->mFlags = (u32)self->mFlags | 0x20000020;
                att3_mDoAud_seStart(0x81C);
            }
            self->mFlags = (u32)self->mFlags | 0x10000000;
        }
    }
    if (!lockStateCopied)
        self->field_0x019 = (u8)self->mLockOnState;
    att3_runSoundProc(i_this);
    att3_runDrawProc(i_this); /* runDrawProc */
    {
        u32 pad = (u32)self->mPadNo;
        u8 s = (u8)self->mLockOnState;
        u32 w = play() + pad * 0x34 + 0x5B00; /* camera attention status */
        if (s == 1)
            AttentionMemory3::st(w, AttentionMemory3::ld(w) | 1);
        else
            AttentionMemory3::st(w, AttentionMemory3::ld(w) & ~1u);
    }
    att3_dAttHint_c_proc(i_this + A_HINT);
    att3_dAttCatch_c_proc(i_this + A_CATCH);
    att3_dAttLook_c_proc(i_this + A_LOOK0);
    att3_dAttLook_c_proc(i_this + A_LOOK1);
    return true;
}

static inline void copy_pos(u32 dst, u32 src) {
    AttentionMemory3::st(dst + 0, AttentionMemory3::ld(src + 0));
    AttentionMemory3::st(dst + 4, AttentionMemory3::ld(src + 4));
    AttentionMemory3::st(dst + 8, AttentionMemory3::ld(src + 8));
}

/* 024EDDF8 */
static void dAttention_c_Draw(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    gabi::Local<Mtx_l> invCamera;
    att3_PSMTXInverse(AttentionMemory3::ld(play() + 0x5FA4) + 0x1E4, gabi::ea(invCamera.get()));
    u32 target = att3_LockonTarget(i_this, 0);
    if (AttentionMemory3::ld8(play() + 0x5292) != 0)
        return; /* dComIfGp_event_runCheck */
    if (AttentionMemory3::ld8(play() + 0x5BB3) != 0)
        return; /* dComIfGp_getScopeMesgStatus */
    u32 inv = gabi::ea(invCamera.get());
    if (target != 0) {
        dAttDraw_c_draw(i_this + A_DRAW, target + AC_ATTN_POS, inv);
        s32 cnt = (s32)(u32)self->mLockonCount;
        if (cnt >= 2 && AttentionMemory3::ld(i_this + A_DRAW + 0xC + 4) != 0) {
            s32 off = (s32)(u32)self->mLockOnOffset;
            if (off == 0)
                off = cnt;
            u32 e = i_this + A_LLIST + (off - 1) * 0x10;
            if (att3_getActor(e) != 0)
                dAttDraw_c_draw(i_this + A_DRAW + 0xC, att3_getActor(e) + AC_ATTN_POS, inv);
        }
        self->mDrawLockonTargetID = att3_LockonTargetPId(i_this, 0);
        AttentionMemory3::st(i_this + A_DRAWPOS + 0,
                             AttentionMemory3::ld(target + AC_ATTN_POS + 0));
        AttentionMemory3::st(i_this + A_DRAWPOS + 4,
                             AttentionMemory3::ld(target + AC_ATTN_POS + 4));
        u32 z = AttentionMemory3::ld(target + AC_ATTN_POS + 8);
        self->field_0x028 = 0;
        AttentionMemory3::st(i_this + A_DRAWPOS + 8, z);
    } else if ((s8)(u8)self->field_0x028 > 0) {
        target = searchByID((u32)self->mDrawLockonTargetID);
        if (target != 0) {
            dAttDraw_c_draw(i_this + A_DRAW, target + AC_ATTN_POS, inv);
            copy_pos(i_this + A_DRAWPOS, target + AC_ATTN_POS);
        } else {
            dAttDraw_c_draw(i_this + A_DRAW, i_this + A_DRAWPOS, inv);
        }
    }
}

/* 024EDFCC */
static bool dAttention_c_LockonTruth(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    u8 s = (u8)self->mLockOnState;
    if (s == 1)
        return true;
    if (s == 2 && att3_LockonTarget(i_this, 0) != 0)
        return true;
    return false;
}

static inline u32 list_at(u32 i_this, u32 cntOff, u32 offOff, u32 listOff, s32 idx) {
    s32 cnt = (s32)AttentionMemory3::ld(i_this + cntOff);
    if (cnt == 0)
        return 0;
    u32 a = AttentionMemory3::ld(i_this + offOff) + (u32)idx;
    u32 r = a - ppc_divw(a, (u32)cnt) * (u32)cnt;
    return i_this + listOff + r * 0x10;
}

/* 024EE020 */
static u32 dAttention_c_GetActionList(u32 i_this, s32 idx) {
    return list_at(i_this, A_ACOUNT, A_AOFFSET, A_ALIST, idx);
}

/* 024EE058 */
static u32 dAttention_c_GetLockonList(u32 i_this, s32 idx) {
    return list_at(i_this, A_LCOUNT, A_LOFFSET, A_LLIST, idx);
}

// HD: the actors are null-checked
/* 024EE090 */
static u32 dAttention_c_getActionBtnB(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    u32 list = dAttention_c_GetLockonList(i_this, 0);
    if (list != 0 && att3_getActor(list) != 0 &&
        (u32)AttentionHD::view<AttentionHD::List>(list)->mType == 1 &&
        dAttention_c_LockonTruth(i_this)) {
        u32 a = att3_getActor(list);
        if (a != 0 && !((u32)AttentionHD::view<fopAc_ac_c>(a)->attention_info.flags & 0x02000000))
            return list;
    }
    if ((u32)self->mActionCount == 0)
        return 0;
    for (s32 i = 0; i < (s32)(u32)self->mActionCount; i++) {
        u32 e = i_this + A_ALIST + i * 0x10;
        if ((u32)AttentionHD::view<AttentionHD::List>(e)->mType == 3) {
            u32 a = att3_getActor(e);
            if (a == 0 ||
                ((u32)AttentionHD::view<fopAc_ac_c>(a)->attention_info.flags & 0x02000000))
                continue;
        }
        return e;
    }
    return 0;
}

/* eventInfo: condition (0xFA) CANTALKITEM 0x20, XY check callback (0x104) */
static inline bool xy_check(u32 a, s32 button) {
    if (!((u16)AttentionHD::view<fopAc_ac_c>(a)->eventInfo.mCondition & 0x20))
        return false;
    u32 cb = (u32)AttentionHD::view<fopAc_ac_c>(a)->eventInfo.mpCheckCB;
    if (cb == 0)
        return true;
    return gabi::call_ptr<s32>(cb, a, button) != 0;
}

/* 024EE190 */
static u32 dAttention_c_getActionBtnXYZ_local(u32 i_this, s32 button) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    u32 list = dAttention_c_GetLockonList(i_this, 0);
    if (list != 0 && att3_getActor(list) != 0 &&
        (u32)AttentionHD::view<AttentionHD::List>(list)->mType == 1 &&
        dAttention_c_LockonTruth(i_this)) {
        u32 a = att3_getActor(list);
        if (xy_check(a, button))
            return list;
        return 0;
    }
    if ((u32)self->mActionCount == 0)
        return 0;
    for (s32 i = 0; i < (s32)(u32)self->mActionCount; i++) {
        u32 e = i_this + A_ALIST + i * 0x10;
        if ((u32)AttentionHD::view<AttentionHD::List>(e)->mType == 3) {
            u32 a = att3_getActor(e);
            if (xy_check(a, button))
                return e;
        }
    }
    return 0;
}

/* 024EE2F4 / 024EE350 / 024EE3AC / 024EE408: getActionBtnX / Y / Z and an HD fourth item button */
static inline u32 getActionBtn(u32 i_this, s32 k) {
    u8 item = AttentionMemory3::ld8(play() + 0x5BBB + k);
    u32 ret = 0;
    if (att3_dComIfGs_checkGetItemNum(item))
        ret = dAttention_c_getActionBtnXYZ_local(i_this, k);
    return ret;
}
/* 024EE2F4 */
static u32 dAttention_c_getActionBtnX(u32 i_this) { return getActionBtn(i_this, 0); }
/* 024EE350 */
static u32 dAttention_c_getActionBtnY(u32 i_this) { return getActionBtn(i_this, 1); }
/* 024EE3AC */
static u32 dAttention_c_getActionBtnZ(u32 i_this) { return getActionBtn(i_this, 2); }
/* 024EE408 */
static u32 dAttention_c_getActionBtn4(u32 i_this) { return getActionBtn(i_this, 3); }

// HD: ActionTarget (matcher: LockonTarget)
/* 024EE464 */
static u32 dAttention_c_ActionTarget(u32 i_this, s32 idx) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    s32 cnt = (s32)(u32)self->mActionCount;
    if (idx >= cnt)
        return 0;
    s32 listIdx = (s32)(u32)self->mActionOffset + idx;
    if (listIdx >= cnt)
        listIdx -= cnt;
    return att3_getActor(i_this + A_ALIST + listIdx * 0x10);
}

/* 024EE49C */
static f32 dAttention_c_LockonReleaseDistanse(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    if (!dAttention_c_LockonTruth(i_this))
        return 0.0f;
    u32 actor = att3_getActor(i_this + A_LLIST + (u32)self->mLockOnOffset * 0x10);
    if (actor == 0)
        return 0.0f;
    u32 e = i_this + A_LLIST + (u32)self->mLockOnOffset * 0x10;
    u32 p = (u32)self->mpPlayer;
    u8 idx = AttentionMemory3::ld8(actor + (u32)AttentionHD::view<AttentionHD::List>(e)->mType +
                                   AC_ATTN_DIST);
    gabi::Local<be<s16>> angle;
    gabi::Local<AttentionHD::Globe> globe;
    gabi::Local<cXyz> d;
    att3_cXyz_mi(actor + AC_ATTN_POS, gabi::ea(d.get()), p + AC_ATTN_POS);
    att3_cSGlobe_ct(gabi::ea(globe.get()), gabi::ea(d.get()));
    att3_cSAngle_mi(gabi::ea(globe.get()) + 6, gabi::ea(angle.get()),
                    AttentionMemory3::lds16((u32)self->mpPlayer + AC_SHAPE_Y));
    u32 dt = DIST_TABLE + idx * 0x1C;
    f32 r = att3_distace_angle_adjust(
        (f32)AttentionHD::view<AttentionHD::DistTbl>(dt)->mDistXZAngleAdjust, (s16)*angle, 1.0f);
    return (f32)AttentionHD::view<AttentionHD::DistTbl>(dt)->mDistXZMaxRelease + r;
}

/* WWHD glue (tools/verify/autoglue.py)
ext 024ED2A0 void att3_runDrawProc(u32)
ext 0273AA24 void att3_JUT_ASSERT(u32,s32,u32)
ext 028F521C void att3_memzero(u32,u32)
ext 0273AD10 u32 att3_operator_new(u32)
ext 0273AF40 void att3_operator_delete(u32)
ext 02006908 void att3_cSAngle_mi(u32,u32,s16)
ext 02007324 void att3_cSGlobe_ct(u32,u32)
ext 0201ADE0 void att3_cXyz_mi(u32,u32,u32)
ext 028E93CC void att3_PSMTXTrans(u32,f32,f32,f32)
ext 028E9108 void att3_PSMTXConcat(u32,u32,u32)
ext 028E91EC BOOL att3_PSMTXInverse(u32,u32)
ext 025E1988 void att3_mDoAud_seStart(u32)
ext 02007CA4 BOOL att3_mDoCPd_L_LOCK_BUTTON(u32)
ext 027200D0 u32 att3_dComIfGs_getOptAttentionType(u32)
ext 02520FE4 BOOL att3_dComIfGs_checkGetItemNum(u8)
ext 025E535C s32 att3_McaMorf_play(u32,u32,u32,u32)
ext 025E55A0 void att3_McaMorf_calc(u32)
ext 025E5590 void att3_McaMorf_entryDL(u32)
ext 025E54D8 void att3_McaMorf_updateDL(u32)
ext 025E4F64 u32 att3_McaMorf_ct(u32,u32,u32,u32,u32,s32,f32,s32,s32,s32,u32,u32,u32)
ext 025E7480 u32 att3_bpkAnm_ct(u32)
ext 025E74FC s32 att3_bpkAnm_init(u32,u32,u32,s32,s32,f32,s32,s32,s32,s32)
ext 025E779C void att3_bpkAnm_entry(u32,u32,f32)
ext 025E3630 u32 att3_createSolidHeapFromGameToCurrent(u32,u32)
ext 025E37D8 void att3_restoreCurrentHeap()
ext 025E3678 s32 att3_adjustSolidHeap(u32)
ext C00088B8 void att3_DCStoreRangeNoSync(u32,u32)
ext 025E3868 void att3_destroySolidHeap(u32)
ext 024EC10C void att3_setAnm(u32,s32,s32,s32)
ext 024EBA14 u32 att3_getActor(u32)
ext 024EC8D0 u32 att3_LockonTarget(u32,s32)
ext 024ECE9C u32 att3_LockonTargetPId(u32,s32)
ext 024EC6A8 void att3_initList(u32,u32)
ext 024ECA8C s32 att3_freeAttention(u32)
ext 024ECCFC void att3_judgementButton(u32)
ext 024ECED4 void att3_judgementStatusSw(u32,u32)
ext 024ED0A4 void att3_judgementStatusHd(u32,u32)
ext 024ED200 void att3_runSoundProc(u32)
ext 024EBAB4 void att3_dAttHint_c_init(u32)
ext 024EBACC void att3_dAttHint_c_proc(u32)
ext 024EBB24 void att3_dAttCatch_c_init(u32)
ext 024EBB44 void att3_dAttCatch_c_proc(u32)
ext 024EBD4C void att3_dAttLook_c_init(u32)
ext 024EBD64 void att3_dAttLook_c_proc(u32)
ext 024EB904 u32 att3_dAttParam_c_ct0(u32)
ext 024EB984 u32 att3_dAttParam_c_ct(u32,s32)
ext 024EB12C f32 att3_distace_angle_adjust(f32,s32,f32)
ext 025D5218 u32 att3_searchActor(u32,u32)
ext 026066C4 u32 att3_getObjectRes(u32,u32,s32)
ext 024EE634 void att3_assureTermination(u32)
*/
