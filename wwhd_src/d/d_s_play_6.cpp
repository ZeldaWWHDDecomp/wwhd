/* d_s_play part 6: phase_4. WWHD. 
 * See d_s_play.cpp for the unit's range and layouts.
 *
 * HD phase_4 follows the GameCube order with these differences: the particle scene comes from
 * "Particle.bfres" ("Pscene%03d.jpc" with the stage's particle number) instead of the scene
 * command; the window is 1280x720; the HD system selected by the scene mode (+0xB64) is
 * initialised; only the darkness HIO child is created; on "GTower" the bow is not taken away
 * (it is only given back elsewhere); the Hyrule monotone check also accepts any stage starting
 * with 'X'; the preload table has 16 entries; the stage-name compares go through
 * sead::SafeString temporaries (literal first). */
#include "bindings.h"

namespace d_s_play_6_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }

static constexpr u32 SSTR_VT = 0x10052EEC;
static constexpr u32 SAVE = 0x101F84DC;
struct sstr_l { be<u32> str, vt; };
struct fixedstr32_l { be<u32> str, vt, size; be<u8> buf[0x20]; }; /* sead::FixedSafeString<32> */

static inline void assure(u32 s) { gabi::call_ptr(ld(ld(s + 4) + 0x14), s); }
static inline u32 rel(u32 p) { u32 off = ld(p); return off != 0 ? p + off : 0; }

/* stage name == literal, through two SafeString temporaries: the literal (assured twice), then
 * the stage name; a pointer test, then at most 0x40001 characters */
static bool stage_is(gabi::Local<sstr_l>& lit, gabi::Local<sstr_l>& stage, u32 str) {
    lit->vt = SSTR_VT;
    lit->str = str;
    u32 play = dComIfGp_ea();
    stage->vt = SSTR_VT;
    stage->str = play + 0x5134;
    u32 a = gabi::ea(lit.get()), b = gabi::ea(stage.get());
    assure(a);
    assure(a);
    u32 s1 = ld(a);
    assure(b);
    u32 s2 = ld(b);
    if (s1 == s2) return true;
    for (u32 n = 0; n < 0x40001; n++, s1++, s2++) {
        u8 c = ld8(s1);
        if (c != ld8(s2)) return false;
        if (c == 0) return true;
    }
    return false;
}

