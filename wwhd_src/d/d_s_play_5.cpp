/* d_s_play part 5: phase_2. WWHD. 
 * See d_s_play.cpp for the unit's range and layouts.
 *
 * HD phase_2: after the "Stage" resources are synced it selects the scene mode (+0xB64: 0 for
 * scene 8, 2 on "ENDumi", 3 on "sea_E", else 1) and waits for the matching HD system state; on
 * the ocean stages ("sea", "ADMumi", "A_umikz", "ENDumi", "sea_T", "sea_E") it sets up the
 * shadow-cloud model ("sea_Stage" / "ww_shadow_clouds") of the HD scene system; on "GanonK" a
 * flag (*(101F86E8)+0x168A) is cleared, elsewhere set; then dStage_infoCreate. The GameCube
 * particle scene loading is not here. */
#include "bindings.h"

namespace d_s_play_5_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }

static constexpr u32 SSTR_VT = 0x10052EEC;
struct sstr_l { be<u32> str, vt; };

static inline void assure(u32 s) { gabi::call_ptr(ld(ld(s + 4) + 0x14), s); }

/* strcmp of the stage SafeString with a literal SafeString (pointer test, then at most 0x40001
 * characters); `twice` = the stage string is assured twice (once otherwise) */
static bool same(u32 stage, u32 lit, bool twice) {
    assure(stage);
    if (twice) assure(stage);
    u32 s1 = ld(stage);
    assure(lit);
    u32 s2 = ld(lit);
    if (s1 == s2) return true;
    for (u32 n = 0; n < 0x40001; n++, s1++, s2++) {
        u8 c = ld8(s1);
        if (c != ld8(s2)) return false;
        if (c == 0) return true;
    }
    return false;
}

static inline u32 rel(u32 p) { /* a self-relative offset at p, or NULL */
    u32 off = ld(p);
    return off != 0 ? p + off : 0;
}

/* 025B168C */
static s32 phase_2(u32 i_this) {
    WWHD_FUNC(0x025B168C, s32, i_this);
    s32 rt = gabi::call<s32>(0x02523D08, 0x100532E8); /* dComIfG_syncStageRes("Stage") */
    if (rt > 0) return 0;
    if (rt < 0) {
        JUT_ASSERT_l(0x10053324, 0x1063, 0x100532D4);
        return 0;
    }
    gabi::Local<sstr_l> stage, l1, l2, l3, l4, l5, l6, l7, l8, l9;
    u32 S = gabi::ea(stage.get());
    stage->str = dComIfGp_ea() + 0x5134; /* dComIfGp_getStartStageName() */
    stage->vt = SSTR_VT;
    st(i_this + 0xB64, 1);
    if (i_this != 0 && (s16)ld16(i_this + 8) == 8) {
        st(i_this + 0xB64, 0);
        if (gabi::call<s32>(0x026FBB8C, ld(0x101F7274), 1) == 0) return 0;
    } else {
        l1->vt = SSTR_VT;
        l1->str = 0x10053300; /* "ENDumi" */
        gabi::call(0x025B376C, S);
        if (same(S, gabi::ea(l1.get()), false)) {
            st(i_this + 0xB64, 2);
            gabi::call(0x026FBBE8, ld(0x101F7274));
            if (gabi::call<s32>(0x026FBB8C, ld(0x101F7274), 5) == 0) return 0;
        } else {
            l2->vt = SSTR_VT;
            l2->str = 0x100532F0; /* "sea_E" */
            if (same(S, gabi::ea(l2.get()), true)) {
                st(i_this + 0xB64, 3);
                if (gabi::call<s32>(0x026FBB8C, ld(0x101F7274), 6) == 0) return 0;
            }
        }
    }
    /* the ocean stages */
    l3->vt = SSTR_VT;
    l3->str = 0x100532E4; /* "sea" */
    st8(i_this + 0xB60, 0);
    bool ocean = same(S, gabi::ea(l3.get()), true);
    if (!ocean) { l4->vt = SSTR_VT; l4->str = 0x10053308; ocean = same(S, gabi::ea(l4.get()), true); } /* "ADMumi" */
    if (!ocean) { l5->vt = SSTR_VT; l5->str = 0x100532DC; ocean = same(S, gabi::ea(l5.get()), true); } /* "A_umikz" */
    if (!ocean) { l6->vt = SSTR_VT; l6->str = 0x10053300; ocean = same(S, gabi::ea(l6.get()), true); } /* "ENDumi" */
    if (!ocean) { l7->vt = SSTR_VT; l7->str = 0x100532F8; ocean = same(S, gabi::ea(l7.get()), true); } /* "sea_T" */
    if (!ocean) { l8->vt = SSTR_VT; l8->str = 0x100532F0; ocean = same(S, gabi::ea(l8.get()), true); } /* "sea_E" */
    if (ocean) {
        gabi::Local<sstr_l> res;
        res->vt = SSTR_VT;
        res->str = 0x10053318; /* "sea_Stage" */
        u32 model_res = gabi::call<u32>(0x0260524C, ld(0x101F4F28), gabi::ea(res.get()));
        if (model_res == 0) JUT_ASSERT_l(0x10053324, 0x109E, 0x10053334);
        res->vt = SSTR_VT;
        res->str = 0x10053348; /* "ww_shadow_clouds" */
        u32 file = ld(model_res + 0x14);
        gabi::call(0x025B376C, gabi::ea(res.get()));
        u32 dict = rel(file + 0x24);
        s32 idx = gabi::call<s32>(0x027DF9B0, dict, (u32)res->str);
        if (idx != -1) {
            u32 dict2 = rel(ld(model_res + 0x14) + 0x24);
            u32 model = rel(dict2 + (u32)(idx << 4) + 0x24);
            gabi::call(0x0274FBF8, ld(0x101F8B18));
            gabi::call(0x02773798, i_this + 0xAD0, model);
            gabi::call(0x0274FCCC, ld(0x101F8B18));
            u32 w = i_this + 0x5D4;
            bool same_cfg = ld(i_this + 0x7A4) == ld(i_this + 0xAD4) && ld(w + 0x1D4) == ld(i_this + 0xAD8) &&
                            ld(w + 0x1D8) == ld(i_this + 0xADC) && ld(w + 0x1DC) == ld(i_this + 0xAE0) &&
                            ld(w + 0x1E0) == ld(i_this + 0xAE4) && ld(w + 0x1E4) == ld(i_this + 0xAE8) &&
                            ld(w + 0x204) == ld(i_this + 0xB08) && ld(w + 0x200) == ld(i_this + 0xB04) &&
                            ld(w + 0x1E8) == ld(i_this + 0xAEC);
            if (same_cfg) {
                u32 a = ld(i_this + 0xAF8), b = ld(i_this + 0xB00);
                st(w + 0x1F4, a);
                st(w + 0x2A8, b);
                st(w + 0x2A0, a);
                st(w + 0x1FC, b);
            } else {
                gabi::call(0x027BDEB4, w + 0x1CC, i_this + 0xAD0);
            }
            u32 g = ld(0x101F95D0);
            u32 n = ld(g + 0x1020);
            u32 p = ld(g + 0x1024);
            if (n > 1) p += 4;
            u32 o = ld(p);
            st(o + 0x6BD0, i_this + 0xA90);
            st(o + 0x6BCC, w);
            st8(i_this + 0xB60, 1);
        }
    }
    l9->vt = SSTR_VT;
    l9->str = 0x10053310; /* "GanonK" */
    bool ganon = same(S, gabi::ea(l9.get()), true);
    st8(ld(0x101F86E8) + 0x168A, ganon ? 0 : 1);
    gabi::call(0x025C2EEC); /* dStage_infoCreate() */
    return 2;
}
VERIFY(0x025B168C, phase_2);

} // namespace d_s_play_5_cpp