/* 025B1FF8 */
static s32 phase_4(u32 i_this) {
    WWHD_FUNC(0x025B1FF8, s32, i_this);
    /* particle scene */
    u32 stageObj = dComIfGp_ea() + 0x5150;
    u32 info = gabi::call_ptr<u32>(ld(ld(stageObj) + 0x15C), stageObj); /* dComIfGp_getStageStagInfo() */
    u32 partNo = (ld16(info + 0xA) >> 3) & 0xFF;                       /* dStage_stagInfo_GetParticleNo */
    gabi::Local<sstr_l> resA, resB;
    resA->vt = SSTR_VT;
    resB->vt = SSTR_VT;
    resA->str = 0x100533B0; /* "Particle" */
    resB->str = 0x100533BC; /* "Particle.bfres" */
    u32 found = gabi::call<u32>(0x026124B0, ld(0x101F4F7C), gabi::ea(resA.get()), gabi::ea(resB.get()), 0);
    u32 scene = 0;
    if (found != 0) {
        u32 files = gabi::call<u32>(0x027E2DC0);
        gabi::Local<fixedstr32_l> name;
        u32 n = gabi::ea(name.get());
        name->buf[0] = 0;
        name->buf[0x1F] = 0;
        name->size = 0x20;
        name->str = n + 0xC;
        name->vt = 0x10052F64;
        gabi::call(0x02759C28, n, 0x100533CC, partNo); /* format("Pscene%03d.jpc", particle no) */
        assure(n);
        u32 dict = rel(files + 0x4C);
        u32 e = gabi::call<u32>(0x027DFA24, dict, (u32)name->str);
        if (e != 0) scene = rel(e);
    }
    gabi::call(0x025A7E18, ld(dComIfGp_ea() + 0x5AB0), scene); /* dComIfGp_particle_createScene */

    u32 bgs = dComIfGp_ea();
    gabi::call_ptr(ld(ld(bgs + 0x26A0) + 0x14), bgs + 0x12A0); /* dComIfG_Bgsp()->Ct() */
    u32 r = gabi::call<u32>(0x02518760, dComIfGp_ea() + 0x26A4);   /* dComIfG_Ccsp()->Ct() */
    r = gabi::call<u32>(0x025286E0, r);                            /* dComIfGp_createDemo() */
    r = gabi::call<u32>(0x0246B42C, r);                            /* daSea_Init() */
    r = gabi::call<u32>(0x0203E8F4, r);
    gabi::call(0x025BDBB0, r);                                     /* dSnap_Create() */
    u32 play = dComIfGp_ea();
    st(play + 0x5B2C, 0); /* dComIfGp_setPlayerInfo(0, NULL, 0) */
    st8(play + 0x5B30, 0);
    for (int i = 0; i < 3; i++) st(dComIfGp_ea() + 0x5B34 + 4 * i, 0); /* dComIfGp_setPlayerPtr(i, NULL) */
    st8(dComIfGp_ea() + 0x5AC9, 1);                                     /* dComIfGp_setWindowNum(1) */
    u32 win = dComIfGp_ea() + 0x5ACC;
    gabi::call(0x0252CC8C, win, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f); /* setViewPort */
    gabi::call(0x0252CCA8, win, 0.0f, 0.0f, 1280.0f, 720.0f);             /* setScissor */
    st8(win + 0x28, 0);
    st8(win + 0x29, 2);
    play = dComIfGp_ea(); /* dComIfGp_setCameraInfo(0, NULL, 0, 0, -1) */
    st8(play + 0x5AFC, 0);
    st8(play + 0x5AFE, 0xFF);
    st(play + 0x5AF8, 0);
    st8(play + 0x5AFD, 0);
    st(play + 0x5B00, 0);
    st(dComIfGp_ea() + 0x5F9C, 0); /* dComIfGd_setWindow(NULL) */
    st(dComIfGp_ea() + 0x5FA0, 0); /* dComIfGd_setViewport(NULL) */
    st(dComIfGp_ea() + 0x5FA4, 0); /* dComIfGd_setView(NULL) */
    u32 heap = gabi::call<u32>(0x025DB8A4, 0x73EA1); /* fopMsgM_createExpHeap */
    if (heap == 0) JUT_ASSERT_l(0x100533DC, 0x1192, 0x100533A4);
    st(dComIfGp_ea() + 0x5C24, heap); /* dComIfGp_setExpHeap2D */
    gabi::call(0x025C3094); /* dStage_Create() */

    u32 mode = ld(i_this + 0xB64);
    if (mode == 0) {
        gabi::call(0x0271F2F0, 0);
        gabi::call(0x0271EF88, ld(0x101F83FC));
    } else if (mode == 1) {
        if (ld8(0x101F8350) == 0) gabi::call(0x02717AD0, 0);
        gabi::call(0x02713368, ld(0x101F8344));
    } else if (mode == 2) {
        gabi::call(0x0271E618, 0);
        gabi::call(0x0271D9FC, ld(0x101F83D0));
    } else if (mode == 3) {
        gabi::call(0x02708188, 0);
        gabi::call(0x02707D70, ld(0x101F8268));
    }
    u32 clk = gabi::call<u32>(0xC0009C80) /* OSGetSystemInfo (import) */;
    gabi::call(0x025F096C, (ld(clk) >> 2) / 30); /* mDoGph_gInf_c::setTickRate((OS_BUS_CLOCK / 4) / 30) */
    st8(0x1047B560, (u8)gabi::call<u32>(0x025F0A10, 0x100533EC, 0x1047B560)); /* g_darkHIO.mNo = mDoHIO_createChild */

    u32 att = dComIfGp_ea() + 0x12A0 + 0x4564; /* placement new of the attention */
    if (att != 0) gabi::call(0x024ED73C, att, ld(dComIfGp_ea() + 0x12A0 + 0x488C), 0);
    gabi::call(0x025CC0C8, dComIfGp_ea() + 0x599C); /* dComIfGp_getVibration().Init() */
    gabi::call(0x025260D8);                          /* daSteamTag_c::init() */
    /* daYkgr_c / daSalvage_c / daDai_c / daNpc_Kg2_c / daIball_c ::init() (inlined) */
    st(0x10475628, 0);
    st8(0x10475656, 0);
    st8(0x10475652, 1);
    st8(0x101D5F42, 0);
    st8(0x10475651, 0);
    st(0x10475638, (u32)-1);
    st8(0x10475654, 0xFF);
    st8(0x10475655, 0);
    st8(0x10475653, 0xFF);
    st(0x10475634, 0);
    gabi::call(0x02526124);
    st8(dComIfGp_ea() + 0x62F1, 0xFF); /* dComIfG_setBrightness(0xFF) */
    st8(0x101F4827, 0);                /* mDoGph_gInf_c::offFade() */

    u32 so = dComIfGp_ea() + 0x5150;
    u32 stag = gabi::call_ptr<u32>(ld(ld(so) + 0x15C), so);
    if (stag != 0 && ((ld(stag + 0xC) >> 16) & 7) == 5 &&
        gabi::call<BOOL>(0x025B8B94, ld(SAVE) + 0x644, 0x801)) { /* dStageType_FF1, isEventBit(0x0801) */
        gabi::call(0x02522398, 0, 0xFF);                         /* dComIfGs_setSelectEquip(0, NONE) */
        st8(dComIfGp_ea() + 0x5BC0, 0xFF);                       /* dComIfGp_setSelectEquip(0, NONE) */
        gabi::call(0x025B79B8, ld(SAVE) + 0xD4, 0, 0);           /* dComIfGs_offCollect(0, 0) */
    }

    gabi::Local<sstr_l> lit, stg;
    if (!stage_is(lit, stg, 0x1005336C)) { /* "GTower" */
        if (ld8(ld(SAVE) + 0x68) == 0xFF) {  /* the bow slot is empty: give it back */
            if (gabi::call<BOOL>(0x025B5D40, ld(SAVE) + 0x71, 0xC, 2)) st8(ld(SAVE) + 0x68, 0x36);
            else if (gabi::call<BOOL>(0x025B5D40, ld(SAVE) + 0x71, 0xC, 1)) st8(ld(SAVE) + 0x68, 0x35);
            else if (gabi::call<BOOL>(0x025B5D40, ld(SAVE) + 0x71, 0xC, 0)) st8(ld(SAVE) + 0x68, 0x27);
        }
    }
    gabi::Local<sstr_l> lit2, stg2, lit3, stg3, lit4, stg4;
    bool xboss = stage_is(lit, stg, 0x10053374) || stage_is(lit2, stg2, 0x1005337C) ||
                 stage_is(lit3, stg3, 0x10053384) || stage_is(lit4, stg4, 0x1005338C); /* "Xboss0".."Xboss3" */
    if (xboss) gabi::call(0x025224A4); /* dComIfGs_setPlayerRecollectionData() */
    gabi::call(0x025E18D4);            /* mDoAud_monsSeInit() */
    s32 name = i_this != 0 ? (s16)ld16(i_this + 8) : 0x7FFF;
    st8(0x101EACB6, 0); /* dScnPly_ply_c::pauseTimer */
    st8(0x101EACB7, 0); /* dScnPly_ply_c::nextPauseTimer */
    st8(0x101F4706, name == 7); /* mDoAud_zelAudio_c::onBgmSet / offBgmSet */

    gabi::Local<sstr_l> h1, s1, h2, s2, h3, s3, h4, s4;
    bool mono;
    bool hyrule = stage_is(h4, s4, 0x10053394) || stage_is(h1, s1, 0x1005339C) || stage_is(h2, s2, 0x10053364);
    if (hyrule && !gabi::call<BOOL>(0x025B8B94, ld(SAVE) + 0x644, 0x3802)) /* COLORS_IN_HYRULE */
        mono = true;
    else
        mono = ld8(dComIfGp_ea() + 0x5134) == 'X';
    if (mono) {
        gabi::call(0x025F0820); /* mDoGph_gInf_c::onMonotone() */
        s16 rate = 400;
        if (stage_is(h3, s3, 0x10053394) && (s8)ld8(dComIfGp_ea() + 0x513F) == 8) rate = -600; /* "Hyrule", layer 8 */
        st16(0x101F4820, (u16)rate); /* setMonotoneRate */
        st16(0x101F4822, 0);         /* setMonotoneRateSpeed(0) */
    } else {
        gabi::call(0x025F0830); /* mDoGph_gInf_c::offMonotone() */
    }
    st8(0x101EACB4, 0xFF); /* preLoadNo = -1 */
    if (ld8(0x101EACB5) != 0) { /* doPreLoad */
        u32 stage = dComIfGp_ea() + 0x5134;
        for (u32 i = 0; i < 16; i++) {
            u32 a = stage, b = ld(0x100530D8 + i * 0xC);
            u8 c1, c2;
            do {
                c1 = ld8(a++);
                c2 = ld8(b++);
            } while (c1 == c2 && c1 != 0);
            if (c1 == c2) st8(0x101EACB4, (u8)i);
        }
    }
    st(ld(0x101F4974) + 0, 0); /* mDoRst::offReset() */
    st(ld(0x101F4974) + 4, 0);
    if ((s8)ld8(0x101EACB4) < 0) return 4;
    u32 hi = gabi::call<u32>(0xC0009CD8); /* OSGetTime (import): r3:r4 */
    u32 lo = gabi::cpu->r[4];
    st(0x1047B600, hi);                    /* resPreLoadTime0 */
    st(0x1047B604, lo);
    return 2;
}
VERIFY(0x025B1FF8, phase_4);

} // namespace d_s_play_6_cpp
